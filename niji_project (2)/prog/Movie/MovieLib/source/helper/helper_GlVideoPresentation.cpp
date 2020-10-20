#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_GlVideoPresentation.h
 Description: Helper file that implements video image presentation to screen
              using OpenGL ES API.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/fnd.h>

/*---------------------------------------------------------------------------*/

#include "helper_DisplayDevice.h"
#include "helper_GlVideoPresentation.h"


#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include "../renderer/MobiclipRenderingPipeLine.h"


/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*
  Constructor.
  Reset RGB texture pointers to NULL.
 *---------------------------------------------------------------------------*/

GlVideoPresentation::GlVideoPresentation()
{
    for (int i=0; i<3; i++)
        m_RgbVideoImageData[i] = NULL;
}

/*---------------------------------------------------------------------------*
  Allocate RGB texture buffers and 
  initialize mw::mo::helper::RgbImage classes that are to be used by 
  the given video stream.
 *---------------------------------------------------------------------------*/

bool GlVideoPresentation::AllocateImage (
										 const mw::mo::moflex::VideoStreamInfo* inVideoStreamInfo, 
                                         u8** outImageData, mw::mo::helper::RgbImage* outImageClass, u32* outStride)
{
    /* We retrieve the width and height */
    int width = inVideoStreamInfo->m_Width;
    int height = inVideoStreamInfo->m_Height;

    /* We find the number of frames per video images.
       If the video is 3D interleaved the value is 2, otherwise it is 1
    */
    int nbFramesPerImage;
    if (inVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_NONE)
        nbFramesPerImage = 1;
    else
        nbFramesPerImage = 2;
    
    /* The texture width will be the closest power of 2 that is greater or equal to the video width.
       This is because GL textures can only be power of 2.
    */
    if (width > 512)
        *outStride = 1024;
    else if (width > 256) 
        *outStride = 512;
    else
        *outStride = 256;

    if (nbFramesPerImage == 2)
        outStride[1] = outStride[0];

    /* The texture height will be the closest power of 2 that is greater or equal to the video height.
       This is because GL textures can only be power of 2.
    */
    int powHeight;
    if (height > 512) 
        powHeight = 1024;
    else if (height > 256) 
        powHeight = 512;
    else
        powHeight = 256;

    for (int i=0; i<nbFramesPerImage; i++)
    {
        /* In case of stereoscopic video and toptobottom or side by size layout, 
           the same texture buffer will be used for both eyes */

        if (i == 1 && (inVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_SIDEBYSIDE_LEFT_EYE_FIRST ||
                       inVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_SIDEBYSIDE_RIGHT_EYE_FIRST ||
                       inVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_TOPTOBOTTOM_LEFT_EYE_FIRST ||
                       inVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_TOPTOBOTTOM_RIGHT_EYE_FIRST))
            outImageData[1] = outImageData[0];
        else 
        {
            outImageData[i] = reinterpret_cast<u8*>(m_pAllocator->Alloc((*outStride) * powHeight * 4, 128));
            if (outImageData[i] == NULL) 
                return false;
        }

        /* In case of stereoscopic video and toptobottom or side by size layout, 
           two mw::mo::helper::RgbImageRgbImage instances will point the same texture buffer */

        if (inVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_SIDEBYSIDE_LEFT_EYE_FIRST ||
            inVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_SIDEBYSIDE_RIGHT_EYE_FIRST)
            outImageClass[i].Initialize( *outStride, powHeight, i * (width>>1), 0, (width>>1), height, GL_RGBA_NATIVE_DMP, GL_UNSIGNED_BYTE, outImageData[i]);
        else if (inVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_TOPTOBOTTOM_LEFT_EYE_FIRST ||
                 inVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_TOPTOBOTTOM_RIGHT_EYE_FIRST)
            outImageClass[i].Initialize( *outStride, powHeight, 0, i * (height>>1), width, (height>>1), GL_RGBA_NATIVE_DMP, GL_UNSIGNED_BYTE, outImageData[i]);
        else
            outImageClass[i].Initialize( (int)(*outStride), powHeight, 0, 0, width, height, GL_RGBA_NATIVE_DMP, GL_UNSIGNED_BYTE, outImageData[i]);

        /* If image is pre-rotated we configure the mw::mo::helper::RgbImageRgbImage rotation parameter */

        if (inVideoStreamInfo->m_Rotation == mw::mo::moflex::ROTATION_CLOCKWISE_90)
            outImageClass[i].SetImageRotation(90.f);
        else if (inVideoStreamInfo->m_Rotation == mw::mo::moflex::ROTATION_CLOCKWISE_180)
            outImageClass[i].SetImageRotation(180.f);
        else if (inVideoStreamInfo->m_Rotation == mw::mo::moflex::ROTATION_CLOCKWISE_270)
            outImageClass[i].SetImageRotation(270.f);
    }

    /* In case of 3D video with right eye encoded first the display index 0 is the one 
       for the right eye buffer, that is NN_GX_DISPLAY0_EXT */

    if (inVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_INTERLEAVED_RIGHT_EYE_FIRST ||
        inVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_TOPTOBOTTOM_RIGHT_EYE_FIRST ||
        inVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_SIDEBYSIDE_RIGHT_EYE_FIRST)
    {
        m_DisplayId[0] = NN_GX_DISPLAY0_EXT;
        m_DisplayId[1] = NN_GX_DISPLAY0;
    }
    else
    {
        m_DisplayId[0] = NN_GX_DISPLAY0;
        m_DisplayId[1] = NN_GX_DISPLAY0_EXT;
    }
    
    return true;
}

