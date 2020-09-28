/* Declarations of non-exposed functions */

#ifndef _matreader_h_
#define _matreader_h_




// Headers -- note that we lack one for the flags
static size_t mi_parse_shape(mde_t * e, void * datap);
static uint32_t mi_parse_flags(mde_t * e, char * datap);
static uint8_t mde_get_class(mde_t * e);
static void mi_parse_name(mde_t * e, const char * data);
static int mi_parse_header(const char * data, 
    uint32_t * type, uint32_t * bytes, uint8_t* small);


static void parse_numeric_mx_CLASS(mde_t * e); // all numeric classes
static void parse_mxSTRUCT_CLASS(mde_t * e, int level);
static void parse_mxCELL_CLASS(mde_t * e, int level);
static void parse_mxCHAR_CLASS(mde_t * e);
// TODO: sparse

/* Allocate a new mde object */
mde_t * mde_new(void);
/* Create a new mde object from data */
mde_t * mde_get_new_from_data(const char * data, size_t * read);

static uint8_t mde_get_class(mde_t * e);

/* Decompression
 */
static char * mde_inflate(mde_t * e, 
    size_t * inflated_bytes);

#endif
