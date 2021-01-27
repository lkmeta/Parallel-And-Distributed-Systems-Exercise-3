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

double AWGN_generator()
{
    double noise_sigma = 0.01;
    double u1 = ((double)rand() / RAND_MAX);
    double u2 = ((double)rand() / RAND_MAX);

    double z0 = sqrt(-2 * log(u1)) * cos(2 * PI * u2) * noise_sigma;
    double z1 = sqrt(-2 * log(u1)) * sin(2 * PI * u2) * noise_sigma;
    return z0;
}

double AWGN_generator2()
{ /* Generates additive white Gaussian Noise samples with zero mean and a standard deviation of 1. */

    double temp1;
    double temp2;
    double result;
    int p;
    double noise_sigma = 0.02;

    p = 1;

    while (p > 0)
    {
        temp2 = (rand() / ((double)RAND_MAX)); /*  rand() function generates an
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

    temp1 = cos((2.0 * (double)PI) * rand() / ((double)RAND_MAX));
    result = sqrt(-2.0 * log(temp2)) * temp1;

    return result * noise_sigma; // return the generated random sample to the caller
}

// double **non_local_means(double **input_image, int patchsize, double filter_sigma, double patch_sigma, int width, int height)
// {
//     double **output_image = (double **)malloc(height * sizeof(double *));
//     for (int i = 0; i < height; i++)
//     {
//         output_image[i] = (double *)malloc(width * sizeof(double));
//     }

//     /* Fill the outside pixels with the pre existing values */
//     // for(int i = 0; i < patchsize / 2; i++) {
//     //     for(int j = 0; j < width; j++) {
//     //         output_image[i][j] = input_image[i][j];
//     //     }
//     // }
//     // for(int i = height - patchsize; i < height; i++) {
//     //     for(int j = 0; j < width; j++) {
//     //         output_image[i][j] = input_image[i][j];
//     //     }
//     // }
//     // for(int j = 0; j < patchsize / 2; j++) {
//     //     for(int i = 0; i < height; i++) {
//     //         output_image[i][j] = input_image[i][j];
//     //     }
//     // }
//     // for(int j = width - patchsize; j < width; j++) {
//     //     for(int i = 0; i < height; i++) {
//     //         output_image[i][j] = input_image[i][j];
//     //     }
//     // }

//     for (int i = 0; i < height; i++)
//     {
//         for (int j = 0; j < width; j++)
//         {
//             output_image[i][j] = input_image[i][j];
//         }
//     }

//     printf("\nDuring denoising 1\n");
//     for (int i = 0; i < 10; i++)
//     {
//         for (int j = 0; j < width; j++)
//         {
//             printf(" %f ", output_image[i][j]);
//         }
//         printf("\n");
//     }

//     /* Loop for each pixel that is inside the patchsize limits */
//     for (int i = patchsize / 2; i < height - patchsize / 2; i++)
//     {
//         for (int j = patchsize / 2; j < width - patchsize / 2; j++)
//         {
//             /* Create the patchsize * patchsize grid with the selected pixel at the centre */
//             double *pixel_patch = (double *)malloc(patchsize * patchsize * sizeof(double));
//             int counter = 0;
//             for (int k = -patchsize / 2; k < patchsize / 2 + 1; k++)
//             {
//                 for (int l = -patchsize / 2; l < patchsize / 2 + 1; l++)
//                 {
//                     pixel_patch[counter++] = input_image[i + k][j + l];
//                 }
//             }
//             /* Initialize the ouput image value to zero */
//             output_image[i][j] = 0;
//             double zeta = 0;
//             /* Comparison patch (we take into account ourselves too) */
//             for (int m = patchsize / 2; m < height - patchsize / 2; m++)
//             {
//                 for (int n = patchsize / 2; n < width - patchsize / 2; n++)
//                 {
//                     /* Create the patchsize * patchsize grid with the selected pixel at the centre */
//                     double *comparison_patch = (double *)malloc(patchsize * patchsize * sizeof(double));
//                     int counter = 0;
//                     for (int k = -patchsize / 2; k < patchsize / 2 + 1; k++)
//                     {
//                         for (int l = -patchsize / 2; l < patchsize / 2 + 1; l++)
//                         {
//                             comparison_patch[counter++] = input_image[m + k][n + l];
//                         }
//                     }
//                     /* Here we should implement the f algorithm */
//                     double difference_squared = 0;
//                     for (int a = 0; a < patchsize * patchsize; a++)
//                     {
//                         difference_squared += (pixel_patch[a] - comparison_patch[a]) * (pixel_patch[a] - comparison_patch[a]);
//                     }

//                     //printf("\ndifference_squared = %f \n", difference_squared);

//                     double w_difference_squared = -difference_squared / (filter_sigma * filter_sigma);

//                     //printf("\nw_difference_squared = %f \n", w_difference_squared);

//                     double w = exp(w_difference_squared);
//                     zeta += w;
//                     output_image[i][j] += input_image[i][j] * w;
//                     if (i == 2 && j == 2 && output_image[i][j] > 0)
//                     {
//                         // printf("w: %f output_image[2][2]: %f \t", w, output_image[i][j]);
//                         // printf("zeta: %f\n", zeta);
//                     }
//                 }

//             }

//             //printf("\noutput_image[5][5]: %lf \t", output_image[i][j]);
//             output_image[i][j] = output_image[i][j] / zeta;
//             //printf("2: %lf ", output_image[i][j]);
//             // printf("zeta: %f\n", zeta);
//         }
//     }

//     printf("\nDuring denoising 2\n");
//     for (int i = 0; i < 10; i++)
//     {
//         for (int j = 0; j < width; j++)
//         {
//             printf(" %f ", output_image[i][j]);
//         }
//         printf("\n");
//     }

//     return output_image;
// }

double **non_local_means(double **input_image, int patchsize, double filter_sigma, double patch_sigma, int width, int height)
{
    double **output_image = (double **)malloc(height * sizeof(double *));
    for (int i = 0; i < height; i++)
    {
        output_image[i] = (double *)malloc(width * sizeof(double));
    }

    /* Loop for each pixel that is inside the patchsize limits */
    for (int i = patchsize / 2; i < height - patchsize / 2; i++)
    {
        for (int j = patchsize / 2; j < width - patchsize / 2; j++)
        {
            /* Create the patchsize * patchsize grid with the selected pixel at the centre */
            double **pixel_patch = (double **)malloc(patchsize * sizeof(double *));
            for (int i = 0; i < patchsize; i++)
            {
                pixel_patch[i] = (double *)malloc(patchsize * sizeof(double));
            }
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

            // if (i == 1 && j == 1)
            // {
            //     counter = 0;
            //     printf("\npixel_patch: ");
            //     for (int k = -patchsize / 2; k < patchsize / 2 + 1; k++)
            //     {
            //         for (int l = -patchsize / 2; l < patchsize / 2 + 1; l++)
            //         {
            //             printf(" %f ", pixel_patch[counter++]);
            //         }
            //     }
            // }

            /* Initialize the ouput image value to zero */
            output_image[i][j] = 0;
            double zeta = 0;
            double zeta2 = 0;
            /* Comparison patch (we take into account ourselves too) */
            for (int m = patchsize / 2; m < height - patchsize / 2; m++)
            {
                for (int n = patchsize / 2; n < width - patchsize / 2; n++)
                {
                    /* Create the patchsize * patchsize grid with the selected pixel at the centre */
                    double **comparison_patch = (double **)malloc(patchsize * sizeof(double *));
                    for (int i = 0; i < patchsize; i++)
                    {
                        comparison_patch[i] = (double *)malloc(patchsize * sizeof(double));
                    }
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

                    // if (i == 1 && j == 1)
                    // {
                    //     counter = 0;
                    //     printf("\ncomparison_patch: ");
                    //     for (int k = -patchsize / 2; k < patchsize / 2 + 1; k++)
                    //     {
                    //         for (int l = -patchsize / 2; l < patchsize / 2 + 1; l++)
                    //         {
                    //             printf(" %f ", comparison_patch[counter++]);
                    //         }
                    //         // printf("\n");
                    //     }
                    // }

                    double distX = 0;
                    double distY = 0;
                    double dist = 0;
                    /* Here we should implement the f algorithm */
                    double difference_squared = 0;
                    for (int a = 0; a < patchsize; a++)
                    {
                        for (int b = 0; b < patchsize; b++)
                        {
                            distX = (a - patchsize / 2) * (a - patchsize / 2);
                            distY = (b - patchsize / 2) * (b - patchsize / 2);
                            dist = -(distX + distY) / (patch_sigma * patch_sigma);
                            dist = exp(dist);
                            //zeta2 += dist;

                            if (i == 5 && j == 5 && n == 5 && m == 5)
                            {
                                printf("\ndist = %f ", dist);
                            }

                            difference_squared += dist * (pixel_patch[a][b] - comparison_patch[a][b]) * (pixel_patch[a][b] - comparison_patch[a][b]);
                        }
                    }

                    //difference_squared /= zeta2;

                    // if (i == 1 && j == 1)
                    // {
                    //     printf("\ndifference_squared = %f ", difference_squared);
                    // }

                    double w_difference_squared = -difference_squared / (filter_sigma * filter_sigma);

                    // if (i == 1 && j == 1)
                    // {
                    //     printf("\nw_difference_squared = %f \n", w_difference_squared);
                    // }

                    double w = exp(w_difference_squared);
                    zeta += w;

                    output_image[i][j] += input_image[m][n] * w;

                    // if (i == 1 && j == 1)
                    // {
                    //     printf("w = %f ", w);
                    //     printf("\nzeta = %f", zeta);
                    //     printf("\noutput_image[i][j] = %f \n", output_image[i][j]);
                    // }

                    // if (i == 2 && j == 2 && output_image[i][j] > 0)
                    // {
                    //     // printf("w: %f output_image[2][2]: %f \t", w, output_image[i][j]);
                    //     // printf("zeta: %f\n", zeta);
                    // }
                }
            }

            printf("\noutput_image[5][5]: %lf \t", output_image[i][j]);
            output_image[i][j] = output_image[i][j] / zeta;
            printf("2: %lf ", output_image[i][j]);
            printf("zeta: %f\n", zeta);
        }
    }


    printf("\nDuring denoising 2\n");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf(" %f ", output_image[i][j]);
        }
        printf("\n");
    }

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
    double *normalized_image = (double *)malloc(width * height * sizeof(double));
    double *noisy_image = (double *)malloc(width * height * sizeof(double));
    uint8_t *noisy_image_for_save = (uint8_t *)malloc(width * height * sizeof(uint8_t));

    double **normalized_noisy_2D = (double **)malloc(height * sizeof(double *));
    for (int i = 0; i < height; i++)
    {
        normalized_noisy_2D[i] = (double *)malloc(width * sizeof(double));
    }

    double **normalized_denoised_2D = (double **)malloc(height * sizeof(double *));
    for (int i = 0; i < height; i++)
    {
        normalized_denoised_2D[i] = (double *)malloc(width * sizeof(double));
    }

    uint8_t *denoised_image = (uint8_t *)malloc(width * height * sizeof(uint8_t));

    /* Image Normalization and Noise Addition*/
    for (int i = 0; i < width * height; i++)
    {
        normalized_image[i] = (double)original_image[i] / (double)(MAX_IMAGE_VALUE - MIN_IMAGE_VALUE + 1);
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

    /* Map to 2D */
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            normalized_noisy_2D[i][j] = noisy_image[i * width + j];
        }
    }

    printf("\nBefore denoising\n");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf(" %f ", normalized_noisy_2D[i][j]);
        }
        printf("\n");
    }

    normalized_denoised_2D = non_local_means(normalized_noisy_2D, patchsize, 0.2, 1.67, width, height);

    printf("\nAfter denoising\n");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf(" %f ", normalized_denoised_2D[i][j]);
        }
        printf("\n");
    }

    /* Denormalize and map into 1D the denoised image */
    int counter = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
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