#if !defined(EGGHATCHINGVIEWCONTAINER_H_INCLUDED)
#define EGGHATCHINGVIEWCONTAINER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EggHatchingViewContainer.h
 * @date   2015/11/24 Tue. 12:44:10
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
#include  "AppLib/include/Ui/UIView.h"
#include  "AppLib/include/Util/app_util_AppDemoUtilityHelper.h"

#include  "./UpperView_Demo.h"
#include  "./LowerView_Selection.h"


namespace NetApp    {
namespace EggHatching {
  class EggHatchingWork;
  class UpperView_Demo;
  class LowerView_Selection;
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
class EggHatchingViewContainer
{
  GFL_FORBID_COPY_AND_ASSIGN(EggHatchingViewContainer);

public:
  EggHatchingViewContainer(EggHatchingWork* pWork);
  virtual ~EggHatchingViewContainer();


  enum  SceneID
  {
    SCENE_Void,

    SCENE_Demo,

    SCENE_
  };
  void  SetScene(const SceneID sceneID);

  void  Update(void);
  void  Draw(const gfl2::gfx::CtrDisplayNo dispNo);

  bool  IsDrawingAny(void) const;
  void  SetupDemoParam(DemoLib::Sequence::SequenceViewSystem* pViewSystem);

  /*
    view
  */
  UpperView_Demo*             m_pUpperView_Demo;
  LowerView_Selection*        m_pLowerView_Selection;

  /*  demo  */
  App::AppDemo::AppDemoUtility*     m_pDemoUtil;


private:
  void  ClearViewArray(void);

  app::ui::UIView*      m_pViewArray[4];    /**<  カレント  */
  /*  demo  */
  enum
  {
    DEMO_POKEMONPARAM_Before  = 0,
    DEMO_POKEMONPARAM_After,
    DEMO_POKEMONPARAM_MAX
  };
  app::util::AppDemoUtilityHelper   m_DemoHelper;
  pml::pokepara::PokemonParam*      m_pDemoPokemonParam[DEMO_POKEMONPARAM_MAX];
};


} /*  namespace EggHatching */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EGGHATCHINGVIEWCONTAINER_H_INCLUDED)  */
