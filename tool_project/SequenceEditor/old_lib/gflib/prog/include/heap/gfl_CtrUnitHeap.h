#ifndef __GFL_CTRUNITHEAP_H__
#define __GFL_CTRUNITHEAP_H__
//======================================================================
/**
 * @file	gfl_CtrUnitHeap.cpp
 * @brief	3DSのユニットヒープ
 * @author	ariizumi
 * @data	10/10/22
 */
//======================================================================
#pragma once

#include <nn.h>
#include <heap/gfl_CtrHeapBase.h>

namespace gfl
{
namespace heap
{

//UnitHeap用の初期化構造体です
//ユニットサイズは実際、管理領域がMEMORY_HEADER_SIZEかアライメントの
//大きいほうのサイズが付加されます。
//
typedef struct
{
  //ユニットのサイズです。このサイズでしか確保できません
  int unitSize;
  //ユニットのアライメントです。
  s32 unitAlign;
}CTR_UNIT_HEAP_INIT;

//#pragma mark [> HeapBase
//拡張ヒープ
typedef nn::fnd::ThreadSafeUnitHeap CtrUnitHeapClass;

class CTRUnitHeap:public CtrHeapBase
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(CTRUnitHeap);
public:
  CTRUnitHeap(void){};
  virtual ~CTRUnitHeap(){};
  virtual void Dump(void);
  virtual void SystemDump(void);
  
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

private:
  virtual void CreateHeap( void *buf , const int size , void *param );
  virtual void DeleteHeap(void);
  virtual void* AllocAlign( const int size , const int align );
  virtual void Free( void* buf );

  CtrUnitHeapClass *m_heapHandle;
  u8 handleBuff[sizeof(CtrUnitHeapClass)];

  CTR_UNIT_HEAP_INIT  m_initWork;
};



} //namespace heap
} //namespace gfl

#endif // __GFL_CTRUNITHEAP_H__
