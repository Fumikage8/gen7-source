// ============================================================================
/*
 * @file JoinFestaBeaconInviteFrame.h
 * @brief ビーコンアプリの一人申し込みプロセスです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACONINVITESINGLE_FRAME_H_INCLUDE )
#define JOINFESTABEACONINVITESINGLE_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconWork.h"
#include "NetApp/JoinFestaBeacon/source/RequestSequence/JoinFestaBeaconRequestClient.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaBeaconInviteFrame : public NetAppLib::System::NetAppFrameBase , public JoinFestaBeaconResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaBeaconInviteFrame );
public:

  //メインシーケンス
  enum
  {
    MAINSEQ_WAIT = 0,
    MAINSEQ_CONNECT_SUCCESS,
    MAINSEQ_CONNECT_NG,
    MAINSEQ_CONNECT_WLANOFF,
    MAINSEQ_CANCEL_REQUEST,
    MAINSEQ_RETURN_OK_WAIT,
    MAINSEQ_RETURN_NG_MESSAGE,
    MAINSEQ_CANCEL_WAIT,
    MAINSEQ_ERROR
  };

  enum
  {
    FINAL_WAIT_COUNT = 45,
  };

public:
  JoinFestaBeaconInviteFrame(
    NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* pJoinFestaBeaconWork );
  virtual~JoinFestaBeaconInviteFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual bool cleanup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  void SetMainSeq( u32 mainSeq );

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   通信エラー
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnError( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   無線LANスイッチOFF
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnWLANSwitchOFF( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   接続成功
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnConnectSuccess( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   相手が断った
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnConnectCanceled( u32 cancelPlayerIndex );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   キャンセル成功
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnCancelSuccess( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   プレイヤー参加を検知
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnJoinPlayer( NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData );

private:

  NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork*                    m_pJoinFestaBeaconWork;

  u32 m_mainSeq;
  u32 m_subSeq;
  JoinFestaBeaconRequestClient m_requestClient;
  u32 m_cancelIndex;
  u32 m_waitMessage;
  u32 m_finalWaitCount;
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACONINVITESINGLE_FRAME_H_INCLUDE
