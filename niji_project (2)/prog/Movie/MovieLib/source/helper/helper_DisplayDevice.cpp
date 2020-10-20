#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_DisplayDevice.cpp
 Description: Helper file that manages CTR display presentation functions in 
              mono(2D) and stereo(3D) mode.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/fnd.h>
#include <nn/gx.h>

/*---------------------------------------------------------------------------*/

#include "helper_DisplayDevice.h"

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {
namespace displaydevice {

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

const int ALIGNMENT_SYSTEM_BUFFER     = 4;
const int ALIGNMENT_VERTEX            = 8;
const int ALIGNMENT_TEXTURE           = 128;
const int ALIGNMENT_RENDER_BUFFER     = 64;
const int ALIGNMENT_DISPLAY_BUFFER    = 16;
const int ALIGNMENT_3D_COMMAND_BUFFER = 16;

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static bool                 s_InitDone = false;
static gfl2::heap::NwAllocator* s_pFcramAlocator;
static uptr                 s_CurrentAddrVramA;
static uptr                 s_CurrentAddrVramB;
static bool                 s_LcdEnabled = false;
static u32                    s_DisplayOptions;
static bool                 s_3DEnabled;
static GLuint               s_CmdList;
static GLuint               s_Display0Buffer[3];
static GLuint               s_Display0BufferExt[3];
static GLuint               s_Display1Buffer[3];
static void*                s_Display0Ptr[3];
static void*                s_Display0PtrExt[3];
static void*                s_Display1Ptr[3];
static s32                  s_CurrentDispBuf0;
static s32                  s_CurrentDispBuf1;
static GLuint               s_RenderBuffer0[2];
static GLuint               s_RenderBuffer1[2];
static GLuint               s_FrameBufferObject[2];
static s32                  s_CurrentDisplay;
static nn::os::LightEvent   s_FlipDoneDisplay0;
static nn::os::LightEvent   s_FlipDoneDisplay1;

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static inline uptr MathRoundup (uptr x, int base)
{
    return ((x) + ((base)-1)) & ~((base)-1);
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static void* GxAllocate (GLenum area, GLenum aim, GLuint id, GLsizei size)
{
    if (s_InitDone == false)
    {
        NN_TPANIC_("DisplayDevice error: not initialized.\n");
    }

    if (size == 0)
        return 0;

    int addrAlign = 8;
    void* resultAddr = NULL;

    switch (aim)
    {
        case NN_GX_MEM_SYSTEM:
            addrAlign = ALIGNMENT_SYSTEM_BUFFER;
            break;
        case NN_GX_MEM_TEXTURE:
            addrAlign = ALIGNMENT_TEXTURE;
            break;
        case NN_GX_MEM_VERTEXBUFFER:
            addrAlign = ALIGNMENT_VERTEX;
            break;
        case NN_GX_MEM_RENDERBUFFER:
            addrAlign = ALIGNMENT_RENDER_BUFFER;
            break;
        case NN_GX_MEM_DISPLAYBUFFER:
            addrAlign = ALIGNMENT_DISPLAY_BUFFER;
            break;
        case NN_GX_MEM_COMMANDBUFFER:
            addrAlign = ALIGNMENT_3D_COMMAND_BUFFER;
            break;
        default:
            NN_TPANIC_("DisplayDevice error: invalid parameter (0x%X).\n", aim);
            break;
    }

    switch (area)
    {
        case NN_GX_MEM_FCRAM:
            if ( (resultAddr = s_pFcramAlocator->Alloc(size, addrAlign)) == NULL)
            {
                NN_TPANIC_("DisplayDevice error: lack of resource on main memory (FCRAM).\n");
            }
            break;
        case NN_GX_MEM_VRAMA:
            {
                if (MathRoundup(s_CurrentAddrVramA, addrAlign) + size > nn::gx::GetVramEndAddr(nn::gx::MEM_VRAMA))
                {
                    NN_TPANIC_("DisplayDevice error: lack of resource on VRAM-A.\n");
                }

                s_CurrentAddrVramA = MathRoundup(s_CurrentAddrVramA, addrAlign);
                resultAddr = reinterpret_cast<void*>(s_CurrentAddrVramA);
                s_CurrentAddrVramA += size;
            }
            break;
        case NN_GX_MEM_VRAMB:
            {
                if (MathRoundup(s_CurrentAddrVramB, addrAlign) + size > nn::gx::GetVramEndAddr(nn::gx::MEM_VRAMB))
                {
                    NN_TPANIC_("DisplayDevice error: lack of resource on VRAM-B.\n");
                }

                s_CurrentAddrVramB = MathRoundup(s_CurrentAddrVramB, addrAlign);
                resultAddr = reinterpret_cast<void*>(s_CurrentAddrVramB);
                s_CurrentAddrVramB += size;
            }
            break;
        default:
            NN_TPANIC_("DisplayDevice error: invalid parameter (0x%X).\n", area);
            break;
    }

    NN_UNUSED_VAR(id);

    return resultAddr;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static void GxDeallocate (GLenum area, GLenum aim, GLuint id, void* addr)
{
    NN_UNUSED_VAR(aim);
    NN_UNUSED_VAR(id);
                
    if (s_InitDone == false)
    {
        NN_TPANIC_("DisplayDevice error: not initialized.\n");
    }

    switch (area)
    {
        case NN_GX_MEM_FCRAM:
            s_pFcramAlocator->Free(addr);
            break;

        case NN_GX_MEM_VRAMA:
        case NN_GX_MEM_VRAMB:
            break;

        default:
            NN_TPANIC_("DisplayDevice error: invalid parameter.\n");
            break;
    }
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static void VSyncCallback(GLenum displayId)
{
    if (displayId == NN_GX_DISPLAY0)
        s_FlipDoneDisplay0.Signal();
    else
        s_FlipDoneDisplay1.Signal();
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static void WaitVsyncDone (s32 display)
{
    if (s_DisplayOptions & ENABLE_WAIT_VSYNC)
    {
        if (display == NN_GX_DISPLAY0 || display == NN_GX_DISPLAY_BOTH)
            s_FlipDoneDisplay0.Wait();
        if (display == NN_GX_DISPLAY1 || display == NN_GX_DISPLAY_BOTH)
            s_FlipDoneDisplay1.Wait();
    }
}

/*---------------------------------------------------------------------------*
  ENABLE_WAIT_VSYNC option of Initialize function tells that we want to wait 
  for the vertical retrace after a Flip function call before accessing display 
  buffers with either GetBackBuffer or GetFrontBuffer function. 
  By default it is selected and GetBackBuffer or GetFrontBuffer 
  function will wait for the VSync to be performed before returning display
  buffer pointer.
  If not selected then GetBackBuffer or GetFrontBuffer will return 
  immediately.
  We recommend keeping this option activated(default) to avoid potential 
  display tearing issues.
 *---------------------------------------------------------------------------*/

bool Initialize (gfl2::heap::NwAllocator* pAllocator, u32 displayOptions)
{
    /*-----------------------------------------------------------------------*/

    if (s_InitDone == true)
        return false;

    s_InitDone = true;

    /*-----------------------------------------------------------------------*
      GX display initialization 
     *-----------------------------------------------------------------------*/

    s_pFcramAlocator = pAllocator;
    
    s_CurrentAddrVramA = nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMA);
    s_CurrentAddrVramB = nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMB);

    if (nngxInitialize(GxAllocate, GxDeallocate) == GL_FALSE)
    {
        NN_PANIC("[mo::helper::DisplayDevice] : nngxInitialize() failed.\n");
    }

    s_DisplayOptions = displayOptions;

    // We activate 3D mode if wanted
    if (s_DisplayOptions & ENABLE_3D)
    {
        s_3DEnabled = true;
        nngxSetDisplayMode(NN_GX_DISPLAYMODE_STEREO);
    }
    else
    {
        s_3DEnabled = false;
        nngxSetDisplayMode(NN_GX_DISPLAYMODE_NORMAL);
    }

    /*-----------------------------------------------------------------------*
      We create one GX command list here
     *-----------------------------------------------------------------------*/

    nngxGenCmdlists(1, &s_CmdList);
    nngxBindCmdlist(s_CmdList);
    nngxCmdlistStorage(0x40000, 256);
    nngxSetCmdlistParameteri(NN_GX_CMDLIST_RUN_MODE, NN_GX_CMDLIST_SERIAL_RUN);

    /*-----------------------------------------------------------------------*
      We create all the display buffer here, that is (display0(upper) and display1(lower) and optionally
      display0ext for 3D mode.
     *-----------------------------------------------------------------------*/

    int nbDisplayBuffer;
    if (s_DisplayOptions & ENABLE_TRIPLE_BUFFERING)
        nbDisplayBuffer = 3;
    else
        nbDisplayBuffer = 2;

    // Upper display (display0) 
    nngxActiveDisplay(NN_GX_DISPLAY0);
    nngxGenDisplaybuffers(nbDisplayBuffer, s_Display0Buffer);
    for (int i=0; i<nbDisplayBuffer; i++)
    {
        nngxBindDisplaybuffer(s_Display0Buffer[i]);

        if (s_DisplayOptions & ENABLE_VRAM_DISPLAY_BUFFER)
        {
            if (i % 2)
                nngxDisplaybufferStorage(GL_RGB8_OES, nn::gx::DISPLAY0_WIDTH, nn::gx::DISPLAY0_HEIGHT, NN_GX_MEM_VRAMB);
            else
                nngxDisplaybufferStorage(GL_RGB8_OES, nn::gx::DISPLAY0_WIDTH, nn::gx::DISPLAY0_HEIGHT, NN_GX_MEM_VRAMA);
        }
        else
            nngxDisplaybufferStorage(GL_RGB8_OES, nn::gx::DISPLAY0_WIDTH, nn::gx::DISPLAY0_HEIGHT, NN_GX_MEM_FCRAM);

        nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_ADDRESS, (GLint*)&s_Display0Ptr[i]);
    }
    nngxDisplayEnv(0, 0);
    
    nngxBindDisplaybuffer(0);
    s_CurrentDispBuf0 = 0;

    // Upper display ext (display0 ext) if 3D mode wanted 
    if (s_DisplayOptions & ENABLE_3D)
    {
        nngxActiveDisplay(NN_GX_DISPLAY0_EXT);
        nngxGenDisplaybuffers(nbDisplayBuffer, s_Display0BufferExt);
        for (int i=0; i<nbDisplayBuffer; i++)
        {
            nngxBindDisplaybuffer(s_Display0BufferExt[i]);

            if (s_DisplayOptions & ENABLE_VRAM_DISPLAY_BUFFER)
            {
                if (i % 2)
                    nngxDisplaybufferStorage(GL_RGB8_OES, nn::gx::DISPLAY0_WIDTH, nn::gx::DISPLAY0_HEIGHT, NN_GX_MEM_VRAMB);
                else
                    nngxDisplaybufferStorage(GL_RGB8_OES, nn::gx::DISPLAY0_WIDTH, nn::gx::DISPLAY0_HEIGHT, NN_GX_MEM_VRAMA);
            }
            else
                nngxDisplaybufferStorage(GL_RGB8_OES, nn::gx::DISPLAY0_WIDTH, nn::gx::DISPLAY0_HEIGHT, NN_GX_MEM_FCRAM);

            nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_ADDRESS, (GLint*)&s_Display0PtrExt[i]);
        }
        nngxDisplayEnv(0, 0);
    
        nngxBindDisplaybuffer(0);
    }

    // Lower display (display1) 
    nngxActiveDisplay(NN_GX_DISPLAY1);
    nngxGenDisplaybuffers(nbDisplayBuffer, s_Display1Buffer);
    for (int i=0; i<nbDisplayBuffer; i++)
    {
        nngxBindDisplaybuffer(s_Display1Buffer[i]);

        if (s_DisplayOptions & ENABLE_VRAM_DISPLAY_BUFFER)
        {
            if (i % 2)
                nngxDisplaybufferStorage(GL_RGB8_OES, nn::gx::DISPLAY1_WIDTH, nn::gx::DISPLAY1_HEIGHT, NN_GX_MEM_VRAMB);
            else
                nngxDisplaybufferStorage(GL_RGB8_OES, nn::gx::DISPLAY1_WIDTH, nn::gx::DISPLAY1_HEIGHT, NN_GX_MEM_VRAMA);
        }
        else
            nngxDisplaybufferStorage(GL_RGB8_OES, nn::gx::DISPLAY1_WIDTH, nn::gx::DISPLAY1_HEIGHT, NN_GX_MEM_FCRAM);

        nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_ADDRESS, (GLint*)&s_Display1Ptr[i]);
    }
    
    nngxBindDisplaybuffer(0);
    s_CurrentDispBuf1 = 0;
    
    /*-----------------------------------------------------------------------*
      Creation of render buffers for upper and lower screens
     *-----------------------------------------------------------------------*/

    glGenFramebuffers(2, s_FrameBufferObject);
    glGenRenderbuffers(2, s_RenderBuffer0);
    glGenRenderbuffers(2, s_RenderBuffer1);

    // For display0(upper screen) 
    glBindRenderbuffer(GL_RENDERBUFFER, s_RenderBuffer0[0]);
    glRenderbufferStorage(GL_RENDERBUFFER | NN_GX_MEM_VRAMA, GL_RGBA8_OES, nn::gx::DISPLAY0_WIDTH, nn::gx::DISPLAY0_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, s_FrameBufferObject[0]);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, s_RenderBuffer0[0]);

    glBindRenderbuffer(GL_RENDERBUFFER, s_RenderBuffer0[1]);
    glRenderbufferStorage(GL_RENDERBUFFER | NN_GX_MEM_VRAMB, GL_DEPTH24_STENCIL8_EXT, nn::gx::DISPLAY0_WIDTH, nn::gx::DISPLAY0_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, s_RenderBuffer0[1]);

    // For display1(lower screen) 
    glBindRenderbuffer(GL_RENDERBUFFER, s_RenderBuffer1[0]);
    glRenderbufferStorage(GL_RENDERBUFFER | NN_GX_MEM_VRAMA, GL_RGBA8_OES, nn::gx::DISPLAY1_WIDTH, nn::gx::DISPLAY1_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, s_FrameBufferObject[1]);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, s_RenderBuffer1[0]);

    glBindRenderbuffer(GL_RENDERBUFFER, s_RenderBuffer1[1]);
    glRenderbufferStorage(GL_RENDERBUFFER | NN_GX_MEM_VRAMB, GL_DEPTH24_STENCIL8_EXT, nn::gx::DISPLAY1_WIDTH, nn::gx::DISPLAY1_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, s_RenderBuffer1[1]);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*-----------------------------------------------------------------------*/

    s_FlipDoneDisplay0.Initialize(true);
    s_FlipDoneDisplay1.Initialize(true);
    nngxSetVSyncCallback(NN_GX_DISPLAY_BOTH, VSyncCallback);

    /*-----------------------------------------------------------------------*/
    
    // Upper screen left eye painted in black
    mw::mo::helper::displaydevice::BeginDraw(NN_GX_DISPLAY0);
    mw::mo::helper::displaydevice::Clear(0.f,0.f,0.f,0.f);
    mw::mo::helper::displaydevice::EndDraw();

    // Upper screen right eye painted in black
    if (s_DisplayOptions & ENABLE_3D)
    {
        mw::mo::helper::displaydevice::BeginDraw(NN_GX_DISPLAY0_EXT);
        mw::mo::helper::displaydevice::Clear(0.f,0.f,0.f,0.f);
        mw::mo::helper::displaydevice::EndDraw();
    }

    // Bottom screen painted in black 
    mw::mo::helper::displaydevice::BeginDraw(NN_GX_DISPLAY1);
    mw::mo::helper::displaydevice::Clear(0.f,0.f,0.f,0.f); 
    mw::mo::helper::displaydevice::EndDraw();

    //Commit draw and flip
    mw::mo::helper::displaydevice::CommitDraw();
    mw::mo::helper::displaydevice::Flip(NN_GX_DISPLAY_BOTH);

    /*-----------------------------------------------------------------------*/

    return true;

    /*-----------------------------------------------------------------------*/
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

