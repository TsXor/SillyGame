#include <format>
#include <spdlog/spdlog.h>
#include "res_loader.hpp"


uvco::coro_fn<void> load_image(uv_loop_t* loop, const char *filename, stb_decoded_image& out_image) {
    uvpp::fs::request open_req, stat_req, read_req, close_req;
    co_await uvco::fs::open(loop, open_req, filename, O_RDONLY, 0);
    if (open_req->result < 0) {
        spdlog::error("Cannot open image file {}: {}", filename, uv_strerror(open_req->result));
        co_return;
    }
    co_await uvco::fs::fstat(loop, stat_req, open_req->result);
    if (stat_req->result < 0) {
        spdlog::error("Cannot stat image file {}: {}", filename, uv_strerror(stat_req->result));
        co_return;
    }
    std::vector<uint8_t> file_buf(stat_req->statbuf.st_size);
    uvpp::buf_view file_iov((char*)file_buf.data(), file_buf.size());
    co_await uvco::fs::read(loop, read_req, open_req->result, &file_iov, 1, -1);
    if (read_req->result < 0) {
        spdlog::error("Cannot read image file {}: {}", filename, uv_strerror(read_req->result));
        co_return;
    }
    out_image = stb_decoded_image(file_buf.data(), file_buf.size());
}
