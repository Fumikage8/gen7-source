#ifndef __GFL_FND_ALLOCATOR_H__
#define __GFL_FND_ALLOCATOR_H__
#pragma once
//======================================================================
/**
 * @file  gfl_FndAllocator.h
 * @brief nn::fnd::IAllocator::Allocate実装
 * @author  suzuki_hisanao
 * @data  2012/6/18
 */
//======================================================================

#include <nw.h>
#include <nw/os.h>

namespace gfl{
namespace heap{

class FndAllocator: public nn::fnd::IAllocator
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(FndAllocator);
public:

  /**
   * @brief  コンストラクタ
   * @param  heapBase 使用するヒープエリア
   * @return none
   */
  FndAllocator(HeapBase *heapBase);

  /**
   * @brief デストラクタ
   */
  virtual ~FndAllocator();

public:
  /**
   * @brief メモリ確保
   * @param size 確保サイズ
   * @param alignment アライメントサイズ
   */
  virtual void* Allocate( size_t size , s32 alignment );

  /**
   * @brief メモリ解放
   * @param memory 解放対象メモリアドレス
   */
  virtual void Free( void *memory );

public:
  /**
   * @brief 設定されているヒープベースの取得
   * @return 設定ヒープベース
   */
  HeapBase*  GetHeapBase(void);

  /**
   * @brief メモリ確保元をBottomから取得するように設定
   * @param flg topから取得するならfalse,
   */
  void       SetIsAllocLow( bool flg );

private:
  HeapBase *m_heapBase; // ヒープベース
  bool     m_AllocLow;  // メモリBottom確保フラグ
};




} //namespace gfl
} //namespace heap


#endif // __GFL_FND_ALLOCATOR_H__
