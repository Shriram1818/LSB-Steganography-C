# LSB Image Steganography in C

## Overview

This project implements **Image Steganography using the Least Significant Bit (LSB) technique** in the C programming language.

Steganography is a method of hiding secret information inside another file. In this project, secret text data is embedded into a **BMP image file** by modifying the least significant bits of pixel data. The hidden data can later be extracted using the decoding program.

The project demonstrates concepts of **bit manipulation, file handling, and data encoding/decoding in C**.

---

## Features

* Encode secret message inside a BMP image
* Decode hidden message from encoded image
* Uses Least Significant Bit (LSB) technique
* Supports text file as secret data
* Maintains original image appearance
* Command line based interface

---

## Technologies Used

* C Programming
* File Handling
* Bit Manipulation
* BMP Image Format

---

## Project Structure

```
LSB-Steganography-C
├── encode.c
├── decode.c
├── encode.h
├── decode.h
├── common.h
├── types.h
├── test_encode.bmp
├── secret.txt
└── README.md
```

---

## How the LSB Technique Works

In a BMP image, each pixel is stored using bytes of data.
The **Least Significant Bit** of each byte is modified to store the secret message.

Example:

Original byte:

```
10010110
```

After encoding 1 bit of secret data:

```
10010111
```

Since only the last bit changes, the **visual appearance of the image remains almost unchanged**.

---

## Compilation

Use the following command to compile the program:

```
gcc encode.c decode.c -o steganography
```

---

## Usage

### Encoding Secret Message

```
./steganography -e input.bmp secret.txt output.bmp
```

### Decoding Secret Message

```
./steganography -d output.bmp decoded.txt
```

---

## Example Output

```
Encoding Magic String
Encoding Secret File Extension
Encoding Secret File Size
Encoding Secret File Data
Encoding Completed Successfully
```

---

## Learning Outcomes

* Understanding steganography concepts
* Bit-level data manipulation
* File I/O operations in C
* Working with binary image formats
* Implementing encoding and decoding algorithms

---

## Future Improvements

* Support for PNG and JPEG images
* GUI based interface
* Encryption before embedding data

---

## Author

Shriram Vishwanath Pai
