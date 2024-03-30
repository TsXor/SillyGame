#include <format>
#include <optional>
#include <spdlog/spdlog.h>
#include "res_loader.hpp"


void res_loader::work() {
    while (true) {
        needed.acquire();
        // 自旋到uvloop真正被激活为止
        do { if (!running) { return; } } while (!uvloop.alive());
        while (uvloop.run(UV_RUN_ONCE) != 0) {}
    }
}

uvco::coro_fn<std::optional<stb_decoded_image>> res_loader::load_image(const char *filename) {
    activate();
    uvpp::fs::request open_req, stat_req, read_req, close_req;
    co_await uvco::fs::open(uvloop, open_req, filename, O_RDONLY, 0);
    if (open_req->result < 0) {
        spdlog::error("Cannot open image file {}: {}", filename, uv_strerror(open_req->result));
        co_return std::nullopt;
    }
    co_await uvco::fs::fstat(uvloop, stat_req, open_req->result);
    if (stat_req->result < 0) {
        spdlog::error("Cannot stat image file {}: {}", filename, uv_strerror(stat_req->result));
        co_return std::nullopt;
    }
    std::vector<uint8_t> file_buf(stat_req->statbuf.st_size);
    uvpp::buf_view file_iov((char*)file_buf.data(), file_buf.size());
    co_await uvco::fs::read(uvloop, read_req, open_req->result, &file_iov, 1, -1);
    if (read_req->result < 0) {
        spdlog::error("Cannot read image file {}: {}", filename, uv_strerror(read_req->result));
        co_return std::nullopt;
    }
    auto result = std::make_optional<stb_decoded_image>(file_buf.data(), file_buf.size());
    if (result->n_channels <= 0 || result->n_channels > 4) {
        spdlog::error("Image file {} have abnormal n_channels({})!", filename, result->n_channels);
        co_return std::nullopt;
    }
    co_return result;
}
