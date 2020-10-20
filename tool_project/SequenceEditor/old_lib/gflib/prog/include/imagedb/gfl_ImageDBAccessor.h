//================================================================================
/**
 * @file   gfl_ImageDBAccessor.h
 * @brief  ImageDB�𗘗p����SD�J�[�h�ւ̎ʐ^���o�͗p�N���X
 * @author hisanao suzuki
 * @date   2012.2.13
 */
//================================================================================
#ifndef __GFL_IMAGE_DB_ACCESSOR_H__
#define __GFL_IMAGE_DB_ACCESSOR_H__
#pragma once

#include <imagedb/gfl_ImageDBTypes.h>


#ifdef GFL_USE_IMAGE_DB

namespace gfl {
namespace imagedb {

class MyAllocator;

//-------------------------------------------------------------------------------
/**
* @brief ImageDB�A�N�Z�X�p�N���X
*/
//-------------------------------------------------------------------------------
class ImageDBAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( ImageDBAccessor );  // �R�s�[���֎~

public:
  static const u32 IMGDB_HEAP_SIZE = 0x130000;    //!< ImageDb���C�u�����Ɋ��蓖�Ă鐄���q�[�v��������
  static const u32 REGISTERD_IMAGE_MAX = 3000;    //!< ImageDb��������ő�ʐ^����

  /**
   * @breif ���U���g�R�[�h
   */
  typedef enum
  {
     E_RET_SUCCESS,         //!< ����
     E_RET_OUT_OF_MEMORY,   //!< �������s��
     E_RET_SDCARD_NOEXIST,  //!< SD�J�[�h���}��
     E_RET_SDCARD_READONLY, //!< SD�J�[�h�����݋֎~
     E_RET_FAILED,          //!< ���s�BGetErrorDetail()�ŏڍׂȃG���[���擾���邱��
  }ResultCode;

public:
  /**
   * @brief  �ۑ��Ɏg�p�����f�t�H���g�摜�t�H�[�}�b�g
   */
  static const nn::jpeg::CTR::PixelFormat IMAGE_FORMAT = nn::jpeg::CTR::PIXEL_FORMAT_ABGR8;

  static const int IMAGE_UPEER_WIDTH  = 400;        //!< �ۑ��Ɏg�p�������ʗp�̉摜����
  static const int IMAGE_UPEER_HEIGHT = 240;        //!< �ۑ��Ɏg�p�������ʗp�̉摜�c��
  static const int IMAGE_LOWER_WIDTH  = 320;        //!< �ۑ��Ɏg�p����鉺��ʗp�̉摜����
  static const int IMAGE_LOWER_HEIGHT = 240;        //!< �ۑ��Ɏg�p����鉺��ʗp�̉摜�c��
  static const int IMAGE_ALIGN_SIZE   = 4;          //!< �C���[�W�o�b�t�@�p�������A���C���T�C�Y
  static const int IMAGE_DEFAULT_JPEG_QUALITY = 80; //!< Jpeg�ۑ����̕i���f�t�H���g�l


public:
  //-------------------------------------------------------------------------------
  /**
   * @brief �R���X�g���N�^
   *
   * @param[in]  heap         �q�[�v
   * @param[in]  deviceHeap   �f�o�C�X�q�[�v
   * @param[in]  appUniqueId  �A�v���P�[�V�����ŗLID(20bit)
   */
  //-------------------------------------------------------------------------------
  ImageDBAccessor( gfl::heap::HeapBase *heap, gfl::heap::HeapBase *deviceHeap, u32 appUniqueId );

  //-------------------------------------------------------------------------------
  /**
   * @brief �f�X�g���N�^
   */
  //-------------------------------------------------------------------------------
  virtual ~ImageDBAccessor();

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief ImageDb���C�u�����̏����������i�J�n���ɂP�x�����R�[���j
   *
   * @param[in] useHeapSize  ImageDb���C�u�����Ɋ��蓖�Ă�q�[�v�T�C�Y
   * @param[in] targetWidth  �o�͌��\�[�X�ƂȂ��ʂ̉���
   * @param[in] targetHeight �o�͌��\�[�X�ƂȂ��ʂ̏c��
   * @param[in] isSimpleMode �C���[�W���[�e�B���e�B�[���g�p���邽�߂����̏��������s���Ȃ�true���w�肷��
   */
  //-------------------------------------------------------------------------------
  ResultCode Initialize( u32 useHeapSize = IMGDB_HEAP_SIZE,
                         u32 targetWidth = IMAGE_UPEER_WIDTH, u32 targetHeight = IMAGE_UPEER_HEIGHT,
                         bool isSimpleMode=false );

  //-------------------------------------------------------------------------------
  /**
   * @brief ImageDb���C�u�����̏I�������i�I�����ɂP�x�����R�[���j
   */
  //-------------------------------------------------------------------------------
  void Finalize( void );

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief ImageDb�����p����SD�J�[�h�A�[�J�C�u���}�E���g����Ă��邩�`�F�b�N
   *
   * @return SD�J�[�h���}�E���g����Ă���Ȃ�true��ԋp
   */
  //-------------------------------------------------------------------------------
  bool IsMountSdmc( void ) const;

  //-------------------------------------------------------------------------------
  /**
   * @brief ImageDb�����p����SD�J�[�h�A�[�J�C�u���}�E���g����
   *
   * @return SD�J�[�h�̃}�E���g�ɐ��������Ȃ�true��ԋp
   */
  //-------------------------------------------------------------------------------
  bool MountSdmc( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief ImageDb�����p����SD�J�[�h�A�[�J�C�u���A���}�E���g����
   *
   * @note  SD�J�[�h�������ꂽ��A�v�����Ŗ{�֐����Ăяo���ăA���}�E���g���Ă�������
   */
  //-------------------------------------------------------------------------------
  void UnmountSdmc( void );

public:

  //-------------------------------------------------------------------------------
  /**
   * @brief  �Ǘ�����Ă���SD�J�[�h���̎ʐ^�������擾����
   *
   * @return �ʐ^�����BSD�J�[�h���}�E���g����Ă��Ȃ��Ȃ畉�̒l��Ԃ�
   */
   //-------------------------------------------------------------------------------
  int GetRegisteredPictureNum( void ) const;

  //-------------------------------------------------------------------------------
  /**
   * @brief  SD�J�[�h�w�ۑ��\�Ȏʐ^���������ς���
   *
   * @param  pResult �B�e�\�Ȃ�ResultSuccess()���A�B�e�s�\�Ȃ炻��ȊO���i�[
   *
   * @par   pResult�Ɋi�[�����l�́A�ȉ��̂Ƃ���B
   *        - ResultErrorOverNumImages
   *        - ResultErrorFileOperationFailure
   *        - ResultErrorNotExistStorage
   *        - ResultErrorWriteProtect
   *        - ResultErrorInsufficientStorageSpace
   *        - ResultErrorReadOnlyDbFile
   *        - ResultErrorNotAvailableStorage
   *
   * @return �ʐ^�̎c��B�e����
   */
   //-------------------------------------------------------------------------------
  int EstimateRemainPictureNum( imgdb::Result* pResult );

  //-------------------------------------------------------------------------------
  /**
   * @brief  SD�J�[�h���̎ʐ^���폜����
   *
   * @param[in]  pPath �폜�ΏۂƂȂ�ʐ^�ւ̃t�@�C���p�X
   *
   * @return �G���[�񋓒萔
   */
   //-------------------------------------------------------------------------------
  imgdb::Result DeletePicture( const wchar_t * pPath );


public:
  //-------------------------------------------------------------------------------
  /**
   * @brief SD�J�[�h�֎ʐ^��ۑ�����B
   *        �X�N���[���V���b�g��Pixel�C���[�W��n�����ƂŎʐ^�f�[�^�Ƃ���SD�J�[�h�֕ۑ������B
   *
   * @attention  �����ň����n��Pixel�f�[�^�̓��j�A�C���[�W�ł���K�v������B
   * @attention  �u���b�N�C���[�W���烊�j�A�C���[�W�ւ̕ϊ���ConvertImageBlock2Liner()���g�p���邱��
   *
   * @param[in] pImageTop      ���Pixel�f�[�^(RGBA)
   * @param[in] pImageTopRight ���̎��p�E���Pixel�f�[�^(RGBA)�BNULL���w�肷�邱�Ƃ��\�B
   * @param[in] width          �C���[�W�f�[�^�̉���(2�ׂ̂���ł��邱�ƁI�j
   * @param[in] height         �C���[�W�f�[�^�̏c��(2�ׂ̂���ł��邱�ƁI�j
   * @param[in] pUserMakerNote ���[�U�[�m�[�g�p������
   * @param[in] noteSize       ���[�U�[�m�[�g�p������T�C�Y
   * @param[in] jpegQuality    Jpeg�̉摜�i���i1�`100)
   *
   * @return ���U���g�R�[�h
   */
  //-------------------------------------------------------------------------------
  ResultCode SaveToSDCard( const void* pImageTop, const void *pImageTopRight, u32 width=512, u32 height=256, const void* pUserMakerNote=NULL, u32 noteSize=0, int jpegQuality=IMAGE_DEFAULT_JPEG_QUALITY );

  //-------------------------------------------------------------------------------
  /**
   * @brief SD�J�[�h����̎ʐ^��ǂݍ��ށB
   *        �i�[��̃o�b�t�@�T�C�Y�́AGetBufferSize()�ɂĎ擾���邱��
   *
   * @attention  �擾�����Pixel�f�[�^�̓��j�A�C���[�W�ł���B
   * @attention  �u���b�N�C���[�W�ւ̕ϊ���ConvertImageLiner2Block()���g�p���邱��
   *
   * @param[in]  filePath       �ǂݍ��ݑΏۂƂȂ�ʐ^�̃t�@�C���p�X
   * @param[out] pImageTop      �f�[�^�i�[��o�b�t�@
   * @param[out] pImageTopRight ���̎��p�E���Pixel�f�[�^�i�[��o�b�t�@�BNULL���w�肷�邱�Ƃ��\�B
   * @param[in]  width          �i�[��o�b�t�@�̉摜����(2�ׂ̂���ł��邱�ƁI�j
   * @param[in]  height         �i�[��o�b�t�@�̉摜�c��(2�ׂ̂���ł��邱�ƁI�j
   *
   * @return ���U���g�R�[�h
   */
  //-------------------------------------------------------------------------------
  ImageDBAccessor::ResultCode LoadFromSDCard( const wchar_t * filePath, void* pImageTop, void *pImageTopRight, u32 width, u32 height );

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief  SaveToSDCard()���s���ʂ̏ڍׂȃG���[�R�[�h���擾����
   *
   * @return �G���[�񋓒萔
   */
   //-------------------------------------------------------------------------------
  imgdb::Result GetResultDetail( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief  SaveToSDCard()��SD�J�[�h�ɕۑ����ꂽ�ʐ^�t�@�C���̐�΃p�X���擾����
   *
   * @retval �󕶎�     ���ۑ��A�܂��͕ۑ����s
   * @retval �󕶎��ȊO �ʐ^�t�@�C���̃t�@�C�����i��΃p�X�j
   */
   //-------------------------------------------------------------------------------
  const wchar_t * GetSavedFilePath() const;

  //-------------------------------------------------------------------------------
  /**
   * @brief  �ǂݍ���Piexel�f�[�^���i�[����ׂɕK�v�ȃo�b�t�@�̃T�C�Y���擾����
   *
   * @return �ǂݍ��݂ɕK�v�ƂȂ�o�b�t�@�T�C�Y�i���̎����͂P��ʕ��̃o�b�t�@�T�C�Y�j
   */
   //-------------------------------------------------------------------------------
  size_t GetBufferSize() const;

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief �u���b�N�C���[�W���烊�j�A�C���[�W�֕ϊ�����
   *
   * @note  �{API��GPU�R�}���h�̂��߁A���s������1�t���[����v����
   *
   * @param[in]  pSrcImg   �ϊ����ƂȂ�u���b�N�C���[�W�i�f�o�C�X�������K�{�I�j
   * @param[out] pDstImg   ���j�A�C���[�W�i�[��i�f�o�C�X�������K�{�I�j
   * @param[in]  width     �C���[�W����
   * @param[in]  height    �C���[�W�c��
   * @param[in]  blockSize �u���b�N�C���[�W�̃T�C�Y�i8�A32�̂����ꂩ�j
   */
  //-------------------------------------------------------------------------------
  static void ConvertImageBlock2Liner( const void * pSrcImg, void* pDstImg, u32 width, u32 height, u32 blockSize );

  //-------------------------------------------------------------------------------
  /**
   * @brief ���j�A�C���[�W����u���b�N�C���[�W�֕ϊ�����
   *
   * @note  �{API��GPU�R�}���h�̂��߁A���s������1�t���[����v����
   *
   * @param[in]  pSrcImg   �ϊ����ƂȂ郊�j�A�C���[�W�i�f�o�C�X�������K�{�I�j
   * @param[out] pDstImg   �u���b�N�C���[�W�i�[��i�f�o�C�X�������K�{�I�j
   * @param[in]  width     �C���[�W����
   * @param[in]  height    �C���[�W�c��
   * @param[in]  blockSize �u���b�N�C���[�W�̃T�C�Y�i8�A32�̂����ꂩ�j
   */
  //-------------------------------------------------------------------------------
  static void ConvertImageLiner2Block( const void * pSrcImg, void* pDstImg, u32 width, u32 height, u32 blockSize );

private:
  void                      *m_imgDbWork;                               //!< ImageDb���C�u�����p�q�[�v������
  MyAllocator               *m_tmpAllocator;                            //!< �Ǝ��A���P�[�^
  gfl::heap::HeapBase       *m_heap;                                    //!< �q�[�v
  gfl::heap::HeapBase       *m_deviceHeap;                              //!< �f�o�C�X�q�[�v
  void                      *m_topBuf;                                  //!< �㕔��ʗp�o�b�t�@
  void                      *m_topRightBuf;                             //!< �㕔��ʗ��̎��p�o�b�t�@
  nn::jpeg::CTR::PixelFormat m_format;                                  //!< �o�͉摜�̃t�H�[�}�b�g
  u32                        m_targetWidth;                             //!< �o�͉摜�̉���
  u32                        m_targetHeight;                            //!< �o�͉摜�̏c��
  u32                        m_appUniqueId;                             //!< �A�v���P�[�V�����ŗLID(20Bit)
  bool                       m_init;                                    //!< �������t���O
  imgdb::Result              m_resultDetail;                            //!< SaveToSDCard()���s����
  wchar_t                    m_filePath[imgdb::imgio::c_nMaxPathSize];  //!< �ۑ����ꂽ�t�@�C����


private:
  ResultCode    AllocateImageBuffer( void );
  void          FreeImageBuffer( void );
  void          CopyStereoView2Memory( void* srcBuf, void* dstBuf, u32 srcWidth, u32 srcHeight, u32 dstWidth, u32 dstHeight );
  void          CopyMemory2StereoView( void* srcBuf, void* dstBuf, u32 srcWidth, u32 srcHeight, u32 dstWidth, u32 dstHeight );
  imgdb::Result SaveScreenshotToSDCard( imgdb::MpSaver* saver,   const void* memNormalBuf, const void* memRightBuf, const void* pUserMakerNote, u32 noteSize );
  imgdb::Result SaveScreenshotToSDCard( imgdb::JpegSaver* saver, const void* memNormalBuf, const void* pUserMakerNote, u32 noteSize );
  imgdb::Result LoadScreenshotFromSDCard( imgdb::MpLoader* loader, const wchar_t* filePath, void* memNormalBuf, void* memRightBuf );
  imgdb::Result LoadScreenshotFromSDCard( imgdb::JpegLoader* loader, const wchar_t* filePath, void* memNormalBuf );
};

} // namespace imagedb
} // namespace gfl


#endif
#endif

