//=============================================================================
/**
 * File:   gfl_RoManager.h
 * Author: Hisanao Suzuki
 *
 * Created on 2011/11/18
 * ���I���W���[���Ǘ��N���X
 **/
//=============================================================================


#ifndef __GFL_RO_MANAGER_H__
#define __GFL_RO_MANAGER_H__

#pragma once

#include <nn.h>
#include <nn/ro.h>
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <base/gfl_Singleton.h>


// =========================================================================
// RO�L���t���O(����OFF)
// =========================================================================
#ifdef GFL_USE_RO
  #define GFL_ENABLE_RO 1

  // �V���{���𖾎��I�Ɍ��J
  #define GFL_DLL_EXPORT NN_DLL_EXPORT

  // �V���{���ւ̎Q�Ƃ𖾎��I�ɐ���
  #define GFL_DLL_IMPORT NN_DLL_IMPORT
#else
  #define GFL_ENABLE_RO 0
  #define GFL_DLL_EXPORT
  #define GFL_DLL_IMPORT
#endif



#if GFL_ENABLE_RO

namespace gfl{

/**
* @namespace gfl::ro
* @brief DLL�p�l�[���X�y�[�X
*/
namespace ro{



// =========================================================================
// DEFINE
// =========================================================================
#define DEFAULT_MODULE_MAX  32              // �f�t�H���g�o�^�\���W���[����


// =========================================================================
//! @brief  ���I���W���[���Ǘ��N���X
// =========================================================================
class RoManager : public gfl::base::Singleton<RoManager>
{
  GFL_FORBID_COPY_AND_ASSIGN(RoManager);

private:
  void*                     m_crsBuffer;      //!< CRS�t�@�C���p�o�b�t�@
  void*                     m_crrBuffer;      //!< CRR�t�@�C���p�o�b�t�@
  nn::ro::RegistrationList* m_registList;     //!< �o�^���W���[���ꗗ
  nn::ro::Module**          m_modules;        //!< ���W���[�����X�g
  bit8**                    m_bssBuffer;      //!< .data/.bss�o�b�t�@
  void**                    m_moduleBuffer;   //!< ���W���[���p�o�b�t�@
  gfl::heap::HeapBase*      m_heapForModules; //!< ���W���[�������p�q�[�v
  u32                       m_moduleMax;      //!< �o�^�\�ȃ��W���[����
  u32                       m_registNum;      //!< �o�^���W���[����
  bool                      m_isInit;         //!< �����t���O

private:
  //---------------------------------------------------------------------
  /**
  * @brief          ���I���W���[���̉��
  *
  * @param[in,out]  targetModule ����ΏۂƂȂ铮�I���W���[��
  *
  * @return �o�^���X�g�̃C���f�b�N�X�l�B0��菬�����l�̏ꍇ�͖����o�B
  */
  // ---------------------------------------------------------------------
  s32 GetModuleIndex( nn::ro::Module* targetModule );

  //---------------------------------------------------------------------
  /**
  * @brief  �t�@�C���ǂݍ��ݏ���
  *
  * @param[in]  heap        �q�[�v�Ǘ��N���X�ւ̃|�C���^
  * @param[in]  fileName    �ǂݍ��݂����t�@�C����
  * @param[in]  size        �t�@�C���T�C�Y���i�[�����̃|�C���^
  *
  * @return �ǂݍ��܂ꂽ�t�@�C�����e���i�[�����o�b�t�@�ւ̃|�C���^
  */
  //---------------------------------------------------------------------
  void* LoadFile( gfl::heap::HeapBase* heap, const char* fileName, size_t *size );

public:

  //---------------------------------------------------------------------
  /**
  * @brief  �R���X�g���N�^
  *
  * @param[in]  modulesHeap DLL���W���[���������Ɏg�p�����q�[�v�G���A
  */
  // ---------------------------------------------------------------------
  RoManager( gfl::heap::HeapBase* modulesHeap );

  //---------------------------------------------------------------------
  /**
  * @brief  �f�X�g���N�^
  */
  // ---------------------------------------------------------------------
  virtual ~RoManager( void );

public:
  //---------------------------------------------------------------------
  /**
  * @brief  ����������
  *
  * @param[in]  heap        �q�[�v�Ǘ��N���X�ւ̃|�C���^
  * @param[in]  crsFileName CRS�t�@�C����
  * @param[in]  crrFileName CRR�t�@�C����
  * @param[in]  moduleMax   �o�^�\���W���[����
  */
  // ---------------------------------------------------------------------
  void Initialize( gfl::heap::HeapBase* heap, const char* crsFileName, const char* crrFileName, u32 moduleMax=DEFAULT_MODULE_MAX );

  //---------------------------------------------------------------------
  /**
  * @brief  ���I���W���[���̃��[�h
  *
  * @param[in]  crsFileName CRS�t�@�C����
  * @param[in]  crrFileName CRR�t�@�C����
  * @param[in]  fixLevel    ���W���[��������̃������̉���i�K�B
  *
  * @par   fixLevel�ɂ���
  *         - �ڂ����́A�ȉ���API���t�@�����X���Q�Ƃ��Ă��������B
  *         - [/CTR_SDK/documents/api/nn/ro/FixLevel.html]
  */
  // ---------------------------------------------------------------------
  nn::ro::Module* LoadModule( const char* croFileName, nn::ro::FixLevel fixLevel=nn::ro::FIX_LEVEL_1 );
  nn::ro::Module* LoadModuleLower( const char* croFileName, nn::ro::FixLevel fixLevel=nn::ro::FIX_LEVEL_1 );

  //---------------------------------------------------------------------
  /**
  * @brief  ���I���W���[�����J�n����
  *
  * @param[in]  targetModule �J�n�ΏۂƂȂ铮�I���W���[��
  * @param[in]  isLinkCheck  �V���{�������N����������Ă��邩�������Ƀ`�F�b�N����Ȃ�true���w��
  *
  * @attention ���s�Ɏ��s��������ł���ꍇ��ASSERT�Œ�~������
  */
  // ---------------------------------------------------------------------
  void StartModule( nn::ro::Module* targetModule, bool isLinkCheck=false );

  //---------------------------------------------------------------------
  /**
  * @brief  ���I���W���[�����������
  *
  * @param[in]  targetModule ����ΏۂƂȂ铮�I���W���[��
  */
  // ---------------------------------------------------------------------
  void DisposeModule( nn::ro::Module* targetModule );

  //---------------------------------------------------------------------
  /**
  * @brief  �o�^����Ă���S�Ă̓��I���W���[���̉��
  */
  // ---------------------------------------------------------------------
  void DisposeAllModule( void );

  //---------------------------------------------------------------------
  /**
  * @brief  �I������
  */
  // ---------------------------------------------------------------------
  void Finalize( void );



#if GFL_DEBUG
  //---------------------------------------------------------------------
  /**
  * @brief  �f�o�b�O�����F���ݓǂݍ��ݒ���DLL���X�g���擾����
  *
  * @param[out]  registNum  �o�^�����i�[���ĕԂ�
  *
  * @return nn::ro::Module�̃|�C���^�̔z��
  */
  // ---------------------------------------------------------------------
  nn::ro::Module**  DebugGetModules( u32 *registNum ){
    *registNum = m_registNum;
    return m_modules;
  };
#endif

};



//---------------------------------------------------------------------
/**
* @brief  ���I���W���[���̃��[�h�i�֐��Łj
*
* @param[in]  croFileName CRO�t�@�C����
* @param[in]  is_lower    �q�[�v�̉�����Ƃ�
*
* @return ���I���W���[���ւ̃|�C���^
*/
// ---------------------------------------------------------------------
extern nn::ro::Module* StartModuleFunc( const char* croFileName, bool is_lower = false );



} // namespace ro
} // namespace gfl


//---------------------------------------------------------------------
/**
* @brief  SINGLETON�̑O���Q�Ɛ錾
*/
// ---------------------------------------------------------------------
SINGLETON_INSTANCE_DECLARATION( gfl::ro::RoManager );


#endif  // GFL_ENABLE_RO

#endif  // __GFL_RO_MANAGER_H__

