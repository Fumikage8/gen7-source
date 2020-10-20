#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_RAMREADER_H__ )
#define __HELPER_RAMREADER_H__
/*---------------------------------------------------------------------------*
 File:          helper_RamReader.h
 Description: Helper file that manages reading a Moflex content from RAM.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_RamReader_h
#define helper_RamReader_h

/*---------------------------------------------------------------------------*/

#include <nn.h>
#include "moflex/moflex_Demuxer.h"

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

struct RamReader
{
    u8* m_Buffer;
    u32 m_BufferSize;
    u32 m_Position;
    u32 m_PacketSize;
    
    RamReader(u8* buffer, u32 bufferSize, u32 packetSize) : m_Buffer(buffer), 
                                                            m_BufferSize(bufferSize), 
                                                            m_Position(0), 
                                                            m_PacketSize(packetSize){}
    
    void*                         GetCookie       (void){return this;}
    const mw::mo::moflex::Reader* GetMoflexReader (void);
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif // ifndef helper_RamReader_h

/*---------------------------------------------------------------------------*/
#endif // __HELPER_RAMREADER_H__
#endif// defined(GF_PLATFORM_CTR)
