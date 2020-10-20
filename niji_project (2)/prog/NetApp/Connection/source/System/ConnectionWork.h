// ============================================================================
/*
 * @file ConnectionWork.h
 * @brief インターネット接続アプリのワーククラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTION_WORK_H_INCLUDE )
#define CONNECTION_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetApp/Connection/source/ConnectionDefine.h"
#include "NetStatic/NetEvent/include/FatalErrorEvent.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)


class ConnectionWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionWork );

private:


public:
  ConnectionWork( gfl2::heap::HEAPID heapId , u32 appHeapSize , u32 deviceHeapSize );
  virtual~ConnectionWork();

  NetApp::Connection::BootMode GetBootMode() const { return m_eBootMode; }
  void SetBootMode( NetApp::Connection::BootMode eBootMode ) { m_eBootMode = eBootMode; }

  bool IsCanceled() const { return m_bIsCanceled; }
  void SetCancel( bool bCancel ) { m_bIsCanceled = bCancel; }

  NetEvent::FatalError::FatalErrorNo GetFatalErrorNo() const { return m_eFatalErrorNo; }
  void SetFatalErrorNo( NetEvent::FatalError::FatalErrorNo eFatalErrorNo ){ m_eFatalErrorNo = eFatalErrorNo; }

  bool IsConnected() const { return m_bIsConnected; }
  void SetConnect( bool bConnect ) { m_bIsConnected = bConnect; }

  NetAppLib::Util::NetAppTrainerIconUtility* GetTrainerIconUtility() { return m_pTrainerIconUtility; }
  void SetTrainerIconUtility( NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtility ){ m_pTrainerIconUtility = pTrainerIconUtility; }


private:

  NetApp::Connection::BootMode              m_eBootMode;
  bool                                      m_bIsCanceled;
  NetEvent::FatalError::FatalErrorNo        m_eFatalErrorNo;
  bool                                      m_bIsConnected;
  NetAppLib::Util::NetAppTrainerIconUtility*                    m_pTrainerIconUtility;
};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTIONWORK_H_INCLUDE
