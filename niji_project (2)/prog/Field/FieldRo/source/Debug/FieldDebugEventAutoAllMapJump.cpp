//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugEventAutoAllMapJump.cpp
 *	@brief  自動全マップジャンプ
 *	@author	tomoya takahashi
 *	@date		2015.06.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#if PM_DEBUG

#include <thread/include/gfl2_ThreadStatic.h>
#include <core/include/gfl2_Utility.h>


#include "FieldDebugEventAutoAllMapJump.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "System/include/HeapDefine.h"

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldRo/include/Debug/Menu/FieldDebugMenuTerrain.h"
#include <niji_conv_header/field/world/world_id.h>
#include <niji_conv_header/field/zone/zone_id.h>
#include "Field/FieldRo/include/Event/FieldEventDebugTerrainCheck.h"
#include "FieldScript/include/FieldScriptSystem.h"

#include "NetStatic/NetEvent/include/JoinFestaOpenEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaCloseEvent.h"

#include "Field/FieldRo/include/Fieldmap.h"

#include "Field/FieldStatic/include/Debug/EventDebugAppReturn.h"

#include <fs/include/gfl2_FsPcUtil.h>

// ▼モジュール読込用で追加
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "niji_conv_header/field/FieldGimmickTypes.h"

GFL_NAMESPACE_BEGIN(Field);

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//Heap名称定義
#define HEAP_DEFINE(str) L ## #str,
static const wchar_t* const sc_HeapNameArr[HEAPID_CHILD_MAX] =
{
  L"HEAPID_SYSTEM",
  #include "System/include/HeapDefine.cdat"
};

struct HEAP_INFO
{
  // c8* pName;
  s32 heapId;
};

// 出力対象ヒープ
static const HEAP_INFO sc_TargetHeapTable[] =
{
  HEAPID_DEVICE,
  HEAPID_RESIDENT,
  HEAPID_RESIDENT_DEVICE,
  HEAPID_APP,
  HEAPID_APP_DEVICE,
  HEAPID_DLL_LOAD,
  HEAPID_EVENT,
  HEAPID_EVENT_DEVICE,
  HEAPID_FIELD,
  HEAPID_FIELD_DEVICE,
  HEAPID_FIELD_SUBSCREEN,
  HEAPID_FIELD_SUBSCREEN_DEVICE,
  HEAPID_FIELD_SCRIPT,
  HEAPID_FIELD_SCRIPT_FILEREAD,
  HEAPID_FIELD_SCRIPT_WORK
};


struct LOCAL_HEAP_INFO
{
  c8* pName;    // ローカルヒープ名
  s32 num;      // ローカルヒープの想定個数(これより多い場合は出力されない)
};

// 出力対象ローカルヒープ
static const LOCAL_HEAP_INFO sc_LocalHeapInfoTable[] =
{
  "ScriptArc", 1,
  "ScriptZoneAmx", 1,
  "ScriptGeneralAmx", 1,
  "ScriptNameList", 1,
  "ScriptTrialModel", 1,
  "FieldCameraSubController", 1,
  "FieldCameraAreaController", 1,
  "FieldCameraScriptEventController", 1,
  "FieldEffectWtr", 6,
  "FieldEffect", 38,
  "FieldActor", 1,
  "StaticModelTotal", 1,
  "StaticModelStatic", 1,
  "StaticModelDynamic", 1,
  "FieldSubWindowCall", 1,
  "FieldTerrainHighBlock", 4,
  "LodListener", 1,
  "FieldTrafficManager", 1,
  "FieldTrafficPath", 1,
  "EncountEffect", 1,
  "FieldRegident", 1,
  "DressModelResource", 1,
  "MoveModelDynamicRide", 1,
  "MoveModelDynamicSubObject", 1,
  "MoveModelDynamicDummy", 1,
  "PLAYER", 1,
  "RIDE", 1,
  "NPC", 30,
  "SUB", 10,
  "Area", 1,
  "BattlePokemon", 1,
  "JoinFesta", 1,
  "BattlePokemon", 1,
  "World", 1,
  "WorldZoneArea", 1,
};

