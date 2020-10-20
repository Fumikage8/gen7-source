//======================================================================
/**
 * @file UpdaterPika.h
 * @date 2015/11/02 21:53:15
 * @author saito_nozomu
 * @brief 更新処理クラス（ピカチュウ鳴き声用）
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#if !defined __UPDATER_PIKA_H_INCLUDED__
#define __UPDATER_PIKA_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "Debug/SoundTest/include/SoundContainer.h"

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(sound)

  /**
   * @class UpdaterBase
   * @brief ピカチュウ鳴き声更新クラス
   */
  class UpdaterPika : public UpdaterBase
  {
      GFL_FORBID_COPY_AND_ASSIGN(UpdaterPika);

  public:
    /**
     * @brief コンストラクタ
     * @param param セットアップパラメータ
     * @param pContainer サウンドコンテナーへのポインタ
     */
    UpdaterPika(SetupParam &param);

    /**
     * @brief デストラクタ
     */
    virtual ~UpdaterPika();

    /**
     * @brief Aボタン押下時の処理
     */
    virtual void FuncButtonA(void);

    /**
     * @brief 右キー押下時の処理
     */
    virtual void FuncKeyRight(void);

    /**
     * @brief 左キー押下時の処理
     */
    virtual void FuncKeyLeft(void);

    /*
     * @brief ページ変更時処理
     */
    virtual void ChangePageFunc(void);

    /**
     * @brief 上画面描画
     * @param pDrawUtilText  テキストユーティリティクラスへのポインタ
     */
    virtual void DrawTop(gfl2::util::DrawUtilText* pDrawUtilText);

  private:
    /**
     * @brief 表示情報の変更
     */
    void ChangeDispInfo(void);

    gfl2::str::StrBuf* m_pInfoStrTop;     //!メッセージバッファ（上画面）
  };

GFL_NAMESPACE_END(sound)
GFL_NAMESPACE_END(debug)

#endif // __UPDATER_PIKA_H_INCLUDED__

#endif //PM_DEBUG