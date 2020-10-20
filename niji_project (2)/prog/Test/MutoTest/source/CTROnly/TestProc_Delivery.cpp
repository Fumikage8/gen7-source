
#if defined(GF_PLATFORM_CTR)
#if PM_DEBUG
//==============================================================================
/**
  *  @file   TestProc_SendIr.cpp
  *  @brief  
  *  @date   2015.06.11 Thu.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <heap/include/gfl2_Heap.h>
#include  <ui/include/gfl2_UI.h>
#include  <str/include/gfl2_Str.h>
#include  "niji_conv_header/poke_lib/item/itemsym.h"
#include  <System/include/HeapDefine.h>
#include  <System/include/ThreadPriority.h>

#include  <Prog/System/include/PokemonVersion.h>
#include  <NetStatic/NetLib/include/NetDefine.h>
#include  <NetStatic/NetLib/include/P2P/P2pConnectionManager.h>

#include  <NetStatic/NetLib/include/NijiNetworkSystem.h>

#include  "Savedata/include/MysteryGiftSave.h"




#include  "../../include/CTROnly/TestProc_Delivery.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
/*
  ※gfl2::fs::DirectSdmcFile内で、
  sdmc:/niji/direct_sdmc/
  までは確定している
*/

#define __TO_WIDE(str)      L ## str
#define TO_WIDE(str)        __TO_WIDE(str)
#define GIFT_PATH_PREFIX    "wondergift/"
  #define GIFT_PATH         "niji/direct_sdmc/" GIFT_PATH_PREFIX;
  static const char* const  pProcTargetPath         = GIFT_PATH;
  static const wchar_t*     pProcTargetPathW        = TO_WIDE( GIFT_PATH );
  static const char*        pProcTargetPathRelative = GIFT_PATH_PREFIX;
#undef  GIFT_PATH_PREFIX





  enum
  {
    UPDATESEQ_SelectMode,
    UPDATESEQ_DelivManagerInitialize,


    UPDATESEQ_Select,


    /*    */
    UPDATESEQ_Send,
    UPDATESEQ_Sending,


    UPDATESEQ_StartReceive,
    UPDATESEQ_Receiving,
    UPDATESEQ_Received,


    UPDATESEQ_Failed,
  
    /*  finalize  */
    UPDATESEQ_StartPrepareFinalize,
    UPDATESEQ_PrepareFinalize,

    UPDATESEQ_Exit,

    UPDATESEQ_
  };




  enum
  {
    ENDSEQ_Done,
    
    ENDSEQ_
  };





  static const struct {
    gflnet2::EN_NET_TYPE                                netType;
    NetLib::Delivery::DeliveryManager::DELIVERY_TYPE    delivType;
    wchar_t*                                            pDescription;
  }connectionModeTable[]  = {

    {gflnet2::TYPE_P2P_WIRELESS,  NetLib::Delivery::DeliveryManager::DELIVERY_TYPE_wireless, L"Local(UDS)"},
    {gflnet2::TYPE_P2P_IR,        NetLib::Delivery::DeliveryManager::DELIVERY_TYPE_ir,       L"Ir"},
//    {gflnet2::TYPE_P2P_WIFI,      NetLib::Delivery::DeliveryManager::DELIVERY_TYPE_wifi,   L"Wifi(BOSS via internet)"},
    
  };


  static const struct {
    const char* const     pArchiveName;
    const wchar_t* const  pArchiveNameW;
  }targetArchiveTable[] = {
#define ARCHIVENAME(_name)  {_name, TO_WIDE(_name)}
    ARCHIVENAME("sdmc"),
  #if GFL_HIO_ENABLE
    #define ARCHIVENAME_HIO "dhio"
    ARCHIVENAME(ARCHIVENAME_HIO),
  #else
    #define ARCHIVENAME_HIO 
  #endif
#undef ARCHIVENAME
  };




  const wchar_t* const    pInfomation_None                    = L"";
  const wchar_t* const    pInfomation_SelectMode              = L"select connection mode";
  const wchar_t* const    pInfomation_DelivManagerInitialize  = L"delivery manager initializing";
  const wchar_t* const    pInfomation_Select                  = L"select function";
  const wchar_t* const    pInfomation_Failed                  = L"failed";
  const wchar_t* const    pInfomation_Sending                 = L"sending";
  const wchar_t* const    pInfomation_Receiving               = L"receiving";
  const wchar_t* const    pInfomation_Received                = L"received";



  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  void  SetStrToBuffer(u8* pDestination, u32 destinationBytes, wchar_t* pStr)
  {
    wchar_t*    pDestinationWide  = reinterpret_cast<wchar_t*>(pDestination);
    const u32   wideNELEMS        = destinationBytes / sizeof(wchar_t);
    
    memcpy(pDestination, pStr, destinationBytes);
    pDestinationWide[wideNELEMS-1]  = L'\0';
  }

} /*  end namespace  */





namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/04/27 Wed. 19:41:00
  */
//==============================================================================
class TestProc_Delivery::DirEntryInfo
{
  GFL_FORBID_COPY_AND_ASSIGN(DirEntryInfo);
public:
  DirEntryInfo(gfl2::heap::HeapBase* pHeapBase)
    : m_pHeapBase(pHeapBase)
    , m_NNFSDir()
    , m_Entries(0)
  {}

  virtual ~DirEntryInfo(void)
  {
    FinalizeNNFSDir();
  }

  void  Refresh(const wchar_t*  pTargetPath)
  {
    GFL_PRINT("%s: targetPath: %ls\n",__FUNCTION__, pTargetPath);
    FinalizeNNFSDir();
    if(InitializeNNFSDir(pTargetPath))
    {
      ReadNNFSDirEntry();
    }
  }

  void  Refresh(const char* const  pTargetPath)
  {
    GFL_PRINT("%s: targetPath: %s\n",__FUNCTION__, pTargetPath);
    FinalizeNNFSDir();
    if(InitializeNNFSDir(pTargetPath))
    {
      ReadNNFSDirEntry();
    }
  }

  u32   Entries(void) const
  {
    return m_Entries;
  }

  const wchar_t*  GetEntryNameW(const u32 index) const
  {
    return ((index < m_Entries) ? m_NNFSDirEntries[index].entryName : L"");
  }

  const char*  GetEntryName(const u32 index) const
  {
    return ((index < m_Entries) ? m_NNFSDirEntriesEntryNames[index] : "");
  }

  u64 GetEntryBytes(const u32 index) const
  {
    return ((index < m_Entries) ? m_NNFSDirEntries[index].entrySize : 0);
  }



private:
  bool  InitializeNNFSDir(const wchar_t* pTargetPath)
  {
    m_Entries = 0;
    
    {
      const nn::Result result = m_NNFSDir.TryInitialize(pTargetPath);
      
      if(result.IsSuccess())
      {
        GFL_PRINT("%s: \"%ls\" open success\n", __FUNCTION__, pTargetPath);
        return true;
      }
    }

    GFL_PRINT("%s: \"%ls\" open failed\n", __FUNCTION__, pTargetPath);
    return false;
  }

  bool  InitializeNNFSDir(const char* const pTargetPath)
  {
    m_Entries = 0;
    
    {
      const nn::Result result = m_NNFSDir.TryInitialize(pTargetPath);
      
      if(result.IsSuccess())
      {
        GFL_PRINT("%s: \"%s\" open success\n", __FUNCTION__, pTargetPath);
        return true;
      }
    }

    GFL_PRINT("%s: \"%s\" open failed\n", __FUNCTION__, pTargetPath);
    return false;
  }


  u32  ReadNNFSDirEntry(void)
  {
    s32               entries = 0;
    const nn::Result  result = m_NNFSDir.TryRead(&entries, m_NNFSDirEntries, GFL_NELEMS(m_NNFSDirEntries));
    
    m_Entries = entries;
    
    if(result.IsSuccess())
    {
      GFL_PRINT("%s: %d entries found.\n", __FUNCTION__, m_Entries);
      for(u32 num = 0; num < m_Entries; ++num)
      {
        GFL_PRINT("  %s\n", GetEntryName(num));
        
        CopyWCharToChar(num, GetEntryNameW(num));
      }
    }

    return m_Entries;
  }


