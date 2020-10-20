//======================================================================
/**
 * @file    FinderStudioImageDBUtil.cpp
 * @date    2016/11/17 18:25:11
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：ImageDBユーティリティ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioImageDBUtil.h>

#include "GameSys/include/GameManager.h"
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
/// ---[ グローバル変数宣言 begin ]---
gfl2::heap::HeapBase* ImageDBUtil::m_pImgdbHeap = NULL;
gfl2::imagedb::ImageDBPictureInfo* ImageDBUtil::m_pImgdbPicInfo = NULL;
u32 ImageDBUtil::m_InfoCount = 0;
/// ---[ グローバル変数宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/// ---[ 静的 begin ]---

//////////////////////////////////////////////////////////////////////////
///
/// @brief    ImageDBAccessor 生成・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   生成
 *
 * @param   workHeap    作業用ヒープ
 *
 * @note    内部で0x1EB89Cのヒープを確保している。
 */
gfl2::imagedb::ImageDBAccessor* ImageDBUtil::CreateImageDBAccessor( gfl2::heap::HeapBase* workHeap )
{
  gfl2::heap::HeapBase* heap = workHeap->GetLowerHandle();

  gfl2::imagedb::ImageDBAccessor *accessor = GFL_NEW( heap ) gfl2::imagedb::ImageDBAccessor( heap, heap, GameSys::GameManager::APLICATION_UNIQUE_ID );
  accessor->Initialize();

  return accessor;
}

/**
 * @brief   破棄
 *
 * @param   accessor    破棄対象のImageDBAccessor
 */
