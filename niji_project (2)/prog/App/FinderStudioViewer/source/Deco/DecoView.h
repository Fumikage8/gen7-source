#if !defined(DECOVIEW_H_INCLUDED)
#define DECOVIEW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   DecoView.h
 * @date   2017/02/15 Wed. 12:53:31
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
#include  <AppLib/include/Ui/UIView.h>

#include  <App/FinderStudioStatic/include/FinderStudioDefine.h>

#include  "./DecoItem.h"


namespace App {
namespace FinderStudio {
namespace Viewer {
namespace Deco {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  *  @date   2017/02/15 Wed. 12:53:37
  */
//==============================================================================
class DecoView
  : public  app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(DecoView);
public:
  DecoView(
    app::util::Heap*                pUtilHeap,
    app::util::AppRenderingManager* pRenderingManager,
    void*                           pResource   /*  ARCID_DECOVIEW, GARC_FinderStudioDecoView_FinderStudioDecoView_applyt_COMP  */
  );
  virtual ~DecoView();

  /*
    override UIView I/F
  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual bool  IsDrawing(void) const;

  void  UpdateSystems(void);      /**<  view以外の部分の更新  */

  void  RequestFrame(const App::FinderStudio::Static::StampData* pStampData);
  void  RequestStamp(const App::FinderStudio::Static::StampData* pStampData, const u32 dataNum);
  void  ReleaseRequest(const bool isFinalize);

  bool  IsItemsLoaded(void) const;
  bool  IsStable(void) const;

  static u32  GetArcID(void);             /**<  ARCID_DECOVIEW                                              */
  static u32  GetArcDatID(void);          /**<  GARC_FinderStudioDecoView_FinderStudioDecoView_applyt_COMP  */
  static bool IsResourceCompressed(void); /**<  true                                                        */

protected:

  app::util::Heap*                  m_pUtilHeap;
  app::util::AppRenderingManager*   m_pRenderingManager;
  gfl2::lyt::LytPane*               m_pDecoParentPane_Frame;
  gfl2::lyt::LytPane*               m_pDecoParentPane_Stamps;

  /*  deco  */
  static const u32  stamps  = App::FinderStudio::Static::DECO_ITEM_MAX;
  DecoItem    m_DecoItem_Frame;
  DecoItem    m_DecoItem_Stamps[stamps];
};


}  /*  namespace Deco  */
}  /*  namespace Viewer  */
}  /*  namespace FinderStudio  */
}  /*  namespace App  */
#endif  /*  #if !defined(DECOVIEW_H_INCLUDED)  */
