#include <logging/logging.hpp>

#include <anton/stdio.hpp>

namespace nebula::detail {
  void log_debug(anton::String_View msg, anton::String_View file, i64 line)
  {
    anton::print(anton::format("[DEBUG] {}:{}: {}\n", file, line, msg));
  }

  void log_info(anton::String_View msg, anton::String_View file, i64 line)
  {
    anton::print(anton::format("[INFO] {}:{}: {}\n", file, line, msg));
  }

  void log_warning(anton::String_View msg, anton::String_View file, i64 line)
  {
    anton::print(anton::format("[WARN] {}:{}: {}\n", file, line, msg));
  }

  void log_error(anton::String_View msg, anton::String_View file, i64 line)
  {
    anton::print(anton::format("[ERROR] {}:{}: {}\n", file, line, msg));
  }

  void log_fatal(anton::String_View msg, anton::String_View file, i64 line)
  {
    anton::print(anton::format("[FATAL] {}:{}: {}\n", file, line, msg));
  }
} // namespace nebula::detail
