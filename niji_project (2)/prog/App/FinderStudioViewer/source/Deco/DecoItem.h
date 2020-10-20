#if !defined(DECOITEM_H_INCLUDED)
#define DECOITEM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   DecoItem.h
 * @date   2017/02/14 Tue. 19:28:07
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
#include  <Layout/include/gfl2_Layout.h>

#include  <niji_conv_header/app/data/finder_studio/finder_studio_deco_type_def.h>
#include  <AppLib/source/Tool/GeneralView/Module/ResourceLoader.h>
#include  <AppLib/include/Util/app_util_heap.h>

#include  <App/FinderStudioStatic/include/FinderStudioDefine.h>

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
  *  @date   2017/02/14 Tue. 19:28:11
  */
//==============================================================================
class DecoItem
{
  GFL_FORBID_COPY_AND_ASSIGN(DecoItem);
public:
  enum State
  {
    STATE_Initial,
    STATE_Empty,
    STATE_Loading,
    STATE_Idle,
    STATE_Release,

    STATE_
  };

  DecoItem(void);
  virtual ~DecoItem();


  void  Setup(app::util::Heap* pUtilHeap, gfl2::lyt::LytPane* pParentPane);
  void  Request(const App::FinderStudio::Static::StampData& rStampData, const bool isFrameData = false);
  State Update(void);
  void  ReleaseRequest(const bool isFinalize);

  State GetState(void) const        {return m_State;}
  bool  IsStable(void) const        {const State state = GetState();  return ((state == STATE_Initial)  || (state == STATE_Empty));}
  bool  IsEmptyOrLoaded(void) const {const State state = GetState();  return ((state == STATE_Empty)    || (state == STATE_Idle));}

protected:
  State                                 m_State;
  app::util::Heap*                      m_pUtilHeap;
  app::tool::ResourceLoader             m_Loader;

  struct LoadRequestInfo
  {
    App::FinderStudio::Static::StampData  stampData;
    bool                                  isFrameData;
    bool                                  isAvailable;
    
    /*  ctor  */
    LoadRequestInfo(void)
      : stampData()
      , isFrameData(false)
      , isAvailable(false)
    {
    }
  }m_LoadRequestInfo;

  App::FinderStudio::Static::StampData  m_StampData;
  gfl2::lyt::LytPane*                   m_pParentPane;
  gfl2::lyt::LytPicture*                m_pPicture;
  gfl2::lyt::LytResTextureInfo          m_TexInfo;
  bool                                  m_bReleaseRequest;
  bool                                  m_IsFrameData;
};


}  /*  namespace Deco  */
}  /*  namespace Viewer  */
}  /*  namespace FinderStudio  */
}  /*  namespace App  */
#endif  /*  #if !defined(DECOITEM_H_INCLUDED)  */
