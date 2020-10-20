//=============================================================================
/**
 * @file   gfl_ExtSaveSystem.h
 * @brief  �g���f�[�^�̈�A�N�Z�X
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
   * @brief �g���Z�[�u�f�[�^�V�X�e��API�̑��쌋��
   */
  //-----------------------------------------------------------------------------
  enum Result {
    RESULT_OK,                      ///<����
    RESULT_CONTINUE,
    RESULT_NG_NOT_FOUND,            ///<���s�F�g���Z�[�u�f�[�^�����݂��Ȃ�
    RESULT_NG_MEDIA_NOT_FOUND,      ///<���s�FSD�J�[�h�����݂��Ȃ��A���邢�͔F���ł��Ȃ�
    RESULT_NG_BAD_FORMAT,           ///<���s�FSD�J�[�h�̃t�H�[�}�b�g���K�v
    RESULT_NG_ERROR,                ///<���s�FSD�J�[�h�̐ڐG�s�Ǔ�
    RESULT_NG_WRITE_PROTECTED,      ///<���s�FSD�J�[�h���邢�̓t�@�C�����������݋֎~�ɂȂ��Ă���
    RESULT_NG_NOT_FORMATTED,        ///<���s�F�g���Z�[�u�f�[�^���쐬����Ă��Ȃ�
    RESULT_NG_NOT_ENOUGH_SPACE,     ///<���s�FSD�J�[�h�ɏ������ݗe�ʂ�����Ȃ�
    RESULT_NG_VERIFICATION_FAILED,  ///<���s�F�Z�[�u�f�[�^�j��or������
    RESULT_NG_MOUNTING,             ///<���s�F�}�E���g���i�������݂̂̃G���[�j
    RESULT_NG_UNKNOWN,              ///<���s�F�z��O�̃G���[
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
   * @brief ����������
   *
   * @param[in]   id                �g���Z�[�u�f�[�^�̔F��ID
   * @param[in]   iconData          �A�C�R���f�[�^�ւ̃|�C���^
   * @param[in]   iconDataSize      �A�C�R���f�[�^�̃T�C�Y
   * @param[in]   entryDirectory    �ő�f�B���N�g����
   * @param[in]   entryFile         �ő�t�@�C����
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
  //              ��������API
  //
  //
  //=============================================================================
  //-----------------------------------------------------------------------------
  /**
   * @brief   �g���Z�[�u�f�[�^�̈���}�E���g����i�t�H�[�}�b�g����j
   * @param[in]   iconBuffer  
   * @param[in]   iconSize
   *
   * @return  �V�X�e��API���쌋��
   *
   * @note  ���݂��Ȃ��ꍇ�A�����Ńt�H�[�}�b�g���s���B
   * @note  ���[�U�[�Ƀt�H�[�}�b�g�������m����ꍇ��Mount���g�����肷��ׂ�
   */
  //-----------------------------------------------------------------------------
  Result MountAndFormat( const void * iconBuffer, size_t iconSize );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �g���Z�[�u�f�[�^�̈���}�E���g����
   * @return  �V�X�e��API���쌋��
   *
   */
  //-----------------------------------------------------------------------------
  Result Mount( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �g���Z�[�u�f�[�^�̈���A���}�E���g����
   *
   * @return  �V�X�e��API���쌋��
   */
  //-----------------------------------------------------------------------------
  Result Unmount( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief �g���Z�[�u�f�[�^�̈�̏����擾����
   *
   * @param[out]  entryDirectory    �ő�f�B���N�g����
   * @param[out]  entryFile         �ő�t�@�C����
   *
   * @return  �V�X�e��API���쌋��
   */
  //-----------------------------------------------------------------------------
  Result GetArchiveInfo( u32 * entryDirectory, u32 * entryFile );

  //-----------------------------------------------------------------------------
  /**
   * @brief �g���Z�[�u�f�[�^�̈�̍폜
   *
   * @return  �V�X�e��API���쌋��
   */
  //-----------------------------------------------------------------------------
  Result DeleteArchive( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief �f�B���N�g�����̏���ǂݏo��
   * @param[in]   pHeap       �g�p����q�[�v
   * @param[in]   dir_path    �ǂݍ��ރf�B���N�g���ւ̃p�X
   * @param[out]  pReadNum    �ǂݍ��񂾃G���g���̐�
   * @param[out]  pEntries    �ǂݍ��ރG���g���̈�ւ̃|�C���^
   * @param[in]   numEntries  �ǂݍ��ޏ��
   *
   * @return      �t�@�C��API���쌋��
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result ReadDirectory(
      gfl::heap::HeapBase * pHeap, const wchar_t * dir_path,
      s32 * pReadNum, nn::fs::DirectoryEntry * pEntries, s32 numEntries );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �f�B���N�g���쐬
   * @param[in]   dir_path  �쐬����f�B���N�g���ւ̃p�X
   *
   * @return      �t�@�C��API���쌋��
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result CreateDirectory( const wchar_t* dir_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �f�B���N�g���폜
   * @param[in]   dir_path  �폜����f�B���N�g���ւ̃p�X
   *
   * @return      �t�@�C��API���쌋��
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result DeleteDirectory( const wchar_t* dir_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief     �f�B���N�g�����ύX
   * @param[in]   current_path  ���݂̃p�X
   * @param[in]   new_path      �V�K�̃p�X
   *
   * @return      �t�@�C��API���쌋��
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result RenameDirectory( const wchar_t* current_path, const wchar_t* new_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C���쐬
   * @param[in]   file_path   �쐬����t�@�C���ւ̃p�X
   * @param[in]   size        �쐬����t�@�C���̃T�C�Y
   *
   * @return      �t�@�C��API���쌋��
   * @note    CreateFileOW�ƈႢ�A�����t�@�C�������݂��Ă���ꍇ�G���[�ƂȂ�
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result CreateFile( const wchar_t* file_path, s64 size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C���쐬
   * @param[in]   file_path   �쐬����t�@�C���ւ̃p�X
   * @param[in]   size        �쐬����t�@�C���̃T�C�Y
   *
   * @return      �t�@�C��API���쌋��
   * @note    CreateFile�ƈႢ�A�����t�@�C�������݂��Ă���ꍇ�폜���ď㏑������
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result CreateFileOW( const wchar_t* file_path, s64 size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C���폜
   * @param[in]   file_path   �폜����t�@�C���ւ̃p�X
   *
   * @return      �t�@�C��API���쌋��
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result DeleteFile( const wchar_t* file_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C�����ύX
   * @param[in]   current_path  ���݂̃p�X
   * @param[in]   new_path      �V�K�̃p�X
   *
   * @return      �t�@�C��API���쌋��
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result RenameFile( const wchar_t* current_path, const wchar_t* new_path );
  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C���ǂݍ���
   * @param[in]   file_path �ǂݍ��ރt�@�C���ւ̃p�X
   * @param[out]  dst       �ۑ���o�b�t�@�ւ̃|�C���^
   * @param[in]   size      �ǂݍ��ރT�C�Y
   *
   * @return      �t�@�C��API���쌋��
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result ReadFile( const wchar_t* file_path, void * dst, size_t size );
  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C��������
   * @param[in]   file_path �������ރt�@�C���ւ̃p�X
   * @param[in]   src       �������ރf�[�^�ւ̃|�C���^
   * @param[in]   size      �������ރf�[�^�̃T�C�Y
   *
   * @return      �t�@�C��API���쌋��
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result WriteFile( const wchar_t* file_path, const void * src, size_t size );
  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C�����݃`�F�b�N
   * @param[in]   file_path �t�@�C���ւ̃p�X
   * @param[out]  bool  true�̂Ƃ��A���݂���
   *
   * @return      �t�@�C��API���쌋��
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result IsFileExists( const wchar_t* file_path, bool *exist_flag );


  //=============================================================================
  //
  //
  //              �񓯊����N�G�X�g
  //
  //
  //=============================================================================
  //-----------------------------------------------------------------------------
  /**
   * @brief   �}�E���g�̔񓯊������J�n�i�t�H�[�}�b�g����j
   *
   * @param[in]   pHeap     �g�p����e���|�����q�[�v
   * @param[in]   priority  �X���b�h�̗D��x
   * @param[in]   iconBuffer  
   * @param[in]   iconSize
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncMountAndFormat( gfl::heap::HeapBase * pHeap, u8 priority, const void * iconBuffer, size_t iconSize );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �}�E���g�̔񓯊������J�n
   *
   * @param[in]   pHeap     �g�p����e���|�����q�[�v
   * @param[in]   priority  �X���b�h�̗D��x
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncMount( gfl::heap::HeapBase * pHeap, u8 priority );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �A���}�E���g�̔񓯊������J�n
   *
   * @param[in]   pHeap     �g�p����e���|�����q�[�v
   * @param[in]   priority  �X���b�h�̗D��x
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncUnmount( gfl::heap::HeapBase * pHeap, u8 priority );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �g���Z�[�u�f�[�^�̈���擾�̔񓯊������J�n
   *
   * @param[in]   pHeap     �g�p����e���|�����q�[�v
   * @param[in]   priority  �X���b�h�̗D��x
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncGetArchiveInfo( gfl::heap::HeapBase * pHeap, u8 priority );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �g���Z�[�u�f�[�^�̈�폜�̔񓯊������J�n
   *
   * @param[in]   pHeap     �g�p����e���|�����q�[�v
   * @param[in]   priority  �X���b�h�̗D��x
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncDeleteArchive( gfl::heap::HeapBase * pHeap, u8 priority );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �f�B���N�g�����ǂݎ��̔񓯊������J�n
   *
   * @param[in]   pHeap       �g�p����e���|�����q�[�v
   * @param[in]   priority    �X���b�h�̗D��x
   * @param[in]   dir_path    �ǂݍ��ރf�B���N�g���ւ̃p�X
   * @param[out]  pReadNum    �ǂݍ��񂾃G���g���̐�
   * @param[out]  pEntries    �ǂݍ��ރG���g���̈�ւ̃|�C���^
   * @param[in]   numEntries  �ǂݍ��ޏ��
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncReadDirectory(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * dir_path,
      s32 * pReadNum, nn::fs::DirectoryEntry * pEntries, s32 numEntries );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C���쐬�̔񓯊������J�n
   *
   * @param[in]   pHeap       �g�p����e���|�����q�[�v
   * @param[in]   priority    �X���b�h�̗D��x
   * @param[in]   file_path   �쐬����t�@�C���ւ̃p�X
   * @param[in]   size        �쐬����t�@�C���̃T�C�Y
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncCreateFile(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * file_path, s64 size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C���쐬�̔񓯊������J�n
   *
   * @param[in]   pHeap       �g�p����e���|�����q�[�v
   * @param[in]   priority    �X���b�h�̗D��x
   * @param[in]   file_path   �쐬����t�@�C���ւ̃p�X
   * @param[in]   size        �쐬����t�@�C���̃T�C�Y
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   * @note    CreateFile�ƈႢ�A�����t�@�C�������݂��Ă���ꍇ�폜���ď㏑������
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncCreateFileOW(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * file_path, s64 size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C���폜�̔񓯊������J�n
   *
   * @param[in]   pHeap       �g�p����e���|�����q�[�v
   * @param[in]   priority    �X���b�h�̗D��x
   * @param[in]   file_path   �폜����t�@�C���ւ̃p�X
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncDeleteFile(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * file_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C�����ύX�̔񓯊������J�n
   *
   * @param[in]   pHeap         �g�p����e���|�����q�[�v
   * @param[in]   priority      �X���b�h�̗D��x
   * @param[in]   current_path  ���݂̃p�X
   * @param[in]   new_path      �V�K�̃p�X
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncRenameFile(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * current_path, const wchar_t * new_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C���ǂݍ��݂̔񓯊������J�n
   *
   * @param[in]   pHeap     �g�p����e���|�����q�[�v
   * @param[in]   priority  �X���b�h�̗D��x
   * @param[in]   file_path �ǂݍ��ރt�@�C���ւ̃p�X
   * @param[out]  dst       �ۑ���o�b�t�@�ւ̃|�C���^
   * @param[in]   size      �ǂݍ��ރT�C�Y
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncReadFile(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * file_path, void * dst, size_t size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C�������݂̔񓯊������J�n
   *
   * @param[in]   pHeap     �g�p����e���|�����q�[�v
   * @param[in]   priority  �X���b�h�̗D��x
   * @param[in]   file_path �������ރt�@�C���ւ̃p�X
   * @param[in]   src       �������ރf�[�^�ւ̃|�C���^
   * @param[in]   size      �������ރf�[�^�̃T�C�Y
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncWriteFile(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * file_path, const void * src, size_t size );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C�����݃`�F�b�N�̔񓯊������J�n
   *
   * @param[in]   pHeap     �g�p����e���|�����q�[�v
   * @param[in]   priority  �X���b�h�̗D��x
   * @param[in]   file_path �t�@�C���ւ̃p�X
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncIsFileExists(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * file_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �f�B���N�g���쐬�̔񓯊������J�n
   *
   * @param[in]   pHeap     �g�p����e���|�����q�[�v
   * @param[in]   priority  �X���b�h�̗D��x
   * @param[in]   dir_path  �쐬����f�B���N�g���ւ̃p�X
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncCreateDirectory(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * dir_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �f�B���N�g���폜�̔񓯊������J�n
   *
   * @param[in]   pHeap     �g�p����e���|�����q�[�v
   * @param[in]   priority  �X���b�h�̗D��x
   * @param[in]   dir_path  �폜����f�B���N�g���ւ̃p�X
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncDeleteDirectory(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * dir_path );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �f�B���N�g�����ύX�̔񓯊������J�n
   *
   * @param[in]   pHeap         �g�p����e���|�����q�[�v
   * @param[in]   priority      �X���b�h�̗D��x
   * @param[in]   current_path  ���݂̃p�X
   * @param[in]   new_path      �V�K�̃p�X
   *
   * @return  bool    �񓯊��������N�G�X�g�̉�
   */
  //-----------------------------------------------------------------------------
  bool StartAsyncRenameDirectory(
      gfl::heap::HeapBase * pHeap, u8 priority,
      const wchar_t * current_path, const wchar_t * new_path );


  //-----------------------------------------------------------------------------
  /**
   * @brief   �X���b�h�I���`�F�b�N
   * @return  bool  true�̂Ƃ��A�X���b�h����I��
   */
  //-----------------------------------------------------------------------------
  bool IsAsyncFinished( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �t�@�C������API�̓����I���҂�
   * @param[out]  ExtSaveFile::Result �g���Z�[�u�t�@�C��API���쌋��
   * @return  bool  true�̂Ƃ��A�X���b�h����I��
   */
  //-----------------------------------------------------------------------------
  bool WaitFileAPI( ExtSaveFile::Result * result );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �V�X�e������API�̓����I���҂�
   * @param[out]  ExtSaveSystem::Result �g���Z�[�u�V�X�e��API���쌋��
   * @return  bool  true�̂Ƃ��A�X���b�h����I��
   */
  //-----------------------------------------------------------------------------
  bool WaitSystemAPI( ExtSaveSystem::Result * result );

  //-----------------------------------------------------------------------------
  /**
   * @brief   �񓯊��t�@�C������̌��ʎ擾
   *
   * @return  ExtSaveFile::Result �g���Z�[�u�t�@�C��API���쌋��
   */
  //-----------------------------------------------------------------------------
  ExtSaveFile::Result GetLastFileResult( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   �񓯊��V�X�e������̌��ʎ擾
   *
   * @return  ExtSaveSystem::Result �g���Z�[�u�V�X�e��API���쌋��
   */
  //-----------------------------------------------------------------------------
  ExtSaveSystem::Result GetLastSystemResult( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   SDK���x���ł̍ŐV�̌��ʂ��擾����
   *
   * @return  nn::Result  SDK�֐��Ăяo������̍ŐV�̌���
   */
  //-----------------------------------------------------------------------------
  nn::Result GetLastNNResult( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief �񓯊��ł̊g���Z�[�u�f�[�^�̈�̏����擾
   *
   * @param[out]  entryDirectory    �ő�f�B���N�g����
   * @param[out]  entryFile         �ő�t�@�C����
   *
   */
  //-----------------------------------------------------------------------------
  void GetArchiveInfoResult( u32 * entryDirectory, u32 * entryFile );

  //-----------------------------------------------------------------------------
  /**
   * @brief �񓯊��ł̃t�@�C�����݃`�F�b�N���ʂ��擾
   *
   * @param[out]    exists_flag
   */
  //-----------------------------------------------------------------------------
  void GetFileExistsResult( bool * exists_flag );

  //=============================================================================
  //
  //
  //        ����J����
  //
  //
  //=============================================================================
  private:

  //-----------------------------------------------------------------------------
  //  �萔��`
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

    ///�g���Z�[�u�f�[�^�A�[�J�C�u�̃}�E���g��
    static const char * MOUNT_NAME_SCHAR;
    static const wchar_t * MOUNT_NAME_WCHAR;

  //-----------------------------------------------------------------------------
  //  �N���X���֐�
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
  //  ����J�@�ϐ�
  //-----------------------------------------------------------------------------
  private:

    // �������p�ϐ�
    nn::fs::ExtSaveDataId m_extSaveDataId;
    u32                   m_entryDirectory;
    u32                   m_entryFile;

    // �񓯊������p�ϐ�
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

