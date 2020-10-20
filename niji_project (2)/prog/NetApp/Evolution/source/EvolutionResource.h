

#if !defined(EVOLUTIONRESOURCE_H_INCLUDED)
#define EVOLUTIONRESOURCE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionResource.h
 * @date   2015/08/28 Fri. 16:58:13
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
#include  <niji_conv_header/message/msg_shinka_demo.h>
/*  commonWindow  */
#include  <arc_index/MenuWindow.gaix>
#include  <arc_index/MenuCursor.gaix>
/*  demo  */
#include  <arc_index/demo_seq_script.gaix>

/*  menuwindow2  */
#include  <arc_index/MenuWindow2.gaix>
#include  <niji_conv_header/app/common_menu_2/MenuWindow2.h>
#include  <niji_conv_header/app/common_menu_2/MenuWindow2_anim_list.h>
#include  <niji_conv_header/app/common_menu_2/MenuWindow2_pane.h>

/*  Evolution  */
#include  <arc_index/Evolution.gaix>
#include  <niji_conv_header/app/Evolution/Evolution.h>
#include  <niji_conv_header/app/Evolution/Evolution_anim_list.h>
#include  <niji_conv_header/app/Evolution/Evolution_pane.h>



namespace NetApp    {
namespace Evolution {
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
  RESOURCE_LayoutMenuWindow2,       /**<  共通下画面layout群            */
  RESOURCE_LayoutCommonMenuCursor,  /**<  共通メニューカーソル          */
//  RESOURCE_BG,                      /**<  BG                            */

  RESOURCE_MAX
};


static const gfl2::fs::ArcFile::ARCDATID  EVOLUTION_MESSAGE_GRAC_ID  = GARC_message_shinka_demo_DAT;


/*  demo  */
static const u32  DEMO_PARENT_HEAP_ID    = HEAPID_APP_DEVICE;
static const gfl2::fs::ArcFile::ARCDATID  EVOLUTION_DEMO_GRAC_ID_Evolution  = GARC_demo_seq_script_sd9090_evolution_BSEQ;
static const gfl2::fs::ArcFile::ARCDATID  EVOLUTION_DEMO_GRAC_ID_Evolved    = GARC_demo_seq_script_sd9090_evolution_after_BSEQ;
static const gfl2::fs::ArcFile::ARCDATID  EVOLUTION_DEMO_GRAC_ID_Evolution_Alola  = GARC_demo_seq_script_sd9090_evolution_alola_BSEQ;
static const gfl2::fs::ArcFile::ARCDATID  EVOLUTION_DEMO_GRAC_ID_Evolved_Alola    = GARC_demo_seq_script_sd9090_evolution_after_alola_BSEQ;



} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONRESOURCE_H_INCLUDED)  */
