#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_VideoConversion.cpp
 Description: Helper file that manages video conversion to RGB format.
              Also performs deblocking.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/fnd.h>
#include <string.h>

/*---------------------------------------------------------------------------*/

#include "filter/filter_Image.h"
#include "filter/filter_Deblocker.h"

#include "helper_VideoConversion.h"
#include "helper_YuvToRgb.h"

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

const int NB_IMAGES_FULL_DEBLOCK    = 5;
const int NB_IMAGES_MEDIUM_DEBLOCK  = 3;

/*---------------------------------------------------------------------------*
  Initialisation method, creates a deblocker object
 *---------------------------------------------------------------------------*/

bool VideoConversion::Initialize(void)
{
    if (m_Deblocker.Create() == false)
        return false;

    m_PushedConversionRequests = 0;
    m_PoppedConversionRequests = 0;
    m_ConversionStarted = false;

    return true;
}

/*---------------------------------------------------------------------------*
 Finalisation method, releases the deblocker object
 *---------------------------------------------------------------------------*/

void VideoConversion::Finalize(void)
{
    m_Deblocker.Release();
}

/*---------------------------------------------------------------------------*
 Gives the y2r standard coefficients that will be used for the conversion.
 *---------------------------------------------------------------------------*/

void VideoConversion::SetY2rCoefficients (nn::y2r::StandardCoefficient standardCoefs)
{
    m_Y2rStandardCoefficients = standardCoefs;
}

/*---------------------------------------------------------------------------*
 Tells if we want to perform YUV->RGB conversion to a linear buffer
 (like buffers used for screen display) or to a twiddled buffer (like buffers
  used for textures).
 *---------------------------------------------------------------------------*/

void VideoConversion::EnableLinearConversion (bool enable)
{
    m_DirectScreenAccessEnable = enable;
}

/*---------------------------------------------------------------------------*
 Tells if we want to perform deblocking before YUV->RGB conversion or not.
 *---------------------------------------------------------------------------*/

void VideoConversion::EnableDeblocking (bool enable)
{
    m_DeblockingEnable = enable;
}

/*---------------------------------------------------------------------------*
 Push a new YUV->RGB conversion request.
 *---------------------------------------------------------------------------*/

bool VideoConversion::PushConversion (mw::mo::GenericVideoDecoder* pVideoDecoder,
                                     mw::mo::moflex::VideoStreamInfo* pVideoStreamInfo,
                                     void* pDstBufferPtr, u32 dstBufferStride)
{
    u32 nbPendingConversion = m_PushedConversionRequests - m_PoppedConversionRequests;
    if (nbPendingConversion == MAX_CONVERSION_REQUEST)
        return false;

    if (pVideoStreamInfo->m_Width > dstBufferStride)
        return false;
    
    int nbFrames;
    if (pVideoDecoder->GetNbPushedFrame(nbFrames) == false || nbFrames == 0)
        return false;

    u32 pushIndex = m_PushedConversionRequests % MAX_CONVERSION_REQUEST;

    m_ConversionRequestQueue[pushIndex].m_pVideoDecoder = pVideoDecoder;
    m_ConversionRequestQueue[pushIndex].m_pVideoStreamInfo = pVideoStreamInfo;
    m_ConversionRequestQueue[pushIndex].m_pDstBufferPtr = pDstBufferPtr;
    m_ConversionRequestQueue[pushIndex].m_DstBufferStride = dstBufferStride;
    m_ConversionRequestQueue[pushIndex].m_ConversionEnded = false;
        
    m_PushedConversionRequests++;

    return true;
}

/*---------------------------------------------------------------------------*
 Pops the oldest finished YUV->RGB conversion request, if any.
 *---------------------------------------------------------------------------*/

bool VideoConversion::PopConversion (void)
{
    u32 nbPendingConversion = m_PushedConversionRequests - m_PoppedConversionRequests;
    if (nbPendingConversion == 0)
        return false;

    u32 popIndex = m_PoppedConversionRequests % MAX_CONVERSION_REQUEST;
    if (m_ConversionRequestQueue[popIndex].m_ConversionEnded == false)
        return false;

    m_PoppedConversionRequests++;

    return true;
}

/*---------------------------------------------------------------------------*
 Get the number of pushed color conversion requests, finished or not.
 *---------------------------------------------------------------------------*/

u32 VideoConversion::GetNbPendingConversions (void)
{
    return m_PushedConversionRequests - m_PoppedConversionRequests;
}

/*---------------------------------------------------------------------------*
 Get the number of finished color conversion requests.
 *---------------------------------------------------------------------------*/

