
#if !defined(BUTTONHELPER_H_INCLUDED)
#define BUTTONHELPER_H_INCLUDED
#pragma once
//==============================================================================
/**
  *  @file   ButtonHelper.h
  *  @brief  
  *  @date   2015.05.20 Wed.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================

struct  ButtonHelperInfo
{
  app::tool::ButtonManager::ButtonId  button_id;

  u32                                 picturePaneIndex;
  u32                                 boundingPaneIndex;
  u32                                 isPartsPicture;           /**<  「部品」ボタンであるか  */

  u32                                 touch_anime_index;        //!< タッチ時のアニメ
  u32                                 release_anime_index;      //!< タッチ決定時のアニメ
  u32                                 cancel_anime_index;       //!< タッチ状態からのキャンセルアニメ
  u32                                 key_select_anime_index;   //!< キー決定時のアニメ ( touch => release の連続アニメ )
  u32                                 active_anime_index;       //!< アクティブアニメ
  u32                                 passive_anime_index;      //!< パッシブアニメ


  /** ctor  */
  ButtonHelperInfo(
    app::tool::ButtonManager::ButtonId  _buttonID,
    u32                                 _picturePaneIndex,
    u32                                 _boundingPaneIndex,
    u32                                 _isPartsPicture   = false,
    u32                                 _AnimID_Touch     = app::tool::ButtonManager::ANIMATION_NULL,
    u32                                 _AnimID_Release   = app::tool::ButtonManager::ANIMATION_NULL,
    u32                                 _AnimID_Cancel    = app::tool::ButtonManager::ANIMATION_NULL,
    u32                                 _AnimID_KeySelect = app::tool::ButtonManager::ANIMATION_NULL,
    u32                                 _AnimID_Active    = app::tool::ButtonManager::ANIMATION_NULL,
    u32                                 _AnimID_Passive   = app::tool::ButtonManager::ANIMATION_NULL
  )
  : button_id(_buttonID)
  , picturePaneIndex(_picturePaneIndex)
  , boundingPaneIndex(_boundingPaneIndex)
  , isPartsPicture(_isPartsPicture)
  , touch_anime_index(_AnimID_Touch)
  , release_anime_index(_AnimID_Release)
  , cancel_anime_index(_AnimID_Cancel)
  , key_select_anime_index(_AnimID_KeySelect)
  , active_anime_index(_AnimID_Active)
  , passive_anime_index(_AnimID_Passive)
  {}


  /*  ButtonInfoExへ適用  */
  void  assignToButtonInfoEx(app::ui::ButtonInfoEx& rInfoEx) const  {
    rInfoEx.button_id               = button_id;
    rInfoEx.picture_pane            = NULL;
    rInfoEx.bound_pane              = NULL;
    rInfoEx.touch_anime_index       = touch_anime_index;
    rInfoEx.release_anime_index     = release_anime_index;
    rInfoEx.cancel_anime_index      = cancel_anime_index;
    rInfoEx.key_select_anime_index  = key_select_anime_index;
    rInfoEx.active_anime_index      = active_anime_index;
    rInfoEx.passive_anime_index     = passive_anime_index;
  }

};


//------------------------------------------------------------------
/**
  *  @func    CreateButton
  *  @brief   ButtonHelperInfoからのボタン生成
  *  @note    複数のLayoutへは対応していない
  */
//------------------------------------------------------------------
void  CreateButton(
  app::util::Heap*                pHeap,
  app::ui::UIResponder*           pResponder,
  gfl2::lyt::LytWk*               pLayout,
  const gfl2::lyt::LytMultiResID& rLayoutMultiResourceID,
  const ButtonHelperInfo*         pHelperInfo,
  const u32                       infoNum
);


//------------------------------------------------------------------
/**
  *  @func    CreateButton
  *  @brief   ButtonHelperInfoからのボタン生成
  *  @note    複数のLayoutへは対応していない
  */
//------------------------------------------------------------------
void  CreateButton(
  app::util::Heap*          pHeap,
  app::ui::UIView*          pUIView,
  const u32                 layoutWorkID,
  const u32                 layoutResourceID,
  const ButtonHelperInfo*   pHelperInfo,
  const u32                 infoNum
);


} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(BUTTONHELPER_H_INCLUDED)  */
