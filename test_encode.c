#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

//gcc *.c -o s && ./s
//cd "Project 2/C-Steganography/C-Project-1/4-SkeletonCode"
// Decode - ./a.out -d stego.bmp secret.txt output.txt
// Encode ./a.out -e beautiful.bmp secret.txt stego.bmp


int main(int argc, char * argv[])
{
    if( argc < 4 )
    {
        printf("Invalid Arguments!\n");
        printf("For encoding --> ./a.out -e beautiful.bmp secret.txt stego.bmp  (stego.bmp is optional)\n");
        printf("For decoding --> ./a.out -d stego.bmp secret.txt output.txt (output.txt is optional)\n");
        return e_failure;
    }

    if ( check_operation_type ( argv ) == e_encode )
    {
        printf("We are going to ENCODE!\n");
        EncodeInfo encode;

        if ( read_and_validate_encode_args ( argv, &encode) == e_success )
        {
            printf("-> Read and Validate is Successfull\n");
        }

        else
        {
            printf("Read and Validate is NOT Successfull!\n");
            return e_failure;
        }

        if( do_encoding(&encode) == e_success )
        {
            printf("\n\n[ ENCODING IS SUCCESSFULL.......... ]\n\n");
        }
        else
        {
            printf("Encoding Unsuccessfull!\n");
            return e_failure;
        }
    }

    else if(check_operation_type(argv) == e_decode)
    {
        printf("We are going to DECODE!\n");
        EncodeInfo decode;

        if(read_and_validate_decode_args(argv, &decode) == e_success)
        {
            printf("Read and Validate is Successful!\n");
        }
        else
        {
            printf("Read and validate is NOT Successful\n");
            return e_failure;
        }

        if(do_decoding(&decode) == e_success)
        {
            printf("\n\n[ DECODING IS SUCCESSFUL ]\n\n");
        }
        else
        {
            printf("Decoding Unsuccessful!\n");
            return e_failure;
        }
    }

    return 0;
}

OperationType check_operation_type ( char * argv[] )
{
    if ( strcmp( argv[1] , "-e") == 0 )
    {
        return e_encode;
    }
    else if ( strcmp( argv[1] , "-d") == 0 )
    {
        return e_decode;
    }
    else
    {
        printf("Invalid Input!\n");
        return e_unsupported;
    }
}