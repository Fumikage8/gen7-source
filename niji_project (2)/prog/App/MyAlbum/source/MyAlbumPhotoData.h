//=============================================================================
/**
 * @file    MyAlbumPhotoData.h
 * @brief   マイアルバム画面写真データ管理
 * @author  Hiroyuki Nakamura
 * @date    2017.02.08
 */
//=============================================================================
#if !defined( MYALBUM_PHOTODATA_H_INCLUDED )
#define MYALBUM_PHOTODATA_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// pmlib
#include <pml/include/pmlib.h>
// momiji
#include "App/FinderStudioStatic/include/FinderStudioDefine.h"
#include "App/FinderStudioStatic/include/FinderStudioUtil.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Static)
struct IntegratedData;
class ImageDBUtil;
GFL_NAMESPACE_END(Static)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(ExtSavedata)
class FinderStudioExtSaveData;
GFL_NAMESPACE_END(ExtSavedata)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MyAlbum)

// 前方宣言
struct SORT_DATA;

//=============================================================================
/**
 * @class MyAlbumPhotoData
 * @brief マイアルバム画面写真管理クラス
 * @date  2017.02.08
 */
//=============================================================================
class MyAlbumPhotoData
{
  GFL_FORBID_COPY_AND_ASSIGN(MyAlbumPhotoData);   // コピーコンストラクタ＋代入禁止

public:
  static const u32 PHOTO_DATA_MAX = App::FinderStudio::Static::SAVE_PHOTO_MAX;  //!< 写真最大数

  static const u32 PHOTO_DATA_NONE = 0xffffffff;  //!< 写真データ無効値

  //! SDカードアクセス処理の戻り値
  enum SDCardAccessResult
  {
    SDC_ACCESS_RESULT_WAIT = 0,         //!< 実行中
    SDC_ACCESS_RESULT_OK,               //!< 正常終了
    SDC_ACCESS_RESULT_NG_FATAL,         //!< フェイタルエラー
    SDC_ACCESS_RESULT_NG_BROKEN,        //!< データ破損
    SDC_ACCESS_RESULT_NG_LACK_OF_SPACE, //!< SDカードの空き容量不足
    SDC_ACCESS_RESULT_NG_OVER_IMAGE,    //!< 写真保存枚数オーバー
    SDC_ACCESS_RESULT_NG_ETC,           //!< その他のエラー
  };

  //! 写真保存枚数チェックの戻り値
  enum ResultPhotoNum
  {
    RESULT_PHOTO_NUM_NORMAL = 0,  //!< 通常
    RESULT_PHOTO_NUM_WARNING,     //!< 保存可能枚数が少ない
    RESULT_PHOTO_NUM_MAX,         //!< 保存できない
  };


private:
  static const u32 THUMBNAIL_MAX = 6;   //!< サムネイル数

  static const u32 THUMBNAIL_BUFFER_SIZE = App::FinderStudio::Static::THUMB_BUFFER_SIZE;  //!< サムネイルバッファサイズ

  //! 写真データ
  struct PHOTO_DATA
  {
    App::FinderStudio::Static::IntegratedData read_data;  //!< 写真統合データ
    u32 index;  //!< インデックス
  };

  //! サムネイルデータ
  struct THUMBNAIL_DATA
  {
    void * buffer;  //!< バッファ
    u16 index;      //!< 写真インデックス
    u16 req;        //!< リクエスト
    u32 seq;        //!< シーケンス
  };

