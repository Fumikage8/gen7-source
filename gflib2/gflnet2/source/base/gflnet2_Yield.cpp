#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_Yield.cpp
 * @brief	 指定した時間Yieldする関数
 * @author k.ohno
 * @date	 2011.8.12
 */
//=============================================================================

#include <nn.h>



#include "gflnet2/include/base/gflnet2_Yield.h"

#include <thread/include/gfl2_ThreadStatic.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)


//------------------------------------------------------------------------------------
/**
 * @brief コンストラクタ ＆ デストラクタ
 */
//------------------------------------------------------------------------------------

Yield::Yield(void )
{
  mRand.Initialize();
  mNumMax = 0;
  mStart = 0;
}

Yield::~Yield()
{

}



//------------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//------------------------------------------------------------------------------------

void Yield::Initialize( int msec )
{
  mStart = nn::os::Tick::GetSystemCurrent();  //スタートする場所の記憶
  mNumMax = mRand.Next( msec );  //実際のmsecは乱数で決める
}



//------------------------------------------------------------------
/**
 * @brief	  通信状態を続けるためにYieldする関数
 * @retval  true  時間終了
 * @retval  false まだ終わってない
 */
//------------------------------------------------------------------

bool Yield::IsEnd(void)
{
  u64 now = nn::os::Tick::GetSystemCurrent();

  gfl2::thread::ThreadStatic::CurrentYield();
  now = now - mStart;
  if(nn::os::Tick(now).ToTimeSpan().GetMilliSeconds() > mNumMax){
    return true;
  }
  return false;
}




//------------------------------------------------------------------
/**
 * @brief	  時間までYieldする
 */
//------------------------------------------------------------------

void Yield::KeepStop(void)
{
  nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(mNumMax) );
}


GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)

#endif
