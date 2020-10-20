//======================================================================
/**
 * @file TeamSelectUppEntryPokePartyView.h
 * @date 2015/07/02 19:29:18
 * @author uchida_yuto
 * @brief チーム選択の上画面選択中のポケモンパーティView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_UPP_ENTRY_POKE_PARTY_VIEW_H_INCLUDED__
#define __TEAM_SELECT_UPP_ENTRY_POKE_PARTY_VIEW_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/ui/UIView.h"

#include "NetApp/TeamSelect/source/Parts/TeamSelectPokePartyLayoutParts.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  /**
   * @class TeamSelectUppEntryPokePartyView
   * @brief チーム選択の上画面選択中のポケモンパーティView
   */
  class TeamSelectUppEntryPokePartyView : public app::ui::UIView
  {
      GFL_FORBID_COPY_AND_ASSIGN(TeamSelectUppEntryPokePartyView);

  public:
    /**
     * @fn
     * ここに関数の説明を書く
     * @brief 要約説明
     * @param[in] var1 var1の説明
     * @param[out] var2 var2の説明
     * @param[in,out] var3 var3の説明
     * @return 戻り値の説明
     * @sa 参照すべき関数を書けばリンクが貼れる
     * @detail 詳細な説明
     */

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      * @param[in] pHeap 使用するメモリ
      */
    //------------------------------------------------------------------
    TeamSelectUppEntryPokePartyView( app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderManager );

    virtual ~TeamSelectUppEntryPokePartyView();

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    void Initialize( void* pLytBinData, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet, app::tool::PokeIcon* pPokeIconTexManager );

    //------------------------------------------------------------------
    /**
      * @brief   終了処理
      */
    //------------------------------------------------------------------
    void Terminate();

    //------------------------------------------------------------------
    /**
      * @brief   破棄可能か
      */
    //------------------------------------------------------------------
    bool IsDelete();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      描画パラメータを設定反映
    * @param[in]  描画パラメータ
    * @param[in]  QRリーダーチェックをするか
    */
    //--------------------------------------------------------------------------------------------
    void SetViewParam( const struct TEAM_VIEW_PARAM* pViewParam, const bool bChkQRReader );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      ポケモン表示にNGフレームを表示させるか
    * @param[in]  pokePartyIdx  ポケモンパーティidx
    * @param[in]  bDrawFlg      表示させるかどうか
    */
    //--------------------------------------------------------------------------------------------
    void SetEnableNGFrame( const u32 pokePartyIdx, const bool bDrawFlg );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      ポケモンパーティーのフレームのオブジェクトY座標設定
    * @param[in]  yPos : Y座標設定
    */
    //--------------------------------------------------------------------------------------------
    void SetYPosPokePartyFrame( const f32 yPos );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      デフォルトの座標取得
    */
    //--------------------------------------------------------------------------------------------
    const gfl2::math::Vector3 GetDefaultPos() const { return m_defaultPos; }

  private:
    //  作成するレイアウトワークの種類
    enum
    {
      LYT_WORK_NORMAL  = 0,
      LYT_WORK_MAX
    };

    //------------------------------------------------------------------
    /**
      * @brief   2D画面の作成
      * @param   pAppLytBuff レイアウトデータのバイナリポインタ
      */
    //------------------------------------------------------------------
    void _Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet );

    //------------------------------------------------------------------
    /**
      * @brief   変数の初期化はここに
      */
    //------------------------------------------------------------------
    void _Clear()
    {
      m_pHeap             = NULL;
      m_pRenderManager    = NULL;
      m_pPokePartyLayout  = NULL;
    }

    app::util::Heap*                m_pHeap;
    app::util::AppRenderingManager* m_pRenderManager;

    TeamSelectPokePartyLayoutParts* m_pPokePartyLayout;

    gfl2::math::Vector3 m_defaultPos;
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_UPP_ENTRY_POKE_PARTY_VIEW_H_INCLUDED__