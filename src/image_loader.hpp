#pragma once
#ifndef __STB_IMAGE_LOADER__
#define __STB_IMAGE_LOADER__

struct stb_decoded_image_view {
    using byte = unsigned char;
    int width, height, n_channels; byte* data;
};

struct stb_decoded_image : stb_decoded_image_view {
    stb_decoded_image();
    stb_decoded_image(const byte* data, size_t data_len);
    ~stb_decoded_image();
    stb_decoded_image(const stb_decoded_image&) = delete;
    stb_decoded_image& operator=(const stb_decoded_image&) = delete;
    stb_decoded_image(stb_decoded_image&& other);
    stb_decoded_image& operator=(stb_decoded_image&& other);
    bool empty() { return data == nullptr; }
};

#endif // _IMAGE_LOADER__
