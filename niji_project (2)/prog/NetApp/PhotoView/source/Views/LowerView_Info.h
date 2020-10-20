#if !defined(LOWERVIEW_INFO_H_INCLUDED)
#define LOWERVIEW_INFO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_Info.h
 * @date   2017/02/07 Tue. 15:05:37
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <NetStatic/NetAppLib/include/UI/NetAppEmptyView.h>
#include  <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>

namespace NetApp {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
class PhotoViewWork;

//==============================================================================
/**
  *  @brief  
  *  @date   2017/02/07 Tue. 15:08:49
  */
//==============================================================================
class LowerView_Info
  : public    NetAppLib::UI::NetAppEmptyView
  , protected app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(LowerView_Info);
public:

  /**
  */
  class IEventHandler
  {
  public:

    enum  EventCode
    {
      /*
        common
      */
      EVENT_Exit,
      EVENT_Cancel,
      
      EVENT_
    };

    virtual void  LowerView_Info_OnEvent(LowerView_Info* pSender, const EventCode eventCode) = 0;
    virtual ~IEventHandler(){}
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}


  void  SetData(const gfl2::str::STRCODE* const pName, const App::FinderStudio::Viewer::PhotoParam& rPhotoParam);


  LowerView_Info(PhotoViewWork* pWork);
  virtual ~LowerView_Info();

  /*  implement UIView I/F  */
//  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;
   /*  endimplement UIView I/F  */

protected:
  /*
    UIInputListener
  */
  virtual app::ui::UIInputListener::ListenerResult  OnLayoutPaneEvent( const u32 buttonID );
//  virtual app::ui::UIInputListener::ListenerResult  OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  /*
    end UIInputListener
  */
  void  Event(const IEventHandler::EventCode event);

  enum 
  {
    TEXTBOX_TrainerName,
    TEXTBOX_Wear0,
    TEXTBOX_Wear1,
    TEXTBOX_Wear2,
    TEXTBOX_Wear3,
    TEXTBOX_Wear4,
    TEXTBOX_Wear5,
    TEXTBOX_Wear6,
    TEXTBOX_Wear7,
    TEXTBOX_LocationName,
    
    TEXTBOX_MAX
  };


  PhotoViewWork*          m_pWork;
  IEventHandler*          m_pHandler;

  gfl2::lyt::LytTextBox*  pTextBoxes[TEXTBOX_MAX];
};




}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */
#endif  /*  #if !defined(LOWERVIEW_INFO_H_INCLUDED)  */
