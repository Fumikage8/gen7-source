//==============================================================================
/**
 * @file  gfl_HashList.cpp
 * @brief
 * @author  kimura_shin
 * @data  2011/03/29, 11:50
 */
// =============================================================================

#include <string.h>

// プラットフォーム別の定義が収めてあります
//#include "gfl2_Config.h"
// 便利クラス　現在はアドレス計算のみ
#include "core/include/gfl2_Utility.h"
// ハッシュリストクラス
#include "core/include/gfl2_HashList.h"


//==============================================================================
// VoidHashList 関数実体
//==============================================================================

namespace gfl2 {
namespace core {
VoidHashList::VoidHashList( GET_KEY_FUNC func, const s32 size, gfl2::heap::HeapBase* heap ) : m_pHeap(heap), m_ppTable( 0 ), m_pGetKeyFunc( func ), m_Capacity( 0 ), m_Counter( 0 ) {
  if( size ){
    ResizeTable( size );
  }
}
VoidHashList::~VoidHashList( void )
{
  clear( );
}
void VoidHashList::SetKeyFunction( GET_KEY_FUNC func )
{
  m_pGetKeyFunc = func;
}
void VoidHashList::clear( void )
{
  delete [] m_ppTable;
  m_ppTable = 0;
  m_Capacity = m_Counter = 0;
}
//------------------------------------------------------------------------------
//  SetItem
//------------------------------------------------------------------------------
void VoidHashList::SetItem( const void* cls, const u32 mask, const void** tbl )
{
  // 文字列からハッシュを取得
  u32 ofs = Utility::CalcHash( ( *m_pGetKeyFunc )( cls ) );
  // オフセット加算数を設定　｜１してるのは１以上にするため
  u32 add = ( ofs << 1 ) | 1;
  // for(;;) でも大丈夫だが、安全性を高めるため
  for( u32 i = 0; i <= mask; ++i ){
    ofs = ( ofs + add ) & mask;
    // テーブル[オフセット] が空か？
    if( tbl[ofs] == 0 ){
      // 空ならアイテムをセット
      tbl[ofs] = cls;
      return;
    }
  }
  GFL_PRINT( "This hashtable is broken. Could not add (%s)\n", ( *m_pGetKeyFunc )( cls ) );
}

//------------------------------------------------------------------------------
//  ResizeTable
//------------------------------------------------------------------------------
void VoidHashList::ResizeTable( const s32 size )
{
  // デフォルトアロケーターを使ってメモリ確保
//  const void **new_tbl = new const void* [size];
  const void **new_tbl = m_pHeap?GFL_NEW_ARRAY(m_pHeap) const void* [size]:new const void* [size];
  // ０クリア
  Utility::Memset( new_tbl, 0x00, size * sizeof(void* ) );
  // size は２のべき数のはずなのでマスクはー１すればよい
  u32 mask = size - 1;
  // 元のテーブルアイテムを新しいテーブルに再配置
  for( s32 i = 0; i < m_Capacity; ++i ){
    const void* cls = m_ppTable[i];
    // 元のテーブルアイテムが空でないか？
    if( cls ){
      // 空でないので再配置
      SetItem( cls, mask, new_tbl );
    }
  }
  // 再配置されたので元のテーブルは開放
  //  LibAllocator::Free(m_ppTable);
  delete [] m_ppTable;
  // 新しいテーブルポインタを設定
  m_ppTable = new_tbl;
  // 新しいテーブルサイズを設定
  m_Capacity = size;
}

// -----------------------------------------------------------------
//  Append
// -----------------------------------------------------------------
void VoidHashList::Append( const void* cls )
{
  s32 tmax = m_Capacity;
  // テーブルアイテムの数が増えてきて、用意された数の半数を超えたら広げる
  if( tmax < ( ++m_Counter << 1 ) ){
    // 元のサイズの２倍に広げる
    tmax = ( tmax ) ? ( tmax << 1 ) : 1;
    // 広げたテーブルに再配置する
    ResizeTable( tmax );
  }
  // 新しいアイテムを追加する
  SetItem( cls, tmax - 1, m_ppTable );
}

// -----------------------------------------------------------------
//  Remove
// -----------------------------------------------------------------
void VoidHashList::Remove( const void* cls )
{
  const void **tbl = m_ppTable;
  s32 tmax = m_Capacity;
  u32 ofs = Utility::CalcHash( ( *m_pGetKeyFunc )( cls ) );
  u32 add = ( ofs << 1 ) | 1;
  u32 mask = tmax - 1;

  for( u32 i = 0; i <= mask; i++ ){
    ofs = ( ofs + add ) & mask;
    if( tbl[ofs] == cls ){
      tbl[ofs] = 0;
      break;
    }
  }

  // アイテムが減ってきたのでテーブルを縮小する
  if( ( --m_Counter << 1 ) <= tmax ){
    ResizeTable( tmax >> 1 );
  }
}

// -----------------------------------------------------------------
//  Retrieve
// -----------------------------------------------------------------
void* VoidHashList::Retrieve( const c8 * const name ) const
{
  if( m_ppTable ){
    u32 ofs = Utility::CalcHash( name );
    u32 add = ( ofs << 1 ) | 1;
    u32 mask = m_Capacity - 1;
    for( u32 i = 0; i <= mask; i++ ){
      ofs = ( ofs + add ) & mask;
      const void* tbl = m_ppTable[ofs];
      // ハッシュから計算されたアイテムに名前が一致するか？
      // 一致しない場合は違うオフセット値の場所に格納されているのでループさせる
      if( tbl && 0 == Utility::Strcmp( name, ( *m_pGetKeyFunc )( tbl ) ) ){
        return const_cast < void* >( tbl );
      }
    }
  }
  return 0;
}

const void** VoidHashList::GetTable( void ) {
  return m_ppTable;
}

const void** VoidHashList::GetTable( void ) const {
  return m_ppTable;
}

s32 VoidHashList::GetCapacity( void ) const {
  return m_Capacity;
}

s32 VoidHashList::GetCounter( void ) const {
  return m_Counter;
}

const void** VoidHashList::begin( void ) const {
  return m_ppTable;
}

const void** VoidHashList::end( void ) const {
  return &m_ppTable[m_Capacity];
}

void VoidHashList::SetHeap( gfl2::heap::HeapBase* Heap ) {
  this->m_pHeap = Heap;
}
}
}
