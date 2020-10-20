//==============================================================================
/**
 * @file    PokeListLocalWork.h
 * @author  fukushima_yuya
 * @date    2015.03.06
 * @brief   ポケモンリスと画面のローカルワーク
 *          sango_project app_pokelist_LocalWork
 */
//==============================================================================


// niji
#include "App/PokeList/source/work/PokeListLocalWork.h"

#include <AppLib/include/Tool/app_tool_MenuWindow.h>
#include <AppLib/include/Tool/app_tool_MenuWindowListener.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <Print/include/SystemFont.h>
#include <Print/include/WordSet.h>
#include <Print/include/WordSetLoader.h>

#include <System/include/PokemonLanguage.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   pHeap     ヒープ
 */
//------------------------------------------------------------------------------
LocalWork::LocalWork( gfl2::heap::HeapBase* pHeap, gfl2::str::MsgData* msgData, print::WordSet* wordSet, WordSetLoader* wordSetLoader )
  : m_pMsgData( msgData )
  , m_pWordSet( wordSet )
  , m_pWordSetLoader( wordSetLoader )
  , m_StoredItemNo( ITEM_DUMMY_DATA )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージデータを取得する
 * @return  メッセージデータ
 */
//------------------------------------------------------------------------------
gfl2::str::MsgData* LocalWork::GetMsgData( void ) const
{
  return m_pMsgData;
}

//------------------------------------------------------------------------------
/**
 * @brief   WORDSETを取得する
 * @return  WORDSET
 */
//------------------------------------------------------------------------------
print::WordSet* LocalWork::GetWordSet( void ) const
{
  return m_pWordSet;
}

//------------------------------------------------------------------------------
/**
 * @brief   預かった道具をセットする
 * @param   item_no  道具No.
 */
//------------------------------------------------------------------------------
void LocalWork::SetStoredItemNo( u16 item_no )
{
  m_StoredItemNo = item_no;
}

//------------------------------------------------------------------------------
/**
 * @brief   預かった道具を取得する
 * @return  預かった道具番号
 */
//------------------------------------------------------------------------------
u16 LocalWork::GetStoredItemNo( void ) const
{
  return m_StoredItemNo;
}



//------------------------------------------------------------------------------
/**
 * @brief   画面モードの変更
 * @param   mode    画面モード
 */
//------------------------------------------------------------------------------
void LocalWork::SetDispMode( DISP_MODE mode )
{
  const char* c_DISP_MODE_NAME[] = {
    "DISP_MODE_NORMAL",
    "DISP_MODE_POKE_EXCHANGE",
    "DISP_MODE_ITEM_EXCHANGE",
    "DISP_MODE_WINDOW",
    "DISP_MODE_FIELD_WAZA",
  };

  FUKUSHIMA_PRINT(
    "DISP MODE : [%s] -> [%s]\n",
    c_DISP_MODE_NAME[m_DispMode],
    c_DISP_MODE_NAME[mode] );

  m_DispMode = mode;
}

//------------------------------------------------------------------------------
/**
 * @brief   画面モードの取得
 * @return  画面モード
 */
//------------------------------------------------------------------------------
DISP_MODE LocalWork::GetDispMode( void ) const
{
  return m_DispMode;
}

GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
