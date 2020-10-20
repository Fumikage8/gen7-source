#ifndef __GFL_HEAPBASE_LOWER_H__
#define __GFL_HEAPBASE_LOWER_H__
//======================================================================
/**
 * @file	gfl_HeapBaseLower.h
 * @brief	GFLヒープ基底クラス
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#pragma once

#include <gfl_Macros.h>
#include <heap/gfl_HeapManager.h>
#include <heap/gfl_CtrHeapBase.h>

namespace gfl
{
namespace heap
{
  
//#pragma mark [> HeapBaseLower
class GflHeapBaseLower:public HeapBase
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(GflHeapBaseLower);
public:
  GflHeapBaseLower(GflHeapBase *parent);
  virtual ~GflHeapBaseLower(){};

  //----------------------------------------------------------------------------
  /**
   *  @brief  確保したメモリのリサイズ
   *
   *  @param  void* buf リサイズするメモリ  
   *  @param  int       変更後のサイズ
   */
  //-----------------------------------------------------------------------------
  virtual void ResizeMemory( void* buf , const int newSize);

  //----------------------------------------------------------------------------
  /**
   *  @brief  メモリ状況の表示
   *          独自で用意したMemoryHeaderを元にダンプします
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual void Dump(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  メモリ状況の表示
   *          ライブラリなどで用意された関数を使いダンプします
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual void SystemDump(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープ破壊チェック
   *
   *  @param  none
   *  @return  bool true壊れていない false壊れている
   */
  //-----------------------------------------------------------------------------
  virtual bool Check(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  デフォルトアライメント値の取得
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual int GetDefaultAlign(void);
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  HEAPIDの取得
   *
   *  @return  HEAPID
   */
  //-----------------------------------------------------------------------------
  virtual HEAPID GetHeapId(void) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープのサイズを取得します
   *
   *  @return  int サイズ
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalSize(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープの合計空きサイズを取得します
   *
   *  @return  int サイズ
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalFreeSize(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープで確保できる最大のサイズを取得します
   *
   *  @return  int サイズ
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalAllocatableSize(void);

protected:
  //AllocCountの操作(HeapBaseLowerから操作があるのでこっちを使う！)
  virtual int GetAllocCount(void)const;
  virtual void IncAllocCount(void);
  virtual void DecAllocCount(void);


private:

  GflHeapBase *m_parentHeap;

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
  virtual void CreateHeap( void *buf , const int size , void *param );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープの削除
   *          friendでgfl::heap::Managerからのみ呼び出し
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual void DeleteHeap(void);
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープを常に後方確保するハンドルを取得します
   */
  //-----------------------------------------------------------------------------
  virtual HeapBase* GetLowerHandle(){return this;}

  //実際のメモリ確保・開放処理
  virtual void* AllocAlign( const int size , const int align );
  virtual void Free( void* buf );
  //ここまで純粋仮想関数

#if GFL_HEAP_DEBUG
#if GFL_HIO_ENABLE
  virtual void SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum, const int align );
#else
  virtual void SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum );
#endif
#else
  //ユーザーヘッダーセット(デフォルト)
  virtual void SetMemoryHeader_UserData( void *userHeader );
#endif
  
};

} //namespace heap
} //namespace gfl

#endif // __GFL_HEAPBASE_LOWER_H__
