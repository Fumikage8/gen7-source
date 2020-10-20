/**
* @brief  FieldDemoProc
* @author NP Yam
* @data   2015.06.15
*/

#include <System/include/HeapDefine.h>
#include "../include/FieldDemoProc.h"
#include <GameSys/include/GameProcManager.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>

// SceneGraph
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

// garc FileLoading
#include <arc_index/chara_model_field.gaix>
#include <arc_def_index/arc_def.h>

#include <arc_index/demo_seq_script.gaix>

#include "Sound/include/Sound.h"
#include "niji_reference_files/script/FieldPawnTypes.h" //スクリプトと共有するDemoType型


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Demo)

enum
{
  FD_STATE_INIT= 0,
  FD_STATE_RUN_LOADSEQ,
  FD_STATE_RUN_PLAYSEQ,
  FD_STATE_RUN_FINISH,
};

FieldDemoProc::FieldDemoProc( int playSequence ) : 
  mStateReg(FD_STATE_INIT),
  mStateRegSub(0),
  mHeap(NULL),  
  mRenderingPipeline( NULL ),
  mSequence(playSequence),
  mDemoType(0),
  mpDressUpParam(NULL),
  mDressUpParamIsMale(false),
  mHeapSizeIsSet(false),
  mMemHeapSize(0),
  mDevHeapSize(0),
  mDrawEnvNode(NULL)
{  
  
}
  
FieldDemoProc::~FieldDemoProc( void )
{
}

/**
* @brief   PROCの初期化を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
*/
gfl2::proc::Result FieldDemoProc::InitFunc( gfl2::proc::Manager* pManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  GFL_PRINT("FieldDemoProc->Init(%i)\n",mStateReg);
  
  switch(mStateReg)
  {
    case FD_STATE_INIT:     
      StateInit_Heap();     
      StateInit_RenderPipeline();     
      StateInit_SeqPlayer();      
      SetState(FD_STATE_RUN_LOADSEQ);     
      result = gfl2::proc::RES_FINISH;
      break;
  
    default:
      result = gfl2::proc::RES_CONTINUE;
      break;
  }
  return result;
}

/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
gfl2::proc::Result FieldDemoProc::EndFunc(gfl2::proc::Manager* pManager)
{ 
  if(!mSequenceViewSys->Finalize()) return gfl2::proc::RES_CONTINUE;  
 
  GFL_SAFE_DELETE(mSequenceViewSys);
  GFL_SAFE_DELETE( mRenderingPipeline );

  GFL_SAFE_DELETE(mDrawEnvNode);

  GFL_SAFE_DELETE(mGLAllocator);
  
  GFL_DELETE_HEAP(mHeap);
  return gfl2::proc::RES_FINISH;
}


/**
* @brief    StateInit_Heap, initiliaze & setup Heap
* @return Result - None
*/
void FieldDemoProc::StateInit_Heap(void)
{ 
  gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
  mHeap = GFL_CREATE_LOCAL_HEAP_NAME(devHeap,0x300000,gfl2::heap::HEAP_TYPE_EXP,false,"FieldDemoProc");
  GFL_ASSERT( mHeap != NULL );
  
  mGLAllocator = GFL_NEW(devHeap) System::nijiAllocator(devHeap);
}

void FieldDemoProc :: StateInit_SeqPlayer(void)
{
  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
  gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
  
  mSequenceViewSys = GFL_NEW(heap)DemoLib::Sequence::SequenceViewSystem();
  if(mHeapSizeIsSet)
  {
    mSequenceViewSys->Initialize( heap,devHeap, mMemHeapSize, mDevHeapSize );
  }
  else
  {
    mSequenceViewSys->Initialize( heap,devHeap );
  }
}

/**
* @brief    StateInit_RenderPipeline, initialize & setup Render Pipeline
* @return Result - None
*/
void FieldDemoProc::StateInit_RenderPipeline(void)
{   
  mRenderingPipeline = GFL_NEW(mHeap)gfl2::renderingengine::renderer::DefaultRenderingPipeLine(mGLAllocator);
  mDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(mGLAllocator);
  
  gfl2::math::AABB envSpace(gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f));
  mDrawEnvNode->SetAABB(envSpace);

  gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild(mDrawEnvNode);
  mRenderingPipeline->AddDrawEnv(mDrawEnvNode);

}


/**
* @brief    PROCのメイン更新を行う関数
* @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result FieldDemoProc::UpdateFunc( gfl2::proc::Manager* pManager )
{ 
  mSequenceViewSys->Update();

  switch(mStateReg)
  {
    case FD_STATE_RUN_LOADSEQ:
      mSequenceViewSys->SeqComFunc_PlaySequenceFile(mSequence);
      if( mpDressUpParam != NULL ){
        mSequenceViewSys->SetDressUpParam(mpDressUpParam,mDressUpParamIsMale);
      }

      mSequenceViewSys->SetDemoType(mDemoType);
      SetState(FD_STATE_RUN_PLAYSEQ);
      break;

    case FD_STATE_RUN_PLAYSEQ:

      if(mSequenceViewSys->SeqComFunc_IsFinishSequenceFile())
      {
        SetState(FD_STATE_RUN_FINISH);        
      }
      break;

    case FD_STATE_RUN_FINISH:      
      return gfl2::proc::RES_FINISH;      

    default:
      break;

  }

  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief  PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void FieldDemoProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  if( mStateReg==FD_STATE_RUN_PLAYSEQ ) 
  {
    mSequenceViewSys->PreDraw();
    return;
  }
}

/**
* @brief  PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void FieldDemoProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{ 

  if( mStateReg==FD_STATE_RUN_PLAYSEQ ) 
  {
    mSequenceViewSys->Draw(displayNo);
    return;
  }

}

/**
* @brief   PROCのメインを行う関数
*  1/30フレームの②フレーム目の処理です。
*  1/60フレーム動作時には、使われません。
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result FieldDemoProc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  return gfl2::proc::RES_CONTINUE;  
}


  GFL_NAMESPACE_END(Demo)
GFL_NAMESPACE_END(Field)

