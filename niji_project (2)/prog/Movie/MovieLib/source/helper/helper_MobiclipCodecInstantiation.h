#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_MOBICLIPCODECINSTANTIATION_H__ )
#define __HELPER_MOBICLIPCODECINSTANTIATION_H__
/*---------------------------------------------------------------------------*
 File:        helper_MobiclipCodecInstantiation_h
 Description: Helper file that manages audio and video codecs instantiation.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_MobiclipCodecInstantiation_h
#define helper_MobiclipCodecInstantiation_h

/*---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/fnd.h>

/*---------------------------------------------------------------------------*/

#include "helper_CodecInstantiation.h"

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

class MobiclipCodecInstantiation : public CodecInstantiation
{
    /* If true, video codecs will be created in a way that memory allocations are
       their minimum; this can cut memory usage by up to two for decompression of 3D
       videos in interleaved mode */
    bool    m_MinimalUseOfMemory;
    int:    24; /* padding added to avoid compiler warning */
public:

    MobiclipCodecInstantiation(void) : m_MinimalUseOfMemory(false){}

    void AllowMinimalUseOfMemory    (bool enable){m_MinimalUseOfMemory = enable;}

    virtual bool CreateVideoCodec   (gfl2::heap::NwAllocator* pAllocator, 
                                     mw::mo::moflex::VideoStreamInfo& videoStreamInfo, 
                                     mw::mo::GenericVideoDecoder*& pOutVideoDecoder);
    virtual void DeleteVideoCodec   (gfl2::heap::NwAllocator* pAllocator, 
                                     mw::mo::GenericVideoDecoder* pVideoDecoder);
    virtual bool CreateAudioCodec   (gfl2::heap::NwAllocator* pAllocator, 
                                     mw::mo::moflex::AudioStreamInfo& audioStreamInfo, 
                                     mw::mo::GenericAudioDecoder*& pOutAudioDecoder);
    virtual void DeleteAudioCodec   (gfl2::heap::NwAllocator* pAllocator, 
                                     mw::mo::GenericAudioDecoder* pAudioDecoder);
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif  // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif  // ifndef helper_MobiclipCodecInstantiation_h

/*---------------------------------------------------------------------------*/

#endif // __HELPER_MOBICLIPCODECINSTANTIATION_H__
#endif// defined(GF_PLATFORM_CTR)
