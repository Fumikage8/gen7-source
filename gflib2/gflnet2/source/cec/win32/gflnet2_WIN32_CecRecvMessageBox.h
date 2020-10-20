// ============================================================================
/*
 * @file gflnet2_WIN32_CecRecvMessageBox.h
 * @brief Win32版すれ違い通信の受信ボックスに関する操作を行うクラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_WIN32
#if !defined( GFLNET2_WIN32_CECRECVMESSAGEBOX_H_INCLUDE )
#define GFLNET2_WIN32_CECRECVMESSAGEBOX_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gflnet2/include/cec/gflnet2_ICecRecvMessageBox.h>
#include <gflnet2/source/cec/gflnet2_CecCommandID.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(win32)


class WIN32_CecRecvMessageBox : public ICecRecvMessageBox
{
  GFL_FORBID_COPY_AND_ASSIGN( WIN32_CecRecvMessageBox );

public:
  WIN32_CecRecvMessageBox();
  virtual~WIN32_CecRecvMessageBox();

  void Update();

  virtual s32 GetMessageCount();
  virtual void GetMessageCountAsync( ICecMessageBoxEventListener* pEventListener );
  virtual const gflnet2::cec::CecRecvPacket* ReadMessage( u32& recvPacketCount );
  virtual void ReadMessageAsync( ICecMessageBoxEventListener* pEventListener );
  virtual bool DeleteAllMessage();
  virtual void DeleteAllMessageAsync( ICecMessageBoxEventListener* pEventListener );

private:
  ICecMessageBoxEventListener*    m_pEventListener;
  CommandID                       m_eCommandID;

};


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_WIN32_CECRECVMESSAGEBOX_H_INCLUDE
#endif