/*---------------------------------------------------------------------------*
  Video presentation initialization.
  Allocates all needed images for top and bottom screen presentation, if any.
 *---------------------------------------------------------------------------*/

bool GlVideoPresentation::Initialize (
									  gfl2::heap::NwAllocator* pAllocator,
                    gfl2::util::GLHeapAllocator* pGLDeviceAllocator,
                    movie::renderer::MobiclipRenderingPipeLine* renderingPipeLine,
                                      const mw::mo::moflex::VideoStreamInfo* topScreenVideoStreamInfo,
                                      const mw::mo::moflex::VideoStreamInfo* bottomScreenVideoStreamInfo)
{
    /* Call of Initialize method of parent class */
    if (VideoPresentationBuffer::Initialize(pAllocator, pGLDeviceAllocator, renderingPipeLine, topScreenVideoStreamInfo, bottomScreenVideoStreamInfo) == false)
        return false;
    
    /* We will display a video to the top screen, we allocate the either monoscopic(1) or stereoscopic(2) images */
    if (m_TopScreenVideoStreamInfo != NULL)
    {
        if (AllocateImage( m_TopScreenVideoStreamInfo, m_RgbVideoImageData, m_RgbVideoImage, m_RgbVideoImageStride) == false)
            return false;
    }

    /* We will display a video to the bottom screen, we allocate the corresponding image */
    if (m_BottomScreenVideoStreamInfo != NULL)
    {
        if (m_BottomScreenVideoStreamInfo->m_Layout != mw::mo::moflex::LAYOUT_NONE)
        {
            NN_LOG("No 3D videos are allowed to be displayed on bottom screen ...\n");
            return false;
        }

        if (AllocateImage( m_BottomScreenVideoStreamInfo, m_RgbVideoImageData+2, m_RgbVideoImage+2, m_RgbVideoImageStride+2) == false)
            return false;

        m_DisplayId[2] = NN_GX_DISPLAY1;
    }

    for (int i=0; i<3; i++)
    {
      const void* buf = m_RgbVideoImage[i].GetPixels();
      float rotate = m_RgbVideoImage[i].GetRotationZ();
      if(buf)
      {
        NAGI_PRINT("SetBufForTexture %d",i);
        switch(i)
        {
        case 0:
          renderingPipeLine->SetBufForTexture(
            gfl2::gfx::CtrDisplayNo::LEFT,
            pGLDeviceAllocator,
            buf,
            gfl2::gfx::Pool::Default,
            512,
            256,
            gfl2::gfx::Format::A8R8G8B8,
            rotate
          );
          break;
        case 1:
          renderingPipeLine->SetBufForTexture(
            gfl2::gfx::CtrDisplayNo::RIGHT,
            pGLDeviceAllocator,
            buf,
            gfl2::gfx::Pool::Default,
            512,
            256,
            gfl2::gfx::Format::A8R8G8B8,
            rotate
          );
          break;
        case 2:
          renderingPipeLine->SetBufForTexture(
            gfl2::gfx::CtrDisplayNo::DOWN,
            pGLDeviceAllocator,
            buf,
            gfl2::gfx::Pool::Default,
            512,
            256,
            gfl2::gfx::Format::A8R8G8B8,
            rotate
          );
          break;
        }
      }
    }

    return true;
}

/*---------------------------------------------------------------------------*
 Video presentation finalization.
  *---------------------------------------------------------------------------*/

