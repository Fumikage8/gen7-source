//======================================================================
/**
 * @file TeamSelectPokePartyLayoutParts.h
 * @date 2015/07/03 20:36:00
 * @author uchida_yuto
 * @brief チーム選択のポケモンパーティレイアウトパーツ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_POKE_PARTY_LAYOUT_PARTS_H_INCLUDED__
#define __TEAM_SELECT_POKE_PARTY_LAYOUT_PARTS_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>

// pmlib
#include <pml/include/pmlib.h>

//  前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class G2DUtil;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class PokeIcon;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  /**
   * @class TeamSelectPokePartyLayoutParts
   * @brief チーム選択のポケモンパーティレイアウトパーツ
   */
  class TeamSelectPokePartyLayoutParts
  {
    GFL_FORBID_COPY_AND_ASSIGN(TeamSelectPokePartyLayoutParts);
  public:

    //  オブジェクトのアニメ一覧
    enum AnimeId
    {
      //  パーティー表示
      ANIME_ID_POKE_PARTY_VISIBLE  = 0,
      //  QR表示
      ANIME_ID_QR_VISIBLE,

      //  チームの色
      ANIME_ID_TEME_01_COLOR,
      ANIME_ID_TEME_02_COLOR,
      ANIME_ID_TEME_03_COLOR,
      ANIME_ID_TEME_04_COLOR,
      ANIME_ID_TEME_05_COLOR,
      ANIME_ID_TEME_06_COLOR,

      ANIME_ID_MAX
    };

    //  ポケモンアイコンアニメ一覧
    enum PokeIconAnimeId
    {
      POKE_ICON_ANIME_ID_DIE,
      POKE_ICON_ANIME_ID_REVIVE,
      POKE_ICON_ANIME_ID_ITEM_CATCH,
      POKE_ICON_ANIME_ID_ITEM_RELEASE,
      POKE_ICON_ANIME_ID_JAMP_HIGH,
      POKE_ICON_ANIME_ID_JAMP_MIDDLE,
      POKE_ICON_ANIME_ID_JAMP_LOW,
      POKE_ICON_ANIME_ID_SICK,
      POKE_ICON_ANIME_ID_JAMP_SELECT,
      POKE_ICON_ANIME_ID_STOP,

      //  性別表示
      POKE_ICON_ANIME_ID_MALE,
      POKE_ICON_ANIME_ID_FEMALE,

      POKE_ICON_ANIME_ID_MAX
    };

    //  レイアウトに必要なアニメデータ
    struct LAYOUT_ANIME_DATA
    {
      //  アニメid一覧
      u32 aAnimeIdx[ ANIME_ID_MAX ];

      //  ポケモンアイコンアニメid一覧
      u32 aPokeIconAnimeId[ pml::PokeParty::MAX_MEMBERS ][ POKE_ICON_ANIME_ID_MAX ];
    };

    //  レイアウトの外部設定
    struct LAYOUT_PARAM
    {
      LAYOUT_PARAM()
      {
        lytWorkId           = 0;
        pG2dUtil            = NULL;
        pPokeIconTexManager = NULL;
        pRootViewPartsPane  = NULL;
        pLayoutWork         = NULL;
        pLytMuitiResID      = NULL;
      }

      u32                           lytWorkId;
      app::util::G2DUtil*           pG2dUtil;
      gfl2::lyt::LytWk*             pLayoutWork;
      gfl2::lyt::LytMultiResID*     pLytMuitiResID;
      app::tool::PokeIcon*          pPokeIconTexManager;

      //  パーツペイン
      gfl2::lyt::LytParts*          pRootViewPartsPane;

      struct LAYOUT_ANIME_DATA      animeData;
    };

    //  コンストラクタ
    TeamSelectPokePartyLayoutParts();

    //------------------------------------------------------------------
    /**
      * @brief      動作に必要な引数を登録
      * @param[in]  チームの描画なパラメータ
      * @param[in]  レイアウト描画に必要なパラメータ
      * @note
      *    必ず最初に呼び出さないと後の動作保障はなし
      */
    //------------------------------------------------------------------
    void Initialize( struct LAYOUT_PARAM* pLayoutParam );

    //------------------------------------------------------------------
    /**
      * @brief      レイアウト内容を設定
      * @param[in]  チームの描画なパラメータ
      * @param[in]  QRリーダーチェックをするか
      */
    //------------------------------------------------------------------
    void SetViewParam( const struct TEAM_VIEW_PARAM* pTeamViewParam, const bool bChkQRReader );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      NGフレームを表示させるか
    * @param[in]  pokePartyIdx  ポケモンパーティidx
    * @param[in]  bFlg          NGかどうか
    */
    //--------------------------------------------------------------------------------------------
    void SetEnableNGFrame( const u32 pokePartyIdx, const bool bFlg );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      全ポケモン表示にＮＧフレームを設定をキャンセル
    */
    //--------------------------------------------------------------------------------------------
    void CancelAllNGFrame();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      ルートオブジェクトのY座標設定
    * @param[in]  yPos  : 設定するY座標
    */
    //--------------------------------------------------------------------------------------------
    void SetYPos( const f32 yPos );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      設定したレイアウトパラメータを取得
    * @return     レイアウトパラメータ
    */
    //--------------------------------------------------------------------------------------------
    const struct LAYOUT_PARAM& GetLayoutParam() const
    {
      return  m_layoutParam;
    }

    //  初期デフォルト座標
    const gfl2::math::Vector3& GetDefaultPos() const { return m_defaultPos; }

  private:
    //  ポケモンアイコンオブジェクトの外部設定
    //  ペインやアニメの設定
    struct LAYOUT_POKE_ICON_PARAM
    {
      LAYOUT_POKE_ICON_PARAM()
      {
        pPokeIconPartsPane  = NULL;
        pPokeIconPicture    = NULL;
        pHaveItemIconPane   = NULL;
        pGenderPartsPane    = NULL;
        pLvTitleTextPane    = NULL;
        pLvNumberTextBox    = NULL;
        pMaskPane           = NULL;
        pNGIconPane         = NULL;
      }

      //  ポケモンアイコンのペイン関連
      gfl2::lyt::LytParts*    pPokeIconPartsPane;
      gfl2::lyt::LytPicture*  pPokeIconPicture;
      gfl2::lyt::LytPane*     pHaveItemIconPane;

      //  性別のペイン関連
      gfl2::lyt::LytParts*    pGenderPartsPane;

      //  レベルタイトルペイン
      gfl2::lyt::LytTextBox*  pLvTitleTextPane;

      //  レベルのペイン関連
      gfl2::lyt::LytTextBox*  pLvNumberTextBox;

      //  マスクのペイン関連
      gfl2::lyt::LytPane*     pMaskPane;

      //  NGアイコンのペイン
      gfl2::lyt::LytPane*     pNGIconPane;
    };

    //  指定したチームがロック中か
    bool _IsLoak( const struct TEAM_DATA* pTeamData );

    void _Clear()
    {
      m_pTeamNameTextBoxPane  = NULL;
      m_pQRMessageTextBoxPane = NULL;
      m_pLockIconPane         = NULL;
    }

    struct LAYOUT_POKE_ICON_PARAM m_aPokeIconParam[ pml::PokeParty::MAX_MEMBERS ];

    struct LAYOUT_PARAM m_layoutParam;

    gfl2::lyt::LytTextBox*  m_pTeamNameTextBoxPane;
    gfl2::lyt::LytTextBox*  m_pQRMessageTextBoxPane;
    gfl2::lyt::LytPane*     m_pLockIconPane;
    gfl2::math::Vector3     m_defaultPos;
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_POKE_PARTY_LAYOUT_PARTS_H_INCLUDED__