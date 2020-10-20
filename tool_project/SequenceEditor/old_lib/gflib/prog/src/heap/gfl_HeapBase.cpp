//======================================================================
/**
 * @file	gfl_HeapBase.cpp
 * @brief	GFL�q�[�v���N���X
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#include <string.h>
#include <gflib.h>
#include "gfl_HeapLocal.h"
#include <heap/gfl_HeapBaseLower.h>

namespace gfl
{
namespace heap
{

#if GFL_HEAP_DEBUG
  u8 GflHeapBase::mDebugFreeMemClear = GflHeapBase::DEBUG_FREE_MEMC_NONE;
#endif // GFL_HEAP_DEBUG

//======================================================================
//	define
//======================================================================
//#pragma mark [> define

//----------------------------------------------------------------------------
/**
 *  @brief  ���������m��
 *          �������̊m�ۂ͂��̃}�N�����g���čs���Ă��������B
 *          AllocMemoryBlock��AllocMemoryBlockAlign�𒼐ڎg�p���Ȃ��ł��������B
 *
 *  @param  heap             HeapBase�N���X(����) �������� HEAPID(�Ăяo���֐����I�[�o�[���[�h���Ă��܂�)
 *  @param  const int size   �m�ۃT�C�Y
 *  @param  const int align  �A���C�����g(�}�C�i�X�Ō������m��)
 *
 *  @return void* �m�ۃ�����(0�N���A����Ă��܂�)
 */
//-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
//AllocMemoryBlock HeapBase��
void *GflHeapAllocMemoryBlock( HeapBase *heapBase ,  const int size , const char *filename , const int linenum )
{
  return heapBase->AllocMemory( size , filename , linenum );
}
#else
void *GflHeapAllocMemoryBlock( HeapBase *heapBase ,  const int size )
{
  return heapBase->AllocMemory( size );
}
#endif

#if GFL_HEAP_DEBUG
//AllocMemoryBlock HEAPID��
void *GflHeapAllocMemoryBlock( HEAPID heapId ,  const int size , const char *filename , const int linenum )
{
  return Manager::GetHeapByHeapId(heapId)->AllocMemory( size , filename , linenum );
}
#else
void *GflHeapAllocMemoryBlock( HEAPID heapId ,  const int size )
{
  return Manager::GetHeapByHeapId(heapId)->AllocMemory( size );
}
#endif

#if GFL_HEAP_DEBUG
//AllocMemoryBlockAlign HeapBase��
void *GflHeapAllocMemoryBlockAlign( HeapBase *heapBase ,  const int size , const int align , const char *filename , const int linenum )
{
  return heapBase->AllocMemoryAlign( size , align , filename , linenum );
}
#else
void *GflHeapAllocMemoryBlockAlign( HeapBase *heapBase ,  const int size , const int align )
{
  return heapBase->AllocMemoryAlign( size , align );
}
#endif

#if GFL_HEAP_DEBUG
//AllocMemoryBlockAlign HEAPID��
void *GflHeapAllocMemoryBlockAlign( HEAPID heapId ,  const int size , const int align , const char *filename , const int linenum )
{
  return Manager::GetHeapByHeapId(heapId)->AllocMemoryAlign( size , align , filename , linenum );
}
#else
void *GflHeapAllocMemoryBlockAlign( HEAPID heapId ,  const int size , const int align )
{
  return Manager::GetHeapByHeapId(heapId)->AllocMemoryAlign( size , align );
}
#endif

#if GFL_HEAP_DEBUG
//AllocMemoryBlockAlign4096
void *GflHeapAllocMemoryBlockAlign4096( HeapBase *heapBase ,  const int size , const char *filename , const int linenum )
{
  return heapBase->AllocMemory4096( size , true , filename , linenum );
}
#else
void *GflHeapAllocMemoryBlockAlign4096( HeapBase *heapBase ,  const int size )
{
  return heapBase->AllocMemory4096( size , true );
}
#endif

