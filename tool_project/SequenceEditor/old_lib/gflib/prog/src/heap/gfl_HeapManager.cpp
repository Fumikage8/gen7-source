//======================================================================
/**
 * @file	gfl_HeapManager.cpp
 * @brief	ヒープ管理システム
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
 *  @brief  ヒープ管理初期化
 *
 *  @param  void *buf 初期バッファ
                      管理領域を取った後HEAPID_SYSTEMとして確保されます
 *  @param  const int size 初期バッファのサイズ
 *  @param  const int HeappNum 定義してあるHEAPIDの最大数です
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
 *  @brief  ヒープ管理開放
 *
 *  @param  none
 */
//-----------------------------------------------------------------------------
void Manager::Finalize( bool isCheck )
{
  //残りヒープのチェック
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
      //保持するHeapチェック
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
        //親がメモリを消去している恐れがあるので、BUFFER確保にしてDelete内でFreeしない
        
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
 *  @brief  ヒープ作成時の実消費サイズ計算 確保するサイズ+管理領域
 *          Createの時は管理領域は自動で追加されます
 *
 *  @param  const int size 確保したいメモリのサイズ
 *  @param  const HeapType heapType 確保したいヒープの種類
 */
//-----------------------------------------------------------------------------
int Manager::CalcCreateHeapSize( const int size , const HeapType heapType )
{
  //プラットフォーム別の分岐
  //3DSの場合
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
  //プラットフォーム別分岐ここまで
  
}

//----------------------------------------------------------------------------
/**
 *  @brief  ヒープ作成 親HeapBase指定版
 *          指定サイズ＋管理領域分が親ヒープから確保されます
 *
 *  @param  HeapBase *parentHeap 親となるHeapBaseのポインタ
 *  @param  const HEAPID childId 新しく作るヒープのHEAPID
 *  @param  const int size 確保するメモリのサイズ
 *  @param  const HeapType heapType 確保するヒープの種類
 *  @param  const bool isBack 後方確保
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
 *  @brief  ヒープ作成 バッファから作成版
 *          指定サイズから管理領域分が引かれ、残りサイズが実際のヒープサイズになります
 *
 *  @param  void *buffer ヒープを作成するバッファ
 *  @param  const HEAPID childId 新しく作るヒープのHEAPID
 *  @param  const int size 確保するメモリのサイズ
 *  @param  const HeapType heapType 確保するヒープの種類
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
 *  @brief  ヒープ作成 バッファから作成版
 *          指定サイズから管理領域分が引かれ、残りサイズが実際のヒープサイズになります
 *          特殊なヒープを作る際にパラメータを渡せる
 *
 *  @param  void *buffer ヒープを作成するバッファ
 *  @param  const HEAPID childId 新しく作るヒープのHEAPID
 *  @param  const int size 確保するメモリのサイズ
 *  @param  const HeapType heapType 確保するヒープの種類
 *  @param  void *param ヒープ用パラメータ
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

//ヒープ作成コア部分
HeapBase* Manager::CreateHeapCore( void *buffer , const HEAPID childId , const int size , const HeapType heapType , const bool isBuffer  , void *param )
{
  int heapClassSize;

  //HEAPID最大値チェック
  GFL_ASSERT_MSG( childId < m_heapNum , "GFL_HEAP New HeapId is over!! HeapNum[%d] newId[%d]\n",m_heapNum,childId );

  //重複チェック
  if( IsExistHeapId( childId ) == true )
  {
    GFL_ASSERT_MSG( false , "GFL_HEAP Create heap is failue! HeapId[%d] is exist!\n",childId );
    return GetHeapByHeapId(childId);
  }
  
  //プラットフォーム別の分岐
  //3DSの場合
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
  //プラットフォーム別分岐ここまで
  //nnInitStartUpで動くからGFL_NEW使えない！
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
 *  @brief  ヒープ削除
 *
 *  @param  HeapBase *heap 削除するHeapBase
 */
//-----------------------------------------------------------------------------
void Manager::DeleteHeap( HeapBase *heap )
{
  //(FinalizeはCoreを直呼びしています)
  DeleteHeapCore(heap,true);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ヒープ削除(チェック処理なし)
 *          子ヒープと解放残しもすべて解放します。
 *          ただしCreateHeapBufferで作られたHeapは解放できません。
 *
 *  @param  HeapBase *heap 削除するHeapBase
 */
//-----------------------------------------------------------------------------
void Manager::DeleteHeapNoCheck( HeapBase *heap )
{
  //(FinalizeはCoreを直呼びしています)
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
      //バッファ渡しの時は親がわからない！！
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
  
  
  //deleteはできないので明示的にデストラクタ
  heap->GetLowerHandle()->~HeapBase();

  m_heapMngData[heapId].heap->TermHeapBase(isCheck);
  m_heapMngData[heapId].heap->DeleteHeap();
  //deleteはできないので明示的にデストラクタ
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
 *  @brief  HEAPIDからHeapBaseクラスを取得
 *
 *  @param  const HEAPID heapId 取得したいヒープのHEAPID
 *  @return HeapBase* 対応するHeapBaseクラス 
 */
//-----------------------------------------------------------------------------
HeapBase* Manager::GetHeapByHeapId( const HEAPID heapId )
{
  //存在チェック
  if( IsExistHeapId( heapId ) == false )
  {
    GFL_ASSERT_STOP_MSG( false , "GFL_HEAP HeapId[%d] is not exist!\n",heapId );//@check
    return NULL;
  }
  
  return m_heapMngData[heapId].heap;
}

//======================================================================
//  utility関数
//======================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  HEAPIDに対応するHeapBaseがあるかチェック
 *
 *  @param  const HEAPID heapId 調べたいヒープのHEAPID
 *  @return bool ture:存在する false:存在しない
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
 *  @brief  確保された領域のサイズを取得(HeapBaseにもあります)
 *
 *  @param  const void mem 調べるメモリ
 *  @return int メモリサイズ(-1は失敗)
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
 *  @brief  渡されたメモリが正当かどうかをチェックします。
 *
 *  @param  const void mem 調べるメモリ
 *  @return bool チェック失敗でfalse
 */
//-----------------------------------------------------------------------------
bool Manager::CheckMemoryBlock( const void *mem )
{
  const MEMORY_HEADER *header = HeapBase::GetMemoryHeader(mem);
  if( IsExistHeapId( header->heapId ) )
  {
    return m_heapMngData[header->heapId].heap->GetMemorySize(mem);
  }
  GFL_PRINT("存在しないHEAPIDのメモリです。[HEAPID:%d]\n",header->heapId);
  return false;
}

//======================================================================
//  デバッグ機能
//======================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  ヒープの取得状況の出力
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
    u16 tab;  // タブ数
  }TREE_WORK;

  TREE_WORK *tree = GFL_NEW_ARRAY(heap)TREE_WORK[m_heapNum];
  int cnt=0;
  int i, j, k;

  // 作られたヒープを抽出
  for( i=0; i<m_heapNum; i++ ){
    if( m_heapMngData[i].flg & HMB_IS_USE ){
      tree[cnt].id  = i;
      tree[cnt].tab = 0;
      cnt++;
    }
  }
  // 親子順に並べる
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
    // "├ ", "│ ", "└ ", "   " の表示
    if( tree[i].tab != 0 ){
      // 最後 or 次のデータが親
      if( i == (cnt-1) || tree[i+1].tab == 0 ){
        for( j=0; j<tree[i].tab-1; j++ ){
          GFL_PRINT( "　 " );
        }
        GFL_PRINT( "└ " );
      // 下に同じ親のデータがある
      }else{
        for( j=0; j<tree[i].tab-1; j++ ){
          for( k=i+1; k<cnt; k++ ){
            if( tree[k].tab == 0 ){
              break;
            }else if( (tree[k].tab-1) == j ){
              GFL_PRINT( "│ " );
              break;
            }else if( (tree[k].tab-1) < j ){
              GFL_PRINT( "　 " );
              break;
            }
          }
        }
        for( j=i+1; j<cnt; j++ ){
          if( tree[i].tab > tree[j].tab ){
            GFL_PRINT( "└ " );
            break;
          }else if( tree[i].tab == tree[j].tab ){
            GFL_PRINT( "├ " );
            break;
          }
        }
      }
    }
    // ヒープID
    GFL_PRINT( "HEAPID[%03d] ", tree[i].id );
/*
    // 親
    if( m_heapMngData[tree[i].id].heap->GetCreateParentFlag() == true ){
      GFL_PRINT( "PARENT[%03d] ", HeapBase::GetMemoryHeader(m_heapMngData[tree[i].id].heap)->heapId );
    }else{
      GFL_PRINT( "PARENT[---] " );
    }
*/
//    GFL_PRINT( "\n" );
    // アドレス
    GFL_PRINT( "Addr[0x%08x] ", m_heapMngData[tree[i].id].heap);
    // サイズ
    GFL_PRINT( "Size[0x%08x] ", m_heapMngData[tree[i].id].heap->GetTotalSize() );
    // 場所
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
