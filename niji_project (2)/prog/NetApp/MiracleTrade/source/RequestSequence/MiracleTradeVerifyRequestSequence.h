// ============================================================================
/*
 * @file MiracleTradeVerifyRequestSequence.h
 * @brief 交換するポケモンの不正チェックをするリクエストを投げるシーケンスです。
 * @date 2015.05.26
 */
// ============================================================================
#if !defined( MIRACLETRADEVERIFYREQUESTSEQUENCE_H_INCLUDE )
#define MIRACLETRADEVERIFYREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetApp/MiracleTrade/source/RequestSequence/MiracleTradeVerifyResponseListener.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"
#include "NetApp/MiracleTrade/source/MiracleTradeDefine.h"
#include "NetStatic/NetAppLib/include/Util/NetAppPokemonValidationUtility.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(RequestSequence)

class MiracleTradeVerifyRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeVerifyRequestSequence );

private:

  static const u32 DEFAULT_NOTIFICATION_WAIT_FRAME = NetApp::MiracleTrade::MIRACLETRADE_DEFAULT_NOTIFICATION_WAIT_FRAME;

public:

  MiracleTradeVerifyRequestSequence();
  virtual~MiracleTradeVerifyRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel();
  void SetRequestParam( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork ){ m_pMiracleTradeWork = pMiracleTradeWork; }
  virtual void RemoveListener(){ m_pVerifyResponseListener = NULL; }
  void SetListener( MiracleTradeVerifyResponseListener* pVerifyResponseListener ){ m_pVerifyResponseListener = pVerifyResponseListener; }

private:

  bool isNotification(){ return ( m_FrameCount >= DEFAULT_NOTIFICATION_WAIT_FRAME ); }

  enum Sequence
  {
    SEQUENCE_INITIALIZE,
    SEQUENCE_VERIFY,
    SEQUENCE_NOTIFICATION_OK,
    SEQUENCE_NOTIFICATION_NG,
    SEQUENCE_NOTIFICATION_ERROR,
    SEQUENCE_TERMINATE,
  };

#if defined( GF_PLATFORM_CTR )
  NetAppLib::Util::NetAppPokemonValidationUtility*              m_pNetAppPokemonValidationUtility;
#endif // defined( GF_PLATFORM_CTR )

  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*    m_pMiracleTradeWork;
  MiracleTradeVerifyResponseListener*                           m_pVerifyResponseListener;
  u32                                                           m_FrameCount;


};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEVERIFYREQUESTSEQUENCE_H_INCLUDE
