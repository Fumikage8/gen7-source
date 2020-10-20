// ============================================================================
/*
 * @file RegulationDLDeleteFrame.cpp
 * @brief レギュレーション選択アプリのダウンロードしたレギュレーションデータを削除するプロセスです。
 * @date 2015.07.01
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/Frame/RegulationDLDeleteFrame.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainUpperView.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainLowerView.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectFrameResult.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectViewObjects.h"

#include <niji_conv_header/message/msg_regulation.h>



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Frame)


RegulationDLDeleteFrame::RegulationDLDeleteFrame(
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork,
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects ) :
  NetApp::RegulationSelect::Frame::RegulationSelectSDCardDeleteFramBase( pRegulationSelectWork ),
  m_pRegulationSelectViewObjects( pRegulationSelectViewObjects ),
  m_RegulationSelectMainLowerViewListener(),
  m_RegulationSelectMessageMenuLowerViewListener()
{
}


RegulationDLDeleteFrame::~RegulationDLDeleteFrame()
{
}


//-----------------------------------------------------------------------------
// RegulationSelectFrameBase
//-----------------------------------------------------------------------------
bool RegulationDLDeleteFrame::startup()
{
  m_pRegulationSelectViewObjects->InputDisableAll();
  m_pRegulationSelectWork->GetRootUpperView()->RemoveAllSubView();
  m_pRegulationSelectWork->GetRootLowerView()->RemoveAllSubView();

  addUpperViewInputDisable( m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView() );
  addLowerViewInputDisable( m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView() );

  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->CheckBoxDisable();
  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->SetRegulationList( m_pRegulationSelectWork->GetDownloadedList(), false, NetApp::RegulationSelect::Draw::RegulationSelectMainLowerView::LIST_TYPE_DOWNLOADED );

  NetAppLib::UI::RegulationDrawInfo* pActiveDeleteRegulationDrawInfo = m_pRegulationSelectWork->GetActiveDeleteRegulationDrawInfo();

  if( pActiveDeleteRegulationDrawInfo )
  {
    m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView()->SetRegulationDrawInfo( pActiveDeleteRegulationDrawInfo );
  }
  else
  {
    NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo = *m_pRegulationSelectWork->GetDownloadedList()->Begin();
    m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView()->SetRegulationDrawInfo( pRegulationDrawInfo );
    m_pRegulationSelectWork->SetActiveDeleteRegulationDrawInfo( pRegulationDrawInfo );
  }

  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->BackgroundAndBackButtonVisible( false );

  return true;
}


void RegulationDLDeleteFrame::setListener()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->SetListener( &m_RegulationSelectMainLowerViewListener );
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetListener( &m_RegulationSelectMessageMenuLowerViewListener );

  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->SetInputEnabled( true );
}


void RegulationDLDeleteFrame::removeListener()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->RemoveListener();
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->RemoveListener();

  m_pRegulationSelectViewObjects->InputDisableAll();
}


void RegulationDLDeleteFrame::updateSequence()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView()->SetRegulationDrawInfo( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );
  m_pRegulationSelectWork->SetActiveDeleteRegulationDrawInfo( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );

  // ==========================================================================================================================
  // 以下 RegulationSelectMainLowerView(メイン画面) の入力イベント
  u32 regulationSelectMainLowerViewInputEventID = m_RegulationSelectMainLowerViewListener.GetInputEventID();

  switch( regulationSelectMainLowerViewInputEventID )
  {
    case NetApp::RegulationSelect::Draw::RegulationSelectMainLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      addLowerViewInputEnable( m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView() );
      m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageYesNoBlackFilter( msg_reg_win_13 );
    }
    break;

    case NetApp::RegulationSelect::Draw::RegulationSelectMainLowerViewListener::INPUT_EVENT_ID_SELECT_REGULATION:
    {
      addLowerViewInputEnable( m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView() );
      m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageMenuBlackFilter( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo(), msg_reg_win_07, msg_reg_sel_06, msg_reg_sel_04, msg_reg_sel_05 );
    }
    break;
  }

  // ==========================================================================================================================
  // 以下 RegulationSelectMessageMenuLowerView(メッセージメニュー) の入力イベント
  u32 regulationSelectMessageMenuLowerViewInputEventID = m_RegulationSelectMessageMenuLowerViewListener.GetInputEventID();

  // Yes/Noの入力イベント
  {
    switch( m_RegulationSelectMessageMenuLowerViewListener.GetMessageID() )
    {
      // -----------------------------------------------------------------------------------------
      // 削除をやめるかやめないかのYes/No選択
      case msg_reg_win_13:
      {
        switch( regulationSelectMessageMenuLowerViewInputEventID )
        {
          case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON:
          {
            removeLowerView();
            exitFrame( NetApp::RegulationSelect::DL_DELETE_RESULT_BACK );
          }
          break;

          case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON:
          {
            removeLowerView();
          }
          break;
        }
      }
      break;

      // -----------------------------------------------------------------------------------------
      // 本当に削除するかしないかのYes/No選択
      case msg_reg_win_14:
      {
        switch( regulationSelectMessageMenuLowerViewInputEventID )
        {
          case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON:
          {
            deleteRegulationData( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );

            m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageBlackFilter( msg_reg_win_15 );
          }
          break;

          case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON:
          {
            removeLowerView();
          }
          break;
        }
      }
      break;
    }
  }
  
  switch( regulationSelectMessageMenuLowerViewInputEventID )
  {
    // 画面タッチ(メッセージの表示が終わりユーザーの入力が行われた時のイベント)
    case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN:
    {
      switch( m_RegulationSelectMessageMenuLowerViewListener.GetMessageID() )
      {
        case msg_reg_win_16:
        {
          deleteRegulation();

          removeLowerView();
          exitFrame( NetApp::RegulationSelect::DL_DELETE_RESULT_BACK );
        }
        break;
      }
    }
    break;

    // メニュー選択時のイベント
    case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_SELECT_MENU:
    {
      switch( m_RegulationSelectMessageMenuLowerViewListener.GetMessageID() )
      {
        case msg_reg_win_07:
        {
          switch( m_RegulationSelectMessageMenuLowerViewListener.GetSelectMenuID() )
          {
            // ------------------------------------------------------------------------
            // 0 : 削除する
            case msg_reg_sel_06:
            {
              m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageYesNoBlackFilter( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo(), msg_reg_win_14 );
            }
            break;

            // ------------------------------------------------------------------------
            // 1 : 詳しく見る
            case msg_reg_sel_04:
            {
              removeLowerView();

              m_pRegulationSelectWork->SetDetailBootInfo( NetApp::RegulationSelect::DETAIL_BOOT_FRAME_DL_DELETE, m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );

              exitFrame( NetApp::RegulationSelect::DL_DELETE_RESULT_TO_DETAIL );
            }
            break;

            // ------------------------------------------------------------------------
            // 2 : 戻る
            case msg_reg_sel_05:
            {
              removeLowerView();
            }
            break;
          }
        }
        break;
      }
    }
    break;
  }

  m_RegulationSelectMainLowerViewListener.ResetInputEvent();
  m_RegulationSelectMessageMenuLowerViewListener.ResetInputEvent();

}


void RegulationDLDeleteFrame::onDeleteRegulationDataSuccess()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetStreamMessageBlackFilter( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo(), msg_reg_win_16 );
}


void RegulationDLDeleteFrame::onDeleteRegulationDataFatalError()
{
  m_pRegulationSelectWork->SetFatalError( true );
}


void RegulationDLDeleteFrame::deleteRegulation()
{
  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pDownloadedList = m_pRegulationSelectWork->GetDownloadedList();

  u32 deleteRegulationIndex = m_RegulationSelectMainLowerViewListener.GetSelectRegulation();

  u32 index = 0;

  for( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>::Iterator it = pDownloadedList->Begin(); it != pDownloadedList->End(); ++it )
  {
    if( index == deleteRegulationIndex )
    {
      GFL_SAFE_DELETE( *it );

      pDownloadedList->Erase( it );

      m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView()->ResetRegulationDrawInfo();

      m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->ResetPaneListCursorInfo( NetApp::RegulationSelect::Draw::RegulationSelectMainLowerView::LIST_TYPE_DOWNLOADED );

      m_pRegulationSelectWork->SetActiveDeleteRegulationDrawInfo( NULL );

      break;
    }

    ++index;
  }
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
