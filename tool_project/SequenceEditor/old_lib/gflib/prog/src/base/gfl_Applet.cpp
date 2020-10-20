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

#include <base/gfl_Applet.h>

#include <std/gfl_Date.h>

#include <grp/gfl_GraphicsSystem.h>

namespace gfl
{
namespace base
{

  /// アプレットインターフェース
  AppletInterface * Applet::m_pInterface = NULL;

  //----------------------------------------------------------------------------
  /**
   *  @brief  インターフェースの設定
   */
  //-----------------------------------------------------------------------------
  void Applet::SetInterface( AppletInterface * pInterface )
  {
    m_pInterface = pInterface;
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレットコール前処理
   *
   *  @note 内部で、AppletInterface->SaveApplication()をコール
   */
  //-----------------------------------------------------------------------------
  void Applet::AppletCallBefore( void )
  {
    if( m_pInterface ){ m_pInterface->SaveApplication(); }

    //------------------------
    //SDK4.0からコマンドリストを完了させることが必須になりました！
    //------------------------
    //実行中のコマンドを終わらせる。
    nngxWaitCmdlistDone();
    nngxWaitVSync(NN_GX_DISPLAY_BOTH);
    //------------------------
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレットコール後処理　（アプリケーションに復帰するときのみ使用する）
   *
   *  @note 内部で、AppletInterface->RecoverApplication()をコール
   */
  //-----------------------------------------------------------------------------
  void Applet::AppletCallAfter( void )
  {
    grp::GraphicsSystem * pGraphicsSystem = GFL_SINGLETON_INSTANCE(gfl::grp::GraphicsSystem);

    nngxWaitCmdlistDone();  ///< 実行を完了させる。
    GFL_GRPGL_ERROR_CHECK();

    /* gx ライブラリを使用している場合 */
    nngxUpdateState(NN_GX_STATE_ALL);
    /* GD ライブラリを使用している場合 */
    nn::gd::System::ForceDirty(nn::gd::System::MODULE_ALL);

    //書き込み先のコマンドリストを設定する。
    if( pGraphicsSystem )
    {
      pGraphicsSystem->BindCurrentCommandList();
    }
    //------------------------

    //------------------------
    //時間復帰
    //------------------------
    gfl::std::DateTime::Update(true);

    if( m_pInterface ){ m_pInterface->RecoverApplication(); }
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレットコール終了処理　（アプリ終了前に呼ばれる）
   *
   *  @note 内部で、AppletInterface->FinalizeApplication()をコール
   */
  //-----------------------------------------------------------------------------
  void Applet::AppletCallFinalize( void )
  {
    if( m_pInterface ){ m_pInterface->FinalizeApplication(); }
  }
  
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
  nn::applet::AppletWakeupState Applet::ProcessHomeButton( void )
  {
    nn::applet::AppletWakeupState r = nn::applet::WAKEUP_STATE_MAX;
    // HOMEボタン処理
    if( nn::applet::IsExpectedToProcessHomeButton() ){

      //---------------------
      // アプレット実行前処理
      //=====================
      AppletCallBefore();

      bool is_sleep = true;
      if( m_pInterface ){
        is_sleep = m_pInterface->IsSleepEnable();
      }
      if( is_sleep ){ nn::applet::DisableSleep(true); }

      nn::applet::ProcessHomeButton();
      r = nn::applet::WaitForStarting();

      if( nn::applet::IsExpectedToCloseApplication() )
      { 
        nn::applet::CTR::PrepareToCloseApplication();
        nn::applet::CTR::CloseApplication();
        // ここには到達しません
      }

      if( is_sleep ){ nn::applet::EnableSleep(true); }

      //---------------------
      // アプレット実行後処理
      //=====================
      AppletCallAfter();
    }

    return r;
  }


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
  void Applet::ProcessPowerButton( void )
  {
    if( nn::applet::IsExpectedToProcessPowerButton() )
    {
      //---------------------
      // アプレット実行前処理
      //=====================
      AppletCallBefore();

      bool is_sleep = true;
      if( m_pInterface ){
        is_sleep = m_pInterface->IsSleepEnable();
      }
      if( is_sleep ){ nn::applet::DisableSleep(true); }


      nn::applet::ProcessPowerButton();
      nn::applet::AppletWakeupState r = nn::applet::WaitForStarting();
      NN_UNUSED_VAR(r);

      if( nn::applet::IsExpectedToCloseApplication() )
      { 
        nn::applet::CTR::PrepareToCloseApplication();
        nn::applet::CTR::CloseApplication();
        // ここには到達しません
      }

      // SDK 5.0現在
      // ここには到達することもありません。
      // 一応コーディングしておきます。

      if( is_sleep ){ nn::applet::EnableSleep(true); }

      //---------------------
      // アプレット実行後処理
      //=====================
      AppletCallAfter();
    }
  }

}
}



