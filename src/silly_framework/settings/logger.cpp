#include <unordered_map>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "silly_framework/settings/logger_all.hpp"
#include "silly_framework/rewheel/os_misc.hpp"

using namespace silly_framework::os_misc;
using namespace silly_framework::logger;
namespace impl = silly_framework::logger;

using logger_ptr = std::shared_ptr<spdlog::logger>;
static logger_ptr logger_obj;
static logger_ptr backup_logger = spdlog::stdout_color_mt(default_name());

size_t impl::lib_identity() {
    static int marker = 0;
    return reinterpret_cast<size_t>(&marker);
}
const std::string& impl::default_name() {
    static constexpr const char* default_name_format = "SillyFramework(proc={}, ident={})";
    static const std::string name = std::format(default_name_format, lib_identity(), proc_id());
    return name;
}

std::shared_ptr<spdlog::logger> impl::is() { return logger_obj ? logger_obj : backup_logger; }
void impl::is(std::shared_ptr<spdlog::logger> logger) { logger_obj = logger; }

const std::string& impl::name() { return is()->name(); }
void impl::make(const std::string& logger_name, const std::string& filename) {
    is(spdlog::basic_logger_mt(logger_name, filename, true));
}
