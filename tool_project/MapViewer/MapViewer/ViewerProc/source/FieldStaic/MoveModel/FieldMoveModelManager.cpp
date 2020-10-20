//=================================================================================================
/**
*  @file   FieldMoveModelManager.cpp
*  @brief  �t�B�[���h�p ���샂�f���}�l�[�W���[
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <heap/include/gfl2_Heap.h>
#include <arc_def_index/arc_def.h>
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModelManager.h"
#include "ViewerProc/include/FieldRo/MyRenderingPipeLine.h" 
#include <renderingengine/include/SceneGraph/Instance/gfl2_InstanceNode.h>
// RTTI�f�o�b�O�V�X�e��
///#include "../../include/RttiDebug/RttiEditor.h"
///#include "../../include/RttiDebug/RttiManager.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  �R���X�g���N�^
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::FieldMoveModelManager()
: m_pInstanceRootNode(NULL)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  �f�X�g���N�^
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::~FieldMoveModelManager()
{
  TerminateWork();
  TerminateResource();
}

//----------------------------------------------------------------------------
/**
*  @brief  ������(���[�N)
*
*  @param  pInstanceRootNode    �c���[���`�����邽�߂̃��[�g�m�[�h
*  @param  pFileHeap            ���[�N�ō쐬�ɗ��p�����q�[�v
*
*  @note   �����ŗ��p�����q�[�v�̃A�h���X�͓����ŕێ����Ă͂����܂���B�ǂ̃A�v�����֌W�Ȃ����������Ă��邽�߃Q�[���N�����̂ݗ��p��
*
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::InitializeWork(gfl2::renderingengine::scenegraph::instance::InstanceNode* pInstanceRootNode,
                                           gfl2::heap::HeapBase*                                      pHeap)
{
  m_pInstanceRootNode = pInstanceRootNode;
  m_pInstanceRootNode->AddChild( &m_fieldMoveModelRoot );

	m_ResourceAllocator.init(pHeap);
}

//----------------------------------------------------------------------------
/**
*  @brief  ������(���\�[�X)
*
*  @param  MyRenderingPipeLine  �����_�����O�p�C�v���C��
*  @param  pAllocator           �A���P�[�^
*  @param  pHeap                �q�[�v
*  @param  pFileManager         �t�@�C���}�l�[�W���[
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::InitializeResource(Field::MyRenderingPipeLine* pMyRenderingPipeLine,
                                               gfl2::gfx::IGLAllocator*    pAllocator,
                                               gfl2::heap::HeapBase*       pHeap,
                                               gfl2::fs::AsyncFileManager* pFileManager )
{
  m_pMyRenderingPipeLine  = pMyRenderingPipeLine;
  m_pAllocator            = pAllocator;
  m_pHeap                 = pHeap;
  m_pFileManager          = pFileManager;
  ///m_charaModelFactory.Initialize(pFileManager, m_pHeap, ARCID_CHARA_MODEL_FIELD);
}

//-----------------------------------------------------------------------------
/**
*  @brief  ���(���[�N)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateWork(void)
{
  // ���샂�f�����c���[����o�^���������[�N���폜
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if(  m_fieldMoveModel[i].IsSetupWork() )
    {
      m_fieldMoveModelRoot.RemoveChild( &m_fieldMoveModel[i] );
      m_fieldMoveModel[i].ClearWork();
    }
  }
  // ���샂�f�����c���[�����
  m_pInstanceRootNode->RemoveChild( &m_fieldMoveModelRoot );
  m_pInstanceRootNode = NULL;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ���(���\�[�X)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateResource(void)
{
  // �p�C�v���C������o�^���������\�[�X���폜
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if(  m_fieldMoveModel[i].IsSetupResource() )
    {
      m_pMyRenderingPipeLine->RemoveEdgeRenderingTarget( m_fieldMoveModel[i].GetCharaDrawInstance()->GetModelInstanceNode() );
      m_fieldMoveModel[i].ClearResource();
    }
  }
  // �L�������f���t�@�N�g���[�̉��
  ///m_charaModelFactory.Finalize();
}

//-----------------------------------------------------------------------------
/**
*  @brief  �O�X�V(�R���W�����ȊO�̍��W/�p���̕ύX���s��)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::PrevUpdate(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_fieldMoveModel[i].IsSetup() )
    {
      m_fieldMoveModel[i].PrevUpdate();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  �g���Ă��Ȃ����샂�f���̗v�f��Ԃ� 
*
*  @return ���샂�f���̗v�f(FIELD_MOVE_MODEL_MAX�̏ꍇ�̓G���[)
*/
//-----------------------------------------------------------------------------
u32 FieldMoveModelManager::GetFieldMoveModelIndexFromFreeSpace(void) 
{ 
  for( u32 i=FIELD_MOVE_MODEL_NPC_START ; i<FIELD_MOVE_MODEL_MAX ; i++ ) 
  { 
    if( !(m_fieldMoveModel[i].IsSetupWork() || m_fieldMoveModel[i].IsSetupResource()) ) 
    { 
      return i; 
    } 
  }
  return FIELD_MOVE_MODEL_MAX; 
}

//-----------------------------------------------------------------------------
/**
*  @brief  �C�x���gID(���ʗp�̃��j�[�N��ID)���瓮�샂�f���̗v�f��Ԃ� 
*
*  @param  eventId               �C�x���gID
*  @return ���샂�f���̗v�f(FIELD_MOVE_MODEL_MAX�̏ꍇ�̓G���[)
*/
//-----------------------------------------------------------------------------
u32 FieldMoveModelManager::GetFieldMoveModelIndexFromEventId(u32 eventId) 
{ 
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ ) 
  { 
    if( m_fieldMoveModel[i].GetEventId() == eventId ) 
    { 
      return i; 
    } 
  }
  return FIELD_MOVE_MODEL_MAX; 
}

//-----------------------------------------------------------------------------
/**
*  @brief  �X�V(�R���W�����V�[���ւ̍��W/�p���̓o�^���s��)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::Update(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_fieldMoveModel[i].IsSetup() )
    {
      m_fieldMoveModel[i].Update();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  ��X�V(�R���W�����V�[������̌��ʂ�K�p���A�ŏI�I�ȍ��W/�p���̌���/�C�x���g�̎��s���s��)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::PostUpdate(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_fieldMoveModel[i].IsSetup() )
    {
      m_fieldMoveModel[i].PostUpdate();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  ���샂�f���̃��[�N�쐬
*
*  @param  fieldMoveModelId       ���샂�f��ID
*  @param  pFieldMoveModelHeader  ���샂�f�����N�G�X�g
*  @return ����
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::CREATE_STATE FieldMoveModelManager::CreateWork(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeader* pFieldMoveModelHeader)
{
  // ���[�N������
  if( !m_fieldMoveModel[ (u32)fieldMoveModelId ].IsSetupWork() )
  {
    // ���샂�f���̃��[�N��������
    m_fieldMoveModel[ (u32)fieldMoveModelId ].SetupWork( pFieldMoveModelHeader );
    // ���샂�f�����c���[�ɓo�^
    m_fieldMoveModelRoot.AddChild( &m_fieldMoveModel[ (u32)fieldMoveModelId ] );
    return CREATE_SUCCESS;
  }
  return CREATE_ERROR;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ���샂�f���̃��\�[�X�쐬
*
*  @param  fieldMoveModelId       ���샂�f��ID
*  @param  pFieldMoveModelHeader  ���샂�f�����N�G�X�g
*  @return ����
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::CREATE_STATE FieldMoveModelManager::CreateResource(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeader* pFieldMoveModelHeader)
{
  // ���\�[�X������
  if( !m_fieldMoveModel[ (u32)fieldMoveModelId ].IsSetupResource() )
  {
    // ���샂�f���̃��\�[�X��������
		m_fieldMoveModel[ (u32)fieldMoveModelId ].SetupResource( m_pAllocator, m_pHeap , /*&m_charaModelFactory,*/ pFieldMoveModelHeader, &m_ResourceAllocator );
    // ���샂�f����`��p�C�v���C���ɓo�^
    m_pMyRenderingPipeLine->AddEdgeRenderingTarget( m_fieldMoveModel[ (u32)fieldMoveModelId ].GetCharaDrawInstance()->GetModelInstanceNode() );
    // ���[�N���烊�\�[�X�̏�Ԃ����ɖ߂�
    if( m_fieldMoveModel[ (u32)fieldMoveModelId ].IsSetupWork() )
    {
      m_fieldMoveModel[ (u32)fieldMoveModelId ].ReturnResourceStateFromWork();
    }
    return CREATE_SUCCESS;
  }
  return CREATE_ERROR;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ���샂�f���̍폜
*
*  @param  fieldMoveModelId  ���샂�f��ID
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::Delete(FIELD_MOVE_MODEL_ID fieldMoveModelId)
{
  if( m_fieldMoveModel[ (u32)fieldMoveModelId ].IsSetupResource() )
  {
    // ���샂�f����`��p�C�v���C������폜
    m_pMyRenderingPipeLine->RemoveEdgeRenderingTarget( m_fieldMoveModel[ (u32)fieldMoveModelId ].GetCharaDrawInstance()->GetModelInstanceNode() );
    // ���샂�f�����\�[�X��j��
    m_fieldMoveModel[ (u32)fieldMoveModelId ].ClearResource();
  }
  if( m_fieldMoveModel[ (u32)fieldMoveModelId ].IsSetupWork() )
  {
    // ���샂�f�����c���[����폜
    m_fieldMoveModelRoot.RemoveChild( &m_fieldMoveModel[ (u32)fieldMoveModelId ] );
    // ���샂�f�����[�N��j��
    m_fieldMoveModel[ (u32)fieldMoveModelId ].ClearWork();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTI�G�f�B�^�Ɏ��g�̃C���X�^���X��o�^
*
*  @param  �G�f�B�^�ɕ\�����������샂�f��ID
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::CreateRttiEditorPage(FIELD_MOVE_MODEL_ID fieldMoveModelId)
{
#if USE_RTTI_DEBUG
  if( fieldMoveModelId == FIELD_MOVE_MODEL_PLAYER ) { m_fieldMoveModel[ (u32)fieldMoveModelId ].CreateRttiEditorPage( L"�v���C���["); }
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTI�G�f�B�^���玩�g�̃C���X�^���X���폜
*
*  @param  �G�f�B�^����폜���������샂�f��ID
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::DeleteRttiEditorPage(FIELD_MOVE_MODEL_ID fieldMoveModelId)
{
#if USE_RTTI_DEBUG
  if( fieldMoveModelId == FIELD_MOVE_MODEL_PLAYER ) { m_fieldMoveModel[ (u32)fieldMoveModelId ].DeleteRttiEditorPage(); }
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  �L���ȑS���샂�f�����C�x���g���(�A�N�V�����R�}���h�A����R�[�h�����ɓ������Ȃ�)�ɂ���
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::OnEventStateAll(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_fieldMoveModel[i].IsSetup() )
    {
      m_fieldMoveModel[i].OnStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_EVENT );
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  �L���ȑS���샂�f�����C�x���g���(�A�N�V�����R�}���h�A����R�[�h�����ɓ������Ȃ�)�����������
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::OffEventStateAll(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_fieldMoveModel[i].IsSetup() )
    {
      m_fieldMoveModel[i].OffStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_EVENT );
    }
  }
}

}; //end of namespace MoveModel
}; //end of namespace Field
