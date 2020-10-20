#ifndef __GFL_READPCFILE_H__
#define __GFL_READPCFILE_H__
//===========================================================================
/**
 * @file    gfl_ReadPCFile.h
 * @brief   GAMEFREAK Library PCファイルを読み込めるクラス
 * @author  k.ohno
 * @date    2010.11.11
 */
//===========================================================================
#pragma once


#ifdef NN_SWITCH_ENABLE_HOST_IO

#include <nw/io/io_HioFileStream.h>

namespace gfl {
namespace fs {

/**
 *  @brief パソコンファイル読み込みクラス
 */

typedef nw::io::HioFileStream  ReadPCFile;



} /* end of namespace fs */
} /* end of namespace gfl */


#endif //NN_SWITCH_ENABLE_HOST_IO


#endif // __GFL_READPCFILE_H__
