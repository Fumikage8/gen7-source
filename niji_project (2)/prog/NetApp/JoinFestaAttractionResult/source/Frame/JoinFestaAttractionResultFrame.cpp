// ============================================================================
/*
 * @file JoinFestaAttractionResultFrame.cpp
 * @brief アトラクション結果アプリの○○プロセスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionResult/source/Frame/JoinFestaAttractionResultFrame.h"
#include "NetApp/JoinFestaAttractionResult/source/JoinFestaAttractionResultFrameResult.h"

#include "NetApp/JoinFestaAttractionResult/source/View/JoinFestaAttractionResultUpperView.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRankingLowerView.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

#include "Sound/include/Sound.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(Frame)

enum Sequence
{
  SEQUENCE_ANIME_SCORE,
  SEQUENCE_ANIME_STAR,
  SEQUENCE_SCORE_UPDATE,
  SEQUENCE_MESSAGE_DRAW,
  SEQUENCE_ANIME_GET_FES_COIN,
  SEQUENCE_INPUT_WAIT_1,
  SEQUENCE_INPUT_WAIT_2,
  SEQUENCE_ME_WAIT,
  SEQUENCE_INPUT_WAIT_3
};

const u32 DIGIT_MAX             = 10;
const u32 SCORE_ANIME_FRAME     = 12;
const u32 STAR_ANIME_FRAME      = 3;

const u32 TOTAL_SCORE_ANIME_DIGIT = 5;
const u32 MY_SCORE_ANIME_DIGIT    = 3;


JoinFestaAttractionResultFrame::JoinFestaAttractionResultFrame(
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork,
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewSystemAccessor* pJoinFestaAttractionResultViewSystemAccessor ) :
  NetApp::JoinFestaAttractionResult::Frame::JoinFestaAttractionResultFrameBase( pJoinFestaAttractionResultWork, pJoinFestaAttractionResultViewSystemAccessor ),
  m_JoinFestaPlayerListMessageMenuLowerViewListener(),
  m_JoinFestaRankingLowerViewListener(),
  m_FrameCount( 0 ),
  m_TotalScore( 0 ),
  m_MyScore( 0 ),
  m_TotalScoreDigit( 0 ),
  m_MyScoreDigit( 0 ),
  m_DigitMax( 0 ),
  m_TotalStarDrawNum( 0 ),
  m_MyStarDrawNum( 0 ),
  m_StarDrawMax( 0 ),
  m_GetFesCoin( 0 ),
  m_GetFesCoinDigit( 0 ),
  m_bIsTotalScoreUpdate( false ),
  m_bIsMyScoreUpdate( false ),
  m_Random(),
  m_MessageStrBuf( 1024, pJoinFestaAttractionResultWork->GetDeviceHeap() ),
  m_bIsMePlaying(false),
  m_PlayME( STRM_ME_FESTA_SUCCESS ),
  m_bIsFinishRequest( false )
{
}


JoinFestaAttractionResultFrame::~JoinFestaAttractionResultFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaAttractionResultFrame::startup()
{
  bool bResult = false;

  NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

  if( m_bIsFinishRequest == false )
  {
    pJoinFestaAttractionManager->FinishRequest();

    m_bIsFinishRequest = true;
  }

  if( pJoinFestaAttractionManager->IsEndRequest() )
  {
    m_pJoinFestaAttractionResultViewObjects->InputDisableAll();
    m_pJoinFestaAttractionResultViewList->RemoveAllView();

    m_pJoinFestaAttractionResultViewList->AddUpperView( m_pJoinFestaAttractionResultViewObjects->GetJoinFestaPlayerListBGUpperView() );
    m_pJoinFestaAttractionResultViewList->AddUpperView( m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView() );

    m_pJoinFestaAttractionResultViewList->AddLowerView( m_pJoinFestaAttractionResultViewObjects->GetJoinFestaPlayerListBGLowerView() );


    NetAppLib::JoinFestaUI::JoinFestaRankingListManager& joinFestaRankingListManager = m_pJoinFestaAttractionResultWork->GetJoinFestaRankingListManager();

    joinFestaRankingListManager.SetupRankingDataList();

    m_pJoinFestaAttractionResultViewObjects->GetJoinFestaRankingLowerView()->SetVisibleUpdateButton( false );
    m_pJoinFestaAttractionResultViewObjects->GetJoinFestaRankingLowerView()->SetVisibleDecideButton( false );
    m_pJoinFestaAttractionResultViewObjects->GetJoinFestaRankingLowerView()->SetVisibleMenuCursor( false );
    m_pJoinFestaAttractionResultViewObjects->GetJoinFestaRankingLowerView()->SetList( &joinFestaRankingListManager.GetJoinFestaRankingDataList() );


    NetAppLib::JoinFesta::JoinFestaAttractionDefine::ResultData& resultData = pJoinFestaAttractionManager->GetResultData();


    // -------------------------------------------------
    {
      m_TotalScore = resultData.totalScore;
      m_MyScore = resultData.privateScore;

      m_TotalScoreDigit = TOTAL_SCORE_ANIME_DIGIT; // getDigit( m_TotalScore );
      m_MyScoreDigit = MY_SCORE_ANIME_DIGIT; // getDigit( m_MyScore );

      m_DigitMax = ( m_TotalScoreDigit > m_MyScoreDigit ) ? m_TotalScoreDigit : m_MyScoreDigit;

      m_Random.Initialize();
    }

    // -------------------------------------------------
    {
      m_TotalStarDrawNum = resultData.totalEvaluationStarNum;
      m_MyStarDrawNum = resultData.manEvaluationStarNum;

      m_StarDrawMax = ( m_TotalStarDrawNum > m_MyStarDrawNum ) ? m_TotalStarDrawNum : m_MyStarDrawNum;
    }

    // -------------------------------------------------
    {
      m_GetFesCoin = resultData.getFesCoin;

      m_GetFesCoinDigit = getDigit( m_GetFesCoin );
    }

    // -------------------------------------------------
    {
      m_bIsTotalScoreUpdate = resultData.isTotalScoreNewRecord;
      m_bIsMyScoreUpdate = resultData.isPrivateScoreNewRecord;
    }

    m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView()->SetupAttractionResultInfo();

    bResult = true;
  }

  return bResult;
}


void JoinFestaAttractionResultFrame::setListener()
{
  m_pJoinFestaAttractionResultViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetListener( &m_JoinFestaPlayerListMessageMenuLowerViewListener );
  m_pJoinFestaAttractionResultViewObjects->GetJoinFestaRankingLowerView()->SetListener( &m_JoinFestaRankingLowerViewListener );
}


void JoinFestaAttractionResultFrame::removeListener()
{
  m_pJoinFestaAttractionResultViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->RemoveListener();
  m_pJoinFestaAttractionResultViewObjects->GetJoinFestaRankingLowerView()->RemoveListener();
}


void JoinFestaAttractionResultFrame::updateSequence()
{
  switch( GetSubSeq() )
  {
    // ------------------------------------------------------------------------------------
    case SEQUENCE_ANIME_SCORE:
    {
      m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView()->SetVisibleScore( true );

      u32 totalScore = randomScore( m_FrameCount, m_TotalScore, m_TotalScoreDigit );
      u32 myScore = randomScore( m_FrameCount, m_MyScore, m_MyScoreDigit );

      if( !Sound::IsSEPlaying( SEQ_SE_JF_COUNTUP ) )
      {
        Sound::PlaySE( SEQ_SE_JF_COUNTUP );
      }

      m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView()->SetTotalScore( totalScore, m_TotalScoreDigit );
      m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView()->SetMyScore( myScore, m_MyScoreDigit );

      if( m_FrameCount > ( SCORE_ANIME_FRAME * m_DigitMax ) )
      {
        m_FrameCount = 0;

        for( u32 i = 0; i < 5; ++i )
        {
          if( i < m_TotalStarDrawNum )
          {
            m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView()->SetVisibleTotalStarIcon( true, i );
          }

          if( i < m_MyStarDrawNum )
          {
            m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView()->SetVisibleMyStarIcon( true, i );
          }
        }

        m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView()->StartTotalStarIconAnimation( m_TotalStarDrawNum );
        m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView()->StartMyStarIconAnimation( m_MyStarDrawNum );

        SetSubSeq( SEQUENCE_ANIME_STAR );
      }
    }
    break;

    // ------------------------------------------------------------------------------------
    case SEQUENCE_ANIME_STAR:
    {
      if( m_FrameCount % STAR_ANIME_FRAME == 0 )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
      }

      if( m_FrameCount > ( STAR_ANIME_FRAME * m_StarDrawMax ) )
      {
        m_FrameCount = 0;

        if( m_bIsTotalScoreUpdate || m_bIsMyScoreUpdate )
        {
          SetSubSeq( SEQUENCE_SCORE_UPDATE );
        }
        else
        {
          SetSubSeq( SEQUENCE_ANIME_GET_FES_COIN );
        }
      }
    }
    break;
    
    // ------------------------------------------------------------------------------------
    case SEQUENCE_SCORE_UPDATE:
    {
      if( m_bIsTotalScoreUpdate )
      {
        m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView()->SetVisibleTotalScoreUpdateMessage( true );
      }

      if( m_bIsMyScoreUpdate )
      {
        m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView()->SetVisibleMyScoreUpdateMessage( true );
      }

      SetSubSeq( SEQUENCE_ANIME_GET_FES_COIN );
    }
    break;

    // ------------------------------------------------------------------------------------
    case SEQUENCE_ANIME_GET_FES_COIN:
    {
#if 0  // @note : 取得できるコインのアニメーションを行う場合は 1
      m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView()->SetVisibleGetFesCoin( true );

      u32 getFesCoin = randomScore( m_FrameCount, m_GetFesCoin, m_GetFesCoinDigit );

      if( !Sound::IsSEPlaying( SEQ_SE_JF_COUNTUP ) )
      {
        Sound::PlaySE( SEQ_SE_JF_COUNTUP );
      }

      m_pJoinFestaAttractionResultViewObjects->GetJoinFestaAttractionResultUpperView()->SetGetFesCoin( getFesCoin, m_GetFesCoinDigit );

      if( m_FrameCount > ( SCORE_ANIME_FRAME * m_GetFesCoinDigit ) )
      {
        SetSubSeq( SEQUENCE_MESSAGE_DRAW );
      }
#else

      SetSubSeq( SEQUENCE_MESSAGE_DRAW );

#endif
    }
    break;

    // ------------------------------------------------------------------------------------
    case SEQUENCE_MESSAGE_DRAW:
    {
      gfl2::str::StrBuf& strbuf = m_pJoinFestaAttractionResultWork->GetMessageUtility()->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_win_25 );

      print::WordSet* pWordSet = m_pJoinFestaAttractionResultWork->GetMessageUtility()->GetWordSet();
      pWordSet->ClearAllBuffer();
      pWordSet->RegisterNumber( 0, m_TotalScore, m_TotalScoreDigit, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 1, m_TotalStarDrawNum, getDigit( m_TotalStarDrawNum ), print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 2, m_MyScore, m_MyScoreDigit, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 3, m_MyStarDrawNum, getDigit( m_MyStarDrawNum ), print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 4, m_TotalStarDrawNum + m_MyStarDrawNum, getDigit( m_TotalStarDrawNum + m_MyStarDrawNum ), print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
      pWordSet->RegisterNumber( 5, m_GetFesCoin, getDigit( m_GetFesCoin ), print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );

      pWordSet->Expand( &m_MessageStrBuf, &strbuf );

      m_pJoinFestaAttractionResultViewList->AddLowerViewInputEnable( m_pJoinFestaAttractionResultViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
      m_pJoinFestaAttractionResultViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessage( jf_playerlist_win_25, m_MessageStrBuf );

      SetSubSeq( SEQUENCE_INPUT_WAIT_1 );
    }
    break;

    // ------------------------------------------------------------------------------------
    case SEQUENCE_INPUT_WAIT_1:
    {
      switch( m_JoinFestaPlayerListMessageMenuLowerViewListener.GetInputEventID() )
      {
        case NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN:
        {
          bool bAttractionSuccess = ( m_GetFesCoin > 0 ) ? true : false;

          m_PlayME = ( bAttractionSuccess ) ? STRM_ME_FESTA_SUCCESS : STRM_ME_FESTA_FAILURE;

          Sound::StartME( m_PlayME );

          m_bIsMePlaying = true;

          if( bAttractionSuccess )
          {
            gfl2::str::StrBuf& strbuf = m_pJoinFestaAttractionResultWork->GetMessageUtility()->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_win_27 );

            u32 val = m_TotalStarDrawNum + m_MyStarDrawNum;

            print::WordSet* pWordSet = m_pJoinFestaAttractionResultWork->GetMessageUtility()->GetWordSet();
            pWordSet->ClearAllBuffer();
            pWordSet->RegisterNumber( 4, val, getDigit( val ), print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
            pWordSet->RegisterNumber( 5, m_GetFesCoin, getDigit( m_GetFesCoin ), print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );

            pWordSet->Expand( &m_MessageStrBuf, &strbuf );

            m_pJoinFestaAttractionResultViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessage( jf_playerlist_win_27, m_MessageStrBuf );
          }
          else
          {
            gfl2::str::StrBuf& strbuf = m_pJoinFestaAttractionResultWork->GetMessageUtility()->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_win_28 );

            u32 val = m_TotalStarDrawNum + m_MyStarDrawNum;

            print::WordSet* pWordSet = m_pJoinFestaAttractionResultWork->GetMessageUtility()->GetWordSet();
            pWordSet->ClearAllBuffer();
            pWordSet->RegisterNumber( 4, val, getDigit( val ), print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );

            pWordSet->Expand( &m_MessageStrBuf, &strbuf );

            m_pJoinFestaAttractionResultViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessage( jf_playerlist_win_28, m_MessageStrBuf );
          }

          SetSubSeq( SEQUENCE_INPUT_WAIT_2 );
        }
        break;
      }
    }
    break;

    // ------------------------------------------------------------------------------------
    case SEQUENCE_INPUT_WAIT_2:
    {
      switch( m_JoinFestaPlayerListMessageMenuLowerViewListener.GetInputEventID() )
      {
        case NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN:
        {
          SetSubSeq( SEQUENCE_ME_WAIT );
        }
        break;
      }
      checkPlayME();
    }
    break;

    // ------------------------------------------------------------------------------------
    case SEQUENCE_ME_WAIT:
    {
      if( !m_bIsMePlaying )
      {
        m_pJoinFestaAttractionResultViewList->RemoveLowerView();
        m_pJoinFestaAttractionResultViewList->AddLowerViewInputEnable( m_pJoinFestaAttractionResultViewObjects->GetJoinFestaRankingLowerView() );

        SetSubSeq( SEQUENCE_INPUT_WAIT_3 );
      }
      checkPlayME();
    }
    break;

    // ------------------------------------------------------------------------------------
    case SEQUENCE_INPUT_WAIT_3:
    {
      switch( m_JoinFestaRankingLowerViewListener.GetInputEventID() )
      {
        case NetAppLib::JoinFestaUI::JoinFestaRankingLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
        {
          exitFrame( NetApp::JoinFestaAttractionResult::RESULT_FRAME_RESULT_BACK );
        }
        break;
      }
    }
    break;
  }

  m_FrameCount++;

  m_JoinFestaPlayerListMessageMenuLowerViewListener.ResetInputEvent();
  m_JoinFestaRankingLowerViewListener.ResetInputEvent();
}


void JoinFestaAttractionResultFrame::checkPlayME()
{
  if( m_bIsMePlaying && Sound::IsMEFinished( m_PlayME ) )
  {
    m_bIsMePlaying = false;
    Sound::StartBaseBGMFromME();
  }
}


u32 JoinFestaAttractionResultFrame::randomScore( u32 frameCount, u32 targetScore, u32 digitMax )
{
  u32 temp = targetScore;

  u32 nums[ DIGIT_MAX ] = { 0 };

  for( u32 i = 0; i < DIGIT_MAX; ++i )
  {
    if( i >= digitMax )
    {
      nums[i] = 0;
    }
    else
    if( frameCount < ( ( i + 1 ) * SCORE_ANIME_FRAME ) )
    {
      nums[i] = m_Random.Next(10);
    }
    else
    {
      nums[i] = temp % 10;
    }

    temp /= 10;
  }

  u32 score = 0;

  temp = 1;

  for( u32 i = 0; i < DIGIT_MAX; ++i )
  {
    score += ( nums[i] * temp );

    temp *= 10;
  }

  return score;
}


u32 JoinFestaAttractionResultFrame::getDigit( u32 num )
{
  u32 digit = static_cast<u32>( std::log10f( static_cast<float>( num ) ) ) + 1;

  return digit;
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)
