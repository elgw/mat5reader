/* "Public" API */

#ifndef matreader_h_
#define matreader_h_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <zlib.h>
#include <sys/types.h>
#include <sys/stat.h>
// #include "hexdump.c"
#include "op_mx.h"
#include "op_mi.h"


/* Matlab data element
 */
typedef struct mde_t
{
  // Data element, 8 byte header + data
  uint32_t data_type; // either internal or external
  uint32_t nbytes;
  char * data;
  char * name;
  struct mde_t * root;
  int nchild;
  struct mde_t ** child;
  int mxCLASS;
  int ndim;
  uint32_t * dims;
  char * xdata; // extracted data converted to the correct mxCLASS
  uint32_t xbytes; // size of xdata
  bool cpx; // Complex flag
  bool glob; // Global flag
  bool log; // Logical flag
} mde_t;

/* To store the header of the mat file
 */
typedef struct
{
  // A header is 128 bytes
  char text[117]; // 116 bytes + 1 byte for '\0'
  uint64_t subsys_data_offset;
  uint16_t version;
  // The two characters 'M' and 'I'
  char endian_indicator[2];
} mat_header_t;


// Parse a MATLAB MAT file
// returns NULL on failure
mde_t * matreader(char * fname, FILE * out);


// Parse the head of a mat file
mat_header_t * mat_header_read(char * data);
void mat_header_print(mat_header_t * header);



/* Free the node as well as all children
 */
void mde_free(mde_t * e);


// Parsing
void mdes_parse(mde_t * root, const char * data, size_t nbytes, int level);



void op_mi_print_name(int op);
void op_mx_print_name(int op);

/* Conversion between internal mi format to external mx format
 * Needs some love
 */
void * mi_to_mx(int mitype, int mxtype, size_t nel, void * data);


/* Preview the data (if possible) */
void mde_print(mde_t * e);
/* Print name, shape and flags to stdout */
void mde_print_desc(mde_t * node);

void mde_add_child(mde_t * root, mde_t * child);

void mde_set_name(mde_t * e, char * name);

// The caller is responsible to free the returned char*
char * mde_get_name(mde_t * e);

// number of elements from the shape vector
size_t mde_get_nel(mde_t * e);

// Output
void mde_print(mde_t * e);
void mde_print_shape(mde_t * e);

#endif
