#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModelManager.h
*  @brief  �t�B�[���h�p ���샂�f���}�l�[�W���[
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <heap/include/gfl2_Heap.h>
#include <renderingengine/include/SceneGraph/Instance/gfl2_InstanceNode.h>
#include "../../../../../../git_program/poke_3d_lib/model/include/gfl2_CharaModelFactory.h"
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModel.h"
#include "ViewerProc/include/FieldRo/FieldRootNode.h"

#include "ViewerProc/include/ReadViewerData.h"

namespace Field
{
  class MyRenderingPipeLine;
}

namespace Field{ namespace MoveModel {

class FieldMoveModelManager
{
public:

  enum CREATE_STATE
  {
    CREATE_SUCCESS,
    CREATE_ERROR,
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �R���X�g���N�^
  */
  //-----------------------------------------------------------------------------
  FieldMoveModelManager();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �f�X�g���N�^
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveModelManager();

  //----------------------------------------------------------------------------
  /**
  *  @brief  ������(���[�N)
  *
  *  @param  pInstanceRootNode    �c���[���`�����邽�߂̃��[�g�m�[�h
  *  @param  pHeap                ���[�N�̍쐬�ɗ��p�����q�[�v
  *
  *  @note   �����ŗ��p�����q�[�v�̃A�h���X�͓����ŕێ����Ă͂����܂���B�ǂ̃A�v�����֌W�Ȃ����������Ă��邽�߃Q�[���N�����̂ݗ��p��
  */
  //-----------------------------------------------------------------------------
  void InitializeWork(gfl2::renderingengine::scenegraph::instance::InstanceNode* pInstanceRootNode,
                      gfl2::heap::HeapBase*                                      pHeap);

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
  void InitializeResource(Field::MyRenderingPipeLine* pMyRenderingPipeLine,
                          gfl2::gfx::IGLAllocator*    pAllocator,
                          gfl2::heap::HeapBase*       pHeap,
                          gfl2::fs::AsyncFileManager* pFileManager );

  //----------------------------------------------------------------------------
  /**
  *  @brief  ���(���[�N)
  */
  //-----------------------------------------------------------------------------
  void TerminateWork(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  ���(���\�[�X)
  */
  //-----------------------------------------------------------------------------
  void TerminateResource(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �O�X�V(�R���W�����ȊO�̍��W/�p���̕ύX���s��)
  */
  //-----------------------------------------------------------------------------
  virtual void PrevUpdate(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �X�V(�R���W�����V�[���ւ̍��W/�p���̓o�^���s��)
  */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ��X�V(�R���W�����V�[������̌��ʂ�K�p���A�ŏI�I�ȍ��W/�p���̌���/�C�x���g�̎��s���s��)
  */
  //-----------------------------------------------------------------------------
  virtual void PostUpdate(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �g���Ă��Ȃ����샂�f���̗v�f��Ԃ� 
  *
  *  @return ���샂�f���̗v�f(FIELD_MOVE_MODEL_MAX�̏ꍇ�̓G���[)
  */
  //-----------------------------------------------------------------------------
  u32 GetFieldMoveModelIndexFromFreeSpace(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �C�x���gID(���ʗp�̃��j�[�N��ID)���瓮�샂�f���̗v�f��Ԃ� 
  *
  *  @param  eventId               �C�x���gID
  *  @return ���샂�f���̗v�f(FIELD_MOVE_MODEL_MAX�̏ꍇ�̓G���[)
  */
  //-----------------------------------------------------------------------------
  u32 GetFieldMoveModelIndexFromEventId(u32 eventId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ���샂�f���̃��[�N�쐬
  *
  *  @param  fieldMoveModelId       ���샂�f��ID
  *  @param  pFieldMoveModelHeader  ���샂�f�����N�G�X�g
  *  @return ����
  */
  //-----------------------------------------------------------------------------
  CREATE_STATE CreateWork(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeader* pFieldMoveModelHeader);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ���샂�f���̃��\�[�X�쐬
  *
  *  @param  fieldMoveModelId       ���샂�f��ID
  *  @param  pFieldMoveModelHeader  ���샂�f�����N�G�X�g
  *  @return ����
  */
  //-----------------------------------------------------------------------------
  CREATE_STATE CreateResource(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeader* pFieldMoveModelHeader);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ���샂�f���̍폜
  *
  *  @param  fieldMoveModelId  ���샂�f��ID
  */
  //-----------------------------------------------------------------------------
  void Delete(FIELD_MOVE_MODEL_ID fieldMoveModelId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTI�G�f�B�^�Ɏ��g�̃C���X�^���X��o�^
  *
  *  @param  �G�f�B�^�ɕ\�����������샂�f��ID
  */
  //-----------------------------------------------------------------------------
  void CreateRttiEditorPage(FIELD_MOVE_MODEL_ID fieldMoveModelId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTI�G�f�B�^���玩�g�̃C���X�^���X���폜
  *
  *  @param  �G�f�B�^����폜���������샂�f��ID
  */
  //-----------------------------------------------------------------------------
  void DeleteRttiEditorPage(FIELD_MOVE_MODEL_ID fieldMoveModelId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ���샂�f���̎擾
  *
  *  @param  fieldMoveModelId  ���샂�f��ID
  *  @return ���샂�f��
  */
  //-----------------------------------------------------------------------------
  FieldMoveModel* GetFieldMoveModel(FIELD_MOVE_MODEL_ID fieldMoveModelId){ return &m_fieldMoveModel[ static_cast<u32>(fieldMoveModelId) ]; }
  FieldMoveModel* GetFieldMoveModel(u32                 fieldMoveModelId){ return &m_fieldMoveModel[                  fieldMoveModelId  ]; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �L���ȑS���샂�f�����C�x���g���(�A�N�V�����R�}���h�A����R�[�h�����ɓ������Ȃ�)�ɂ���
  */
  //-----------------------------------------------------------------------------
  void OnEventStateAll(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �L���ȑS���샂�f�����C�x���g���(�A�N�V�����R�}���h�A����R�[�h�����ɓ������Ȃ�)�����������
  */
  //-----------------------------------------------------------------------------
  void OffEventStateAll(void);

  //=====================================================================================================================
  //! @brief poke_3d::model::CharaModelManager�̃��b�p�[����
  //=====================================================================================================================
#if 0
  //-----------------------------------------------------------------------------
  /**
  *  @brief  �w��L�����N�^�[���\�[�X�̔񓯊����[�h�J�n
  *
  *  @param  characterId       �L�����N�^�[ID
  */
  //-----------------------------------------------------------------------------
  void LoadAsync(u32 characterId){ m_charaModelFactory.LoadModelAsync(characterId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �����ւ��p�[�c���\�[�X�̔񓯊����[�h�J�n
  *
  *  @param  dressUpParam      �����ւ��p�����[�^
  */
  //-----------------------------------------------------------------------------
  void LoadDressUpPartsAsync(const poke_3d::model::DressUpParam& dressUpParam){ m_charaModelFactory.LoadDressUpPartsAsync(dressUpParam); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �w��L�����N�^�[���\�[�X���񓯊����[�h�����ǂ���
  *
  *  @param  characterId       �L�����N�^�[ID
  *  @return true�͐����Afalse�͎��s
  */
  //-----------------------------------------------------------------------------
  bool IsLoaded(u32 characterId){ return m_charaModelFactory.IsModelLoaded(characterId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �w�蒅���ւ��p�[�c���\�[�X���񓯊����[�h�����ǂ���
  *
  *  @param  dressUpParam      �����ւ��p�����[�^
  *  @return true�͐����Afalse�͎��s
  */
  //-----------------------------------------------------------------------------
  bool IsDressUpPartsLoaded(const poke_3d::model::DressUpParam& dressUpParam){ return m_charaModelFactory.IsDressUpPartsLoaded(dressUpParam); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �w��L�����N�^�[���\�[�X�̔j��
  *
  *  @param  characterId       �L�����N�^�[ID
  */
  //-----------------------------------------------------------------------------
  void Unload(u32 characterId){ m_charaModelFactory.UnloadModel(characterId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �w�蒅���ւ��p�[�c���\�[�X�̔j��
  *
  *  @param  dressUpParam      �����ւ��p�����[�^
  */
  //-----------------------------------------------------------------------------
  void UnloadDressUpParts(const poke_3d::model::DressUpParam& dressUpParam){ m_charaModelFactory.UnloadDressUpParts(dressUpParam); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �L�����N�^�[���\�[�X�̑S�j��
  */
  //-----------------------------------------------------------------------------
  void UnloadAll(void){ m_charaModelFactory.UnloadModelAll(); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �����ւ��p�[�c���\�[�X�̑S�j��
  */
  //-----------------------------------------------------------------------------
  void UnloadDressUpPartsAll(void){ m_charaModelFactory.UnloadDressUpPartsAll(); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �w��L�����N�^�[���\�[�X�m�[�h�̍쐬
  *
  *  @param  characterId       �L�����N�^�[ID
  *  @return ����
  */
  //-----------------------------------------------------------------------------
  bool SetupModel(u32 characterId){ return m_charaModelFactory.SetupModel( m_pAllocator , characterId ); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �w�蒅���ւ��p�[�c���\�[�X�m�[�h�̍쐬
  *
  *  @param  pDressUpParam     �����ւ��p�����[�^
  *  @return ����
  */
  //-----------------------------------------------------------------------------
  bool SetupDressUpParts(poke_3d::model::DressUpParam* pDressUpParam){ return m_charaModelFactory.SetupDressUpParts( m_pAllocator , *pDressUpParam ); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief ��풓�A�j���[�V�����p�b�N�̔񓯊��ǂݍ��݊J�n
  *
  *  @param dynamicAnimationId �ǂݍ��ޔ�풓�A�j���[�V����ID
  */
  //-----------------------------------------------------------------------------
  void LoadDynamicAnimationAsync( u32 dynamicAnimationId ){ m_charaModelFactory.LoadDynamicAnimationAsync(dynamicAnimationId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief ��풓�A�j���[�V�����p�b�N�̔񓯊��ǂݍ��݂̊����m�F
  *
  *  @param dynamicAnimationId �m�F�����풓�A�j���[�V����ID
  *  @return �ǂݍ��݊������Ă�����true, �܂��Ȃ�false
  */
  //-----------------------------------------------------------------------------
  bool IsDynamicAnimationLoaded( u32 dynamicAnimationId ){ return m_charaModelFactory.IsDynamicAnimationLoaded(dynamicAnimationId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief ��풓�A�j���[�V�����p�b�N�̃Z�b�g�A�b�v
  *
  *  @param dynamicAnimationId �Z�b�g�A�b�v�����풓�A�j���[�V����ID
  */
  //-----------------------------------------------------------------------------
  void SetupDynamicAnimation( u32 dynamicAnimationId ){ m_charaModelFactory.SetupDynamicAnimation(m_pAllocator, m_pHeap, dynamicAnimationId); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief ��풓�A�j���[�V�����p�b�N�̃A�����[�h
  *
  *  @param dynamicAnimationId �A�����[�h�����풓�A�j���[�V����ID
  */
  //-----------------------------------------------------------------------------
  void UnloadDynamicAnimation( u32 dynamicAnimationId ){ m_charaModelFactory.UnloadDynamicAnimation(dynamicAnimationId); }
#endif //@saito_del
private:
  
  //=====================================================================================================================
  //! @brief ����
  //=====================================================================================================================

  // �V�[���O���t�p
  gfl2::renderingengine::scenegraph::instance::InstanceNode* m_pInstanceRootNode;
  FieldMoveModel                    m_fieldMoveModelRoot;
  FieldMoveModel                    m_fieldMoveModel[FIELD_MOVE_MODEL_MAX];
  Field::MyRenderingPipeLine       *m_pMyRenderingPipeLine;

  // �L�����N�^�[�`��p�C���X�^���X�̃t�@�N�g���[�p
  gfl2::gfx::IGLAllocator          *m_pAllocator;
  gfl2::heap::HeapBase             *m_pHeap;
  gfl2::fs::AsyncFileManager       *m_pFileManager;
  ///poke_3d::model::CharaModelFactory m_charaModelFactory;

	GLResourceAllocator m_ResourceAllocator;

};

}; //end of namespace MoveModel
}; //end of namespace Field
