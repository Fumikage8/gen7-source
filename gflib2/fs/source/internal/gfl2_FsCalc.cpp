//==============================================================================
/**
 @file    gfl2_FsCalc.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.31
 @brief   ファイルシステムで扱う計算
 */
//==============================================================================


// gfl2のインクルード
#include <thread/include/gfl2_ThreadStatic.h>


// fsのインクルード
#include "gfl2_FsCalc.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


bool Calc::IsPowerOf2(u32 val)
{
  return ( (val!=0) && ((val&(val-1))==0) );
}

u32 Calc::AlignVal(u32 val, u32 align)
{
  // alignはIsPowerOf2がtrueとなるような値であること。

  return ( (val+align-1)&(~(align-1)) );  // @note GflHeapAllocMemoryAlignのアラインメント計算式を流用してもいいかも。
}

void Calc::SleepInLoop(void)
{
  gfl2::thread::ThreadStatic::CurrentSleep(1);
  //gfl2::thread::ThreadStatic::CurrentSleepMicro(1);  // micro sec 1000なら動く、100以下だと動かない。だからコメントアウト。
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)