class DebugEventAutoAllMapJump::HeapInfoSaver
{
public:

#if defined(GF_PLATFORM_WIN32)
#define HEAP_INFO_PUTS_TEXT( text, ... ) m_Start += sprintf_s( &m_Buffer[ m_Start ], sizeof( m_Buffer ) - m_Start, text, __VA_ARGS__ )
#elif defined(GF_PLATFORM_CTR)
#define HEAP_INFO_PUTS_TEXT( ... ) m_Start += sprintf( &m_Buffer[ m_Start ], __VA_ARGS__ )
#endif

  HeapInfoSaver( gfl2::heap::HeapBase* pHeap, Field::Debug::SymbolNameLoader* pZoneNameLoader ) : m_Start(0), m_pZoneNameLoader( pZoneNameLoader )
  {
    // m_pZoneNameLoader = GFL_NEW(pHeap) Field::Debug::SymbolNameLoader;
    // m_pZoneNameLoader->Load( pHeap, Field::Debug::SymbolNameLoader::SymbolType::ZONE_ID );
  }
  virtual ~HeapInfoSaver(void)
  {
    // GFL_SAFE_DELETE(m_pZoneNameLoader);
  }
  void WriteHeader( void )
  {
    m_Start = 0;
    
    HEAP_INFO_PUTS_TEXT(",");

    for (int i = 0; i < GFL_NELEMS(sc_TargetHeapTable); i++)
    {
      HEAP_INFO_PUTS_TEXT("%ls,,", sc_HeapNameArr[sc_TargetHeapTable[i].heapId]);
    }

    for (int i = 0; i < GFL_NELEMS(sc_LocalHeapInfoTable); i++)
    {
      for (int k = 0; k < sc_LocalHeapInfoTable[i].num; k++)
      {
        HEAP_INFO_PUTS_TEXT("%s,,", sc_LocalHeapInfoTable[i].pName );
      }
    }

    HEAP_INFO_PUTS_TEXT("\n");
    HEAP_INFO_PUTS_TEXT("ZONE_ID,");

    for (int i = 0; i < GFL_NELEMS(sc_TargetHeapTable); i++)
    {
      HEAP_INFO_PUTS_TEXT("Free,Max," );
    }

    for (int i = 0; i < GFL_NELEMS(sc_LocalHeapInfoTable); i++)
    {
      for (int k = 0; k < sc_LocalHeapInfoTable[i].num; k++)
      {
        HEAP_INFO_PUTS_TEXT("Free,Max," );
      }
    }
    
    HEAP_INFO_PUTS_TEXT("\n");
    WriteFile();
  }
  void WriteHeap( u32 zoneId )
  {
    c8* pZoneName;
    m_pZoneNameLoader->GetName(zoneId, &pZoneName);
    HEAP_INFO_PUTS_TEXT("%s,", pZoneName );

    // ヒープを出力
    for (int i = 0; i < GFL_NELEMS(sc_TargetHeapTable); i++)
    {
      if (gfl2::heap::Manager::IsExistHeapId( sc_TargetHeapTable[i].heapId ))
      {
        gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(sc_TargetHeapTable[i].heapId);
        const int size = pHeap->GetTotalSize();
        const int free = pHeap->GetTotalFreeSize();
        const int allocate = pHeap->GetTotalAllocatableSize();
        const bool isFragment = ((free - allocate)>gfl2::heap::MEMORY_HEADER_SIZE);

        HEAP_INFO_PUTS_TEXT("%d,%d,", free / 1024, size / 1024 );
      }
      else
      {
        HEAP_INFO_PUTS_TEXT(",,");
      }
    }

    // ローカルヒープを出力
    // ローカルヒープは同名ヒープが複数あるので、想定数分検索して出力する
    for (int i = 0; i < GFL_NELEMS(sc_LocalHeapInfoTable); i++)
    {
      int heapIndex = 0;
      u32 checkLen = ::std::strlen( sc_LocalHeapInfoTable[i].pName );

      for (int k = 0; k < sc_LocalHeapInfoTable[i].num; k++)
      {
        gfl2::heap::HeapBase* pHeap( NULL );

        for (; heapIndex < HEAPID_LOCAL_MAX; heapIndex++)
        {
          if (gfl2::heap::Manager::IsExistHeapId(GFL_HEAP_INDEX_TO_LOCAL_HEAP_ID(heapIndex)))
          {
            gfl2::heap::HeapBase* pHeapWork = gfl2::heap::Manager::GetHeapByHeapId(GFL_HEAP_INDEX_TO_LOCAL_HEAP_ID(heapIndex));

            if (gfl2::core::Utility::Strncmp(sc_LocalHeapInfoTable[i].pName, pHeapWork->GetHeapName(), checkLen) == 0)
            {
              pHeap = pHeapWork;
              break;
            }
          }
        }

        if (pHeap)
        {
          const int size = pHeap->GetTotalSize();
          const int free = pHeap->GetTotalFreeSize();
          const int allocate = pHeap->GetTotalAllocatableSize();
          const bool isFragment = ((free - allocate)>gfl2::heap::MEMORY_HEADER_SIZE);

          HEAP_INFO_PUTS_TEXT("%d,%d,", free / 1024, size / 1024 );
        }
        else
        {
          HEAP_INFO_PUTS_TEXT(",," );
        }
      }
    }

    HEAP_INFO_PUTS_TEXT("\n");
    WriteFileByAddMode();
  }



private:

