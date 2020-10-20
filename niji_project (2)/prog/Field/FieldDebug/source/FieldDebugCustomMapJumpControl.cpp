//======================================================================
/**
* @file FieldDebugCustomMapJumpControl.cpp
* @author ikuta_junya
* @brief ユーザーカスタマイズ可能なマップジャンプ機能管理
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <fs/include/gfl2_FsPcUtil.h>
#include <util/include/gfl2_std_string.h>
#include <core/include/gfl2_Utility.h>

// niji
#include "Field/FieldDebug/include/FieldDebugCustomMapJumpControl.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"

#include <arc_def_index/arc_def.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include <niji_conv_header/field/world/world_id.h>
#include <thread/include/gfl2_ThreadStatic.h>


#include "Field/FieldStatic/include/Event/EventMapJump.h"

#include "Field/FieldStatic/include/Debug/DebugZoneNameLoader.h"
#include "Field/FieldStatic/include/Debug/DebugCsvParser.h"
#include "Field/FieldStatic/include/Debug/DebugSymbolNameLoader.h"



GFL_NAMESPACE_BEGIN(Field);
GFL_NAMESPACE_BEGIN(Debug);

//
// 実態、デバッグ項目のOpenCloseに合わせて生成・破棄される
//

class CustomMapJumpControl::ControlCore
{
  GFL_FORBID_COPY_AND_ASSIGN( ControlCore );

public:

  // 1項目あたりのデータ
  struct DATA
  {
    b8  isValid;
    c8* pText;      // 表示名
    u32 zoneId;     // ゾーンID

    b32 isDefaultPos;         // true デフォルト位置
    gfl2::math::Vector3 pos;  // 座標
  };


  ControlCore( gfl2::heap::HeapBase* pHeap ) :
    m_pDataTable( NULL )
    ,m_DataNum( 0 )
  {
    SetUp( pHeap );
  }

  virtual ~ControlCore()
  {
    for( u32 i = 0; i < m_DataNum; i++ )
    {
      GFL_SAFE_DELETE_ARRAY( m_pDataTable[i].pText );
    }
    GFL_SAFE_DELETE_ARRAY( m_pDataTable );
    m_DataNum = 0;
  }

  void SetUp( gfl2::heap::HeapBase* pHeap )
  {
    // ゾーン名をロード
    Debug::ZoneNameLoader* pZoneNameLoader = GFL_NEW( pHeap ) Debug::ZoneNameLoader;
    if( !pZoneNameLoader->Load( pHeap ) )
    {
      GFL_ASSERT_MSG( 0, "ゾーン名文字列のロードに失敗" );
      GFL_SAFE_DELETE( pZoneNameLoader );
      return;
    }

    // CSVをロード
    Debug::CsvParser* pCsvData = GFL_NEW( pHeap ) Debug::CsvParser( pHeap, "R:/home/momiji/exec/develop_desc/Bin/CustomMapJumpZoneList_utf8.csv" ); 
    if( !pCsvData->Exists() )
    {
      GFL_PRINT( "Warning:exec/develop_desc/Bin/CustomMapJumpZoneList_utf8.csvのロードに失敗\n" );
      GFL_SAFE_DELETE( pCsvData );
      GFL_SAFE_DELETE( pZoneNameLoader );
      return;
    }

    s32 headerLineNum = 1;

    if( pCsvData->GetLineNum() > headerLineNum )
    {
      u32 dataNum = pCsvData->GetLineNum() - headerLineNum;
      m_pDataTable = GFL_NEW_ARRAY( pHeap ) DATA[ dataNum ];
      m_DataNum = dataNum;
      
      for( u32 i = 0; i < m_DataNum; i++ )
      {
        DATA data;

        Debug::CsvParser::RowObject* pRowObject = pCsvData->GetRowData( pHeap, i + headerLineNum);

        // 必要データが足りない
        if( pRowObject->GetSize() < 2 )
        {
          GFL_SAFE_DELETE( pRowObject );
          data.isValid = false;
          continue;
        }

        // ゾーンID取得
        u32 zoneId;
        if( !pZoneNameLoader->GetZoneId( pRowObject->GetColumnData( 0 ), &zoneId ) )
        {
          GFL_SAFE_DELETE( pRowObject );
          data.isValid = false;
          continue;
        }
        data.zoneId = zoneId;

        // 表示名取得
        s32 textLen = strlen( pRowObject->GetColumnData( 1 ) ) + 1;  // \0
        data.pText = GFL_NEW_ARRAY( pHeap ) c8[ textLen ];
        gfl2::std::MemCopy( pRowObject->GetColumnData( 1 ), data.pText, textLen );

        // ジャンプ位置取得
        {
          // @note 多バイト文字状態では、csv=ShiftJIS、win32=ShiftJIS、ctr=utf-8状態なので、utf-16に変換してから比較する
          if( pRowObject->Comparisons( 2, pHeap, L"○" ) )
          {
            data.isDefaultPos = true;
          }
          else
          {
            data.isDefaultPos = false;
            data.pos.x = atof( pRowObject->GetColumnData( 3 ) );
            data.pos.z = atof( pRowObject->GetColumnData( 4 ) );
            data.pos.y = 0.0f;
          }
        }

        data.isValid = true;
        m_pDataTable[ i ] = data;
        
        GFL_SAFE_DELETE( pRowObject );
      }
    }

    GFL_SAFE_DELETE( pCsvData );
    GFL_SAFE_DELETE( pZoneNameLoader );
  }

  /**
  * @breaf 項目をデバッグメニューに生成する
  * @param pParent 追加先
  * @param pHeap      デバッグメニューに追加する際に使用するヒープ
  */
  void AddDebugMenu( gfl2::debug::DebugWinGroup* pGroup, gfl2::heap::HeapBase* pHeap )
  {
    if( m_DataNum == 0 )
    {
      pGroup->ResizeItem( m_DataNum + 1, pHeap );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup , pHeap, L"データ追加", NULL, AddNuwZoneData, NULL );
      return;
    }

    pGroup->ResizeItem( m_DataNum + 1, pHeap );
    for( u32 i = 0; i < m_DataNum; i++ )
    {
      if( m_pDataTable[i].isValid )
      {
        gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup , pHeap, L"", &m_pDataTable[i], UpdateDebugMapJump, DrawDebugMapJump );
      }
      else
      {
        gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup , pHeap, L"zoneId is not found", NULL, NULL, NULL );
      }
    }
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup , pHeap, L"現在地をcsvに追加", NULL, AddNuwZoneData, NULL );
  }

