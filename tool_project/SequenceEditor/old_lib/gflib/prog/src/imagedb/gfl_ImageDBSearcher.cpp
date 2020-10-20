//================================================================================
/**
 * @file   gfl_ImageDBSearcher.cpp
 * @brief  ImageDBを利用したSDカード内写真の検索
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
     * @@brief コンストラクタ
     */
    //-------------------------------------------------------------------------------
    ImageDBSearcher::ImageDBSearcher( void ) : m_init( false )
    {
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------------
    ImageDBSearcher::~ImageDBSearcher()
    {
      Finalize();
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief 写真検索機能の初期化処理（開始時に１度だけコール）
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
     * @brief 写真検索機能の終了処理（終了時に１度だけコール）
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
     * @brief  写真検索情報の更新
     *         ※Initialize()にて１度コールされる
     * @return 実行結果
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
     * @brief  SearchPicture()の結果、SDカードの写真枚数を取得する
     *
     * @param  filterId フィルタリングするアプリID。0を指定した場合はフィルタなし
     *
     * @return SDカードから発見された写真枚数
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
     * @brief  SDカード内の写真情報を取得する
     * @param  offset 取得開始オフセット( 0 ～ GetResultNum()-1 )
     * @param  num    取得情報数(1 ～ GetResultNum())
     * @param  outBuf 情報格納先バッファ
     * @return 実行結果
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

