//================================================================================
/**
 * @file   gfl2_ImageDBSearcher.h
 * @brief  ImageDBを利用したSDカード内写真の検索
 *         ImageDBAccessorの「Initalize()～Finalize()期間中」でのみ使用可能
 * @author hisanao suzuki
 * @date   2012.2.13
 * @date   2015.11.30 niji用に移植 takahashi tomoya 
 */
//================================================================================
#ifndef __GFL2_IMAGE_DB_SEARCHER_H__
#define __GFL2_IMAGE_DB_SEARCHER_H__
#pragma once

#include <imagedb/include/gfl2_ImageDBTypes.h>

#include <system/include/Date/gfl2_Date.h>

#if GFL2_ENABLE_IMAGE_DB

namespace gfl2 {
/**
* @namespace gfl2::imagedb
* @brief ImageDB用ネームスペース
*/
namespace imagedb {

//-------------------------------------------------------------------------------
/**
 * @brief SD写真画像情報
 * @note 下記の構造体にアクセルする処理は、GF_PLATFORM_CTRでくくってください。
 */
//-------------------------------------------------------------------------------
typedef struct
{
#ifdef GF_PLATFORM_CTR

  wchar_t                   m_path[imgdb::imgio::c_nMaxPathSize];  //!< ファイルパス
  u32                       m_uniqueId;                            //!< タイトルユニークID
  imgdb::imgio::ImageKind   m_kind;                                //!< 画像種別
  nn::fnd::DateTime         m_date;                                //!< 取得画像
#elif defined(GF_PLATFORM_WIN32)
  u32 dummy;
#endif
}ImageDBPictureInfo;



//-------------------------------------------------------------------------------
/**
* @brief ImageDB写真検索用クラス
*/
//-------------------------------------------------------------------------------
class ImageDBSearcher
{
  GFL_FORBID_COPY_AND_ASSIGN( ImageDBSearcher );  // コピーを禁止

public:
  /**
   * @breif リザルトコード
   */
  typedef enum
  {
     E_RET_SUCCESS,           //!< 成功
     E_RET_NOT_INIT,          //!< 未初期化
     E_RET_ILLIGAL_ARGUMENT,  //!< 引数不正
     E_RET_INDEXOUT,          //!< 指定バッファオーバー
  }ResultCode;

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-------------------------------------------------------------------------------
  ImageDBSearcher( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-------------------------------------------------------------------------------
  virtual ~ImageDBSearcher();

  //-------------------------------------------------------------------------------
  /**
   * @brief 写真検索機能の初期化処理（開始時に１度だけコール）
   */
  //-------------------------------------------------------------------------------
  void Initialize( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief 写真検索機能の終了処理（終了時に１度だけコール）
   */
  //-------------------------------------------------------------------------------
  void Finalize( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief  写真検索情報の更新
   *
   * @note   Initialize()にて内部で１度コールされる。
   *
   * @return 実行結果
   */
   //-------------------------------------------------------------------------------
  ResultCode SearchPicture( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief  SearchPicture()の結果、SDカードの写真枚数を取得する
   *
   * @param[in]  filterId フィルタリングするアプリID。0を指定した場合はフィルタなし
   *
   * @return SDカードから発見された写真枚数
   */
   //-------------------------------------------------------------------------------
  int GetResultNum( u32 filterId = 0 );

  //-------------------------------------------------------------------------------
  /**
   * @brief  SDカード内の写真情報を取得する
   *
   * @param[in]   offset   取得開始オフセット( 0 ～ GetResultNum()-1 )
   * @param[in]   num      取得情報数(1 ～ GetResultNum())
   * @param[out]  outBuf   情報格納先バッファ
   * @param[in]   filterId フィルタリングするアプリID。0を指定した場合はフィルタなし
   *
   * @return 実行結果
   */
   //-------------------------------------------------------------------------------
  ResultCode GetInfo( u32 offset, u32 num, ImageDBPictureInfo* outBuf, u32 filterId = 0 );

  //-------------------------------------------------------------------------------
  /**
   * @brief  SDカード内の写真情報を取得する
   *
   * @param[in]   num      取得情報数(1 ～ GetResultNum())
   * @param[out]  outBuf   情報格納先バッファ
   * @param[in]   filterId フィルタリングするアプリID。0を指定した場合はフィルタなし
   *
   * @return 実行結果
   *
   * @fix MMCat[319]
   */
  //-------------------------------------------------------------------------------
  ResultCode GetInfo( u32 num, ImageDBPictureInfo* outBuf, u32 filterId = 0 );


private:
  //-------------------------------------------------------------------------------
  /**
   * @brief   写真情報の設定
   *
   * @param[out]  outBuf          情報格納先バッファ
   * @param[in]   photoIndex      写真インデックス
   * @param[in]   titleUniqueID   タイトルユニークID
   */
  //-------------------------------------------------------------------------------
  void _SetPictureInfo( ImageDBPictureInfo* outBuf, u32 photoIndex, u32 titleUniqueID );

  //-------------------------------------------------------------------------------
  /**
   * @brief   タイトルユニークIDが有効か
   *
   * @param[in]   titleUniqueID   タイトルユニークID
   * @param[in]   filterID        フィルタリングID
   *
   * @retval  "true"なら有効なタイトルユニークID
   * @retval  "false"なら無効なタイトルユニークID
   */
  //-------------------------------------------------------------------------------
  bool _IsAvailableTitleUniqueID( u32 titleUniqueID, u32 filterID );

private:
  bool                       m_init;  //!< 初期化フラグ
};

} // namespace imagedb
} // namespace gfl2


#endif
#endif

