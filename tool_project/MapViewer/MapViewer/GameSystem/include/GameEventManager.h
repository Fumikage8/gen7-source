#ifndef __GAMEEVENTMANAGER_H__
#define __GAMEEVENTMANAGER_H__
//===================================================================
/**
 * @file    GameEventManager.h
 * @brief   �C�x���g�̐���N���X
 * @author  k.ohno
 * @date    2011.3.6   Syachi����ڐA
 */
//===================================================================

#pragma once

#include <base/include/gfl2_Singleton.h>


namespace GameSystem {

  class GameEvent;
  class GameManager;
}

//!PROC�̌Ăяo��   GFLIB_PROC_CALL<ManagerClass��, CallClass��>(ManagerClass*)
template <class ManagerClass , class CallClass> CallClass* GAMESYSTEM_EVENT_CALL(ManagerClass* pManager)
{
#if PM_DEBUG
  pManager->SetChkHeapData(); // �풓�q�[�v�Ď�
#endif // PM_DEBUG
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pManager->GetHeap() );
  pManager->CallEvent( pCC );
  return pCC;
}

//!PROC�̐؂�ւ��i���ʃR�[���j   GAMESYSTEM_EVENT_CHANGE<ManagerClass��, CallClass��>(ManagerClass*)
template <typename ManagerClass , typename CallClass> CallClass* GAMESYSTEM_EVENT_CHANGE(ManagerClass* pManager )
{
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pManager->GetHeap() );
  pManager->ChangeEvent( pCC );
  return pCC;
}

//! DLL�g�p�@�R�[��
template <class ManagerClass , class CallClass> CallClass* GAMESYSTEM_EVENT_RO_CALL(ManagerClass* pManager, const char * croFileName )
{
#if PM_DEBUG
  pManager->SetChkHeapData(); // �풓�q�[�v�Ď�
#endif // PM_DEBUG
  nn::ro::Module* pModule = gfl::ro::StartModuleFunc( croFileName );
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pManager->GetHeap() );
  pCC->SetRoModule( pModule );
  pManager->CallEvent( pCC );
  return pCC;
}

//!PROC�̐؂�ւ��i���ʃR�[���j   GAMESYSTEM_EVENT_CHANGE<ManagerClass��, CallClass��>(ManagerClass*)
template <typename ManagerClass , typename CallClass> CallClass* GAMESYSTEM_EVENT_RO_CHANGE(ManagerClass* pManager, const char * croFileName )
{
  nn::ro::Module* pModule = gfl::ro::StartModuleFunc( croFileName );
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pManager->GetHeap() );
  pCC->SetRoModule( pModule );
  pManager->ChangeEvent( pCC );
  return pCC;
}

#if PM_DEBUG
//! DLL�g�p�@�R�[�� DLL���q�[�v�̉�����Ƃ�
template <class ManagerClass , class CallClass> CallClass* GAMESYSTEM_EVENT_RO_LOWER_CALL(ManagerClass* pManager, const char * croFileName )
{
#if PM_DEBUG
  pManager->SetChkHeapData(); // �풓�q�[�v�Ď�
#endif // PM_DEBUG
  nn::ro::Module* pModule = gfl::ro::StartModuleFunc( croFileName, true );
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pManager->GetHeap() );
  pCC->SetRoModule( pModule );
  pManager->CallEvent( pCC );
  return pCC;
}
#endif // PM_DEBUG

namespace GameSystem {
  
  //-----------------------------------------------------------------------------
  /**
   * @brief	�C�x���g����֐��̖߂�l��`
   */
  //-----------------------------------------------------------------------------
  enum GMEVENT_RESULT{
    GMEVENT_RES_CONTINUE = 0, ///<�C�x���g�p����
    GMEVENT_RES_FINISH,       ///<�C�x���g�I��
    GMEVENT_RES_OFF,       ///<�C�x���g�I����
    GMEVENT_RES_CONTINUE_DIRECT = 33,  ///<�C�x���g�p���i�����������j
  };

  class GameEventManager
  {
    GFL_FORBID_COPY_AND_ASSIGN(GameEventManager); //�R�s�[�R���X�g���N�^�{����֎~

    friend class GameManager; // isExists ���g������

    private:
      GameEvent* mpEvent;     ///< ���s���̃C�x���g
      GameManager* mpGameMgr;	///<�Q�[���̑S�̊Ǘ��N���X�̃|�C���^

      //! �`�F���W�C�x���g
      GameEvent* mpChangeEvent;

