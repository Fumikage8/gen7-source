// ============================================================================
/*
 * @file ResourceManager.cpp
 * @brief アプリで使用する各リソースを管理するクラスです。
 *        ArcFileを読み込む機能を持っており、
 *        読み込んだバッファを内部で持ちます。
 *        自身が解放された時に読み込んだバッファもクリアされます。
 * @date 2015.03.27
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include <debug/include/gfl2_Assert.h>
#include "GameSys/include/GameManager.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


ResourceManager::ResourceManager( app::util::Heap* pAppHeap, u32 resourceCount ) :
  m_pAppHeap( pAppHeap ),
  m_eLoadState( LOAD_STATE_FILE_OPEN_START ),
  m_FontResourceManager(pAppHeap)
{
  createResourceBuffer( resourceCount, true );
}


ResourceManager::~ResourceManager()
{
  deleteResourceBuffer();
}


void ResourceManager::createResourceBuffer( u32 resourceCount, bool useDeviceHeap )
{
  m_LoadRequestList.CreateBuffer( getHeap( useDeviceHeap ), resourceCount );
  m_LoadCompleteList.CreateBuffer( getHeap( useDeviceHeap ), resourceCount );
}


void ResourceManager::deleteResourceBuffer()
{
  GFL_ASSERT_MSG( ( m_LoadRequestList.Size() == 0 ), "Error !! : Not completed load request." );

  for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadCompleteList.Begin(); it != m_LoadCompleteList.End(); ++it )
  {
    if( (*it)->pBuffer != NULL )
    {
      GflHeapFreeMemory( (*it)->pBuffer );
    }
    GFL_SAFE_DELETE( (*it) );
  }

  m_LoadCompleteList.Clear();
  m_LoadRequestList.Clear();
}


gfl2::heap::HeapBase* ResourceManager::getHeap( bool useDeviceHeap )
{
  if( useDeviceHeap )
  {
    return m_pAppHeap->GetDeviceHeap();
  }
  return m_pAppHeap->GetSystemHeap();
}


void ResourceManager::Update()
{
  switch( m_eLoadState )
  {
    // ------------------------------------------------------------------------
    case LOAD_STATE_FILE_OPEN_START:
    {
      gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
      for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadRequestList.Begin(); it != m_LoadRequestList.End(); ++it )
      {
        gfl2::fs::AsyncFileManager::ArcFileOpenReq fileOpenRequest;
        fileOpenRequest.arcId = (*it)->loadArcID;
        fileOpenRequest.heapForFile = getHeap( (*it)->useDeviceHeap )->GetLowerHandle();
        fileOpenRequest.heapForReq = getHeap( (*it)->useDeviceHeap )->GetLowerHandle();
        fileOpenRequest.lang       = (*it)->langTag;

        pAsyncFileManager->AddArcFileOpenReq( fileOpenRequest );
        m_LoadCompleteList.PushBack( (*it) );
      }
      m_eLoadState = LOAD_STATE_FILE_OPEN_WAIT;
    }
    break;

    // ------------------------------------------------------------------------
    case LOAD_STATE_FILE_OPEN_WAIT:
    {
      gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
      bool isFinish = true;
      for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadCompleteList.Begin(); it != m_LoadCompleteList.End(); ++it )
      {
        if( pAsyncFileManager->IsArcFileOpenFinished( (*it)->loadArcID ) == false )
        {
          isFinish = false;
        }
      }

      if( isFinish )
      {
        m_eLoadState = LOAD_STATE_FILE_READ_START;
      }
      else
      {
        break;
      }
    }
    //break;

    // ------------------------------------------------------------------------
    case LOAD_STATE_FILE_READ_START:
    {
      gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
      for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadCompleteList.Begin(); it != m_LoadCompleteList.End(); ++it )
      {
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq fileLoadRequest;
        fileLoadRequest.arcId             = (*it)->loadArcID;
        fileLoadRequest.datId             = (*it)->loadDataID;
        fileLoadRequest.ppBuf             = &(*it)->pBuffer;
        fileLoadRequest.heapForBuf        = getHeap( (*it)->useDeviceHeap );
        fileLoadRequest.heapForReq        = getHeap( (*it)->useDeviceHeap )->GetLowerHandle();
        fileLoadRequest.heapForCompressed = ((*it)->compressed)?(getHeap( (*it)->useDeviceHeap )->GetLowerHandle()):(NULL);
        fileLoadRequest.align             = 128;
        fileLoadRequest.lang              = (*it)->langTag;
        pAsyncFileManager->AddArcFileLoadDataReq( fileLoadRequest );

        //  ロードするデータサイズを取得
        {
          gfl2::fs::ArcFile*  pArcFile  = (gfl2::fs::ArcFile*)pAsyncFileManager->GetArcFile( (*it)->loadArcID );
          if( pArcFile != NULL )
          {
            pArcFile->GetDataSize( &(*it)->dataSize, (*it)->loadDataID, getHeap( (*it)->useDeviceHeap )->GetLowerHandle() );
          }
        }
      }

      m_eLoadState = LOAD_STATE_FILE_READ_WAIT;
    }
    break;

    // ------------------------------------------------------------------------
    case LOAD_STATE_FILE_READ_WAIT:
    {
      gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
      bool isFinish = true;
      for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadCompleteList.Begin(); it != m_LoadCompleteList.End(); ++it )
      {
        if( pAsyncFileManager->IsArcFileLoadDataFinished( &(*it)->pBuffer ) == false )
        {
          isFinish = false;
        }
      }

      if( isFinish )
      {
        m_eLoadState = LOAD_STATE_FILE_CLOSE_START;
      }
      else
      {
        break;
      }
    }
    //break;

    // ------------------------------------------------------------------------
    case LOAD_STATE_FILE_CLOSE_START:
    {
      gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
      for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadCompleteList.Begin(); it != m_LoadCompleteList.End(); ++it )
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq fileCloseRequest;
        fileCloseRequest.arcId      = (*it)->loadArcID;
        fileCloseRequest.heapForReq = getHeap( (*it)->useDeviceHeap )->GetLowerHandle();

        pAsyncFileManager->AddArcFileCloseReq( fileCloseRequest );
      }
      m_eLoadState = LOAD_STATE_FILE_CLOSE_WAIT;
    }
    break;

    // ------------------------------------------------------------------------
    case LOAD_STATE_FILE_CLOSE_WAIT:
    {
      gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
      bool isFinish = true;
      for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadCompleteList.Begin(); it != m_LoadCompleteList.End(); ++it )
      {
        if( pAsyncFileManager->IsArcFileCloseFinished( (*it)->loadArcID ) == false )
        {
          isFinish = false;
        }
      }

      if( isFinish )
      {
        m_LoadRequestList.Clear();
        m_eLoadState = LOAD_STATE_LOAD_FINISH;
      }
    }
    break;
    case LOAD_STATE_LOAD_FINISH:
    break;
  }

  /*  font  */
  m_FontResourceManager.Update();
}