  void WriteFile(void)
  {
    m_Buffer[m_Start] = '\0';
    gfl2::fs::PcUtil::WriteFile("R:/home/momiji/exec/direct_sdmc/heap_info.csv", m_Buffer, m_Start);
    m_Start = 0;
  }

  void WriteFileByAddMode(void)
  {
    m_Buffer[m_Start] = '\0';
    gfl2::fs::PcUtil::WriteFileByAddMode("R:/home/momiji/exec/direct_sdmc/heap_info.csv", m_Buffer, m_Start);
    m_Start = 0;
  }

  char m_Buffer[20480];
  u32 m_Start;

  Field::Debug::SymbolNameLoader* m_pZoneNameLoader;
};


//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
DebugEventAutoAllMapJump::DebugEventAutoAllMapJump( gfl2::heap::HeapBase* heap ) : 
  GameSys::GameEvent(heap)
{
  m_MinTotalFreeHeapSize = 0xFFFFFFF;
  m_MinTotalFreeHeapSizeZoneId = 0;
  m_MinAreaResourceFreeSize = 0xFFFFFFF;
  m_MinAreaResourceFreeSizeZoneId = 0;

  m_AppReturnCount = 0;
  m_MapJumpCount = 0;
  m_pHeapInfoSaver = NULL;

  m_pZoneNameLoader = GFL_NEW(heap) Field::Debug::SymbolNameLoader;
  m_pZoneNameLoader->Load(heap, Field::Debug::SymbolNameLoader::SymbolType::ZONE_ID);
  
  m_pAreaNameLoader = NULL;
  m_pWorldNameLoader = NULL;

  m_LowestFreeSizeZoneID  = ZONE_ID_INVALID;
  m_LowestFreeSize        = 0xFFFFFFF;
}

DebugEventAutoAllMapJump::~DebugEventAutoAllMapJump()
{
  GFL_SAFE_DELETE( m_pHeapInfoSaver );
  GFL_SAFE_DELETE(m_pZoneNameLoader);
  GFL_SAFE_DELETE(m_pAreaNameLoader);
  GFL_SAFE_DELETE(m_pWorldNameLoader);
}

//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool DebugEventAutoAllMapJump::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

