<h1>Basic Image Steganography</h1>

This project is an extension of a final project for a CS course in Parallel Computing. The goal is
to create a program that takes an input string, and encodes it within an image by manipulating the
bits of the pixels. 

This program is supposed to have both a linear and parallel version, however, only the linear
version is implemented at the moment.

<h3>Linear</h3>

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

<h3>Parallel</h3>



<h3>TODO:</h3>

<ul>
<li>Parallelized version of the program</li>
<li>Compression of the image</li>
<li>Increase number of chars that can be encoded</li>
</ul>

