/**
* @brief  デバッグ用カメラ位置調整イベント
* @file   FieldEventDebugTerrainCheck.cpp
* @author ikuta_junya
* @data   2015.05.23
*/

#if PM_DEBUG

// gfl2
#include <Debug/include/gfl2_DebugPrint.h>

// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "Sound/include/Sound.h"

// field
#include "Field/FieldRo/include/Event/FieldEventDebugTerrainCheck.h"
#include "Field/FieldRo/include/FieldMap.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainLayoutData.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainFactory.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

#include <core/include/gfl2_Utility.h>

#include <fs/include/gfl2_FsPcUtil.h>


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Event );

class EventDebugTerrainCheck::HeapInfoSaver
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
    HEAP_INFO_PUTS_TEXT("ZONE_ID,x,z,freeSize,HeapSize");
    HEAP_INFO_PUTS_TEXT("\n");
    WriteFile();
  }
  void WriteInfo( c8* pWorldName, c8* pAreaName, s32 x, s32 z, const Field::Terrain::TerrainFactory::DEBUG_INFO& rOut )
  {
    c8* pZoneName;
    // m_pZoneNameLoader->GetName(zoneId, &pZoneName);
    HEAP_INFO_PUTS_TEXT("%s,%s,", pWorldName, pAreaName );
    HEAP_INFO_PUTS_TEXT("%d,", x );
    HEAP_INFO_PUTS_TEXT("%d,", z );
    HEAP_INFO_PUTS_TEXT("%d,", rOut.heapFreeSize );
    HEAP_INFO_PUTS_TEXT("%d,", rOut.heapSize );
    HEAP_INFO_PUTS_TEXT("%d,", rOut.binaryCompressSize);
    HEAP_INFO_PUTS_TEXT("%d,", rOut.binarySize);
    HEAP_INFO_PUTS_TEXT("\n");
    WriteFileByAddMode();
  }

private:

  void WriteFile(void)
  {
    m_Buffer[m_Start] = '\0';
    gfl2::fs::PcUtil::WriteFileByAddMode("R:/home/momiji/exec/direct_sdmc/block_heap_info.csv", m_Buffer, m_Start);
    m_Start = 0;
  }

  void WriteFileByAddMode(void)
  {
    m_Buffer[m_Start] = '\0';
    gfl2::fs::PcUtil::WriteFileByAddMode("R:/home/momiji/exec/direct_sdmc/block_heap_info.csv", m_Buffer, m_Start);
    m_Start = 0;
  }

  char m_Buffer[20480];
  u32 m_Start;

  Field::Debug::SymbolNameLoader* m_pZoneNameLoader;
};

/**
* @brief コンストラクタ
*
* @param  pSystemHeap GameEventに渡すヒープ
*
* @return 無し
*/ 
EventDebugTerrainCheck::EventDebugTerrainCheck( gfl2::heap::HeapBase* pSystemHeap ) :

  GameSys::GameEvent( pSystemHeap )

  // 内部制御
  ,m_X( 0 )
  ,m_Z( 0 )
  ,m_WorldID( 0 )
  ,m_AreaID( 0 )
  ,m_IsHeapInfoSave( false )

  // 内部システム
  // 外部オブジェクト
  ,m_pHeapInfoSaver( NULL )
  ,m_pZoneNameLoader( NULL )
  // 外部システム
  ,m_pFieldMap( NULL )
  ,m_pSystemHeap( NULL )

{

}

/**
* @brief デストラクタ
*
* @return 無し
*/ 
EventDebugTerrainCheck::~EventDebugTerrainCheck()
{
  GFL_SAFE_DELETE(m_pHeapInfoSaver);
}

/**
* @brief 初期化処理(インスタンス生成直後に実行する)
*
* @param  rDesc   初期化設定
*
* @return 無し
*/ 
void EventDebugTerrainCheck::Initialize( const InitializeDescription& rDesc )
{
  m_pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  m_pAreaNameLoader = rDesc.m_pAreaNameLoader;
  m_pWorldNameLoader = rDesc.m_pWorldNameLoader;
  m_WorldID = rDesc.m_WorldId;
  m_AreaID = rDesc.m_AreaId;
  m_IsHeapInfoSave = rDesc.m_isHeapInfoSave;
  // m_pSystemHeap = rDesc.m_pHeap;
}

