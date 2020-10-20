#if !defined(DIVEUI_LOWER_BG_H_INCLUDED)
#define DIVEUI_LOWER_BG_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   DiveUI_Lower_BG.h
 * @date   2016/11/29 Tue. 18:31:32
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
#include  <AppLib/include/Tool/GeneralView/Module/IResourceProvider.h>


#if 0
namespace App  {
namespace LunaSolDive  {
namespace UI  {
#else
namespace Test  {
namespace MutoTest  {
#endif
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  *  @date   2016/11/29 Tue. 18:32:11
  */
//==============================================================================
class DiveUI_Lower_BG
  : public    app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(DiveUI_Lower_BG);
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
  DiveUI_Lower_BG(
    app::util::Heap*                pHeap,
    app::util::AppRenderingManager* pRenderingManager
  );

  /**
    dtor
  */
  virtual ~DiveUI_Lower_BG();

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void    Setup(app::tool::IResourceProvider* pResourceProvider);


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


#if 0
} /*  namespace UI  */
} /*  namespace LunaSolDive  */
} /*  namespace App  */
#else
} /*  namespace Test  */
} /*  namespace MutoTest  */
#endif
#endif  /*  #if !defined(DIVEUI_LOWER_BG_H_INCLUDED)  */
