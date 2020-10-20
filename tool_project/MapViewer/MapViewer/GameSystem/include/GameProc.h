#ifndef __GAMEPROC_H__
#define __GAMEPROC_H__
//=============================================================================================
/**
 * @file   GameProc.h
 * @brief  �Q�[���̃��C���v���Z�X
 * @author k.ohno
 * @date   2011.3.31
 */
//=============================================================================================
#pragma once


#include <proc/include/gfl2_Proc.h>

#include "GameSystem/include/GameFrameMode.h"
#include "GameSystem/include/GameManager.h"

namespace GameSystem{

  class GameProcManager;


  class GameProc : public gfl2::proc::BaseProcess{
    GFL_FORBID_COPY_AND_ASSIGN(GameProc); //�R�s�[�R���X�g���N�^�{����֎~

  protected:
    GameProcManager*                mpGameProcManager;  ///< �v���Z�X�}�l�[�W��


  public:
    GameProc(void);  //�R���X�g���N�^
    virtual ~GameProc(void){};  //�f�X�g���N�^


    //------------------------------------------------------------------
    /**
     * @brief   FrameMode�̎擾
     * @retval   FrameMode
     *
     * �t���[�����[�h��؂�ւ���Ƃ��́A�߂�l��ς��Ă��������B
     */
    //------------------------------------------------------------------
    virtual GameSystem::FrameMode GetFrameMode( void ) const { return GameSystem::FRAMEMODE_DEFAULT; }

    //------------------------------------------------------------------
    /**
     * @brief   PROC�̏��������s���֐�
     * @return  Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����UpdateFunc�ɐi��
     */
    //------------------------------------------------------------------
    virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

    //------------------------------------------------------------------
    /**
     * @brief	  PROC�̃��C���X�V���s���֐�
     * @return	Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����EndFunc�ɐi��
     */
    //------------------------------------------------------------------
    virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

    /**
    * @brief	PROC�̕`�揈��(������)
    * @param pManager   �v���Z�X�Ǘ��}�l�[�W���[
    * �o�^����PROC�ɂ��A�f�B�X�v���C�o�b�t�@�Ɉˑ����Ȃ��`�揈�������s���܂��B
    * ��) ���̎����̕`��R�}���h�̃Z�[�u�A�I�t�X�N���[���o�b�t�@�ւ̕`��
    */
    virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

    /**
    * @brief	PROC�̕`�揈��
    * @param pManager   �v���Z�X�Ǘ��}�l�[�W���[
    * @param displayNo  �`��Ώۉ��
    * �o�^����PROC�ɂ��A�t���[���o�b�t�@�ւ̕`������s���܂��B
    * �f�B�X�v���C�o�b�t�@�̐؂�ւ��A�f�B�X�v���C�o�b�t�@�ւ̓]���͌Ăяo�������s���܂��B
    */
    virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

    //------------------------------------------------------------------
    /**
     * @brief   PROC�̃��C�����s���֐�
     *  1/30�t���[���̇A�t���[���ڂ̏����ł��B
     *  1/60�t���[�����쎞�ɂ́A�g���܂���B
     * @return  Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����EndFunc�ɐi��
     */
    //------------------------------------------------------------------
    virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);
    //------------------------------------------------------------------
    /**
     * @brief   PROC�̏I�����s���֐�
     * @return  Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤���ŏI������Ɖ��߂���
     */
    //------------------------------------------------------------------
    virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
    //------------------------------------------------------------------
    /**
     * @brief   PROC�̃C�x���g�N�����m����
     *
     *  �ȉ��̃^�C�~���O�ōs���ׂ��A�C�x���g�N������
     *  �@GameProc->EventCheckFunc
     *  �@EventManager->Main
     *  �@GameProc->Main
     */
    //------------------------------------------------------------------
    virtual void EventCheckFunc(GameManager* pManager);


    //------------------------------------------------------------------
    /**
     * @brief   GameProcManager�̐ݒ�
     * @param   GameProcManager�|�C���^
     */
    //------------------------------------------------------------------
    inline void SetGameProcManager(GameProcManager* pGameProcManager){ mpGameProcManager = pGameProcManager; };
    //------------------------------------------------------------------
    /**
     * @brief   GameProcManager�̎擾
     * @return  GameProcManager�|�C���^
     */
    //------------------------------------------------------------------
    inline GameProcManager* GetGameProcManager(void){ return mpGameProcManager; };

    //----------------------------------------------------------------------------
    /**
     *	@brief  GameManager�̎擾
     *	@return GameManager�|�C���^
     */
    //-----------------------------------------------------------------------------
    GameManager* GetGameManager(void);


  };  //class GameProc


}//namespace GameSys

#endif //__GAMEPROC_H__