#if GFL_HEAP_DEBUG
//AllocMemoryBlockAlign4096
void *GflHeapAllocMemoryBlockAlign4096Lower( HeapBase *heapBase ,  const int size , const char *filename , const int linenum )
{
  return heapBase->AllocMemory4096Lower( size , true , filename , linenum );
}
#else
void *GflHeapAllocMemoryBlockAlign4096Lower( HeapBase *heapBase ,  const int size )
{
  return heapBase->AllocMemory4096Lower( size , true );
}
#endif
//----------------------------------------------------------------------------
/**
 *  @brief  ���������J��
 *          �֐�������̂��ߗp�ӂ��Ă��܂�
 *
 *  @param  void  �J��������
 */
//-----------------------------------------------------------------------------
void GflHeapFreeMemoryBlock(void* buf)
{
  gfl::heap::GflHeapBase::FreeMemoryAuto(buf);
}

//#pragma mark [> HeapBase

GflHeapBase::GflHeapBase()
:m_lowerHandle(NULL)
,mAllocCntLock()
{
  mAllocCntLock.Initialize();
}

GflHeapBase::~GflHeapBase()
{
  mAllocCntLock.Finalize();
}


//GflHeapBase���x���̏�����
//friend��gfl::heap::Manager����̂݌Ăяo��
//�m���ɏ��������ɌĂ΂�܂�
void GflHeapBase::InitHeapBase( const HEAPID heapId )
{
  m_heapId = heapId;
  m_allocCount = 0;
  
#if GFL_HEAP_DEBUG
  mDebugFlg = false;
  mDebugUserID = 0;
#endif
#if GFL_HIO_ENABLE
  mHioDump      = false;
  mHioLineIndex = 0;
#endif
}


//GflHeapBase���x���̊J��
//friend��gfl::heap::Manager����̂݌Ăяo��
//�m���ɊJ�����ɌĂ΂�܂�(DeleteHeap�̑O�ł�)
void GflHeapBase::TermHeapBase(bool isCheck)
{
  if( GetAllocCount() != 0 )
  {
    if( isCheck )
    {
      Dump();
      GFL_ASSERT_MSG( GetAllocCount() == 0 , "GFL_HEAP Memory is remained![%d]\n",GetAllocCount() );  //@check
    }
    FreeAll();
  }
  
#if GFL_HEAP_DEBUG
  if( mDebugFlg )
  {
    GFL_PRINT("GFL_HEAP HeapId[%d] Minimal memory [0x%08x]\n",m_heapId,mMinLeast);
  }
#endif// GFL_HEAP_DEBUG
}

