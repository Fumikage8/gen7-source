//==============================================================================
/**
 * @file   JoinFestaResourceManager.cpp
 * @date   2015/10/23 Fri. 12:16:22
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetApp/JoinFesta/include/JoinFestaResourceManager/JoinFestaResourceManager.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <arc_index/JoinFestaSelectWordBin.gaix>

// =============================================================================
/**/
//==============================================================================
namespace
{
  static const u32  MESSAGE_RESOURCES = 2;


  static const u32  arcIDs[] ={
    ARCID_JOIN_FESTA_BIN,
    ARCID_JOIN_FESTA_SELECT_WORD_BIN,
  };


  enum
  {
    STATE_Initial,
    STATE_Loading,
    STATE_Closing,
    STATE_Loaded,

    STATE_
  };



  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  gfl2::fs::ArcFile::ARCDATID ResourceIDtoArcDatID(const NetApp::JoinFesta::JoinFestaResourceManager::ResourceID resourceID)
  {
    gfl2::fs::ArcFile::ARCDATID arcDatID  = gfl2::fs::ArcFile::ARCDATID_NULL;


    switch(resourceID)
    {
      /** messageDataを追加する際はここへ   */
      case  NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFPhrase:   arcDatID  = GARC_message_jf_phrase_DAT;   break;
      case  NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFShopName: arcDatID  = GARC_message_jf_shop_name_DAT;   break;
    }

    GFL_ASSERT((arcDatID  != gfl2::fs::ArcFile::ARCDATID_NULL));

    return arcDatID;
  }
}




