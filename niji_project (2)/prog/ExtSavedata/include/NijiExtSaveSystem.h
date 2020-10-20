
#ifndef __XY_EXTSAVESYSTEM_H__
#define __XY_EXTSAVESYSTEM_H__
//==============================================================================
/**
 * @file    NijiExtSaveSystem.h
 * @brief   拡張セーブロードのコントロールを行うクラス
 * @author  tamada
 * @date    2012.09.25
 */
//==============================================================================
#pragma once
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <fs/include/CTR/gfl2_FsCtrExtSaveSystem.h>

#include "ExtSavedata/include/NijiExtSaveData.h"
#include "ExtSavedata/include/NijiExtSaveSystemListener.h"

#if defined(GF_PLATFORM_WIN32)
namespace gfl2{ namespace fs{ 
class ExtSaveFile{
public:
  enum Result {
    ACCESS_RESULT_OK,                     // 成功
    ACCESS_RESULT_NG_PATH_NOT_EXIST,      // 失敗：指定したパスが存在しない
    ACCESS_RESULT_NG_PATH_ALREADY_EXIST,  // 失敗：指定したパスが既に存在する
    ACCESS_RESULT_NG_BROKEN_DATA,         // 失敗：データが壊れているか, 改竄されている
    ACCESS_RESULT_NG_ERROR,               // 失敗：接触不良など, ハードウェア要因
    ACCESS_RESULT_NG_LACK_OF_SPACE,       // 失敗：メディアの空き容量不足
    ACCESS_RESULT_NG_WRITE_PROTECTED,     // 失敗：ライトプロテクトされている
    ACCESS_RESULT_NG_MEDIA_NOT_FOUND,     // 失敗：SDカードが挿されていない
    ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR,  // 失敗：接触不良など
    ACCESS_RESULT_NG_ARCHIVE_INVALIDATED, // 失敗：アーカイブ無効（SDカード抜き等）
    ACCESS_RESULT_NG_UNKNOWN,             // 失敗：想定外のエラー
  };
};
class ExtSaveSystem{
public:
  enum Result {
    RESULT_OK,                      ///<成功
    RESULT_CONTINUE,
    RESULT_NG_NOT_FOUND,            ///<失敗：拡張セーブデータが存在しない
    RESULT_NG_MEDIA_NOT_FOUND,      ///<失敗：SDカードが存在しない、あるいは認識できない
    RESULT_NG_BAD_FORMAT,           ///<失敗：SDカードのフォーマットが必要
    RESULT_NG_ERROR,                ///<失敗：SDカードの接触不良等
    RESULT_NG_WRITE_PROTECTED,      ///<失敗：SDカードあるいはファイルが書き込み禁止になっている
    RESULT_NG_NOT_FORMATTED,        ///<失敗：拡張セーブデータが作成されていない
    RESULT_NG_NOT_ENOUGH_SPACE,     ///<失敗：SDカードに書き込み容量が足りない
    RESULT_NG_VERIFICATION_FAILED,  ///<失敗：セーブデータ破壊or改ざん
    RESULT_NG_MOUNTING,             ///<失敗：マウント中（消去時のみのエラー）
    RESULT_NG_UNKNOWN,              ///<失敗：想定外のエラー
  };
};
}}

namespace nn{ namespace fs{ 
typedef int DirectoryEntry;
enum
{
  MAX_FILE_PATH_LENGTH = 1024,
};
}}

#endif


GFL_NAMESPACE_BEGIN(ExtSavedata)

  class ExtSaveLoadControlThread;
  class NijiExtSaveData;

  struct EXTFILE_PARAM;

//--------------------------------------------------------------
/**
 * @class NijiExtSaveSystem
 */
//--------------------------------------------------------------
class NijiExtSaveSystem{
  GFL_FORBID_COPY_AND_ASSIGN(NijiExtSaveSystem); //コピーコンストラクタ＋代入禁止

  friend class ExtSaveLoadControlThread;

public:
  enum EXTARC_STATUS {
    EXTARC_STATUS_OK = 0,           ///<正常、存在している
    EXTARC_STATUS_NEED_CREATE,      ///<正常、新規作成の必要がある
    EXTARC_STATUS_INCORRECT,        ///<異常、削除→再作成の必要がある
    EXTARC_STATUS_MEDIA_NOT_FOUND,  ///<異常、SDカード認識関連
    EXTARC_STATUS_ERROR,            ///<異常、継続できない
  };

