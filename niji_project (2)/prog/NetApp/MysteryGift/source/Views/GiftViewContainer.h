
#if !defined(GIFTVIEWCONTAINER_H_INCLUDED)
#define GIFTVIEWCONTAINER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftViewContainer.h
 * @date   2015/07/31 Fri. 16:22:47
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


#include  "./GiftMessageMenu.h"

#include  "./UpperView_BG.h"
#include  "./UpperView_Card.h"
#include  "./UpperView_Message.h"
#include  "./LowerView_BG.h"
#include  "./LowerView_Album.h"
#include  "./LowerView_Back.h"
#include  "./LowerView_InfoWindow.h"
#include  "./LowerView_Message.h"
#include  "./LowerView_Selection.h"
#include  "./LowerView_SelectData.h"
#include  "./LowerView_SelectInstruction.h"


namespace NetApp  {
namespace Gift    {
  class GiftWork;
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
class GiftViewContainer
{
  GFL_FORBID_COPY_AND_ASSIGN(GiftViewContainer);

public:
  GiftViewContainer(GiftWork* pWork);
  virtual ~GiftViewContainer();


  enum  SceneID
  {
    SCENE_Void,
    SCENE_BGOnly,

    /*  mainmenu  */
    SCENE_MainMenu,

    /*  instruction  */
    SCENE_Instruction_Menu,           /**<  項目選択  */
    SCENE_Instruction_Instruction,    /**<  説明表示  */

    /*  album  */
    SCENE_Album,

    /*  receive  */
    SCENE_Receive_Information,
    SCENE_Receive_Selection,
    SCENE_Receive_SearchingViaUDS,

    /*  receiveIr  */
    SCENE_Receive_Ir,

    /*  receiveInternet  */
    SCENE_Receive_Internet,

    /*  receiveSerial  */
    SCENE_Receive_Serial,
    SCENE_Receive_Serial_Auth,

    /*  selectData  */
    SCENE_SelectData,

    /*  receiveResult  */
    SCENE_ReceiveResult,

    SCENE_
  };
  void  SetScene(const SceneID sceneID);

  void  Update(void);
  void  Draw(const gfl2::gfx::CtrDisplayNo dispNo);
#if PM_DEBUG
  void  DebugDraw(const gfl2::gfx::CtrDisplayNo dispNo);
#endif

  bool  TryFinalize(void);
  bool  IsDrawingAny(void) const;


  void  SetAllInputEnable(const bool isEnable);




  /*
    view
  */
  GiftMessageMenu*              m_pMessageMenu;
  
  UpperView_BG*                 m_pUpperView_BG;
  UpperView_Card*               m_pUpperView_Card;
  UpperView_Message*            m_pUpperView_Message;

  LowerView_BG*                 m_pLowerView_BG;
  LowerView_Album*              m_pLowerView_Album;
  LowerView_Back*               m_pLowerView_Back;
  LowerView_InfoWindow*         m_pLowerView_InfoWindow;
  LowerView_Message*            m_pLowerView_Message;
  LowerView_Selection*          m_pLowerView_Selection;
  LowerView_SelectData*         m_pLowerView_SelectData;
  LowerView_SelectInstruction*  m_pLowerView_SelectInstruction;

private:
  void  ClearViewArray(void);


  app::ui::UIView*      m_pViewArray[4];    /**<  カレント  */



};

} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(GIFTVIEWCONTAINER_H_INCLUDED)  */
