#if PM_DEBUG

//======================================================================
/**
 * @file FieldDebugHeapLog.h
 * @date 2015/10/06 21:38:33
 * @author takahashi_tomoya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_DEBUG_HEAP_LOG__
#define __FIELD_DEBUG_HEAP_LOG__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_heap.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 * @class 
 * @brief クラスの説明
 */
class DebugHeapLog
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugHeapLog);


public:

  struct Log
  {
    u32 heapID;
    u32 totalFreeSize;
    u32 totalAllocatableSize;
    const char * msg;
    u32 pushCount;
  };

public:

  /**
   *  @brief 生成
   */
  static DebugHeapLog * Create( gfl2::heap::HeapBase* pHeap, u32 log_max );

  /**
   *  @brief 取得
   */
  static DebugHeapLog * GetInstance();



  ~DebugHeapLog();

  /**
   *  @brief ログの保存
   *  @param  pHeap   ヒープポインタ
   *  @param  cp_msg  ログメッセージ
   */
  void AddLog( gfl2::heap::HeapBase* pHeap, const char * cp_msg );

  /**
   *  @brief  ログの表示
   */
  void ShowLog() const;


  /**
   *  @brief ログのクリア
   */
  void Clear() { m_LogIndex = 0; }


  /**
   *  @brief 階層Push
   */
  void Push();

  /**
   *  @brief 階層Pop
   */
  void Pop();


private:
  DebugHeapLog( u32 log_max, gfl2::heap::HeapBase* pHeap );

  /**
   *  @brief 1つ前のヒープ情報を取得する。
   *  @param  index     このインデックスよりも小さいインデックスのログをあさる。
   *  @param  pushCount 階層
   *  @param  heapID  チェックするヒープID
   *  @param  lastTotalFreeSize  一つ前の指定HeapIDのトータルヒープ残量
   *  @param  lastTotalAllocatableSize  一つ前の指定HeapIDのアロケータブルサイズ
   */
  void GetLastHeapSize( u32 index, u32 pushCount, u32 heapID, u32 * lastTotalFreeSize, u32 * lastTotalAllocatableSize ) const;

private:
 
  Log * m_pLogTable; 
  u32 m_LogIndex;
  u32 m_LogMax;
  u32 m_PushCount;

  static DebugHeapLog* m_pDebugHeapLog;

};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_DEBUG_HEAP_LOG__


#endif