#if !defined(BATTLESPOTVIEWCONTAINER_H_INCLUDED)
#define BATTLESPOTVIEWCONTAINER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotViewContainer.h
 * @date   2015/10/02 Fri. 13:05:47
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

#include  "NetStatic/NetAppLib/include/Util/NetAppEffectBG.h"

#include  "./UpperView_Information.h"
#include  "./LowerView_Selection.h"
#include  "./LowerView_Card.h"


namespace NetApp      {
namespace BattleSpot  {
  class BattleSpotWork;
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
class BattleSpotViewContainer
{
  GFL_FORBID_COPY_AND_ASSIGN(BattleSpotViewContainer);

public:
  BattleSpotViewContainer(BattleSpotWork* pWork);
  virtual ~BattleSpotViewContainer();

  /*  viewの組み合わせ  */
  enum  SceneID
  {
    SCENE_Void,

    /*  welcome  */
    SCENE_Welcome,

    /*  freebattle  */
    SCENE_FreeBattleEntry,

    /*  ratingBattle  */
    SCENE_RatingBattleEntry,

    /*  internetMatch  */
    SCENE_InternetMatchEntry,
    SCENE_InternetMatch_Matching,

    /*  friendMatch  */
    SCENE_FriendMatchEntry,
    SCENE_FriendMatch_Matching,

    SCENE_MAX
  };

  void  SetAllInputEnabled( bool enable );

  void  SetScene(const SceneID sceneID);

  void  Update(void);
  void  Draw(const gfl2::gfx::CtrDisplayNo dispNo);

  bool  IsDrawingAny(void) const;

  /*
    view
  */
  UpperView_Information*          m_pUpperView_Information;
  LowerView_Selection*            m_pLowerView_Selection;
  LowerView_Card*                 m_pLowerView_Card;
  NetAppLib::Util::NetAppEffectBG m_EffectBG;


private:
  void  ClearViewArray(void);

  app::ui::UIView*      m_pViewArray[4];    /**<  カレント  */
};


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTVIEWCONTAINER_H_INCLUDED)  */

