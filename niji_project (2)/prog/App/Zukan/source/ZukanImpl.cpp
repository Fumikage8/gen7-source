//==============================================================================
/**
 @file    ZukanImpl.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑の実装
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>
#include <Fade/include/gfl2_FadeManager.h>
#include <debug/include/gfl2_DebugPrint.h>

// nijiのインクルード
#include <System/include/HeapDefine.h>

#include <System/include/nijiAllocator.h>
#include <Print/include/SystemFont.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <AppLib/include/Frame/AppFrameManager.h>

// 図鑑登録の呼び出しEventのインクルード
#include <App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h>

// 図鑑登録チェッカーのインクルード
#include <PokeTool/include/PokeToolPokeGet.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDefine.h"
#include "ZukanDataManager.h"
#include "ZukanPokePicManager.h"
#include "ZukanFrame.h"
#include "ZukanView.h"
#include "ZukanTool.h"
#include "ZukanContents.h"

#include "ZukanImpl.h"

#include "ZukanDebugWin.h"
#include "ZukanSaveDataDebugWin.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


ZukanImpl::ZukanImpl(ZukanProcParam* param, app::util::Heap* appHeap, GameSys::GameManager* gameManager, void* debugInfo)
  : m_param(param),
    m_appHeap(appHeap),
    m_gameManager(gameManager),
    m_implInstanceDevMem(NULL),
    m_dataDevMem(NULL),
    m_frameManagerDevMem(NULL),
    m_vramAllocatorDevMem(NULL),
    m_contentsDevMem(NULL),
    m_commonAppHeap(NULL),
    m_pokeModelCompDevMem(NULL),
    m_langMsgDataDevMem(NULL),
    m_langFontDevMem(NULL),
    m_instanceAppHeap(NULL),
    m_debugDevMem(NULL),
    m_nijiAllocatorForEdgeMapTexture(NULL),
    m_fadeColor(0.0f, 0.0f, 0.0f, 255.0f),
    m_seq(0),
    m_seqCount(0),
    m_dataManager(NULL),
    m_pokePicManager(NULL),
    m_contents(NULL),
    m_frameManager(NULL),
    m_frameListener(NULL),
    m_frameCell(NULL),
    m_frameCreated(false),
    m_frameReqDestroy(false),
    m_frameDestroying(false),
    m_appRenderingManager(NULL),
    m_lytSys(NULL),
    m_view(NULL),
    m_tool(NULL),
    m_setupState(ZukanType::SETUP_STATE_NONE),
    m_zukanDebugWin(NULL),
    m_zukanSaveDataDebugWin(NULL),
    m_debugInfo(debugInfo),
    m_RotomToneGenerator()
{
  // メモリ
  m_implInstanceDevMem  = GFL_CREATE_LOCAL_HEAP(m_appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_IMPL_INSTANCE_DEV_MEM,  gfl2::heap::HEAP_TYPE_EXP, false);
  
  if(m_param->caller==ZukanProcParam::CALLER_ZUKAN_EVENT)
  {
    m_dataDevMem        = GFL_CREATE_LOCAL_HEAP(m_appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_DATA_DEV_MEM,           gfl2::heap::HEAP_TYPE_EXP, false);
  }
  else  // if(m_param->caller!=ZukanProcParam::CALLER_ZUKAN_EVENT)
  {
    m_dataDevMem        = GFL_CREATE_LOCAL_HEAP(m_appHeap->GetDeviceHeap(), ZUKAN_REGISTER_MEM_SIZE_DATA_DEV_MEM,           gfl2::heap::HEAP_TYPE_EXP, false);
  }
  
  m_frameManagerDevMem  = GFL_CREATE_LOCAL_HEAP(m_appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_FRAME_MANAGER_DEV_MEM,  gfl2::heap::HEAP_TYPE_EXP, false);
  m_vramAllocatorDevMem = GFL_CREATE_LOCAL_HEAP(m_appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_VRAM_ALLOCATOR_DEV_MEM, gfl2::heap::HEAP_TYPE_EXP, false);
  m_contentsDevMem      = GFL_CREATE_LOCAL_HEAP(m_appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_CONTENTS_DEV_MEM,       gfl2::heap::HEAP_TYPE_EXP, false);
  
  m_commonAppHeap = GFL_NEW(m_implInstanceDevMem) app::util::Heap();
  m_commonAppHeap->LocalHeapCreate(m_appHeap->GetSystemHeap(), m_appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_COMMON_HEAP_MEM, ZUKAN_MEM_SIZE_COMMON_DEV_MEM);

  m_pokeModelCompDevMem = GFL_CREATE_LOCAL_HEAP(m_appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_POKEMODEL_COMP_DEV_MEM,  gfl2::heap::HEAP_TYPE_EXP, false);

  if(m_param->caller==ZukanProcParam::CALLER_ZUKAN_EVENT)
  {
    m_langMsgDataDevMem = GFL_CREATE_LOCAL_HEAP(m_appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_LANG_MSG_DATA_DEV_MEM,  gfl2::heap::HEAP_TYPE_EXP, false);
    m_langFontDevMem    = GFL_CREATE_LOCAL_HEAP(m_appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_LANG_FONT_DEV_MEM,      gfl2::heap::HEAP_TYPE_EXP, false);
  }

  m_instanceAppHeap = GFL_NEW(m_implInstanceDevMem) app::util::Heap();
  if(m_param->caller==ZukanProcParam::CALLER_ZUKAN_EVENT)
  { 
    m_instanceAppHeap->LocalHeapCreate(m_appHeap->GetSystemHeap(), m_appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_INSTANCE_HEAP_MEM, ZUKAN_MEM_SIZE_INSTANCE_DEV_MEM);
  }
  else  // if(m_param->caller!=ZukanProcParam::CALLER_ZUKAN_EVENT)
  {
    m_instanceAppHeap->LocalHeapCreate(m_appHeap->GetSystemHeap(), m_appHeap->GetDeviceHeap(), ZUKAN_MEM_SIZE_INSTANCE_HEAP_MEM, ZUKAN_REGISTER_MEM_SIZE_INSTANCE_DEV_MEM);
  }

#if PM_DEBUG
  {
    gfl2::heap::HeapBase* parentHeapMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
    m_debugDevMem = GFL_CREATE_LOCAL_HEAP(parentHeapMem, ZUKAN_MEM_SIZE_DEBUG_DEV_MEM, gfl2::heap::HEAP_TYPE_EXP, false);
  }
#endif

  // VRAMアロケータ
  m_nijiAllocatorForEdgeMapTexture = GFL_NEW(m_implInstanceDevMem) System::nijiAllocator(m_vramAllocatorDevMem); 
}
ZukanImpl::~ZukanImpl()
{
  // VRAMアロケータ
  GFL_SAFE_DELETE(m_nijiAllocatorForEdgeMapTexture);

  // メモリ
#if PM_DEBUG
  GFL_DELETE_HEAP(m_debugDevMem);
  m_debugDevMem = NULL;
#endif
  GFL_SAFE_DELETE(m_instanceAppHeap);

  if(m_langFontDevMem)
  {
    GFL_DELETE_HEAP(m_langFontDevMem);
    m_langFontDevMem = NULL;
  }
  if(m_langMsgDataDevMem)
  {
    GFL_DELETE_HEAP(m_langMsgDataDevMem);
    m_langMsgDataDevMem = NULL;
  }

  GFL_DELETE_HEAP(m_pokeModelCompDevMem);
  m_pokeModelCompDevMem = NULL;
  GFL_SAFE_DELETE(m_commonAppHeap);
  GFL_DELETE_HEAP(m_contentsDevMem);
  m_contentsDevMem = NULL;
  GFL_DELETE_HEAP(m_vramAllocatorDevMem);
  m_vramAllocatorDevMem = NULL;
  GFL_DELETE_HEAP(m_frameManagerDevMem);
  m_frameManagerDevMem = NULL;
  GFL_DELETE_HEAP(m_dataDevMem);
  m_dataDevMem = NULL;
  GFL_DELETE_HEAP(m_implInstanceDevMem);
  m_implInstanceDevMem = NULL;
}

gfl2::proc::Result ZukanImpl::InitFunc(void)
{
  enum
  {
    SEQ_FIRST_ONE,  // 最初の1フレームだけ
    SEQ_START_CREATE,
    SEQ_WAIT_CREATED,
    SEQ_WAIT_TOOL_CREATED,
    SEQ_END,
  };
      
  gfl2::proc::Result res = gfl2::proc::RES_CONTINUE;

  switch(m_seq)
  {
  case SEQ_FIRST_ONE:
    {
      // 最初に画面を真っ黒にしておく
      GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut(gfl2::Fade::DISP_DOUBLE, &m_fadeColor);

      m_setupState = ZukanType::SETUP_STATE_ONE_CREATING;
#if PM_DEBUG
      m_zukanDebugWin = GFL_NEW(m_debugDevMem) ZukanDebugWin(m_debugDevMem, NULL);
      m_zukanSaveDataDebugWin = GFL_NEW(m_debugDevMem) ZukanSaveDataDebugWin(m_debugDevMem, NULL);
#endif
      this->changeSeq(SEQ_START_CREATE);
      //breakしない
    }
  case SEQ_START_CREATE:
    {
#if 0
      テスト
      {
        u32 infoNum = m_gameManager->GetAsyncFileManager()->GetFsDebugLogArcFileInfoNum();
        for(u32 reverseIndex=0; reverseIndex<infoNum; ++reverseIndex)
        {
          const gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfo* info = m_gameManager->GetAsyncFileManager()->GetFsDebugLogArcFileInfo(reverseIndex);
          GFL_PRINT("%s [micro second]\n", __FUNCTION__);
          GFL_PRINT("inde: ty k arc datId  compSize      size      fileAdd    fileStart   fileFinish      fileF-S    fileF-Add    uncompAdd  uncompStart uncompFinish    uncompF-S  uncompF-Add   uncoF-filS uncoF-filAdd\n");
          //        "tIdx: typ kin arcI datI comp size filAdd fStart fFinis fiF_S fiF_A uncADD uncSta uncFin unF_S unF_A uF_fS uF_fA\n"
          GFL_PRINT("%4ld: %2d %1d %3ld %5ld %9ld %9ld %12lld %12lld %12lld %12ld %12ld %12lld %12lld %12lld %12ld %12ld %12ld %12ld\n",
              info->totalIndex,
              info->type,
              info->kind,
              info->arcId,
              info->datId,
              info->compSize,
              info->size,
              info->time[gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_ADD     ],
              info->time[gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_START   ],
              info->time[gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoStep::FILE_FINISH  ],
              info->diffTime[gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoTerm::FILE_FINISH_START       ],
              info->diffTime[gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoTerm::FILE_FINISH_ADD         ],
              info->time[gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoStep::UNCOMP_ADD   ],
              info->time[gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoStep::UNCOMP_START ],
              info->time[gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoStep::UNCOMP_FINISH],
              info->diffTime[gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_START     ],
              info->diffTime[gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_ADD       ],
              info->diffTime[gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_FILE_START],
              info->diffTime[gfl2::fs::AsyncFileManager::FsDebugLogArcFileInfoTerm::UNCOMP_FINISH_FILE_ADD  ]
          );
        }
      }
#endif

      // データマネージャ
      {
        m_dataManager = GFL_NEW(m_implInstanceDevMem) ZukanDataManager(
            m_dataDevMem,
            m_gameManager->GetAsyncFileManager(),
            (m_param->caller==ZukanProcParam::CALLER_ZUKAN_EVENT)?(ZukanDataManager::MODE_ZUKAN):(ZukanDataManager::MODE_ZUKAN_REGISTER)
        );
        m_dataManager->Start();
      }

      // 図鑑の絵巻のポケモン画像のマネージャ
      {
        m_pokePicManager = GFL_NEW(m_implInstanceDevMem) ZukanPokePicManager(m_commonAppHeap->GetDeviceHeap());
      }

      // フレームマネージャ生成
      {
        m_frameManager = GFL_NEW(m_implInstanceDevMem) applib::frame::Manager(m_frameManagerDevMem);  // マネージャはヒープが荒れるので、専用のヒープを渡す。
        m_frameListener = GFL_NEW(m_implInstanceDevMem) ZukanFrameListener();
        m_frameManager->SetAppFrameListener(m_frameListener);
      }
      
      // レンダリングマネージャ
      {
        m_appRenderingManager = GFL_NEW(m_implInstanceDevMem) app::util::AppRenderingManager();
      
        app::util::AppRenderingManager::Description desc;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder       = app::util::AppRenderingManager::RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_LAYOUT2;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].useDistortionPath = true;
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder       = app::util::AppRenderingManager::RenderOrderType::LAYOUT0_LAYOUT1_LAYOUT2;
        m_appRenderingManager->StartAsyncCreate(
            desc,
            m_commonAppHeap,
            m_nijiAllocatorForEdgeMapTexture,
            m_gameManager->GetAsyncFileManager()
        );
      }

      // レイアウト生成
      {
        gfl2::lyt::LytSysSetUp setup =
        {
          GFL_SINGLETON_INSTANCE(print::SystemFont)->GetRectDrawerShaderRes() ,
          app::util::G2DUtil::DEFAULT_LYTSYS_SETUP_RES_MAX                    ,    // 管理リソース最大数
          app::util::G2DUtil::DEFAULT_LYTSYS_SETUP_WORK_MAX                        // 管理レイアウトワーク最大数
        };
        m_lytSys = GFL_NEW(m_implInstanceDevMem) gfl2::lyt::LytSys( &setup, m_commonAppHeap->GetSystemAllocator(), m_commonAppHeap->GetDeviceAllocator(), m_gameManager->GetDrawingLytWkManager() );
      }

      // ビュー生成
      {
        m_view = GFL_NEW(m_implInstanceDevMem) ZukanView(m_commonAppHeap);
        m_view->SetInputEnabledAllView(false);  // ZukanViewの親であるapp::ui::UIViewのメンバ
      } 
      
      this->changeSeq(SEQ_WAIT_CREATED);
      //breakしない
    }
#if 1  // 必要最小限のものだけのセットアップにし、高速化をはかる
  case SEQ_WAIT_CREATED:
    {
      if(m_param->caller==ZukanProcParam::CALLER_ZUKAN_EVENT)
      {
        void* zukanRes2dDataBuf        = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_RES2D        );
        void* zukanGrpFontRes2dDataBuf = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_GRPFONT_RES2D);
        void* zukanMstxtDataBuf        = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_MSTXT        );
       
        // データマネージャ
        // レンダリングマネージャ
        if(
               zukanRes2dDataBuf      
            && zukanGrpFontRes2dDataBuf           
            && zukanMstxtDataBuf      
            
            && m_appRenderingManager->IsCreated()
        )
        {
          ////////////////////////////////
          // ツールやフレームセルは他のものに依存しているので、後で生成、先に破棄。
          ////////////////////////////////      
          // ツール生成
          {
            m_tool = GFL_NEW(m_implInstanceDevMem) ZukanTool(m_gameManager, m_appRenderingManager);
            // ツールの言語別MsgData、言語別Font
            m_tool->CreateLangMsgDataManager(m_commonAppHeap, m_langMsgDataDevMem);
            m_tool->CreateLangFontManager(m_commonAppHeap, m_langFontDevMem, m_dataManager);
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
            m_tool->CreateDebugInstance(m_debugDevMem);
#endif
          }
          
          this->changeSeq(SEQ_WAIT_TOOL_CREATED);
          //breakしない
        }
        else
        {
          break;
        }
      }
      else  // if(m_param->caller!=ZukanProcParam::CALLER_ZUKAN_EVENT)
      {
        void* zukanRes2dDataBuf        = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_RES2D        );
        void* zukanMstxtDataBuf        = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_MSTXT        );
       
        // データマネージャ
        // レンダリングマネージャ
        if(
               zukanRes2dDataBuf      
            && zukanMstxtDataBuf      
            
            && m_appRenderingManager->IsCreated()
        )
        {
          ////////////////////////////////
          // ツールやフレームセルは他のものに依存しているので、後で生成、先に破棄。
          ////////////////////////////////
          // ツール生成
          {
            m_tool = GFL_NEW(m_implInstanceDevMem) ZukanTool(m_gameManager, m_appRenderingManager);
            // CALLER_ZUKAN_EVENTでなければ下記の生成
            // ツールの言語別MsgData、言語別Font
            // m_tool->CreateLangMsgDataManager
            // m_tool->CreateLangFontManager
            // はしなくてよい 
          }
          
          this->changeSeq(SEQ_WAIT_TOOL_CREATED);
          //breakしない
        }
        else
        {
          break;
        }
      }
    }
  case SEQ_WAIT_TOOL_CREATED:
    {
      if(m_param->caller==ZukanProcParam::CALLER_ZUKAN_EVENT)
      {
        void* zukanMstxtDataBuf        = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_MSTXT        );

        // ツール
        if(
               m_tool->CreateMenuCursor(m_commonAppHeap)
            && m_tool->CreatePokePersonalSort(m_commonAppHeap)
            && m_tool->CreateMsgSys(m_commonAppHeap)
        )
        {
          // コンテンツ
          {
            m_contents = GFL_NEW(m_contentsDevMem) ZukanContents(m_tool->GetPokeExtendData(), m_tool->GetPokePersonalSort());
            m_contents->InitAchievementDataBySaveData();
          }
       
          // ツールのBg
          {
            ZukanViewBg::Param param;
            param.param               = m_param;
            param.appHeap             = m_commonAppHeap;
            param.gameManager         = m_gameManager;
            param.dataManager         = m_dataManager;
            param.frameManager        = m_frameManager;
            param.appRenderingManager = m_appRenderingManager;
            param.lytSys              = m_lytSys;
            param.parentView          = m_view;
            m_tool->CreateBg(param);
          }
          // ツールのMsgData
          {
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_ZUKAN       , zukanMstxtDataBuf      , m_commonAppHeap->GetDeviceHeap());
          }
       
          // フレーム生成
          {
            m_frameCell = MENU_FRAME_CALL_MODAL<applib::frame::Manager, ZukanFrameCell>(m_frameManager);
            ZukanFrameCell::Param param;
            param.param               = m_param;
            //param.contentsDevMem      = m_contentsDevMem;
            param.instanceAppHeap     = m_instanceAppHeap;
            param.gameManager         = m_gameManager;
            param.dataManager         = m_dataManager;
            param.frameManager        = m_frameManager;
            param.appRenderingManager = m_appRenderingManager;
            param.rootView            = m_view;
            param.pokePicManager      = m_pokePicManager;
            param.tool                = m_tool;
            param.contents            = m_contents;
            param.setupState          = &m_setupState;
            param.pRotomToneGenerator = &m_RotomToneGenerator;
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
            param.debugInfo           = static_cast<ZukanDebugInfo*>(m_debugInfo);
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
            m_frameCell->Setup(param);

            m_frameCreated = true;
          }
 
          this->changeSeq(SEQ_END);
          //breakしない
        }
        else
        {
          break;
        }
      }
      else  // if(m_param->caller!=ZukanProcParam::CALLER_ZUKAN_EVENT)
      {
        void* zukanMstxtDataBuf        = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_MSTXT        );

        // ツール
        if(
               m_tool->CreateMenuCursor(m_commonAppHeap)
            && m_tool->CreatePokeModelSys(m_commonAppHeap, m_pokeModelCompDevMem)
            && m_tool->CreatePokePersonalSort(m_commonAppHeap)
            && m_tool->CreateMsgSys(m_commonAppHeap)
        )
        {
          // コンテンツ
          {
            m_contents = GFL_NEW(m_contentsDevMem) ZukanContents(m_tool->GetPokeExtendData(), m_tool->GetPokePersonalSort());
            //m_contents->InitAchievementDataBySaveData();  // 今捕まえたポケモンを反映してから呼び出したいので、ここでの呼び出しはコメントアウトする。
          }
      
          // コンテンツ
          {
            m_contents->SetPokeModelSys(m_tool->GetPokeModelSys());

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_PRINT_POKE_MODEL_ALL
            m_contents->DebugPrintPokeModelAll();
#endif

            if( m_param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT )
            {
              if( PokeTool::PokeGetUtil::DoesSetFlag(m_param->zukanRegisterEventParam->pp, m_param->zukanRegisterEventParam->callerType) )
              {
                PokeTool::PokeGetUtil::SetFlag(m_param->zukanRegisterEventParam->pp);
              }
              m_contents->CreateDataForRegister(m_contentsDevMem, m_param->zukanRegisterEventParam->pp);
            }
            else if( m_param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_LIST )
            {
              m_contents->CreateDataForRegister(m_contentsDevMem, m_param->zukanRegisterEventParam->pp);
            }
            else if( m_param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_STATUS )
            {
              m_contents->CreateDataForRegister(m_contentsDevMem, m_param->zukanRegisterEventParam->pp);
            }
            else  // if(m_param->caller == ZukanProcParam::CALLER_ZUKAN_EVENT)
            {
              m_contents->CreateData(m_contentsDevMem);
            }
            
            m_contents->InitAchievementDataBySaveData();  // 今捕まえたポケモンも数に含めたいので、この位置で呼び出す。  // @fix NMCat[1022] 完成していないのに王冠マークがついてしまう → 登録時のコンプリート判定を正しくするためにこのようにここも修正した。
          }

          // ツールのBg
          {
            ZukanViewBg::Param param;
            param.param               = m_param;
            param.appHeap             = m_commonAppHeap;
            param.gameManager         = m_gameManager;
            param.dataManager         = m_dataManager;
            param.frameManager        = m_frameManager;
            param.appRenderingManager = m_appRenderingManager;
            param.lytSys              = m_lytSys;
            param.parentView          = m_view;
            m_tool->CreateBg(param);
          }
          // ツールのMsgData
          {
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_ZUKAN       , zukanMstxtDataBuf      , m_commonAppHeap->GetDeviceHeap());
          }
       
          // フレーム生成
          {
            m_frameCell = MENU_FRAME_CALL_MODAL<applib::frame::Manager, ZukanFrameCell>(m_frameManager);
            ZukanFrameCell::Param param;
            param.param               = m_param;
            //param.contentsDevMem      = m_contentsDevMem;
            param.instanceAppHeap     = m_instanceAppHeap;
            param.gameManager         = m_gameManager;
            param.dataManager         = m_dataManager;
            param.frameManager        = m_frameManager;
            param.appRenderingManager = m_appRenderingManager;
            param.rootView            = m_view;
            param.pokePicManager      = m_pokePicManager;
            param.tool                = m_tool;
            param.contents            = m_contents;
            param.setupState          = &m_setupState;
            param.pRotomToneGenerator = &m_RotomToneGenerator;
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
            param.debugInfo           = static_cast<ZukanDebugInfo*>(m_debugInfo);
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
            m_frameCell->Setup(param);

            m_frameCreated = true;
          }
 
          this->changeSeq(SEQ_END);
          //breakしない
        }
        else
        {
          break;
        }
      }
    }
#else  // 必要最小限のものだけのセットアップにし、高速化をはかる(必要最小限のものだけにする前↓)
#endif  // 必要最小限のものだけのセットアップにし、高速化をはかる
  case SEQ_END:
  default:
    {
      m_setupState = ZukanType::SETUP_STATE_ALL_CREATING;
#if PM_DEBUG
      this->dumpMem();
#endif
      res = this->finishFunc();
      break;
    }
  }

  this->updateSys();

  return res;
}

gfl2::proc::Result ZukanImpl::UpdateFunc(void)
{
#if 1  // 必要最小限のものだけのセットアップにし、高速化をはかる
  if(m_param->caller==ZukanProcParam::CALLER_ZUKAN_EVENT)
  {
    if(m_setupState == ZukanType::SETUP_STATE_ALL_CREATING)
    {
      // データマネージャ
      void* zukanDistributionDataBuf = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_DISTRIBUTION );
      void* talkWindowRes2dDataBuf   = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_TALK_WINDOW_RES2D  );
      void* menuWindowRes2dDataBuf   = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_MENU_WINDOW_RES2D  );
      void* pokeSpeciesMstxtDataBuf  = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_POKE_SPECIES_MSTXT );
      void* pokeFormMstxtDataBuf     = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_POKE_FORM_MSTXT    );
      void* pokeHeightMstxtDataBuf   = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_POKE_HEIGHT_MSTXT  );
      void* pokeWeightMstxtDataBuf   = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_POKE_WEIGHT_MSTXT  );
      void* pokeCommentMstxtDataBuf  = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_POKE_COMMENT_MSTXT );
      void* initialMstxtDataBuf      = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_INITIAL_MSTXT      );
      void* zukanHyoukaMstxtDataBuf  = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_HYOUKA_MSTXT );
      if(
             zukanDistributionDataBuf      
          && talkWindowRes2dDataBuf 
          && menuWindowRes2dDataBuf
          && pokeSpeciesMstxtDataBuf   
          && pokeFormMstxtDataBuf   
          && pokeHeightMstxtDataBuf 
          && pokeWeightMstxtDataBuf 
          && pokeCommentMstxtDataBuf
          && initialMstxtDataBuf 
          && zukanHyoukaMstxtDataBuf 
               
          && m_pokePicManager->CreatingManager()
      )
      {
        // ツール
        if(
               m_tool->CreateMessageMenuView(m_commonAppHeap, menuWindowRes2dDataBuf)
            && m_tool->CreatePokeModelSys(m_commonAppHeap, m_pokeModelCompDevMem)
            && m_tool->CreateFont(m_commonAppHeap, ZukanTool::FONT_TYPE_NUM_FONT)
            && m_tool->CreateCommonGrpIcon(m_commonAppHeap)
        )
        {
          // 図鑑の絵巻のポケモン画像のマネージャ
          {
            m_pokePicManager->SetPokePicAccessorBin(m_dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_POKE_PIC_ACCESSOR));
          }

          // ツールのRegisterFade
          {
            ZukanViewRegisterFade::Param param;
            param.param               = m_param;
            param.appHeap             = m_commonAppHeap;
            param.gameManager         = m_gameManager;
            param.dataManager         = m_dataManager;
            param.frameManager        = m_frameManager;
            param.appRenderingManager = m_appRenderingManager;
            param.lytSys              = m_lytSys;
            param.parentView          = m_view;
            m_tool->CreateRegisterFade(param);
          }
          // ツールのMsgData
          {
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_POKE_SPECIES, pokeSpeciesMstxtDataBuf, m_commonAppHeap->GetDeviceHeap());
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_POKE_FORM   , pokeFormMstxtDataBuf   , m_commonAppHeap->GetDeviceHeap());
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_POKE_HEIGHT , pokeHeightMstxtDataBuf , m_commonAppHeap->GetDeviceHeap());
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_POKE_WEIGHT , pokeWeightMstxtDataBuf , m_commonAppHeap->GetDeviceHeap());
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_POKE_COMMENT, pokeCommentMstxtDataBuf, m_commonAppHeap->GetDeviceHeap());
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_INITIAL     , initialMstxtDataBuf    , m_commonAppHeap->GetDeviceHeap());
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_ZUKAN_HYOUKA, zukanHyoukaMstxtDataBuf, m_commonAppHeap->GetDeviceHeap());
          }
 
          // コンテンツ
          {
            m_contents->SetPokeModelSys(m_tool->GetPokeModelSys());

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_PRINT_POKE_MODEL_ALL
            m_contents->DebugPrintPokeModelAll();
#endif

            if( m_param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT )
            {
              if( PokeTool::PokeGetUtil::DoesSetFlag(m_param->zukanRegisterEventParam->pp, m_param->zukanRegisterEventParam->callerType) )
              {
                PokeTool::PokeGetUtil::SetFlag(m_param->zukanRegisterEventParam->pp);
              }
              m_contents->CreateDataForRegister(m_contentsDevMem, m_param->zukanRegisterEventParam->pp);
            }
            else if( m_param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_LIST )
            {
              m_contents->CreateDataForRegister(m_contentsDevMem, m_param->zukanRegisterEventParam->pp);
            }
            else if( m_param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_STATUS )
            {
              m_contents->CreateDataForRegister(m_contentsDevMem, m_param->zukanRegisterEventParam->pp);
            }
            else  // if(m_param->caller == ZukanProcParam::CALLER_ZUKAN_EVENT)
            {
              m_contents->CreateData(m_contentsDevMem);  // 最初
            }
          }

          m_setupState = ZukanType::SETUP_STATE_ALL_CREATING_2;
        }
      }
    }
    else if(m_setupState == ZukanType::SETUP_STATE_ALL_CREATING_2)
    {
      if( m_contents->CreateData(m_contentsDevMem) )  // ２回目以降
      {
        m_setupState = ZukanType::SETUP_STATE_ALL_FINISHED;
      }
    }
    else if(m_setupState == ZukanType::SETUP_STATE_ALL_FINISHED)
    {
      ZukanTool::LangMsgDataManager* langMsgDataManager = m_tool->GetLangMsgDataManager();
      if(!langMsgDataManager->IsCreated())
      {
        langMsgDataManager->UpdateCreate();
        langMsgDataManager->UpdateCreate();  // 1フレームに2個進める
      }
      else
      {
        ZukanTool::LangFontManager* langFontManager = m_tool->GetLangFontManager();
        if(!langFontManager->IsCreated())
        {
          langFontManager->UpdateCreate();
        }
      }
    }
  }
  else  // if(m_param->caller!=ZukanProcParam::CALLER_ZUKAN_EVENT)
  {
    if(m_setupState == ZukanType::SETUP_STATE_ALL_CREATING)
    {
      // データマネージャ
      void* zukanGrpFontRes2dDataBuf = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_GRPFONT_RES2D);
      void* talkWindowRes2dDataBuf   = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_TALK_WINDOW_RES2D  );
      void* menuWindowRes2dDataBuf   = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_MENU_WINDOW_RES2D  );
      void* pokeSpeciesMstxtDataBuf  = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_POKE_SPECIES_MSTXT );
      void* pokeFormMstxtDataBuf     = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_POKE_FORM_MSTXT    );
      void* pokeHeightMstxtDataBuf   = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_POKE_HEIGHT_MSTXT  );
      void* pokeWeightMstxtDataBuf   = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_POKE_WEIGHT_MSTXT  );
      void* pokeCommentMstxtDataBuf  = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_POKE_COMMENT_MSTXT );
      void* zukanHyoukaMstxtDataBuf  = m_dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_HYOUKA_MSTXT );
      if(
             zukanGrpFontRes2dDataBuf      
          && talkWindowRes2dDataBuf 
          && menuWindowRes2dDataBuf
          && pokeSpeciesMstxtDataBuf   
          && pokeFormMstxtDataBuf   
          && pokeHeightMstxtDataBuf 
          && pokeWeightMstxtDataBuf 
          && pokeCommentMstxtDataBuf
          && zukanHyoukaMstxtDataBuf 
               
          && m_pokePicManager->CreatingManager()
      )
      {
        // ツール
        if(
               m_tool->CreateMessageMenuView(m_commonAppHeap, menuWindowRes2dDataBuf)
            && m_tool->CreatePokeModelSys(m_commonAppHeap, m_pokeModelCompDevMem)
            && m_tool->CreateFont(m_commonAppHeap, ZukanTool::FONT_TYPE_NUM_FONT)
            && m_tool->CreateCommonGrpIcon(m_commonAppHeap)
        )
        {
          // 図鑑の絵巻のポケモン画像のマネージャ
          {
            m_pokePicManager->SetPokePicAccessorBin(m_dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_POKE_PIC_ACCESSOR));
          }

          // ツールのRegisterFade
          {
            ZukanViewRegisterFade::Param param;
            param.param               = m_param;
            param.appHeap             = m_commonAppHeap;
            param.gameManager         = m_gameManager;
            param.dataManager         = m_dataManager;
            param.frameManager        = m_frameManager;
            param.appRenderingManager = m_appRenderingManager;
            param.lytSys              = m_lytSys;
            param.parentView          = m_view;
            m_tool->CreateRegisterFade(param);
          }
          // ツールのMsgData
          {
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_POKE_SPECIES, pokeSpeciesMstxtDataBuf, m_commonAppHeap->GetDeviceHeap());
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_POKE_FORM   , pokeFormMstxtDataBuf   , m_commonAppHeap->GetDeviceHeap());
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_POKE_HEIGHT , pokeHeightMstxtDataBuf , m_commonAppHeap->GetDeviceHeap());
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_POKE_WEIGHT , pokeWeightMstxtDataBuf , m_commonAppHeap->GetDeviceHeap());
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_POKE_COMMENT, pokeCommentMstxtDataBuf, m_commonAppHeap->GetDeviceHeap());
            m_tool->CreateMsgData(ZukanTool::MSG_TYPE_ZUKAN_HYOUKA, zukanHyoukaMstxtDataBuf, m_commonAppHeap->GetDeviceHeap());
          }
 
          m_setupState = ZukanType::SETUP_STATE_ALL_FINISHED;
        }
      }
    }
  }
#endif  // 必要最小限のものだけのセットアップにし、高速化をはかる

#if PM_DEBUG
  gfl2::ui::DeviceManager* uiDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  const gfl2::ui::Button*  button = uiDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( button->IsTrigger(gfl2::ui::BUTTON_L) && button->IsTrigger(gfl2::ui::BUTTON_R) )
  {
    this->dumpMem();
  }
#endif
  
  gfl2::proc::Result res = gfl2::proc::RES_CONTINUE;

  // ビュー
  if(m_view) m_view->UpdateTree();
  if(m_tool)
  {
    app::ui::UIView* viewRegisterFade = m_tool->GetViewRegisterFade();
    if(viewRegisterFade) viewRegisterFade->UpdateTree();
  }

  this->updateSys();

  if(m_frameReqDestroy)
  {
    if(m_setupState == ZukanType::SETUP_STATE_ALL_FINISHED)
    {
      res = this->finishFunc();
    }
  }

  /*  rotom tone  */
  m_RotomToneGenerator.Update();

  return res;
}

