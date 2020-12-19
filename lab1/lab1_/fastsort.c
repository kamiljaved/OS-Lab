#include <stdio.h>
#include <stdlib.h>     // exit(), free(), mallow()
#include <sys/stat.h>   // struct stat
#include <unistd.h>     // opterr
#include <getopt.h>     // optarg, getopt()
#include <string.h>     // strdup()
#include <fcntl.h>      // file-handling constants 
#include "sort.h"       // struct rec_t, NUMRECS

// global variables
rec_t *arrRec;
int nRec = 0;
int print = 0;

// function prototypes
void PrintRecordArray();
void BubbleSort();
void InsertionSort();
void MergeSort();
void MergeS();
void Merge();

void usage (char* progname)
{   
    // fprintf is used to print a formatted string to a specific file
    fprintf(stderr, "usage: %s <-i input_file> <-o output_file> (optional: <-p> <-k> <-a bubble/insertion/merge>)\n", progname);
}

// about used flags in open()
// O_RDONLY    Open for reading only.
// O_WRONLY    Open for writing only.
// O_RDWR      Open for reading and writing. The result is undefined if this flag is applied to a FIFO.
// O_CREAT     If the file exists, this flag has no effect. Otherwise, the file shall be created.
// O_TRUNC     If the file exists and is a regular file, and the file is successfully opened O_RDWR or O_WRONLY, its length shall be truncated to 0, and the mode and owner shall be unchanged.

// about the access permission bits
// S_IRUSR     Read permission bit for the owner of the file. On many systems this bit is 0400.
// S_IWUSR     Write permission bit for the owner of the file. Usually 0200.
// S_IXUSR     Execute (for ordinary files) or search (for directories) permission bit for the owner of the file. Usually 0100.
// S_IRWXU     This is equivalent to ‘(S_IRUSR | S_IWUSR | S_IXUSR)’.

int main (int argc, char* argv[])
{
    // required args (5):	fastsort -i inputfile -o outputfile
	if (argc < 5) usage(argv[0]);

	// input and output file paths
    char* inFile = "/no/such/file";
    char* outFile = "/no/such/file";
    
    int c;
    opterr = 0;
    char* method = "bubble";        // default sorting method

    // parse command line args
    while((c = getopt(argc, argv, "i:o:a:p k")) != -1)
    {
        switch(c)
        {
            case 'i':
                inFile = strdup(optarg);
                break;
    		case 'o':
    			outFile = strdup(optarg);
    			break;
    		case 'p':
    			print = 1;
    			break;
    		case 'k':
    			print = 2;
                break;
    		case 'a':
    			method = strdup(optarg);
    			break;
    		default:
    			usage(argv[0]);
        }
    }

    // open input file
    int fd = open(inFile, O_RDONLY);
    if (fd < 0)
    {
        perror("error: open");
        exit(1);
    }

    // get file-size in bytes
    struct stat buffer;
    stat(inFile, &buffer);
    int bytes = buffer.st_size;

    // calculate no. of records in file (each record is of 100 bytes)
    nRec = bytes/sizeof(rec_t);

    printf("Number of Records in file: %d\n", nRec);

    // allocate memory to store records
    int allocBytes = 0;
    if ((allocBytes = sizeof(rec_t)*nRec) != 0)
    {
        arrRec = malloc(allocBytes);
        // On error, malloc() will return NULL. NULL may also be returned by a successful call to malloc() with a size of zero.
        if (arrRec == NULL)
        {
            perror("error: malloc");
            exit(1);
        }
    } 
    else
    {
        printf("No Records to Sort! Finishing Execution.\n");
        return 0;
    }

    // read each record from file into the array
    int i = 0, rc;
    while(i < nRec) 
    {
        rc = read(fd, &arrRec[i], sizeof(rec_t));
        if (rc == 0) break;     // EOF
        if (rc < 0)
        {
            perror("error: read");
            exit(1);
        }
        i++;
    }
    (void) close(fd);

    if (print != 0) PrintRecordArray();
    
    printf("-------------------SORTING BEGIN-------------------\n");
    // sort the loaded records (w.r.t. keys)
    if (strcmp(method, "insertion") == 0)       InsertionSort();
    else if (strcmp(method, "merge") == 0)		MergeSort();
    else										BubbleSort();
    printf("--------------------SORTING END--------------------\n");

    if (print != 0) PrintRecordArray();

    // create output file
    fd = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
    if (fd < 0)
    {
    	perror("error: open");
    	exit(1);
    }

    // write entire record array to file at-once
    rc = write(fd, arrRec, sizeof(rec_t)*nRec);
    if (rc != sizeof(rec_t)*nRec)
    {
        perror("error: write");
        exit(1);
    }
    (void) close(fd);

	// deallocate used memory
	free(arrRec);

    return 0;
}

// Method to Print the Record Array
void PrintRecordArray()
{
    for (int i = 0; i<nRec; i++)
    {
    	printf("[%10d] ", arrRec[i].key);
    	if(print == 1)
    	{
    		printf("Rec = ");
    		for (int j = 0; j< NUMRECS; j++)
    		{
    			printf("%d ", arrRec[i].record[j]);
    		}
    		printf("\n");
    	}
    }
    if(print != 1) printf("\n");
}

//----------------------//
//	Sorting Algorithms	//
//----------------------//

void BubbleSort()
{
	printf("Using Bubble-Sort Algorithm\n");

	int i, j; rec_t tmp;
    for (i = 0; i < nRec-1; i++)
    {
        for (j = 0; j < nRec-i-1; j++)
        {    
            if (arrRec[j].key > arrRec[j+1].key)
            {
                // swap structures
                tmp = arrRec[j];
                arrRec[j] = arrRec[j+1];
                arrRec[j+1] = tmp;
            }
        }
    }
}

void InsertionSort()
{
	printf("Using Insertion-Sort Algorithm\n");

	int i, j; rec_t k;
	for (i = 1; i < nRec; i++)
	{
		j = i-1;
		k = arrRec[j+1];
        // move KEY-rec (k) down as long as its key is smaller
		while (j >= 0 && arrRec[j].key > k.key)
		{
			arrRec[j+1] = arrRec[j];
			j--;
		}
		arrRec[j+1] = k;
	}
}

void MergeSort()
{
	printf("Using Merge-Sort Algorithm\n");

	// initial call for sorting
	MergeS(0, nRec-1);
}

void MergeS(int l, int r)
{
    if (l < r)
    {
        int m = l+(r-l)/2;
        MergeS(l, m);
        MergeS(m+1, r);
        Merge(l, m, r);
    }
}

// Merges two subarrays { ...arr[l..m], ...arr[m+1..r] } 
void Merge(int l, int m, int r)
{
    int i, j, k, n1 = m - l + 1, n2 =  r - m;
    rec_t L[n1], R[n2];     // for left and right temp subarrays
    for (i = 0; i < n1; i++)    L[i] = arrRec[l + i];
    for (j = 0; j < n2; j++)    R[j] = arrRec[m + 1+ j];
    i = 0; j = 0; k = l;
    while (i < n1 && j < n2)
    {
        if (L[i].key <= R[j].key)
        {
            arrRec[k] = L[i];
            i++;
        }
        else
        {
            arrRec[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1)
    {
        arrRec[k] = L[i];
        i++;
        k++;
    }
    while (j < n2)
    {
        arrRec[k] = R[j];
        j++;
        k++;
    }
}
