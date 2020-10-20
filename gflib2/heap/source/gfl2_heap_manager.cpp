//======================================================================
/**
 * @file	gfl2_HeapManager.cpp
 * @brief	ヒープ管理システム
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "gfl2_heap_local.h"

#include <heap/include/gfl2_heap_base_lower.h>
#include <heap/include/gfl2_WinHeap.h>
#include <heap/include/gfl2_CtrExpHeap.h>
#include <heap/include/gfl2_HeapMacros.h>

namespace gfl2
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
HEAP_MNG_DATA *Manager::m_localHeapMngData;
int Manager::m_localHeapNum;
Manager::DeleteFunc Manager::m_unmanagedDeleteFunc = NULL;
HeapBase* Manager::m_defaultNewUseHeap = NULL;
#if GFL_HEAP_DEBUG
bool Manager::m_hioDumpDefault = false;
#endif //GFL_HEAP_DEBUG

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
void Manager::Initialize( void *buf , const int size , const int HeapNum , const int localHeapNum )
{
  
  m_heapMngData = (HEAP_MNG_DATA*)buf;
  m_heapNum = HeapNum;
  for( int i=0;i<HeapNum;i++ )
  {
    m_heapMngData[i].heap = NULL;
    m_heapMngData[i].flg = HEAP_MNG_DEFAULT_FLG;
  }

  //ローカルヒープ系の処理
  if( localHeapNum > 0 )
  {
    m_localHeapMngData = (HEAP_MNG_DATA*)(reinterpret_cast<size_t>(buf) + sizeof(HEAP_MNG_DATA)*HeapNum);
    m_localHeapNum = localHeapNum;
    for( int i=0;i<localHeapNum;i++ )
    {
      m_localHeapMngData[i].heap = NULL;
      m_localHeapMngData[i].flg = HEAP_MNG_DEFAULT_FLG;
    }
  }
  else
  {
    m_localHeapMngData = NULL;
    m_localHeapNum = 0;
  }
  
  void *heapBuf = reinterpret_cast<void*>(reinterpret_cast<size_t>(buf) + sizeof(HEAP_MNG_DATA)*(HeapNum+localHeapNum));
  CreateHeapBuffer( heapBuf , HEAPID_SYSTEM , size - (sizeof(HEAP_MNG_DATA)*(HeapNum+localHeapNum)) , HEAP_TYPE_EXP );
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

  for( int i=0;i<m_localHeapNum;i++ )
  {
    if( m_localHeapMngData[i].heap != NULL ||
        m_localHeapMngData[i].flg != HEAP_MNG_DEFAULT_FLG )
    {
      if( isCheck )
      {
        GFL_ASSERT_MSG( false , "GFL_HEAP Manager exit LocalHeap is left!\n" );//@check
      }

      DeleteHeapCore( m_localHeapMngData[i].heap , isCheck );
      
      m_localHeapMngData[i].heap = NULL;
      m_localHeapMngData[i].flg = HEAP_MNG_DEFAULT_FLG;
    }
  }
  m_localHeapNum = 0;
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

  //ローカルヒープはリセット時には全部消す
  for( int id=0;id<m_localHeapNum;id++ )
  {
    if( m_localHeapMngData[id].heap != NULL ||
        m_localHeapMngData[id].flg != HEAP_MNG_DEFAULT_FLG )
    {
      //親がメモリを消去している恐れがあるので、BUFFER確保にしてDelete内でFreeしない
        
      m_heapMngData[id].flg |= HMB_CREATE_BUFFER;
      DeleteHeapCore( m_heapMngData[id].heap , false );
        
      m_heapMngData[id].heap = NULL;
      m_heapMngData[id].flg = HEAP_MNG_DEFAULT_FLG;
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
  //プラットフォーム別の分CTRExpHeap岐
  //3DSの場合
  switch(heapType)
  {
  case HEAP_TYPE_EXP:
  default:
#if defined(GF_PLATFORM_CTR)
    return sizeof(CTRExpHeap) + size + sizeof(GflHeapBaseLower);

#elif defined(GF_PLATFORM_WIN32)
    return sizeof(WinHeap) + size + sizeof(GflHeapBaseLower);
#endif  //プラットフォーム分岐
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
HeapBase* Manager::CreateHeap( HeapBase *parentHeap , const HEAPID childId , const int size , const HeapType heapType , const bool isBack, const char *heapName, const char *filename, const int linenum )
#else
HeapBase* Manager::CreateHeap( HeapBase *parentHeap , const HEAPID childId , const int size , const HeapType heapType , const bool isBack, const char *heapName )
#endif
{
	int align = parentHeap->GetDefaultAlign();
	if( isBack )
	{
		align *= -1;
	}

  int heap_size = size;

#if defined(GF_PLATFORM_WIN32)
  heap_size *= parentHeap->GetCreateHeapScale();  // Win32では一律scale倍する。
#endif

#if GFL_HEAP_DEBUG
  void *buf = parentHeap->AllocMemoryCore( heap_size , align , false , filename , linenum );
//  void *buf = gfl::heap::GflHeapAllocMemoryBlockAlign( parentHeap , size , align , filename, linenum );
#else
  void *buf = parentHeap->AllocMemoryCore( heap_size , align , false );
//  void *buf = GflHeapAllocMemoryAlign( parentHeap , size , align );
#endif

#if GFL_HEAP_DEBUG
    HeapBase * base;
	base = CreateHeapCore( buf , childId , heap_size , heapType , false , NULL );
	base->SetCreateData( filename, linenum, heapName,true );
#else
  HeapBase * base = CreateHeapCore( buf , childId , heap_size , heapType , false , NULL );
#endif

#if defined(GF_PLATFORM_WIN32)
  base->SetCreateHeapScale( parentHeap->GetCreateHeapScale() );
#endif

#if GFL_HEAP_DEBUG
#if defined(GF_PLATFORM_CTR)
#if GFL_HIO_ENABLE
  if( IsHioDumpDefault() )
  {
    base->StartHioDump();
    base->SetDebugModeFlg(true);
  }
#endif //GFL_HIO_ENABLE
#endif //defined(GF_PLATFORM_CTR)
#endif //GFL_HEAP_DEBUG

	return base;
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
HeapBase* Manager::CreateHeapBuffer( void *buffer , const HEAPID childId , const int size , const HeapType heapType, const char *heapName, const char *filename, const int linenum )
{
	HeapBase * base = CreateHeapCore( buffer , childId , size , heapType , true , NULL );
	base->SetCreateData( filename, linenum, heapName,false );

#if defined(GF_PLATFORM_CTR)
#if GFL_HIO_ENABLE
  if( IsHioDumpDefault() )
  {
    base->StartHioDump();
    base->SetDebugModeFlg(true);
  }
#endif //GFL_HIO_ENABLE
#endif //defined(GF_PLATFORM_CTR)

	return base;
}
#else
HeapBase* Manager::CreateHeapBuffer( void *buffer , const HEAPID childId , const int size , const HeapType heapType, const char *heapName )
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
HeapBase* Manager::CreateHeapBufferEx( void *buffer , const HEAPID childId , const int size , const HeapType heapType , void *param, const char *heapName, const char *filename, const int linenum )
{
	HeapBase * base = CreateHeapCore( buffer , childId , size , heapType , true , param );
	base->SetCreateData( filename, linenum, heapName,false );

#if defined(GF_PLATFORM_CTR)
#if GFL_HIO_ENABLE
  if( IsHioDumpDefault() )
  {
    base->StartHioDump();
  }
#endif //GFL_HIO_ENABLE
#endif //defined(GF_PLATFORM_CTR)

	return base;
}
#else
HeapBase* Manager::CreateHeapBufferEx( void *buffer , const HEAPID childId , const int size , const HeapType heapType , void *param, const char *heapName)
{
  return CreateHeapCore( buffer , childId , size , heapType , true , param );
}
#endif

//ヒープ作成コア部分
HeapBase* Manager::CreateHeapCore( void *buffer , const HEAPID childId , const int size , const HeapType heapType , const bool isBuffer  , void *param )
{
  int heapClassSize;
  HEAPID heapId = childId;

  //HEAPID最大値チェック
  GFL_ASSERT_MSG( childId < m_heapNum || childId == GFL_HEAP_LOCAL_ID , "GFL_HEAP New HeapId is over!! HeapNum[%d] newId[%d]\n",m_heapNum,childId );

  //重複チェック
  if( IsExistHeapId( childId ) == true )
  {
    GFL_ASSERT_MSG( false , "GFL_HEAP Create heap is failue! HeapId[%d] is exist!\n",childId );
    return GetHeapByHeapId(childId);
  }
  
  HEAP_MNG_DATA *mngData = NULL;
  if( childId == GFL_HEAP_LOCAL_ID )
  {
    //空いてるローカルヒープを探す
    for( int i=0;i<m_localHeapNum;i++ )
    {
      if( (m_localHeapMngData[i].flg & HMB_IS_USE) == 0 )
      {
        mngData = &m_localHeapMngData[i];
        //GFL_HEAP_LOCAL_ID_BASE(-10)からマイナス方向に増えていく
        heapId = GFL_HEAP_INDEX_TO_LOCAL_HEAP_ID(i);
        break;
      }
    }
    //ローカルヒープが見つからない
    GFL_ASSERT_STOP_MSG( mngData , "GFL_HEAP LocalHeap is full!!" );
  }
  else
  {
    mngData = &m_heapMngData[childId];
  }

  //プラットフォーム別の分岐
  //3DSの場合
  switch(heapType)
  {
  case HEAP_TYPE_EXP:
  default:
#if defined(GF_PLATFORM_CTR)
    mngData->heap = new(buffer) CTRExpHeap;
    heapClassSize = sizeof(CTRExpHeap);
#elif defined(GF_PLATFORM_WIN32)
    mngData->heap = new(buffer) WinHeap;
    heapClassSize = sizeof(WinHeap);
#endif
    break;
  }
  //プラットフォーム別分岐ここまで
  //nnInitStartUpで動くからGFL_NEW使えない！
  void *lowerBuf = reinterpret_cast<void*>(reinterpret_cast<uptr>(buffer) + heapClassSize);
  GflHeapBaseLower *lowerHandle = new(lowerBuf)GflHeapBaseLower(mngData->heap);
  lowerHandle->InitHeapBase(heapId);
  
  mngData->heap->InitHeapBase(heapId);
  void *heapBuf = reinterpret_cast<void*>(reinterpret_cast<uptr>(buffer) + heapClassSize + sizeof(GflHeapBaseLower));
  mngData->heap->CreateHeap( heapBuf , size - (heapClassSize+sizeof(GflHeapBaseLower)) , param );
  mngData->heap->SetLowerHandle(lowerHandle);
  
  mngData->flg |= HMB_IS_USE;
  if( isBuffer == true )
  {
    mngData->flg |= HMB_CREATE_BUFFER;
  }

  GFL_PRINT_EX(HEAP_PRINT1 , "GFL_HEAP Create new heap[%d] size[%x]\n",heapId,size);
  GFL_PRINT_EX(HEAP_PRINT2 , "ClassSize[%x] RealSize[%x]\n",heapClassSize,size-(heapClassSize+ sizeof(GflHeapBaseLower)));
  GFL_PRINT_EX(HEAP_PRINT2 , "GFL_HEAP HeapClass[%x] Buffer[%x]\n",&mngData->heap,reinterpret_cast<void*>(reinterpret_cast<uptr>(buffer) + heapClassSize + sizeof(GflHeapBaseLower)));
  
  return mngData->heap;
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
#if GFL_HEAP_DEBUG
#if defined(GF_PLATFORM_CTR)
#if GFL_HIO_ENABLE
  if( IsHioDumpDefault() )
  {
    heap->EndHioDump();
  }
#endif //GFL_HIO_ENABLE
#endif //defined(GF_PLATFORM_CTR)
#endif //GFL_HEAP_DEBUG


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
  
  //子ヒープを探す
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
  //子ヒープを探す(ローカルヒープ
  for( int i=0; i<m_localHeapNum; i++ )
  {
    HEAPID localHeapId = GFL_HEAP_INDEX_TO_LOCAL_HEAP_ID(i);
    if( IsExistHeapId(localHeapId) && localHeapId != heapId )
    {
      //バッファ渡しの時は親がわからない！！
      if( !(m_localHeapMngData[i].flg & HMB_CREATE_BUFFER ))
      {
        const HEAPID parentId = HeapBase::GetMemoryHeader(m_localHeapMngData[i].heap)->heapId;
        if( heapId == parentId )
        {
          if( isCheck )
          {
            GFL_ASSERT_MSG( false , "GFL_HEAP HeapId[%d] is have child heap!!childId[%d]\n",heapId,GFL_HEAP_INDEX_TO_LOCAL_HEAP_ID(i) );//@check
          }
          DeleteHeapCore( m_localHeapMngData[i].heap,isCheck );
        }
      }
    }
  }  

  HEAP_MNG_DATA *mngData = IsGetHeapManageData(heapId);

  //deleteはできないので明示的にデストラクタ
  heap->GetLowerHandle()->~HeapBase();

  mngData->heap->TermHeapBase(isCheck);
  mngData->heap->DeleteHeap();
  //deleteはできないので明示的にデストラクタ
  heap->~HeapBase();
  if( !(mngData->flg & HMB_CREATE_BUFFER ))
  {
    GflHeapFreeMemory( heap );
  }
  else
  {
  }
  
  mngData->flg = HEAP_MNG_DEFAULT_FLG;
  mngData->heap = NULL;
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
  if( heapId <= GFL_HEAP_LOCAL_ID_BASE )
  {
    return m_localHeapMngData[GFL_HEAP_LOCAL_HEAP_ID_TO_INDEX(heapId)].heap;
  }
  else
  {
    return m_heapMngData[heapId].heap;
  }
}

//======================================================================
//  utility関数
//======================================================================
//----------------------------------------------------------------------------
/**
  *  @brief  ローカルヒープを加味し、HEAP_MNG_DATAを取得
  *
  *  @param  const HEAPID heapId 調べたいヒープのHEAPID
  *  @return HEAP_MNG_DATA
  */
