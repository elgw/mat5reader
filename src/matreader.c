/* 
 *  Read MATLAB Level 5 MAT-files
 *
 */

#include "matreader.h"
#include "_matreader.h"

void op_mi_print_name(int op)
{
switch(op){
  case 1:
    printf("miINT8");
break;
  case 2:
    printf("miUINT8");
break;
  case 3:
    printf("miINT16");
break;
  case 4:
    printf("miUINT16");
break;
  case 5:
    printf("miINT32");
break;
  case 6:
    printf("miUINT32");
break;
  case 7:
    printf("miSINGLE");
break;
  case 9:
    printf("miDOUBLE");
break;
  case 12:
    printf("miINT64");
break;
  case 13:
    printf("miUINT64");
break;
  case 14:
    printf("miMATRIX");
break;
  case 15:
    printf("miCOMPRESSED");
break;
  case 16:
    printf("miUTF8");
break;
  case 17:
    printf("miUTF16");
break;
  case 18:
    printf("miUTF32");
break;
default:
printf("op %d is unknown\n", op);
}
}

void op_mx_print_name(int op)
{
switch(op){
  case 1:
    printf("mxCELL_CLASS");
break;
  case 2:
    printf("mxSTRUCT_CLASS");
break;
  case 3:
    printf("mxOBJECT_CLASS");
break;
  case 4:
    printf("mxCHAR_CLASS");
break;
  case 5:
    printf("mxSPARSE_CLASS");
break;
  case 6:
    printf("mxDOUBLE_CLASS");
break;
  case 7:
    printf("mxSINGLE_CLASS");
break;
  case 8:
    printf("mxINT8_CLASS");
break;
  case 9:
    printf("mxUINT8_CLASS");
break;
  case 10:
    printf("mxINT16_CLASS1");
break;
  case 11:
    printf("mxUINT16_CLASS");
break;
  case 12:
    printf("mxINT32_CLASS");
break;
  case 13:
    printf("mxUINT32_CLASS");
break;
  case 14:
    printf("mxINT64_CLASS");
break;
  case 15:
    printf("mxUINT64_CLASS");
break;
default:
printf("%d is unknown mx op", op);
}
}
mde_t * mde_new()
{
  mde_t * e = malloc(sizeof(mde_t));
  e->root = NULL;
  e->nchild = 0;
  e->child = NULL; // Convention: is NULL when e->nchild == 0
  e->name = NULL;
  e->mxCLASS = 0;
  e->xbytes = 0;
  e->xdata = NULL;
  e->dims = NULL;
  e->ndim = 0;
  e->data = NULL;
  return e;
}

static size_t mi_parse_shape(mde_t * e, void * datap)
{
  // Parse the number of dimensions and the size of the dimensions
  // return how many bytes to skip until the next data item
  uint32_t epos = 0;
  uint32_t dimformat;
  memcpy(&dimformat, datap, 4);
  assert(dimformat == miINT32);
  uint32_t dimbytes;
  memcpy(&dimbytes, datap+4, 4);
  epos+=8;
  int ndim = dimbytes/4;
  e->ndim = ndim;
  e->dims = malloc(ndim*sizeof(uint32_t));

  for(int kk = 0; kk<ndim; kk++)
  {
    uint32_t dim = 0;
    memcpy(&dim, datap+epos, 4); epos+=4;
    e->dims[kk] = dim;
  }

  // Advance to the next 8 byte boundary
  if(epos % 8 != 0)
  {
    epos += 8-epos%8;
  }
  return epos;
}

void mde_print_shape(mde_t * e)
{
  printf("[");
  for(int dd = 0; dd < e->ndim; dd++)
  {
    printf("%u", e->dims[dd]);
    if(dd + 1 < e->ndim)
    {
      printf("x");
    }
  }
  printf("]");
}

