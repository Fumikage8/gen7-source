// ============================================================================
/*
 * @file GTSScrollMenuLowerViewListener.h
 * @brief スクロールするメニューの入力イベントを受け取る為のリスナーです。
 * @date 2015.04.01
 */
// ============================================================================
#if !defined( GTSSCROLLMENULOWERVIEWLISTENER_H_INCLUDE )
#define GTSSCROLLMENULOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <niji_conv_header/poke_lib/monsno_def.h>
#include <pml/include/pml_Const.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ScrollMenu)


class GTSScrollMenuLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  GTSScrollMenuLowerViewListener() :
    m_SelectMonsno( MONSNO_NULL ),
    m_SelectGender( NetApp::GTS::GENDER_UNKNOWN ),
    m_SelectLevelBand( NetApp::GTS::LEVEL_BAND_NONE ),
    m_MessageID( 0xffffffff )
  {
  }
  ~GTSScrollMenuLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_SELECT_POKEMON,
    INPUT_EVENT_ID_SELECT_GENDER,
    INPUT_EVENT_ID_SELECT_LEVEL_BAND,
    INPUT_EVENT_ID_TOUCH_INPUT_BUTTON,
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void SelectPokemon( MonsNo monsno, u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_SELECT_POKEMON ); m_SelectMonsno = monsno; m_MessageID = messageID; }
  void SelectGender( u32 gender, u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_SELECT_GENDER ); m_SelectGender = gender; m_MessageID = messageID; }
  void SelectLevelBand( u32 levelBand, u32 messageID ){ SetInputEventID( INPUT_EVENT_ID_SELECT_LEVEL_BAND ); m_SelectLevelBand = levelBand; m_MessageID = messageID; }
  void TouchInputButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_INPUT_BUTTON ); }

  MonsNo GetSelectMonsNo() const { return m_SelectMonsno; }
  u32 GetSelectGender() const { return m_SelectGender; }
  u32 GetSelectLevelBand() const { return m_SelectLevelBand; }
  u32 GetMessageID() const { return m_MessageID; }

private:

  MonsNo    m_SelectMonsno;
  u32       m_SelectGender;
  u32       m_SelectLevelBand;
  u32       m_MessageID;

};


GFL_NAMESPACE_END(ScrollMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSCROLLMENULOWERVIEWLISTENER_H_INCLUDE
