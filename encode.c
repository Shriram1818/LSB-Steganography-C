#include <stdio.h> //header i/o functions
#include<string.h>  //header for the string inbilt functions
#include "encode.h" //Header for encoding functions
#include "types.h"   // Header for type definitions
#include "common.h"  //header contain magic sgtring

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
   // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
   // printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_encode_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");  //open the source file in read mode for reading contents from the .bmp file
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)  
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");  //openening secret.txt in read mode for reading secret message
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");//opening Output file in write mode
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
    if(strstr(argv[2],".bmp"))    //set source image file name 
    {
      encInfo->src_image_fname=argv[2]; //if it is present store the file in src_image_fname_variable 
    }
    else
    {
      fprintf(stderr,"Error:Source image should be .bmp fiile\n");
      return e_failure;
    }
    
    if(strstr(argv[3],".txt") || strstr(argv[3],".c") || strstr(argv[3],".sh"))  //set secret file name
    {
       encInfo->secret_fname=argv[3];
    }
    else
    {
      fprintf(stderr,"Errror:File should be .txt,.c,.sh file\n");
      return e_failure;

    }
     if(argv[4]!=NULL)
     {
      if(strstr(argv[4],".bmp"))     //check stego file name is provided
      {
        encInfo->stego_image_fname=argv[4];    // set stego file name
      }
      else{
        fprintf(stderr,"Error:File is not .bmp file\n");
        return e_failure;
      }
     }
     else{
          encInfo->stego_image_fname="stego.bmp";
     }
     return e_success;

    }
   Status do_encoding(EncodeInfo *encInfo)
   {

     if(open_encode_files(encInfo)==e_failure)   //open required files
     { 
              return e_failure;
     }
     if (validate_encode_inputs(encInfo) == e_failure)  //validate input file formats
    {
        fprintf(stderr, "Validation of encode inputs failed\n");
        return e_failure;
    }
     if(check_capacity(encInfo)==e_failure)   //check for image has enough space
     {
      return e_failure;
     }
     if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)  //copy header bmp as it is
     {
      return e_failure;
     }
     if(encode_magic_string( "#*",encInfo)==e_failure)//  encode magic string
     {
      return e_failure;
     }
     if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_failure)  //how many secret file data to be encoded
     {
      return e_failure;
     }
    const char *extn=strchr(encInfo->secret_fname,'.');
     int exten_len=strlen(extn);
     if(encode_extn_size(exten_len,encInfo)==e_failure)  //encode extension size
     {
      return e_failure;
     }
     printf("Encoded extension file is %d\n",exten_len);

     if (encode_secret_file_extn(extn, encInfo) == e_failure)  //get secret file extension
     {
    return e_failure;
     }
     printf("Encoded secret file extension is %s",extn);
     printf("Encoded secret file size is %ld\n",encInfo->size_secret_file);

     if (encode_secret_file_data(encInfo) == e_failure)  //encode actual secret file data to be encode
     {
    return e_failure;
     }
     printf("Secret file data is encoded successfully\n");
     if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure) //copy if the remaining image data 
    {
    return e_failure;
    }

      printf("Encoding done successfully\n");
      return e_success;
      

}
Status check_capacity(EncodeInfo *encInfo)//check image has enough space to hold all encoded data
{
  encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image); //get the source image size
   encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);  //get the secret file size
  int magic_string_len=2;  
  int required_capacity=magic_string_len*8+encInfo->size_secret_file*8+32+MAX_FILE_SUFFIX*8;
  if(required_capacity>encInfo->image_capacity)
  {
    fprintf(stderr,"ERROR:Image does not hold required capacity\n");
    return e_failure;
  }
  return e_success;
}

Status encode_byte_lsb(char data,char *image_buffer)  //encode a byte into LSB of 8 image bytes
{

  for(int i=0;i<8;i++)
  {
   image_buffer[i]=image_buffer[i]&~1;  //clear LSB of image 
   int bit=(data >> (7-i))&1;   //get a bit from data
   image_buffer[i]=image_buffer[i] | bit;  //encode a bit into LSB of image
    printf("Embedding bit %d into image byte[%d]: New value = %u\n", bit, i, image_buffer[i]); 
  }
  return e_success;
}  
  Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image) //copy bmp header as it is in the stego image
  {
    char header[54];
      rewind(fptr_src_image);
      rewind(fptr_dest_image);
      fread(header,1,54,fptr_src_image);
      fwrite(header,1,54,fptr_dest_image);
      return e_success;
  }
  Status encode_magic_string(const char* magic_string,EncodeInfo *encInfo) //encode the magic string to identify the presence of the secret data
  {
    char image_buffer[8];  //declare image buffer
    for(int i=0;magic_string[i]!='\0';i++)
    {
    fread(image_buffer,1,8,encInfo->fptr_src_image);  //read 8 bytes from image
    encode_byte_lsb(magic_string[i],image_buffer);   //encode 1 character into 8 image bytes in the lsb
    fwrite(image_buffer,1,8,encInfo->fptr_stego_image); //write encoded image buffer into stego image
    }
    printf("Magic string encoded succesfully");
    return e_success;
  }
  uint get_file_size(FILE *fptr)  //get file size by the using fseek and ftell
  {
    fseek(fptr,0,SEEK_END); // move the offset from the beginning to end of the file
    uint size=ftell(fptr);   //  return the size pointing current position and return the file size
    rewind(fptr); //move the file to start for further modification
    return size;
  }
 Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)  //encode file extension characters
 {
    int extn_file=strlen(file_extn); //find the size of file extension
    char image_buffer[8]; //declare image buffer
    for(int i=0;i<extn_file;i++)
    {
    fread(image_buffer,1,8,encInfo->fptr_src_image); //read 8 bytes from image
    encode_byte_lsb(file_extn[i],image_buffer);  //encode file extension characters into image buffer
    fwrite(image_buffer,1,8,encInfo->fptr_stego_image);  //write 8 bytes into stego image
    }
    return e_success;
 } 
 Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)  //encode size of 32 bit integer
 {
  char image_data[32];
  fread(image_data,1,32,encInfo->fptr_src_image);
  for(int i=0;i<32;i++)
  {
 //read 32 bytes of data from image buffer
    unsigned char bit = (file_size >> (31 - i)) & 1;
     image_data[i] = (image_data[i] & ~1) | bit;                               
      
  }
  fwrite(image_data,1,32,encInfo->fptr_stego_image);
       return e_success;                                   //read 32 bits encoded bits from image buffer to stego image
  //write 32bytes of encoded data into stego.bmp
 }

