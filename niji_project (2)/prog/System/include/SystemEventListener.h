//=============================================================================
/**
 * @file   SystemEventListener.h
 *
 * @brief  本体システムイベント通知リスナ
 *
 * @author hisanao suzuki
 */
//=============================================================================
#ifndef __NIJI_SYSTEM_EVENT_LISTENER_H__
#define __NIJI_SYSTEM_EVENT_LISTENER_H__
#pragma once


#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>



namespace System{

/**
 * @brief システムアプレット予約番号
 */
typedef enum
{
  E_APPLET_BROWSER,       //!< ブラウザアプレット
  E_APPLET_PHOTO_SELECT,  //!< 写真アプレット
  E_APPLET_EULA_AGREE,    //!< EULA同意アプレット
  E_APPLET_COPPA_PIN,     //!< COPPA PINコード入力画面
  //
  E_RESERVE_APPLET_MAX
}RESERVE_APPLET;


/**
 * @brief システムに起因するサスペンド＆レジュームイベント用リスナ
 */
class SystemEventListener
{
public:

public:
  //------------------------------------------------------------------
  /**
  * @brief スリープ遷移時イベント（スリープ禁止の場合は呼ばれない）
  */
  //------------------------------------------------------------------
  virtual void OnSuspendWithSleep( void ){};

  //------------------------------------------------------------------
  /**
  * @brief スリープからの復帰時イベント（スリープ禁止の場合は呼ばれない）
  */
  //------------------------------------------------------------------
  virtual void OnResumeWithSleep( void ){};

  //------------------------------------------------------------------
  /**
  * @brief 蓋閉じ時イベント（スリープ禁止、許可にかかわらず呼ばれる）
  */
  //------------------------------------------------------------------
  virtual void CallCloseCover( void ){};

  //------------------------------------------------------------------
  /**
  * @brief ホームメニュー遷移時イベント
  */
  //------------------------------------------------------------------
  virtual void OnSuspendWithHomeMenu( void ){};

  //------------------------------------------------------------------
  /**
  * @brief ホームメニューからの復帰時イベント
  */
  //------------------------------------------------------------------
  virtual void OnResumeWithHomeMenu( void ){};


  //------------------------------------------------------------------
  /**
  * @brief 予約されていたアプレットコール実行イベント
  *
  * @note 本リスナ内部でアプレットに関する処理を実行してください
  *
  * @param calledApplet 実行許可されたアプレット
  */
  //------------------------------------------------------------------
  virtual void OnCallApplet( System::RESERVE_APPLET calledApplet ){};

  //------------------------------------------------------------------
  /**
   * @brief   ゲーム終了処理発生時イベント
   *
   * @retval  true    終了処理完了
   * @retval  false   終了処理中
   *
   * @note    適切な終了処理を記述してください。
   * @attention  On～という名前なのに他のメンバとふるまいが違うので注意
   * @attention  Observerパターンとして名前が適切なのかは疑問の余地あり
   */
  //------------------------------------------------------------------
  virtual bool OnCloseApplication( void ) { return true; };

  //------------------------------------------------------------------
  /**
   * @brief   ホーム、スリープ、ゲーム終了処理発生時イベント
   *
   * @retval  true    終了処理完了
   * @retval  false   終了処理中
   *
   * @note    適切な終了処理を記述してください。
   * @attention  On～という名前なのに他のメンバとふるまいが違うので注意
   * @attention  Observerパターンとして名前が適切なのかは疑問の余地あり
   */
  //------------------------------------------------------------------
  virtual bool OnSuspendWithHomeSleepAndPower( void ) { return true; };


};


}	


#endif // __XY_SYSTEM_EVENT_LISTENER_H__
