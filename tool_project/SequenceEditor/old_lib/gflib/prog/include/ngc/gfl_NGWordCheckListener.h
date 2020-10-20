//================================================================================
/**
 * @file   gfl_NGWordChecker.h
 * @brief  NG���[�h�`�F�b�N�p�N���X
 * @author hisanao suzuki
 * @date   2012.2.15
 */
//================================================================================
#ifndef __GFL_NG_WORD_CHECK_LISTENER_H__
#define __GFL_NG_WORD_CHECK_LISTENER_H__
#pragma once

#include <nn.h>
#include <nn/ngc.h>


namespace gfl {
namespace ngc {

  /**
   * @brief NG���[�h�̌�������
   */
  enum Result
  {
    E_SUCCESS,                      //!< ���Ȃ�
    E_ERROR_NUMBER_COUNT,           //!< �����������G���[
    E_ERROR_FREEWORD_CHAR_COUNT,    //!< �t���[���[�h�������G���[
    E_ERROR_WORD,                   //!< �P��G���[
  };

  //-------------------------------------------------------------------------------
  /**
  * @brief NG���[�h�`�F�b�N�����ʒm�p���X�i�N���X
  */
  //-------------------------------------------------------------------------------
  class NGWordCheckListener
  {
//    GFL_FORBID_COPY_AND_ASSIGN( NGWordCheckListener );  // �R�s�[���֎~

  public:
    //-------------------------------------------------------------------------------
    /**
    * @brief  NG���[�h�����̊����ʒm�C�x���g
    *
    * @param[in]  result ��������
    */
    // ---------------------------------------------------------------------
    virtual void OnNGWordCheckFinish( gfl::ngc::Result result ) = 0;

  };

} // ngc
} // gfl

#endif  // __GFL_NG_WORD_CHECK_LISTENER_H__
