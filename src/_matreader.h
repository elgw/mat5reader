#ifndef _matreader_h_
#define _matreader_h_

/* Non-exposed API */

// Headers -- note that we lack one for the flags
static size_t mi_parse_shape(mde_t * e, void * datap);
static uint8_t mde_get_class(mde_t * e);
static void mi_print_variable_name(mde_t * e, const char * data);
static int mi_parse_header(const char * data, uint32_t * type, uint32_t * bytes, uint8_t* small);

// There should be one per mxCLASS so some more to write
static void parse_mxDOUBLE_CLASS(mde_t * e);
static void parse_mxSTRUCT_CLASS(mde_t * e, int level);
static void parse_mxCELL_CLASS(mde_t * e, int level);
static void parse_mxCHAR_CLASS(mde_t * e);


static uint8_t mde_get_class(mde_t * e);

/* Decompression
 */
static char * mde_inflate(mde_t * e, 
    size_t * inflated_bytes);

#endif
