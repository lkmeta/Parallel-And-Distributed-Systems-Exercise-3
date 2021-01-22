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
  double noise_sigma = 0.05;

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

double** non_local_means(double** input_image, int patchsize, double filter_sigma, double patch_sigma, int width, int height) {
    double** output_image = (double **)malloc(height * sizeof(double *));
    for(int i = 0; i < height; i++) {
        output_image[i] = (double *)malloc(width * sizeof(double));
    }

    /* Fill the outside pixels with the pre existing values */
    for(int i = 0; i < patchsize / 2; i++) {
        for(int j = 0; j < width; j++) {
            output_image[i][j] = input_image[i][j];
        }
    }
    for(int i = height - patchsize; i < height; i++) {
        for(int j = 0; j < width; j++) {
            output_image[i][j] = input_image[i][j];
        }
    }
    for(int j = 0; j < patchsize / 2; j++) {
        for(int i = 0; i < height; i++) {
            output_image[i][j] = input_image[i][j];
        }
    }
    for(int j = width - patchsize; j < width; j++) {
        for(int i = 0; i < height; i++) {
            output_image[i][j] = input_image[i][j];
        }
    }

    /* Loop for each pixel that is inside the patchsize limits */
    for(int i = patchsize / 2; i < height - patchsize / 2; i++) {
        for(int j = patchsize / 2; j < width - patchsize / 2; j++) {
            /* Create the patchsize * patchsize grid with the selected pixel at the centre */
            double* pixel_patch = (double *)malloc(patchsize * patchsize * sizeof(double));
            int counter = 0;
            for(int k = -patchsize / 2; k < patchsize / 2 + 1 ; k++) {
                for(int l = -patchsize / 2; l < patchsize / 2 + 1; l++) {
                    pixel_patch[counter++] = input_image[i + k][j + l];
                }
            }
            /* Initialize the ouput image value to zero */
            output_image[i][j] = 0;
            double zeta = 0;
            /* Comparison patch (we take into account ourselves too) */
            for(int m = patchsize / 2; m < height - patchsize / 2; m++) {
                for(int n = patchsize / 2; n < width - patchsize / 2; n++) { 
                    /* Create the patchsize * patchsize grid with the selected pixel at the centre */
                    double* comparison_patch = (double *)malloc(patchsize * patchsize * sizeof(double));
                    int counter = 0;
                    for(int k = -patchsize / 2; k < patchsize / 2 + 1 ; k++) {
                        for(int l = -patchsize / 2; l < patchsize / 2 + 1; l++) {
                            comparison_patch[counter++] = input_image[m + k][n + l];
                        }
                    }
                    /* Here we should implement the f algorithm */
                    double difference_squared = 0;
                    for(int a = 0; a < patchsize * patchsize; a++) {
                        difference_squared += (pixel_patch[a] - comparison_patch[a]) * (pixel_patch[a] - comparison_patch[a]);
                    }
                    double w_difference_squared = - difference_squared / (filter_sigma * filter_sigma);
                    double w = exp(w_difference_squared);
                    zeta += w;
                    output_image[i][j] += input_image[i][j] * w;
                    if(i == 5 && j == 5 && output_image[i][j] > 0) {
                        printf("w: %f output_image[5][5]: %f\n", w, output_image[i][j]);
                    }
                }
            }
            output_image[i][j] = output_image[i][j] / zeta;
        }
    }
    return output_image;
}

int main() {
    int width, height, bpp;
    int patchsize = 3;
    time_t t;

    /* Intializes random number generator */
    srand((unsigned) time(&t));

    uint8_t* original_image = stbi_load("../images/musk.jpg", &width, &height, &bpp, 1);
    double* normalized_image = (double *)malloc(width * height * sizeof(double));
    double* noisy_image = (double *)malloc(width * height * sizeof(double));
    uint8_t* noisy_image_for_save = (uint8_t *)malloc(width * height * sizeof(uint8_t));

    double** normalized_noisy_2D = (double **)malloc(height * sizeof(double *));
    for(int i = 0; i < height; i++) {
        normalized_noisy_2D[i] = (double *)malloc(width * sizeof(double));
    }

    double** normalized_denoised_2D = (double **)malloc(height * sizeof(double *));
    for(int i = 0; i < height; i++) {
        normalized_denoised_2D[i] = (double *)malloc(width * sizeof(double));
    }

    uint8_t* denoised_image = (uint8_t *)malloc(width * height * sizeof(uint8_t));

    /* Image Normalization and Noise Addition*/
    for(int i = 0; i < width * height; i++) {
        normalized_image[i] = (double)original_image[i] / (double)(MAX_IMAGE_VALUE - MIN_IMAGE_VALUE + 1);
        noisy_image[i] = normalized_image[i] + AWGN_generator2();
        /* Fix black and white bullets */
        if(noisy_image[i] > 1) {
            noisy_image[i] = 1;
        }
        if(noisy_image[i] < 0) {
            noisy_image[i] = 0;
        }
    }

    /* Denormalize and Save the noisy image */
    for (int i = 0; i < width * height; i++) {
        noisy_image_for_save[i] = (uint8_t)(noisy_image[i] * 255);
    }
    
    stbi_write_jpg("../images/musk_black_white.jpg", width, height, 1, original_image, 0);
    stbi_write_jpg("../images/noisy_image.jpg", width, height, 1, noisy_image_for_save, 0);

    /* Map to 2D */
    for(int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            normalized_noisy_2D[i][j] = noisy_image[i * width + j];
        }        
    }

    normalized_denoised_2D = non_local_means(normalized_noisy_2D, patchsize, 0.05, 1.66, width, height);

    /* Denormalize and map into 1D the denoised image */
    int counter = 0;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            denoised_image[counter++] = normalized_denoised_2D[i][j] * 255;
            // denoised_image[counter++] = normalized_denoised_2D[i][j] * 0;
        }
    }

    stbi_write_jpg("../images/denoised_image.jpg", width, height, CHANNEL_NUM, denoised_image, 0);

    stbi_image_free(original_image);
    free(noisy_image_for_save);
    free(normalized_image);
    free(noisy_image);

    return 0;
}