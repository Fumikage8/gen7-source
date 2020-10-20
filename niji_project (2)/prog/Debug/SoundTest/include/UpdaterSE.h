//======================================================================
/**
 * @file UpdaterSE.h
 * @date 2015/10/29 12:00:31
 * @author saito_nozomu
 * @brief 更新処理クラス（SE用）
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#if !defined __UPDATER_SE_H_INCLUDED__
#define __UPDATER_SE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "Debug/SoundTest/include/SoundContainer.h"

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(sound)

  /**
   * @class UpdaterBase
   * @brief ME更新クラス
   */
  class UpdaterSE : public UpdaterBase
  {
      GFL_FORBID_COPY_AND_ASSIGN(UpdaterSE);

  public:
    /**
     * @brief コンストラクタ
     * @param param セットアップパラメータ
     */
    UpdaterSE(SetupParam &param);

    /**
     * @brief デストラクタ
     */
    virtual ~UpdaterSE();

    /**
     * @brief Aボタン押下時の処理
     */
    virtual void FuncButtonA(void);

    /**
     * @brief Bボタン押下時の処理
     */
    virtual void FuncButtonB(void);

    /**
     * @brief 右キー押下時の処理
     */
    virtual void FuncKeyRight(void);

    /**
     * @brief 左キー押下時の処理
     */
    virtual void FuncKeyLeft(void);

    /**
     * @brief タッチ時の処理
     * @param x Ｘ座標
     * @param y Ｙ座標
     */
    virtual void FuncTouch(int x, int y);

    /*
     * @brief ページ変更時処理
     */
    virtual void ChangePageFunc(void);

    /**
     * @brief 上画面描画
     * @param pDrawUtilText  テキストユーティリティクラスへのポインタ
     */
    virtual void DrawTop(gfl2::util::DrawUtilText* pDrawUtilText);

    /**
     * @brief 下画面描画
     * @param pDrawUtilText  テキストユーティリティクラスへのポインタ
     */
    virtual void DrawBottom(gfl2::util::DrawUtilText* pDrawUtilText);

  private:
    /**
     * @brief 表示情報の変更
     */
    void ChangeDispInfo(void);

    gfl2::str::StrBuf* m_pInfoStrTop;     //!メッセージバッファ（上画面）
    gfl2::str::StrBuf* m_pInfoStrBottom;  //!メッセージバッファ（下画面）
  };

GFL_NAMESPACE_END(sound)
GFL_NAMESPACE_END(debug)

#endif // __UPDATER_SE_H_INCLUDED__

#endif // PM_DEBUG