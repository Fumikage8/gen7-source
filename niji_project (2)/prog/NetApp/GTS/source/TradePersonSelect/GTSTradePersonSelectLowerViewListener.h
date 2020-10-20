// ============================================================================
/*
 * @file GTSTradePersonSelectLowerViewListener.h
 * @brief 交換相手を選択する画面の入力イベントを受け取る為のリスナーです。
 * @date 2015.04.15
 */
// ============================================================================
#if !defined( GTSTRADEPERSONSELECTLOWERVIEWLISTENER_H_INCLUDE )
#define GTSTRADEPERSONSELECTLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/GTSTypes.h"
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(TradePersonSelect)


class GTSTradePersonSelectLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  GTSTradePersonSelectLowerViewListener() :
    m_pSelecctTradePartnerInfo( NULL ),
    m_pGTSIndexData( NULL ),
    m_bUpdateView( false )
  {
  }
  ~GTSTradePersonSelectLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_TOUCH_REFINE_SEARCH_BUTTON,
    INPUT_EVENT_ID_TRADE_PERSON_SELECT,
    INPUT_EVENT_ID_SEARCH_POKEMON_PREVIOUS,
    INPUT_EVENT_ID_SEARCH_POKEMON_NEXT
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void TouchRefineSearchButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_REFINE_SEARCH_BUTTON ); }
  void TradePersonSelect( NetApp::GTS::GtsSearchResultInfo* pSelectTradePartnerInfo ){ SetInputEventID( INPUT_EVENT_ID_TRADE_PERSON_SELECT ); m_pSelecctTradePartnerInfo = pSelectTradePartnerInfo; }
  void SearchPokemonPrevious(){ SetInputEventID( INPUT_EVENT_ID_SEARCH_POKEMON_PREVIOUS ); }
  void SearchPokemonNext(){ SetInputEventID( INPUT_EVENT_ID_SEARCH_POKEMON_NEXT ); }

  void UpdateView( NetApp::GTS::GtsIndexData* pGTSIndexData )
  {
    m_bUpdateView = true;
    m_pGTSIndexData = pGTSIndexData;
  }

  bool IsUpdateView() const
  {
    return m_bUpdateView;
  }

  void ResetEvent()
  {
    m_bUpdateView = false;
    ResetInputEvent();
  }

  NetApp::GTS::GtsSearchResultInfo*     GetSelecctTradePartnerInfo() const { return m_pSelecctTradePartnerInfo; }
  NetApp::GTS::GtsIndexData*            GetGTSIndexData() const { return m_pGTSIndexData; }

private:

  NetApp::GTS::GtsSearchResultInfo*     m_pSelecctTradePartnerInfo;
  NetApp::GTS::GtsIndexData*            m_pGTSIndexData;
  bool                                  m_bUpdateView;

};


GFL_NAMESPACE_END(TradePersonSelect)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPERSONSELECTLOWERVIEWLISTENER_H_INCLUDE
