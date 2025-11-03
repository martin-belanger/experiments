/**
 * Copyright (c) 2025, Martin Belanger.
 *
 * Experimental Software Notice
 *
 * This software is provided on an "AS IS" basis, without warranties or conditions
 * of any kind, either express or implied, including, without limitation, any
 * warranties or conditions of title, non-infringement, merchantability, or
 * fitness for a particular purpose.
 *
 * This code is experimental and may contain defects, bugs, or incomplete features.
 * The author(s) assume no responsibility or liability for any damages or losses
 * arising from the use of this software.
 *
 * Use at your own risk.
 *
 *******************************************************************************
 *
 * g++ -std=c++17 -O2 generate_accessors.cpp -o generate_accessors
 *
 * Example usage:
 *   ./generate_accessors --outdir gen --verbose myfile.h
 *
 * This program parses C header files and generates accessor
 * functions (setter/getter) for each member found within.
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
#include <cstdlib>  // For std::exit

namespace fs = std::filesystem;

struct Member {
	void clear() {
		type.clear();
		name.clear();
		array_size.clear();
		is_char_array = false;
		is_const = false;
	}
	std::string type;
	std::string name;
	std::string array_size = "";
	bool        is_char_array = false;
	bool        is_const = false;
};

using StructMembers = std::vector<Member>;
using StructMap     = std::map<std::string, StructMembers>;
using StringSet     = std::set<std::string>;
using StringList    = std::vector<std::string>;

struct Args {
	fs::path     outdir = ".";
	bool         combine = false;
	std::string  excl_file;
	std::string  incl_file;
	std::string  prefix;
	bool         verbose = false;
	StringList   hdr_files;
};

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
			       bool              verbose) {
	std::smatch                  match;
	StructMap                    structs;
	std::regex                   regex_struct(R"(struct\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\{([\s\S]*?)\};)");
	std::regex                   regex_c_comment(R"(/\*[\s\S]*?\*/)");
	std::regex                   regex_char_array(R"((const\s+)?char\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\[\s*([A-Z0-9_]+)\s*\]\s*;)");
	std::regex                   regex_member(R"(([a-zA-Z_][a-zA-Z0-9_\s]*)([*\s]+)([a-zA-Z_][a-zA-Z0-9_]*)\s*;)");
	std::string::const_iterator  search_start(text.cbegin());

	while (std::regex_search(search_start, text.cend(), match, regex_struct)) {
		Member              m;
		std::string         body;
		std::string         struct_name;
		std::string         line;
		StructMembers       members;
		std::istringstream  lines;
		bool                is_ptr;

		search_start = match.suffix().first; // Move the start forward (for next loop iteration)

		struct_name = match[1];
		if (!incl_list.empty() && incl_list.count(struct_name) == 0)
			continue;

		body = match[2];

		// Remove multi-line /* ... */ comments from the struct body
		lines = std::istringstream(std::regex_replace(body, regex_c_comment, ""));

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
			if (line.find("struct") != std::string::npos) continue; // skip struct members

			m.clear();

			/* Look for char arrays. E.g. char buffer[10] */
			if (std::regex_match(line, fmatch, regex_char_array)) {
				m.is_char_array = true;
				m.name = fmatch[2];
				m.is_const = !trim(fmatch[1]).empty();
				m.type = "const char *"; // used as the getter return type
				try {
					m.array_size = std::to_string(std::stoul(fmatch[3]) - 1);
				} catch (std::invalid_argument &ex) {
					m.array_size = fmatch[3].str() + " - 1";
				}
				members.push_back(m);
				continue;
			}

			/* All other members */
			if (!std::regex_match(line, fmatch, regex_member))
				continue;

			std::string tmp_type(trim(fmatch[1]));
			m.is_const = tmp_type.find("const") == 0;

			is_ptr = trim(fmatch[2]) == "*";
			if (is_ptr) {
				// If we have a pointer, but it's not a "char *" then skip.
				if (tmp_type.find("char") == std::string::npos)
					continue;

				tmp_type = "const char *"; // used as the getter return type
			}

			m.type = tmp_type;
			m.name = trim(fmatch[3]);
			members.push_back(m);
		}

		if (!members.empty()) {
			structs[struct_name] = members;
			if (verbose)
				std::cout << "Found struct: " << struct_name << " (" << members.size() << " members)\n";
		}
	}

	return structs;
}