//friend��gfl::heap::Manager����̂݌Ăяo��LowerHandle�̍쐬
void GflHeapBase::SetLowerHandle( GflHeapBaseLower *handle )
{
  m_lowerHandle = handle;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v����Ɍ���m�ۂ���n���h�����擾���܂�
 */
//-----------------------------------------------------------------------------
HeapBase* GflHeapBase::GetLowerHandle()
{
  return m_lowerHandle;
}


#if GFL_HEAP_DEBUG
void GflHeapBase::SetDebugModeFlg( bool flg )
{
  mDebugFlg = flg;
  if( flg )
  {
    mMinLeast = GetTotalAllocatableSize();
  }
  else
  {
    mMinLeast = 0;
  }
}
#endif

//----------------------------------------------------------------------------
/**
 *  @brief  ���������m��
 *          �������̊m�ۂ͂��̃}�N�����g���čs���Ă��������B
 *          AllocMemoryBlock��AllocMemoryBlockAlign�𒼐ڎg�p���Ȃ��ł��������B
 *
 *  @param  const int size   �m�ۃT�C�Y
 *
 *  @return void* �m�ۃ�����(0�N���A����Ă��܂�)
 */
//-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
void* GflHeapBase::AllocMemory( const int size , const char *filename , const int linenum )
{
  return AllocMemoryCore( size , GetDefaultAlign() , true , filename , linenum );
}
#else
void* GflHeapBase::AllocMemory( const int size )
{
  return AllocMemoryCore( size , GetDefaultAlign() , true );
}
#endif

//----------------------------------------------------------------------------
/**
 *  @brief  ���������m��(�A���C�������g)
 *          ���ڎg�p���Ȃ��ŁAGflHeapAllocMemoryBlock��GflHeapAllocMemoryBlockAlign��
 *          �g�p���Ă��������B
 *
 *  @param  const int size   �m�ۃT�C�Y
 *  @param  const int align  �A���C�����g(�}�C�i�X�Ō������m��)
 *
 *  @return void* �m�ۃ�����(0�N���A����Ă��܂�)
 */
//-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
void* GflHeapBase::AllocMemoryAlign( const int size , const int align , const char *filename , const int linenum )
{
  return AllocMemoryCore( size , align , true , filename , linenum );
}
#else
void* GflHeapBase::AllocMemoryAlign( const int size , const int align )
{
  return AllocMemoryCore( size , align , true );
}
#endif

//�������m�ۃR�A����
#if GFL_HEAP_DEBUG
void* GflHeapBase::AllocMemoryCore(  const int size , const int align , const bool isClearMemory , const char *filename , const u16 linenum )
#else
void* GflHeapBase::AllocMemoryCore(  const int size , const int align , const bool isClearMemory )
#endif
{
  if( size == 0 )
  {
    GFL_PRINT("GFL_HEAP Alloc Memory size = zero!\n");
    //GFL_ASSERT(0); //@check
    //return NULL;
  }
  
//  GFL_PRINT("alloc : %s : %d \n",filename, linenum);
  
  if( Manager::IsExistHeapId(0) )
  {
    GFL_ASSERT( Manager::GetHeapByHeapId(0)->Check() ); //@check
  }
  
  void *buf;
  void *retBuf;
  //�A���C�����g����w�b�_�[���̃I�t�Z�b�g�����߂�
  int adjustSize = CalcAllocateAdjustSize( align );

  buf = AllocAlign(size+adjustSize,align);
  
  if( Manager::IsExistHeapId(0) )
  {
    GFL_ASSERT( Manager::GetHeapByHeapId(0)->Check() ); //@check
  }

  if( buf == NULL )
  {
    GFL_PRINT("GFL_HEAP Alloc failed!\n");
    GFL_PRINT("Request size     [0x%x] + [0x%x]\n",size,adjustSize);
    GFL_PRINT("Allocatable size [0x%x]\n",GetTotalAllocatableSize());
    GFL_PRINT("Total free size  [0x%x]\n",GetTotalFreeSize());
    Dump();
    GFL_PRINT("GFL_HEAP Alloc failed!\n");
    GFL_PRINT("Request size     [0x%x] + [0x%x]\n",size,adjustSize);
    GFL_PRINT("Allocatable size [0x%x]\n",GetTotalAllocatableSize());
    GFL_PRINT("Total free size  [0x%x]\n",GetTotalFreeSize());
#if GFL_HEAP_DEBUG
    GFL_ASSERT_MSG( buf,"GFL_HEAP Alloc failed!\n[%x:%s:%d] HeapId[%d]\n",size,filename,linenum,GetHeapId() ); //@check
    GFL_ASSERT_MSG( buf,"Request size     [0x%x] + [0x%x]\n",size,adjustSize);
    GFL_ASSERT_MSG( buf,"Allocatable size [0x%x]\n",GetTotalAllocatableSize());
    GFL_ASSERT_MSG( buf,"Total free size  [0x%x]\n",GetTotalFreeSize());
#else
    GFL_ASSERT_MSG( buf,"GFL_HEAP Alloc failed!\n",size ); //@check
#endif
#if !(GFL_DEBUG)
    GFL_ASSERT_STOP(0);  //�{�Ԃ�ASSERTSTOP=�G���[
#endif
#ifdef __KLOCWORK__
    GFL_ASSERT_STOP(0);  //KLOCWORK�̎���HALT
#endif
    return NULL;
  }

  //�������N���A
  if( isClearMemory == true )
  {
    std::MemClear( buf , size+adjustSize );
  }

  //�w�b�_�𖄂ߍ��ޏ���
  retBuf = reinterpret_cast<void*>(reinterpret_cast<uptr>(buf) + adjustSize);
  
#if GFL_HEAP_DEBUG
#if GFL_HIO_ENABLE
  SetMemoryHeader( GetMemoryHeader(retBuf) , adjustSize , filename , linenum, align );
  HioLineAdd( buf, GetMemorySize(retBuf) + GetMemoryHeader(retBuf)->offset, filename, linenum, align );
#else
  SetMemoryHeader( GetMemoryHeader(retBuf) , adjustSize , filename , linenum );
#endif
#else
  SetMemoryHeader( GetMemoryHeader(retBuf) , adjustSize );
#endif

  if( adjustSize > MEMORY_HEADER_SIZE )
  {
    //�w�b�_��擪�ɃR�s�[
    //����������Ȃ̂Ŏ��ۂ̐擪�A�h���X����������ۂɑΉ��\�ɂ��Ă���
    gfl::std::MemCopy( GetMemoryHeader(retBuf) , buf , MEMORY_HEADER_SIZE );
  }
  IncAllocCount();
#if GFL_HEAP_DEBUG
  if( mDebugFlg )
  {
    const int allocatableSize = GetTotalAllocatableSize();
    if( mMinLeast > allocatableSize )
    {
      GFL_PRINT_EX( HEAP_PRINT2 , "HeapId[%d] LeastSize update[0x%08x]->[0x%08x]\n",m_heapId,mMinLeast,allocatableSize);
      mMinLeast = allocatableSize;
    }
  }
#endif
  
#if GFL_HEAP_DEBUG
  GFL_PRINT_EX( HEAP_PRINT1 , "GFL_HEAP Alloc[0x%p(0x%p)][size0x%x(0x%x)][Id %d]\n",buf,(void*)((int)buf - adjustSize),size,size+adjustSize,GetHeapId());
  GFL_PRINT_EX( HEAP_PRINT1 , "              [%s][%d]\n",filename,linenum);
  GFL_PRINT_EX( HEAP_PRINT2 , "GFL_HEAP Alloc allocCnt[%d]\n",GetAllocCount());
#endif
  return retBuf;
}

//�������m�ۃR�A����
#if GFL_HEAP_DEBUG
void* GflHeapBase::AllocMemory4096(  const int size , const bool isClearMemory , const char *filename , const u16 linenum )
{
  return AllocMemory4096Core( size , isClearMemory , false , filename , linenum);
}
void* GflHeapBase::AllocMemory4096Lower(  const int size , const bool isClearMemory , const char *filename , const u16 linenum )
{
  return AllocMemory4096Core( size , isClearMemory , true , filename , linenum);
}
#else
void* GflHeapBase::AllocMemory4096(  const int size , const bool isClearMemory )
{
  return AllocMemory4096Core( size , isClearMemory , false );
}
void* GflHeapBase::AllocMemory4096Lower(  const int size , const bool isClearMemory )
{
  return AllocMemory4096Core( size , isClearMemory , true );
}
#endif

#if GFL_HEAP_DEBUG
void* GflHeapBase::AllocMemory4096Core(  const int size , const bool isClearMemory ,  bool isLower , const char *filename , const u16 linenum )
#else
void* GflHeapBase::AllocMemory4096Core(  const int size , const bool isClearMemory , bool isLower )
#endif
{

  if( size == 0 )
  {
    GFL_PRINT("GFL_HEAP Alloc Memory size = zero!\n");
    //GFL_ASSERT(0); //@check
    //return NULL;
  }
  
  void *buf;
  void *retBuf;
  //�A���C�����g����w�b�_�[���̃I�t�Z�b�g�����߂�
  int adjustSize = 4096+MEMORY_HEADER_SIZE;
  
  if( isLower )
  {
    buf = AllocAlign(size+adjustSize,-MEMORY_HEADER_SIZE);
  }
  else
  {
    buf = AllocAlign(size+adjustSize,MEMORY_HEADER_SIZE);
  }

  if( buf == NULL )
  {
    GFL_PRINT("GFL_HEAP Alloc failed!\n");
    GFL_PRINT("size[%x+%x] least[%x]\n",size,adjustSize,GetTotalAllocatableSize());
    Dump();
#if GFL_HEAP_DEBUG
    GFL_ASSERT_MSG( buf,"GFL_HEAP Alloc failed!\n[%x:%s:%d]\n",size,filename,linenum ); //@check
#else
    GFL_ASSERT_MSG( buf,"GFL_HEAP Alloc failed!\n",size ); //@check
#endif
#if !(GFL_DEBUG)
    GFL_ASSERT_STOP(0);  //�{�Ԃ�ASSERTSTOP=�G���[
#endif
#ifdef __KLOCWORK__
    GFL_ASSERT_STOP(0);  //KLOCWORK�̎���HALT
#endif
    return NULL;
  }

  //�������N���A
  if( isClearMemory == true )
  {
    std::MemClear( buf , size+adjustSize );
  }
  
  uptr topPos = reinterpret_cast<uptr>(buf);
  uptr bufPos = topPos+MEMORY_HEADER_SIZE;
  bufPos = gfl::math::RoundUp( bufPos , 4096 );
  GFL_ASSERT( bufPos-topPos >= MEMORY_HEADER_SIZE ); //@check ���_��Ȃ��͂�
  
  adjustSize = bufPos-topPos;
  
  //�w�b�_�𖄂ߍ��ޏ���
  retBuf = reinterpret_cast<void*>(reinterpret_cast<uptr>(buf) + adjustSize);
  
#if GFL_HEAP_DEBUG
#if GFL_HIO_ENABLE
  SetMemoryHeader( GetMemoryHeader(retBuf) , adjustSize , filename , linenum, 4096 );
  HioLineAdd( buf, GetMemorySize(retBuf) + GetMemoryHeader(retBuf)->offset, filename, linenum, 4096 );
#else
  SetMemoryHeader( GetMemoryHeader(retBuf) , adjustSize , filename , linenum );
#endif
#else
  SetMemoryHeader( GetMemoryHeader(retBuf) , adjustSize );
#endif

  if( adjustSize > MEMORY_HEADER_SIZE )
  {
    //�w�b�_��擪�ɃR�s�[
    //����������Ȃ̂Ŏ��ۂ̐擪�A�h���X����������ۂɑΉ��\�ɂ��Ă���
    gfl::std::MemCopy( GetMemoryHeader(retBuf) , buf , MEMORY_HEADER_SIZE );
  }
  IncAllocCount();
#if GFL_HEAP_DEBUG
  if( mDebugFlg )
  {
    const int allocatableSize = GetTotalAllocatableSize();
    if( mMinLeast > allocatableSize )
    {
      GFL_PRINT_EX( HEAP_PRINT2 , "HeapId[%d] LeastSize update[0x%08x]->[0x%08x]\n",m_heapId,mMinLeast,allocatableSize);
      mMinLeast = allocatableSize;
    }
  }
#endif
  
#if GFL_HEAP_DEBUG
  GFL_PRINT_EX( HEAP_PRINT1 , "GFL_HEAP Alloc[0x%p(0x%p)][size0x%x(0x%x)][Id %d]\n",buf,(void*)((int)buf - adjustSize),size,size+adjustSize,GetHeapId());
  GFL_PRINT_EX( HEAP_PRINT1 , "              [%s][%d]\n",filename,linenum);
  GFL_PRINT_EX( HEAP_PRINT2 , "GFL_HEAP Alloc allocCnt[%d]\n",GetAllocCount());
#endif
  return retBuf;  
}



//----------------------------------------------------------------------------
/**
 *  @brief  ���������J��
 *          GflHeapFreeMemoryBlock���g�p���Ă�������
 *          �m�ۂ��s����HeapBase���g���ĊJ�����Ă�������
 *
 *  @param  void  �J��������
 */
//-----------------------------------------------------------------------------
void GflHeapBase::FreeMemory( void* buf )
{
  MEMORY_HEADER *header = GetMemoryHeader(buf);
  
  //�e�q�[�v�Ⴂ
  if( header->heapId != GetHeapId() )
  {
    GFL_ASSERT_MSG( false ,"GFL_HEAP Free failed!\n Parent heap in not match[bufId %d selfId %d]\n",header->heapId,GetHeapId() ); //@check
    
    HeapBase *parentHeap = Manager::GetHeapByHeapId( header->heapId );
    parentHeap->Free( GetMemoryTop(buf) );
    
    parentHeap->DecAllocCount();
    GFL_PRINT_EX( HEAP_PRINT1 , "GFL_HEAP Free[0x%p(0x%p)]!!\n",buf,GetMemoryTop(buf));
    return;
  }

#if GFL_HIO_ENABLE
  HioLineDel( header->heapId, reinterpret_cast<void*>(header->userData) );
#endif

  DecAllocCount();

#if GFL_HEAP_DEBUG
  // ����������̃N���A
  if( mDebugFreeMemClear == DEBUG_FREE_MEMC_ZERO ) gfl::std::MemClear( buf, GetMemorySize(buf) );
  else if( mDebugFreeMemClear == DEBUG_FREE_MEMC_FF ) gfl::std::MemFill( buf, 0xFF, GetMemorySize(buf) );
#endif // GFL_HEAP_DEBUG


  Free(GetMemoryTop(buf));
  GFL_PRINT_EX( HEAP_PRINT1 , "GFL_HEAP Free[0x%p(0x%p)]\n",buf,GetMemoryTop(buf));
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����Őe�q�[�v�𔻒f�����������J��
 *          GflHeapFreeMemoryBlock���g�p���Ă�������
 *
 *  @param  void  �J��������
 */
//-----------------------------------------------------------------------------
void GflHeapBase::FreeMemoryAuto( void* buf )
{
  if( buf == NULL )
  {
    GFL_PRINT_EX( HEAP_PRINT1 , "GFL_HEAP buf = NULL!\n");
    //GFL_ASSERT(0); //@check
    return;
  }

  MEMORY_HEADER *header = GetMemoryHeader(buf);
  HeapBase *parentHeap = Manager::GetHeapByHeapId( header->heapId );
  //GFL_PRINT_EX( HEAP_PRINT2 , "GFL_HEAP FreeAuto[0x%p(0x%p)][Id %d]\n",buf,GetMemoryTop(buf),header->heapId);
  parentHeap->FreeMemory( buf );
}

//�w�b�_�[�Z�b�g
#if GFL_HEAP_DEBUG
#if GFL_HIO_ENABLE
void GflHeapBase::SetMemoryHeader( MEMORY_HEADER *buf , const u32 offset , const char *filename , const u16 linenum, const int align )
#else
void GflHeapBase::SetMemoryHeader( MEMORY_HEADER *buf , const u32 offset , const char *filename , const u16 linenum )
#endif
#else
void GflHeapBase::SetMemoryHeader( MEMORY_HEADER *buf , const u32 offset )
#endif
{
  MEMORY_HEADER *header = static_cast<MEMORY_HEADER*>(buf);
  
  //�w�b�_�[�ɐeHEAPID�ƃ������擪�܂ł̃I�t�Z�b�g�ݒ�
  header->heapId = GetHeapId();
  header->offset = offset;
  
#if GFL_HEAP_DEBUG
#if GFL_HIO_ENABLE
  SetMemoryHeader_UserData( reinterpret_cast<void*>(header->userData) , filename , linenum, align );
#else
  SetMemoryHeader_UserData( reinterpret_cast<void*>(header->userData) , filename , linenum );
#endif
#else
  SetMemoryHeader_UserData( reinterpret_cast<void*>(header->userData) );
#endif
}

//���[�U�[�w�b�_�[�Z�b�g(�f�t�H���g)
#if GFL_HEAP_DEBUG
#if GFL_HIO_ENABLE
void GflHeapBase::SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum, const int align )
#else
void GflHeapBase::SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum )
#endif
#else
void GflHeapBase::SetMemoryHeader_UserData( void *userHeader )
#endif
{
  MEMORY_USER_HEADER *header = static_cast<MEMORY_USER_HEADER*>(userHeader);
#if GFL_HEAP_DEBUG
  header->line = linenum;
  header->id   = mDebugUserID;
	SetFilename( header->fileName, filename );
#if GFL_HIO_ENABLE
  header->align      = align;
  header->line_index = mHioLineIndex;
  mHioLineIndex++;
#endif
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  �A���C�����g��n���w�b�_�[���݂Ŏ��ۂ̃������m�ۗʂ��v�Z
 *          32�o�C�g�ȏ�̃A���C�����g��ݒ肵���ꍇ�A���̃T�C�Y���w�b�_�[��
 *          �m�ۂ���܂��B
 *
 *  @return  int ���m�ۃT�C�Y
 */
//-----------------------------------------------------------------------------
int GflHeapBase::CalcAllocateAdjustSize( const int align ) const 
{
  int adjustSize = MEMORY_HEADER_SIZE;
  if( adjustSize < gfl::math::Abs(align) )
  {
    adjustSize = gfl::math::Abs(align);
  }
  else
  {
    //�A���C�����g��MEMSIZE�ƍ���Ȃ��ꍇ
    //�A�T�[�g�őΉ�(MEMORY_HEADER_SIZE��32�̌��薳���Ǝv��)
    GFL_ASSERT_MSG( (MEMORY_HEADER_SIZE % align ) == 0 , "GFL_HEAP Align size error! align[%d]" , align );
    
  }
  return adjustSize;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �n���ꂽ���������������ǂ������`�F�b�N���܂��B
 *
 *  @param  const void mem ���ׂ郁����
 *  @return bool �`�F�b�N���s��false
 */
//-----------------------------------------------------------------------------
bool GflHeapBase::CheckMemoryBlock( const void *mem )
{
  GFL_PRINT("�`�F�b�N�ł��܂���ł����B\n");
  return true;
}


//�w�b�_�[�ʒu�擾
MEMORY_HEADER* GflHeapBase::GetMemoryHeader( const void *buf )
{
  return reinterpret_cast<MEMORY_HEADER*>(reinterpret_cast<uptr>(buf)-MEMORY_HEADER_SIZE);
}

//���ۂ̃������擪�m��
void* GflHeapBase::GetMemoryTop( const void *buf )
{
  MEMORY_HEADER* memHead = GetMemoryHeader(buf);
  return reinterpret_cast<void*>(reinterpret_cast<uptr>(buf)-memHead->offset);
}

//�w�b�_�[�T�C�Y�擾(GetMemoryTop�̑O�̃A�h���X��n��
int GflHeapBase::GetMemoryHeaderSize( const void *buf )
{
  MEMORY_HEADER* memHead = GetMemoryHeader(buf);
  return memHead->offset;
}

void GflHeapBase::IncAllocCount(void)
{
  mAllocCntLock.Enter();
  m_allocCount++; 
  mAllocCntLock.Leave();
};

void GflHeapBase::DecAllocCount(void)
{ 
  mAllocCntLock.Enter();
  m_allocCount--; 
  mAllocCntLock.Leave();
};

#if GFL_HEAP_DEBUG
void GflHeapBase::SetFilename( char * buf, const char * filename )
{
  //���O���t���p�X�œ����Ă���̂ŁE�E�E
  //\�L���܂ł������񕪂��w�b�_�Ɋi�[
  int nameLen = strlen(filename)-3;
  buf[MEMORY_USER_HEADER_FILE_BUFLEN-1] = '\0';
  for( int i=MEMORY_USER_HEADER_FILE_BUFLEN-2  ;i>=0;i-- )
  {
    if( nameLen >= 0 && filename[nameLen] != '\\' )
    {
      buf[i] = filename[nameLen];
      nameLen--;
    }
    else
    {
      buf[i] = ' ';
    }
  }
}
void GflHeapBase::SetCreateData( const char * filename, const int linenum, bool parent )
{
	createData.parentFlag = parent;
	createData.sourceFile.line = linenum;
	SetFilename( createData.sourceFile.fileName, filename );

}
const char * GflHeapBase::GetCreateDataFilename(void)
{
	return createData.sourceFile.fileName;
}
int GflHeapBase::GetCreateDataLine(void)
{
	return createData.sourceFile.line;
}
bool GflHeapBase::GetCreateParentFlag(void)
{
	return createData.parentFlag;
}
void GflHeapBase::SetDebugUserID( u8 id )
{
  mDebugUserID = id;
}
#endif	// GFL_HEAP_DEBUG

#if GFL_HIO_ENABLE

static  void  hio_write( gfl::hio::HioSharedMemory* hsm, char* write_buf, size_t write_size,
                                                         char* to_top_buf, size_t to_top_size );

void  GflHeapBase::HioLineAdd( void* adrs, const int size, const char* filename, const int linenum, const int align )
{ 
  if( mHioDump != false )
  { 
    gfl::hio::HioSharedMemory* hsm = gfl::hio::HioSharedMemory::GetInstance();
    if( hsm )
    { 
      char  buf[ 256 ];
      char  head[ 32 ];
      char  body[ 256 -32 ];
      char  to_top_buf[ 4 ];
      int   write_size;
      MEMORY_HEADER *header = static_cast<MEMORY_HEADER*>(adrs);
      MEMORY_USER_HEADER *user_header = static_cast<MEMORY_USER_HEADER*>((void*)header->userData);
      sprintf( body, "%d %08x %d %d %d %s %d ", GetHeapId(), adrs, size, linenum, align, filename, user_header->line_index );
      write_size = strlen( body ) + 8;
      sprintf( head, "FCA %03d", write_size );
      sprintf( buf, "%s %s", head, body );
      sprintf( to_top_buf, "TOT " );
      hio_write( hsm, buf, write_size, to_top_buf, 4 );
    }
  }
}

void  GflHeapBase::HioLineDel( HEAPID heapId, void* userHeader )
{ 
  if( mHioDump != false )
  { 
    gfl::hio::HioSharedMemory* hsm = gfl::hio::HioSharedMemory::GetInstance();
    if( hsm )
    { 
      char  buf[ 256 ];
      char  head[ 32 ];
      char  body[ 256 -32 ];
      char  to_top_buf[ 4 ];
      int   size;
      MEMORY_USER_HEADER *header = static_cast<MEMORY_USER_HEADER*>(userHeader);
      sprintf( body, "%d %d ", heapId, header->line_index );
      size = strlen( body ) + 8;
      sprintf( head, "FCD %03d", size );
      sprintf( buf, "%s %s", head, body );
      sprintf( to_top_buf, "TOT " );
      hio_write( hsm, buf, size, to_top_buf, 4 );
    }
  }
}

static  void  hio_write( gfl::hio::HioSharedMemory* hsm, char* write_buf, size_t write_size,
                                                         char* to_top_buf, size_t to_top_size )
{ 
  nn::Result  result;
  bool        to_top_result = false;

  do
  { 
    result = hsm->WriteRingBuffer( write_buf, write_size, to_top_buf, 4, &to_top_result );
    if( result.IsFailure() )
    { 
      GFL_PRINT("hio write failed\n");
    }
  }while( result.IsFailure() );

  if( to_top_result != false )
  { 
    GFL_PRINT("ring buffer to top\n");
    char  read_buf[ 4 ];
    int   time_out = 0;
    do{ 
      hsm->Read( read_buf, 3, 0 );
      time_out++;
      if( time_out > 3600 * 10 )
      { 
        GFL_PRINT("ring buffer to top time_out\n");
        break;
      }
    }while( strncmp( read_buf, "XXX", 3 ) != 0 );
    GFL_PRINT("ring buffer to top ok\n");
  }
}
#endif

} //namespace heap
} //namespace gfl

