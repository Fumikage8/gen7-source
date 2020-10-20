
#if !defined(SURFUI_LOWER_TRICKLIST_H_INCLUDED)
#define SURFUI_LOWER_TRICKLIST_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   SurfUI_Lower_TrickList.h
 * @date   2017/01/23 Mon. 17:07:05
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

namespace App  {
namespace MantainSurf  {
namespace UI  {
  class SurfUIResourceProvider;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @date   2017/01/23 Mon. 17:07:25
  */
//==============================================================================
class SurfUI_Lower_TrickList
  : public    app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(SurfUI_Lower_TrickList);
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
  SurfUI_Lower_TrickList(
    app::util::Heap*                pHeap,
    app::util::AppRenderingManager* pRenderingManager
  );

  /**
    dtor
  */
  virtual ~SurfUI_Lower_TrickList();

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


  void  SetMode(const App::MantainSurf::MantainSurfAppData::SkillListMode mode);


protected:
  /*
    funcs
  */

  /*
    members
  */
  app::util::Heap*                            m_pHeap;
  app::util::AppRenderingManager*             m_pRenderingManager;
};


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(SURFUI_LOWER_TRICKLIST_H_INCLUDED)  */