void GlVideoPresentation::Finalize (movie::renderer::MobiclipRenderingPipeLine* renderingPipeLine)
{
    for (int i=0; i<3; i++)
    {
      switch(i)
      {
      case 0:
        renderingPipeLine->UnsetBufForTexture(
          gfl2::gfx::CtrDisplayNo::LEFT
        );
        break;
      case 1:
        renderingPipeLine->UnsetBufForTexture(
          gfl2::gfx::CtrDisplayNo::RIGHT
        );
        break;
      case 2:
        renderingPipeLine->UnsetBufForTexture(
          gfl2::gfx::CtrDisplayNo::DOWN
        );
        break;
      }
    }

    for (int i=0; i<3; i++)
    {
        if (m_RgbVideoImageData[i] != NULL)
        {
            m_RgbVideoImage[i].Finalize();
            m_pAllocator->Free(m_RgbVideoImageData[i]);
            if (i == 0 && m_RgbVideoImageData[0] == m_RgbVideoImageData[1])
                m_RgbVideoImageData[1] = NULL;
            m_RgbVideoImageData[i] = NULL;
        }
    }

    VideoPresentationBuffer::Finalize(renderingPipeLine);
}

/*---------------------------------------------------------------------------*
 Tell to the caller (typically a mw::mo::helper::VideoConversion class)
  that the RGB buffers of the video presentation class is NOT linear.
 *---------------------------------------------------------------------------*/

bool GlVideoPresentation::ScreenBufferLinear (void)
{
    return false;
}

/*---------------------------------------------------------------------------*
 Get the RGB buffer pointer of the video presentation class that is 
  associated to the given eye index:
  - 0 value is the first frame of a left/right eye image tuple of the top 
    screen.
  - 1 value is the second frame of a left/right eye image tuple of the top 
    screen.
    If image is no 3D interleaved, NULL is returned
  - 2 value is the bottom screen.
 *---------------------------------------------------------------------------*/

void* GlVideoPresentation::GetScreenBufferPtr(int eyeIndex)
{
    return m_RgbVideoImageData[eyeIndex];
}

/*---------------------------------------------------------------------------*
 Get the buffer stride, in pixels of the buffer that is associated to the 
 given eye index:
  - 0 value is the first frame of a left/right eye image tuple of the top 
    screen.
  - 1 value is the second frame of a left/right eye image tuple of the top 
    screen. 
  - 2 value is the bottom screen
 *---------------------------------------------------------------------------*/

u32 GlVideoPresentation::GetScreenStride(int eyeIndex)
{
    return m_RgbVideoImageStride[eyeIndex];
}

/*---------------------------------------------------------------------------*
 Get GX display Id that is associated to the given eye index:
  - 0 value is the first frame of a left/right eye image tuple of the top 
    screen.
  - 1 value is the second frame of a left/right eye image tuple of the top 
    screen. 
  - 2 value is the bottom screen
 *---------------------------------------------------------------------------*/

u32 GlVideoPresentation::GetDisplayId(int eyeIndex)
{
    return m_DisplayId[eyeIndex];
}

/*---------------------------------------------------------------------------*
 Draw the images to the buffers. This performs GL rendering for each video 
 frames. 
 *---------------------------------------------------------------------------*/

void GlVideoPresentation::Draw(void)
{
    for (int i=0; i<3; i++)
    {
        if (m_RgbVideoImageData[i] != NULL)
        {
            m_RgbVideoImage[i].Draw();
        }
    }
}
void GlVideoPresentation::UpdateTexture()
{
    for (int i=0; i<3; i++)
    {
        if (m_RgbVideoImageData[i] != NULL)
        {
        	m_RgbVideoImage[i].UpdateTexture();
        }
    }
}

/*---------------------------------------------------------------------------*
  Present the drawn images to the screen. We only have to make a display 
  buffers flip in this case.
 *---------------------------------------------------------------------------*/

void GlVideoPresentation::Flip()
{
    if (m_TopScreenVideoStreamInfo != NULL)
    {
        if (m_BottomScreenVideoStreamInfo != NULL)
            mw::mo::helper::displaydevice::Flip(NN_GX_DISPLAY_BOTH);
        else
            mw::mo::helper::displaydevice::Flip(NN_GX_DISPLAY0);
    }
    else
        if (m_BottomScreenVideoStreamInfo != NULL)
            mw::mo::helper::displaydevice::Flip(NN_GX_DISPLAY1);
}

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
