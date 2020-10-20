
#if defined(GF_PLATFORM_CTR)
#if PM_DEBUG
//==============================================================================
/**
 * @file   TestProc_SendTool_LiveRegulation.cpp
 * @date   2016/01/06 Wed. 15:24:14
 * @author muto_yu
 * @brief  ライブ大会レギュレーション送信ツール
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <heap/include/gfl2_Heap.h>
#include  <fs/include/gfl2_Fs.h>
#include  <System/include/HeapDefine.h>
#include  "Battle/Regulation/include/Regulation.h"

#include  "System/include/PokemonVersion.h"

#include  "../../include/CTROnly/TestProc_SendTool_LiveRegulation.h"


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

#define __TO_WIDE(str)            L ## str
#define TO_WIDE(str)              __TO_WIDE(str)
#define REGULATION_PATH_PREFIX    "regulation/live/"
  static const struct
  {
    u32       pokeLang;
    char*     pFilePath;
    wchar_t*  pFilePathW;
  }liveRegulationDataTable[]  = {
#define FILEDEF(pokelang, filepath) {pokelang, REGULATION_PATH_PREFIX #filepath, TO_WIDE(REGULATION_PATH_PREFIX #filepath)}

    FILEDEF(POKEMON_LANG_JAPAN,   regulation_jp.bin),
    FILEDEF(POKEMON_LANG_ENGLISH, regulation_en.bin),
    FILEDEF(POKEMON_LANG_FRANCE,  regulation_fr.bin),
    FILEDEF(POKEMON_LANG_ITALY,   regulation_it.bin),
    FILEDEF(POKEMON_LANG_GERMANY, regulation_de.bin),
    FILEDEF(POKEMON_LANG_SPAIN,   regulation_es.bin),
    FILEDEF(POKEMON_LANG_KOREA,   regulation_ko.bin),
    FILEDEF(POKEMON_LANG_CHINA,   regulation_zh-cn.bin),  /**<  簡体字  */
    FILEDEF(POKEMON_LANG_TAIWAN,  regulation_zh-tw.bin),  /**<  繁体字  */

#undef  FILEDEF
  };
#undef  REGULATION_PATH_PREFIX
#undef  TO_WIDE
#undef  __TO_WIDE


  static const u32  liveRegulationDataTableNum            = GFL_NELEMS(liveRegulationDataTable);
  static const u32  liveRegulationDataTableDefaultIndex   = 1;  /**<  english  */

  enum  State
  {
    STATE_Entry,

    STATE_FileCheck,
    STATE_FileCheckFailed,

    STATE_Idle,

    STATE_ConnectStart,
    STATE_Connecting,
    STATE_SendStart,
    STATE_Sending,
    STATE_Complete,
    STATE_DisconnectStart,
    STATE_Disconnecting,

    STATE_Exit,
    STATE_
  };
}


namespace Test      {
namespace MutoTest  {


struct TestProc_SendTool_LiveRegulation::ToolInfo
{
  gfl2::heap::HeapBase* pHeapBase;
  u32                   fileIndex;
  Regulation*           pSendRegulations[liveRegulationDataTableNum];   /**<  全言語分  */
  u8                    regulationReadBuffer[Regulation::REGULATION_MAX_DATA_SIZE_LIVE];

  ToolInfo(gfl2::heap::HeapBase* _pHeapBase)
    : pHeapBase(_pHeapBase)
    , fileIndex(0)
  {
    for(u32 index = 0; index < GFL_NELEMS(pSendRegulations); ++index)
    {
      pSendRegulations[index] = GFL_NEW(pHeapBase) Regulation(pHeapBase);
    }
  }

