#if PM_DEBUG

//======================================================================
/**
 * @file FieldDebugHeapLog.cpp
 * @date 2015/10/06 21:39:35
 * @author takahashi_tomoya
 * @brief ヒープのログをとるデバッグ機能
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldStatic/include/Debug/FieldDebugHeapLog.h"

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)


// シングルトン
DebugHeapLog* DebugHeapLog::m_pDebugHeapLog = NULL;


/**
 *  @brief 生成
 */
DebugHeapLog * DebugHeapLog::Create( gfl2::heap::HeapBase* pHeap, u32 log_max )
{
  if( m_pDebugHeapLog == NULL )
  {
    m_pDebugHeapLog = GFL_NEW( pHeap ) DebugHeapLog( log_max, pHeap );
  }

  return m_pDebugHeapLog;
}

/**
 *  @brief 取得
 */
DebugHeapLog * DebugHeapLog::GetInstance()
{
  return m_pDebugHeapLog;
}





/**
 *  @brief コンストラクタ
 */
DebugHeapLog::DebugHeapLog( u32 log_max, gfl2::heap::HeapBase* pHeap )
{
  m_LogMax = log_max;
  m_LogIndex = 0;
  m_PushCount =  0;
  m_pLogTable = GFL_NEW_ARRAY(pHeap) Log[log_max];
}

DebugHeapLog::~DebugHeapLog()
{
  GFL_DELETE_ARRAY( m_pLogTable );
  m_LogMax = 0;
  m_LogIndex = 0;
  m_PushCount =  0;
}

/**
 *  @brief ログの保存
 *  @param  pHeap   ヒープポインタ
 *  @param  cp_msg  ログメッセージ
 */
void DebugHeapLog::AddLog( gfl2::heap::HeapBase* pHeap, const char * cp_msg )
{
  if( m_LogIndex < m_LogMax )
  {
    m_pLogTable[m_LogIndex].heapID                = pHeap->GetHeapId();
    m_pLogTable[m_LogIndex].totalFreeSize         = pHeap->GetTotalFreeSize();
    m_pLogTable[m_LogIndex].totalAllocatableSize  = pHeap->GetTotalAllocatableSize();
    m_pLogTable[m_LogIndex].msg                   = cp_msg;
    m_pLogTable[m_LogIndex].pushCount             = m_PushCount;

    ++ m_LogIndex;
  }
  else
  {
    GFL_ASSERT( m_LogIndex < m_LogMax );
  }

}

/**
 *  @brief  ログの表示
 */
void DebugHeapLog::ShowLog() const
{
  for( u32 i=0; i<m_LogIndex; ++i )
  {
    u32 lastTotalFreeSize;
    u32 lastTotalAllocatableSize;
    this->GetLastHeapSize( i, m_pLogTable[i].pushCount, m_pLogTable[i].heapID, &lastTotalFreeSize, &lastTotalAllocatableSize );

    GFL_PRINT( "=====================================================\n" );
    GFL_PRINT( "LOG             %s\n", m_pLogTable[i].msg );
    GFL_PRINT( "HEAP ID         %d\n", m_pLogTable[i].heapID );
    GFL_PRINT( "USE SIZE        0x%x\n", lastTotalFreeSize  - m_pLogTable[i].totalFreeSize );
    GFL_PRINT( "SUB ALLOCATABLE 0x%x\n", lastTotalAllocatableSize  - m_pLogTable[i].totalAllocatableSize );
    GFL_PRINT( "NOW TOTAL FREE  0x%x\n", m_pLogTable[i].totalFreeSize );
    GFL_PRINT( "NOW ALLOCATABLE 0x%x\n", m_pLogTable[i].totalAllocatableSize );
    GFL_PRINT( "LAST TOTAL FREE  0x%x\n", lastTotalFreeSize );
    GFL_PRINT( "LAST ALLOCATABLE 0x%x\n", lastTotalAllocatableSize );
    GFL_PRINT( "=====================================================\n" );
  }
}

/**
 *  @brief 階層Push
 */
void DebugHeapLog::Push()
{
  ++m_PushCount ;
}

/**
 *  @brief 階層Pop
 */
void DebugHeapLog::Pop()
{
  if(m_PushCount > 0)
  {
    ++m_PushCount ;
  }
  else
  {
    GFL_ASSERT( m_PushCount > 0 );
  }
}


/**
 *  @brief 1つ前のヒープ情報を取得する。
 *  @param  index   このインデックスよりも小さいインデックスのログをあさる。
 *  @param  pushCount 階層
 *  @param  heapID  チェックするヒープID
 *  @param  lastTotalFreeSize  一つ前の指定HeapIDのトータルヒープ残量
 *  @param  lastTotalAllocatableSize  一つ前の指定HeapIDのアロケータブルサイズ
 */
void DebugHeapLog::GetLastHeapSize( u32 index, u32 pushCount, u32 heapID, u32 * lastTotalFreeSize, u32 * lastTotalAllocatableSize ) const
{
  if( index > 0 )
  {
    for( s32 i = static_cast<s32>(index)-1; i>=0; --i )
    {
      if( (m_pLogTable[i].heapID == heapID) && (m_pLogTable[i].pushCount == pushCount) )
      {
        *lastTotalFreeSize = m_pLogTable[i].totalFreeSize;
        *lastTotalAllocatableSize = m_pLogTable[i].totalAllocatableSize;
        return ;
      }
    }
  }


  // ファースト情報
  *lastTotalFreeSize = gfl2::heap::Manager::GetHeapByHeapId(heapID)->GetTotalSize();
  *lastTotalAllocatableSize = gfl2::heap::Manager::GetHeapByHeapId(heapID)->GetTotalSize();

}


GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)


#endif