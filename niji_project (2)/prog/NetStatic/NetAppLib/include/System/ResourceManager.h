// ============================================================================
/*
 * @file ResourceManager.h
 * @brief アプリで使用する各リソースを管理するクラスです。
 *        ArcFileを読み込む機能を持っており、
 *        読み込んだバッファを内部で持ちます。
 *        自身が解放された時に読み込んだバッファも削除します。
 * @date 2015.03.27
 */
// ============================================================================
#if !defined( RESOURCEMANAGER_H_INCLUDE )
#define RESOURCEMANAGER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <util/include/gfl2_List.h>
#include "AppLib/include/Util/app_util_heap.h"
#include "NetStatic/NetAppLib/include/System/FontResourceManager.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)

typedef   s32   ResourceID;     // @note : ユニークなIDとして使います

enum CommonResourceID
{
  COMMON_RESOURCE_ID_MENU_WINDOW = 1000,
  COMMON_RESOURCE_ID_MENU_CURSOR = 1001,
  COMMON_RESOURCE_COUNT          = 2
};

class ResourceManager
{
  GFL_FORBID_COPY_AND_ASSIGN( ResourceManager );
private:

  enum LoadState
  {
    LOAD_STATE_FILE_OPEN_START,
    LOAD_STATE_FILE_OPEN_WAIT,
    LOAD_STATE_FILE_READ_START,
    LOAD_STATE_FILE_READ_WAIT,
    LOAD_STATE_FILE_CLOSE_START,
    LOAD_STATE_FILE_CLOSE_WAIT,
    LOAD_STATE_LOAD_FINISH
  };

  typedef struct
  {
    ResourceID                    resourceID;
    gfl2::fs::ArcFile::ARCID      loadArcID;
    gfl2::fs::ArcFile::ARCDATID   loadDataID;
    void*                         pBuffer;
    bool                          compressed;
    bool                          useDeviceHeap;

    gfl2::fs::ToolDefine::ArcLangType::Tag  langTag;
    size_t                        dataSize;
  } ResourceData;

public:
  ResourceManager( app::util::Heap* pAppHeap, u32 resourceCount );
  virtual~ResourceManager();
  
  // @note : アプリケーションシステムで更新処理を呼び出します
  void Update();

  void* GetResourceBuffer( NetAppLib::System::ResourceID resourceID );
  void* GetResourceBuffer( const gfl2::fs::ArcFile::ARCID loadArcID, const gfl2::fs::ArcFile::ARCDATID loadDataID );
  void DeleteResourceBuffer( NetAppLib::System::ResourceID resourceID );
  
  //  リソースサイズを取得
  size_t  GetResourceSize( NetAppLib::System::ResourceID resourceID );

  void AddLoadRequest( NetAppLib::System::ResourceID resourceID, gfl2::fs::ArcFile::ARCID loadArcID, gfl2::fs::ArcFile::ARCDATID loadDataID, bool compressed, bool useDeviceHeap, gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG );
  bool IsLoadFinished();


  enum  FontType
  {
    FONT_NUMBER,
    FONT_BATTLE,

    FONT_
  };
  void FontLoadRequest(const FontType fontType);

private:
  gfl2::heap::HeapBase* getHeap( bool useDeviceHeap );
  bool checkLoadResource( NetAppLib::System::ResourceID resourceID );

  void createResourceBuffer( u32 resourceCount, bool useDeviceHeap = true );
  void deleteResourceBuffer();

private:
  app::util::Heap*                            m_pAppHeap;
  gfl2::util::List<ResourceData*>             m_LoadRequestList;
  gfl2::util::List<ResourceData*>             m_LoadCompleteList;
  gfl2::util::List<ResourceData*>::Iterator   m_ItLoadResource;
  LoadState                                   m_eLoadState;
  FontResourceManager                         m_FontResourceManager;
};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

#endif // RESOURCEMANAGER_H_INCLUDE
