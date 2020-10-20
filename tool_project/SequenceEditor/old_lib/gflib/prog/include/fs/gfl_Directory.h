//=============================================================================
/**
 * @file    gfl_Directory.h
 * @brief
 * @author  tamada
 * @date    2012.09.22
 */
//=============================================================================
#if !defined(__GFL_DIRECTORY_H__)
#define __GFL_DIRECTORY_H__
#pragma once

#include <gflib.h>

namespace gfl{
namespace fs{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class Directory
{
  public:
  enum Result {
    ACCESS_RESULT_OK,                     // 成功
    ACCESS_RESULT_NG_PATH_NOT_EXIST,      // 失敗：指定したパスが存在しない
    ACCESS_RESULT_NG_PATH_ALREADY_EXIST,  // 失敗：指定したパスが既に存在する
    ACCESS_RESULT_NG_BAD_FORMAT,          // 失敗：フォーマットが不正な状態になっている
    ACCESS_RESULT_NG_BROKEN_DATA,         // 失敗：データが壊れているか, 改竄されている
    ACCESS_RESULT_NG_ERROR,               // 失敗：接触不良など, ハードウェア要因
    ACCESS_RESULT_NG_LACK_OF_SPACE,       // 失敗：メディアの空き容量不足
    ACCESS_RESULT_OUT_OF_RESOURCE,        // 失敗：リソース不足
    ACCESS_RESULT_ACCESS_DENIED,          // 失敗：アクセス権がない
    ACCESS_RESULT_NG_WRITE_PROTECTED,     // 失敗：ライトプロテクトされている
    ACCESS_RESULT_NG_MEDIA_NOT_FOUND,     // 失敗：指定したパスが存在しない
    ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR,  // 失敗：接触不良など
    ACCESS_RESULT_NG_ARCHIVE_INVALIDATED, // 失敗：アーカイブ無効（SDカード抜き等）
    ACCESS_RESULT_NG_UNKNOWN,             // 失敗：想定外のエラー
  };

public:
  Directory();
  ~Directory();

  Result Open( const wchar_t * dir_path );
  void Close( void );
  Result Read( s32 * pNum, nn::fs::DirectoryEntry * pEntries, s32 readMax );
  Result GetEntriesCount( gfl::heap::HeapBase * pHeap, s32 * count );

  /**
   * @brief 直前の行ったファイルアクセスの結果を取得：nn::Result版
   */
  inline nn::Result GetLastAccessNNResult( void ) const { return m_last_nn_result; }

private:
  static const u32 MAX_FILE_PATH_LENGTH = nn::fs::MAX_FILE_PATH_LENGTH + 1;
  static const u32 USE_ENTRY_MAX = 10;

  nn::fs::Directory m_dir;
  nn::Result m_last_nn_result;
  Result m_last_result;

  static Result GetAccessResult( nn::Result nn_result );
};

}; //namespace fs
}; //namespace gfl

#endif  // __GFL_DIRECTORY_H__
