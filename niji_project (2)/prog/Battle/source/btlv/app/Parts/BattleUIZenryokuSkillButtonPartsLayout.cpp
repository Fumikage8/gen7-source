//=============================================================================
/**
 * @file BattleUIZenryokuSkillButtonPartsLayout.h
 * @brief パーツレイアウト全力スキル項目ボタンの制御
 * @author yuto_uchida
 * @date 2015.04.28
 */
//=============================================================================
#include "Battle/source/btlv/app/parts/BattleUIZenryokuSkillButtonPartsLayout.h"
#include "Battle/source/btlv/app/parts/BattleUIKoukaIconPartsLayout.h"

//  nijiのインクルード
#include "AppLib/include/Util/app_util_2d.h"
#include "AppLib/include/Util/app_util_Common.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"

#include "niji_conv_header/message/msg_btl_app.h"

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( parts )

  BattleUIZenryokuSkillButtonPartsLayout::BattleUIZenryokuSkillButtonPartsLayout( ::app::util::G2DUtil* pG2DUtil, s32 lytWkId )
  {
    _Clear();

    GFL_ASSERT( pG2DUtil );
    GFL_ASSERT( 0 <= lytWkId );

    m_pG2DUtil  = pG2DUtil;
    m_lytWkId   = lytWkId;
  }


  BattleUIZenryokuSkillButtonPartsLayout::~BattleUIZenryokuSkillButtonPartsLayout()
  {
    GFL_SAFE_DELETE( m_pKoukaIconPartsLayout );
  }

 /**
  * @brief  初期化処理
  */
  void  BattleUIZenryokuSkillButtonPartsLayout::Initialize( const INIT_PARAM& initParam, const INIT_PANE_PARAM& rInitPaneParam, ::app::tool::AppCommonGrpIconData* pAppCommonGrpIconData, gfl2::heap::HeapBase* pHeap )
  {
    GFL_ASSERT( pHeap );
    GFL_ASSERT( m_pG2DUtil );
    GFL_ASSERT( pAppCommonGrpIconData );

    m_initParam             = initParam;
    m_initPaneParam         = rInitPaneParam;
    m_pAppCommonGrpIconData = pAppCommonGrpIconData;

    GFL_ASSERT( m_initPaneParam.pSkillNameTextBoxPane );
    GFL_ASSERT( m_initPaneParam.pKoukaIconPartsPane );
    GFL_ASSERT( m_initPaneParam.pCursorNullPane );
    GFL_ASSERT( m_initPaneParam.pRootPartsPane );

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

    //  項目名の元のテキスト色を取得
    {
      m_pG2DUtil->GetTextBoxPaneColor( m_initPaneParam.pSkillNameTextBoxPane, &m_titleTopColor, &m_titleBottomColor );
    }
  }

  /**
  * @brief  アクティブ設定
  */
  void BattleUIZenryokuSkillButtonPartsLayout::SetActive( const bool bFlg )
  {
    bool  bNewActive = bFlg;
    // @fix NMCat[965] 所持ＰＰが０の場合はエフェクトは出さない
    if( m_bPPZero == true )
    {
      bNewActive  = false;
    }

    if( bNewActive )
    {
      if( m_bAcitve == false )
      {
//        m_pG2DUtil->StartAnime( m_lytWkId, m_initParam.a_anim_id_list[ ANIM_TYPE_ACTIVE ] );
        if( m_bPowerEffect == true )
        {
          m_pG2DUtil->StartAnime( m_lytWkId, m_initParam.a_anim_id_list[ ANIME_TYPE_POWER_EFFECT ] );
        }
        else
        if( m_bNormalEffect == true )
        {
          m_pG2DUtil->StartAnime( m_lytWkId, m_initParam.a_anim_id_list[ ANIME_TYPE_NORMAL_EFFECT ] );
        }
      }
    }
    else
    {
      if( m_bAcitve )
      {
        // @fix NMCat[965] パッシブにするとボタンが押せなくなるのでパッシブアニメは再生しない
//        m_pG2DUtil->StartAnime( m_lytWkId, m_initParam.a_anim_id_list[ ANIM_TYPE_PASSIVE ] );
        if( m_bPowerEffect == true )
        {
          m_pG2DUtil->StopAnime( m_lytWkId, m_initParam.a_anim_id_list[ ANIME_TYPE_POWER_EFFECT ] );
        }
        else
        if( m_bNormalEffect == true )
        {
          m_pG2DUtil->StopAnime( m_lytWkId, m_initParam.a_anim_id_list[ ANIME_TYPE_NORMAL_EFFECT ] );
        }
      }
    }

    // @fix NMCat[965] アニメ設定すると設定したカラー値が元に戻るので、ここで再設定する
    m_pG2DUtil->SetTextBoxPaneColor( m_initPaneParam.pSkillNameTextBoxPane, m_nameTopColor, m_nameBottomColor );

    m_bAcitve = bNewActive;
  }

  /**
  * @brief  表示設定
  */
  void BattleUIZenryokuSkillButtonPartsLayout::SetVisible( const bool bFlg )
  {
    m_pG2DUtil->SetPaneVisible( m_initPaneParam.pRootPartsPane, bFlg );
  }

  /**
  * @brief  表示しているか
  */
  bool BattleUIZenryokuSkillButtonPartsLayout::IsVisible()
  {
    return  m_pG2DUtil->GetPaneVisible( m_initPaneParam.pRootPartsPane );
  }

  /**
  * @brief  インフォアイコン表示設定
  */
  void BattleUIZenryokuSkillButtonPartsLayout::SetInfoIconVisible( const bool bFlg )
  {
    m_pG2DUtil->SetPaneVisible( m_initPaneParam.pInfoIconPartsPane, bFlg );
  }

  /**
  * @brief  インフォアイコン表示しているかどうか
  */
  bool BattleUIZenryokuSkillButtonPartsLayout::IsInfoIconVisible()
  {
    return m_pG2DUtil->GetPaneVisible( m_initPaneParam.pInfoIconPartsPane );
  }

  /**
  * @brief 技名設定
  * @note id設定と同時に技名の表示設定をしている
  */
  void BattleUIZenryokuSkillButtonPartsLayout::SetName( const WazaID wazaId, const gfl2::str::StrBuf* pName, const bool bPPZero )
  {
    GFL_ASSERT( pName );

    m_wazaId  = wazaId;

    m_nameBottomColor = m_titleTopColor;
    m_nameTopColor    = m_titleBottomColor;

    if( bPPZero )
    {
      //  テキスト色を赤に
      m_nameTopColor = m_nameBottomColor  = gfl2::lyt::ColorU8( 255, 0, 0, 255 );
    }
    m_bPPZero = bPPZero;

    //  名前を設定
    {
      m_pG2DUtil->SetRegisterWord( 0, *pName );
      m_pG2DUtil->SetTextBoxPaneStringExpand( m_initPaneParam.pSkillNameTextBoxPane, STR_WAZA_NAME );

      m_pG2DUtil->SetTextBoxPaneColor( m_initPaneParam.pSkillNameTextBoxPane, m_nameTopColor, m_nameBottomColor );
    }
  }

  /**
  * @brief  効果タイプ
  */
  void BattleUIZenryokuSkillButtonPartsLayout::SetKoukaType( const BtlTypeAffAbout koukaType )
  {
    m_pKoukaIconPartsLayout->SetType( koukaType );
  }

  /**
  * @brief  効果タイプ表示
  */
  void BattleUIZenryokuSkillButtonPartsLayout::SetKoukaTypeVisible( const bool bFlg )
  {
    m_pKoukaIconPartsLayout->SetVisible( bFlg );
  }

  //  技タイプ
  void BattleUIZenryokuSkillButtonPartsLayout::SetWazaType( const PokeType_tag type )
  {
    //  スキルタイプ
    _SetSkillType( type );
  }

  /**
  * @brief  スキルタイプ
  */
  void BattleUIZenryokuSkillButtonPartsLayout::_SetSkillType( PokeType_tag skillType )
  {
    //  パネルのタイプアイコンのテクスチャーを差し替えする
    {
      GFL_ASSERT( m_initPaneParam.pTypeIconPicPane );
      m_pAppCommonGrpIconData->ReplacePaneTextureByWazaTypeIcon( skillType, m_initPaneParam.pTypeIconPicPane );
    }
  }

GFL_NAMESPACE_END( parts )
GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