  //--------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //--------------------------------------------------------------
  NijiExtSaveSystem(gfl2::heap::HeapBase* heap);

  //--------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //--------------------------------------------------------------
  ~NijiExtSaveSystem();

  //--------------------------------------------------------------
  /**
   * @brief   エラーが致命的か？の判定
   * @param[in]   result
   *
   * @note  ゲーム中はSDカード必須なので、起動時に事前チェックがある。
   * @note  アプリ内でSDカード無効エラーが発生した場合、致命的エラーとする
   */
  //--------------------------------------------------------------
  static bool IsFatalError( gfl2::fs::ExtSaveFile::Result result );

  //--------------------------------------------------------------
  /**
   * @brief   ResultMediaNotFoundでFATALERRORを起こす
   * @note  「SDカードが抜かれました」のエラー画面に遷移する
   */
  //--------------------------------------------------------------
  static void ThrowMediaNotFoundError( void );

  //=============================================================================
  //
  //
  //        同期処理（メインプロセスをロックする）
  //
  //        暗転中初期化処理などで使用する
  //
  //
  //=============================================================================
  //--------------------------------------------------------------
  /**
   * @brief
   * @param[in]   pTempHeap
   * @return      EXTARC_STATUS
   */
  //--------------------------------------------------------------
  EXTARC_STATUS CheckArchiveStatus( gfl2::heap::HeapBase * pTempHeap );

  //--------------------------------------------------------------
  /**
   * @brief   拡張セーブデータ領域を使用可能にする
   */
  //--------------------------------------------------------------
  void BootProcess( gfl2::heap::HeapBase * pTempHeap );

  //--------------------------------------------------------------
  /**
   * @brief   拡張セーブデータ領域の削除
   *
   * @caution 回復不能なエラー状況でもない限り使用しません
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveSystem::Result DeleteArchive( void );

  //--------------------------------------------------------------
  /**
   * @brief     ファイル作成（パス指定版）
   * @param[in]   path    作成するファイルへのパス
   * @param[in]   size    作成するファイルのサイズ
   * @return  gfl2::fs::ExtSaveFile::Result
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result CreateExtFile( const wchar_t * path, size_t size );

  //--------------------------------------------------------------
  /**
   * @brief     ファイル作成（ファイルID指定版）
   * @param[in]   id          ディレクトリ指定ID
   * @param[in]   fileindex   ファイル指定インデックス
   * @param[in]   size        作成するファイルのサイズ
   *
   * @return  gfl2::fs::ExtSaveFile::Result
   * @note  sizeが0のとき、各企画で定めた最大サイズを指定します
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result CreateExtFile(
      NIJI_EXTSAVE_ID id, int fileindex, size_t size = 0 );

  //--------------------------------------------------------------
  /**
   * @brief     ファイル削除（パス指定版）
   * @param[in]   path    削除するファイルへのパス
   * @return  gfl2::fs::ExtSaveFile::Result
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result DeleteExtFile( const wchar_t * path );

  //--------------------------------------------------------------
  /**
   * @brief     ファイル削除（ファイルID指定版）
   * @param[in]   path    削除するファイルへのパス
   * @return  gfl2::fs::ExtSaveFile::Result
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result DeleteExtFile( NIJI_EXTSAVE_ID id, int fileindex );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル読み込み（パス指定版）
   *
   * @param[in]   path      読み込むファイルへのパス
   * @param[out]  dst       保存先バッファへのポインタ
   * @param[in]   size      読み込むファイルのサイズ
   *
   * @return  gfl2::fs::ExtSaveFile::Result
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result ReadFile(
      const wchar_t * path, void * dst, size_t size );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル読み込み（ファイルID指定版）
   *
   * @param[in]   id          ディレクトリ指定ID
   * @param[in]   fileindex   ファイル指定インデックス
   * @param[out]  dst         保存先バッファへのポインタ
   * @param[in]   size        作成するファイルのサイズ
   *
   * @return  gfl2::fs::ExtSaveFile::Result
   *
   * @note  sizeが0のとき、各企画で定めた最大サイズを指定します
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result ReadFile(
      NIJI_EXTSAVE_ID id, int fileindex, void * dst, size_t size = 0 );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル書込み（パス指定版）
   *
   * @param[in]   path      書き込むファイルへのパス
   * @param[in]   src       書き込むデータへのポインタ
   * @param[in]   size      書き込むデータのサイズ
   *
   * @return  gfl2::fs::ExtSaveFile::Result
   *
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result WriteFile(
      const wchar_t * path, const void * src, size_t size );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル書込み（ファイルID指定版）
   *
   * @param[in]   id          ディレクトリ指定ID
   * @param[in]   fileindex   ファイル指定インデックス
   * @param[in]   src         書き込むデータへのポインタ
   * @param[in]   size        書き込むデータのサイズ
   *
   * @return  gfl2::fs::ExtSaveFile::Result
   *
   * @note  sizeが0のとき、各企画で定めた最大サイズを指定します
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result WriteFile(
      NIJI_EXTSAVE_ID id, int fileindex, const void * src, size_t size = 0 );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル名変更（パス指定版）
   *
   * @param[in]   current_name  ファイル指定インデックス
   * @param[in]   new_name      書き込むデータへのポインタ
   *
   * @return  gfl2::fs::ExtSaveFile::Result
   *
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result RenameFile(
      const wchar_t * current_name, const wchar_t * new_name );
  //--------------------------------------------------------------
  /**
   * @brief   ファイル名変更（ファイルID指定版）
   *
   * @param[in]   id              ディレクトリ指定ID
   * @param[in]   current_index   ファイル指定インデックス
   * @param[in]   new_index       書き込むデータへのポインタ
   *
   * @return  gfl2::fs::ExtSaveFile::Result
   *
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result RenameFile(
      NIJI_EXTSAVE_ID id, int current_index, int new_index );

  //--------------------------------------------------------------
  /**
   * @brief   ファイルの存在チェック（パス指定版）
   *
   * @param[in]   path  チェックするファイルパス
   * @return      bool  存在するとき、true
   *
   * @attention    ファイル数、サイズなどの検証はアプリ側で対応
   * @attention    壊れていた場合の処理もアプリ側で対応
   */
  //--------------------------------------------------------------
  bool IsFileExists( const wchar_t * path );

