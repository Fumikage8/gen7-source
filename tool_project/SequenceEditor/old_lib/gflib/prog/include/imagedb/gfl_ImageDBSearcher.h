//================================================================================
/**
 * @file   gfl_ImageDBSearcher.h
 * @brief  ImageDB�𗘗p����SD�J�[�h���ʐ^�̌���
 *         ImageDBAccessor�́uInitalize()�`Finalize()���Ԓ��v�ł̂ݎg�p�\
 * @author hisanao suzuki
 * @date   2012.2.13
 */
//================================================================================
#ifndef __GFL_IMAGE_DB_SEARCHER_H__
#define __GFL_IMAGE_DB_SEARCHER_H__
#pragma once

#include <imagedb/gfl_ImageDBTypes.h>

#ifdef GFL_USE_IMAGE_DB

namespace gfl {
/**
* @namespace gfl::imagedb
* @brief ImageDB�p�l�[���X�y�[�X
*/
namespace imagedb {

//-------------------------------------------------------------------------------
/**
 * @brief SD�ʐ^�摜���
 */
//-------------------------------------------------------------------------------
typedef struct
{
  wchar_t                  m_path[imgdb::imgio::c_nMaxPathSize];  //!< �t�@�C���p�X
  u32                      m_uniqueId;                            //!< �^�C�g�����j�[�NID
  imgdb::imgio::ImageKind  m_kind;                                //!< �摜���
  nn::fnd::DateTime        m_date;                                //!< �擾�摜
}ImageDBPictureInfo;



//-------------------------------------------------------------------------------
/**
* @brief ImageDB�ʐ^�����p�N���X
*/
//-------------------------------------------------------------------------------
class ImageDBSearcher
{
  GFL_FORBID_COPY_AND_ASSIGN( ImageDBSearcher );  // �R�s�[���֎~

public:
  /**
   * @breif ���U���g�R�[�h
   */
  typedef enum
  {
     E_RET_SUCCESS,           //!< ����
     E_RET_NOT_INIT,          //!< ��������
     E_RET_ILLIGAL_ARGUMENT,  //!< �����s��
     E_RET_INDEXOUT,          //!< �w��o�b�t�@�I�[�o�[
  }ResultCode;

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief �R���X�g���N�^
   */
  //-------------------------------------------------------------------------------
  ImageDBSearcher( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief �f�X�g���N�^
   */
  //-------------------------------------------------------------------------------
  virtual ~ImageDBSearcher();

  //-------------------------------------------------------------------------------
  /**
   * @brief �ʐ^�����@�\�̏����������i�J�n���ɂP�x�����R�[���j
   */
  //-------------------------------------------------------------------------------
  void Initialize( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief �ʐ^�����@�\�̏I�������i�I�����ɂP�x�����R�[���j
   */
  //-------------------------------------------------------------------------------
  void Finalize( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief  �ʐ^�������̍X�V
   *
   * @note   Initialize()�ɂē����łP�x�R�[�������B
   *
   * @return ���s����
   */
   //-------------------------------------------------------------------------------
  ResultCode SearchPicture( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief  SearchPicture()�̌��ʁASD�J�[�h�̎ʐ^�������擾����
   *
   * @param[in]  filterId �t�B���^�����O����A�v��ID�B0���w�肵���ꍇ�̓t�B���^�Ȃ�
   *
   * @return SD�J�[�h���甭�����ꂽ�ʐ^����
   */
   //-------------------------------------------------------------------------------
  int GetResultNum( u32 filterId = 0 );

  //-------------------------------------------------------------------------------
  /**
   * @brief  SD�J�[�h���̎ʐ^�����擾����
   *
   * @param[in]   offset   �擾�J�n�I�t�Z�b�g( 0 �` GetResultNum()-1 )
   * @param[in]   num      �擾���(1 �` GetResultNum())
   * @param[out]  outBuf   ���i�[��o�b�t�@
   * @param[in]   filterId �t�B���^�����O����A�v��ID�B0���w�肵���ꍇ�̓t�B���^�Ȃ�
   *
   * @return ���s����
   */
   //-------------------------------------------------------------------------------
  ResultCode GetInfo( u32 offset, u32 num, ImageDBPictureInfo* outBuf, u32 filterId = 0 );


private:
  bool                       m_init;  //!< �������t���O
};

} // namespace imagedb
} // namespace gfl


#endif
#endif

