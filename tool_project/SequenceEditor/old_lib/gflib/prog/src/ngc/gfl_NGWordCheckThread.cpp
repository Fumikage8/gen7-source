//=============================================================================
/**
 * @file   gfl_NGWordCheckThread.cpp
 * @brief  �m�f���[�h�`�F�b�N�p�X���b�h
 * @author h.suzuki
 * @date   2012.2.15
 */
//=============================================================================

#include <ngc/gfl_NGWordCheckListener.h>
#include <ngc/gfl_NGWordChecker.h>
//
#include "gfl_NGWordCheckThread.h"

namespace gfl {
namespace ngc {

//--------------------------------------------------------------------------------
/**
 * @brief �R���X�g���N�^
 *
 * @param heap        �X���b�h�̃X�^�b�N�̈�Ɏg�p����q�[�v
 * @param stack_size  �X���b�h�̃X�^�b�N�̈�̃T�C�Y
 * @param listener    �`�F�b�N�����ʒm�p���X�i
 */
//--------------------------------------------------------------------------------
NGWordCheckThread::NGWordCheckThread( gfl::heap::HeapBase* heap, u32 stack_size, NGWordCheckListener *listener ) :
  gfl::base::Thread( heap, stack_size, gfl::base::Thread::STACK_TYPE_SAFE ),
  m_listener(listener), m_pText(NULL), m_checker(NULL)
{
}

//--------------------------------------------------------------------------------
/**
 * @brief �f�X�g���N�^
 */
//--------------------------------------------------------------------------------
NGWordCheckThread::~NGWordCheckThread()
{
}


//--------------------------------------------------------------------------------
/**
 * @brief  ���͂��`�F�b�N���A���̂�����𔭌��������̋�����ݒ�
 *
 * @param  nPatternCode    �`�F�b�N���s�����[�W�������r�b�g�Ŏw�肷��
 * @param  pText           �����ΏۂƂȂ�NULL�I�[������BNG�ӏ���*�ɒu����������
 * @param  checker         �m�f���[�h�`�F�b�J�[
 */
//--------------------------------------------------------------------------------
void NGWordCheckThread::Setup( nn::ngc::CTR::ProfanityFilterPatternList nPatternCode, wchar_t *pText, NGWordChecker* checker )
{
  m_nPatternCode = nPatternCode;
  m_pText        = pText;
  m_checker      = checker;
}


//--------------------------------------------------------------------------------
/**
 * @brief ����������
 */
//--------------------------------------------------------------------------------
void NGWordCheckThread::Initialize( void )
{
}


//--------------------------------------------------------------------------------
/**
 * @brief �폜����
 */
//--------------------------------------------------------------------------------
void NGWordCheckThread::OnKill( void )
{
}


//--------------------------------------------------------------------------------
/**
 * @brief �I������
 */
//--------------------------------------------------------------------------------
void NGWordCheckThread::Finalize( void )
{
}


//--------------------------------------------------------------------------------
/**
 * @brief �X���b�h���C������
 */
//--------------------------------------------------------------------------------
void NGWordCheckThread::Main( void )
{
  if( m_checker != NULL )
  {
    gfl::ngc::Result ret = m_checker->CheckFreewordWithRegulation( m_nPatternCode, m_pText );

    if( m_listener ){
      m_listener->OnNGWordCheckFinish( ret );
    }
  }

  this->Kill();
}


} // ngc
} // gfl


