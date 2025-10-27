#ifndef DECODE_H
#define DECODE_H
#include "types.h"
#include "encode.h"

/*Read and Validate */
Status read_and_validate_decode_args(char *argv[], EncodeInfo *decode);

/* Main decoding function */
Status do_decoding(EncodeInfo *encInfo);

/* Open files */
Status open_files_for_decoding(EncodeInfo *encInfo);

/* Decode magic string */
Status decode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Decode integer (32 bits) */
int decode_int_from_lsb(EncodeInfo *encInfo);

/* Decode secret file extension */
Status decode_secret_file_extn(EncodeInfo *encInfo, int extn_size);

/* Decode secret file data */
Status decode_secret_file_data(EncodeInfo *encInfo);

/* Decode a byte from 8 LSBs */
char decode_byte_from_lsb(char *image_buffer);

#endif
