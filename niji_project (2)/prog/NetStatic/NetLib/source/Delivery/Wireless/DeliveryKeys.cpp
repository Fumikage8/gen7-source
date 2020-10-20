﻿//==============================================================================
/**
 * @file   DeliveryKeys.cpp
 * @date   2016/07/05 Tue. 16:06:08
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./DeliveryKeys.h"



// =============================================================================
/**/
//==============================================================================
namespace
{
  static const u8 publicKey[] =
  {
    0x30,0x7C,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x01,0x05,
    0x00,0x03,0x6B,0x00,0x30,0x68,0x02,0x61,0x00,0xB7,0x56,0xE1,0xDC,0xD8,0xCE,0xCE,
    0x78,0xE1,0x48,0x10,0x7B,0x1B,0xAC,0x11,0x5F,0xDB,0x17,0xDE,0x84,0x34,0x53,0xCA,
    0xB7,0xD4,0xE6,0xDF,0x8D,0xD2,0x1F,0x5A,0x3D,0x17,0xB4,0x47,0x7A,0x8A,0x53,0x1D,
    0x97,0xD5,0x7E,0xB5,0x58,0xF0,0xD5,0x8A,0x4A,0xF5,0xBF,0xAD,0xDD,0xA4,0xA0,0xBC,
    0x1D,0xC2,0x2F,0xF8,0x75,0x76,0xC7,0x26,0x8B,0x94,0x28,0x19,0xD4,0xC8,0x3F,0x78,
    0xE1,0xEE,0x92,0xD4,0x06,0x66,0x2F,0x4E,0x68,0x47,0x1E,0x4D,0xE8,0x33,0xE5,0x12,
    0x6C,0x32,0xEB,0x63,0xA8,0x68,0x34,0x5D,0x1D,0x02,0x03,0x01,0x00,0x01,
  };
  static const u32  publicKeyBytes  = sizeof(publicKey);


#if PM_DEBUG
  static const u8 secretKey[] =
  {
    0x30,0x82,0x01,0xC9,0x02,0x01,0x00,0x02,0x61,0x00,0xB7,0x56,0xE1,0xDC,0xD8,0xCE,
    0xCE,0x78,0xE1,0x48,0x10,0x7B,0x1B,0xAC,0x11,0x5F,0xDB,0x17,0xDE,0x84,0x34,0x53,
    0xCA,0xB7,0xD4,0xE6,0xDF,0x8D,0xD2,0x1F,0x5A,0x3D,0x17,0xB4,0x47,0x7A,0x8A,0x53,
    0x1D,0x97,0xD5,0x7E,0xB5,0x58,0xF0,0xD5,0x8A,0x4A,0xF5,0xBF,0xAD,0xDD,0xA4,0xA0,
    0xBC,0x1D,0xC2,0x2F,0xF8,0x75,0x76,0xC7,0x26,0x8B,0x94,0x28,0x19,0xD4,0xC8,0x3F,
    0x78,0xE1,0xEE,0x92,0xD4,0x06,0x66,0x2F,0x4E,0x68,0x47,0x1E,0x4D,0xE8,0x33,0xE5,
    0x12,0x6C,0x32,0xEB,0x63,0xA8,0x68,0x34,0x5D,0x1D,0x02,0x03,0x01,0x00,0x01,0x02,
    0x60,0x76,0xF0,0xE9,0xA4,0xD2,0x0F,0x10,0xCA,0x41,0x34,0xF4,0x64,0x39,0xA6,0x9E,
    0xBA,0xF6,0x33,0x7D,0x0B,0x7D,0xBB,0x22,0x97,0xB2,0x8A,0x79,0x5B,0xD3,0x86,0xBD,
    0x8F,0x46,0xD2,0x91,0x50,0xC1,0x6E,0x7C,0x93,0x65,0x51,0x47,0xAB,0x44,0xCA,0xF4,
    0x64,0xE6,0x01,0x59,0x6E,0xAD,0x55,0xCB,0xC8,0x48,0x0B,0x74,0xE2,0xF7,0xB2,0x94,
    0xB4,0xFD,0x15,0xDA,0xCB,0xC0,0x07,0x5D,0xAF,0x35,0x69,0xB7,0x5A,0x36,0x99,0x50,
    0x66,0x35,0x9A,0x91,0xB2,0xA9,0x74,0x47,0xF5,0x70,0x75,0xD7,0x78,0xE1,0x05,0x84,
    0x6D,0x02,0x31,0x00,0xD9,0x91,0xCF,0xA8,0xF9,0x41,0x6C,0xCB,0x5B,0x72,0xB3,0x73,
    0xE8,0x6B,0x6F,0x07,0x0F,0x9B,0x02,0x06,0x59,0x1F,0x9D,0xE3,0x58,0x3A,0x27,0x18,
    0x96,0x70,0x54,0xCC,0x57,0x7C,0x44,0x0F,0x9A,0x84,0xC5,0x8B,0xBC,0x03,0x5D,0x92,
    0x07,0x56,0xC5,0x43,0x02,0x31,0x00,0xD7,0xB9,0x3A,0xEF,0xD6,0x5D,0xC4,0x8A,0x64,
    0x8C,0x99,0xEB,0x5D,0x5F,0x86,0x9A,0xCA,0x89,0x1B,0x93,0xF6,0xCA,0xA8,0x87,0x2B,
    0x78,0xD1,0x52,0x2F,0x13,0x42,0x3A,0x09,0xE4,0x74,0xA4,0x2D,0xA7,0x13,0x7A,0xA9,
    0xB1,0x5B,0xC1,0xF2,0x7E,0xFE,0x1F,0x02,0x30,0x74,0xEA,0xA8,0x5F,0x33,0x62,0xF7,
    0x25,0xE8,0xC4,0xA0,0xAE,0xBE,0xCD,0x4C,0xE2,0xCC,0x7E,0x0D,0xFE,0x7F,0x52,0x40,
    0x34,0x75,0xAA,0xFA,0x6B,0x7A,0x1C,0x6F,0x55,0x48,0x06,0x1D,0x45,0x0D,0x52,0x20,
    0x1E,0x51,0x30,0xB5,0xCD,0xCD,0x67,0xD1,0xA5,0x02,0x30,0x05,0xEE,0x77,0xEA,0x64,
    0x50,0x8B,0xB8,0xEA,0x6E,0xDB,0x44,0x97,0x7A,0xAA,0xEE,0xD2,0xA6,0xD6,0x75,0x49,
    0x11,0xE5,0x74,0x62,0x03,0x9E,0x6D,0xCE,0x11,0x8F,0x7E,0x9D,0x0B,0x05,0x12,0x06,
    0x97,0x53,0x77,0xC9,0xF6,0xB0,0x79,0x32,0xBB,0xBF,0x2D,0x02,0x30,0x7E,0x0A,0x80,
    0x09,0x11,0x69,0xDD,0x7A,0x8E,0x6B,0x52,0x35,0x34,0xED,0x92,0xB5,0x5B,0xED,0x21,
    0xD5,0x79,0xA4,0x64,0xC4,0x05,0xAA,0x5B,0x82,0xDC,0xBF,0x04,0x04,0x6A,0xD7,0x02,
    0x12,0xB0,0xBB,0xBF,0x5A,0x25,0xD7,0xF5,0x9F,0xE6,0x2A,0x59,0xE5,
  };
#endif

  static const u32  secretKeyBytes  =
#if PM_DEBUG
    sizeof(secretKey);
#else
    0;
#endif

}




namespace NetLib    {
namespace Delivery  {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
const u8*   DeliveryKeys::PublicKey(void)
{
  return  publicKey;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32     DeliveryKeys::PublicKeyBytes(void)
{
  return  publicKeyBytes;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
const u8*   DeliveryKeys::SecretKey(void)
{
#if PM_DEBUG
  return secretKey;
#else
  return NULL;
#endif
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32     DeliveryKeys::SecretKeyBytes(void)
{
  return secretKeyBytes;
}


} /*  namespace Delivery  */
} /*  namespace NetLib    */
