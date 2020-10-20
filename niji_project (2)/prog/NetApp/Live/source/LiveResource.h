#if !defined(LIVERESOURCE_H_INCLUDED)
#define LIVERESOURCE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveResource.h
 * @date   2015/07/09 Thu. 13:18:11
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

#include  <arc_def_index/arc_def.h>
#include  "Sound/include/Sound.h"

/*  message  */
#include  <arc_index/message.gaix>
#include  <niji_conv_header/message/msg_live_tournament.h>
/*  commonWindow  */
#include  <arc_index/MenuWindow.gaix>
#include  <arc_index/MenuCursor.gaix>

/*  live  */
#include  <arc_index/Live.gaix>
#include  <niji_conv_header/app/Live/Live.h>
#include  <niji_conv_header/app/Live/Live_anim_list.h>
#include  <niji_conv_header/app/Live/Live_pane.h>

namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief   ResourceID
  *           NetAppLib::System::ResourceManagerで管理されるリソースを本IDで取り出す
  */
//------------------------------------------------------------------
enum  ResourceID
{
  RESOURCE_Message,                 /**<  メッセージ                    */
  RESOURCE_Layout,                  /**<  layout群                      */
  RESOURCE_LayoutCommonWindow,      /**<  メッセージウインドウlayout群  */
  RESOURCE_LayoutCommonMenuCursor,  /**<  共通メニューカーソル          */
  RESOURCE_BG,                      /**<  BG                            */

  RESOURCE_MAX
};


static const u32  SEID_DECIDE   = SEQ_SE_DECIDE1;
static const u32  SEID_CANCEL   = SEQ_SE_CANCEL1;

static const gfl2::fs::ArcFile::ARCDATID  LIVE_MESSAGE_GRAC_ID  = GARC_message_live_tournament_DAT;


} /*  namespace Live */
} /*  namespace NetApp      */
#endif  /*  #if !defined(LIVERESOURCE_H_INCLUDED)  */
