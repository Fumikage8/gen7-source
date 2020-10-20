//======================================================================
/**
 * @file UpdaterPVZukan.h
 * @date 2015/10/29 12:00:31
 * @author saito_nozomu
 * @brief 更新処理クラス（図鑑順ポケモン鳴き声用）
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#if !defined __UPDATER_PV_ZUKAN_H_INCLUDED__
#define __UPDATER_PV_ZUKAN_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "Debug/SoundTest/include/SoundContainer.h"

GFL_NAMESPACE_BEGIN(debug)
class NumInput;
GFL_NAMESPACE_BEGIN(sound)

  /**
   * @class UpdaterBase
   * @brief PV更新クラス
   */
  class UpdaterPVZukan : public UpdaterBase
  {
      GFL_FORBID_COPY_AND_ASSIGN(UpdaterPVZukan);

  public:
    /**
     * @brief コンストラクタ
     * @param param セットアップパラメータ
     * @param pContainer サウンドコンテナーへのポインタ
     */
    UpdaterPVZukan(SetupParam &param, SoundContainer *pContainer);

    /**
     * @brief デストラクタ
     */
    virtual ~UpdaterPVZukan();

    /**
     * @brief 更新処理
     */
    virtual bool UpdateFunc(void);

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

    /**
     * @brief 上キー押下時の処理
     */
    virtual void FuncKeyUp(void);

    /**
     * @brief 下キー押下時の処理
     */
    virtual void FuncKeyDown(void);

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

    /**
     * @brief 検索後処理
     * @param page ページ
     */
    virtual void SearchAfterFunc(Page &page);

    /**
     * @brief ページ切り替え可能か
     * @return bool true で可能
     */
    virtual bool CanPageChange(void);

  private:
    /**
     * @brief タッチ時の処理
     * @param id
     */
    void Touch(int id);

    /**
     * @brief 名前で検索
     */
    void SearchByName(void);

    /**
     * @brief ナンバーで検索
     */
    void SearchByNo(void);

    /**
     * @brief 表示情報の変更
     */
    void ChangeDispInfo(void);

    SoundContainer *m_pContainer;
    int m_PokeNo;
    int m_PokeCursor;
    debug::NumInput* m_pNumInput; //! 数字入力クラスへのポインタ

    gfl2::str::StrBuf* m_pInfoStrTopVoice[POKE_VOICE_NUM];     //!メッセージバッファ（上画面）
    gfl2::str::StrBuf* m_pInfoStrTop;     //!メッセージバッファ（上画面）
    gfl2::str::StrBuf* m_pInfoStrBottomZukan;  //!メッセージバッファ（下画面）
    gfl2::str::StrBuf* m_pInfoStrBottomName;  //!メッセージバッファ（下画面）
    int m_VoiceCount;                         //!１匹のポケモンが持つ鳴き声の数
    bool m_InputDel;                         //! インプットモジュールを削除するかのフラグ（１フレ開放対策）
  };

GFL_NAMESPACE_END(sound)
GFL_NAMESPACE_END(debug)

#endif // __UPDATER_PV_ZUKAN_H_INCLUDED__

#endif // PM_DEBUG