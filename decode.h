#ifndef DECODE_H
#define DECODE_H
#include "types.h"   //contain user defined types
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
typedef struct _DecodeInfo
{
    /*Stego image info*/
    char *stego_image_fname;   
    FILE *fptr_stego_image;
    char image_data[8];
    /*output image info*/
    char *output_fname;
    FILE *fptr_output;
    uint exten_size;
    char extn_secret_file[10];
    long size_secret_file;
} DecodeInfo;
/*Decode function prototypes*/
/*Read and validate args*/
/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);
Status skip_bmp_header(FILE *fptr_stego_image);
Status open_decode_files(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
char decode_byte_lsb(unsigned char *image_buffer);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
Status decode_extn_size(DecodeInfo *decInfo);
Status validate_decode_inputs(DecodeInfo *decInfo);

#endif

