// ============================================================================
/*
 * @file RegulationDLSelectFrame.cpp
 * @brief レギュレーション選択アプリのダウンロードしたレギュレーションデータを選択するプロセスです。
 * @date 2015.07.01
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/Frame/RegulationDLSelectFrame.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainUpperView.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainLowerView.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMessageMenuLowerView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectFrameResult.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectViewObjects.h"
#include "Sound/include/Sound.h"
#include <niji_conv_header/message/msg_regulation.h>



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Frame)


RegulationDLSelectFrame::RegulationDLSelectFrame(
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork,
  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects ) :
  NetApp::RegulationSelect::Frame::RegulationSelectSDCardSaveFramBase( pRegulationSelectWork ),
  m_pRegulationSelectViewObjects( pRegulationSelectViewObjects ),
  m_RegulationSelectMainLowerViewListener(),
  m_RegulationSelectMessageMenuLowerViewListener()
{
}


RegulationDLSelectFrame::~RegulationDLSelectFrame()
{
}


//-----------------------------------------------------------------------------
// RegulationSelectFrameBase
//-----------------------------------------------------------------------------
bool RegulationDLSelectFrame::startup()
{
  m_pRegulationSelectViewObjects->InputDisableAll();
  m_pRegulationSelectWork->GetRootUpperView()->RemoveAllSubView();
  m_pRegulationSelectWork->GetRootLowerView()->RemoveAllSubView();

  addUpperViewInputDisable( m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView() );
  addLowerViewInputDisable( m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView() );

  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->CheckBoxEnable();
  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->SetRegulationList( m_pRegulationSelectWork->GetDownloadList(), true, NetApp::RegulationSelect::Draw::RegulationSelectMainLowerView::LIST_TYPE_DOWNLOAD );

  NetAppLib::UI::RegulationDrawInfo* pActiveDownloadRegulationDrawInfo = m_pRegulationSelectWork->GetActiveDownloadRegulationDrawInfo();

  if( pActiveDownloadRegulationDrawInfo )
  {
    m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView()->SetRegulationDrawInfo( pActiveDownloadRegulationDrawInfo );
  }
  else
  {
    NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo = *m_pRegulationSelectWork->GetDownloadList()->Begin();
    m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView()->SetRegulationDrawInfo( pRegulationDrawInfo );
    m_pRegulationSelectWork->SetActiveDownloadRegulationDrawInfo( pRegulationDrawInfo );
  }

  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->BackgroundAndBackButtonVisible( false );

  return true;
}


void RegulationDLSelectFrame::setListener()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->SetListener( &m_RegulationSelectMainLowerViewListener );
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetListener( &m_RegulationSelectMessageMenuLowerViewListener );

  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->SetInputEnabled( true );
}


void RegulationDLSelectFrame::removeListener()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->RemoveListener();
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->RemoveListener();

  m_pRegulationSelectViewObjects->InputDisableAll();
}


void RegulationDLSelectFrame::updateSequence()
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMainUpperView()->SetRegulationDrawInfo( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );
  m_pRegulationSelectWork->SetActiveDownloadRegulationDrawInfo( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );

  // ==========================================================================================================================
  // 以下 RegulationSelectMainLowerView(メイン画面) の入力イベント
  u32 regulationSelectMainLowerViewInputEventID = m_RegulationSelectMainLowerViewListener.GetInputEventID();

  switch( regulationSelectMainLowerViewInputEventID )
  {
    case NetApp::RegulationSelect::Draw::RegulationSelectMainLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      addLowerViewInputEnable( m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView() );
      m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageYesNoBlackFilter( msg_reg_win_18 );
    }
    break;

    case NetApp::RegulationSelect::Draw::RegulationSelectMainLowerViewListener::INPUT_EVENT_ID_SELECT_REGULATION:
    {
      addLowerViewInputEnable( m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView() );
      m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageMenuBlackFilter( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo(), msg_reg_win_07, msg_reg_sel_03, msg_reg_sel_04, msg_reg_sel_05 );
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
      // 終了するかしないかのYes/No選択
      case msg_reg_win_18:
      {
        switch( regulationSelectMessageMenuLowerViewInputEventID )
        {
          case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON:
          {
            exitFrame( NetApp::RegulationSelect::DL_SELECT_RESULT_BACK );
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
      // レギュレーションデータを削除するかしないかのYes/No選択
      case msg_reg_win_11:
      case msg_reg_win_21:
      {
        switch( regulationSelectMessageMenuLowerViewInputEventID )
        {
          case NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON:
          {
            m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetStreamMessageBlackFilter( msg_reg_win_12 );
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
        case msg_reg_win_08:
        {
          removeLowerView();
        }
        break;

        case msg_reg_win_10:
        {
          removeLowerView();

          m_pRegulationSelectViewObjects->GetRegulationSelectMainLowerView()->SetRegulationList( m_pRegulationSelectWork->GetDownloadList(), true, NetApp::RegulationSelect::Draw::RegulationSelectMainLowerView::LIST_TYPE_DOWNLOAD );
        }
        break;

        case msg_reg_win_20:
        {
          exitFrame( NetApp::RegulationSelect::DL_SELECT_RESULT_BACK );
        }
        break;

        case msg_reg_win_12:
        {
          removeLowerView();
          exitFrame( NetApp::RegulationSelect::DL_SELECT_RESULT_TO_DELETE );
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
            // 0 : ダウンロードする
            case msg_reg_sel_03:
            {
              if( isDownloaded( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() ) )
              {
                m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetStreamMessageBlackFilter( msg_reg_win_08 );
              }
              else
              {
                saveRegulationData( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );

                m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageBlackFilter( msg_reg_win_09 );
              }
            }
            break;

            // ------------------------------------------------------------------------
            // 1 : 詳しく見る
            case msg_reg_sel_04:
            {
              removeLowerView();

              m_pRegulationSelectWork->SetDetailBootInfo( NetApp::RegulationSelect::DETAIL_BOOT_FRAME_DL_SELECT, m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo() );

              exitFrame( NetApp::RegulationSelect::DL_SELECT_RESULT_TO_DETAIL );
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


bool RegulationDLSelectFrame::isDownloaded( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo )
{
  bool bIsDownloaded = false;

  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pDownloadedList = m_pRegulationSelectWork->GetDownloadedList();

  for( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>::Iterator it = pDownloadedList->Begin(); it != pDownloadedList->End(); ++it )
  {
    // @fix NMCat[4266]配信レギュレーションで大会IDが同じデータが、ローカル側で別なデータとして扱われてしまう　の修正
    //    対処：
    //      SDカードに大会IDが同じ＆バージョンが上または同じレギュレーションが存在するかチェック
    //
    if( (*it)->GetRegulation().GetValueParam(Regulation::DISCRIMINANT_ID) == pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::DISCRIMINANT_ID) &&
        (*it)->GetRegulation().GetVersion() >= pRegulationDrawInfo->GetRegulation().GetVersion() )
    {
      GFL_RELEASE_PRINT("NMCat[4266]:同じレギュレーション 大会ID:%d バージョン:%d \n", pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::DISCRIMINANT_ID), pRegulationDrawInfo->GetRegulation().GetVersion());
      bIsDownloaded = true;
      break;
    }
  }
  
  return bIsDownloaded;

}




void RegulationDLSelectFrame::onSaveRegulationDataSuccess( int saveRegulationType )
{
  m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetStreamMessageBlackFilter( m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo(), msg_reg_win_10 );

  Sound::PlaySE( SEQ_SE_SAVE );

  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pDownloadedList = m_pRegulationSelectWork->GetDownloadedList();

  Regulation& saveRegulation = m_RegulationSelectMainLowerViewListener.GetSelectRegulationDrawInfo()->GetRegulation();

  u32 size = saveRegulation.GetSerializeDataSize();

  gfl2::heap::HeapBase* pHeap = m_pRegulationSelectWork->GetDeviceHeap();

  u8* pTemp = GFL_NEW_ARRAY( pHeap->GetLowerHandle() ) u8[ size ];
          
  saveRegulation.Serialize( pTemp );

  m_pRegulationSelectWork->AddRegulation( pDownloadedList, pTemp, size, saveRegulationType );

  GFL_SAFE_DELETE_ARRAY( pTemp );
}


void RegulationDLSelectFrame::onSaveRegulationDataError( ErrorID eErrorID, int useFileSlotCount )
{
  switch( eErrorID )
  {
    case ERROR_ID_SLOT_MAX:
    {
      m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageYesNoBlackFilter( msg_reg_win_11 );
    }
    break;

    case ERROR_ID_NO_SPACE:
    {
      if( useFileSlotCount >= 1 )
      {
        m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetMessageYesNoBlackFilter( msg_reg_win_21 );
      }
      else
      {
        m_pRegulationSelectViewObjects->GetRegulationSelectMessageMenuLowerView()->SetStreamMessageBlackFilter( msg_reg_win_20 );
      }
    }
    break;
  }
}


void RegulationDLSelectFrame::onSaveRegulationDataFatalError()
{
  m_pRegulationSelectWork->SetFatalError( true );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
