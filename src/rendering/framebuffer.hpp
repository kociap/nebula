#pragma once

#include <anton/expected.hpp>
#include <anton/fixed_array.hpp>

#include <core/error.hpp>
#include <core/types.hpp>
#include <rendering/opengl.hpp>

#include <rendering/opengl_defs.hpp>

namespace nebula::rendering {
  class Framebuffer {
  public:
    static constexpr u64 max_color_attachments = opengl::min_color_attachments;

    using Internal_Format = opengl::Sized_Internal_Format;

    enum class Bind_Mode {
      read = GL_READ_FRAMEBUFFER,
      draw = GL_DRAW_FRAMEBUFFER,
      read_draw = GL_FRAMEBUFFER,
    };

    static constexpr Bind_Mode read = Bind_Mode::read;
    static constexpr Bind_Mode draw = Bind_Mode::draw;
    static constexpr Bind_Mode read_draw = Bind_Mode::read_draw;

    enum class Buffer_Type {
      renderbuffer,
      texture,
    };

    struct Color_Buffer_Info {
      Internal_Format internal_format = Internal_Format::rgb8;
      Buffer_Type buffer_type = Buffer_Type::texture;
    };

    struct Depth_Buffer_Info {
      Internal_Format internal_format = Internal_Format::depth_component24;
      Buffer_Type buffer_type = Buffer_Type::renderbuffer;
      bool enabled = false;
    };

    struct Stencil_Buffer_Info {
      Internal_Format internal_format = Internal_Format::stencil_index8;
      Buffer_Type buffer_type = Buffer_Type::renderbuffer;
      bool enabled = false;
    };

    struct Construct_Info {
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

    [[nodiscard]] Expected<void, Error> create_framebuffer(Construct_Info info);
    void delete_framebuffer();

    [[nodiscard]] bool created() const;

    void bind(Bind_Mode bind_mode = read_draw) const;
    [[nodiscard]] Expected<void, Error> resize(i32 width, i32 height);

    [[nodiscard]] math::Vec2 size() const;
    [[nodiscard]] u32 get_color_texture(i32 index) const;
    [[nodiscard]] Slice<u32 const> get_color_textures() const;
    [[nodiscard]] u32 get_depth_texture() const;
    [[nodiscard]] u32 get_framebuffer_gl_handle() const;
    [[nodiscard]] Construct_Info get_construct_info() const;

    friend void swap(Framebuffer& lhs, Framebuffer& rhs);

  private:
    Construct_Info info;
    Fixed_Array<u32, max_color_attachments> color_buffers;
    i32 active_color_buffers = 0;
    u32 framebuffer = 0;
    u32 depth_buffer = 0;
    u32 stencil_buffer = 0;
  };

  void blit_framebuffer(Framebuffer& dest, Framebuffer const& source,
                        opengl::Buffer_Mask);
  void blit_framebuffer(Framebuffer* dest, Framebuffer const* source,
                        opengl::Buffer_Mask);

  void bind_default_framebuffer(
    Framebuffer::Bind_Mode bind_mode = Framebuffer::read_draw);
} // namespace nebula::rendering

#include <rendering/opengl_undefs.hpp>