  void  FinalizeNNFSDir(void)
  {
    m_NNFSDir.Finalize();
    m_Entries = 0;
  }

  void  CopyWCharToChar(const u32 index, const wchar_t* pSrc)
  {
    const wchar_t*  pWChar  = pSrc;
    char*           pChar   = m_NNFSDirEntriesEntryNames[index];
    u32             num     = 0;
    
//    num     = snprintf(pChar, nn::fs::MAX_FILE_PATH_LENGTH, "%s", pProcTargetPathRelative);
//    pChar += num;
    for(; num < (GFL_NELEMS(m_NNFSDirEntriesEntryNames)-1); ++num)
    {
      *pChar = static_cast<char>(*pWChar);
      if(*pWChar == L'\0')  break;
      
      ++pChar;
      ++pWChar;
    }
    *pChar  = '\0';
  }



  gfl2::heap::HeapBase*   m_pHeapBase;
  nn::fs::Directory       m_NNFSDir;
  nn::fs::DirectoryEntry  m_NNFSDirEntries[128];
  char                    m_NNFSDirEntriesEntryNames[128][nn::fs::MAX_FILE_PATH_LENGTH+1];
  u32                     m_Entries;
};




//==============================================================================
/**
  *  @brief  
  *  @date   2017/04/06 Thu. 16:55:42
  */
//==============================================================================
class TestProc_Delivery::FileReader
{
  GFL_FORBID_COPY_AND_ASSIGN(FileReader);
public:
  FileReader(gfl2::heap::HeapBase* pHeapBase)
   : m_pHeapBase(pHeapBase)
   , m_pData(NULL)
   , m_BufBytes(0)
   , m_ReadBytes(0)
  {}

  virtual ~FileReader(void)
  {
    Release();
  }

  s32 Read(const char* const pPath, const u32 bytes)     {return ReadCore<char>(pPath, bytes);}
  s32 Read(const wchar_t* const pPath, const u32 bytes)  {return ReadCore<wchar_t>(pPath, bytes);}

  void  Release(void)
  {
    if(m_pData)
    {
      GFL_SAFE_DELETE(m_pData);
      m_BufBytes = 0;
      m_ReadBytes = 0;
    }
  }

  void* GetData(void) {return m_pData;}

protected:
  void  Alloc(const u32 bytes)
  {
    Release();
    m_pData     = GFL_NEW(m_pHeapBase) u8[bytes];
    m_BufBytes  = bytes;
  }

  template<class T>
  s32 ReadCore(const T* const pPath, const u32 bytes)
  {
    Alloc(bytes);
    
    nn::fs::FileReader  reader(pPath);
    return reader.TryRead(&m_ReadBytes, m_pData, m_BufBytes).IsSuccess() ? m_ReadBytes : 0;
  }


  gfl2::heap::HeapBase* m_pHeapBase;
  u8*                   m_pData;
  u32                   m_BufBytes;
  s32                   m_ReadBytes;
};


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2017/04/06 Thu. 17:05:24
  */
//==============================================================================
#if GFL_HIO_ENABLE
class TestProc_Delivery::HIOMounter
{
  GFL_FORBID_COPY_AND_ASSIGN(HIOMounter);
public:
  HIOMounter(gfl2::heap::HeapBase* pHeapBase)
    : m_pHeapBase(pHeapBase)
    , m_pWorkBuf(NULL)
    , m_WorkBytes(0)
  {}

  virtual ~HIOMounter(void)
  {
    Unmount();
  }

  bool Mount(void)
  {
    const size_t  maxFile       = 1;
    const size_t  maxDir        = 1;
    const s32     requiredBytes = nn::fs::hio::GetHioRequiredMemorySize(maxFile, maxDir);

    if(m_pWorkBuf)  return true;

    m_pWorkBuf  = GFL_NEW(m_pHeapBase) u8[requiredBytes];
    m_WorkBytes = requiredBytes;

    /*
      in	archiveName	アーカイブ名を指定します。
      in	hostPath	PC 上のルートディレクトリを指定します。
      in	maxFile	同時に開くファイルの最大数を指定します。
      in	maxDirectory	同時に開くディレクトリの最大数を指定します。
      in	workingMemory	アーカイブの動作に使用するメモリ領域の先頭アドレスを指定します。
      in	workingMemorySize	アーカイブの動作に使用するメモリ領域のサイズを指定します。
    */
    return  nn::fs::hio::MountHioArchive(
               ARCHIVENAME_HIO,
               L"/",
               maxFile, /* size_t maxFile         */
               maxDir, /* size_t maxDirectory,   */
               m_pWorkBuf,
               m_WorkBytes
            ).IsSuccess();
                
  }


