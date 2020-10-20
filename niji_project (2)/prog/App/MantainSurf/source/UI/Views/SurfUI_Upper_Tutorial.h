#if !defined(SURFUI_UPPER_TUTORIAL_H_INCLUDED)
#define SURFUI_UPPER_TUTORIAL_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   SurfUI_Upper_Tutorial.h
 * @date   2017/02/06 Mon. 15:44:27
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

namespace App {
namespace MantainSurf {
namespace UI {
  class SurfUIResourceProvider;
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @brief  
  *  @date   2017/02/06 Mon. 15:45:00
  */
//==============================================================================
class SurfUI_Upper_Tutorial
  : public    app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(SurfUI_Upper_Tutorial);
public:
  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    *  @param    [in] pHeap               heap
    *  @param    [in] pRenderingManager   renderingManager
    */
  //------------------------------------------------------------------
  SurfUI_Upper_Tutorial(
    app::util::Heap*                pHeap,
    app::util::AppRenderingManager* pRenderingManager
  );

  /**
    dtor
  */
  virtual ~SurfUI_Upper_Tutorial();

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




}  /*  namespace UI  */
}  /*  namespace MantainSurf  */
}  /*  namespace App  */
#endif  /*  #if !defined(SURFUI_UPPER_TUTORIAL_H_INCLUDED)  */

