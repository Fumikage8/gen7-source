//======================================================================
/**
 * @file	gfl_HeapManager.cpp
 * @brief	�q�[�v�Ǘ��V�X�e��
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#include <new>
#include <gflib.h>
#include "gfl_HeapLocal.h"
#include <heap/gfl_HeapBaseLower.h>

namespace gfl
{
namespace heap
{

//======================================================================
//  define
//======================================================================
//#pragma mark [> define

//======================================================================
//  enum
//======================================================================
//#pragma mark [> enum
typedef enum
{
  HMB_IS_USE = 1<<0,
  HMB_CREATE_BUFFER = 1<<1,
  
  HMB_MAX
}HEAP_MNG_BIT;

struct _HEAP_MNG_DATA
{
  HeapBase *heap;
  u32      flg;
};
static const u32 HEAP_MNG_DEFAULT_FLG = 0;
//======================================================================
//  Class Manager
//======================================================================
//#pragma mark [> Manager
HEAP_MNG_DATA *Manager::m_heapMngData;
int Manager::m_heapNum;

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�Ǘ�������
 *
 *  @param  void *buf �����o�b�t�@
                      �Ǘ��̈���������HEAPID_SYSTEM�Ƃ��Ċm�ۂ���܂�
 *  @param  const int size �����o�b�t�@�̃T�C�Y
 *  @param  const int HeappNum ��`���Ă���HEAPID�̍ő吔�ł�
 */
