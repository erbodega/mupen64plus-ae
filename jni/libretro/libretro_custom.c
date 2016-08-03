#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <api/libretro.h>
#include <Graphics/plugin.h>

retro_log_printf_t log_cb = NULL;
retro_video_refresh_t video_cb = NULL;
extern void DebugMessage(int level, const char *message, ...);

bool flip_only;
static bool     pushed_frame        = false;
unsigned frame_dupe = false;
uint32_t *blitter_buf;
uint32_t *blitter_buf_lock   = NULL;

uint32_t screen_width = 640;
uint32_t screen_height = 480;
uint32_t screen_pitch;

bool emu_step_render(void);

int retro_return(int just_flipping)
{
   flip_only = just_flipping;

   if (just_flipping)
   {
      emu_step_render();
   }

   return 0;
}

bool emu_step_render(void)
{
   if (flip_only)
   {
      switch (gfx_plugin)
      {
         case GFX_ANGRYLION:
            video_cb((screen_pitch == 0) ? NULL : blitter_buf_lock, screen_width, screen_height, screen_pitch);
            break;

#if defined(HAVE_VULKAN)
         case GFX_PARALLEL:
            video_cb(parallel_frame_is_valid() ? RETRO_HW_FRAME_BUFFER_VALID : NULL,
                  parallel_frame_width(), parallel_frame_height(), 0);
            break;
#endif

         default:
#if defined(HAVE_OPENGL) || defined(HAVE_OPENGLES)
            video_cb(RETRO_HW_FRAME_BUFFER_VALID, screen_width, screen_height, 0);
#else
            video_cb((screen_pitch == 0) ? NULL : blitter_buf_lock, screen_width, screen_height, screen_pitch);
#endif
            break;
      }

      pushed_frame = true;
      return true;
   }

   if (!pushed_frame && frame_dupe) // Dupe. Not duping violates libretro API, consider it a speedhack.
      video_cb(NULL, screen_width, screen_height, screen_pitch);

   return false;
}

void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }

void DebugLog(enum retro_log_level level, const char *fmt, ...)
{
    va_list arguments;
    va_start ( arguments, fmt );
    DebugMessage( level, fmt, arguments);
    va_end ( arguments );
}
void retro_init(void)
{
   log_cb = DebugLog;
   screen_pitch = 0;
}