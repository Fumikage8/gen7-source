//=============================================================================
/**
 * @file   gfl_NGWordCheckThread.h
 * @author suzuki_hisanao
 */
//=============================================================================

#ifndef __GFL_NG_WORD_CHECK_THREAD_H__
#define __GFL_NG_WORD_CHECK_THREAD_H__

#pragma once

#include <nn.h>
#include <gfl_Base.h>
#include <gfl_Heap.h>


namespace gfl {
namespace ngc {

class NGWordCheckListener;
class NGWordChecker;


/**
 * @brief �m�f���[�h�`�F�b�N�p�����X���b�h�N���X�B<p>
 *        �`�F�b�N������񓯊����s����ׂɏ������ꂽ�N���X�ł��B
 *
 */
class NGWordCheckThread : public gfl::base::Thread
{
  GFL_FORBID_COPY_AND_ASSIGN(NGWordCheckThread); //�R�s�[�R���X�g���N�^�{����֎~

private:
  nn::ngc::CTR::ProfanityFilterPatternList  m_nPatternCode; //!< ���̃��[�W�����Ƃ̃f�[�^�������s����
  NGWordCheckListener*                      m_listener;     //!< �C�x���g���X�i
  wchar_t*                                  m_pText;        //!< �����Ώە����ւ̃|�C���^
  NGWordChecker*                            m_checker;      //!< NG���[�h�`�F�b�J�[

public:
  //--------------------------------------------------------------------------------
  /**
   * @brief �R���X�g���N�^
   *
   * @param[in] heap           �X���b�h�̃X�^�b�N�̈�Ɏg�p����q�[�v
   * @param[in] stack_size     �X���b�h�̃X�^�b�N�̈�̃T�C�Y
   * @param[in] listener       �`�F�b�N�����ʒm�p���X�i
   */
  //--------------------------------------------------------------------------------
  NGWordCheckThread( gfl::heap::HeapBase* heap, u32 stack_size, NGWordCheckListener *listener );

  //--------------------------------------------------------------------------------
  /**
   * @brief �f�X�g���N�^
   */
  //--------------------------------------------------------------------------------
  virtual ~NGWordCheckThread();

  //--------------------------------------------------------------------------------
  /**
   * @brief  ���͂��`�F�b�N���A���̂�����𔭌��������̋�����ݒ�
   *
   * @param[in]      nPatternCode    �`�F�b�N���s�����[�W�������r�b�g�Ŏw�肷��
   * @param[in,out]  pText           �����ΏۂƂȂ�NULL�I�[������BNG�ӏ���*�ɒu����������
   * @param[in]      checker         �m�f���[�h�`�F�b�J�[
   */
  //--------------------------------------------------------------------------------
  void Setup( nn::ngc::CTR::ProfanityFilterPatternList nPatternCode, wchar_t *pText, NGWordChecker* checker );

public:
  virtual void Initialize( void );
  virtual void Finalize( void );
  virtual void OnKill( void );
  virtual void Main( void );
};

} // ngc
} // gfl


#endif

