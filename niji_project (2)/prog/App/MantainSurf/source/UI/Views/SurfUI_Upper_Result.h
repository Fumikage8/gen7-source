
#if !defined(SURFUI_UPPER_RESULT_H_INCLUDED)
#define SURFUI_UPPER_RESULT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   SurfUI_Upper_Result.h
 * @date   2016/11/14 Mon. 16:32:38
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
#include  <AppLib/include/Tool/app_tool_PaneListView.h>
//#include  <AppLib/source/Tool/GeneralView/Module/CursorController.h>
//#include  <AppLib/include/Tool/GeneralView/Module/IResourceIDHandler.h>

/*  surf  */
#include  "../../../include/MantainSurfAppData.h"
#include  "./Modules/NumberController_ResultItem.h"
#include  "./Modules/NumberController_ResultTotal.h"
#include  "./Modules/SEPlayer.h"

namespace App  {
namespace MantainSurf  {
namespace UI  {
  class SurfUIResourceProvider;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/11/14 Mon. 16:33:49
  */
//==============================================================================
class SurfUI_Upper_Result
  : public    app::ui::UIView
  , public    app::ui::UIInputListener
  , protected app::tool::PaneList::CallBack
  , protected app::tool::PaneListView::Listener
  , public    SEPlayer
{
  GFL_FORBID_COPY_AND_ASSIGN(SurfUI_Upper_Result);
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
    virtual void  SurfUI_Upper_Result_OnEvent(SurfUI_Upper_Result* pSender, const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pEventHandler = pHandler;}

  /**
    リストデータ提供
  */
  class IListDataProvider
  {
  public:
    virtual ~IListDataProvider(){}
    virtual bool  OnRequireListData(const u32 listIndex, App::MantainSurf::MantainSurfAppData::SkillId& rTrickID, u32& rScore) = 0;
  };
  void  SetListDataProvider(IListDataProvider* pListDataProvider) {m_pListDataProvider  = pListDataProvider;}


  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    *  @param    [in] pHeap               heap
    *  @param    [in] pRenderingManager   renderingManager
    */
  //------------------------------------------------------------------
  SurfUI_Upper_Result(
    app::util::Heap*                pHeap,
    app::util::AppRenderingManager* pRenderingManager
  );

  /**
    dtor
  */
  virtual ~SurfUI_Upper_Result();

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void    Setup(SurfUIResourceProvider* pResourceProvider);

  void    SetupList(const u32 itemNum, const bool isShowBottom = false);
//  void    SetupListItem(const u32 index, const u32 trickID, const u32 score);
//  void    AppearItem(const u32 index);
  void    CleanupList(void);


  /*
    override UIView I/F
  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;


  void  SetBestScore(const u32 score);
  void  ShowResult(void);
  void  HideResult(void);

  void  ScrollUp(void);
  void  ScrollDown(void);

  void  SetCourseID(const App::MantainSurf::MantainSurfAppData::CourseID courseID);

protected:
  /*
    funcs
  */
  void  SetTotalValue(const u32 totalValue) { GFL_PRINT("setTotalVallue: %u", totalValue); m_NumberController_ResultTotal.SetTargetValue(totalValue); }
  void  Event(const IEventHandler::EventCode eventCode);
  bool  IsListEmpty(void) const {return (!m_ListItemNum);}
  bool  IsListItemAnimEnd(void) const;
  bool  ShowNextItem(void);
  void  UpdateArrows(void);
  void  ProcResultItemAppearRequest(void);

/*  PaneList  */
  virtual void  CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )  {}
  virtual void  CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )       {}
  virtual void  CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void  CallBack_ItemChange( void * work, u32 pos1, u32 pos2 ) {}
/*  PaneListView  */
  virtual void  PaneListItemSelectListener( u32 pos ) {}
  virtual void  PaneListItemCatchListener( u32 pos )  {}
  /*  implement UIInputListener I/F  */
  virtual app::ui::UIInputListener::ListenerResult  OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  /*  end implement UIInputListener I/F  */

  /*
    members
  */
  app::util::Heap*                            m_pHeap;
  app::util::AppRenderingManager*             m_pRenderingManager;
  u32                                         m_State;

  /*  handler  */
  IEventHandler*                              m_pEventHandler;


  /*
    paneList項目の表示モード
  */
  enum ListItemAppearanceMode
  {
    MODE_Invisible,         /**<  初期状態（非表示）            */
    MODE_AppearRequest,     /**<  表示リクエスト（演出待ち）    */
    MODE_Appear,            /**<  表示開始（Requestを処理）     */
    MODE_ShowImmediately,   /**<  即時表示（通常のPaneList挙動  */
    MODE_
  };
  /*
    paneList項目を遅延表示させるための情報
  */
  struct ResultListInfo
  {
    ListItemAppearanceMode                        mode;           /**<  表示モード                                                          */
    /*  request  */
    u32                                           paneDataIndex;  /**<  requestを受け付けた際、表示対象が割り当てられた先のPaneDataのIndex  */
    u32                                           animID_TrickName;
    u32                                           animID_In;
    NumberController_ResultItem*                  pNumberController;
    App::MantainSurf::MantainSurfAppData::SkillId trickID;
    u32                                           score;
    
    /*  ctor  */
    ResultListInfo(void)
      : mode(MODE_Invisible)
      , paneDataIndex(UINT_MAX)
      , animID_TrickName(UINT_MAX)
      , animID_In(UINT_MAX)
      , pNumberController(NULL)
      , trickID(App::MantainSurf::MantainSurfAppData::SKILL_ID_EX_01)
      , score(0)
    {}
    
    /*  validation  */
    bool  IsValidRequest(void) const
    {
      return (
        (paneDataIndex != UINT_MAX)
        &&  (animID_TrickName != UINT_MAX)
        &&  (animID_In != UINT_MAX)
        &&  (pNumberController)
      );
    }
  };

  static const u32                            PaneDataNum  = 6;
  app::tool::PaneListView*                    m_pPaneListView;
  app::tool::PaneList::PANE_DATA              m_PaneListPaneData[PaneDataNum];
  NumberController_ResultItem                 m_NumberController_PaneList[PaneDataNum];
  ResultListInfo*                             m_pResultListInfo;
  bool                                        m_IsPaneListScrolling;
  IListDataProvider*                          m_pListDataProvider;
  u32                                         m_ListItemNum;
  u32                                         m_LisScrollMax;

  /*  panes  */
  enum
  {
    PANE_ScrollBar,
    PANE_ListItemParent,
    PANE_Arrow_Up,
    PANE_Arrow_Down,
    
    PANE_MAX
  };

//  gfl2::lyt::LytTextBox*                      m_pTextBoxes[TEXTBOX_MAX];
  gfl2::lyt::LytPane*                         m_pPanes[PANE_MAX];

  /*  number controller  */
  NumberController_ResultTotal                m_NumberController_ResultTotal;
  NumberController_ResultTotal                m_NumberController_BestScore;
  bool                                        m_IsNewRecord;
};


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(SURFUI_UPPER_RESULT_H_INCLUDED)  */
