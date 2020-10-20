#if !defined(SURFUI_H_INCLUDED)
#define SURFUI_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   SurfUI.h
 * @date   2016/11/14 Mon. 15:06:59
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <util/include/gfl2_List.h>

/*    */
#include  <Layout/include/gfl2_LytSys.h>
#include  <AppLib/include/Tool/GeneralView/Module/ResourceProvider.h>
#include  <AppLib/include/Ui/UIView.h>

/*  surf  */
#include  "../..//include/MantainSurfAppData.h"

/*  surfUI  */
#include  "./Views/SurfUI_Upper_Result.h"
#include  "./Views/SurfUI_Upper_Trick.h"
#include  "./Views/SurfUI_Upper_Tutorial.h"
#include  "./Views/SurfUI_Lower_Tutorial.h"
#include  "./Views/SurfUI_Lower_BG.h"


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
  *  @date   2016/11/14 Mon. 16:22:19
  */
//==============================================================================
class SurfUI
  : protected SurfUI_Upper_Result::IListDataProvider
  , protected SurfUI_Upper_Result::IEventHandler
  , protected SurfUI_Lower_Tutorial::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(SurfUI);
public:
  /**
    内部イベント取得ハンドラ
  */
  class IEventHandler
  {
  public:
    enum  EventCode
    {
      EVENT_ResultEnd,    /**<  結果表示を終了した  */
      EVENT_TutorialEnd,  /**<  tutorialを終了した  */
      
      EVENT_
    };

    virtual ~IEventHandler(){}
    virtual void  SurfUI_OnEvent(SurfUI* pSender, const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pEventHandler = pHandler;}


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  static u32  GetRequiredHeapBytes(void);

  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    *  @param    [in] pHeap               heap
    *  @param    [in] pRenderingManager   renderingManager
    *  @param    [in] isAutoSetup         リソース読み込みを内部で行い、Setupまでを自動で行うか（ObserveSetup()によって終了を監視する）
    */
  //------------------------------------------------------------------
  SurfUI(
    gfl2::heap::HeapBase*           pHeapBase,
    app::util::AppRenderingManager* pRenderingManager
  );

  /**
    dtor
  */
  virtual ~SurfUI();

  /*
    setup
  */
  void  SetResourceProvider(SurfUIResourceProvider* pResourceProvider) {GFL_ASSERT(!m_pResourceProvider); m_pResourceProvider = pResourceProvider;}
  
  //------------------------------------------------------------------
  /**
    *  @brief   リソース読み込みやSetupなどの処理を進行させる
    */
  //------------------------------------------------------------------
  bool  ObserveSetup(void);


  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual bool  IsDrawing(void) const;

  void  SetBestScore(const u32 score) {m_pUpper_Result->SetBestScore(score);}

  /*  notify  */
  void  OnJumpStart(void);    /**<  jump開始通知：次回のRegisterTrickResultで上画面ジャンプスコアを表示する  */
  void  OnJumpEnd(void);      /**<  jump終了通知：一定時間後に上画面ジャンプスコアを非表示にする             */

  /*  trick  */
  void  ShowTrick(const App::MantainSurf::MantainSurfAppData::SkillId trickID, const u32 score){}             /**<  削除予定                                  */
  void  ShowTrickName(const App::MantainSurf::MantainSurfAppData::SkillId trickID){}                          /**<  削除予定:名前のみの表示、結果は登録しない */
  void  RegisterTrickResult(const App::MantainSurf::MantainSurfAppData::SkillId trickID, const u32 score);    /**<  技とスコアを表示し、結果をResult用に登録  */

  /*  result  */
  void  ShowResult(void);
  void  HideResult(void)  {m_pUpper_Result->HideResult();}

  void  Result_ScrollUp(void)   { m_pUpper_Result->ScrollUp(); }
  void  Result_ScrollDown(void) { m_pUpper_Result->ScrollDown(); }

  /*  tutorial  */
  void  StartTutorial(const MantainSurfAppData::TutorialID tutorialID);

  /*  tricklist  */
  void  ShowTrickList(void)                                         {m_pLower_BG->SetTrickListVisibility(true);}
  void  ShowTrickList(const MantainSurfAppData::SkillListMode mode) {ShowTrickList(); m_pLower_BG->SetMode(mode);}
  void  HideTrickList(void)                                         {m_pLower_BG->SetTrickListVisibility(false);}

  /*  course  */
  void  SetCourseID(const MantainSurfAppData::CourseID courseID)    {m_pUpper_Result->SetCourseID(courseID);}

#if PM_DEBUG
  void  Debug_EnableSE(const bool bEnableSE);
#endif

protected:
  /*
    funcs
  */
  void  CreateHeap(gfl2::heap::HeapBase* pHeapBase);
  void  Event(const IEventHandler::EventCode eventCode);
  void  Setup(void);

  /*  upper_result  */
  virtual bool  OnRequireListData(const u32 listIndex, App::MantainSurf::MantainSurfAppData::SkillId& rTrickID, u32& rScore);
  virtual void  SurfUI_Upper_Result_OnEvent(SurfUI_Upper_Result* pSender, const SurfUI_Upper_Result::IEventHandler::EventCode eventCode);
  /*  tutorial  */
  virtual void  SurfUI_Lower_Tutorial_OnEvent(SurfUI_Lower_Tutorial* pSender, const SurfUI_Lower_Tutorial::IEventHandler::EventCode eventCode);


  struct  sScoreInfo
  {
    App::MantainSurf::MantainSurfAppData::SkillId   trickID;
    u32                                             score;
    
    sScoreInfo(const App::MantainSurf::MantainSurfAppData::SkillId trickID, const u32 _score)
      : trickID(trickID)
      , score(_score)
    {}
    sScoreInfo(void)
      : trickID(static_cast<App::MantainSurf::MantainSurfAppData::SkillId>(0))
      , score(0)
    {}
  };

  app::util::Heap                 m_UtilHeap;
  app::util::AppRenderingManager* m_pRenderingManager;
  SurfUIResourceProvider*         m_pResourceProvider;
  SurfUIResourceProvider*         m_pDefaultResourceProvider;

  IEventHandler*                  m_pEventHandler;

  /*  views */
  SurfUI_Upper_Result*            m_pUpper_Result;
  SurfUI_Upper_Trick*             m_pUpper_Trick;
  SurfUI_Upper_Tutorial*          m_pUpper_Tutorial;
  SurfUI_Lower_Tutorial*          m_pLower_Tutorial;
  SurfUI_Lower_BG*                m_pLower_BG;

  u32                             m_State;

  /*  score */
  gfl2::util::List<sScoreInfo>    m_ScoreInfoList;

};


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(SURFUI_H_INCLUDED)  */
