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

/**
 * Function that generates additive white Gaussian Noise samples
 * with zero mean and a standard deviation of 1. 
 * Input: 
 *      No input values
 * Output:
 *      float result * noise_sigma : The generated random sample to the caller
 *       
*/
float AWGN_generator()
{
    float temp1;
    float temp2;
    float result;
    int p;
    float noise_sigma = 0.02;

    p = 1;

    while (p > 0)
    {
        temp2 = (rand() / ((float)RAND_MAX)); /*  rand() function generates an
                                                  integer between 0 and  RAND_MAX,
                                                  which is defined in stdlib.h.
                                              */

        if (temp2 == 0)
        { // temp2 is >= (RAND_MAX / 2)
            p = 1;
        } // end if
        else
        { // temp2 is < (RAND_MAX / 2)
            p = -1;
        } // end else

    } // end while()

    temp1 = cos((2.0 * (float)PI) * rand() / ((float)RAND_MAX));
    result = sqrt(-2.0 * log(temp2)) * temp1;

    return result * noise_sigma;
}

/** 
* Function that implements the Non Local Means sequential algorithm.
* At first we allocate memory for the output image, for the pixel patch and for the comparison patch.
* Then we run a nested for loop in order to get the pixel patch and compare it with all the other patches.
* After the calculation we save the results in the output image and we return it.
* Input:
*       float **input_image  : values from every pixel for the input image in 2D
*       int patchsize        : value from the patch size
*       float filter_sigma   : value from the filter sigma
*       float patch_sigma    : value from the patch sigma
*       int width            : value for the image width
*       int height           : value for the image height
* Output:
*       float **output_image : values from every pixel for the output image in 2D
**/
float **non_local_means(float **input_image, int patchsize, float filter_sigma, float patch_sigma, int width, int height)
{   
    /* Allocate memory for the output image */
    float **output_image = (float **)malloc(height * sizeof(float *));
    for (int i = 0; i < height; i++)
    {
        output_image[i] = (float *)malloc(width * sizeof(float));
    }

    /* Allocate memory for the pixel patch */
    float **pixel_patch = (float **)malloc(patchsize * sizeof(float *));
    for (int i = 0; i < patchsize; i++)
    {
        pixel_patch[i] = (float *)malloc(patchsize * sizeof(float));
    }

    /* Allocate memory for the comparison patch */
    float **comparison_patch = (float **)malloc(patchsize * sizeof(float *));
    for (int i = 0; i < patchsize; i++)
    {
        comparison_patch[i] = (float *)malloc(patchsize * sizeof(float));
    }

    /* Loop for each pixel that is inside the patchsize limits */
    for (int i = patchsize / 2; i < height - patchsize / 2; i++)
    {
        for (int j = patchsize / 2; j < width - patchsize / 2; j++)
        {
            /* Create the patchsize * patchsize grid with the selected pixel at the centre */
            int counter_i = 0;
            for (int k = -patchsize / 2; k < patchsize / 2 + 1; k++)
            {
                int counter_j = 0;
                for (int l = -patchsize / 2; l < patchsize / 2 + 1; l++)
                {
                    pixel_patch[counter_i][counter_j++] = input_image[i + k][j + l];
                }
                counter_i++;
            }

            /* Initialize the ouput image value and the zeta to zero */
            output_image[i][j] = 0;
            float zeta = 0;

            /* Comparison patch (we take into account ourselves too) */
            for (int m = patchsize / 2; m < height - patchsize / 2; m++)
            {
                for (int n = patchsize / 2; n < width - patchsize / 2; n++)
                {
                    /* Create the patchsize * patchsize grid with the selected pixel at the centre */
                    int counter_i = 0;
                    for (int k = -patchsize / 2; k < patchsize / 2 + 1; k++)
                    {
                        int counter_j = 0;
                        for (int l = -patchsize / 2; l < patchsize / 2 + 1; l++)
                        {
                            comparison_patch[counter_i][counter_j++] = input_image[m + k][n + l];
                        }
                        counter_i++;
                    }

                    /* Here we implement the Non Local Means algorithm */
                    float difference_squared = 0;
                    for (int a = 0; a < patchsize; a++)
                    {
                        for (int b = 0; b < patchsize; b++)
                        {
                            float distX = (a - patchsize / 2) * (a - patchsize / 2);
                            float distY = (b - patchsize / 2) * (b - patchsize / 2);
                            float dist = -(distX + distY) / (patch_sigma * patch_sigma);
                            dist = exp(dist);

                            difference_squared += dist * (pixel_patch[a][b] - comparison_patch[a][b]) * (pixel_patch[a][b] - comparison_patch[a][b]);
                        }
                    }

                    float w_difference_squared = -difference_squared / (filter_sigma * filter_sigma);

                    float w = exp(w_difference_squared);
                    zeta += w;

                    output_image[i][j] += input_image[m][n] * w;
                }
            }
            /* Save the output value after the calculation */
            output_image[i][j] = output_image[i][j] / zeta;
        }
    }

    /* Deallocate used memory */
    free(comparison_patch);
    free(pixel_patch);

    return output_image;
}

