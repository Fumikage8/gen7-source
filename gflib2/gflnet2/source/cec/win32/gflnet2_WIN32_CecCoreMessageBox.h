// ============================================================================
/*
 * @file gflnet2_WIN32_CecCoreMessageBox.h
 * @brief Win32版すれ違い通信ボックスに関する操作を行うクラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_WIN32
#if !defined( GFLNET2_WIN32_CECCOREMESSAGEBOX_H_INCLUDE )
#define GFLNET2_WIN32_CECCOREMESSAGEBOX_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gflnet2/include/cec/gflnet2_ICecCoreMessageBox.h>
#include <gflnet2/source/cec/gflnet2_CecCommandID.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(win32)


class WIN32_CecCoreMessageBox : public ICecCoreMessageBox
{
  GFL_FORBID_COPY_AND_ASSIGN( WIN32_CecCoreMessageBox );

public:
  WIN32_CecCoreMessageBox();
  virtual~WIN32_CecCoreMessageBox();

  void Update();

  virtual bool IsCreateMessageBox();
  virtual void IsCreateMessageBoxAsync( ICecMessageBoxEventListener* pEventListener );
  virtual bool CreateMessageBox( const CecMessageBoxCreateCoreParam* pCreateCoreParam );
  virtual void CreateMessageBoxAsync( const CecMessageBoxCreateCoreParam* pCreateCoreParam, ICecMessageBoxEventListener* pEventListener );
  virtual bool DeleteMessageBox();
  virtual void DeleteMessageBoxAsync( ICecMessageBoxEventListener* pEventListener );

private:
  ICecMessageBoxEventListener*    m_pEventListener;
  CommandID                       m_eCommandID;

};


GFL_NAMESPACE_END(win32)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_WIN32_CECCOREMESSAGEBOX_H_INCLUDE
#endif
