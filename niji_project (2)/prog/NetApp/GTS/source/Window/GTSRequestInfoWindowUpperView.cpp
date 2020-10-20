// ============================================================================
/*
 * @file GTSRequestInfoWindowUpperView.cpp
 * @brief リクエスト情報を表示するクラスです。
 * @date 2015.04.30
 */
// ============================================================================
#include "NetApp/GTS/source/Window/GTSRequestInfoWindowUpperView.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "NetApp/GTS/source/GTSResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/GTS/GTS.h>
#include <niji_conv_header/app/GTS/GTS_anim_list.h>
#include <niji_conv_header/app/GTS/GTS_pane.h>
#include <niji_conv_header/message/msg_gts.h>
#include <pml/include/item/item.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(Window)



GTSRequestInfoWindowUpperView::GTSRequestInfoWindowUpperView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pGTSWork,
    NetApp::GTS::GTS_RESOURCE_ID_LAYOUT_GTS,
    LYTRES_GTS_GTS_STA_UPP_000_BFLYT,
    LA_GTS_GTS_STA_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pGTSWork->GetMessageUtility(),
    GARC_message_gts_DAT ),
  m_pGTSWork( pGTSWork ),
  m_PlayerName( 256, pGTSWork->GetDeviceHeap() ),
  m_PokemonName( 256, pGTSWork->GetDeviceHeap() ),
  m_pItemIconTool( NULL )
{
  gfl2::heap::HeapBase* pHeap = m_pGTSWork->GetDeviceHeap();

  {
    app::tool::ItemIconToolEx::ItemIconToolParam  initParam;
    initParam.layout_id = LAYOUT_WORK_ID_UPPER;
    initParam.pHeap     = m_pGTSWork->GetAppHeap();
    initParam.pUIView   = this;
    initParam.res_id    = LAYOUT_RESOURCE_ID_GTS;
    initParam.iconMax   = 1;
    initParam.sync      = false;

    m_pItemIconTool = GFL_NEW( pHeap ) app::tool::ItemIconToolEx( initParam );
  }

  setupTextMessage();
}


GTSRequestInfoWindowUpperView::~GTSRequestInfoWindowUpperView()
{
  GFL_SAFE_DELETE( m_pItemIconTool );
}


bool GTSRequestInfoWindowUpperView::IsDrawing(void) const
{
  bool result = false;

  if( UIView::IsDrawing() )
  {
    result = true;
  }

  if( !m_pItemIconTool->EndFunc() )
  {
    result = true;
  }

  return result;
}


void GTSRequestInfoWindowUpperView::setupTextMessage()
{
  setupLayout( LAYOUT_WORK_ID_UPPER, LA_GTS_GTS_STA_UPP_000_MSG_IN );

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  // 画面上のテキスト
  {
    gfl2::lyt::LytTextBox* pText_Msg = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXT_MSG );
    pG2DUtil->SetTextBoxPaneStringExpand( pText_Msg, msg_gts_sys_cap_05 ); // GTSに預けているポケモン情報
  }

  // 持ち物
  {
    gfl2::lyt::LytTextBox* pTextHead_01 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTHEAD_01 );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextHead_01, msg_gts_sys_cap_10 ); // 持ち物
  }

  // 欲しいポケモン
  {
    gfl2::lyt::LytTextBox* pTextHead_02 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTHEAD_02 );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextHead_02, msg_gts_sel_04 ); // 欲しいポケモン
  }

  // 性別
  {
    gfl2::lyt::LytTextBox* pTextHead_03 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTHEAD_03 );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextHead_03, msg_gts_sel_06 ); // 性別
  }

  // レベル
  {
    gfl2::lyt::LytTextBox* pTextHead_04 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTHEAD_04 );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextHead_04, msg_gts_sel_07 ); // レベル
  }

  // レベル(グラフィックフォントの方)
  {
    gfl2::lyt::LytTextBox* pTextBox_00 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTBOX_00 );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBox_00, msg_gts_sys_cap_09 ); // レベル
  }

}


