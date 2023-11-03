#pragma once

#include <anton/format.hpp>

#include <core/types.hpp>

namespace nebula::detail {
  void log_info(anton::String_View msg, anton::String_View file, i64 line);
  void log_warning(anton::String_View msg, anton::String_View file, i64 line);
  void log_error(anton::String_View msg, anton::String_View file, i64 line);
  void log_fatal(anton::String_View msg, anton::String_View file, i64 line);
} // namespace nebula::detail

#define LOG_INFO(msg, ...)                                                    \
  ::nebula::detail::log_info(::anton::format(msg __VA_OPT__(, ) __VA_ARGS__), \
                             __FILE__, __LINE__)
#define LOG_WARNING(msg, ...)    \
  ::nebula::detail::log_warning( \
    ::anton::format(msg __VA_OPT__(, ) __VA_ARGS__), __FILE__, __LINE__)

#define LOG_ERROR(msg, ...)                                                    \
  ::nebula::detail::log_error(::anton::format(msg __VA_OPT__(, ) __VA_ARGS__), \
                              __FILE__, __LINE__)

#define LOG_FATAL(msg, ...)                                                    \
  ::nebula::detail::log_fatal(::anton::format(msg __VA_OPT__(, ) __VA_ARGS__), \
                              __FILE__, __LINE__)
