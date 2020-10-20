// ============================================================================
/*
 * @file JoinFestaAttractionSelectLowerViewListener.h
 * @brief アトラクション選択下画面のリスナー
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONSELECTLOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTAATTRACTIONSELECTLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListLowerViewBaseListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionListManager.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaAttractionSelectLowerViewListener : public NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener
{
public:

  JoinFestaAttractionSelectLowerViewListener() :
    m_pSelectAttraction( NULL ),
    m_pActiveAttraction( NULL ),
    m_UpdateUpperView( false )
  {
  }
  ~JoinFestaAttractionSelectLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_SELECT_ATTRACTION = JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_START
  };

  void SetSelectAttraction( NetAppLib::JoinFestaUI::JoinFestaAttractionData* pData ) { SetInputEventID( INPUT_EVENT_ID_SELECT_ATTRACTION ); m_pSelectAttraction = pData; }
  void SetActiveAttraction( NetAppLib::JoinFestaUI::JoinFestaAttractionData* pData ) { m_pActiveAttraction = pData; m_UpdateUpperView = true; }

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* GetSelectAttraction() { return m_pSelectAttraction; }
  NetAppLib::JoinFestaUI::JoinFestaAttractionData* GetActiveAttraction() { return m_pActiveAttraction; }

  bool IsUpdateUpperView() const { return m_UpdateUpperView; }
  
  void ResetInputEvent()
  {
    NetAppLib::System::NetAppViewListenerBase::ResetInputEvent();
    m_UpdateUpperView = false;
  }

private:

  NetAppLib::JoinFestaUI::JoinFestaAttractionData*        m_pSelectAttraction;
  NetAppLib::JoinFestaUI::JoinFestaAttractionData*        m_pActiveAttraction;
  bool                                                    m_UpdateUpperView;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAATTRACTIONSELECTLOWERVIEWLISTENER_H_INCLUDE
