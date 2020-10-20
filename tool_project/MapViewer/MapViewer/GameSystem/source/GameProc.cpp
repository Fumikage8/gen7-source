//===================================================================
/**
 * @file    GameProc.cpp
 * @brief   �Q�[�����j�̃v���Z�X�N���X
 * @author  k.ohno
 * @date    2011.3.31
*/
//===================================================================

#include <GameSystem/include/GameProc.h>
#include <GameSystem/include/GameProcManager.h>


namespace GameSystem {

//-----------------------------------------------------------------------------
/**
*     Proc
 */
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief   �R���X�g���N�^
 */
//------------------------------------------------------------------
GameProc::GameProc(void) : 
  mpGameProcManager(NULL)
{
}



//------------------------------------------------------------------
/**
 * @brief	  PROC�̏��������s���֐�
 * @return	Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����MainFunc�ɐi��
 */
//------------------------------------------------------------------
gfl2::proc::Result GameProc::InitFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED(pManager);

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	  PROC�̃��C���X�V���s���֐�
 * @return	Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����EndFunc�ɐi��
 */
//------------------------------------------------------------------
gfl2::proc::Result GameProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED(pManager);
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROC�̕`�揈��(������)
* @param pManager   �v���Z�X�Ǘ��}�l�[�W���[
* �o�^����PROC�ɂ��A�f�B�X�v���C�o�b�t�@�Ɉˑ����Ȃ��`�揈�������s���܂��B
* ��) ���̎����̕`��R�}���h�̃Z�[�u�A�I�t�X�N���[���o�b�t�@�ւ̕`��
*/
void GameProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED(pManager);
}

/**
* @brief	PROC�̕`�揈��
* @param pManager   �v���Z�X�Ǘ��}�l�[�W���[
* @param displayNo  �`��Ώۉ��
* �o�^����PROC�ɂ��A�t���[���o�b�t�@�ւ̕`������s���܂��B
* �f�B�X�v���C�o�b�t�@�̐؂�ւ��A�f�B�X�v���C�o�b�t�@�ւ̓]���͌Ăяo�������s���܂��B
*/
void GameProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  GFL_UNUSED(pManager);
}

//------------------------------------------------------------------
/**
 * @brief	  PROC�̃��C�����s���֐�
 * @return	Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����EndFunc�ɐi��
 */
//------------------------------------------------------------------
gfl2::proc::Result GameProc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED(pManager);

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	  PROC�̏I�����s���֐�
 * @return	Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤���ŏI������Ɖ��߂���
 */
//------------------------------------------------------------------
gfl2::proc::Result GameProc::EndFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED(pManager);

  return gfl2::proc::RES_FINISH;
}


//----------------------------------------------------------------------------
/**
 * @brief   PROC�̃C�x���g�N�����m����
 *
 *	@param	pManager    �v���Z�X�}�l�[�W��
 */
//-----------------------------------------------------------------------------
void GameProc::EventCheckFunc(GameManager* pManager)
{
  GFL_UNUSED(pManager);
}

//----------------------------------------------------------------------------
/**
 *	@brief  GameManager�̎擾
 *	@return GameManager�|�C���^
 */
//-----------------------------------------------------------------------------
GameManager* GameProc::GetGameManager(void)
{
  return mpGameProcManager->GetGameManager(); 
}


} /* namespace GameSys */