  void  Unmount(void)
  {
    if(m_pWorkBuf)
    {
      GFL_SAFE_DELETE(m_pWorkBuf);
      m_WorkBytes = 0;
      nn::fs::Unmount(ARCHIVENAME_HIO);
    }
  }

protected:
  gfl2::heap::HeapBase*   m_pHeapBase;
  void*                   m_pWorkBuf;
  size_t                  m_WorkBytes;
};
#endif







//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
TestProc_Delivery::TestProc_Delivery(void)
  : GameSys::GameProc()
  , m_Heap()
  , m_TextDrawer()
  , m_SelectIndex_TargetArchive(0)
  , m_SelectIndexMax_TargetArchive(GFL_NELEMS(targetArchiveTable))
  , m_SelectIndex_ConnectionMode(0)
  , m_SelectIndexMax_ConnectionMode(GFL_NELEMS(connectionModeTable))
  , m_pConnectionMode(NULL)
  , m_SelectIndex(0)
  , m_SelectIndexMax(0)
  , m_UpdateSeq(UPDATESEQ_SelectMode)
  , m_EndSeq(ENDSEQ_Done)
  , m_ReceivingProgress(0)
  , m_bCorruptCRC(false)
  , m_bDoUDSSigning(true)
  , m_pInformation(NULL)
  , m_pSendDataDescription(NULL)
  , m_pDirInfo(NULL)
  , m_pFileReader(NULL)
  , m_pHIOMounter(NULL)
{
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
TestProc_Delivery::~TestProc_Delivery()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::proc::Result TestProc_Delivery::InitFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;


  CreateHeap();
  m_TextDrawer.Initialize(&m_Heap);
  m_TextDrawer.Scale(.75f, .75f);


  /*  create delivery manager  */
  GFL_NEW(m_Heap.GetDeviceHeap()) NetLib::Delivery::DeliveryManager();

  result	= gfl2::proc::RES_FINISH;

  NetLib::NijiNetworkSystem::InitializeLocalP2PNetwork();

  m_pDirInfo    = GFL_NEW(m_Heap.GetDeviceHeap()) DirEntryInfo(m_Heap.GetDeviceHeap());
  m_pFileReader = GFL_NEW(m_Heap.GetDeviceHeap()) FileReader(m_Heap.GetDeviceHeap());
#if GFL_HIO_ENABLE
  m_pHIOMounter = GFL_NEW(m_Heap.GetDeviceHeap()) HIOMounter(m_Heap.GetDeviceHeap());
  if(m_pHIOMounter->Mount())  GFL_PRINT("hio archive [%s] mounted", ARCHIVENAME_HIO);
  else                        GFL_PRINT("hio archive [%s] mount failed", ARCHIVENAME_HIO);
#endif

/*
  m_pDirInfo->Refresh( pProcTargetPathW );
  m_SelectIndex     = 0;
  m_SelectIndexMax  = m_pDirInfo->Entries();
*/
  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
gfl2::proc::Result TestProc_Delivery::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;

  NetLib::Delivery::DeliveryManager*   pDeliv  = GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager);



  switch(m_EndSeq)
  {

    case  ENDSEQ_Done:
    default:
    {
      GFL_SAFE_DELETE(m_pDirInfo);
      GFL_SAFE_DELETE(m_pFileReader);
#if GFL_HIO_ENABLE
      GFL_SAFE_DELETE(m_pHIOMounter);
#endif
      GFL_SAFE_DELETE(pDeliv);
      m_TextDrawer.Finalize();
      DeleteHeap();
      result	= gfl2::proc::RES_FINISH;
    }
      break;
  }

  NetLib::NijiNetworkSystem::TerminateNetwork();

  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::proc::Result TestProc_Delivery::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	                    result	        = gfl2::proc::RES_CONTINUE;
  gfl2::ui::DeviceManager*                pDeviceManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button*                       pButton         = pDeviceManager->GetButton(0);
  NetLib::Delivery::DeliveryManager*      pDeliv          = GFL_SINGLETON_INSTANCE(NetLib::Delivery::DeliveryManager);


  m_pInformation  = pInfomation_None;
  pDeliv->Update();

  switch(m_UpdateSeq)
  {

    /*  select mode  */
    case  UPDATESEQ_SelectMode:
      m_pInformation    = pInfomation_SelectMode;
      m_pConnectionMode = connectionModeTable[m_SelectIndex_ConnectionMode].pDescription;
      
      if(false)/* dummy  */
      {
      }
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_UP))     m_SelectIndex_ConnectionMode = (m_SelectIndex_ConnectionMode + m_SelectIndexMax_ConnectionMode-1) % m_SelectIndexMax_ConnectionMode;
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_DOWN))   m_SelectIndex_ConnectionMode = (m_SelectIndex_ConnectionMode + 1) % m_SelectIndexMax_ConnectionMode;
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_A))     m_UpdateSeq   = UPDATESEQ_DelivManagerInitialize;
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_B))     m_UpdateSeq   = UPDATESEQ_Exit;
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_Y))     m_bCorruptCRC   = !m_bCorruptCRC;
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_X))     m_bDoUDSSigning = !m_bDoUDSSigning;
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_L))     {m_SelectIndex_TargetArchive = (++m_SelectIndex_TargetArchive) % m_SelectIndexMax_TargetArchive; RefreshList();}
      break;


    case  UPDATESEQ_DelivManagerInitialize:
    {
      m_pInformation  = pInfomation_DelivManagerInitialize;
      const gflnet2::InitParameter
              initParameter = {
                nn::cfg::GetTransferableId(GameSys::GameManager::APLICATION_UNIQUE_ID),   //  自分のTransferdID 
                gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT ),
                gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ),
                connectionModeTable[m_SelectIndex_ConnectionMode].netType,       /*  gflnet2::TYPE_P2P_WIRELESS,  // ネットワーク種別  */
                NetLib::P2P::NIJI_P2P_GIFT,   /*  ENUM_NIJI_P2P_ID,    // 通信別のID  */
                2,                          /*  最大接続人数                */
                false                       /*  //マイグレーション有効無効  */
              };

      const gflnet2::InitParameterFixed
              initParameterFixed  = {
                NIJI_PRODUCT_CODE,      ///< 任天堂からソフトごとに割り当ててくれるID 
                System::GetLang(),      ///< 言語番号 
                System::GetVersion(),   ///< ＲＯＭバージョン番号 
                NIJI_NET_VERSION        ///< 通信バージョン              
              };

      pDeliv->Initialize(&initParameter, &initParameterFixed, connectionModeTable[m_SelectIndex_ConnectionMode].delivType);
      pDeliv->SetUDSSignatureMode(m_bDoUDSSigning, m_bDoUDSSigning);
      pDeliv->SetSendListener( this );
      pDeliv->SetRecvListener( this );
      
      RefreshList();
      m_UpdateSeq = UPDATESEQ_Select;
    }
      break;



    /*
    */
    case  UPDATESEQ_Select:
    {
      m_pInformation  = pInfomation_Select;
      
      if(pButton->IsTrigger(gfl2::ui::BUTTON_R))
      {
        RefreshList();
      }
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_B))     m_UpdateSeq   = UPDATESEQ_StartPrepareFinalize;

      if(m_SelectIndexMax)
      {
             if(pButton->IsRepeat(gfl2::ui::BUTTON_UP))     m_SelectIndex = (m_SelectIndex + m_SelectIndexMax-1) % m_SelectIndexMax;
        else if(pButton->IsRepeat(gfl2::ui::BUTTON_DOWN))   m_SelectIndex = (m_SelectIndex + 1) % m_SelectIndexMax;
        else if(pButton->IsTrigger(gfl2::ui::BUTTON_A))     m_UpdateSeq   = UPDATESEQ_Send;
      }
    }
      break;



    /*
        send
TestDeliveryID にはMYSTERY_GIFT_DATA::eventIDをいれる
    */
