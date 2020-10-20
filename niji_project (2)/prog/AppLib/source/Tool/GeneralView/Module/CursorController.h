#if !defined(CURSORCONTROLLER_H_INCLUDED)
#define CURSORCONTROLLER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   CursorController.h
 * @date   2015/09/30 Wed. 15:33:24
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

/*  gfl  */
#include  <Layout/include/gfl2_LytSys.h>
#include  <ui/include/gfl2_UI.h>
#include  <gfx/include/gfl2_types.h>

#include  "AppLib/include/Ui/UIInputListener.h"


namespace app   {
  namespace util  {
    class Heap;
    class AppLytAccessor;
    class AppRenderingManager;
  }
}


namespace app   {
namespace tool  {
  class MenuCursor;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/09/30 Wed. 15:33:49
  */
//==============================================================================
/*  app::tool  */
class CursorController
{
  GFL_FORBID_COPY_AND_ASSIGN(CursorController);
public:
  static const u32  LOCATION_MAX  = 8;

  class IEventHandler
  {
  public:
    enum  EventCode
    {
      EVENT_OnMove,      /**<  カーソルが移動した    */
      EVENT_OnDecided,   /**<  カーソルで決定された  */
      
      EVENT_
    };


    virtual ~IEventHandler(){}
    virtual void  CursorController_OnEvent(CursorController* pSender, const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pEventHandler = pHandler;}


  CursorController(app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager);   /**<  ctor  */
  virtual ~CursorController();                                                                   /**<  dtor  */

  virtual void                                      Setup(
                                                      void*                         pResource,
                                                      gfl2::lyt::LytSys*            pLytSys,
                                                      const gfl2::lyt::LytWkSetUp*  pLytWkSetUp,
                                                      app::util::AppLytAccessor*    pAppLytAccessor,
                                                      const bool                    bForUpper = false
                                                    );
  virtual bool                                      Cleanup(void) const;
  virtual void                                      OnUpdate(void);
  virtual void                                      OnDraw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual app::ui::UIInputListener::ListenerResult  OnKeyAction(gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick);


  void  SetLocationNum(const u32 locationNum);
  void  RegisterLocation(const u32 locationIndex, gfl2::lyt::LytWk* pTargetLytWk, gfl2::lyt::LytPane* pTargetPane);

  void  Show(void);
  void  Hide(void);
  bool  IsVisible(void) const;


  //------------------------------------------------------------------
  /**
    *  @brief   カーソル移動
    *  @note    SEは鳴らされない
    */
  //------------------------------------------------------------------
  void  MoveTo(const u32 locationIndex);
  void  UpdatePos(void)                   {MoveTo(GetSelectedIndex());}


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  u32   GetSelectedIndex(void) const  {return m_SelectedIndex;}

protected:
  /*
    キーに対する挙動を変える場合はここで
  */
  virtual app::ui::UIInputListener::ListenerResult  OnUp(   const bool isTrigger = false);
  virtual app::ui::UIInputListener::ListenerResult  OnDown( const bool isTrigger = false);
  virtual app::ui::UIInputListener::ListenerResult  OnLeft( const bool isTrigger = false) {return app::ui::UIInputListener::ENABLE_ACCESS;}
  virtual app::ui::UIInputListener::ListenerResult  OnRight(const bool isTrigger = false) {return app::ui::UIInputListener::ENABLE_ACCESS;}

  void  Event(const IEventHandler::EventCode eventCode);

  /*
    members
  */
  app::util::Heap*                m_pHeap;
  app::util::AppRenderingManager* m_pRenderingManager;
  u32                             m_SelectedIndexMax;
  u32                             m_SelectedIndex;
  struct LocationInfo
  {
    gfl2::lyt::LytWk*       pTargetLytWk;
    gfl2::lyt::LytPane*     pTargetPane;

    LocationInfo(void)
      : pTargetLytWk(NULL), pTargetPane(NULL)
    {}
    bool  IsValid(void) const {return (pTargetLytWk && pTargetPane);}
  }m_LocationInfo[LOCATION_MAX];
  MenuCursor*                     m_pMenuCursor;
  IEventHandler*                  m_pEventHandler;
  bool                            m_bEnable;
  bool                            m_bForUpper;
};



} /*  namespace tool  */
} /*  namespace app   */
#endif  /*  #if !defined(CURSORCONTROLLER_H_INCLUDED)  */
