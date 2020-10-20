#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_VIDEOCONVERSION_H__ )
#define __HELPER_VIDEOCONVERSION_H__
/*---------------------------------------------------------------------------*
 File:        helper_VideoConversion_h
 Description: Helper file that manages video conversion to RGB format.
              Also performs deblocking.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_VideoConversion_h
#define helper_VideoConversion_h

/*---------------------------------------------------------------------------*/

#include <nn/types.h>
#include <nn/fnd.h>
#include <nn/y2r.h>

/*---------------------------------------------------------------------------*/

#include "mo_GenericVideoDecoder.h"
#include "moflex/moflex_Demuxer.h"
#include "filter/filter_Deblocker.h"

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/
/* This is the maximum number of color conversion requests that can be       */
/* pushed at once                                                            */
/*---------------------------------------------------------------------------*/

const int MAX_CONVERSION_REQUEST    = 4; 

/*---------------------------------------------------------------------------*/
/* This stucture holds all needed color conversion request parameters        */
/*---------------------------------------------------------------------------*/

struct ConversionRequest
{
    mw::mo::GenericVideoDecoder*     m_pVideoDecoder;
    mw::mo::moflex::VideoStreamInfo* m_pVideoStreamInfo;
    void*                            m_pDstBufferPtr;
    u32                              m_DstBufferStride;
    volatile bool                    m_ConversionEnded;
    int:                             24; // padding added to avoid compiler warning
};

/*---------------------------------------------------------------------------*/
/* The color conversion class                                                */
/*---------------------------------------------------------------------------*/

class VideoConversion
{
    mw::mo::filter::Deblocker           m_Deblocker;
    nn::y2r::CTR::StandardCoefficient   m_Y2rStandardCoefficients;
    bool                                m_DirectScreenAccessEnable;
    bool                                m_DeblockingEnable;
    bool                                m_ConversionStarted;
    ConversionRequest                   m_ConversionRequestQueue[MAX_CONVERSION_REQUEST];
    vu32                                m_PushedConversionRequests;
    vu32                                m_PoppedConversionRequests;
	double m_time;

public:

    VideoConversion() : m_Y2rStandardCoefficients(nn::y2r::COEFFICIENT_ITU_R_BT_601_SCALING), 
                        m_DirectScreenAccessEnable(false), 
                        m_DeblockingEnable(true){}
    ~VideoConversion() {Finalize();}

    bool Initialize (void);
    void Finalize   (void);
    
    void SetY2rCoefficients     (nn::y2r::StandardCoefficient standardCoefs);
    void EnableLinearConversion (bool enable);
    void EnableDeblocking       (bool enable);

    bool Run (void);

    bool PushConversion     (mw::mo::GenericVideoDecoder* pVideoDecoder,
                             mw::mo::moflex::VideoStreamInfo* pVideoStreamInfo,
                             void* pDstBufferPtr, u32 dstBufferStride);
    bool PopConversion      (void);
    bool WaitConversionDone (void);

	void PrintStatistics	(void);

    mw::mo::GenericVideoDecoder*     GetConvertedVideoDecoder    (void);
    mw::mo::moflex::VideoStreamInfo* GetConvertedVideoStreamInfo (void);
    void*                            GetConvertedBufferPtr       (void);
    u32                              GetConvertedBufferStride    (void);
    u32                              GetNbPendingConversions     (void);
    u32                              GetNbConvertedImages        (void);
	double GetTime(){return m_time;}
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif  // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif  // ifndef helper_VideoConversion.h

/*---------------------------------------------------------------------------*/

#endif // __HELPER_VIDEOCONVERSION_H__
#endif// defined(GF_PLATFORM_CTR)
