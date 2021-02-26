# Non Local Means
# **Parallel and Distributed Computer Systems**  
## **Exercise 3**
## Marios Pakas | mariospakas@ece.auth.gr | 9498  <br />
## Meta Louis-Kosmas | louismeta@ece.auth.gr | 9390 <br />

Taskbar

* Write the sequential cpu c code
    * Read image file - Done
    * Normalise the values of the image (0-1) - Done
    * Add noise - Done
    * Save the noisy image - Done
    * Denoise using the algorithm- Done
* Parallelized the existing code with cuda
    * Parallelized the two outer loops
* ...

---
### Contents
   1. [How to build](#1)
   2. [Results from HPC](#2)
   3. [Graphs](#3)
   4. [Sources](#4)
   
<a name="1"></a>
### **How to build** 

**File ```Makefile```:**  
   + Use command line ```make main``` to build main function with C.
   + Use command line ```make cuda``` to build main function with C.
   + Use command line ```make testMain``` to test sequential code with an image (Elon Musk image 64px with patch size 5px, filter sigma value 0.2 and patch sigma value 1.67). 
   + Use command line ```make testCuda``` to test parallel code with an image (Elon Musk image 128px with patch size 7px, filter sigma value 0.2 and patch sigma value 1.67). 
   
**Examples:**  
To run the functions you should contain 4 arguments.  
If you want to run a function with one of the files that already in images folder run as shown below:
   + ```../out/main <Path for jpg file> <Patch size value> <Filter Sigma value> <Patch Sigma value>```   
   + ```../out/main_cuda <Path for jpg file> <Patch size value> <Filter Sigma value> <Patch Sigma value>``` 

---
<a name="2"></a>
### **Results from HPC** 

**Note:** We used the following [Scripts](TODO) in AUTh High Performance Computing (HPC) infrastructure to test our code.  
We used different time durations for these scripts to compute the [results](TODO) for the tested files. 

**Script [```sequential.sh```](TODO):**
   + calculates Non Local Means using sequential C code with the files for the following combinations of patch size values
   + patch size values: 3 5 7
   + creates the directory results and outputs for sequential code

**Script [```parallel.sh```](TODO):**
   + calculates Non Local Means using paraller Cuda code with the files for the following combinations of patch size values
   + patch size values: 3 5 7
   + creates the directory results and outputs for parallel code

**_Code Validation_**  
The correctness of the sequential and the parallel code for the Non Local Means algorithm we used can be easily verified by observing the following images. 
Firstly, we see the file we use as input in our functions. The second image shows the same image but in black and white. Then with the appropriate function we add [AWGN](https://en.wikipedia.org/wiki/Additive_white_Gaussian_noise) as you can distinguish from the 3rd one. Finally, we use the algorithm and we remove the Gaussian Noise as you can notice in the 4th image. The last image shows the subtracted noise.

![Elon Musk](https://github.com/Mavioux/Parallel-And-Distributed-Systems-Exercise-3/blob/main/images/musk_final.jpg)  

---
<a name="3"></a>
### **Graphs** 

Serial Implemention using only CPU
|     | 3      | 5      | 7      |
|:---:| :---:  | :---:  | :---:  |
| 64  | 6,57   | 12,61  | 20,97  |
| 128 | 111,28 | 240,43 |        |
| 256 |        |        |        |

TODO


### Helpful functions

In order to manipulate images in c, a library found on github was used that enabled image loading and storing in 1D arrays<sup>[1]</sup>. For the addition of white gaussian noise to the original image, the box-muller algorithm was used. There were two awgn_generator functions created, one was our own implementation, and the second one was a function found on a site<sup>[2]</sup>. Since the second function seemed to provide better noisy images, we proceeded  with that implementation.

### NonLocal Image Algorithm

#### Serial

The serial implementation of the algorithm looped through each individual image pixel and calculated its final value. To achieve that, it created an array(pixel_patch) with a size of patchsize2, that would be then compared with the comparison_patch array of each individual pixel  of the image. For that a second loop was used to go through each pixel of the image, and the comparison_patch was stored and then subtracted from the pixel_patch. This, divided by sigma2, given a negative sign and then passed as a power of e would be multiplied with the value of the comparison pixel and added to the value of the original pixel. All those weights would be added to a variable zeta, with which we will divide the value of the original pixel, so as to make sure that in the end the value is between 0 and 1. When the initial loop is finished (O(n4)) the image produced would have been smoothed out and the gaussian noise should not be visible anymore.

#### Cuda

So as to speed up the calculation of the value of each pixel, parallel programming with the help of a vector architecture, such as Cuda, was used. The general idea here was to call a width*height kernel and assign each pixel to a different gpu thread. This enabled us to calculate the final value the same way as serially, but simultaneously for each pixel. This was the complexity of the code was reduced down to O(n2) with dramatic differences to the time it took to finish the job, especially as the size of the image or the patchsize increases.


---
<a name="4"></a>
### **Sources** 

[1] Image Manipulation Library: https://github.com/nothings/stb/

[2] AWGN: https://www.embeddedrelated.com/showcode/311.php

[3] Extra PDF on NonLocal Image Algorithm: https://hal.archives-ouvertes.fr/hal-00271147/PDF/ijcvrevised.pdf

