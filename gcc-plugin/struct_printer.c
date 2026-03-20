/*
 * GCC Plugin to print all struct definitions and their members
 *
 * Build:
 *   g++ -I`gcc -print-file-name=plugin`/include -fPIC -shared -o struct_printer.so struct_printer.c
 *
 * Usage:
 *   g++ -fplugin=./struct_printer.so your_source.c
 */

#include <gcc-plugin.h>
#include <plugin-version.h>
#include <tree.h>
#include <cp/cp-tree.h>
#include <stringpool.h>

int plugin_is_GPL_compatible;

/* Track which structs we've already printed to avoid duplicates */
static hash_set<tree> *printed_structs;

/* Print information about a struct */
static void print_struct_info(tree type_node) {
    tree field;
    const char *struct_name;

    if (!type_node || TREE_CODE(type_node) != RECORD_TYPE) {
        return;
    }

    /* Skip incomplete types */
    if (!COMPLETE_TYPE_P(type_node)) {
        return;
    }

    /* Get struct name */
    tree name = TYPE_NAME(type_node);
    if (!name) {
        return; /* Anonymous struct - skip for now */
    }

    /* Check if we've already printed this struct */
    if (printed_structs->contains(type_node)) {
        return;
    }
    printed_structs->add(type_node);

    if (TREE_CODE(name) == TYPE_DECL) {
        struct_name = IDENTIFIER_POINTER(DECL_NAME(name));
    } else if (TREE_CODE(name) == IDENTIFIER_NODE) {
        struct_name = IDENTIFIER_POINTER(name);
    } else {
        return;
    }

    fprintf(stderr, "struct %s {\n", struct_name);

    /* Iterate through all fields */
    for (field = TYPE_FIELDS(type_node); field; field = DECL_CHAIN(field)) {
        if (TREE_CODE(field) != FIELD_DECL) {
            continue;
        }

        const char *field_name = IDENTIFIER_POINTER(DECL_NAME(field));
        tree field_type = TREE_TYPE(field);
        const char *type_name = "<unknown>";

        /* Determine field type name */
        if (TYPE_NAME(field_type)) {
            tree tn = TYPE_NAME(field_type);
            if (TREE_CODE(tn) == TYPE_DECL) {
                type_name = IDENTIFIER_POINTER(DECL_NAME(tn));
            } else if (TREE_CODE(tn) == IDENTIFIER_NODE) {
                type_name = IDENTIFIER_POINTER(tn);
            }
        } else {
            /* Handle built-in types without names */
            switch (TREE_CODE(field_type)) {
                case INTEGER_TYPE:
                    if (TYPE_UNSIGNED(field_type)) {
                        switch (TYPE_PRECISION(field_type)) {
                            case 8: type_name = "unsigned char"; break;
                            case 16: type_name = "unsigned short"; break;
                            case 32: type_name = "unsigned int"; break;
                            case 64: type_name = "unsigned long long"; break;
                            default: type_name = "unsigned"; break;
                        }
                    } else {
                        switch (TYPE_PRECISION(field_type)) {
                            case 8: type_name = "signed char"; break;
                            case 16: type_name = "short"; break;
                            case 32: type_name = "int"; break;
                            case 64: type_name = "long long"; break;
                            default: type_name = "int"; break;
                        }
                    }
                    break;
                case REAL_TYPE:
                    type_name = TYPE_PRECISION(field_type) == 32 ? "float" : "double";
                    break;
                case POINTER_TYPE: {
                    tree pointee = TREE_TYPE(field_type);
                    if (TYPE_NAME(pointee)) {
                        tree ptn = TYPE_NAME(pointee);
                        if (TREE_CODE(ptn) == TYPE_DECL) {
                            static char ptr_name[256];
                            snprintf(ptr_name, sizeof(ptr_name), "%s*",
                                   IDENTIFIER_POINTER(DECL_NAME(ptn)));
                            type_name = ptr_name;
                        }
                    } else {
                        type_name = "void*";
                    }
                    break;
                }
                case ARRAY_TYPE: {
                    tree elem_type = TREE_TYPE(field_type);
                    tree domain = TYPE_DOMAIN(field_type);
                    static char arr_name[256];

                    const char *elem_name = "<unknown>";
                    if (TYPE_NAME(elem_type)) {
                        tree etn = TYPE_NAME(elem_type);
                        if (TREE_CODE(etn) == TYPE_DECL) {
                            elem_name = IDENTIFIER_POINTER(DECL_NAME(etn));
                        } else if (TREE_CODE(etn) == IDENTIFIER_NODE) {
                            elem_name = IDENTIFIER_POINTER(etn);
                        }
                    } else if (TREE_CODE(elem_type) == INTEGER_TYPE) {
                        if (TYPE_UNSIGNED(elem_type)) {
                            switch (TYPE_PRECISION(elem_type)) {
                                case 8: elem_name = "unsigned char"; break;
                                case 16: elem_name = "unsigned short"; break;
                                case 32: elem_name = "unsigned int"; break;
                                case 64: elem_name = "unsigned long long"; break;
                                default: elem_name = "unsigned"; break;
                            }
                        } else {
                            switch (TYPE_PRECISION(elem_type)) {
                                case 8: elem_name = "char"; break;
                                case 16: elem_name = "short"; break;
                                case 32: elem_name = "int"; break;
                                case 64: elem_name = "long long"; break;
                                default: elem_name = "int"; break;
                            }
                        }
                    }

                    /* Get array size if available */
                    if (domain && TYPE_MAX_VALUE(domain)) {
                        tree max_val = TYPE_MAX_VALUE(domain);
                        if (TREE_CODE(max_val) == INTEGER_CST) {
                            /* Array size is max_value + 1 (since it's 0-indexed) */
                            unsigned HOST_WIDE_INT size = TREE_INT_CST_LOW(max_val) + 1;
                            snprintf(arr_name, sizeof(arr_name), "%s[%lu]",
                                   elem_name, (unsigned long)size);
                            type_name = arr_name;
                        } else {
                            snprintf(arr_name, sizeof(arr_name), "%s[]", elem_name);
                            type_name = arr_name;
                        }
                    } else {
                        snprintf(arr_name, sizeof(arr_name), "%s[]", elem_name);
                        type_name = arr_name;
                    }
                    break;
                }
                default:
                    break;
            }
        }

        fprintf(stderr, "  %s %s;\n", type_name, field_name);
    }

    fprintf(stderr, "}\n\n");
}

