#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
*/

Status read_and_validate_encode_args ( char * argv[], EncodeInfo *encInfo )
{
    if ( strstr (argv[2] , ".bmp") != NULL )
    {
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        printf(".bmp file not found!\n");
        return e_failure;
    }


    if ( strstr (argv[3] , ".txt") != NULL )
    {
        encInfo -> secret_fname = argv[3];
    }
    else
    {
        printf(".txt file not found!\n");
        return e_failure;
    }

    if ( argv[4] != NULL && strstr (argv[4] , ".bmp") != NULL )
    {
        encInfo -> stego_image_fname = argv[4];
    }
    else
    {
        printf(".bmp [ stego.bmp ] file not found!\n");
        encInfo -> stego_image_fname = "stego.bmp";
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if ( open_files(encInfo) == e_success )
    {
        printf("-> All files are opened\n");
    }
    else
    {
        printf("Files NOT opened!\n");
        return e_failure;
    }

    if ( check_capacity(encInfo) == e_success )
    {
        printf("-> Check capacity is Successfull\n");
    }
    else
    {
        printf("Check capacity is NOT Successfull!\n");
        return e_failure;
    }

    if( copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image ) == e_success )
    {
        printf("-> Header bytes are copied\n");
    }
    else
    {
        printf("Header files are NOT copied!\n");
        return e_failure;
    }

    if( encode_magic_string(MAGIC_STRING, encInfo ) == e_success )
    {
        printf("-> Magic String Encoded Successfully\n");
    }
    else
    {
        printf("Magic String NOT Encoded\n");
        return e_failure;
    }

    strcpy ( encInfo -> extn_secret_file, strchr(encInfo -> secret_fname, '.') );

    if ( encode_secret_file_size_extn( strlen(encInfo -> extn_secret_file), encInfo) == e_success )
    {
        printf("-> File Extension Size Encoded Successfully\n");
    }
    else
    {
        printf("File Extension Size NOT Encoded!\n");
    }

    if( encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success )
    {
        printf("-> Secret File Extension Encoded Successfully\n");
    }
    else
    {
        printf("-> Secret File Extension NOT Encoded !\n");
    }


    if(encode_secret_file_size( encInfo -> size_secret_file, encInfo) == e_success )
    {
        printf("-> Secret File Size is Encoded Successfully\n");
    }
    else
    {
        printf("Secret File Size NOT Encoded !\n");
    }

    if(encode_secret_file_data(encInfo) == e_success )
    {
        printf("-> Secret File Data Encoded Successfully\n");
    }
    else
    {
        printf("Secret File Data NOT Encoded !\n");
    }

    if( copy_remaining_img_data(encInfo) == e_success )
    {
        printf("-> Remaining Data Copied Successfully\n");
    }
    else
    {
        printf("Remaining Data NOT Copied !\n");
    }

    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo -> fptr_src_image = fopen ( encInfo -> src_image_fname , "r" );

    if ( encInfo -> fptr_src_image == NULL )
    {
        printf("Source file is not found!\n");
        return e_failure;
    }

    encInfo -> fptr_secret = fopen (encInfo -> secret_fname , "r" );

    if ( encInfo -> fptr_secret == NULL )
    {
        printf("Secret file is not found!\n");
        return e_failure;
    }

    encInfo -> fptr_stego_image = fopen (encInfo -> stego_image_fname , "w" );

    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image );

    encInfo -> size_secret_file= get_file_size(encInfo -> fptr_secret);

    if ( encInfo -> image_capacity > ( 16 + 32 + 32 + 32 + encInfo -> size_secret_file * 8) )
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }

} 

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width,height;

    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, 4, 1, fptr_image);
    fread(&height, 4, 1, fptr_image);

    printf(" _________________________________\n");
    printf("|           Image Info            |\n");
    printf("|_________________________________|\n");
    printf("| Width  : %u   |  Height : %u |\n", width,height);
    printf("|_________________________________|\n\n");

    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek( fptr_src_image, 0, SEEK_SET);

    char str[54];
    fread( str, 54, 1, fptr_src_image);
    fwrite( str, 54, 1, fptr_dest_image);

    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if( encode_data_to_image(magic_string, strlen(magic_string), encInfo) == e_success )
    {
        printf("-> Magic string encoded succesfully\n");
    }
    else
    {
        printf("Magic string NOT Encoded !\n");
        return e_failure;
    }

    return e_success;
}

Status encode_data_to_image(const char *data, int size, EncodeInfo *encInfo)
{
    for( int i=0; i<size; i++ )
    {
        fread( encInfo -> image_data, 8, 1, encInfo -> fptr_src_image );
        encode_byte_to_lsb( data[i], encInfo -> image_data);
        fwrite( encInfo -> image_data, 8, 1, encInfo -> fptr_stego_image );
    }

    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0; i<8; i++ )
    {
        image_buffer[i] = ( image_buffer[i] & 0xFE ) | ( (data >> i) & 1 );
    }

    return e_success;
}

Status encode_secret_file_size_extn(int size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(size, encInfo);

    return e_success;
}

Status encode_size_to_lsb(int size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo -> fptr_src_image);

    for(int i=0; i<32; i++)
    {
        buffer[i]= ( (buffer[i] & 0xFE) | (size >> i) & 1 );
    }

    fwrite(buffer, 32, 1, encInfo -> fptr_stego_image );

    return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo);

    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size, encInfo);

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char str[encInfo -> size_secret_file];
    fread(str, encInfo -> size_secret_file, 1, encInfo -> fptr_secret);

    encode_data_to_image(str, encInfo -> size_secret_file, encInfo);

    return e_success;
}

Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    char ch;

    while(fread(&ch, 1, 1, encInfo -> fptr_src_image) > 0 )
    {
        fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
    }

    return e_success;
}