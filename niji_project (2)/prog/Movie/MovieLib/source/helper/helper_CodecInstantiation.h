#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_CODECINSTANTIATION_H__ )
#define __HELPER_CODECINSTANTIATION_H__
/*---------------------------------------------------------------------------*
 File:        helper_CodecInstantiation_h
 Description: Helper file that exposes a generic interface for audio and 
              video codecs instantiation.
              
              All 4 virtual methods must be implemented by derived classes.
              See helper_MobiclipCodecInstantiation.h and.cpp files for an
              implementation example.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_CodecInstantiation_h
#define helper_CodecInstantiation_h

/*---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/fnd.h>

//#include <gflib.h>
#include "heap/include/gfl2_Heap.h"
#include "heap/include/gfl2_NwAllocator.h"
/*---------------------------------------------------------------------------*/

#include "mo_GenericVideoDecoder.h"
#include "mo_GenericAudioDecoder.h"

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

class CodecInstantiation
{
public:

    virtual bool CreateVideoCodec (gfl2::heap::NwAllocator* pAllocator, 
                                   mw::mo::moflex::VideoStreamInfo& videoStreamInfo, 
                                   mw::mo::GenericVideoDecoder*& pOutVideoDecoder) = 0;
    virtual void DeleteVideoCodec (gfl2::heap::NwAllocator* pAllocator, 
                                   mw::mo::GenericVideoDecoder* pVideoDecoder) = 0;
    virtual bool CreateAudioCodec (gfl2::heap::NwAllocator* pAllocator, 
                                   mw::mo::moflex::AudioStreamInfo& audioStreamInfo, 
                                   mw::mo::GenericAudioDecoder*& pOutAudioDecoder) = 0;
    virtual void DeleteAudioCodec (gfl2::heap::NwAllocator* pAllocator, 
                                   mw::mo::GenericAudioDecoder* pAudioDecoder) = 0;
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif  // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif  // ifndef helper_CodecInstantiation_h

/*---------------------------------------------------------------------------*/

#endif // __HELPER_CODECINSTANTIATION_H__
#endif// defined(GF_PLATFORM_CTR)
