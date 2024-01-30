#pragma once

#include <rendering/opengl_defs.hpp>

#include <core/types.hpp>
namespace nebula::opengl {
  // OpenGL 4.5 Core required constants.
  /**
   * @brief Minimum required number of color attachments for OpenGL 4.5 Core.
   */
  constexpr i32 min_color_attachments = 8;
  /**
   * @brief Minimum required number of textures per stage for OpenGL 4.5 Core.
   */
  constexpr i32 min_textures_per_stage = 16;

  /**
   * @brief Enumeration of sized internal formats for OpenGL textures.
   */
  enum class Sized_Internal_Format : u32 {
    // unsigned normalized
    r8 = GL_R8,
    r16 = GL_R16,
    rg8 = GL_RG8,
    rg16 = GL_RG16,
    rgb4 = GL_RGB4,
    rgb8 = GL_RGB8,
    rgb16 = GL_RGB16,
    rgba2 = GL_RGBA2,
    rgba4 = GL_RGBA4,
    rgba8 = GL_RGBA8,
    rgba16 = GL_RGBA16,
    // signed normalized
    r8_snorm = GL_R8_SNORM,
    r16_snorm = GL_R16_SNORM,
    rg8_snorm = GL_RG8_SNORM,
    rg16_snorm = GL_RG16_SNORM,
    rgb8_snorm = GL_RGB8_SNORM,
    rgb16_snorm = GL_RGB16_SNORM,
    rgba16_snorm = GL_RGBA16_SNORM,
    // float
    r16f = GL_R16F,
    rg16f = GL_RG16F,
    rgb16f = GL_RGB16F,
    rgba16f = GL_RGBA16F,
    r32f = GL_R32F,
    rg32f = GL_RG32F,
    rgb32f = GL_RGB32F,
    rgba32f = GL_RGBA32F,
    // signed integer
    r8i = GL_R8I,
    r16i = GL_R16I,
    r32i = GL_R32I,
    rg8i = GL_RG8I,
    rg16i = GL_RG16I,
    rg32i = GL_RG32I,
    rgb8i = GL_RGB8I,
    rgb16i = GL_RGB16I,
    rgb32i = GL_RGB32I,
    rgba16i = GL_RGBA16I,
    rgba32i = GL_RGBA32I,
    // unsigned integer
    r8ui = GL_R8UI,
    r16ui = GL_R16UI,
    r32ui = GL_R32UI,
    rg8ui = GL_RG8UI,
    rg16ui = GL_RG16UI,
    rg32ui = GL_RG32UI,
    rgb8ui = GL_RGB8UI,
    rgb16ui = GL_RGB16UI,
    rgb32ui = GL_RGB32UI,
    rgba16ui = GL_RGBA16UI,
    rgba32ui = GL_RGBA32UI,
    // SRGB
    srgb8 = GL_SRGB8,
    srgb8_alpha8 = GL_SRGB8_ALPHA8,
    // Depth and Stencil
    depth_component16 = GL_DEPTH_COMPONENT16,
    depth_component24 = GL_DEPTH_COMPONENT24,
    depth_component32 = GL_DEPTH_COMPONENT32, // Not required
    depth_component32f = GL_DEPTH_COMPONENT32F,
    depth24_stencil8 = GL_DEPTH24_STENCIL8,
    depth32f_stencil8 = GL_DEPTH32F_STENCIL8,
    stencil_index1 = GL_STENCIL_INDEX1, // Not required
    stencil_index4 = GL_STENCIL_INDEX4, // Not required
    stencil_index8 = GL_STENCIL_INDEX8,
    stencil_index16 = GL_STENCIL_INDEX16, // Not required
  };

  /**
   * @brief Enumeration of buffer masks for framebuffer operations.
   */
  enum class Buffer_Mask : u32 {
    color_buffer_bit = GL_COLOR_BUFFER_BIT,
    depth_buffer_bit = GL_DEPTH_BUFFER_BIT,
    stencil_buffer_bit = GL_STENCIL_BUFFER_BIT,
  };

  /**
   * @brief Buffer mask constant for color buffer operations.
   */
  constexpr Buffer_Mask color_buffer_bit = Buffer_Mask::color_buffer_bit;
  /**
   * @brief Buffer mask constant for depth buffer operations.
   */
  constexpr Buffer_Mask depth_buffer_bit = Buffer_Mask::depth_buffer_bit;
  /**
   * @brief Buffer mask constant for stencil buffer operations.
   */
  constexpr Buffer_Mask stencil_buffer_bit = Buffer_Mask::stencil_buffer_bit;

  [[nodiscard]] i32 get_max_texture_image_units();
  [[nodiscard]] i32 get_max_combined_texture_units();
  [[nodiscard]] i32 get_max_renderbuffer_size();
  [[nodiscard]] i32 get_max_color_attachments();
  [[nodiscard]] i32 get_max_draw_buffers();
  [[nodiscard]] i32 get_uniform_buffer_offset_alignment();
  [[nodiscard]] i32 get_min_map_buffer_alignment();

  void install_debug_callback();

  void load_constants();
} // namespace nebula::opengl

#include <rendering/opengl_undefs.hpp>
