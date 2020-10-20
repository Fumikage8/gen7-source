//======================================================================
/**
* @file DebugSymbolNameLoader.cpp
* @date 2016/10/04 16:18:19
* @author ikuta_junya
* @brief デバッグ機能用、ゾーン名文字列をロードする機能
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

// gfl
#include <util/include/gfl2_std_string.h>
#include <core/include/gfl2_Utility.h>

#include "Field/FieldStatic/include/Debug/DebugSymbolNameLoader.h"

#include <arc_def_index/arc_def.h>
#include "GameSys/include/GameManager.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

struct SymbolNameLoader::DataHeader
{
  s32 m_NameLengthLimit;
  s32 m_NameNum;
};


SymbolNameLoader::SymbolNameLoader(void) :
m_pDataBuf(NULL)
, m_pNameBuf(NULL)
, m_NameLengthLimit(0)
, m_NameNum(0)
, m_DataBufSize(0)
{

}

SymbolNameLoader::~SymbolNameLoader()
{
  Delete();
}

/**
* @breaf データをロードする
*
* @param pHeap        ロード先を確保するためのヒープ
* @param symbolType   ロードするシンボルデータの種類を指定するオプションです
* @return 成否
*/
b32 SymbolNameLoader::Load(gfl2::heap::HeapBase* pHeap, SymbolType::Value symbolType)
{
  if (symbolType >= SymbolType::VALUE_MAX)
  {
    return false;
  }

  // 同期ロード
  {
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId = ARCID_DEBUG_FIELD_SYMBOL_NAME;
      openReq.heapForFile = pHeap->GetLowerHandle();
      openReq.heapForArcSrc = pHeap->GetLowerHandle();
      openReq.heapForReq = pHeap->GetLowerHandle();
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager()->SyncArcFileOpen(openReq);
    }
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
      loadReq.arcId = ARCID_DEBUG_FIELD_SYMBOL_NAME;
      loadReq.datId = symbolType;
      loadReq.ppBuf = reinterpret_cast<void**>(&m_pDataBuf);
      loadReq.heapForBuf = pHeap;
      loadReq.pBufSize = &m_DataBufSize;
      loadReq.align = 4;
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager()->SyncArcFileLoadData(loadReq);
    }
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_DEBUG_FIELD_SYMBOL_NAME;
      closeReq.heapForReq = pHeap;
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager()->SyncArcFileClose(closeReq);
    }
  }


  // データを解凍
  {
    DataHeader* pHeader = reinterpret_cast<DataHeader*>( m_pDataBuf );
    m_NameLengthLimit = pHeader->m_NameLengthLimit;
    m_NameNum = pHeader->m_NameNum;
    m_pNameBuf = m_pDataBuf + sizeof( DataHeader );
  }

  return true;
}

/**
* @breaf データを削除
*/
void SymbolNameLoader::Delete(void)
{
  GflHeapSafeFreeMemory(m_pDataBuf);
  m_DataBufSize = 0;
}

/**
* @breaf 文字列から値を取得する
* @note  同じ文字列が複数ある場合はデータ順先勝ちとなります
* @param pName      文字列
* @param pOut       値を格納するバッファです
* @return 見当たらなかった場合、false
**/
b32 SymbolNameLoader::GetValue(const c8* pName, u32* pOut) const
{
  if (!Exists())
  {
    return false;
  }

  for (u32 i = 0; i < m_NameNum; i++)
  {
    if (gfl2::core::Utility::Strncmp( &m_pNameBuf[i * m_NameLengthLimit], pName, m_NameLengthLimit ) == 0)
    {
      *pOut = i;
      return true;
    }
  }
  return false;
}

/**
* @breaf 値から文字列を取得します
* @note  同じ値が複数ある場合はデータ順先勝ちとなります
* @param value      値
* @param ppOut      文字列のアドレスを格納するバッファです、実体はこのクラスがロードされたデータです。末尾には\0が付いています。
* @return 見当たらなかった場合、false
**/
b32 SymbolNameLoader::GetName(s32 value, c8** ppOut) const
{
  if (!Exists())
  {
    return false;
  }

  if ( value >= m_NameNum )
  {
    return false;
  }

  *ppOut = &m_pNameBuf[value * m_NameLengthLimit];
  return true;
}

/**
* @breaf データの内容を出力します
* @return 無し
*/
void SymbolNameLoader::Dump(void) const
{
  GFL_RELEASE_PRINT("SymbolNameLoader::Dump Start\n");
  GFL_RELEASE_PRINT("DataCount=%d\n", m_NameNum );
  for (u32 i = 0; i < m_NameNum; i++)
  {
    GFL_RELEASE_PRINT( "[%d]%s\n", i, &m_pNameBuf[i * m_NameLengthLimit] );
  }
  GFL_RELEASE_PRINT("SymbolNameLoader::Dump End\n");
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif