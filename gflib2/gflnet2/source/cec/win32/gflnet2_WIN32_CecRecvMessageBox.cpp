// ============================================================================
/*
 * @file gflnet2_WIN32_CecRecvMessageBox.cpp
 * @brief Win32版すれ違い通信の受信ボックスに関する操作を行うクラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_WIN32
#include <stdio.h>
#include <gflnet2/source/cec/win32/gflnet2_WIN32_CecRecvMessageBox.h>
#include <gflnet2/include/cec/gflnet2_ICecMessageBoxEventListener.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(win32)


WIN32_CecRecvMessageBox::WIN32_CecRecvMessageBox() :
  m_pEventListener( NULL ),
  m_eCommandID( gflnet2::cec::NONE )
{
}


WIN32_CecRecvMessageBox::~WIN32_CecRecvMessageBox()
{
}


void WIN32_CecRecvMessageBox::Update()
{
  if( m_pEventListener != NULL )
  {
    switch( m_eCommandID )
    {
      case gflnet2::cec::GET_MESSAGE_COUNT:
        m_pEventListener->OnGetMessageCountFailure();
      break;

      case gflnet2::cec::READ_MESSAGE:
        m_pEventListener->OnReadMessageFailure();
      break;

      case gflnet2::cec::DELETE_ALL_MESSAGE:
        m_pEventListener->OnDeleteAllMessageFailure( gflnet2::cec::RECV_BOX );
      break;
    }

    m_pEventListener = NULL;
  }
}


s32 WIN32_CecRecvMessageBox::GetMessageCount()
{
  return 0;
}


void WIN32_CecRecvMessageBox::GetMessageCountAsync( ICecMessageBoxEventListener* pEventListener )
{
  m_pEventListener = pEventListener;
  m_eCommandID = gflnet2::cec::GET_MESSAGE_COUNT;
}


const gflnet2::cec::CecRecvPacket* WIN32_CecRecvMessageBox::ReadMessage( u32& recvPacketCount )
{
  recvPacketCount = 0;
  return NULL;
}


void WIN32_CecRecvMessageBox::ReadMessageAsync( ICecMessageBoxEventListener* pEventListener )
{
  m_pEventListener = pEventListener;
  m_eCommandID = gflnet2::cec::READ_MESSAGE;
}


bool WIN32_CecRecvMessageBox::DeleteAllMessage()
{
  return false;
}


void WIN32_CecRecvMessageBox::DeleteAllMessageAsync( ICecMessageBoxEventListener* pEventListener )
{
  m_pEventListener = pEventListener;
  m_eCommandID = gflnet2::cec::DELETE_ALL_MESSAGE;
}




GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif
