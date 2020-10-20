#if !defined(SURFUI_UPPER_TOTALSCORE_H_INCLUDED)
#define SURFUI_UPPER_TOTALSCORE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   SurfUI_Upper_TotalScore.h
 * @date   2017/01/23 Mon. 15:06:00
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

/*  surf  */
#include  "../../../include/MantainSurfAppData.h"
#include  "./Modules/NumberController_ResultTotal.h"

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
  *  @date   2017/01/23 Mon. 15:06:36
  */
//==============================================================================
class SurfUI_Upper_TotalScore
  : public    app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(SurfUI_Upper_TotalScore);
public:
  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    *  @param    [in] pHeap               heap
    *  @param    [in] pRenderingManager   renderingManager
    */
  //------------------------------------------------------------------
  SurfUI_Upper_TotalScore(
    app::util::Heap*                pHeap,
    app::util::AppRenderingManager* pRenderingManager
  );

  /**
    dtor
  */
  virtual ~SurfUI_Upper_TotalScore();

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void    Setup(SurfUIResourceProvider* pResourceProvider);

  /*
    override UIView I/F
  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;

  void  AddScore(const u32 score);
  void  ResetScore(void)            {m_NumberController.SetTargetValue(0, true); m_TotalScore = 0;}

  /*  notify  */
  enum NotifyEvent
  {
    NOTIFY_JumpStart,   /**<  ジャンプ開始  */
    NOTIFY_JumpEnd,     /**<  着水          */
    NOTIFY_
  };
  void  Notify(const NotifyEvent notifyEvent);

  class Notifier
  {
  public:
    Notifier(void)
    : m_pTarget(NULL)
    {}
    virtual ~Notifier(){}

    virtual void  Notify(const NotifyEvent notifyEvent) {if(m_pTarget) m_pTarget->Notify(notifyEvent);}
  protected:
    void  SetNotifyTarget(SurfUI_Upper_TotalScore* pTgarget)  {m_pTarget  = pTgarget;}
  
    SurfUI_Upper_TotalScore*  m_pTarget;
  };


protected:
  /*
    funcs
  */

  /*
    members
  */
  app::util::Heap*                  m_pHeap;
  app::util::AppRenderingManager*   m_pRenderingManager;
  u32                               m_State;
  u32                               m_TotalScore;

  /*  number controller  */
  NumberController_ResultTotal      m_NumberController;
};


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(SURFUI_UPPER_TOTALSCORE_H_INCLUDED)  */
