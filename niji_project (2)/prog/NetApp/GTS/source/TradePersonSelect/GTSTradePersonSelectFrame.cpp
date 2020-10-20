// ============================================================================
/*
 * @file GTSTradePersonSelectFrame.cpp
 * @brief 交換相手を選択するプロセスです。
 * @date 2015.03.18
 */
// ============================================================================
#include "NetApp/GTS/source/TradePersonSelect/GTSTradePersonSelectFrame.h"
#include "NetApp/GTS/source/TradePersonSelect/GTSTradePersonSelectLowerView.h"
#include "NetApp/GTS/source/SearchPokemon/GTSSearchPokemonLowerView.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerView.h"
#include "NetApp/GTS/source/Window/GTSRequestInfoWindowUpperView.h"
#include "NetApp/GTS/source/RequestSequence/GTSRequestFacade.h"
#include "NetApp/GTS/source/GTSFrameResult.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSViewObjects.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include <niji_conv_header/message/msg_gts.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(TradePersonSelect)


GTSTradePersonSelectFrame::GTSTradePersonSelectFrame(
  NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
  NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects ) :
  NetApp::GTS::GTSFrameBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pGTSViewObjects( pGTSViewObjects ),
  m_bBegin( true ),
  m_GTSTradePersonSelectLowerViewListener(),
  m_GTSMessageWindowLowerViewListener(),
  m_RetrySearchCount( 0 ),
  m_bIsSearchPokemonRequest( false ),
  m_bCursorVisibleFlag( false )
{
}


GTSTradePersonSelectFrame::~GTSTradePersonSelectFrame()
{
}


//-----------------------------------------------------------------------------
// GTSFrameBase
//-----------------------------------------------------------------------------
bool GTSTradePersonSelectFrame::startup()
{
  m_pGTSViewObjects->InputDisableAll();
  m_pGTSWork->GetRootUpperView()->RemoveAllSubView();
  m_pGTSWork->GetRootLowerView()->RemoveAllSubView();

  addUpperViewInputDisable( m_pGTSViewObjects->GetGTSRequestInfoWindowUpperView() );
  addLowerViewInputDisable( m_pGTSViewObjects->GetGTSTradePersonSelectLowerView() );

  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBlackFilter( true );

  m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->SetListener( &m_GTSTradePersonSelectLowerViewListener );
  m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->ResetView( true );

  if( m_GTSTradePersonSelectLowerViewListener.IsUpdateView() )
  {
    NetApp::GTS::GtsIndexData* pGTSIndexData = m_GTSTradePersonSelectLowerViewListener.GetGTSIndexData();
    if( pGTSIndexData )
    {
      m_pGTSViewObjects->GetGTSRequestInfoWindowUpperView()->SetGTSIndexData( *pGTSIndexData, false );
    }
  }

  return true;
}


void GTSTradePersonSelectFrame::setListener()
{
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetListener( &m_GTSMessageWindowLowerViewListener );

  m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->SetInputEnabled( true );
}


void GTSTradePersonSelectFrame::removeListener()
{
  // [NCL要望]:検索画面に戻る時はカーソルの位置を『欲しいポケモン』ボタンの位置に合わせて欲しい。サーバー負荷対策の為。
  m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->CursorMoveToIndex(0);

  m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->RemoveListener();
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->RemoveListener();

  m_pGTSViewObjects->InputDisableAll();
}


