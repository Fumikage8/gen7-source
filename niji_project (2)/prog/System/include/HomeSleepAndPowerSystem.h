
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		HomeSleepAndPowerSystem.h
 *	@brief  ホーム、スリープ、電源ボタン、リセットの管理
 *	@author	
 *	@date		
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __HOME_SLEEP_AND_POWER_SYSTEM_H__
#define __HOME_SLEEP_AND_POWER_SYSTEM_H__
#pragma once


#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include <System/include/SystemEventManager.h>

namespace System
{


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class HomeSleepAndPowerSystem 
{
  GFL_FORBID_COPY_AND_ASSIGN(HomeSleepAndPowerSystem); //コピーコンストラクタ＋代入禁止

public:

    enum E_HOME_SLEEP_TYPE_BIT{
    HOME_SLEEP_P2P_ACC      = 0x01,  //!< P2Pが動いている
    HOME_SLEEP_SDCARD_ACC   = 0x02,  //!< SDカードアクセス
    HOME_SLEEP_SAVE_ACC     = 0x04,  //!< セーブアクセス
    HOME_SLEEP_IMAGEDB_ACC  = 0x08,  //!< ImageDBでのSDカードアクセス
    HOME_SLEEP_QR_ACC       = 0x10,  //!< QRアプリでのカメラ起動ブロック
    HOME_SLEEP_NETAPP_ACC     = 0x20,  //!< 通信アプリ
    HOME_SLEEP_JF_ATTRACTION_ACC = 0x40,  //!< ジョインフェスタのアトラクションが動いている
    HOME_SLEEP_JF_BEACON_ACC = 0x80, //!< ジョインフェスタビーコンアプリ
    HOME_SLEEP_MASK_BIT     = 0xff,  //!< SLEEP_MASK_BIT
  };

  typedef unsigned char SLEEPTYPE;

private:

  // コンストラクタ・デストラクタ
  HomeSleepAndPowerSystem(void);
  virtual ~HomeSleepAndPowerSystem();



public:
  //-----------------------------------------------------------------------------
  /**
   *	初期化　コールバックの登録等
  */
  //-----------------------------------------------------------------------------
  static void Initialize( void );

  //-----------------------------------------------------------------------------
  /**
   *	終了処理
  */
  //-----------------------------------------------------------------------------
  static void Finalize( void );

  //更新 この中でHome遷移、スリープ遷移、電源ボタン終了、リセットする
  static void Update();

  /*------------------------------------------------------------------------*
    スリープの状態変更
   *------------------------------------------------------------------------*/
  static void AddConditions( E_HOME_SLEEP_TYPE_BIT bit );
  static void SubConditions( E_HOME_SLEEP_TYPE_BIT bit );

  // 指定した条件が禁止されているか
  static bool IsEnableSleepConditions( E_HOME_SLEEP_TYPE_BIT bitType = HOME_SLEEP_MASK_BIT );


  //--------------------------------------------------------------------------
  /**
   * @brief ホームに遷移しても良いか
   * @return  true:OK false:NG
   */
  //--------------------------------------------------------------------------
  static bool CheckHomeExecEnable( void );

  //--------------------------------------------------------------------------
  /**
   * @brief スリープに遷移しても良いか
   * @return  true:OK false:NG
   */
  //--------------------------------------------------------------------------
  static bool CheckSleepExecEnable( void );

  //--------------------------------------------------------------------------
  /**
   * @brief システムイベントマネージャ設定
   */
  //--------------------------------------------------------------------------
  static void SetSystemEventManager( System::SystemEventManager* pSystemEventManager );

private:
#if defined(GF_PLATFORM_CTR)

  //-----------------------------------------------------------------------------
  /**
   *	Home遷移、電源ボタン終了、リセット　の処理
  */
  //-----------------------------------------------------------------------------
  static void PowerResetAndHomeExec();
    //-----------------------------------------------------------------------------
  /**
   *	スリープの処理
  */
  //-----------------------------------------------------------------------------
  static void SleepExec();


  /*------------------------------------------------------------------------*
      ホーム＆スリープ＆電源OFF＆リセット に遷移する時の共通処理
   *------------------------------------------------------------------------*/
  static void HomeSleepAndPowerSuspendFunc( void );

  /*------------------------------------------------------------------------*
      ホームに遷移する時の処理
   *------------------------------------------------------------------------*/
  static void HomeSuspendFunc( void );

  /*------------------------------------------------------------------------*
      スリープに遷移する時の処理
   *------------------------------------------------------------------------*/
  static void SleepSuspendFunc( void );

  /*------------------------------------------------------------------------*
      ホーム＆スリープから復帰した時の共通処理
   *------------------------------------------------------------------------*/
  static void HomeAndSleepResumeFunc( void );

  /*------------------------------------------------------------------------*
      ホームから復帰した時の処理
   *------------------------------------------------------------------------*/
  static void HomeResumeFunc( void );

  /*------------------------------------------------------------------------*
      スリープから復帰した時の処理
   *------------------------------------------------------------------------*/
  static void SleepResumeFunc( void );

  /*------------------------------------------------------------------------*
      蓋を閉じた時の処理（スリープ可能、禁止に関わらず呼ばれる）
   *------------------------------------------------------------------------*/
  static void CoverCloseFunc( void );

  /*------------------------------------------------------------------------*
      電源OFF or リセットする時の処理
   *------------------------------------------------------------------------*/
  static void CloseApplication( void );

public:
  /*------------------------------------------------------------------------*
      スリープ問い合わせ時に呼ばれるコールバック
   *------------------------------------------------------------------------*/
  static AppletQueryReply SleepQueryCallback( uptr arg );
  /*------------------------------------------------------------------------*
      スリープ復帰時に呼ばれるコールバック
   *------------------------------------------------------------------------*/
  static void AwakeCallback( uptr arg );

#endif

private:

  static SLEEPTYPE sBitSleep;
  static System::SystemEventManager* s_pSystemEventManager;
#if defined(GF_PLATFORM_CTR)
  static nn::os::LightEvent s_AwakeEvent;
#endif
  static bool s_closeTrriger;//蓋とじイベント用トリガー
  
};
  

} // namespace System

#endif	// __HOME_SLEEP_AND_POWER_SYSTEM_H__


