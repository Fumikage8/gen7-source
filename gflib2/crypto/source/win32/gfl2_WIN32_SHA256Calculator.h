// ============================================================================
/*
 * @file gfl2_WIN32_SHA256Calculator.h
 * @brief shaによるハッシュ値を計算するクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#ifdef GF_PLATFORM_WIN32
#if !defined( GFL2_WIN32_SHA256CALCULATOR_H_INCLUDE )
#define GFL2_WIN32_SHA256CALCULATOR_H_INCLUDE
#pragma once

#include <crypto/include/gfl2_ISHA256Calculator.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(win32)


class WIN32_SHA256Calculator : public gfl2::crypto::ISHA256Calculator
{
  GFL_FORBID_COPY_AND_ASSIGN( WIN32_SHA256Calculator );
public:
  WIN32_SHA256Calculator();
  virtual~WIN32_SHA256Calculator();
  virtual void Update();
  virtual bool IsBusy();
  virtual bool CalcHashAsync( ISHA256CalcEventListener* pEventListener, const void* pData, u32 dataSize );
  virtual gfl2::crypto::SHA256Hash* CalcHash( const void* pData, u32 dataSize );
};


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_WIN32_SHA256CALCULATOR_H_INCLUDE
#endif // GF_PLATFORM_WIN32
