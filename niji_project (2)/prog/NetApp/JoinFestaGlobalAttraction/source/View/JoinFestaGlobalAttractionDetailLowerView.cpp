// ============================================================================
/*
 * @file JoinFestaGlobalAttractionDetailLowerView.cpp
 * @brief グローバルアトラクションアプリの下画面を表示するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionDetailLowerView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction.h>
#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction_anim_list.h>
#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction_pane.h>
#include <niji_conv_header/message/msg_jf_attraction.h>

#include "Sound/include/Sound.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)

JoinFestaGlobalAttractionDetailLowerView::JoinFestaGlobalAttractionDetailLowerView( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pJoinFestaGlobalAttractionWork,
    NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_ATTRACTION_FJA_WIN_LOW_001_BFLYT,
    LA_JOIN_FESTA_ATTRACTION_FJA_WIN_LOW_001___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pJoinFestaGlobalAttractionWork->GetMessageUtility(),
    GARC_message_jf_attraction_DAT ),
  m_pJoinFestaGlobalAttractionWork( pJoinFestaGlobalAttractionWork ),
  m_pJoinFestaGlobalAttractionDetailLowerViewListener( NULL )
{
  setupButton();

  SetInputListener( this );
}


JoinFestaGlobalAttractionDetailLowerView::~JoinFestaGlobalAttractionDetailLowerView()
{
}


void JoinFestaGlobalAttractionDetailLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaGlobalAttractionWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


void JoinFestaGlobalAttractionDetailLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BACK_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_ATTRACTION_FJA_WIN_LOW_001__BACKBUTTON_TOUCH,
      LA_JOIN_FESTA_ATTRACTION_FJA_WIN_LOW_001__BACKBUTTON_RELEASE,
      LA_JOIN_FESTA_ATTRACTION_FJA_WIN_LOW_001__BACKBUTTON_CANSEL,
      LA_JOIN_FESTA_ATTRACTION_FJA_WIN_LOW_001__BACKBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_ATTRACTION_FJA_WIN_LOW_001__BACKBUTTON_ACTIVE,
      LA_JOIN_FESTA_ATTRACTION_FJA_WIN_LOW_001__BACKBUTTON_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAGLOBALATTRACTION );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_FJA_WIN_LOW_001_PANE_BACKBUTTON );
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pJoinFestaGlobalAttractionWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK_BUTTON, SEQ_SE_CANCEL1 );
}


::app::ui::UIInputListener::ListenerResult JoinFestaGlobalAttractionDetailLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pJoinFestaGlobalAttractionDetailLowerViewListener )
      {
        m_pJoinFestaGlobalAttractionDetailLowerViewListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult JoinFestaGlobalAttractionDetailLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( IsButtonAnimation() )
  {
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_BACK_BUTTON );
    result = DISABLE_ACCESS;
  }

  return result;
}


void JoinFestaGlobalAttractionDetailLowerView::SetGlobalAttractionInfo( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo )
{
  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pGlobalAttractionSaveData = NULL;

  int index = m_pJoinFestaGlobalAttractionWork->IDToIndex( pGlobalAttractionInfo );

  if( index >= 0 )
  {
    pGlobalAttractionSaveData = pJoinFestaDataSave->GetGlobalAttractionData( static_cast<u8>( index ) );
  }

  if( pGlobalAttractionInfo && pGlobalAttractionSaveData )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

    NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaGlobalAttractionWork->GetMessageUtility();

    // プレイヤー名の記録
    {
      u32 messageID = jf_attraction_cap_11;

      // @fix GFNMCat[5608] 開催中の時は『プレイヤー名の現在の記録』と表示する
      if( m_pJoinFestaGlobalAttractionWork->IsOpenGlobalAttraction( pGlobalAttractionInfo ) )
      {
        messageID = jf_attraction_cap_10;
      }

      Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( PANENAME_FJA_WIN_LOW_001_PANE_DAIJI_03 );

      pG2DUtil->SetRegisterPlayerName( 0, pMyStatus );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, pMessageUtility->GetString( GARC_message_jf_attraction_DAT, messageID ) );
    }

    // 参加しているか、参加していないか
    {
      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( PANENAME_FJA_WIN_LOW_001_PANE_UKETORI_01 );

      if( pGlobalAttractionSaveData->participateState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_NULL )
      {
        pTextBox->SetVisible( true );
        pG2DUtil->SetTextBoxPaneString( pTextBox, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_16 ) );
      }
      else
      {
        pTextBox->SetVisible( false );
      }
    }

    // スコア
    {
      u32 score = static_cast<u32>( pGlobalAttractionSaveData->uploadRecord );

      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( PANENAME_FJA_WIN_LOW_001_PANE_SCORE_09 );

      pG2DUtil->SetRegisterNumber( 0, score, 4 );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_12 ) );
    }

    // ご褒美のフェスコイン
    {
      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( PANENAME_FJA_WIN_LOW_001_PANE_DAIJI_04 );
      pG2DUtil->SetTextBoxPaneString( pTextBox, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_13 ) );
    }

    // もらえるコインの枚数か受け取った報酬のコインの枚数
    {
      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( PANENAME_FJA_WIN_LOW_001_PANE_COIN_00 );

      // ご褒美を受け取った後
      if( pGlobalAttractionSaveData->rewardState == 1 )
      {
        // @fix GFNMCat[5595] 受け取った報酬のコインの枚数を表示
        pG2DUtil->SetRegisterNumber( 0, pGlobalAttractionSaveData->rewardValue, 5 );
        pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_15 ) );
      }
      // ご褒美を受け取る前
      else
      {
        // もらえるコインの枚数を表示
        u32 getCoinMin = static_cast<u32>( pGlobalAttractionInfo->failureReward );
        u32 getCoinMax = static_cast<u32>( pGlobalAttractionInfo->successReward );

        // @fix GFNMCat[4021] PGL会員の時は2倍表記にする
        if( pGlobalAttractionSaveData->isPglMember )
        {
          getCoinMin *= 2; // 仕様書に報酬の最大値が65535と書いてあるが、
          getCoinMax *= 2; // 実際には使われないとの事なので2倍しても大丈夫(2倍すると6桁になるが実際には大きい値の報酬は無いとの事)
        }

        pG2DUtil->SetRegisterNumber( 0, getCoinMin, 5 ); // メッセージテキストのデータが5桁に変更されていたので、プログラムの方も5桁に合わせました
        pG2DUtil->SetRegisterNumber( 1, getCoinMax, 5 ); // メッセージテキストのデータが5桁に変更されていたので、プログラムの方も5桁に合わせました
        pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_14 ) );
      }
    }

    // 報酬を受け取ったとか受け取ってないか
    {
      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( PANENAME_FJA_WIN_LOW_001_PANE_CAPTION_00 );

      // ご褒美が受け取れる状態
      if( pGlobalAttractionSaveData->rewardValue > 0 && pGlobalAttractionSaveData->rewardState == 0 )
      {
        pTextBox->SetVisible( true );
        pG2DUtil->SetTextBoxPaneString( pTextBox, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_20 ) );
      }
      // ご褒美が受け取った
      else
      if( pGlobalAttractionSaveData->rewardState == 1 )
      {
        u32 year   = pGlobalAttractionSaveData->rewardYear;
        u32 month  = pGlobalAttractionSaveData->rewardMonth;
        u32 day    = pGlobalAttractionSaveData->rewardDay;
        u32 hour   = pGlobalAttractionSaveData->rewardHour;
        u32 minute = pGlobalAttractionSaveData->rewardMinitue;

        pTextBox->SetVisible( true );

        pG2DUtil->SetRegisterNumber( 0, year,   2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
        pG2DUtil->SetRegisterNumber( 1, month,  2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
        pG2DUtil->SetRegisterNumber( 2, day,    2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
        pG2DUtil->SetRegisterNumber( 3, hour,   2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
        pG2DUtil->SetRegisterNumber( 4, minute, 2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
        pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_18 ) );
      }
      // 上以外
      else
      {
        pTextBox->SetVisible( false );
      }
    }

  }
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)
