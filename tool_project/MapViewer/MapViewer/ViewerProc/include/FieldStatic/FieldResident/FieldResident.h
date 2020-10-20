//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldResident.h
 *	@brief  �t�B�[���hresident
 *	@author	tomoya takahashi
 *	@date		2015.05.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_RESIDENT_H__ )
#define __FIELD_RESIDENT_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <ro/include/gfl2_RoManager.h>



//----------------------------------------------------------------------------
///�N���X�̑O���錾
//----------------------------------------------------------------------------
// �R���W�����V�X�e��
class BaseCollisionScene;

namespace System{
  class nijiAllocator;
}
/**
namespace Field { namespace StaticModel {
  class StaticModelManager;
}}
*/
GFL_NAMESPACE_BEGIN(Field);

#include "niji_conv_header/field/field_resident/field_resident_pack.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�O���錾
*/
//-----------------------------------------------------------------------------
///class PlacementDataManager;

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

class FieldResident
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( FieldResident );

public:
  enum CollisionSceneType
  {
    COLLISION_SCENE_TERRAIN,      /// �n�`
    COLLISION_SCENE_STATIC_MODEL, /// �z�u����Ă��郂�f��
    COLLISION_SCENE_EVENT_POS,    /// POS�C�x���g
    COLLISION_SCENE_EVENT_TALK,   /// ��b�C�x���g
    COLLISION_SCENE_EVENT_PUSH,   /// �������݃C�x���g
    COLLISION_SCENE_ENCOUNT,      /// �G���J�E���g
    COLLISION_SCENE_MAX,          /// 
  };

public:

  FieldResident();
  ~FieldResident();

  // �������ς݃`�F�b�N
  inline bool IsInitialized() const { return m_IsInitialized; }

  // ������
  void Initialize( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::fs::AsyncFileManager* p_fileManager);

  // �����������҂�
  bool InitializeWait();
  
  // �j��
  void Terminate( void );

  // �q�[�v�擾
  inline gfl2::heap::HeapBase* GetFieldHeap(void) const { return m_pFieldHeap; }   ///< �t�B�[���h�q�[�v
  inline gfl2::heap::HeapBase* GetFieldDeviceHeap(void) const { return m_pFieldDeviceHeap; } ///< �t�B�[���h�f�o�C�X�q�[�v
  inline System::nijiAllocator* GetFieldDeviceHeapAllocator(void) const { return m_pFieldDeviceHeapAllocator; } ///< �t�B�[���h�f�o�C�X�q�[�v�A���P�[�^�[

  // �q�[�v�擾(����ʃ��j���[�p)
  // @note ���L�q�[�v�����̍\���̓��j���[�ǂ̊Ǌ��ɂȂ�܂�
  inline gfl2::heap::HeapBase* GetFieldSubScreenHeap(void) const { return m_pFieldSubScreenHeap; }   ///< �t�B�[���h����ʃ��j���[�p�q�[�v
  inline gfl2::heap::HeapBase* GetFieldSubScreenDeviceHeap(void) const { return m_pFieldSubScreenDeviceHeap; } ///< �t�B�[���h����ʃ��j���[�p�f�o�C�X�q�[�v
  inline System::nijiAllocator* GetFieldSubScreenDeviceHeapAllocator(void) const { return m_pFieldSubScreenDeviceHeapAllocator; } ///< �t�B�[���h����ʃ��j���[�p�f�o�C�X�q�[�v�A���P�[�^�[

  // �o�C�i���擾
  inline void * GetResource( u32 idx ) { return m_BinAccessor.GetData(idx); }
  inline u32 GetResourceSize( u32 idx ) { return m_BinAccessor.GetDataSize(idx); }
  
  // �R���W�����V�[���̎擾
  inline BaseCollisionScene* GetCollisionScene( CollisionSceneType type ){ return m_pCollisionScene[type]; }

  // �z�u�f�[�^�}�l�[�W���擾
  ///inline PlacementDataManager* GetPlacementDataManager() { return m_pPlacementDataManager; }

  // �z�u���f���Ǘ��N���X�̎擾
  ///inline StaticModel::StaticModelManager* GetStaticModelManager() { return m_pStaticModelManager; }

private:

  //! �q�[�v�����E�j��
  void InitializeHeap( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap );
  void TerminateHeap( void );

  //! �풓���\�[�X�ǂݍ��݁E�j��
  ///void InitializeResource();
  ///bool WaitInitializeResource();
  ///void TerminateResouce();

  //! DLL
  ///void InitializeFieldDLL();
  ///void TerminateFieldDLL();

  //! �R���W�����V�[������
  void InitializeCollisionScene();
  void TeminateCollisionScene();
	/**
  //! �z�u�f�[�^�}�l�[�W���[
  void InitializePlacementDataManager();
  void TerminatePlacementDataManager();

  //! �z�u���f�������E�j��
  void InitializeStaticModelManager();
  void TerminateStaticModelManager();
	*/
private:

  bool m_IsInitialized;

  u32 m_InitializeSeq;

  gfl2::heap::HeapBase *  m_pFieldHeap;       //! �t�B�[���h�q�[�v
  gfl2::heap::HeapBase *  m_pFieldDeviceHeap; //! �t�B�[���h�f�o�C�X�q�[�v
  System::nijiAllocator *   m_pFieldDeviceHeapAllocator;  //! �t�B�[���h�f�o�C�X�q�[�v�A���P�[�^�[

  // @note ���L�q�[�v�����̍\���̓��j���[�ǂ̊Ǌ��ɂȂ�܂�
  gfl2::heap::HeapBase*  m_pFieldSubScreenHeap;                 //! �t�B�[���h����ʃ��j���[�p�q�[�v
  gfl2::heap::HeapBase*  m_pFieldSubScreenDeviceHeap;           //! �t�B�[���h����ʃ��j���[�p�f�o�C�X�q�[�v
  System::nijiAllocator* m_pFieldSubScreenDeviceHeapAllocator;  //! �t�B�[���h����ʃ��j���[�p�f�o�C�X�q�[�v�A���P�[�^�[


  // DLL
  gfl2::ro::Module* m_pRoModule;  

  gfl2::fs::AsyncFileManager * m_pAsyncFileMan; //! �t�@�C���ǂݍ��݃}�l�[�W��

  // �풓���\�[�X
  void * m_pBuffer;
  gfl2::fs::BinLinkerAccessor m_BinAccessor;

  // �R���W�����V�[��
  BaseCollisionScene* m_pCollisionScene[COLLISION_SCENE_MAX];

  // PlacementDataManager
  ///PlacementDataManager* m_pPlacementDataManager;

  // �z�u���f��
  ///StaticModel::StaticModelManager*  m_pStaticModelManager;
};

GFL_NAMESPACE_END(Field);

#endif	// __FIELD_RESIDENT_H__

