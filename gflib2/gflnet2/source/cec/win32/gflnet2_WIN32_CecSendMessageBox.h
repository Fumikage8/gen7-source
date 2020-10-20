// ============================================================================
/*
 * @file gflnet2_WIN32_CecSendMessageBox.h
 * @brief Win32版すれ違い通信の送信ボックスに関する操作を行うクラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_WIN32
#if !defined( GFLNET2_WIN32_CECSENDMESSAGEBOX_H_INCLUDE )
#define GFLNET2_WIN32_CECSENDMESSAGEBOX_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gflnet2/include/cec/gflnet2_ICecSendMessageBox.h>
#include <gflnet2/source/cec/gflnet2_CecCommandID.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(win32)


class WIN32_CecSendMessageBox : public ICecSendMessageBox
{
  GFL_FORBID_COPY_AND_ASSIGN( WIN32_CecSendMessageBox );

public:
  WIN32_CecSendMessageBox();
  virtual~WIN32_CecSendMessageBox();

  void Update();

  virtual bool DeleteAllMessage();
  virtual void DeleteAllMessageAsync( ICecMessageBoxEventListener* pEventListener );
  virtual bool WriteMessage( const CecSendPacket* pSendPacket );
  virtual void WriteMessageAsync( const CecSendPacket* pSendPacket, ICecMessageBoxEventListener* pEventListener );

private:
  ICecMessageBoxEventListener*    m_pEventListener;
  CommandID                       m_eCommandID;

};


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_WIN32_CECSENDMESSAGEBOX_H_INCLUDE
#endif
