// ============================================================================
/*
 * @file gflnet2_WIN32_CecCoreMessageBox.cpp
 * @brief Win32版すれ違い通信ボックスに関する操作を行うクラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_WIN32
#include <stdio.h>
#include <gflnet2/source/cec/win32/gflnet2_WIN32_CecCoreMessageBox.h>
#include <gflnet2/include/cec/gflnet2_ICecMessageBoxEventListener.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(win32)


WIN32_CecCoreMessageBox::WIN32_CecCoreMessageBox() :
  m_pEventListener( NULL ),
  m_eCommandID( gflnet2::cec::NONE )
{
}


WIN32_CecCoreMessageBox::~WIN32_CecCoreMessageBox()
{
}


bool WIN32_CecCoreMessageBox::IsCreateMessageBox()
{
  return false;
}


void WIN32_CecCoreMessageBox::Update()
{
  if( m_pEventListener != NULL )
  {
    switch( m_eCommandID )
    {
      case gflnet2::cec::IS_CREATE_MESSAGE_BOX:
        m_pEventListener->OnIsCreateMessageBox( false );
      break;

      case gflnet2::cec::CREATE_MESSAGE_BOX:
        m_pEventListener->OnCreateMessageBoxFailure();
      break;

      case gflnet2::cec::DELETE_MESSAGE_BOX:
        m_pEventListener->OnDeleteMessageBoxFailure();
      break;
    }

    m_pEventListener = NULL;
  }
}


void WIN32_CecCoreMessageBox::IsCreateMessageBoxAsync( ICecMessageBoxEventListener* pEventListener )
{
  m_pEventListener = pEventListener;
  m_eCommandID = gflnet2::cec::IS_CREATE_MESSAGE_BOX;
}


bool WIN32_CecCoreMessageBox::CreateMessageBox( const CecMessageBoxCreateCoreParam* pCreateCoreParam )
{
  return false;
}


void WIN32_CecCoreMessageBox::CreateMessageBoxAsync( const CecMessageBoxCreateCoreParam* pCreateCoreParam, ICecMessageBoxEventListener* pEventListener )
{
  m_pEventListener = pEventListener;
  m_eCommandID = gflnet2::cec::CREATE_MESSAGE_BOX;
}


bool WIN32_CecCoreMessageBox::DeleteMessageBox()
{
  return false;
}


void WIN32_CecCoreMessageBox::DeleteMessageBoxAsync( ICecMessageBoxEventListener* pEventListener )
{
  m_pEventListener = pEventListener;
  m_eCommandID = gflnet2::cec::DELETE_MESSAGE_BOX;
}


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif