//=============================================================================
/**
 * @file   gfl_ExtSaveSystem.cpp
 * @brief  �g���f�[�^�̈�A�N�Z�X
 * @author tamada
 * @date   2012.09.11
 *
 * @note 
 */
//=============================================================================

#include <fs/gfl_ExtSaveSystem.h>

#include "gfl_ExtSaveThread.h"

namespace gfl {
namespace fs {
//=============================================================================
//=============================================================================
const char * ExtSaveSystem::MOUNT_NAME_SCHAR = "exdt:";
const wchar_t * ExtSaveSystem::MOUNT_NAME_WCHAR = L"exdt:";

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
  ExtSaveSystem::ExtSaveSystem( gfl::heap::HeapBase * pHeap ) :
    m_extSaveDataId(0),
    m_entryDirectory(0),
    m_entryFile(0),

    m_thread(NULL),
    m_thread_mode(THREAD_MODE_NOTHING),
    m_thread_use_path(),
    m_thread_use_size(0),
    //m_temp_nn_result(0),
    m_thread_result(RESULT_OK),
    m_thread_file_result(ExtSaveFile::ACCESS_RESULT_OK)

  {
    initThreadParams();
  }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
  ExtSaveSystem::~ExtSaveSystem()
  {
  }

//-----------------------------------------------------------------------------
/**
 */
//-----------------------------------------------------------------------------
  void ExtSaveSystem::Initialize( 
      nn::fs::ExtSaveDataId id,
      const void * iconData,
      size_t  iconDataSize,
      u32 entryDirectory,
      u32 entryFile
      )
  {
    m_extSaveDataId   = id;
    m_entryDirectory  = entryDirectory;
    m_entryFile       = entryFile;
  }

  void ExtSaveSystem::Initialize(
      nn::fs::ExtSaveDataId id,
      u32 entryDirectory,
      u32 entryFile
      )
  {
    m_extSaveDataId   = id;
    m_entryDirectory  = entryDirectory;
    m_entryFile       = entryFile;
  }

//=============================================================================
//
//
//              ��������API
//
//
//=============================================================================
//-----------------------------------------------------------------------------
/**
 * @brief �g���Z�[�u�f�[�^�̃}�E���g
 *
 * @note  ���݂��Ȃ��ꍇ�A�����Ńt�H�[�}�b�g���s��
 */
//-----------------------------------------------------------------------------
ExtSaveSystem::Result ExtSaveSystem::Mount( void )
{
  m_last_result = nn::fs::MountExtSaveData(MOUNT_NAME_SCHAR, m_extSaveDataId);
  if ( !m_last_result.IsFailure() ) return RESULT_OK;

  //����}�E���g�G���[�̏ꍇ
  Result mount_res = CheckMountError( m_last_result );
  //GFL_PRINT("ExtSaveSystem::MountResult=%d\n", mount_res );
  return mount_res;
}

//-----------------------------------------------------------------------------
/**
 * @brief �g���Z�[�u�f�[�^�̃}�E���g
 *
 * @note  ���݂��Ȃ��ꍇ�A�����Ńt�H�[�}�b�g���s��
 */
//-----------------------------------------------------------------------------
ExtSaveSystem::Result ExtSaveSystem::MountAndFormat( const void * iconBuffer, size_t iconSize )
{
  m_last_result = nn::fs::MountExtSaveData(MOUNT_NAME_SCHAR, m_extSaveDataId);
  if ( !m_last_result.IsFailure() ) return RESULT_OK;

  //����}�E���g�G���[�̏ꍇ
  Result mount_res = CheckMountError( m_last_result );
  //GFL_PRINT("ExtSaveSystem::MountResult=%d\n", mount_res );
  if ( mount_res == RESULT_NG_NOT_FOUND )
  { //���݂��Ă��Ȃ���Create����
    m_last_result = nn::fs::CreateExtSaveData( m_extSaveDataId, iconBuffer, iconSize, m_entryDirectory, m_entryFile );
  }
  else if ( mount_res == RESULT_NG_VERIFICATION_FAILED || mount_res == RESULT_NG_NOT_FORMATTED )
  { //���Ă���or�����񂳂ꂽor�O��쐬���s��Delete��Create����
    m_last_result = nn::fs::DeleteExtSaveData( m_extSaveDataId );
    if(m_last_result.IsSuccess())
    {
      m_last_result = nn::fs::CreateExtSaveData( m_extSaveDataId, iconBuffer, iconSize, m_entryDirectory, m_entryFile );
    }
  }
  else
  { //��L�ȊO�����s�I��
    return mount_res;
  }

  // �쐬��̔��� �� �쐬���s�Ȃ�I��
  Result create_res = CheckCreateError( m_last_result );
  //GFL_PRINT("ExtSaveSystem::CreateResult=%d\n", create_res );
  if ( create_res != RESULT_OK ) return create_res;

  {
    // �ă}�E���g
    m_last_result = nn::fs::MountExtSaveData(MOUNT_NAME_SCHAR, m_extSaveDataId);
    if(m_last_result.IsFailure())
    {
      // �n�߂̌Ăяo�����Ɠ��l�B
      // (�⑫) SD �J�[�h��������Ȃ�����A�쐬����Ƀ}�E���g�����s���邱�Ƃ́A�܂�����܂���B
      mount_res = CheckMountError( m_last_result );
      //GFL_PRINT("ExtSaveSystem::MountResult=%d\n", mount_res );
      return mount_res;
    }
  }

  return RESULT_OK;
}
//-----------------------------------------------------------------------------
/**
 * @brief �g���Z�[�u�f�[�^�̃}�E���g����
 */
//-----------------------------------------------------------------------------
ExtSaveSystem::Result ExtSaveSystem::Unmount( void )
{
  m_last_result = nn::fs::Unmount( GetMountName() );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
    if ( m_last_result <= nn::fs::ResultNotFound() )
    {
      return RESULT_NG_NOT_FOUND;
    }
    return RESULT_NG_UNKNOWN;
  }
  return RESULT_OK;
}

//-----------------------------------------------------------------------------
/**
 * @brief �g���Z�[�u�f�[�^�̈�̏����擾����
 */
//-----------------------------------------------------------------------------
ExtSaveSystem::Result ExtSaveSystem::GetArchiveInfo( u32 * entryDirectory, u32 * entryFile )
{
  m_last_result = nn::fs::GetExtSaveDataFormatInfo( entryDirectory, entryFile, m_extSaveDataId );
  if ( m_last_result.IsFailure() )
  {
    return CheckInfoError( m_last_result );
  }
  return RESULT_OK;
}

//-----------------------------------------------------------------------------
/**
 * @brief �g���Z�[�u�f�[�^�̈�̍폜
 *
 * @return  �V�X�e��API���쌋��
 */
//-----------------------------------------------------------------------------
ExtSaveSystem::Result ExtSaveSystem::DeleteArchive( void )
{
  m_last_result = nn::fs::DeleteExtSaveData( m_extSaveDataId );
  if ( m_last_result.IsFailure() )
  {
    return CheckDeleteError( m_last_result );
  }
  return RESULT_OK;
}

//-----------------------------------------------------------------------------
/**
 * @brief �f�B���N�g�����̏���ǂݏo��
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::ReadDirectory(
    gfl::heap::HeapBase * pHeap, const wchar_t * dir_path,
    s32 * pReadNum, nn::fs::DirectoryEntry * pEntries, s32 numEntries )
{
  nn::fs::Directory * dir = GFL_NEW( pHeap ) nn::fs::Directory();
  m_last_result = dir->TryInitialize( dir_path );
  if ( m_last_result.IsSuccess() )
  {
    m_last_result = dir->TryRead( pReadNum, pEntries, numEntries );
  }
  dir->Finalize();
  GFL_DELETE dir;
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief   �f�B���N�g���쐬
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::CreateDirectory( const wchar_t* dir_path )
{
  GFL_PRINT("ExtSaveSystem::CreateDirectory()\n");

  m_last_result = nn::fs::TryCreateDirectory( dir_path );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief   �f�B���N�g���폜
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::DeleteDirectory( const wchar_t* dir_path )
{
  GFL_PRINT("ExtSaveSystem::DeleteDirectory()\n");

  m_last_result = nn::fs::TryDeleteDirectory( dir_path );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief     �f�B���N�g�����ύX
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::RenameDirectory( const wchar_t* current_path, const wchar_t* new_path )
{
  GFL_PRINT("ExtSaveSystem::RenameDirectory()\n");
  m_last_result = nn::fs::TryRenameDirectory( current_path, new_path );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief   �t�@�C���쐬
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::CreateFile( const wchar_t* file_path, s64 size )
{
  GFL_PRINT("ExtSaveSystem::CreateFile()\n");
  GFL_ASSERT_STOP( size >= 0 );
  m_last_result = nn::fs::TryCreateFile( file_path, size );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
    GFL_PRINT("CreateFileError\n");
    //���łɑ��݂��遨�Ȃɂ������߂�
    if ( nn::fs::ResultAlreadyExists::Includes(m_last_result) )
    {
      return ExtSaveFile::GetAccessResult( m_last_result );
    }

    //�G���[�̏ꍇ�A���݃t�@�C�����c���Ă���\�������遨�폜
    nn::Result delete_nn_result = nn::fs::TryDeleteFile( file_path );
    ExtSaveFile::Result delete_result;
    delete_result = ExtSaveFile::GetAccessResult(delete_nn_result);
    if ( delete_result == ExtSaveFile::ACCESS_RESULT_OK )
    { // ���������ꍇ�A�S�~�t�@�C����������
      GFL_PRINT("ExtSaveSystem:: Delete Dust File\n");
    }
    else if ( delete_result == ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST )
    { // ResultNotFound�̏ꍇ�͂��݃t�@�C�����Ȃ������B
      GFL_PRINT("ExtSaveSystem:: Dust File Not Exists\n");
    }
    else
    { // ����ȊO�͂Ȃɂ��̃G���[�B
      NN_DBG_PRINT_RESULT( delete_nn_result );
      GFL_PRINT("DeleteFileError\n");
    }
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief �t�@�C���쐬�i�����㏑������j
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::CreateFileOW( const wchar_t* file_path, s64 size )
{
  ExtSaveFile::Result create_1st_result;
  create_1st_result = CreateFile( file_path, size );
  if (create_1st_result != ExtSaveFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST )
  {
    //�uResultAlreadyExists�v�ȊO�̏ꍇ�A���̂܂ܕԂ�
    return create_1st_result;
  }
  //�t�@�C�������݂��Ă��Ď��s�����ꍇ�A�폜����
  ExtSaveFile::Result delete_result = DeleteFile( file_path );
  if ( delete_result != ExtSaveFile::ACCESS_RESULT_OK 
      && delete_result != ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST )
  {
    return delete_result;
  }
  //ACCESS_RESULT_NG_PATH_NOT_EXIST�F���^�f�[�^�ɑ��݂��Ȃ������ۂɂ͑��݂����ꍇ�B
  //���G���[�����폜�͂���Ă���B�쐬�����݂�
  //�t�@�C���̍폜�ɐ���������A�ēx�쐬���Ă݂�
  return CreateFile( file_path, size );

}

//-----------------------------------------------------------------------------
/**
 * @brief   �t�@�C���폜
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::DeleteFile( const wchar_t* file_path )
{
  GFL_PRINT("ExtSaveSystem::DeleteFile()\n");
  m_last_result = nn::fs::TryDeleteFile( file_path );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief   �t�@�C�����ύX
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::RenameFile( const wchar_t* current_path, const wchar_t* new_path )
{
  GFL_PRINT("ExtSaveSystem::RenameFile()\n");
  m_last_result = nn::fs::TryRenameFile( current_path, new_path );
  if ( m_last_result.IsFailure() )
  {
    NN_DBG_PRINT_RESULT( m_last_result );
  }
  return ExtSaveFile::GetAccessResult( m_last_result );
}

//-----------------------------------------------------------------------------
/**
 * @brief   �t�@�C���ǂݍ���
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::ReadFile( const wchar_t* file_path, void * dst, size_t size )
{
  GFL_PRINT("ExtSaveSystem::ReadFile()\n");
  gfl::fs::ExtSaveFile file;
  bool exist_flag = file.Open(file_path, OPEN_MODE_READ);
  gfl::fs::ExtSaveFile::Result access_result;
  access_result = file.GetLastAccessResult();
  m_last_result = file.GetLastAccessNNResult();
  if ( access_result == gfl::fs::ExtSaveFile::ACCESS_RESULT_OK )
  {
    size_t filesize = file.GetSize();
    if ( filesize < size )
    {
      GFL_PRINT("ExtSaveSystem:: filesize(%d)<read_size(%d)\n", filesize, size );
      size = filesize;
    }
    size_t readsize = file.Read( dst, size );
    file.Close();
    access_result = file.GetLastAccessResult();
    m_last_result = file.GetLastAccessNNResult();
  }
  return access_result;
}

//-----------------------------------------------------------------------------
/**
 * @brief   �t�@�C��������
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::WriteFile( const wchar_t* file_path, const void * src, size_t size )
{
  GFL_PRINT("ExtSaveSystem::WriteFile()\n");
  gfl::fs::ExtSaveFile file;
  bool exist_flag = file.Open(file_path, OPEN_MODE_WRITE_ADD);
  gfl::fs::ExtSaveFile::Result access_result;
  access_result = file.GetLastAccessResult();
  m_last_result = file.GetLastAccessNNResult();
  if ( access_result == gfl::fs::ExtSaveFile::ACCESS_RESULT_OK )
  {
    size_t filesize = file.GetSize();
    if ( filesize < size )
    {
      GFL_PRINT("ExtSaveSystem:: filesize(%d)<write_size(%d)\n", filesize, size );
      size = filesize;
    }
    size_t writesize = file.Write( src, size );
    file.Close();
    access_result = file.GetLastAccessResult();
    m_last_result = file.GetLastAccessNNResult();
  }
  return access_result;
}

//-----------------------------------------------------------------------------
/**
 * @brief   �t�@�C�����݃`�F�b�N
 */
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::IsFileExists( const wchar_t* file_path, bool *exist_flag )
{
  GFL_PRINT("ExtSaveSystem::IsFileExists()\n");
  gfl::fs::ExtSaveFile file;
  *exist_flag  = (file.Open(file_path, OPEN_MODE_READ) == true );
  gfl::fs::ExtSaveFile::Result access_result;
  access_result = file.GetLastAccessResult();
  m_last_result = file.GetLastAccessNNResult();
  if ( access_result == gfl::fs::ExtSaveFile::ACCESS_RESULT_OK )
  {
    file.Close();
    access_result = file.GetLastAccessResult();
    m_last_result = file.GetLastAccessNNResult();
  }
  else if ( access_result == gfl::fs::ExtSaveFile::ACCESS_RESULT_NG_BROKEN_DATA )
  { // nn::fs::ResultVerificationFailed(==ACCESS_RESULT_NG_BROKEN_DATA)�̏ꍇ��
    // �t�@�C��Open�����s�ł͂��邪�A���݂͂��Ă���
    *exist_flag = true;
  }
  return access_result;
}

//=============================================================================
//
//
//              �񓯊����N�G�X�g
//
//
//=============================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncMountAndFormat( gfl::heap::HeapBase * pHeap, u8 priority, const void * iconBuffer, size_t iconSize )
{
  if ( m_thread ) return false;
  initThreadParams();
  m_thread_icon_buffer  = iconBuffer;
  m_thread_icon_size    = iconSize;
  this->startThread( pHeap, priority, THREAD_MODE_MOUNT_AND_FORMAT );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncMount( gfl::heap::HeapBase * pHeap, u8 priority )
{
  if ( m_thread ) return false;
  initThreadParams();
  this->startThread( pHeap, priority, THREAD_MODE_MOUNT );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncUnmount( gfl::heap::HeapBase * pHeap, u8 priority )
{
  if ( m_thread ) return false;
  initThreadParams();
  this->startThread( pHeap, priority, THREAD_MODE_UNMOUNT );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncGetArchiveInfo( gfl::heap::HeapBase * pHeap, u8 priority )
{
  if ( m_thread ) return false;
  initThreadParams();
  this->startThread( pHeap, priority, THREAD_MODE_GET_ARCHIVE_INFO );
  return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncDeleteArchive( gfl::heap::HeapBase * pHeap, u8 priority )
{
  if ( m_thread ) return false;
  initThreadParams();
  this->startThread( pHeap, priority, THREAD_MODE_DELETE_ARCHIVE );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncReadDirectory( gfl::heap::HeapBase * pHeap,
    u8 priority, const wchar_t * dir_path, s32 * pReadNum,
    nn::fs::DirectoryEntry * pEntries, s32 numEntries )
{
  if ( m_thread ) return false;
  initThreadParams();
  m_thread_read_num     = pReadNum;
  m_thread_num_entries  = numEntries;
  m_thread_entries      = pEntries;
  setPath( m_thread_use_path, dir_path );
  this->startThread( pHeap, priority, THREAD_MODE_READ_DIRECTORY );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncCreateFile(
    gfl::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * file_path, s64 size )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, file_path );
  m_thread_use_size = size;
  this->startThread( pHeap, priority, THREAD_MODE_CREATE_FILE );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncCreateFileOW(
    gfl::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * file_path, s64 size )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, file_path );
  m_thread_use_size = size;
  this->startThread( pHeap, priority, THREAD_MODE_CREATE_FILE_OW );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncDeleteFile(
    gfl::heap::HeapBase * pHeap, u8 priority, const wchar_t * file_path )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, file_path );
  this->startThread( pHeap, priority, THREAD_MODE_DELETE_FILE );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncRenameFile(
    gfl::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * current_path, const wchar_t * new_path )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, current_path );
  setPath( m_thread_use_path2, new_path );
  this->startThread( pHeap, priority, THREAD_MODE_RENAME_FILE );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncReadFile(
    gfl::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * file_path, void * dst, size_t size )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, file_path );
  m_thread_dst_buffer = dst;
  m_thread_use_size = size;
  this->startThread( pHeap, priority, THREAD_MODE_READ_FILE );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncWriteFile(
    gfl::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * file_path, const void * src, size_t size )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, file_path );
  m_thread_src_buffer = src;
  m_thread_use_size = size;
  this->startThread( pHeap, priority, THREAD_MODE_WRITE_FILE );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncIsFileExists(
    gfl::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * file_path )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, file_path );
  this->startThread( pHeap, priority, THREAD_MODE_IS_FILE_EXISTS );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncCreateDirectory( gfl::heap::HeapBase * pHeap, u8 priority, const wchar_t * dir_path )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, dir_path );
  this->startThread( pHeap, priority, THREAD_MODE_CREATE_DIRECTORY );
  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncDeleteDirectory( gfl::heap::HeapBase * pHeap, u8 priority, const wchar_t * dir_path )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, dir_path );
  this->startThread( pHeap, priority, THREAD_MODE_DELETE_DIRECTORY );
  return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool ExtSaveSystem::StartAsyncRenameDirectory(
    gfl::heap::HeapBase * pHeap, u8 priority,
    const wchar_t * current_path, const wchar_t * new_path )
{
  if ( m_thread ) return false;
  initThreadParams();
  setPath( m_thread_use_path, current_path );
  setPath( m_thread_use_path2, new_path );
  this->startThread( pHeap, priority, THREAD_MODE_RENAME_DIRECTORY );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   �X���b�h�I���`�F�b�N
 */
//-----------------------------------------------------------------------------
bool ExtSaveSystem::IsAsyncFinished( void )
{
  if ( !m_thread ) return true;
  if ( m_thread->IsAlive() ) return false;

  GFL_DELETE m_thread;
  m_thread = NULL;
  m_thread_mode = THREAD_MODE_NOTHING;
  return true;
}
//-----------------------------------------------------------------------------
/*
 * @brief   �V�X�e������API�̓����I���҂�
 */
//-----------------------------------------------------------------------------
bool ExtSaveSystem::WaitSystemAPI( ExtSaveSystem::Result * result )
{
  if (IsAsyncFinished())
  {
    *result = m_thread_result;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief   �t�@�C������API�̓����I���҂�
 */
//-----------------------------------------------------------------------------
bool ExtSaveSystem::WaitFileAPI( ExtSaveFile::Result * result )
{
  if (IsAsyncFinished())
  {
    *result = m_thread_file_result;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ExtSaveFile::Result ExtSaveSystem::GetLastFileResult( void ) const
{
  return m_thread_file_result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ExtSaveSystem::Result ExtSaveSystem::GetLastSystemResult( void ) const
{
  return m_thread_result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
nn::Result ExtSaveSystem::GetLastNNResult( void ) const
{
  return m_last_result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ExtSaveSystem::GetArchiveInfoResult( u32 * entryDirectory, u32 * entryFile )
{
  *entryDirectory = m_thread_dir_count_result;
  *entryFile      = m_thread_file_count_result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ExtSaveSystem::GetFileExistsResult( bool * exists_flag )
{
  *exists_flag = m_thread_use_bool;
}

//=============================================================================
//
//
//        ����J����
//
//
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief ���i�łɎc���Ă͂����Ȃ��G���[�̃`�F�b�N
 */
//-----------------------------------------------------------------------------
 ExtSaveSystem::Result ExtSaveSystem::CheckProgrammersError( nn::Result result )
 {
   GFL_ASSERT_STOP ( nn::fs::ResultOutOfResource::Includes( result ) );
   GFL_ASSERT_STOP ( nn::fs::ResultAccessDenied::Includes( result ) );
   GFL_ASSERT_STOP ( nn::fs::ResultInvalidArgument::Includes( result ) );
   GFL_ASSERT_STOP ( nn::fs::ResultNotInitialized::Includes( result ) );
   GFL_ASSERT_STOP ( nn::fs::ResultAlreadyInitialized::Includes( result ) );
   GFL_ASSERT_STOP ( nn::fs::ResultUnsupportedOperation::Includes( result ) );
   return RESULT_OK;
 }

//-----------------------------------------------------------------------------
/**
 * @brief �}�E���g���̃G���[�`�F�b�N
 *
 * @param[in] result
 * @return  ExtSaveSystem::Result
 */
//-----------------------------------------------------------------------------
  ExtSaveSystem::Result ExtSaveSystem::CheckMountError( nn::Result result )
  {
    NN_DBG_PRINT_RESULT( result );
    if(result <= nn::fs::ResultNotFound())
    {
      if(result <= nn::fs::ResultMediaNotFound())
      {
        // SD �J�[�h���}�����Ă��܂���B
        // (�⑫) ��ꂽ SD �J�[�h�� SD �J�[�h�ł͂Ȃ����f�B�A���}����Ă���Ƃ��ɂ��A���̃G���[���Ԃ�܂��B
        //        �A���A���̏ꍇ�� RegisterSdmcInsertedEvent �œo�^�����}���C�x���g�̓V�O�i������܂��B
        return RESULT_NG_MEDIA_NOT_FOUND;
      }
      else
      {
        // �w�肵���g���Z�[�u�f�[�^�͑��݂��Ă��Ȃ��̂ŁA�쐬����K�v������܂��B
        // (�⑫) �J�����́A�w�肵�� ID �����������m�F�����Ă��������B
        return RESULT_NG_NOT_FOUND;
      }
    }
    else if(result <= nn::fs::ResultNotFormatted())
    {
      // �g���Z�[�u�f�[�^����蒼���K�v������܂��B
      // (���R) �g���Z�[�u�f�[�^�̍쐬�Ɏ��s������Ԃł��B
      return RESULT_NG_NOT_FORMATTED;
    }
    else if(result <= nn::fs::ResultBadFormat())
    {
      // SD �J�[�h���t�H�[�}�b�g����K�v������܂��B
      return RESULT_NG_BAD_FORMAT;
    }
    else if(result <= nn::fs::ResultVerificationFailed())
    {
      // �g���Z�[�u�f�[�^����蒼���K�v������܂��B
      // (���R) �Z�[�u�f�[�^�����Ă��邩�A��₂���Ă��܂��B
      return RESULT_NG_VERIFICATION_FAILED;
    }
    else if(result <= nn::fs::ResultOperationDenied())
    {
      if(result <= nn::fs::ResultWriteProtected())
      {
        // SD �J�[�h���������݋֎~�ɂ���Ă��܂��B
        // (�⑫) �}�E���g���Ƀf�[�^�̕����V�[�P���X�����s����邱�Ƃ�����A���̂Ƃ��ɔ������܂��B
        return RESULT_NG_WRITE_PROTECTED;
      }
      else if(result <= nn::fs::ResultMediaAccessError())
      {
        // �ڐG�s�Ǔ��ɂ��n�[�h�E�F�A�I�ȗv���̂Ƃ��̂݁A���̃G���[���Ԃ�܂��B
        // ���̂Ƃ��́ASD �J�[�h�̑}�������A�{�̂̍ċN���Ȃǂŕ��A����\��������܂��B
        return RESULT_NG_ERROR;
      }
      else
      {
        // SD �J�[�h��̃t�@�C���A�������̓f�B���N�g�����ǂݎ���p�ɂ���Ă���\��������܂��B
        // (�⑫) �}�E���g���Ƀf�[�^�̕����V�[�P���X�����s����邱�Ƃ�����A���̂Ƃ��ɔ������܂��B
        return RESULT_NG_WRITE_PROTECTED;
      }
    }
    else
    {
      // ��L�ȊO�́A�z��O�̃G���[�ł��B���̊g���Z�[�u�f�[�^�A�������� SD �J�[�h�͔F���ł��܂���ł����B
      // NN_ERR_THROW_FATAL �n�̃}�N���͌Ăяo���Ȃ��悤�ɂ��Ă��������B
      CheckProgrammersError( result );
      //GFL_ASSERT_STOP( 0 );
      return RESULT_NG_UNKNOWN;
    }
  }

//-----------------------------------------------------------------------------
/**
 * @brief     �g���Z�[�u�f�[�^�폜���̃G���[�`�F�b�N
 *
 * @param[in] result
 * @return  ExtSaveSystem::Result
 */
//-----------------------------------------------------------------------------
  ExtSaveSystem::Result ExtSaveSystem::CheckDeleteError( nn::Result result )
  {
    NN_DBG_PRINT_RESULT( result );
    if(result <= nn::fs::ResultNotFound())
    {
      if(result <= nn::fs::ResultMediaNotFound())
      {
        // SD �J�[�h���}�����Ă��܂���B
        // (�⑫) ��ꂽ SD �J�[�h�� SD �J�[�h�ł͂Ȃ����f�B�A���}����Ă���Ƃ��ɂ��A���̃G���[���Ԃ�܂��B
        //        �A���A���̏ꍇ�� RegisterSdmcInsertedEvent �œo�^�����}���C�x���g�̓V�O�i������܂��B
        return RESULT_NG_MEDIA_NOT_FOUND;
      }
      else
      {
        // �w�肵���g���Z�[�u�f�[�^�͑��݂��Ă��Ȃ��̂ŁA�쐬����K�v������܂��B
        // (�⑫) �J�����́A�w�肵�� ID �����������m�F�����Ă��������B
        return RESULT_NG_NOT_FOUND;
      }
    }
    else if(result <= nn::fs::ResultBadFormat())
    {
      // SD �J�[�h���t�H�[�}�b�g����K�v������܂��B
      return RESULT_NG_BAD_FORMAT;
    }
    else if(result <= nn::fs::ResultOperationDenied())
    {
      if(result <= nn::fs::ResultWriteProtected())
      {
        // SD �J�[�h���������݋֎~�ɂ���Ă��܂��B
        // (�⑫) �}�E���g���Ƀf�[�^�̕����V�[�P���X�����s����邱�Ƃ�����A���̂Ƃ��ɔ������܂��B
        return RESULT_NG_WRITE_PROTECTED;
      }
      else if(result <= nn::fs::ResultMediaAccessError())
      {
        // �ڐG�s�Ǔ��ɂ��n�[�h�E�F�A�I�ȗv���̂Ƃ��̂݁A���̃G���[���Ԃ�܂��B
        // ���̂Ƃ��́ASD �J�[�h�̑}�������A�{�̂̍ċN���Ȃǂŕ��A����\��������܂��B
        return RESULT_NG_ERROR;
      }
      else
      {
        //�w�肵���g���Z�[�u�f�[�^�͌��݃}�E���g���̂��߁A����͓r���Ŏ��s���܂����B 
        return RESULT_NG_MOUNTING;
      }
    }
    else
    {
      // ��L�ȊO�́A�z��O�̃G���[�ł��B���̊g���Z�[�u�f�[�^�A�������� SD �J�[�h�͔F���ł��܂���ł����B
      // NN_ERR_THROW_FATAL �n�̃}�N���͌Ăяo���Ȃ��悤�ɂ��Ă��������B
      CheckProgrammersError( result );
      //GFL_ASSERT_STOP( 0 );
      return RESULT_NG_UNKNOWN;
    }
  }


//-----------------------------------------------------------------------------
/**
 * @brief   �g���Z�[�u�f�[�^�̈�쐬���̃G���[�`�F�b�N
 *
 * @param[in] result
 * @return  ExtSaveSystem::Result
 */
//-----------------------------------------------------------------------------
  ExtSaveSystem::Result ExtSaveSystem::CheckCreateError( nn::Result result )
  {
    NN_DBG_PRINT_RESULT( result );
    if(result.IsFailure())
    {
      if(result <= nn::fs::ResultMediaNotFound())
      {
        // SD �J�[�h���}�����Ă��܂���B
        // (�⑫) ��ꂽ SD �J�[�h�� SD �J�[�h�ł͂Ȃ����f�B�A���}����Ă���Ƃ��ɂ��A���̃G���[���Ԃ�܂��B
        return RESULT_NG_MEDIA_NOT_FOUND;
      }
      else if(result <= nn::fs::ResultBadFormat())
      {
        // SD �J�[�h���t�H�[�}�b�g����K�v������܂��B
        return RESULT_NG_BAD_FORMAT;
      }
      if(result <= nn::fs::ResultNotEnoughSpace())
      {
        // SD �J�[�h�ɕK�v�ȋ󂫗e�ʂ�����܂���B
        return RESULT_NG_NOT_ENOUGH_SPACE;
      }
      else if(result <= nn::fs::ResultNotFormatted())
      {
        // �G���[���������A�쐬���r���Œ��f����܂����B
        return RESULT_NG_ERROR;
      }
      else if(result <= nn::fs::ResultOperationDenied())
      {
        if(result <= nn::fs::ResultWriteProtected())
        {
          // SD �J�[�h���������݋֎~�ɂ���Ă��܂��B
          return RESULT_NG_WRITE_PROTECTED;
        }
        else if(result <= nn::fs::ResultMediaAccessError())
        {
          // �ڐG�s�Ǔ��ɂ��n�[�h�E�F�A�I�ȗv���̂Ƃ��̂݁A���̃G���[���Ԃ�܂��B
          // ���̂Ƃ��́A���g���C�����A�Q�[���J�[�h�̑}�������A�{�̂̍ċN���Ȃǂŕ��A����\��������܂��B
          return RESULT_NG_ERROR;
        }
        else
        {
          // SD �J�[�h��̃f�B���N�g�����ǂݎ���p�ɂ���Ă���\��������܂��B
          // (�⑫) �}�E���g���Ƀf�[�^�̕����V�[�P���X�����s����邱�Ƃ�����A���̂Ƃ��ɔ������܂��B
          return RESULT_NG_WRITE_PROTECTED;
        }
      }
      else
      {
        // ���̊g���Z�[�u�f�[�^�A�������� SD �J�[�h�͔F���ł��܂���ł����B
        // �����ɊY������G���[�́A�܂Ƃ߂ăn���h�����O���s���Ă��������B(FATAL �G���[�\����ʂɂ͑J�ڂ����Ȃ��ł��������B)
        // (�⑫) �J�����́AiconData, iconDataSize �ɕs���Ȓl��n���Ă��Ȃ����m���߂Ă��������B
        CheckProgrammersError( result );
        return RESULT_NG_UNKNOWN;
      }
    }
    return RESULT_OK;
  }

//-----------------------------------------------------------------------------
/**
 * @brief   �g���Z�[�u�f�[�^�̈�̏��擾���̃G���[�`�F�b�N
 *
 * @param[in] result
 * @return  ExtSaveSystem::Result
 */
//-----------------------------------------------------------------------------
  ExtSaveSystem::Result ExtSaveSystem::CheckInfoError( nn::Result result )
  {
    NN_DBG_PRINT_RESULT( result );
    if(result <= nn::fs::ResultNotFound())
    {
      if(result <= nn::fs::ResultMediaNotFound())
      {
        // SD �J�[�h���}�����Ă��܂���B
        // (�⑫) ��ꂽ SD �J�[�h�� SD �J�[�h�ł͂Ȃ����f�B�A���}����Ă���Ƃ��ɂ��A���̃G���[���Ԃ�܂��B
        //        �A���A���̏ꍇ�� RegisterSdmcInsertedEvent �œo�^�����}���C�x���g�̓V�O�i������܂��B
        return RESULT_NG_MEDIA_NOT_FOUND;
      }
      else
      {
        // �w�肵���g���Z�[�u�f�[�^�͑��݂��Ă��Ȃ��̂ŁA�쐬����K�v������܂��B
        // (�⑫) �J�����́A�w�肵�� ID �����������m�F�����Ă��������B
        return RESULT_NG_NOT_FOUND;
      }
    }
    else if(result <= nn::fs::ResultBadFormat())
    {
      // SD �J�[�h���t�H�[�}�b�g����K�v������܂��B
      return RESULT_NG_BAD_FORMAT;
    }
    else if(result <= nn::fs::ResultOperationDenied())
    {
      if(result <= nn::fs::ResultWriteProtected())
      {
        // SD �J�[�h���������݋֎~�ɂ���Ă��܂��B
        // (�⑫) �}�E���g���Ƀf�[�^�̕����V�[�P���X�����s����邱�Ƃ�����A���̂Ƃ��ɔ������܂��B
        return RESULT_NG_WRITE_PROTECTED;
      }
      else if(result <= nn::fs::ResultMediaAccessError())
      {
        // �ڐG�s�Ǔ��ɂ��n�[�h�E�F�A�I�ȗv���̂Ƃ��̂݁A���̃G���[���Ԃ�܂��B
        // ���̂Ƃ��́ASD �J�[�h�̑}�������A�{�̂̍ċN���Ȃǂŕ��A����\��������܂��B
        return RESULT_NG_ERROR;
      }
      else
      {
        // SD �J�[�h��̃f�B���N�g�����ǂݎ���p�ɂ���Ă���\��������܂��B
        // (�⑫) �}�E���g���Ƀf�[�^�̕����V�[�P���X�����s����邱�Ƃ�����A���̂Ƃ��ɔ������܂��B
        return RESULT_NG_WRITE_PROTECTED;
      }
    }
    else
    {
      // ��L�ȊO�́A�z��O�̃G���[�ł��B���̊g���Z�[�u�f�[�^�A�������� SD �J�[�h�͔F���ł��܂���ł����B
      // NN_ERR_THROW_FATAL �n�̃}�N���͌Ăяo���Ȃ��悤�ɂ��Ă��������B
      CheckProgrammersError( result );
      //GFL_ASSERT_STOP( 0 );
      return RESULT_NG_UNKNOWN;
    }
  }


//-----------------------------------------------------------------------------
/**
 * @brief �X���b�h���C������
 *
 * ExtSaveThread����CallBack�Ƃ��ČĂ΂�郁�C�������B
 */
//-----------------------------------------------------------------------------
void ExtSaveSystem::ThreadCallBack( void )
{
  switch ( m_thread_mode )
  {
  case THREAD_MODE_NOTHING:
    return;
  case THREAD_MODE_MOUNT_AND_FORMAT:
    m_thread_result = this->MountAndFormat( m_thread_icon_buffer, m_thread_icon_size );
    return;
  case THREAD_MODE_MOUNT:
    m_thread_result = this->Mount();
    return;
  case THREAD_MODE_UNMOUNT:
    m_thread_result = this->Unmount();
    return;
  case THREAD_MODE_GET_ARCHIVE_INFO:
    m_thread_result = this->GetArchiveInfo(
        &m_thread_dir_count_result, &m_thread_file_count_result );
    return;
  case THREAD_MODE_DELETE_ARCHIVE:
    m_thread_result = this->DeleteArchive();
    return;
  case THREAD_MODE_READ_DIRECTORY:
    m_thread_file_result = this->ReadDirectory(
        m_thread_heap, m_thread_use_path,
        m_thread_read_num, m_thread_entries, m_thread_num_entries );
    return;

  case THREAD_MODE_CREATE_FILE:
    m_thread_file_result = this->CreateFile(
        m_thread_use_path, m_thread_use_size );
    return;
  case THREAD_MODE_CREATE_FILE_OW:
    m_thread_file_result = this->CreateFileOW(
        m_thread_use_path, m_thread_use_size );
    return;
  case THREAD_MODE_DELETE_FILE:
    m_thread_file_result = this->DeleteFile( m_thread_use_path );
    return;
  case THREAD_MODE_RENAME_FILE:
    m_thread_file_result = this->RenameFile(
        m_thread_use_path, m_thread_use_path2 );
    return;
  case THREAD_MODE_READ_FILE:
    m_thread_file_result = this->ReadFile(
        m_thread_use_path, m_thread_dst_buffer, m_thread_use_size );
    return;
  case THREAD_MODE_WRITE_FILE:
    m_thread_file_result = this->WriteFile(
        m_thread_use_path, m_thread_src_buffer, m_thread_use_size );
    return;
  case THREAD_MODE_IS_FILE_EXISTS:
    m_thread_file_result = this->IsFileExists(
        m_thread_use_path, &m_thread_use_bool );
    return;
  case THREAD_MODE_CREATE_DIRECTORY:
    m_thread_file_result = this->CreateDirectory( m_thread_use_path );
    return;
  case THREAD_MODE_DELETE_DIRECTORY:
    m_thread_file_result = this->DeleteDirectory( m_thread_use_path );
    return;
  case THREAD_MODE_RENAME_DIRECTORY:
    m_thread_file_result = this->RenameDirectory(
        m_thread_use_path, m_thread_use_path2 );
    return;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �񓯊������p���[�N�̏���������
 */
//-----------------------------------------------------------------------------
void ExtSaveSystem::initThreadParams( void )
{
  GFL_ASSERT_STOP( m_thread == NULL );

  m_thread_use_path[0]        = L'\0';
  m_thread_use_path2[0]       = L'\0';
  m_thread_use_size           = 0;
  m_thread_use_bool           = false;

  m_thread_result             = gfl::fs::ExtSaveSystem::RESULT_OK;;
  m_thread_file_result        = gfl::fs::ExtSaveFile::ACCESS_RESULT_OK;
  m_thread_dir_count_result   = 0;
  m_thread_file_count_result  = 0;
  m_thread_entries            = NULL;
  m_thread_read_num           = NULL;
  m_thread_num_entries        = 0;
  m_thread_dst_buffer         = NULL;
  m_thread_src_buffer         = NULL;

  m_thread_icon_buffer        = NULL;
  m_thread_icon_size          = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief �p�X���i�[����
 */
//-----------------------------------------------------------------------------
void ExtSaveSystem::setPath( wchar_t * dst, const wchar_t * src )
{
  if ( src )
  {
    wcsncpy( dst, src, MAX_FILE_PATH_LENGTH - 1 );
    if ( wcslen(src) >= MAX_FILE_PATH_LENGTH - 1 )
    {
      dst[MAX_FILE_PATH_LENGTH - 1] = L'\0';
    }
  }
  else
  {
    dst[0] = L'\0';
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �X���b�h�J�n����
 * @param[in] pHeap       �X���b�h�����Ɏg�p����q�[�v
 * @param[in] priority    �X���b�h�̓���v���C�I���e�B
 * @param[in] mode        �X���b�h�̓��샂�[�h
 */
//-----------------------------------------------------------------------------
void ExtSaveSystem::startThread(
    gfl::heap::HeapBase * pHeap, u8 priority, THREAD_MODE mode )
{
  if ( m_thread ) return;

  m_thread_heap = pHeap;
  m_thread_mode = mode;
  m_thread      = GFL_NEW( pHeap ) ExtSaveThread( pHeap, THREAD_STACK_SIZE, this );
  m_thread->Start( priority );
}



  }  // namespace fs
}  // namespace gfl
