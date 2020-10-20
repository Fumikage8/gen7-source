#if !defined(GIFTRESOURCE_H_INCLUDED)
#define GIFTRESOURCE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftResource.h
 * @date   2015/07/31 Fri. 14:49:34
 * @author muto_yu
 * @brief  ふしぎなおくりもの リソース関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <fs/include/gfl2_FsArcFile.h>


#include  <arc_def_index/arc_def.h>

#include  "Sound/include/Sound.h"

/*  message  */
#include  <arc_index/message.gaix>
#include  <niji_conv_header/message/msg_mystery.h>
#include  <niji_conv_header/message/msg_mystery_card.h>
/*  commonWindow  */
#include  <arc_index/MenuWindow.gaix>
#include  <arc_index/MenuCursor.gaix>

/*  gift  */
#include  <arc_index/Gift.gaix>
#include  <niji_conv_header/app/gift/Gift.h>
#include  <niji_conv_header/app/gift/Gift_anim_list.h>
#include  <niji_conv_header/app/gift/Gift_pane.h>

/*  menuwindow2  */
#include  <arc_index/MenuWindow2.gaix>

/*  savedata  */
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief   ふしぎなおくりもの ResourceID
  *           NetAppLib::System::ResourceManagerで管理されるリソースを本IDで取り出す
  */
//------------------------------------------------------------------
enum  ResourceID
{
  RESOURCE_Message,                 /**<  メッセージ                    */
  RESOURCE_Message_Card,            /**<  メッセージ（カード用）        */
  RESOURCE_Layout,                  /**<  layout群                      */
  RESOURCE_LayoutCommonWindow,      /**<  メッセージウインドウlayout群  */
  RESOURCE_LayoutCommonMenuCursor,  /**<  共通メニューカーソル          */
  RESOURCE_LayoutMenuWindow2,       /**<  共通下画面layout群            */
  RESOURCE_EmitterSet_1,            /**<  EmitterSet                    */
  RESOURCE_EmitterSet_2,            /**<  EmitterSet                    */

  RESOURCE_MAX
};

static const gfl2::fs::ArcFile::ARCDATID  GIFT_MESSAGE_GRAC_ID      = GARC_message_mystery_DAT;
static const gfl2::fs::ArcFile::ARCDATID  GIFT_MESSAGE_CARD_GRAC_ID = GARC_message_mystery_card_DAT;

static const u32  SEID_DECIDE   = SEQ_SE_DECIDE1;
static const u32  SEID_CANCEL   = SEQ_SE_CANCEL1;

static const u32  MAX_RECEIVE_ATONCE  = 20;   /**<  一度に配信されるデータの最大  */

extern Savedata::MysteryGiftSave*   GetMysteryGiftSave(void);


} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(GIFTRESOURCE_H_INCLUDED)  */
