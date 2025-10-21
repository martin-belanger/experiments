// g++ -I`gcc -print-file-name=plugin`/include -fPIC -shared -fno-rtti -O2 extract_structs_plugin.c -o extract_structs_plugin.so
#include "gcc-plugin.h"
#include "plugin-version.h"
#include "tree.h"
#include "cp/cp-tree.h"
#include "tree-pretty-print.h"
#include "context.h"
#include "diagnostic.h"

int plugin_is_GPL_compatible;

// Utility: print struct info
static void print_struct(tree t) {
	if (TREE_CODE(t) != RECORD_TYPE) return;

	// Get the struct name (might be anonymous)
	const char *name = TYPE_NAME(t) ? IDENTIFIER_POINTER(DECL_NAME(TYPE_NAME(t))) : "<anonymous>";
	printf("struct %s {\n", name);

	// Iterate over fields
	for (tree field = TYPE_FIELDS(t); field; field = TREE_CHAIN(field)) {
		if (TREE_CODE(field) == FIELD_DECL) {
			const char *fname = IDENTIFIER_POINTER(DECL_NAME(field));
			tree ftype = TREE_TYPE(field);
			const char *ftype_name;

			// Get type name for printing
			if (TYPE_NAME(ftype) && DECL_NAME(TYPE_NAME(ftype))) ftype_name = IDENTIFIER_POINTER(DECL_NAME(TYPE_NAME(ftype)));
			else ftype_name = "<anon-type>";

			printf("    %s %s;\n", ftype_name, fname);
		}
	}
	printf("};\n\n");
}

// Traverse the translation unit and print structs
static void extract_structs(void *gcc_data, void *user_data) {
	tree decl;

	// Loop over all global declarations in the TU
	for (decl = global_namespace; decl; decl = TREE_CHAIN(decl)) {
		// In C++, decls may be wrapped differently — for plain C, RECORD_TYPE suffices
		if (TREE_CODE(decl) == TYPE_DECL) {
			tree type = TREE_TYPE(decl);
			if (TREE_CODE(type) == RECORD_TYPE)
				print_struct(type);
		}
	}
}

int plugin_init(struct plugin_name_args *plugin_info,
		struct plugin_gcc_version *version) {
	printf("[extract_structs] Loaded plugin: %s\n", plugin_info->base_name);

	register_callback(plugin_info->base_name,
			  PLUGIN_FINISH_TYPE,
			  extract_structs,
			  NULL);

	return 0;
}