size_t mde_get_nel(mde_t * e)
{
  if(e->ndim == 0)
  {
    return 0;
  }

  size_t nel = 1;
  for(int kk = 0; kk<e->ndim; kk++)
  {
    nel*=e->dims[kk];
  }
  return nel;
}

void mde_set_name(mde_t * e, char * name)
{
  if(e->name != NULL)
  {
    free(e->name);
  }
  e->name = malloc(strlen(name)+2);
  sprintf(e->name, "%s", name);
  return;
}

void mde_add_child(mde_t * root, mde_t * child)
{
  // "Append" child to list of children
  // TODO: dynamically increase the size or use linked list
  mde_t ** oldchild = root->child;
  root->child = malloc((root->nchild+1)*sizeof(mde_t *));
  for(int kk = 0; kk<root->nchild; kk++)
  {
    root->child[kk] = oldchild[kk];
  }
  root->child[root->nchild] = child;
  free(oldchild);
  root->nchild++;
}

void * mi_to_mx(int mitype, int mxtype, size_t nel, void * data)
{
    // Conversion from internal to external type
  if(mitype == miUTF8 && mxtype == mxCHAR_CLASS)
  {
    uint8_t * idata = (uint8_t *) data;
    char * odata = malloc(nel+1); // add space for '\0'
    for(size_t kk = 0; kk<nel; kk++)
    {
      odata[kk] = (char) idata[kk];
    }
    odata[nel] = '\0';
    return odata;
  }

  if(mitype == miINT8 && mxtype == mxDOUBLE_CLASS)
  {
    int8_t * idata = (int8_t*) data;
    double * odata = malloc(nel*sizeof(double));
    for(size_t kk = 0; kk<nel; kk++)
    {
      odata[kk] = (double) idata[kk];
    }
    return odata;
  }

  if(mitype == miUINT8 && mxtype == mxDOUBLE_CLASS)
  {
    uint8_t * idata = (uint8_t*) data;
    double * odata = malloc(nel*sizeof(double));
    for(size_t kk = 0; kk<nel; kk++)
    {
      odata[kk] = (double) idata[kk];
    }
    return odata;
  }

  if(mitype == miINT16 && mxtype == mxDOUBLE_CLASS)
  {
    int16_t * idata = (int16_t*) data;
    double * odata = malloc(nel*sizeof(double));
    for(size_t kk = 0; kk<nel; kk++)
    {
      odata[kk] = (double) idata[kk];
    }
    return odata;
  }

  if(mitype == miUINT16 && mxtype == mxDOUBLE_CLASS)
  {
    uint16_t * idata = (uint16_t*) data;
    double * odata = malloc(nel*sizeof(double));
    for(size_t kk = 0; kk<nel; kk++)
    {
      odata[kk] = (double) idata[kk];
    }
    return odata;
  }


  if(mitype == miDOUBLE && mxtype == mxDOUBLE_CLASS)
  {
    double * idata = (double *) data;
    double * odata = malloc(nel*sizeof(double));
    for(size_t kk = 0; kk<nel; kk++)
    {
      odata[kk] = (double) idata[kk];
    }
    return odata;
  }

  if(0){
  printf("Conversion from ");
  op_mi_print_name(mitype);
  printf(" to ");
  op_mx_print_name(mxtype);
  printf("\n");
  printf(" ---- Not supported\n");
  
  }
  return NULL;
}


mat_header_t * mat_header_read(char * data)
{
  size_t pos = 0;
  // Read the 116 byte header
  mat_header_t * header = malloc(sizeof(mat_header_t));
  memcpy(header->text, data+pos, 116);
  header->text[116] = '\0';
  pos += 116;

  memcpy(& header->subsys_data_offset, data+pos, 8);
  pos += 8;
  memcpy(&header->version, data+pos, 2);
  pos += 2;
  if(header->version != 0x0100)
  {
    goto fail;
  }
  memcpy(header->endian_indicator, data+pos, 2);
  pos += 2;

  return header;

fail:
  free(header);
  return NULL;
}


