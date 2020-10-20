#if PM_DEBUG

//======================================================================
/**
 * @file    ImageDBTestUtility.cpp
 * @date    2016/10/21 17:28:22
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)
// nn
#include <nn/jpeg.h>
// gfl2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
// product
#include "GameSys/include/GameManager.h"

#include "ImageDBTestUtility.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

namespace
{
  static const u32 FINDER_PHOTO_WIDTH  = 256;
  static const u32 FINDER_PHOTO_HEIGHT = 512;
  static const u32 SHRINK_LEVEL        = 2;

  static const u32 CONV_BLOCK_IMAGE_BLOCK_SIZE = 8;
  static const u32 PHOTO_DATA_ALIGNMENT = 128;
}



//------------------------------------------------------------------------------
// @brief   エンコード・デコード
//------------------------------------------------------------------------------
/**
 * @brief   カラーバッファをJpegにエンコードする
 *
 * @param[in]   workHeap        作業ヒープ
 * @param[in]   srcColorBuffer  カラーバッファ
 * @param[out]  dstJpeg         出力先
 */
void ImageDBUtil::EncodeJpeg( gfl2::heap::HeapBase* workHeap, void* srcColorBuffer, FinderPhotoJpeg* dstJpeg )
{
  gfl2::heap::HeapBase* heap = workHeap->GetLowerHandle();
  u32 size = nn::jpeg::JpegMpEncoder::GetWorkBufferSize();

  nn::jpeg::JpegMpEncoder* encoder = GFL_NEW( heap ) nn::jpeg::JpegMpEncoder();
  void *buffer = GflHeapAllocMemoryAlign( heap, size, nn::jpeg::JpegMpEncoder::BUFFER_ALIGNMENT );
  encoder->Initialize( buffer, size );
  encoder->SetThumbnailSize( FINDER_PHOTO_WIDTH >> SHRINK_LEVEL, FINDER_PHOTO_HEIGHT >> SHRINK_LEVEL );
  encoder->SetInputBufferWidth( FINDER_PHOTO_WIDTH );

  // エンコード
  u8 *dst = dstJpeg->data;
  u32 dst_size = encoder->StartJpegEncoder( dst, Field::TrialModel::SIZE_OF_FINDER_JPEG, srcColorBuffer, FINDER_PHOTO_WIDTH, FINDER_PHOTO_HEIGHT, 80, nn::jpeg::PIXEL_SAMPLING_YUV420, nn::jpeg::PIXEL_FORMAT_ABGR8, true );
  dstJpeg->size = dst_size;

  // 掃除
  GflHeapFreeMemory( buffer );
  GFL_SAFE_DELETE( encoder );
}

/**
 * @brief   Jpegをカラーバッファにデコードする
 *
 * @param[in]   workHeap        作業ヒープ
 * @param[in]   srcJpeg         Jpeg
 * @param[out]  dstColorBuffer  出力先
 * @param[in]   format          ピクセルフォーマット
 */
void ImageDBUtil::DecodeColorBuffer( gfl2::heap::HeapBase* workHeap, FinderPhotoJpeg* srcJpeg, void* dstColorBuffer, nn::jpeg::PixelFormat format )
{
  gfl2::heap::HeapBase* heap = workHeap->GetLowerHandle();
  u32 size = nn::jpeg::JpegMpDecoder::GetWorkBufferSize();

  nn::jpeg::JpegMpDecoder* decoder = GFL_NEW( heap ) nn::jpeg::JpegMpDecoder();
  void *buffer = GflHeapAllocMemoryAlign( heap, size, nn::jpeg::JpegMpDecoder::BUFFER_ALIGNMENT );
  decoder->Initialize( buffer, size );
  size = nn::jpeg::JpegMpDecoder::GetDstBufferSize( FINDER_PHOTO_WIDTH, FINDER_PHOTO_HEIGHT, format );

  // デコード
  decoder->StartJpegDecoder( dstColorBuffer, size, srcJpeg->data, srcJpeg->size, FINDER_PHOTO_WIDTH, FINDER_PHOTO_HEIGHT, format, false );
  
  // 掃除
  GflHeapFreeMemory( buffer );
  GFL_SAFE_DELETE( decoder );
}

