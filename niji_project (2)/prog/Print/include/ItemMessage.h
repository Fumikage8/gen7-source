//=============================================================================
/**
 * @file    ItemMessage.h
 * @brief   道具のメッセージ関連
 * @author  Hiroyuki Nakamura
 * @date    2015.04.24
 */
//=============================================================================
#if !defined( ITEM_MESSAGE_H_INCLUDED )
#define ITEM_MESSAGE_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>
// resource
#include "arc_index/message.gaix"


GFL_NAMESPACE_BEGIN(print)

//=============================================================================
/**
 * @class ItemMessage
 * @brief 道具メッセージクラス
 * @date  2015.04.24
 */
//=============================================================================
class ItemMessage
{
  GFL_FORBID_COPY_AND_ASSIGN(ItemMessage);  // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラク（通常版）
   *
   * @param   heap  ヒープ
   * @param   type  読み込みタイプ
   */
  //-----------------------------------------------------------------------------
  ItemMessage( gfl2::heap::HeapBase * heap, gfl2::str::MsgData::LoadType type );

  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ（リソース外部読み込み版）
   *
   * @param   heap  ヒープ
   * @param   buf   読み込んだメッセージリソース
   */
  //-----------------------------------------------------------------------------
  ItemMessage( gfl2::heap::HeapBase * heap, void * buf );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ItemMessage();

  //-----------------------------------------------------------------------------
  /**
   * @func    GetInfoMessage
   * @brief   道具説明取得
   * @date    2015.04.24
   *
   * @param   item  道具ID
   * @param   buf   文字列格納場所
   */
  //-----------------------------------------------------------------------------
  void GetInfoMessage( u32 item, gfl2::str::StrBuf * buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetArcDatID
   * @brief   道具説明のアークデータID取得
   * @date    2015.04.24
   *
   * @return  アークデータID取得
   */
  //-----------------------------------------------------------------------------
  static gfl2::fs::ArcFile::ARCDATID GetArcDatID(void){ return GARC_message_iteminfo_DAT; }


private:
  gfl2::str::MsgData * m_pMsgData;  //!< メッセージデータ
};

GFL_NAMESPACE_END(print)

#endif  // ITEM_MESSAGE_H_INCLUDED