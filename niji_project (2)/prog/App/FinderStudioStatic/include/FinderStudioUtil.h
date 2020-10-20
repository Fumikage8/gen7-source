//============================================================================================
/**
 * @file    FinderStudioUtil.h
 *
 * @author ichiraku_katsuhiko
 */
//============================================================================================

#ifndef __FINDER_STUDIO_UTIL_H__
#define __FINDER_STUDIO_UTIL_H__
#pragma once

// ファインダースタジオ
#include "App/FinderStudioStatic/include/FinderStudioDefine.h"
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"
#include "App/FinderStudioStatic/include/FinderStudioErrorConverter.h"

// 拡張セーブデータ
#include <ExtSavedata/include/FinderStudioExtSaveData.h>

#include <heap/include/gfl2_Heap.h>

// 前方宣言
GFL_NAMESPACE_BEGIN( ExtSavedata )
class FinderStudioExtSaveData;
GFL_NAMESPACE_END( ExtSavedata )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

// 前方宣言
class ImageDBUtil;

/**
 * @brief FinderStudio用ユーティリティクラス。<p>
 *        FinderStudioで汎用的に使われる機能をstatic関数として提供する。
 */
class FinderStudioUtil
{
private:
  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief    コンストラクタ(隠蔽化)
   */
  /* ----------------------------------------------------------------------------------------- */
  FinderStudioUtil( void );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief    デストラクタ(隠蔽化)
   */
  /* ----------------------------------------------------------------------------------------- */
  ~FinderStudioUtil(void);

public:


  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief       写真データのCRC一致チェック
   * @param[in]   pPhotoData   検査する写真データ
   *
   * @return trueで正常  falseで不正
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool CheckPhotoDataCrc( const App::FinderStudio::Static::PhotoData* pPhotoData );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief       写真データのCRC取得
   * @param[in]   pPhotoData   写真データ
   *
   * @return crc
   */
  /* ----------------------------------------------------------------------------------------- */
  static u32 GetPhotoDataCrc( const App::FinderStudio::Static::PhotoData* pPhotoData );
};




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファインダースタジオ用拡張セーブデータアクセサ
///
//////////////////////////////////////////////////////////////////////////
class FinderStudioExtSaveDataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderStudioExtSaveDataAccessor );

public:
  /// リザルト
  enum Result
  {
    RESULT_CONTINUE,    //!< 処理中
    RESULT_SUCCESS,     //!< 終了：成功
    RESULT_FAILED,      //!< 終了：失敗
    RESULT_NOT_EXIST,   //!< 終了：ファイルなし
    RESULT_BROKEN,      //!< 終了：破損あり
    RESULT_NUM,
  };

private:
  /// 保存データのサイズ
  static const u32 SAVE_DATA_SIZE = ExtSavedata::FinderStudioExtSaveData::SAVE_DATA_SIZE;
  /// データファイル(保存データ×420)のサイズ
  static const u32 DATA_FILE_SIZE = ExtSavedata::FinderStudioExtSaveData::DATA_FILE_SIZE;
  /// 保存データのインデックス：１つになったので固定
  static const u32 SAVE_FILE_INDEX = 0;
public:
  /// 保存データの最大数
  static const u32 SAVE_DATA_MAX = Static::SAVE_PHOTO_MAX;

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  FinderStudioExtSaveDataAccessor( gfl2::heap::HeapBase* heap );
  ~FinderStudioExtSaveDataAccessor( void );

public:
  //-------------------------------------------------------
  // @brief   ファイルアクセス
  //-------------------------------------------------------
  Result ReadFile( IntegratedData* dstDataArray = NULL );
  Result WriteFile( IntegratedData* srcData, const u32 dataIndex = SAVE_DATA_MAX );
  Result DeleteData( const u32 dataIndex );

#if PM_DEBUG
  /// デバッグ用：データファイル削除
  Result DeleteDataFile( void );
  /// データカウントの設定
  void SetDataIndex( void );
#endif

  //-------------------------------------------------------
  // @brief   取得関数
  //-------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result GetResult( void ) const { return m_Result; }

  u32 GetDataCount( void ) const { return m_DataCount; }
  u32 GetDataIndex( u32 id ) const { return m_DataIndex[id]; }

private:
  //-------------------------------------------------------
  // @brief   ファイルアクセス(同期)：非公開
  //-------------------------------------------------------
  bool _FileExist_Sync( u32 fileIndex );
  bool _FileRead_Sync( u32 fileIndex, IntegratedData* dstData );
  bool _FileDelete_Sync( u32 fileIndex );

  //-------------------------------------------------------
  // @brief   ファイルアクセス(非同期)：非公開
  //-------------------------------------------------------
  bool _FileExist_Async( u32 fileIndex );
  bool _FileRead_Async( u32 fileIndex, IntegratedData* dstData );
  bool _FileWrite_Async( u32 fileIndex, const IntegratedData* srcData );
  bool _FileCreate_Async( u32 fileIndex );
  bool _FileDelete_Async( u32 fileIndex );

  //-------------------------------------------------------
  // @brief   ファインダースタジオ用拡張セーブデータ 生成・破棄：非公開
  //-------------------------------------------------------
  void _CreateFinderStudioExtSaveData( void );
  void _DeleteFinderStudioExtSaveData( void );

  //-------------------------------------------------------
  // @brief   設定・取得：非公開
  //-------------------------------------------------------
  void _SetWriteData( IntegratedData* srcData, const u32 dataIndex );
  void _SetDataIndex( void );

  void _SetFileExistFlg( bool exist ) { m_bFileExist = exist; }
  bool _GetFileExistFlg( void ) const { return m_bFileExist; }

  void _SetFileBrokenFlg( bool broken ) { m_bFileBroken = broken; }
  bool _GetFileBrokenFlg( void ) const { return m_bFileBroken; }

  void _SetDataUpdateFlg( bool update ) { m_bDataUpdate = update; }
  bool _GetDataUpdateFlg( void ) const { return m_bDataUpdate; }

  //-------------------------------------------------------
  // @brief   チェック関数：非公開
  //-------------------------------------------------------
  bool _IsFileResultSuccess( void ) const;
  bool _IsFileResultBroken( void ) const;
  bool _IsFileResultNotExist( void ) const;

  bool _IsExistSaveData( IntegratedData* data );
  bool _IsMatchFilepath( IntegratedData* data );
  bool _IsMatchCRC( IntegratedData* data );

  //-------------------------------------------------------
  // @brief   シーケンス：非公開
  //-------------------------------------------------------
  void _SetMainSeq( u8 mainSeq );
  void _SetSubSeq( u8 subSeq );

  //-------------------------------------------------------
  // @brief   リセット：非公開
  //-------------------------------------------------------
  void _ResetData_AllSaveData( void );
  void _ResetData_SaveData( IntegratedData* data );

  void _ResetData_AllDataIndex( void );

private:
  gfl2::heap::HeapBase*   m_pHeap;

  ExtSavedata::FinderStudioExtSaveData*   m_pExtSaveData;
  
  IntegratedData  m_Data[SAVE_DATA_MAX];

  gfl2::fs::ExtSaveFile::Result       m_Result;

  u8      m_MainSeq;
  u8      m_SubSeq;

  u32     m_DataCount;
  u32     m_DataIndex[SAVE_DATA_MAX];

  bool    m_bFileExist;
  bool    m_bFileBroken;
  bool    m_bDataUpdate;
};

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )




#endif // __FINDER_STUDIO_UTIL_H__

