//==============================================================================
/**
 @file    ZukanPokeIconTextureManager.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.01
 @brief   図鑑のポケアイコンのテクスチャのマネージャ
 */
//==============================================================================

// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>

// 図鑑のインクルード
#include "ZukanDefine.h"
#include "ZukanUtil.h"
#include "ZukanDataManager.h"

#include "ZukanPokeIconTextureManager.h"



  
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanPokeIconTextureManager
//
//==============================================================================
 
ZukanPokeIconTextureManager::ZukanPokeIconTextureManager(app::util::Heap* appHeap, const ZukanDataManager* dataManager, u32 pokeIconTexNum)
  : m_dataManager(dataManager),
    m_pokeIconTexNum(pokeIconTexNum),
    m_tempMem(NULL),
    m_bufferIndex(NULL)
{
  for(u32 i=0; i<2; ++i)
  {
    m_devMem[i] = NULL;
  }
  for(u32 i=0; i<2; ++i)
  {
    m_pokeIconTexBuf[i] = NULL;
  }

  // ヒープ
  for(u32 i=0; i<2; ++i)
  {
    m_devMem[i] = GFL_NEW_ARRAY(appHeap->GetDeviceHeap()) gfl2::heap::HeapBase*[m_pokeIconTexNum];
  }
  for(u32 i=0; i<2; ++i)
  {
    for(u32 j=0; j<m_pokeIconTexNum; ++j)
    {
      m_devMem[i][j] = GFL_CREATE_LOCAL_HEAP(appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_POKE_ICON_TEX_BUF_DEV_MEM, gfl2::heap::HEAP_TYPE_EXP, false);
    }
  }

  m_tempMem = GFL_CREATE_LOCAL_HEAP(appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_POKE_ICON_TEX_BUF_TEMP_MEM, gfl2::heap::HEAP_TYPE_EXP, false);

  // テクスチャバッファ
  for(u32 i=0; i<2; ++i)
  {
    m_pokeIconTexBuf[i] = GFL_NEW_ARRAY(appHeap->GetDeviceHeap()) void*[m_pokeIconTexNum];
  }
  for(u32 i=0; i<2; ++i)
  {
    for(u32 j=0; j<m_pokeIconTexNum; ++j)
    {
      m_pokeIconTexBuf[i][j] = NULL;
    }
  }

  // バッファインデックス
  m_bufferIndex = GFL_NEW_ARRAY(appHeap->GetDeviceHeap()) u8[m_pokeIconTexNum];
  for(u32 j=0; j<m_pokeIconTexNum; ++j)
  {
    m_bufferIndex[j] = 0;
  }
}

ZukanPokeIconTextureManager::~ZukanPokeIconTextureManager()
{
  // バッファインデックス
  GFL_SAFE_DELETE_ARRAY(m_bufferIndex);
  
  // テクスチャバッファ
  for(u32 i=0; i<2; ++i)
  {
    for(u32 j=0; j<m_pokeIconTexNum; ++j)
    {
      GflHeapSafeFreeMemory(m_pokeIconTexBuf[i][j]);
    }
  }
  for(u32 i=0; i<2; ++i)
  {
    GFL_SAFE_DELETE_ARRAY(m_pokeIconTexBuf[i]);
  }

  // ヒープ
  GFL_DELETE_HEAP(m_tempMem);
  m_tempMem = NULL;

  for(u32 i=0; i<2; ++i)
  {
    for(u32 j=0; j<m_pokeIconTexNum; ++j)
    {
      GFL_DELETE_HEAP(m_devMem[i][j]);
      m_devMem[i][j] = NULL;
    }
  }
  for(u32 i=0; i<2; ++i)
  {
    GFL_SAFE_DELETE_ARRAY(m_devMem[i]);
  }
}

void ZukanPokeIconTextureManager::CreatePokeIconTexture(
    u32                           pokeIconTexIndex,
    gfl2::lyt::LytPicture*        pane,
    u32                           material,
    u32                           texture,
    const PokeTool::SimpleParam&  sp,
    bool                          flip
)
{
  GflHeapSafeFreeMemory(m_pokeIconTexBuf[m_bufferIndex[pokeIconTexIndex]][pokeIconTexIndex]);

  m_pokeIconTexBuf[m_bufferIndex[pokeIconTexIndex]][pokeIconTexIndex] = ZukanUtil::CreatePokeIconTexture(
      m_devMem[m_bufferIndex[pokeIconTexIndex]][pokeIconTexIndex],
      m_tempMem,
      pane,
      material,
      texture,
      sp,
      flip,
      m_dataManager
  );

  // 使ったので番号を変えておく。
  m_bufferIndex[pokeIconTexIndex] = (m_bufferIndex[pokeIconTexIndex] +1) %2;
}

void ZukanPokeIconTextureManager::CreatePokeIconTexture2(
    u32                           pokeIconTexIndex,
    gfl2::lyt::LytPicture*        pane0pane,
    u32                           pane0material,
    u32                           pane0texture,
    gfl2::lyt::LytPicture*        pane1pane,
    u32                           pane1material,
    u32                           pane1texture,
    const PokeTool::SimpleParam&  sp,
    bool                          flip
)
{
  GflHeapSafeFreeMemory(m_pokeIconTexBuf[m_bufferIndex[pokeIconTexIndex]][pokeIconTexIndex]);

  m_pokeIconTexBuf[m_bufferIndex[pokeIconTexIndex]][pokeIconTexIndex] = ZukanUtil::CreatePokeIconTexture(
      m_devMem[m_bufferIndex[pokeIconTexIndex]][pokeIconTexIndex],
      m_tempMem,
      pane0pane,
      pane0material,
      pane0texture,
      sp,
      flip,
      m_dataManager
  );

  // 使ったので番号を変えておく。
  m_bufferIndex[pokeIconTexIndex] = (m_bufferIndex[pokeIconTexIndex] +1) %2;

  // 2個目のペインに貼る
  gfl2::lyt::LytResTextureInfo textureInfo;
  pane0pane->GetMaterial(pane0material)->GetTexMap(pane0texture).GetTextureInfo(&textureInfo);
  bool replaceTexRes = app::tool::LytTexReplaceSystem::ReplaceTextureStatic(
      &textureInfo,
      pane1pane,
      pane1material,
      pane1texture
  );
  GFL_ASSERT(replaceTexRes);  // 開発中に気付かせるためのASSERT。テクスチャを置き換えるのに失敗。
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