void mat_header_print(mat_header_t * header)
{
  printf("HEADER: %s\n", header->text);
  printf("SUBSYS DATA OFFSET: %lu\n", header->subsys_data_offset);
  printf("VERSION: %u\n", header->version);
  printf("ENDIAN INDICATOR: %c%c %d%d", 
      header->endian_indicator[0], header->endian_indicator[1],
      header->endian_indicator[0], header->endian_indicator[1]
      );
  if(header->endian_indicator[0] == 'I')
  {
    printf("(byte swapping needed)\n");
  } else {
    printf("(no byte swapping needed)\n");
  }
  return;
}

static uint8_t mde_get_class(mde_t * e)
{
  // Read class from Array Flags 
  size_t epos = 8;
  uint8_t class = 0;
  memcpy(&class, e->data+epos, 1);
  epos += 8;
  return class;
}

static void parse_mxCHAR_CLASS(mde_t * e)
{

  // Skip 8 bytes (external class, size)
  size_t epos = 8; 

  // Skip flags for now TODO: complex flags
  epos+=8;

  epos += mi_parse_shape(e, e->data+epos);

  // Read variable name
  mi_print_variable_name(e, e->data+epos);

  // Advance past the variable name
  uint8_t small; uint32_t mitype; uint32_t mibytes;
  epos += mi_parse_header(e->data+epos, &mitype, &mibytes, &small);
  // Finally, read data

  // Advance past the data
  size_t dpos = epos;
  epos += mi_parse_header(e->data+epos, &mitype, &mibytes, &small);
  if(small)
  {
    dpos+=4;
  } else {
    dpos+=8;
  }

  // Look back for the data
  // Needed conversion from mi-format to mx-format (internal to external)
  size_t nel = mde_get_nel(e);
  double * data = (double *) mi_to_mx(mitype, mxCHAR_CLASS, nel, e->data+dpos);
  if(data != NULL)
  {
    e->xdata = (char *) data;
  }
  //assert(epos == e->nbytes);

  return;
}

static void parse_mxDOUBLE_CLASS(mde_t * e)
{

  // Skip 8 bytes (external class, size)
  size_t epos = 8; 

  // Skip flags for now TODO: complex flags
  epos+=8;

  epos += mi_parse_shape(e, e->data+epos);

  // Read variable name
  mi_print_variable_name(e, e->data+epos);

  // Advance past the variable name
  uint8_t small; uint32_t mitype; uint32_t mibytes;
  epos += mi_parse_header(e->data+epos, &mitype, &mibytes, &small);
  // Finally, read data
  // Advance past the data
  size_t dpos = epos;
  epos += mi_parse_header(e->data+epos, &mitype, &mibytes, &small);
  if(small)
  {
    dpos+=4;
  } else {
    dpos+=8;
  }
  // Look back for the data
  // Needed conversion from mi-format to mx-format (internal to external)
  size_t nel = mde_get_nel(e);
  double * data = (double *) mi_to_mx(mitype, mxDOUBLE_CLASS, nel, e->data+dpos);
  if(data != NULL)
  {
    if(0){
    size_t nshow = 10;
    if(nel < nshow)
      nshow = nel;
    for(size_t kk = 0 ; kk<nshow; kk++)
    {
      printf("%f ", data[kk]);
    }
    if(nel>nshow)
      printf(" ... (%zu more)", nel-nshow);
    printf("\n");
    }
    e->xdata = (char*) data;
  }
  //assert(epos == e->nbytes);

  return;
}


static void parse_mxCELL_CLASS(mde_t * e, int level)
{
  size_t epos = 8;
  uint32_t mitype = 0;
  uint32_t mibytes = 0;
  uint8_t small = 0;

  // Array flags
  epos += mi_parse_header(e->data+epos, &mitype, &mibytes, &small);
  //printf("mibytes: %u\n", mibytes);

  // Dimensions array
  epos += mi_parse_shape(e, e->data+epos);

  // Array name
  mi_print_variable_name(e, e->data+epos);
  epos += mi_parse_header(e->data+epos, &mitype, &mibytes, &small);
  //printf("mibytes: %u\n", mibytes);

  if(e->nbytes > epos)
  {
    mdes_parse(e, e->data + epos, e->nbytes - epos, level);
  }
}


