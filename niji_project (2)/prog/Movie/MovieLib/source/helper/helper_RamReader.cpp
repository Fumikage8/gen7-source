#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_RamReader.h
 Description: Helper file that manages reading a Moflex content from RAM.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <string.h>

/*---------------------------------------------------------------------------*/

#include "helper_RamReader.h"

/*---------------------------------------------------------------------------*/

using namespace mw::mo::helper;

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static bool RamReaderSetPacketSize (void* pUserData, u32 packetSize)
{
    RamReader* cookie = (RamReader*)pUserData;
    if (cookie->m_BufferSize - cookie->m_Position < packetSize)
        return false;

    cookie->m_PacketSize = packetSize;
    
    return true;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static u32 RamReaderGetPacketSize (void* pUserData)
{
    RamReader* cookie = (RamReader*)pUserData;
    
    return cookie->m_PacketSize;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static bool RamReaderGetPacketPtr (void* pUserData, const u8** packetPtr)
{
    RamReader* cookie = (RamReader*)pUserData;

    if (cookie->m_BufferSize - cookie->m_Position == 0)
        return false;
    
    *packetPtr = cookie->m_Buffer + cookie->m_Position;

    return true;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static bool RamReaderPacketAddBytesRead (void* pUserData, u32 bytesRead)
{
    RamReader* cookie = (RamReader*)pUserData;

    if (cookie->m_BufferSize - cookie->m_Position < bytesRead)
        return false;
    
    cookie->m_Position += bytesRead;

    return true;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static s64 RamReaderGetSize(void* pUserData)
{
    RamReader* cookie = (RamReader*)pUserData;

    return (s64)cookie->m_BufferSize;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static s64 RamReaderGetPosition (void* pUserData)
{
    RamReader* cookie = (RamReader*)pUserData;

    return (s64)cookie->m_Position;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static bool RamReaderSetPosition (void* pUserData, s64 newPosition)
{
    RamReader* cookie = (RamReader*)pUserData;
    
    if (newPosition > cookie->m_BufferSize)
        return false;

    cookie->m_Position = (u32)newPosition;

    return true;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static bool RamReaderIsEndOfData (void* pUserData)
{
    RamReader* cookie = (RamReader*)pUserData;

    return (cookie->m_Position == cookie->m_BufferSize);
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static const mw::mo::moflex::Reader s_RamReader = 
{
    RamReaderSetPacketSize,
    RamReaderGetPacketSize,
    RamReaderGetPacketPtr,
    RamReaderPacketAddBytesRead,
    RamReaderGetSize,
    RamReaderGetPosition,
    RamReaderSetPosition,
    RamReaderIsEndOfData
};

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

const mw::mo::moflex::Reader* RamReader::GetMoflexReader(void)
{
    return &s_RamReader;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

#endif// defined(GF_PLATFORM_CTR)
