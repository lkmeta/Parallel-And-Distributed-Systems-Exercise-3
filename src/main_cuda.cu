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

float AWGN_generator2()
{ /* Generates additive white Gaussian Noise samples with zero mean and a standard deviation of 1. */

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

    return result * noise_sigma; // return the generated random sample to the caller
}

__global__ void pixel_algorithm(float *input_image_gpu, float *output_image_gpu, float *pixel_patch_gpu, float *comparison_patch_gpu, int width, int height, int patchsize, float filter_sigma, float patch_sigma) {
    int ix = blockIdx.x * blockDim.x + threadIdx.x;
    int i = ix / height;
    int j = ix % width;

    // printf("width - patchsize / 2 - 1: %d\n", width - patchsize / 2 - 1);

    

    // Return if out of computation bounds
    if(i < patchsize / 2 || i > width - patchsize / 2 - 1) {
        // printf("Returning for ix: %d\t i: %d\t j: %d\n", ix, i , j);
        return;
    }
    if(j < patchsize / 2 || j > height - patchsize / 2 - 1) {
        // printf("Returning for ix: %d\t i: %d\t j: %d\n", ix, i , j);
        return;
    }
    // printf("%d\t i: %d\t j: %d\n", ix, i , j);

    /* Create the patchsize * patchsize grid with the selected pixel at the centre */
    int counter_i = 0;
    for (int k = -patchsize / 2; k < patchsize / 2 + 1; k++)
    {
        int counter_j = 0;
        for (int l = -patchsize / 2; l < patchsize / 2 + 1; l++)
        {
            pixel_patch_gpu[counter_i * (patchsize) + counter_j] = input_image_gpu[(i + k) * width + (j + l)];
            counter_j++;
        }
        counter_i++;
    }

    /* Initialize the ouput image value to zero */
    output_image_gpu[i * width + j] = 0;
    float zeta = 0;

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
                    comparison_patch_gpu[counter_i * (patchsize) + counter_j] = input_image_gpu[(m + k) * width + (n + l)];
                    counter_j++;
                }
                counter_i++;
            }

            /* Here we should implement the f algorithm */
            float difference_squared = 0;
            for (int a = 0; a < patchsize; a++)
            {
                for (int b = 0; b < patchsize; b++)
                {
                    float distX = (a - patchsize / 2) * (a - patchsize / 2);
                    float distY = (b - patchsize / 2) * (b - patchsize / 2);
                    float dist = -(distX + distY) / (patch_sigma * patch_sigma);
                    dist = exp(dist);
                    //used to be patchsize / 2 but could not remember why so i reverted it
                    difference_squared += dist * (pixel_patch_gpu[a * (patchsize) + b] - comparison_patch_gpu[a * (patchsize) + b]) * (pixel_patch_gpu[a * (patchsize) + b] - comparison_patch_gpu[a * (patchsize) + b]);
                }
            }

            float w_difference_squared = -difference_squared / (filter_sigma * filter_sigma);

            float w = exp(w_difference_squared);
            zeta += w;

            output_image_gpu[i * width + j] += input_image_gpu[m * width + n] * w;
        }
    }
    output_image_gpu[i * width + j] = output_image_gpu[i * width + j] / zeta;
    printf("%d\t i: %d\t j: %d\t output_image_gpu[%d]: %f\n", ix, i , j, i * width + j, output_image_gpu[i * width + j]);
}