#define DELIVTEST_SENDSTART(_pSendData, _dataLength, _pDesc)          \
        do  {                                                         \
          m_pSendDataDescription  = _pDesc;                           \
          if(                                                         \
            pDeliv->StartSend(                                        \
              System::ThreadPriority::DELIVERY_THREAD_PRIORITY-1,     \
              (_pSendData)->dataBody.eventID,                         \
              (_pSendData),                                           \
              (_dataLength)                                           \
            )                                                         \
          ) {                                                         \
            m_UpdateSeq = UPDATESEQ_Sending;                          \
          }else {                                                     \
            m_UpdateSeq = UPDATESEQ_Failed;                           \
          }                                                           \
        }while(0)                                                   

    case  UPDATESEQ_Send:
    {
      const wchar_t*            pFilePathW  = m_pDirInfo->GetEntryNameW(m_SelectIndex);
      const char*               pFilePath   = m_pDirInfo->GetEntryName(m_SelectIndex);
      const u32                 entryBytes  = m_pDirInfo->GetEntryBytes(m_SelectIndex);
//      gfl2::fs::DirectSdmcFile* pSDMC       = gfl2::fs::System::CreateDirectSdmcFile(m_Heap.GetDeviceHeap(), pFilePath, gfl2::fs::Define::OpenMode::READ);

//      if(pSDMC)
      {
        s32               readBytes;
        const s32         bufferBytes   = sizeof(m_GiftReceiveData);
//        const size_t      requestBytes  = bufferBytes;
//        gfl2::fs::Result  fsResult;
        bool              bReadSuccess  = false;

        {
          char  targetFullPath[nn::fs::MAX_FILE_PATH_LENGTH+1];
          
          snprintf(targetFullPath, nn::fs::MAX_FILE_PATH_LENGTH, "%s:%s%s%s", targetArchiveTable[m_SelectIndex_TargetArchive].pArchiveName, m_SelectIndex_TargetArchive ? "" : "/", pProcTargetPath, pFilePath);
          GFL_PRINT("read %s\n", targetFullPath);
          readBytes = m_pFileReader->Read(targetFullPath, entryBytes);
          bReadSuccess  = (0 < readBytes);
        }

//        fsResult  = pSDMC->Read(&readBytes, &m_GiftReceiveData, bufferBytes, requestBytes);
        if(bReadSuccess)
        {
          {
            memcpy(m_GiftReceiveData, m_pFileReader->GetData(), gfl2::math::Min(bufferBytes, readBytes));
          }

          /*
            一日一回チェックのためDateをセットする必要があるが、CRCは前計算であるため不整合を起こす
            ここで再計算することで回避する
          */
          const u32 dataNum = readBytes / sizeof(m_GiftReceiveData[0]);
          nn::fnd::DateTime now = nn::fnd::DateTime::GetNow();
          u32 date = ( now.GetYear() % 100 ) * 10000 + now.GetMonth() * 100 + now.GetDay();
          u16 newCRC;
          for( u32 index = 0 ; index < dataNum; ++index )
          {
            {
              const bool bCRCCheckResult  = m_GiftReceiveData[index].CRCCheck();
              
              GFL_ASSERT(bCRCCheckResult);    /*  意図的に破壊したデータをツールで配信しないように  */
            }


            /*  dateを設定して再計算  */
            m_GiftReceiveData[index].dataBody.recvDate = date;
            m_GiftReceiveData[index].CRCCheck(&newCRC);

            /*
              NMCat[2880]（再）   【ふしぎなおくりもの】壊れたデータを受信した時に、受け取り失敗のテキストが表示されない

              壊れたデータを本ツールに読み込んだ場合でも、ここで上書きされるため正常データとして送られてしまう
              フロー確認のため意図的に壊す機能を用意する
            */
            if(m_bCorruptCRC)
            {
              const u16 corruptCRC  = ~newCRC;
              
              GFL_PRINT("corruptCRC: 0x%04x -> 0x%04x\n", newCRC, corruptCRC);
              m_GiftReceiveData[index].crc  = corruptCRC;
            }
            else
            {
              m_GiftReceiveData[index].crc  = newCRC;
            }

            GFL_PRINT("dataCRC:  0x%04x\n", m_GiftReceiveData[index].crc);
          }

          /*  送信開始  */
          DELIVTEST_SENDSTART(&m_GiftReceiveData[0], readBytes, pFilePathW);
        }
        else
        {
          m_UpdateSeq = UPDATESEQ_Select;
        }
        
//        gfl2::fs::System::DestroyDirectSdmcFile(pSDMC);
      }
/*
      else
      {
        m_UpdateSeq = UPDATESEQ_Select;
      }
*/
    }
      break;

