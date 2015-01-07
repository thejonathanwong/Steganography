<h1>Basic Image Steganography</h1>

This directory currently contains two c files: imEncrypt and imDecrypt. These two files when
combined can be used to encrypt an ASCII string within an image, and then decrypt it. 

imEncrypt can be run using the following command:

```
./imEncrypt <TIFF Image File>
```

You will then be given the maximum number of characters that can be encoded within the given image,
and then prompted to enter in a string to be encrypted.

imDecrypt can be run using the following command:

```
./imDecrypt <TIFF Image File>
```



<h3>TODO:</h3>

Parallelized version of the program
Compression of the image
Increase number of chars that can be encoded

