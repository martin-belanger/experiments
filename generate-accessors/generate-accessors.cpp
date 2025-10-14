/**
 * g++ -std=c++17 -O2 generate_accessors.cpp -o generate_accessors
 *
 * ./generate_accessors --outdir gen --verbose example_structs.h
 *
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <filesystem>
#include <getopt.h>
#include <glob.h>   // For wildcard expansion

namespace fs = std::filesystem;

struct Field {
	std::string type;
	std::string name;
	std::string arraySize = "";
	bool        isCharArray = false;
	bool        isConst = false;
};

using StructFields = std::vector<Field>;
using StructMap    = std::map<std::string, StructFields>;
using StringSet    = std::set<std::string>;
using StringList   = std::vector<std::string>;

static std::string trim(const std::string &s) {
	size_t start = s.find_first_not_of(" \t\r\n");
	size_t end = s.find_last_not_of(" \t\r\n");

	return start == std::string::npos ? "" : s.substr(start, end - start + 1);
}

static StringSet load_list(const std::string &file) {
	StringSet      list;
	std::ifstream  in(file);
	std::string    line;

	if (!in)
		return list;

	while (std::getline(in, line)) {
		line = trim(line);
		if (!line.empty() && line[0] != '#')
			list.insert(line);
	}
	return list;
}

static StructMap parse_structs(const std::string &text,
			       const StringSet   &incl_list,
			       bool               verbose) {
	StructMap                    structs;
	std::regex                   structPattern(R"(struct\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\{([\s\S]*?)\};)");
	std::regex                   blockComment(R"(/\*[\s\S]*?\*/)");
	std::regex                   charArrayRegex(R"((const\s+)?char\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\[\s*([A-Z0-9_]+)\s*\]\s*;)");
	std::regex                   fieldRegex(R"(([a-zA-Z_][a-zA-Z0-9_\s]*)([*\s]+)([a-zA-Z_][a-zA-Z0-9_]*)\s*;)");
	std::smatch                  match;
	std::string::const_iterator  searchStart(text.cbegin());

	while (std::regex_search(searchStart, text.cend(), match, structPattern)) {
		Field               f;
		std::string         body;
		std::string         structName;
		std::string         line;
		StructFields        fields;
		std::istringstream  lines;

		searchStart = match.suffix().first;

		structName = match[1];
		if (!incl_list.empty() && incl_list.count(structName) == 0)
			continue;

		body = match[2];

		// Remove multi-line /* ... */ comments from the struct body
		lines = std::istringstream(std::regex_replace(body, blockComment, ""));

		while (std::getline(lines, line)) {
			std::smatch fmatch;

			/* Remove inline // comments */
			auto commentPos = line.find("//");
			if (commentPos != std::string::npos)
				line = line.substr(0, commentPos);

			line = trim(line);
			if (line.empty()) continue;

			if (line.find(';') == std::string::npos) continue; // skip lines without semicolon
			if (line.find("static") != std::string::npos) continue; // skip static members

			/* Look for char arrays. E.g. char buffer[10] */
			if (std::regex_match(line, fmatch, charArrayRegex)) {
				f.isCharArray = true;
				f.name = fmatch[2];
				f.isConst = !trim(fmatch[1]).empty();
				f.type = "const char *"; // used as the getter return type
				try {
					f.arraySize = std::to_string(std::stoul(fmatch[3]) - 1);
				} catch (std::invalid_argument &ex) {
					f.arraySize = fmatch[3].str() + " - 1";
				}
				fields.push_back(f);
				continue;
			}

			/* All other fields */
			if (!std::regex_match(line, fmatch, fieldRegex))
				continue;

			f.type = trim(fmatch[1]); // used as the getter return type
			f.name = trim(fmatch[3]);
			if (trim(fmatch[2]) == "*") f.type += " *";
			f.isConst = f.type.find("const") == 0;
			fields.push_back(f);
		}

		if (!fields.empty()) {
			structs[structName] = fields;
			if (verbose)
				std::cout << "Found struct: " << structName << " (" << fields.size() << " fields)\n";
		}
	}

	return structs;
}

