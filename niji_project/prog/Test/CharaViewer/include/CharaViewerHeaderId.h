//======================================================================
/**
 * @file CharaViewerHeaderId.h
 * @date 2015/07/24 20:05:03
 * @author takebe_makoto
 * @brief ヘッダーから文字列とIDを取得
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined __CHARA_VIEWER_HEADER_ID__
#define __CHARA_VIEWER_HEADER_ID__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

// niji
#include "GameSys/include/GameProc.h"
#include "System/include/GflUse.h"
#include <system/include/nijiAllocator.h>

#include <model/include/gfl2_CharaModelFactory.h>
#include <model/include/gfl2_CharaModel.h>

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)

struct HeaderId
{
  const char* pName;
  u32 id;
  u32 refCounter;

  HeaderId():refCounter(0){}

  const char* GetCharaName() const
  {
    return pName + 15; // "chara_model_id"
  }

  const char* GetPartsName() const
  {
    return pName + 17; // "dressup_parts_id"
  }

  void IncrementRefCounter()
  {
    ++refCounter;
  }

  void DecrementRefCounter()
  {
    --refCounter;
  }
};

class HeaderIdList
{
public:

  static const HeaderIdList* Create(const u32* pData, gfl2::heap::HeapBase* pHeap)
  {
    u32 dataCount = *pData++;
    HeaderIdList* pHeaderIdList = GFL_NEW(pHeap) HeaderIdList(pHeap, dataCount);

    // ID読み取り
    for (u32 i = 0; i < dataCount; ++i)
    {
      pHeaderIdList->GetData(i)->id = *pData++;
    }

    // 名前読み取り
    const char* pName = reinterpret_cast<const char*>(pData);
    for (u32 i = 0; i < dataCount; ++i)
    {
      pHeaderIdList->GetData(i)->pName = pName;

      pName += ::std::strlen(pName) + 1;
    }

    // ヘッダーの名前
    pHeaderIdList->SetName(pName);

    return pHeaderIdList;
  }

  ~HeaderIdList()
  {
    GFL_SAFE_DELETE_ARRAY(m_pHeaderIdList);
  }

  u32 GetDataCount() const
  {
    return m_DataCount;
  }

  HeaderId* GetData(u32 index)
  {
    GFL_ASSERT(index < m_DataCount);
    return &m_pHeaderIdList[index];
  }
  const HeaderId* GetData(u32 index) const
  {
    GFL_ASSERT(index < m_DataCount);
    return &m_pHeaderIdList[index];
  }

  void SetName(const char* pName)
  {
    m_pName = pName;
  }

  const char* GetName() const
  {
    return m_pName;
  }

private:

  HeaderIdList(gfl2::heap::HeapBase* pHeap, u32 dataCount) :
    m_DataCount(0),
    m_pHeaderIdList(NULL)
  {
    m_DataCount = dataCount;
    if (dataCount > 0)
    {
      m_pHeaderIdList = GFL_NEW_ARRAY(pHeap) HeaderId[dataCount];
    }
  }

private:

  u32 m_DataCount;
  HeaderId* m_pHeaderIdList;
  const char* m_pName;
};

class HeaderIdMap
{
public:

  static const HeaderIdMap* Create(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pHeap, u32 arcId, u32 datId)
  {
    void* pData = NULL;

    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId				= arcId;
      req.heapForFile	= pHeap->GetLowerHandle();
      req.heapForReq	= pHeap->GetLowerHandle();

      pFileManager->SyncArcFileOpen(req);
    }
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      req.arcId      = arcId;
      req.datId      = datId;
      req.ppBuf      = &pData;
      req.heapForBuf = pHeap;
      req.align      = 128; // テクスチャは128バイトアラインメント
      req.heapForReq = pHeap->GetLowerHandle();
      req.heapForCompressed = NULL;

      pFileManager->SyncArcFileLoadData(req);
    }

    gfl2::fs::BinLinkerAccessor binLinkerAccessor(pData);
    u32 dataCount = binLinkerAccessor.GetDataMax();

    HeaderIdMap* pHeaderIdMap = GFL_NEW(pHeap) HeaderIdMap(pHeap, dataCount, pData);

    for (u32 i = 0; i < dataCount; ++i)
    {
      const u32* pBinData = static_cast<const u32*>(binLinkerAccessor.GetData(i));
      const HeaderIdList* pHeaderIdList = HeaderIdList::Create(pBinData, pHeap);

      pHeaderIdMap->Insert(i, pHeaderIdList);
    }

    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId				= arcId;
      req.heapForReq	= pHeap->GetLowerHandle();

      pFileManager->SyncArcFileClose(req);
    }

    return pHeaderIdMap;
  }

  ~HeaderIdMap()
  {
    for (u32 i = 0; i < m_DataCount; ++i)
    {
      GFL_SAFE_DELETE(m_pHeaderIdListList[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_pHeaderIdListList);

    GflHeapSafeFreeMemory(m_pAllocatedMemory);
  }

  void Insert(u32 index, const HeaderIdList* pHeaderIdList)
  {
    m_pHeaderIdListList[index] = pHeaderIdList;
  }

  const HeaderIdList* Find(const char* pName) const
  {
    for (u32 i = 0; i < m_DataCount; ++i)
    {
      const HeaderIdList* pHeaderIdList = m_pHeaderIdListList[i];
      if (::std::strcmp(pHeaderIdList->GetName(), pName) == 0)
      {
        return pHeaderIdList;
      }
    }

    return NULL;
  }

private:

  HeaderIdMap(gfl2::heap::HeapBase* pHeap, u32 dataCount, void* pAllocatedMemory) :
    m_DataCount(0),
    m_pHeaderIdListList(NULL),
    m_pAllocatedMemory(NULL)
  {
    m_DataCount = dataCount;
    m_pHeaderIdListList = GFL_NEW_ARRAY(pHeap) const HeaderIdList*[dataCount];

    m_pAllocatedMemory = pAllocatedMemory;
  }

private:

  u32 m_DataCount;
  const HeaderIdList** m_pHeaderIdListList;

  void* m_pAllocatedMemory;
};

GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // __HEADER_H_INCLUDED__

#endif // PM_DEBUG
