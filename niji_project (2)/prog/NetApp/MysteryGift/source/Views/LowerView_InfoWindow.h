#if !defined(LOWERVIEW_INFOWINDOW_H_INCLUDED)
#define LOWERVIEW_INFOWINDOW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_InfoWindow.h
 * @date   2015/08/20 Thu. 17:25:09
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

#include  "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"


// =============================================================================
/**/
//==============================================================================
namespace app {
namespace tool {
  class InfoWindowBookType;
}
}


namespace NetApp  {
namespace Gift    {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/08/20 Thu. 17:25:39
  */
//==============================================================================
class LowerView_InfoWindow
  : public  NetAppLib::UI::NetAppEmptyView
  , public  app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN(LowerView_InfoWindow);

public:
  //==============================================================================
  /**
    *  @class  IEventHandler
    *  @brief  LowerView_InfoWindowのイベントハンドラI/F
    */
  //==============================================================================
  class IEventHandler
  {
  public:
    virtual ~IEventHandler(){}

    enum  EventCode
    {
      EVENT_Exit,
      
      EVENT_NONE
    };
  
  
    virtual void  LowerView_InfoWindow_OnEvent(const EventCode event) =0;
  };
  void  SetEventHandler(IEventHandler* pHandler) {m_pHandler  = pHandler;}



  /**
    説明項目
  */
  enum  ViewMode
  {
    VIEWMODE_Instruction_Gift,
    VIEWMODE_Instruction_Album,
    VIEWMODE_Instruction_AlbumFull,
    
    VIEWMODE_MAX,
    VIEWMODE_NONE
  };
  void  SetViewMode(const ViewMode mode);


  /**  infoWindowの終了処理、trueが返れば破棄OK */
  bool  ExitQuery(void);



  LowerView_InfoWindow(GiftWork* pWork);
  virtual ~LowerView_InfoWindow();


protected:
  /*
    implement app::tool::InfoWindowBookTypeListener I/F
  */
  virtual void OnClose(void);
  virtual void OnChangePage(u32 /*pageNo*/);
  virtual void OnCommand(u32 /*pageNo*/);


  /*
    members
  */
  GiftWork*                         m_pWork;
  IEventHandler*                    m_pHandler;

  ViewMode                          m_ViewMode;
  app::tool::InfoWindowBookType*    m_pInfoWindow;

  /*  pages  */
  const u32*                        m_pMessageIDs;
  u32                               m_MessagePages;
};





} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LOWERVIEW_INFOWINDOW_H_INCLUDED)  */
