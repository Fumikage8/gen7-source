#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_FSREADER_H__ )
#define __HELPER_FSREADER_H__
/*---------------------------------------------------------------------------*
 File:        helper_FsReader.h
 Description: Helper file that manages reading a Moflex content using CTR FS 
              library.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_FsReader_h
#define helper_FsReader_h

/*---------------------------------------------------------------------------*/

#include <nn/fs.h>
#include <nn/fnd.h>
#include "moflex/moflex_Demuxer.h"

//#include <gflib.h>
#include "heap/include/gfl2_Heap.h"
#include "heap/include/gfl2_NwAllocator.h"
/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

class FsReader
{
private:
        
    static void FsThreadFunc (FsReader* fsReader);

public:

    vs64 m_BufferReadIndex;
    vs64 m_BufferWriteIndex;
    s64  m_FileSize;
    s64  m_Position;
    u8*  m_Buffer;
    u8*  m_Overflow;
    u32  m_OverflowSize;
    u32  m_PacketSize;
    
    nn::os::Thread           m_FsThread;
    nn::fs::FileInputStream* m_Fs;
    gfl2::heap::NwAllocator*     m_pAllocator;
    nn::os::CriticalSection  m_FsCriticalSection;
    nn::os::Event            m_ReadRequest;
    nn::os::Event            m_EndRequest;
    nn::os::Event*           m_EventArray[2];
	static double m_time;
    
    int: 32; // padding added to avoid compiler warning
        
    FsReader() : m_BufferReadIndex(0), m_BufferWriteIndex(0), m_FileSize(0),
                 m_Buffer(NULL), m_Overflow(NULL), m_OverflowSize(0), 
                 m_PacketSize(4096), m_Fs(NULL), m_pAllocator(NULL)
                 {}

    ~FsReader(){Finalize();}

    bool Initialize (nn::fs::FileInputStream* fs, gfl2::heap::NwAllocator* pAllocator);
    void Finalize   (void);
    
    void*                         GetCookie       (void){return this;}
    const mw::mo::moflex::Reader* GetMoflexReader (void);
	
	double GetTime(){return m_time;}
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif // ifndef helper_FsReader_h

/*---------------------------------------------------------------------------*/
#endif // __HELPER_FSREADER_H__
#endif// defined(GF_PLATFORM_CTR)
