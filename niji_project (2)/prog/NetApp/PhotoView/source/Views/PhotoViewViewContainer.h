
#if !defined(PHOTOVIEWVIEWCONTAINER_H_INCLUDED)
#define PHOTOVIEWVIEWCONTAINER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewViewContainer.h
 * @date   2017/02/07 Tue. 15:00:35
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  <AppLib/include/Ui/UIView.h>

#include  <NetStatic/NetAppLib/include/Util/NetAppEffectBG.h>

#include  "./UpperView_Photo.h"
#include  "./LowerView_Info.h"
#include  "./LowerView_BG.h"


namespace NetApp  {
namespace PhotoView    {
  class PhotoViewWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/17 Fri. 16:13:11
  */
//==============================================================================
class PhotoViewViewContainer
{
  GFL_FORBID_COPY_AND_ASSIGN(PhotoViewViewContainer);

public:
  PhotoViewViewContainer(PhotoViewWork* pWork);
  virtual ~PhotoViewViewContainer();


  enum  SceneID
  {
    SCENE_Void,

    SCENE_View,

    SCENE_
  };
  void  SetScene(const SceneID sceneID);

  void  Update(void);
  void  Draw(const gfl2::gfx::CtrDisplayNo dispNo);

  bool  IsDrawingAny(void) const;
  void  SetAllInputEnable(const bool isEnable);

  /*
    view
  */
  UpperView_Photo*    m_pUpperView_Photo;
  LowerView_Info*     m_pLowerView_Info;
  LowerView_BG*       m_pLowerView_BG;

private:
  void  ClearViewArray(void);

  app::ui::UIView*      m_pViewArray[4];    /**<  カレント  */



};

} /*  namespace PhotoView    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(PHOTOVIEWVIEWCONTAINER_H_INCLUDED)  */
