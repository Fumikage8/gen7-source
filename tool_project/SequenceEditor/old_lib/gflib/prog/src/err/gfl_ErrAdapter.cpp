//=============================================================================
/**
 * @file   gfl_ErrAdapter.cpp
 * @brief  エラーEULAアプレットを適切に処理するクラス
 * @author k.ohno
 * @date   2012.10.23
 */
//=============================================================================

#include <gflib.h>
#include "err/gfl_ErrAdapter.h"
#include <base/gfl_Applet.h>


GFL_NAMESPACE_BEGIN(gfl)
GFL_NAMESPACE_BEGIN(err)

nn::erreula::Parameter ErrAdapter::mEreParam;  //渡すパラメータ 1KB以上あるので定義

//------------------------------------------------------------------
/**
 * @brief   任天堂のnn:Resultに格納されたエラーをセットする
 * @param   errorCode  エラーコード
 * @param   errorType  エラータイプ
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode ErrAdapter::ErrorDisp(u32 errorCode, nn::erreula::ErrorType errorType)
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
nn::erreula::CTR::ReturnCode ErrAdapter::ErrorDisp(gfl::str::StrBuf* pBuf)
{

  // 設定構造体初期化
  nn::erreula::InitializeConfig( &mEreParam.config );

  mEreParam.config.errorType = nn::erreula::ERROR_TYPE_ERROR_TEXT;
  pBuf->PutStr(mEreParam.config.errorText,nn::erreula::ERROR_TEXT_LENGTH_MAX);
  return _ErrorDispDirectMain(&mEreParam);
}

#if GFL_DEBUG

//------------------------------------------------------------------
/**
 * @brief   任天堂のnn:Resultに格納されたエラーをセットする
 * @param   errorCode  エラーコード
 * @param   errorType  エラータイプ
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode ErrAdapter::ErrorDisp(const wchar_t* pMessage)
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
nn::erreula::CTR::ReturnCode ErrAdapter::ErrorDisp(const char* pMessage)
{
  // 設定構造体初期化
  nn::erreula::InitializeConfig( &mEreParam.config );

  mEreParam.config.errorType = nn::erreula::ERROR_TYPE_ERROR_TEXT;
  mbstowcs(mEreParam.config.errorText , pMessage, nn::erreula::ERROR_TEXT_LENGTH_MAX);
  return _ErrorDispDirectMain(&mEreParam);
}

void ErrAdapter::ErrorDispChar(const char* pMessage)
{
  ErrorDisp(pMessage);
}




#endif //GFL_DEBUG

//------------------------------------------------------------------
/**
 * @brief   任天堂のnn:Resultに格納されたエラーをセットする
 * @param   errorCode  エラーコード
 * @param   errorType  エラータイプ
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode ErrAdapter::_ErrorDispDirectMain(nn::erreula::Parameter* ere_param)
{
  //----------------------------------------------------------------------------
  /**
   *  @brief  アプレットコール前処理
   */
  //-----------------------------------------------------------------------------
  base::Applet::AppletCallBefore();

  gfl::cfg::Config config;
  config.Initialize();

  nn::applet::AppletWakeupState wstate;

  // ソフトリセット＆HOMEボタンを禁止
  ere_param->config.upperScreenFlag = nn::erreula::UPPER_SCREEN_NORMAL;
  ere_param->config.homeButton    = false;
  ere_param->config.softwareReset = false;
  ere_param->config.appJump = false;

  nn::erreula::StartErrEulaApplet( &wstate, ere_param );

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
  base::Applet::AppletCallAfter();

  config.Finalize();

  return ere_param->config.returnCode;
}


GFL_NAMESPACE_END(err)
GFL_NAMESPACE_END(gfl)