u32 VideoConversion::GetNbConvertedImages (void)
{
    /* Retrieves the number of pending video conversion requests */
    u32 nbPendingConversion = m_PushedConversionRequests - m_PoppedConversionRequests;
    u32 nbConvertedImages = 0;

    /* Goes through all pending Video conversion requests, increments the number
       of converted images each time we find a fully converted one */
    for (u32 i=0; i<nbPendingConversion; i++)
    {
        u32 popIndex = (m_PoppedConversionRequests + i) % MAX_CONVERSION_REQUEST;
        if (m_ConversionRequestQueue[popIndex].m_ConversionEnded == true)
            nbConvertedImages++;
    }

    return nbConvertedImages;
}

/*---------------------------------------------------------------------------*
 Get the video decoder class that owns the oldest converted YUV image.
 *---------------------------------------------------------------------------*/

mw::mo::GenericVideoDecoder* VideoConversion::GetConvertedVideoDecoder (void)
{
    u32 nbPendingConversion = m_PushedConversionRequests - m_PoppedConversionRequests;
    if (nbPendingConversion == 0)
        return NULL;

    u32 popIndex = m_PoppedConversionRequests % MAX_CONVERSION_REQUEST;
    if (m_ConversionRequestQueue[popIndex].m_ConversionEnded == false)
        return NULL;

    return m_ConversionRequestQueue[popIndex].m_pVideoDecoder;
}

/*---------------------------------------------------------------------------*
 Get the video decoder class that owns the oldest converted YUV image.
 *---------------------------------------------------------------------------*/

mw::mo::moflex::VideoStreamInfo* VideoConversion::GetConvertedVideoStreamInfo (void)
{
    u32 nbPendingConversion = m_PushedConversionRequests - m_PoppedConversionRequests;
    if (nbPendingConversion == 0)
        return NULL;

    u32 popIndex = m_PoppedConversionRequests % MAX_CONVERSION_REQUEST;
    if (m_ConversionRequestQueue[popIndex].m_ConversionEnded == false)
        return NULL;

    return m_ConversionRequestQueue[popIndex].m_pVideoStreamInfo;
}

/*---------------------------------------------------------------------------*
 Get the RGB buffer pointer of the oldest converted YUV image.
 *---------------------------------------------------------------------------*/

void* VideoConversion::GetConvertedBufferPtr (void)
{
    u32 nbPendingConversion = m_PushedConversionRequests - m_PoppedConversionRequests;
    if (nbPendingConversion == 0)
        return NULL;

    u32 popIndex = m_PoppedConversionRequests % MAX_CONVERSION_REQUEST;
    if (m_ConversionRequestQueue[popIndex].m_ConversionEnded == false)
        return NULL;

    return m_ConversionRequestQueue[popIndex].m_pDstBufferPtr;
}

/*---------------------------------------------------------------------------*
 Get the RGB buffer stride, in pixels, of the oldest converted YUV image.
 *---------------------------------------------------------------------------*/

u32 VideoConversion::GetConvertedBufferStride (void)
{
    u32 nbPendingConversion = m_PushedConversionRequests - m_PoppedConversionRequests;
    if (nbPendingConversion == 0)
        return NULL;

    u32 popIndex = m_PoppedConversionRequests % MAX_CONVERSION_REQUEST;
    if (m_ConversionRequestQueue[popIndex].m_ConversionEnded == false)
        return NULL;

    return m_ConversionRequestQueue[popIndex].m_DstBufferStride;
}

/*---------------------------------------------------------------------------*
 Runs and refreshes the color conversion process.
 *---------------------------------------------------------------------------*/