  //--------------------------------------------------------------
  /**
   * @brief   ファイルの存在チェック（ファイルID指定版）
   * @param[in]   id          ディレクトリ指定ID
   * @param[in]   fileindex   ファイル指定インデックス
   * @retval  true    ファイルが存在する
   * @retval  false   ファイルが存在しない
   */
  //--------------------------------------------------------------
  bool IsFileExists( NIJI_EXTSAVE_ID id, int fileindex );

  //--------------------------------------------------------------
  /**
   * @brief ディレクトリ情報の読み取り
   * @param[in]   pHeap       使用するテンポラリヒープ
   * @param[in]   id          ディレクトリ指定ID
   * @param[out]  pReadNum    読み込んだエントリの数
   * @param[out]  pEntries    読み込むエントリ領域へのポインタ
   * @param[in]   numEntries  読み込む上限
   *
   * @note
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result ReadDirectory(
      gfl2::heap::HeapBase * pHeap,
      NIJI_EXTSAVE_ID id, s32 * pReadNum,
      nn::fs::DirectoryEntry * pEntries, s32 numEntries );

  //=============================================================================
  //
  //
  //        非同期リクエスト
  //
  //
  //=============================================================================
  //--------------------------------------------------------------
  /**
   * @brief   システム起動処理の非同期開始
   */
  //--------------------------------------------------------------
  bool StartBootProcess( gfl2::heap::HeapBase * pHeap );

  //--------------------------------------------------------------
  /**
   * @brief   システム起動処理待ち
   */
  //--------------------------------------------------------------
  bool WaitBootProcess( void );

  //--------------------------------------------------------------
  /**
   * @brief     システムが起動しているかどうかの判定
   */
  //--------------------------------------------------------------
  bool IsBooted( void ) const { return m_bBooted; }

  //--------------------------------------------------------------
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveSystem::Result GetBootResult( void ) const;