  virtual ~ToolInfo()
  {
    for(u32 index = 0; index < GFL_NELEMS(pSendRegulations); ++index)
    {
      GFL_SAFE_DELETE(pSendRegulations[index]);
    }
  }
};


// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
TestProc_SendTool_LiveRegulation::TestProc_SendTool_LiveRegulation(void)
  : GameSys::GameProc()
  , NetApp::Live::LiveNet::IEventHandler()
  , m_Heap()
  , m_TextDrawer()
  , m_State(STATE_Entry)
  , m_pToolInfo(NULL)
  , m_pLiveNet(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
TestProc_SendTool_LiveRegulation::~TestProc_SendTool_LiveRegulation()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
gfl2::proc::Result TestProc_SendTool_LiveRegulation::InitFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result  result  = gfl2::proc::RES_CONTINUE;

  CreateHeap();
  m_TextDrawer.Initialize(&m_Heap);


  gfl2::heap::HeapBase* pHeapBase = m_Heap.GetDeviceHeap();

  {
    m_pToolInfo = GFL_NEW(pHeapBase)  ToolInfo(pHeapBase);
  }


  /*  livenet  */
  {
    
    m_pLiveNet  = GFL_NEW(pHeapBase) NetApp::Live::LiveNet(pHeapBase);
    m_pLiveNet->Initialize();
    m_pLiveNet->SetEventHandler(this);
  }




  result  = gfl2::proc::RES_FINISH;
  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::proc::Result TestProc_SendTool_LiveRegulation::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	      result	        = gfl2::proc::RES_CONTINUE;

  gfl2::ui::DeviceManager*  pDeviceManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button*         pButton         = pDeviceManager->GetButton(0);


  m_TextDrawer.CLS();
  m_TextDrawer.LOC(0,0);
  m_TextDrawer.PrintLn(L"[SendTool] LiveReguration");

  m_pLiveNet->Update();

  switch(m_State)
  {
    case  STATE_Entry:
      m_pToolInfo->fileIndex    = 0;
      m_State                   = STATE_FileCheck;
      
      /*  fallthrough  */


    case  STATE_FileCheck:
    {
      if(m_pToolInfo->fileIndex < liveRegulationDataTableNum)
      {
        const wchar_t*            pFilePathW  = liveRegulationDataTable[m_pToolInfo->fileIndex].pFilePathW;
        const char*               pFilePath   = liveRegulationDataTable[m_pToolInfo->fileIndex].pFilePath;
        gfl2::fs::DirectSdmcFile* pSDMC       = gfl2::fs::System::CreateDirectSdmcFile(m_pToolInfo->pHeapBase, pFilePath, gfl2::fs::Define::OpenMode::READ);


        m_TextDrawer.PrintLn(L"  read regulation data from SDMC: [%d/%d]", m_pToolInfo->fileIndex+1, liveRegulationDataTableNum);
        m_TextDrawer.PrintLn(L"    \"%ls\"", pFilePathW);
        
        if(pSDMC)
        {
          size_t            readBytes;
          const size_t      bufferBytes   = sizeof(m_pToolInfo->regulationReadBuffer);
          const size_t      requestBytes  = bufferBytes;
          gfl2::fs::Result  fsResult;

          fsResult  = pSDMC->Read(&readBytes, m_pToolInfo->regulationReadBuffer, bufferBytes, requestBytes);
          if(fsResult.IsSuccess())
          {
            m_pToolInfo->pSendRegulations[m_pToolInfo->fileIndex]->Deserialize(m_pToolInfo->regulationReadBuffer, readBytes);
            ++m_pToolInfo->fileIndex;
          }
          else
          {
            m_State = STATE_FileCheckFailed;
          }
          
          gfl2::fs::System::DestroyDirectSdmcFile(pSDMC);
        }
        else
        {
          m_State = STATE_FileCheckFailed;
        }
      }
      else
      {
        m_State = STATE_Idle;
      }
    }
      break;


    case  STATE_FileCheckFailed:
    {
      m_TextDrawer.PrintLn(L"  \"%ls\"", liveRegulationDataTable[m_pToolInfo->fileIndex].pFilePathW);
      m_TextDrawer.PrintLn(L"  read failed");
      m_TextDrawer.CRLF();
      m_TextDrawer.PrintLn(L"    A:retry");
      m_TextDrawer.PrintLn(L"    B:exit");
      
      if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
      {
        m_pToolInfo->fileIndex    = 0;
        m_State                   = STATE_FileCheck;
      }
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_B))
      {
        m_State                   = STATE_Exit;
      }
    }
      break;