float *non_local_means(float *input_image, int patchsize, float filter_sigma, float patch_sigma, int width, int height)
{
    float *output_image = (float *)malloc(height * width * sizeof(float));

    float *output_image_gpu;
    cudaMalloc(&output_image_gpu, height * width * sizeof(float));

    float *input_image_gpu;
    cudaMalloc(&input_image_gpu, height * width * sizeof(float));

    float *pixel_patch_gpu;
    cudaMalloc(&pixel_patch_gpu, patchsize * patchsize * sizeof(float));

    float *comparison_patch_gpu;
    cudaMalloc(&comparison_patch_gpu, patchsize * patchsize * sizeof(float));

    // cudaMemcpy(output_image_gpu, output_image, height * width * sizeof(float), cudaMemcpyHostToDevice); // This might be useless in the beggining
    cudaMemcpy(input_image_gpu, input_image, height * width * sizeof(float), cudaMemcpyHostToDevice);

    // Technically here we would like to initiate width * height number of threads in order to compute in parallel the output value of each i in width*size
    pixel_algorithm<<<width, height>>>(input_image_gpu, output_image_gpu, pixel_patch_gpu, comparison_patch_gpu, width, height, patchsize, filter_sigma , patch_sigma);

    cudaMemcpy(output_image, output_image_gpu, height * width * sizeof(float), cudaMemcpyDeviceToHost);
    return output_image;
}

int main()
{
    int width, height, bpp;
    int patchsize = 3;
    time_t t;

    /* Intializes random number generator */
    srand((unsigned)time(&t));

    uint8_t *original_image = stbi_load("../images/musk.jpg", &width, &height, &bpp, 1);
    float *normalized_image = (float *)malloc(width * height * sizeof(float));
    float *noisy_image = (float *)malloc(width * height * sizeof(float));
    float *denoised_image_float = (float *)malloc(width * height * sizeof(float));
    uint8_t *noisy_image_for_save = (uint8_t *)malloc(width * height * sizeof(uint8_t));

    // float **normalized_noisy_2D = (float **)malloc(height * sizeof(float *));
    // for (int i = 0; i < height; i++)
    // {
    //     normalized_noisy_2D[i] = (float *)malloc(width * sizeof(float));
    // }

    // float **normalized_denoised_2D = (float **)malloc(height * sizeof(float *));
    // for (int i = 0; i < height; i++)
    // {
    //     normalized_denoised_2D[i] = (float *)malloc(width * sizeof(float));
    // }

    uint8_t *denoised_image = (uint8_t *)malloc(width * height * sizeof(uint8_t));

    /* Image Normalization and Noise Addition*/
    for (int i = 0; i < width * height; i++)
    {
        normalized_image[i] = (float)original_image[i] / (float)(MAX_IMAGE_VALUE - MIN_IMAGE_VALUE + 1);
        noisy_image[i] = normalized_image[i] + AWGN_generator2();
        //noisy_image[i] = normalized_image[i];
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

    stbi_write_jpg("../images/fight_black_white.jpg", width, height, 1, original_image, 0);
    stbi_write_jpg("../images/noisy_image.jpg", width, height, 1, noisy_image_for_save, 0);

    // /* Map to 2D */
    // for (int i = 0; i < height; i++)
    // {
    //     for (int j = 0; j < width; j++)
    //     {
    //         normalized_noisy_2D[i][j] = noisy_image[i * width + j];
    //     }
    // }

    denoised_image_float = non_local_means(noisy_image, patchsize, 0.2, 1.67, width, height);

    /* Denormalize and Save the denoised image */
    for (int i = 0; i < width * height; i++)
    {
        denoised_image[i] = (uint8_t)(denoised_image_float[i] * 255);
    }

    // /* Denormalize and map into 1D the denoised image */
    // int counter = 0;
    // for (int i = 0; i < height; i++)
    // {
    //     for (int j = 0; j < width; j++)
    //     {
    //         denoised_image[counter++] = normalized_denoised_2D[i][j] * 255;
    //         // denoised_image[counter++] = normalized_denoised_2D[i][j] * 0;
    //     }
    // }

    stbi_write_jpg("../images/denoised_image.jpg", width, height, CHANNEL_NUM, denoised_image, 0);
    
    printf("Width: %d\nHeight: %d\nTotal Kernels", width, height, width * height);

    stbi_image_free(original_image);
    free(noisy_image_for_save);
    free(normalized_image);
    free(noisy_image);

    return 0;
}