bool Finalize (void)
{
    if (s_InitDone == false)
        return false;
    
    CommitDraw();

    nngxSetVSyncCallback(NN_GX_DISPLAY_BOTH, 0);

    s_FlipDoneDisplay0.Finalize();
    s_FlipDoneDisplay1.Finalize();

	int nbDisplayBuffer;
    if (s_DisplayOptions & ENABLE_TRIPLE_BUFFERING)
        nbDisplayBuffer = 3;
    else
        nbDisplayBuffer = 2;

    glDeleteRenderbuffers(nbDisplayBuffer, s_RenderBuffer1);
    glDeleteRenderbuffers(nbDisplayBuffer, s_RenderBuffer0);
    glDeleteFramebuffers(nbDisplayBuffer, s_FrameBufferObject);

    nngxDeleteDisplaybuffers(nbDisplayBuffer, s_Display1Buffer);
    nngxDeleteDisplaybuffers(nbDisplayBuffer, s_Display0BufferExt);
    nngxDeleteDisplaybuffers(nbDisplayBuffer, s_Display0Buffer);
    nngxDeleteCmdlists(1, &s_CmdList);
    nngxFinalize();

    s_InitDone = false;

    return true;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

bool Enable3d (bool enable3D)
{
    if (!(s_DisplayOptions & ENABLE_3D))
        return false;

    if (s_3DEnabled != enable3D)
    {
        s_3DEnabled = enable3D;
        if (s_3DEnabled == true)
            nngxSetDisplayMode(NN_GX_DISPLAYMODE_STEREO);
        else
            nngxSetDisplayMode(NN_GX_DISPLAYMODE_NORMAL);
    }

    return true;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

bool Is3dEnabled (void)
{
    return s_3DEnabled;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void* GetBackBuffer (s32 display)
{
    switch (display)
    {
        case NN_GX_DISPLAY0:
            WaitVsyncDone(display);
            return s_Display0Ptr[s_CurrentDispBuf0];

        case NN_GX_DISPLAY0_EXT:
            WaitVsyncDone(display);
            return s_Display0PtrExt[s_CurrentDispBuf0];

        case NN_GX_DISPLAY1:
            WaitVsyncDone(display);
            return s_Display1Ptr[s_CurrentDispBuf1];

        default:
            return NULL;
    }
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void* GetFrontBuffer (s32 display)
{
    switch (display)
    {
        case NN_GX_DISPLAY0:
            WaitVsyncDone(display);
            if (s_DisplayOptions & ENABLE_TRIPLE_BUFFERING)
                return s_Display0Ptr[(s_CurrentDispBuf0 + 2) % 3];
            else
                return s_Display0Ptr[!s_CurrentDispBuf0];

        case NN_GX_DISPLAY0_EXT:
            WaitVsyncDone(display);
            if (s_DisplayOptions & ENABLE_TRIPLE_BUFFERING)
                return s_Display0PtrExt[(s_CurrentDispBuf0 + 2) % 3];
            else
                return s_Display0PtrExt[!s_CurrentDispBuf0];

        case NN_GX_DISPLAY1:
            WaitVsyncDone(display);
            if (s_DisplayOptions & ENABLE_TRIPLE_BUFFERING)
                return s_Display1Ptr[(s_CurrentDispBuf1 + 2) % 3];
            else
                return s_Display1Ptr[!s_CurrentDispBuf1];

        default:
            return NULL;
    }
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void BeginDraw (s32 display)
{    
    s_CurrentDisplay = display;

    switch (display)
    {
        case NN_GX_DISPLAY0:
        case NN_GX_DISPLAY0_EXT:
            glBindFramebuffer(GL_FRAMEBUFFER, s_FrameBufferObject[0]);
            glViewport(0, 0, nn::gx::DISPLAY0_WIDTH, nn::gx::DISPLAY0_HEIGHT);
            break;

        case NN_GX_DISPLAY1:
            glBindFramebuffer(GL_FRAMEBUFFER, s_FrameBufferObject[1]);
            glViewport(0, 0, nn::gx::DISPLAY1_WIDTH, nn::gx::DISPLAY1_HEIGHT);
            break;

        default:
            break;
    }
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void EndDraw (void)
{
    switch (s_CurrentDisplay)
    {
        case NN_GX_DISPLAY0:
            glBindFramebuffer(GL_FRAMEBUFFER, s_FrameBufferObject[0]);
            nngxTransferRenderImage(s_Display0Buffer[s_CurrentDispBuf0], NN_GX_ANTIALIASE_NOT_USED, GL_FALSE, 0, 0);
            break;

        case NN_GX_DISPLAY0_EXT:
            glBindFramebuffer(GL_FRAMEBUFFER, s_FrameBufferObject[0]);
            nngxTransferRenderImage(s_Display0BufferExt[s_CurrentDispBuf0], NN_GX_ANTIALIASE_NOT_USED, GL_FALSE, 0, 0);
            break;

        case NN_GX_DISPLAY1:
            glBindFramebuffer(GL_FRAMEBUFFER, s_FrameBufferObject[1]);
            nngxTransferRenderImage(s_Display1Buffer[s_CurrentDispBuf1], NN_GX_ANTIALIASE_NOT_USED, GL_FALSE, 0, 0);    
            break;

        default:
            break;
    }
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void Clear (const f32 red, const f32 green, const f32 blue, const f32 alpha)
{
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void CommitDraw (void)
{
    nngxRunCmdlist();
    nngxWaitCmdlistDone();
    nngxClearCmdlist();
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void Flip (s32 display)
{

    if (display == NN_GX_DISPLAY0 || display == NN_GX_DISPLAY_BOTH)
    {
        nngxActiveDisplay(NN_GX_DISPLAY0);
        nngxBindDisplaybuffer(s_Display0Buffer[s_CurrentDispBuf0]);
                
        if (s_DisplayOptions & ENABLE_3D)
        {
            nngxActiveDisplay(NN_GX_DISPLAY0_EXT);
            nngxBindDisplaybuffer(s_Display0BufferExt[s_CurrentDispBuf0]);
        }

        if (s_DisplayOptions & ENABLE_TRIPLE_BUFFERING)
            s_CurrentDispBuf0 = (s_CurrentDispBuf0 + 1) % 3;
        else
            s_CurrentDispBuf0 = !s_CurrentDispBuf0;

        s_FlipDoneDisplay0.ClearSignal();
    }
    if (display == NN_GX_DISPLAY1 || display == NN_GX_DISPLAY_BOTH)
    {
        nngxActiveDisplay(NN_GX_DISPLAY1);
        nngxBindDisplaybuffer(s_Display1Buffer[s_CurrentDispBuf1]);

        if (s_DisplayOptions & ENABLE_TRIPLE_BUFFERING)
            s_CurrentDispBuf1 = (s_CurrentDispBuf1 + 1) % 3;
        else
            s_CurrentDispBuf1 = !s_CurrentDispBuf1;

        s_FlipDoneDisplay1.ClearSignal();
    }
    
    nngxSwapBuffers(display);

    if (s_LcdEnabled == false)
    {
        nngxStartLcdDisplay();
        s_LcdEnabled = true;
    }
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

} // end of namespace displaydevice
} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
