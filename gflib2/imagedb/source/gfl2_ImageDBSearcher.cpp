//================================================================================
/**
 * @file   gfl2_ImageDBSearcher.cpp
 * @brief  ImageDBを利用したSDカード内写真の検索
 * @author hisanao suzuki
 * @date   2012.2.13
 */
//================================================================================
#include <imagedb/include/gfl2_ImageDBSearcher.h>


#if GFL2_ENABLE_IMAGE_DB


namespace gfl2 {
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
#if defined(GF_PLATFORM_CTR)
        imgdb::search::Initialize();
#endif
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
#if defined(GF_PLATFORM_CTR)
        imgdb::search::Finalize();
#endif
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
#if defined(GF_PLATFORM_CTR)
        imgdb::search::SearchPicture();
#endif
 
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
#if defined(GF_PLATFORM_CTR)
      u32 max   = imgdb::search::GetResultNum();
#elif defined(GF_PLATFORM_WIN32)
      u32 max = 0;
#endif
      
      if( filterId == 0 ){
        return max;
      }
      u32 count = 0;

#if defined(GF_PLATFORM_CTR)
      for( u32 i = 0; i < max; i++ )
      {
        if( imgdb::search::GetTitleUniqueId(i) == filterId ){
          count++;
        }
      }
#endif

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
      
#if defined(GF_PLATFORM_CTR)
      for( u32 i = offset; i < length; i++ )
      {
        imgdb::search::GetPath( outBuf->m_path, imgdb::imgio::c_nMaxPathSize, i );
        outBuf->m_uniqueId = imgdb::search::GetTitleUniqueId(i);
        outBuf->m_date     = imgdb::search::GetDateTime(i);
        outBuf->m_kind     = imgdb::search::GetImageKind(i);
        outBuf++;
      } 
#endif
      
      return E_RET_SUCCESS;
    }


    //-------------------------------------------------------------------------------
    /**
     * @brief  SDカード内の写真情報を取得する
     * @param  num        取得情報数(1 ～ GetResultNum())
     * @param  outBuf     情報格納先バッファ
     * @param  filterId   フィルタリングするアプリID
     * @return 実行結果
     * @fix MMCat[319]
     */
    //-------------------------------------------------------------------------------
    ImageDBSearcher::ResultCode ImageDBSearcher::GetInfo( u32 num, ImageDBPictureInfo* outBuf, u32 filterId /* = 0 */ )
    {
      u32 max = GetResultNum();

      if( outBuf == NULL ){
        return E_RET_ILLIGAL_ARGUMENT;
      }
      if( num > max )
      {
        return E_RET_INDEXOUT;
      }

#if defined(GF_PLATFORM_CTR)
      u32 count = 0;

      for( u32 i = 0; i < max; i++ )
      {
        u32 titleUniqueID = imgdb::search::GetTitleUniqueId( i );

        /// タイトルユニークIDのフィルタリング
        if( !_IsAvailableTitleUniqueID( titleUniqueID, filterId ) )
        {
          continue;
        }

        /// 写真情報の設定
        _SetPictureInfo( &outBuf[count], i, titleUniqueID );

        count++;
        /// 終了確認：取得情報数以上になったらループから抜ける
        if( count >= num )
        {
          break;
        }
      }
#endif

      return E_RET_SUCCESS;
    }


    //-------------------------------------------------------------------------------
    /**
     * @brief   写真情報の設定
     */
    //-------------------------------------------------------------------------------
    void ImageDBSearcher::_SetPictureInfo( ImageDBPictureInfo* outBuf, u32 photoIndex, u32 titleUniqueID )
    {
#if defined(GF_PLATFORM_CTR)
      imgdb::search::GetPath( outBuf->m_path, imgdb::imgio::c_nMaxPathSize, photoIndex );
      outBuf->m_uniqueId = titleUniqueID;
      outBuf->m_date = imgdb::search::GetDateTime( photoIndex );
      outBuf->m_kind = imgdb::search::GetImageKind( photoIndex );
#endif
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief   タイトルユニークIDが有効か
     */
    //-------------------------------------------------------------------------------
    bool ImageDBSearcher::_IsAvailableTitleUniqueID( u32 titleUniqueID, u32 filterID )
    {
      /// フィルターIDが０なら有効
      if( filterID == 0 )
      {
        return true;
      }
      /// フィルターIDとタイトルユニークIDが一致しているなら有効
      if( titleUniqueID == filterID )
      {
        return true;
      }

      /// それ以外は無効
      return false;
    }


  } // namespace imagedb
} // namespace gfl2

#endif

