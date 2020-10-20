//=============================================================================
/**
 * @file BattleUISkillButtonPartsLayout.h
 * @brief パーツレイアウトスキル項目ボタンの制御
 * @author yuto_uchida
 * @date 2015.04.15
 */
//=============================================================================
#include "Battle/source/btlv/app/parts/BattleUISkillButtonPartsLayout.h"
#include "Battle/source/btlv/app/parts/BattleUIKoukaIconPartsLayout.h"

#include "AppLib/include/Util/app_util_2d.h"
#include "AppLib/include/Util/app_util_Common.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"

#include "niji_conv_header/message/msg_btl_app.h"

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( parts )

  BattleUISkillButtonPartsLayout::BattleUISkillButtonPartsLayout( ::app::util::G2DUtil* pG2DUtil, s32 lytWkId )
  {
    _Clear();

    GFL_ASSERT( pG2DUtil );
    GFL_ASSERT( 0 <= lytWkId );

    m_pG2DUtil  = pG2DUtil;
    m_lytWkId   = lytWkId;
  }

  BattleUISkillButtonPartsLayout::~BattleUISkillButtonPartsLayout()
  {
    GFL_SAFE_DELETE( m_pKoukaIconPartsLayout );
  }

 /**
  * @brief  初期化処理
  */
  void  BattleUISkillButtonPartsLayout::Initialize( const INIT_PARAM& initParam, const INIT_PANE_PARAM& rInitPaneParam, ::app::tool::AppCommonGrpIconData*  pAppCommonGrpIconData, gfl2::heap::HeapBase* pHeap )
  {
    GFL_ASSERT( pHeap );
    GFL_ASSERT( m_pG2DUtil );
    GFL_ASSERT( pAppCommonGrpIconData );

    m_initParam             = initParam;
    m_initPaneParam         = rInitPaneParam;
    m_pAppCommonGrpIconData = pAppCommonGrpIconData;

    GFL_ASSERT( m_initPaneParam.pNowPPTextBoxPane );
    GFL_ASSERT( m_initPaneParam.pMaxPPNumTextBoxPane );
    GFL_ASSERT( m_initPaneParam.pSkillNameTextBoxPane );
    GFL_ASSERT( m_initPaneParam.pKoukaIconPartsPane );
    GFL_ASSERT( m_initPaneParam.pCursorNullPane );
    GFL_ASSERT( m_initPaneParam.pRootPartsPane );
    GFL_ASSERT( m_initPaneParam.pPPTitleTextPane );

    //  ボタンマネージャーに登録データを作成（パネル用）
    {
      m_panelInfoEx.button_id               = initParam.panel_button_id;
      m_panelInfoEx.picture_pane            = m_initPaneParam.pRootPartsPane;
      GFL_ASSERT( m_panelInfoEx.picture_pane );

      m_panelInfoEx.bound_pane              = m_initPaneParam.pButtonHitBoxPane;
      GFL_ASSERT( m_panelInfoEx.bound_pane );

      m_panelInfoEx.touch_anime_index       = initParam.a_anim_id_list[ ANIM_TYPE_TOUCH        ];  //!< タッチ時のアニメ
      m_panelInfoEx.release_anime_index     = initParam.a_anim_id_list[ ANIM_TYPE_RELEASE      ];  //!< タッチ決定時のアニメ
      m_panelInfoEx.cancel_anime_index      = initParam.a_anim_id_list[ ANIM_TYPE_CANSEL       ];  //!< タッチ状態からのキャンセルアニメ
      m_panelInfoEx.key_select_anime_index  = initParam.a_anim_id_list[ ANIM_TYPE_TOUCH_RELEASE];  //!< キー決定時のアニメ ( touch => release の連続アニメ )
      m_panelInfoEx.active_anime_index      = initParam.a_anim_id_list[ ANIM_TYPE_ACTIVE       ];  //!< アクティブアニメ
      m_panelInfoEx.passive_anime_index     = initParam.a_anim_id_list[ ANIM_TYPE_PASSIVE      ];  //!< パッシブアニメ
    }

    //  ボタンマネージャーに登録データを作成（インフォ用）
    {
      //  パネルの中にある部品パーツを取得
      m_panelInfoIconInfoEx.button_id               = initParam.info_button_id;
      m_panelInfoIconInfoEx.picture_pane            = m_initPaneParam.pInfoIconPartsPane;
      GFL_ASSERT( m_panelInfoIconInfoEx.picture_pane );

      m_panelInfoIconInfoEx.bound_pane              = m_initPaneParam.pInfoIconHitBoxPane;
      GFL_ASSERT( m_panelInfoIconInfoEx.bound_pane );

      m_panelInfoIconInfoEx.touch_anime_index       = initParam.a_anim_id_list[ ANIM_TYPE_INFO_TOUCH        ];  //!< タッチ時のアニメ
      m_panelInfoIconInfoEx.release_anime_index     = initParam.a_anim_id_list[ ANIM_TYPE_INFO_RELEASE      ];  //!< タッチ決定時のアニメ
      m_panelInfoIconInfoEx.cancel_anime_index      = initParam.a_anim_id_list[ ANIM_TYPE_INFO_CANSEL       ];  //!< タッチ状態からのキャンセルアニメ
      m_panelInfoIconInfoEx.key_select_anime_index  = initParam.a_anim_id_list[ ANIM_TYPE_INFO_TOUCH_RELEASE];  //!< キー決定時のアニメ ( touch => release の連続アニメ )
      m_panelInfoIconInfoEx.active_anime_index      = initParam.a_anim_id_list[ ANIM_TYPE_INFO_ACTIVE       ];  //!< アクティブアニメ
      m_panelInfoIconInfoEx.passive_anime_index     = initParam.a_anim_id_list[ ANIM_TYPE_INFO_PASSIVE      ];  //!< パッシブアニメ
    }

    //  効果アイコンのインスタンス作成
    {
      BattleUIKoukaIconPartsLayout::INIT_PARAM partsInitParam  =
      {
      /*
        //  パーツの細かい設定を行うのに必要
        ::app::util::G2DUtil*    p_g2d_util;

        //  パーツが配置されているLayoutId
        s32                     layout_id;

        //  パーツペイン
        gfl2::lyt::LytParts*    p_parts_pane;

        //  有効化アニメidx(アニメ名「active」)
        s32 active_anime_idx;

        //  無効化化アニメidx(アニメ名「passive」)
        s32 passive_anime_idx;

        //  効果なしアニメidx(アニメ名「kouka_02」)
        s32 kouka_none_anime_idx;

        //  効果いまひとつアニメidx(アニメ名「kouka_01」)
        s32 kouka_imahitotsu_anime_idx;

        //  効果ばつぐんアニメidx(アニメ名「kouka_00」)
        s32 kouka_batsugun_anime_idx;

        //  効果無記名アニメ名(アニメ名「kouka_03」)
        s32 kouka_normal_anime_idx;
      */
        m_pG2DUtil,
        m_lytWkId,
        m_initPaneParam.pKoukaIconPartsPane,
        m_initParam.a_anim_id_list[ ANIM_TYPE_KOUKA_ACTIVE ],
        m_initParam.a_anim_id_list[ ANIM_TYPE_KOUKA_PASSIVE ],
        m_initParam.a_anim_id_list[ ANIM_TYPE_KOUKA_KOUKA_02 ],
        m_initParam.a_anim_id_list[ ANIM_TYPE_KOUKA_KOUKA_01 ],
        m_initParam.a_anim_id_list[ ANIM_TYPE_KOUKA_KOUKA_00 ],
        m_initParam.a_anim_id_list[ ANIM_TYPE_KOUKA_KOUKA_03 ],
      };

      m_pKoukaIconPartsLayout = GFL_NEW( pHeap ) parts::BattleUIKoukaIconPartsLayout( partsInitParam );
      GFL_ASSERT( m_pKoukaIconPartsLayout );
    }

    //  PPタイトルの表記
    {
      m_pG2DUtil->SetTextBoxPaneString( m_initPaneParam.pPPTitleTextPane, STR_PP );
    }
  }

  /**
  * @brief  アクティブ設定
  */
  void BattleUISkillButtonPartsLayout::SetActive( const bool bFlg )
  {
    if( bFlg )
    {
      if( m_bAcitve == false )
      {
        m_pG2DUtil->StartAnime( m_lytWkId, m_initParam.a_anim_id_list[ ANIM_TYPE_ACTIVE ] );
      }
    }
    else
    {
      if( m_bAcitve )
      {
        m_pG2DUtil->StartAnime( m_lytWkId, m_initParam.a_anim_id_list[ ANIM_TYPE_PASSIVE ] );
      }
    }

    m_bAcitve = bFlg;
  }

  /**
  * @brief  表示設定
  */
  void BattleUISkillButtonPartsLayout::SetVisible( const bool bFlg )
  {
    m_pG2DUtil->SetPaneVisible( m_initPaneParam.pRootPartsPane, bFlg );
  }

  /**
  * @brief  表示しているか
  */
  bool BattleUISkillButtonPartsLayout::IsVisible()
  {
    return  m_pG2DUtil->GetPaneVisible( m_initPaneParam.pRootPartsPane );
  }

  /**
  * @brief  インフォアイコン表示設定
  */
  void BattleUISkillButtonPartsLayout::SetInfoIconVisible( const bool bFlg )
  {
    m_pG2DUtil->SetPaneVisible( m_initPaneParam.pInfoIconPartsPane, bFlg );
  }

  /**
  * @brief  インフォアイコン表示しているかどうか
  */
  bool BattleUISkillButtonPartsLayout::IsInfoIconVisible()
  {
    return m_pG2DUtil->GetPaneVisible( m_initPaneParam.pInfoIconPartsPane );
  }

  /**
  * @brief  技id設定
  */
  void BattleUISkillButtonPartsLayout::SetId( const WazaID wazaId )
  {
    m_wazaId  = wazaId;

    m_pG2DUtil->SetRegisterWazaName( 0, wazaId );
    m_pG2DUtil->SetTextBoxPaneStringExpand( m_initPaneParam.pSkillNameTextBoxPane, STR_WAZA_NAME );

    //  スキルタイプ
    _SetSkillType( (PokeType_tag)pml::wazadata::GetType( wazaId ) );
  }

  void BattleUISkillButtonPartsLayout::SetId( const WazaID wazaId, gfl2::str::StrBuf* pWazaName )
  {
    GFL_ASSERT( pWazaName );

    m_wazaId  = wazaId;

    m_pG2DUtil->SetTextBoxPaneString( m_initPaneParam.pSkillNameTextBoxPane, pWazaName );

    //  スキルタイプ
    _SetSkillType( (PokeType_tag)pml::wazadata::GetType( wazaId ) );
  }

  /**
  * @brief  PP値の設定
  */
  void BattleUISkillButtonPartsLayout::SetPPNum( const u8 nowNum, const u8 maxNum )
  {
    //  分子
    {
      m_pG2DUtil->SetTextBoxPaneNumber( m_initPaneParam.pNowPPTextBoxPane, STR_VALUE_2, nowNum, 2, 0 );
    }

    //  分母
    {
      m_pG2DUtil->SetTextBoxPaneNumber( m_initPaneParam.pMaxPPNumTextBoxPane, STR_VALUE_2, maxNum, 2, 0 );
    }

    //  色替え( PP値と技名 )
    {
      //  PP
      {
        gfl2::lyt::ColorU8  color = ::app::util::GetWazaTextColor( nowNum, maxNum );
        m_pG2DUtil->SetTextBoxPaneColor( m_initPaneParam.pNowPPTextBoxPane, color, color );
        m_pG2DUtil->SetTextBoxPaneColor( m_initPaneParam.pMaxPPNumTextBoxPane, color, color );
        /** @fix  MMCat[171]   つよさをみる画面のPP値の文字色に関する質問:  "PP","/"も色変更の対象とする  */
        m_pG2DUtil->SetTextBoxPaneColor( m_initPaneParam.pPPLavelTextBoxPane, color, color );
        m_pG2DUtil->SetTextBoxPaneColor( m_initPaneParam.pPPSlashTextBoxPane, color, color );
      }
/*
      [fix]GFNMCat[1911]:技名には文字色を変える必要はなくなった
      //  技名
      {
        gfl2::lyt::ColorU8  color = ::app::util::GetWazaTextColor( nowNum, maxNum, gfl2::lyt::ColorU8( 0, 0, 0, 255 ) );
        m_pG2DUtil->SetTextBoxPaneColor( m_initPaneParam.pSkillNameTextBoxPane, color, color );
      }
*/
    }

    m_nowPPNum  = nowNum;
    m_maxPPNum  = maxNum;
  }

  /**
  * @brief  効果タイプ
  */
  void BattleUISkillButtonPartsLayout::SetKoukaType( const BtlTypeAffAbout koukaType )
  {
    m_pKoukaIconPartsLayout->SetType( koukaType );
  }

  //  効果タイプ表示設定
  void BattleUISkillButtonPartsLayout::SetKoukaTypeVisible( const bool bFlg )
  {
    m_pKoukaIconPartsLayout->SetVisible( bFlg );
  }

  /**
  * @brief  スキルタイプ
  */
  void BattleUISkillButtonPartsLayout::_SetSkillType( PokeType_tag skillType )
  {
    //  パネルの色決め
    {
      //  タイプ列挙とアニメ列挙との関連テーブル
      const s32 aColorAnimTable[ POKETYPE_MAX ] =
      {
        ANIM_TYPE_COLOR_00,       //  ノーマル色
        ANIM_TYPE_COLOR_01,       //  かくとう色
        ANIM_TYPE_COLOR_02,       //  ひこう色
        ANIM_TYPE_COLOR_03,       //  どく色
        ANIM_TYPE_COLOR_04,       //  じめん色
        ANIM_TYPE_COLOR_05,       //  いわ色
        ANIM_TYPE_COLOR_06,       //  むし色
        ANIM_TYPE_COLOR_07,       //  ゴースト色
        ANIM_TYPE_COLOR_08,       //  はがね色
        ANIM_TYPE_COLOR_09,       //  ほのお色
        ANIM_TYPE_COLOR_10,       //  みず色
        ANIM_TYPE_COLOR_11,       //  くさ色
        ANIM_TYPE_COLOR_12,       //  でんき色
        ANIM_TYPE_COLOR_13,       //  エスパー色
        ANIM_TYPE_COLOR_14,       //  こおり色
        ANIM_TYPE_COLOR_15,       //  ドラゴン色
        ANIM_TYPE_COLOR_16,       //  あく色
        ANIM_TYPE_COLOR_17,       //  フェアリー色
      };

      m_pG2DUtil->StartAnime( m_lytWkId, m_initParam.a_anim_id_list[ aColorAnimTable[ skillType ] ] );
    }

    //  パネルのタイプアイコンのテクスチャーを差し替えする
    {
      GFL_ASSERT( m_initPaneParam.pTypeIconPicPane );
      m_pAppCommonGrpIconData->ReplacePaneTextureByWazaTypeIcon( skillType, m_initPaneParam.pTypeIconPicPane );
    }
  }

GFL_NAMESPACE_END( parts )
GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