#undef  DELIVTEST_SENDSTART



    case  UPDATESEQ_Sending:
    {
      m_pInformation  = pInfomation_Sending;

      if(pButton->IsTrigger(gfl2::ui::BUTTON_B))
      {
        m_UpdateSeq = UPDATESEQ_StartPrepareFinalize;
      }
    }
      break;



    /*
        recv
    */
    case  UPDATESEQ_StartReceive:
      m_pInformation      = pInfomation_Receiving;
      m_ReceivingProgress = 0;
      if(pDeliv->StartRecv(System::ThreadPriority::DELIVERY_THREAD_PRIORITY))
      {
        m_UpdateSeq = UPDATESEQ_Receiving;
      }else {
        m_UpdateSeq = UPDATESEQ_Failed;
      }
      break;

    case  UPDATESEQ_Receiving:
      m_pInformation  = pInfomation_Receiving;
      if(pButton->IsTrigger(gfl2::ui::BUTTON_B) || pButton->IsTrigger(gfl2::ui::BUTTON_A))
      {
        m_UpdateSeq = UPDATESEQ_StartPrepareFinalize;
      }
      break;


    case  UPDATESEQ_Received:
      m_pInformation      = pInfomation_Received;
      if(pButton->IsTrigger(gfl2::ui::BUTTON_B) || pButton->IsTrigger(gfl2::ui::BUTTON_A))
      {
        m_UpdateSeq = UPDATESEQ_StartPrepareFinalize;
      }
      break;



    /*
      finalize manager
    */
    case  UPDATESEQ_StartPrepareFinalize:
      pDeliv->PrepareFinalize();
      m_UpdateSeq  = UPDATESEQ_PrepareFinalize;
      break;
  

    case  UPDATESEQ_PrepareFinalize:
      if(pDeliv->IsCompretePrepareFinalize())
      {
        pDeliv->Finalize();
        m_UpdateSeq  = UPDATESEQ_SelectMode;
      }
      else
      {
        pDeliv->Update();
      }
      break;
  

   /*
    */
    case  UPDATESEQ_Failed:
    {
      m_pInformation  = pInfomation_Failed;
      
      if(pButton->IsTrigger(gfl2::ui::BUTTON_B))
      {
        m_UpdateSeq = UPDATESEQ_StartPrepareFinalize;
      }
    }
      break;


    case  UPDATESEQ_Exit:
    default:
    {
      result	= gfl2::proc::RES_FINISH;
    }
      break;
  }


  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  TestProc_Delivery::PreDrawFunc( gfl2::proc::Manager* pManager )
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  TestProc_Delivery::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_TextDrawer.CLS();
  m_TextDrawer.LOC(0,0);


  m_TextDrawer.PrintLn(L"[DeliveryTest]");
  m_TextDrawer.PrintLn(L"  U/D: select item");
  m_TextDrawer.PrintLn(L"    A: decide");
  m_TextDrawer.PrintLn(L"    B: cancel");
  m_TextDrawer.PrintLn(L"    X: signing(UDS): [%ls]", m_bDoUDSSigning ? L"Yes" : L"No");
  m_TextDrawer.PrintLn(L"    Y: corrupt CRC: [%ls]",  m_bCorruptCRC   ? L"Yes" : L"No");
  m_TextDrawer.PrintLn(L"    R: refresh list");
  m_TextDrawer.PrintLn(L"    L: archive: [%ls]", targetArchiveTable[m_SelectIndex_TargetArchive].pArchiveNameW);
  m_TextDrawer.CRLF();
  m_TextDrawer.PrintLn(L"  %ls",        m_pInformation);
  m_TextDrawer.PrintLn(L"  mode: %ls",  m_pConnectionMode);

  switch(m_UpdateSeq)
  {
    case  UPDATESEQ_Select:
      if(m_SelectIndexMax)
      {
        m_TextDrawer.PrintLn(L"    ->send");
        m_TextDrawer.PrintLn(L"      : [%d/%d]%ls", m_SelectIndex+1, m_SelectIndexMax, m_pDirInfo->GetEntryNameW(m_SelectIndex));
      }
      else
      {
        m_TextDrawer.PrintLn(L"    giftdata not found");
        m_TextDrawer.PrintLn(L"    targetpath: \"%ls\"", pProcTargetPathW);
      }
      break;

    case  UPDATESEQ_Sending:
      m_TextDrawer.PrintLn(L"    %ls", m_pDirInfo->GetEntryNameW(m_SelectIndex));
      break;

    case  UPDATESEQ_Receiving:
      m_TextDrawer.PrintLn(L"    %3d %%", m_ReceivingProgress);
      break;

    case  UPDATESEQ_Received:
//      m_TextDrawer.PrintLn(L"    %s", dataTable[m_SelectIndex].pDescription);
      m_TextDrawer.PrintLn(L"    received");
      break;
  }

  m_TextDrawer.Draw(displayNo);
}







