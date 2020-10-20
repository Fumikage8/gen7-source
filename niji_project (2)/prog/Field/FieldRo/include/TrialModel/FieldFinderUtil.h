//======================================================================
/**
 * @file FieldFinderUtil.h
 * @date 2015/10/23 11:39:26
 * @author miyachi_soichi
 * @brief ファインダー用便利関数
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_FINDER_UTIL_H_INCLUDED__
#define __FIELD_FINDER_UTIL_H_INCLUDED__
#pragma once

#if defined(GF_PLATFORM_CTR)

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <imagedb/include/gfl2_ImageDBAccessor.h>
// Finder
#include "Field/FieldRo/include/TrialModel/FieldFinderModel_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)

/**
 *  @class  FinderUtil
 */
class FinderUtil
{
public:
  /**
   *  @brief  カラーバッファをJpegにエンコードする
   *  @param  pWorkHeap     作業用ヒープ
   *  @param  pColorBuffer  カラーバッファ
   *  @param  out_pData     出力先
   */
  static void EncodeJpeg( gfl2::heap::HeapBase *pWorkHeap, void *pColorBuffer, FinderPhotoJpeg *out_pData );

  /**
   *  @brief  Jpegをカラーバッファにデコードする
   *  @param  pWorkHeap         作業用ヒープ
   *  @param  pJpeg             出力先
   *  @param  out_pColorBuffer  カラーバッファ
   *  @param  format            カラーバッファのフォーマットを指定
   */
  static void DecodeColorBuffer( gfl2::heap::HeapBase *pWorkHeap, FinderPhotoJpeg *pJpeg, void *out_pColorBuffer, nn::jpeg::PixelFormat format );

  /**
   *  @brief  Jpegをサムネイルにデコードする
   *  @param  pWorkHeap         作業用ヒープ
   *  @param  pJpeg             出力先
   *  @param  out_pData         サムネイル
   */
  static void DecodeThumbnail( gfl2::heap::HeapBase *pWorkHeap, FinderPhotoJpeg *pJpeg, FinderPhotoThumbnail *out_pData );

  /**
   *  @brief  SDカードチェック
   *  @param  out_result  返却用リザルト
   *  @param  pWorkHeap   ImageDBで使用する作業用ヒープ
   *  @return SDカードへ保存できる残り枚数
   *
   *  @note   out_resultにはImageDBのエラーコードが保存される
   *          正常終了ならばResultSuccess、それ以外の場合撮影が不可能
   *          以下ImageDBアクセッサからの転載と追記
   *         ・ResultErrorOverNumImages
   *            ImageDBの扱えるデータ数を超えた
   *         ・ResultErrorFileOperationFailure
   *            ファイルのオープン、リード、ライトなどで予期せぬファイル操作失敗エラーが発生した
   *         ・ResultErrorNotExistStorage
   *            SDカードを認識していない
   *         ・ResultErrorWriteProtect
   *            SDカードにライトプロテクトがなされている
   *         ・ResultErrorInsufficientStorageSpace
   *            SDカードの空き容量不足で保存に失敗した
   *         ・ResultErrorReadOnlyDbFile
   *            管理ファイルが読み取り専用属性になっている
   *         ・ResultErrorNotAvailableStorage
   *            対応しないSDカードが挿入されている
   */
  static s32 CheckSDCard( gfl2::imagedb::ImageDBAccessor::imgDbResult *out_result, gfl2::heap::HeapBase *pWorkHeap );

  /**
   *  @brief  SDカードにカラーバッファを保存する＠ImageDB
   *  @param  out_result    返却用リザルト
   *  @param  pWorkHeap     作業用ヒープ
   *  @param  pColorBuffer  カラーバッファ
   *  @return リザルトコードを返却
   *
   *  @note   戻り値にgfl2のリザルトが入る
   *          E_RET_FAILEDの場合詳細が必要となるのでout_resultにて詳細を参照
   */
  static gfl2::imagedb::ImageDBAccessor::ResultCode SaveColorBuffer( gfl2::imagedb::ImageDBAccessor::imgDbResult *out_result, gfl2::heap::HeapBase *pWorkHeap, void *pColorBuffer );

  /**
   *  @brief  現在の得点で機能解放ができるか判定する
   *  @param  pGameManager  ゲームマネージャ
   *  @param  out_exp       次までの残り点数：不要の場合省略して問題ない
   *  @retval true          機能解放できる
   *  @retval false         機能解放できない
   */
  static bool CheckNextLevel( GameSys::GameManager *pGameManager, u32 *next_exp = NULL );

  /**
   *  @brief  現在の機能解放状況の取得：ズーム
   *  @param  pGameManager  ゲームマネージャ
   *  @param  out_Maxi      ズーム最大値：最大値が不要の場合、省略して問題ない
   *  @retval true          ズームが解放されている
   *  @retval false         ズームが解放されていない
   */
  static bool GetFinderSkillZoom( GameSys::GameManager *pGameManager, f32 *out_Maxi = NULL );

  /**
   *  @brief  現在の機能解放状況の取得：ポケモンを呼ぶ
   *  @param  pGameManager  ゲームマネージャ
   *  @retval true          解放されている
   *  @retval false         解放されていない
   */
  static  bool GetFinderSkillCall( GameSys::GameManager *pGameManager );

};

GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)

#endif // GF_PLATFORM_CTR
#endif // __FIELD_FINDER_UTIL_H_INCLUDED__