void GTSTradePersonSelectFrame::updateSequence()
{
  bool bIsPaneListAction = m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->IsPaneListAction();
  u32 customInputEventID = m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->InputCustom();
  u32 gtsTradePersonSelectLowerViewInputEventID = m_GTSTradePersonSelectLowerViewListener.GetInputEventID();

  if( customInputEventID != NetAppLib::System::NetAppViewListenerBase::INPUT_EVENT_ID_NONE )
  {
    gtsTradePersonSelectLowerViewInputEventID = customInputEventID;
  }

  // 検索リクエストを投げている時は、何かしらの入力イベントが来てしまっても全てなかったことにする
  if( m_bIsSearchPokemonRequest || bIsPaneListAction )
  {
    gtsTradePersonSelectLowerViewInputEventID = NetAppLib::System::NetAppViewListenerBase::INPUT_EVENT_ID_NONE;
  }

  if( bIsPaneListAction )
  {
    m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->SetPaneListSetInputInvalidMode( true );
    m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->SetInputEnabled( false );
    m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->SetCursorVisible( false );
    m_bCursorVisibleFlag = true;
  }
  else
  {
    if( m_bCursorVisibleFlag )
    {
      m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->SetInputEnabled( true );
      m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->SetPaneListSetInputInvalidMode( false );
      m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->ReUpdate();
      m_bCursorVisibleFlag = false;
    }
  }

  switch( gtsTradePersonSelectLowerViewInputEventID )
  {
    case NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      NetApp::GTS::ApplicationSystem::GtsSearchResultList& refineSearchPokemonResultList = m_pGTSWork->GetRefineSearchPokemonResultList();
      refineSearchPokemonResultList.Erase();

      exitFrame( NetApp::GTS::TRADE_PERSON_SELECT_RESULT_BACK );
    }
    break;

    case NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerViewListener::INPUT_EVENT_ID_TOUCH_REFINE_SEARCH_BUTTON:
    {
      exitFrame( NetApp::GTS::TRADE_PERSON_SELECT_RESULT_REFINE_SEARCH );
    }
    break;

    case NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerViewListener::INPUT_EVENT_ID_TRADE_PERSON_SELECT:
    {
      NetApp::GTS::GtsSearchResultInfo* pSelecctTradePartnerInfo = m_GTSTradePersonSelectLowerViewListener.GetSelecctTradePartnerInfo();
      if( pSelecctTradePartnerInfo )
      {
        m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_BOX );

        m_pGTSWork->SetTradePartnerInfo( *pSelecctTradePartnerInfo );

        exitFrame( NetApp::GTS::TRADE_PERSON_SELECT_RESULT_SELECT );
      }
    }
    break;

    case NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerViewListener::INPUT_EVENT_ID_SEARCH_POKEMON_PREVIOUS:
    {
      u32 startOffset = m_pGTSWork->GetSearchRangeStartOffset();
      if( startOffset >= 100 )
      {
        startOffset -= 100;
        m_pGTSWork->SetSearchRangeStartOffset( startOffset );
        NetApp::GTS::RequestSequence::GTSRequestFacade::AddSearchPokemonRequest( this, m_pGTSWork );
        m_bIsSearchPokemonRequest = true;
        m_bBegin = false;
        m_RetrySearchCount = 0;

        addLowerViewInputDisable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );
        m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetMessage( msg_gts_win_25 );
      }
    }
    break;

    case NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerViewListener::INPUT_EVENT_ID_SEARCH_POKEMON_NEXT:
    {
      u32 startOffset = m_pGTSWork->GetSearchRangeStartOffset();
      startOffset += 100;
      m_pGTSWork->SetSearchRangeStartOffset( startOffset );
      NetApp::GTS::RequestSequence::GTSRequestFacade::AddSearchPokemonRequest( this, m_pGTSWork );
      m_bIsSearchPokemonRequest = true;
      m_bBegin = true;
      m_RetrySearchCount = 0;

      addLowerViewInputDisable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );
      m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetMessage( msg_gts_win_25 );
    }
    break;
  }

  if( m_GTSTradePersonSelectLowerViewListener.IsUpdateView() )
  {
    NetApp::GTS::GtsIndexData* pGTSIndexData = m_GTSTradePersonSelectLowerViewListener.GetGTSIndexData();
    if( pGTSIndexData )
    {
      m_pGTSViewObjects->GetGTSRequestInfoWindowUpperView()->SetGTSIndexData( *pGTSIndexData, false );
    }
  }

  switch( m_GTSMessageWindowLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::Window::GTSMessageWindowLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN:
    {
      switch( m_GTSMessageWindowLowerViewListener.GetMessageID() )
      {
        case msg_gts_win_27:
        {
          removeLowerView();
          m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->ResetView( m_bBegin );

          NetApp::GTS::GtsIndexData* pGTSIndexData = m_GTSTradePersonSelectLowerViewListener.GetGTSIndexData();

          if( pGTSIndexData )
          {
            m_pGTSViewObjects->GetGTSRequestInfoWindowUpperView()->SetGTSIndexData( *pGTSIndexData, false );
          }

          m_bIsSearchPokemonRequest = false;
        }
        break;

        case msg_gts_win_28:
        {
          NetApp::GTS::ApplicationSystem::GtsSearchResultList& refineSearchPokemonResultList = m_pGTSWork->GetRefineSearchPokemonResultList();
          refineSearchPokemonResultList.Erase();

          removeLowerView();

          exitFrame( NetApp::GTS::TRADE_PERSON_SELECT_RESULT_BACK );
        }
        break;
      }
    }
    break;
  }

  m_GTSTradePersonSelectLowerViewListener.ResetEvent();
  m_GTSMessageWindowLowerViewListener.ResetInputEvent();

}


//-----------------------------------------------------------------------------
// GTSSearchPokemonResponseListener
//-----------------------------------------------------------------------------
void GTSTradePersonSelectFrame::OnSearchPokemonSuccess()
{
  NetApp::GTS::ApplicationSystem::GtsSearchResultList& searchPokemonResultList = m_pGTSWork->GetSearchPokemonResultList();

  if( searchPokemonResultList.Size() > 0 )
  {
    if( m_RetrySearchCount == 0 )
    {
      removeLowerView();
      m_pGTSViewObjects->GetGTSTradePersonSelectLowerView()->ResetView( m_bBegin );

      m_bIsSearchPokemonRequest = false;
    }
    else
    {
      m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_27 );
    }
  }
  else
  {
    if( m_RetrySearchCount > 0 )
    {
      m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_28 );

      return;
    }

    m_pGTSWork->SetSearchRangeStartOffset( 0 );
    NetApp::GTS::RequestSequence::GTSRequestFacade::AddSearchPokemonRequest( this, m_pGTSWork );
    m_bIsSearchPokemonRequest = true;
    m_bBegin = true;
    ++m_RetrySearchCount;
  }
}


void GTSTradePersonSelectFrame::OnSearchPokemonCancel()
{
  m_bIsSearchPokemonRequest = false;
}


void GTSTradePersonSelectFrame::OnSearchPokemonError()
{
  m_bIsSearchPokemonRequest = false;

  removeLowerView();

  exitFrame( NetApp::GTS::TRADE_PERSON_SELECT_RESULT_ERROR );
}


GFL_NAMESPACE_END(TradePersonSelect)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
