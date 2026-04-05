#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
OperationType check_operation_type(char *argv[])  //check operationtype function for -e and -d
{
  if(argv[1]!=NULL)
  {
    if(!strcmp(argv[1],"-e"))    // first argument should be -e while selecting encoding
    return e_encode;        //printf("selected encoding");
   
    else if(!strcmp(argv[1],"-d")) //first argument should be -e while selecting encoding
    return e_decode;  
  }    //printf("selected decoding")
     //printf("invalid input")
    return e_unsupported; 

}

int main(int argc,char*argv[])

{ 
  if (argc < 2)
    {
        printf("ERROR: Not enough arguments\n");
        return e_failure;
    }

  switch (check_operation_type(argv))  
  {
    case e_encode:
   if (argc != 5)  //validate argument for encoding
            {
                printf("Usage: ./a.out -e source_image.bmp secret.txt output_image.bmp\n");
                return e_failure;
            }
    
    EncodeInfo encodefo;  // structure to encoding data
    printf("selected encoding\n");   //performing encoding
    if(read_and_validate_encode_args(argv,&encodefo)==e_success)
    {
      do_encoding(&encodefo);               //perform encoding
    }
    else{
      printf("Read and validate is not success\n");
      return 0;
    } 
      
    break;
  
    case e_decode:  //if decoding is selected
    if(argc <0 && argc>3)  // checks minimum number of argument is not more than 3
  {
    printf("ERROR: Invalid command line arguments\n");
    return e_failure;
  }
    DecodeInfo decInfo;
    printf("selected decoding\n");
    if(read_and_validate_decode_args(argv,&decInfo)==e_success)
    {
      do_decoding(&decInfo);           //perform encoding
    }
    else{
      printf("Read and validate is not success\n");
      return 0;
    } 
      
    break;

   default:
   printf("Invalid input\n");    // Handle unsupported operations

  }

    return 0;
}
