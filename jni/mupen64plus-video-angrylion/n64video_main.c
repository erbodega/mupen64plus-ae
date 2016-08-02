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