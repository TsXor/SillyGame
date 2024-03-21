#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "image_loader.hpp"


stb_decoded_image::stb_decoded_image(): stb_decoded_image_view{0, 0, 0, nullptr} {}

stb_decoded_image::stb_decoded_image(const byte* raw_data, size_t raw_data_len) {
    data = stbi_load_from_memory(raw_data, raw_data_len, &width, &height, &n_channels, 0);
}

stb_decoded_image::~stb_decoded_image() {
    if (data) { stbi_image_free(data); }
}

stb_decoded_image::stb_decoded_image(stb_decoded_image&& other):
stb_decoded_image_view{other.height, other.width, other.n_channels, other.data} {
    other.height = 0; other.width = 0; other.n_channels = 0; other.data = nullptr;
}

stb_decoded_image& stb_decoded_image::operator=(stb_decoded_image&& other) {
    if (data) { stbi_image_free(data); }
    height = other.height;  width = other.width; n_channels = other.n_channels; data = other.data;
    other.height = 0; other.width = 0; other.n_channels = 0; other.data = nullptr;
    return *this;
}
