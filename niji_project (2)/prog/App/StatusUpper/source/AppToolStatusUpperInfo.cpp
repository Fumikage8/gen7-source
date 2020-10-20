//======================================================================
/**
 * @file    AppToolStatusUpperInfo.cpp
 * @date    2015/12/21 18:55:47
 * @author  fukushima_yuya
 * @brief   フィールドステータス：ポケモン情報(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "AppToolStatusUpperInfo.h"

#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <AppLib/include/Tool/AppToolGaugePartsLayout.h>
#include <PokeTool/include/PokeTool.h>
#include <PokeTool/include/PokeToolExtendData.h>

// pml
#include <pml/include/pmlib.h>

// arc
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>
#include <niji_conv_header/message/msg_pokemon_status.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FieldStatusUpperInfo::FieldStatusUpperInfo( CreateParam& param )
  : m_CreateParam(param)
  , m_pPokeParam(NULL)
  , m_pCommonIcon(param.cmnIcon)
  , m_pTrainerIcon(param.trainerIconPtr)
  , m_pExpGauge(NULL)
  , m_pTextZukanNo(NULL)
  , m_pTextPokeName(NULL)
  , m_pTextPlayerName(NULL)
  , m_pTextIDNo(NULL)
  , m_pTextExpTotal(NULL)
  , m_pTextExpNext(NULL)
  , m_pLangIcon(NULL)
  , m_pTrainerIconPicture(NULL)
  , m_pTrainerIconVisibilityControl(NULL)
  , m_pTrainerIconFrame(NULL)
  , m_pTextTrainerName(NULL)
  , m_pTextBFCGrade(NULL)
{
  for( u8 i=0; i<POKE_TYPE_ICON_MAX; ++i )
  {
    m_pTypeIconParts[i] = NULL;
    m_pTypeIconPane[i]  = NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
FieldStatusUpperInfo::~FieldStatusUpperInfo( void )
{
  GFL_SAFE_DELETE( m_pExpGauge );
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void FieldStatusUpperInfo::Initialize( void )
{
  // EXPゲージの生成
  CreateExpGauge();

  // ペインの設定
  {
    gfl2::lyt::LytWk* lytwk        = m_CreateParam.lytwk;
    gfl2::lyt::LytMultiResID resID = m_CreateParam.resID;

    // ずかんNo
    m_pTextZukanNo    = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_00 );
    // ポケモン種族名
    m_pTextPokeName   = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_01 );
    // プレイヤー名
    m_pTextPlayerName = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_02 );
    // IDNo
    m_pTextIDNo       = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_03 );
    // けいけんち
    m_pTextExpTotal   = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_04 );
    // つぎのレベルまで
    m_pTextExpNext    = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_06 );

    // 言語アイコン
    gfl2::lyt::LytParts* langIconParts = lytwk->GetPartsPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARTS_LANG );
    m_pLangIcon = lytwk->GetPicturePane( langIconParts, PANENAME_COMMON_ICN_008_PANE_BG_00, &resID );

    // トレーナー名/BFCグレード
    m_pTrainerIconPicture           = lytwk->GetPicturePane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PLAYERPHOTO );
    m_pTrainerIconVisibilityControl = lytwk->GetPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_NULL_TRAINERICON );
    m_pTrainerIconFrame             = lytwk->GetPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_WINDOW_PLAYER_FRAME );

    m_pTextTrainerName  = lytwk->GetTextBoxPane(PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXT_TRAINER);
    m_pTextBFCGrade     = lytwk->GetTextBoxPane(PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXT_PLNAME);

    // タイプアイコン
    const u32 c_TypeIconPartsIndex[] = {
      PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARTSTYPE_00,
      PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARTSTYPE_01,
    };

    for( u8 i=0; i<POKE_TYPE_ICON_MAX; ++i )
    {
      m_pTypeIconParts[i] = lytwk->GetPartsPane( c_TypeIconPartsIndex[i] );
      m_pTypeIconPane[i]  = lytwk->GetPicturePane( m_pTypeIconParts[i], PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &resID );
    }
  }

  // 初期テキスト
  InitializeText();
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト
 */