private:

  /**
*    @brief マップジャンプゾーン項目　追加
*/
  static void AddNuwZoneData(void* userWork, gfl2::debug::DebugWinItem* item)
  {
    // Aボタンが押されたらマップジャンプ
    GameSys::GameManager*    pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::Button*	       pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    const gfl2::ui::VectorDevice*  cross = pDeviceManager->GetCrossKey(gfl2::ui::DeviceManager::CROSSKEY_STANDARD);

    if (pButton->IsTrigger(gfl2::ui::BUTTON_A) == true)
    {
      Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
      
      // ゾーン名をロード
      gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);
      Debug::SymbolNameLoader* pZoneNameLoader = GFL_NEW(pHeap) Debug::SymbolNameLoader;
      if (!pZoneNameLoader->Load( pHeap, Debug::SymbolNameLoader::SymbolType::ZONE_ID ) )
      {
        GFL_ASSERT_MSG(0, "ゾーン名文字列のロードに失敗");
        GFL_SAFE_DELETE(pZoneNameLoader);
        return;
      }

      c8* pZoneName;
      if (!pZoneNameLoader->GetName( pFieldmap->GetZoneID(), &pZoneName))
      {
        GFL_SAFE_DELETE(pZoneNameLoader);
        return;
      }


      // pFieldmap->GetZoneID();

      MoveModel::FieldMoveModel* pChara = pGameManager->GetFieldmap()->GetPlayerCharacter();
      gfl2::math::Vector3 pos = pChara->GetLocalSRT().translate;

      char m_Buffer[1024];
      s32 m_Start = 0;
#if defined(GF_PLATFORM_WIN32)
#define HEAP_INFO_PUTS_TEXT( text, ... ) m_Start += sprintf_s( &m_Buffer[ m_Start ], sizeof( m_Buffer ) - m_Start, text, __VA_ARGS__ )
#elif defined(GF_PLATFORM_CTR)
#define HEAP_INFO_PUTS_TEXT( ... ) m_Start += sprintf( &m_Buffer[ m_Start ], __VA_ARGS__ )
#endif

      HEAP_INFO_PUTS_TEXT("%s,AddData,,%f,%f,\n", pZoneName, pos.x, pos.z );

      m_Buffer[m_Start] = '\0';
      gfl2::fs::PcUtil::WriteFileByAddMode("R:/home/momiji/exec/develop_desc/CustomMapJumpZoneList.csv", m_Buffer, m_Start);
      m_Start = 0;
    }
  }

  /**
  *    @brief マップジャンプゾーン項目　Update
  */
  static void UpdateDebugMapJump(void* userWork, gfl2::debug::DebugWinItem* item )
  {
    //
    // 決定ボタンが押された際に、その項目のゾーンにジャンプする
    //

    DATA* pData = reinterpret_cast<DATA*>( userWork );

    // Aボタンが押されたらマップジャンプ
    GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
    {
      // ワールドに登録されているゾーンかチェック
      u16 world_id = pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneWorldId( pData->zoneId );
      const char * cpErrorStr = "このゾーンはWorldに登録されていない為ジャンプできません\n";


      // @todo デバッグマップジャンプからこぴぺ、共通化したい
      bool jump = false;
      if( (world_id != WORLD_ERROR) )
      {
        gfl2::heap::HeapBase* pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
        World* pWorld = GFL_NEW(pDebugHeap) World( HEAPID_DEBUG );
        pWorld->Initialize( world_id, pGameManager->GetAsyncFileManager() );
        while( pWorld->InitializeWait() == false )
        {
          gfl2::thread::ThreadStatic::CurrentSleep(1000);
        }

        jump = pWorld->CanMapJump();

        if( !jump )
        {
          cpErrorStr = "このゾーンは設計マップ段階なためジャンプできません\n";
        }

        GFL_DELETE pWorld;
      }

      if( jump )
      {
        FieldScript::FieldScriptSystem* script = pGameManager->GetFieldScriptSystem();
        if( script->IsRunning()){
          GFL_PRINT( "スクリプトイベント実行中はデバッグマップジャンプできません\n" );
          return;
        }
        if( pData->isDefaultPos )
        {
          EventMapJump::CallMapJumpDefaultPos( pGameManager, pData->zoneId );
        }
        else
        {
          EventMapJump::CallMapJumpPos( pGameManager, pData->zoneId, pData->pos );
        }

        // イベントステータスを立てる。
        pGameManager->GetGameData()->GetFieldCharaModelManager()->AddEventRequestAll();

        // ウィンドウを閉じる
        gfl2::debug::DebugWin_CloseWindow();
      }
      else
      {
        GFL_PRINT( cpErrorStr );
      }
    }
  }

  /**
  *    @brief マップジャンプゾーン項目　Draw
  */
  static wchar_t* DrawDebugMapJump( void* userWork, gfl2::debug::DebugWinItem* item )
  {
    //
    // 対象項目に結び付けられているゾーン名を表示する
    //

    DATA* pData = reinterpret_cast<DATA*>( userWork );
    wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

    // マルチバイト文字列をワイド文字列に変換
    if( Field::Debug::CsvParser::MbstoWcs( workStr, pData->pText, gfl2::debug::DEBUGWIN_NAME_LEN ) == static_cast< size_t >( -1 ) )
    {
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"error" );
    }

    return workStr;
  }

  DATA* m_pDataTable;             // 設定ファイルから解凍したデータ
  u32   m_DataNum;                // 解凍データ数
};



