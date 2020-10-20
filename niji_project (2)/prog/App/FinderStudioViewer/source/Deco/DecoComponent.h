#if !defined(DECOCOMPONENT_H_INCLUDED)
#define DECOCOMPONENT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   DecoComponent.h
 * @date   2017/02/15 Wed. 15:49:39
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================

#include  <AppLib/include/Util/app_util_heap.h>
#include  <AppLib/source/Tool/GeneralView/Module/ResourceLoader.h>
#include  "./DecoView.h"


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
  *  @date   2017/02/15 Wed. 15:50:24
  */
//==============================================================================
class DecoComponent
{
  GFL_FORBID_COPY_AND_ASSIGN(DecoComponent);
public:
  DecoComponent(void);
  virtual ~DecoComponent();

  void  Setup(gfl2::heap::HeapBase* pHeapBase, app::util::AppRenderingManager* pRenderingManager);
  bool  InitFunc(void);
  bool  EndFunc(void);
  void  Update(void);
  void  Draw(const gfl2::gfx::CtrDisplayNo displayNo);


  void  Request(const App::FinderStudio::Static::DecorationData& rDecoData) {Request(rDecoData.deco_param, GFL_NELEMS(rDecoData.deco_param));}
  void  Request(const App::FinderStudio::Static::StampData* pStampData, const u32 dataNum);

  bool  IsLoaded(void) const;

protected:
  app::util::Heap                 m_UtilHeap;
  app::util::AppRenderingManager* m_pRenderingManager;
  app::tool::ResourceLoader       m_Loader;
  DecoView*                       m_pDecoView;
  u32                             m_State;
};




}  /*  namespace Deco  */
}  /*  namespace Viewer  */
}  /*  namespace FinderStudio  */
}  /*  namespace App  */
#endif  /*  #if !defined(DECOCOMPONENT_H_INCLUDED)  */
