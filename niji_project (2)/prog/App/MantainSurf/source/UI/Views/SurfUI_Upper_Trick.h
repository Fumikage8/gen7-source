#if !defined(SURFUI_UPPER_TRICK_H_INCLUDED)
#define SURFUI_UPPER_TRICK_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   SurfUI_Upper_Trick.h
 * @date   2016/11/15 Tue. 11:38:28
 * @author muto_yu
 * @brief  ゲーム中の技表示
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
//#include  <AppLib/source/Tool/GeneralView/Module/CursorController.h>
//#include  <AppLib/include/Tool/GeneralView/Module/IResourceIDHandler.h>

/*  surf  */
#include  "../../../include/MantainSurfAppData.h"
#include  "./Modules/NumberController_TrickScore.h"
#include  "./Modules/SEPlayer.h"
#include  "./SurfUI_Upper_TotalScore.h"

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
  *  @date   2016/11/15 Tue. 11:39:05
  */
//==============================================================================
class SurfUI_Upper_Trick
  : public  app::ui::UIView
  , public  SurfUI_Upper_TotalScore::Notifier
  , public  SEPlayer
{
  GFL_FORBID_COPY_AND_ASSIGN(SurfUI_Upper_Trick);
public:
  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    *  @param    [in] pHeap               heap
    *  @param    [in] pRenderingManager   renderingManager
    */
  //------------------------------------------------------------------
  SurfUI_Upper_Trick(
    app::util::Heap*                pHeap,
    app::util::AppRenderingManager* pRenderingManager
  );

  /**
    dtor
  */
  virtual ~SurfUI_Upper_Trick();

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void    Setup(SurfUIResourceProvider* pResourceProvider);


  void  ShowTrick(const App::MantainSurf::MantainSurfAppData::SkillId trickID, const u32 score);

  /*
    override UIView I/F
  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;


protected:
  enum PaneListID
  {
    NULL_TrickNew,
    NULL_Trick00,
    NULL_Trick01,
    NULL_Trick02,
    NULL_Trick03,
    NULL_Trick04,
    NULL_Trick05,
    NULL_Trick06,
    NULL_Trick07,
    NULL_TrickName00_SETiming,
    NULL_TrickName01_SETiming,
    NULL_TrickName02_SETiming,
    NULL_TrickName03_SETiming,
    NULL_TrickName04_SETiming,
    NULL_TrickName05_SETiming,
    NULL_TrickName06_SETiming,
    NULL_TrickName07_SETiming,
    NULL_TrickName08_SETiming,
    NULL_TrickFree,
    
    PANE_MAX
  };
  static const u32   TrickItemNum  = 9;

  /*
    funcs
  */
  gfl2::lyt::LytPane*   ChildPaneTreeMove(const PaneListID from, const PaneListID to);  /*  子Paneのツリー間移動  */
  void                  UpdateSE(void);

  /*
    members
  */
  app::util::Heap*                            m_pHeap;
  app::util::AppRenderingManager*             m_pRenderingManager;
  u32                                         m_State;
  NumberController_TrickScore                 m_ScoreControllers[TrickItemNum];

  /*  panes  */
//  gfl2::lyt::LytTextBox*                      m_pTextBoxes[TEXTBOX_MAX];
  gfl2::lyt::LytPane*                         m_pPanes[PANE_MAX];

  SurfUI_Upper_TotalScore                     m_TotalScoreView;
};


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(SURFUI_UPPER_TRICK_H_INCLUDED)  */
