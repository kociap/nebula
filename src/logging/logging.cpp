#include <logging/logging.hpp>

#include <anton/console.hpp>

namespace nebula::detail {
  void log_info(anton::String_View msg, anton::String_View file, i64 line)
  {
    STDOUT_Stream stdout;
    stdout.write(anton::format("[INFO] {}:{}: {}\n", file, line, msg));
  }

  void log_warning(anton::String_View msg, anton::String_View file, i64 line)
  {
    STDOUT_Stream stdout;
    stdout.write(anton::format("[WARN] {}:{}: {}\n", file, line, msg));
  }

  void log_error(anton::String_View msg, anton::String_View file, i64 line)
  {
    STDOUT_Stream stdout;
    stdout.write(anton::format("[ERROR] {}:{}: {}\n", file, line, msg));
  }

  void log_fatal(anton::String_View msg, anton::String_View file, i64 line)
  {
    STDOUT_Stream stdout;
    stdout.write(anton::format("[FATAL] {}:{}: {}\n", file, line, msg));
  }
} // namespace nebula::detail
