#ifndef __GAMEPROCMANAGER_H__
#define __GAMEPROCMANAGER_H__
//=============================================================================================
/**
 * @file   GameProcManager.h
 * @brief  �Q�[���̃v���Z�X�}�l�[�W���[
 * @author k.ohno
 * @date   2011.4.19
 */
//=============================================================================================
#pragma once

#include <proc/include/gfl2_Proc.h>
#include <ro/include/gfl2_RoManager.h>

#include "GameSystem/include/GameFrameMode.h"
#include "GameProc.h"

/**
  �Ăяo���ɂ͂�������g���Ă��������B
  NEW�͊O�� DELETE�͓����ōs���݌v�ƂȂ��Ă��܂����̂�
  define��`�ŉB�����ň�a�����ɘa����˂炢�ł�
 */

//===========================================================================================
// �ÓI���W���[���pPROC
//===========================================================================================
#if 0
//!PROC�̌Ăяo���i���ʃR�[���j
#define NIJI_PROC_CALL( pManager, CallClass )            \
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass; \
  pCC->SetGameProcManager( pManager ); \
  pManager->CallProc( pCC )

//!PROC�̐؂�ւ�
#define NIJI_PROC_SETNEXT( pManager, NextClass )            \
  NextClass* pCC = GFL_NEW(pManager->GetHeap()) NextClass;  \
  pCC->SetGameProcManager( pManager );                      \
  pManager->SetNextProc( pCC )
#endif

//!PROC�̐���   NIJI_PROC_CALL<ManagerClass��, CallClass��>(ManagerClass*)
template <class ManagerClass, class CallClass> CallClass* NIJI_PROC_CREATE(ManagerClass* pManager)
{
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;
  pCC->SetGameProcManager( pManager );
  return pCC;
}
//!PROC�̐���( �������t���R���X�g���N�^�� )   NIJI_PROC_CALL<ManagerClass��, CallClass��, ArgClass��>(ManagerClass*, ArgClass*)
template <class ManagerClass, class CallClass, class ArgClass> CallClass* NIJI_PROC_CREATE(ManagerClass* pManager, ArgClass* pArgument)
{
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pArgument );
  pCC->SetGameProcManager( pManager );
  return pCC;
}

//!PROC�̌Ăяo��   NIJI_PROC_CALL<ManagerClass��, CallClass��>(ManagerClass*)
template <class ManagerClass, class CallClass> CallClass* NIJI_PROC_CALL(ManagerClass* pManager)
{
  CallClass* pCC = NIJI_PROC_CREATE<ManagerClass,CallClass>(pManager);
  pCC->SetGameProcManager( pManager );
  pManager->CallProc( pCC );
  return pCC;
}
//!PROC�̌Ăяo��( �������t���R���X�g���N�^�� )   NIJI_PROC_CALL<ManagerClass��, CallClass��, ArgClass��>(ManagerClass*, ArgClass*)
template <class ManagerClass, class CallClass, class ArgClass> CallClass* NIJI_PROC_CALL(ManagerClass* pManager, ArgClass* pArgument)
{
  CallClass* pCC = NIJI_PROC_CREATE<ManagerClass,CallClass,ArgClass>(pManager,pArgument);
  pCC->SetGameProcManager( pManager );
  pManager->CallProc( pCC );
  return pCC;
}

//!PROC�̐؂�ւ��i���ʃR�[���j   NIJI_PROC_SETNEXT<ManagerClass��, CallClass��>(ManagerClass*)
template <typename ManagerClass, typename CallClass> CallClass* NIJI_PROC_SETNEXT(ManagerClass* pManager)
{
  CallClass* pCC = NIJI_PROC_CREATE<ManagerClass,CallClass>(pManager);
  pCC->SetGameProcManager( pManager );
  pManager->SetNextProc( pCC );
  return pCC;
}
//!PROC�̐؂�ւ��i���ʃR�[��, �������t���R���X�g���N�^�Łj   NIJI_PROC_SETNEXT<ManagerClass��, CallClass��, ArgClass��>(ManagerClass*, ArcClass*)
template <typename ManagerClass, typename CallClass, typename ArgClass> CallClass* NIJI_PROC_SETNEXT(ManagerClass* pManager, ArgClass* pArgument)
{
  CallClass* pCC = NIJI_PROC_CREATE<ManagerClass,CallClass,ArgClass>(pManager,pArgument);
  pCC->SetGameProcManager( pManager );
  pManager->SetNextProc( pCC );
  return pCC;
}


//===========================================================================================
// ���I���W���[���pPROC
//===========================================================================================
//!PROC�̐���   NIJI_PROC_CALL<ManagerClass��, CallClass��>(ManagerClass*)
template <class ManagerClass , class CallClass> CallClass* NIJI_RO_PROC_CREATE(ManagerClass* pManager, gfl2::fs::AsyncFileManager *fileMng , const char* croFileName)
{
  gfl2::ro::Module* pModule = gfl2::ro::StartModuleFunc( fileMng , croFileName );

  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;
  pCC->SetModulePtr( pModule );
  pCC->SetGameProcManager( pManager );

  return pCC;
}

