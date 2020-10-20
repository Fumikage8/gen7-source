// ============================================================================
/*
 * @file JoinFestaBeaconResponseListener.h
 * @brief JoinFestaBeaconアプリで行う通信リクエストのレスポンスリスナーです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( JOINFESTABEACONRESPONSELISTENER_H_INCLUDE )
#define JOINFESTABEACONRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)




//--------------------------------------------------------------------------------------------
/**
  * @brief   JoinFestaBeaconアプリで行う通信リクエストのレスポンスリスナー
  */
//--------------------------------------------------------------------------------------------
class JoinFestaBeaconResponseListener
{



public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   通信エラー
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnError( void ) = 0;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   無線LANスイッチOFF
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnWLANSwitchOFF( void ) = 0;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   接続成功
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnConnectSuccess( void ) = 0;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   相手が断った
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnConnectCanceled( u32 cancelPlayerIndex ) = 0;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   キャンセル成功
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnCancelSuccess( void ) = 0;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   プレイヤー参加を検知
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnJoinPlayer( NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData ) = 0;

};


GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACONRESPONSELISTENER_H_INCLUDE
