// ============================================================================
/*
 * @file JoinFestaRecordListManager.h
 * @brief レコードリスト管理クラス
 * @date 2015.10.22
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTARECORDLISTMANAGER_H_INCLUDE )
#define JOINFESTARECORDLISTMANAGER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <util/include/gfl2_List.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


struct JoinFestaRecordData
{
  u32     titleMessageID;
  u32     menuMessageID;
  u32     rank;
  u32     recordMenuMessageIDTable[8];
  u32     recordParams[8];
  bool    bEnable[8];
  bool    bVisible[8];
};


typedef gfl2::util::List<NetAppLib::JoinFestaUI::JoinFestaRecordData>             JoinFestaRecordDataList;
typedef gfl2::util::List<NetAppLib::JoinFestaUI::JoinFestaRecordData>::Iterator   ItJoinFestaRecordDataList;


class JoinFestaRecordListManager
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaRecordListManager );

private:

  enum
  {
    RECORD_LIST_MAX = 7
  };

public:

  JoinFestaRecordListManager( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaRecordListManager();

  NetAppLib::JoinFestaUI::JoinFestaRecordDataList& GetJoinFestaRecordDataList() { return m_JoinFestaRecordDataList; }

  void SetupRecordDataList( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData );

private:

  void setRecordParam( JoinFestaRecordData& data, u32 index, u32 messageID, u32 param, bool bEnable, bool bVisible );

private:

  NetAppLib::JoinFestaUI::JoinFestaRecordDataList     m_JoinFestaRecordDataList;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTARECORDLISTMANAGER_H_INCLUDE
