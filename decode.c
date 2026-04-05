#include <stdio.h>  //header for i/o  operations
#include<string.h>  //for string functions
#include "decode.h" //header file for decode related declarations
#include "common.h" 
#include "types.h" //project specific type definations


//open stego image and output files for decoding
Status open_decode_files(DecodeInfo *decInfo)   
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r"); //open stego file in binary mode  
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen"); //print system error 
        fprintf(stderr, "ERROR: Unable to open the file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }

    decInfo->fptr_output = fopen(decInfo->output_fname, "wb"); //open output file in write mode
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open the file %s\n", decInfo->output_fname);
        return e_failure;
    }

    return e_success;  //file open successfully
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo) //validate command line arguments
{
  if(strstr(argv[2],".bmp")) //check if stego image has .bmp extension
  {
  decInfo->stego_image_fname=argv[2];
  }
  else
  {
  fprintf(stderr,"ERROR File is not bmp file\n");
  return e_failure;
  }
  if(strstr(argv[3],".txt")) //check if the output file has  .txt extension
  {
    decInfo->output_fname=argv[3];
  }
  else
    {
      fprintf(stderr,"Errror:File should be .txt\n");
      return e_failure;
    }
      return e_success;
}
Status decode_magic_string(DecodeInfo *decInfo)  //decode magic string from the image
{
  char magic_string[10]; //buffer to read 8 bytes from the image
   printf("Enter the magic string\n");
   scanf("%s",magic_string);
   if(strcmp(magic_string,"#*")!=0)
   {
    printf("Enter valid magic string\n");
    return e_failure;
   }
   unsigned char image_buffer[8];
   int i;
   unsigned char data[3];
   for(int i=0;i<2;i++) //decode 2 bytes of character
   {
   fread(image_buffer,1,8,decInfo->fptr_stego_image); //read 8 bytes from the image
   data[i]=decode_byte_lsb(image_buffer);  //decode 1 byte from the lsb's
   }
   data[2]='\0';  //magic string terminated with null character
   printf("Decoded magic string is %s\n",data);
   for(int j=0;j<2;j++)
   {
    printf("character[%d]= %c| ASCII =%d\n",j,data[j],data[j]); //print the two charactrers which is decoded
   }
    printf("Expected string is %s | obtained string is %s\n",MAGIC_STRING,data);
   if(strcmp(data,MAGIC_STRING)!=0) //compare decoded and expected magic string
   {
     printf("MAGIC STRING NOT FOUND\n");
     return e_failure;
   }
   printf("Expected string is %s | obtained string is %s\n",MAGIC_STRING,data);
    return e_success;
}
// char decode_byte_lsb(unsigned char *image_buffer)
// {
//     char data = 0;
//     for (int i = 0; i < 8; i++)
//     {
//         data = (data << 1) | (image_buffer[i] & 1);
//     }
//     return data;
// }
char decode_byte_lsb(unsigned char *image_buffer) //extract 1 bytes from the 8 bytes of LSB's
{
    char data = 0;
    for (int i = 0; i < 8; i++)
    {
        data = (data << 1) | (image_buffer[i] & 1); //get LSB and add to data
    }
    return data;
}