void DebugEventAutoAllMapJump::SetDesc( const DebugEventAutoAllMapJump::InitializeDescription& rDesc )
{
  m_InitDesc = rDesc;
  m_ZoneID = m_InitDesc.m_StartZoneID;
  m_JumpType = JUMPTYPE_NORMAL;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void DebugEventAutoAllMapJump::InitFunc(GameSys::GameManager* gmgr)
{
  // イベントステータスを立てる。
  gmgr->GetGameData()->GetFieldCharaModelManager()->AddEventRequestAll();

  if (m_InitDesc.m_bBlockHeapInfoSave)
  {
    gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEBUGGER );
    
    m_pAreaNameLoader = GFL_NEW(heap) Field::Debug::SymbolNameLoader;
    m_pAreaNameLoader->Load(heap, Field::Debug::SymbolNameLoader::SymbolType::AREA_ID);

    m_pWorldNameLoader = GFL_NEW(heap) Field::Debug::SymbolNameLoader;
    m_pWorldNameLoader->Load(heap, Field::Debug::SymbolNameLoader::SymbolType::WORLD_ID);
  }

  // Heap情報をHIOでファイル出力する機能
  if (m_InitDesc.m_bHeapInfoSave )
  {
    bool flag = false;
    gfl2::fs::Result res = gfl2::fs::PcUtil::IsPcAccessEnableFlag(&flag);

    if (res.IsSuccess() && flag && gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_DEBUGGER ) )
    {
      gfl2::heap::HeapBase * pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEBUGGER );
      m_pHeapInfoSaver = GFL_NEW( pDebugHeap ) HeapInfoSaver( pDebugHeap, m_pZoneNameLoader );
      m_pHeapInfoSaver->WriteHeader();
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT DebugEventAutoAllMapJump::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_MAPJUMP_ZONE_CHECK,
    SEQ_MAPJUMP_CALL,
    SEQ_JFOPEN_CALL,
    SEQ_TERRAIN_CHECK,
    SEQ_MODULE_LOAD_CHECK,      // 2017/03/29 追加 バトル遷移時のシーケンスプレイヤーモジュール読込まで実行してメモリがあふれないかチェックする(モジュールを積んでテストするだけ)
    SEQ_APP_RETURN_CHECK,
    SEQ_APP_RETURN_CHECK_WAIT,
    SEQ_UPDATE,
    SEQ_INCREMENT,
    SEQ_END,
  };


  switch (this->GetSeqNo())
  {
  case SEQ_MAPJUMP_ZONE_CHECK:
  {
    // BCancelチェック
    if (m_InitDesc.m_bBCancel)
    {
      //Bを押していたら終了
      if (GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD)->IsHold(gfl2::ui::BUTTON_B))
      {
        this->SetSeqNo(SEQ_END);
        break;
      }
    }


    ZoneDataLoader* pZoneData = gmgr->GetGameData()->GetFieldZoneDataLoader();
    if (pZoneData->GetZoneNum() > static_cast<u32>(m_ZoneID))
    {
      m_JumpType = IsJump(gmgr, m_ZoneID);
      switch (m_JumpType)
      {
      case JUMPTYPE_SKIP:    // スキップ
        // 次をチェック
        switch (m_InitDesc.m_Mode)
        {
        case MODE_RANDOM:
        case MODE_APP_RETURN:
        {
          ZoneDataLoader* pZoneData = gmgr->GetGameData()->GetFieldZoneDataLoader();
          m_ZoneID = System::GflUse::GetPublicRand(pZoneData->GetZoneNum());

          if ((m_ZoneID == Z_F0601JF0101) || (m_ZoneID == Z_F0601JF0102) || (m_ZoneID == Z_F0601JF0103))
          {
            m_ZoneID = System::GflUse::GetPublicRand(Z_F0601JF0101);  // RANDOMジャンプでは、JFを回避
          }
        }
        break;

        default:
          m_ZoneID++;
          break;
        }
        break;
      case JUMPTYPE_NORMAL:  // ノーマル
        m_MapJumpCount++;
        this->SetSeqNo(SEQ_MAPJUMP_CALL);
        break;
      case JUMPTYPE_JF:      // JFジャンプ
        m_MapJumpCount++;
        this->SetSeqNo(SEQ_JFOPEN_CALL);
        break;

      default:
        GFL_ASSERT(0);
        break;
      }
    }
    else
    {
      // 終了
      this->SetSeqNo(SEQ_END);
    }
    break;
  }

  case SEQ_MAPJUMP_CALL:
  {
    c8* pZoneName;
    if (m_pZoneNameLoader->GetName(m_ZoneID, &pZoneName))
    {
      GFL_PRINT("AutoMapJump::ZoneID[%s](%d) Start\n", pZoneName, m_ZoneID);
    }
    else
    {
      GFL_PRINT("AutoMapJump::ZoneID[error](%d) Start\n", m_ZoneID);
    }
    EventMapJump::CallMapJumpDefaultPos(gmgr, m_ZoneID);
    m_CountUp = 0;
    if (m_InitDesc.m_Mode != MODE_APP_RETURN)
    {
      this->SetSeqNo(SEQ_TERRAIN_CHECK);
    }
    else
    {
      this->SetSeqNo(SEQ_APP_RETURN_CHECK);
    }
    break;
  }
  case SEQ_JFOPEN_CALL:
  {
    c8* pZoneName;
    if (m_pZoneNameLoader->GetName(m_ZoneID, &pZoneName))
    {
      GFL_PRINT("AutoMapJump::ZoneID[%s](%d) Start\n", pZoneName, m_ZoneID);
    }
    else
    {
      GFL_PRINT("AutoMapJump::ZoneID[error](%d) Start\n", m_ZoneID);
    }

    NetEvent::JoinFestaOpenEvent::StartEvent(gmgr, true);
    m_CountUp = 0;
    this->SetSeqNo(SEQ_TERRAIN_CHECK);
    break;
  }
  case SEQ_TERRAIN_CHECK:
    {
      c8* pZoneName;
      if (m_pZoneNameLoader->GetName(m_ZoneID, &pZoneName))
      {
        GFL_PRINT("AutoMapJump::ZoneID[%s](%d) Terrain Check Start\n", pZoneName, m_ZoneID);
      }
      else
      {
        GFL_PRINT("AutoMapJump::ZoneID[error](%d) Terrain Check Start\n", m_ZoneID);
      }

      Event::EventDebugTerrainCheck* pEvent = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, Event::EventDebugTerrainCheck >( gmgr->GetGameEventManager() );
      Event::EventDebugTerrainCheck::InitializeDescription initDesc;
      initDesc.m_pHeap = NULL;
      initDesc.dummy = 0;
      initDesc.m_pAreaNameLoader = m_pAreaNameLoader;
      initDesc.m_pWorldNameLoader = m_pWorldNameLoader;
      initDesc.m_isHeapInfoSave = false;

      if (m_InitDesc.m_bBlockHeapInfoSave)
      {
        ZoneDataLoader* pZoneData = gmgr->GetGameData()->GetFieldZoneDataLoader();
        u16 world_id = pZoneData->GetZoneWorldId( m_ZoneID );

        gfl2::heap::HeapBase* pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);
        World* pWorld = GFL_NEW(pDebugHeap) World(HEAPID_DEBUG);
        pWorld->Initialize(world_id, gmgr->GetAsyncFileManager());
        while (pWorld->InitializeWait() == false)
        {
          gfl2::thread::ThreadStatic::CurrentSleep(1000);
        }
         
        initDesc.m_WorldId = world_id;
        initDesc.m_AreaId = pWorld->GetIDZoneInfo(m_ZoneID).m_AreaID;
        GFL_SAFE_DELETE(pWorld);

        initDesc.m_isHeapInfoSave = true;
      }

      pEvent->Initialize( initDesc );

      // DEBUG/DEVELOP版の処理挙動を変えないための対応
