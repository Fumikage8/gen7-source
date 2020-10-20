#ifndef __GFL_READFILE_H__
#define __GFL_READFILE_H__
//===========================================================================
/**
 * @file  gf_ReadFile.h
 * @brief GAMEFREAK Library FileIO System ( File Reader )
 * @author  GAMEFREAK inc.
 * @date  2010.09.24
 */
//===========================================================================
#pragma once


#include <nw/io/io_RomFileStream.h>

namespace gfl {
namespace fs {

/**
 *  ファイル読み込みクラス
 */
typedef nw::io::RomFileStream   ReadFile;

/**
 *  ファイル読み込み基底クラス
 */
typedef nw::io::FileStream      FileReadStream;


  
/**
 *  非同期処理コールバック関数型
 */
typedef nw::io::IOStream::IOStreamCallback    AsyncCallback;

/**
 *  ファイルシーク開始指定
 */
enum{
  FILE_STREAM_SEEK_BEGIN   = nw::io::FILE_STREAM_SEEK_BEGIN,
  FILE_STREAM_SEEK_CURRENT = nw::io::FILE_STREAM_SEEK_CURRENT,
  FILE_STREAM_SEEK_END     = nw::io::FILE_STREAM_SEEK_END
};

} /* end of namespace fio */
} /* end of namespace gfl */



#endif // __GFL_READFILE_H__
