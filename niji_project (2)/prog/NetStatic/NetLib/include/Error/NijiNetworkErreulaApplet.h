//==============================================================================
/**
 * @file	NijiNetworkErreulaApplet.h
 * @brief	エラー・EULAアプレット表示クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/05/15 12:33:35
 *
 *  gfl_ErrAdapter.hの移植
 */
// =============================================================================

#if defined(GF_PLATFORM_CTR)
#ifndef __NIJI_NETWORK_ERROREULA_APPLET_H__
#define __NIJI_NETWORK_ERROREULA_APPLET_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_StrBuf.h>
#include <nn/erreula.h>


GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class NijiNetworkErreulaApplet
{
  GFL_FORBID_COPY_AND_ASSIGN(NijiNetworkErreulaApplet);
private:
  static nn::erreula::Parameter mEreParam;  //渡すパラメータ 1KB以上あるので定義
  static nn::erreula::CTR::ReturnCode _ErrorDispDirectMain(nn::erreula::Parameter* ere_param);

public:


  //------------------------------------------------------------------
  /**
   * @brief   アプレットが指定された番号のエラーを出す
   * @param   errorCode  エラーコード
   * @param   errorType  エラータイプ
   * @return  nn::erreula::CTR::ReturnCode 任天堂アプレットのリターンコード
   */
  //------------------------------------------------------------------
  static nn::erreula::CTR::ReturnCode ErrorDisp(u32 errorCode , nn::erreula::ErrorType errorType);
  //------------------------------------------------------------------
  /**
   * @brief   直接エラー文章を出す
   * @param   pBuf StrBufのポインタ
   * @return  nn::erreula::CTR::ReturnCode 任天堂アプレットのリターンコード
   */
  //------------------------------------------------------------------
  static nn::erreula::CTR::ReturnCode ErrorDisp(gfl2::str::StrBuf* pBuf);
#if PM_DEBUG
  //------------------------------------------------------------------
  /**
   * @brief   直接エラーメッセージを出す デバッグ用途
   * @param   ワイドキャラ型文字列
   * @return  nn::erreula::CTR::ReturnCode 任天堂アプレットのリターンコード
   */
  //------------------------------------------------------------------
  static nn::erreula::CTR::ReturnCode ErrorDisp(const wchar_t* pMessage);
  //------------------------------------------------------------------
  /**
   * @brief   直接エラーメッセージを出す デバッグ用途
   *          mbstowcsには限界があり、正しく表示されない事があります。 wchar_tなら出ます
   * @param   文字列
   * @return  nn::erreula::CTR::ReturnCode 任天堂アプレットのリターンコード
   */
  //------------------------------------------------------------------
  static nn::erreula::CTR::ReturnCode ErrorDisp(const char* pMessage);
  static void ErrorDispChar(const char* pMessage);

#endif //PM_DEBUG

};

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

#endif // __NIJI_NETWORK_ERROREULA_APPLET_H__
#endif // GF_PLATFORM_CTR