namespace NetApp    {
namespace JoinFesta {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
JoinFestaResourceManager::JoinFestaResourceManager(app::util::Heap* pHeap)
  : m_pHeap(pHeap)
  , m_MessageUtil(pHeap->GetDeviceHeap(), MESSAGE_RESOURCES)
  , m_DataTable()
  , m_SelectWordTable()
  , m_State(STATE_Initial)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
JoinFestaResourceManager::~JoinFestaResourceManager()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  JoinFestaResourceManager::RequestLoad(void)
{

  if(!ReleaseData())  {
    return false;
  }

  const gfl2::fs::ArcFile::ARCID  messageArcID  = print::GetMessageArcId();


  /*  arcのopen  */
  {
    gfl2::heap::HeapBase*   pHeapBase = m_pHeap->GetDeviceHeap();
    
    
    for(u32 index = 0; index < GFL_NELEMS(arcIDs); ++index)
    {
      app::util::FileAccessor::FileOpen(arcIDs[index], pHeapBase, true);
    }

    /*  message  */
    app::util::FileAccessor::FileOpen(messageArcID, pHeapBase, true);
  }


  /*  読み込みリソースを追加する際はここへ  */
  m_ResourceLoaders[RESOURCE_Message_JFPhrase             ].RequestLoad(m_pHeap, messageArcID,                      ResourceIDtoArcDatID(RESOURCE_Message_JFPhrase),                              false, true);
  m_ResourceLoaders[RESOURCE_Message_JFShopName           ].RequestLoad(m_pHeap, messageArcID,                      ResourceIDtoArcDatID(RESOURCE_Message_JFShopName),                            false, true);

  /*  datatable  */
  m_ResourceLoaders[RESOURCE_Bin_ShopItem                 ].RequestLoad(m_pHeap, ARCID_JOIN_FESTA_BIN,              GARC_JoinFestaBin_JoinFestaDataTable_ShopItemInfoTable_BIN,                   false, true);
  m_ResourceLoaders[RESOURCE_Bin_ShopService              ].RequestLoad(m_pHeap, ARCID_JOIN_FESTA_BIN,              GARC_JoinFestaBin_JoinFestaDataTable_ShopServiceInfoTable_BIN,                false, true);
  m_ResourceLoaders[RESOURCE_Bin_LotInfo                  ].RequestLoad(m_pHeap, ARCID_JOIN_FESTA_BIN,              GARC_JoinFestaBin_JoinFestaDataTable_LotInfoTable_BIN,                        false, true);
  m_ResourceLoaders[RESOURCE_Bin_AffinityAttraction       ].RequestLoad(m_pHeap, ARCID_JOIN_FESTA_BIN,              GARC_JoinFestaBin_JoinFestaDataTable_AffinityAttraction_BIN,                  false, true);
  m_ResourceLoaders[RESOURCE_Bin_PokeTypeAttractionAlola  ].RequestLoad(m_pHeap, ARCID_JOIN_FESTA_BIN,              GARC_JoinFestaBin_JoinFestaDataTable_PokeTypeAttractionAlola_BIN,             false, true);
  m_ResourceLoaders[RESOURCE_Bin_PokeTypeAttractionAll    ].RequestLoad(m_pHeap, ARCID_JOIN_FESTA_BIN,              GARC_JoinFestaBin_JoinFestaDataTable_PokeTypeAttractionAll_BIN,               false, true);
  m_ResourceLoaders[RESOURCE_Bin_KisekaePattern           ].RequestLoad(m_pHeap, ARCID_JOIN_FESTA_BIN,              GARC_JoinFestaBin_JoinFestaDataTable_KisekaePattern_BIN,                      false, true);
  m_ResourceLoaders[RESOURCE_Bin_SelectWordInfo           ].RequestLoad(m_pHeap, ARCID_JOIN_FESTA_SELECT_WORD_BIN,  GARC_JoinFestaSelectWordBin_JoinFestaSelectWordTable_SelectWordInfoTable_BIN, false, true);
  m_ResourceLoaders[RESOURCE_Bin_SelectWordCategory       ].RequestLoad(m_pHeap, ARCID_JOIN_FESTA_SELECT_WORD_BIN,  GARC_JoinFestaSelectWordBin_JoinFestaSelectWordTable_CategoryTable_BIN,       false, true);

  m_State = STATE_Loading;

  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  JoinFestaResourceManager::ObserveLoad(void)
{
  switch(m_State)
  {
    case  STATE_Initial:
      break;

    case  STATE_Loading:
    {
      bool  isLoaded  = true;
      for(u32 index = 0; index < GFL_NELEMS(m_ResourceLoaders); ++index)
      {
        if(!m_ResourceLoaders[index].ObserveLoad())
        {
          isLoaded = false;
        }
      }
      
      if(isLoaded)
      {
        /*  arcのclose  */
        gfl2::heap::HeapBase*   pHeapBase = m_pHeap->GetDeviceHeap();
        
        
        for(u32 index = 0; index < GFL_NELEMS(arcIDs); ++index)
        {
          app::util::FileAccessor::FileClose(arcIDs[index], pHeapBase);
        }

        /*  message  */
        app::util::FileAccessor::FileClose(print::GetMessageArcId(), pHeapBase);
        m_State  = STATE_Closing;
      }
    }
      break;

    case  STATE_Closing:
    {
      bool  bAllClosed  = true;
      
      for(u32 index = 0; index < GFL_NELEMS(arcIDs); ++index)
      {
        if(!app::util::FileAccessor::IsFileClose(arcIDs[index]))
        {
          bAllClosed  = false;
        }
      }
      
      if(bAllClosed)
      {
        /*  message  */
        bAllClosed  = app::util::FileAccessor::IsFileClose(print::GetMessageArcId());
      }
      
      if(bAllClosed)
      {

        // @fix MMCat[745]：MessageUtilityクラスに予め登録しておく
        //                 GFMMCat[536]の対処で、JoinFestaResouceManagerのヒープをFieldDeviceからFieldExtHeapに変更した。
        //                 MessageUtilityクラスは、Getするタイミングでgfl2::str::MsgDataをNewして登録する仕組みです。
        //                 それ自体は問題ないのですが、FieldExtHeapはイベントの前後のヒープ残量をチェックしており、そのアサートにひっかかります。
        //                 初期化のタイミングで登録するように変更。
        m_MessageUtil.LoadFromMemory( ResourceIDtoArcDatID( RESOURCE_Message_JFPhrase ),    m_ResourceLoaders[RESOURCE_Message_JFPhrase].GetData() );
        m_MessageUtil.LoadFromMemory( ResourceIDtoArcDatID( RESOURCE_Message_JFShopName ),  m_ResourceLoaders[RESOURCE_Message_JFShopName].GetData() );

        m_DataTable.pShopItemInfoTable                = static_cast<ShopItemInfoTable*>(    m_ResourceLoaders[RESOURCE_Bin_ShopItem   ].GetData());
        m_DataTable.pShopServiceInfoTable             = static_cast<ShopServiceInfoTable*>( m_ResourceLoaders[RESOURCE_Bin_ShopService].GetData());
        m_DataTable.pLotInfoTable                     = static_cast<LotInfoTable*>(         m_ResourceLoaders[RESOURCE_Bin_LotInfo    ].GetData());
        m_DataTable.pAffinityAttractionInfoTable      = static_cast<AffinityAttractionInfoTable*>( m_ResourceLoaders[RESOURCE_Bin_AffinityAttraction    ].GetData());
        m_DataTable.pPokeTypeAttractionAlolaInfoTable = static_cast<PokeTypeAttractionAlolaInfoTable*>( m_ResourceLoaders[RESOURCE_Bin_PokeTypeAttractionAlola    ].GetData());
        m_DataTable.pPokeTypeAttractionAllInfoTable   = static_cast<PokeTypeAttractionAllInfoTable*>( m_ResourceLoaders[RESOURCE_Bin_PokeTypeAttractionAll    ].GetData());
        m_DataTable.pKisekaePatternInfoTable          = static_cast<KisekaePatternInfoTable*>( m_ResourceLoaders[RESOURCE_Bin_KisekaePattern    ].GetData());

//        m_DataTable.Dump();

        m_SelectWordTable.Setup( m_ResourceLoaders[ RESOURCE_Bin_SelectWordInfo ].GetData(), m_ResourceLoaders[ RESOURCE_Bin_SelectWordCategory ].GetData() );
        m_State = STATE_Loaded;
      }
    }
      break;

    case  STATE_Loaded:
      break;
    
    default:
      break;
  }


  return (m_State == STATE_Loaded);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void* JoinFestaResourceManager::GetData(const ResourceID resourceID)
{
  return  (m_ResourceLoaders[resourceID].IsLoadComplete() ? m_ResourceLoaders[resourceID].GetData() : NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  JoinFestaResourceManager::ReleaseData(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_ResourceLoaders); ++index)
  {
    if(!m_ResourceLoaders[index].ReleaseData())
    {
      return false;
    }
  }

  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
gfl2::str::StrBuf&    JoinFestaResourceManager::GetString(const ResourceID resourceID, const u32 messageID, const bool isExpand)
{
  const gfl2::fs::ArcFile::ARCDATID arcDatID  = ResourceIDtoArcDatID(resourceID);

  GFL_ASSERT(arcDatID != gfl2::fs::ArcFile::ARCDATID_NULL);

  if(!m_MessageUtil.GetData(arcDatID))
  {
    m_MessageUtil.LoadFromMemory(arcDatID, m_ResourceLoaders[resourceID].GetData());
  }

  if(isExpand)  return m_MessageUtil.GetExpandedString(arcDatID, messageID);
  else          return m_MessageUtil.GetString(arcDatID, messageID);
}

//------------------------------------------------------------------
/**
  *  @func     GetMsgData
  *  @brief    メッセージデータ取得
  *  @param    resourceID     リソースID
  *  @return   MsgData
  */
//------------------------------------------------------------------
gfl2::str::MsgData*   JoinFestaResourceManager::GetMsgData( const ResourceID resourceID )
{
  const gfl2::fs::ArcFile::ARCDATID arcDatID  = ResourceIDtoArcDatID(resourceID);

  GFL_ASSERT(arcDatID != gfl2::fs::ArcFile::ARCDATID_NULL);

  if(!m_MessageUtil.GetData(arcDatID))
  {
    m_MessageUtil.LoadFromMemory(arcDatID, m_ResourceLoaders[resourceID].GetData());
  }

  return m_MessageUtil.GetData( arcDatID );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------



} /*  namespace JoinFesta */
} /*  namespace NetApp    */
