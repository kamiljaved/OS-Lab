#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"
#include <sys/types.h>
#include <sys/stat.h>

void
usage(char *prog) 
{
    fprintf(stderr, "usage: %s <-i input file> <-o output file>\n", prog);
    exit(1);
}

int
compare_rec_t(rec_t *ptr1_rec_t, rec_t *ptr2_rec_t) {
	return ptr1_rec_t->key - ptr2_rec_t->key;
}

void
swap_rec_t(rec_t **ptr_ptr1_rec_t, rec_t **ptr_ptr2_rec_t) {
	rec_t *temp;
	temp = *ptr_ptr1_rec_t;
	*ptr_ptr1_rec_t = *ptr_ptr2_rec_t;
	*ptr_ptr2_rec_t = temp;
}

void
ascending_sort_rec_t(rec_t **ptr_ptr_records, size_t array_size) {
	// bubble sort, can be replaced by better sort
	for (int outer_index = 0; outer_index < array_size; ++outer_index) {
		for (int inner_index = outer_index + 1; inner_index < array_size; ++inner_index) {
			if (compare_rec_t(ptr_ptr_records[outer_index], ptr_ptr_records[inner_index]) > 0) {
				swap_rec_t(ptr_ptr_records + outer_index, ptr_ptr_records + inner_index);
			}
		}
	}
}

int
main(int argc, char *argv[])
{
    // program assumes a 4-byte key in a 100-byte record
    assert(sizeof(rec_t) == 100);

    // arguments
    char *inFile = "/no/such/file";
    char *outFile = "/no/such/file";

    // input params
    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "i:o:")) != -1) {
		switch (c) {
			case 'i':
				inFile = strdup(optarg);
				break;
			case 'o':
				outFile = strdup(optarg);
				break;
			default:
				usage(argv[0]);
		}
    }
    
    // open input file
    int in_fd = open(inFile, O_RDONLY);
	if (in_fd < 0) {
		perror("open");
		exit(1);
    }
    
    int no_of_records, rc;
    rec_t r;
    
    no_of_records = 0;
    while ((rc = read(in_fd, &r, sizeof(rec_t)))) {
		if (rc < 0) {
			perror("read");
			exit(1);
		}
		if (rc < sizeof(rec_t)) {
			fprintf(stderr, "invalid record size, record size should be %ld\n", sizeof(rec_t));
			exit(1);
		}
		++no_of_records;
    }
    close(in_fd);
    
    printf("no_of_records: %d\n", no_of_records);
    
    // allocate memory for pointers to records
    rec_t **pointers_to_records;
    int no_of_bytes_to_allocate;
    no_of_bytes_to_allocate = no_of_records*sizeof(rec_t **);
    pointers_to_records = malloc(no_of_bytes_to_allocate);
    if (no_of_bytes_to_allocate != 0 && pointers_to_records == NULL) {
        perror("malloc");
        exit(1);
    }

    // open input file
    in_fd = open(inFile, O_RDONLY);
	if (in_fd < 0) {
		perror("open");
		exit(1);
    }
    
    int record_no;
    
    record_no = 0;
    while ((rc = read(in_fd, &r, sizeof(rec_t)))) {		// rc will be 0 at EOF, terminating loop
		if (rc < 0) {
			perror("read");
			exit(1);
		}
		if (rc < sizeof(rec_t)) {
			fprintf(stderr, "invalid record size, record size should be %ld\n", sizeof(rec_t));
			exit(1);
		}
		// allocate memory for record and copy the record to it
		no_of_bytes_to_allocate = sizeof(rec_t);
		pointers_to_records[record_no] = malloc(no_of_bytes_to_allocate);
		if (no_of_bytes_to_allocate != 0 && pointers_to_records[record_no] == NULL) {
			perror("malloc");
			exit(1);
		}
		*pointers_to_records[record_no] = r;
		++record_no;
    }
    
    // sort records
    ascending_sort_rec_t(pointers_to_records, no_of_records);
    
    // open output file
    int out_fd = open(outFile, O_WRONLY|O_TRUNC, S_IRWXU);
    if (out_fd < 0) {
		perror("open");
		exit(1);
    }
    
    // write sorted records to output file
    for (int element_no = 0; element_no < no_of_records; ++element_no) {
		rc = write(out_fd, pointers_to_records[element_no], sizeof(rec_t));
		if (rc != sizeof(rec_t)) {
			perror("write");
			exit(1);
		}
	}
	printf("Wrote records to output file\n");

	// deallocate memory and close files
	for (record_no = 0; record_no < no_of_records; ++record_no) {
		free(pointers_to_records[record_no]);
	}
	free(pointers_to_records);
    (void) close(in_fd);
    (void) close(out_fd);

    return 0;
}