//------------------------------------------------------------------------------
void FieldStatusUpperInfo::InitializeText( void )
{
  gfl2::lyt::LytWk* lytwk = m_CreateParam.lytwk;
  app::ui::UIView* view   = m_CreateParam.view;

  // 項目名
  const u32 c_TextHeadIndex[] = {
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTHEAD_00,  /**<  図鑑No.         */
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTHEAD_01,  /**<  名前            */
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTHEAD_02,  /**<  タイプ          */
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTHEAD_03,  /**<  親/性格(BFC)    */
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTHEAD_04,  /**<  IDNo.           */
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTHEAD_05,  /**<  現在の経験値    */
    PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTHEAD_06,  /**<  次のレベルまで  */
  };
  const u32 c_TextHeadMsgID[] = {
    PARAM_ZUKAN_NO,
    PARAM_NAME,
    PARAM_TYPE,
    m_CreateParam.isBFCLayout ? PARAM_NATURE : PARAM_OYA,
    PARAM_ID_NO,
    PARAM_EXP,
    PARAM_EXP_NEXT_01,
  };
  for( u32 i=0; i<GFL_NELEMS(c_TextHeadIndex); ++i )
  {
    gfl2::lyt::LytTextBox* textbox = lytwk->GetTextBoxPane( c_TextHeadIndex[i] );
    view->SetTextboxPaneMessage( textbox, c_TextHeadMsgID[i] );
  }

  //「あと」
  gfl2::lyt::LytTextBox* expText = lytwk->GetTextBoxPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_TEXTVALUE_05 );
  view->SetTextboxPaneMessage( expText, PARAM_EXP_NEXT_02 );
}

//------------------------------------------------------------------------------
/**
 * @brief   EXPゲージの生成
 */
//------------------------------------------------------------------------------
void FieldStatusUpperInfo::CreateExpGauge( void )
{
  gfl2::lyt::LytWk* lytwk       = m_CreateParam.lytwk;
  app::ui::UIView* view         = m_CreateParam.view;
  gfl2::heap::HeapBase* devHeap = m_CreateParam.heap->GetDeviceHeap();

  app::tool::AppToolGaugePartsLayout::INIT_PARAM param;
  {
    param.p_g2d_util         = view->GetG2DUtil();
    param.layout_id          = 0;
    param.p_parts_pane       = lytwk->GetPartsPane( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARTS_EXP );
    param.gauge_hp_anime_idx = LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_EXP_GAUGE_00;
  }

  m_pExpGauge = GFL_NEW(devHeap) app::tool::AppToolGaugePartsLayout( param, devHeap );
  GFL_ASSERT( m_pExpGauge );
}


