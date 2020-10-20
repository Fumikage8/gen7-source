/**
* @brief  �t�B�[���h�J�����̃^�[�Q�b�g�A���샂�f��
* @file   FieldCameraTargetTypeMoveModel.cpp
* @author ikuta_junya
* @data   2015.05.21
*/


// field
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModel.h"
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetTypeMoveModel.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Target );


/**
* @brief �R���X�g���N�^
*
* @param  pMoveModel ���g�A�����͊Ǘ����Ȃ�
*
* @return ����
*/ 
TargetTypeMoveModel::TargetTypeMoveModel( MoveModel::FieldMoveModel* pMoveModel ) :
  m_pMoveModel( pMoveModel )
{


}

/**
* @brief �f�X�g���N�^
*
* @return ����
*/
TargetTypeMoveModel::~TargetTypeMoveModel( void )
{

}

/**
* @brief �J�������݂���W���擾����
*
* @param  pOut  �i�[��
*
* @return ����
*/
b32 TargetTypeMoveModel::GetCameraTargetPosition( gfl2::math::Vector3* pOut ) const
{
  if( m_pMoveModel == NULL )
  {
    return false;
  }
  
  poke_3d::model::CharaModel* pCharaModel = m_pMoveModel->GetCharaDrawInstance();
  if( pCharaModel == NULL )
  {
    return false;
  }

  *pOut = pCharaModel->GetPosition();
  pOut->y += 133.0f;  // @todo ��
  return true;
}

/**
* @brief ���g��ݒ�
*
* @param  pMoveModel ���g�A�����͊Ǘ����Ȃ�
*
* @return ����
*/ 
void TargetTypeMoveModel::SetContent( MoveModel::FieldMoveModel* pMoveModel )
{
  m_pMoveModel = pMoveModel;
}



GFL_NAMESPACE_END( Target );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );