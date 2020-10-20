// ============================================================================
/*
 * @file JoinFestaSelectWordMainFrame.cpp
 * @brief 挨拶選択アプリの○○プロセスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/Frame/JoinFestaSelectWordMainFrame.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordFrameResult.h"

#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordBGUpperView.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordUpperView.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordBGLowerView.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordLowerView.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordMessageMenuLowerView.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <niji_conv_header/message/msg_setphrase.h>
#include <niji_conv_header/message/msg_jf_phrase.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(Frame)


static const int SEQUENCE_SELECT_CATEGORY = 0;
static const int SEQUENCE_SELECT_MESSAGE  = 1;



JoinFestaSelectWordMainFrame::JoinFestaSelectWordMainFrame(
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork,
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor* pJoinFestaSelectWordViewSystemAccessor ) :
  NetApp::JoinFestaSelectWord::Frame::JoinFestaSelectWordFrameBase( pJoinFestaSelectWordWork, pJoinFestaSelectWordViewSystemAccessor ),
  m_JoinFestaSelectWordLowerViewListener(),
  m_JoinFestaSelectWordMessageMenuLowerViewListener(),
  m_SelectCategoryMessageID( 0 ),
  m_bSequenceInitFlag( false )
{
}


JoinFestaSelectWordMainFrame::~JoinFestaSelectWordMainFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaSelectWordMainFrame::startup()
{
  m_pJoinFestaSelectWordViewObjects->InputDisableAll();
  m_pJoinFestaSelectWordViewList->RemoveAllView();

  m_pJoinFestaSelectWordViewList->AddUpperView( m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordBGUpperView() );
  m_pJoinFestaSelectWordViewList->AddUpperView( m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordUpperView() );

  m_pJoinFestaSelectWordViewList->AddLowerView( m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordBGLowerView() );
  m_pJoinFestaSelectWordViewList->AddLowerView( m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordLowerView() );

  setupScreen( m_pJoinFestaSelectWordWork->GetMainFrameSequenceNo() );

  return true;
}


void JoinFestaSelectWordMainFrame::setListener()
{
  m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordLowerView()->SetListener( &m_JoinFestaSelectWordLowerViewListener );
  m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordMessageMenuLowerView()->SetListener( &m_JoinFestaSelectWordMessageMenuLowerViewListener );

  m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordLowerView()->SetInputEnabled( true );
}


void JoinFestaSelectWordMainFrame::removeListener()
{
  m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordLowerView()->RemoveListener();
  m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordMessageMenuLowerView()->RemoveListener();
}


void JoinFestaSelectWordMainFrame::setupScreen( int sequenceNo )
{
  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  u16 selectCategoryMessageIDSaveData = pJoinFestaDataSave->GetPatternCateogry( getPatternType() );
  u16 selectMessageIDSaveData         = pJoinFestaDataSave->GetPattern( getPatternType() );

  if( sequenceNo == SEQUENCE_SELECT_CATEGORY )
  {
    m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordUpperView()->SetCategory( static_cast<u32>( selectCategoryMessageIDSaveData ), static_cast<u32>( selectMessageIDSaveData ) );
    m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordLowerView()->SetCategoryTable( m_pJoinFestaSelectWordWork->GetSelectWordTable().GetCategoryTable(), m_pJoinFestaSelectWordWork->GetSelectWordTable().GetCategoryTableSize() );

  }
  else
  if( sequenceNo == SEQUENCE_SELECT_MESSAGE )
  {
    u32 index = m_pJoinFestaSelectWordWork->GetSelectCategoryIndex();

    NetAppLib::JoinFesta::JoinFestaSelectWordTable::Category eSelectCategory = static_cast< NetAppLib::JoinFesta::JoinFestaSelectWordTable::Category >( index );

    m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordUpperView()->SetMessage( static_cast<u32>( selectCategoryMessageIDSaveData ), static_cast<u32>( selectMessageIDSaveData ) );
    m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordLowerView()->SetMessageTable( m_pJoinFestaSelectWordWork->GetSelectWordTable().GetMessageTable( eSelectCategory ), m_pJoinFestaSelectWordWork->GetSelectWordTable().GetMessageTableSize( eSelectCategory ) );
  }

  SetSubSeq( sequenceNo );

  m_pJoinFestaSelectWordWork->SetMainFrameSequenceNo( sequenceNo );

}


void JoinFestaSelectWordMainFrame::updateSequence()
{
  if( !m_bSequenceInitFlag )
  {
    SetSubSeq( m_pJoinFestaSelectWordWork->GetMainFrameSequenceNo() );
    m_bSequenceInitFlag = true;
  }

  switch( GetSubSeq() )
  {
    case SEQUENCE_SELECT_CATEGORY:
    {
      sequenceSelectCategory();
    }
    break;

    case SEQUENCE_SELECT_MESSAGE:
    {
      sequenceSelectMessage();
    }
    break;
  }

  m_JoinFestaSelectWordLowerViewListener.ResetInputEvent();
  m_JoinFestaSelectWordMessageMenuLowerViewListener.ResetInputEvent();
}


void JoinFestaSelectWordMainFrame::sequenceSelectCategory()
{
  //------------------------------------------------------------------------------------------------
  {
    u32  inputEventID          = m_JoinFestaSelectWordLowerViewListener.GetInputEventID();
    bool bTouchBackButton      = ( inputEventID == NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bSelectedMessage      = ( inputEventID == NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordLowerViewListener::INPUT_EVENT_ID_SELECT_MESSAGE ) ? true : false;
    bool bTouchInfoButton      = ( inputEventID == NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordLowerViewListener::INPUT_EVENT_ID_TOUCH_INFO_BUTTON ) ? true : false;
    m_SelectCategoryMessageID  = m_JoinFestaSelectWordLowerViewListener.GetSelectMessageID();

    if( bTouchBackButton )
    {
      exitFrame( NetApp::JoinFestaSelectWord::MAIN_FRAME_RESULT_BACK );
    }
    else
    if( bSelectedMessage )
    {
      m_pJoinFestaSelectWordWork->SetSelectCategoryIndex( m_JoinFestaSelectWordLowerViewListener.GetSelectListIndex() );

      setupScreen( SEQUENCE_SELECT_MESSAGE );
    }
    else
    if( bTouchInfoButton )
    {
      m_pJoinFestaSelectWordWork->SetDescriptionOpenAnimeFlag( true );
      exitFrame( NetApp::JoinFestaSelectWord::MAIN_FRAME_RESULT_TO_DESCRIPTION );
    }
  }

}


void JoinFestaSelectWordMainFrame::sequenceSelectMessage()
{
  //------------------------------------------------------------------------------------------------
  {
    u32  inputEventID        = m_JoinFestaSelectWordLowerViewListener.GetInputEventID();
    bool bTouchBackButton    = ( inputEventID == NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bSelectedMessage    = ( inputEventID == NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordLowerViewListener::INPUT_EVENT_ID_SELECT_MESSAGE ) ? true : false;
    bool bTouchInfoButton    = ( inputEventID == NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordLowerViewListener::INPUT_EVENT_ID_TOUCH_INFO_BUTTON ) ? true : false;
    u32  selectMessageID     = m_JoinFestaSelectWordLowerViewListener.GetSelectMessageID();

    if( bTouchBackButton )
    {
      setupScreen( SEQUENCE_SELECT_CATEGORY );
    }
    else
    if( bSelectedMessage )
    {
      Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

      bool bSetFlag = false;

      JoinFestaScript::PatternType patternType = JoinFestaScript::PATTERN_HELLO;

      switch( m_pJoinFestaSelectWordWork->GetEditMenuID() )
      {
        // 出会いの挨拶
        case NetApp::JoinFestaSelectWord::EDIT_MENU_ID_MEETING_MESSAGE:
        {
          patternType = JoinFestaScript::PATTERN_HELLO;
          bSetFlag = true;
        }
        break;

        // 感動した時
        case NetApp::JoinFestaSelectWord::EDIT_MENU_ID_THROB:
        {
          patternType = JoinFestaScript::PATTERN_EMOTION;
          bSetFlag = true;
        }
        break;

        // 残念な時
        case NetApp::JoinFestaSelectWord::EDIT_MENU_ID_REGRETFUL:
        {
          patternType = JoinFestaScript::PATTERN_REGRET;
          bSetFlag = true;
        }
        break;

        // 別れの挨拶
        case NetApp::JoinFestaSelectWord::EDIT_MENU_ID_GOODBY_MESSAGE:
        {
          patternType = JoinFestaScript::PATTERN_PARTING;
          bSetFlag = true;
        }
        break;

        // 私の秘密
        case NetApp::JoinFestaSelectWord::EDIT_MENU_ID_MY_SECRET:
        {
          patternType = JoinFestaScript::PATTERN_SECRET;
          bSetFlag = true;
        }
        break;
      }

      if( bSetFlag )
      {
        pJoinFestaDataSave->SetPattern( patternType, static_cast<u16>( selectMessageID ), static_cast<u16>( m_SelectCategoryMessageID ) );

#if defined( GF_PLATFORM_CTR )

        NetAppLib::JoinFesta::JoinFestaPacketManager* pJoinFestaPacketManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager );

        pJoinFestaPacketManager->SetPacketGameStatusWithMe();

        pJoinFestaPacketManager->SendMyData();

#endif // defined( GF_PLATFORM_CTR )
      }

      m_pJoinFestaSelectWordViewList->AddLowerViewInputEnable( m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordMessageMenuLowerView() );
      m_pJoinFestaSelectWordViewObjects->GetJoinFestaSelectWordMessageMenuLowerView()->SetStreamMessageBlackFilter( msg_setphrase_win_02, selectMessageID, false );
    }
    else
    if( bTouchInfoButton )
    {
      m_pJoinFestaSelectWordWork->SetDescriptionOpenAnimeFlag( true );
      exitFrame( NetApp::JoinFestaSelectWord::MAIN_FRAME_RESULT_TO_DESCRIPTION );
    }
  }

  //------------------------------------------------------------------------------------------------
  {
    u32 inputEventID    = m_JoinFestaSelectWordMessageMenuLowerViewListener.GetInputEventID();
    bool bTouchScreen   = ( inputEventID == NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN ) ? true : false;
    u32 messageID       = m_JoinFestaSelectWordMessageMenuLowerViewListener.GetMessageID();

    if( bTouchScreen )
    {
      if( messageID == msg_setphrase_win_02 )
      {
        exitFrame( NetApp::JoinFestaSelectWord::MAIN_FRAME_RESULT_BACK );
      }
    }
  }

}


JoinFestaScript::PatternType JoinFestaSelectWordMainFrame::getPatternType()
{
  JoinFestaScript::PatternType patternType = JoinFestaScript::PATTERN_HELLO;

  switch( m_pJoinFestaSelectWordWork->GetEditMenuID() )
  {
    // 出会いの挨拶
    case NetApp::JoinFestaSelectWord::EDIT_MENU_ID_MEETING_MESSAGE:
    {
      patternType = JoinFestaScript::PATTERN_HELLO;
    }
    break;

    // 感動した時
    case NetApp::JoinFestaSelectWord::EDIT_MENU_ID_THROB:
    {
      patternType = JoinFestaScript::PATTERN_EMOTION;
    }
    break;

    // 残念な時
    case NetApp::JoinFestaSelectWord::EDIT_MENU_ID_REGRETFUL:
    {
      patternType = JoinFestaScript::PATTERN_REGRET;
    }
    break;

    // 別れの挨拶
    case NetApp::JoinFestaSelectWord::EDIT_MENU_ID_GOODBY_MESSAGE:
    {
      patternType = JoinFestaScript::PATTERN_PARTING;
    }
    break;

    // 私の秘密
    case NetApp::JoinFestaSelectWord::EDIT_MENU_ID_MY_SECRET:
    {
      patternType = JoinFestaScript::PATTERN_SECRET;
    }
    break;
  }

  return patternType;
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)