/*
  NetLib::Delivery::DeliverySendListener
*/
void  TestProc_Delivery::OnNetworkError( const nn::Result &result )
{
  GFL_PRINT("%s\n", __FUNCTION__);
  m_UpdateSeq = UPDATESEQ_Failed;
}


/*
  NetLib::Delivery::DeliveryRecvListener
*/
bool  TestProc_Delivery::OnRecvComplete( const void* pData , size_t dataSize )
{
  GFL_PRINT("%s\n", __FUNCTION__);

  m_UpdateSeq = UPDATESEQ_Received;

  return false;
}


void  TestProc_Delivery::OnUpdateProgress( const u32 progress )
{
  GFL_PRINT("%s\n", __FUNCTION__);

  m_ReceivingProgress = progress;
}


void  TestProc_Delivery::OnNintendoZoneConnected()
{
  GFL_PRINT("%s\n", __FUNCTION__);
  m_UpdateSeq = UPDATESEQ_Failed;
}


void  TestProc_Delivery::OnNetworkError( const nn::Result &result , u32 errorCode )
{
  GFL_PRINT("%s\n", __FUNCTION__);
  m_UpdateSeq = UPDATESEQ_Failed;
}


void  TestProc_Delivery::OnWifiError( const u32 errorCode )
{
  GFL_PRINT("%s\n", __FUNCTION__);
  m_UpdateSeq = UPDATESEQ_Failed;
}










