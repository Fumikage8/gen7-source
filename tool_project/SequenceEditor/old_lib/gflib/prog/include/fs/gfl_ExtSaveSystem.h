//=============================================================================
/**
 * @file   gfl_ExtSaveSystem.h
 * @brief  拡張データ領域アクセス
 * @author tamada
 * @date   2012.09.11
 *
 * @note
 * */
//=============================================================================

#if !defined(__GFL_EXTSAVESYSTEM_H__)
#define __GFL_EXTSAVESYSTEM_H__
#pragma once

#include <gflib.h>

#include <fs/gfl_ExtSaveFile.h>

namespace gfl {
  namespace fs {

    class ExtSaveThread;
//-----------------------------------------------------------------------------
/**
 * @class ExtSaveSystem
 */
//-----------------------------------------------------------------------------
class ExtSaveSystem
{
  friend class ExtSaveThread;

  public:

  //-----------------------------------------------------------------------------
  /**
   * @brief 拡張セーブデータシステムAPIの操作結果
   */
  //-----------------------------------------------------------------------------
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

  static const char * GetMountName( void ) { return MOUNT_NAME_SCHAR; }
  static const wchar_t * GetMountNameWChar( void ) { return MOUNT_NAME_WCHAR; }

  //-----------------------------------------------------------------------------
  /// constructor
  //-----------------------------------------------------------------------------
  ExtSaveSystem( gfl::heap::HeapBase * pHeap );

  //-----------------------------------------------------------------------------
  /// destructor
  //-----------------------------------------------------------------------------
  ~ExtSaveSystem();


  //-----------------------------------------------------------------------------
  /**
   * @brief 初期化処理
   *
   * @param[in]   id                拡張セーブデータの認証ID
   * @param[in]   iconData          アイコンデータへのポインタ
   * @param[in]   iconDataSize      アイコンデータのサイズ
   * @param[in]   entryDirectory    最大ディレクトリ数
   * @param[in]   entryFile         最大ファイル数
   */
  //-----------------------------------------------------------------------------
  void Initialize(
      nn::fs::ExtSaveDataId id,
      const void * iconData,
      size_t  iconDataSize,
      u32 entryDirectory,
      u32 entryFile
      );

  void Initialize(
      nn::fs::ExtSaveDataId id,
      u32 entryDirectory,
      u32 entryFile
      );
  //=============================================================================
  //
  //
  //              同期処理API
  //
  //
  //=============================================================================
  //-----------------------------------------------------------------------------
  /**
   * @brief   拡張セーブデータ領域をマウントする（フォーマットあり）
   * @param[in]   iconBuffer  
   * @param[in]   iconSize
   *
   * @return  システムAPI操作結果
   *
   * @note  存在しない場合、内部でフォーマットも行う。
   * @note  ユーザーにフォーマット中を告知する場合はMountを使い判定するべき
   */
  //-----------------------------------------------------------------------------
  Result MountAndFormat( const void * iconBuffer, size_t iconSize );

  //-----------------------------------------------------------------------------
  /**
   * @brief   拡張セーブデータ領域をマウントする
   * @return  システムAPI操作結果
   *
   */
  //-----------------------------------------------------------------------------
  Result Mount( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief   拡張セーブデータ領域をアンマウントする
   *
   * @return  システムAPI操作結果
   */
  //-----------------------------------------------------------------------------
  Result Unmount( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief 拡張セーブデータ領域の情報を取得する
   *
   * @param[out]  entryDirectory    最大ディレクトリ数
   * @param[out]  entryFile         最大ファイル数
   *
   * @return  システムAPI操作結果
   */
  //-----------------------------------------------------------------------------
  Result GetArchiveInfo( u32 * entryDirectory, u32 * entryFile );

  //-----------------------------------------------------------------------------
  /**
   * @brief 拡張セーブデータ領域の削除
   *
   * @return  システムAPI操作結果
   */
  //-----------------------------------------------------------------------------
  Result DeleteArchive( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief ディレクトリ内の情報を読み出す
   * @param[in]   pHeap       使用するヒープ
   * @param[in]   dir_path    読み込むディレクトリへのパス
   * @param[out]  pReadNum    読み込んだエントリの数
   * @param[out]  pEntries    読み込むエントリ領域へのポインタ
   * @param[in]   numEntries  読み込む上限
   *
   * @return      ファイルAPI操作結果
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result ReadDirectory(
      gfl::heap::HeapBase * pHeap, const wchar_t * dir_path,
      s32 * pReadNum, nn::fs::DirectoryEntry * pEntries, s32 numEntries );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ディレクトリ作成
   * @param[in]   dir_path  作成するディレクトリへのパス
   *
   * @return      ファイルAPI操作結果
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result CreateDirectory( const wchar_t* dir_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ディレクトリ削除
   * @param[in]   dir_path  削除するディレクトリへのパス
   *
   * @return      ファイルAPI操作結果
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result DeleteDirectory( const wchar_t* dir_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief     ディレクトリ名変更
   * @param[in]   current_path  現在のパス
   * @param[in]   new_path      新規のパス
   *
   * @return      ファイルAPI操作結果
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result RenameDirectory( const wchar_t* current_path, const wchar_t* new_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル作成
   * @param[in]   file_path   作成するファイルへのパス
   * @param[in]   size        作成するファイルのサイズ
   *
   * @return      ファイルAPI操作結果
   * @note    CreateFileOWと違い、同名ファイルが存在している場合エラーとなる
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result CreateFile( const wchar_t* file_path, s64 size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル作成
   * @param[in]   file_path   作成するファイルへのパス
   * @param[in]   size        作成するファイルのサイズ
   *
   * @return      ファイルAPI操作結果
   * @note    CreateFileと違い、同名ファイルが存在している場合削除して上書きする
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result CreateFileOW( const wchar_t* file_path, s64 size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル削除
   * @param[in]   file_path   削除するファイルへのパス
   *
   * @return      ファイルAPI操作結果
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result DeleteFile( const wchar_t* file_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル名変更
   * @param[in]   current_path  現在のパス
   * @param[in]   new_path      新規のパス
   *
   * @return      ファイルAPI操作結果
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result RenameFile( const wchar_t* current_path, const wchar_t* new_path );
  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル読み込み
   * @param[in]   file_path 読み込むファイルへのパス
   * @param[out]  dst       保存先バッファへのポインタ
   * @param[in]   size      読み込むサイズ
   *
   * @return      ファイルAPI操作結果
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result ReadFile( const wchar_t* file_path, void * dst, size_t size );
  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル書込み
   * @param[in]   file_path 書き込むファイルへのパス
   * @param[in]   src       書き込むデータへのポインタ
   * @param[in]   size      書き込むデータのサイズ
   *
   * @return      ファイルAPI操作結果
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result WriteFile( const wchar_t* file_path, const void * src, size_t size );
  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル存在チェック
   * @param[in]   file_path ファイルへのパス
   * @param[out]  bool  trueのとき、存在する
   *
   * @return      ファイルAPI操作結果
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result IsFileExists( const wchar_t* file_path, bool *exist_flag );


  //=============================================================================
  //
  //
  //              非同期リクエスト
  //
  //
  //=============================================================================
  //-----------------------------------------------------------------------------
  /**
   * @brief   マウントの非同期処理開始（フォーマットあり）
   *
   * @param[in]   pHeap     使用するテンポラリヒープ
   * @param[in]   priority  スレッドの優先度
   * @param[in]   iconBuffer  
   * @param[in]   iconSize
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncMountAndFormat( gfl::heap::HeapBase * pHeap, u8 priority, const void * iconBuffer, size_t iconSize );

  //-----------------------------------------------------------------------------
  /**
   * @brief   マウントの非同期処理開始
   *
   * @param[in]   pHeap     使用するテンポラリヒープ
   * @param[in]   priority  スレッドの優先度
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncMount( gfl::heap::HeapBase * pHeap, u8 priority );

  //-----------------------------------------------------------------------------
  /**
   * @brief   アンマウントの非同期処理開始
   *
   * @param[in]   pHeap     使用するテンポラリヒープ
   * @param[in]   priority  スレッドの優先度
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncUnmount( gfl::heap::HeapBase * pHeap, u8 priority );

  //-----------------------------------------------------------------------------
  /**
   * @brief   拡張セーブデータ領域情報取得の非同期処理開始
   *
   * @param[in]   pHeap     使用するテンポラリヒープ
   * @param[in]   priority  スレッドの優先度
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncGetArchiveInfo( gfl::heap::HeapBase * pHeap, u8 priority );

  //-----------------------------------------------------------------------------
  /**
   * @brief   拡張セーブデータ領域削除の非同期処理開始
   *
   * @param[in]   pHeap     使用するテンポラリヒープ
   * @param[in]   priority  スレッドの優先度
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncDeleteArchive( gfl::heap::HeapBase * pHeap, u8 priority );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ディレクトリ情報読み取りの非同期処理開始
   *
   * @param[in]   pHeap       使用するテンポラリヒープ
   * @param[in]   priority    スレッドの優先度
   * @param[in]   dir_path    読み込むディレクトリへのパス
   * @param[out]  pReadNum    読み込んだエントリの数
   * @param[out]  pEntries    読み込むエントリ領域へのポインタ
   * @param[in]   numEntries  読み込む上限
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncReadDirectory(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * dir_path,
      s32 * pReadNum, nn::fs::DirectoryEntry * pEntries, s32 numEntries );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル作成の非同期処理開始
   *
   * @param[in]   pHeap       使用するテンポラリヒープ
   * @param[in]   priority    スレッドの優先度
   * @param[in]   file_path   作成するファイルへのパス
   * @param[in]   size        作成するファイルのサイズ
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncCreateFile(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * file_path, s64 size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル作成の非同期処理開始
   *
   * @param[in]   pHeap       使用するテンポラリヒープ
   * @param[in]   priority    スレッドの優先度
   * @param[in]   file_path   作成するファイルへのパス
   * @param[in]   size        作成するファイルのサイズ
   *
   * @return  bool    非同期処理リクエストの可否
   * @note    CreateFileと違い、同名ファイルが存在している場合削除して上書きする
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncCreateFileOW(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * file_path, s64 size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル削除の非同期処理開始
   *
   * @param[in]   pHeap       使用するテンポラリヒープ
   * @param[in]   priority    スレッドの優先度
   * @param[in]   file_path   削除するファイルへのパス
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncDeleteFile(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * file_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル名変更の非同期処理開始
   *
   * @param[in]   pHeap         使用するテンポラリヒープ
   * @param[in]   priority      スレッドの優先度
   * @param[in]   current_path  現在のパス
   * @param[in]   new_path      新規のパス
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncRenameFile(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * current_path, const wchar_t * new_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル読み込みの非同期処理開始
   *
   * @param[in]   pHeap     使用するテンポラリヒープ
   * @param[in]   priority  スレッドの優先度
   * @param[in]   file_path 読み込むファイルへのパス
   * @param[out]  dst       保存先バッファへのポインタ
   * @param[in]   size      読み込むサイズ
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncReadFile(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * file_path, void * dst, size_t size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル書込みの非同期処理開始
   *
   * @param[in]   pHeap     使用するテンポラリヒープ
   * @param[in]   priority  スレッドの優先度
   * @param[in]   file_path 書き込むファイルへのパス
   * @param[in]   src       書き込むデータへのポインタ
   * @param[in]   size      書き込むデータのサイズ
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncWriteFile(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * file_path, const void * src, size_t size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル存在チェックの非同期処理開始
   *
   * @param[in]   pHeap     使用するテンポラリヒープ
   * @param[in]   priority  スレッドの優先度
   * @param[in]   file_path ファイルへのパス
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncIsFileExists(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * file_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ディレクトリ作成の非同期処理開始
   *
   * @param[in]   pHeap     使用するテンポラリヒープ
   * @param[in]   priority  スレッドの優先度
   * @param[in]   dir_path  作成するディレクトリへのパス
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncCreateDirectory(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * dir_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ディレクトリ削除の非同期処理開始
   *
   * @param[in]   pHeap     使用するテンポラリヒープ
   * @param[in]   priority  スレッドの優先度
   * @param[in]   dir_path  削除するディレクトリへのパス
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncDeleteDirectory(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * dir_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ディレクトリ名変更の非同期処理開始
   *
   * @param[in]   pHeap         使用するテンポラリヒープ
   * @param[in]   priority      スレッドの優先度
   * @param[in]   current_path  現在のパス
   * @param[in]   new_path      新規のパス
   *
   * @return  bool    非同期処理リクエストの可否
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncRenameDirectory(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * current_path, const wchar_t * new_path );


  //-----------------------------------------------------------------------------
  /**
   * @brief   スレッド終了チェック
   * @return  bool  trueのとき、スレッド動作終了
   */
  //-----------------------------------------------------------------------------
  bool IsAsyncFinished( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ファイル操作APIの同期終了待ち
   * @param[out]  ExtSaveFile::Result 拡張セーブファイルAPI操作結果
   * @return  bool  trueのとき、スレッド動作終了
   */
  //-----------------------------------------------------------------------------
  bool WaitFileAPI( ExtSaveFile::Result * result );

  //-----------------------------------------------------------------------------
  /**
   * @brief   システム操作APIの同期終了待ち
   * @param[out]  ExtSaveSystem::Result 拡張セーブシステムAPI操作結果
   * @return  bool  trueのとき、スレッド動作終了
   */
  //-----------------------------------------------------------------------------
  bool WaitSystemAPI( ExtSaveSystem::Result * result );

  //-----------------------------------------------------------------------------
  /**
   * @brief   非同期ファイル操作の結果取得
   *
   * @return  ExtSaveFile::Result 拡張セーブファイルAPI操作結果
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result GetLastFileResult( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   非同期システム操作の結果取得
   *
   * @return  ExtSaveSystem::Result 拡張セーブシステムAPI操作結果
   */
  //-----------------------------------------------------------------------------
  ExtSaveSystem::Result GetLastSystemResult( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   SDKレベルでの最新の結果を取得する
   *
   * @return  nn::Result  SDK関数呼び出しからの最新の結果
   */
  //-----------------------------------------------------------------------------
  nn::Result GetLastNNResult( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief 非同期での拡張セーブデータ領域の情報を取得
   *
   * @param[out]  entryDirectory    最大ディレクトリ数
   * @param[out]  entryFile         最大ファイル数
   *
   */
  //-----------------------------------------------------------------------------
  void GetArchiveInfoResult( u32 * entryDirectory, u32 * entryFile );

  //-----------------------------------------------------------------------------
  /**
   * @brief 非同期でのファイル存在チェック結果を取得
   *
   * @param[out]    exists_flag
   */
  //-----------------------------------------------------------------------------
  void GetFileExistsResult( bool * exists_flag );

  //=============================================================================
  //
  //
  //        非公開部分
  //
  //
  //=============================================================================
  private:

  //-----------------------------------------------------------------------------
  //  定数定義
  //-----------------------------------------------------------------------------
    enum THREAD_MODE {
      THREAD_MODE_NOTHING = 0,
      THREAD_MODE_MOUNT_AND_FORMAT,
      THREAD_MODE_MOUNT,
      THREAD_MODE_UNMOUNT,
      THREAD_MODE_GET_ARCHIVE_INFO,
      THREAD_MODE_DELETE_ARCHIVE,
      THREAD_MODE_READ_DIRECTORY,
      THREAD_MODE_CREATE_FILE,
      THREAD_MODE_CREATE_FILE_OW,
      THREAD_MODE_DELETE_FILE,
      THREAD_MODE_RENAME_FILE,
      THREAD_MODE_READ_FILE,
      THREAD_MODE_WRITE_FILE,
      THREAD_MODE_IS_FILE_EXISTS,
      THREAD_MODE_CREATE_DIRECTORY,
      THREAD_MODE_DELETE_DIRECTORY,
      THREAD_MODE_RENAME_DIRECTORY,
    };

    static const u32 MAX_FILE_PATH_LENGTH = nn::fs::MAX_FILE_PATH_LENGTH + 1;
    static const u32 THREAD_STACK_SIZE = 4096;

    ///拡張セーブデータアーカイブのマウント名
    static const char * MOUNT_NAME_SCHAR;
    static const wchar_t * MOUNT_NAME_WCHAR;

  //-----------------------------------------------------------------------------
  //  クラス内関数
  //-----------------------------------------------------------------------------
    Result CheckProgrammersError( nn::Result result );
    Result CheckMountError( nn::Result result );
    Result CheckCreateError( nn::Result result );
    Result CheckDeleteError( nn::Result result );
    Result CheckInfoError( nn::Result result );

    void ThreadCallBack( void );
    void startThread(
        gfl::heap::HeapBase * pHeap, u8 priority, THREAD_MODE mode );

    void initThreadParams( void );
    static void setPath( wchar_t * dst, const wchar_t * src );

  //-----------------------------------------------------------------------------
  //  非公開　変数
  //-----------------------------------------------------------------------------
  private:

    // 初期化用変数
    nn::fs::ExtSaveDataId m_extSaveDataId;
    u32                   m_entryDirectory;
    u32                   m_entryFile;

    // 非同期処理用変数
    ExtSaveThread *           m_thread;
    THREAD_MODE               m_thread_mode;
    gfl::heap::HeapBase *     m_thread_heap;

    wchar_t                   m_thread_use_path[ MAX_FILE_PATH_LENGTH ];
    wchar_t                   m_thread_use_path2[ MAX_FILE_PATH_LENGTH ];
    s64                       m_thread_use_size;
    bool                      m_thread_use_bool;

    Result                    m_thread_result;
    ExtSaveFile::Result       m_thread_file_result;
    u32                       m_thread_dir_count_result;
    u32                       m_thread_file_count_result;
    nn::fs::DirectoryEntry *  m_thread_entries;
    s32 *                     m_thread_read_num;
    s32                       m_thread_num_entries;
    void *                    m_thread_dst_buffer;
    const void *              m_thread_src_buffer;

    const void *              m_thread_icon_buffer;
    size_t                    m_thread_icon_size;

    nn::Result                m_last_result;
      
}; //end of class ExtSaveSystem

};  //namespace fs
};    //namespace gfl

#endif  // __GFL_EXTSAVESYSTEM_H__

