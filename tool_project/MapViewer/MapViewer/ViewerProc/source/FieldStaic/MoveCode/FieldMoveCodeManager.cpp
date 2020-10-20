//=================================================================================================
/**
*  @file   FieldMoveCodeManager.cpp
*  @brief  �t�B�[���h�p ����R�[�h�}�l�[�W���[
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCodeManager.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCodeBase.h"
///#include "../../include/MoveCode/FieldMoveCodeTemplate.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCodePlayer.h"
///#include "../../include/MoveCode/FieldMoveCodeNpc.h"
///#include "../../include/MoveCode/FieldMoveCodeTrainerEye.h"
///#include "../../include/MoveCode/FieldMoveCodeRideKentaros.h"

namespace Field{ namespace MoveModel {

//=====================================================================================================================
// �}�l�[�W���[���g�Ɋւ��鑀��
//=====================================================================================================================

const FieldMoveCodeBase * FieldMoveCodeManager::m_MoveCodeTable[ FIELD_MOVE_CODE_NUM ] ={};

//-----------------------------------------------------------------------------
/**
*  @brief  �R���X�g���N�^
*/
//-----------------------------------------------------------------------------
FieldMoveCodeManager::FieldMoveCodeManager()
{
  for (int i = 0; i < FIELD_MOVE_CODE_NUM; ++ i )
  {
    m_MoveCodeTable[i] = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  �f�X�g���N�^
*/
//-----------------------------------------------------------------------------
FieldMoveCodeManager::~FieldMoveCodeManager()
{
  Terminate();
}

//----------------------------------------------------------------------------
/**
*  @brief  ������
*
*  @param  pHeap  ���[�N�쐬���ɗ��p����q�[�v
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeManager::Initialize(gfl2::heap::HeapBase * pHeap)
{
  m_MoveCodeTable[FIELD_MOVE_CODE_NONE]           = GFL_NEW( pHeap ) FieldMoveCodeBase();
  m_MoveCodeTable[FIELD_MOVE_CODE_PLAYER]         = GFL_NEW( pHeap ) FieldMoveCodePlayer();
  ///m_MoveCodeTable[FIELD_MOVE_CODE_NPC]            = GFL_NEW( pHeap ) FieldMoveCodeNpc();
  ///m_MoveCodeTable[FIELD_MOVE_CODE_TEMPLATE]       = GFL_NEW( pHeap ) FieldMoveCodeTemplate();
  ///m_MoveCodeTable[FIELD_MOVE_CODE_TRAINER_EYE]    = GFL_NEW( pHeap ) FieldMoveCodeTrainerEye();
  ///m_MoveCodeTable[FIELD_MOVE_CODE_RIDE_KENTAROS]  = GFL_NEW( pHeap ) FieldMoveCodeRideKentaros();
}

//----------------------------------------------------------------------------
/**
*  @brief  ���
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeManager::Terminate(void)
{
  for (int i = 0; i < FIELD_MOVE_CODE_NUM; ++ i )
  {
    GFL_SAFE_DELETE( m_MoveCodeTable[i] );
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  ����R�[�h�̎擾
*
*  @param  fieldMoveCodeId   ����R�[�hID
*  @return ����R�[�h
*/
//-----------------------------------------------------------------------------
const FieldMoveCodeBase * FieldMoveCodeManager::GetMoveCode(FIELD_MOVE_CODE_ID fieldMoveCodeId)
{
  const FieldMoveCodeBase * move_code = m_MoveCodeTable[static_cast<u32>(fieldMoveCodeId)];
  if ( move_code )
  {
    return move_code;
  }
  return m_MoveCodeTable[0];  //MOVE_CODE_NONE
}

//=====================================================================================================================
// ����R�[�h�̃��[�N�Ɋւ��鑀��
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
*  @brief  ����R�[�h�̃��[�N��������
*
*  @param  pMoveCodeWork  ����R�[�h���[�N
*  @param  pModel         ���샂�f��
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeManager::InitializeMoveCodeWork(FieldMoveCodeWork* pMoveCodeWork, FieldMoveModel* pModel)
{
  pMoveCodeWork->pModel     = pModel;
  pMoveCodeWork->moveCodeId = FIELD_MOVE_CODE_NONE;
}

//----------------------------------------------------------------------------
/**
*  @brief  ����R�[�h�̃��[�N���擾
*    
*  @param  pMoveCodeWork  ����R�[�h����R�[�h���[�N
*  @param  size           �K�v�ȃT�C�Y
*  @return ����R�[�h���[�N
*/
//-----------------------------------------------------------------------------
void* FieldMoveCodeManager::GetMoveCodeWork(FieldMoveCodeWork* pMoveCodeWork, u32 size)
{
  GFL_ASSERT( size <= FIELD_MOVE_CODE_WORK_SIZE );
  return reinterpret_cast<void*>(pMoveCodeWork->work);
}

}; //end of namespace MoveModel
}; //end of namespace Field