void GTSRequestInfoWindowUpperView::SetGTSIndexData( NetApp::GTS::GtsIndexData& gtsIndexData, bool bIsDownloadPokemon )
{
  NetAppLib::Util::NetAppPokeModelUtility* pPokeModelUtility = m_pGTSWork->GetPokeModelUtility();
  if( pPokeModelUtility )
  {
    PokeTool::SimpleParam simpleParam;

    simpleParam.monsNo  = static_cast<MonsNo>(gtsIndexData.PokeInfo.kind);
    simpleParam.formNo  = static_cast<FormNo>(gtsIndexData.PokeInfo.form);
    simpleParam.isRare  = gtsIndexData.PokeInfo.isRare;
    simpleParam.isEgg   = false;
    simpleParam.perRand = gtsIndexData.PokeInfo.personalRandom;

    if(gtsIndexData.PokeInfo.gender == 1)       simpleParam.sex = pml::SEX_MALE;
    else if(gtsIndexData.PokeInfo.gender == 2)  simpleParam.sex = pml::SEX_FEMALE;
    else                                        simpleParam.sex = pml::SEX_UNKNOWN;

    pPokeModelUtility->RequestEx( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, 0, simpleParam, 1 );

    pPokeModelUtility->SetFramePosition( NetApp::GTS::POKE_IN_FRAME_LEFT, NetApp::GTS::POKE_IN_FRAME_TOP, NetApp::GTS::POKE_IN_FRAME_RIGHT, NetApp::GTS::POKE_IN_FRAME_BOTTOM );
  }

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  // 上画面
  {
    // 言語コードアイコン
    {
      gfl2::lyt::LytParts* pLanguageIconParts = pUpper->GetPartsPane( PANENAME_GTS_STA_UPP_000_PANE_ICON_LANG );

      if( pLanguageIconParts )
      {
        gfl2::lyt::LytPicture* pLanguageIcon = pUpper->GetPicturePane( pLanguageIconParts, PANENAME_COMMON_ICN_008_PANE_BG_00, &gtsResourceID );
        
        if( pLanguageIcon )
        {
          app::tool::AppCommonGrpIconData* pAppCommonGrpIconData = m_pGTSWork->GetAppCommonGrpIconData();
      
          if( pAppCommonGrpIconData )
          {
            // @note : GtsIndexData に後から languageCode を追加したので、
            //         サーバー側には 0( POKEMON_LANG_NULL ) が存在する。
            //         その為、POKEMON_LANG_NULL チェックを行っています。
            //         追加日 2016/04/06

            u32 pokemonLanguageCode = POKEMON_LANG_JAPAN;
            
            if( gtsIndexData.pokemonLanguageCode != POKEMON_LANG_NULL )
            {
              pokemonLanguageCode = gtsIndexData.pokemonLanguageCode;
            }

            pAppCommonGrpIconData->ReplacePaneTextureByLangIcon( pokemonLanguageCode, pLanguageIcon );
          }
        }
      }
    }

    // 交換対象のポケモンのニックネーム
    {
      gfl2::lyt::LytTextBox* pTextHead_00 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTHEAD_00 );
      pG2DUtil->SetTextBoxPaneString( pTextHead_00, reinterpret_cast<gfl2::str::STRCODE*>(gtsIndexData.PokeInfo.nickname) );
    }

    // 交換対象のポケモンの性別
    {
      gfl2::lyt::LytParts* pParts = pUpper->GetPartsPane( PANENAME_GTS_STA_UPP_000_PANE_GENDER_00 );
      u8 drawGender = NetApp::GTS::GTSUtility::GetDrawGender( static_cast<MonsNo>( gtsIndexData.PokeInfo.kind ), gtsIndexData.PokeInfo.gender, gtsIndexData.PokeInfo.haveNickName );
      if( drawGender == NetApp::GTS::GENDER_UNKNOWN )
      {
        pParts->SetVisible( false );
      }
      else
      {
        u32 animeID = ( drawGender == NetApp::GTS::GENDER_MALE ) ? LA_GTS_GTS_STA_UPP_000__GENDER_00_ICON_MALE : LA_GTS_GTS_STA_UPP_000__GENDER_00_ICON_FEMALE;
        pParts->SetVisible( true );
        pG2DUtil->StartAnime( LAYOUT_WORK_ID_UPPER, animeID );
      }
    }

    // 交換対象のポケモンのレベル
    {
      gfl2::lyt::LytTextBox* pTextParam_00 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTPARAM_00 );
      pG2DUtil->SetRegisterNumber( 0, gtsIndexData.PokeInfo.level, 3 );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextParam_00, msg_gts_sys_cap_14_S );
    }

    // 交換対象を捕獲したボール
    {
      m_pItemIconTool->SetItemIcon( 0, item::ITEM_BallID2ItemID( static_cast<item::BALL_ID>( gtsIndexData.PokeInfo.ballID ) ), PANENAME_GTS_STA_UPP_000_PANE_BALL_00 );
    }

    // 欲しいポケモン
    {
      bool bDrawInputNamePokemon = false;

      if( bIsDownloadPokemon )
      {
        Savedata::GtsData* pGTSSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

        pGTSSaveData->GetWantPokeName( &m_PokemonName );

        if( gtsIndexData.PokeInfo.requestKind == MONSNO_NULL )
        {
          bDrawInputNamePokemon = true;
        }
      }

      gfl2::lyt::LytTextBox* pTextParam_02 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTPARAM_02 );
      
      if( bDrawInputNamePokemon )
      {
        pG2DUtil->SetTextBoxPaneString( pTextParam_02, &m_PokemonName );
      }
      else
      {
        pG2DUtil->SetRegisterMonsNameNo( 0, static_cast<MonsNo>(gtsIndexData.PokeInfo.requestKind) );
        pG2DUtil->SetTextBoxPaneStringExpand( pTextParam_02, msg_gts_sel_05 );
      }
    }

    // 欲しいポケモンを捕獲したかしていないかを表すボールアイコン
    {
      bool bPokeSeeFlag = false;

      if( gtsIndexData.PokeInfo.requestKind != MONSNO_NULL )
      {
        Savedata::ZukanData* pZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
        bPokeSeeFlag = pZukanData->GetPokeGetFlag( static_cast<MonsNo>(gtsIndexData.PokeInfo.requestKind) );
      }

      gfl2::lyt::LytPane* pBallIcon = pUpper->GetPane( PANENAME_GTS_STA_UPP_000_PANE_BALL_01 );
      pBallIcon->SetVisible( bPokeSeeFlag );
    }

    // 持っているアイテム
    {
      gfl2::lyt::LytTextBox* pTextParam_01 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTPARAM_01 );
      pG2DUtil->SetRegisterItemName( 0, static_cast<u32>(gtsIndexData.PokeInfo.item) );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextParam_01, msg_gts_sys_cap_12 );
    }

    // 性別
    {
      gfl2::lyt::LytTextBox* pTextParam_03 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTPARAM_03 );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextParam_03, NetApp::GTS::GTSUtility::GetGenderMessageID( gtsIndexData.PokeInfo.requestGender ) );
    }

    // レベル
    {
      gfl2::lyt::LytTextBox* pTextParam_04 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTPARAM_04 );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextParam_04, NetApp::GTS::GTSUtility::GetLevelBandMessageID( gtsIndexData.PokeInfo.requestLevelBand ) );
    }

    // ○○から一言：
    {
      gfl2::lyt::LytTextBox* pTextComment_00 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTCOMMENT_00 );
      m_PlayerName.SetStr( reinterpret_cast<const gfl2::str::STRCODE*>(gtsIndexData.TrainerName) );
      pG2DUtil->SetRegisterWord( 0, m_PlayerName );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextComment_00, msg_gts_sys_cap_13 );
    }

    // 一言メッセージ
    {
      gfl2::lyt::LytTextBox* pTextComment_01 = pUpper->GetTextBoxPane( PANENAME_GTS_STA_UPP_000_PANE_TEXTCOMMENT_01 );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextComment_01, gtsIndexData.messageID );
    }
  }
}


void GTSRequestInfoWindowUpperView::Update()
{
  NetAppLib::System::NetApplicationViewBase::Update();
  m_pItemIconTool->Update();
}


void GTSRequestInfoWindowUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}


//-----------------------------------------------------------------------------
// UIView
//-----------------------------------------------------------------------------
void GTSRequestInfoWindowUpperView::OnRemovedFromParent()
{
  NetAppLib::Util::NetAppPokeModelUtility* pPokeModelUtility = m_pGTSWork->GetPokeModelUtility();
  if( pPokeModelUtility )
  {
    pPokeModelUtility->ReleaseRequest(0);

    pPokeModelUtility->SetFramePosition( NetApp::GTS::POKE_OUT_FRAME_LEFT, NetApp::GTS::POKE_OUT_FRAME_TOP, NetApp::GTS::POKE_OUT_FRAME_RIGHT, NetApp::GTS::POKE_OUT_FRAME_BOTTOM );
  }
}


GFL_NAMESPACE_END(Window)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
