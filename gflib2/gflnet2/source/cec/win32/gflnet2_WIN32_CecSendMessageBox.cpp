// ============================================================================
/*
 * @file gflnet2_WIN32_CecSendMessageBox.cpp
 * @brief Win32版すれ違い通信の送信ボックスに関する操作を行うクラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_WIN32
#include <stdio.h>
#include <gflnet2/source/cec/win32/gflnet2_WIN32_CecSendMessageBox.h>
#include <gflnet2/include/cec/gflnet2_ICecMessageBoxEventListener.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(win32)


WIN32_CecSendMessageBox::WIN32_CecSendMessageBox() :
  m_pEventListener( NULL ),
  m_eCommandID( gflnet2::cec::NONE )
{
}


WIN32_CecSendMessageBox::~WIN32_CecSendMessageBox()
{
}


void WIN32_CecSendMessageBox::Update()
{
  if( m_pEventListener != NULL )
  {
    switch( m_eCommandID )
    {
      case gflnet2::cec::DELETE_ALL_MESSAGE:
        m_pEventListener->OnDeleteAllMessageFailure( gflnet2::cec::SEND_BOX );
      break;

      case gflnet2::cec::WRITE_MESSAGE:
        m_pEventListener->OnWriteMessageFailure();
      break;
    }

    m_pEventListener = NULL;
  }
}


bool WIN32_CecSendMessageBox::DeleteAllMessage()
{
  return false;
}


void WIN32_CecSendMessageBox::DeleteAllMessageAsync( ICecMessageBoxEventListener* pEventListener )
{
  m_pEventListener = pEventListener;
  m_eCommandID = gflnet2::cec::DELETE_ALL_MESSAGE;
}


bool WIN32_CecSendMessageBox::WriteMessage( const CecSendPacket* pSendPacket )
{
  return false;
}


void WIN32_CecSendMessageBox::WriteMessageAsync( const CecSendPacket* pSendPacket, ICecMessageBoxEventListener* pEventListener )
{
  m_pEventListener = pEventListener;
  m_eCommandID = gflnet2::cec::WRITE_MESSAGE;
}


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif