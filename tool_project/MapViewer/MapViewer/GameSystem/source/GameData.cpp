//===================================================================
/**
 * @file    GameData.cpp
 * @brief   �Q�[���i�s�f�[�^�Ǘ�
 * @author  Miyuki Iwasawa 
 * @date    2015.02.28
*/
//===================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <System/include/GflvUse.h>
#include "GameSystem/include/GameData.h"

// �L�����N�^�[�V�X�e��
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModel_define.h"
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModelManager.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCodeManager.h"
///#include "ViewerProc/include/FieldRo/ActionCommand/FieldActionCommandManager.h"

#include "ViewerProc/include/FieldRo/FieldRootNode.h"

GFL_NAMESPACE_BEGIN(GameSystem)

//============================================================================================
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief �Z�[�u�f�[�^�ɐ�������t�@�C����
 * //@note  kujira�ł̓��A���n��|�P�������A�ʏ�i�s�f�[�^��2��
 * @note  2013.03.20  �ʏ�i�s�f�[�^��1���݂̂ɏC��
 */
//--------------------------------------------------------------
enum {
  //SAVE_FILES_MAX = 1 + 1,
  SAVE_FILES_MAX = 1,
};

//============================================================================================
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief   �R���X�g���N�^
 */
//--------------------------------------------------------------
GameData::GameData( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* deviceHeap, gfl2::ui::DeviceManager* uiDeviceManager )
{
  // �A���P�[�^�쐬
  m_pHeapAllocator       = GFL_NEW(heap) gfl2::util::GLHeapAllocator(heap);
  m_pDeviceHeapAllocator = GFL_NEW(deviceHeap) gfl2::util::GLHeapAllocator(deviceHeap);

  // ���샂�f���p
  m_pFieldRootNode            = GFL_NEW(deviceHeap) Field::FieldRootNode();
  m_pFieldCharaModelManager   = GFL_NEW(deviceHeap) Field::MoveModel::FieldMoveModelManager();
  m_pFieldMoveCodeManager     = GFL_NEW(deviceHeap) Field::MoveModel::FieldMoveCodeManager();
  ///m_pFieldActionComandManager = GFL_NEW(deviceHeap) Field::MoveModel::FieldActionCommandManager();

  // ���샂�f���}�l�[�W���[�̃��\�[�X���ȊO��������
  // *�⑫* ���샂�f���}�l�[�W���[�̓t�B�[���h�v���b�N���Ń������𓮓I�Ɋm�ۂ���ӏ�������̂ŁA�t�B�[���h�v���b�N�p�q�[�v�����蓖�Ăĕʓr����������K�v������܂�
  m_pFieldRootNode->Initialize( m_pDeviceHeapAllocator );
	m_pFieldCharaModelManager->InitializeWork(m_pFieldRootNode->GetInstanceRootNode(),deviceHeap);
  m_pFieldMoveCodeManager->Initialize(deviceHeap);
  ///m_pFieldActionComandManager->Initialize(deviceHeap);

}


//--------------------------------------------------------------
/**
 * @brief   �f�X�g���N�^
 */
//--------------------------------------------------------------
GameData::~GameData()
{
  // ���샂�f���p
  GFL_SAFE_DELETE(m_pFieldCharaModelManager);
  GFL_SAFE_DELETE(m_pFieldMoveCodeManager);
  ///GFL_SAFE_DELETE(m_pFieldActionComandManager);
  GFL_SAFE_DELETE(m_pFieldRootNode);

  // �A���P�[�^�폜
  GFL_SAFE_DELETE(m_pHeapAllocator);
  GFL_SAFE_DELETE(m_pDeviceHeapAllocator);
}



//============================================================================================
//
//
//      ����������
//
//
//============================================================================================
//----------------------------------------------------------------------------
  /**
   * @brief �u�������傩��v�Q�[���J�n�p�f�[�^�ݒ�
   */
//----------------------------------------------------------------------------
void GameData::SetFirstData( gfl2::heap::HeapBase * pTempHeap, gfl2::heap::HeapBase * pTempDevHeap )
{
  GFL_PRINT("GameData::SetFirstData\n");
}

#if PM_DEBUG
//----------------------------------------------------------------------------
  /**
   * @brief �f�o�b�O�J�n�p�f�[�^�ݒ�
   */
//----------------------------------------------------------------------------
void GameData::SetDebugData( DebugDataType type, gfl2::heap::HeapBase * pTempHeap, gfl2::heap::HeapBase * pTempDevHeap, int sex )
{
  GFL_PRINT("GameData::SetDebugData\n");

  gfl2::heap::HeapBase * heap = pTempHeap;
  //�f�o�b�O�J�n���͈�U�Z�[�u�f�[�^������������
  this->ClearSaveData( heap );
}

//----------------------------------------------------------------------------
  /**
   * @brief �f�o�b�O�o�g���J�n�p�f�[�^�ݒ�
   */
//----------------------------------------------------------------------------
void GameData::SetDebugBattleData( gfl2::heap::HeapBase * pTempHeap )
{
  GFL_PRINT("GameData::SetDebugBattleData\n");

  gfl2::heap::HeapBase * heap = pTempHeap;
  //�f�o�b�O�J�n���͈�U�Z�[�u�f�[�^������������
  this->ClearSaveData( heap );
}
#endif //PM_DEBUG

//============================================================================================
//
//
//    �A�N�Z�X�֐�
//
//
//============================================================================================

GFL_NAMESPACE_END(GameSystem)