static std::string generate_hdr(const std::string  &structName,
				const StructFields &fields,
				const std::string  &hdr_file,
				const StringSet    &excl_list,
				const std::string  &prefix,
				StringSet          &excluded,
				bool               combine=false) {
	std::ostringstream out;
	std::string guard;

	if (combine) {
		out << "/****************************************************************************\n"
		       " * Accessors for: struct " << structName << "\n"
		       " */\n";
	} else {
		guard = "ACCESSORS_" + structName + "_H";
		for (auto &c : guard) c = std::toupper(c);

		out << "/* Auto-generated setter/getter code */\n";
		out << "#ifndef " << guard << "\n"
		       "#define " << guard << "\n\n";
		out << "#include \"" << hdr_file << "\"\n"
		       "#include <stdlib.h>\n"
		       "#include <string.h>\n\n";
	}

	for (const auto &f : fields) {
		std::string fname;
		std::string key;

		// Check if struct is to be excluded
		if (excl_list.count(structName) != 0) {
			excluded.insert(structName);
			continue;
		}

		// Check if struct::member is to be excluded
		key = structName + "::" + f.name;
		if (excl_list.count(key) != 0) {
			excluded.insert(key);
			continue;
		}

		fname = prefix + structName + "_" + f.name;

		// Setter method
		if (!f.isConst) { // No setter on const members
			if (f.isCharArray || f.type == "char *")
				out << "void " << fname << "_set(struct " << structName << "* p, const char* " << f.name << ");\n";
			else
				out << "void " << fname << "_set(struct " << structName << "* p, " << f.type << " " << f.name << ");\n";
		}

		// Getter method
		out << f.type << " " << fname << "_get(struct " << structName << "* p);\n";
	}

	if (!guard.empty()) {
		out << "\n#endif /* " << guard << " */\n";
	}
	return out.str();
}

static std::string generate_src(const std::string  &structName,
				const StructFields &fields,
				const StringSet    &excl_list,
				const std::string  &prefix,
				bool               combine=false) {
	std::ostringstream out;
	if (combine) {
		out << "/****************************************************************************\n"
		       " * Accessors for: struct " << structName << "\n"
		       " */\n";
	} else {
		out << "/* Auto-generated setter/getter code */\n"
		       "#include <stdlib.h>\n"
		       "#include <string.h>\n";
		out << "#include \"" << structName << "_accessors.h\"\n\n";
	}

	for (const auto &f : fields) {
		std::string fname;
		std::string key;

		// Check if struct is to be excluded
		if (excl_list.count(structName) != 0)
			continue;

		// Check if struct::member is to be excluded
		key = structName + "::" + f.name;
		if (excl_list.count(key) != 0)
			continue;

		fname = prefix + structName + "_" + f.name;

		// Setter method
		if (!f.isConst) {
			if (f.type == "char *" && !f.isCharArray) { // dynamic string
				out << "void " << fname << "_set(struct " << structName << "* p, const char* " << f.name << ") {\n";
				out << "    free(p->" << f.name << ");\n";
				out << "    p->" << f.name << " = " << f.name << " ? strdup(" << f.name << ") : NULL;\n";
				out << "}\n\n";
			} else if (f.isCharArray) { // fixed-size array
				out << "void " << fname << "_set(struct " << structName << "* p, const char* " << f.name << ") {\n";
				out << "    strncpy(p->" << f.name << ", " << f.name << ", " << f.arraySize << ");\n";
				out << "    p->" << f.name << "[" << f.arraySize << "] = '\\0';\n";
				out << "}\n\n";
			} else { // numeric or struct
				out << "void " << fname << "_set(struct " << structName << "* p, " << f.type << " " << f.name << ") {\n";
				out << "    p->" << f.name << " = " << f.name << ";\n";
				out << "}\n\n";
			}
		}

		// Getter method
		out << f.type << " " << fname << "_get(struct " << structName << "* p) {\n";
		out << "    return p->" << f.name << ";\n";
		out << "}\n\n";
	}
	return out.str();
}

static void write_file(const fs::path &path, const std::string &content) {
	std::ofstream out(path);
	if (!out) throw std::runtime_error("Cannot write file: " + path.string());
	out << content;
}

static void print_usage(const char *prog) {
	std::cout << "Usage: " << prog << " [options] <header_file>\n"
		<< "Options:\n"
		<< "  -o, --outdir <dir>   Output directory. Default: current dir\n"
		<< "  -c, --combine        Generate one combined accessors.c/h file\n"
		<< "  -e, --excl <file>    Exclusion list. Which member of a struct to exclude (struct::member per line). Default: do not exclude anything\n"
		<< "  -i, --incl <file>    Inclusion list. Which struct to include (struct name per line). Default: include every struct found\n"
		<< "  -p, --prefix <str>   Prefix for generated function names\n"
		<< "  -v, --verbose        Verbose output\n"
		<< "  -h, --help           Show this message\n";
}

