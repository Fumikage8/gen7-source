
#if !defined(BATTLESPOTRESOURCE_H_INCLUDED)
#define BATTLESPOTRESOURCE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotResource.h
 * @date   2015/10/02 Fri. 12:00:59
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <fs/include/gfl2_FsArcFile.h>

/*  heap  */
#include  <System/include/HeapDefine.h>

/*  arc  */
#include  <arc_def_index/arc_def.h>
/*  message  */
#include  <arc_index/message.gaix>
#include  <niji_conv_header/message/msg_battlespot.h>
#include  <niji_conv_header/message/msg_battle_bgm_select.h>
/*  commonWindow  */
#include  <arc_index/MenuWindow.gaix>
#include  <arc_index/MenuCursor.gaix>
/*  menuwindow2  */
#include  <arc_index/MenuWindow2.gaix>
#include  <niji_conv_header/app/common_menu_2/MenuWindow2.h>
#include  <niji_conv_header/app/common_menu_2/MenuWindow2_anim_list.h>
#include  <niji_conv_header/app/common_menu_2/MenuWindow2_pane.h>

/*  BattleSpot  */
#include  <arc_index/BattleSpot.gaix>
#include  <niji_conv_header/app/battlespot/BattleSpot.h>
#include  <niji_conv_header/app/battlespot/BattleSpot_anim_list.h>

namespace BattleSpotPane  {
#include  <niji_conv_header/app/battlespot/BattleSpot_pane.h>
}
// =============================================================================
/**/
//==============================================================================
enum  ResourceID
{
  RESOURCE_Message,                 /**<  メッセージ                    */
  RESOURCE_Message_BGMList,         /**<  メッセージ                    */
  RESOURCE_BG,                      /**<  BG                            */
  RESOURCE_Layout,                  /**<  layout群                      */
  RESOURCE_LayoutCommonWindow,      /**<  メッセージウインドウlayout群  */
  RESOURCE_LayoutMenuWindow2,       /**<  共通下画面layout群            */
  RESOURCE_LayoutCommonMenuCursor,  /**<  共通メニューカーソル          */

  RESOURCE_MAX
};

enum  TrainerIconID
{
  TRAINER_ICON_Player,
  TRAINER_ICON_Opponent_0,
  TRAINER_ICON_Opponent_1,
  TRAINER_ICON_Opponent_2,

  TRAINER_ICON_MAX,
  TRAINER_ICON_MULTI = TRAINER_ICON_MAX
};



static const gfl2::fs::ArcFile::ARCDATID  BATTLESPOT_MESSAGE_ARCDAT           = GARC_message_battlespot_DAT;
static const gfl2::fs::ArcFile::ARCDATID  BATTLESPOT_MESSAGE_BGMLIST_ARCDAT   = GARC_message_battle_bgm_select_DAT;


#endif  /*  #if !defined(BATTLESPOTRESOURCE_H_INCLUDED)  */