static void parse_mxSTRUCT_CLASS(mde_t * e, int level)
{
  size_t epos = 8;
  uint32_t mitype = 0;
  uint32_t mibytes = 0;
  uint8_t small = 0;

  // Array flags
  epos += mi_parse_header(e->data+epos, &mitype, &mibytes, &small);
  //printf("mibytes: %u\n", mibytes);

  // Dimensions array
  epos += mi_parse_shape(e, e->data+epos);

  // Array name
  mi_print_variable_name(e, e->data+epos);
  epos += mi_parse_header(e->data+epos, &mitype, &mibytes, &small);
  //printf("mibytes: %u\n", mibytes);

  // Field name
  epos += mi_parse_header(e->data+epos, &mitype, &mibytes, &small);
  //printf("field_size: %u\n", mibytes);
//  op_mi_print_name(mitype); printf("\n");

  // Field name length
  mi_parse_header(e->data+epos, &mitype, &mibytes, &small);
  epos+=8;
  //printf("mibytes: %u\n", mibytes);
  int nfields = 0;
  // Figure out how many fields -- must be a better way
  for(int kk = 1; kk<mibytes; kk++)
    if((e->data[epos+kk] == '\0') && (e->data[epos+kk-1] != '\0'))
      nfields++;
  //printf("nfields: %d\n", nfields);
  int delta = mibytes/nfields;
  uint32_t * fnamepos = malloc(nfields*sizeof(uint32_t));

  for(int kk = 0; kk< nfields; kk++)
  {
    fnamepos[kk] = epos+delta*kk;
//    printf("Field name %d: %s\n", kk, e->data+epos + delta*kk);
  }
  // Parse the fields ... note that they don't have names
  // otherwise they are like normal miMATRIXes
  if(mibytes % 8 == 0)
  {
    epos += mibytes;
  } else {
    epos += mibytes + 8 - (mibytes % 8);
  }

  if(e->nbytes > epos)
  {
    mdes_parse(e, e->data + epos, e->nbytes - epos, level);
  }

  // Set names of the children here
  assert(e->nchild == nfields);
  for(uint32_t cc = 0; cc<nfields; cc++)
  {
    mde_set_name(e->child[cc], e->data+fnamepos[cc]);
  }
  free(fnamepos);
}


mde_t * mde_new_from_data(const char * data, size_t * read)
{
  /* Create a new structure 
   * where the data type and size of the data element
   * can be retrieved.
   * Note that the memory region that is stored in
   * -> data does not contain the 8 byte header line
   *  Read says how much to advance in the memory region to look for the 
   *  next data element.
   */

  mde_t * e = mde_new();

  memcpy(&e->data_type, data, 4); 
  assert(e->data_type < 19);
  assert(e->data_type>0);
  memcpy(&e->nbytes, data+4, 4); 
  e->data = malloc(e->nbytes);
  memcpy(e->data, data+8, e->nbytes);
  read[0] = 8+e->nbytes;

  return e;
}

void mde_free(mde_t * e)
{
  assert(e != NULL);
  if(e->data != NULL)
  {
    free(e->data);
    e->data = NULL;
  }
  if(e->name != NULL)
  {
    free(e->name);
    e->name = NULL;
  }
  for(int kk = 0; kk<e->nchild; kk++)
  {
    mde_free(e->child[kk]);
  }
  if(e->child != NULL)
  {
    free(e->child);
    e->child = NULL;
  }
  if(e->xdata != NULL)
  {
    free(e->xdata);
    e->xdata = NULL;
  }
  if(e->dims != NULL)
  {
    free(e->dims);
    e->dims = NULL;
  }
  free(e);
  return;
}