int main(int argc, char *argv[]) {
	int                opt;
	int                option_index = 0;
	bool               combine = false;
	bool               verbose = false;
	std::string        exclFile;
	std::string        inclFile;
	std::string        prefix;
	fs::path           outdir = ".";
	StringList         hdr_files;
	StringSet          incl_list;
	StringSet          excl_list;
	StringSet          excluded;
	std::ifstream      in;
	std::stringstream  buffer;
	std::ostringstream headerIntro;
	std::ostringstream headerBody;
	std::ostringstream sourceIntro;
	std::ostringstream sourceBody;

	static struct option long_options[] = {
		{ "outdir",  required_argument, 0, 'o' },
		{ "combine", no_argument,       0, 'c' },
		{ "excl",    required_argument, 0, 'e' },
		{ "incl",    required_argument, 0, 'i' },
		{ "prefix",  required_argument, 0, 'p' },
		{ "verbose", no_argument,       0, 'v' },
		{ "help",    no_argument,       0, 'h' },
		{ 0, 0, 0, 0 }
	};

	while ((opt = getopt_long(argc, argv, "o:ci:e:p:vh", long_options, &option_index)) != -1) {
		switch (opt) {
		case 'o': outdir   = optarg;    break;
		case 'c': combine  = true;      break;
		case 'e': exclFile = optarg;    break;
		case 'i': inclFile = optarg;    break;
		case 'p': prefix   = optarg;    break;
		case 'v': verbose  = true;      break;
		case 'h': print_usage(argv[0]); return 0;
		default:  print_usage(argv[0]); return 1;
		}
	}

	// Remaining arguments after options are file names or wildcards
        if (optind >= argc) {
		std::cerr << "Please specify header file(s) to parse" << std::endl;
		return 1;
        }

	for (int i = optind; i < argc; ++i) {
		glob_t glob_result{};
		int ret = glob(argv[i], GLOB_TILDE, nullptr, &glob_result);
		if (ret == 0) {
			for (size_t j = 0; j < glob_result.gl_pathc; ++j)
				hdr_files.emplace_back(glob_result.gl_pathv[j]);
		} else {
			std::cerr << "Warning: No match for " << argv[i] << "\n";
		}
		globfree(&glob_result);
	}

	fs::create_directories(outdir);
	incl_list = load_list(inclFile);
	excl_list = load_list(exclFile);

	if (combine) {
		headerIntro << "/* Auto-generated setter/getter code */\n"
			     "#ifndef ACCESSORS_H\n"
			     "#define ACCESSORS_H\n"
			     "\n"
			     "#include <stdlib.h>\n"
			     "#include <string.h>\n"
			     "\n";
		sourceIntro << "/* Auto-generated combined accessors */\n"
			     "#include <stdlib.h>\n"
			     "#include <string.h>\n"
			     "#include \"accessors.h\"\n"
			     "\n";
	}

	for (const auto &hdr_file : hdr_files) {
		if (verbose)
			std::cout << "\nProcessing " << hdr_file << '\n';

		in = std::ifstream(hdr_file);
		if (!in) {
			std::cerr << "Cannot open file " << hdr_file << '\n';
			continue;
		}

		buffer.str(""); // Clear the buffer content
		buffer.clear(); // Clear any error flag
		buffer << in.rdbuf();
		auto structs = parse_structs(buffer.str(), incl_list, verbose);
		if (structs.empty()) {
			if (verbose) {
				if (incl_list.empty())
					std::cout << "No structs found in " << hdr_file << ".\n";
				else
					std::cout << "No structs found in " << hdr_file << " that are part of the include list.\n";
			}
			continue;
		}

		excluded.clear();
		if (combine) {
			headerIntro << "#include \"" << fs::path(hdr_file).filename().string() << "\"\n";
			sourceIntro << "#include \"" << fs::path(hdr_file).filename().string() << "\"\n";
			excluded.clear();
			for (const auto &[sname, fields]: structs) {
				headerBody << generate_hdr(sname, fields, fs::path(hdr_file).filename().string(), excl_list, prefix, excluded, combine) << "\n";
				sourceBody << generate_src(sname, fields, excl_list, prefix, combine) << "\n";
			}
		} else {
			for (const auto &[sname, fields]: structs) {
				write_file(outdir / (sname + "_accessors.h"), generate_hdr(sname, fields, fs::path(hdr_file).filename().string(), excl_list, prefix, excluded, combine));
				write_file(outdir / (sname + "_accessors.c"), generate_src(sname, fields, excl_list, prefix, combine));
				std::cout << "Generated " << sname << "_accessors.h/.c\n";
			}
		}
		if (verbose && !excluded.empty()) {
			std::cout << "Excluded member(s):\n";
			for (auto excl_item: excluded) {
				std::cout << "   " << excl_item << "\n";
			}
		}
	}

	if (combine) {
		headerBody << "#endif /* ACCESSORS_H */\n";
		headerIntro << "\n";
		sourceIntro << "\n";
		write_file(outdir / "accessors.h", headerIntro.str() + headerBody.str());
		write_file(outdir / "accessors.c", sourceIntro.str() + sourceBody.str());
		std::cout << "\nGenerated combined accessors.h and accessors.c\n";
	}

	return 0;
}

