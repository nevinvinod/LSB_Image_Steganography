#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], EncodeInfo *decInfo)
{
    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo -> src_image_fname = argv[2];
    }
    else
    {
        printf(".bmp file not found!\n");
        return e_failure;
    }

    if (argv[3] != NULL && strstr(argv[3], ".txt") != NULL)
    {
        decInfo -> secret_fname = argv[3];
    }
    else
    {
        decInfo -> secret_fname = "output.txt";
    }

    return e_success;
}

Status open_files_for_decoding(EncodeInfo *encInfo)
{
    encInfo -> fptr_src_image = fopen( encInfo -> src_image_fname, "r");

    if( encInfo -> fptr_src_image == NULL) 
    {
        printf("Source Image not found!\n");
        return e_failure;
    }
    
    encInfo -> fptr_secret = fopen( encInfo -> secret_fname, "w");

    if( encInfo -> fptr_secret == NULL)
    {
        printf("Secret File not found!\n");
        return e_failure;
    }

    return e_success;
}

Status do_decoding(EncodeInfo *encInfo)
{
    if(open_files_for_decoding(encInfo) == e_success)
    {
        printf("-> Files Opened Successfully\n");
    }
    else
    {
        printf("-> Files NOT Opened !\n");
        return e_failure;
    }

    fseek( encInfo -> fptr_src_image, 54, SEEK_SET);


    if(decode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("-> Magic string DECODED successfully\n");
    }
    else
    {
        printf("-> Magic string NOT DECODED !\n");
        return e_failure;
    }

    int extn_size = decode_int_from_lsb(encInfo);
    printf("-> Secret file extension size DECODED\n");

    if(decode_secret_file_extn(encInfo, extn_size) == e_success)
    {
        printf("-> Secret File Extension DECODED Successfully\n");
    }
    else
    {
        printf("-> Secret File Extension NOT DECODED !\n");
        return e_failure;
    }

    encInfo -> size_secret_file = decode_int_from_lsb(encInfo);
    printf("-> Secret file size DECODED\n");

    if(decode_secret_file_data(encInfo) == e_success)
    {
        printf("-> Secret data DECODED Successfully\n");
    }
    else
    {
        printf("-> Secret data NOT DECODED !\n");
        return e_failure;
    }

    return e_success;
}

Status decode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buffer[16];

    for( int i=0; i < strlen( magic_string ); i++ )
    {
        fread( encInfo -> image_data, 8, 1, encInfo -> fptr_src_image );
        buffer[i] = decode_byte_from_lsb ( encInfo -> image_data );
    }

    buffer [ strlen ( magic_string ) ] = '\0';

    if( strcmp ( buffer, magic_string ) == 0 )
    {
        return e_success;
    }

    return e_failure;
}

int decode_int_from_lsb(EncodeInfo *encInfo)
{
    char buffer[32];

    fread(buffer, 8, 4, encInfo->fptr_src_image);

    int val = 0;

    for(int i=0; i<32; i++)
    {
        val |= ( (buffer[i] & 1) << i );
    }

    return val;
}

Status decode_secret_file_extn(EncodeInfo *encInfo, int extn_size)
{
    for( int i = 0; i < extn_size; i++ )
    {
        fread( encInfo -> image_data, 8, 1, encInfo -> fptr_src_image );
        encInfo -> extn_secret_file[i] = decode_byte_from_lsb ( encInfo -> image_data );
    }

    encInfo -> extn_secret_file[ extn_size ] = '\0';

    return e_success;
}

Status decode_secret_file_data(EncodeInfo *encInfo)
{
    for(int i=0; i<encInfo->size_secret_file; i++)
    {
        fread(encInfo -> image_data, 8, 1, encInfo -> fptr_src_image );

        encInfo -> secret_data[0] = decode_byte_from_lsb(encInfo -> image_data );

        fwrite(encInfo -> secret_data, 1, 1, encInfo -> fptr_secret);
    }

    return e_success;
}

char decode_byte_from_lsb(char *image_buffer)
{
    char data = 0;

    for(int i=0; i<8; i++)
    {
        data |= ((image_buffer[i] & 1) << i);
    }

    return data;
}