      bool mDoEvent;  ///< �C�x���g���s�����t���[������true

#if PM_DEBUG
    //! �풓�q�[�v�̊Ď�
      enum{
      CHK_SYSTEM,           ///< �V�X�e���q�[�v
      CHK_RESIDENT,         ///< Resident�q�[�v
      CHK_RESIDENT_DEVICE,  ///< Resident�f�o�C�X�q�[�v
      CHK_APP,              ///< �A�v���P�[�V�����q�[�v
      CHK_APP_DEVICE,       ///< �A�v���P�[�V�����f�o�C�X�q�[�v 
      CHK_EVENT,            ///< �C�x���g�q�[�v
      CHK_EVENT_DEVICE,     ///< �C�x���g�f�o�C�X�q�[�v
      CHK_APP_CONTROL,      ///< �A�v���P�[�V�����R���g���[���q�[�v

      CHK_HEAP_MAX,
      };
      //! �q�[�v�`�F�b�N�f�[�^
      struct ChkHeapData
      {
        u32 free;         ///< �󂫗�
        u32 allocatable;  ///< �ő�m�ۗ�
      };
      static const u32  m_scChkHeapTbl[CHK_HEAP_MAX];     ///< �q�[�v�`�F�b�N�e�[�u��
      static const char*m_scChkHeapNameTbl[CHK_HEAP_MAX]; ///< �q�[�v�`�F�b�N���O�e�[�u��
      ChkHeapData       m_ChkHeapData[CHK_HEAP_MAX];      ///< �q�[�v�`�F�b�N���e�[�u��
      b32               m_ChkHeapDataFlag;  ///< �`�F�b�N�t���O
      b32               m_ChkHeapDataError; ///< �G���[���
      u32               m_ChkHeapDataErrorHeapID; ///< �G���[���N�����q�[�vID
      u8                m_ChkHeapDataId; ///< �`�F�b�NID�̃J�E���^�[

      void ChkHeapDataFunc( void ); ///< �ێ��q�[�v���ƍ��̃q�[�v�����r
#endif

      //------------------------------------------------------------------
      /**
       * @brief	�C�x���g�ؑ�
       *
       * @param	event	  	  ���̃C�x���g���䃏�[�N�ւ̃|�C���^
       */
      //------------------------------------------------------------------
      void Change(GameEvent * event);
    
      //------------------------------------------------------------------
      /**
       * @brief	�C�x���g�N�����`�F�b�N
       * @retval	true	�C�x���g�N����
       * @retval	false	�C�x���g�Ȃ�
       */
      //------------------------------------------------------------------
      bool isExists(void) const;

    public:

      //�R���X�g���N�^
      GameEventManager(GameManager* gmgr);
      // �f�X�g���N�^
      ~GameEventManager();

      //------------------------------------------------------------------
      /**
       * @brief	�C�x���g�ؑ�
       * @param	event		���̃C�x���g���䃏�[�N�ւ̃|�C���^
       */
      //------------------------------------------------------------------
      void ChangeEvent(GameEvent * next_event);

      //------------------------------------------------------------------
      /**
       * @brief	�C�x���g�Ăяo��
       * @param	event		�q�i�Ăяo���j�C�x���g���䃏�[�N�ւ̃|�C���^
       */
      //------------------------------------------------------------------
      void CallEvent(GameEvent * child);
    
      //------------------------------------------------------------------
      /**
       * @brief	�C�x���g�N�����`�F�b�N ���̃t���[��1�x�ł��C�x���g���N�����Ă�����TRUE
       * @retval	true	�C�x���g�N����
       * @retval	false	�C�x���g�Ȃ�
       */
      //------------------------------------------------------------------
      bool IsExists(void) const;

      //------------------------------------------------------------------
      /**
       * @brief	  �C�x���g���䃁�C��
       * @retval	GMEVENT_RESULT
       */
      //------------------------------------------------------------------
      GMEVENT_RESULT Main(void);

      //----------------------------------------------------------------------------
      /**
       *	@brief  �C�x���g�p�̃q�[�v�̎擾
       */
      //-----------------------------------------------------------------------------
      gfl2::heap::HeapBase* GetHeap(void);

#if PM_DEBUG
      /**
       * @brief �C�x���g���X�g�̕`��
       */
      ///void DEBUG_EventListDraw( gfl::grp::GraphicsSystem *grpSys ); todo ni�Fji���Ή�
#endif // PM_DEBUG


#if PM_DEBUG
      void SetChkHeapData( void );    ///< ���̃q�[�v����ێ�
      inline void SetDebugChkHeapData( bool flag ){ m_ChkHeapDataFlag = flag; } ///< �q�[�v�`�F�b�N�̗L�������ݒ�
      inline b32 IsDebugChkHeapDataError( void ) const{ return m_ChkHeapDataError; }  ///< �풓�q�[�v�@leak���������Ă��邩�H
      inline u32 IsDebugChkHeapErrorHeapID( void ) const { return m_ChkHeapDataErrorHeapID; } ///< �������������Ō��HEAPID
#endif // PM_DEBUG

  };

} //namespace GameSys


#endif //__GAMEEVENTMANAGER_H__

