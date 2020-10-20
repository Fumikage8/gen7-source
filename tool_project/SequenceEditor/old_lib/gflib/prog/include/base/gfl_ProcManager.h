#ifndef __GFL_PROCMANAGER_H__
#define __GFL_PROCMANAGER_H__
//=============================================================================
/**
 * @file	  gfl_ProcManager.h
 * @brief	  �v���Z�X�Ǘ��}�l�[�W���[
 * @author  k.ohno
 * @date	  2010.10.18
 */
//=============================================================================

#pragma once


#include <nn.h>
#include <gfl_Macros.h>
#include <ro/gfl_RoManager.h>
#include <base/gfl_ProcBaseProcess.h>
#include <gfl_Heap.h>


/**
  �Ăяo���ɂ͂�������g���Ă��������B
  NEW�͊O�� DELETE�͓����ōs���݌v�ƂȂ��Ă��܂����̂�
  define��`�ŉB�����ň�a�����ɘa����˂炢�ł�
*/

//===========================================================================================
// �ÓI���W���[���pPROC
//===========================================================================================
//PROC�̌Ăяo���i���ʃR�[���j
#define GFL_PROC_CALL( pManager, CallClass ) \
  pManager->CallProc( GFL_NEW(pManager->GetHeap()) CallClass )

//PROC�̐؂�ւ�
#define GFL_PROC_SETNEXT( pManager, NextClass) \
  pManager->SetNextProc( GFL_NEW(pManager->GetHeap()) NextClass )


//PROC�̌Ăяo��   GFLIB_PROC_CALL<ManagerClass��, CallClass��>(ManagerClass*)
template <class ManagerClass , class CallClass> CallClass* GFLIB_PROC_CALL(ManagerClass* pManager )
{
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;
  pManager->CallProc( pCC );
  return pCC;
}

//PROC�̐؂�ւ��i���ʃR�[���j   GFLIB_PROC_SETNEXT<ManagerClass��, CallClass��>(ManagerClass*)
template <typename ManagerClass , typename CallClass> CallClass* GFLIB_PROC_SETNEXT(ManagerClass* pManager )
{
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;
  pManager->SetNextProc( pCC );
  return pCC;
}

//===========================================================================================
// ���I���W���[���pPROC
//===========================================================================================
#ifdef GFL_USE_RO

//PROC�̌Ăяo��   GFLIB_PROC_CALL<ManagerClass��, CallClass��>(ManagerClass*)
template <class ProcManagerClass , class CallClass> CallClass* GFLIB_RO_PROC_CALL(ProcManagerClass* pManager, const char* croFileName )
{
  nn::ro::Module* pModule = gfl::ro::StartModuleFunc( croFileName );

  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;

  pManager->CallProc( pCC );
  pCC->SetModulePtr( pModule );

  return pCC;
}

//PROC�̐؂�ւ��i���ʃR�[���j   GFLIB_PROC_SETNEXT<ManagerClass��, CallClass��>(ManagerClass*)
template <typename ProcManagerClass , typename CallClass> CallClass* GFLIB_RO_PROC_SETNEXT(ProcManagerClass* pManager, const char* croFileName )
{
  nn::ro::Module* pModule = gfl::ro::StartModuleFunc( croFileName );

  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;

  pManager->SetNextProc( pCC );
  pCC->SetModulePtr( pModule );

  return pCC;
}

#endif



//===========================================================================================

namespace gfl {
  namespace proc {

    class BaseProcess;

    //------------------------------------------------------------------
    /**
     * @brief	�v���Z�X���C���֐��̌���
     *
     * �v���Z�X�̏������E���C���E�I���֐���o�^�����f�[�^
     */
    //------------------------------------------------------------------
    typedef enum {
      MAINSTATUS_NULL,         ///<PROC�����݂��Ȃ�
      MAINSTATUS_VALID,        ///<�L����PROC������
      MAINSTATUS_CHANGE,       ///<PROC���؂�ւ����
      MAINSTATUS_CALL,         ///<���炽��PROC���R�[��
    } MainStatus;

    //------------------------------------------------------------------
    /**
     * @brief	�v���Z�X����̉ߒ�
     */
    //------------------------------------------------------------------
    typedef enum {
      SEQ_LOAD,   ///< ���������܂�
      SEQ_INIT,   ///< ��������
      SEQ_MAIN,   ///< ���C�����쒆
      SEQ_END,    ///< �I����
      SEQ_UNLOAD, ///< �I���ς�
      SEQ_USER,   ///< �ȉ����[�U�[��` procMain���I�[�o�[���C�h���Ďg���ꍇ
    } ProcSeq;

    class Manager {

      GFL_FORBID_COPY_AND_ASSIGN(Manager); //�R�s�[�R���X�g���N�^�{����֎~

    private:

      //------------------------------------------------------------------
      /**
       * @brief	�v���Z�X����̊Ǘ��N���X�i�����j
       */
      //------------------------------------------------------------------

      class processHandle {
      public:
        ProcSeq proc_seq;				  ///<�v���Z�X�����V�[�P���X
        processHandle * parent;			///<��ʃv���Z�X�ւ̃|�C���^
        processHandle * child;			///<���ʃv���Z�X�ւ̃|�C���^
        BaseProcess * my;		    ///<���݂̃��C���v���Z�X�̃|�C���^
      };

    private:

      //------------------------------------------------------------------
      /**
       * @brief	�v���Z�X�V�X�e�������o�ϐ�
       */
      //------------------------------------------------------------------
      gfl::heap::HeapBase * pHeapBase; ///< PROC����鎞�Ɏg��HeapBase
      bool call_flag;				///<�q�v���Z�X�Ăяo���̃��N�G�X�g�t���O
      bool jump_flag;				///<���̃��C���v���Z�X�����݂��邩�ǂ����̃t���O
      bool draw_flag;       ///<Draw�̏������s�����̃t���O�BMain��UpdateFunc���ĂԂƗ���
      BaseProcess * next_data;	///<���̃��C���v���Z�X�̐����f�[�^

