//==============================================================================
/**
 * @file	NijiNetworkErreulaApplet.cpp
 * @brief	エラー・EULAアプレット表示クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/05/15 12:38:22
 */
// =============================================================================
#if defined(GF_PLATFORM_CTR)

// mine
#include "NetStatic/NetLib/include/Error/NijiNetworkErreulaApplet.h"
#include <nw/snd.h>
#include "System/include/CTR/CTR_Applet.h"
#include "System/include/CTR/CTR_Config.h"
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

nn::erreula::Parameter NijiNetworkErreulaApplet::mEreParam;  //渡すパラメータ 1KB以上あるので定義

//------------------------------------------------------------------
/**
 * @brief   任天堂のnn:Resultに格納されたエラーをセットする
 * @param   errorCode  エラーコード
 * @param   errorType  エラータイプ
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode NijiNetworkErreulaApplet::ErrorDisp(u32 errorCode, nn::erreula::ErrorType errorType)
{

  // 設定構造体初期化
  nn::erreula::InitializeConfig( &mEreParam.config );

  mEreParam.config.errorType = errorType;
  mEreParam.config.errorCode = errorCode;
  
  return _ErrorDispDirectMain(&mEreParam);
}

//------------------------------------------------------------------
/**
 * @brief   任天堂のnn:Resultに格納されたエラーをセットする
 * @param   errorCode  エラーコード
 * @param   errorType  エラータイプ
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode NijiNetworkErreulaApplet::ErrorDisp(gfl2::str::StrBuf* pBuf)
{

  // 設定構造体初期化
  nn::erreula::InitializeConfig( &mEreParam.config );

  mEreParam.config.errorType = nn::erreula::ERROR_TYPE_ERROR_TEXT;
  pBuf->PutStr(mEreParam.config.errorText,nn::erreula::ERROR_TEXT_LENGTH_MAX);
  return _ErrorDispDirectMain(&mEreParam);
}

#if PM_DEBUG

//------------------------------------------------------------------
/**
 * @brief   任天堂のnn:Resultに格納されたエラーをセットする
 * @param   errorCode  エラーコード
 * @param   errorType  エラータイプ
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode NijiNetworkErreulaApplet::ErrorDisp(const wchar_t* pMessage)
{
  // 設定構造体初期化
  nn::erreula::InitializeConfig( &mEreParam.config );

  mEreParam.config.errorType = nn::erreula::ERROR_TYPE_ERROR_TEXT;
  memcpy(mEreParam.config.errorText , pMessage, nn::erreula::ERROR_TEXT_LENGTH_MAX);
  return _ErrorDispDirectMain(&mEreParam);
}


//------------------------------------------------------------------
/**
 * @brief   任天堂のnn:Resultに格納されたエラーをセットする
 *          mbstowcsには限界があり、正しく表示されない事があります。 wchar_tなら出ます
 * @param   errorCode  エラーコード
 * @param   errorType  エラータイプ
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode NijiNetworkErreulaApplet::ErrorDisp(const char* pMessage)
{
  // 設定構造体初期化
  nn::erreula::InitializeConfig( &mEreParam.config );

  mEreParam.config.errorType = nn::erreula::ERROR_TYPE_ERROR_TEXT;
  mbstowcs(mEreParam.config.errorText , pMessage, nn::erreula::ERROR_TEXT_LENGTH_MAX);
  return _ErrorDispDirectMain(&mEreParam);
}

void NijiNetworkErreulaApplet::ErrorDispChar(const char* pMessage)
{
  nw::snd::SoundSystem::LockSoundThread(); // GFBTS2500対策のサウンドスレッド一時停止
  ErrorDisp(pMessage);
  nw::snd::SoundSystem::UnlockSoundThread();// GFBTS2500対策のサウンドスレッド一時停止解除
}

#endif //PM_DEBUG

//------------------------------------------------------------------
/**
 * @brief   任天堂のnn:Resultに格納されたエラーをセットする
 * @param   errorCode  エラーコード
 * @param   errorType  エラータイプ
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode NijiNetworkErreulaApplet::_ErrorDispDirectMain(nn::erreula::Parameter* ere_param)
{
  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレットコール前処理
   */
  //-----------------------------------------------------------------------------
  System::CTR::Applet::AppletCallBefore();

  System::CTR::Config config;
  config.Initialize();

  nn::applet::AppletWakeupState wstate;

  // ソフトリセット＆HOMEボタンを禁止
  ere_param->config.upperScreenFlag = nn::erreula::UPPER_SCREEN_NORMAL;
  ere_param->config.homeButton    = false;
  ere_param->config.softwareReset = false;
  ere_param->config.appJump = false;

  // @fix GFNMCat[5732]：アプリケーションが終了する状態の場合はアプレット呼び出しをしない
  if( nn::applet::IsExpectedToCloseApplication() == false )
  {
#if PM_DEBUG
  //緑箱のdev1はアプレットを出せない
  if( nn::os::GetAppMemorySize() != 0x6000000 )
  {
    nn::erreula::StartErrEulaApplet( &wstate, ere_param );
  }
  else
  {
    GFL_ASSERT_MSG( 0,"通信エラーです。緑箱dev1なのでアサートでお知らせします。意図的に出しているのでバグではありません。" );//とりあえずアサート
  }
#else
  nn::erreula::StartErrEulaApplet( &wstate, ere_param );
#endif
  }


  if(nn::applet::IsExpectedToCloseApplication()){
    // 終了への遷移は、ソフトウェアに任せる（絶対に用意されている）
    /*
    {
      nn::applet::PrepareToCloseApplication();
      nn::applet::CloseApplication();
      // ここには到達しません
    }
    */
    return ere_param->config.returnCode;
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレットコール後処理
   */
  //-----------------------------------------------------------------------------
  System::CTR::Applet::AppletCallAfter();

  config.Finalize();

  return ere_param->config.returnCode;
}

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

#endif // GF_PLATFORM_CTR