#ifdef GF_CONFIG_RELEASE
      // リリース版のみモジュール読込テストを挟む
      this->SetSeqNo(SEQ_MODULE_LOAD_CHECK);
#else
      // リリース版ではない場合は従来通りに処理を続行
      this->SetSeqNo(SEQ_UPDATE);
#endif
      break;
    }

  case SEQ_MODULE_LOAD_CHECK:
    {
      GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
      gfl2::ro::RoManager* pRoManager = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager);
      gfl2::heap::HeapBase* pDllLoad = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD);


      bool isSkip = false;

      // ポケモンセンターかどうかチェック
      void* pZoneData = pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneData(m_ZoneID);
      ZoneDataAccessor zoneDataAccessor(pZoneData);
      u32 gimmickID = zoneDataAccessor.GetGimmickID();
      if (gimmickID == GIMMICK_TYPE_POKECEN)
      {
        // バトルを行わないポケモンセンターはスキップ
        if (m_ZoneID != Z_F0301PC0401 && m_ZoneID != Z_F0401PC0101)
        {
          isSkip = true;
        }
          
      }
      // スキップ対象のゾーンかチェック
      switch (m_ZoneID)
      {
      case Z_F0301PL0601EV01:     // ┬エンディングでのみ使用されるゾーン
      case Z_F0301D1101EV01:      // ┤
      case Z_F0301I1201EV01:      // ┤
      case Z_F0401D0602EV01:      // ┤
      case Z_F0501I0201EV01:      // ┤
      case Z_F0102I0501EV01:      // ┤
      case Z_F0102EV02_C0103:     // ┤
      case Z_F0102D0102EV01:      // ┤
      case Z_F0102EV02_T0101:     // ┤
      case Z_F0102D0203EV01:      // ┤
      case Z_F0102EV05_S0101:     // ┘
      case Z_F0601JF0101:         // ┬ジョインフェスタ
      case Z_F0601JF0102:         // ┤
      case Z_F0601JF0103:         // ┘
        isSkip = true;
        break;
      }
      // スキップフラグが立っている場合はスキップ
      if (isSkip)
      {
        this->SetSeqNo(SEQ_UPDATE);
        break;
      }

      c8* pZoneName;
      if (m_pZoneNameLoader->GetName(m_ZoneID, &pZoneName))
      {
        GFL_RELEASE_PRINT("AutoMapJump::ZoneID[%s](%d) Module Load Check Start\n", pZoneName, m_ZoneID);
      }
      else
      {
        GFL_RELEASE_PRINT("AutoMapJump::ZoneID[error](%d) Module Load Check Start\n", m_ZoneID);
      }
      
      struct DllData
      {
        bool isLow; // メモリの後方確保を行なうか
        char* dllName;
      };

      // Roモジュール
      static const int RO_MODULE_NUM = 2;
      const DllData RO_MODULES[RO_MODULE_NUM] =
      {
        { true, "Object.cro" },
        { true, "SequencePlayer.cro" },
      };
      gfl2::ro::Module* pModules[RO_MODULE_NUM];

      // モジュールを全て読み込む
      for (u32 i = 0; i < RO_MODULE_NUM; ++i)
      {
        GFL_RELEASE_PRINT("Load Module Start [%s]\n", RO_MODULES[i].dllName);
        if (RO_MODULES[i].isLow)
        {
          pModules[i] = pRoManager->LoadModuleLower(pGameManager->GetAsyncFileManager(), RO_MODULES[i].dllName, pDllLoad);
        }
        else{
          pModules[i] = pRoManager->LoadModule(pGameManager->GetAsyncFileManager(), RO_MODULES[i].dllName, pDllLoad);
        }
        pRoManager->StartModule(pModules[i], true);
        GFL_RELEASE_PRINT("Load Module End [%s]\n", RO_MODULES[i].dllName);
        pDllLoad->Dump();
      }
      GFL_RELEASE_PRINT("Module loading completed.\n");
      GFL_RELEASE_PRINT("└free[0x%x] allocatable[0x%x]\n", pDllLoad->GetTotalFreeSize(), pDllLoad->GetTotalAllocatableSize());

      // メモリサイズが現在記録されている最低サイズを下回っているか確認
      if (m_LowestFreeSize > pDllLoad->GetTotalFreeSize())
      {
        // 最低サイズ更新
        m_LowestFreeSizeZoneID = m_ZoneID;
        m_LowestFreeSize = pDllLoad->GetTotalFreeSize();
      }

      for (u32 i = 0; i < RO_MODULE_NUM; ++i)
      {
        pRoManager->DisposeModule(pModules[i]);
      }
      GFL_RELEASE_PRINT("DisposeModule end\n");

      
      
      this->SetSeqNo( SEQ_UPDATE );
      break;
    }

  // アプリケーションジャンプを繰り返す処理
  case SEQ_APP_RETURN_CHECK:
    {
      // 繰り返し条件
      if( m_AppReturnCount < 20 )
      {
        m_AppReturnType = System::GflUse::GetPublicRand( APPRETURN_MAX );
        
        // アプリジャンプ制御
        DebugAppReturn* p_event;
        p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, DebugAppReturn > (
              gmgr->GetGameEventManager( ) );

        switch( m_AppReturnType )
        {
        case APPRETURN_KEEP:
          {
            p_event->SetMode( DebugAppReturn::MODE_APP_KEEP );
          }
          break;
        case APPRETURN_NORMAL:
          {
            p_event->SetMode( DebugAppReturn::MODE_APP );
          }
          break;
        case APPRETURN_JF:
          {
            p_event->SetMode( DebugAppReturn::MODE_JF );
          }
          break;
        }

        m_AppReturnCount ++;

        this->SetSeqNo( SEQ_APP_RETURN_CHECK_WAIT );
      }
      // 終了
      else
      {
        m_AppReturnCount = 0;
        this->SetSeqNo( SEQ_UPDATE );
      }
    }
    break;

  case SEQ_APP_RETURN_CHECK_WAIT:
    {
      this->SetSeqNo( SEQ_APP_RETURN_CHECK );
    }
    break;


  // ゾーン更新処理
  case SEQ_UPDATE:
    if( m_CountUp == 0 ){

      c8* pZoneName;
      if (m_pZoneNameLoader->GetName(m_ZoneID, &pZoneName))
      {
        GFL_PRINT("AutoMapJump::ZoneID[%s](%d) Finish\n", pZoneName, m_ZoneID);
      }
      else
      {
        GFL_PRINT("AutoMapJump::ZoneID[error](%d) Finish\n", m_ZoneID);
      }

      gfl2::heap::HeapBase * pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FIELD_DEVICE );

      {
        u32 totalFreeSize = pHeap->GetTotalFreeSize();
        GFL_PRINT( "AutoMapJump::HEAPID_FIELD_DEVICE TotalFreeSize [0x%x]\n", totalFreeSize );
        if( m_MinTotalFreeHeapSize > totalFreeSize )
        {
          m_MinTotalFreeHeapSize = totalFreeSize;
          m_MinTotalFreeHeapSizeZoneId = m_ZoneID;
        }
      }

      {
        u32 totalFreeSize = gmgr->GetGameData()->GetFieldArea()->DebugGetResourceFreeSize();
        GFL_PRINT( "AutoMapJump::AREA RESOURCE TotalFreeSize [0x%x]\n", totalFreeSize );
        if( m_MinAreaResourceFreeSize > totalFreeSize )
        {
          m_MinAreaResourceFreeSize = totalFreeSize;
          m_MinAreaResourceFreeSizeZoneId = m_ZoneID;
        }
      }
    }
    m_CountUp ++;
    if( m_CountUp >= 10 )
    {
      if (m_pHeapInfoSaver)
      {
        m_pHeapInfoSaver->WriteHeap(m_ZoneID);
      }

      //予約スクリプトを破棄しないと、イベント状態が解除されないので多重登録アサートに引っかかる
      gmgr->GetFieldScriptSystem()->SetReserveScript( FieldScript::SCRID_DUMMY );


      // 次をチェック
      switch( m_InitDesc.m_Mode )
      {
      case MODE_RANDOM:
      case MODE_APP_RETURN:
        {
          ZoneDataLoader* pZoneData =  gmgr->GetGameData()->GetFieldZoneDataLoader();
          m_ZoneID = System::GflUse::GetPublicRand( pZoneData->GetZoneNum() );

          if( (m_ZoneID == Z_F0601JF0101) || (m_ZoneID == Z_F0601JF0102) || (m_ZoneID == Z_F0601JF0103 ) )
          {
            m_ZoneID = System::GflUse::GetPublicRand( Z_F0601JF0101 );  // RANDOMジャンプでは、JFを回避
          }
        }
        break;

      default:

        // JFの場合は、元の場所に戻っておく
        //  JFの内マップはJF扱いなのでJFのまま飛ばせたい為、JFの最後の条件は内マップかどうかになる
        //if( m_JumpType == JUMPTYPE_JF )
        if( m_ZoneID == Z_F0601JF0103 )
        {
          NetEvent::JoinFestaCloseEvent::StartEvent( gmgr );
        }

        m_ZoneID++;
        break;
      }

      this->SetSeqNo( SEQ_MAPJUMP_ZONE_CHECK );
    }
    break;

  case SEQ_END:
    GFL_RELEASE_PRINT( "AutoMapJump JumpCount [%d]\n", m_MapJumpCount );
    GFL_RELEASE_PRINT( "AutoMapJump::HEAPID_FIELD_DEVICE TotalFreeSize Min Zone[%d] Size[0x%x]\n", m_MinTotalFreeHeapSizeZoneId, m_MinTotalFreeHeapSize );
    GFL_RELEASE_PRINT( "AutoMapJump::AREA RESOURCE TotalFreeSize Min Zone[%d] Size[0x%x]\n", m_MinAreaResourceFreeSizeZoneId, m_MinAreaResourceFreeSize );
    GFL_RELEASE_PRINT( "AutoMapJump SUCCESS\n", m_MapJumpCount );