void ImageDBUtil::DeleteImageDBAccessor( gfl2::imagedb::ImageDBAccessor* accessor )
{
  if( accessor )
  {
    accessor->Finalize();
    GFL_DELETE( accessor );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ImageDBSearcher 生成・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   生成
 *
 * @param   workHeap    作業用ヒープ
 */
gfl2::imagedb::ImageDBSearcher* ImageDBUtil::CreateImageDBSearcher( gfl2::heap::HeapBase* workHeap )
{
  gfl2::heap::HeapBase* heap = workHeap->GetLowerHandle();

  gfl2::imagedb::ImageDBSearcher *searcher = GFL_NEW( heap ) gfl2::imagedb::ImageDBSearcher();
  searcher->Initialize();

  return searcher;
}

/**
 * @brief   破棄
 *
 * @param   searcher    破棄対象のImageDBSearcher
 */
void ImageDBUtil::DeleteImageDBSearcher( gfl2::imagedb::ImageDBSearcher* searcher )
{
  if( searcher )
  {
    searcher->Finalize();
    GFL_DELETE( searcher );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    保存可能枚数チェック
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   SDカードチェック
 *
 * @param[out]  dstRes    エラー出力
 * @param[in]   workHeap  作業ヒープ
 *
 * @return      写真の残り撮影枚数
 */
s32 ImageDBUtil::CheckSDCard( gfl2::imagedb::ImageDBAccessor::imgDbResult* dstRes, gfl2::heap::HeapBase* workHeap )
{
  s32 nResult = 1;

  // ImageDBの準備
  gfl2::imagedb::ImageDBAccessor *accessor = CreateImageDBAccessor( workHeap );

  // SDカードのチェック
  nResult = accessor->EstimateRemainPictureNum( dstRes );

  // ImageDBの破棄
  DeleteImageDBAccessor( accessor );

  return nResult;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真の読込み処理
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   SDカードから読込み
 *
 * @param[out]    dstRes      エラー出力
 * @param[in]     workHeap    作業ヒープ
 * @param[out]    buffer      バッファ格納先
 * @param[in]     filepath    ファイルパス
 * @param[in]     bThumbnail  サムネイルフラグ(デフォルトはfalse)
 *
 * @return      リザルトコード
 *
 * @note    読み込んだ写真データはリニア形式のためブロック形式に変換する必要あり。
 */
gfl2::imagedb::ImageDBAccessor::ResultCode ImageDBUtil::LoadPhotoData( gfl2::imagedb::ImageDBAccessor::imgDbResult* dstRes, gfl2::heap::HeapBase* workHeap, void* buffer, wchar_t* filepath, bool bThumbnail/* = false*/ )
{
  // ImageDBの準備
  gfl2::imagedb::ImageDBAccessor* accessor = CreateImageDBAccessor( workHeap );

  u32 width = 0, height = 0;
  _GetBufferSizeWH( &width, &height, bThumbnail );

  FUKUSHIMA_PRINT( "ImageDBUtil::LoadPhotoData：width=[%d] height=[%d] thumb=[%d]\n", width, height, bThumbnail );

  // SDカードへの保存：右目は保存しない
  gfl2::imagedb::ImageDBAccessor::ResultCode resCode = accessor->LoadFromSDCard( filepath, buffer, NULL, width, height, bThumbnail );

  // ガイドライン対応：画像サイズが一致しなかったら読み込みに失敗したことにする
  {
    const u32 width  = ( !bThumbnail ) ? 400 : 112;
    const u32 height = ( !bThumbnail ) ? 240 : 64;
    const u32 size   = ( !bThumbnail ) ? PHOTO_BUFFER_SIZE : THUMB_BUFFER_SIZE;

    if( accessor->GetLoadImageWidth() != width || accessor->GetLoadImageHeight() != height )
    {
      // クリア
      if( buffer )
      {
        gfl2::std::MemClear( buffer, size );
      }
    }
  }

  if( resCode != gfl2::imagedb::ImageDBAccessor::E_RET_SUCCESS )
  {
    if( dstRes )
    {
      *dstRes = accessor->GetResultDetail();
    }
  }

  // ImageDBの破棄
  DeleteImageDBAccessor( accessor );

  return resCode;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真の保存処理
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   SDカードに保存
 *
 * @param[out]  dstResult   エラー出力
 * @param[in]   workHeap    作業ヒープ
 * @param[in]   texture     写真テクスチャ
 *
 * @return      リザルトコード
 */
gfl2::imagedb::ImageDBAccessor::ResultCode ImageDBUtil::SavePhotoData( gfl2::imagedb::ImageDBAccessor::imgDbResult* dstRes, gfl2::heap::HeapBase* workHeap, void* buffer, wchar_t* filepath )
{
#if defined(GF_PLATFORM_CTR)
  // ImageDBの準備
  gfl2::imagedb::ImageDBAccessor* accessor = CreateImageDBAccessor( workHeap );

  // SDカードへの保存：右目は保存しない
  gfl2::imagedb::ImageDBAccessor::ResultCode resCode = accessor->SaveToSDCard( buffer, NULL );

  if( resCode != gfl2::imagedb::ImageDBAccessor::E_RET_SUCCESS )
  {
    if( dstRes )
    {
      *dstRes = accessor->GetResultDetail();
    }
  }
  else {
    ::std::wcscpy( filepath, accessor->GetSavedFilePath() );
  }

  // ImageDBの破棄
  DeleteImageDBAccessor( accessor );

  return resCode;
#else
  return gfl2::imagedb::ImageDBAccessor::E_RET_SUCCESS;
#endif
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真の削除処理
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   SDカードから削除
 *
 * @param[out]    dstResult   エラー出力
 * @param[in]     heap        作業ヒープ
 * @param[in]     filepath    削除対象のパス
 */
void ImageDBUtil::DeletePhotoData( gfl2::imagedb::ImageDBAccessor::imgDbResult* dstRes, gfl2::heap::HeapBase* workHeap, wchar_t* filepath )
{
  // ImageDBの準備
  gfl2::imagedb::ImageDBAccessor* accessor = CreateImageDBAccessor( workHeap );

  // SDカードから削除
  *dstRes = accessor->DeletePicture( filepath );

  // ImageDBの破棄
  DeleteImageDBAccessor( accessor );
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真の削除処理
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   写真の存在チェック
 *
 * @retval  "true"なら存在する
 * @retval  "false"なら存在しない
 */
bool ImageDBUtil::IsExistsPhoto( gfl2::heap::HeapBase* workHeap, const wchar_t* filepath )
{
  // ImageDBの準備
  gfl2::imagedb::ImageDBAccessor* accessor = CreateImageDBAccessor( workHeap );

  // 写真の存在チェック
  bool bExist = accessor->IsSameTitleUniqueID( filepath );

  // ImageDBの破棄
  DeleteImageDBAccessor( accessor );

  return bExist;
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真の検索処理
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   写真の検索
 *
 * @param   workHeap    作業用ヒープ
 * @param   infoHeap    写真データ確保用
 */
void ImageDBUtil::SearchPhotoData( gfl2::heap::HeapBase* workHeap, gfl2::heap::HeapBase* infoHeap )
{
  // ImageDBAccessorの生成
  gfl2::imagedb::ImageDBAccessor* accessor = CreateImageDBAccessor( workHeap );
  // ImageDBSearcherの生成
  gfl2::imagedb::ImageDBSearcher* searcher = CreateImageDBSearcher( workHeap );

  // SDカードに保存されている写真の枚数を取得
  m_InfoCount = searcher->GetResultNum( GameSys::GameManager::APLICATION_UNIQUE_ID );
  FUKUSHIMA_PRINT("ImageDBSearch ImageCount=[%d]\n", m_InfoCount);
  
  // 枚数分の写真データを生成
  m_pImgdbPicInfo = GFL_NEW_ARRAY( workHeap ) gfl2::imagedb::ImageDBPictureInfo[IMGDB_PHOTO_MAX];

  // 写真データを検索
  gfl2::imagedb::ImageDBSearcher::ResultCode resCode =
    searcher->GetInfo( m_InfoCount, m_pImgdbPicInfo, GameSys::GameManager::APLICATION_UNIQUE_ID );

  if( resCode != gfl2::imagedb::ImageDBSearcher::E_RET_SUCCESS )
  {
    GFL_PRINT( "ImageDB Search Error!! resCode[%d]\n", resCode );
    GFL_ASSERT( 0 );

    // 写真データを削除
    DeletePictureInfo();
  }

  // ImageDBSearcherの破棄
  DeleteImageDBSearcher( searcher );
  // ImageDBAccessorの破棄
  DeleteImageDBAccessor( accessor );
}

/**
 * @brief   写真データを破棄
 */
void ImageDBUtil::DeletePictureInfo( void )
{
  GFL_SAFE_DELETE_ARRAY( m_pImgdbPicInfo );
  m_InfoCount = 0;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンバート
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   リニア形式からブロック形式に変換
 *
 * @param[in]   写真バッファ
 * @param[out]  格納先バッファ
 * @param[in]   サムネイルフラグ
 *
 * @note    GPUコマンドのため１フレーム待機を入れてください。
 */
void ImageDBUtil::ConvertImageLinerToBlock( void* srcBuffer, void* dstBuffer, bool bThumbnail )
{
  u32 width = 0, height = 0;
  _GetBufferSizeWH( &width, &height, bThumbnail );

  FUKUSHIMA_PRINT( "ImageDBUtil::ConvertImageLinerToBlock：width=[%d] height=[%d] thumb=[%d]\n", width, height, bThumbnail );

  gfl2::imagedb::ImageDBAccessor::ConvertImageLiner2Block( srcBuffer, dstBuffer, width, height, BLOCK_IMAGE_BLOCK_SIZE );
}

/**
 * @brief   ブロック形式からリニア形式に変換：512*256サイズ専用
 *
 * @param[in]   写真バッファ
 * @param[out]  格納先バッファ
 *
 * @note    GPUコマンドのため１フレーム待機を入れてください。
 */
void ImageDBUtil::ConvertImageBlockToLiner( void* srcBuffer, void* dstBuffer )
{
  gfl2::std::MemClear( dstBuffer, PHOTO_BUFFER_SIZE );

  // リニアイメージへコンバート
  // GPUコマンドのため実行完了まで１フレーム必要
  gfl2::imagedb::ImageDBAccessor::ConvertImageBlock2Liner( srcBuffer, dstBuffer, PHOTO_BUFFER_WIDTH, PHOTO_BUFFER_HEIGHT, BLOCK_IMAGE_BLOCK_SIZE );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真バッファ 生成・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   生成
 */
void* ImageDBUtil::CreateColorBuffer( gfl2::heap::HeapBase* heap, bool bThumbnail/* = false*/, u32 align/* = PHOTO_BUFFER_ALIGN*/ )
{
  u32 bufSize = PHOTO_BUFFER_SIZE;

  if( bThumbnail )
  {
    bufSize = THUMB_BUFFER_SIZE;
  }

  return GflHeapAllocMemoryAlign( heap, bufSize, align );
}

/**
 * @brief   破棄
 */
void ImageDBUtil::DeleteColorBuffer( void* buffer )
{
  GflHeapSafeFreeMemory( buffer );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    サイズ取得
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   写真サイズの取得
 */
void ImageDBUtil::_GetBufferSizeWH( u32* width, u32* height, bool bThumbnail )
{
  *width  = PHOTO_BUFFER_HEIGHT;
  *height = PHOTO_BUFFER_WIDTH;

  if( bThumbnail )
  {
    *width = THUMB_BUFFER_WIDTH;
    *height = THUMB_BUFFER_HEIGHT;
  }
}


/// ---[ 静的 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/// ---[ 動的 begin ]---

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
ImageDBUtil::ImageDBUtil( void )
  : m_pHeap( NULL )
  , m_Seq( 0 )
  , m_pFilepath( NULL )
  , m_pPhotoData( NULL )
  , m_pTempPhotoData( NULL )
  , m_pTempConvData( NULL )
  , m_bThumbnail( false )
  , m_BufSize( 0 )
  , m_ResultCode( ErrorConverter::IMGDB_RESULT_NUM )
{
  ;
}

/**
 * @brief   デストラクタ
 */
ImageDBUtil::~ImageDBUtil( void )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    読込み処理
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   読込み処理の開始
 *
 * @param[in]   ヒープ
 * @param[in]   読み込むファイルのパス
 * @param[out]  読み込んだデータの格納先
 * @param[in]   サムネイルフラグ
 */
void ImageDBUtil::StartLoadImage( gfl2::heap::HeapBase* heap, wchar_t* filepath, void* dstBuffer, bool bThumbnail )
{
#if defined(GF_PLATFORM_CTR)
  m_pHeap = heap;
  m_pFilepath = filepath;
  m_pPhotoData = dstBuffer;
  m_bThumbnail = bThumbnail;

  m_BufSize = (m_bThumbnail) ? THUMB_BUFFER_SIZE : PHOTO_BUFFER_SIZE;

  m_Seq = 0;
  ResetResult();
#endif
}

/**
 * @brief   読込み処理
 *
 * @retval  "true"なら終了
 * @retval  "false"ならそれ以外
 *
 * @note    ImageDBの結果はGetResultで取得可能
 */
bool ImageDBUtil::IsFinishLoadImage( void )
{
#if defined(GF_PLATFORM_CTR)
  enum {
    SEQ_INIT,
    SEQ_GPU_WAIT,
    SEQ_COPY,
    SEQ_DELETE,
    SEQ_END,
  };

  switch( m_Seq )
  {
    case SEQ_INIT:
    {
      // ImageDBリザルト
      gfl2::imagedb::ImageDBAccessor::imgDbResult imgRes = imgdb::ResultSuccess;

      // 一時確保用データ
      m_pTempPhotoData = ImageDBUtil::CreateColorBuffer( m_pHeap, m_bThumbnail );
      m_pTempConvData = ImageDBUtil::CreateColorBuffer( m_pHeap, m_bThumbnail );

      // ImageDBを使用した写真の読込み
      ImageDBUtil::LoadPhotoData( &imgRes, m_pHeap, m_pTempPhotoData, m_pFilepath, m_bThumbnail );

      // 独自リザルトコードへ変換
      m_ResultCode = ErrorConverter::ConvertImageDBErrorFromImgDBResult( imgRes );

      if( m_ResultCode == ErrorConverter::IMGDB_RESULT_SUCCESS )
      {
        // ブロック形式に変換
        ImageDBUtil::ConvertImageLinerToBlock( m_pTempPhotoData, m_pTempConvData, m_bThumbnail );

        m_Seq = SEQ_GPU_WAIT;
      }
      else {
        GFL_PRINT( "ImageDB Load Failed : resCode=[%]\n", m_ResultCode );
        GFL_ASSERT( 0 );  //!< 開発中に気づかせるためのアサート
        m_Seq = SEQ_DELETE;
      }
    } break;

    case SEQ_GPU_WAIT:
    {
      // GPUコマンドのため待機
      m_Seq = SEQ_COPY;
    } break;

    case SEQ_COPY:
    {
      gfl2::std::MemCopy( m_pTempConvData, m_pPhotoData, m_BufSize );
      m_Seq = SEQ_DELETE;
    } // no break;

    case SEQ_DELETE:
    {
      ImageDBUtil::DeleteColorBuffer( m_pTempConvData );
      ImageDBUtil::DeleteColorBuffer( m_pTempPhotoData );
      m_Seq = SEQ_END;
    } // no break;

    case SEQ_END:
    {
      // データリセット
      _ResetData();
      return true;
    } // no break;
  }

  return false;
#else
  return true;
#endif
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    保存処理
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   保存処理の開始
 *
 * @param[in]   heap        ヒープ
 * @param[out]  filepath    保存
 * @param[in]   srcBuffer   写真データ
 */
void ImageDBUtil::StartSaveImage( gfl2::heap::HeapBase* heap, wchar_t* filepath, void* srcBuffer )
{
#if defined(GF_PLATFORM_CTR)
  m_pHeap = heap;
  m_pFilepath = filepath;
  m_pPhotoData = srcBuffer;

  m_Seq = 0;
  ResetResult();
#endif
}

/**
 * @brief   保存処理
 *
 * @retval  "true"なら終了
 * @retval  "false"ならそれ以外
 *
 * @note    ImageDBの結果はGetResultで取得可能
 */
bool ImageDBUtil::IsFinishSaveImage( void )
{
#if defined(GF_PLATFORM_CTR)
  enum {
    SEQ_INIT,
    SEQ_GPU_WAIT,
    SEQ_SAVE,
    SEQ_END,
  };

  switch( m_Seq )
  {
    case SEQ_INIT:
    {
      // 一時確保用データ
      m_pTempPhotoData = ImageDBUtil::CreateColorBuffer( m_pHeap );

      // リニア形式に変換
      ImageDBUtil::ConvertImageBlockToLiner( m_pPhotoData, m_pTempPhotoData );

      m_Seq = SEQ_GPU_WAIT;
    } break;

    case SEQ_GPU_WAIT:
    {
      m_Seq = SEQ_SAVE;
    } break;

    case SEQ_SAVE:
    {
      // ImageDB用リザルト
      gfl2::imagedb::ImageDBAccessor::imgDbResult imgRes = imgdb::ResultSuccess;

      // SDカードへの保存
      ImageDBUtil::SavePhotoData( &imgRes, m_pHeap, m_pTempPhotoData, m_pFilepath );

      // 独自リザルトコードへ変換
      m_ResultCode = ErrorConverter::ConvertImageDBErrorFromImgDBResult( imgRes );

      m_Seq = SEQ_END;
    } break;

    case SEQ_END:
    {
      ImageDBUtil::DeleteColorBuffer( m_pTempPhotoData );

      return true;
    } // no break;
  }

  return false;
#else
  return true;
#endif
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真の削除
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   削除
 *
 * @param   filepath    削除する写真のファイルパス
 *
 * @note    ImageDBの結果はGetResultで取得可能
 */
void ImageDBUtil::DeleteImage( gfl2::heap::HeapBase* heap, wchar_t* filepath )
{
#if defined(GF_PLATFORM_CTR)
  // ImageDB用リザルト
  gfl2::imagedb::ImageDBAccessor::imgDbResult imgRes = imgdb::ResultSuccess;

  // 写真の存在チェック
  if( ImageDBUtil::IsExistsPhoto( heap, filepath ) )
  {
    // SDカードへの保存
    ImageDBUtil::DeletePhotoData( &imgRes, heap, filepath );
  }
  else {
    GFL_PRINT( "Not Exists Picture [%ls]\n", filepath );
  }

  // 独自リザルトコードへ変換
  m_ResultCode = ErrorConverter::ConvertImageDBErrorFromImgDBResult( imgRes );
#endif
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    便利関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   データリセット
 */
void ImageDBUtil::_ResetData( void )
{
  m_Seq = 0;
  m_pHeap = NULL;
  m_pFilepath = NULL;
  m_pPhotoData = NULL;
  m_bThumbnail = false;
}

/// ---[ 動的 end ]---
//////////////////////////////////////////////////////////////////////////

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
