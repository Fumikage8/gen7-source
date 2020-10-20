// ============================================================================
/*
 * @file gflnet2_WIN32_CecSystem.h
 * @brief Win32版すれ違い通信行う為のクラスです。
 * @date 2015.02.13
 */
// ============================================================================
#ifdef  GF_PLATFORM_WIN32
#if !defined( GFLNET2_WIN32_CECSYSTEM_H_INCLUDE )
#define GFLNET2_WIN32_CECSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gflnet2/include/cec/gflnet2_ICecSystem.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(win32)

class WIN32_CecCoreMessageBox;
class WIN32_CecSendMessageBox;
class WIN32_CecRecvMessageBox;


class WIN32_CecSystem : public ICecSystem
{
  GFL_FORBID_COPY_AND_ASSIGN( WIN32_CecSystem );

public:
  WIN32_CecSystem();
  virtual~WIN32_CecSystem();

  virtual void Initialize( gfl2::heap::HeapBase* pHeap, const gflnet2::cec::CecSystemInitParam* pInitParam );
  virtual void Terminate();
  virtual void Update();
  virtual ICecCoreMessageBox* GetCecCoreMessageBox();
  virtual ICecSendMessageBox* GetCecSendMessageBox();
  virtual ICecRecvMessageBox* GetCecRecvMessageBox();

private:
  gflnet2::cec::win32::WIN32_CecCoreMessageBox*    m_pCecCoreMessageBox;
  gflnet2::cec::win32::WIN32_CecSendMessageBox*    m_pCecSendMessageBox;
  gflnet2::cec::win32::WIN32_CecRecvMessageBox*    m_pCecRecvMessageBox;

};


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_WIN32_CECSYSTEM_H_INCLUDE
#endif