/**
 * @brief   Jpegをサムネイルにデコードする
 *
 * @param[in]   workHeap   作業ヒープ
 * @param[in]   srcJpeg    Jpeg
 * @param[out]  dstThumb   出力先
 */
void ImageDBUtil::DecodeThumbnail( gfl2::heap::HeapBase* workHeap, FinderPhotoJpeg* srcJpeg, FinderPhotoThumbnail* dstThumb )
{
  gfl2::heap::HeapBase* heap = workHeap->GetLowerHandle();
  u32 size = nn::jpeg::JpegMpDecoder::GetWorkBufferSize();

  nn::jpeg::JpegMpDecoder* decoder = GFL_NEW( heap ) nn::jpeg::JpegMpDecoder();
  void *buffer = GflHeapAllocMemoryAlign( heap, size, nn::jpeg::JpegMpDecoder::BUFFER_ALIGNMENT );
  decoder->Initialize( buffer, size );
  decoder->SetOutputBufferWidth( FINDER_PHOTO_WIDTH >> SHRINK_LEVEL );
#if 1
  size = nn::jpeg::JpegMpDecoder::GetDstBufferSize( FINDER_PHOTO_WIDTH >> SHRINK_LEVEL, FINDER_PHOTO_HEIGHT >> SHRINK_LEVEL, nn::jpeg::PIXEL_FORMAT_CTR_RGBA8_BLOCK8 );
  // デコードで1/4にする
  decoder->StartJpegDecoderShrink( dstThumb->data, size, srcJpeg->data, srcJpeg->size, FINDER_PHOTO_WIDTH, FINDER_PHOTO_HEIGHT, nn::jpeg::PIXEL_FORMAT_CTR_RGBA8_BLOCK8, false, SHRINK_LEVEL );
#else
  size = nn::jpeg::JpegMpDecoder::GetDstBufferSize( FINDER_PHOTO_WIDTH, FINDER_PHOTO_HEIGHT, nn::jpeg::PIXEL_FORMAT_ABGR8 );
  void *dst = GflHeapAllocMemoryAlign( heap, size, 16 );
  // デコード
  decoder->StartJpegDecoder( dst, size, srcJpeg->data, Field::TrialModel::SIZE_OF_FINDER_JPEG, FINDER_PHOTO_WIDTH >> SHRINK_LEVEL, FINDER_PHOTO_HEIGHT >> SHRINK_LEVEL, nn::jpeg::PIXEL_FORMAT_ABGR8, true );
  memcpy( dstThumb->data, dst, Field::TrialModel::SIZE_OF_FINDER_THUMBNAIL );
  GflHeapFreeMemory( dst );
#endif
  // 掃除
  GflHeapFreeMemory( buffer );
  GFL_SAFE_DELETE( decoder );
}



//------------------------------------------------------------------------------
// @brief   保存・読込み
//------------------------------------------------------------------------------
/**
 * @brief   SDカードチェック
 *
 * @param[out]  dstResult   エラー出力
 * @param[in]   workHeap    作業ヒープ
 *
 * @return      写真の残り撮影枚数
 */
s32 ImageDBUtil::CheckSDCard( ImageDBAccessor::imgDbResult* dstResult, gfl2::heap::HeapBase* workHeap )
{
  s32 nResult = 1;

  // ImageDBの準備
  ImageDBAccessor *accessor = CreateImageDBAccessor( workHeap );

  // SDカードのチェック
  nResult = accessor->EstimateRemainPictureNum( dstResult );

  // ImageDBの破棄
  DeleteImageDBAccessor( accessor );

  return nResult;
}

/**
 * @brief   SDカードにカラーバッファを保存する＠ImageDB
 *
 * @param[out]  dstResult         エラー出力
 * @param[in]   workHeap          作業ヒープ
 * @param[in]   srcColorBuffer    カラーバッファ
 * @param[out]  dstFilePath       保存先のパス
 *
 * @return      写真の残り撮影枚数
 */