static std::string generate_hdr(const std::string   &struct_name,
				const StructMembers &members,
				const std::string   &hdr_file,
				const StringSet     &excl_list,
				const std::string   &prefix,
				bool                combine=false) {
	std::ostringstream out;
	std::string guard;

	if (combine) {
		out << "/****************************************************************************\n"
		       " * Accessors for: struct " << struct_name << "\n"
		       " */\n";
	} else {
		guard = "ACCESSORS_" + struct_name + "_H";
		for (auto &c : guard) c = std::toupper(c);

		out << "/* Auto-generated setter/getter code */\n";
		out << "#ifndef " << guard << "\n"
		       "#define " << guard << "\n\n";
		out << "#include \"" << hdr_file << "\"\n"
		       "#include <stdlib.h>\n"
		       "#include <string.h>\n\n";
	}

	for (const auto &m : members) {
		std::string fname;
		std::string key;

		// Check if struct is to be excluded
		if (excl_list.count(struct_name) != 0)
			continue;

		// Check if struct::member is to be excluded
		key = struct_name + "::" + m.name;
		if (excl_list.count(key) != 0)
			continue;

		fname = prefix + struct_name + "_" + m.name;

		// Setter method
		if (!m.is_const) { // No setter on const members
			if (m.is_char_array || m.type == "char *")
				out << "void " << fname << "_set(struct " << struct_name << "* p, const char* " << m.name << ");\n";
			else
				out << "void " << fname << "_set(struct " << struct_name << "* p, " << m.type << " " << m.name << ");\n";
		}

		// Getter method
		out << m.type << " " << fname << "_get(struct " << struct_name << "* p);\n";
	}

	if (!guard.empty()) {
		out << "\n#endif /* " << guard << " */\n";
	}
	return out.str();
}

