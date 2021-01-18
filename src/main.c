#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main() {
    int width, height, bpp;

    uint8_t* original_image = stbi_load("../images/boat.jpg", &width, &height, &bpp, 3);

    printf("%d \n", original_image[0]);

    stbi_image_free(original_image);

    return 0;
}