//=============================================================================
/**
 * @file    ItemMessage.cpp
 * @brief   道具のメッセージ関連
 * @author  Hiroyuki Nakamura
 * @date    2015.04.24
 */
//=============================================================================
#include "Print/include/ItemMessage.h"
#include "Print/include/PrintSystem.h"


GFL_NAMESPACE_BEGIN(print)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラク（通常版）
 *
 * @param   heap  ヒープ
 * @param   type  読み込みタイプ
 */
//-----------------------------------------------------------------------------
ItemMessage::ItemMessage( gfl2::heap::HeapBase * heap, gfl2::str::MsgData::LoadType type )
{
  m_pMsgData = GFL_NEW(heap) gfl2::str::MsgData( GetMessageArcId(), GARC_message_iteminfo_DAT, heap, type );
}

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ（リソース外部読み込み版）
 *
 * @param   heap  ヒープ
 * @param   buf   読み込んだメッセージリソース
 */
//-----------------------------------------------------------------------------
ItemMessage::ItemMessage( gfl2::heap::HeapBase * heap, void * buf )
{
  m_pMsgData = GFL_NEW(heap) gfl2::str::MsgData( buf, heap );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
ItemMessage::~ItemMessage()
{
  GFL_DELETE m_pMsgData;
}

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
void ItemMessage::GetInfoMessage( u32 item, gfl2::str::StrBuf * buf )
{
  m_pMsgData->GetString( item, *buf );
}

GFL_NAMESPACE_END(print)
