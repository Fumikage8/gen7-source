#ifndef __GFL_CTRHEAPBASE_H__
#define __GFL_CTRHEAPBASE_H__
//======================================================================
/**
 * @file	gfl2_CtrHeapBase.h
 * @brief	3DSヒープ基底クラス
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#pragma once
#ifdef GF_PLATFORM_CTR
#include <nn.h>

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_heap_base.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(heap)

//#pragma mark [> CtrHeapBase
class CtrHeapBase:public GflHeapBase
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(CtrHeapBase);
  
public:
  CtrHeapBase(void){};
  virtual ~CtrHeapBase(){};

  //必要であれば実装 必要な方は有泉まで。
  virtual void SetGroupId( const bit8 id ){GFL_UNUSED(id);};
  virtual bit8 GetGroupId(){return 0;};

  virtual int GetDefaultAlign(){return nn::fnd::HeapBase::DEFAULT_ALIGNMENT;};

};
#endif //GF_PLATFORM_CTR

GFL_NAMESPACE_END(heap)
GFL_NAMESPACE_END(gfl2)
#endif // __GFL_CTRHEAPBASE_H__