//
// CustomMapJumpControl
//

CustomMapJumpControl::CustomMapJumpControl() :
  m_pParentGroup( NULL )
  ,m_pGrop( NULL )
  ,m_pCore( NULL )
{

}

CustomMapJumpControl::~CustomMapJumpControl()
{
  if( m_pGrop )
  {
    m_pParentGroup->RemoveItem( m_pGrop );
    m_pGrop = NULL;
    m_pParentGroup = NULL;
  }

  // @note RemoveItemではDestroyGroupが実行されない
  DestroyCore();
}

/**
* @breaf 項目をデバッグメニューに生成する
* @param pParent 追加先
* @param pHeap      デバッグメニューに追加する際に使用するヒープ
* @param pHeapCore  デバッグメニューの項目が開かれた際に使用するヒープ
*/
void CustomMapJumpControl::CreateItem( gfl2::debug::DebugWinGroup* pParent, gfl2::heap::HeapBase* pHeap,  gfl2::heap::HeapBase* pHeapCore )
{
  m_pHeapCore = pHeapCore; 
  m_pParentGroup = pParent;

  // @note Removeはフィールドに任せる
  m_pGrop = gfl2::debug::DebugWin_AddGroup( pHeap, L"2.1.カスタムマップジャンプ", m_pParentGroup );
  m_pGrop->SetCreateDestroyGroupFunc( this, CreateGroup, DestroyGroup );
}

/**
* @breaf 実態を生成する
*/
void CustomMapJumpControl::CreateCore( void )
{
  m_pCore = GFL_NEW( m_pHeapCore ) CustomMapJumpControl::ControlCore( m_pHeapCore );
  m_pCore->AddDebugMenu( m_pGrop, m_pHeapCore );
}

/**
* @breaf 実態を破棄する
*/
void CustomMapJumpControl::DestroyCore( void )
{
  GFL_SAFE_DELETE( m_pCore );
}

/**
* @breaf カスタムマップジャンプグループ、生成破棄コールバック関数
* @param userWork 対象グループを管理するCustomMapJumpControlインスタンス
*/
void CustomMapJumpControl::CreateGroup( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CustomMapJumpControl* pControl = reinterpret_cast<CustomMapJumpControl*>( userWork );
  pControl->CreateCore();
}

void CustomMapJumpControl::DestroyGroup( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CustomMapJumpControl* pControl = reinterpret_cast<CustomMapJumpControl*>( userWork );
  pControl->DestroyCore();

  // 全項目削除
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG