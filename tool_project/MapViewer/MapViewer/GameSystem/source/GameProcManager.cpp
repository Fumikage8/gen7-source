//=============================================================================================
/**
 * @file	 GameProcManager.cpp
 * @brief	 �Q�[���̃v���Z�X�}�l�[�W���[
 * @author k.ohno
 * @date	 2011.4.19
 */
//=============================================================================================

#include "GameSystem/include/GameProcManager.h"
#include "GameSystem/include/GameManager.h"


namespace GameSystem{


GameProcManager::GameProcManager(gfl2::heap::HeapBase* pBase, GameManager* pGameManager)
     : gfl2::proc::Manager(pBase),
       m_IsCallNextFrame(false)
{
  mpGameManager = pGameManager;
}



//------------------------------------------------------------------
/**
 * @brief	PROC�̃C�x���g���m����
 * �o�^����PROC��EventCheckFunc�������s���܂��B �K�؂Ȉʒu�ŌĂ�ł��������B
 */
//------------------------------------------------------------------
void GameProcManager::EventCheck(void)
{
  // EventCheckFunc�֐����R�[���B
  if( this->GetMainHandleProcSeq() == gfl2::proc::SEQ_MAIN ){
    GameProc* p_proc = reinterpret_cast<GameProc*>(this->GetMainHandleBaseProcess());
    p_proc->EventCheckFunc( mpGameManager );
  }
}

//------------------------------------------------------------------
/**
 * @brief	PROC�̏���
 * �o�^����PROC�̏������s���܂��B �K�؂Ȉʒu�ŌĂ�ł��������B
 * @return MainStatus �X�e�[�^�X�̓������A���Ă��܂�
 */
//------------------------------------------------------------------
gfl2::proc::MainStatus GameProcManager::Main(void)
{
  // MainFunc�𓮍삳�����t���[���̂݁ANextFrameMainFunc�����s�����悤�ɊǗ�����B
  // InitFunc��EndFunc�Ɋւ��Ă͂́AMain�̃^�C�~���O�ł������삵�Ȃ��B

  // MainFunc�����삵�����`�F�b�N
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
      mpGameManager->SetDefaultRenderingParameter();  // �v���Z�X���ς�����u�ԂɃ��Z�b�g
    }
  }

  return status;
}

//------------------------------------------------------------------
/**
 * @brief	PROC�̏���
 * �o�^����PROC��NextFrameMainFunc�������s���܂��B �K�؂Ȉʒu�ŌĂ�ł��������B
 * @return MainStatus �X�e�[�^�X�̓������A���Ă��܂�
 */
//------------------------------------------------------------------
gfl2::proc::MainStatus GameProcManager::NextFrameMain(void)
{
  gfl2::proc::Result result;
  
  // �֐����R�[���B
  if( m_IsCallNextFrame ){
    if( this->GetMainHandleProcSeq() == gfl2::proc::SEQ_MAIN ){
      GameProc* p_proc = reinterpret_cast<GameProc*>(this->GetMainHandleBaseProcess());
	  	result = p_proc->NextFrameMainFunc(this);

      // NextFrame��SEQ_MAIN���I���ꍇ�̕���
      if (result == gfl2::proc::RES_FINISH) {  
  			this->SetMainHandleProcSeq(gfl2::proc::SEQ_END);
  			p_proc->ResetSubSeq();
	  	}
    }
  }

  return gfl2::proc::MAINSTATUS_VALID;
}








} //gamesystem
