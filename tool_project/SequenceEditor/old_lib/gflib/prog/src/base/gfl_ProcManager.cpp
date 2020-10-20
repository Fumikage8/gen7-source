//=============================================================================================
/**
 * @file	gfl_ProcManager.cpp
 * @brief	�v���Z�X�Ǘ��}�l�[�W���[
 * @date	2010.10.18
 */
//=============================================================================================


#include "gfl_base.h"
#include "gfl_debug.h"
#include <ro/gfl_RoManager.h>

namespace gfl {
namespace proc {




//=============================================================================================
//
//		�O�����J�֐�
//
//=============================================================================================

//------------------------------------------------------------------
/**
 * @brief	  �v���Z�X�Ǘ��V�X�e��������
 * @param	  heap_id			�g�p����q�[�v��ID
 */
//------------------------------------------------------------------
Manager::Manager(gfl::heap::HeapBase * pBase)
{
	pHeapBase = pBase;
	mainHandle = NULL;
	call_flag = false;
	jump_flag = false;
	draw_flag = false;
  next_data = NULL;
}

//------------------------------------------------------------------
/**
 * @brief	  �v���Z�X�Ǘ��V�X�e���I��
 */
//------------------------------------------------------------------

Manager::~Manager()
{
#if GFL_DEBUG //============== GFL_DEBUG
  if(mainHandle!=NULL){
    GFL_WARNING("mainHandle���������Ă��Ȃ�\n");
  }
#endif //GFL_DEBUG
}


//------------------------------------------------------------------
// �q�v���b�N�ɐ؂�ւ�
//------------------------------------------------------------------
void Manager::entryCallProc(void)
{
  processHandle* proc;
  proc = GFL_NEW(pHeapBase) processHandle;

  if(mainHandle){
    //�e�q�֐؂�ւ��̏ꍇ�AMAIN�ł̐؂�ւ��Ƃ����킩��₷����Ԃɂ���
    GFL_ASSERT(mainHandle->proc_seq == SEQ_MAIN);  //@fix�i����

    //�e�q�֌W�\�z
    proc->parent = mainHandle;
    mainHandle->child = proc;
    mainHandle = proc;
  }
  else{
    mainHandle = proc;
  }
  mainHandle->my = next_data;
  //�t���O�𗎂�
  draw_flag = false;
  call_flag = false;
  next_data = NULL;
}

//------------------------------------------------------------------
// �v���b�N�ړ�
//------------------------------------------------------------------
void Manager::jumpCallProc(void)
{
  processHandle* proc;
  proc = GFL_NEW(pHeapBase) processHandle;

  //�ړ��̏ꍇ�͑S���I��������ɍs����
  GFL_ASSERT_STOP(mainHandle->proc_seq == SEQ_UNLOAD);  //@fix

  //�e�q�֌W�\�z
  if(mainHandle->parent){
    proc->parent = mainHandle->parent;
    proc->parent->child = proc;
  }

  deleteMainProc( mainHandle );

  mainHandle = proc;
  mainHandle->my = next_data;

  //�t���O�𗎂�
  jump_flag = false;
  next_data = NULL;
}

//------------------------------------------------------------------
// ���C���v���Z�X�̍폜
//------------------------------------------------------------------
void Manager::deleteMainProc( processHandle* proc )
{
#if GFL_ENABLE_RO
  nn::ro::Module* modules[16+1];
  u32             childNum = proc->my->GetChildModulePtrBufferNum();
  //
  gfl::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl::ro::RoManager );

  // ���[�J���o�b�t�@��16�����m�ۂ��Ă��Ȃ��̂ŔO�̂���
  GFL_ASSERT_STOP( (childNum<=16) );

  // ���蓖�Ă�ꂽ���W���[�����R�s�[
  modules[0] = proc->my->GetModulePtr();
  for( u32 i = 0; i < childNum; i++ ){
    modules[1+i] = proc->my->GetChildModulePtr(i);
  }

