#if !defined(SURFUI_LOWER_BG_H_INCLUDED)
#define SURFUI_LOWER_BG_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   SurfUI_Lower_BG.h
 * @date   2016/11/25 Fri. 18:54:16
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
//#include  <AppLib/source/Tool/GeneralView/Module/CursorController.h>
//#include  <AppLib/include/Tool/GeneralView/Module/IResourceIDHandler.h>

/*  surf  */
#include  "../../../include/MantainSurfAppData.h"
#include  "./SurfUI_Lower_TrickList.h"

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
class SurfUI_Lower_BG
  : public    app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(SurfUI_Lower_BG);
  /**
    内部イベント取得ハンドラ
  */
public:
  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    *  @param    [in] pHeap               heap
    *  @param    [in] pRenderingManager   renderingManager
    */
  //------------------------------------------------------------------
  SurfUI_Lower_BG(
    app::util::Heap*                pHeap,
    app::util::AppRenderingManager* pRenderingManager
  );

  /**
    dtor
  */
  virtual ~SurfUI_Lower_BG();

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void    Setup(SurfUIResourceProvider* pResourceProvider);
  void    SetMode(const App::MantainSurf::MantainSurfAppData::SkillListMode mode) {m_TrickList.SetMode(mode);}


  /*  tricklist  */
  void  SetTrickListVisibility(const bool isVisible)  {m_TrickList.SetVisible(isVisible);}


  /*
    override UIView I/F
  */
//  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;


protected:
  /*
    funcs
  */

  /*
    members
  */
  app::util::Heap*                            m_pHeap;
  app::util::AppRenderingManager*             m_pRenderingManager;

  SurfUI_Lower_TrickList                      m_TrickList;
};


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(SURFUI_LOWER_BG_H_INCLUDED)  */