Status encode_secret_file_data(EncodeInfo *encInfo) // encode actual secret file data
{
    char ch;

    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
        if (fread(&ch, 1, 1, encInfo->fptr_secret) != 1)
        {
            fprintf(stderr, "ERROR: fread failed at byte %d\n", i);
            return e_failure;
        }

        if (encode_data_to_image(&ch, 1, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
        {
            fprintf(stderr, "ERROR: encode_data_to_image failed at byte %d\n", i);
            return e_failure;
        }

        printf("Encoding character[%d] = %c | ASCII = %d\n", i, ch, ch);
    }

    printf("Encoded secret file data successfully\n");
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image) //encode given data  buffer into image
{
    char image_data[8];

    for (int i = 0; i < size; i++)
    {
        if (fread(image_data, 1, 8, fptr_src_image) != 8)
        {
            fprintf(stderr, "ERROR: fread failed in encode_data_to_image at i=%d\n", i);
            return e_failure;
        }

        encode_byte_lsb(data[i], image_data);

        if (fwrite(image_data, 1, 8, fptr_stego_image) != 8)
        {
            fprintf(stderr, "ERROR: fwrite failed in encode_data_to_image at i=%d\n", i);
            return e_failure;
        }

        printf("Encoding character[%d]: '%c' | ASCII = %d\n", i, data[i], (unsigned char)data[i]);
    }

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest) //copy remaining image data after encoding section
{
   char image_buffer;
   while(fread(&image_buffer,1,1,fptr_src)>0)
   {
    fwrite(&image_buffer,1,1,fptr_dest);
   }
   return e_success;
}
Status encode_extn_size(int size,EncodeInfo *encInfo)  //encode size of extension in 32 bits
{
  unsigned char image_buffer[32];
  fread(image_buffer,1,32,encInfo->fptr_src_image);                                         // Read 32 bits from source image
   for(int i=0;i<32;i++)
   {
    unsigned char bit=(size>>(31-i))&1;
    image_buffer[i]=(image_buffer[i]&~1) | bit;
   }          
   fwrite(image_buffer,1,32,encInfo->fptr_stego_image);                                     // Encode 32-bit integer `size` into 32 bytes of LSBs
    return e_success;                                        // Write 32 bits into stego image
}
Status validate_encode_inputs(EncodeInfo *encInfo) // validate source bmp and secret file format
{
    // Validate .bmp extension 
    const char *extn = strrchr(encInfo->src_image_fname, '.'); //get file extension
    if (!extn || strcasecmp(extn, ".bmp") != 0)
    {
        fprintf(stderr, "file found is not .bmp file\n");
        return e_failure;
    }

    // Check BMP header
    char header[2];
    if (fread(header, 1, 2, encInfo->fptr_src_image) != 2)
    {
        fprintf(stderr, "ERROR: Could not read BMP header\n");
        return e_failure;
    }

    if (header[0] != 'B' || header[1] != 'M')  //get bmp header
    {
        fprintf(stderr, "ERROR: File is not .BMP FILE\n");
        return e_failure;
    }
    //validate .txt extension
    char header1[2];
    const char *dot=strchr(encInfo->secret_fname,'.'); // get file extension
    if(!dot || strcasecmp(dot,".txt")!=0)
    {
      fprintf(stderr,"File found is not .txt extension\n");
      return e_failure;
    }
    if(fread(header1,1,2,encInfo->fptr_secret)!=2)
    {
      fprintf(stderr,"ERROR:could not read txt header\n");
      return e_failure;
    if(header1[0]!= 't' || header1[1]!= 'x')  //get text file header
    {
      fprintf(stderr,"File does not have .txt extension\n");
      return e_failure;
    }
  }
    
    return e_success;
}
