#include <format>
#include <optional>
#include "silly_framework/settings/logger_all.hpp"
#include "silly_framework/facilities/res_loader.hpp"

using namespace silly_framework;

void res_loader::work() {
    while (true) {
        needed.acquire();
        do { // 自旋到uvloop真正被激活为止
            // 检查标志以防卡在这里不能退出
            if (!running) { return; }
            // 如果你发现自旋的CPU占用过高，那就加sleep。古事记是这样记载的。
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } while (!uvloop.alive());
        while (uvloop.run(UV_RUN_ONCE) != 0) {}
    }
}

auto res_loader::load_image(const char *filename) -> coutils::async_fn<std::optional<rewheel::stb_decoded_image>> {
    activate();
    uvpp::fs::request open_req, stat_req, read_req, close_req;
    co_await uvco::fs::open(uvloop, open_req, filename, O_RDONLY, 0);
    if (open_req->result < 0) {
        logger::is()->error("Cannot open image file {}: {}", filename, uv_strerror(open_req->result));
        co_return std::nullopt;
    }
    co_await uvco::fs::fstat(uvloop, stat_req, open_req->result);
    if (stat_req->result < 0) {
        logger::is()->error("Cannot stat image file {}: {}", filename, uv_strerror(stat_req->result));
        co_return std::nullopt;
    }
    std::vector<uint8_t> file_buf(stat_req->statbuf.st_size);
    uvpp::buf_view file_iov((char*)file_buf.data(), file_buf.size());
    co_await uvco::fs::read(uvloop, read_req, open_req->result, &file_iov, 1, -1);
    if (read_req->result < 0) {
        logger::is()->error("Cannot read image file {}: {}", filename, uv_strerror(read_req->result));
        co_return std::nullopt;
    }
    co_await uvco::fs::close(uvloop, close_req, open_req->result);
    auto result = std::make_optional<rewheel::stb_decoded_image>(file_buf.data(), file_buf.size());
    if (result->n_channels <= 0 || result->n_channels > 4) {
        logger::is()->error("Image file {} have abnormal n_channels({})!", filename, result->n_channels);
        co_return std::nullopt;
    }
    co_return result;
}