Status skip_bmp_header(FILE *fptr_stego_image) //skip 54 byte of header
{
 fseek(fptr_stego_image,54,SEEK_SET);
 return e_success;
}
Status do_decoding(DecodeInfo *decInfo) //perform decoding operation
{
  if(open_decode_files(decInfo)==e_failure)
   {
  return e_failure;
   }
   if(decInfo->fptr_output==NULL)
   {
    fprintf(stderr,"File is not opened\n");
   }
   else{
    printf("File is opened successfully\n");
   }
  if(skip_bmp_header(decInfo->fptr_stego_image)==e_failure)
  {
    return e_failure;
  }
  if(decode_magic_string(decInfo)==e_failure)
  {
    return e_failure;
  }
  if(decode_secret_file_size(decInfo)==e_failure)
  {
    return e_failure;
  }
  printf("Secret file size is %ld\n",decInfo->size_secret_file);
  if(decode_extn_size(decInfo)==e_failure)
  {
    return e_failure;
  }
  if(decode_secret_file_extn(decInfo)==e_failure)
  {
    return e_failure;
  }
  printf("DEcoded secret file extension is %s\n",decInfo->extn_secret_file);
  if(decode_secret_file_data(decInfo)==e_failure)
  {
    return e_failure;
  }
 printf("Secret message written is %s\n",decInfo->output_fname);
 printf("Decoding done successfully\n"); 
 return e_success;

}
Status decode_secret_file_extn(DecodeInfo *decInfo) //decode secret file extension from image
{
  char buffer[8];
     if(decInfo->exten_size==0  || decInfo->exten_size> 100)
     {
      fprintf(stderr,"ERROR Extension file cannot be found\n");
       return e_failure;
     }
     for(int i=0;i<decInfo->exten_size;i++) //loop to decode extension characters 
     {
      fread(buffer,1,8,decInfo->fptr_stego_image);
       decInfo->extn_secret_file[i]=decode_byte_lsb(buffer);
       printf("Decoded secret file extn is %d :%c (ASCII) = %d\n",i,decInfo->extn_secret_file[i],decInfo->extn_secret_file[i]);
     }
     decInfo->extn_secret_file[decInfo->exten_size]='\0'; //decoded secret extension terminated with null
    // printf("Decoded secret file extn is %d :%c (ASCII) = %d\n",i,decInfo->extn_secret_file[i],decInfo->extn_secret_file[i]);
     printf("Decoded secret file extension is %s\n",decInfo->extn_secret_file);
     return e_success;
}
Status decode_secret_file_size(DecodeInfo *decInfo) //decode secret file size of 32 bits
{
 unsigned  char image_buffer[32];
  uint size=0;
  fread(image_buffer, 1, 32, decInfo->fptr_stego_image); //read 32  bytes to extract size
  for(int i=0;i<32;i++) //extract 32 lsb's
  {
    size= (size << 1) | (image_buffer[i]&1);
}
decInfo->size_secret_file=size; //store decoded size
printf("Assigned size_secret_file = %ld to decInfo\n", decInfo->size_secret_file);
return e_success;
}
Status decode_secret_file_data(DecodeInfo *decInfo)  //decode actual secret file data and write it in output file
{
    unsigned char image_buffer[8];
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
      printf("Calling decode_secret_file_data with size: %ld\n", decInfo->size_secret_file);
        int read = fread(image_buffer, 1, 8, decInfo->fptr_stego_image);//read 8 bytes for 1 character
        if (read != 8)
        {
            fprintf(stderr, "ERROR: fread failed at i=%d, read=%d\n", i, read);
            return e_failure;
        }

        char decoded_byte = decode_byte_lsb(image_buffer); //decode char
        
        int written = fwrite(&decoded_byte,1,1, decInfo->fptr_output); //write decoded character in output
        printf("Decoded character: %c | ASCII = %d\n",decoded_byte,decoded_byte);
        if (written != 1)
        {
            fprintf(stderr, "ERROR: fwrite failed at i=%d\n", i);
            return e_failure;
        }
    } 

     return e_success;
}



Status decode_extn_size(DecodeInfo *decInfo)  //decode extension size
{
    unsigned char image_buffer[32];
    unsigned int size = 0;

    // Read 32 bytes (each has 1 LSB bit of the extension size)
    if (fread(image_buffer, 1, 32, decInfo->fptr_stego_image) != 32) 
    {
        fprintf(stderr, "ERROR: Failed to read 32 bytes for extension size\n");
        return e_failure;
    }

    // Extract bits from LSBs of each byte
    for (int i = 0; i < 32; i++)
    {
        size = (size << 1) | (image_buffer[i] & 1);
    }

    if (size == 0 || size > MAX_FILE_SUFFIX)
    {
        fprintf(stderr, "ERROR: Decoded extension size (%u) is invalid\n", size);
        return e_failure;
    }

    decInfo->exten_size = size; //store extension size
    printf("Decoded extension size is %u\n", decInfo->exten_size);

    return e_success;
}
Status validate_decode_inputs(DecodeInfo *decInfo) //validate BMP format of input file
{
    // Check if source file is BMP
    const char *extn = strrchr(decInfo->stego_image_fname, '.'); //get file extension
    if (!extn || strcasecmp(extn, ".bmp") != 0)
    {
        fprintf(stderr, "file found not .bmp file\n");
        return e_failure;
    }

    // Check if BMP header is valid
    char header[2];
    fread(header, 1, 2, decInfo->fptr_stego_image); //read first 2 bytes of bmp 
    if (header[0] != 'B' || header[1] != 'M')
    {
        fprintf(stderr, "ERROR: File is not a valid BMP FILE\n"); 
        return e_failure;
    }
   

    rewind(decInfo->fptr_stego_image); // Reset file pointer for further reading
    return e_success;
}
