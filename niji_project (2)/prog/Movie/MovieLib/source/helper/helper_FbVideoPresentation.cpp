#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_FbVideoPresentation.h
 Description: Helper file that implements video image presentation to screen
              by directly accessing the CTR device screen frame buffers.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/fnd.h>

/*---------------------------------------------------------------------------*/

#include "helper_DisplayDevice.h"
#include "helper_FbVideoPresentation.h"

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*
  Initialization of the video presentation object
 *---------------------------------------------------------------------------*/

bool FbVideoPresentation::Initialize (gfl2::heap::NwAllocator* pAllocator, 
                                      const mw::mo::moflex::VideoStreamInfo* topScreenVideoStreamInfo,
                                      const mw::mo::moflex::VideoStreamInfo* bottomScreenVideoStreamInfo)
{
    /* We call the parent Initialize method */
    if (VideoPresentation::Initialize(pAllocator, topScreenVideoStreamInfo, bottomScreenVideoStreamInfo) == false)
        return false;

    /* We check that the videos are all pre-rotated by 90ｰ */
    if ((m_TopScreenVideoStreamInfo != NULL && m_TopScreenVideoStreamInfo->m_Rotation != mw::mo::moflex::ROTATION_CLOCKWISE_90) || 
        (m_BottomScreenVideoStreamInfo != NULL && m_BottomScreenVideoStreamInfo->m_Rotation != mw::mo::moflex::ROTATION_CLOCKWISE_90))
    {
        NN_LOG("Direct screen access needs pre-rotated images by 90ｰ clockwise...\n");
        return false;
    }

    if (m_TopScreenVideoStreamInfo != NULL)
    {
        /* We check that the top screen video is either 2D of 3D interleaved */
        if (m_TopScreenVideoStreamInfo->m_Layout != mw::mo::moflex::LAYOUT_NONE &&
            m_TopScreenVideoStreamInfo->m_Layout != mw::mo::moflex::LAYOUT_INTERLEAVED_LEFT_EYE_FIRST &&
            m_TopScreenVideoStreamInfo->m_Layout != mw::mo::moflex::LAYOUT_INTERLEAVED_RIGHT_EYE_FIRST)
        {
            NN_LOG("Direct screen access only allows 3D interleaved or 2D videos...\n");
            return false;
        }

        if (m_TopScreenVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_INTERLEAVED_RIGHT_EYE_FIRST)
        {
            m_DisplayId[0] = NN_GX_DISPLAY0_EXT;
            m_DisplayId[1] = NN_GX_DISPLAY0;
        }    
        else
        {
            m_DisplayId[0] = NN_GX_DISPLAY0;
            m_DisplayId[1] = NN_GX_DISPLAY0_EXT;
        }
    }

    if (m_BottomScreenVideoStreamInfo != NULL)
    {
        /* We check that the bottom screen video is not 3D */
        if (m_BottomScreenVideoStreamInfo->m_Layout != mw::mo::moflex::LAYOUT_NONE)
        {
            NN_LOG("No 3D videos are allowed to be displayed on bottom screen ...\n");
            return false;
        }

        m_DisplayId[2] = NN_GX_DISPLAY1;
    }

    return true;
}

/*---------------------------------------------------------------------------*
  Tell to the caller (typically a mw::mo::helper::VideoConversion class)
  that the RGB buffers of the video presentation class is linear.
 *---------------------------------------------------------------------------*/

bool FbVideoPresentation::ScreenBufferLinear (void)
{
    return true;
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

void* FbVideoPresentation::GetScreenBufferPtr (int eyeIndex)
{
    if (eyeIndex == 2 && m_BottomScreenVideoStreamInfo == NULL)
        return NULL;
    if (eyeIndex == 1 && (m_TopScreenVideoStreamInfo == NULL || m_TopScreenVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_NONE))
        return NULL;

    return mw::mo::helper::displaydevice::GetBackBuffer(m_DisplayId[eyeIndex]);
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

u32 FbVideoPresentation::GetScreenStride (int eyeIndex)
{
    (void)eyeIndex; // This is to avoid compiler warning

    return 240;
}

/*---------------------------------------------------------------------------*
 Get GX display Id that is associated to the given eye index:
  - 0 value is the first frame of a left/right eye image tuple of the top 
    screen.
  - 1 value is the second frame of a left/right eye image tuple of the top 
    screen. 
  - 2 value is the bottom screen
 *---------------------------------------------------------------------------*/

u32 FbVideoPresentation::GetDisplayId (int eyeIndex)
{
    return m_DisplayId[eyeIndex];
}

/*---------------------------------------------------------------------------*
  Draw the images to the buffers. As all the job is already done by the 
  mw::mo::helper::VideoConversion class, the function does nothing.
 *---------------------------------------------------------------------------*/

void FbVideoPresentation::Draw ()
{
}

/*---------------------------------------------------------------------------*
  Present the drawn images to the screen. We only have to make a display 
  buffers flip in this case.
 *---------------------------------------------------------------------------*/

void FbVideoPresentation::Flip ()
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
