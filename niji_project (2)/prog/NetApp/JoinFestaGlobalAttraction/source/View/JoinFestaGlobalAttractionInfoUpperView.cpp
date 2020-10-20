// ============================================================================
/*
 * @file JoinFestaGlobalAttractionInfoUpperView.cpp
 * @brief グローバルアトラクションアプリの上画面を表示するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionInfoUpperView.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction.h>
#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction_anim_list.h>
#include <niji_conv_header/app/join_festa_attraction/join_festa_attraction_pane.h>
#include <niji_conv_header/message/msg_jf_attraction.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)

JoinFestaGlobalAttractionInfoUpperView::JoinFestaGlobalAttractionInfoUpperView( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pJoinFestaGlobalAttractionWork,
    NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_ATTRACTION_JFA_WIN_UPP_000_BFLYT,
    LA_JOIN_FESTA_ATTRACTION_JFA_WIN_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pJoinFestaGlobalAttractionWork->GetMessageUtility(),
    GARC_message_jf_attraction_DAT ),
  m_pJoinFestaGlobalAttractionWork( pJoinFestaGlobalAttractionWork ),
  m_pActiveGlobalAttractionInfo( NULL )
{
}


JoinFestaGlobalAttractionInfoUpperView::~JoinFestaGlobalAttractionInfoUpperView()
{
}


void JoinFestaGlobalAttractionInfoUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaGlobalAttractionWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}


void JoinFestaGlobalAttractionInfoUpperView::UpdateGlobalAttractionInfo()
{
  SetGlobalAttractionInfo( m_pActiveGlobalAttractionInfo );
}

void JoinFestaGlobalAttractionInfoUpperView::SetGlobalAttractionInfo( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo )
{
  m_pActiveGlobalAttractionInfo = pGlobalAttractionInfo;

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

    gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

    NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaGlobalAttractionWork->GetMessageUtility();

    // アトラクションの名前
    {
      gfl2::lyt::LytTextBox* pTextBox = pUpper->GetTextBoxPane( PANENAME_JFA_WIN_UPP_000_PANE_ATTRACTION_NAME_01 );

      pG2DUtil->SetTextBoxPaneString( pTextBox, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, m_pJoinFestaGlobalAttractionWork->GetAttractionNameMassageID( pGlobalAttractionInfo ) ) );
    }

    // アトラクションの説明
    {
      gfl2::lyt::LytTextBox* pTextBox = pUpper->GetTextBoxPane( PANENAME_JFA_WIN_UPP_000_PANE_ATTRACTION_INFOMATION_00 );

      pG2DUtil->SetTextBoxPaneString( pTextBox, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, m_pJoinFestaGlobalAttractionWork->GetAttractionDescriptionMassageID( pGlobalAttractionInfo ) ) );
    }

    // きかん(UTC)
    {
      gfl2::lyt::LytTextBox* pTextBox = pUpper->GetTextBoxPane( PANENAME_JFA_WIN_UPP_000_PANE_UTC_00 );

      pG2DUtil->SetTextBoxPaneString( pTextBox, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_03 ) );
    }

    // 開催中か集計中か終了しているか
    {
      bool bVisible = true;

      gfl2::lyt::LytTextBox* pTextBox = pUpper->GetTextBoxPane( PANENAME_JFA_WIN_UPP_000_PANE_UTC_01 );

      u32 messageID = jf_attraction_cap_04;

      if( m_pJoinFestaGlobalAttractionWork->IsOpenGlobalAttraction( pGlobalAttractionInfo ) )
      {
        messageID = jf_attraction_cap_04;
      }
      else
      {
        if( pGlobalAttractionSaveData->openState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_TOTAL_FINISH )
        {
          messageID = jf_attraction_cap_05;
        }
        else
        if( pGlobalAttractionSaveData->openState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_NULL )
        {
          // @fix GFNMCat[5658] 特定の条件で開催終了しているけど、未開催ステータスのデータが来てしまうので、非表示にする
          bVisible = false;
        }
        else
        {
          // @fix GFNMCat[5588] 『集計中』表示の対応
          messageID = jf_attraction_cap_22;
        }
      }

      pG2DUtil->SetTextBoxPaneString( pTextBox, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, messageID ) );
      pTextBox->SetVisible( bVisible );
    }

    // 開催期間
    {
      gfl2::lyt::LytTextBox* pTextBox = pUpper->GetTextBoxPane( PANENAME_JFA_WIN_UPP_000_PANE_TIME_00 );

      print::WordSet* pWordSet = pMessageUtility->GetWordSet();
      pWordSet->ClearAllBuffer();

      pWordSet->RegisterNumber( 0, pGlobalAttractionInfo->startDateTimeYear,    2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 1, pGlobalAttractionInfo->startDateTimeMonth,   2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 2, pGlobalAttractionInfo->startDateTimeDay,     2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 3, pGlobalAttractionInfo->startDateTimeHour,    2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 4, pGlobalAttractionInfo->startDateTimeMinute,  2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 5, pGlobalAttractionInfo->endDateTimeYear,      2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 6, pGlobalAttractionInfo->endDateTimeMonth,     2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 7, pGlobalAttractionInfo->endDateTimeDay,       2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 8, pGlobalAttractionInfo->endDateTimeHour,      2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 9, pGlobalAttractionInfo->endDateTimeMinute,    2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );

      gfl2::str::StrBuf& strbuf = pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_06 );

      gfl2::str::StrBuf* pTempStrBuf = pG2DUtil->GetTempStrBuf(0);
      pWordSet->Expand( pTempStrBuf, &strbuf );

      pG2DUtil->SetTextBoxPaneString( pTextBox, pTempStrBuf );
    }

    // 目標
    {
      gfl2::lyt::LytTextBox* pTextBox = pUpper->GetTextBoxPane( PANENAME_JFA_WIN_UPP_000_PANE_UTC_02 );

      pG2DUtil->SetTextBoxPaneString( pTextBox, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_07 ) );
    }

    // 達成記録
    {
      gfl2::lyt::LytTextBox* pTextBox = pUpper->GetTextBoxPane( PANENAME_JFA_WIN_UPP_000_PANE_STATE_00 );

      pG2DUtil->SetRegisterNumber( 0, pGlobalAttractionInfo->successRecord, 10 );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_08 ) );
    }

    bool bTotalDateTimeAndScoreVisibleFlag = true;

    // 集計時間か最終結果
    {
      bool bDrawFinalResult = false;

      gfl2::lyt::LytTextBox* pTextBox = pUpper->GetTextBoxPane( PANENAME_JFA_WIN_UPP_000_PANE_TIME_01 );

      if( m_pJoinFestaGlobalAttractionWork->IsOpenGlobalAttraction( pGlobalAttractionInfo ) )
      {
        bDrawFinalResult = false;
      }
      else
      {
        if( pGlobalAttractionSaveData->openState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_TOTAL_FINISH )
        {
          bDrawFinalResult = true;
        }
        else
        {
          bDrawFinalResult = false;
        }
      }

      if( bDrawFinalResult )
      {
        pG2DUtil->SetTextBoxPaneString( pTextBox, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_19 ) );
      }
      else
      {
        // @fix GFNMCat[5587] 集計時間を表示
        pG2DUtil->SetRegisterNumber( 0, pGlobalAttractionSaveData->totalDlYear,    2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
        pG2DUtil->SetRegisterNumber( 1, pGlobalAttractionSaveData->totalDlMonth,   2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
        pG2DUtil->SetRegisterNumber( 2, pGlobalAttractionSaveData->totalDlDay,     2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
        pG2DUtil->SetRegisterNumber( 3, pGlobalAttractionSaveData->totalDlHour,    2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
        pG2DUtil->SetRegisterNumber( 4, pGlobalAttractionSaveData->totalDlMinitue, 2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
        pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_09 ) );
      }

      // @fix GFNMCat[5596] ゲームシンクで取得出来なかった場合は非表示
      if( pGlobalAttractionSaveData->totalDlMonth == 0 || pGlobalAttractionSaveData->totalDlDay == 0 )
      {
        bTotalDateTimeAndScoreVisibleFlag = false;
      }

      pTextBox->SetVisible( bTotalDateTimeAndScoreVisibleFlag );
    }

    u32 currentScore = pGlobalAttractionSaveData->totalRecord;

    // 現在の記録か最終結果の記録
    {
      gfl2::lyt::LytTextBox* pTextBox = pUpper->GetTextBoxPane( PANENAME_JFA_WIN_UPP_000_PANE_STATE_01 );

      pG2DUtil->SetRegisterNumber( 0, currentScore, 10 );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_08 ) );

      // @fix GFNMCat[5596] ゲームシンクで取得出来なかった場合は非表示
      pTextBox->SetVisible( bTotalDateTimeAndScoreVisibleFlag );
    }

    // 記録達成しているかしていないか
    {
      gfl2::lyt::LytTextBox* pTextBox = pUpper->GetTextBoxPane( PANENAME_JFA_WIN_UPP_000_PANE_HANKO );

      pG2DUtil->SetTextBoxPaneString( pTextBox, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, jf_attraction_cap_17 ) );

      if( currentScore >= pGlobalAttractionInfo->successRecord )
      {
        setupLayout( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_ATTRACTION_JFA_WIN_UPP_000_HANKO_ON );
      }
      else
      {
        setupLayout( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_ATTRACTION_JFA_WIN_UPP_000_HANKO_OFF );
      }
    }
  }
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)