gfl2::imagedb::ImageDBAccessor::ResultCode ImageDBUtil::SaveColorBuffer( ImageDBAccessor::imgDbResult* dstResult, gfl2::heap::HeapBase* workHeap, void* srcColorBuffer, wchar_t* dstFilePath )
{
  // ImageDBの準備
  ImageDBAccessor *accessor = CreateImageDBAccessor( workHeap );
  // Save
  ImageDBAccessor::ResultCode resCode = accessor->SaveToSDCard( srcColorBuffer, NULL );   // 右目側は保存しない

  // 成功
  if( resCode == ImageDBAccessor::E_RET_SUCCESS )
  {
    if( dstFilePath )
    {
      // ファイル名を取得
      ::std::wcscpy( dstFilePath, accessor->GetSavedFilePath() );
    }
  }
  // 失敗
  else if( resCode == ImageDBAccessor::E_RET_FAILED )
  {
    if( dstResult )
    {
      *dstResult = accessor->GetResultDetail();
    }
  }

  // ImageDBの破棄
  DeleteImageDBAccessor( accessor );

  return resCode;
}

/**
 * @brief   SDカードからカラーバッファを取得する＠ImageDB
 *
 * @param[in]   workHeap        作業ヒープ
 * @param[in]   filepath        ロードするファイルパス
 * @param[out]  dstColorBuffer  出力先
 *
 * @return      "true  = 成功"
 * @return      "false = 失敗"
 */
bool ImageDBUtil::LoadSnapImage( gfl2::heap::HeapBase* workHeap, const wchar_t* filepath, void* tmpConvBuf, void* dstColorBuffer )
{
  ImageDBAccessor::imgDbResult imgRes = imgdb::ResultSuccess;

  // 読込み
  ImageDBAccessor::ResultCode resCode =
    LoadImage( &imgRes, workHeap, filepath, tmpConvBuf, PHOTO_SNAP_WIDTH, PHOTO_SNAP_HEIGHT );

  if( resCode != ImageDBAccessor::E_RET_SUCCESS || imgRes != imgdb::ResultSuccess )
  {
    GFL_PRINT( "LoadSnapImage Failed : filepath[%s] ResultCode[%d] imgDBResult[%d]\n", filepath, resCode, imgRes );
    return false;
  }

  // リニア形式からブロック形式へコンバート
  ImageDBAccessor::ConvertImageLiner2Block( tmpConvBuf, dstColorBuffer, PHOTO_SNAP_WIDTH, PHOTO_SNAP_HEIGHT, CONV_BLOCK_IMAGE_BLOCK_SIZE );

  return true;
}

/**
 * @brief   SDカードからカラーバッファを取得する＠ImageDB：サムネイル版
 *
 * @param[in]   workHeap    作業ヒープ
 * @param[in]   filepath    ロードするファイルパス
 * @param[out]  dstThumb    出力先
 *
 * @return      "true  = 成功"
 * @return      "false = 失敗"
 */
bool ImageDBUtil::LoadThumbImage( gfl2::heap::HeapBase* workHeap, const wchar_t* filepath, FinderPhotoThumbnail* tmpThumb, FinderPhotoThumbnail* dstThumb )
{
  ImageDBAccessor::imgDbResult imgRes = imgdb::ResultSuccess;

  // 読込み
  ImageDBAccessor::ResultCode resCode =
    LoadImage( &imgRes, workHeap, filepath, tmpThumb, PHOTO_THUMB_WIDTH, PHOTO_THUMB_HEIGHT, true );

  if( resCode != ImageDBAccessor::E_RET_SUCCESS || imgRes != imgdb::ResultSuccess )
  {
    GFL_PRINT( "LoadThumbImage Failed : filepath[%s] ResultCode[%d] imgDBResult[%d]\n", filepath, resCode, imgRes );
    return false;
  }

  // リニア形式からブロック形式へコンバート
  ImageDBAccessor::ConvertImageLiner2Block( tmpThumb->data, dstThumb->data, PHOTO_THUMB_WIDTH, PHOTO_THUMB_HEIGHT, CONV_BLOCK_IMAGE_BLOCK_SIZE );

  return true;
}

