// ============================================================================
/*
 * @file gfl2_WIN32_SHA256Calculator.cpp
 * @brief AES暗号方式による暗号化、復号化を行うクラスです。
 * @date 2015.03.11
 */
// ============================================================================
#ifdef GF_PLATFORM_WIN32
#include <crypto/source/win32/gfl2_WIN32_SHA256Calculator.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)
GFL_NAMESPACE_BEGIN(win32)


WIN32_SHA256Calculator::WIN32_SHA256Calculator()
{
}


WIN32_SHA256Calculator::~WIN32_SHA256Calculator()
{
}


void WIN32_SHA256Calculator::Update()
{
}


bool WIN32_SHA256Calculator::IsBusy()
{
  return false;
}


bool WIN32_SHA256Calculator::CalcHashAsync( ISHA256CalcEventListener* pEventListener, const void* pData, u32 dataSize )
{
  return false;
}


gfl2::crypto::SHA256Hash* WIN32_SHA256Calculator::CalcHash( const void* pData, u32 dataSize )
{
  return NULL;
}


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_WIN32
