// ============================================================================
/*
 * @file JoinFestaRecordSelectLowerViewListener.h
 * @brief レコード選択下画面のリスナー
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTARECORDSELECTLOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTARECORDSELECTLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListLowerViewBaseListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordListManager.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaRecordSelectLowerViewListener : public NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener
{
public:

  JoinFestaRecordSelectLowerViewListener() :
    m_pSelectRecord( NULL )
  {
  }
  ~JoinFestaRecordSelectLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_SELECT_RECORD = JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_START
  };

  void SetSelectRecord( NetAppLib::JoinFestaUI::JoinFestaRecordData* pData ) { SetInputEventID( INPUT_EVENT_ID_SELECT_RECORD ); m_pSelectRecord = pData; }

  NetAppLib::JoinFestaUI::JoinFestaRecordData* GetSelectRecord() { return m_pSelectRecord; }

private:

  NetAppLib::JoinFestaUI::JoinFestaRecordData*      m_pSelectRecord;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTARECORDSELECTLOWERVIEWLISTENER_H_INCLUDE
