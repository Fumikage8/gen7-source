#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_FsReader.h
 Description: Helper file that manages reading a Moflex content using CTR FS 
              library.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <string.h>

/*---------------------------------------------------------------------------*/

#include "helper_FsReader.h"
#include "helper_Application.h"

/*---------------------------------------------------------------------------*/

using namespace mw::mo::helper;

/*---------------------------------------------------------------------------*
  Constants definition.
 *---------------------------------------------------------------------------*/

// Uncomment line below if you want to play a movie content from SD/MMC/SDHC
// This will force the reader to use a bigger read buffer.
//#define ENABLE_SD_READING

const int FS_THREAD_PRIORITY    = 4;
const int FS_THREAD_STACK_SIZE  = 4096;
const int FS_READ_SIZE          = 65536;

#ifdef ENABLE_SD_READING
const int FS_BUFFER_SIZE        = FS_READ_SIZE * 8;
#else
const int FS_BUFFER_SIZE        = FS_READ_SIZE * 4;
#endif

double FsReader::m_time = 0;

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static bool FsReaderSetPacketSize (void* pUserData, u32 packetSize)
{
    FsReader* cookie = (FsReader*)pUserData;

    cookie->m_PacketSize = packetSize;
    
    return true;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static u32 FsReaderGetPacketSize (void* pUserData)
{
    FsReader* cookie = (FsReader*)pUserData;
    
    return cookie->m_PacketSize;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static bool FsReaderGetPacketPtr (void* pUserData, const u8** packetPtr)
{
    FsReader* cookie = (FsReader*)pUserData;

    if (cookie->m_Position == cookie->m_FileSize)
        return false;

    u32 returnedSize = cookie->m_FileSize - cookie->m_Position;
    if (returnedSize > cookie->m_PacketSize)
        returnedSize = cookie->m_PacketSize;

    u32 bytesAvailableForReading = cookie->m_BufferWriteIndex - cookie->m_BufferReadIndex;
    if (bytesAvailableForReading < returnedSize)
    {
        cookie->m_ReadRequest.Signal(); // Start reading moflex data
        return false; // Don't have a full moflex packet yet
    }

    u32 readIndex = (cookie->m_BufferReadIndex % FS_BUFFER_SIZE);
    if ((readIndex + returnedSize) > FS_BUFFER_SIZE)
    {
        // The moflex packet is fragmented :
        // - one part is at the end of the read buffer 
        // - another part is at the beginning of the read buffer
        // We will copy the 2 fragments in an overflow buffer so that the caller 
        // has linear access to the full moflex packet.

        if (cookie->m_OverflowSize < cookie->m_PacketSize)
        {
            if (cookie->m_Overflow != NULL)
                cookie->m_pAllocator->Free(cookie->m_Overflow);
            cookie->m_Overflow = (u8*)cookie->m_pAllocator->Alloc(cookie->m_PacketSize, 4);
            if (cookie->m_Overflow == NULL)
                return false;

            cookie->m_OverflowSize = cookie->m_PacketSize;
        }

        int size = FS_BUFFER_SIZE - readIndex;
        memcpy(cookie->m_Overflow, cookie->m_Buffer + readIndex, size);
        memcpy(cookie->m_Overflow + size, cookie->m_Buffer, returnedSize - size);

        *packetPtr = cookie->m_Overflow;
    }
    else // The moflex packet is not fragmented, we directly return the read buffer pointer
        *packetPtr = cookie->m_Buffer + readIndex;

    return true;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static bool FsReaderPacketAddBytesRead (void* pUserData, u32 bytesRead)
{
    FsReader* cookie = (FsReader*)pUserData;

    if ((cookie->m_FileSize - cookie->m_Position) < bytesRead)
        return false;

    cookie->m_Position += bytesRead;
    cookie->m_BufferReadIndex += bytesRead;
    
    cookie->m_ReadRequest.Signal(); // Start reading moflex data

    return true;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static s64 FsReaderGetSize(void* pUserData)
{
    FsReader* cookie = (FsReader*)pUserData;

    return cookie->m_FileSize;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static s64 FsReaderGetPosition (void* pUserData)
{
    FsReader* cookie = (FsReader*)pUserData;

    return cookie->m_Position;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static bool FsReaderSetPosition (void* pUserData, s64 newPosition)
{
    FsReader* cookie = (FsReader*)pUserData;
    
    if (newPosition > cookie->m_FileSize)
        return false;

    cookie->m_FsCriticalSection.Enter();

    cookie->m_Position = newPosition;

    cookie->m_BufferReadIndex = newPosition;
    cookie->m_BufferWriteIndex = newPosition;
    cookie->m_Fs->Seek(newPosition, nn::fs::POSITION_BASE_BEGIN);

    cookie->m_FsCriticalSection.Leave();

    cookie->m_ReadRequest.Signal(); // Start reading moflex data
    
    return true;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static bool FsReaderIsEndOfData (void* pUserData)
{
    FsReader* cookie = (FsReader*)pUserData;

    return (cookie->m_Position == cookie->m_FileSize);
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static const mw::mo::moflex::Reader s_FsReader = 
{
    FsReaderSetPacketSize,
    FsReaderGetPacketSize,
    FsReaderGetPacketPtr,
    FsReaderPacketAddBytesRead,
    FsReaderGetSize,
    FsReaderGetPosition,
    FsReaderSetPosition,
    FsReaderIsEndOfData
};

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

const mw::mo::moflex::Reader* FsReader::GetMoflexReader(void)
{
    return &s_FsReader;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void FsReader::FsThreadFunc (FsReader* fsReader)
{
    for(;;)
    {
    	double time = nn::os::Tick::GetSystemCurrent();
        s32 eventIndex = nn::os::WaitObject::WaitAny(reinterpret_cast<nn::os::WaitObject**>(fsReader->m_EventArray), 2);

        if (eventIndex == 0) // Read request
        {
            fsReader->m_FsCriticalSection.Enter();

            u32 bytesAvailableForReading = fsReader->m_BufferWriteIndex - fsReader->m_BufferReadIndex;
            if ((FS_BUFFER_SIZE - bytesAvailableForReading) < FS_READ_SIZE)
            {
                fsReader->m_FsCriticalSection.Leave();
                continue;
            }

            u32 sizeToRead = fsReader->m_FileSize - fsReader->m_BufferWriteIndex;
            if (sizeToRead > FS_READ_SIZE)
                sizeToRead = FS_READ_SIZE;

            u32 sizeToEndOfBuffer = FS_BUFFER_SIZE - (fsReader->m_BufferWriteIndex % FS_BUFFER_SIZE);
            if (sizeToRead > sizeToEndOfBuffer)
                sizeToRead = sizeToEndOfBuffer;

//            mw::mo::helper::application::GetSleepCriticalSection().Enter();
            u32 size = (u32) fsReader->m_Fs->Read(fsReader->m_Buffer + (fsReader->m_BufferWriteIndex % FS_BUFFER_SIZE), sizeToRead);
//            mw::mo::helper::application::GetSleepCriticalSection().Leave();

            if (size != sizeToRead)
            {
                NN_PANIC("FsReader error: should read %d bytes, %d bytes read.\n",sizeToRead,size);
            }
            
            fsReader->m_BufferWriteIndex += size;
            fsReader->m_FsCriticalSection.Leave();
        }
        else //End of thread request
            break;
    	
    	double time2 = nn::os::Tick::GetSystemCurrent();
    	m_time = time2 - time;
    }
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/
static nn::os::StackMemoryBlock s_stack;
bool FsReader::Initialize (nn::fs::FileInputStream* fs, gfl2::heap::NwAllocator* pAllocator)
{
    m_Fs = fs;
    m_FileSize = (u32)fs->GetSize();
    m_pAllocator = pAllocator;

    m_Buffer = (u8*)m_pAllocator->Alloc(FS_BUFFER_SIZE, 4);
    m_BufferReadIndex = 0;
    m_BufferWriteIndex = 0;
    m_Position = 0;

    m_FsCriticalSection.Initialize();
    m_ReadRequest.Initialize(false);
    m_EndRequest.Initialize(false);
    m_EventArray[0] = &m_ReadRequest;
    m_EventArray[1] = &m_EndRequest;
            
	s_stack.Initialize(FS_THREAD_STACK_SIZE);
	m_FsThread.Start(FsThreadFunc, this, s_stack, FS_THREAD_PRIORITY);
    //m_FsThread.StartUsingAutoStack(FsThreadFunc, this, FS_THREAD_STACK_SIZE, FS_THREAD_PRIORITY);
    m_ReadRequest.Signal(); // Start reading moflex data

    return true;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void FsReader::Finalize (void)
{
    if (m_Fs == NULL)
        return;

    m_EndRequest.Signal();
    m_FsThread.Join();
    m_FsThread.Finalize();
	s_stack.Finalize();

    m_FsCriticalSection.Finalize();
    m_ReadRequest.Finalize();
    m_EndRequest.Finalize();

    if (m_Buffer != NULL)
    {
        m_pAllocator->Free(m_Buffer);
        m_Buffer = NULL;
    }

    if (m_Overflow != NULL)
    {
        m_pAllocator->Free(m_Overflow);
        m_Overflow = NULL;
    }

    m_OverflowSize = 0;    
    m_BufferReadIndex = 0;
    m_BufferWriteIndex = 0;
    m_Position = 0;

    m_Fs = NULL;
    m_pAllocator = NULL;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