  // �f�X�g���N�^���s�ׁ̈ADLL���W���[������̑O��PROC���������
  GFL_SAFE_DELETE( proc->my );

  // DLL���W���[�������
  u32 moduleNum = (childNum+1);
  for( u32 i = 0; i < moduleNum; i++ )
  {
    if( modules[i] != NULL ){
      roMgr->DisposeModule( modules[i] );
    }
  }
#else
  GFL_SAFE_DELETE( proc->my );
#endif
  proc->my = NULL;
}

//------------------------------------------------------------------
// �v���b�N�̏����ƃW�����v���s��
//------------------------------------------------------------------
void Manager::deleteAndJumpProc(void)
{
  processHandle* proc = mainHandle;

  if(jump_flag){  //�W�����v���s��
    jumpCallProc();
  }
  else if(proc->parent){   //�e�q�֌W��߂�
    proc->parent->child = NULL;
    mainHandle = proc->parent;
    deleteMainProc( proc );
  }
  else{  //�������͂Ȃ��Ȃ����̂ŏ���
    deleteMainProc( proc );
    mainHandle = NULL;
  }
  GFL_DELETE proc;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X���쏈�����C��
 * @param	proc	�v���Z�X�ւ̃|�C���^
 * @retval	true	�v���Z�X����I��
 * @retval	false	�v���Z�X����p����
 */
//------------------------------------------------------------------
bool Manager::procMain(void)
{
	Result result;

	switch (mainHandle->proc_seq) {
	case SEQ_LOAD:
		mainHandle->proc_seq = SEQ_INIT;
		/* fall through */
	case SEQ_INIT:
		result = mainHandle->my->InitFunc(this);
		if (result == RES_FINISH) {
			mainHandle->proc_seq = SEQ_MAIN;
			mainHandle->my->ResetSubSeq();
		}
		break;
	case SEQ_MAIN:
		result = mainHandle->my->UpdateFunc(this);
    draw_flag = true;

		if (result == RES_FINISH) {
			mainHandle->proc_seq = SEQ_END;
			mainHandle->my->ResetSubSeq();
		}
		break;
	case SEQ_END:
    draw_flag = false;
		result = mainHandle->my->EndFunc(this);
		if (result == RES_FINISH) {
			mainHandle->proc_seq = SEQ_UNLOAD;
      return true;
		}
		break;
	}
	return false;
}

//------------------------------------------------------------------
/**
 * @brief	PROC�̏���
 * �o�^����PROC�̏������s���܂��B �K�؂Ȉʒu�ŌĂ�ł��������B
 * �K�v�ȏꍇ�́A�O��ŃO���t�B�b�N�̏������Ă�ł��������B
 */
//------------------------------------------------------------------
void Manager::Draw(void)
{
  if( draw_flag )
  {
    //draw_flag = false;
    mainHandle->my->DrawFunc(this);
  }
}

//------------------------------------------------------------------
/**
 * @brief	�i�s���n���h���ւ̃A�N�Z�T
 */
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �i�s���n���h���̃V�[�P���X���擾
 *
 *	@retval �V�[�P���X
 */
//-----------------------------------------------------------------------------
ProcSeq Manager::GetMainHandleProcSeq( void ) const
{
  if( mainHandle ){
    return mainHandle->proc_seq;
  }
  return SEQ_LOAD;  // �i�s���̂��̂��Ȃ��B
}

//----------------------------------------------------------------------------
/**
 *	@brief  �i�s���n���h���̃V�[�P���X��ݒ�
 *
 *	@param	seq   �V�[�P���X
 */
//-----------------------------------------------------------------------------
void Manager::SetMainHandleProcSeq( ProcSeq seq )
{
  if( mainHandle ){
    mainHandle->proc_seq = seq;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �i�s���n���h���̃v���Z�X���擾
 *
 *	@retval NULL�ȊO  �v���Z�X
 *	@retval NULL      ���i�s���̃v���Z�X�͂Ȃ�
 */
//-----------------------------------------------------------------------------
BaseProcess* Manager::GetMainHandleBaseProcess( void ) const
{
  if( mainHandle ){
    return mainHandle->my;
  }
  return NULL;
}

/**
 * @brief �`��t���O���Z�b�g
 *
 * @param draw  �`��t���O
 */
void Manager::SetDrawFlag( bool draw )
{
  draw_flag = draw;
}


//------------------------------------------------------------------
/**
 * brief	�v���Z�X�Ǘ��V�X�e�����C��
 * @param	psys
 */
//------------------------------------------------------------------
MainStatus Manager::Main(void)
{

	if( (mainHandle == NULL) && (next_data==NULL) ){
    return MAINSTATUS_NULL;   //�����Ă��镨�͂Ȃ�
  }

  MainStatus status = MAINSTATUS_VALID;

  if(call_flag) {  // �R�[������
    entryCallProc();
    status = MAINSTATUS_CALL;
  }

  if ( procMain() ) {
    deleteAndJumpProc();  //�e�ɖ߂邩�W�����v���s��
    status = MAINSTATUS_CHANGE;
  }else{

    if((mainHandle == NULL) && (next_data==NULL) ){
      status = MAINSTATUS_NULL;
    }
  }

  return status;
}


//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�؂�ւ����N�G�X�g
 * @param	psys
 * @param	procdata
 * @param	pwork
 * @return  ��������Proc
 */
//------------------------------------------------------------------
BaseProcess* Manager::SetNextProc(BaseProcess * process)
{
  GFL_ASSERT( jump_flag == false );  //@fix
  GFL_ASSERT(mainHandle);  //@fix

  if(mainHandle==NULL){
    return CallProc(process);
  }
  jump_flag = true;
  next_data = process;
  draw_flag = false;
  return next_data;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�Ăяo��
 * @param	psys
 * @param	procdata
 * @param	pwork
 * @return  ��������Proc
 */
//------------------------------------------------------------------
BaseProcess* Manager::CallProc(BaseProcess * process)
{
  GFL_ASSERT( call_flag == false );  //@fix
  call_flag = true;
  next_data = process;
  draw_flag = false;
  return next_data;
}


//------------------------------------------------------------------
/**
 * @brief	 �������Ă���BaseProcess�̐e�ɓ�����BaseProcess��Ԃ�
 * @return BaseProcess�N���X�|�C���^ �e���Ȃ����NULL
 */
//------------------------------------------------------------------

BaseProcess* Manager::GetParentBaseProcess(void)
{
  if(mainHandle->parent){
    return mainHandle->parent->my;
  }
  return NULL;
}


//------------------------------------------------------------------
/**
 * @brief	 �������Ă���BaseProcess��Ԃ�
 * @return BaseProcess�N���X�|�C���^ �Ȃ����NULL
 */
//------------------------------------------------------------------

BaseProcess* Manager::GetBaseProcess(void)
{
  if(mainHandle){
    return mainHandle->my;
  }
  return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	 ���ɐݒ肳��Ă���BaseProcess��Ԃ�
 * @return BaseProcess�N���X�|�C���^ �Ȃ����NULL
 */
//------------------------------------------------------------------

BaseProcess* Manager::GetNextBaseProcess(void)
{
  if( jump_flag ){
    return next_data;
  }
  return NULL;
}

// 
//----------------------------------------------------------------------------
/**
 *	@brief  ���C�����쒆�̃v���Z�X�̏����������`�F�b�N
 *
 *	@retval true  ����������
 */
//-----------------------------------------------------------------------------
bool Manager::GetBaseProcessInitialized( void ) const
{
  // Jump��Call�����s����Ă����ԂŁA
  if( !jump_flag && !call_flag ){
    // MAIN�V�[�P���X�ɂȂ����B
    if( this->GetMainHandleProcSeq() == SEQ_MAIN ){
      return true;
    }
  }
  return false;
}



} //proc
} //gfl
