// ============================================================================
/*
 * @file ConnectionInternetConnectionFrame.h
 * @brief インターネット接続アプリのインターネット接続を行うプロセスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_INTERNET_CONNECTION_FRAME_H_INCLUDE )
#define CONNECTION_INTERNET_CONNECTION_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/Connection/source/Frame/ConnectionFrameBase.h"
#include "NetApp/Connection/source/View/ConnectionLowerViewListener.h"
#include "NetApp/Connection/source/RequestSequence/ConnectionResponseListener.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncConfirmPlayStatusResponseListener.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncSaveDataSyncResponseListener.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncCreateSaveDataResponseListener.h"
#include "NetStatic/NetAppLib/include/GlobalAttraction/GlobalAttractionOpenCheckResponseListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(Frame)


class ConnectionInternetConnectionFrame :
  public NetApp::Connection::Frame::ConnectionFrameBase,
  public NetApp::Connection::RequestSequence::ConnectionResponseListener,
  public NetAppLib::GameSync::GameSyncConfirmPlayStatusResponseListener,
  public NetAppLib::GameSync::GameSyncSaveDataSyncResponseListener,
  public NetAppLib::GameSync::GameSyncCreateSaveDataResponseListener,
  public NetAppLib::GlobalAttraction::GlobalAttractionOpenCheckResponseListener

{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionInternetConnectionFrame );
public:
  ConnectionInternetConnectionFrame(
    NetApp::Connection::System::ConnectionWork* pConnectionWork,
    NetApp::Connection::System::ConnectionViewSystemAccessor* pConnectionViewSystemAccessor );
  virtual~ConnectionInternetConnectionFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  NetAppLib::System::FrameResult getExitFrameResult();

  void initializeNijiNetworkSystem();

  void defaultProcedure();

public:

  // ConnectionResponseListener
  virtual void OnConnectionSuccess();
  virtual void OnConnectionError();

  // GameSyncConfirmPlayStatusResponseListener
  virtual void OnConfirmPlayStatusSuccess();
  virtual void OnConfirmPlayStatusError( u32 errorCode );
#if defined(GF_PLATFORM_CTR)
  virtual void OnConfirmPlayStatusError( u32 errorCode, NetLib::Pgl::PGL_COMMON_RESULT ePglCommonResult );
#endif // defined(GF_PLATFORM_CTR)

  // GameSyncSaveDataSyncResponseListener
  virtual void OnSaveDataSyncSuccess();
  virtual void OnSaveDataSyncError( u32 errorCode );
#if defined(GF_PLATFORM_CTR)
  virtual void OnSaveDataSyncError( u32 errorCode, NetLib::Pgl::PGL_COMMON_RESULT ePglCommonResult );
#endif // defined(GF_PLATFORM_CTR)

  // GameSyncCreateSaveDataResponseListener
  virtual void OnCreateSaveDataSuccess();
  virtual void OnCreateSaveDataError();

  // GlobalAttractionOpenCheckResponseListener
  virtual void OnOpenCheckSuccess( bool bIsOpen );
  virtual void OnOpenCheckError();
  virtual void OnOpenCheckCancel();

private:

  NetApp::Connection::View::ConnectionLowerViewListener   m_ConnectionLowerViewListener;
  NetAppLib::Util::NetAppCommonSaveUtility                m_NetAppCommonSaveUtility;
  NetAppLib::Util::NetAppTimeout                          m_NetAppTimeout;
  bool                                                    m_bIsValidNexUniqueID;
  bool                                                    m_bIsTimeOutStart;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTION_INTERNET_CONNECTION_FRAME_H_INCLUDE
