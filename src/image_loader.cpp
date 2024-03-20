#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "image_loader.hpp"


stb_decoded_image::stb_decoded_image(const byte* data_start, const byte* data_end) {
    data = stbi_load_from_memory(data_start, data_end - data_start, &width, &height, &n_channels, 0);
}

stb_decoded_image::~stb_decoded_image() {
    if (data) { stbi_image_free(data); }
}

stb_decoded_image::stb_decoded_image(stb_decoded_image&& other):
stb_decoded_image_view{other.height, other.width, other.n_channels, other.data} {
    other.data = nullptr;
}