//-----------------------------------------------------------------------------
HEAP_MNG_DATA* Manager::IsGetHeapManageData( const HEAPID heapId )
{
  if( heapId >= 0 && heapId < m_heapNum )
  {
    return &m_heapMngData[heapId];
  }

    //ローカルヒープ
  if( heapId <= GFL_HEAP_LOCAL_ID_BASE &&
      GFL_HEAP_LOCAL_HEAP_ID_TO_INDEX(heapId) < m_localHeapNum )
  {
    return &m_localHeapMngData[GFL_HEAP_LOCAL_HEAP_ID_TO_INDEX(heapId)];
  }
  return NULL;
}

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
  HEAP_MNG_DATA *mngData = IsGetHeapManageData(heapId);

  if( mngData )
  {
    if( mngData->flg & HMB_IS_USE )
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
  if( IsExistHeapId( header->heapId ) ) //ローカルヒープはここではじく
  {
    HEAP_MNG_DATA *mngData = IsGetHeapManageData(header->heapId);
    return mngData->heap->GetMemorySize(mem);
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
  if( IsExistHeapId( header->heapId ) )//ローカルヒープはここではじく
  {
    HEAP_MNG_DATA *mngData = IsGetHeapManageData(header->heapId);
    return !(mngData->heap->GetMemorySize(mem)==0);
  }
  GFL_PRINT("存在しないHEAPIDのメモリです。[HEAPID:%d]\n",header->heapId);
  return false;
}

/**
*  @brief  渡されたメモリが管理しているメモリかチェックする
*  @note   CheckMemoryBlockでは偶然の一致を検出できない
*  @note   検索するため、重い
*
*  @param  pMemory 調べるメモリ
*
*  @return 管理しているメモリならtrue、管理外ならfalseを返す
*/
b32 Manager::IsManaged( const void *pMemory )
{
  // メモリの先頭を取得
  // これはアドレスをマイナスしているだけなので安全
  const MEMORY_HEADER *pHeader = HeapBase::GetMemoryHeader( pMemory );

  for( int i = 0; i < m_heapNum; i++ )
  {
    if( m_heapMngData[i].flg & HMB_IS_USE )
    {
      if( m_heapMngData[i].heap->IsManaged( pHeader ) )
      {
        return true;
      }
    }
  }
  for( int i = 0; i < m_localHeapNum; i++ )
  {
    if( m_localHeapMngData[i].flg & HMB_IS_USE )
    {
      if( m_localHeapMngData[i].heap->IsManaged( pHeader ) )
      {
        return true;
      }
    }
  }

  return false;
}

/**
*  @brief  渡されたメモリが管理しているメモリかチェックする
*  @note   初期確保サイズの範囲チェックのみを行う
*
*  @param  pMemory 調べるメモリ
*
*  @return 管理しているメモリならtrue、管理外ならfalseを返す
*/
b32 Manager::IsManagedFast( const void *pMemory )
{
  // メモリの先頭を取得
  // これはアドレスをマイナスしているだけなので安全
  const MEMORY_HEADER *pHeader = HeapBase::GetMemoryHeader( pMemory );

  for( int i = 0; i < m_heapNum; i++ )
  {
    if( m_heapMngData[i].flg & HMB_IS_USE )
    {
      if( m_heapMngData[i].heap->IsManagedFast( pHeader ) )
      {
        return true;
      }
    }
  }
  for( int i = 0; i < m_localHeapNum; i++ )
  {
    if( m_localHeapMngData[i].flg & HMB_IS_USE )
    {
      if( m_localHeapMngData[i].heap->IsManagedFast( pHeader ) )
      {
        return true;
      }
    }
  }

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
      GFL_PRINT( "HEAPID[%3d] ",i);
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
  GFL_PRINT( "-----GflHeapManager LocalHeap DumpStart-----\n");
  for( int i=0;i<m_localHeapNum;i++ )
  {
    if( m_localHeapMngData[i].flg & HMB_IS_USE )
    {
      GFL_PRINT( "HEAPID[%3d] ",GFL_HEAP_INDEX_TO_LOCAL_HEAP_ID(i));
#if GFL_HEAP_DEBUG
			if( m_localHeapMngData[i].heap->GetCreateParentFlag() == true ){
				GFL_PRINT( "PARENT[%2d] ",HeapBase::GetMemoryHeader(m_localHeapMngData[i].heap)->heapId);
			}else{
				GFL_PRINT( "PARENT[--] " );
			}
      GFL_PRINT( "Addr[0x%08x] ",m_localHeapMngData[i].heap);
      GFL_PRINT( "Size[0x%08x] ",m_localHeapMngData[i].heap->GetTotalSize());
      GFL_PRINT( "Free[0x%08x] ",m_localHeapMngData[i].heap->GetTotalFreeSize());
      GFL_PRINT( "Allocate[0x%08x] ",m_localHeapMngData[i].heap->GetTotalAllocatableSize());
      GFL_PRINT( "Create[%s(%d)]\n",m_localHeapMngData[i].heap->GetCreateDataFilename(),m_localHeapMngData[i].heap->GetCreateDataLine());
#else
      GFL_PRINT( "Addr[0x%08x] ",m_localHeapMngData[i].heap);
      GFL_PRINT( "Size[0x%08x]\n",m_localHeapMngData[i].heap->GetTotalSize());
#endif
    }
  }
  GFL_PRINT( "-----GflHeapManager DumpFinish----\n");
}

void Manager::DumpTree(HeapBase *heap)
{
#if GFL_HEAP_DEBUG
  typedef struct {
    int id;   // ID
    u16 tab;  // タブ数
    HEAP_MNG_DATA *mng;
  }TREE_WORK;

  //TREE_WORK *tree = GFL_NEW_ARRAY(heap)TREE_WORK[m_heapNum+m_localHeapNum];
  TREE_WORK tree[250];
  int cnt=0;
  int i, j, k;

  // 作られたヒープを抽出
  for( i=0; i<m_heapNum; i++ ){
    if( m_heapMngData[i].flg & HMB_IS_USE ){
      tree[cnt].id  = m_heapMngData[i].heap->GetHeapId();
      tree[cnt].tab = 0;
      tree[cnt].mng = &m_heapMngData[i];
      cnt++;
    }
  }
  for( i=0; i<m_localHeapNum; i++ ){
    if( m_localHeapMngData[i].flg & HMB_IS_USE ){
      tree[cnt].id  = m_localHeapMngData[i].heap->GetHeapId();
      tree[cnt].tab = 0;
      tree[cnt].mng = &m_localHeapMngData[i];
      cnt++;
    }
  }
  // 親子順に並べる
  for( i=0; i<cnt; i++ ){
    if( tree[i].mng->heap->GetCreateParentFlag() == true ){
      int p1 = HeapBase::GetMemoryHeader(tree[i].mng->heap)->heapId;
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
  GFL_PRINT( "HEAPID[]→GlobalHeap HEAPID()→LocalHeap\n" );
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
              GFL_PRINT( "　 " );
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
    if( tree[i].id >= 0 )
    {
      GFL_PRINT( "HEAPID[%3d] ", tree[i].id );
    }
    else
    {
      GFL_PRINT( "HEAPID(%3d) ", GFL_HEAP_LOCAL_HEAP_ID_TO_INDEX(tree[i].id) );
    }
/*
    // 親
    if( tree[i].mng->heap->GetCreateParentFlag() == true ){
      GFL_PRINT( "PARENT[%03d] ", HeapBase::GetMemoryHeader(tree[i].mng->heap)->heapId );
    }else{
      GFL_PRINT( "PARENT[---] " );
    }
*/
//    GFL_PRINT( "\n" );
    // アドレス
    GFL_PRINT( "Addr[0x%08x] ", tree[i].mng->heap);
    // サイズ
    GFL_PRINT( "Size[0x%08x] ", tree[i].mng->heap->GetTotalSize() );
    // 場所
    GFL_PRINT(
      "Create[%s(%d)]\n",
      tree[i].mng->heap->GetCreateDataFilename(),
      tree[i].mng->heap->GetCreateDataLine() );
  }
  GFL_PRINT( "----- GflHeapManager DumpTree Finish -----\n" );
  
  //GFL_DELETE_ARRAY( tree );
#endif
}


#if GFL_HEAP_DEBUG
void Manager::SetHioDumpDefault( bool isOn )
{

#if defined(GF_PLATFORM_CTR)
#if GFL_HIO_ENABLE
  //一度ONにしたら、ファイル破棄時にEndHioDumpになるので、
  //生きているヒープは、HioDumpを中断する
  if( m_hioDumpDefault == true && isOn == false )
  {
    for( int i=0; i<m_heapNum; i++ )
    {
      if( m_heapMngData[i].flg & HMB_IS_USE )
      {
        m_heapMngData[i].heap->AbortHioDump();
      }
    }
    for( int i=0; i<m_localHeapNum; i++ )
    {
      if( m_localHeapMngData[i].flg & HMB_IS_USE )
      {
        m_localHeapMngData[i].heap->AbortHioDump();
      }
    }
  }
#endif //HIO
#endif //CTR

  m_hioDumpDefault = isOn; 
}
#endif


} //namespace heap
} //namespace gfl2
