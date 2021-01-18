#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define CHANNEL_NUM 1

#define MAX_IMAGE_VALUE 255
#define MIN_IMAGE_VALUE 0
#define PI 3.1415926536

double AWGN_generator() {
    double noise_sigma = 0.01;
    double u1 = ((double)rand() / RAND_MAX);
    double u2 = ((double)rand() / RAND_MAX);

    double z0 = sqrt(-2 * log(u1)) * cos(2 * PI * u2) * noise_sigma;
    double z1 = sqrt(-2 * log(u1)) * sin(2 * PI * u2) * noise_sigma;
    // printf("z0: %f \n", z0);
    // printf("z1: %f \n", z1);

    return z0;
}

double AWGN_generator2()
{/* Generates additive white Gaussian Noise samples with zero mean and a standard deviation of 1. */
 
  double temp1;
  double temp2;
  double result;
  int p;
  double noise_sigma = 0.01;

  p = 1;

  while( p > 0 )
  {
	temp2 = ( rand() / ( (double)RAND_MAX )); /*  rand() function generates an
                                                       integer between 0 and  RAND_MAX,
                                                       which is defined in stdlib.h.
                                                   */

    if ( temp2 == 0 )
    {// temp2 is >= (RAND_MAX / 2)
      p = 1;
    }// end if
    else
    {// temp2 is < (RAND_MAX / 2)
       p = -1;
    }// end else

  }// end while()

  temp1 = cos( ( 2.0 * (double)PI ) * rand() / ( (double)RAND_MAX ) );
  result = sqrt( -2.0 * log( temp2 ) ) * temp1;

  return result * noise_sigma;	// return the generated random sample to the caller

}

int main() {
    int width, height, bpp;
    time_t t;

    /* Intializes random number generator */
   srand((unsigned) time(&t));

    uint8_t* original_image = stbi_load("../images/mpezos.jpg", &width, &height, &bpp, 1);
    double* normalized_image = (double *)malloc(width * height * sizeof(double));
    double* noisy_image = (double *)malloc(width * height * sizeof(double));
    uint8_t* noisy_image_for_save = (uint8_t *)malloc(width * height * sizeof(uint8_t));

    /* Image Normalization and Noise Addition*/
    for(int i = 0; i < width * height; i++) {
        normalized_image[i] = (double)original_image[i] / (double)(MAX_IMAGE_VALUE - MIN_IMAGE_VALUE + 1);
        noisy_image[i] = normalized_image[i] + AWGN_generator2();
    }

    /* Denormalize and Save the noisy image */
    for (int i = 0; i < width * height; i++) {
        noisy_image_for_save[i] = (uint8_t)(noisy_image[i] * 255);
    }
    
    stbi_write_jpg("../images/noisy_boat.jpg", width, height, 1, original_image, 1);

    printf("%d \n", noisy_image_for_save[1000]);
    printf("%d \n", original_image[1000]);

    stbi_image_free(original_image);
    free(noisy_image_for_save);
    free(normalized_image);
    free(noisy_image);

    return 0;
}