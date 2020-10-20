#ifndef __GFL_STRBUFREADER_H__
#define __GFL_STRBUFREADER_H__
//===========================================================================
/**
 * @file  gfl_StrBufReader.h
 * @brief GAMEFREAK Library String System ( String Buffer Reader)
 * @author  GAMEFREAK inc.
 * @date  2010.10.28
 */
//===========================================================================
#pragma once

#include <gfl_Common.h>
#include <gfl_Heap.h>
#include <str/gfl_StrBuf.h>

namespace gfl {
namespace str {


//==============================================================================================
/**
 *  ������o�b�t�@���[�_�[�N���X
 *
 *  �w�肳�ꂽ StrBuf �̓��e���P���������₵�Ȃ���ǂݏo���@�\
 */
//==============================================================================================
class StrBufReader
{
  // �R�s�[�R���X�g���N�^��������Z�q�̋֎~
  GFL_FORBID_COPY_AND_ASSIGN( StrBufReader );

private:
  enum {
    TEMP_BUFFER_SIZE_MIN = 512   ///< �Œ���A���ꂾ���̕��������i�[���邾���̃o�b�t�@���m�ۂ���
  };

public:
  //---  �R���X�g���N�^�^�f�X�g���N�^ -----------
  StrBufReader( gfl::heap::HeapBase* pHeap );

  ~StrBufReader();


  //----------------------------------------------------------------------
  /**
   * @brief     �R���X�g���N�^�ɂ�鐳��ȃC���X�^���X�\�z����������������
   *
   * @retval    bool    ����ɍ\�z����Ă����� true
   */
  //----------------------------------------------------------------------
  bool   IsAvirable( void ) const { return m_bAvailable; }

  //----------------------------------------------------------------------
  /**
   * @brief   ���̃��\�b�h���Ăяo�����ƂɂP���������e�������� StrBuf �ւ̎Q�Ƃ��Ԃ�
   *
   * @retval  const StrBuf&
   */
  //----------------------------------------------------------------------
  const StrBuf& ReadNext( void );

  //----------------------------------------------------------------------
  /**
   * ���ɓǂݏo�������R�[�h���擾
   *
   * @retval  STRCODE
   */
  //----------------------------------------------------------------------
  STRCODE  GetNextCode( void ) const;

  //----------------------------------------------------------------------
  /**
   * @brief     �Ō�܂œǂݏo����������
   *
   * @retval    bool    �Ō�܂ŌĂ�ł����� true
   */
  //----------------------------------------------------------------------
  bool  IsEnd( void ) const;

  //----------------------------------------------------------------------
  /**
   * @brief       �ǂݏo���������؂�ւ���
   *
   * @param[in]   srcBuf    �ǂݏo������������
   * @param[in]   pHeap     �����o�b�t�@�� srcBuf ���i�[������Ȃ��ꍇ�A
   *                        �V���Ƀo�b�t�@���\�z���������߂̃q�[�v
   *
   */
  //----------------------------------------------------------------------
  void  SetString( const StrBuf& srcBuf, gfl::heap::HeapBase* pHeap );

  void  SetString( const STRCODE* str, u32 strLen, gfl::heap::HeapBase* pHeap );


  //----------------------------------------------------------------------
  /**
   * @brief     �ǂݏo���|�C���^�����Z�b�g�i���� ReadNext() ���ĂԂƁA�ŏ��̂P���������Ԃ�j
   */
  //----------------------------------------------------------------------
  void  Reset( void );

  //----------------------------------------------------------------------
  /**
   * @brief     �Ō�ɏ������ꂽ�^�O�R�[�h���擾
   *
   * @param[out]   dst      �Ō�ɏ������ꂽ�^�O�R�[�h���󂯎��
   *
   * @retval       bool     dst�ɗL���Ȓl��ݒ肵���ꍇtrue, ����ȊO false
   */
  //----------------------------------------------------------------------
  bool  GetLatestTagCode( STRCODE* dst ) const;


    u16           m_srcLength;
    u16           m_readPtr;

private:
    StrBuf*       m_buffer;
    u16           m_bufLength;
    STRCODE       m_replaceCode;
    STRCODE       m_latestTagCode;
    bool          m_tagProceeded;
    bool          m_bAvailable;

    u16           m_latestTagParamCount;
    u16           m_latestTagParam[ NUM_TAG_PARAM_MAX ];
};



} /* end of namespace str */
} /* end of namespace gfl */



#endif // __GFL_STRBUFREADER_H__