void* ResourceManager::GetResourceBuffer( NetAppLib::System::ResourceID resourceID )
{
  for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadCompleteList.Begin(); it != m_LoadCompleteList.End(); ++it )
  {
    if( (*it)->resourceID == resourceID )
    {
      return (*it)->pBuffer;
    }
  }
  return NULL;
}


void* ResourceManager::GetResourceBuffer( const gfl2::fs::ArcFile::ARCID loadArcID, const gfl2::fs::ArcFile::ARCDATID loadDataID )
{
  for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadCompleteList.Begin(); it != m_LoadCompleteList.End(); ++it )
  {
    if( ((*it)->loadArcID == loadArcID) && ((*it)->loadDataID == loadDataID) )
    {
      return (*it)->pBuffer;
    }
  }
  return NULL;
}


void ResourceManager::DeleteResourceBuffer( NetAppLib::System::ResourceID resourceID )
{
  for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadCompleteList.Begin(); it != m_LoadCompleteList.End(); ++it )
  {
    if( (*it)->resourceID == resourceID )
    {
      GflHeapFreeMemory( (*it)->pBuffer );
      GFL_SAFE_DELETE( (*it) );
      m_LoadCompleteList.Erase( it );
      break;
    }
  }
}

//  リソースサイズを取得
size_t  ResourceManager::GetResourceSize( NetAppLib::System::ResourceID resourceID )
{
  for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadCompleteList.Begin(); it != m_LoadCompleteList.End(); ++it )
  {
    if( (*it)->resourceID == resourceID )
    {
      return (*it)->dataSize;
    }
  }

  return  0;
}

bool ResourceManager::IsLoadFinished()
{
  return (
    (m_LoadRequestList.Size() == 0)
    &&  (m_FontResourceManager.IsStable())
  );
}


void ResourceManager::AddLoadRequest( 
NetAppLib::System::ResourceID resourceID, 
gfl2::fs::ArcFile::ARCID loadArcID, 
gfl2::fs::ArcFile::ARCDATID loadDataID, 
bool compressed, 
bool useDeviceHeap,
gfl2::fs::ToolDefine::ArcLangType::Tag lang  )
{
  if( !checkLoadResource( resourceID ) )
  {
    ResourceData* pResourceData = GFL_NEW( getHeap( useDeviceHeap ) ) ResourceData;
  
    pResourceData->resourceID    = resourceID;
    pResourceData->loadArcID     = loadArcID;
    pResourceData->loadDataID    = loadDataID;
    pResourceData->pBuffer       = NULL;
    pResourceData->compressed    = compressed;
    pResourceData->useDeviceHeap = useDeviceHeap;
    pResourceData->langTag       = lang;

    m_LoadRequestList.PushBack( pResourceData );
  }
}


bool ResourceManager::checkLoadResource( NetAppLib::System::ResourceID resourceID )
{
  for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadCompleteList.Begin(); it != m_LoadCompleteList.End(); ++it )
  {
    if( (*it)->resourceID == resourceID )
    {
      return true;
    }
  }
  for( gfl2::util::List<ResourceData*>::Iterator it = m_LoadRequestList.Begin(); it != m_LoadRequestList.End(); ++it )
  {
    if( (*it)->resourceID == resourceID )
    {
      return true;
    }
  }
  return false;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void ResourceManager::FontLoadRequest(const FontType fontType)
{
  FontResourceManager::FontType fontResourceType;
  bool                          isValidType = true;

  switch(fontType)
  {
    case  FONT_NUMBER:
      fontResourceType  = FontResourceManager::FONT_NUMBER;
      break;
    case  FONT_BATTLE:
      fontResourceType  = FontResourceManager::FONT_BATTLE;
      break;

    default:
      GFL_PRINT("warning %s(%d):%s: invalid fonttype(%d)\n", __FILE__, __LINE__, __FUNCTION__, static_cast<int>(fontType));
      isValidType = false;
      break;
  }

  if(isValidType)
  {
    m_FontResourceManager.LoadRequest(fontResourceType);
  }
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)
