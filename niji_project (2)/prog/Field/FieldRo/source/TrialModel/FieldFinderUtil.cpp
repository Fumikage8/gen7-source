//======================================================================
/**
 * @file FieldFinderUtil.cpp
 * @date 2015/10/23 11:44:28
 * @author miyachi_soichi
 * @brief ファインダー用便利関数
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)
// nn
#include <nn/jpeg.h>
// gfl2
#include <debug/include/gfl2_Assert.h>
// product
#include <NetStatic/NetLib/include/NetDefine.h>
// GameSys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
// savedata
#include "Savedata/include/PokeFinderSave.h"
// self
#include "Field/FieldRo/include/TrialModel/FieldFinderUtil.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)

static const u32 FINDER_PHOTO_WIDTH      = 256;
static const u32 FINDER_PHOTO_HEIGHT     = 512;
static const u32 SHRINK_LEVEL            = 2;

/**
 *  @class  FinderUtil
 */
/**
 *  @brief  カラーバッファをJpegにエンコードする
 *  @param  pWorkHeap     作業用ヒープ
 *  @param  pColorBuffer  カラーバッファ
 *  @param  out_pData     出力先
 */
void FinderUtil::EncodeJpeg( gfl2::heap::HeapBase *pWorkHeap, void *pColorBuffer, FinderPhotoJpeg *out_pData )
{
  u32 size = nn::jpeg::JpegMpEncoder::GetWorkBufferSize();

  nn::jpeg::JpegMpEncoder *pEncoder = GFL_NEW( pWorkHeap ) nn::jpeg::JpegMpEncoder();
  void *buffer = GflHeapAllocMemoryAlign( pWorkHeap, size, nn::jpeg::JpegMpEncoder::BUFFER_ALIGNMENT );
  pEncoder->Initialize( buffer, size );
  pEncoder->SetThumbnailSize( FINDER_PHOTO_WIDTH>>SHRINK_LEVEL, FINDER_PHOTO_HEIGHT>>SHRINK_LEVEL );
  pEncoder->SetInputBufferWidth(FINDER_PHOTO_WIDTH);
  // エンコード
  u8 *dst = out_pData->data;
  u32 dst_size = pEncoder->StartJpegEncoder( dst, Field::TrialModel::SIZE_OF_FINDER_JPEG, pColorBuffer, FINDER_PHOTO_WIDTH, FINDER_PHOTO_HEIGHT, 80, nn::jpeg::PIXEL_SAMPLING_YUV420, nn::jpeg::PIXEL_FORMAT_ABGR8, true );
  out_pData->size = dst_size;
  // 掃除
  GflHeapFreeMemory( buffer );
  GFL_SAFE_DELETE( pEncoder );
}

/**
 *  @brief  Jpegをカラーバッファにデコードする
 *  @param  pWorkHeap         作業用ヒープ
 *  @param  pJpeg             出力先
 *  @param  out_pColorBuffer  カラーバッファ
 */
void FinderUtil::DecodeColorBuffer( gfl2::heap::HeapBase *pWorkHeap, FinderPhotoJpeg *pJpeg, void *out_pColorBuffer, nn::jpeg::PixelFormat format )
{
  u32 size = nn::jpeg::JpegMpDecoder::GetWorkBufferSize();

  nn::jpeg::JpegMpDecoder *pDecoder = GFL_NEW( pWorkHeap ) nn::jpeg::JpegMpDecoder();
  void *buffer = GflHeapAllocMemoryAlign( pWorkHeap, size, nn::jpeg::JpegMpDecoder::BUFFER_ALIGNMENT );
  pDecoder->Initialize( buffer, size );
  size = nn::jpeg::JpegMpDecoder::GetDstBufferSize( FINDER_PHOTO_WIDTH, FINDER_PHOTO_HEIGHT, format );
  // デコード
  pDecoder->StartJpegDecoder( out_pColorBuffer, size, pJpeg->data, pJpeg->size, FINDER_PHOTO_WIDTH, FINDER_PHOTO_HEIGHT, format, false );
  // 掃除
  GflHeapFreeMemory( buffer );
  GFL_SAFE_DELETE( pDecoder );
}

