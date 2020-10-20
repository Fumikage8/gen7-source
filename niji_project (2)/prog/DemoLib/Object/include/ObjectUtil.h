//======================================================================
/**
 * @file ObjectUtil.h
 * @date 2015/08/20 22:19:04
 * @author ariizumi_nobuhiko
 * @brief Object関係のユーティリティー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __OBJECT_UTIL_H_INCLUDE__
#define __OBJECT_UTIL_H_INCLUDE__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

//エフェクト管理用ビットフィールド
class EffectIndexManager
{
public:
  //コンストラクタ
  //@param heap ヒープ
  //@param fieldNum 管理領域(fieldNum×32個管理可能)
  EffectIndexManager(gfl2::heap::HeapBase *heap,int filedNum)
  {
    mFiledNum = filedNum;
    mBit = GFL_NEW_ARRAY(heap)u32[filedNum];
    for( u32 i=0;i<mFiledNum;i++ )
    {
      mBit[i] = 0;
    }
  }
  //デストラクタ
  ~EffectIndexManager()
  {
    GFL_DELETE_ARRAY(mBit);
  }

  //空きインデックスの取得
  int GetIndex(void)
  {
    for( u32 i=0;i<mFiledNum;i++ )
    {
      if( mBit[i] != 0xFFFFFFFF )
      {
        for( int j=0;j<FIELD;j++ )
        {
          if( !(mBit[i] & (1<<j)) )
          {
            mBit[i] |= (1<<j);
            return i*FIELD + j;
          }
        }
      }
    }
    GFL_PRINT("Error");
    GFL_ASSERT_STOP_MSG(0,"管理領域不足");
    return -1;
  }

  //インデックスの返却
  void ReturnIndex( int idx )
  {
    const int arrIdx = idx/FIELD;
    const int bitIdx = idx%FIELD;

    GFL_ASSERT_STOP_MSG(mBit[arrIdx] & (1<<bitIdx),"bit立ってない");


    mBit[arrIdx] -= (1<<bitIdx);
  }
  
  //インデックスの個数
  const int GetIndexSize(void){return (mFiledNum*FIELD);}
private:
  //配列の個数と、bitの数
  static const int FIELD = 32;
  u32 mFiledNum;
  u32 *mBit; //64*32 2048個
};

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)

#endif // __OBJECT_UTIL_H_INCLUDE__