static std::string generate_src(const std::string   &struct_name,
				const StructMembers &members,
				const StringSet     &excl_list,
				const std::string   &prefix,
				bool                combine=false) {
	std::ostringstream out;
	if (combine) {
		out << "/****************************************************************************\n"
		       " * Accessors for: struct " << struct_name << "\n"
		       " */\n";
	} else {
		out << "/* Auto-generated setter/getter code */\n"
		       "#include <stdlib.h>\n"
		       "#include <string.h>\n";
		out << "#include \"" << struct_name << "_accessors.h\"\n\n";
	}

	for (const auto &m : members) {
		std::string fname;
		std::string key;

		// Check if struct is to be excluded
		if (excl_list.count(struct_name) != 0)
			continue;

		// Check if struct::member is to be excluded
		key = struct_name + "::" + m.name;
		if (excl_list.count(key) != 0)
			continue;

		fname = prefix + struct_name + "_" + m.name;

		// Setter method
		if (!m.is_const) {
			if (m.type == "char *" && !m.is_char_array) { // dynamic string
				out << "void " << fname << "_set(struct " << struct_name << "* p, const char* " << m.name << ") {\n";
				out << "    free(p->" << m.name << ");\n";
				out << "    p->" << m.name << " = " << m.name << " ? strdup(" << m.name << ") : NULL;\n";
				out << "}\n\n";
			} else if (m.is_char_array) { // fixed-size array
				out << "void " << fname << "_set(struct " << struct_name << "* p, const char* " << m.name << ") {\n";
				out << "    strncpy(p->" << m.name << ", " << m.name << ", " << m.array_size << ");\n";
				out << "    p->" << m.name << "[" << m.array_size << "] = '\\0';\n";
				out << "}\n\n";
			} else { // numeric or struct
				out << "void " << fname << "_set(struct " << struct_name << "* p, " << m.type << " " << m.name << ") {\n";
				out << "    p->" << m.name << " = " << m.name << ";\n";
				out << "}\n\n";
			}
		}

		// Getter method
		out << m.type << " " << fname << "_get(struct " << struct_name << "* p) {\n";
		out << "    return p->" << m.name << ";\n";
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

static Args extract_args(int argc, char *argv[]) {
	int   opt;
	int   option_index = 0;
	Args  args;

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

	while ((opt = getopt_long(argc, argv, "o:ce:i:p:vh", long_options, &option_index)) != -1) {
		switch (opt) {
		case 'o': args.outdir    = optarg; break;
		case 'c': args.combine   = true;   break;
		case 'e': args.excl_file = optarg; break;
		case 'i': args.incl_file = optarg; break;
		case 'p': args.prefix    = optarg; break;
		case 'v': args.verbose   = true;   break;
		case 'h': print_usage(argv[0]); std::exit(0);
		default:  print_usage(argv[0]); std::exit(1);
		}
	}

	// Remaining arguments after options are file names or wildcards
        if (optind >= argc) {
		std::cerr << "Please specify header file(s) to parse" << std::endl;
		std::exit(1);
        }

	for (int i = optind; i < argc; ++i) {
		glob_t glob_result{};
		int ret = glob(argv[i], GLOB_TILDE, nullptr, &glob_result);
		if (ret == 0) {
			for (size_t j = 0; j < glob_result.gl_pathc; ++j)
				args.hdr_files.emplace_back(glob_result.gl_pathv[j]);
		} else {
			std::cerr << "Warning: No match for " << argv[i] << "\n";
		}
		globfree(&glob_result);
	}

	return args;
}

int main(int argc, char *argv[]) {
	StringSet           incl_list;
	StringSet           excl_list;
	std::ifstream       in;
	std::stringstream   buffer;
	std::ostringstream  hdr_intro;
	std::ostringstream  hdr_body;
	std::ostringstream  src_intro;
	std::ostringstream  src_body;
	Args                args;

	args = extract_args(argc, argv);

	fs::create_directories(args.outdir);
	incl_list = load_list(args.incl_file);
	excl_list = load_list(args.excl_file);

	if (args.combine) {
		hdr_intro << "/* Auto-generated setter/getter code */\n"
			     "#ifndef ACCESSORS_H\n"
			     "#define ACCESSORS_H\n"
			     "\n"
			     "#include <stdlib.h>\n"
			     "#include <string.h>\n"
			     "\n";
		src_intro << "/* Auto-generated combined accessors */\n"
			     "#include <stdlib.h>\n"
			     "#include <string.h>\n"
			     "#include \"accessors.h\"\n"
			     "\n";
	}

	for (const auto &hdr_file : args.hdr_files) {
		if (args.verbose)
			std::cout << "\nProcessing " << hdr_file << '\n';

		in = std::ifstream(hdr_file);
		if (!in) {
			std::cerr << "Cannot open file " << hdr_file << '\n';
			continue;
		}

		buffer.str(""); // Clear the buffer's content
		buffer.clear(); // Clear any error flag
		buffer << in.rdbuf();
		auto structs = parse_structs(buffer.str(), incl_list, args.verbose);
		if (structs.empty()) {
			if (args.verbose) {
				if (incl_list.empty())
					std::cout << "No structs found in " << hdr_file << ".\n";
				else
					std::cout << "No structs found in " << hdr_file << " that are part of the include list.\n";
			}
			continue;
		}

		if (args.combine) {
			hdr_intro << "#include \"" << fs::path(hdr_file).filename().string() << "\"\n";
			src_intro << "#include \"" << fs::path(hdr_file).filename().string() << "\"\n";
			for (const auto &[sname, members]: structs) {
				hdr_body << generate_hdr(sname, members, fs::path(hdr_file).filename().string(), excl_list, args.prefix, args.combine) << "\n";
				src_body << generate_src(sname, members, excl_list, args.prefix, args.combine) << "\n";
			}
		} else {
			for (const auto &[sname, members]: structs) {
				write_file(args.outdir / (sname + "_accessors.h"), generate_hdr(sname, members, fs::path(hdr_file).filename().string(), excl_list, args.prefix, args.combine));
				write_file(args.outdir / (sname + "_accessors.c"), generate_src(sname, members, excl_list, args.prefix, args.combine));
				std::cout << "Generated " << sname << "_accessors.h/.c\n";
			}
		}
	}

	if (args.combine) {
		hdr_body << "#endif /* ACCESSORS_H */\n";
		hdr_intro << "\n";
		write_file(args.outdir / "accessors.h", hdr_intro.str() + hdr_body.str());

		src_intro << "\n";
		write_file(args.outdir / "accessors.c", src_intro.str() + src_body.str());

		std::cout << "\nGenerated combined accessors.h and accessors.c\n";
	}

	std::exit(0);
}

