// ============================================================================
/*
 * @file JoinFestaBeaconWaitInvitedFrame.h
 * @brief ビーコンアプリの参加承諾プロセスです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACONWAITINVITED_FRAME_H_INCLUDE )
#define JOINFESTABEACONWAITINVITED_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconWork.h"
#include "NetApp/JoinFestaBeacon/source/RequestSequence/JoinFestaBeaconRequestClient.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaBeaconWaitInvitedFrame : public NetAppLib::System::NetAppFrameBase , public JoinFestaBeaconResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaBeaconWaitInvitedFrame );
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


public:
  JoinFestaBeaconWaitInvitedFrame(
    NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* pJoinFestaBeaconWork );
  virtual~JoinFestaBeaconWaitInvitedFrame();

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
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACONWAITINVITED_FRAME_H_INCLUDE