  //--------------------------------------------------------------
  /**
   * @brief   ファイル作成の非同期処理開始（パス指定版）
   * @param[in]   pHeap   使用するテンポラリヒープ
   * @param[in]   path    作成するファイルへのパス
   * @param[in]   size    作成するファイルのサイズ
   * @retval  true    非同期処理リクエスト成功
   * @retval  false   リクエスト失敗
   */
  //--------------------------------------------------------------
  bool StartCreateFile(
      gfl2::heap::HeapBase *pHeap, const wchar_t * path, size_t size );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル作成の非同期処理開始（ファイルID指定版）
   *
   * @param[in]   pHeap       使用するテンポラリヒープ
   * @param[in]   id          ディレクトリ指定ID
   * @param[in]   fileindex   ファイル指定インデックス
   * @param[in]   size        作成するファイルのサイズ
   *
   * @return  bool    非同期処理リクエストの可否
   * @note  sizeが0のとき、各企画で定めた最大サイズを指定します
   */
  //--------------------------------------------------------------
  bool StartCreateFile(
      gfl2::heap::HeapBase *pHeap,
      NIJI_EXTSAVE_ID id, int fileindex, size_t size = 0 );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル作成の非同期処理待ち
   * @param[out]  result    結果を受け取るポインタ
   * @retval      true      非同期処理完了
   * @retval      false     処理中
   */
  //--------------------------------------------------------------
  bool WaitCreateFile( gfl2::fs::ExtSaveFile::Result * result );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル削除の非同期処理開始（パス指定版）
   * @param[in]   pHeap   使用するテンポラリヒープ
   * @param[in]   path    削除するファイルへのパス
   * @retval  true    非同期処理リクエスト成功
   * @retval  false   リクエスト失敗
   */
  //--------------------------------------------------------------
  bool StartDeleteFile(
      gfl2::heap::HeapBase *pHeap, const wchar_t * path );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル削除の非同期処理開始（ファイルID指定版）
   *
   * @param[in]   pHeap       使用するテンポラリヒープ
   * @param[in]   id          ディレクトリ指定ID
   * @param[in]   fileindex   ファイル指定インデックス
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //--------------------------------------------------------------
  bool StartDeleteFile(
      gfl2::heap::HeapBase *pHeap, NIJI_EXTSAVE_ID id, int fileindex );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル削除の非同期処理待ち
   * @param[out]  result    結果を受け取るポインタ
   * @retval      true      非同期処理完了
   * @retval      false     処理中
   */
  //--------------------------------------------------------------
  bool WaitDeleteFile( gfl2::fs::ExtSaveFile::Result * result );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル読み込みの非同期処理開始（パス指定版）
   *
   * @param[in]   pHeap     使用するテンポラリヒープ
   * @param[in]   path      読み込むファイルへのパス
   * @param[out]  dst       保存先バッファへのポインタ
   * @param[in]   size      読み込むファイルのサイズ
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //--------------------------------------------------------------
  bool StartReadFile( gfl2::heap::HeapBase * pHeap,
    const wchar_t * path, void * dst, size_t size );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル読み込みの非同期処理開始（ファイルID指定版）
   *
   * @param[in]   pHeap       使用するテンポラリヒープ
   * @param[in]   id          ディレクトリ指定ID
   * @param[in]   fileindex   ファイル指定インデックス
   * @param[out]  dst         保存先バッファへのポインタ
   * @param[in]   size        作成するファイルのサイズ
   * @return  bool    非同期処理リクエストの可否
   *
   * @note  sizeが0のとき、各企画で定めた最大サイズを指定します
   */
  //--------------------------------------------------------------
  bool StartReadFile( gfl2::heap::HeapBase * pHeap,
    NIJI_EXTSAVE_ID id, int fileindex, void * dst, size_t size = 0 );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル読み込みの非同期初期化処理待ち
   * @param[out]  result    結果を受け取るポインタ
   * @retval      true      非同期処理完了
   * @retval      false     処理中
   */
  //--------------------------------------------------------------
  bool WaitReadFile( gfl2::fs::ExtSaveFile::Result * result );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル書込みの非同期処理開始（パス指定版）
   *
   * @param[in]   pHeap     使用するテンポラリヒープ
   * @param[in]   path      書き込むファイルへのパス
   * @param[in]   src       書き込むデータへのポインタ
   * @param[in]   size      書き込むデータのサイズ
   * @return  bool    非同期処理リクエストの可否
   *
   */
  //--------------------------------------------------------------
  bool StartWriteFile( gfl2::heap::HeapBase * pHeap,
    const wchar_t * path, const void * src, size_t size );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル書込みの非同期処理開始（ファイルID指定版）
   *
   * @param[in]   pHeap       使用するテンポラリヒープ
   * @param[in]   id          ディレクトリ指定ID
   * @param[in]   fileindex   ファイル指定インデックス
   * @param[in]   src         書き込むデータへのポインタ
   * @param[in]   size        書き込むデータのサイズ
   * @return  bool    非同期処理リクエストの可否
   *
   * @note  sizeが0のとき、各企画で定めた最大サイズを指定します
   */
  //--------------------------------------------------------------
  bool StartWriteFile( gfl2::heap::HeapBase * pHeap,
    NIJI_EXTSAVE_ID id, int fileindex, const void * src, size_t size = 0 );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル書込みの非同期処理待ち
   * @param[out]  result    結果を受け取るポインタ
   * @retval      true      非同期処理完了
   * @retval      false     処理中
   */
  //--------------------------------------------------------------
  bool WaitWriteFile( gfl2::fs::ExtSaveFile::Result * result );

