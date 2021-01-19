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

double* non_local_means(double* input_image, int patchsize, double filter_sigma, double patch_sigma) {

}

int main() {
    int width, height, bpp;
    time_t t;

    /* Intializes random number generator */
   srand((unsigned) time(&t));

    uint8_t* original_image = stbi_load("../images/mpezos.jpg", &width, &height, &bpp, CHANNEL_NUM);
    double* normalized_noisy_image = (double *)malloc(width * height * sizeof(double));
    double** normalized_noisy_2D = (double **)malloc(height * sizeof(double *));
    for(int i = 0; i < height; i++) {
        normalized_noisy_2D[i] = (double *)malloc(width * sizeof(double));
    }
    
    stbi_write_jpg("../images/temp_boat.jpg", width, height, CHANNEL_NUM, original_image, 1);

    uint8_t* noisy_image = stbi_load("../images/temp_boat.jpg", &width, &height, &bpp, CHANNEL_NUM);

    /* Image Normalization and Noise Addition*/
    for(int i = 0; i < width * height; i++) {
        normalized_noisy_image[i] = (double)noisy_image[i] / (double)(MAX_IMAGE_VALUE - MIN_IMAGE_VALUE + 1);
    }

    /* Map to 2D */
    for(int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            normalized_noisy_2D[i][j] = normalized_noisy_image[i * width + j];
        }        
    }

    for (int i = 0; i < 5; i++)
    {
        printf("%f %f\n", normalized_noisy_image[i], normalized_noisy_2D[0][i]);
    }
    

    stbi_image_free(original_image);
    stbi_image_free(noisy_image);

    return 0;
}