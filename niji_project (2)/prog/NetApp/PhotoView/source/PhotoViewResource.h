
#if !defined(PHOTOVIEWRESOURCE_H_INCLUDED)
#define PHOTOVIEWRESOURCE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewResource.h
 * @date   2017/02/07 Tue. 12:31:50
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
#include  <niji_conv_header/message/msg_jf_playerlist.h>
#include  <niji_conv_header/message/msg_fs_photo.h>
/*  commonWindow  */
#include  <arc_index/MenuWindow.gaix>
#include  <arc_index/MenuCursor.gaix>

/*  live  */
#include  <arc_index/join_festa_playerlist.gaix>
#include  <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include  <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include  <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>


namespace NetApp {
namespace PhotoView {
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
  RESOURCE_PhotoLocation,           /**<  撮影背景名                    */
  RESOURCE_Layout,                  /**<  layout群                      */
  RESOURCE_LayoutCommonWindow,      /**<  メッセージウインドウlayout群  */
  RESOURCE_LayoutCommonMenuCursor,  /**<  共通メニューカーソル          */

  RESOURCE_MAX
};

static const u32  SEID_DECIDE = SEQ_SE_DECIDE1;
static const u32  SEID_CANCEL = SEQ_SE_CANCEL1;

static const gfl2::fs::ArcFile::ARCDATID  MESSAGE_GRAC_ID   = GARC_message_jf_playerlist_DAT;
static const gfl2::fs::ArcFile::ARCDATID  LOCATION_GRAC_ID  = GARC_message_fs_photo_DAT;




}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */
#endif  /*  #if !defined(PHOTOVIEWRESOURCE_H_INCLUDED)  */

