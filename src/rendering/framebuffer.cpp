#include <rendering/framebuffer.hpp>

#include <glad/glad.h>

#include <anton/type_traits/properties.hpp>

#include <logging/logging.hpp>
#include <rendering/opengl.hpp>

#define ENUM_TO_VALUE(v) static_cast<underlying_type<decltype(v)>>(v)

namespace nebula::rendering {
  Expected<void, Error>
  Framebuffer::create_framebuffer(Construct_Info const construct_info)
  {
    ANTON_VERIFY(construct_info.width < opengl::get_max_renderbuffer_size() &&
                   construct_info.height < opengl::get_max_renderbuffer_size(),
                 "too big buffer size");
    ANTON_VERIFY(construct_info.width > 0 && construct_info.height > 0,
                 "one or both dimensions are less than or equal 0");
    ANTON_VERIFY(!construct_info.multisampled ||
                   construct_info.multisampled && construct_info.samples > 0,
                 "multisampled framebuffer must have more than 0 samples");

    info = construct_info;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    active_color_buffers = static_cast<i32>(info.color_buffers.size());
    color_buffers.resize(active_color_buffers);

    // TODO: Add support for renderbuffer color attachments.

    // Generate color buffer.
    if(active_color_buffers > 0) {
      if(info.multisampled) {
        glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, active_color_buffers,
                         color_buffers.data());
        for(i32 i = 0; i < active_color_buffers; ++i) {
          GLenum const internal_format =
            ENUM_TO_VALUE(info.color_buffers[i].internal_format);
          glTextureStorage2DMultisample(color_buffers[i], info.samples,
                                        internal_format, info.width,
                                        info.height, GL_TRUE);
          glTextureParameteri(color_buffers[i], GL_TEXTURE_WRAP_S,
                              GL_CLAMP_TO_EDGE);
          glTextureParameteri(color_buffers[i], GL_TEXTURE_WRAP_T,
                              GL_CLAMP_TO_EDGE);
          glFramebufferTexture2D(
            GL_FRAMEBUFFER, static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i),
            GL_TEXTURE_2D_MULTISAMPLE, color_buffers[i], 0);
        }
      } else {
        glCreateTextures(GL_TEXTURE_2D, active_color_buffers,
                         color_buffers.data());
        for(i32 i = 0; i < active_color_buffers; ++i) {
          GLenum const internal_format =
            ENUM_TO_VALUE(info.color_buffers[i].internal_format);
          glTextureStorage2D(color_buffers[i], 1, internal_format, info.width,
                             info.height);
          glTextureParameteri(color_buffers[i], GL_TEXTURE_MIN_FILTER,
                              GL_LINEAR);
          glTextureParameteri(color_buffers[i], GL_TEXTURE_MAG_FILTER,
                              GL_LINEAR);
          glTextureParameteri(color_buffers[i], GL_TEXTURE_WRAP_S,
                              GL_CLAMP_TO_EDGE);
          glTextureParameteri(color_buffers[i], GL_TEXTURE_WRAP_T,
                              GL_CLAMP_TO_EDGE);
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                 GL_TEXTURE_2D, color_buffers[i], 0);
        }
      }

      u32 active_color_attachments[max_color_attachments];
      for(i32 i = 0; i < color_buffers.size(); ++i) {
        active_color_attachments[i] = GL_COLOR_ATTACHMENT0 + i;
      }
      glDrawBuffers(active_color_buffers, active_color_attachments);
      glReadBuffer(GL_COLOR_ATTACHMENT0);
    } else {
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
    }

    Depth_Buffer_Info depth_info = info.depth_buffer;
    if(depth_info.enabled) {
      bool const uses_depth_stencil =
        depth_info.internal_format == Internal_Format::depth24_stencil8 ||
        depth_info.internal_format == Internal_Format::depth32f_stencil8;
      GLenum const depth_buffer_attachment =
        uses_depth_stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
      if(depth_info.buffer_type == Buffer_Type::renderbuffer) {
        glGenRenderbuffers(1, &depth_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
        GLenum const internal_format =
          ENUM_TO_VALUE(depth_info.internal_format);
        if(info.multisampled) {
          glRenderbufferStorageMultisample(GL_RENDERBUFFER, info.samples,
                                           internal_format, info.width,
                                           info.height);
        } else {
          glRenderbufferStorage(GL_RENDERBUFFER, internal_format, info.width,
                                info.height);
        }

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, depth_buffer_attachment,
                                  GL_RENDERBUFFER, depth_buffer);
      } else {
        GLenum const internal_format =
          ENUM_TO_VALUE(depth_info.internal_format);
        if(info.multisampled) {
          glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &depth_buffer);
          glTextureStorage2DMultisample(depth_buffer, info.samples,
                                        internal_format, info.width,
                                        info.height, GL_TRUE);
          glFramebufferTexture2D(GL_FRAMEBUFFER, depth_buffer_attachment,
                                 GL_TEXTURE_2D_MULTISAMPLE, depth_buffer, 0);
        } else {
          glCreateTextures(GL_TEXTURE_2D, 1, &depth_buffer);
          glTextureStorage2D(depth_buffer, 1, internal_format, info.width,
                             info.height);
          glTextureParameteri(depth_buffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          glTextureParameteri(depth_buffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          glTextureParameteri(depth_buffer, GL_TEXTURE_WRAP_S, GL_REPEAT);
          glTextureParameteri(depth_buffer, GL_TEXTURE_WRAP_T, GL_REPEAT);
          glFramebufferTexture2D(GL_FRAMEBUFFER, depth_buffer_attachment,
                                 GL_TEXTURE_2D, depth_buffer, 0);
        }
      }
    }

    // TODO: Implement.
    if(info.stencil_buffer.enabled) {
      //glGenRenderbuffers(1, &stencil_buffer);
      //glBindRenderbuffer(GL_RENDERBUFFER, stencil_buffer);
      //glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
      //glBindRenderbuffer(GL_RENDERBUFFER, 0);
      //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencil_buffer);
    }

    GLenum const le_fremebuffere_statouse =
      glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER);
    if(le_fremebuffere_statouse != GL_FRAMEBUFFER_COMPLETE) {
      String message;
      switch(le_fremebuffere_statouse) {
      case GL_FRAMEBUFFER_UNDEFINED:
        message = "framebuffer undefined"_sv;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        message = "framebuffer incomplete attachment"_sv;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        message = "framebuffer incomplete, missing attachment"_sv;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        message = "framebuffer incomplete, missing draw buffer"_sv;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        message =
          "framebuffer incomplete, read buffer without color attachment"_sv;
        break;
      case GL_FRAMEBUFFER_UNSUPPORTED:
        message =
          "framebuffer unsupported: combination of internal formats of the "
          "attached images violates an implementation-dependent set of "
          "restrictions"_sv;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        message = "framebuffer incomplete multisample"_sv;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        message = "framebuffer incomplete layer targets"_sv;
        break;

      // Zero occurs when an error happens
      case 0:
        message = "framebuffer status check failed"_sv;
        break;

      default:
        ANTON_UNREACHABLE("unhandled framebuffer error status");
      }
      delete_framebuffer();
      return {expected_error, ANTON_MOV(message)};
    }

    return {expected_value};
  }

  void Framebuffer::delete_framebuffer()
  {
    if(!created()) {
      return;
    }

    LOG_INFO("deleting framebuffer '{}'", info.name);
    if(depth_buffer != 0) {
      if(info.depth_buffer.buffer_type == Buffer_Type::renderbuffer) {
        glDeleteRenderbuffers(1, &depth_buffer);
      } else {
        glDeleteTextures(1, &depth_buffer);
      }
      depth_buffer = 0;
    }

    if(stencil_buffer != 0) {
      glDeleteRenderbuffers(1, &stencil_buffer);
      stencil_buffer = 0;
    }

    if(active_color_buffers > 0) {
      glDeleteTextures(active_color_buffers, color_buffers.data());
      active_color_buffers = 0;
    }

    glDeleteFramebuffers(1, &framebuffer);
    framebuffer = 0;
  }

  Framebuffer::Framebuffer(Framebuffer&& fbo)
  {
    info = fbo.info;
    swap(framebuffer, fbo.framebuffer);
    swap(depth_buffer, fbo.depth_buffer);
    swap(stencil_buffer, fbo.stencil_buffer);
  }

  Framebuffer& Framebuffer::operator=(Framebuffer&& fbo)
  {
    info = fbo.info;
    swap(framebuffer, fbo.framebuffer);
    swap(depth_buffer, fbo.depth_buffer);
    swap(stencil_buffer, fbo.stencil_buffer);
    return *this;
  }

  Framebuffer::~Framebuffer()
  {
    delete_framebuffer();
  }

  bool Framebuffer::created() const
  {
    return framebuffer != 0;
  }

  void Framebuffer::bind(Bind_Mode const bind_mode) const
  {
    ANTON_ASSERT(created(),
                 "binding framebuffer that has not been initialised");
    glBindFramebuffer(ENUM_TO_VALUE(bind_mode), framebuffer);
  }

  Expected<void, Error> Framebuffer::resize(i32 const width, i32 const height)
  {
    if(width != info.width || height != info.height) {
      delete_framebuffer();
      info.width = width;
      info.height = height;
      return create_framebuffer(info);
    }

    return {expected_value};
  }

  math::Vec2 Framebuffer::size() const
  {
    return {(f32)info.width, (f32)info.height};
  }

  u32 Framebuffer::get_color_texture(i32 const index) const
  {
    ANTON_ASSERT(index >= 0 && index < active_color_buffers,
                 "indexing unbound color buffer");
    return color_buffers[index];
  }

  Slice<u32 const> Framebuffer::get_color_textures() const
  {
    return color_buffers;
  }

  u32 Framebuffer::get_depth_texture() const
  {
    ANTON_ASSERT(info.depth_buffer.enabled, "depth buffer not present");
    ANTON_ASSERT(info.depth_buffer.buffer_type == Buffer_Type::texture,
                 "depth buffer is not a texture");
    return depth_buffer;
  }

  u32 Framebuffer::get_framebuffer_gl_handle() const
  {
    return framebuffer;
  }

  Framebuffer::Construct_Info Framebuffer::get_construct_info() const
  {
    return info;
  }

  void swap(Framebuffer& lhs, Framebuffer& rhs)
  {
    using anton::swap;
    swap(lhs.info, rhs.info);
    swap(lhs.color_buffers, rhs.color_buffers);
    swap(lhs.active_color_buffers, rhs.active_color_buffers);
    swap(lhs.framebuffer, rhs.framebuffer);
    swap(lhs.depth_buffer, rhs.depth_buffer);
    swap(lhs.stencil_buffer, rhs.stencil_buffer);
  }

  void blit_framebuffer(Framebuffer& dest, Framebuffer const& source,
                        opengl::Buffer_Mask const mask)
  {
    Framebuffer::Construct_Info const source_info = source.get_construct_info();
    Framebuffer::Construct_Info const dest_info = dest.get_construct_info();
    glBlitFramebuffer(0, 0, source_info.width, source_info.height, 0, 0,
                      dest_info.width, dest_info.height, ENUM_TO_VALUE(mask),
                      GL_NEAREST);
  }

  void blit_framebuffer(Framebuffer* dest, Framebuffer const* source,
                        opengl::Buffer_Mask const mask)
  {
    Framebuffer::Construct_Info const source_info =
      source->get_construct_info();
    Framebuffer::Construct_Info const dest_info = dest->get_construct_info();
    glBlitFramebuffer(0, 0, source_info.width, source_info.height, 0, 0,
                      dest_info.width, dest_info.height, ENUM_TO_VALUE(mask),
                      GL_NEAREST);
  }

  void bind_default_framebuffer(Framebuffer::Bind_Mode const bind_mode)
  {
    glBindFramebuffer(ENUM_TO_VALUE(bind_mode), 0);
  }
} // namespace nebula::rendering