#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_YUVTORGB_H__ )
#define __HELPER_YUVTORGB_H__
/*---------------------------------------------------------------------------*
 File:        helper_YuvToRgb.h
 Description: Helper file that manages YUV to RGB conversion using 
              CTR nn::y2r API.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_YuvToRgb_h
#define helper_YuvToRgb_h

/*---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/types.h>
#include <nn/os.h>
#include <nn/y2r.h>

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

namespace yuvtorgb
{
    void Initialize (void);
    void Finalize   (void);

    void LaunchConversion (const int width, const int height, 
                           const nn::y2r::CTR::OutputFormat outputFormat,
                           const nn::y2r::CTR::StandardCoefficient standardCoefficient,
                           const nn::y2r::CTR::BlockAlignment blockAlignent,    
                           const u8* inYBuffer, const u8* inUBuffer, const u8* inVBuffer, 
                           const int inYStride, const int inUStride, const int inVStride,
                           u8* outRgbBuffer, const int outStride = 0);

    void LaunchConversion (const int width, const int height, 
                           const nn::y2r::CTR::OutputFormat outputFormat,
                           const nn::y2r::CTR::StandardCoefficient standardCoefficient,
                           const u8* inYuvBuffer, u8* outRgbBuffer, const int outStride=0);

    bool           ConversionFinished    (void);
    nn::os::Event* GetEndConversionEvent (void);
}  // end of namespace yuvtorgb

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

#endif // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif // ifndef helper_YuvToRgb.h

/*---------------------------------------------------------------------------*/
#endif // __HELPER_YUVTORGB_H__
#endif// defined(GF_PLATFORM_CTR)