//--------------------------------------------------------------
/**
* @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @retval  true  起動してよい
* @retval  false 起動しない
*/
//--------------------------------------------------------------
bool EventDebugTerrainCheck::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventDebugTerrainCheck::InitFunc(GameSys::GameManager* gmgr)
{
  m_pFieldMap = gmgr->GetFieldmap();

  // Heap情報をHIOでファイル出力する機能
  if ( m_IsHeapInfoSave )
  {
    bool flag = false;
    gfl2::fs::Result res = gfl2::fs::PcUtil::IsPcAccessEnableFlag(&flag);

    if (res.IsSuccess() && flag && gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_DEBUGGER ) )
    {
      gfl2::heap::HeapBase * pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEBUGGER );
      m_pHeapInfoSaver = GFL_NEW( pDebugHeap ) HeapInfoSaver( pDebugHeap, NULL );
      // m_pHeapInfoSaver->WriteHeader();
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
GameSys::GMEVENT_RESULT EventDebugTerrainCheck::MainFunc(GameSys::GameManager* gmgr)
{
  // シーケンス制御値
  struct Seq
  {
    enum Value
    {
      INIT = 0,
      MAIN,
      TERM,
      TERM_WAIT,
      END,
    };
  };

  switch( this->GetSeqNo() )
  {
  case Seq::INIT:
    {
      Field::Terrain::TerrainManager* pTerrainManager = m_pFieldMap->GetTerrainManager();
      const Field::Terrain::TerrainLayoutData* pTerrainLayoutData = pTerrainManager->GetLayoutData();
      u32 numX = pTerrainManager->GetLayoutData()->GetBlockNumOfXAxis();
      u32 numZ = pTerrainManager->GetLayoutData()->GetBlockNumOfZAxis();

      // 終わり
      if (m_Z >= numZ)
      {
        this->SetSeqNo(Seq::TERM);
        break;
      }

      // ロードリクエスト
      {
        pTerrainManager->SetExternalBlockControlPointByBlockNo(m_X, m_Z);
      }

      this->SetSeqNo( Seq::MAIN );
      break;
    } // case Seq::INIT:

  case Seq::MAIN:
    {
      Field::Terrain::TerrainManager* pTerrainManager = m_pFieldMap->GetTerrainManager();
      const Field::Terrain::TerrainLayoutData* pTerrainLayoutData = pTerrainManager->GetLayoutData();
      u32 numX = pTerrainManager->GetLayoutData()->GetBlockNumOfXAxis();
      u32 numZ = pTerrainManager->GetLayoutData()->GetBlockNumOfZAxis();

      if( pTerrainManager->IsBlockLodChanging() )
      {

      }
      else
      {

        if (m_pHeapInfoSaver)
        {
          c8* pWorldName;
          m_pWorldNameLoader->GetName( m_WorldID, &pWorldName);

          c8* pAreaName;
          m_pAreaNameLoader->GetName( m_AreaID, &pAreaName);

          const Field::Terrain::TerrainFactory* pTerrainFactory = pTerrainManager->GetTerrainFactory();
          for (u32 x_count = 0; x_count < 2; x_count++)
          {
            for (u32 z_count = 0; z_count < 2; z_count++)
            {
              Field::Terrain::TerrainFactory::DEBUG_INFO info;
              u32 x = m_X + x_count;
              u32 z = m_Z + z_count;
              if ( pTerrainFactory->DebugGetInfo( x, z, &info ) )
              {
                m_pHeapInfoSaver->WriteInfo(pWorldName, pAreaName, x, z, info);
              }
            }
          }
        }

        GFL_PRINT("EventDebugTerrainCheck Load [%2d][%2d] / [%2d][%2d]\n", m_X, m_Z, numX, numZ );
        
        // すすめる
        m_X += 2;
        if (m_X >= numX)
        {
          m_X = 0;
          m_Z += 2;
        }

        // 終わり
        if( m_Z >= numZ )
        {
          this->SetSeqNo( Seq::TERM );
          break;
        }

        // ロードリクエスト
        {
          pTerrainManager->SetExternalBlockControlPointByBlockNo( m_X, m_Z );
        }

      }

      break;
    } // case Seq::MAIN:

   case Seq::TERM:
    {
      this->SetSeqNo( Seq::TERM_WAIT );
      break;

    } // case Seq::TERM

   case Seq::TERM_WAIT:
     {
       this->SetSeqNo( Seq::END );
       break;
     } // case Seq::TERM_WAIT

   case Seq::END:
     {
       // 制御点の上書きをクリア
       Field::Terrain::TerrainManager* pTerrainManager = m_pFieldMap->GetTerrainManager();
       pTerrainManager->ClearExternalBlockControlPoint();
       return GameSys::GMEVENT_RES_FINISH;
     } // case Seq::END:

  } // switch( this->GetSeqNo() )

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventDebugTerrainCheck::EndFunc(GameSys::GameManager* gmgr)
{

}

GFL_NAMESPACE_END( Event );
GFL_NAMESPACE_END( Field );

#endif