#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_BaseProcess.cpp
 * @brief	 プロセスのベースクラス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================

#include <nn.h>
#include <nn/Result.h>
#include <nn/hid.h>
#include <nn/uds.h>



#include "gflnet2/include/base/gflnet2_BaseProcess.h"



GFL_NAMESPACE_BEGIN(gflnet2)



BaseProcess::BaseProcess(void) : gfl2::proc::BaseProcess()
{
  mpThread = NULL;
  mpNetBase=NULL;
  mpNetManager=0;
  mThreadNo=0;
  mChangeNo=0;
}


//------------------------------------------------------------------
/**
 * @brief	  外部からスレッドの終了を呼び出す
 */
//------------------------------------------------------------------

bool BaseProcess::KillThread(void)
{
  if(mpThread){
    mpThread->KillNetThread();
    return true;
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief	  スレッドの削除
 */
//------------------------------------------------------------------

void BaseProcess::delThread(void)
{
  if(mpThread){
    GFL_DELETE mpThread;
    mpThread=NULL;
  }
}



GFL_NAMESPACE_END(gflnet2)
#endif
