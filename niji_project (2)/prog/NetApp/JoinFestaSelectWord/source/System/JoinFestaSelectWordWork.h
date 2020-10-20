// ============================================================================
/*
 * @file JoinFestaSelectWordWork.h
 * @brief 挨拶選択アプリのワーククラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORD_WORK_H_INCLUDE )
#define JOINFESTASELECTWORD_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaSelectWordTable.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaSelectWordWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_APP_DEVICE_HEAP_SIZE;

public:
  JoinFestaSelectWordWork();
  virtual~JoinFestaSelectWordWork();

  void SetupTable();
  NetAppLib::JoinFesta::JoinFestaSelectWordTable& GetSelectWordTable(){ return m_JoinFestaSelectWordTable; }

  void SetEditMenuID( NetApp::JoinFestaSelectWord::EditMenuID eEditMenuID ){ m_eEditMenuID = eEditMenuID; }
  NetApp::JoinFestaSelectWord::EditMenuID GetEditMenuID(){ return m_eEditMenuID; }

  void SetMainFrameSequenceNo( int nSequenceNo ){ m_nMainFrameSequenceNo = nSequenceNo; }
  int GetMainFrameSequenceNo(){ return m_nMainFrameSequenceNo; }

  void SetSelectCategoryIndex( u32 index ){ m_unSelectCategoryIndex = index; }
  u32 GetSelectCategoryIndex(){ return m_unSelectCategoryIndex; }

  void SetDescriptionOpenAnimeFlag( bool bFlag ) { m_bDescriptionOpenAnimeFlag = bFlag; }
  bool GetDescriptionOpenAnimeFlag() { return m_bDescriptionOpenAnimeFlag; }

private:

  NetApp::JoinFestaSelectWord::EditMenuID           m_eEditMenuID;
  NetAppLib::JoinFesta::JoinFestaSelectWordTable    m_JoinFestaSelectWordTable;

  int                                               m_nMainFrameSequenceNo;
  u32                                               m_unSelectCategoryIndex;

  bool                                              m_bDescriptionOpenAnimeFlag;

};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORDWORK_H_INCLUDE
