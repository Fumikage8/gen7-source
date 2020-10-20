
#if !defined(LOWERVIEW_SELECTION_H_INCLUDED)
#define LOWERVIEW_SELECTION_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_Selection.h
 * @date   2015/11/24 Tue. 12:57:43
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include  "AppLib/include/Tool/GeneralView/GeneralLowerView.h"
#include  "AppLib/include/Tool/GeneralView/Module/IResourceProvider.h"



namespace NetApp    {
namespace EggHatching {
  class EggHatchingWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  */
//==============================================================================
class LowerView_Selection
  : public    NetAppLib::UI::NetAppEmptyView
  , protected app::tool::GeneralLowerView::IEventHandler
  , protected app::tool::IResourceProvider
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_Selection );
public:

  class IEventHandler
  {
  public:
    enum  EventCode
    {
      EVENT_Nickname_Yes, /**<  ニックネームつける   */
      EVENT_Nickname_No,  /**<  ニックネームつけない */
      EVENT_AddTo_Party,  /**<  手持ちに加える       */
      EVENT_AddTo_Box,    /**<  Boxに送る            */
      
      EVENT_
    };

    virtual ~IEventHandler(){}
    virtual void  LowerView_Selection_OnEvent(const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}


  void  ShowChoice(void);
  void  HideChoice(void);


  enum  ViewMode
  {
    VIEWMODE_None,
    
    VIEWMODE_Nickname,    /**<  ニックネーム  */
    VIEWMODE_AddTo,       /**<  手持ち/BOX    */
    
    VIEWMODE_
  };
  void  SetViewMode(const ViewMode viewMode);



  LowerView_Selection(EggHatchingWork* pWork);
  virtual ~LowerView_Selection();

  /*  implement UIView I/F  */
//  virtual void  Update(void);
//  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
   /*  endimplement UIView I/F  */


protected:
  /*  implement app::tool::IResourceProvider  */
  virtual void*  IResourceProvider_OnResourceRequest(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID);

  /*  implement app::tool::GeneralLowerView::IEventHandler  */
  virtual void  GeneralLowerView_OnEvent(app::tool::GeneralLowerView* pSender, const app::tool::GeneralLowerView::IEventHandler::EventCode eventCode);


  void  Event(const IEventHandler::EventCode eventCode);


  /*
    members
  */
  EggHatchingWork*              m_pWork;
  app::tool::GeneralLowerView*  m_pGeneralLowerView;
  IEventHandler*                m_pHandler;
  ViewMode                      m_ViewMode;
};



} /*  namespace EggHatching */
} /*  namespace NetApp    */
#endif  /*  #if !defined(LOWERVIEW_SELECTION_H_INCLUDED)  */