  //--------------------------------------------------------------
  /**
   * @brief   セーブ終了処理（電源切断時用）
   * @retval  true  セーブ処理終了
   * @retval  false セーブ処理継続中
   *
   * @note  電源切断時に呼び出される
   */
  //--------------------------------------------------------------
  bool FinalizeSaveForShutdown( void );

  //--------------------------------------------------------------
  /**
   * @brief   セーブ終了待ち（スリープ用）
   */
  //--------------------------------------------------------------
  void WaitSaveForSleep( void );

  //--------------------------------------------------------------
  /**
   * @brief   ファイルの存在チェックの非同期開始（パス指定版）
   *
   * @param[in]   pHeap       使用するテンポラリヒープ
   * @param[in]   path        ファイルへのパス
   *
   */
  //--------------------------------------------------------------
  bool StartIsFileExists( gfl2::heap::HeapBase * pHeap, const wchar_t * path );

  //--------------------------------------------------------------
  /**
   * @brief   ファイルの存在チェックの非同期開始（ファイルID指定版）
   * @param[in]   pHeap       使用するテンポラリヒープ
   * @param[in]   id          ディレクトリ指定ID
   * @param[in]   fileindex   ファイル指定インデックス
   * @retval  true    ファイルが存在する
   * @retval  false   ファイルが存在しない
   */
  //--------------------------------------------------------------
  bool StartIsFileExists( gfl2::heap::HeapBase * pHeap,
      NIJI_EXTSAVE_ID id, int fileindex );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル存在チェックの非同期処理待ち
   * @param[out]  result    結果を受け取るポインタ
   * @retval      true      非同期処理完了
   * @retval      false     処理中
   */
  //--------------------------------------------------------------
  bool WaitIsFileExists( gfl2::fs::ExtSaveFile::Result * result );

  //--------------------------------------------------------------
  /**
   * @brief   ディレクトリ情報読み取りの非同期処理開始
   * @param[in]   pHeap   使用するテンポラリヒープ
   * @param[in]   id          ディレクトリ指定ID
   * @param[out]  pReadNum    読み込んだエントリの数
   * @param[out]  pEntries    読み込むエントリ領域へのポインタ
   * @param[in]   numEntries  読み込む上限
   */
  //--------------------------------------------------------------
  bool StartReadDirectory(
      gfl2::heap::HeapBase * pHeap,
      NIJI_EXTSAVE_ID id, s32 * pReadNum,
      nn::fs::DirectoryEntry * pEntries, s32 numEntries );

  //--------------------------------------------------------------
  /**
   * @brief   ディレクトリ情報読み取りの非同期処理待ち
   * @param[out]  result    結果を受け取るポインタ
   * @retval      true      非同期処理完了
   * @retval      false     処理中
   */
  //--------------------------------------------------------------
  bool WaitReadDirectory( gfl2::fs::ExtSaveFile::Result * result );

  //--------------------------------------------------------------
  /**
   * @brief ディレクトリ名を生成する
   * @param[out] dst         ディレクトリ名の格納先
   * @param[in]  id          ディレクトリ指定ID
   */
  //--------------------------------------------------------------
  void CreateDirPathStr( wchar_t * dst, NIJI_EXTSAVE_ID id );

