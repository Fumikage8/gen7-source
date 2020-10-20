//=================================================================================================
/**
 *  @file   CollisionSceneDefault.cpp
 *  @brief  �f�t�H���g�R���W�����V�[��
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include "ViewerProc/include/FieldStatic/Collision/CollisionSceneDefault.h"
#include "ViewerProc/include/FieldStatic/Collision/CollisionSolverDefault.h"

//-----------------------------------------------------------------------------
/**
*  @brief  �R���X�g���N�^
*/
//-----------------------------------------------------------------------------
CollisionSceneDefault::CollisionSceneDefault()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  �f�X�g���N�^
*/
//-----------------------------------------------------------------------------
CollisionSceneDefault::~CollisionSceneDefault()
{
}

//----------------------------------------------------------------------------
/**
*  @brief   �����҂̍쐬
*
*  @return  solver
*  @note    ���̊֐����I�[�o�[���C�h���邱�Ƃɂ��Փ˂̋�����ύX�ł��܂�
*/
//-----------------------------------------------------------------------------
BaseSolver* CollisionSceneDefault::CreateSolver(void)
{
  // �_���ăf�t�H���gnew���g�p���Ă��܂��B(�e�N���X��m_pAllocator�������܂�)
  return new CollisionSolverDefault();
}