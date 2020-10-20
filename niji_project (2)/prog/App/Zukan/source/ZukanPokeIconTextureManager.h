#if !defined(NIJI_PROJECT_APP_ZUKAN_POKEICONTEXTUREMANAGER_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_POKEICONTEXTUREMANAGER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanPokeIconTextureManager.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.01
 @brief   図鑑のポケアイコンのテクスチャのマネージャ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDataManager;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanPokeIconTextureManager
 @brief     図鑑のポケアイコンのテクスチャのマネージャ
 */
//==============================================================================
class ZukanPokeIconTextureManager
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanPokeIconTextureManager);

public:
  ZukanPokeIconTextureManager(app::util::Heap* appHeap, const ZukanDataManager* dataManager, u32 pokeIconTexNum);
  virtual ~ZukanPokeIconTextureManager();

public:
  void CreatePokeIconTexture(
      u32                           pokeIconTexIndex,
      gfl2::lyt::LytPicture*        pane,
      u32                           material,
      u32                           texture,
      const PokeTool::SimpleParam&  sp,
      bool                          flip
  );

  // 2個のペインに同じものを貼る
  void CreatePokeIconTexture2(
      u32                           pokeIconTexIndex,
      gfl2::lyt::LytPicture*        pane0pane,
      u32                           pane0aterial,
      u32                           pane0texture,
      gfl2::lyt::LytPicture*        pane1pane,
      u32                           pane1material,
      u32                           pane1texture,
      const PokeTool::SimpleParam&  sp,
      bool                          flip
  );

private:
  const ZukanDataManager* m_dataManager;
  u32                     m_pokeIconTexNum;
  gfl2::heap::HeapBase*   m_tempMem;
  gfl2::heap::HeapBase**  m_devMem[2];
  void**                  m_pokeIconTexBuf[2];
  u8*                     m_bufferIndex;  // これから使ってもいいインデックス(0 or 1)。使うと番号が変わる。
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_POKEICONTEXTUREMANAGER_H_INCLUDED

