//=============================================================================================
/**
 * @file	 GameProcManager.cpp
 * @brief	 ゲームのプロセスマネージャー
 * @author k.ohno
 * @date	 2011.4.19
 */
//=============================================================================================

#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameManager.h"


namespace GameSys{


GameProcManager::GameProcManager(gfl2::heap::HeapBase* pBase, GameManager* pGameManager)
     : gfl2::proc::Manager(pBase),
       m_IsCallNextFrame(false)
{
  mpGameManager = pGameManager;
}



//------------------------------------------------------------------
/**
 * @brief	PROCのイベント検知処理
 * 登録したPROCのEventCheckFunc処理を行います。 適切な位置で呼んでください。
 */
//------------------------------------------------------------------
void GameProcManager::EventCheck(void)
{
  // EventCheckFunc関数をコール。
  if( this->GetMainHandleProcSeq() == gfl2::proc::SEQ_MAIN ){
    GameProc* p_proc = reinterpret_cast<GameProc*>(this->GetMainHandleBaseProcess());
    p_proc->EventCheckFunc( mpGameManager );
  }
}

//------------------------------------------------------------------
/**
 * @brief	PROCの処理
 * 登録したPROCの処理を行います。 適切な位置で呼んでください。
 * @return MainStatus ステータスの動きが帰ってきます
 */
//------------------------------------------------------------------
gfl2::proc::MainStatus GameProcManager::Main(void)
{
  // MainFuncを動作させたフレームのみ、NextFrameMainFuncが実行されるように管理する。
  // InitFuncとEndFuncに関してはは、Mainのタイミングでしか動作しない。

  // MainFuncが動作したかチェック
  m_IsCallNextFrame = false;
  if( this->GetMainHandleProcSeq() == gfl2::proc::SEQ_MAIN ){
    m_IsCallNextFrame = true;
  }

  gfl2::proc::MainStatus status;
  
  // gfl_ProcManager->Main
  status = gfl2::proc::Manager::Main();

  if( (status == gfl2::proc::MAINSTATUS_CHANGE) ||
      (status == gfl2::proc::MAINSTATUS_CALL) ){
    GameProc* p_proc = reinterpret_cast<GameProc*>(this->GetMainHandleBaseProcess());
    if(p_proc){
      mpGameManager->RequestFrameMode( p_proc->GetFrameMode() );  
      mpGameManager->SetDefaultRenderingParameter();  // プロセスが変わった瞬間にリセット
    }
  }

  return status;
}

//------------------------------------------------------------------
/**
 * @brief	PROCの処理
 * 登録したPROCのNextFrameMainFunc処理を行います。 適切な位置で呼んでください。
 * @return MainStatus ステータスの動きが帰ってきます
 */
//------------------------------------------------------------------
gfl2::proc::MainStatus GameProcManager::NextFrameMain(void)
{
  gfl2::proc::Result result;
  
  // 関数をコール。
  if( m_IsCallNextFrame ){
    if( this->GetMainHandleProcSeq() == gfl2::proc::SEQ_MAIN ){
      GameProc* p_proc = reinterpret_cast<GameProc*>(this->GetMainHandleBaseProcess());
	  	result = p_proc->NextFrameMainFunc(this);

      // NextFrameでSEQ_MAINが終わる場合の分岐
      if (result == gfl2::proc::RES_FINISH) {  
  			this->SetMainHandleProcSeq(gfl2::proc::SEQ_END);
  			p_proc->ResetSubSeq();
	  	}
    }
  }

  return gfl2::proc::MAINSTATUS_VALID;
}








} //gamesystem