//!PROC�̐���( �������t���R���X�g���N�^�� )   NIJI_PROC_CALL<ManagerClass��, CallClass��, ArgClass��>(ManagerClass*, ArgClass*)
template <class ManagerClass , class CallClass, class ArgClass> CallClass* NIJI_RO_PROC_CREATE(ManagerClass* pManager, ArgClass* pArgument, gfl2::fs::AsyncFileManager *fileMng , const char* croFileName )
{
  gfl2::ro::Module* pModule = gfl2::ro::StartModuleFunc( fileMng , croFileName );

  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pArgument );
  pCC->SetGameProcManager( pManager );
  pCC->SetModulePtr( pModule );

  return pCC;
}

//!PROC�̌Ăяo��   NIJI_PROC_CALL<ManagerClass��, CallClass��>(ManagerClass*)
template <class ManagerClass , class CallClass> CallClass* NIJI_RO_PROC_CALL(ManagerClass* pManager, gfl2::fs::AsyncFileManager *fileMng , const char* croFileName)
{
  CallClass* pCC = NIJI_RO_PROC_CREATE<ManagerClass,CallClass>(pManager, fileMng , croFileName);
  pCC->SetGameProcManager( pManager );
  pManager->CallProc( pCC );
  return pCC;
}

//! PROC�̌Ăяo��( �������t���R���X�g���N�^�� )   NIJI_PROC_CALL<ManagerClass��, CallClass��, ArgClass��>(ManagerClass*, ArgClass*)
template <class ManagerClass , class CallClass, class ArgClass> CallClass* NIJI_RO_PROC_CALL(ManagerClass* pManager, ArgClass* pArgument, gfl2::fs::AsyncFileManager *fileMng , const char* croFileName )
{
  CallClass* pCC = NIJI_RO_PROC_CREATE<ManagerClass,CallClass,ArgClass>(pManager, pArgument, fileMng , croFileName);
  pCC->SetGameProcManager( pManager );
  pManager->CallProc( pCC );

  return pCC;
}

//! PROC�̐؂�ւ��i���ʃR�[���j   NIJI_PROC_SETNEXT<ManagerClass��, CallClass��>(ManagerClass*)
template <typename ManagerClass, typename CallClass> CallClass* NIJI_RO_PROC_SETNEXT(ManagerClass* pManager, gfl2::fs::AsyncFileManager *fileMng , const char* croFileName)
{
  CallClass* pCC = NIJI_RO_PROC_CREATE<ManagerClass,CallClass>(pManager, fileMng , croFileName);
  pCC->SetGameProcManager( pManager );
  pManager->SetNextProc( pCC );
  return pCC;
}

//! PROC�̐؂�ւ�( �������t���R���X�g���N�^�� )   NIJI_PROC_SETNEXT<ManagerClass��, CallClass��, ArgClass��>(ManagerClass*, ArgClass*)
template <class ManagerClass , class CallClass, class ArgClass> CallClass* NIJI_RO_PROC_SETNEXT(ManagerClass* pManager, ArgClass* pArgument, gfl2::fs::AsyncFileManager *fileMng , const char* croFileName )
{
  CallClass* pCC = NIJI_RO_PROC_CREATE<ManagerClass,CallClass,ArgClass>(pManager, pArgument, fileMng , croFileName);
  pCC->SetGameProcManager( pManager );
  pManager->SetNextProc( pCC );

  return pCC;
}

namespace GameSystem{

  class GameManager;
  class GameProc;

  class GameProcManager : public gfl2::proc::Manager{
    GFL_FORBID_COPY_AND_ASSIGN(GameProcManager); //�R�s�[�R���X�g���N�^�{����֎~

  private:
    GameManager* mpGameManager;     ///< �Q�[���}�l�[�W���[
    b32          m_IsCallNextFrame; ///< NextFrame������s�����̃t���O

  public:
    GameProcManager(gfl2::heap::HeapBase* pBase, GameManager* pGameManager);  //�R���X�g���N�^
    virtual ~GameProcManager(void){};  //�f�X�g���N�^


    //------------------------------------------------------------------
    /**
     * @brief	PROC�̃C�x���g���m����
     * �o�^����PROC��EventCheckFunc�������s���܂��B �K�؂Ȉʒu�ŌĂ�ł��������B
     */
    //------------------------------------------------------------------
    void EventCheck(void);


    //------------------------------------------------------------------
    /**
     * @brief	PROC�̏���
     * �o�^����PROC�̏������s���܂��B �K�؂Ȉʒu�ŌĂ�ł��������B
     * @return MainStatus �X�e�[�^�X�̓������A���Ă��܂�
     */
    //------------------------------------------------------------------
    gfl2::proc::MainStatus Main(void);

    //------------------------------------------------------------------
    /**
     * @brief	PROC�̏���
     * �o�^����PROC��NextFrameMainFunc�������s���܂��B �K�؂Ȉʒu�ŌĂ�ł��������B
     * @return MainStatus �X�e�[�^�X�̓������A���Ă��܂�
     */
    //------------------------------------------------------------------
    gfl2::proc::MainStatus NextFrameMain(void);

    //------------------------------------------------------------------
    /**
     * @brief   GameManager�𓾂�
     * @return
     */
    //------------------------------------------------------------------
    inline GameManager* GetGameManager(void){ return mpGameManager; }

  };  //class GameProc


}//namespace GameSystem 

#endif //__GAMEPROC_H__
