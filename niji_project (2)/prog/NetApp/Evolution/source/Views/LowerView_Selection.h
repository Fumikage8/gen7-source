
#if !defined(LOWERVIEW_SELECTION_H_INCLUDED)
#define LOWERVIEW_SELECTION_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_Selection.h
 * @date   2015/09/01 Tue. 15:24:19
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
#include  "AppLib/include/Tool/app_tool_2ButtonsView.h"



namespace NetApp    {
namespace Evolution {
  class EvolutionWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  */
//==============================================================================
class LowerView_Selection
  : public NetAppLib::UI::NetAppEmptyView
  , public app::tool::TwoButtonsView::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_Selection );
public:

  class IEventHandler
  {
  public:
    enum  EventCode
    {
      EVENT_ForgetWaza,    /**<  技を忘れる   */
      EVENT_KeepWaza,      /**<  技を忘れない */
      
      EVENT_
    };

    virtual ~IEventHandler(){}
    virtual void  LowerView_Selection_OnEvent(const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}


  void  ShowChoice(void);
  void  HideChoice(void);



  LowerView_Selection(EvolutionWork* pWork);
  virtual ~LowerView_Selection();

  /*  implement UIView I/F  */
//  virtual void  Update(void);
//  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
   /*  endimplement UIView I/F  */


#if 0
  enum  ViewMode
  {
    VIEWMODE_TournamentName,                /**<  大会名のみ              */
    VIEWMODE_TournamentNameWithID,          /**<  大会名 + ID情報         */
    VIEWMODE_TournamentNameWith2Line,       /**<  大会名 + 2行情報        */
    VIEWMODE_TournamentNameWithPlayerInfo,  /**<  大会名 + プレイヤー情報 */

    VIEWMODE_
  };
  void  SetViewMode(const ViewMode viewMode);
#endif


protected:
  /*  implement app::tool::TwoButtonsView::IEventHandler  */
  virtual void  TwoButtonsView_OnEvent(const app::tool::TwoButtonsView::IEventHandler::EventCode eventCode);

  void  Event(const IEventHandler::EventCode eventCode);


  /*
    members
  */
  EvolutionWork*                m_pWork;
  app::tool::TwoButtonsView*    m_p2ButtonsView;
  IEventHandler*                m_pHandler;

#if 0
  enum  TextBoxID
  {
    TEXTBOX_Date,           /**<  大会日付            */
    TEXTBOX_LiveName1of2,   /**<  大会名1/2           */
    TEXTBOX_LiveName2of2,   /**<  大会名2/2           */
    TEXTBOX_Description,    /**<  説明文（2行）       */
    
    TEXTBOX_PlayerInfo1of4, /**<  プレイヤー詳細1/4   */
    TEXTBOX_PlayerInfo2of4, /**<  プレイヤー詳細2/4   */
    TEXTBOX_PlayerInfo3of4, /**<  プレイヤー詳細3/4   */
    TEXTBOX_PlayerInfo4of4, /**<  プレイヤー詳細4/4   */
        
    TEXTBOX_MAX
  };
  gfl2::lyt::LytTextBox*    m_TextBoxes[TEXTBOX_MAX];
#endif
};





} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(LOWERVIEW_SELECTION_H_INCLUDED)  */