/* Callback: called after each type is finished */
static void finish_type_callback(void *gcc_data, void *user_data) {
    tree type = (tree) gcc_data;

    if (type && TREE_CODE(type) == RECORD_TYPE) {
        print_struct_info(type);
    }
}

/* Optional: callback at the start to print header */
static void start_unit_callback(void *gcc_data, void *user_data) {
    static bool header_printed = false;
    if (!header_printed) {
        fprintf(stderr, "\n=== Struct Definitions Found ===\n\n");
        header_printed = true;
    }
}

/* Plugin initialization */
int plugin_init(struct plugin_name_args *plugin_info,
                struct plugin_gcc_version *version) {

    /* Check GCC version compatibility */
    if (!plugin_default_version_check(version, &gcc_version)) {
        fprintf(stderr, "GCC version mismatch\n");
        return 1;
    }

    const char *plugin_name = plugin_info->base_name;

    /* Initialize the hash set for tracking printed structs */
    printed_structs = new hash_set<tree>;

    /* Register callback to print header at start */
    register_callback(plugin_name, PLUGIN_START_UNIT,
                     start_unit_callback, NULL);

    /* Register callback to execute after each type is completed */
    register_callback(plugin_name, PLUGIN_FINISH_TYPE,
                     finish_type_callback, NULL);

    return 0;
}