      processHandle * mainHandle;  // �i�s���̃n���h��


      //------------------------------------------------------------------
      /**
       * @brief	�v���Z�X�V�X�e�������o�֐�
       */
      //------------------------------------------------------------------
      void entryCallProc(void);
      void jumpCallProc(void);
      void deleteAndJumpProc(void);
      void deleteMainProc( processHandle* proc );
      virtual bool procMain(void);


    protected:
      //------------------------------------------------------------------
      /**
       * @brief	�i�s���n���h���ւ̃A�N�Z�T
       */
      //------------------------------------------------------------------
      ProcSeq GetMainHandleProcSeq( void ) const;
      void SetMainHandleProcSeq( ProcSeq seq );
      BaseProcess* GetMainHandleBaseProcess( void ) const;
      void SetDrawFlag( bool draw );

    public:
      Manager( gfl::heap::HeapBase* pBase );
      virtual ~Manager();


      //------------------------------------------------------------------
      /**
       * @brief	PROC�̏���
       * �o�^����PROC�̏������s���܂��B �K�؂Ȉʒu�ŌĂ�ł��������B
       * @return MainStatus �X�e�[�^�X�̓������A���Ă��܂�
       */
      //------------------------------------------------------------------
      MainStatus Main(void);
      //------------------------------------------------------------------
      /**
       * @brief	PROC�̏���
       * �o�^����PROC�̏������s���܂��B �K�؂Ȉʒu�ŌĂ�ł��������B
       * �K�v�ȏꍇ�́A�O��ŃO���t�B�b�N�̏������Ă�ł��������B
       */
      //------------------------------------------------------------------
      void Draw(void);


      //------------------------------------------------------------------
      /**
       * @brief	PROC�̌Ăяo���i���ʃR�[���j
       * @param	procdata	�V�����v���Z�X�̒�`�f�[�^�A�h���X
       * @param	pwork		�p�����[�^���[�N�ւ̃|�C���^
       * @return �����BaseProcess
       * ���̊֐����Ăяo�������̃t���[������w�肵���v���Z�X���Ăяo�����B
       * �Ăяo���ꂽ�v���Z�X���I������ƁA���݂̃v���Z�X�ւƎ������A����B
       */
      //------------------------------------------------------------------
      BaseProcess* CallProc(BaseProcess * proc);

      //------------------------------------------------------------------
      /**
       * @brief	PROC�̐؂�ւ�
       * @param	procdata	�V�����v���Z�X�̒�`�f�[�^�A�h���X
       * @param	pwork		�p�����[�^���[�N�ւ̃|�C���^
       * @return �����BaseProcess
       * ���݂̃v���Z�X���I��������ɁA���̊֐��Ŏw�肵���v���Z�X�ւƐ��䂪�؂�ւ��B
       */
      //------------------------------------------------------------------
      BaseProcess* SetNextProc(BaseProcess * proc);


      //------------------------------------------------------------------
      /**
       * @brief	PROC����鎞�Ɋm�ۂ���ׂ�HeapBase��Ԃ�
       * @return HeapBase �q�[�v�x�[�X�N���X�̃|�C���^
       */
      //------------------------------------------------------------------

      inline gfl::heap::HeapBase* GetHeap(void){ return pHeapBase; };

      //------------------------------------------------------------------
      /**
       * @brief	 �������Ă���BaseProcess�̐e�ɓ�����BaseProcess��Ԃ�
       * @return BaseProcess�N���X�|�C���^ �e���Ȃ����NULL
       */
      //------------------------------------------------------------------

      BaseProcess* GetParentBaseProcess(void);

      //------------------------------------------------------------------
      /**
       * @brief	 �������Ă���BaseProcess��Ԃ�
       * @return BaseProcess�N���X�|�C���^ �Ȃ����NULL
       */
      //------------------------------------------------------------------

      BaseProcess* GetBaseProcess(void);

      //------------------------------------------------------------------
      /**
       * @brief	 ���ɐݒ肳��Ă���BaseProcess��Ԃ�
       * @return BaseProcess�N���X�|�C���^ �Ȃ����NULL
       */
      //------------------------------------------------------------------
      BaseProcess* GetNextBaseProcess(void);

      //------------------------------------------------------------------
      /**
       * @brief	 �W�����v�v���Z�X�ւ̃t���O�������Ă��邩�ǂ����m�F����
       * @return jump_flag
       */
      //------------------------------------------------------------------
      inline bool GetJumpFlag(void){ return jump_flag; };

      //------------------------------------------------------------------
      /**
       * @brief	 mainHandle�Ɏ��̂����邩�ǂ���
       * @return ��������true
       */
      //------------------------------------------------------------------
      bool IsMainHandle(void){ return (mainHandle!=NULL); };
      //------------------------------------------------------------------
      /**
       * @brief	 �`��t���O
       * @return Main��UpdateFunc���ĂԂƗ���
       */
      //------------------------------------------------------------------
      bool GetDrawFlag(void){return draw_flag;}
      
      // ���C�����쒆�̃v���Z�X�̏����������`�F�b�N
      //------------------------------------------------------------------
      /**
       * @brief	 ���C�����쒆�̃v���Z�X�̏����������`�F�b�N
       * @return ������������true
       */
      //------------------------------------------------------------------
      bool GetBaseProcessInitialized( void ) const;

    };

  } // namespace proc
} // namespace gfl


#endif // __GFL_PROCMANAGER_H__
