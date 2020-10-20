//================================================================================
/**
 * @file   gfl_ImageDBSearcher.cpp
 * @brief  ImageDB�𗘗p����SD�J�[�h���ʐ^�̌���
 * @author hisanao suzuki
 * @date   2012.2.13
 */
//================================================================================
#include <imagedb/gfl_ImageDBSearcher.h>


#if GFL_ENABLE_IMAGE_DB


namespace gfl {
  namespace imagedb {

    //-------------------------------------------------------------------------------
    /**
     * @@brief �R���X�g���N�^
     */
    //-------------------------------------------------------------------------------
    ImageDBSearcher::ImageDBSearcher( void ) : m_init( false )
    {
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------------
    ImageDBSearcher::~ImageDBSearcher()
    {
      Finalize();
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief �ʐ^�����@�\�̏����������i�J�n���ɂP�x�����R�[���j
     */
    //-------------------------------------------------------------------------------
    void ImageDBSearcher::Initialize( void )
    {
      if( !m_init )
      {
        m_init = true;
        imgdb::search::Initialize();
      }
      SearchPicture();
    }


    //-------------------------------------------------------------------------------
    /**
     * @brief �ʐ^�����@�\�̏I�������i�I�����ɂP�x�����R�[���j
     */
    //-------------------------------------------------------------------------------
    void ImageDBSearcher::Finalize( void )
    {
      if( m_init )
      {
        imgdb::search::Finalize();
        m_init = false;
      }
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief  �ʐ^�������̍X�V
     *         ��Initialize()�ɂĂP�x�R�[�������
     * @return ���s����
     */
     //-------------------------------------------------------------------------------
    ImageDBSearcher::ResultCode ImageDBSearcher::SearchPicture( void )
    {
      if( m_init )
      {
        imgdb::search::SearchPicture(); 
        return E_RET_SUCCESS;
      }
      return E_RET_NOT_INIT;
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief  SearchPicture()�̌��ʁASD�J�[�h�̎ʐ^�������擾����
     *
     * @param  filterId �t�B���^�����O����A�v��ID�B0���w�肵���ꍇ�̓t�B���^�Ȃ�
     *
     * @return SD�J�[�h���甭�����ꂽ�ʐ^����
     */
     //-------------------------------------------------------------------------------
    int ImageDBSearcher::GetResultNum( u32 filterId )
    {
      u32 max   = imgdb::search::GetResultNum();
      
      if( filterId == 0 ){
        return max;
      }
      u32 count = 0;

      for( u32 i = 0; i < max; i++ )
      {
        if( imgdb::search::GetTitleUniqueId(i) == filterId ){
          count++;
        }
      }
      return count;
    }
    
    //-------------------------------------------------------------------------------
    /**
     * @brief  SD�J�[�h���̎ʐ^�����擾����
     * @param  offset �擾�J�n�I�t�Z�b�g( 0 �` GetResultNum()-1 )
     * @param  num    �擾���(1 �` GetResultNum())
     * @param  outBuf ���i�[��o�b�t�@
     * @return ���s����
     */
     //-------------------------------------------------------------------------------
    ImageDBSearcher::ResultCode ImageDBSearcher::GetInfo( u32 offset, u32 num, ImageDBPictureInfo* outBuf, u32 filterId )
    {
      u32 length = offset+num;
      u32 max    = GetResultNum( filterId );

      if( outBuf == NULL ){
        return E_RET_ILLIGAL_ARGUMENT;
      }
      if( length > max ){
        return E_RET_INDEXOUT;
      }
      
      for( u32 i = offset; i < length; i++ )
      {
        imgdb::search::GetPath( outBuf->m_path, imgdb::imgio::c_nMaxPathSize, i );
        outBuf->m_uniqueId = imgdb::search::GetTitleUniqueId(i);
        outBuf->m_date     = imgdb::search::GetDateTime(i);
        outBuf->m_kind     = imgdb::search::GetImageKind(i);
        outBuf++;
      } 
      
      return E_RET_SUCCESS;
    }


  } // namespace imagedb
} // namespace gfl

#endif

