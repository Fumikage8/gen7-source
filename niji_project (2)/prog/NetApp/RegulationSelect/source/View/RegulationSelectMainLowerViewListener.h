// ============================================================================
/*
 * @file RegulationSelectMainLowerViewListener.h
 * @brief レギュレーション選択の下画面の入力イベントを受け取るリスナーです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECTMAINLOWERVIEWLISTENER_H_INCLUDE )
#define REGULATIONSELECTMAINLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"
#include "Battle/Regulation/include/Regulation.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Draw)


class RegulationSelectMainLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  RegulationSelectMainLowerViewListener() :
    m_SelectRegulation( 0 ),
    m_pSelectRegulationDrawInfo( NULL )
  {
  }
  ~RegulationSelectMainLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_SELECT_REGULATION
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void SelectRegulation( u32 index ){ SetInputEventID( INPUT_EVENT_ID_SELECT_REGULATION ); m_SelectRegulation = index; }

  u32 GetSelectRegulation() const { return m_SelectRegulation; }

  NetAppLib::UI::RegulationDrawInfo* GetSelectRegulationDrawInfo() { return m_pSelectRegulationDrawInfo; }
  void SetSelectRegulationDrawInfo( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo ) { m_pSelectRegulationDrawInfo = pRegulationDrawInfo; }

private:

  u32                   m_SelectRegulation;
  NetAppLib::UI::RegulationDrawInfo*   m_pSelectRegulationDrawInfo;

};


GFL_NAMESPACE_END(Draw)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECTMAINLOWERVIEWLISTENER_H_INCLUDE