/**
* @brief   SDカードからカラーバッファを取得する＠ImageDB
*
* @param[out]    dstResult         エラー出力先
* @param[in]     workHeap          作業ヒープ
* @param[in]     filepath          読み込むファイルパス
* @param[out]    dstColorBuffer    出力先
* @param[in]     width             バッファサイズ(横幅)
* @param[in]     height            バッファサイズ(高さ)
* @param[in]     thumbnail         サムネイルモードフラグ(trueならサムネイルモード)
*
* @return    結果コード
*/
ImageDBAccessor::ResultCode ImageDBUtil::LoadImage( ImageDBAccessor::imgDbResult* dstResult, gfl2::heap::HeapBase* workHeap, const wchar_t* filepath, void* dstColorBuffer, u32 width, u32 height, bool thumbnail )
{
  // ImageDBの準備
  ImageDBAccessor* accessor = CreateImageDBAccessor( workHeap );

  ImageDBAccessor::ResultCode resCode =
    accessor->LoadFromSDCard( filepath, dstColorBuffer, NULL, width, height, thumbnail );

  // 失敗
  if( resCode == ImageDBAccessor::E_RET_FAILED )
  {
    if( dstResult )
    {
      *dstResult = accessor->GetResultDetail();
    }
  }

  // ImageDBを破棄
  DeleteImageDBAccessor( accessor );

  return resCode;
}



//------------------------------------------------------------------------------
// @brief   テクスチャの差し替え
//------------------------------------------------------------------------------
/**
 * @brief   テクスチャの差し替え：撮影したイメージバッファで差し替え
 *
 * @param[in]    imgBuf      イメージバッファ
 * @param[in]    picture     ピクチャペイン
 */
void ImageDBUtil::ReplaceTexture_ShootSnap( void* imgBuf, gfl2::lyt::LytPicture* picture )
{
  const nw::lyt::TexSize texSize( 256, 512 );
  const nw::lyt::TexSize realSize( 256, 512 );
  const nw::lyt::TexFormat format = nw::lyt::TEXFORMAT_RGBA8;

  ReplaceTexture( imgBuf, picture, texSize, realSize, format );
}

/**
 * @brief   テクスチャの差し替え：SDカードから読み込んだイメージバッファで差し替え
 *
 * @param[in]    imgBuf      イメージバッファ
 * @param[in]    picture     ピクチャペイン
 */
void ImageDBUtil::ReplaceTexture_LoadSnap( void* imgBuf, gfl2::lyt::LytPicture* picture )
{
  const nw::lyt::TexSize texSize( 512, 256 );
  const nw::lyt::TexSize realSize( 512, 256 );
  const nw::lyt::TexFormat format = nw::lyt::TEXFORMAT_RGBA8;

  ReplaceTexture( imgBuf, picture, texSize, realSize, format );
}

/**
 * @brief   テクスチャの差し替え：SDカードから読み込んだサムネイル用のイメージバッファで差し替え
 *
 * @param[in]    imgBuf      イメージバッファ
 * @param[in]    picture     ピクチャペイン
 */
void ImageDBUtil::ReplaceTexture_LoadThumb( void* imgBuf, gfl2::lyt::LytPicture* picture )
{
  const nw::lyt::TexSize texSize( 128, 64 );
  const nw::lyt::TexSize realSize( 128, 64 );
  const nw::lyt::TexFormat format = nw::lyt::TEXFORMAT_RGBA8;

  ReplaceTexture( imgBuf, picture, texSize, realSize, format );
}

/**
 * @brief   テクスチャの差し替え
 *
 * @param[in]    imgBuf      イメージバッファ
 * @param[in]    picture     ピクチャペイン
 * @param[in]    texSize     テクスチャサイズ
 * @param[in]    realSize    リアルサイズ
 * @param[in]    format      テクスチャフォーマット
 */
void ImageDBUtil::ReplaceTexture( void* imgBuf, gfl2::lyt::LytPicture* picture, const nw::lyt::TexSize texSize, const nw::lyt::TexSize realSize, const nw::lyt::TexFormat format )
{
  if( picture )
  {
    // 仮想アドレスから物理アドレスへの変換
    uptr physicalAddr = nn::gx::GetPhysicalAddr( reinterpret_cast<uptr>( imgBuf ) );

    // テクスチャの生成
    gfl2::lyt::LytResTextureInfo texInfo;
    texInfo.Set( 0, physicalAddr, texSize, realSize, format );

    // テクスチャの差し替え
    gfl2::lyt::LytMaterial* material = picture->GetMaterial();
    material->SetTexMap( 0, &texInfo );
  }
}