/**
 *  @brief  Jpegをサムネイルにデコードする
 *  @param  pWorkHeap         作業用ヒープ
 *  @param  pJpeg             出力先
 *  @param  out_pData         サムネイル
 */
void FinderUtil::DecodeThumbnail( gfl2::heap::HeapBase *pWorkHeap, FinderPhotoJpeg *pJpeg, FinderPhotoThumbnail *out_pData )
{
  u32 size = nn::jpeg::JpegMpDecoder::GetWorkBufferSize();

  nn::jpeg::JpegMpDecoder *pDecoder = GFL_NEW( pWorkHeap ) nn::jpeg::JpegMpDecoder();
  void *buffer = GflHeapAllocMemoryAlign( pWorkHeap, size, nn::jpeg::JpegMpDecoder::BUFFER_ALIGNMENT );
  pDecoder->Initialize( buffer, size );
  pDecoder->SetOutputBufferWidth(FINDER_PHOTO_WIDTH>>SHRINK_LEVEL);
#if 1
  size = nn::jpeg::JpegMpDecoder::GetDstBufferSize( FINDER_PHOTO_WIDTH>>SHRINK_LEVEL, FINDER_PHOTO_HEIGHT>>SHRINK_LEVEL, nn::jpeg::PIXEL_FORMAT_CTR_RGBA8_BLOCK8 );
  // デコードで1/4にする
  pDecoder->StartJpegDecoderShrink( out_pData->data, size, pJpeg->data, pJpeg->size, FINDER_PHOTO_WIDTH, FINDER_PHOTO_HEIGHT, nn::jpeg::PIXEL_FORMAT_CTR_RGBA8_BLOCK8, false, SHRINK_LEVEL );
#else
  size = nn::jpeg::JpegMpDecoder::GetDstBufferSize( FINDER_PHOTO_WIDTH, FINDER_PHOTO_HEIGHT, nn::jpeg::PIXEL_FORMAT_ABGR8 );
  void *dst = GflHeapAllocMemoryAlign( pWorkHeap, size, 16 );
  // デコード
  pDecoder->StartJpegDecoder( dst, size, pJpeg->data, Field::TrialModel::SIZE_OF_FINDER_JPEG, FINDER_PHOTO_WIDTH>>SHRINK_LEVEL, FINDER_PHOTO_HEIGHT>>SHRINK_LEVEL, nn::jpeg::PIXEL_FORMAT_ABGR8, true );
  memcpy( out_pData->data, dst, Field::TrialModel::SIZE_OF_FINDER_THUMBNAIL );
  GflHeapFreeMemory( dst );
#endif
  // 掃除
  GflHeapFreeMemory( buffer );
  GFL_SAFE_DELETE( pDecoder );
}

/**
 *  @brief  SDカードチェック
 */
s32 FinderUtil::CheckSDCard( gfl2::imagedb::ImageDBAccessor::imgDbResult *out_result, gfl2::heap::HeapBase *pWorkHeap )
{
  s32 nResult = 1;

  // ImageDBの準備
  gfl2::imagedb::ImageDBAccessor *accessor = NULL;
  accessor = GFL_NEW( pWorkHeap ) gfl2::imagedb::ImageDBAccessor( pWorkHeap, pWorkHeap, GameSys::GameManager::APLICATION_UNIQUE_ID );
  accessor->Initialize();

  // SDカードのチェック
  nResult = accessor->EstimateRemainPictureNum( out_result );

  // ImageDBの破棄
  accessor->Finalize();
  GFL_SAFE_DELETE( accessor );
  return nResult;
}

/**
 *  @brief  SDカードにカラーバッファを保存する＠ImageDB
 */
gfl2::imagedb::ImageDBAccessor::ResultCode FinderUtil::SaveColorBuffer( gfl2::imagedb::ImageDBAccessor::imgDbResult *out_result, gfl2::heap::HeapBase *pWorkHeap, void *pColorBuffer )
{
  // ImageDBの準備
  gfl2::imagedb::ImageDBAccessor *accessor = NULL;
  accessor = GFL_NEW( pWorkHeap ) gfl2::imagedb::ImageDBAccessor( pWorkHeap, pWorkHeap, GameSys::GameManager::APLICATION_UNIQUE_ID );
  accessor->Initialize();

  // Save
  gfl2::imagedb::ImageDBAccessor::ResultCode result_code = accessor->SaveToSDCard( pColorBuffer, NULL );

  switch( result_code )
  {
  case gfl2::imagedb::ImageDBAccessor::E_RET_SUCCESS:         // 成功
    break;
  case gfl2::imagedb::ImageDBAccessor::E_RET_OUT_OF_MEMORY:   // メモリ不足
    GFL_ASSERT_MSG( 0, "FinderUtil::SaveColorBuffer out of memory!!\n" );
    break;
  case gfl2::imagedb::ImageDBAccessor::E_RET_SDCARD_NOEXIST:  // SDカード未挿入
    break;
  case gfl2::imagedb::ImageDBAccessor::E_RET_SDCARD_READONLY: // SDカード書込み禁止
    break;
  case gfl2::imagedb::ImageDBAccessor::E_RET_FAILED:          // 失敗
    if( out_result )
    {
      *out_result = accessor->GetResultDetail();
    }
    break;
  }
  // ImageDBの破棄
  accessor->Finalize();
  GFL_SAFE_DELETE( accessor );

  return result_code;
}

/**
 *  @brief  次の機能解放までに必要な点数を取得する
 */
bool FinderUtil::CheckNextLevel( GameSys::GameManager *pGameManager, u32 *next_exp /*= NULL*/ )
{
  Savedata::PokeFinderSave *pSave = pGameManager->GetGameData()->GetPokeFinderSave();

  static const u32 nextExpTbl[] =
  {
    1500,    // 0 -> 1
    10000,   // 1 -> 2
    100000,  // 2 -> 3
    1500000, // 3 -> 4
    0,       // 4 -> error
  };
  u32 lv = pSave->GetCameraVersion();
  u32 exp = pSave->GetTotalValue();

  u32 next = 0;
  if( nextExpTbl[ lv ] > exp )
  {
    next = nextExpTbl[ lv ] - exp;
  }

  // 返却値の保存
  if( next_exp != NULL )
  {
    *next_exp = next;
  }

  // 必要経験値を満たしている+レベルが最大でないこと
  if( next <= 0 && lv < Savedata::PokeFinderSave::MAX_POKE_FINDER_LEVEL )
  {
    // レベルをあげられる
    return true;
  }

  // レベルは上がらない
  return false;
}

/**
 *  @brief  現在の機能解放状況の取得：ズーム
 */
bool FinderUtil::GetFinderSkillZoom( GameSys::GameManager *pGameManager, f32 *out_Maxi /*= NULL*/ )
{
  Savedata::PokeFinderSave *pSave = pGameManager->GetGameData()->GetPokeFinderSave();

  static const f32 zoomTbl[] =
  {
    1.0f, // 0
    2.0f, // 1
    3.0f, // 2
    4.0f, // 3
    4.0f, // 4
  };
  u32 lv = pSave->GetCameraVersion();

  f32 zoomMaxi = zoomTbl[lv];

  // 返却値の保存
  if( out_Maxi != NULL )
  {
    *out_Maxi = zoomMaxi;
  }

  // lvで解放するより返却値でチェックしたほうが安全か？
  return zoomMaxi > 1.0f;
}

/**
 *  @brief  現在の機能解放状況の取得：ポケモンを呼ぶ
 */
bool FinderUtil::GetFinderSkillCall( GameSys::GameManager *pGameManager )
{
  Savedata::PokeFinderSave *pSave = pGameManager->GetGameData()->GetPokeFinderSave();

  static const bool callTbl[] =
  {
    false,  // 0
    false,  // 1
    false,  // 2
    false,  // 3
    true,   // 4
  };
  u32 lv = pSave->GetCameraVersion();

  return callTbl[lv];
}

GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)

#endif
