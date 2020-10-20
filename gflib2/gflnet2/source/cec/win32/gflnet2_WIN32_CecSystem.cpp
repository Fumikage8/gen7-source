// ============================================================================
/*
 * @file gflnet2_WIN32_CecSystem.cpp
 * @brief Win32版すれ違い通信行う為のクラスです。
 * @date 2015.02.13
 */
// ============================================================================
#ifdef  GF_PLATFORM_WIN32
#include <gflnet2/source/cec/win32/gflnet2_WIN32_CecSystem.h>
#include <gflnet2/source/cec/win32/gflnet2_WIN32_CecCoreMessageBox.h>
#include <gflnet2/source/cec/win32/gflnet2_WIN32_CecSendMessageBox.h>
#include <gflnet2/source/cec/win32/gflnet2_WIN32_CecRecvMessageBox.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(win32)


WIN32_CecSystem::WIN32_CecSystem() : 
  m_pCecCoreMessageBox( NULL ),
  m_pCecSendMessageBox( NULL ),
  m_pCecRecvMessageBox( NULL )
{
}


WIN32_CecSystem::~WIN32_CecSystem()
{
}


void WIN32_CecSystem::Initialize( gfl2::heap::HeapBase* pHeap, const gflnet2::cec::CecSystemInitParam* pInitParam )
{
  m_pCecCoreMessageBox = GFL_NEW( pHeap ) gflnet2::cec::win32::WIN32_CecCoreMessageBox();
  m_pCecSendMessageBox = GFL_NEW( pHeap ) gflnet2::cec::win32::WIN32_CecSendMessageBox();
  m_pCecRecvMessageBox = GFL_NEW( pHeap ) gflnet2::cec::win32::WIN32_CecRecvMessageBox();
}


void WIN32_CecSystem::Terminate()
{
  GFL_SAFE_DELETE( m_pCecRecvMessageBox );
  GFL_SAFE_DELETE( m_pCecSendMessageBox );
  GFL_SAFE_DELETE( m_pCecCoreMessageBox );
}


void WIN32_CecSystem::Update()
{
  m_pCecCoreMessageBox->Update();
  m_pCecSendMessageBox->Update();
  m_pCecRecvMessageBox->Update();
}


ICecCoreMessageBox* WIN32_CecSystem::GetCecCoreMessageBox()
{
  return m_pCecCoreMessageBox;
}


ICecSendMessageBox* WIN32_CecSystem::GetCecSendMessageBox()
{
  return m_pCecSendMessageBox;
}


ICecRecvMessageBox* WIN32_CecSystem::GetCecRecvMessageBox()
{
  return m_pCecRecvMessageBox;
}


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif
