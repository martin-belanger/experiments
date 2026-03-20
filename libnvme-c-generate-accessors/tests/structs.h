// file: example_structs.h

#ifndef EXAMPLE_STRUCTS_H
#define EXAMPLE_STRUCTS_H

#include <stdint.h>
#include <stdbool.h>

#define NICKSIZE 32

struct person {
	char *name;                 // dynamic string
	int age;                    // normal integer

	// C++ comment

	const int id;               // read-only integer

	uint32_t ssn;

	/* C-comment */

	char nickname[NICKSIZE];    // fixed-size array w/o explicit number for the size
	bool skip_this_member;


	/* Another C-comment,
	   but on multiple line */

	const char country[16];     /* read-only fixed-size
				       array with explicit number
				       for the size */


	// Another C++ comment
};


struct car {
        char *vin;                  // dynamic string
	int year;
	const char *make;           // read-only string
	char model[64];             // fixed-size array

	struct person owner;

	static int *price;
};

typedef struct car2 {
	int year;
	const char *make;           // read-only string

	static struct person owner;

} car2_t;

#endif /* EXAMPLE_STRUCTS_H */

