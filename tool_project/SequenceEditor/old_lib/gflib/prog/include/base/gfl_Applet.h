//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Applet.h
 *	@brief  アプレット　管理
 *	@author tomoya takahashi
 *	@date		2013.02.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __GFL_APPLET_H__
#define __GFL_APPLET_H__

namespace gfl
{
namespace base
{
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレット起動前セーブ処理、　後リカバー処理　インターフェース
   */
  //-----------------------------------------------------------------------------
  class AppletInterface
  {
  public:
    virtual void SaveApplication( void ) = 0;  ///< アプリケーションセーブ処理
    virtual void RecoverApplication( void ) = 0;  ///< アプリケーション復帰処理
    virtual void FinalizeApplication( void ) = 0;  ///< アプリケーション終了処理
    virtual bool IsSleepEnable( void ) = 0; ///< アプリケーション側でスリープを許可しているか？
  };


  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレット起動前後管理関数群
   */
  //-----------------------------------------------------------------------------
  class Applet
  {
  public:
    //----------------------------------------------------------------------------
    /**
     *  @brief  インターフェースの設定
     */
    //-----------------------------------------------------------------------------
    static void SetInterface( AppletInterface * pInterface );

    //----------------------------------------------------------------------------
    /**
     *  @brief  アプレットコール前処理
     *
     *  @note 内部で、AppletInterface->SaveApplication()をコール
     */
    //-----------------------------------------------------------------------------
    static void AppletCallBefore( void );

    //----------------------------------------------------------------------------
    /**
     *  @brief  アプレットコール後処理　（アプリケーションに復帰するときのみ使用する）
     *
     *  @note 内部で、AppletInterface->RecoverApplication()をコール
     */
    //-----------------------------------------------------------------------------
    static void AppletCallAfter( void );

    //----------------------------------------------------------------------------
    /**
     *  @brief  アプレットコール終了処理　（アプリ終了前に呼ばれる）
     *
     *  @note 内部で、AppletInterface->FinalizeApplication()をコール
     */
    //-----------------------------------------------------------------------------
    static void AppletCallFinalize( void );

    //----------------------------------------------------------------------------
    /**
     *  @brief  Homeボタンプロセスの実行
     *
     *  @note 下記の処理を実行します。
     *        IsExpectedToProcessHomeButton()チェック 
     *        DisableSleep()
     *        AppletCallBefore()
     *        ProcessHomeButton()
     *        nn::applet::WaitForStarting()
     *        nn::applet::IsExpectedToCloseApplication()チェック  -> アプリケーション終了
     *        AppletCallAfter()
     *        EnableSleep()
     *        
     *  @note アプリケーション終了に遷移した場合、この関数からは抜けてきません。
     */
    //-----------------------------------------------------------------------------
    static nn::applet::AppletWakeupState ProcessHomeButton( void );



    //----------------------------------------------------------------------------
    /**
     *  @brief  電源ボタンプロセスの実行
     *
     *  @note 下記の処理を実行します。
     *        IsExpectedToProcessPowerButton()()チェック 
     *        DisableSleep()
     *        AppletCallBefore()
     *        ProcessPowerButton()
     *        nn::applet::WaitForStarting()
     *        nn::applet::IsExpectedToCloseApplication()チェック  -> アプリケーション終了
     *        
     *  @note この関数からは抜けてきません。
     */
    //-----------------------------------------------------------------------------
    static void ProcessPowerButton( void );

  private:

    // セーブ　・　リカバー　インターフェース
    static AppletInterface * m_pInterface;
  };
}
}

#endif // __GFL_APPLET_H__