bool VideoConversion::Run (void)
{
    /* Checks the number of pending video conversion requests,
       returns immediatly if there are none */

    u32 nbPendingConversion = m_PushedConversionRequests - m_PoppedConversionRequests;
    if (nbPendingConversion == 0)
        return true;
	
	double time = nn::os::Tick::GetSystemCurrent();

    /* Goes through all pending Video conversion requests, exits just after mananing
       the first conversion request that is not completed yet */
    for (u32 i=0; i<nbPendingConversion; i++)
    {
        u32 index = (m_PoppedConversionRequests+i) % MAX_CONVERSION_REQUEST;
        int pushedFrames;
        m_ConversionRequestQueue[index].m_pVideoDecoder->GetNbPushedFrame(pushedFrames);

        /* Current video conversion request not completed yet*/
        if (m_ConversionRequestQueue[index].m_ConversionEnded == false && pushedFrames > 0)
        {
            if (m_ConversionStarted == true)
            {
                /* YUV->RGB conversion has been started */
                if (mw::mo::helper::yuvtorgb::ConversionFinished() == true)
                {
                    /* YUV->RGB conversion is finished, we flag the current video 
                       conversion request as completed */
                    m_ConversionRequestQueue[index].m_ConversionEnded = true;
                    m_ConversionRequestQueue[index].m_pVideoDecoder->PopFrame();
                    m_ConversionStarted = false;
                }
                else /* YUV->RGB conversion not finished yet, we quit the Run method */
                    return true;
            }
            else
            {
                /* No YUV->RGB conversion has been started yet */

                mw::mo::filter::YuvImage    image;
                mw::mo::filter::YuvImage    deblockedImage;
                mw::mo::filter::YuvImage*    imagePtr;

                /* We compute the number of monoscopic or steroscopic (full or partial) video images pushed */
                int pushedImages;
                if (m_ConversionRequestQueue[index].m_pVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_INTERLEAVED_LEFT_EYE_FIRST ||
                    m_ConversionRequestQueue[index].m_pVideoStreamInfo->m_Layout == mw::mo::moflex::LAYOUT_INTERLEAVED_RIGHT_EYE_FIRST)
                    pushedImages = (pushedFrames+1)>>1;
                else
                    pushedImages = pushedFrames;

                /* We retrieve the YUV frame to convert to RGB, and we optionally deblock it */
                m_ConversionRequestQueue[index].m_pVideoDecoder->GetFrame(image);

                if (pushedImages >= NB_IMAGES_MEDIUM_DEBLOCK && m_DeblockingEnable == true)
                {
                    mw::mo::filter::DeblockerQuality deblockerQuality;
                    if (pushedImages >= NB_IMAGES_FULL_DEBLOCK)    
                        deblockerQuality = mw::mo::filter::DEBLOCKER_QUALITY_HIGH;
                    else
                        deblockerQuality = mw::mo::filter::DEBLOCKER_QUALITY_MEDIUM;

                    /* If something strange happened during deblocking process (like a memory 
                       allocation that failed), we quit the Run method with the value false */
                    mw::mo::filter::DeblockerReturn deblockerReturn;
                    deblockerReturn = m_Deblocker.Apply(image,deblockedImage,deblockerQuality);
                    NN_ASSERT(deblockerReturn == mw::mo::filter::DEBLOCKER_RETURN_OK);
                    if (deblockerReturn != mw::mo::filter::DEBLOCKER_RETURN_OK)
                        return false; 
                    imagePtr = &deblockedImage;
                }
                else
                    imagePtr = &image;

                /* We launch the YUV->RGB conversion process and we exit the Run method just after that */
                nn::y2r::BlockAlignment blockAlignment;
                if (m_DirectScreenAccessEnable == true)
                    blockAlignment = nn::y2r::CTR::BLOCK_LINE;
                else
                    blockAlignment = nn::y2r::CTR::BLOCK_8_BY_8;

                mw::mo::helper::yuvtorgb::LaunchConversion(imagePtr->m_Width,imagePtr->m_Height,
                                                           nn::y2r::CTR::OUTPUT_RGB_32, m_Y2rStandardCoefficients, blockAlignment,
                                                           (const u8*)imagePtr->m_YPtr, (const u8*)imagePtr->m_UPtr, (const u8*)imagePtr->m_VPtr,
                                                           imagePtr->m_YStride - imagePtr->m_Width, 
                                                           imagePtr->m_UStride - (imagePtr->m_Width>>1), 
                                                           imagePtr->m_VStride - (imagePtr->m_Width>>1),
                                                           (u8*)m_ConversionRequestQueue[index].m_pDstBufferPtr, 
                                                           m_ConversionRequestQueue[index].m_DstBufferStride - imagePtr->m_Width);            

                m_ConversionStarted = true;
                break;
            }
        }
    }
	
	double time2 = nn::os::Tick::GetSystemCurrent();
	m_time = time2 - time;

    return true;
}

/*---------------------------------------------------------------------------*
 Wait that all the color conversions are finished.
 *---------------------------------------------------------------------------*/

bool VideoConversion::WaitConversionDone (void)
{
    for(;;)
    {
        if (Run() == false)
            return false;

        if (GetNbPendingConversions() > 0 && GetNbPendingConversions() == GetNbConvertedImages())
            return true;

        mw::mo::helper::yuvtorgb::GetEndConversionEvent()->Wait();
    }
}

/*---------------------------------------------------------------------------*
 
 *---------------------------------------------------------------------------*/

void VideoConversion::PrintStatistics (void)
{
	m_Deblocker.PrintStatistics();
}

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif// defined(GF_PLATFORM_CTR)
