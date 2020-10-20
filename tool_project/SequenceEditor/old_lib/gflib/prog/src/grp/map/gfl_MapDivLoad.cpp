//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MapDivLoad.cpp
 *	@brief  �}�b�v�u���b�N�����ǂݍ���
 *	@author	tomoya takahashi
 *	@date		2011.01.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <grp/map/gfl_MapDivLoad.h>


namespace gfl{
namespace grp{
namespace map{

#define GFL_MAP_DIVLOAD_THREAD (1)

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					����
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
DivLoad::DivLoad( void ) : 
  m_Priority(0),
  m_CompFlag(false),
  m_pLoadBuff(NULL),
  m_pBuff(NULL),
  m_BuffSize(0),
  m_pArcFile(NULL),
  m_OneLoadSize(0),
  m_MemoryOffset(0),
  m_FileOffset(0),
  m_FileSize(0),
  m_pTmpHeap(NULL)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
DivLoad::~DivLoad()
{
  Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  ������
 *
 *	@param	p_buff      �o�b�t�@
 *	@param	buffSize    �o�b�t�@�T�C�Y
 *	@param  p_tmp_heap  �X���b�h����ɕK�v�ȃe���|�������[�N�m�ۗp
 *	@param  mode        ���샂�[�h
 *	@param  priority    �X���b�h�D�揇��
 */
//-----------------------------------------------------------------------------
void DivLoad::Initialize( void* p_buff, u32 buffSize, gfl::heap::HeapBase* p_tmp_heap, Mode mode, gfl::base::Thread::Priority priority )
{
  // �ǂݍ��ݒ��`�F�b�N
  if( IsLoading() ){
    GFL_ASSERT( 0 );
    u32 count = 0;
    while( (ResetFileLoad() == false) && (count < 10000)  ){ 
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMicroSeconds( 10000 ) );
      ++count;
    }
    GFL_ASSERT_STOP( count < 10000 );
  }
  
  m_pLoadBuff   = p_buff;
  m_pBuff       = p_buff;
  m_BuffSize    = buffSize;
  m_pTmpHeap    = p_tmp_heap;
  if( mode < MODE_MAX ) {
    m_Mode      = mode;
    m_Priority  = priority;
  }else{
    GFL_ASSERT( mode < MODE_MAX ); //@check
    m_Mode      = MODE_PEACE_LOAD;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 */
//-----------------------------------------------------------------------------
void DivLoad::Finalize( void )
{
  // ���ɔj��������̂͂Ȃ��B
}


//----------------------------------------------------------------------------
/**
 *	@brief  �t�@�C���ǂݍ��݊J�n
 *
 *	@param	  p_arc         �A�[�J�C�u�t�@�C��
 *	@param    dataID        �f�[�^ID
 *	@param    one_load_size 1��̓ǂݍ��݃T�C�Y
 */
//-----------------------------------------------------------------------------
void DivLoad::StartFileLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size )
{
  if(m_Mode == MODE_THREAD_LOAD){
    this->startFileSyncLoad( p_arc, dataID, one_load_size );
  }else{
    this->startFilePieceLoad( p_arc, dataID, one_load_size );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���k�̊|�������t�@�C���̓ǂݍ��݊J�n
 *
 *	@param	p_arc         �A�[�J�C�u�t�@�C��
 *	@param  dataID        �f�[�^�h�c
 *	@param  one_load_size ��x�ɓǂݍ��ރT�C�Y
 *	@param  gfl::heap::HeapBase* p_tmp_heap �e���|�����o�b�t�@�����p�q�[�v
 */
//-----------------------------------------------------------------------------
void DivLoad::StartCompFileLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size, gfl::heap::HeapBase* p_tmp_heap )
{
  if(m_Mode == MODE_THREAD_LOAD){
    this->startFileSyncLoadComp( p_arc, dataID, one_load_size, p_tmp_heap );
  }else{
    this->startFilePieceLoadComp( p_arc, dataID, one_load_size, p_tmp_heap );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �t�@�C���ǂݍ���1�񕪎��s
 *
 *  @retval true    �t�@�C���ǂݍ��݊���
 *  @retval false   �t�@�C���ǂݍ��ݓr��
 */
//-----------------------------------------------------------------------------
b32 DivLoad::ContinueFileLoad( void )
{
  if(m_Mode == MODE_THREAD_LOAD){
   return this->continueFileSyncLoad();
  }
  return this->continueFilePieceLoad();
}


//----------------------------------------------------------------------------
/**
 *	@brief  �t�@�C���ǂݍ��݁@���Z�b�g
 */
//-----------------------------------------------------------------------------
bool DivLoad::ResetFileLoad( void )
{
  if(m_Mode == MODE_THREAD_LOAD){
    return this->resetFileSyncLoad();
  }
  else{
    this->resetFilePieceLoad();
  }
  return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ǂݍ��ݒ����`�F�b�N
 *
 *	@retval true    �ǂݍ��ݒ�
 *	@retval false   �ǂݍ���łȂ�
 */
//-----------------------------------------------------------------------------
b32 DivLoad::IsLoading( void ) const
{
  if(m_Mode == MODE_THREAD_LOAD){
    return this->isLoadingFileSyncLoad();
  }
  return this->isLoadingFilePieceLoad();
}




//----------------------------------------------------------------------------
/**
 *	@brief  �����ǂݍ��݊J�n
 *
 *	@param	p_arc             �A�[�J�C�u�t�@�C��
 *	@param	dataID            �f�[�^ID
 *	@param	one_load_size     �P��̓ǂݍ��݃T�C�Y
 */
//-----------------------------------------------------------------------------
void DivLoad::startFilePieceLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size )
{
  if(one_load_size == 0){
    GFL_ASSERT( one_load_size ); //@check
    one_load_size = 1;
  }

  
  m_pArcFile    = p_arc;
  m_DataID      = dataID;
  m_FileSize    = p_arc->GetDataSize( dataID );
  m_OneLoadSize = one_load_size;
  m_FileOffset  = 0;

  // �V�[�N
  m_pArcFile->SeekDataPtr( m_DataID, m_FileOffset );

  // �o�b�t�@�I�[�o�[�`�F�b�N
  GFL_ASSERT_STOP( (m_FileSize + m_MemoryOffset) < m_BuffSize );

  m_CompFlag = false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����ǂݍ��݁@���k�t�@�C���̏ꍇ
 *
 *	@param	p_arc               �A�[�J�C�u�t�@�C��
 *	@param	dataID              �f�[�^�h�c
 *	@param	one_load_size       �P��̓ǂݍ��݃T�C�Y
 *	@param	p_tmp_heap          ���k�f�[�^�ǂݍ��݂悤�ꎞ�o�b�t�@�m�ې�
 */
//-----------------------------------------------------------------------------
void DivLoad::startFilePieceLoadComp( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size, gfl::heap::HeapBase* p_tmp_heap )
{
  if(one_load_size == 0){
    GFL_ASSERT( one_load_size ); //@check
    one_load_size = 1;
  }

  
  m_pArcFile    = p_arc;
  m_DataID      = dataID;
  m_FileSize    = p_arc->GetDataSize( dataID );
  m_OneLoadSize = one_load_size;
  m_FileOffset  = 0;

  // �V�[�N
  m_pArcFile->SeekDataPtr( m_DataID, m_FileOffset );

  // �ǂݍ��ݗp�o�b�t�@����
  m_pLoadBuff = GflHeapAllocMemoryLow( p_tmp_heap, m_FileSize );
  

  m_CompFlag = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����ǂݍ��݂̍X�V
 *
 *	@retval true  ����
 *	@retval false �p��
 */
//-----------------------------------------------------------------------------
b32 DivLoad::continueFilePieceLoad(void)
{
  //��ǂݍ��ݓr���`�F�b�N
  if( m_FileOffset >= m_FileSize ){
    return true;
  }

  b32 ret = false;
  u32 loadSize = 0;
  uptr buffAddr = reinterpret_cast<uptr>(m_pLoadBuff);
  u32 memory_offset;

  // ���C�X�Ɠǂݍ��݃`�F�b�N
  if( (m_FileOffset + m_OneLoadSize) >= m_FileSize ){
    loadSize  = m_FileSize - m_FileOffset;
    ret = true;
  }else{
    loadSize = m_OneLoadSize;
  }

  //GFL_PRINT( "Offset 0x%x loadOne 0x%x\n", m_FileOffset, loadSize );

  // �o�b�t�@�̓ǂݍ��ݐ�����߂�
  if( m_CompFlag ){
    memory_offset = m_FileOffset;
  }else{
    memory_offset = m_MemoryOffset;
  }

  // �ǂݍ���
  m_pArcFile->LoadDataPtr( loadSize, reinterpret_cast<void*>(buffAddr + memory_offset) );
  m_FileOffset += loadSize;

  if( !m_CompFlag ){
    m_MemoryOffset += loadSize;
  }


  if( ret ){
    // ���k�f�[�^��
    if( m_CompFlag ){

      uptr destAddr = reinterpret_cast<uptr>(m_pBuff);
      m_FileSize = gfl::cx::GetUncompressedSize( m_pLoadBuff );
      // �o�b�t�@�I�[�o�[�`�F�b�N
      GFL_ASSERT_STOP( (m_FileSize + m_MemoryOffset) < m_BuffSize );

      gfl::cx::Uncompress( m_pLoadBuff, reinterpret_cast<void*>(destAddr + m_MemoryOffset) );  // ��
      GflHeapFreeMemory( m_pLoadBuff );
      m_pLoadBuff = m_pBuff;
      m_CompFlag = false;
    }
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����ǂݍ��݂̃��Z�b�g
 */
//-----------------------------------------------------------------------------
void DivLoad::resetFilePieceLoad(void)
{
  m_pArcFile    = NULL;
  m_DataID      = 0;
  m_OneLoadSize = 0;
  m_MemoryOffset= 0;
  m_FileSize    = 0;
  m_FileOffset  = 0;

  if( m_CompFlag ){
    GflHeapFreeMemory( m_pLoadBuff );
    m_pLoadBuff = NULL;
    m_pLoadBuff = m_pBuff;
    m_CompFlag = false;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ǂݍ��݃`�F�b�N
 *
 *	@retval true    �ǂݍ��ݒ�
 *	@retval false   �ǂݍ��݊���
 */
//-----------------------------------------------------------------------------
b32 DivLoad::isLoadingFilePieceLoad(void) const
{
  if( m_FileSize > m_FileOffset ){
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �X���b�h�ǂݍ��݊J�n
 *
 *	@param	p_arc           �A�[�J�C�u�t�@�C��
 *	@param	dataID          �f�[�^ID
 *	@param  one_load_size   �P��̓ǂݍ��݃T�C�Y�@�i0=�f�[�^�T�C�Y)
 */
//-----------------------------------------------------------------------------
void DivLoad::startFileSyncLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size )
{
  bool result;

  m_pArcFile    = p_arc;
  m_FileSize    = p_arc->GetDataSize( dataID );
  m_DataID      = dataID;
  m_FileOffset  = 0;


  if(one_load_size == 0){
    one_load_size = m_FileSize;
  }
  m_OneLoadSize = one_load_size;

  // �o�b�t�@�I�[�o�[�`�F�b�N
  GFL_ASSERT_STOP( (m_FileSize + m_MemoryOffset) < m_BuffSize );

  //GFL_PRINT( "arc start data_id %d\n", dataID );
  m_pLoadBuff = m_pBuff;

  // �񓯊��ǂݍ��݊J�n
  uptr buffAddr = reinterpret_cast<uptr>(m_pLoadBuff);
  result = m_pArcFile->StartAsyncRead( m_pTmpHeap, dataID, 
      reinterpret_cast<void*>(buffAddr + m_MemoryOffset),
      m_Priority  );


  GFL_ASSERT_STOP( result );

  m_CompFlag = false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���k�`���̃t�@�C�����X���b�h�ǂݍ��݊J�n
 *
 *	@param	p_arc           �A�[�J�C�u�t�@�C��
 *	@param	dataID          �f�[�^�h�c
 *	@param	one_load_size   �P��̓ǂݍ��݃T�C�Y�i0=�f�[�^�T�C�Y�j
 *	@param  p_tmp_heap      �e���|�����o�b�t�@�m�ۃq�[�v�x�[�X
 */
//-----------------------------------------------------------------------------
void DivLoad::startFileSyncLoadComp( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size, gfl::heap::HeapBase* p_tmp_heap )
{
  bool result;
  m_pArcFile    = p_arc;
  m_FileSize    = p_arc->GetDataSize( dataID );
  m_DataID      = dataID;
  m_FileOffset  = 0;


  if(one_load_size == 0){
    one_load_size = m_FileSize;
  }
  m_OneLoadSize = one_load_size;

  // �ǂݍ��݂悤���������m��
  m_pLoadBuff = GflHeapAllocMemoryLow( p_tmp_heap, m_FileSize );

  // �񓯊��ǂݍ��݊J�n
  result = m_pArcFile->StartAsyncRead( m_pTmpHeap, dataID, 
      m_pLoadBuff,
      m_Priority  );


  GFL_ASSERT_STOP( result );
  m_CompFlag = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X���b�h�ǂݍ��݁@�I���܂�
 *
 *	@retval true    �I��
 *	@retval false   �p��
 */
//-----------------------------------------------------------------------------
b32 DivLoad::continueFileSyncLoad(void)
{
  if( m_pArcFile->WaitAsyncRead() == false ){

    //GFL_PRINT( "arc end\n" );
  
    if( m_CompFlag ){

      uptr buffAddr = reinterpret_cast<uptr>(m_pBuff);

      // LoadBuffer�̓��e���𓀂��āAm_pBuffer�Ɋi�[����B
      // m_pBuffer��m_MemoryOffset�̐悩��i�[����B
      m_FileSize = gfl::cx::GetUncompressedSize( m_pLoadBuff );
      
      // �o�b�t�@�I�[�o�[�`�F�b�N
      GFL_ASSERT_STOP( (m_FileSize + m_MemoryOffset) < m_BuffSize );
      gfl::cx::Uncompress( m_pLoadBuff, reinterpret_cast<void*>(buffAddr + m_MemoryOffset) );  // ��

      GflHeapFreeMemory( m_pLoadBuff );
      m_pLoadBuff = m_pBuff;
      m_CompFlag = false;
    }
    
    
    m_FileOffset    = m_FileSize;
    m_MemoryOffset += m_FileSize;

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X���b�h�ǂݍ��݁@���f
 *	��one_load_size���ǂ񂾂Ƃ���Œ��f���܂��B
 */
//-----------------------------------------------------------------------------
bool DivLoad::resetFileSyncLoad(void)
{
  if(m_pArcFile){
    // �ǂݍ��݂���ׂ��B
    if( m_pArcFile->WaitAsyncRead() ){

      // �����I�ɂƂ߂�B
      //m_pArcFile->CancelAsyncRead();
      TOMOYA_PRINT( "DivLoad cancel \n" );
      
      return false;
    }
  }

  m_pArcFile    = NULL;
  m_DataID      = 0;
  m_OneLoadSize = 0;
  m_MemoryOffset= 0;
  m_FileSize    = 0;
  m_FileOffset  = 0;


  if( m_CompFlag ){
    GflHeapFreeMemory( m_pLoadBuff );
    m_pLoadBuff = NULL;
    m_pLoadBuff = m_pBuff;
    m_CompFlag = false;
  }
  
  return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ǂݍ��݃`�F�b�N
 *
 *	@retval true    �ǂݍ��ݒ�
 *	@retval false   �ǂݍ��݊���
 */
//-----------------------------------------------------------------------------
b32 DivLoad::isLoadingFileSyncLoad(void) const
{
  if( m_FileSize > m_FileOffset ){
    return true;
  }
  return false;
}



} //namespace map{
} //namespace grp{  
} //namespace gfl{