/**
 * Main has 4 command line arguments
 * Argument 1: Path for jpg file, Argument 2: Patch size value, Argument 3: Filter Sigma value, Argument 4: Patch Sigma value
 * 
*/
int main(int argc, char **argv)
{

    /* Initialize the variables by using the arguments */
    int width, height, bpp;
    int patchsize = atoi(argv[2]);

    time_t t;

    char *image_file_name = argv[1];
    char buf[512];

    float filter_signa = atof(argv[3]); // 0.2
    float patch_sigma = atof(argv[4]);  // 1.67

    /* Initialize random number generator */
    srand((unsigned)time(&t));

    /* Load the original image and allocate memory for the normalized and the noisy images */
    uint8_t *original_image = stbi_load(image_file_name, &width, &height, &bpp, 1);
    float *normalized_image = (float *)malloc(width * height * sizeof(float));
    float *noisy_image = (float *)malloc(width * height * sizeof(float));
    uint8_t *noisy_image_for_save = (uint8_t *)malloc(width * height * sizeof(uint8_t));

    float **normalized_noisy_2D = (float **)malloc(height * sizeof(float *));
    for (int i = 0; i < height; i++)
    {
        normalized_noisy_2D[i] = (float *)malloc(width * sizeof(float));
    }

    float **normalized_denoised_2D = (float **)malloc(height * sizeof(float *));
    for (int i = 0; i < height; i++)
    {
        normalized_denoised_2D[i] = (float *)malloc(width * sizeof(float));
    }

    /* Allocate memory for noise subtracted and denoised images */
    uint8_t *noise_subtracted_image = (uint8_t *)malloc(width * height * sizeof(uint8_t));
    uint8_t *denoised_image = (uint8_t *)malloc(width * height * sizeof(uint8_t));

    /* Image Normalization and Noise Addition*/
    for (int i = 0; i < width * height; i++)
    {
        normalized_image[i] = (float)original_image[i] / (float)(MAX_IMAGE_VALUE - MIN_IMAGE_VALUE + 1);
        noisy_image[i] = normalized_image[i] + AWGN_generator();

        /* Check and avoid getting out of the limit colors */
        if (noisy_image[i] > 1)
        {
            noisy_image[i] = 1;
        }

        if (noisy_image[i] < 0)
        {
            noisy_image[i] = 0;
        }
    }

    /* Denormalize and Save the noisy image */
    for (int i = 0; i < width * height; i++)
    {
        noisy_image_for_save[i] = (uint8_t)(noisy_image[i] * 255);
    }

    snprintf(buf, sizeof buf, "%s%s_%d_%s", "../images/", image_file_name, patchsize, "black_white.jpg");
    stbi_write_jpg(buf, width, height, 1, original_image, 0);
    snprintf(buf, sizeof buf, "%s%s_%d_%s", "../images/", image_file_name, patchsize, "noisy.jpg");
    stbi_write_jpg(buf, width, height, 1, noisy_image_for_save, 0);

    /* Map to 2D */
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            normalized_noisy_2D[i][j] = noisy_image[i * width + j];
        }
    }

    /* Start measuring time */
    clock_t begin = clock();

    /* Run algorith that calculates the denoised image */
    normalized_denoised_2D = non_local_means(normalized_noisy_2D, patchsize, filter_signa, patch_sigma, width, height);

    /* Stop measuring time */
    clock_t end = clock();
    double duration = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Duration: %f\n", duration);

    /* Denormalize and map into 1D the denoised image */
    int counter = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            denoised_image[counter++] = normalized_denoised_2D[i][j] * 255;
        }
    }

    /* Calculate the Noise subtraction */
    counter = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            noise_subtracted_image[counter] = 128 + (noisy_image_for_save[counter] - denoised_image[counter]);
            counter++;
        }
    }

    /* Save the noise subtracted image */
    snprintf(buf, sizeof buf, "%s%s_%d_%s", "../images/", image_file_name, patchsize, "noise_subtracted.jpg");
    stbi_write_jpg(buf, width, height, CHANNEL_NUM, noise_subtracted_image, 0);

    /* Save the noise denoised image */
    snprintf(buf, sizeof buf, "%s%s_%d_%s", "../images/", image_file_name, patchsize, "denoised.jpg");
    stbi_write_jpg(buf, width, height, CHANNEL_NUM, denoised_image, 0);

    /* Deallocate used memory */
    stbi_image_free(original_image);
    free(noisy_image_for_save);
    free(normalized_image);
    free(noisy_image);
    free(normalized_noisy_2D);
    free(normalized_denoised_2D);
    free(noise_subtracted_image);
    free(denoised_image);

    return 0;
}