//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void FieldStatusUpperInfo::Setup( const pml::pokepara::PokemonParam* pp, const BattleFesTool::BFCTrainerInfo* pBFCTrainerInfo )
{
  GFL_ASSERT( pp );

  //if( m_pPokeParam != pp )
  {
    app::ui::UIView* view    = m_CreateParam.view;
    print::WordSet*  wordSet = m_CreateParam.wordSet;

    bool fastmode_flag = pp->StartFastMode();
    {
      app::util::G2DUtil* g2d = m_CreateParam.view->GetG2DUtil();
      {
        u32 animID;

        if( PokeTool::ExtendData::IsExistLocalZukanStatic( pp->GetMonsNo(), PokeTool::ExtendData::LOCAL_AREA_A ) )
        {
          /*  通常時  */
          u32 localNo = PokeTool::ExtendData::GetLocalNumberStatic( pp->GetMonsNo(), PokeTool::ExtendData::LOCAL_AREA_A );
          view->SetTextboxPaneNumber( m_pTextZukanNo, PARAM_VALUE_3, localNo, 3, 0, print::NUM_DISP_ZERO );

          /*  pattern  */
          if(m_CreateParam.isBFCLayout)
          {
            animID = pBFCTrainerInfo ? LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_ZUKAN_01 : LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_ZUKAN_2;
          }
          else
          {
            animID  = LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_ZUKAN_1;
          }
        }
        else {
          /*  図鑑番号がない  */

          /*  pattern  */
          if(m_CreateParam.isBFCLayout)
          {
            animID = pBFCTrainerInfo ? LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_ZUKAN_02 : LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_ZUKAN_00;
          }
          else
          {
            animID  = LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000_ZUKAN_0;
          }
        }

        g2d->StartAnime( 0, animID );
      }
      
      // ポケモン種族名
      wordSet->RegisterPokeMonsNameOrTamago( 0, pp );
      view->SetTextboxPaneMessage( m_pTextPokeName, PARAM_POKE_NAME );
      
      // プレイヤー名(おや) / 性格
      {
        if(m_CreateParam.isBFCLayout)
        {
          wordSet->RegisterSeikakuName( 0, pp->GetSeikaku() );
        }
        else
        {
          wordSet->RegisterPokeOyaName( 0, pp );
        }
        view->SetTextboxPaneMessage( m_pTextPlayerName, PARAM_OYA_VALUE );
      }
      
      // IDNo
      u32 id = 0;
      bool isSixID = PokeTool::GetDrawID( pp, &id );
      if( isSixID )
      {
        view->SetTextboxPaneNumber( m_pTextIDNo, PARAM_ID_VALUE, id, 6, 0, print::NUM_DISP_ZERO );
      }
      else {
        view->SetTextboxPaneNumber( m_pTextIDNo, PARAM_ID_VALUE, id, 5, 0, print::NUM_DISP_ZERO );
      }
      
      // 属性アイコンのセット
      SetTypeIcon( pp );

      // 経験値のセット
      SetNextExp( pp );

      // 言語アイコンのセット
      SetLangIcon( pp );

      // trainer
      if(m_CreateParam.isBFCLayout && pBFCTrainerInfo)
      {
        NetAppLib::JoinFesta::PacketGameStatus1 pgs1;
        app::util::G2DUtil* pG2DUtil  = view->GetG2DUtil();
        gfl2::str::StrBuf*  pStrBuf   = pG2DUtil->GetTempStrBuf(1);
        
        pStrBuf->SetStr(pBFCTrainerInfo->name);
        
        /*  name/grade  */
        pG2DUtil->SetRegisterWord(0, *pStrBuf);
        view->SetTextboxPaneMessage( m_pTextTrainerName, PARAM_OYA_VALUE );

        pG2DUtil->SetRegisterNumber(0, pBFCTrainerInfo->grade, 2);
        view->SetTextboxPaneMessage( m_pTextBFCGrade, PARAM_GRADE );
        m_pTextBFCGrade->SetVisible(true);

        /*  icon  */
        if(m_pTrainerIcon)
        {
          app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA data;

          data.ConvFromMyStatusIconData( pBFCTrainerInfo->iconData );
          data.pReplacePane             = m_pTrainerIconPicture;
          data.pSyncVisiblePane         = m_pTrainerIconVisibilityControl;
          data.pInLoadVisibleChangePane = m_pTrainerIconFrame;
          data.pTimerIconRootPane       = NULL;
          m_pTrainerIcon->SetIconObjectData( 0, data );
        }
      }
    }
    pp->EndFastMode( fastmode_flag );

    // 表示するポケモンへとパラメータを変更
    m_pPokeParam = pp;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タイプアイコンのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void FieldStatusUpperInfo::SetTypeIcon( const pml::pokepara::PokemonParam* pp )
{
  pml::PokeType type1 = pp->GetType1();
  pml::PokeType type2 = pp->GetType2();

  // タイプ１のアイコンのセット
  m_pCommonIcon->ReplacePaneTextureByWazaTypeIcon( static_cast<PokeType_tag>(type1), m_pTypeIconPane[0] );
  // タイプ２のアイコンのセット
  m_pCommonIcon->ReplacePaneTextureByWazaTypeIcon( static_cast<PokeType_tag>(type2), m_pTypeIconPane[1] );

  app::util::G2DUtil* g2d = m_CreateParam.view->GetG2DUtil();
  // タイプ２の表示切替
  g2d->SetPaneVisible( m_pTypeIconParts[1], (type1 != type2) );
}

//------------------------------------------------------------------------------
/**
 * @brief   次の経験値のセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void FieldStatusUpperInfo::SetNextExp( const pml::pokepara::PokemonParam* pp )
{
  app::ui::UIView* view = m_CreateParam.view;

  u32 nowExp = pp->GetExp();
  u32 curExp = pp->GetExpForCurrentLevel();
  u32 nexExp = pp->GetExpForNextLevel();

  // けいけんち
  view->SetTextboxPaneNumber( m_pTextExpTotal, PARAM_EXP_VALUE, pp->GetExp(), 7 );

  // つぎのレベルまで
  u32 nextExp = pp->GetExpForNextLevel() - pp->GetExp();
  view->SetTextboxPaneNumber( m_pTextExpNext, PARAM_EXP_NEXT_VALUE, nextExp, 7 );

  // EXPゲージ
  m_pExpGauge->SetNumber( (nowExp - curExp), (nexExp - curExp) );

  // EXPゲージの更新
  m_pExpGauge->Update();
}

//------------------------------------------------------------------------------
/**
 * @brief   言語アイコンのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void FieldStatusUpperInfo::SetLangIcon( const pml::pokepara::PokemonParam* pp )
{
  u32 pokeLangID = pp->GetLangId();
  u32 langID     = static_cast<u32>(System::GetLang());

  // 言語アイコンの変更
  m_pCommonIcon->ReplacePaneTextureByLangIcon( pokeLangID, m_pLangIcon );
  // 言語アイコンの表示切替
  app::util::G2DUtil* g2d = m_CreateParam.view->GetG2DUtil();
  g2d->SetPaneVisible( m_pLangIcon, (pokeLangID != langID) );
}

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)