void ZukanImpl::PreDrawFunc(void)
{
  // レンダリングマネージャ
  if(m_appRenderingManager) m_appRenderingManager->PreDraw();
}

void ZukanImpl::DrawFunc(gfl2::gfx::CtrDisplayNo displayNo)
{
  // ビュー
  if(m_view) m_view->DrawTree(displayNo);
  if(m_tool)
  {
    app::ui::UIView* viewRegisterFade = m_tool->GetViewRegisterFade();
    if(viewRegisterFade) viewRegisterFade->DrawTree(displayNo);
  }

  // フレームマネージャ
  if(m_frameManager) m_frameManager->Draw(displayNo);
  
   // レンダリングマネージャ
  if(m_appRenderingManager) m_appRenderingManager->Draw(displayNo);
}

gfl2::proc::Result ZukanImpl::EndFunc(void)
{
  enum
  {
    SEQ_WAIT_NOT_DRAWING,
    SEQ_START_TOOL_DESTROY,
    SEQ_WAIT_TOOL_DESTROYED,
    SEQ_WAIT_DESTROYED,
    SEQ_WAIT_DATA_MANAGER_DESTROYED,
    SEQ_LAST_ONE,  // 最後の1フレームだけ
    SEQ_END,
  };
      
  gfl2::proc::Result res = gfl2::proc::RES_CONTINUE;

  // シーケンス
  switch(m_seq)
  {
  case SEQ_WAIT_NOT_DRAWING:
    {
      // 1フレーム描画しない
      m_frameDestroying = true;
      this->changeSeq(SEQ_START_TOOL_DESTROY);
      break;
    }
  case SEQ_START_TOOL_DESTROY:
    {
      ////////////////////////////////
      // ツールやフレームセルは他のものに依存しているので、後で生成、先に破棄。
      ////////////////////////////////
      if(m_tool)
      {
        // ツールのMsgData
        m_tool->DestroyMsgData(ZukanTool::MSG_TYPE_ZUKAN       );
        //m_tool->DestroyMsgData(ZukanTool::MSG_TYPE_POKE_NAME   );
        m_tool->DestroyMsgData(ZukanTool::MSG_TYPE_POKE_SPECIES);
        m_tool->DestroyMsgData(ZukanTool::MSG_TYPE_POKE_FORM   );
        m_tool->DestroyMsgData(ZukanTool::MSG_TYPE_POKE_HEIGHT );
        m_tool->DestroyMsgData(ZukanTool::MSG_TYPE_POKE_WEIGHT );
        m_tool->DestroyMsgData(ZukanTool::MSG_TYPE_POKE_COMMENT);
        m_tool->DestroyMsgData(ZukanTool::MSG_TYPE_INITIAL     );
        m_tool->DestroyMsgData(ZukanTool::MSG_TYPE_ZUKAN_HYOUKA);
      
        // ツールのRegisterFade
        m_tool->DestroyRegisterFade();
        // ツールのBg
        m_tool->DestroyBg();
      }

      this->changeSeq(SEQ_WAIT_TOOL_DESTROYED);
      //breakしない
    }
  case SEQ_WAIT_TOOL_DESTROYED:
    {
      bool canChangeSeq = true;

      // コンテンツ破棄
      if(m_contents)
      {
        GFL_SAFE_DELETE(m_contents);
      }
     
      // ツール破棄
      if(m_tool)
      {
        if(
               m_tool->DestroyCommonGrpIcon()
            && m_tool->DestroyFont(ZukanTool::FONT_TYPE_NUM_FONT)
            //&& m_tool->DestroyMultiButtonsView()
            //&& m_tool->DestroyTwoButtonsView()
            && m_tool->DestroyMsgSys()
            && m_tool->DestroyPokePersonalSort()
            //&& m_tool->DestroyPokeExtendData()
            && m_tool->DestroyPokeModelSys()
            && m_tool->DestroyMessageMenuView()
            && m_tool->DestroyMenuCursor()
            //&& m_tool->DestroyTalkWindow()
        )
        {
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
          m_tool->DestroyDebugInstance();
#endif
          // ツールの言語別MsgData、言語別Font
          m_tool->DestroyLangFontManager();
          m_tool->DestroyLangMsgDataManager();

          GFL_SAFE_DELETE(m_tool);
        }
        else
        {
          canChangeSeq = false;
        }
      }

      if(canChangeSeq)
      {
        this->changeSeq(SEQ_WAIT_DESTROYED);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_WAIT_DESTROYED:
    {
      bool canChangeSeq = true;
      
      // ビュー破棄
      if(m_view)
      {
        GFL_SAFE_DELETE(m_view);
      }

      // レイアウト破棄
      if(m_lytSys)
      {
        GFL_SAFE_DELETE(m_lytSys);
      }

      // レンダリングマネージャ
      if(m_appRenderingManager)
      {
        if(m_appRenderingManager->CanDestroy())
        {
          GFL_SAFE_DELETE(m_appRenderingManager);
        }
        else
        {
          canChangeSeq = false;
        }
      }

      // フレームマネージャ破棄
      if(m_frameManager)
      {
        if(m_frameManager->End() == false)
        {
          GFL_SAFE_DELETE(m_frameManager);
          GFL_SAFE_DELETE(m_frameListener);
        }
        else
        {
          canChangeSeq = false;
        }
      }

      if(m_pokePicManager)
      {
        if(m_pokePicManager->DestroyingManager())
        {
          GFL_SAFE_DELETE(m_pokePicManager);
        }
        else
        {
          canChangeSeq = false;
        }
      }

      if(canChangeSeq)
      {
        // データマネージャ
        m_dataManager->Stop();
        m_dataManager->End();
        this->changeSeq(SEQ_WAIT_DATA_MANAGER_DESTROYED);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_WAIT_DATA_MANAGER_DESTROYED:
    {
      // データマネージャ
      if(m_dataManager->CanDestroy())
      {
        GFL_SAFE_DELETE(m_dataManager);
        this->changeSeq(SEQ_LAST_ONE);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_LAST_ONE:
    {
#if PM_DEBUG
      GFL_SAFE_DELETE(m_zukanSaveDataDebugWin);
      GFL_SAFE_DELETE(m_zukanDebugWin);
#endif
      this->changeSeq(SEQ_END);
      //breakしない
    }
  case SEQ_END:
  default:
    {
      res = this->finishFunc();
      break;
    }
  }

  this->updateSys();

  return res;
}

void ZukanImpl::updateSys(void)
{
  // データマネージャ
  if(m_dataManager) m_dataManager->Update();

  // 図鑑の絵巻のポケモン画像のマネージャ
  if(m_pokePicManager) m_pokePicManager->Update();

  // ツール
  if(m_tool) m_tool->Update();

  // フレーム
  applib::frame::MainStatus frameStatus = applib::frame::MAINSTATUS_NULL;
  if(  m_frameManager  &&  m_frameDestroying == false  ) frameStatus = m_frameManager->Main();
  if(  m_frameCreated  &&  frameStatus == applib::frame::MAINSTATUS_NULL  ) m_frameReqDestroy = true;

  // レンダリングマネージャ
  if(m_appRenderingManager) m_appRenderingManager->Update();
}

void ZukanImpl::changeSeq(s32 seq)
{
  m_seq      = seq;
  m_seqCount = 0;
}

gfl2::proc::Result ZukanImpl::finishFunc(void)
{
  m_seq      = 0;
  m_seqCount = 0;
  return gfl2::proc::RES_FINISH;
}

#if PM_DEBUG
void ZukanImpl::dumpMem(void)
{
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_PRINT_DUMP_MEM

  GFL_PRINT("ZukanImpl::dumpMem Start ================================\n");

  {
    gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
    if(parentDevMem)
    {
      GFL_PRINT("HEAPID_APP_DEVICE ================================\n");
      parentDevMem->Dump();
    }
  }

  if(m_appHeap)
  {
    GFL_PRINT("IMPL_HEAP ================================\n");
    m_appHeap->GetSystemHeap()->Dump();
    GFL_PRINT("IMPL_DEV ================================\n");
    m_appHeap->GetDeviceHeap()->Dump();
  }

  if(m_implInstanceDevMem)
  {
    GFL_PRINT("IMPL_INSTANCE_DEV ================================\n");
    m_implInstanceDevMem->Dump();
  }
  if(m_dataDevMem)
  {
    GFL_PRINT("DATA_DEV ================================\n");
    m_dataDevMem->Dump();
  }
  if(m_frameManagerDevMem)
  {
    GFL_PRINT("FRAME_MANAGER_DEV ================================\n");
    m_frameManagerDevMem->Dump();
  }
  if(m_vramAllocatorDevMem)
  {
    GFL_PRINT("VRAM_ALLOCATOR_DEV ================================\n");
    m_vramAllocatorDevMem->Dump();
  }
  if(m_contentsDevMem)
  {
    GFL_PRINT("CONTENTS_DEV ================================\n");
    m_contentsDevMem->Dump();
  }

  if(m_commonAppHeap)
  {
    GFL_PRINT("COMMON_HEAP ================================\n");
    m_commonAppHeap->GetSystemHeap()->Dump();
    GFL_PRINT("COMMON_DEV ================================\n");
    m_commonAppHeap->GetDeviceHeap()->Dump();
  }

  if(m_pokeModelCompDevMem)
  {
    GFL_PRINT("POKEMODEL_COMP_DEV ================================\n");
    m_pokeModelCompDevMem->Dump();
  }

  if(m_langMsgDataDevMem)
  {
    GFL_PRINT("LANG_MSG_DATA_DEV ================================\n");
    m_langMsgDataDevMem->Dump();
  }

  if(m_langFontDevMem)
  {
    GFL_PRINT("LANG_FONT_DEV ================================\n");
    m_langFontDevMem->Dump();
  }

  if(m_instanceAppHeap)
  {
    GFL_PRINT("INSTANCE_HEAP ================================\n");
    m_instanceAppHeap->GetSystemHeap()->Dump();
    GFL_PRINT("INSTANCE_DEV ================================\n");
    m_instanceAppHeap->GetDeviceHeap()->Dump();
  }

  if(m_debugDevMem)
  {
    GFL_PRINT("DEBUG_DEV ================================\n");
    m_debugDevMem->Dump();
  }
  GFL_PRINT("ZukanImpl::dumpMem End ================================\n");

#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_PRINT_DUMP_MEM
}
#endif


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

