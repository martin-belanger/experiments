# Generate Accessors Tool

This tool generates **setter and getter functions** for C structs automatically.
 It supports dynamic strings, fixed-size char arrays, const fields, and exclusion/inclusion lists.

------

## Compilation / Testing

```bash
make
make test
```

------

## Usage

```
./generate-accessors [options] <header-file>
```

**Options:**

| Short | Long        | Argument | Description                                                  |
| ----- | ----------- | -------- | ------------------------------------------------------------ |
| `-o`  | `--outdir`  | `<dir>`  | Output directory (default is current directory)              |
| `-c`  | `--combine` | none     | Generate a single combined `accessors.c` and `accessors.h` file. Default is to generate separate files (`<struct>_accessors.[ch]`) for each `struct` processed. |
| `-e`  | `--excl`    | `<file>` | Exclusion list file with `struct::member` per line           |
| `-i`  | `--incl`    | `<file>` | Inclusion list file with `struct` per line. The list of `struct` to be included in the generation. When not specified, accessors will be generated for all `struct` found in the `header-file`. |
| `-p`  | `--prefix`  | `<str>`  | Prefix for generated function names                          |
| `-v`  | `--verbose` | none     | Verbose output showing which `struct` is being processed     |
| `-h`  | `--help`    | none     | Show this help message                                       |

------

## Examples

### Single Struct Example

Header file `person.h`:

```
struct person {
    char *name;
    int age;
    const char *id;
};
```

Command:

```
./generate-accessors --prefix my_ person.h
```

Generated `person_accessors.h`:

```
#ifndef ACCESSORS_PERSON_H
#define ACCESSORS_PERSON_H

#include "person.h"
#include <stdlib.h>
#include <string.h>

void my_person_name_set(struct person* p, const char* name);
char* my_person_name_get(struct person* p);

void my_person_age_set(struct person* p, int age);
int my_person_age_get(struct person* p);

const char* my_person_id_get(struct person* p);

#endif /* ACCESSORS_PERSON_H */
```

Generated `person_accessors.c`:

```
#include <stdlib.h>
#include <string.h>
#include "person_accessors.h"

void my_person_name_set(struct person* p, const char* name) {
    free(p->name);
    p->name = name ? strdup(name) : NULL;    
}
char* my_person_name_get(struct person* p) { return p->name; }

void my_person_age_set(struct person* p, int age) { p->age = age; }
int my_person_age_get(struct person* p) { return p->age; }

const char* my_person_id_get(struct person* p) { return p->id; }
```

------

### Multi-Struct Example (`--combine`)

Header file `example_structs.h`:

```
struct person {
    char *name;
    int age;
    const char *id;
};

struct car {
    char *model;
    int year;
    const char *vin;
};
```

Command:

```
./generate-accessors --combine --prefix my_ example_structs.h
```

Generated `accessors.h`:

```
#ifndef ACCESSORS_H
#define ACCESSORS_H

#include "example_structs.h"
#include <stdlib.h>
#include <string.h>

/* person struct accessors */
void my_person_name_set(struct person* p, const char* name);
char* my_person_name_get(struct person* p);
void my_person_age_set(struct person* p, int age);
int my_person_age_get(struct person* p);
const char* my_person_id_get(struct person* p);

/* car struct accessors */
void my_car_model_set(struct car* p, const char* model);
char* my_car_model_get(struct car* p);
void my_car_year_set(struct car* p, int year);
int my_car_year_get(struct car* p);
const char* my_car_vin_get(struct car* p);

#endif /* ACCESSORS_H */
```

Generated `accessors.c`:

```
#include <stdlib.h>
#include <string.h>
#include "example_structs.h"
#include "accessors.h"

/* person: dynamic string setter/getter for 'name' */
void my_person_name_set(struct person* p, const char* name) {
    free(p->name);
    p->name = name ? strdup(name) : NULL;
}
char* my_person_name_get(struct person* p) { return p->name; }

/* person: setter/getter for 'age' */
void my_person_age_set(struct person* p, int age) { p->age = age; }
int my_person_age_get(struct person* p) { return p->age; }

/* person: getter only for const 'id' */
const char* my_person_id_get(struct person* p) { return p->id; }

/* car: dynamic string setter/getter for 'model' */
void my_car_model_set(struct car* p, const char* model) {
    free(p->model);
    p->model = model ? strdup(model) : NULL;
}
char* my_car_model_get(struct car* p) { return p->model; }

/* car: setter/getter for 'year' */
void my_car_year_set(struct car* p, int year) { p->year = year; }
int my_car_year_get(struct car* p) { return p->year; }

/* car: getter only for const 'vin' */
const char* my_car_vin_get(struct car* p) { return p->vin; }
```

------

### Notes

1. **Dynamic strings** (`char *`) are NULL-safe.
2. **Const fields** generate **getter-only functions**.
3. Numeric fields and other types have normal setters/getters.
4. The `--prefix` option adds a custom prefix to all generated functions.
5. The `--combine` option merges all structs into a single `.h`/`.c` output.
6. The exclusion list (`--excl`) prevents generating accessors for specific `struct:member` pairs.
7. The inclusion list limits generation to only the listed `struct` names.