#if !defined(SURFUI_LOWER_TUTORIAL_H_INCLUDED)
#define SURFUI_LOWER_TUTORIAL_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   SurfUI_Lower_Tutorial.h
 * @date   2017/01/19 Thu. 11:14:48
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>

/*    */
#include  <Layout/include/gfl2_LytSys.h>
#include  <AppLib/include/Ui/UIView.h>
#include  <AppLib/include/Tool/GeneralView/Module/IResourceProvider.h>
#include  <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>

/*  surf  */
#include  "../../../include/MantainSurfAppData.h"

namespace app {
  namespace tool {
    class InfoWindowBookType;
  }
}

namespace App  {
namespace MantainSurf  {
namespace UI  {
  class SurfUIResourceProvider;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @brief  
  *  @date   2017/01/19 Thu. 11:15:22
  */
//==============================================================================
class SurfUI_Lower_Tutorial
  : public    app::ui::UIView
//  , public    app::ui::UIInputListener
  , app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN(SurfUI_Lower_Tutorial);
public:
  /**
    内部イベント取得ハンドラ
  */
  class IEventHandler
  {
  public:
    enum  EventCode
    {
      EVENT_End,
      
      EVENT_
    };

    virtual ~IEventHandler(){}
    virtual void  SurfUI_Lower_Tutorial_OnEvent(SurfUI_Lower_Tutorial* pSender, const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pEventHandler = pHandler;}


  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    *  @param    [in] pHeap               heap
    *  @param    [in] pRenderingManager   renderingManager
    */
  //------------------------------------------------------------------
  SurfUI_Lower_Tutorial(
    app::util::Heap*                pHeap,
    app::util::AppRenderingManager* pRenderingManager
  );

  /**
    dtor
  */
  virtual ~SurfUI_Lower_Tutorial();

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void    Setup(SurfUIResourceProvider* pResourceProvider);

  /*
    override UIView I/F
  */
//  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;

  //------------------------------------------------------------------
  /**
    *  @brief    infoWindowを終了させるためのメソッド
    */
  //------------------------------------------------------------------
  bool  Finalize(void);


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  Start(const MantainSurfAppData::TutorialID tutorialID);


protected:
  /*
    funcs
  */
  void  Event(const IEventHandler::EventCode eventCode);

  /*  implement UIInputListener I/F  */
//  virtual app::ui::UIInputListener::ListenerResult  OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  /*  end implement UIInputListener I/F  */

  /*
    implement app::tool::InfoWindowBookTypeListener I/F
  */
  virtual void OnClose(void);
  virtual void OnChangePage(u32 /*pageNo*/);
  virtual void OnCommand(u32 /*pageNo*/);

  /*
    members
  */
  app::util::Heap*                  m_pHeap;
  app::util::AppRenderingManager*   m_pRenderingManager;
  u32                               m_State;
  SurfUIResourceProvider*           m_pResourceProvider;
  /*  handler  */
  IEventHandler*                    m_pEventHandler;

  /*  info  */
  app::tool::InfoWindowBookType*    m_pInfoWindow;
  MantainSurfAppData::TutorialID    m_TutorialID;
  u32                               m_MessagePages;
};


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(SURFUI_LOWER_TUTORIAL_H_INCLUDED)  */