void mde_print(mde_t * e)
{
//  printf("Type: %u -- ", e->data_type);
//  op_mi_print_name(e->data_type);
//  printf("\n");
//  printf("Bytes: %u\n", e->nbytes);
  if(e->mxCLASS == mxCHAR_CLASS)
  {
    if(e->xdata != NULL)
    {
      printf("%s\n", e->xdata);
    } else {
      printf("No data available\n");
    }
  }

  if(e->mxCLASS == mxDOUBLE_CLASS)
  {
    if(e->xdata != NULL)
    {
      double * data = (double *) e->xdata;
      uint32_t nel = mde_get_nel(e);
      int show = nel;
      if(show>10)
      {
        show = 10;
      }
      printf("[");
      for(int kk = 0; kk<show; kk++)
      {
        printf("%f", data[kk]);
        if(kk+1 < show)
          printf(" ");
      }
      if(nel > show){
        printf(" ...");
      } else {
      printf("]");
      }
      printf("\n");
    }
    return;
  }
}

static int mi_parse_header(const char * data, uint32_t * type, uint32_t * bytes, uint8_t* small)
{
  // Returns number of bytes consumed for this mi element
  small[0] = 0;
  memcpy(type, data, 4);
  memcpy(bytes, data+4, 4);
  if(type[0] > 255)
  { // Small Data Element Format
    uint16_t stype = 0;
    uint16_t sbytes = 0;
    memcpy(&sbytes, data+2, 2);
    memcpy(&stype, data, 2);
    type[0] = (uint32_t) stype;
    bytes[0] = (uint32_t) sbytes;
    small[0] = 1;
    return 8;
  }

  // This does strangely break things
  int corr = 0;
  if(bytes[0] % 8 == 0)
    corr = 8-(bytes[0] % 8);
  corr = 0;

  return 8 + bytes[0] + corr;
}

static void mi_print_variable_name(mde_t * e, const char * data)
{
  // hexdump(data, 16);
  uint32_t mitype = 0;
  uint32_t mibytes = 0;
  uint8_t small = 0;
  mi_parse_header(data, &mitype, &mibytes, &small);    
  size_t offset = 8;
  if(small == 1)
  {
    offset = 4;
  }
  //printf("small: %u mibytes: %u\n", small, mibytes);
  assert(e->name == NULL);
  e->name = malloc(mibytes+1);
  e->name[mibytes] = '\0';

  for(int kk = 0; kk<mibytes; kk++)
  {
    e->name[kk] = data[offset + kk];
  }
  //printf("name: %s\n", e->name);
}


static char * mde_inflate(mde_t * e, 
    size_t * inflated_bytes)
{
  int verbose = 0;
  z_stream infstream;
  infstream.zalloc = Z_NULL;
  infstream.zfree = Z_NULL;
  infstream.opaque = Z_NULL;

  infstream.next_in = (Bytef *) e->data; // data to be decompressed
  infstream.avail_in = (uInt)(e->nbytes); // size of input

  size_t e_data_size_max = 20*e->nbytes;
  char * inflated_data = malloc(e_data_size_max);

  if(verbose)
  {
    printf("bytes to decompress: %u\n", infstream.avail_in);
    printf("deflateBound: %zu\n", e_data_size_max);
  }
  infstream.next_out = (Bytef *) inflated_data; // allocated output
  infstream.avail_out = (uInt) e_data_size_max; // size of output allocation

  // the actual DE-compression work.
  inflateInit(&infstream);
  int status = inflate(&infstream, Z_SYNC_FLUSH);
  assert(status == Z_STREAM_END);

  inflateEnd(&infstream);
  /* TODO
   * e_data_max_size might be too small.
   * Decompression has to be iterated to avoid this
   * or it should make a dry run first to determine the exact size
   * of the uncompressed data.
   */

  // Check that everything was processed
  assert(infstream.avail_in == 0);
  if(verbose)
    printf("Size of input: %u, size of inflated: %zu\n", e->nbytes, infstream.total_out);

  inflated_bytes[0] = infstream.total_out;
  return inflated_data;
}

