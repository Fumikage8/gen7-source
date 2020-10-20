#ifndef __GFL_READPCFILE_H__
#define __GFL_READPCFILE_H__
//===========================================================================
/**
 * @file    gfl_ReadPCFile.h
 * @brief   GAMEFREAK Library PC�t�@�C����ǂݍ��߂�N���X
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
 *  @brief �p�\�R���t�@�C���ǂݍ��݃N���X
 */

typedef nw::io::HioFileStream  ReadPCFile;



} /* end of namespace fs */
} /* end of namespace gfl */


#endif //NN_SWITCH_ENABLE_HOST_IO


#endif // __GFL_READPCFILE_H__
