#pragma once

#include <types.hpp>

namespace nebula {
  enum struct Node_Kind : u8;
  struct Node;
  struct IO_Connector;
  struct Bus;
  struct Junction;
  struct Bus_Splitter;
  struct Bus_Combiner;
  enum struct Gate_Kind : u8;
  struct Gate;
  struct Component;
} // namespace nebula
