// ============================================================================
/*
 * @file JoinFestaGlobalAttractionSelectListLowerViewListener.h
 * @brief グローバルアトラクションアプリの下画面の入力イベントを受け取るリスナーです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTIONSELECTLISTLOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTIONSELECTLISTLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaGlobalAttractionSelectListLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  JoinFestaGlobalAttractionSelectListLowerViewListener() :
    m_pActiveGlobalAttractionInfo( NULL ),
    m_pSelectGlobalAttractionInfo( NULL )
  {
  }
  ~JoinFestaGlobalAttractionSelectListLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_SELECT_GLOBAL_ATTRACTION
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void SelectGlobalAttractionInfo( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pSelectGlobalAttractionInfo ){ SetInputEventID( INPUT_EVENT_ID_SELECT_GLOBAL_ATTRACTION ); m_pSelectGlobalAttractionInfo = pSelectGlobalAttractionInfo; }

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* GetSelectGlobalAttractionInfo() { return m_pSelectGlobalAttractionInfo; }

  void SetActiveGlobalAttractionInfo( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo ){ m_pActiveGlobalAttractionInfo = pGlobalAttractionInfo; }
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* GetActiveGlobalAttractionInfo(){ return m_pActiveGlobalAttractionInfo; }

private:

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo*       m_pActiveGlobalAttractionInfo;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo*       m_pSelectGlobalAttractionInfo;


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTIONSELECTLISTLOWERVIEWLISTENER_H_INCLUDE