    case  STATE_Idle:
    {
      m_TextDrawer.PrintLn(L"  idle");
      m_TextDrawer.CRLF();
      m_TextDrawer.PrintLn(L"    A:send");
      m_TextDrawer.PrintLn(L"    B:exit");

      if(pButton->IsTrigger(gfl2::ui::BUTTON_A))        m_State = STATE_ConnectStart;
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_B))   m_State = STATE_Exit;
   }
      break;

    case  STATE_ConnectStart:
      m_pLiveNet->Connect();
      m_State = STATE_Connecting;
      /*  fallthrough  */

    case  STATE_Connecting:
    {
      m_TextDrawer.PrintLn(L"  Ir connecting...");
      m_TextDrawer.CRLF();
      m_TextDrawer.PrintLn(L"    B:cancel");

      if(m_pLiveNet->IsConnected())
      {
        const u8  pokeLangID_Oppo = m_pLiveNet->GetMyStatus_Opponent()->GetPokeLanguageId();

        /*  送信データ選択  */
        m_pToolInfo->fileIndex  = liveRegulationDataTableDefaultIndex;
        for(u32 index = 0; index < liveRegulationDataTableNum; ++index)
        {
          if(liveRegulationDataTable[index].pokeLang == pokeLangID_Oppo)
          {
            m_pToolInfo->fileIndex  = index;
            break;
          }
        }
        m_State = STATE_SendStart;
      }
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_B))
      {
        m_State = STATE_DisconnectStart;
      }
    }
      break;

    case  STATE_SendStart:
      m_pLiveNet->ExchangeRegulation(m_pToolInfo->pSendRegulations[m_pToolInfo->fileIndex]);
      m_State = STATE_Sending;
      /*  fallthrough  */

    case  STATE_Sending:
    {
      const wchar_t*            pFilePathW  = liveRegulationDataTable[m_pToolInfo->fileIndex].pFilePathW;
      gfl2::str::STRCODE        nameBufW[256];
      Savedata::MyStatus*       pMyStatus_Oppo  =   m_pLiveNet->GetMyStatus_Opponent();
      const u8                  pokeLangID_Oppo = m_pLiveNet->GetMyStatus_Opponent()->GetPokeLanguageId();

      pMyStatus_Oppo->GetNameString(nameBufW);

      m_TextDrawer.PrintLn(L"  Ir sending...");
      m_TextDrawer.CRLF();
      m_TextDrawer.PrintLn(L"    send:");
      m_TextDrawer.PrintLn(L"      \"%ls\"", pFilePathW);
      m_TextDrawer.PrintLn(L"    to:");
      m_TextDrawer.PrintLn(L"      %ls", nameBufW);
      m_TextDrawer.PrintLn(L"        langID: %d", pokeLangID_Oppo);
      m_TextDrawer.CRLF();
      m_TextDrawer.PrintLn(L"    B:cancel");

      if(pButton->IsTrigger(gfl2::ui::BUTTON_B))
      {
        m_State = STATE_DisconnectStart;
      }

    }
      break;


    case  STATE_DisconnectStart:
      m_pLiveNet->Disconnect();
      m_State  = STATE_Disconnecting;
      /*  fallthrough  */

    case  STATE_Disconnecting:
    {
      m_TextDrawer.PrintLn(L"  Ir disconnecting...");

      if(m_pLiveNet->IsDisconnected())
      {
        m_State = STATE_Idle;
      }
    }
      break;

    case  STATE_Exit:
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
  *  @brief    
  */
//------------------------------------------------------------------
void TestProc_SendTool_LiveRegulation::PreDrawFunc( gfl2::proc::Manager* pManager )
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void TestProc_SendTool_LiveRegulation::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{



  m_TextDrawer.Draw(displayNo);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::proc::Result TestProc_SendTool_LiveRegulation::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result  result  = gfl2::proc::RES_CONTINUE;


  GFL_SAFE_DELETE(m_pToolInfo);

  {
    m_pLiveNet->Finalize();
    GFL_SAFE_DELETE(m_pLiveNet);
  }



  m_TextDrawer.Finalize();
  DeleteHeap();

  result  = gfl2::proc::RES_FINISH;
  return result;
 
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  TestProc_SendTool_LiveRegulation::CreateHeap(void)
{
  gfl2::heap::HeapBase*   pAppHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase*   pDevHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  const size_t            appHeapSize   = pAppHeapBase->GetTotalAllocatableSize() * 2 / 10 - 0x0100;
  const size_t            devHeapSize   = pDevHeapBase->GetTotalAllocatableSize() * 8 / 10 - 0x0100;

  m_Heap.LocalHeapCreate( pAppHeapBase, pDevHeapBase, appHeapSize, devHeapSize);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  TestProc_SendTool_LiveRegulation::DeleteHeap(void)
{
  m_Heap.Delete();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
/*  implement NetApp::Live::LiveNet::IEventHandler  */
void  TestProc_SendTool_LiveRegulation::OnReceive_Regulation(     NetApp::Live::LiveNet* pSender, const NetApp::Live::SerializedRegulation& rSerializedRegulation )
{
  if(m_State == STATE_Sending)
  {
    m_State  = STATE_DisconnectStart;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  TestProc_SendTool_LiveRegulation::OnReceive_LiveBattleInfo( NetApp::Live::LiveNet* pSender, const NetApp::Live::LiveBattleInfo& rMyLiveBattleInfo, const NetApp::Live::LiveBattleInfo& rOpponentLiveBattleInfo)
{
  /*  nop  */
}


} /*  namespace MutoTest  */
} /*  namespace Test      */

#endif /* PM_DEBUG  */
#endif /* defined(GF_PLATFORM_CTR)  */
