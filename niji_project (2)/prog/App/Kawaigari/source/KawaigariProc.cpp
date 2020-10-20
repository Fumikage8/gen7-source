//======================================================================
/**
 * @file  KawaigariProc.cpp
 * @brief 15/08/31
 * @author  ariizumi
 * @data  かわいがりProc
 */
//======================================================================

#include <App/Kawaigari/include/KawaigariProc.h>

#include "KawaigariMain.h"

#include <niji_conv_header/battle/background/bgsys_normal_ground_def.h>
#include <niji_conv_header/battle/background/bgsys_near_ground_def.h>
#include <niji_conv_header/battle/background/bgsys_far_ground_def.h>

#include "Sound/include/Sound.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariProc::KawaigariProc(KawaigariProcParam *param)
:mInitSeq(0)
,mExitSeq(0)
{
  
  mParam = param;
  param->exitCode = KAWAIGARI_EXITCODE_NORMAL;

  // calling from debug? (Set Default BG)
  if(param->bgParam.groundType == btl::bg::PLATE_NONE)
  {    
    param->bgParam.groundType = btl::bg::PLATE_GRASS;
    param->bgParam.nearType = btl::bg::NEAR_GRASS;
    param->bgParam.farType = btl::bg::FAR_MOUNT;
  }

  gfl2::std::MemCopy(param,&mInitParam,sizeof(KawaigariProcParam));
}

KawaigariProc::~KawaigariProc(void)
{

}

//------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result KawaigariProc::InitFunc(gfl2::proc::Manager* pManager)
{
  switch( mInitSeq )
  {
  case 0:
    mHeap = GFL_CREATE_LOCAL_HEAP_NAME(gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE),0x2500000,gfl2::heap::HEAP_TYPE_EXP,false,"KwMain");
    ///mHeap = GFL_CREATE_LOCAL_HEAP_NAME(gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE),0x1B00000,gfl2::heap::HEAP_TYPE_EXP,false,"KwMain");
    mMainSystem = GFL_NEW(mHeap)KawaigariMain();

#if PM_DEBUG
    if(mInitParam.mode == KAWAIGARI_MODE_DEBUG)
    {
      mMainSystem->SetIsDebugMode(true);
      mInitParam.mode = KAWAIGARI_MODE_NORMAL;
    }
#endif

    mInitSeq++;
    //break;  スルー
  case 1:
    if( mMainSystem->Initialize(mHeap,&mInitParam) )
    {
      Sound::ChangeBGMVolume( 0.4f , 20 );
      return gfl2::proc::RES_FINISH;      
    }
    break;
  }
  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
//------------------------------------------------------------------
gfl2::proc::Result KawaigariProc::EndFunc(gfl2::proc::Manager* pManager)
{
  switch( mExitSeq )
  {
  case 0:
    if( mMainSystem->Finalize() == false )
    {
      return gfl2::proc::RES_CONTINUE;
    }
    mExitSeq++;
    //break;スルー
  case 1:
    GFL_SAFE_DELETE(mMainSystem);
    GFL_DELETE_HEAP(mHeap);
    Sound::ChangeBGMVolume( 1.0f , 20 );
    return gfl2::proc::RES_FINISH;
    //break;
  }

  return gfl2::proc::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
  * @brief   PROCのメインを行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result KawaigariProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  const bool ret = mMainSystem->Update();
  if( ret )
  {
    mParam->exitCode = mInitParam.exitCode; // Copy the exit code
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void KawaigariProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void KawaigariProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  mMainSystem->Draw(displayNo);
}

GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)
