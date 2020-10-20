#if !defined( __PHOTOSELECT_H__ )
#define __PHOTOSELECT_H__
#pragma once

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		PhotoSelect.h
 *	@brief  �ʐ^�I���A�v���b�g
 *	@author	Koji Kawada
 *	@date		2012.06.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR-SDK
#include <nn.h>
#include <nn/phtsel.h>

// gflib
#include <gfl_Heap.h>


namespace gfl {
namespace std {


// @brief  �ʐ^�I���A�v���b�g
class PhotoSelect
{
  GFL_FORBID_COPY_AND_ASSIGN(PhotoSelect);
  

  //###########################
  //###########################
  //
  // ���J�����o
  //
  //###########################
  //###########################
public:
  // @brief �萔
  enum
  {
    WORK_BUFFER_SIZE     = 950272,  // �g�p���郁�����T�C�Y(�o�C�g)
    FILE_PATH_LENGTH_MAX = 513      // �t�@�C���p�X������ő�l(�I�[����1���������܂݂܂�)(2�o�C�g��1�����Ƃ����ӂ��ɐ�����������)
  };

  // @brief �I������
  enum Result
  {
    RESULT_DECIDE,      // �ʐ^��I�����ďI��
    RESULT_CANCEL,      // �ʐ^��I�������ɏI��
    RESULT_RETURN_CODE  // nn::phtsel::CTR::ReturnCode���`�F�b�N����K�v������I���̎d��
  };
  
  // @brief �ݒ���e
  struct Description
  {
    bool enableSoftReset;   //!< �\�t�g���Z�b�g���L�����ǂ���
    bool enableHomeButton;  //!< �z�[���{�^�����L�����ǂ���

    Description(void)
    {
      enableSoftReset  = true;
      enableHomeButton = true;
    }
  };

public:
  //-----------------------------------------------------------------------------
  /**
   *   @brief  �R���X�g���N�^
   *           ���̎��_�ł̓������͊m�ۂ���܂���B
   *           �R���X�g���N�^��Start�𕪂��Ă���̂́APreloadPhtsel, WaitForPreloadPhtsel, IsRegisteredPhtsel�ɑΉ������邩������Ȃ�����B
   *           
   *   @param[in]  description  �ݒ���e
   */
  //-----------------------------------------------------------------------------
  PhotoSelect(
    const Description& description
  );

  //-----------------------------------------------------------------------------
  /**
   *   @brief  �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  ~PhotoSelect();

  //-----------------------------------------------------------------------------
  /**
   *   @brief  �N��
   *
   *   @param[in]  heap_memory  �q�[�v������(�f�o�C�X�������̓_���ł�)(WORK_BUFFER_SIZE�������������g�p���܂�)
   */
  //-----------------------------------------------------------------------------
  void Start(
    gfl::heap::HeapBase* heap_memory
  );

  //-----------------------------------------------------------------------------
  /**
   *   @brief  �I����ɁA�I�����ʂ��擾����
   *
   *   @retval  �I������
   */
  //-----------------------------------------------------------------------------
  Result GetResult(void) const; 
  //-----------------------------------------------------------------------------
  /**
   *   @brief  �I����ɁA�I�����ꂽ�ʐ^�̃t�@�C���p�X���擾����
   *
   *   @retval  �t�@�C���p�X�BNULL�̂Ƃ��A�L�����Z�����ꂽ���G���[(GetResult()�̖߂�l��RESULT_DECIDE�ȊO�ɂȂ��Ă���)�B
   */
  //-----------------------------------------------------------------------------
  const wchar_t* GetFilePath(void) const;
  //-----------------------------------------------------------------------------
  /**
   *   @brief  �I����ɁA�I�����ꂽ�ʐ^�̃t�@�C���p�X���擾����
   *
   *   @param[out]  file_path  �t�@�C���p�X���R�s�[�����o�b�t�@�B�m���ɁuGetFilePathLength()+�I�[����1�������v�͊m�ۂ��Ă����ĉ������B
   *
   *   @retval  �I������
   *            RESULT_DECIDE�ȊO�̂Ƃ��́A�R�s�[�����܂���B
   */
  //-----------------------------------------------------------------------------
  Result GetFilePath(wchar_t* file_path) const;
  //-----------------------------------------------------------------------------
  /**
   *   @brief  �I����ɁA�I�����ꂽ�ʐ^�̃t�@�C���p�X�̒������擾����
   *
   *   @retval  �t�@�C���p�X�̒����B2�o�C�g��1�����Ƃ����ӂ��ɐ�������������Ԃ��B�I�[�����͊܂܂�Ȃ��������B
   *            GetResult()�̖߂�l��RESULT_DECIDE�ȊO�̂Ƃ��́A0��Ԃ��܂��B
   */
  //-----------------------------------------------------------------------------
  u16 GetFilePathLength(void) const;
  //-----------------------------------------------------------------------------
  /**
   *   @brief  �I����ɁA���^�[���R�[�h���擾����
   *
   *   @retval  ���^�[���R�[�h
   */
  //-----------------------------------------------------------------------------
  nn::phtsel::CTR::ReturnCode GetReturnCode() const;


  //###########################
  //###########################
  //
  // ����J�����o
  //
  //###########################
  //###########################
private:
  nn::phtsel::CTR::Parameter m_parameter;
};


} // namespace std
} // namespace gfl


#endif // __PHOTOSELECT_H__