/*
*/
void TestProc_Delivery::CreateHeap( void )
{
  gfl2::heap::HeapBase*   pAppHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase*   pDevHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  
  const size_t            appHeapSize   = 0x00100000;
  const size_t            devHeapSize   = 0x00400000;

  m_Heap.LocalHeapCreate( pAppHeapBase, pDevHeapBase, appHeapSize, devHeapSize);
}


/*
*/
void TestProc_Delivery::DeleteHeap( void )
{
  m_Heap.Delete();
}


void  TestProc_Delivery::RefreshList(void)
{
  char  targetFullPath[nn::fs::MAX_FILE_PATH_LENGTH+1];
  
  snprintf(targetFullPath, nn::fs::MAX_FILE_PATH_LENGTH, "%s:%s%s", targetArchiveTable[m_SelectIndex_TargetArchive].pArchiveName, m_SelectIndex_TargetArchive ? "" : "/", pProcTargetPath);
  m_pDirInfo->Refresh(targetFullPath);
  m_SelectIndex     = 0;
  m_SelectIndexMax  = m_pDirInfo->Entries();
}



} /*  namespace MutoTest  */
} /*  namespace Test  */
#endif  /*  #if PM_DEBUG  */
#endif  /*  #if defined(GF_PLATFORM_CTR)  */
