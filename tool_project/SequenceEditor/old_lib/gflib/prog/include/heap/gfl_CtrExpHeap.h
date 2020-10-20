#ifndef __GFL_CTREXPHEAP_H__
#define __GFL_CTREXPHEAP_H__
//======================================================================
/**
 * @file	gfl_CtrExpHeap.h
 * @brief	3DSの拡張ヒープ
 * @author	ariizumi
 * @data	10/10/18
 */
//======================================================================
#pragma once

#include <nn.h>
#include <heap/gfl_CtrHeapBase.h>

namespace gfl
{
namespace heap
{

//#pragma mark [> HeapBase
//拡張ヒープ
typedef nn::fnd::ThreadSafeExpHeap CtrExpHeapClass;

class CTRExpHeap:public CtrHeapBase
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(CTRExpHeap);
public:
  CTRExpHeap(void){};
  virtual ~CTRExpHeap(){};
  virtual void Dump(void);
  virtual void DumpId(u8 id);
  virtual void SystemDump(void);
  virtual bool Check(void);
  
  void DumpBlock( void *buf );

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

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープの先頭アドレスを返します(管理領域を含まない
   *
   *  @return  void* 
   */
  //-----------------------------------------------------------------------------
  virtual void* GetStartAddress(void);

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
   *  @brief  確保された領域のサイズを取得(HeapManagerにもあります)
   *          継承していなければ非対応
   *
   *  @param  const void mem 調べるメモリ
   *  @return int メモリサイズ(-1は失敗)
   */
  //-----------------------------------------------------------------------------
  virtual int GetMemorySize( const void *mem );

  //----------------------------------------------------------------------------
  /**
   *  @brief  渡されたメモリが正当かどうかをチェックします。
   *
   *  @param  const void mem 調べるメモリ
   *  @return bool チェック失敗でfalse
   */
  //-----------------------------------------------------------------------------
  virtual bool CheckMemoryBlock( const void *mem );

  //----------------------------------------------------------------------------
  /**
   *  @brief  すべてのメモリを解放します。
   *
   *  @return bool 失敗したらfalse
   */
  //-----------------------------------------------------------------------------
  virtual bool FreeAll(void);

#if GFL_HIO_ENABLE
  //----------------------------------------------------------------------------
  /**
   *  @brief  HIOでのメモリ状況の監視のON/OFF
   *
   *  @param[in]  flag    true:監視開始 false:監視終了
   */
  //-----------------------------------------------------------------------------
  virtual void HioDump( bool flag );
#endif

private:
  virtual void CreateHeap( void *buf , const int size , void *param );
  virtual void DeleteHeap(void);
  virtual void* AllocAlign( const int size , const int align );
  virtual void Free( void* buf );

  static void BlockVisitorCallback( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
  static void BlockVisitorCallbackID( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
  static void BlockVisitorCallback_Free( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
#if GFL_HIO_ENABLE
  static void BlockVisitorHioCallback( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param );
#endif

  // ダンプ処理ローカル関数
  void dump( bool chk_id = false, u8 id = 0 );

  CtrExpHeapClass *m_heapHandle;
  u8 handleBuff[sizeof(CtrExpHeapClass)];

  //<システムヒープ用アロケータオブジェクトの実態
  //nn::util::aligned_storage<sizeof(CtrExpHeapClass::Allocator), nn::util::alignment_of<CtrExpHeapClass::Allocator>::value >::type allocaterBuffer;
  //CtrExpHeapClass::Allocator *allocater;

};



} //namespace heap
} //namespace gfl

#endif // __GFL_CTREXPHEAP_H__