#ifdef GF_CONFIG_RELEASE
    // リリース版のみモジュール読込チェックの結果を出力
    {
      c8* pZoneName;
      if (m_pZoneNameLoader->GetName(m_LowestFreeSizeZoneID, &pZoneName))
      {
        // 最もメモリの空き容量が少なかったゾーン情報を出力
        GFL_RELEASE_PRINT("AutoMapJump MODULE_CHECK_RESULT\n");
        GFL_RELEASE_PRINT("AutoMapJump ├ Lowest Free Size ZoneID[%s](%d)\n", pZoneName, m_LowestFreeSizeZoneID);
        GFL_RELEASE_PRINT("AutoMapJump └ Lowest Free Size [0x%x](%d)\n", m_LowestFreeSize, m_LowestFreeSize);
      }
    }
#endif

    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void DebugEventAutoAllMapJump::EndFunc(GameSys::GameManager* gmgr)
{
  // イベントステータスを立てる。
  gmgr->GetGameData()->GetFieldCharaModelManager()->PullEventRequestAll();
}

//--------------------------------------------------------------
/**
* @brief   対象ゾーンにジャンプできるかチェック
* @param   zoneId 対象ゾーン
*/
//--------------------------------------------------------------
DebugEventAutoAllMapJump::JumpType DebugEventAutoAllMapJump::IsJump(GameSys::GameManager* gmgr, ZoneID zoneId ) const
{
  ZoneDataLoader* pZoneData = gmgr->GetGameData()->GetFieldZoneDataLoader();
  u16 world_id = pZoneData->GetZoneWorldId( zoneId );
 
  //
  // ジャンプ可能かチェック
  //

  // ワールドに所属していない
  if( world_id == WORLD_ERROR )
  {
    return JUMPTYPE_SKIP;
  }

  // 指定ワールドではない
  if( m_InitDesc.m_Mode == MODE_ONLY_WORLD )
  {
    if( m_InitDesc.m_WorldID != world_id )
    {
      return JUMPTYPE_SKIP;
    }
  }

  // スキップ指定ゾーン
  const ZoneID c_SkipZoneTable[] =
  {
#if 0
    // 何かあったらここにスキップするゾーンを列挙
#else
    ZONE_ID_INVALID
#endif
  };

  for( u32 i = 0; i < GFL_NELEMS( c_SkipZoneTable ); i++ )
  {
    if( zoneId == c_SkipZoneTable[i] )
    {
      return JUMPTYPE_SKIP;
    }
  }


  {
    gfl2::heap::HeapBase* pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    World* pWorld = GFL_NEW(pDebugHeap) World( HEAPID_DEBUG );
    pWorld->Initialize( world_id, gmgr->GetAsyncFileManager() );
    while( pWorld->InitializeWait() == false )
    {
      gfl2::thread::ThreadStatic::CurrentSleep(1000);
    }

    b32 jump = false;
    jump = pWorld->CanMapJump();

    GFL_DELETE pWorld;

    if( !jump )
    {
      return JUMPTYPE_SKIP;
    }
  }


  // JFの外マップ
  if( zoneId == Z_F0601JF0101 )
  {
    return JUMPTYPE_JF;
  }

  return JUMPTYPE_NORMAL;

}

GFL_NAMESPACE_END(Field);

#endif // PM_DEBUG
