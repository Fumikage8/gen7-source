
#if !defined(QRREADERRESOURCE_H_INCLUDED)
#define QRREADERRESOURCE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderResource.h
 * @date   2016/01/14 Thu. 11:51:28
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
#include  <niji_conv_header/message/msg_qrscan.h>

/*  generalView  */
#include  <arc_index/MenuCursor.gaix>
#include  <arc_index/MenuWindow.gaix>
#include  <arc_index/MenuWindow2.gaix>

/*  QRReader  */
#include  <arc_index/QRReader.gaix>
#include  <niji_conv_header/app/qr_scan/QRReader.h>
#include  <niji_conv_header/app/qr_scan/QRReader_anim_list.h>
#include  <niji_conv_header/app/qr_scan/QRReader_pane.h>

/*  fieldresident  */
#include  <arc_index/field_resident.gaix>


/*  savedata  */
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"

namespace NetApp      {
namespace QRReader {
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
  RESOURCE_Message_PlaceName,

  RESOURCE_Layout,                  /**<  layout群                      */
  RESOURCE_LayoutCommonWindow,      /**<  メッセージウインドウlayout群  */
//  RESOURCE_LayoutMenuWindow2,       /**<  共通下画面layout群            */
  RESOURCE_LayoutCommonMenuCursor,  /**<  共通メニューカーソル          */
  RESOURCE_FieldResident,           /**<  フィールド常駐                */

  RESOURCE_MAX
};

static const gfl2::fs::ArcFile::ARCDATID  QRREADER_MESSAGE_GRAC_ID          = GARC_message_qrscan_DAT;
static const gfl2::fs::ArcFile::ARCDATID  QRREADER_MESSAGE_PLACENAME_ARCDAT = GARC_message_place_name_DAT;

extern Savedata::QRReaderSaveData*   GetQRReaderSaveData(void);

} /*  namespace QRReader */
} /*  namespace NetApp      */
#endif  /*  #if !defined(QRREADERRESOURCE_H_INCLUDED)  */
