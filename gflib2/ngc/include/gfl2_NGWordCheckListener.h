﻿//================================================================================
/**
 * @file   gfl2_NGWordChecker.h
 * @brief  NGワードチェック用クラス
 * @author hisanao suzuki
 * @date   2012.2.15
 */
//================================================================================
#ifndef __GFL2_NG_WORD_CHECK_LISTENER_H__
#define __GFL2_NG_WORD_CHECK_LISTENER_H__
#pragma once

#ifdef GF_PLATFORM_CTR
#include <nn.h>
#include <nn/ngc.h>
#endif

namespace gfl2 {
namespace ngc {

  /**
   * @brief NGワードの検査結果
   */
  enum Result
  {
    E_SUCCESS,                      //!< 問題なし
    E_ERROR_NUMBER_COUNT,           //!< 数字文字数エラー
    E_ERROR_FREEWORD_CHAR_COUNT,    //!< フリーワード文字数エラー
    E_ERROR_WORD                    //!< 単語エラー
  };

  //-------------------------------------------------------------------------------
  /**
  * @brief NGワードチェック完了通知用リスナクラス
  */
  //-------------------------------------------------------------------------------
  class NGWordCheckListener
  {
//    GFL_FORBID_COPY_AND_ASSIGN( NGWordCheckListener );  // コピーを禁止

  public:
    //-------------------------------------------------------------------------------
    /**
    * @brief  NGワード検査の完了通知イベント
    *
    * @param[in]  result 検査結果
    */
    // ---------------------------------------------------------------------
    virtual void OnNGWordCheckFinish( gfl2::ngc::Result result ) = 0;

  };

} // ngc
} // gfl2

#endif  // __GFL_NG_WORD_CHECK_LISTENER_H__