  //--------------------------------------------------------------
  /**
   * @brief ファイルへのパスを生成する
   * @param[out]  dst         パスの格納先
   * @param[in]   dir_name    ディレクトリ名
   * @param[in]   file_name   ファイル名
   * @caution   あまり使ってほしくない
   */
  //--------------------------------------------------------------
  void CreateFilePathStr(
      wchar_t * dst, const wchar_t * dir_name, const wchar_t * file_name );

  //--------------------------------------------------------------
  /**
   * @brief ファイルへのパスを生成する
   * @param[out] dst          パスの格納先
   * @param[in]  id           ディレクトリ指定ID
   * @param[in]   fileindex   ファイル指定ID
   */
  //--------------------------------------------------------------
  void CreateFilePathStr( wchar_t * dst, NIJI_EXTSAVE_ID id, int fileindex );

  //--------------------------------------------------------------
  /**
   * @brief gfl2::fs::ExtSaveSystemのポインタを取得する
   *
   * @caution   デバッグ用途以外使用禁止
   */
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveSystem* GetExtSaveSystem( void ) const
  {
    return mpExtSaveSystem;
  }

  void SetListener( NijiExtSaveSystemListener* pListener )
  {
    m_pListener = pListener;
  }

#if PM_DEBUG
  static bool* GetDebugExtSaveFatalErrorFlag(void)
  {
    return &m_DebugExtSaveMustFatalError;
  }
#endif

  //=============================================================================
  //
  //
  //        非公開部分
  //
  //
  //=============================================================================
private:

  //--------------------------------------------------------------
  //--------------------------------------------------------------
  enum EXT_SYS_THREAD_MODE{
    THREAD_MODE_NOTHING = 0,
    THREAD_MODE_BOOT_PROCESS,
  };

  static const int MAX_FILE_PATH_LENGTH = nn::fs::MAX_FILE_PATH_LENGTH;

  //--------------------------------------------------------------
  //
  //  非公開　変数定義
  //
  //--------------------------------------------------------------
  gfl2::fs::ExtSaveSystem* mpExtSaveSystem;

  EXTARC_STATUS m_arcStatus;
  bool m_bBooted;
  EXT_SYS_THREAD_MODE m_threadMode;
  ExtSaveLoadControlThread * m_pMyThread;
  gfl2::fs::ExtSaveSystem::Result m_threadSystemResult;

  gfl2::fs::ExtSaveSystem::Result m_systemAPIResult;
  gfl2::fs::ExtSaveFile::Result m_fileAPIResult;


  wchar_t m_tempPath[ MAX_FILE_PATH_LENGTH ];
  wchar_t m_tempPath2[ MAX_FILE_PATH_LENGTH ];

  void * m_iconBuffer;
  size_t m_iconSize;

  bool m_IsWriting;

  NijiExtSaveSystemListener* m_pListener;

#if PM_DEBUG
  static bool m_DebugExtSaveMustFatalError;
#endif

  //--------------------------------------------------------------
  //
  //  非公開関数
  //
  //--------------------------------------------------------------
private:
  void checkFatalError( gfl2::fs::ExtSaveFile::Result result );
  void startThread( u8 priority, EXT_SYS_THREAD_MODE mode, gfl2::heap::HeapBase * pHeap, EXTFILE_PARAM * pSaveData );
  void ThreadCallBack(gfl2::heap::HeapBase * pHeap, EXTFILE_PARAM * pSaveData );
  bool isAsyncFinished( void );

  bool waitFileAPI( gfl2::fs::ExtSaveFile::Result * result );
  bool waitSystemAPI( gfl2::fs::ExtSaveSystem::Result * result );
  void CreateDirPathStr( wchar_t * dst, const wchar_t * dir_name );

  void bootSystem( gfl2::heap::HeapBase * pTempHeap );
  bool createDirectories( nn::fs::DirectoryEntry * entries, int dir_max );
  void createIconBuffer( gfl2::heap::HeapBase * pTempHeap );
  void deleteIconBuffer( void );



}; //class NijiExtSaveSystem

GFL_NAMESPACE_END(ExtSavedata)

#endif //__XY_EXTSAVESYSTEM_H__
