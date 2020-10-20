#if !defined(JOINFESTARESOURCEMANAGER_H_INCLUDED)
#define JOINFESTARESOURCEMANAGER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaResourceManager.h
 * @date   2015/10/23 Fri. 11:55:04
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
/*
  gfl2
*/
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
//#include  <heap/include/gfl2_Heap.h>
//#include  <fs/include/gfl2_Fs.h>
//#include  <str/include/gfl2_Str.h>

#include  "AppLib/include/Util/app_util_heap.h"
#include  "AppLib/source/Tool/GeneralView/Module/ResourceLoader.h"

/*  message  */
#include  "NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h"

/*  datatable  */
#include  "../JoinFestaDataTable/JoinFestaDataTable.h"

/*
  resources
*/
/*  arc  */
#include  <arc_def_index/arc_def.h>
/*  messages  */
#include  <arc_index/message.gaix>
#include  <niji_conv_header/message/msg_jf_phrase.h>
/*  bin  */
#include  <arc_index/JoinFestaBin.gaix>

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaSelectWordTable.h"

namespace NetApp    {
namespace JoinFesta {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/23 Fri. 12:10:59
  */
//==============================================================================
class JoinFestaResourceManager
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaResourceManager);
public:
  enum  ResourceID
  {
    RESOURCE_Message_JFPhrase,
    RESOURCE_Message_JFShopName,

    /*  dataTable  */
    RESOURCE_Bin_ShopItem,
    RESOURCE_Bin_ShopService,
    RESOURCE_Bin_LotInfo,
    RESOURCE_Bin_SelectWordInfo,
    RESOURCE_Bin_SelectWordCategory,
    RESOURCE_Bin_AffinityAttraction,
    RESOURCE_Bin_PokeTypeAttractionAlola,
    RESOURCE_Bin_PokeTypeAttractionAll,
    RESOURCE_Bin_KisekaePattern,

    RESOURCE_MAX
  };


  JoinFestaResourceManager(app::util::Heap* pHeap);
  virtual ~JoinFestaResourceManager();

  bool  RequestLoad(void);
  bool  ObserveLoad(void);
  void* GetData(const ResourceID resourceID);
  bool  ReleaseData(void);

  /*  message  */
  print::WordSet*           GetWordSet(void)  {return m_MessageUtil.GetWordSet();}
  gfl2::str::StrBuf&        GetString(const ResourceID resourceID, const u32 messageID, const bool isExpand = false);
  gfl2::str::MsgData*       GetMsgData( const ResourceID resourceID );

  /*  dataTable  */
  const JoinFestaDataTable& GetDataTable(void) const    {return m_DataTable;}
  const JoinFestaDataTable* GetDataTablePtr(void) const {return &m_DataTable;}

  /* SelectWordTable */
  NetAppLib::JoinFesta::JoinFestaSelectWordTable* GetSelectWordTable() { return &m_SelectWordTable; }

protected:
  app::util::Heap*                    m_pHeap;
  app::tool::ResourceLoader           m_ResourceLoaders[RESOURCE_MAX];
  NetAppLib::Message::MessageUtility  m_MessageUtil;
  JoinFestaDataTable                  m_DataTable;
  NetAppLib::JoinFesta::JoinFestaSelectWordTable m_SelectWordTable;
  u32                                 m_State;
};




} /*  namespace JoinFesta */
} /*  namespace NetApp    */
#endif  /*  #if !defined(JOINFESTARESOURCEMANAGER_H_INCLUDED)  */
