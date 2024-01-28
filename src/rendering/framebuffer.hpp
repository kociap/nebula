#pragma once

#include <anton/expected.hpp>
#include <anton/fixed_array.hpp>

#include <core/error.hpp>
#include <core/types.hpp>
#include <rendering/opengl.hpp>

#include <rendering/opengl_defs.hpp>

namespace nebula::rendering {
  /**
   * @brief Represents a framebuffer object for rendering.
   *
   * The Framebuffer class encapsulates the functionality related to framebuffers,
   * including color attachments, depth, and stencil buffers. It provides methods
   * for creating, resizing, and managing the framebuffer.
   */
  class Framebuffer {
  public:
    /**
     * @brief Maximum number of color attachments supported by the framebuffer.
     */
    static constexpr u64 max_color_attachments = opengl::min_color_attachments;
    /**
     * @brief Internal format used in the framebuffer.
     */
    using Internal_Format = opengl::Sized_Internal_Format;

    /**
     * @brief Enumeration for framebuffer bind modes.
     */
    enum class Bind_Mode {
      read = GL_READ_FRAMEBUFFER,
      draw = GL_DRAW_FRAMEBUFFER,
      read_draw = GL_FRAMEBUFFER,
    };

    static constexpr Bind_Mode read = Bind_Mode::read;
    static constexpr Bind_Mode draw = Bind_Mode::draw;
    static constexpr Bind_Mode read_draw = Bind_Mode::read_draw;

    /**
     * @brief Enumeration for buffer types.
     */
    enum class Buffer_Type {
      renderbuffer,
      texture,
    };

    /**
     * @brief Structure containing information about color buffers.
     */
    struct Color_Buffer_Info {
      Internal_Format internal_format = Internal_Format::rgb8;
      Buffer_Type buffer_type = Buffer_Type::texture;
    };

    /**
     * @brief Structure containing information about the depth buffer.
     */
    struct Depth_Buffer_Info {
      Internal_Format internal_format = Internal_Format::depth_component24;
      Buffer_Type buffer_type = Buffer_Type::renderbuffer;
      bool enabled = false;
    };

    /**
     * @brief Structure containing information about the stencil buffer.
     */
    struct Stencil_Buffer_Info {
      Internal_Format internal_format = Internal_Format::stencil_index8;
      Buffer_Type buffer_type = Buffer_Type::renderbuffer;
      bool enabled = false;
    };

    /**
     * @brief Structure containing information for constructing the framebuffer.
     */
    struct Construct_Info {
      String_View name;
      Fixed_Array<Color_Buffer_Info, max_color_attachments> color_buffers;
      Depth_Buffer_Info depth_buffer;
      Stencil_Buffer_Info stencil_buffer;
      i32 width = 0;
      i32 height = 0;
      i32 samples = 0;
      bool multisampled = false;
    };

    Framebuffer() = default;
    Framebuffer(Framebuffer&&);
    Framebuffer& operator=(Framebuffer&&);
    ~Framebuffer();

    Framebuffer(Framebuffer const&) = delete;
    Framebuffer& operator=(Framebuffer const&) = delete;

    /**
     * @brief Creates the framebuffer based on the provided construction information.
     *
     * @param info - Construction information for the framebuffer.
     * @return Expected<void, Error> indicating success or failure.
     */
    [[nodiscard]] Expected<void, Error> create_framebuffer(Construct_Info info);

    /**
     * @brief Deletes the framebuffer and associated resources.
     */
    void delete_framebuffer();

    /**
     * @brief Checks if the framebuffer has been created.
     *
     * @return True if the framebuffer has been created; false otherwise.
     */
    [[nodiscard]] bool created() const;

    /**
     * @brief Checks if the framebuffer has been created.
     *
     * @return True if the framebuffer has been created; false otherwise.
     */
    void bind(Bind_Mode bind_mode = read_draw) const;

    /**
     * @brief Resizes the framebuffer to the specified width and height.
     *
     * @param width - New width of the framebuffer.
     * @param height - New height of the framebuffer.
     * @return Expected<void, Error> indicating success or failure.
     */
    [[nodiscard]] Expected<void, Error> resize(i32 width, i32 height);

    /**
     * @brief Resizes the framebuffer to the specified width and height.
     *
     * @param width - New width of the framebuffer.
     * @param height - New height of the framebuffer.
     * @return Expected<void, Error> indicating success or failure.
     */
    [[nodiscard]] math::Vec2 size() const;

    /**
     * @brief Gets the OpenGL handle of the color texture at the specified index.
     *
     * @param index - Index of the color texture.
     * @return OpenGL handle of the color texture.
     */
    [[nodiscard]] u32 get_color_texture(i32 index) const;

    /**
     * @brief Gets a slice of OpenGL handles for all color textures attached to the framebuffer.
     *
     * @return Slice of OpenGL handles for color textures.
     */
    [[nodiscard]] Slice<u32 const> get_color_textures() const;

    /**
     * @brief Gets the OpenGL handle of the depth texture.
     *
     * @return OpenGL handle of the depth texture.
     */
    [[nodiscard]] u32 get_depth_texture() const;

    /**
     * @brief Gets the OpenGL handle of the framebuffer.
     *
     * @return OpenGL handle of the framebuffer.
     */
    [[nodiscard]] u32 get_framebuffer_gl_handle() const;

    /**
     * @brief Gets the construction information used to create the framebuffer.
     *
     * @return Construct_Info containing the construction information.
     */
    [[nodiscard]] Construct_Info get_construct_info() const;

    /**
     * @brief Friend function to swap two framebuffer objects.
     *
     * @param lhs - Left-hand side framebuffer.
     * @param rhs - Right-hand side framebuffer.
     */
    friend void swap(Framebuffer& lhs, Framebuffer& rhs);

  private:
    Construct_Info info;
    Fixed_Array<u32, max_color_attachments> color_buffers;
    i32 active_color_buffers = 0;
    u32 framebuffer = 0;
    u32 depth_buffer = 0;
    u32 stencil_buffer = 0;
  };

  /**
   * @brief Blits the contents of the source framebuffer to the destination framebuffer.
   *
   * This function performs a framebuffer blit operation, copying the contents of the
   * source framebuffer to the destination framebuffer. The specified buffer mask determines
   * which buffers are included in the blit operation.
   *
   * @param dest - Reference to the destination framebuffer.
   * @param source - Reference to the source framebuffer.
   * @param buffer_mask - Buffer mask indicating which buffers to include in the blit operation.
   */
  void blit_framebuffer(Framebuffer& dest, Framebuffer const& source,
                        opengl::Buffer_Mask);
  /**
   * @brief Blits the contents of the source framebuffer to the destination framebuffer.
   *
   * This function performs a framebuffer blit operation, copying the contents of the
   * source framebuffer to the destination framebuffer. The specified buffer mask determines
   * which buffers are included in the blit operation.
   *
   * @param dest - Pointer to the destination framebuffer.
   * @param source - Pointer to the source framebuffer.
   * @param buffer_mask - Buffer mask indicating which buffers to include in the blit operation.
   */
  void blit_framebuffer(Framebuffer* dest, Framebuffer const* source,
                        opengl::Buffer_Mask);

  /**
   * @brief Blits the contents of the source framebuffer to the destination framebuffer.
   *
   * This function performs a framebuffer blit operation, copying the contents of the
   * source framebuffer to the destination framebuffer. The specified buffer mask determines
   * which buffers are included in the blit operation.
   *
   * @param dest - Pointer to the destination framebuffer.
   * @param source - Pointer to the source framebuffer.
   * @param buffer_mask - Buffer mask indicating which buffers to include in the blit operation.
   */
  void bind_default_framebuffer(
    Framebuffer::Bind_Mode bind_mode = Framebuffer::read_draw);
} // namespace nebula::rendering

#include <rendering/opengl_undefs.hpp>
