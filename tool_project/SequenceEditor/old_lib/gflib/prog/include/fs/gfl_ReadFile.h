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
 *  �t�@�C���ǂݍ��݃N���X
 */
typedef nw::io::RomFileStream   ReadFile;

/**
 *  �t�@�C���ǂݍ��݊��N���X
 */
typedef nw::io::FileStream      FileReadStream;


  
/**
 *  �񓯊������R�[���o�b�N�֐��^
 */
typedef nw::io::IOStream::IOStreamCallback    AsyncCallback;

/**
 *  �t�@�C���V�[�N�J�n�w��
 */
enum{
  FILE_STREAM_SEEK_BEGIN   = nw::io::FILE_STREAM_SEEK_BEGIN,
  FILE_STREAM_SEEK_CURRENT = nw::io::FILE_STREAM_SEEK_CURRENT,
  FILE_STREAM_SEEK_END     = nw::io::FILE_STREAM_SEEK_END
};

} /* end of namespace fio */
} /* end of namespace gfl */



#endif // __GFL_READFILE_H__
