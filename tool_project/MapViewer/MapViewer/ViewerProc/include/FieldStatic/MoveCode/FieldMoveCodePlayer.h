#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodePlayer.h
*  @brief  �t�B�[���h�p ����R�[�h �v���C���[
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_Quaternion.h>

#include "FieldMoveCode_define.h"
#include "FieldMoveCodeBase.h"

namespace Field{ namespace MoveModel {

class FieldMoveCodePlayer : public FieldMoveCodeBase
{
public:

  // �X�e�[�g�̎��
  enum PlayerMove
  {
    PLAYER_MOVE_WAIT=0,

    PLAYER_MOVE_WALK_SLOW,        // �E�ѕ���

    PLAYER_MOVE_WALK_START,       // �ʏ����
    PLAYER_MOVE_WALK_LOOP,
    PLAYER_MOVE_WALK_END,

    PLAYER_MOVE_RUN_START,        // ����
    PLAYER_MOVE_RUN_LOOP,
    PLAYER_MOVE_RUN_END,

    PLAYER_MOVE_DEBUG_RUN_START,  // �f�o�b�O����
    PLAYER_MOVE_DEBUG_RUN_LOOP,
    PLAYER_MOVE_DEBUG_RUN_END,
    PLAYER_MOVE_MAX
  };

  // �X�e�[�g�̓��e
  struct PlayerMoveSetting
  {
    c16  name[256];
    f32  moveSpeed;     // �ړ����x
    f32  motionSpeed;   // ���[�V�����Đ����x
    s32  motionId;      // �Đ����[�V����
    f32  stickLevel;    // ���͒l�����̒l�ȉ��Ȃ甭���Ώ�
  };

  // �X�e�[�g�̑J�ړ��ɕK�v�ȃ��[�N
  struct Work
  {
    u32                    oldState;
    u32                    oldoldState;
    gfl2::math::Quaternion targetQua;
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �R���X�g���N�^
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodePlayer(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �f�X�g���N�^
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodePlayer(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  �Ή����Ă��铮��R�[�h�̎擾
  *
  *  @return ����R�[�h
  */
  //-----------------------------------------------------------------------------
  virtual FIELD_MOVE_CODE_ID GetMVCode(void) const { return FIELD_MOVE_CODE_PLAYER; }
  
  //----------------------------------------------------------------------------
  /**
  *  @brief  ����R�[�h�����@������
  *
  *  @param  pMoveCodeWork  ����R�[�h���[�N
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  ����R�[�h����  ���C���@
  *
  *  @param  pMoveCodeWork  ����R�[�h���[�N
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTI�}�l�[�W���[�Ɏ��g�̃v���p�e�B��o�^
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void) const;

private:

  //----------------------------------------------------------------------------
  /**
  *  @brief  �A�i���O�X�e�B�b�N�̓|�������X�e�[�g���擾
  *
  *  @param  stick  �A�i���O�X�e�B�b�N�̓|���
  *  @return �X�e�[�g
  */
  //-----------------------------------------------------------------------------
  u32 GetStateFromStickLevel( f32 stick ) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  ����R�[�h����  ���C���̃T�u�֐�(�X�e�[�g���㏑������^�C�~���O�ŌĂ΂��)
  *
  *  @param  pMoveCodeWork  ����R�[�h���[�N
  */
  //-----------------------------------------------------------------------------
  virtual u32 MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 nowState,u32 oldState,u32 oldoldState) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  ����R�[�h����  ���C���̃T�u�֐�(�X�e�[�g���ς�����Ƃ��ɌĂ΂��)
  *
  *  @param  pMoveCodeWork  ����R�[�h���[�N
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 nowState,u32 oldState,u32 oldoldState) const;

private:

  // ���]���x
  f32 m_turnSpeed;
  // �⊮�t���[��
  u32 m_interpFrameForRunToRunStop;
  u32 m_interpFrameForRunToWalk;
  u32 m_interpFrameForWalkToRun;

protected:

  // �e�X�e�[�g�̓��e
  PlayerMoveSetting m_PlayerMoveSettingTable[ PLAYER_MOVE_MAX ];

};  // end of class FieldMoveCodePlayer;
  
}; //end of namespace mmodel
}; //end of namespace fld
