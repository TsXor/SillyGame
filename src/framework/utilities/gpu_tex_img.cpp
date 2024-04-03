#include "gpu_tex_img.hpp"


gl::Texture2D load_texture2d_from_stb(const stb_decoded_image_view& img) {
    gl::Texture2D texture;
    
    gl::enums::PixelDataInternalFormat internal_format;
    gl::enums::PixelDataFormat format;
    switch (img.n_channels) {
        case 1:
            internal_format = gl::kR8;
            format = gl::kRed;
            break;
        case 2:
            internal_format = gl::kRg8;
            format = gl::kRg;
            break;
        case 3:
            internal_format = gl::kSrgb8;
            format = gl::kRgb;
            break;
        case 4:
            internal_format = gl::kSrgb8Alpha8;
            format = gl::kRgba;
            break;
        // 不应该发生，因为加载图片时已验证
        default: return texture;
    }
    
    gl::Bind(texture);
    texture.upload(internal_format, img.width, img.height, format, gl::kUnsignedByte, img.data);
    // 默认使用最近邻缩放
    texture.minFilter(gl::kNearest); texture.magFilter(gl::kNearest);
    gl::Unbind(texture);
    return texture;
}

gpu_tex2d::gpu_tex2d(const stb_decoded_image_view& cpu_img):
width(cpu_img.width), height(cpu_img.height), n_channels(cpu_img.n_channels),
tex(load_texture2d_from_stb(cpu_img)) {}

gpu_tex2d::~gpu_tex2d() {}
