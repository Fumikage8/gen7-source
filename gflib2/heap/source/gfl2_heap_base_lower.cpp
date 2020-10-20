//======================================================================
/**
 * @file	gfl2_HeapBaseLower.cpp
 * @brief	GFLヒープ基底クラス
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#include <string.h>
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "gfl2_heap_local.h"

#include <heap/include/gfl2_heap_base_lower.h>

namespace gfl2
{
namespace heap
{

GflHeapBaseLower::GflHeapBaseLower(HeapBase *parent)
:m_parentHeap(parent)
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  確保したメモリのリサイズ
 *
 *  @param  void* buf リサイズするメモリ  
 *  @param  int       変更後のサイズ
 */
//-----------------------------------------------------------------------------
void GflHeapBaseLower::ResizeMemory( void* buf , const int newSize)
{
  m_parentHeap->ResizeMemory(buf,newSize);
}

//----------------------------------------------------------------------------
/**
 *  @brief  メモリ状況の表示
 *          独自で用意したMemoryHeaderを元にダンプします
 *
 *  @param  none
 */
//-----------------------------------------------------------------------------
void GflHeapBaseLower::Dump(void)
{
  m_parentHeap->Dump();
}


//----------------------------------------------------------------------------
/**
 *  @brief  メモリ状況の表示
 *          ライブラリなどで用意された関数を使いダンプします
 *
 *  @param  none
 */
//-----------------------------------------------------------------------------
void GflHeapBaseLower::SystemDump(void)
{
  m_parentHeap->SystemDump();
}


//----------------------------------------------------------------------------
/**
 *  @brief  ヒープ破壊チェック
 *
 *  @param  none
 *  @return  bool true壊れていない false壊れている
 */
//-----------------------------------------------------------------------------
bool GflHeapBaseLower::Check(void)
{
  return m_parentHeap->Check();
}


//----------------------------------------------------------------------------
/**
 *  @brief  デフォルトアライメント値の取得
 *
 *  @param  none
 */
//-----------------------------------------------------------------------------
int GflHeapBaseLower::GetDefaultAlign(void)
{
  return m_parentHeap->GetDefaultAlign();
}

//----------------------------------------------------------------------------
/**
 *  @brief  HEAPIDの取得
 *
 *  @return  HEAPID
 */
//-----------------------------------------------------------------------------
HEAPID GflHeapBaseLower::GetHeapId(void) const
{
  return m_parentHeap->GetHeapId();
}

//----------------------------------------------------------------------------
/**
 *  @brief  ヒープのサイズを取得します
 *
 *  @return  int サイズ
 */
//-----------------------------------------------------------------------------
int GflHeapBaseLower::GetTotalSize(void)
{
  return m_parentHeap->GetTotalSize();
}


//----------------------------------------------------------------------------
/**
 *  @brief  ヒープの合計空きサイズを取得します
 *
 *  @return  int サイズ
 */
//-----------------------------------------------------------------------------
int GflHeapBaseLower::GetTotalFreeSize(void)
{
  return m_parentHeap->GetTotalFreeSize();
}


//----------------------------------------------------------------------------
/**
 *  @brief  ヒープで確保できる最大のサイズを取得します
 *
 *  @return  int サイズ
 */
//-----------------------------------------------------------------------------
int GflHeapBaseLower::GetTotalAllocatableSize(void)
{
  return m_parentHeap->GetTotalAllocatableSize();
}

//AllocCountの操作(HeapBaseLowerから操作があるのでこっちを使う！)
int GflHeapBaseLower::GetAllocCount(void)const
{
  return m_parentHeap->GetAllocCount();
}
void GflHeapBaseLower::IncAllocCount(void)
{
  m_parentHeap->IncAllocCount();
}
void GflHeapBaseLower::DecAllocCount(void)
{
  m_parentHeap->DecAllocCount();
}

//以下純粋仮想関数
//継承先で実装してください。
//----------------------------------------------------------------------------
/**
 *  @brief  ヒープの作成
 *          friendでgfl::heap::Managerからのみ呼び出し
 *
 *  @param  void *buf 確保用バッファ
 *  @param  const int size バッファサイズ
 *  @param  void* param ユーザーパラメーター
 */
//-----------------------------------------------------------------------------
void GflHeapBaseLower::CreateHeap( void *buf , const int size , void *param )
{
  GFL_UNUSED(buf);
  GFL_UNUSED(size);
  GFL_UNUSED(param);
  GFL_ASSERT_STOP_MSG(0,"GlfHeap HeapBaseLower is can't create!!\n"); //@check システムで特殊な作り方するため。ユーザーの呼び出し禁止
}


//----------------------------------------------------------------------------
/**
 *  @brief  ヒープの削除
 *          friendでgfl::heap::Managerからのみ呼び出し
 *
 *  @param  none
 */
//-----------------------------------------------------------------------------
void GflHeapBaseLower::DeleteHeap(void)
{
  GFL_ASSERT_STOP_MSG(0,"GlfHeap HeapBaseLower is can't delete!!\n"); //@check システムで特殊な作り方するため。ユーザーの呼び出し禁止
}


//実際のメモリ確保・開放処理
void* GflHeapBaseLower::AllocAlign( const int size , const int align )
{
  return m_parentHeap->AllocAlign(size,(align>0?-align:align));
}

#if GFL_HIO_ENABLE
void GflHeapBaseLower::AllocDump( void* buf )
{
  m_parentHeap->AllocDump(buf);
}
#endif // GFL_HIO_ENABLE

void GflHeapBaseLower::Free( void* buf )
{
  m_parentHeap->Free(buf);
}

//ここまで純粋仮想関数

#if GFL_HEAP_DEBUG
void GflHeapBaseLower::SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum )
{
  m_parentHeap->SetMemoryHeader_UserData(userHeader,filename,linenum);
}
#else
//ユーザーヘッダーセット(デフォルト)
void GflHeapBaseLower::SetMemoryHeader_UserData( void *userHeader )
{
  m_parentHeap->SetMemoryHeader_UserData(userHeader);
}
#endif

} //namespace heap
} //namespace gfl2

