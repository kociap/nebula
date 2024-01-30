#pragma once

#include <anton/expected.hpp>
#include <anton/slice.hpp>

#include <core/error.hpp>
#include <core/types.hpp>
#include <rendering/framebuffer.hpp>
#include <rendering/vertex.hpp>

namespace nebula::rendering {
  /**
   * @brief Represents a draw command for indexed rendering.
   */
  struct Draw_Elements_Command {
    // Number of indices.
    u32 count;
    u32 instance_count;
    // Index of the first index to use.
    u32 first_index;
    // Offset to the first vertex.
    u32 base_vertex;
    // Instance ID of the first instance.
    u32 base_instance;
  };

  /**
   * @brief Represents a draw command for persistent geometry.
   */
  struct Draw_Persistent_Geometry_Command {
    u32 handle;
    u32 instance_count;
    // Instance ID of the first instance.
    u32 base_instance;
  };

  /**
   * @brief Represents a buffer with head and end pointers for managing data.
   *
   * The Buffer structure is used to manage generic buffers with head and end
   * pointers. It is templated on type T.
   */
  template<typename T>
  struct Buffer {
    T* buffer;
    T* head;
    T* end;
  };

  /**
   * @brief Initializes the rendering system with the specified width and
   * height.
   *
   * This function initializes the rendering system, including framebuffers,
   * buffers, and other necessary components, with the specified width and
   * height.
   *
   * @param width - Width of the rendering system.
   * @param height - Height of the rendering system.
   * @return Expected<void, Error> indicating success or failure.
   */
  Expected<void, Error> initialise(i64 width, i64 height);

  /**
   * @brief Tears down the rendering system, releasing allocated resources.
   *
   * This function tears down the rendering system, releasing allocated
   * resources, including framebuffers, buffers, and other components.
   */
  void teardown();

  /**
   * @brief Retrieves a pointer to the primary framebuffer.
   *
   * @return Pointer to the primary framebuffer.
   */
  [[nodiscard]] Framebuffer* get_primary_framebuffer();
  /**
   * @brief Retrieves a pointer to the front postprocess framebuffer.
   *
   * @return Pointer to the front postprocess framebuffer.
   */
  [[nodiscard]] Framebuffer* get_front_postprocess_framebuffer();
  /**
   * @brief Retrieves a pointer to the back postprocess framebuffer.
   *
   * @return Pointer to the back postprocess framebuffer.
   */
  [[nodiscard]] Framebuffer* get_back_postprocess_framebuffer();
  /**
   * @brief Swaps the front and back postprocess framebuffers.
   *
   * This function swaps the front and back postprocess framebuffers, useful for
   * double buffering.
   */
  void swap_postprocess_framebuffers();
  /**
   * @brief Resizes the framebuffers to the specified width and height.
   *
   * This function resizes the framebuffers to the specified width and height,
   * adjusting their internal structures accordingly.
   *
   * @param width - New width of the framebuffers.
   * @param height - New height of the framebuffers.
   */
  void resize_framebuffers(i64 width, i64 height);

  /**
   * @brief Binding number for the buffer containing vertex (geometry) data.
   */
  constexpr u32 vertex_buffer_binding = 1;

  /**
   * @brief Binds the draw buffers for rendering.
   *
   * This function binds the necessary draw buffers for rendering, including
   * vertex buffers.
   */
  void bind_draw_buffers();
  /**
   * @brief Binds transient geometry buffers for rendering.
   *
   * This function binds transient geometry buffers for rendering. These buffers
   * are typically used for temporary or short-lived geometry data.
   */
  void bind_transient_geometry_buffers();

  /**
   * @brief Writes indexed vertex data (geometry) to GPU buffers.
   *
   * The underlying storage works as a ring buffer, meaning that geometry will
   * eventually be overwritten. The function returns a Draw_Elements_Command
   * structure representing the draw command for the written geometry.
   *
   * @param vertices - Slice of constant Vertex data representing the vertices.
   * @param indices - Slice of constant u32 data representing the indices.
   * @return Draw_Elements_Command representing the draw command for the written
   * geometry.
   */
  [[nodiscard]] Draw_Elements_Command write_geometry(Slice<Vertex const>,
                                                     Slice<u32 const>);

  /**
   * @brief Adds a draw command for indexed rendering to the rendering queue.
   *
   * This function adds a Draw_Elements_Command representing indexed rendering
   * to the rendering queue.
   *
   * @param command - Draw_Elements_Command representing the draw command for
   * indexed rendering.
   */
  void add_draw_command(Draw_Elements_Command);
  /**
   * @brief Adds a draw command for persistent geometry to the rendering queue.
   *
   * This function adds a Draw_Persistent_Geometry_Command representing
   * persistent geometry rendering to the rendering queue.
   *
   * @param command - Draw_Persistent_Geometry_Command representing the draw
   * command for persistent geometry.
   */
  void add_draw_command(Draw_Persistent_Geometry_Command);
  /**
   * @brief Commits the accumulated draw commands in the rendering queue for
   * execution.
   *
   * This function commits the accumulated draw commands in the rendering queue
   * for execution. After calling this function, the draw commands are
   * processed, and the rendering state is updated.
   */
  void commit_draw();
} // namespace nebula::rendering
