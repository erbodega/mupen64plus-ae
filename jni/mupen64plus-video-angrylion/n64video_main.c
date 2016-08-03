#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "z64.h"
#include "Gfx #1.3.h"
#include "vi.h"
#include "rdp.h"
#include "m64p_types.h"
#include "m64p_config.h"
#ifdef HAVE_RDP_DUMP
#include "../mupen64plus-video-paraLLEl/rdp_dump.h"
#endif
#include <stdlib.h>
#include "osal_dynamiclib.h"

extern unsigned int screen_width, screen_height;
extern uint32_t screen_pitch;

GFX_INFO gfx_info;

int res;
RECT __dst, __src;
int32_t pitchindwords;

int ProcessDListShown = 0;

int retro_return(int just_flipping);

EXPORT void CALL ChangeWindow (void)
{
}

void CloseDLL (void)
{
    return;
}

EXPORT void CALL ReadScreen2(void *dest, int *width, int *height, int front)
{
}

 
void DrawScreen (void)
{
}

void GetDllInfo(PLUGIN_INFO* PluginInfo)
{
    PluginInfo -> Version = 0x0103;
    PluginInfo -> Type  = 2;
    strcpy(
    PluginInfo -> Name, "angrylion's RDP"
    );
    PluginInfo -> NormalMemory = true;
    PluginInfo -> MemoryBswaped = true;
}

EXPORT void CALL SetRenderingCallback(void (*callback)(int))
{
}

EXPORT m64p_error CALL InitiateGFX (GFX_INFO Gfx_Info)
{
#ifdef HAVE_RDP_DUMP
   const char *env = getenv("RDP_DUMP");
   if (env)
      rdp_dump_init(env, 8 * 1024 * 1024);
#endif

   gfx_info = Gfx_Info;
   return true;
}

 
EXPORT void CALL MoveScreen (int xpos, int ypos)
{
}

 
EXPORT void CALL ProcessDList(void)
{
    if (!ProcessDListShown)
    {
        DisplayError("ProcessDList");
        ProcessDListShown = 1;
    }
}

EXPORT void CALL ProcessRDPList(void)
{
    process_RDP_list();
    return;
}

EXPORT void CALL RomClosed (void)
{
    rdp_close();
#ifdef HAVE_RDP_DUMP
    rdp_dump_end();
#endif
}

static m64p_handle l_ConfigAngrylion;
 
EXPORT m64p_error CALL RomOpen (void)
{
   /* TODO/FIXME: For now just force it to 640x480.
    *
    * Later on we might want a low-res mode (320x240)
    * for better performance as well in case screen_width
    * is 320 and height is 240.
    */
   if (screen_width < 640)
      screen_width = 640;
   if (screen_width > 640)
      screen_width = 640;

   if (screen_height < 480)
      screen_height = 480;
   if (screen_height > 480)
      screen_height = 480;

   pitchindwords = PRESCALE_WIDTH / 1; /* sizeof(DWORD) == sizeof(pixel) == 4 */
   screen_pitch  = PRESCALE_WIDTH << 2;

   rdp_init();
   overlay = ConfigGetParamBool(l_ConfigAngrylion, "VIOverlay");
   return 1;
}

EXPORT void CALL UpdateScreen(void)
{
    static int counter;

#ifdef HAVE_FRAMESKIP
    if (counter++ < skip)
        return;
    counter = 0;
#endif
    rdp_update();
    retro_return(true);
#if 0
    if (step != 0)
        MessageBox(NULL, "Updated screen.\nPaused.", "Frame Step", MB_OK);
#endif
    return;
}

EXPORT void CALL ShowCFB (void)
{
    //MessageBox(NULL, "ShowCFB", NULL, MB_ICONWARNING);
    UpdateScreen();
    return;
}


EXPORT void CALL ViStatusChanged (void)
{
}

EXPORT void CALL ViWidthChanged (void)
{
}

void FBWrite(unsigned int addr, unsigned int size)
{
}

EXPORT void CALL FBRead(unsigned int addr)
{
}

EXPORT void CALL FBGetFrameBufferInfo(void *pinfo)
{
}

EXPORT m64p_error CALL PluginGetVersion(m64p_plugin_type *PluginType, int *PluginVersion, int *APIVersion, const char **PluginNamePtr, int *Capabilities)
{
   /* set version info */
   if (PluginType != NULL)
      *PluginType = M64PLUGIN_GFX;

   if (PluginVersion != NULL)
      *PluginVersion = 0x016304;

   if (APIVersion != NULL)
      *APIVersion = 0x020100;

   if (PluginNamePtr != NULL)
      *PluginNamePtr = "MAME/Angrylion/HatCat video Plugin";

   if (Capabilities != NULL)
      *Capabilities = 0;

   return M64ERR_SUCCESS;
}


