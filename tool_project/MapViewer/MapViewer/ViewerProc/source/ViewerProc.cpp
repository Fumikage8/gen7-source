//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   FieldProc.cpp
 *  @brief  �t�B�[���h�v���Z�X�N���X
 *  @author tomoya takahashi
 *  @date   2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// gfl2
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_heap_manager.h>

// niji
#include "System/include/GflvUse.h"              // for DisplayDeviceHandle @todo GflUse.h�Q�Ɖӏ��𑝂₵�����Ȃ��̂ŁA�ʃt�@�C���������� 150215 ikuta_junya 
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "GameSystem/include/GameManager.h"


// Field
#include "ViewerProc/include/ViewerProc.h"
#include "ViewerProc/include/FieldStatic/FieldResident/FieldResident.h"


GFL_NAMESPACE_BEGIN( Field );

//-----------------------------------------------------------------------------
/**
 *          �萔�錾
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *          ����
*/
//-----------------------------------------------------------------------------

/**------------------------------------------------------------------
 *      ���J�֐�
 ------------------------------------------------------------------*/

/**------------------------------------------------------------------
 *      GameProc���z�֐�
 ------------------------------------------------------------------*/
/**
* @brief �R���X�g���N�^
*
* @return ����
*/ 
ViewerProc::ViewerProc( void/*FieldmapDesc* desc*/ )
{
	m_pFieldmap = NULL;
  ///m_desc = *desc;
}

/**
* @brief �f�X�g���N�^
*
* @return ����
*/
ViewerProc::~ViewerProc( void )
{
}

/**
* @brief   PROC�̏��������s���֐�
* @return  Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����UpdateFunc�ɐi��
*/
gfl2::proc::Result ViewerProc::InitFunc( gfl2::proc::Manager* /*pManager*/ )
{
  enum
  {
    SEQ_CREATE,
    SEQ_INITIALIZE,
  };
  int seq = this->GetSubSeq();
  switch(seq){
  case SEQ_CREATE:
    {
      // �t�B�[���h�풓�f�[�^����
      gfl2::heap::HeapBase *pResidentHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT );
      FieldResident * pResident = GFL_NEW(pResidentHeap) FieldResident();
      {
				GameSystem::GameManager *gmgr = this->GetGameManager();
        gfl2::heap::HeapBase* pAppHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP); 
        gfl2::heap::HeapBase* pAppHeapDevice = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE); 
        pResident->Initialize( pAppHeap, pAppHeapDevice, gmgr->GetAsyncFileManager() );
        gmgr->SetFieldResident( pResident );
      }
      
      gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP );
      m_pFieldmap = GFL_NEW(systemHeap) Fieldmap(/*m_desc*/);
      this->GetGameManager()->SetFieldmap( m_pFieldmap );
      this->AddSubSeq();
    }
    // break through
  case SEQ_INITIALIZE:
    if( m_pFieldmap->Initialize(this->GetGameManager()) ){
      return gfl2::proc::RES_FINISH; // �I��
    }
    break;
  }
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief    PROC�̃��C���X�V���s���֐�
* @return Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����EndFunc�ɐi��
*/
gfl2::proc::Result ViewerProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
{
  if( m_pFieldmap->Update() ){
    return gfl2::proc::RES_FINISH; // �I��
  }
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief  PROC�̕`�揈��(������)
* @param pManager   �v���Z�X�Ǘ��}�l�[�W���[
* �o�^����PROC�ɂ��A�f�B�X�v���C�o�b�t�@�Ɉˑ����Ȃ��`�揈�������s���܂��B
* ��) ���̎����̕`��R�}���h�̃Z�[�u�A�I�t�X�N���[���o�b�t�@�ւ̕`��
*/
void ViewerProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
{
}

/**
* @brief  PROC�̕`�揈��
* @param pManager   �v���Z�X�Ǘ��}�l�[�W���[
* @param displayNo  �`��Ώۉ��
* �o�^����PROC�ɂ��A�t���[���o�b�t�@�ւ̕`������s���܂��B
* �f�B�X�v���C�o�b�t�@�̐؂�ւ��A�f�B�X�v���C�o�b�t�@�ւ̓]���͌Ăяo�������s���܂��B
*/
void ViewerProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pFieldmap->Draw( displayNo );
}

/**
* @brief   PROC�̃��C�����s���֐�
*  1/30�t���[���̇A�t���[���ڂ̏����ł��B
*  1/60�t���[�����쎞�ɂ́A�g���܂���B
* @return  Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����EndFunc�ɐi��
*/
gfl2::proc::Result ViewerProc::NextFrameMainFunc(gfl2::proc::Manager* /*pManager*/)
{
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief   PROC�̏I�����s���֐�
* @return  Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤���ŏI������Ɖ��߂���
*/
gfl2::proc::Result ViewerProc::EndFunc(gfl2::proc::Manager* /*pManager*/)
{
  enum
  {
    SEQ_ENDWAIT,
    SEQ_DELETE,
  };
  int seq = this->GetSubSeq();
  switch(seq){

  case SEQ_ENDWAIT:
    {
      // �����̃`�F�b�N
      if( m_pFieldmap->Terminate() ){
        this->AddSubSeq();
      }
    }
    break;

  case SEQ_DELETE:
    {
      GFL_DELETE(m_pFieldmap);
      m_pFieldmap = NULL;
      this->GetGameManager()->SetFieldmap( NULL );

			GameSystem::GameManager *gmgr = this->GetGameManager();
			FieldResident * pResident = gmgr->GetFieldResident();
			pResident->Terminate();
      GFL_DELETE(pResident);
			gmgr->SetFieldResident( NULL );
    }
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

#if 0
//------------------------------------------------------------------
/**
 * @brief   PROC�̃C�x���g�N�����m����
 *
 *  �ȉ��̃^�C�~���O�ōs���ׂ��A�C�x���g�N������
 *  �@GameProc->EventCheckFunc
 *  �@EventManager->Main
 *  �@GameProc->Main
 */
//------------------------------------------------------------------
void ViewerProc::EventCheckFunc(GameSystem::GameManager* /*pManager*/)
{
  if( m_pFieldmap ){
    m_pFieldmap->EventCheck();
  }
}
#endif


GFL_NAMESPACE_END( Field );