//-----------------------------------------------------------------------------
void Manager::Initialize( void *buf , const int size , const int HeapNum )
{
  
  m_heapMngData = (HEAP_MNG_DATA*)buf;
  void *heapBuf = reinterpret_cast<void*>(reinterpret_cast<uptr>(buf) + sizeof(HEAP_MNG_DATA)*HeapNum);
  m_heapNum = HeapNum;
  for( int i=0;i<HeapNum;i++ )
  {
    m_heapMngData[i].heap = NULL;
    m_heapMngData[i].flg = HEAP_MNG_DEFAULT_FLG;
  }
  
  CreateHeapBuffer( heapBuf , HEAPID_SYSTEM , size - (sizeof(HEAP_MNG_DATA)*HeapNum) , HEAP_TYPE_EXP );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�Ǘ��J��
 *
 *  @param  none
 */
//-----------------------------------------------------------------------------
void Manager::Finalize( bool isCheck )
{
  //�c��q�[�v�̃`�F�b�N
  for( int i=0;i<m_heapNum;i++ )
  {
    if( m_heapMngData[i].heap != NULL ||
        m_heapMngData[i].flg != HEAP_MNG_DEFAULT_FLG )
    {
      if( isCheck )
      {
        GFL_ASSERT_MSG( false , "GFL_HEAP Manager exit  HeapId[%d] is left!\n",i );//@check
      }

      DeleteHeapCore( m_heapMngData[i].heap , isCheck );
      
      m_heapMngData[i].heap = NULL;
      m_heapMngData[i].flg = HEAP_MNG_DEFAULT_FLG;
    }
  }
  m_heapNum = 0;
}

void Manager::Reset( const HEAPID *clearIdArr , const int clearIdNum ,  const HEAPID *keepIdArr , const int keepIdNum )
{
  for( int id=0;id<m_heapNum;id++ )
  {
    if( m_heapMngData[id].heap != NULL ||
        m_heapMngData[id].flg != HEAP_MNG_DEFAULT_FLG )
    {
      //�ێ�����Heap�`�F�b�N
      bool isKeep = false;
      for( int sub=0;sub<keepIdNum;sub++ )
      {
        if( id == keepIdArr[sub] )
        {
          isKeep = true;
        }
      }
      if( isKeep )
      {
        break;
      }
      
      bool isClear = false;
      for( int sub=0;sub<clearIdNum;sub++ )
      {
        if( id == clearIdArr[sub] )
        {
          isClear = true;
        }
      }
      if( isClear )
      {
        m_heapMngData[id].heap->FreeAll();
      }
      else
      {
        //�e�����������������Ă��鋰�ꂪ����̂ŁABUFFER�m�ۂɂ���Delete����Free���Ȃ�
        
        m_heapMngData[id].flg |= HMB_CREATE_BUFFER;
        DeleteHeapCore( m_heapMngData[id].heap , false );
        
        m_heapMngData[id].heap = NULL;
        m_heapMngData[id].flg = HEAP_MNG_DEFAULT_FLG;
      }
      
    }
  }
  
}

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�쐬���̎�����T�C�Y�v�Z �m�ۂ���T�C�Y+�Ǘ��̈�
 *          Create�̎��͊Ǘ��̈�͎����Œǉ�����܂�
 *
 *  @param  const int size �m�ۂ������������̃T�C�Y
 *  @param  const HeapType heapType �m�ۂ������q�[�v�̎��
 */
//-----------------------------------------------------------------------------
int Manager::CalcCreateHeapSize( const int size , const HeapType heapType )
{
  //�v���b�g�t�H�[���ʂ̕���
  //3DS�̏ꍇ
  switch(heapType)
  {
  case HEAP_TYPE_EXP:
  default:
    return sizeof(CTRExpHeap) + size + sizeof(GflHeapBaseLower);
    //break;
  case HEAP_TYPE_UNIT:
    return sizeof(CTRUnitHeap) + size + sizeof(GflHeapBaseLower);
    //break;
  }
  //�v���b�g�t�H�[���ʕ��򂱂��܂�
  
}

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�쐬 �eHeapBase�w���
 *          �w��T�C�Y�{�Ǘ��̈敪���e�q�[�v����m�ۂ���܂�
 *
 *  @param  HeapBase *parentHeap �e�ƂȂ�HeapBase�̃|�C���^
 *  @param  const HEAPID childId �V�������q�[�v��HEAPID
 *  @param  const int size �m�ۂ��郁�����̃T�C�Y
 *  @param  const HeapType heapType �m�ۂ���q�[�v�̎��
 *  @param  const bool isBack ����m��
 */
//-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
HeapBase* Manager::CreateHeap( HeapBase *parentHeap , const HEAPID childId , const int size , const HeapType heapType , const bool isBack, const char *filename, const int linenum )
#else
HeapBase* Manager::CreateHeap( HeapBase *parentHeap , const HEAPID childId , const int size , const HeapType heapType , const bool isBack )
#endif
{
	int align = parentHeap->GetDefaultAlign();
	if( isBack )
	{
		align *= -1;
	}
#if GFL_HEAP_DEBUG
  void *buf = parentHeap->AllocMemoryCore( size , align , false , filename , linenum );
//  void *buf = gfl::heap::GflHeapAllocMemoryBlockAlign( parentHeap , size , align , filename, linenum );
#else
  void *buf = parentHeap->AllocMemoryCore( size , align , false );
//  void *buf = GflHeapAllocMemoryAlign( parentHeap , size , align );
#endif

#if GFL_HEAP_DEBUG
	HeapBase * base = CreateHeapCore( buf , childId , size , heapType , false , NULL );
	base->SetCreateData( filename, linenum, true );
	return base;
#else
	return CreateHeapCore( buf , childId , size , heapType , false , NULL );
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�쐬 �o�b�t�@����쐬��
 *          �w��T�C�Y����Ǘ��̈敪��������A�c��T�C�Y�����ۂ̃q�[�v�T�C�Y�ɂȂ�܂�
 *
 *  @param  void *buffer �q�[�v���쐬����o�b�t�@
 *  @param  const HEAPID childId �V�������q�[�v��HEAPID
 *  @param  const int size �m�ۂ��郁�����̃T�C�Y
 *  @param  const HeapType heapType �m�ۂ���q�[�v�̎��
 */
//-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
HeapBase* Manager::CreateHeapBuffer( void *buffer , const HEAPID childId , const int size , const HeapType heapType, const char *filename, const int linenum )
{
	HeapBase * base = CreateHeapCore( buffer , childId , size , heapType , true , NULL );
	base->SetCreateData( filename, linenum, false );
	return base;
}
#else
HeapBase* Manager::CreateHeapBuffer( void *buffer , const HEAPID childId , const int size , const HeapType heapType )
{
  return CreateHeapCore( buffer , childId , size , heapType , true , NULL );
}
#endif

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�쐬 �o�b�t�@����쐬��
 *          �w��T�C�Y����Ǘ��̈敪��������A�c��T�C�Y�����ۂ̃q�[�v�T�C�Y�ɂȂ�܂�
 *          ����ȃq�[�v�����ۂɃp�����[�^��n����
 *
 *  @param  void *buffer �q�[�v���쐬����o�b�t�@
 *  @param  const HEAPID childId �V�������q�[�v��HEAPID
 *  @param  const int size �m�ۂ��郁�����̃T�C�Y
 *  @param  const HeapType heapType �m�ۂ���q�[�v�̎��
 *  @param  void *param �q�[�v�p�p�����[�^
 */
//-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
HeapBase* Manager::CreateHeapBufferEx( void *buffer , const HEAPID childId , const int size , const HeapType heapType , void *param, const char *filename, const int linenum )
{
	HeapBase * base = CreateHeapCore( buffer , childId , size , heapType , true , param );
	base->SetCreateData( filename, linenum, false );
	return base;
}
#else
HeapBase* Manager::CreateHeapBufferEx( void *buffer , const HEAPID childId , const int size , const HeapType heapType , void *param )
{
  return CreateHeapCore( buffer , childId , size , heapType , true , param );
}
#endif

//�q�[�v�쐬�R�A����
HeapBase* Manager::CreateHeapCore( void *buffer , const HEAPID childId , const int size , const HeapType heapType , const bool isBuffer  , void *param )
{
  int heapClassSize;

  //HEAPID�ő�l�`�F�b�N
  GFL_ASSERT_MSG( childId < m_heapNum , "GFL_HEAP New HeapId is over!! HeapNum[%d] newId[%d]\n",m_heapNum,childId );

  //�d���`�F�b�N
  if( IsExistHeapId( childId ) == true )
  {
    GFL_ASSERT_MSG( false , "GFL_HEAP Create heap is failue! HeapId[%d] is exist!\n",childId );
    return GetHeapByHeapId(childId);
  }
  
  //�v���b�g�t�H�[���ʂ̕���
  //3DS�̏ꍇ
  switch(heapType)
  {
  case HEAP_TYPE_EXP:
  default:
    m_heapMngData[childId].heap = new(buffer) CTRExpHeap;
    heapClassSize = sizeof(CTRExpHeap);
    break;
  
  case HEAP_TYPE_UNIT:
    m_heapMngData[childId].heap = new(buffer) CTRUnitHeap;
    heapClassSize = sizeof(CTRUnitHeap);
    break;
  
  }
  //�v���b�g�t�H�[���ʕ��򂱂��܂�
  //nnInitStartUp�œ�������GFL_NEW�g���Ȃ��I
  void *lowerBuf = reinterpret_cast<void*>(reinterpret_cast<uptr>(buffer) + heapClassSize);
  GflHeapBaseLower *lowerHandle = new(lowerBuf)GflHeapBaseLower(m_heapMngData[childId].heap);
  lowerHandle->InitHeapBase(childId);
  
  m_heapMngData[childId].heap->InitHeapBase(childId);
  void *heapBuf = reinterpret_cast<void*>(reinterpret_cast<uptr>(buffer) + heapClassSize + sizeof(GflHeapBaseLower));
  m_heapMngData[childId].heap->CreateHeap( heapBuf , size - (heapClassSize+sizeof(GflHeapBaseLower)) , param );
  m_heapMngData[childId].heap->SetLowerHandle(lowerHandle);
  
  m_heapMngData[childId].flg |= HMB_IS_USE;
  if( isBuffer == true )
  {
    m_heapMngData[childId].flg |= HMB_CREATE_BUFFER;
  }

  GFL_PRINT_EX(HEAP_PRINT1 , "GFL_HEAP Create new heap[%d] size[%x]\n",childId,size);
  GFL_PRINT_EX(HEAP_PRINT2 , "ClassSize[%x] RealSize[%x]\n",heapClassSize,size-(heapClassSize+ sizeof(GflHeapBaseLower)));
  GFL_PRINT_EX(HEAP_PRINT2 , "GFL_HEAP HeapClass[%x] Buffer[%x]\n",&m_heapMngData[childId].heap,reinterpret_cast<void*>(reinterpret_cast<uptr>(buffer) + heapClassSize + sizeof(GflHeapBaseLower)));
  
  return m_heapMngData[childId].heap;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�폜
 *
 *  @param  HeapBase *heap �폜����HeapBase
 */
//-----------------------------------------------------------------------------
void Manager::DeleteHeap( HeapBase *heap )
{
  //(Finalize��Core�𒼌Ăт��Ă��܂�)
  DeleteHeapCore(heap,true);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�폜(�`�F�b�N�����Ȃ�)
 *          �q�q�[�v�Ɖ���c�������ׂĉ�����܂��B
 *          ������CreateHeapBuffer�ō��ꂽHeap�͉���ł��܂���B
 *
 *  @param  HeapBase *heap �폜����HeapBase
 */
//-----------------------------------------------------------------------------
void Manager::DeleteHeapNoCheck( HeapBase *heap )
{
  //(Finalize��Core�𒼌Ăт��Ă��܂�)
  DeleteHeapCore(heap,false);
}

void Manager::DeleteHeapCore( HeapBase *heap , bool isCheck )
{
  const HEAPID heapId = heap->GetHeapId();
  if( IsExistHeapId( heapId ) == false )
  {
    if( isCheck )
    {
      GFL_ASSERT_MSG( false , "GFL_HEAP Delete heap is delete yet! HeapMax[%d] is not exist!\n",heapId );//@check
    }
    return;
  }
  
  for( int i=0; i<m_heapNum; i++ )
  {
    if( IsExistHeapId(i) && i != heapId )
    {
      //�o�b�t�@�n���̎��͐e���킩��Ȃ��I�I
      if( !(m_heapMngData[i].flg & HMB_CREATE_BUFFER ))
      {
        const HEAPID parentId = HeapBase::GetMemoryHeader(m_heapMngData[i].heap)->heapId;
        if( heapId == parentId )
        {
          if( isCheck )
          {
            GFL_ASSERT_MSG( false , "GFL_HEAP HeapId[%d] is have child heap!!childId[%d]\n",heapId,i );//@check
          }
          DeleteHeapCore( GetHeapByHeapId(i),isCheck );
        }
      }
    }
  }
  
  
  //delete�͂ł��Ȃ��̂Ŗ����I�Ƀf�X�g���N�^
  heap->GetLowerHandle()->~HeapBase();

  m_heapMngData[heapId].heap->TermHeapBase(isCheck);
  m_heapMngData[heapId].heap->DeleteHeap();
  //delete�͂ł��Ȃ��̂Ŗ����I�Ƀf�X�g���N�^
  heap->~HeapBase();
  if( !(m_heapMngData[heapId].flg & HMB_CREATE_BUFFER ))
  {
    GflHeapFreeMemory( heap );
  }
  else
  {
  }
  
  m_heapMngData[heapId].flg = HEAP_MNG_DEFAULT_FLG;
  m_heapMngData[heapId].heap = NULL;
  GFL_PRINT_EX(HEAP_PRINT1 , "GFL_HEAP Delete heap[%d]\n",heapId);
  
}


//----------------------------------------------------------------------------
/**
 *  @brief  HEAPID����HeapBase�N���X���擾
 *
 *  @param  const HEAPID heapId �擾�������q�[�v��HEAPID
 *  @return HeapBase* �Ή�����HeapBase�N���X 
 */
//-----------------------------------------------------------------------------
HeapBase* Manager::GetHeapByHeapId( const HEAPID heapId )
{
  //���݃`�F�b�N
  if( IsExistHeapId( heapId ) == false )
  {
    GFL_ASSERT_STOP_MSG( false , "GFL_HEAP HeapId[%d] is not exist!\n",heapId );//@check
    return NULL;
  }
  
  return m_heapMngData[heapId].heap;
}

//======================================================================
//  utility�֐�
//======================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  HEAPID�ɑΉ�����HeapBase�����邩�`�F�b�N
 *
 *  @param  const HEAPID heapId ���ׂ����q�[�v��HEAPID
 *  @return bool ture:���݂��� false:���݂��Ȃ�
 */
//-----------------------------------------------------------------------------
bool Manager::IsExistHeapId( const HEAPID heapId )
{
  if( heapId >= 0 && heapId < m_heapNum )
  {
    if( m_heapMngData[heapId].flg & HMB_IS_USE )
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �m�ۂ��ꂽ�̈�̃T�C�Y���擾(HeapBase�ɂ�����܂�)
 *
 *  @param  const void mem ���ׂ郁����
 *  @return int �������T�C�Y(-1�͎��s)
 */
//-----------------------------------------------------------------------------
int Manager::GetMemorySize( const void *mem )
{
  const MEMORY_HEADER *header = HeapBase::GetMemoryHeader(mem);
  if( IsExistHeapId( header->heapId ) )
  {
    return m_heapMngData[header->heapId].heap->GetMemorySize(mem);
  }
  return -1;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �n���ꂽ���������������ǂ������`�F�b�N���܂��B
 *
 *  @param  const void mem ���ׂ郁����
 *  @return bool �`�F�b�N���s��false
 */
//-----------------------------------------------------------------------------
bool Manager::CheckMemoryBlock( const void *mem )
{
  const MEMORY_HEADER *header = HeapBase::GetMemoryHeader(mem);
  if( IsExistHeapId( header->heapId ) )
  {
    return m_heapMngData[header->heapId].heap->GetMemorySize(mem);
  }
  GFL_PRINT("���݂��Ȃ�HEAPID�̃������ł��B[HEAPID:%d]\n",header->heapId);
  return false;
}

//======================================================================
//  �f�o�b�O�@�\
//======================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�̎擾�󋵂̏o��
 */
//-----------------------------------------------------------------------------
void Manager::Dump(void)
{
  GFL_PRINT( "-----GflHeapManager DumpStart-----\n");
  for( int i=0;i<m_heapNum;i++ )
  {
    if( m_heapMngData[i].flg & HMB_IS_USE )
    {
      GFL_PRINT( "HEAPID[%2d] ",i);
#if GFL_HEAP_DEBUG
			if( m_heapMngData[i].heap->GetCreateParentFlag() == true ){
				GFL_PRINT( "PARENT[%2d] ",HeapBase::GetMemoryHeader(m_heapMngData[i].heap)->heapId);
			}else{
				GFL_PRINT( "PARENT[--] " );
			}
      GFL_PRINT( "Addr[0x%08x] ",m_heapMngData[i].heap);
      GFL_PRINT( "Size[0x%08x] ",m_heapMngData[i].heap->GetTotalSize());
      GFL_PRINT( "Free[0x%08x] ",m_heapMngData[i].heap->GetTotalFreeSize());
      GFL_PRINT( "Allocate[0x%08x] ",m_heapMngData[i].heap->GetTotalAllocatableSize());
      GFL_PRINT( "Create[%s(%d)]\n",m_heapMngData[i].heap->GetCreateDataFilename(),m_heapMngData[i].heap->GetCreateDataLine());
#else
      GFL_PRINT( "Addr[0x%08x] ",m_heapMngData[i].heap);
      GFL_PRINT( "Size[0x%08x]\n",m_heapMngData[i].heap->GetTotalSize());
#endif
    }
  }
  GFL_PRINT( "-----GflHeapManager DumpFinish----\n");
}

void Manager::DumpTree(HeapBase *heap)
{
#if GFL_HEAP_DEBUG
  typedef struct {
    u16 id;   // ID
    u16 tab;  // �^�u��
  }TREE_WORK;

  TREE_WORK *tree = GFL_NEW_ARRAY(heap)TREE_WORK[m_heapNum];
  int cnt=0;
  int i, j, k;

  // ���ꂽ�q�[�v�𒊏o
  for( i=0; i<m_heapNum; i++ ){
    if( m_heapMngData[i].flg & HMB_IS_USE ){
      tree[cnt].id  = i;
      tree[cnt].tab = 0;
      cnt++;
    }
  }
  // �e�q���ɕ��ׂ�
  for( i=0; i<cnt; i++ ){
    if( m_heapMngData[tree[i].id].heap->GetCreateParentFlag() == true ){
      int p1 = HeapBase::GetMemoryHeader(m_heapMngData[tree[i].id].heap)->heapId;
      for( j=0; j<cnt; j++ ){
        if( p1 == tree[j].id ){
          TREE_WORK tmp = tree[i];
          if( i > j ){
            for( k=i; k>j+1; k-- ){
              tree[k] = tree[k-1];
            }
          }else{
            for( k=i; k<j; k++ ){
              tree[k] = tree[k+1];
            }
          }
          tree[k] = tmp;
          if( k != 0 ){
            tree[k].tab = tree[k-1].tab+1;
          }
          break;
        }
      }
    }
  }

  GFL_PRINT( "----- GflHeapManager DumpTree Start -----\n" );
  for( i=0; i<cnt; i++ ){
    // "�� ", "�� ", "�� ", "   " �̕\��
    if( tree[i].tab != 0 ){
      // �Ō� or ���̃f�[�^���e
      if( i == (cnt-1) || tree[i+1].tab == 0 ){
        for( j=0; j<tree[i].tab-1; j++ ){
          GFL_PRINT( "�@ " );
        }
        GFL_PRINT( "�� " );
      // ���ɓ����e�̃f�[�^������
      }else{
        for( j=0; j<tree[i].tab-1; j++ ){
          for( k=i+1; k<cnt; k++ ){
            if( tree[k].tab == 0 ){
              break;
            }else if( (tree[k].tab-1) == j ){
              GFL_PRINT( "�� " );
              break;
            }else if( (tree[k].tab-1) < j ){
              GFL_PRINT( "�@ " );
              break;
            }
          }
        }
        for( j=i+1; j<cnt; j++ ){
          if( tree[i].tab > tree[j].tab ){
            GFL_PRINT( "�� " );
            break;
          }else if( tree[i].tab == tree[j].tab ){
            GFL_PRINT( "�� " );
            break;
          }
        }
      }
    }
    // �q�[�vID
    GFL_PRINT( "HEAPID[%03d] ", tree[i].id );
/*
    // �e
    if( m_heapMngData[tree[i].id].heap->GetCreateParentFlag() == true ){
      GFL_PRINT( "PARENT[%03d] ", HeapBase::GetMemoryHeader(m_heapMngData[tree[i].id].heap)->heapId );
    }else{
      GFL_PRINT( "PARENT[---] " );
    }
*/
//    GFL_PRINT( "\n" );
    // �A�h���X
    GFL_PRINT( "Addr[0x%08x] ", m_heapMngData[tree[i].id].heap);
    // �T�C�Y
    GFL_PRINT( "Size[0x%08x] ", m_heapMngData[tree[i].id].heap->GetTotalSize() );
    // �ꏊ
    GFL_PRINT(
      "Create[%s(%d)]\n",
      m_heapMngData[tree[i].id].heap->GetCreateDataFilename(),
      m_heapMngData[tree[i].id].heap->GetCreateDataLine() );
  }
  GFL_PRINT( "----- GflHeapManager DumpTree Finish -----\n" );
  
  GFL_DELETE_ARRAY tree;
#endif
}


} //namespace heap
} //namespace gfl
