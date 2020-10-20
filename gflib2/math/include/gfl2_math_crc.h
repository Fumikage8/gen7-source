#ifndef __GFL_MATH_CRC_H__
#define __GFL_MATH_CRC_H__
//=============================================================================
/**
 * @file	  gfl_math_crc.h
 * @brief	  計算クラス
 * @author  k.ohno
 * @date	  2014.1.16
 */
//=============================================================================
#pragma once

#include "macro/include/gfl2_Macros.h"
#include "gfl2_math_control.h"

namespace gfl2{
namespace math {

class Crc
{
  GFL_FORBID_COPY_AND_ASSIGN(Crc);
  /* RFC 1952 CRC */

private:

  /* Table of CRCs of all 8-bit messages. */
  static unsigned long crc_table[256];
  
  static unsigned short crc16table[256];

private:
  static unsigned long update_crc(unsigned long crc, unsigned char *buf, int len);
  static void make_crc_table(void);


  
public:
  
  Crc(void);
  virtual  ~Crc(){};

  static void Initialize(void);
  //---------------------------------------------------------------------------------
  /**
   * @brief  32bit CRCを得る
   * @param  buf  計算するバッファの先頭
   * @param  len  バッファの長さ
   */
  //---------------------------------------------------------------------------------
  static u32 Crc32(unsigned char *buf, int len);

  //---------------------------------------------------------------------------------
  /**
   * @brief  16bit CRCを得る
   * @param  buf  計算するバッファの先頭
   * @param  len  バッファの長さ
   */
  //---------------------------------------------------------------------------------
  static u16 Crc16(u16 crc, u8 const *ptr, int len);


};

};  //  Math
};  //gfl

#endif // __GFL_MATH_CRC_H__
