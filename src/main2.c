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

int main() {
    int width, height, bpp;
    int width2, height2, bpp2;
    time_t t;

    /* Intializes random number generator */
   srand((unsigned) time(&t));

    uint8_t* original_image = stbi_load("../images/mpezos.jpg", &width, &height, &bpp, CHANNEL_NUM);
    double* normalized_noisy_image = (double *)malloc(width * height * sizeof(double));
    
    stbi_write_jpg("../images/temp_boat.jpg", width, height, CHANNEL_NUM, original_image, 1);

    uint8_t* noisy_image = stbi_load("../images/temp_boat.jpg", &width2, &height2, &bpp2, CHANNEL_NUM);

    /* Image Normalization and Noise Addition*/
    for(int i = 0; i < width * height; i++) {
        normalized_noisy_image[i] = (double)noisy_image[i] / (double)(MAX_IMAGE_VALUE - MIN_IMAGE_VALUE + 1);
    }

    stbi_image_free(original_image);
    stbi_image_free(noisy_image);

    return 0;
}