  static const u16 THUMBNAIL_NONE = 0xffff;   //!< サムネイル無効データ


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  MyAlbumPhotoData( app::util::Heap * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~MyAlbumPhotoData();

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   アップデート
   * @date    2017.02.09
   */
  //-----------------------------------------------------------------------------
  void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize
   * @brief   初期設定
   * @date    2017.02.09
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Initialize(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate
   * @brief   終了設定
   * @date    2017.02.09
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Terminate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsTerminate
   * @brief   終了できるか
   * @date    2017.02.09
   *
   * @retval  true  = 可
   * @retval  false = 不可
   */
  //-----------------------------------------------------------------------------
  bool IsTerminate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckImageDBError
   * @brief   イメージDBのエラーチェック
   * @date    2017.03.13
   *
   * @return  処理結果
   */
  //-----------------------------------------------------------------------------
  SDCardAccessResult CheckImageDBError(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsDataBroken
   * @brief   データが破損していたか
   * @date    2017.02.16
   *
   * @retval  true  = 破損
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsDataBroken(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitDataBrokenFlag
   * @brief   データ破損フラグを初期化
   * @date    2017.03.13
   */
  //-----------------------------------------------------------------------------
  void InitDataBrokenFlag(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadData
   * @brief   データ読み込み
   * @date    2017.02.08
   *
   * @return  処理結果
   */
  //-----------------------------------------------------------------------------
  SDCardAccessResult LoadData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadData
   * @brief   データ削除
   * @date    2017.02.08
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool UnloadData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckPhotoNum
   * @brief   写真保存枚数をチェック
   * @date    2017.03.13
   *
   * @return  処理結果
   */
  //-----------------------------------------------------------------------------
  ResultPhotoNum CheckPhotoNum(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SaveData
   * @brief   データセーブ
   * @date    2017.02.14
   *
   * @param   index   インデックス
   *
   * @return  処理結果
   */
  //-----------------------------------------------------------------------------
  SDCardAccessResult SaveData( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetData
   * @brief   写真データ取得
   * @date    2017.02.13
   *
   * @param   pos   データ位置
   *
   * @return  写真データ
   */
  //-----------------------------------------------------------------------------
  App::FinderStudio::Static::IntegratedData * GetData( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetDataIndex
   * @brief   インデックスから写真データ取得
   * @date    2017.02.15
   *
   * @param   index   SDカード上のインデックス
   *
   * @return  写真データ
   */
  //-----------------------------------------------------------------------------
  App::FinderStudio::Static::IntegratedData * GetDataIndex( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetIndexOrigin
   * @brief   写真データのSDカード上のインデックスを取得（全データ版）
   * @date    2017.02.27
   *
   * @param   pos   データ位置
   *
   * @return  写真データインデックス
   */
  //-----------------------------------------------------------------------------
  u32 GetIndexOrigin( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetIndex
   * @brief   写真データのSDカード上のインデックスを取得（絞込み済みデータ版）
   * @date    2017.02.14
   *
   * @param   pos   データ位置
   *
   * @return  写真データインデックス
   */
  //-----------------------------------------------------------------------------
  u32 GetIndex( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetDataCount
   * @brief   絞込み済みデータ数取得
   * @date    2017.02.08
   *
   * @return  データ数
   */
  //-----------------------------------------------------------------------------
  u32 GetDataCount(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetDataCountOrigin
   * @brief   全データ数取得
   * @date    2017.02.27
   *
   * @return  データ数
   */
  //-----------------------------------------------------------------------------
  u32 GetDataCountOrigin(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMonsNo
   * @brief   ポケモン番号取得
   * @date    2017.02.08
   *
   * @param   pos   データ位置
   *
   * @return  ポケモン番号
   */
  //-----------------------------------------------------------------------------
  MonsNo GetMonsNo( u32 pos ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetDate
   * @brief   日付取得
   * @date    2017.02.08
   *
   * @param   pos     データ位置
   * @param   yesr    年格納場所
   * @param   month   月格納場所
   * @param   day     日格納場所
   */
  //-----------------------------------------------------------------------------
  void GetDate( u32 pos, u32 * year, u32 * month, u32 * day ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFavoriteOrigin
   * @brief   お気に入りに登録されているか（全データ版）
   * @date    2017.02.27
   *
   * @param   pos   データ位置
   *
   * @retval  true  = 登録されている
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsFavoriteOrigin( u32 pos ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFavorite
   * @brief   お気に入りに登録されているか（絞込み済みデータ版）
   * @date    2017.02.08
   *
   * @param   pos   データ位置
   *
   * @retval  true  = 登録されている
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsFavorite( u32 pos ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    IsShareOrigin
   * @brief   共有されているか（全データ版）
   * @date    2017.02.27
   *
   * @param   pos   データ位置
   *
   * @retval  true  = 共有されている
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsShareOrigin( u32 pos ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    IsShare
   * @brief   共有されているか（絞込み済みデータ版）
   * @date    2017.02.08
   *
   * @param   pos   データ位置
   *
   * @retval  true  = 共有されている
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsShare( u32 pos ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFavoriteOrigin
   * @brief   お気に入り設定（全データ版）
   * @date    2017.02.27
   *
   * @param   pos   データ位置
   * @param   flg   true = 登録
   */
  //-----------------------------------------------------------------------------
  void SetFavoriteOrigin( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFavorite
   * @brief   お気に入り設定（絞込み済みデータ版）
   * @date    2017.02.08
   *
   * @param   pos   データ位置
   * @param   flg   true = 登録
   */
  //-----------------------------------------------------------------------------
  void SetFavorite( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetShareOrigin
   * @brief   共有設定（全データ版）
   * @date    2017.02.27
   *
   * @param   pos   データ位置
   * @param   flg   true = 共有
   */
  //-----------------------------------------------------------------------------
  void SetShareOrigin( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetShare
   * @brief   共有設定（絞込み済みデータ版）
   * @date    2017.02.08
   *
   * @param   pos   データ位置
   * @param   flg   true = 共有
   */
  //-----------------------------------------------------------------------------
  void SetShare( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetShareIndex
   * @brief   共有データのインデックスを取得
   * @date    2017.02.14
   *
   * @return  インデックス
   */
  //-----------------------------------------------------------------------------
  u32 GetShareIndex(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsDeco
   * @brief   飾りつけされているか
   * @date    2017.02.13
   *
   * @param   pos   データ位置
   *
   * @retval  true  = されている
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsDeco( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    ClearData
   * @brief   データクリア
   * @date    2017.01.26
   *
   * @param   data  写真データ
   */
  //-----------------------------------------------------------------------------
  void ClearData( PHOTO_DATA * data );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitDataRow
   * @brief   データの空きを詰める
   * @date    2017.01.26
   *
   * @param   param   ソートパラメータ
   */
  //-----------------------------------------------------------------------------
  void InitDataRow( const SORT_DATA * param );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteData
   * @brief   写真データ削除
   * @date    2017.02.13
   *
   * @param   pos   データ位置
   *
   * @return  処理結果
   */
  //-----------------------------------------------------------------------------
  SDCardAccessResult DeleteData( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadThumbnailRequest
   * @brief   サムネイル読み込みリクエスト
   * @date    2017.02.09
   *
   * @param   data_pos        写真データ位置
   * @param   thumbnail_pos   サムネイルデータ位置
   *
   * @retval  true  = 成功
   * @retval  false = 失敗
   */
  //-----------------------------------------------------------------------------
  bool LoadThumbnailRequest( u32 data_pos, u32 thumbnail_pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsLoadThumbnail
   * @brief   サムネイルデータの読み込みが完了したか
   * @date    2017.02.09
   *
   * @param   pos   サムネイルデータ位置
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsLoadThumbnail( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetThumbnailBuffer
   * @brief   サムネイルのバッファを取得
   * @date    2017.02.10
   *
   * @param   pos   サムネイルデータ位置
   *
   * @return  バッファ
   */
  //-----------------------------------------------------------------------------
  void * GetThumbnailBuffer( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckThumbnailLoadError
   * @brief   サムネイル読み込み時のエラーチェック
   * @date    2017.02.17
   *
   * @return  チェック結果
   */
  //-----------------------------------------------------------------------------
  SDCardAccessResult CheckThumbnailLoadError(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckMonsNoData
   * @brief   指定ポケモンの写真が存在するか
   * @date    2017.02.22
   *
   * @param   mons  ポケモン番号
   *
   * @retval  true  = 存在する
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CheckMonsNoData( MonsNo mons ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateSortData
   * @brief   写真データをソート
   * @date    2017.02.23
   *
   * @param   param   ソートパラメータ
   */
  //-----------------------------------------------------------------------------
  void CreateSortData( const SORT_DATA * param );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    CreateThumbnail
   * @brief   サムネイルデータ作成
   * @date    2017.02.09
   */
  //-----------------------------------------------------------------------------
  void CreateThumbnail(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteThumbnail
   * @brief   サムネイルデータ削除
   * @date    2017.02.09
   */
  //-----------------------------------------------------------------------------
  void DeleteThumbnail(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateImageDBUtil
   * @brief   イメージDB作成
   * @date    2017.02.09
   */
  //-----------------------------------------------------------------------------
  void CreateImageDBUtil(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateImageDBUtil
   * @brief   イメージDB削除
   * @date    2017.02.09
   */
  //-----------------------------------------------------------------------------
  void DeleteImageDBUtil(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadThumbnail
   * @brief   サムネイル読み込み（全体）
   * @date    2017.02.09
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadThumbnail(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadThumbnail
   * @brief   サムネイル読み込み（個別）
   * @date    2017.02.09
   *
   * @param   pos   サムネイルデータ位置
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadThumbnail( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsAddSortData
   * @brief   指定データがソート条件を満たすか
   * @date    2017.02.23
   *
   * @param   param   ソートパラメータ
   * @param   data    写真データ
   *
   * @retval  true  = 条件を満たす
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsAddSortData( const SORT_DATA * param, const PHOTO_DATA * data );

  //-----------------------------------------------------------------------------
  /**
   * @func    CalcDate
   * @brief   日付を32bitデータに変換
   * @date    2017.02.23
   *
   * @param   data    写真データ
   *
   * @return  変換した値
   *
   * @note  上から 23bit: 年, 4bit: 月, 4bit: 日
   */
  //-----------------------------------------------------------------------------
  u32 CalcDate( const PHOTO_DATA * data );

  //-----------------------------------------------------------------------------
  /**
   * @func    CalcTime
   * @brief   時間を32bitデータに変換
   * @date    2017.02.23
   *
   * @param   data    写真データ
   *
   * @return  変換した値
   *
   * @note  上から 20bit: 時, 6bit: 分, 6bit: 秒
   */
  //-----------------------------------------------------------------------------
  u32 CalcTime( const PHOTO_DATA * data );


private:
  app::util::Heap * m_heap;   //!< ヒープ

  App::FinderStudio::Static::FinderStudioExtSaveDataAccessor * m_pExtSaveData;  //!< 写真セーブデータ
  u32 m_extDataCount;   //!< 写真セーブデータ数  

  //App::FinderStudio::Static::FinderStudioUtil::Matching * m_pMatching;  //!< 整合性チェック

  App::FinderStudio::Static::IntegratedData*    m_pReadDataArray;
  SDCardAccessResult m_readDataLoadResult;
  u32 m_saveDataIndex;

  PHOTO_DATA m_photoData[PHOTO_DATA_MAX];   //!< 写真データ本体
  u32 m_photoDataMax;                       //!< 写真データ数
  u32 m_readDataCount;                      //!< 読み込みデータカウント

  PHOTO_DATA * m_sortData[PHOTO_DATA_MAX];  //!< 写真データ（絞込み対応）
  u32 m_sortDataMax;

  App::FinderStudio::Static::ImageDBUtil * m_pImageDBUtil;  //!< イメージDB

  THUMBNAIL_DATA m_pThumbnail[THUMBNAIL_MAX];   //!< サムネイルデータ
  u32 m_thumbnailLoadIndex;                     //!< 読み込み中のサムネイルインデックス
  SDCardAccessResult m_thumbnailLoadResult;     //!< サムネイル読み込みエラー格納場所

  u32 m_mainSeq;  //!< メインシーケンス

  bool m_isBroken;  //!< true = データが破損している
};


GFL_NAMESPACE_END(MyAlbum)
GFL_NAMESPACE_END(App)


#endif  // MYALBUM_PHOTODATA_H_INCLUDED