void mdes_parse(mde_t * root, const char * data, size_t nbytes, int level)
{
  /* Main parser: Parse nbytes of data which belongs to the root node
  */

  mde_t * e = NULL;
  size_t offset = 0;
  int iter = 0;

  while(offset < nbytes)
  {
    size_t read = 0;
    // Strip of a data element from the buffer
    e = mde_new_from_data(data+offset, &read);
    assert(e != NULL);

    if(0){
    for(int kk = 0 ; kk < level; kk++)
    { printf(">");}
    

    op_mi_print_name(e->data_type); 
    if(e->data_type != miCOMPRESSED)
    {
      printf("/"); op_mx_print_name(mde_get_class(e));
    }
    printf("\n");
    }

    // Increase the position before trying to read another mde
    // note: assert(read % 8 == 0); is not always true
    offset += read; // Refers to the data buffer

    // mde_print(e);

    if(e->data_type == miMATRIX)
    {
      // Add it to the parent
      mde_add_child(root, e);
      e->root = root;

      // Continue depending on class
      uint8_t class = mde_get_class(e);
      e->mxCLASS = class;

      switch(class)
      {
        case mxCHAR_CLASS:
          parse_mxCHAR_CLASS(e);
          break;
        case mxDOUBLE_CLASS:
          parse_mxDOUBLE_CLASS(e);
          break;
        case mxSTRUCT_CLASS:  
          parse_mxSTRUCT_CLASS(e, level+1);
          break;
        case mxCELL_CLASS:
          parse_mxCELL_CLASS(e, level+1);
          break;
        default:
          if(0){
          printf("No support for "); 
          op_mx_print_name(class); 
          printf(" yet\n");
          }
      }
    }

    // Note: This changes the content of e
    // so it has to be at the end
    if(e->data_type == miCOMPRESSED)
    {
      //printf("e->nbytes = %u\n", e->nbytes);
      size_t inflated_bytes = 0;
      char * inflated_data = mde_inflate(e, &inflated_bytes);
      mde_free(e);
      mdes_parse(root, inflated_data, inflated_bytes, level+1);
      free(inflated_data);
    }

    //    mde_free(e);    
    iter++;
  }

  return;
}

mde_t * matreader(char * fname, FILE * outbuf)
{
  struct stat statbuf;
  int rval;

  rval = stat(fname, &statbuf);
  if (rval == -1) {
    fprintf(outbuf, "Failed to get the size of %s\n", fname);
    return NULL;
  } 

  size_t nbytes = statbuf.st_size;
  if(nbytes < 116)
  {
    fprintf(outbuf, "Not a MATLAB file: file size < 116 bytes\n");
    return NULL;
  }

  FILE * f = fopen(fname, "r");
  if(f == NULL)
  {
    fprintf(outbuf, "Failed to open %s\n", fname);
    return NULL;
  }
  char * matdata = malloc(nbytes);
  assert(matdata != NULL);

  size_t nread = fread(matdata, 1, nbytes, f);
  if(nread != nbytes)
  {
    free(matdata);
    fprintf(outbuf, "Failed to read %s\n", fname);
    return NULL;
  }
  fclose(f);
  // printf("Read %zu bytes from %s\n", nread, argv[1]);

  mat_header_t * header = mat_header_read(matdata);
  if(header == NULL)
  {
    free(matdata);
    printf("Not a MATLAB file: can't parse the header\n");
    NULL;
  }
  mat_header_print(header);

  mde_t * root = mde_new();
  mde_set_name(root, "root");

  size_t offset = 116+8+4;
  mdes_parse(root, matdata + offset, nbytes-offset, 1);

  free(header);
  free(matdata);

  return root;
}
