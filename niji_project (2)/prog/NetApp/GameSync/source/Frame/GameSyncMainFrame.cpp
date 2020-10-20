// ============================================================================
/*
 * @file GameSyncMainFrame.cpp
 * @brief ゲームシンクアプリの○○プロセスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/Frame/GameSyncMainFrame.h"
#include "NetApp/GameSync/source/GameSyncFrameResult.h"

#include "NetApp/GameSync/source/View/GameSyncBGUpperView.h"
#include "NetApp/GameSync/source/View/GameSyncUppCaptureUpperView.h"
#include "NetApp/GameSync/source/View/GameSyncLowCaptureUpperView.h"
#include "NetApp/GameSync/source/View/GameSyncMainUpperView.h"

#include "NetApp/GameSync/source/View/GameSyncBGLowerView.h"
#include "NetApp/GameSync/source/View/GameSyncTwoSelectMenuLowerView.h"
#include "NetApp/GameSync/source/View/GameSyncThreeSelectMenuLowerView.h"
#include "NetApp/GameSync/source/View/GameSyncMessageMenuLowerView.h"

#include "NetStatic/NetAppLib/include/GameSync/GameSyncRequestFacade.h"

#include <niji_conv_header/message/msg_gamesync.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <Savedata/include/GameSyncSave.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(Frame)


static const int SEQUENCE_DEFAULT                                     = 0;
static const int SEQUENCE_SYNC_SAVE                                   = 1;  // ゲームシンク前のセーブ
static const int SEQUENCE_REQUEST_SAVE                                = 2;  // 通信リクエスト後のセーブ
static const int SEQUENCE_CONFIRM_PLAY_STATUS_WAIT                    = 3;  // プレイステータスの確認待ち
static const int SEQUENCE_SAVE_DATA_SYNC_WAIT                         = 4;  // セーブデータ同期待ち


GameSyncMainFrame::GameSyncMainFrame(
  NetApp::GameSync::System::GameSyncWork* pGameSyncWork,
  NetApp::GameSync::System::GameSyncViewSystemAccessor* pGameSyncViewSystemAccessor ) :
  NetApp::GameSync::Frame::GameSyncFrameBase( pGameSyncWork, pGameSyncViewSystemAccessor ),
  m_GameSyncMessageMenuLowerViewListener(),
  m_GameSyncTwoSelectMenuLowerViewListener(),
  m_GameSyncThreeSelectMenuLowerViewListener(),
  m_NetAppCommonSaveUtility()
{
}


GameSyncMainFrame::~GameSyncMainFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool GameSyncMainFrame::startup()
{
  m_pGameSyncViewObjects->InputDisableAll();
  m_pGameSyncViewList->RemoveAllView();

  if( m_pGameSyncWork->GetMainBootFlag() )
  {
    const Savedata::GameSyncSave::GameSyncSaveData& gameSyncSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveData();

    NetApp::GameSync::System::GameSyncWork::DescriptionMenuType eType = m_pGameSyncWork->GetDescriptionMenuType();

    if( eType == NetApp::GameSync::System::GameSyncWork::DESCRIPTION_MENU_TYPE_TWO )
    {
      m_pGameSyncViewList->AddUpperView( m_pGameSyncViewObjects->GetGameSyncBGUpperView() );
      m_pGameSyncViewList->AddLowerView( m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView() );
      m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView()->SetMessage( msg_gamesync_win_12,
        msg_gamesync_sel_04, !gameSyncSaveData.m_config.m_isDispedGuide1,   // ゲームシンクとは？
        msg_gamesync_sel_05, !gameSyncSaveData.m_config.m_isDispedGuide2 ); // ゲームシンクの準備
    }
    else
    if( eType == NetApp::GameSync::System::GameSyncWork::DESCRIPTION_MENU_TYPE_THREE )
    {
      m_pGameSyncViewList->AddUpperView( m_pGameSyncViewObjects->GetGameSyncBGUpperView() );
      m_pGameSyncViewList->AddLowerView( m_pGameSyncViewObjects->GetGameSyncThreeSelectMenuLowerView() );
      m_pGameSyncViewObjects->GetGameSyncThreeSelectMenuLowerView()->SetMessage( msg_gamesync_win_12,
        msg_gamesync_sel_04, !gameSyncSaveData.m_config.m_isDispedGuide1,   // ゲームシンクとは？
        msg_gamesync_sel_05, !gameSyncSaveData.m_config.m_isDispedGuide2,   // ゲームシンクの準備
        msg_gamesync_sel_06, !gameSyncSaveData.m_config.m_isDispedGuide3 ); // ゲームシンクのやりかた
    }
    else
    {
      setupMainView();
    }
  }
  else
  {
    const Savedata::GameSyncSave::GameSyncSaveData& gameSyncSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveData();

    bool bStartDescriptionFlag = gameSyncSaveData.m_config.m_isDispedFirstGuide;

    bool bPGLAlertFlag = gameSyncSaveData.m_config.m_isNoRegistPGL;

    if( !bStartDescriptionFlag )
    {
      m_pGameSyncViewList->AddUpperView( m_pGameSyncViewObjects->GetGameSyncBGUpperView() );
      m_pGameSyncViewList->AddLowerView( m_pGameSyncViewObjects->GetGameSyncBGLowerView() );
      m_pGameSyncViewList->AddLowerView( m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView() );
      m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetStreamMessage( msg_gamesync_win_01 );
    }
    else
    if( bPGLAlertFlag )
    {
      m_pGameSyncViewList->AddUpperView( m_pGameSyncViewObjects->GetGameSyncBGUpperView() );
      m_pGameSyncViewList->AddLowerView( m_pGameSyncViewObjects->GetGameSyncBGLowerView() );
      m_pGameSyncViewList->AddLowerView( m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView() );
      m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetStreamMessage( msg_gamesync_win_02 );
    }
    else
    {
      setupMainView();
    }

    m_pGameSyncWork->SetMainBootFlag( true );
  }

  return true;
}


void GameSyncMainFrame::setupMainView()
{
  m_pGameSyncViewObjects->InputDisableAll();
  m_pGameSyncViewList->RemoveAllView();

  const Savedata::GameSyncSave::GameSyncSaveData& gameSyncSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveData();

  Savedata::MysteryGiftSave* pMysteryGiftSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetMysteryGiftSave();

  bool bIsCreateGameSyncID = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->GetIsCreateGameSyncSaveID();
  bool bIsExecuteGameSync  = gameSyncSaveData.m_config.m_isExecuteGameSync;
  bool bIsPGLRegister      = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->IsSignedPGL();

  bool bIsDispedGuide = false;

  if( !bIsCreateGameSyncID )
  {
    bIsDispedGuide = ( !gameSyncSaveData.m_config.m_isDispedGuide1 || !gameSyncSaveData.m_config.m_isDispedGuide2 ) ? false : true;
  }
  else
  {
    bIsDispedGuide = ( !gameSyncSaveData.m_config.m_isDispedGuide1 || !gameSyncSaveData.m_config.m_isDispedGuide2 || !gameSyncSaveData.m_config.m_isDispedGuide3 ) ? false : true;
  }

  // 構成A:ゲームシンクID未発行
  if( !bIsCreateGameSyncID )
  {
    m_pGameSyncViewList->AddUpperView( m_pGameSyncViewObjects->GetGameSyncBGUpperView() );
    m_pGameSyncViewList->AddLowerViewInputEnable( m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView() );
    m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView()->SetMessage( msg_gamesync_win_03,
      msg_gamesync_sel_01, false,             // ゲームシンクIDを作る
      msg_gamesync_sel_02, !bIsDispedGuide ); // 説明を読む
  }
  // 構成B:ゲームシンクID発行済みで、ゲームシンクを行っていない
  else
  if( ( bIsCreateGameSyncID && !bIsExecuteGameSync ) || !bIsPGLRegister ) // @fix GFNMCat[5601] PGLに登録していない時はPGL登録を促すメッセージを表示する
  {
    m_pGameSyncViewList->AddUpperView( m_pGameSyncViewObjects->GetGameSyncMainUpperView() );
    m_pGameSyncViewList->AddLowerViewInputEnable( m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView() );
    m_pGameSyncViewObjects->GetGameSyncMainUpperView()->SetGameSyncID( msg_gamesync_sys_02, false );
    m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView()->SetMessage( msg_gamesync_win_03,
      msg_gamesync_sel_03, false,             // ゲームシンクをする
      msg_gamesync_sel_02, !bIsDispedGuide ); // 説明を読む
  }
  // 構成E:ゲームシンクID発行済みで、PGLにコード登録し、ゲームシンクを行い、道具を受け取れる状態
  else
  if( bIsCreateGameSyncID && bIsExecuteGameSync && pMysteryGiftSave->IsMysteryGiftItemReceived() )
  {
    m_pGameSyncViewList->AddUpperView( m_pGameSyncViewObjects->GetGameSyncMainUpperView() );
    m_pGameSyncViewList->AddLowerViewInputEnable( m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView() );
    m_pGameSyncViewObjects->GetGameSyncMainUpperView()->SetGameSyncID( msg_gamesync_sys_04, true );
    m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView()->SetMessage( msg_gamesync_win_03,
      msg_gamesync_sel_03, false,             // ゲームシンクをする
      msg_gamesync_sel_02, !bIsDispedGuide ); // 説明を読む
  }
  // 構成D:ゲームシンクID発行済みで、PGLにコード登録し、ゲームシンクを行った
  else
  {
    m_pGameSyncViewList->AddUpperView( m_pGameSyncViewObjects->GetGameSyncMainUpperView() );
    m_pGameSyncViewList->AddLowerViewInputEnable( m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView() );
    m_pGameSyncViewObjects->GetGameSyncMainUpperView()->SetGameSyncID( msg_gamesync_sys_05, true );
    m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView()->SetMessage( msg_gamesync_win_03,
      msg_gamesync_sel_03, false,             // ゲームシンクをする
      msg_gamesync_sel_02, !bIsDispedGuide ); // 説明を読む
  }

}


void GameSyncMainFrame::setListener()
{
  const Savedata::GameSyncSave::GameSyncSaveData& gameSyncSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveData();

  bool bStartDescriptionFlag = gameSyncSaveData.m_config.m_isDispedFirstGuide;

  bool bPGLAlertFlag = gameSyncSaveData.m_config.m_isNoRegistPGL;

  if( !bStartDescriptionFlag )
  {
    m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetInputEnabled( true );
  }
  else
  if( bPGLAlertFlag )
  {
    m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetInputEnabled( true );
  }
  else
  {
    NetApp::GameSync::System::GameSyncWork::DescriptionMenuType eType = m_pGameSyncWork->GetDescriptionMenuType();

    if( eType == NetApp::GameSync::System::GameSyncWork::DESCRIPTION_MENU_TYPE_THREE )
    {
      m_pGameSyncViewObjects->GetGameSyncThreeSelectMenuLowerView()->SetInputEnabled( true );
    }
    else
    {
      m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView()->SetInputEnabled( true );
    }
  }

  m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetListener( &m_GameSyncMessageMenuLowerViewListener );
  m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView()->SetListener( &m_GameSyncTwoSelectMenuLowerViewListener );
  m_pGameSyncViewObjects->GetGameSyncThreeSelectMenuLowerView()->SetListener( &m_GameSyncThreeSelectMenuLowerViewListener );

  m_pGameSyncWork->SetDescriptionMenuType( NetApp::GameSync::System::GameSyncWork::DESCRIPTION_MENU_TYPE_NULL );
}


void GameSyncMainFrame::removeListener()
{
  m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->RemoveListener();
  m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView()->RemoveListener();
  m_pGameSyncViewObjects->GetGameSyncThreeSelectMenuLowerView()->RemoveListener();
}


void GameSyncMainFrame::updateSequence()
{
  switch( GetSubSeq() )
  {
    case SEQUENCE_DEFAULT:
    {
      sequenceDefault();
    }
    break;

    case SEQUENCE_SYNC_SAVE:
    {
      if( m_NetAppCommonSaveUtility.Save() )
      {
        m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetMessageBlackFilter( msg_gamesync_win_05 );

        NetAppLib::GameSync::GETPLAYSTATUS_PARAM param;
        std::memset( &param , 0 , sizeof(param) );
        param.pWork = m_pGameSyncWork;
        param.mode = NetAppLib::GameSync::GETPLAYSTATUS_MODE_GAMESYNC;

        NetAppLib::GameSync::GameSyncRequestFacade::AddConfirmPlayStatusRequest( this, &param );

        SetSubSeq( SEQUENCE_CONFIRM_PLAY_STATUS_WAIT );
      }
    }
    break;

    case SEQUENCE_REQUEST_SAVE:
    {
      if( m_NetAppCommonSaveUtility.Save() )
      {
        m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetStreamMessageBlackFilter( msg_gamesync_win_09 );
        
        SetSubSeq( SEQUENCE_DEFAULT );
      }
    }
    break;
  }

  m_GameSyncMessageMenuLowerViewListener.ResetInputEvent();
  m_GameSyncTwoSelectMenuLowerViewListener.ResetInputEvent();
  m_GameSyncThreeSelectMenuLowerViewListener.ResetInputEvent();
}


void GameSyncMainFrame::sequenceDefault()
{
  // -------------------------------------------------------------------------------------------------
  // Message menu event.
  {
    u32 inputEventID          = m_GameSyncMessageMenuLowerViewListener.GetInputEventID();
    u32 messageID             = m_GameSyncMessageMenuLowerViewListener.GetMessageID();
    bool bEndStreamMessage    = ( inputEventID == NetApp::GameSync::View::GameSyncMessageMenuLowerViewListener::INPUT_EVENT_ID_END_STREAM_MESSAGE ) ? true : false;
    bool bTouchYesButton      = ( inputEventID == NetApp::GameSync::View::GameSyncMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON ) ? true : false;
    bool bTouchNoButton       = ( inputEventID == NetApp::GameSync::View::GameSyncMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON ) ? true : false;

    if( bEndStreamMessage )
    {
      switch( messageID )
      {
        case msg_gamesync_win_01:
        {
          const Savedata::GameSyncSave::GameSyncSaveData& gameSyncSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveData();

          bool bPGLAlertFlag = gameSyncSaveData.m_config.m_isNoRegistPGL;
          
          if( bPGLAlertFlag )
          {
            m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetStreamMessageBlackFilter( msg_gamesync_win_02 );
          }
          else
          {
            setupMainView();
          }

          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->SetDispedFirstGuideFlag( true );
        }
        break;

        case msg_gamesync_win_02:
        {
          setupMainView();

          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->SetIsNoRegistPGL( false );
        }
        break;

        case msg_gamesync_win_07:
        {
          m_pGameSyncViewList->RemoveLowerView();
        }
        break;

        case msg_gamesync_win_09:
        {
          setupMainView();
        }
        break;

        case msg_gamesync_win_10:
        {
          m_pGameSyncViewList->RemoveLowerView();
        }
        break;

        case msg_gamesync_win_11:
        {
          setupMainView();
        }
        break;
      }
    }

    if( messageID == msg_gamesync_win_04 )
    {
      if( bTouchYesButton )
      {
        bool bPGLAccessLockFlag = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->CheckLockGameSyncSaveFor5xxError();

        if( bPGLAccessLockFlag )
        {
          m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetStreamMessageBlackFilter( msg_gamesync_win_07 );
        }
        else
        {
          m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetMessageBlackFilter( msg_gamesync_win_05 );

          NetAppLib::GameSync::GAMESYNC_PARAM param;
          std::memset( &param , 0 , sizeof(param) );
          param.pWork = m_pGameSyncWork;
          param.pTrainerIconUtility = m_pGameSyncWork->GetTrainerIconUtility();
          param.pReplacePane = m_pGameSyncViewObjects->GetGameSyncMainUpperView()->GetIconPane();

          NetAppLib::GameSync::GameSyncRequestFacade::AddCreateSaveDataRequest( this, &param );
        }
      }
      else
      if( bTouchNoButton )
      {
        m_pGameSyncViewList->RemoveLowerView();
      }
    }
  }

  // -------------------------------------------------------------------------------------------------
  // Two select menu event.
  {
    u32 inputEventID           = m_GameSyncTwoSelectMenuLowerViewListener.GetInputEventID();
    u32 messageID              = m_GameSyncTwoSelectMenuLowerViewListener.GetMessageID();
    u32 selectMenuID           = m_GameSyncTwoSelectMenuLowerViewListener.GetSelectMenuID();
    bool bTouchBackButton      = ( inputEventID == NetApp::GameSync::View::GameSyncTwoSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bTouchSelectMenu      = ( inputEventID == NetApp::GameSync::View::GameSyncTwoSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SELECT_MENU ) ? true : false;

    if( bTouchBackButton )
    {
      switch( messageID )
      {
        // ------------------------------------------------------------------------
        case msg_gamesync_win_12:
        {
          setupMainView();
        }
        break;

        // ------------------------------------------------------------------------
        default:
        {
          exitFrame( NetApp::GameSync::MAIN_FRAME_RESULT_BACK );
        }
        break;
      }
    }
    else
    if( bTouchSelectMenu )
    {
      switch( messageID )
      {
        // ------------------------------------------------------------------------
        case msg_gamesync_win_03:
        {
          switch( selectMenuID )
          {
            case msg_gamesync_sel_01:
            {
              m_pGameSyncViewList->AddLowerViewInputEnable( m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView() );
              m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetStreamMessageYesNoBlackFilter( msg_gamesync_win_04 );
            }
            break;

            case msg_gamesync_sel_02:
            {
              const Savedata::GameSyncSave::GameSyncSaveData& gameSyncSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveData();

              bool bIsCreateGameSyncID = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->GetIsCreateGameSyncSaveID();
            
              m_pGameSyncViewObjects->InputDisableAll();
              m_pGameSyncViewList->RemoveAllView();

              m_pGameSyncViewList->AddUpperView( m_pGameSyncViewObjects->GetGameSyncBGUpperView() );

              if( !bIsCreateGameSyncID )
              {
                m_pGameSyncViewList->AddLowerViewInputEnable( m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView() );
                m_pGameSyncViewObjects->GetGameSyncTwoSelectMenuLowerView()->SetMessage( msg_gamesync_win_12,
                  msg_gamesync_sel_04, !gameSyncSaveData.m_config.m_isDispedGuide1,   // ゲームシンクとは？
                  msg_gamesync_sel_05, !gameSyncSaveData.m_config.m_isDispedGuide2 ); // ゲームシンクの準備
              }
              else
              {
                m_pGameSyncViewList->AddLowerViewInputEnable( m_pGameSyncViewObjects->GetGameSyncThreeSelectMenuLowerView() );
                m_pGameSyncViewObjects->GetGameSyncThreeSelectMenuLowerView()->SetMessage( msg_gamesync_win_12,
                  msg_gamesync_sel_04, !gameSyncSaveData.m_config.m_isDispedGuide1,   // ゲームシンクとは？
                  msg_gamesync_sel_05, !gameSyncSaveData.m_config.m_isDispedGuide2,   // ゲームシンクの準備
                  msg_gamesync_sel_06, !gameSyncSaveData.m_config.m_isDispedGuide3 ); // ゲームシンクのやりかた
              }
            }
            break;
          
            case msg_gamesync_sel_03:
            {
              m_pGameSyncViewList->AddLowerViewInputEnable( m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView() );

              m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetMessageBlackFilter( msg_gamesync_win_08 );

              SetSubSeq( SEQUENCE_SYNC_SAVE );
            }
            break;
          }
        }
        break;

        // ------------------------------------------------------------------------
        case msg_gamesync_win_12:
        {
          switch( selectMenuID )
          {
            case msg_gamesync_sel_04:
            case msg_gamesync_sel_05:
            {
              m_pGameSyncWork->SetDescriptionType( selectMenuID );
              m_pGameSyncWork->SetDescriptionMenuType( NetApp::GameSync::System::GameSyncWork::DESCRIPTION_MENU_TYPE_TWO );
              exitFrame( NetApp::GameSync::MAIN_FRAME_RESULT_GO_TO_DESCRIPTION );
            }
            break;
          }
        }
        break;
      }
    }
  }

  // -------------------------------------------------------------------------------------------------
  // Three select menu event.
  {
    u32 inputEventID           = m_GameSyncThreeSelectMenuLowerViewListener.GetInputEventID();
    u32 messageID              = m_GameSyncThreeSelectMenuLowerViewListener.GetMessageID();
    u32 selectMenuID           = m_GameSyncThreeSelectMenuLowerViewListener.GetSelectMenuID();
    bool bTouchBackButton      = ( inputEventID == NetApp::GameSync::View::GameSyncThreeSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bTouchSelectMenu      = ( inputEventID == NetApp::GameSync::View::GameSyncThreeSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SELECT_MENU ) ? true : false;

    if( bTouchBackButton )
    {
      setupMainView();
    }
    else
    if( bTouchSelectMenu )
    {
      switch( messageID )
      {
        // ------------------------------------------------------------------------
        case msg_gamesync_win_12:
        {
          switch( selectMenuID )
          {
            case msg_gamesync_sel_04:
            case msg_gamesync_sel_05:
            case msg_gamesync_sel_06:
            {
              m_pGameSyncWork->SetDescriptionType( selectMenuID );
              m_pGameSyncWork->SetDescriptionMenuType( NetApp::GameSync::System::GameSyncWork::DESCRIPTION_MENU_TYPE_THREE );
              exitFrame( NetApp::GameSync::MAIN_FRAME_RESULT_GO_TO_DESCRIPTION );
            }
            break;
          }
        }
        break;
      }
    }
  }
}


void GameSyncMainFrame::OnCreateSaveDataSuccess()
{
  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveDataDirect().m_config.m_isUploaded = true;
  m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetMessageBlackFilter( msg_gamesync_win_08 );

  SetSubSeq( SEQUENCE_REQUEST_SAVE );
}


void GameSyncMainFrame::OnCreateSaveDataError()
{
  m_pGameSyncViewList->RemoveLowerView();
}


void GameSyncMainFrame::OnConfirmPlayStatusSuccess()
{
  const Savedata::GameSyncSave::GameSyncSaveData& gameSyncSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveData();

  bool isSigned = gameSyncSaveData.m_registInf.m_isSigned;

  if( isSigned == false )
  {//PGLアカウント紐付けしてない
    m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetStreamMessageBlackFilter( msg_gamesync_win_02 );

    SetSubSeq( SEQUENCE_DEFAULT );
  }
  else
  {//PGLアカウント紐付け済み
    m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetMessageBlackFilter( msg_gamesync_win_06 );

    NetAppLib::GameSync::GAMESYNC_PARAM param;
    std::memset( &param , 0 , sizeof(param) );
    param.pWork = m_pGameSyncWork;
    param.pTrainerIconUtility = m_pGameSyncWork->GetTrainerIconUtility();
    param.pReplacePane = m_pGameSyncViewObjects->GetGameSyncMainUpperView()->GetIconPane();

    NetAppLib::GameSync::GameSyncRequestFacade::AddSaveDataSyncRequest( this, &param );

    SetSubSeq( SEQUENCE_SAVE_DATA_SYNC_WAIT );
  }
}


void GameSyncMainFrame::OnConfirmPlayStatusError( u32 errorCode )
{
  setupMainView();

  SetSubSeq( SEQUENCE_DEFAULT );
}


void GameSyncMainFrame::OnSaveDataSyncSuccess()
{
  m_pGameSyncViewObjects->GetGameSyncMessageMenuLowerView()->SetStreamMessageBlackFilter( msg_gamesync_win_11 );

  SetSubSeq( SEQUENCE_DEFAULT );
}


void GameSyncMainFrame::OnSaveDataSyncError( u32 errorCode )
{
  setupMainView();

  SetSubSeq( SEQUENCE_DEFAULT );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)
