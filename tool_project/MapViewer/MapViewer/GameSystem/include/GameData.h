///#if !defined( __GAMEDATA_H__ )
///#define __GAMEDATA_H__
//===================================================================
/**
 * @file    GameData.h
 * @brief   �Q�[���i�s�f�[�^�Ǘ�
 * @author  k.ohno -> Miyuki Iwasawa
 * @date    15.2.28
 * @note
 * ��{�I�ɃQ�[���i�s�f�[�^�̓Z�[�u�f�[�^�Ƃ��đ��݂��邪�A
 * �ʐM���̕����v���C���[����t�B�[���h�}�b�v���ȂǃZ�[�u�f�[�^��
 * �̂�Ƃ͌���Ȃ���������B
 * ���̂悤�ȃZ�[�u�f�[�^�E��Z�[�u�f�[�^�֕���ɃA�N�Z�X����C���^�[�t�F�C�X��
 * �e�p�[�g���Ƃɍ\������ƃp�[�g���ƂɃA�N�Z�X��@���������A�܂������@�\��
 * ���̂��������݂���Ȃǃv���O�����S�̂̕��G���������Ă��܂��\���������B
 * ���������邽�߁A���ʃC���^�[�t�F�C�X����邱�Ƃɂ���Ċȗ�������B
 */
//===================================================================
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <ui/include/gfl2_Ui.h>
//#include <pml_PokeParty.h>

#include "System/include/PokemonVersion.h"

//===================================================================
//
//
//  �O���Q�Ɛ錾
//
//
//  GameData/SaveData�̓Q�[���i�s�f�[�^�̊e�N���X�ւ̃A�N�Z�X��
//  �񋟂��邾���Ȃ̂ŁA�N���X�ڍׂ�m��K�v�͂Ȃ��B
//  �J�v�Z������i�߂邽�߂ɂ͂��ꂼ��̗��p�ӏ��ŁA
//  �Y���N���X�̃w�b�_���C���N���[�h���ė��p����B
//
//===================================================================

namespace gfl2 {
  namespace util {
    class GLHeapAllocator;
  }
}

namespace Field{

  namespace MoveModel{
    class FieldMoveModelManager;
    class FieldMoveCodeManager;
    ///class FieldActionCommandManager;
  }

  class FieldRootNode;
}


GFL_NAMESPACE_BEGIN(GameSystem)

//--------------------------------------------------------------
/**
 * @class GameData
 */
//--------------------------------------------------------------
class GameData
{
  GFL_FORBID_COPY_AND_ASSIGN(GameData); //�R�s�[�R���X�g���N�^�{����֎~

private:

  gfl2::util::GLHeapAllocator* m_pHeapAllocator;        ///< �q�[�v�A���P�[�^
  gfl2::util::GLHeapAllocator* m_pDeviceHeapAllocator;  ///< �f�o�C�X�q�[�v�A���P�[�^

  gfl2::heap::HeapBase * mTempHeap;

  // ���샂�f���p
  Field::MoveModel::FieldMoveModelManager*     m_pFieldCharaModelManager;
  Field::MoveModel::FieldMoveCodeManager*      m_pFieldMoveCodeManager;
  ///Field::MoveModel::FieldActionCommandManager* m_pFieldActionComandManager;
  Field::FieldRootNode*                        m_pFieldRootNode;


public:
  //�R���X�g���N�^
  GameData(gfl2::heap::HeapBase* systemHeap, gfl2::heap::HeapBase* deviceHeap, gfl2::ui::DeviceManager* uiDeviceManager );
  //�f�X�g���N�^
  ~GameData();

  ///�u�������傩��v�Q�[���J�n�p�f�[�^�ݒ�
  void SetFirstData( gfl2::heap::HeapBase * pTempHeap, gfl2::heap::HeapBase * pTempDevHeap );

#if PM_DEBUG
  ///�f�o�b�O�J�n�p�f�[�^�ݒ�
  enum DebugDataType
  {
    DEBUG_DATA_TYPE_CTRNAME,  ///<�v���C���[����CTR�{�̖����g��
    DEBUG_DATA_TYPE_DEFAULTNAME,  ///<�G�����f�t�H���g�����g��
  };
  void SetDebugData( DebugDataType type, gfl2::heap::HeapBase* pTempHeap, gfl2::heap::HeapBase* pTempDevHeap, int sex = PM_FEMALE );
  ///�o�g���f�o�b�O�p�f�[�^�ݒ�i�S�A�C�e����100�������܂��j
  void SetDebugBattleData( gfl2::heap::HeapBase * pTempHeap );
#endif

  //============================================================================================
  //    �Z�[�u����
  //============================================================================================

  //============================================================================================
  //  ���샂�f���p�A�N�Z�T
  //============================================================================================

  //-----------------------------------------------------------------------------
  /**
   * @brief ���샂�f���}�l�[�W���[���擾
   */
  //-----------------------------------------------------------------------------
  inline Field::MoveModel::FieldMoveModelManager* GetFieldCharaModelManager( void ) const { return m_pFieldCharaModelManager; }

  //-----------------------------------------------------------------------------
  /**
   * @brief ����R�[�h�}�l�[�W���[���擾
   */
  //-----------------------------------------------------------------------------
  inline Field::MoveModel::FieldMoveCodeManager* GetFieldMoveCodeManager( void ) const { return m_pFieldMoveCodeManager; }

  //-----------------------------------------------------------------------------
  /**
   * @brief �A�N�V�����R�}���h�}�l�[�W���[���擾
   */
  //-----------------------------------------------------------------------------
  ///inline Field::MoveModel::FieldActionCommandManager* GetFieldActionComandManager( void ) const { return m_pFieldActionComandManager; }

  //-----------------------------------------------------------------------------
  /**
   * @brief �t�B�[���h�p�m�[�h���[�g���擾
   */
  //-----------------------------------------------------------------------------
  inline Field::FieldRootNode* GetFieldRootNode( void ) const { return m_pFieldRootNode; }

private:

};  //GameData

GFL_NAMESPACE_END(GameSystem)



///#endif //__GAMEDATA_H__