EXPORT m64p_error CALL PluginStartup(m64p_dynlib_handle CoreLibHandle, void *Context,
                                   void (*DebugCallback)(void *, int, const char *))
{
   CoreConfig_ConfigOpenSection = (ptr_ConfigOpenSection) osal_dynlib_getproc(CoreLibHandle, "ConfigOpenSection");
   CoreConfig_ConfigSetParameter = (ptr_ConfigSetParameter) osal_dynlib_getproc(CoreLibHandle, "ConfigSetParameter");
   CoreConfig_ConfigGetParameter = (ptr_ConfigGetParameter) osal_dynlib_getproc(CoreLibHandle, "ConfigGetParameter");
   CoreConfig_ConfigSetDefaultInt = (ptr_ConfigSetDefaultInt) osal_dynlib_getproc(CoreLibHandle, "ConfigSetDefaultInt");
   CoreConfig_ConfigSetDefaultFloat = (ptr_ConfigSetDefaultFloat) osal_dynlib_getproc(CoreLibHandle, "ConfigSetDefaultFloat");
   CoreConfig_ConfigSetDefaultBool = (ptr_ConfigSetDefaultBool) osal_dynlib_getproc(CoreLibHandle, "ConfigSetDefaultBool");
   CoreConfig_ConfigSetDefaultString = (ptr_ConfigSetDefaultString) osal_dynlib_getproc(CoreLibHandle, "ConfigSetDefaultString");
   CoreConfig_ConfigGetParamInt = (ptr_ConfigGetParamInt) osal_dynlib_getproc(CoreLibHandle, "ConfigGetParamInt");
   CoreConfig_ConfigGetParamFloat = (ptr_ConfigGetParamFloat) osal_dynlib_getproc(CoreLibHandle, "ConfigGetParamFloat");
   CoreConfig_ConfigGetParamBool = (ptr_ConfigGetParamBool) osal_dynlib_getproc(CoreLibHandle, "ConfigGetParamBool");
   CoreConfig_ConfigGetParamString = (ptr_ConfigGetParamString) osal_dynlib_getproc(CoreLibHandle, "ConfigGetParamString");

   CoreConfig_ConfigGetSharedDataFilepath = (ptr_ConfigGetSharedDataFilepath) osal_dynlib_getproc(CoreLibHandle, "ConfigGetSharedDataFilepath");
   CoreConfig_ConfigGetUserConfigPath = (ptr_ConfigGetUserConfigPath) osal_dynlib_getproc(CoreLibHandle, "ConfigGetUserConfigPath");
   CoreConfig_ConfigGetUserDataPath = (ptr_ConfigGetUserDataPath) osal_dynlib_getproc(CoreLibHandle, "ConfigGetUserDataPath");
   CoreConfig_ConfigGetUserCachePath = (ptr_ConfigGetUserCachePath) osal_dynlib_getproc(CoreLibHandle, "ConfigGetUserCachePath");

   /* Get the core Video Extension function pointers from the library handle */
   CoreVideo_Init = (ptr_VidExt_Init) osal_dynlib_getproc(CoreLibHandle, "VidExt_Init");
   CoreVideo_Quit = (ptr_VidExt_Quit) osal_dynlib_getproc(CoreLibHandle, "VidExt_Quit");
   CoreVideo_ListFullscreenModes = (ptr_VidExt_ListFullscreenModes) osal_dynlib_getproc(CoreLibHandle, "VidExt_ListFullscreenModes");
   CoreVideo_SetVideoMode = (ptr_VidExt_SetVideoMode) osal_dynlib_getproc(CoreLibHandle, "VidExt_SetVideoMode");
   CoreVideo_SetCaption = (ptr_VidExt_SetCaption) osal_dynlib_getproc(CoreLibHandle, "VidExt_SetCaption");
   CoreVideo_ToggleFullScreen = (ptr_VidExt_ToggleFullScreen) osal_dynlib_getproc(CoreLibHandle, "VidExt_ToggleFullScreen");
   CoreVideo_ResizeWindow = (ptr_VidExt_ResizeWindow) osal_dynlib_getproc(CoreLibHandle, "VidExt_ResizeWindow");
   CoreVideo_GL_GetProcAddress = (ptr_VidExt_GL_GetProcAddress) osal_dynlib_getproc(CoreLibHandle, "VidExt_GL_GetProcAddress");
   CoreVideo_GL_SetAttribute = (ptr_VidExt_GL_SetAttribute) osal_dynlib_getproc(CoreLibHandle, "VidExt_GL_SetAttribute");
   CoreVideo_GL_GetAttribute = (ptr_VidExt_GL_GetAttribute) osal_dynlib_getproc(CoreLibHandle, "VidExt_GL_GetAttribute");
   CoreVideo_GL_SwapBuffers = (ptr_VidExt_GL_SwapBuffers) osal_dynlib_getproc(CoreLibHandle, "VidExt_GL_SwapBuffers");

   CoreVideo_Init();

   BOOL configResult = Config_Open();

   retro_init();
   update_variables(true);

   l_DebugCallback = DebugCallback;
   l_DebugCallContext = Context;

   if(!configResult)
   {
      log_cb (RETRO_LOG_ERROR, "Could not open configuration");
   }

   ReadSettings();
   setAttributes();

   if(CoreVideo_GL_SwapBuffers == NULL)
   {
      log_cb(RETRO_LOG_INFO, "Invalid SwapByffers function");
   }

   CoreVideo_SetVideoMode(settings.scr_res_x, settings.scr_res_y, 0, M64VIDEO_FULLSCREEN, (m64p_video_flags) 0);
   return M64ERR_SUCCESS;
}

EXPORT m64p_error CALL PluginShutdown(void)
{
   CoreVideo_Quit();

   return M64ERR_SUCCESS;
}