//------------------------------------------------------------------------------
// @brief   
//------------------------------------------------------------------------------
/**
 * @brief   ImageDBAccessorの準備
 */
ImageDBAccessor* ImageDBUtil::CreateImageDBAccessor( gfl2::heap::HeapBase* workHeap )
{
  gfl2::heap::HeapBase* heap = workHeap->GetLowerHandle();
  
  ImageDBAccessor *accessor = GFL_NEW( heap ) ImageDBAccessor( heap, heap, GameSys::GameManager::APLICATION_UNIQUE_ID );
  accessor->Initialize();
  GFL_PRINT( "sizeof(ImageDBAccessor)= [%d]\n", sizeof( ImageDBAccessor ) );

  return accessor;
}

/**
 * @brief   ImageDBAccessorの破棄
 */
void ImageDBUtil::DeleteImageDBAccessor( ImageDBAccessor* accessor )
{
  accessor->Finalize();
  GFL_SAFE_DELETE( accessor );
}





//------------------------------------------------------------------------------
// @brief   各種バッファの生成・破棄
//------------------------------------------------------------------------------
/**
 * @brief   カラーバッファの生成
 */
void* ImageDBUtil::CreateColorBuffer( gfl2::heap::HeapBase* heap, u32 align )
{
  const u32 size = sizeof( u32 ) * PHOTO_SNAP_WIDTH * PHOTO_SNAP_HEIGHT;
  GFL_PRINT( "CreateColorBuffer size[%d]\n", size );
  return GflHeapAllocMemoryAlign( heap, size, align );
}

/**
 * @brief   カラーバッファの破棄
 */
void ImageDBUtil::DeleteColorBuffer( void* buffer )
{
  GflHeapSafeFreeMemory( buffer );
}

/**
 * @brief   Jpegデータの生成
 */
FinderPhotoJpeg* ImageDBUtil::CreatePhotoJpeg( gfl2::heap::HeapBase* heap )
{
  GFL_PRINT( "CreatePhotoJpeg size[%d]\n", sizeof( FinderPhotoJpeg ) );
  return GFL_NEW( heap ) FinderPhotoJpeg;
}

/**
 * @brief   Jpegデータの破棄
 */
void ImageDBUtil::DeletePhotoJpeg( FinderPhotoJpeg* jpeg )
{
  GFL_SAFE_DELETE( jpeg );
}

/**
 * @brief   スナップデータの生成
 */
FinderPhotoSnap* ImageDBUtil::CreatePhotoSnap( gfl2::heap::HeapBase* heap )
{
  GFL_PRINT( "CreatePhotoSnap size[%d]\n", sizeof( FinderPhotoSnap ) );
  return static_cast<FinderPhotoSnap*>( GflHeapAllocMemoryAlign( heap, sizeof( FinderPhotoSnap ), PHOTO_DATA_ALIGNMENT ) );
}

/**
 * @brief   スナップデータの破棄
 */
void ImageDBUtil::DeletePhotoSnap( FinderPhotoSnap* snap )
{
  GflHeapSafeFreeMemory( snap );
}

/**
 * @brief   サムネイルデータの生成
 */
void* ImageDBUtil::CreatePhotoThumbnail( gfl2::heap::HeapBase* heap )
{
  GFL_PRINT( "CreatePhotoThumbnail size[%d]\n", sizeof( FinderPhotoThumbnail ) );
  return GflHeapAllocMemoryAlign( heap, sizeof( FinderPhotoThumbnail ), PHOTO_DATA_ALIGNMENT );
}

/**
 * @brief   サムネイルデータの破棄
 */
void ImageDBUtil::DeletePhotoThumbnail( void* thumb )
{
  GflHeapSafeFreeMemory( thumb );
}

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // defined(GF_PLATFORM_CTR)

#endif // #if PM_DEBUG
