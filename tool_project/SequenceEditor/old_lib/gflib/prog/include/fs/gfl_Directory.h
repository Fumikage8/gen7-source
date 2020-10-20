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
    ACCESS_RESULT_OK,                     // ����
    ACCESS_RESULT_NG_PATH_NOT_EXIST,      // ���s�F�w�肵���p�X�����݂��Ȃ�
    ACCESS_RESULT_NG_PATH_ALREADY_EXIST,  // ���s�F�w�肵���p�X�����ɑ��݂���
    ACCESS_RESULT_NG_BAD_FORMAT,          // ���s�F�t�H�[�}�b�g���s���ȏ�ԂɂȂ��Ă���
    ACCESS_RESULT_NG_BROKEN_DATA,         // ���s�F�f�[�^�����Ă��邩, ��₂���Ă���
    ACCESS_RESULT_NG_ERROR,               // ���s�F�ڐG�s�ǂȂ�, �n�[�h�E�F�A�v��
    ACCESS_RESULT_NG_LACK_OF_SPACE,       // ���s�F���f�B�A�̋󂫗e�ʕs��
    ACCESS_RESULT_OUT_OF_RESOURCE,        // ���s�F���\�[�X�s��
    ACCESS_RESULT_ACCESS_DENIED,          // ���s�F�A�N�Z�X�����Ȃ�
    ACCESS_RESULT_NG_WRITE_PROTECTED,     // ���s�F���C�g�v���e�N�g����Ă���
    ACCESS_RESULT_NG_MEDIA_NOT_FOUND,     // ���s�F�w�肵���p�X�����݂��Ȃ�
    ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR,  // ���s�F�ڐG�s�ǂȂ�
    ACCESS_RESULT_NG_ARCHIVE_INVALIDATED, // ���s�F�A�[�J�C�u�����iSD�J�[�h�������j
    ACCESS_RESULT_NG_UNKNOWN,             // ���s�F�z��O�̃G���[
  };

public:
  Directory();
  ~Directory();

  Result Open( const wchar_t * dir_path );
  void Close( void );
  Result Read( s32 * pNum, nn::fs::DirectoryEntry * pEntries, s32 readMax );
  Result GetEntriesCount( gfl::heap::HeapBase * pHeap, s32 * count );

  /**
   * @brief ���O�̍s�����t�@�C���A�N�Z�X�̌��ʂ��擾�Fnn::Result��
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
