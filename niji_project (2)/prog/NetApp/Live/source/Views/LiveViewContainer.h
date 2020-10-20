#if !defined(LIVEVIEWCONTAINER_H_INCLUDED)
#define LIVEVIEWCONTAINER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveViewContainer.h
 * @date   2015/07/17 Fri. 16:12:46
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

#include  "NetStatic/NetAppLib/include/Util/NetAppEffectBG.h"

#include  "./Base/UpperView_Message.h"
#include  "./Base/UpperView_Main.h"
#include  "./Base/LowerView_Selection.h"



namespace NetApp  {
namespace Live    {
  class LiveWork;
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
class LiveViewContainer
{
  GFL_FORBID_COPY_AND_ASSIGN(LiveViewContainer);

public:
  LiveViewContainer(LiveWork* pWork);
  virtual ~LiveViewContainer();


  enum  SceneID
  {
    SCENE_Void,

    SCENE_MainMenu,
    SCENE_ReceivePlayerPass,
    SCENE_Registration,
    SCENE_CheckRemainingMatches,
    SCENE_MatchMenu,
    SCENE_ReplayMatch,
    SCENE_Deregistration,
    SCENE_Matching,
    SCENE_MatchResult,

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
  UpperView_Message*              m_pUpperView_Message;
  UpperView_Main*                 m_pUpperView_Main;
  LowerView_Selection*            m_pLowerView_Selection;
  NetAppLib::Util::NetAppEffectBG m_EffectBG;

private:
  void  ClearViewArray(void);


  app::ui::UIView*      m_pViewArray[4];    /**<  カレント  */



};

} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LIVEVIEWCONTAINER_H_INCLUDED)  */
