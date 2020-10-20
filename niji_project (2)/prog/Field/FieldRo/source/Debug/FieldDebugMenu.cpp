/**
* @brief  フィールドデバッグメニュー
* @file   FieldDebugMenu.cpp
* @author ikuta_junya
* @data   2015.03.08
*/

#if PM_DEBUG

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <thread/include/gfl2_ThreadStatic.h>
#include  <debug/include/gfl2_DebugLogWindow.h>

// niji
#include "Debug/DebugWin/include/DebugWinSystem.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/NijiTime.h"
#include "System/include/HeapDefine.h"
#include "Sound/include/Sound.h"
#include "Savedata/include/MyStatus.h"
#include "PokeTool/include/PokeToolRibbon.h"
#include "Savedata/include/MysteryGiftSave.h"
 
// Field
#include "Field/FieldStatic/include/Event/EventGameStart.h"
#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/FieldWarpData.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeNpc.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/FieldEnvManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainLayoutData.h"
#include "Field/FieldRo/include/EnvSound/EnvSoundManager.h"
#include "Field/FieldStatic/include/Item/FieldItemManager.h"
#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"
#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"
#include "Field/FieldRo/include/FieldRenderingPipeLineManager.h" 

#include "../EventCheck/FieldEventCheck.h"
#include "Field/FieldStatic/include/Event/EventGameStart.h"

// StrInput
#include "Field/FieldStatic/include/Debug/EventDebugStrInput.h"

// FieldScript
#include "FieldScript/include/EventScriptCall.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptAssert.h"

// Field Debug
#include "../../include/Debug/FieldDebugMenu.h"
#include "../../include/Debug/FieldDebugMapJump.h"
#include "../../include/Debug/FieldDebugTraffic.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"                // @note フィールド上のデバック定義は可能な限りここに集める
#include "Field/FieldStatic/include/Debug/FieldDebugSwitchGroup.h"
#include "Field/FieldStatic/include/Debug/EventDebugCharaViewer.h"
#include "Field/FieldStatic/include/Debug/EventDebugKisekae.h"
#include "Field/FieldStatic/include/Debug/EventDebugKisekaeCurrentPosition.h"
#include "Field/FieldStatic/include/Debug/EventDebugRidePokemon.h"
#include "Field/FieldRo/include/Debug/Menu/FieldDebugMenuTerrain.h"
#include "Field/FieldRo/include/Debug/FieldDebugMenuCameraGroup.h"
#include "Field/FieldRo/include/Debug/FieldDebugSignal.h"
#include "Field/FieldRo/include/Debug/FieldDebugFade.h"
#include "Field/FieldRo/include/Debug/FieldDebugEffect.h"
#include "Field/FieldRo/include/Debug/FieldDebugTrialModel.h"
#include "Field/FieldRo/include/Debug/FieldDebugEncount.h"
#include "Field/FieldRo/include/Debug/FieldDebugGimmickEncount.h"
#include "Field/FieldRo/include/Debug/FieldDebugPokeFinder.h"
#include "Field/FieldRo/include/Debug/FieldDebugDispInfo.h"
#include "Field/FieldRo/include/Debug/FieldDebugWeather.h"
#include "Field/FieldRo/include/Debug/FieldDebugCharaLayoutEditor.h"
#include "Field/FieldRo/include/Debug/FieldDebugEventCameraEditor.h"
#include "Field/FieldRo/include/Debug/FieldDebugStaticModel.h"
#include "Field/FieldRo/include/Debug/FieldDebugFieldDemo.h"
#include "Field/FieldRo/include/Debug/FieldDebugSodateya.h"
#include "Field/FieldRo/include/Debug/FieldDebugFishing.h"
#include "Field/FieldRo/include/Debug/FieldDebugBattleInst.h"
#include "Field/FieldRo/include/Debug/FieldDebugRotomPower.h"

#include "Debug/DebugEvent/EventDebugPokeMake/include/EventDebugPokeMake.h"
#include "Debug/DebugEvent/EventDebugItemMake/include/EventDebugItemMake.h"

#include "App/FieldMenu/source/ReportPlayer.h"

#include "niji_conv_header\field\script\inc\debug_event_jump.inc"
#include "niji_conv_header\field\script\inc\debug_event_jump_sub.inc"
#include "niji_conv_header\field\script\inc\debug_scr.inc"
#include "niji_conv_header\field\script\inc\debug_iwasawa.inc"
#include "niji_conv_header\field\script\inc\debug_kojima_yoshikazu.inc"
#include "niji_conv_header\field\script\inc\debug_muranaka_ryota.inc"
#include "niji_conv_header\field\script\inc\debug_suginaka_katsunori.inc"
#include "niji_conv_header\field\script\inc\debug_takeda_nobutaka.inc"
#include "niji_conv_header\field\script\inc\debug_tamagawa_satoshi.inc"
#include "niji_conv_header\field\script\inc\debug_watanabe_yuya.inc"
#include "niji_conv_header\field\script\inc\debug_tani_hiroyuki.inc"
#include "niji_conv_header\field\script\inc\debug_ohhira_syo.inc"
#include "niji_conv_header\field\script\inc\debug_hosaka_genya.inc"
#include "niji_conv_header\field\script\inc\debug_asaba_akinori.inc"
#include "niji_conv_header\field\script\inc\debug_onoue_masayuki.inc"
#include "niji_conv_header\field\script\inc\debug_koda_rena.inc"
#include "niji_conv_header\field\script\inc\pokecen.inc"
#include "niji_conv_header\field\chara\chara_model_id.h"
#include "Field/FieldDebug/include/Menu/FieldDebugMenuAttrDispEditor.h"

#include "Field/FieldDebug/include/FieldDebugCustomMapJumpControl.h"

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

// other
#include <arc_def_index/arc_def.h>
#include <arc_index/debug_message.gaix>
#include <arc_index/message.gaix>

// モデルのインデックス
#include <niji_conv_header/field/chara/chara_model_id.h>

#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h>
#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h>
#include <Savedata/include/JoinFestaDataSave.h>
// グローバルアトラクション定義
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaGlobalAttractionDefine.h"

#include <NetStatic/NetLib/include/Validation/PokemonValidation.h>
#include "Battle/Regulation/include/RegulationScriptChecker.h"
#include <NetApp/GTS/source/GTSTypes.h>
#include <NetApp/GTS/source/GTSUtility.h>

#include "Field/FieldStatic/include/Debug/FieldDebugFlagWorkSearchProc.h"

#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"
#include "AppLib/include/Tool/AppToolRotomFriendship.h"

#include "Field/FieldDebug/include/FieldDebugMantainRecord.h"

//-----------------------------------------------------------------------------
/**
 *  @brief  時間帯情報
 */
//-----------------------------------------------------------------------------
struct TimeControl {
  u32      labelId;       // メッセージスタジオのラベル
  u32      status;
  s32      hour;
  s32      minute;
  u32      rate;
};
enum{
  ENTIME_STOPFAKETIME,
  ENTIME_STARTFAKETIME,
  ENTIME_FIXFAKETIME,
};
static const TimeControl sc_TIMECONTROL[] = 
{
  {  msg_dmenu_field_time_ctrl_normal,   ENTIME_STOPFAKETIME,  },
  {  msg_dmenu_field_time_ctrl_stop,     ENTIME_STARTFAKETIME,  0, 0, 0 },
  {  msg_dmenu_field_time_ctrl_x1,      ENTIME_STARTFAKETIME,  0, 0, 1 },
  {  msg_dmenu_field_time_ctrl_x60,      ENTIME_STARTFAKETIME,  0, 0, 60 },
  {  msg_dmenu_field_time_ctrl_x600,     ENTIME_STARTFAKETIME,  0, 0, 600 },
  {  msg_dmenu_field_time_ctrl_x1800,    ENTIME_STARTFAKETIME,  0, 0, 1800 },
  {  msg_dmenu_field_time_ctrl_5sec1day, ENTIME_STARTFAKETIME,  0, 0, 17280 },

  {  msg_dmenu_field_time_ctrl_morning,  ENTIME_FIXFAKETIME,  6, 20 },
  {  msg_dmenu_field_time_ctrl_noon,     ENTIME_FIXFAKETIME, 10, 20 },
  {  msg_dmenu_field_time_ctrl_evening,  ENTIME_FIXFAKETIME, 17, 20 },
  {  msg_dmenu_field_time_ctrl_night,    ENTIME_FIXFAKETIME, 21, 10 },
  {  msg_dmenu_field_time_ctrl_midnight, ENTIME_FIXFAKETIME,  0, 20 },

  {  msg_dmenu_field_time_ctrl_8,   ENTIME_FIXFAKETIME, 8 },
  {  msg_dmenu_field_time_ctrl_12,   ENTIME_FIXFAKETIME, 12 },
  {  msg_dmenu_field_time_ctrl_16,   ENTIME_FIXFAKETIME, 16 },
  {  msg_dmenu_field_time_ctrl_18,   ENTIME_FIXFAKETIME, 18 },
  {  msg_dmenu_field_time_ctrl_20,   ENTIME_FIXFAKETIME, 20 },
  {  msg_dmenu_field_time_ctrl_0,   ENTIME_FIXFAKETIME, 0 },
  {  msg_dmenu_field_time_ctrl_4,   ENTIME_FIXFAKETIME, 4 },
};

//--------------------------------------------------------------
/// フラグ操作：データ解釈用定義
//==============================================================
enum
{
  DEBUG_SCR_MAX = 16,
  DEBUG_SCR_OFS_ID = 0,
  DEBUG_SCR_OFS_NAME = 2,
  DEBUG_SCR_NAME_LEN = 30,
  DEBUG_SCR_EACH_SIZE = 32,
};







GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Debug )





// フラグ・ワーク関連処理
// @note 1種類のタイプのフラグワーク文字列を管理する
class FlagWorkImpl
{
  GFL_FORBID_COPY_AND_ASSIGN( FlagWorkImpl );

public:

  FlagWorkImpl( void ) :
    m_LabelBufferType( 0 )
    ,m_LabelSize( 0 )
    ,m_pLabelBuffer( NULL )
  {
  }

  ~FlagWorkImpl()
  {
  }

  /**
  * @brief 指定タイプのフラグワーク文字列をロード
  */
  void SyncLoadLabel( gfl2::heap::HeapBase* pHeap, s32 type )
  {
    DeleteLabel();

    size_t  readSize = 0;

    // @todo ARCID_SCRIPTDEBUGSYMBOLひらきっぱ
    gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq  req;
    req.arcId = ARCID_SCRIPTDEBUGSYMBOL;
    req.datId = type;
    req.ppBuf = &m_pLabelBuffer; ///< SyncArcFileLoadData 内で確保される
    req.pBufSize = NULL;
    req.pRealReadSize = &readSize;
    req.heapForBuf = pHeap;
    req.heapForReq = pHeap->GetLowerHandle();
    req.align = 4;
    man->SyncArcFileLoadData(req);


    m_LabelBufferType = type;
    m_LabelSize = readSize / DEBUG_SCR_EACH_SIZE;
  }

  /**
  * @brief フラグワーク文字列を破棄
  */
  void DeleteLabel()
  {
    GflHeapSafeFreeMemory( m_pLabelBuffer );
  }

  /**
  * @brief フラグワーク文字列の数を取得
  */
  s32 GetSize( void ) const
  {
    return m_LabelSize;
  }

  /**
  * @brief 指定番号のフラグワーク文字列を取得
  */
  const c8* GetLabelPtr( s32 index ) const
  {
    return &( reinterpret_cast<const c8*>( m_pLabelBuffer )[ index * DEBUG_SCR_EACH_SIZE ] );
  }

  /**
  * @brief フラグワーク文字列から実IDを取得
  */
  u16 GetLabelPtrToRealId( const c8* ptr )
  {
    return *(reinterpret_cast<const u16*>(&ptr[ DEBUG_SCR_OFS_ID ]));
  }
  
  /**
  * @brief フラグワーク文字列から実表示文字列を取得
  */
  const c8* GetLabelPtrPtrToText( const c8* ptr )
  {
    return &ptr[ DEBUG_SCR_OFS_NAME ];
  }

  /**
  * @brief ロードされているフラグワークの種別を取得
  */
  s32 GetLabelBufferType( void ) const { return m_LabelBufferType; }


  
  /**
  * @brief ワーク、フラグ判定
  * @param datId データID
  * @return true : フラグ, false : ワーク
  */
  static bool IsEventWorkFlag( u32 datId )
  {
    bool is_flag = true;
    if(
      datId == GARC_ScriptDebugSymbol_SceneWork_BIN ||
      datId == GARC_ScriptDebugSymbol_SysWork_BIN ||
      datId == GARC_ScriptDebugSymbol_OtherWork_BIN
      )
    {
      is_flag = false;
    }

    return is_flag;
  }

  /**
  * @brief フラグ操作項目 更新
  */
  static void UpdateFlagWorkItem( void* userWork, gfl2::debug::DebugWinItem* item )
  {
    const c8* labelTopPtr = reinterpret_cast<const c8*>( userWork );

    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    FlagWorkImpl* pFlagWorkImpl = pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetFlagWorkImpl();

    const gfl2::ui::Button*   pBTN  = pGameManager->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

    u16 real_id = pFlagWorkImpl->GetLabelPtrToRealId( labelTopPtr );
    bool is_flag = IsEventWorkFlag( pFlagWorkImpl->GetLabelBufferType() );

    EventWork* pEV = pGameManager->GetGameData()->GetEventWork();
    if( is_flag )
    {
      if( pBTN->IsRepeat( gfl2::ui::DIR_RIGHT ) || pBTN->IsRepeat( gfl2::ui::DIR_LEFT ) )
      {
        bool val = pEV->CheckEventFlag( real_id );
        if( val )
        {
          pEV->ResetEventFlag( real_id );
        }
        else
        {
          pEV->SetEventFlag( real_id );
        }
        //item->PlaySound( gfl2::debug::SE_CHANGE_VALUE );
      }
    }
    else
    {
      int val = 1;

      if( pBTN->IsHold( gfl2::ui::BUTTON_L ) )
      {
        val *= 100;
      }
      if( pBTN->IsHold( gfl2::ui::BUTTON_R ) )
      {
        val *= 10;
      }

      if( pBTN->IsRepeat( gfl2::ui::DIR_RIGHT ) )
      {
        u16* wk = pEV->GetEventWorkAdrs( real_id );
        *wk += val;
        //item->PlaySound( gfl2::debug::SE_CHANGE_VALUE );
      }
      else if( pBTN->IsRepeat( gfl2::ui::DIR_LEFT ) )
      {
        u16* wk = pEV->GetEventWorkAdrs( real_id );
        *wk -= val;
        //item->PlaySound( gfl2::debug::SE_CHANGE_VALUE );
      }
    }
  }

  /**
  * @brief フラグ操作項目 描画
  */
  static wchar_t* DrawFlagWorkItem( void* userWork, gfl2::debug::DebugWinItem* item )
  {
    const c8* labelTopPtr = reinterpret_cast<const c8*>( userWork );
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    FlagWorkImpl* pFlagWorkImpl = pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetFlagWorkImpl();

    wchar_t     *workStr = gfl2::debug::DebugWin_GetWorkStr();
    u16 real_id = pFlagWorkImpl->GetLabelPtrToRealId( labelTopPtr );
    const char  *cp_name = pFlagWorkImpl->GetLabelPtrPtrToText( labelTopPtr );
    bool is_flag = IsEventWorkFlag( pFlagWorkImpl->GetLabelBufferType() );
    
    int val;
    EventWork* pEV = pGameManager->GetGameData()->GetEventWork();
    if( is_flag )
    {
      val = pEV->CheckEventFlag( real_id );
    }
    else
    {
      u16* wk = pEV->GetEventWorkAdrs( real_id );
      val = *wk;
    }

#if defined(GF_PLATFORM_WIN32)
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%S[%d]", cp_name, val );
#elif defined(GF_PLATFORM_CTR)
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%s[%d]", cp_name, val ); //printfの%S書式がARMには無い
#endif

    return workStr;
  }

private:

  s32   m_LabelBufferType;    // バッファに読み込まれているフラグの種別
  s32   m_LabelSize;          // バッファに読み込まれているラベル数
  void* m_pLabelBuffer;       // フラグ文字列をロードするバッファ

};

// フラグ・ワーク検索機能関連処理
// 全ての種類のフラグ・ワーク文字列を管理する
class FlagWorkSearchImpl
{
  GFL_FORBID_COPY_AND_ASSIGN( FlagWorkSearchImpl );

public:

  FlagWorkSearchImpl( void )
  {
    for( u32 i = 0; i < GARC_ScriptDebugSymbol_DATA_NUM; i++ )
    {
      m_LabelSizeTable[i] = 0;
      m_LabelBufferTable[i] = NULL;
    }
  }

  ~FlagWorkSearchImpl()
  {
  }

  /**
  * @brief フラグワーク文字列をロード
  */
  void SyncLoadLabel( gfl2::heap::HeapBase* pHeap )
  {
    DeleteLabel();


    for( u32 i = 0; i < GARC_ScriptDebugSymbol_DATA_NUM; i++ )
    {
      size_t  readSize = 0;
      // @todo ARCID_SCRIPTDEBUGSYMBOLひらきっぱ
      gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq  req;
      req.arcId = ARCID_SCRIPTDEBUGSYMBOL;
      req.datId = i;
      req.ppBuf = &m_LabelBufferTable[i]; ///< SyncArcFileLoadData 内で確保される
      req.pBufSize = NULL;
      req.pRealReadSize = &readSize;
      req.heapForBuf = pHeap;
      req.heapForReq = pHeap->GetLowerHandle();
      req.align = 4;
      man->SyncArcFileLoadData(req);

      m_LabelSizeTable[i] = readSize / DEBUG_SCR_EACH_SIZE;
    }
  }

  /**
  * @brief フラグワーク文字列を破棄
  */
  void DeleteLabel()
  {
    for( u32 i = 0; i < GARC_ScriptDebugSymbol_DATA_NUM; i++ )
    {
      GflHeapSafeFreeMemory( m_LabelBufferTable[i] );
      m_LabelSizeTable[i] = 0;
    }
  }

  /**
  * @brief 指定種類のフラグワーク文字列の数を取得
  */
  s32 GetSize( s32 type ) const
  {
    return m_LabelSizeTable[ type ];
  }

  /**
  * @brief 指定種類、指定番号のフラグワーク文字列を取得
  */
  const c8* GetLabelPtr( s32 type, s32 index ) const
  {
    return &( reinterpret_cast<const c8*>( m_LabelBufferTable[type] )[ index * DEBUG_SCR_EACH_SIZE ] );
  }

  /**
  * @brief フラグワーク文字列から実IDを取得
  */
  u16 GetLabelPtrToRealId( const c8* ptr )
  {
    return *(reinterpret_cast<const u16*>(&ptr[ DEBUG_SCR_OFS_ID ]));
  }
  
  /**
  * @brief フラグワーク文字列から実表示文字列を取得
  */
  const c8* GetLabelPtrPtrToText( const c8* ptr )
  {
    return &ptr[ DEBUG_SCR_OFS_NAME ];
  }

  /**
  * @brief フラグワーク文字列からフラグの種別を取得
  */
  s32 GetLabelPtrToBufferType( const c8* ptr ) const 
  { 
    for( u32 i = 0; i < GARC_ScriptDebugSymbol_DATA_NUM; i++ )
    {
      if( m_LabelSizeTable[i] == 0 )
      {
        continue;
      }

      if( ptr >= m_LabelBufferTable[i] && ptr < &( reinterpret_cast<const c8*>( m_LabelBufferTable[i] )[ m_LabelSizeTable[i] * DEBUG_SCR_EACH_SIZE ] ))
      {
        return i;
      }
    }

    return 0;
  }

  /**
  * @brief フラグ操作項目 更新
  */
  static void UpdateFlagWorkItem( void* userWork, gfl2::debug::DebugWinItem* item )
  {
    const c8* labelTopPtr = reinterpret_cast<const c8*>( userWork );

    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    FlagWorkSearchImpl* pFlagWorkImpl = pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetFlagWorkSearchImpl();

    const gfl2::ui::Button*   pBTN  = pGameManager->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

    s32 type = pFlagWorkImpl->GetLabelPtrToBufferType( labelTopPtr );
    u16 real_id = pFlagWorkImpl->GetLabelPtrToRealId( labelTopPtr );
    bool is_flag = FlagWorkImpl::IsEventWorkFlag( type );

    EventWork* pEV = pGameManager->GetGameData()->GetEventWork();
    if( is_flag )
    {
      if( pBTN->IsRepeat( gfl2::ui::DIR_RIGHT ) || pBTN->IsRepeat( gfl2::ui::DIR_LEFT ) )
      {
        bool val = pEV->CheckEventFlag( real_id );
        if( val )
        {
          pEV->ResetEventFlag( real_id );
        }
        else
        {
          pEV->SetEventFlag( real_id );
        }
        //item->PlaySound( gfl2::debug::SE_CHANGE_VALUE );
      }
    }
    else
    {
      int val = 1;

      if( pBTN->IsHold( gfl2::ui::BUTTON_L ) )
      {
        val *= 100;
      }
      if( pBTN->IsHold( gfl2::ui::BUTTON_R ) )
      {
        val *= 10;
      }

      if( pBTN->IsRepeat( gfl2::ui::DIR_RIGHT ) )
      {
        u16* wk = pEV->GetEventWorkAdrs( real_id );
        *wk += val;
        //item->PlaySound( gfl2::debug::SE_CHANGE_VALUE );
      }
      else if( pBTN->IsRepeat( gfl2::ui::DIR_LEFT ) )
      {
        u16* wk = pEV->GetEventWorkAdrs( real_id );
        *wk -= val;
        //item->PlaySound( gfl2::debug::SE_CHANGE_VALUE );
      }
    }
  }

  /**
  * @brief フラグ操作項目 描画
  */
  static wchar_t* DrawFlagWorkItem( void* userWork, gfl2::debug::DebugWinItem* item )
  {
    const c8* labelTopPtr = reinterpret_cast<const c8*>( userWork );
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    FlagWorkSearchImpl* pFlagWorkImpl = pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetFlagWorkSearchImpl();

    wchar_t     *workStr = gfl2::debug::DebugWin_GetWorkStr();
    u16 real_id = pFlagWorkImpl->GetLabelPtrToRealId( labelTopPtr );
    const char  *cp_name = pFlagWorkImpl->GetLabelPtrPtrToText( labelTopPtr );
    s32 type = pFlagWorkImpl->GetLabelPtrToBufferType( labelTopPtr );
    bool is_flag = FlagWorkImpl::IsEventWorkFlag( type );
    
    int val;
    EventWork* pEV = pGameManager->GetGameData()->GetEventWork();
    if( is_flag )
    {
      val = pEV->CheckEventFlag( real_id );
    }
    else
    {
      u16* wk = pEV->GetEventWorkAdrs( real_id );
      val = *wk;
    }

#if defined(GF_PLATFORM_WIN32)
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%S[%d]", cp_name, val );
#elif defined(GF_PLATFORM_CTR)
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%s[%d]", cp_name, val ); //printfの%S書式がARMには無い
#endif

    return workStr;
  }

private:

  s32   m_LabelSizeTable[GARC_ScriptDebugSymbol_DATA_NUM];          // バッファに読み込まれているラベル数
  void* m_LabelBufferTable[GARC_ScriptDebugSymbol_DATA_NUM];     // フラグ文字列をロードするバッファ

};

static wchar_t* Draw_DispMoveModel(void* userWork, gfl2::debug::DebugWinItem* item)
{
  const c8* labelTopPtr = reinterpret_cast<const c8*>(userWork);
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  wchar_t     *workStr = gfl2::debug::DebugWin_GetWorkStr();
  int val = pGameManager->GetFieldmap()->GetMoveModelManager()->GetCountOfDrawingCullingTargetModel();
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), val);
  return workStr;
}


//=============================================================================
//  リボンデータ
//=============================================================================
//----------------------------------------------------------------------------------
/**
 * @brief	リボンをつけなおす（リボンデータは指定して削除できないのですべて書き直す）
 *
 * @param   poke		  PokemonParam
 * @param   ribbon_id	リボンID
 * @param   flag		  つけるか消すか
 *
 */
//----------------------------------------------------------------------------------
static void setRibbonFlag( pml::pokepara::PokemonParam *poke, u32 ribbon_id, bool flag )
{
  s8 ribbon_array[PokeTool::RIBBON_NO_MAX];

  // リボン所持情報を37番目まで取得
  for( int i=0; i<PokeTool::RIBBON_NO_MAX; i++ ){
		// まとめリボンは別取得
		if( i == PokeTool::RIBBON_NO_LUMPING_CONTEST ){
			ribbon_array[i] = poke->GetLumpingRibbon( pml::pokepara::LUMPING_RIBBON_A );
		}else if( i == PokeTool::RIBBON_NO_LUMPING_TOWER ){
			ribbon_array[i] = poke->GetLumpingRibbon( pml::pokepara::LUMPING_RIBBON_B );
		}else{
			ribbon_array[i] = poke->HaveRibbon(i);
		}
  }

  // リボン全消去
  poke->RemoveAllRibbon();

  // 所持情報を一斉変更
	if( ribbon_id == PokeTool::RIBBON_NO_LUMPING_CONTEST ||
			ribbon_id == PokeTool::RIBBON_NO_LUMPING_TOWER ){
		// まとめリボンは所持数が増減する
		int plus = 1;
		if( flag == false ){
			plus = -1;
		}
		ribbon_array[ ribbon_id ] += plus;
		if( ribbon_array[ribbon_id] < 0 ){
			ribbon_array[ribbon_id] = 0;
		}
		if( ribbon_id == PokeTool::RIBBON_NO_LUMPING_CONTEST ){
			if( ribbon_array[ribbon_id] > PokeTool::RIBBON_COMPLETE_LUMPING_CONTEST ){
				ribbon_array[ribbon_id] = PokeTool::RIBBON_COMPLETE_LUMPING_CONTEST;
			}
		}else if( ribbon_id==PokeTool::RIBBON_NO_LUMPING_TOWER ){
			if( ribbon_array[ribbon_id] > PokeTool::RIBBON_COMPLETE_LUMPING_TOWER ){
				ribbon_array[ribbon_id] = PokeTool::RIBBON_COMPLETE_LUMPING_TOWER;
			}
		}
	}else{
		// 普通のリボンはON/OFFのみの管理
		if( flag == true ){
			ribbon_array[ribbon_id] = 1;
		}else{
			ribbon_array[ribbon_id] = 0;
		}
	}

	// リボン状況を反映させる
	for( int i=0; i<PokeTool::RIBBON_NO_MAX; i++ ){
		if( ribbon_array[i] == 0 ){
			continue;
		}
		if( i == PokeTool::RIBBON_NO_LUMPING_CONTEST ){
			poke->SetLumpingRibbon( pml::pokepara::LUMPING_RIBBON_A, ribbon_array[i] );
		}else if( i == PokeTool::RIBBON_NO_LUMPING_TOWER ){
			poke->SetLumpingRibbon( pml::pokepara::LUMPING_RIBBON_B, ribbon_array[i] );
		}else{
			poke->SetRibbon(i);
		}
	}
}

/* ------------------------------------------------------------------------- */
/**
 * @brief リボンリスト キー処理
 *
 * @param userWork
 * @param item
 */
/* ------------------------------------------------------------------------- */
static void DebugWinFunc_RibbonList( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>( userWork );

  GameSys::GameManager *p_gman = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  const gfl2::ui::Button*   pBTN  = p_gman->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  GameSys::GameData* game_data   = p_gman->GetGameData();
  pml::PokeParty* party             = game_data->GetPlayerParty();
  pml::pokepara::PokemonParam* poke = party->GetMemberPointer(0);

  if( pBTN->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    setRibbonFlag( poke, pUserWork->labelId, 1 );
  }
  else if( pBTN->IsRepeat( gfl2::ui::DIR_LEFT ) )
  {
    setRibbonFlag( poke, pUserWork->labelId, 0 );
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief リボン 描画
 *
 * @param userWork
 * @param item
 */
/* ------------------------------------------------------------------------- */
static wchar_t* DebugWinFunc_RibbonListDraw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>( userWork );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  GameSys::GameManager *p_gman = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  GameSys::GameData* game_data   = p_gman->GetGameData();
  pml::PokeParty* party             = game_data->GetPlayerParty();
  pml::pokepara::PokemonParam* poke = party->GetMemberPointer(0);

  int val;
	if( pUserWork->labelId == PokeTool::RIBBON_NO_LUMPING_CONTEST ){
		val = poke->GetLumpingRibbon( pml::pokepara::LUMPING_RIBBON_A );		// まとめリボン
	}else if( pUserWork->labelId == PokeTool::RIBBON_NO_LUMPING_TOWER ){
		val = poke->GetLumpingRibbon( pml::pokepara::LUMPING_RIBBON_B );		// まとめリボン
	}else{
		val = poke->HaveRibbon( pUserWork->labelId );					// 通常リボン
	}

	swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), val );

	return workStr;
}


//------------------------------------------------------------------------------
/**
 * @func DebugWin_DebugPokemonAdd
 */
//------------------------------------------------------------------------------
static void DebugWin_DebugPokemonAdd( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  //gfl2::debug::DebugWinItem *item 使うな！！
//  ::Debug::DebugWin::DebugWinWork *debWin = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
  GameSys::GameManager *p_gman = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Field::Debug::EventDebugPokeMake::CallDebugPokeAdd( p_gman, true );
    gfl2::debug::DebugWin_CloseWindow();
  }
#endif
}

static void UpdateDebugPokemonEdit(void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  // Aボタンが押されたら
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  pml::PokeParty* party = pGameManager->GetGameData()->GetPlayerParty();
  u32 party_num = party->GetMemberCount();

  u32 index = Field::Debug::EventDebugPokeMake::DebugPokeEditIndex;
  if( index >= party_num ){
    index = 0;
  }

  if( cross->IsRepeat( gfl2::ui::DIR_LEFT ) ){
    index = (index+(party_num-1))%party_num;
  }else if( cross->IsRepeat( gfl2::ui::DIR_RIGHT ) ){
    index = (index+1)%party_num;
  }
  Field::Debug::EventDebugPokeMake::DebugPokeEditIndex = index;

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    Field::Debug::EventDebugPokeMake::CallDebugPokeEdit( pGameManager, index, true );
    gfl2::debug::DebugWin_CloseWindow();
  }
#endif
}

static wchar_t* DrawDebugPokemonEdit( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), Field::Debug::EventDebugPokeMake::DebugPokeEditIndex);
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), Field::Debug::EventDebugPokeMake::DebugPokeEditIndex);
#endif
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugPokemonDel(void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  // Aボタンが押されたら
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  pml::PokeParty* party = pGameManager->GetGameData()->GetPlayerParty();
  u32 party_num = party->GetMemberCount();
  u32 battle_num = 0;


  u32 index = Field::Debug::EventDebugPokeMake::DebugPokeEditIndex;
  if( index >= party_num ){
    index = 0;
  }

  if( cross->IsRepeat( gfl2::ui::DIR_LEFT ) ){
    index = (index+(party_num-1))%party_num;
  }else if( cross->IsRepeat( gfl2::ui::DIR_RIGHT ) ){
    index = (index+1)%party_num;
  }
  Field::Debug::EventDebugPokeMake::DebugPokeEditIndex = index;
  battle_num = party->GetMemberCountEx( pml::PokeParty::POKE_COUNT_TYPE_BATTLE_ENABLE, 0x01<<index );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    if( battle_num >= 1 ){
      party->RemoveMember(index);
    }
  }
#endif
}

static wchar_t* DrawDebugPokemonDel( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  GameSys::GameManager* pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  pml::PokeParty* party = pGameManager->GetGameData()->GetPlayerParty();
  u32 party_num = party->GetMemberCountEx( pml::PokeParty::POKE_COUNT_TYPE_BATTLE_ENABLE, 0x01<<Field::Debug::EventDebugPokeMake::DebugPokeEditIndex );

  if( party_num < 1){
    ::Debug::DebugWin::DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
    GFL_ASSERT( debWork );
    gfl2::str::MsgData * pMsgData = debWork->GetMessageData();
    gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
    pMsgData->GetString( msg_dmenu_pokemon_cannot_battle, *workStrBuf );
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]%ls", item->GetName(), Field::Debug::EventDebugPokeMake::DebugPokeEditIndex, workStrBuf->GetPtr());  
    return workStr;
  }
#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), Field::Debug::EventDebugPokeMake::DebugPokeEditIndex);
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), Field::Debug::EventDebugPokeMake::DebugPokeEditIndex);
#endif
  return workStr;
#else
  return NULL;
#endif
}

// DrawTag、地形Highモデル項目表示
static wchar_t* Draw_DrawTagNumTableForTerrainHighModel( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu::BLOCK_INFO* pDrawTagNumTable = static_cast<FieldDebugMenu::BLOCK_INFO*>( userWork );
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L" %ls[%02d_%02d,%03d][%02d_%02d,%03d][%02d_%02d,%03d][%02d_%02d,%03d]", item->GetName(), 
    pDrawTagNumTable[0].x, pDrawTagNumTable[0].z, pDrawTagNumTable[0].drawTagNum, 
    pDrawTagNumTable[1].x, pDrawTagNumTable[1].z, pDrawTagNumTable[1].drawTagNum, 
    pDrawTagNumTable[2].x, pDrawTagNumTable[2].z, pDrawTagNumTable[2].drawTagNum, 
    pDrawTagNumTable[3].x, pDrawTagNumTable[3].z, pDrawTagNumTable[3].drawTagNum
  );
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L" %ls[%02d_%02d,%03d][%02d_%02d,%03d][%02d_%02d,%03d][%02d_%02d,%03d]", item->GetName(), 
    pDrawTagNumTable[0].x, pDrawTagNumTable[0].z, pDrawTagNumTable[0].drawTagNum, 
    pDrawTagNumTable[1].x, pDrawTagNumTable[1].z, pDrawTagNumTable[1].drawTagNum, 
    pDrawTagNumTable[2].x, pDrawTagNumTable[2].z, pDrawTagNumTable[2].drawTagNum, 
    pDrawTagNumTable[3].x, pDrawTagNumTable[3].z, pDrawTagNumTable[3].drawTagNum
  );
#endif

  return workStr;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  BOX埋めるサブ
 */
//-----------------------------------------------------------------------------
#if GFL_DEBUGWIN_USE
static void fill_poke_1box( Savedata::BoxPokemon *pBoxPoke, int tray, GameSys::GameData *game_data, gfl2::heap::HeapBase *heap, int startnum )
{
  Savedata::MyStatus *mystatus = game_data->GetPlayerStatus();
  u32 myId = mystatus->GetID();
  pml::pokepara::PokemonParam *core  = GFL_NEW(heap) pml::pokepara::PokemonParam( heap, MONSNO_HUSIGIDANE, 50, myId );

  for(int i=0;i<Savedata::BoxPokemon::TRAY_POKE_MAX;i++){

    MonsNo monsno = static_cast<MonsNo>(i + startnum);
    if(monsno>=MONSNO_TAMAGO){
      monsno = static_cast<MonsNo> (i+1);
    }
    core->ChangeMonsNo( monsno, FORMNO_HUSIGIBANA_NORMAL );
    core->SetGetBall( item::BALLID_MONSUTAABOORU );
    if(((1+i) % 3) == 0)
    {
      core->SetItem(1+i);
    }
    core->SetDefaultWaza();
/*
    if(monsno==101){
      // タマゴフラグをたてる
      core->SetEggFlag();
      // ニックネームを "タマゴ" にする
      gfl2::str::StrBuf nickname( pml::MONS_NAME_LENGTH + pml::EOM_LENGTH, heap );
      pml::personal::GetMonsName( &nickname, MONSNO_TAMAGO );
      core->SetNickName( &nickname );
    }
*/
    //親名設定
    mystatus->SetPokemonParent( core, heap );
    core->SetCassetteVersion( System::GetVersion() );
    core->SetLangId( System::GetLang() );
    core->SetDebugEditFlag(true);
    PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(heap,core,PokeTool::PokeGetUtil::CallerType::HOKAKU);
    pBoxPoke->PutPokemon( core, tray, i, heap );
  }

  GFL_DELETE core;

}
#endif

static void UpdateDebugPokemonBoxFull(void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
	Savedata::BOX *pBox             = game_data->GetBOX();
  Savedata::BoxPokemon *pBoxPoke  = game_data->GetBoxPokemon();
	gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    for(int i=0;i<Savedata::BoxPokemon::TRAY_MAX;i++){
     fill_poke_1box( pBoxPoke, i, game_data, workHeap , i * Savedata::BoxPokemon::TRAY_POKE_MAX + 1);
    }
    pBoxPoke->UpdateTrayMax(workHeap);
  }

#endif
}

static wchar_t* DrawDebugPokemonBoxFull( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName());
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName());
#endif
  return workStr;
#else
  return NULL;
#endif
}


static void UpdateDebugPokemonBoxRandom(void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
	Savedata::BOX *pBox             = game_data->GetBOX();
  Savedata::BoxPokemon *pBoxPoke  = game_data->GetBoxPokemon();
	gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  gfl2::math::Random rand;

  rand.Initialize();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    for(int i = 1; i < 4;i++){  //3boxだけ
      fill_poke_1box( pBoxPoke, i, game_data, workHeap, rand.Next() % (MONSNO_MAX-31) );
    }
    pBoxPoke->UpdateTrayMax(workHeap);
  }

#endif
}

static wchar_t* DrawDebugPokemonBoxRandom( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

#if defined(GF_PLATFORM_WIN32)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName());
#elif defined(GF_PLATFORM_CTR)
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, item->GetName());
#endif
  return workStr;
#else
  return NULL;
#endif
}


static int fieldDebugMenuBattleTeamOK = 1;
static int fieldDebugMenuBattleTeamNG = 2;

static void SetPokemonForPlace( pml::pokepara::CoreParam* cp, bool isParty, int battleTeamNo, int pos )
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  GameSys::GameData * gd = gm->GetGameData();
  gfl2::heap::HeapBase* workHeap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

  if( isParty )
  {
    pml::PokeParty* party = gd->GetPlayerParty();

    pml::pokepara::PokemonParam* pp = GFL_NEW( workHeap ) pml::pokepara::PokemonParam( workHeap, *cp );
    if( pos < party->GetMemberCount() )
    {
      pml::pokepara::PokemonParam* partyPoke = party->GetMemberPointer(pos);
      partyPoke->CopyFrom( *pp );
    }
    else 
    {
      party->AddMember(*pp);
    }
    GFL_DELETE pp;
  }
  else
  {
    Savedata::BOX * sv_box = gd->GetBOX();
    Savedata::BoxPokemon * sv_box_pokemon = gd->GetBoxPokemon();

    u32 teamNo = battleTeamNo;
 
    //BOXにいれて、バトルチーム登録
    u16 boxPos = 0;
    u32 boxTray = sv_box_pokemon->PutPokemonEmptyTrayAll( cp, 0, workHeap, &boxPos );
    if( boxTray != Savedata::BoxPokemon::PUT_POKEMON_ERROR )
    {
      sv_box->SetTeamPokePos( teamNo, pos, ((boxTray & 0xFF) << 8) | (boxPos & 0xFF)  );
    }
  }
}

// バトルチーム追加（正常）
static void UpdateDebugPokemonBattleTeam(void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
	Savedata::BOX *pBox             = game_data->GetBOX();
  Savedata::BoxPokemon *pBoxPoke  = game_data->GetBoxPokemon();
  Savedata::MyStatus *myStatus    = game_data->GetPlayerStatus();
  Savedata::ZukanData* zukan      = game_data->GetZukanData();
	gfl2::heap::HeapBase* workHeap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) == true )
  {
    if( --fieldDebugMenuBattleTeamOK < 0 )
    {
      fieldDebugMenuBattleTeamOK = 6;
    }
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) == true )
  {
    if( ++fieldDebugMenuBattleTeamOK > 6 )
    {
      fieldDebugMenuBattleTeamOK = 0;
    }
  }
  
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    if( pBoxPoke->GetSpaceCountAll( workHeap ) < 6 )
    {
      item->PlaySystemSound(gfl2::debug::SE_ERROR);
      GFL_PRINT("!!!BOXに空きがない\n");
      return;
    }
    pml::pokepara::PokemonParam* pp;

    bool isParty = (fieldDebugMenuBattleTeamOK==0);
    const u32 teamNo = fieldDebugMenuBattleTeamOK-1;   // チーム番号

    wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"DebugOK[%d]", teamNo+1 );
    if( !isParty )
    {
      pBox->SetTeamName( workStr, teamNo );
    }

    //  ジュナイパー
    {
      pp = GFL_NEW( workHeap ) pml::pokepara::PokemonParam( workHeap, MONSNO_HUKUROU3, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
      pp->SetTokuseiIndex( 0 );
      pp->SetWaza(0, WAZANO_RIIHUBUREEDO);
      pp->SetWaza(1, WAZANO_HUIUTI);
      pp->SetWaza(2, WAZANO_TURUGINOMAI);
      pp->SetWaza(3, WAZANO_FEZAADANSU);
      pp->ChangeSeikaku( pml::pokepara::SEIKAKU_IJIPPARI );
      for( int i=0; i<pml::pokepara::POWER_NUM; i++ ){
        pp->ChangeTalentPower( static_cast<pml::pokepara::PowerID>(i), 31 );
      }
      pp->ChangeEffortPower( pml::pokepara::POWER_HP, 180 );
      pp->ChangeEffortPower( pml::pokepara::POWER_ATK, 252 );
      pp->ChangeEffortPower( pml::pokepara::POWER_DEF, 4 );
      pp->ChangeEffortPower( pml::pokepara::POWER_AGI, 62 );
      pp->ChangeEffortPower( pml::pokepara::POWER_SPDEF, 12 );
      pp->SetLangId(System::GetLang());
      pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
      pp->SetCassetteVersion( System::GetVersion() );
      pp->SetDebugEditFlag(true);
      PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(workHeap, pp, PokeTool::PokeGetUtil::CallerType::HOKAKU);
      myStatus->SetPokemonParent( pp, workHeap );
      zukan->SetPokeSee(*pp);
      zukan->SetPokeGet(*pp);
      SetPokemonForPlace( pp, isParty, teamNo, 0 );

      GFL_DELETE pp;
    }
    //  ガオガエン
    {
      pp = GFL_NEW( workHeap ) pml::pokepara::PokemonParam( workHeap, MONSNO_TORA3, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
      pp->SetTokuseiIndex( 0 );
      pp->SetWaza(0, WAZANO_KAENHOUSYA);
      pp->SetWaza(1, WAZANO_KUROSUTYOPPU);
      pp->SetWaza(2, WAZANO_ONIBI);
      pp->SetWaza(3, WAZANO_ITYAMON);
      pp->ChangeSeikaku( pml::pokepara::SEIKAKU_HIKAEME );
      for( int i=0; i<pml::pokepara::POWER_NUM; i++ ){
        pp->ChangeTalentPower( static_cast<pml::pokepara::PowerID>(i), 31 );
      }
      pp->ChangeEffortPower( pml::pokepara::POWER_HP, 244 );
      pp->ChangeEffortPower( pml::pokepara::POWER_DEF, 6 );
      pp->ChangeEffortPower( pml::pokepara::POWER_AGI, 4 );
      pp->ChangeEffortPower( pml::pokepara::POWER_SPATK, 252 );
      pp->ChangeEffortPower( pml::pokepara::POWER_SPDEF, 4 );
      pp->SetLangId(System::GetLang());
      pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
      pp->SetCassetteVersion( System::GetVersion() );
      pp->SetDebugEditFlag(true);
      PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(workHeap, pp, PokeTool::PokeGetUtil::CallerType::HOKAKU);
      myStatus->SetPokemonParent( pp, workHeap );
      zukan->SetPokeSee(*pp);
      zukan->SetPokeGet(*pp);
      SetPokemonForPlace( pp, isParty, teamNo, 1 );
      GFL_DELETE pp;
    }
    //  アシレーヌ
    {
      pp = GFL_NEW( workHeap ) pml::pokepara::PokemonParam( workHeap, MONSNO_ASIKA3, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
      pp->SetTokuseiIndex( 0 );
      pp->SetWaza(0, WAZANO_NAMINORI);
      pp->SetWaza(1, WAZANO_REITOUBIIMU);
      pp->SetWaza(2, WAZANO_SAIKOKINESISU);
      pp->SetWaza(3, WAZANO_KAGEBUNSIN);
      pp->ChangeSeikaku( pml::pokepara::SEIKAKU_OKUBYOU );
      for( int i=0; i<pml::pokepara::POWER_NUM; i++ ){
        pp->ChangeTalentPower( static_cast<pml::pokepara::PowerID>(i), 31 );
      }
      pp->ChangeEffortPower( pml::pokepara::POWER_HP, 252 );
      pp->ChangeEffortPower( pml::pokepara::POWER_AGI, 6 );
      pp->ChangeEffortPower( pml::pokepara::POWER_SPDEF, 252 );
      pp->SetLangId(System::GetLang());
      pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
      pp->SetCassetteVersion( System::GetVersion() );
      pp->SetDebugEditFlag(true);
      PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(workHeap, pp, PokeTool::PokeGetUtil::CallerType::HOKAKU);
      myStatus->SetPokemonParent( pp, workHeap );
      zukan->SetPokeSee(*pp);
      zukan->SetPokeGet(*pp);
      SetPokemonForPlace( pp, isParty, teamNo, 2 );
      GFL_DELETE pp;
    }
    //  ドデカバシ
    {
      pp = GFL_NEW( workHeap ) pml::pokepara::PokemonParam( workHeap, MONSNO_KUTIBASI3, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
      pp->SetTokuseiIndex( 0 );
      pp->SetWaza(0, WAZANO_SORAWOTOBU);
      pp->SetWaza(1, WAZANO_ONGAESI);
      pp->SetWaza(2, WAZANO_OUMUGAESI);
      pp->SetWaza(3, WAZANO_HAGANENOTUBASA);
      pp->ChangeSeikaku( pml::pokepara::SEIKAKU_YOUKI );
      for( int i=0; i<pml::pokepara::POWER_NUM; i++ ){
        pp->ChangeTalentPower( static_cast<pml::pokepara::PowerID>(i), 31 );
      }
      pp->ChangeEffortPower( pml::pokepara::POWER_ATK, 246 );
      pp->ChangeEffortPower( pml::pokepara::POWER_AGI, 252 );
      pp->ChangeEffortPower( pml::pokepara::POWER_SPDEF, 12 );
      pp->SetLangId(System::GetLang());
      pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
      pp->SetCassetteVersion( System::GetVersion() );
      pp->SetDebugEditFlag(true);
      PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(workHeap, pp, PokeTool::PokeGetUtil::CallerType::HOKAKU);
      myStatus->SetPokemonParent( pp, workHeap );
      zukan->SetPokeSee(*pp);
      zukan->SetPokeGet(*pp);
      SetPokemonForPlace( pp, isParty, teamNo, 3 );
      GFL_DELETE pp;
    }
    // クラガノン
    {
      pp = GFL_NEW( workHeap ) pml::pokepara::PokemonParam( workHeap, MONSNO_KUWAGATA3, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
      pp->SetTokuseiIndex( 0 );
      pp->SetWaza(0, WAZANO_10MANBORUTO);
      pp->SetWaza(1, WAZANO_MUSINOSAZAMEKI);
      pp->SetWaza(2, WAZANO_EASURASSYU);
      pp->SetWaza(3, WAZANO_KAMIKUDAKU);
      pp->ChangeSeikaku( pml::pokepara::SEIKAKU_REISEI );
      for( int i=0; i<pml::pokepara::POWER_NUM; i++ ){
        pp->ChangeTalentPower( static_cast<pml::pokepara::PowerID>(i), 31 );
      }
      pp->ChangeEffortPower( pml::pokepara::POWER_ATK, 244 );
      pp->ChangeEffortPower( pml::pokepara::POWER_DEF, 10 );
      pp->ChangeEffortPower( pml::pokepara::POWER_SPATK, 252 );
      pp->ChangeEffortPower( pml::pokepara::POWER_SPDEF, 4 );
      pp->SetLangId(System::GetLang());
      pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
      pp->SetCassetteVersion( System::GetVersion() );
      pp->SetDebugEditFlag(true);
      PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(workHeap, pp, PokeTool::PokeGetUtil::CallerType::HOKAKU);
      myStatus->SetPokemonParent( pp, workHeap );
      zukan->SetPokeSee(*pp);
      zukan->SetPokeGet(*pp);
      SetPokemonForPlace( pp, isParty, teamNo, 4 );
      GFL_DELETE pp;
    }
    //  バンバドロ
    {
      pp = GFL_NEW( workHeap ) pml::pokepara::PokemonParam( workHeap, MONSNO_ROBA2, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
      pp->SetTokuseiIndex( 0 );
      pp->SetWaza(0, WAZANO_SUTEMITAKKURU);
      pp->SetWaza(1, WAZANO_ZISIN);
      pp->SetWaza(2, WAZANO_IWANADARE);
      pp->SetWaza(3, WAZANO_IBARU);
      pp->ChangeSeikaku( pml::pokepara::SEIKAKU_NONKI );
      for( int i=0; i<pml::pokepara::POWER_NUM; i++ ){
        pp->ChangeTalentPower( static_cast<pml::pokepara::PowerID>(i), 31 );
      }
      pp->ChangeEffortPower( pml::pokepara::POWER_HP, 252 );
      pp->ChangeEffortPower( pml::pokepara::POWER_DEF, 252 );
      pp->ChangeEffortPower( pml::pokepara::POWER_SPDEF, 4 );
      pp->SetLangId(System::GetLang());
      pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
      pp->SetCassetteVersion( System::GetVersion() );
      pp->SetDebugEditFlag(true);
      PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(workHeap, pp, PokeTool::PokeGetUtil::CallerType::HOKAKU);
      myStatus->SetPokemonParent( pp, workHeap );
      zukan->SetPokeSee(*pp);
      zukan->SetPokeGet(*pp);
      SetPokemonForPlace( pp, isParty, teamNo, 5 );
      GFL_DELETE pp;
    }

    item->PlaySystemSound(gfl2::debug::SE_ACCEPT);
  }

#endif
}

static wchar_t* DrawDebugPokemonBattleTeam( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  static const wchar_t * PLACE[] =
  {
    L"Party",
    L"Battle1",
    L"Battle2",
    L"Battle3",
    L"Battle4",
    L"Battle5",
    L"Battle6",
  };

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), PLACE[fieldDebugMenuBattleTeamOK] );
  return workStr;
#else
  return NULL;
#endif
}

// バトルチーム追加（レギュレーション違反）
static void UpdateDebugPokemonBattleTeamNg(void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
	Savedata::BOX *pBox             = game_data->GetBOX();
  Savedata::BoxPokemon *pBoxPoke  = game_data->GetBoxPokemon();
  Savedata::MyStatus *myStatus    = game_data->GetPlayerStatus();
  Savedata::ZukanData* zukan      = game_data->GetZukanData();
	gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) == true )
  {
    if( --fieldDebugMenuBattleTeamNG < 0 )
    {
      fieldDebugMenuBattleTeamNG = 6;
    }
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) == true )
  {
    if( ++fieldDebugMenuBattleTeamNG > 6 )
    {
      fieldDebugMenuBattleTeamNG = 0;
    }
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    if( pBoxPoke->GetSpaceCountAll( workHeap ) < 6 )
    {
      item->PlaySystemSound(gfl2::debug::SE_ERROR);
      GFL_PRINT("!!!BOXに空きがない\n");
      return;
    }
    pml::pokepara::PokemonParam* pp;
    // トレイ
    bool isParty = (fieldDebugMenuBattleTeamNG==0);
    const u32 teamNo = fieldDebugMenuBattleTeamNG-1;   // チーム番号

    if( !isParty )
    {
      wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"DebugNg[%d]", fieldDebugMenuBattleTeamNG+1 );
      pBox->SetTeamName( workStr, teamNo );
    }

    //  フラエッテ破壊フォルム
    {
      pp = GFL_NEW( workHeap ) pml::pokepara::PokemonParam( workHeap, MONSNO_HURAETTE, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
      pp->ChangeFormNo( FORMNO_HURAETTE_HAKAI );
      pp->SetDefaultWaza();
      pp->SetItem( ITEM_KOKORONOSIZUKU );
      pp->SetLangId(System::GetLang());
      pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
      pp->SetCassetteVersion( VERSION_Y );
      pp->SetDebugEditFlag(true);
      myStatus->SetPokemonParent(pp, workHeap);
      zukan->SetPokeSee(*pp);
      zukan->SetPokeGet(*pp);
      SetPokemonForPlace( pp, isParty, teamNo, 0 );
      GFL_DELETE pp;
    }
    //  ミュウツー
    //{
    //  pp = GFL_NEW( workHeap ) pml::pokepara::PokemonParam( workHeap, MONSNO_MYUUTUU, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
    //  pp->SetDefaultWaza();
    //  pp->SetItem( ITEM_MYUUTUNAITOx );
    //  pp->SetLangId(System::GetLang());
    //  pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
    //  pp->SetCassetteVersion( VERSION_X );
    //  myStatus->SetPokemonParent( pp, workHeap );
    //  zukan->SetPokeSee(*pp);
    //  zukan->SetPokeGet(*pp);
    //  SetPokemonForPlace( pp, isParty, teamNo, 0 );
    //  GFL_DELETE pp;
    //}
    item->PlaySystemSound(gfl2::debug::SE_ACCEPT);
  }

#endif
}

static wchar_t* DrawDebugPokemonBattleTeamNg( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  static const wchar_t * PLACE[] =
  {
    L"Party",
    L"Battle1",
    L"Battle2",
    L"Battle3",
    L"Battle4",
    L"Battle5",
    L"Battle6",
  };

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), PLACE[fieldDebugMenuBattleTeamNG] );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugPokemonSetDameTamago(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  //gfl2::ui::Button *pButton = gfl2::debug::DebugWin_GetUiButton();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 member_count = party->GetMemberCount();
    if( member_count > 0)
    { // 手持ちがある場合のみ有効
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(member_count - 1);
      poke->SetFuseiTamagoFlag();
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

// バトルチームにタマゴをセット
static void UpdateDebugPokemonSetBattleTeamTamago( void * userWork, gfl2::debug::DebugWinItem * item )
{
  GFL_UNUSED( userWork );
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::Button * button = gm->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameData * gd = gm->GetGameData();
    Savedata::BOX * sv_box = gd->GetBOX();
    Savedata::BoxPokemon * sv_box_pokemon = gd->GetBoxPokemon();
    gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

    for( u32 i=0; i<Savedata::BOX::TEAM_MAX; i++ )
    {
      if( sv_box->IsTeamPokeIn( i ) != false )
      {
        u16 index = sv_box->GetTeamPokePos( i, 0 );
        u8 tray = ( index >> 8 ) & 0xff;
        u8 pos  = ( index & 0xff );
        pml::pokepara::CoreParam * cp = sv_box_pokemon->GetPokemon( tray, pos, heap );
        cp->ChangeEgg( heap );
        sv_box_pokemon->UpdatePokemonConst( cp, tray, pos );
        GFL_DELETE cp;
      }
    }
  }
#endif //GFL_DEBUGWIN_USE
}

// バトルチームにダメタマゴをセット
static void UpdateDebugPokemonSetBattleTeamDameTamago( void * userWork, gfl2::debug::DebugWinItem * item )
{
  GFL_UNUSED( userWork );
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::Button * button = gm->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameData * gd = gm->GetGameData();
    Savedata::BOX * sv_box = gd->GetBOX();
    Savedata::BoxPokemon * sv_box_pokemon = gd->GetBoxPokemon();
    gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

    for( u32 i=0; i<Savedata::BOX::TEAM_MAX; i++ )
    {
      if( sv_box->IsTeamPokeIn( i ) != false )
      {
        u16 index = sv_box->GetTeamPokePos( i, 0 );
        u8 tray = ( index >> 8 ) & 0xff;
        u8 pos  = ( index & 0xff );
        pml::pokepara::CoreParam * cp = sv_box_pokemon->GetPokemon( tray, pos, heap );
        cp->SetFuseiTamagoFlag();
        sv_box_pokemon->UpdatePokemonConst( cp, tray, pos );
        GFL_DELETE cp;
      }
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 *  @brief    ポケモンを不正メガ化する
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalMega(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 member_count = party->GetMemberCount();
    for ( int index = 0; index < member_count; ++ index ) {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      if( poke->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false ){
        if ( poke->GetMegaFormCount() ){
          FormNo form = 1;  //default
          poke->ChangeFormNo( form );
          poke->SetDebugEditFlag(true);
        }
      }
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：最大HP上昇
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalMaxhp(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 member_count = party->GetMemberCount();
    for ( int index = 0; index < member_count; ++ index ) {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      if( poke->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false ){
        u32 max_hp = poke->GetMaxHp();
        poke->SetMaxHp( max_hp + 100 );
        poke->SetDebugEditFlag(true);
        break;  //先頭1匹だけ不正にする
      }
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：レベル111
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalLevel(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetLevel( 111 );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：攻撃999
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalAtkValue(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetDebugEditFlag(true);
      poke->SetAtk(999);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：ニックネーム空
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetEmptyNickName(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
      gfl2::str::StrBuf str( 20, workHeap );

      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetNickName( &str );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：ニックネーム不正文字
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalNickNameChar(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::PokeParty* party = game_data->GetPlayerParty();
      u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
      if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
      {
        gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
        gfl2::str::StrBuf *str = GFL_NEW( workHeap ) gfl2::str::StrBuf( L"不正な名前", workHeap );

        pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
        poke->SetNickName( str );
        poke->SetDebugEditFlag(true);
        GFL_DELETE str;
      }
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：ニックネームEOMなし
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetNoEomNickName(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->RemoveNickNameEom();
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：親名空
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetEmptyOyaName(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
      gfl2::str::StrBuf str( 20, workHeap );

      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetParentName( &str );
      poke->SetDebugEditFlag(true);
    }

  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：親名不正文字
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalOyaNameChar(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
      gfl2::str::StrBuf *str = GFL_NEW( workHeap ) gfl2::str::StrBuf( L"不正な親名", workHeap );

      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetParentName( str );
      poke->SetDebugEditFlag(true);
      GFL_DELETE str;

    }

  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：親名EOMなし
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetNoEomOyaName(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->RemoveOyaNameEom();
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：過去親名EOMなし
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetNoEomPastOyaName(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->RemovePastOyaNameEom();
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：親名13文字
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSet13CharOyaName(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetOyaName13Chars();
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：ニックネーム13文字
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSet13CharNickName(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetNickName13Chars();
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}


//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：努力値合計510オーバー
 */
//-----------------------------------------------------------------------------

static void UpdateDebugPokemonSetIllegalBasePoint(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->ChangeEffortPowerDirect( pml::pokepara::POWER_HP, 252 );
      poke->ChangeEffortPowerDirect( pml::pokepara::POWER_SPDEF, 252 );
      poke->ChangeEffortPowerDirect( pml::pokepara::POWER_AGI, 252 );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：わざナンバーオーバー
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalWazaNo(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetWazaNoDirect( 0, static_cast<WazaNo>(WAZANO_MAX + 1) );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：せいかくナンバーオーバー
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalSeikakuNo(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->ChangeSeikaku( pml::pokepara::SEIKAKU_NUM );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：性別強制セット:♂
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetForceMale(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->ChangeSexDirect( pml::SEX_MALE );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：性別強制セット:♀
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetForceFemale(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->ChangeSexDirect( pml::SEX_FEMALE );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：性別強制セット:中性
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetForceNeutral(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->ChangeSexDirect( pml::SEX_UNKNOWN );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：ボールIDナンバーオーバー
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalBallID(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetGetBall( item::BALLID_MAX + 3 );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：ポケモンナンバーオーバー
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalMonsNo(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetMonsNoDirect( static_cast<MonsNo>(MONSNO_MAX + 3) );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：不正な経験値
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalExp(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    static const u32 ILLEGAL_EXP[] = 
    {
      600001,
      800001,
      1000001,
      1059861,
      1250001,
      1640001,
    };
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 member_count = party->GetMemberCount();
    for ( int index = 0; index < member_count; ++ index ) {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      if( poke->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false ){
        poke->SetExpDirect( ILLEGAL_EXP[index] );
        poke->SetDebugEditFlag(true);
      }
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：不正な道具
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalItem(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetItem( ITEM_DATA_MAX + 1 );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：不正な地名ID　捕まえた場所
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalTakenPlace(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_PLACE, 59000 );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモン不正化：不正な地名ID　産まれた場所
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonSetIllegalContactPlace(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 59000 );
      poke->SetDebugEditFlag(true);
    }
  }
#endif //GFL_DEBUGWIN_USE
}

static int PokemonPartyIndex = 0;
//-----------------------------------------------------------------------------
/**
 * @brief ポケモンバイナリ表示
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonDumpPokemonPrintBinary(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  int slot = *((int*)userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    u8 buffer[pml::pokepara::CoreParam::DATASIZE];

    pml::PokeParty* party = game_data->GetPlayerParty();
    pml::pokepara::PokemonParam* poke = party->GetMemberPointer( slot );
    poke->Serialize_Core( buffer );
#if 0
    GFL_PRINT("/*\n");
    poke->Dump();
    GFL_PRINT("*/\n");
#endif
    GFL_PRINT("//======== CoreParam Dump Start ========\n");
    for( int i = 0; i < pml::pokepara::CoreParam::DATASIZE; ++i )
    {
      GFL_PRINT("0x%x,", buffer[i]);
      if((i+1)%16==0)
      GFL_PRINT("\n");
    }
    GFL_PRINT("\n//======== CoreParam Dump End ========\n");
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if( --slot < 0 )
    {
      slot = 5;
    }
    *((int*)userWork) = slot;
  }
  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    int slot = *((int*)userWork);
    if( ++slot > 5 )
    {
      slot = 0;
    }
    *((int*)userWork) = slot;
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモンバイナリ出力
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonDumpPokemonOutputBinary(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  int slot = *((int*)userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    u8 buffer[pml::pokepara::CoreParam::DATASIZE];
    char fileName[255];

    pml::PokeParty* party = game_data->GetPlayerParty();
    pml::pokepara::PokemonParam* poke = party->GetMemberPointer( slot );
    poke->Serialize_Core( buffer );

    GameSys::DeviceDate date;
    sprintf(fileName, "../pokemonBinary_%02d%02d_%02d%02d_%d%d.bin", date.GetMonth(), date.GetDay(), date.GetHour(), date.GetMinute(), date.GetSecond(), date.GetMilliSecond() );
    gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    gfl2::fs::AsyncFileManager::FileEasyWriteReq req;
    req.fileName = fileName;
    req.pBuf = &buffer;
    req.bufSize = pml::pokepara::CoreParam::DATASIZE;
    req.heapForReq = workHeap;
    req.hioUseSetting = gfl2::fs::AsyncFileManager:: HioUseSetting::ALWAYS_HIO;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager()->SyncFileEasyWrite( req );
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if( --slot < 0 )
    {
      slot = 5;
    }
    *((int*)userWork) = slot;
  }
  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    int slot = *((int*)userWork);
    if( ++slot > 5 )
    {
      slot = 0;
    }
    *((int*)userWork) = slot;
  }
#endif //GFL_DEBUGWIN_USE
}

static wchar_t* DrawDebugPokemon_PrintPokemonNameInParty( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  pml::PokeParty* party = game_data->GetPlayerParty();
  int slot = *((int*)userWork);
  pml::pokepara::PokemonParam* poke = party->GetMemberPointer( slot );

  gfl2::str::STRCODE nickname[pml::PERSON_NAME_LENGTH + pml::EOM_LENGTH];

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  poke->GetNickName( nickname, GFL_NELEMS(nickname) );
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d:%ls]", item->GetName(), slot, nickname );
  return workStr;
#else
  return NULL;
#endif
}


//-----------------------------------------------------------------------------
/**
 * @brief ポケモンインデックス出力：ポケモン設定
 */
//-----------------------------------------------------------------------------
static void UpdatePokeIndexMake_SetPokemon(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    s32 slot = *((int*)userWork);
    if( --slot < 0 )
    {
      slot = 5;
    }
    *((int*)userWork) = slot;
  }
  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    int slot = *((int*)userWork);
    if( ++slot > 5 )
    {
      slot = 0;
    }
    *((int*)userWork) = slot;
  }
#endif //GFL_DEBUGWIN_USE
}

static wchar_t* DrawPokeIndexMake_SetPokemon( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  pml::PokeParty* party = game_data->GetPlayerParty();
  s32 slot = *((int*)userWork);
  pml::pokepara::PokemonParam* poke = party->GetMemberPointer( slot );

  gfl2::str::STRCODE nickname[pml::PERSON_NAME_LENGTH + pml::EOM_LENGTH];

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  poke->GetNickName( nickname, GFL_NELEMS(nickname) );
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d:%ls]", item->GetName(), slot, nickname );
  return workStr;
#else
  return NULL;
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモンインデックス出力：メソッド
 */
//-----------------------------------------------------------------------------
static void UpdatePokeIndexMake_Method(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  s32 slot = *((int*)userWork);
  const int END_SLOT = 1;
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if( --slot < 0 )
    {
      slot = END_SLOT;
    }
    *((int*)userWork) = slot;
  }
  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    int slot = *((int*)userWork);
    if( ++slot > END_SLOT )
    {
      slot = 0;
    }
    *((int*)userWork) = slot;
  }
#endif //GFL_DEBUGWIN_USE
}

static wchar_t* DrawPokeIndexMake_Method( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  s32 slot = *((int*)userWork);
  static const wchar_t * METHOD[] =
  {
    L"Index+Pokemon(168+232byte)",
    L"Index(168byte)",
  };

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), METHOD[slot] );
  return workStr;
#else
  return NULL;
#endif
}


//-----------------------------------------------------------------------------
/**
 * @brief ポケモンインデックス出力：作成
 */
//-----------------------------------------------------------------------------
static void UpdatePokeIndexMake_Generate(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  Savedata::MyStatus* pMyStatus = game_data->GetPlayerStatus();
  pml::PokeParty* party = game_data->GetPlayerParty();
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  pml::pokepara::PokemonParam* poke = party->GetMemberPointer( pDebug->GetPokeIndexMakePokemonIndex() );
  gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    NetApp::GTS::GtsIndexData gtsIndexData;
    gfl2::std::MemClear( &gtsIndexData, sizeof( NetApp::GTS::GtsIndexData ) );

    gtsIndexData.PokeInfo.kind                = static_cast<u16>(poke->GetMonsNo());
    gtsIndexData.PokeInfo.gender              = poke->GetSex() == pml::SEX_MALE ? 1 : (poke->GetSex() == pml::SEX_FEMALE ? 2 : 0);
    gtsIndexData.PokeInfo.levelBand           = (poke->GetLevel() + 9) / 10;
    gtsIndexData.PokeInfo.requestKind         = pDebug->GetPokemonIndexMakeRequestKind();
    gtsIndexData.PokeInfo.requestGender       = pDebug->GetPokemonIndexMakeRequestGender();
    gtsIndexData.PokeInfo.requestLevelBand    = pDebug->GetPokemonIndexMakeRequestLevelBand();
    gtsIndexData.PokeInfo.requestNoLegend     = pDebug->GetPokemonIndexMakeRequestNoLegend();
    gtsIndexData.PokeInfo.searchDisable       = pDebug->GetPokemonIndexMakeSearchDisable();
    gtsIndexData.PokeInfo.form                = static_cast<u8>(poke->GetFormNo());
    gtsIndexData.PokeInfo.level               = static_cast<u8>(poke->GetLevel());
    poke->GetNickName( reinterpret_cast<gfl2::str::STRCODE*>(gtsIndexData.PokeInfo.nickname), NetApp::GTS::POKEMON_NAME_SIZE );
    gtsIndexData.PokeInfo.item                = poke->GetItem();
    gtsIndexData.PokeInfo.isRare              = static_cast<u8>(poke->IsRare());
    gtsIndexData.PokeInfo.haveNickName        = static_cast<u8>(poke->HaveNickName());
    gtsIndexData.PokeInfo.ballID              = static_cast<u8>(poke->GetGetBall());
    gtsIndexData.PokeInfo.personalRandom      = poke->GetPersonalRnd();
    gtsIndexData.uniqId                       = pDebug->GetPokemonIndexMakeUniqId();//pMyStatus->GetNexUniqueID();
    gtsIndexData.dataStoreId                  = pDebug->GetPokemonIndexMakeDataStoreId();
    gtsIndexData.principalId                  = pDebug->GetPokemonIndexMakePrincibalId();//pMyStatus->GetPrincipalId();
    gfl2::str::StrBuf trainerName( NetApp::GTS::TRAINER_NAME_SIZE, workHeap );
    pMyStatus->GetNameString( &trainerName );
    trainerName.PutStr( reinterpret_cast<gfl2::str::STRCODE*>( gtsIndexData.TrainerName ), trainerName.GetSize() );
    gtsIndexData.messageID                    = pDebug->GetPokemonIndexMakeMessageId();
    gtsIndexData.Country                      = static_cast<u16>( pMyStatus->GetCountry() );
    gtsIndexData.Region                       = static_cast<u16>( pMyStatus->GetArea() );
    gtsIndexData.iconData                     = pMyStatus->GetIcon();
    gtsIndexData.romCode                      = pMyStatus->GetRomCode();
    gtsIndexData.getVersion                   = static_cast<u8>(poke->GetCassetteVersion());
    gtsIndexData.trainerGender                = pMyStatus->GetSex() == pml::SEX_MALE ? 1 : (pMyStatus->GetSex() == pml::SEX_FEMALE ? 2 : 0);
    gtsIndexData.pokemonLanguageCode          = static_cast<u8>( poke->GetLangId() );
    gtsIndexData.languageCode                 = System::GetLang();


    GFL_PRINT("index %d simple %d icon %d\n", sizeof(NetApp::GTS::GtsIndexData), sizeof(NetApp::GTS::SimplePokemonInfo), sizeof(Savedata::MyStatus::ICON_DATA) );
    const u32 bufferSize = sizeof(NetApp::GTS::GtsIndexData) + pml::pokepara::CoreParam::DATASIZE;
    u8 buffer[ bufferSize ];
    gfl2::std::MemClear( &buffer, bufferSize );
    u8 * pokeBuffer = buffer + sizeof(NetApp::GTS::GtsIndexData);
    gfl2::std::MemCopy( &gtsIndexData, (void*)buffer, sizeof(NetApp::GTS::GtsIndexData) );
    poke->Serialize_Core( pokeBuffer );

    GameSys::DeviceDate date;
    char fileName[255];
    sprintf(fileName, "../pokemonIndexBinary_%02d%02d_%02d%02d_%d%d.bin", date.GetMonth(), date.GetDay(), date.GetHour(), date.GetMinute(), date.GetSecond(), date.GetMilliSecond() );
    gfl2::fs::AsyncFileManager::FileEasyWriteReq req;
    req.fileName = fileName;
    req.pBuf = &buffer;
    if( pDebug->GetPokemonIndexMakeMethod() == 0 )
    {
      //Index+poke
      req.bufSize = bufferSize;
    }
    else
    {
      //Index
      req.bufSize = sizeof(NetApp::GTS::GtsIndexData);
    }
    req.heapForReq = workHeap;
    req.hioUseSetting = gfl2::fs::AsyncFileManager:: HioUseSetting::ALWAYS_HIO;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager()->SyncFileEasyWrite( req );
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ニックネームを空に
 */
//-----------------------------------------------------------------------------
static void UpdateDebugMyStatusSetEmptyName(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  Savedata::MyStatus* myStatus = game_data->GetPlayerStatus();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    gfl2::str::StrBuf str( 20, workHeap );

    myStatus->SetNameString( &str );
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ニックネームに不正文字
 */
//-----------------------------------------------------------------------------
static void UpdateDebugMyStatusSetilligalName(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  Savedata::MyStatus* myStatus = game_data->GetPlayerStatus();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    gfl2::str::StrBuf str( L"不正な名前", workHeap );

    myStatus->SetNameString( &str );
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ニックネームEOMなし
 */
//-----------------------------------------------------------------------------
static void UpdateDebugMyStatusSetNoEomName(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  Savedata::MyStatus* myStatus = game_data->GetPlayerStatus();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    myStatus->RemoveNameEom();
  }
#endif //GFL_DEBUGWIN_USE
}

//-----------------------------------------------------------------------------
/**
 * @brief ニックネーム13文字
 */
//-----------------------------------------------------------------------------
static void UpdateDebugMyStatusSet13CharName(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  Savedata::MyStatus* myStatus = game_data->GetPlayerStatus();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    myStatus->SetName13Chars();
  }
#endif //GFL_DEBUGWIN_USE
}

static int debugPokemonForm = 0;
//-----------------------------------------------------------------------------
/**
 * @brief フォルム
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemon_SetIllegalFormToPokemon(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    pml::PokeParty* party = game_data->GetPlayerParty();
    u32 index = party->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG );
    if ( index != pml::PokeParty::MEMBER_INDEX_ERROR )
    {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      poke->SetFormNoDirect( debugPokemonForm );
      poke->SetDebugEditFlag(true);
    }
  }

  const int END_FORM = 31;
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) == true )
  {
    if( --debugPokemonForm < 0 )
    {
      debugPokemonForm = END_FORM;
    }
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) == true )
  {
    if( ++debugPokemonForm >= END_FORM )
    {
      debugPokemonForm = 0;
    }
  }
#endif //GFL_DEBUGWIN_USE
}

static wchar_t* DrawDebugPokemon_SetIllegalFormToPokemon( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  pml::PokeParty* party = game_data->GetPlayerParty();
  pml::pokepara::PokemonParam* poke = party->GetMemberPointer( 0 );

  gfl2::str::STRCODE nickname[pml::PERSON_NAME_LENGTH + pml::EOM_LENGTH];

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  poke->GetNickName( nickname, GFL_NELEMS(nickname) );
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]Form:%d", item->GetName(), nickname, debugPokemonForm );
  return workStr;
#else
  return NULL;
#endif
}

#if 0
static void SetLegalPokemon( pml::pokepara::CoreParam * pp, const Savedata::MyStatus * mystatus, const PokeTool::SimpleParam & SimpleParam )
{
  pp->ChangeMonsNo( SimpleParam.monsNo, SimpleParam.formNo );
  pp->ChangeFormNo( SimpleParam.formNo );
  pp->SetGetBall( 1 );
  pp->SetDefaultWaza();

  gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  mystatus->SetPokemonParent( pp, heap );
  pp->SetCassetteVersion( System::GetVersion() );
  pp->SetLangId( System::GetLang() );
  pp->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 4 );
  pp->SetItem(0);
}

//-----------------------------------------------------------------------------
/**
 * @brief 不正通過ポケをチームにセット
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemon_SetLegalPokemonForParty(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE

  static const PokeTool::SimpleParam LEGAL_POKEMON[]  =
  {
    PokeTool::SimpleParam( MONSNO_RIZAADON, 0, pml::SEX_FEMALE, false, false, 0 ),
    PokeTool::SimpleParam( MONSNO_HUSIGIBANA, 0, pml::SEX_MALE, false, false, 0 ),
    PokeTool::SimpleParam( MONSNO_KAMEKKUSU, 0, pml::SEX_FEMALE, false, false, 0 ),
    PokeTool::SimpleParam( MONSNO_HUKUROU1, 0, pml::SEX_MALE, false, false, 0 ),
    PokeTool::SimpleParam( MONSNO_TORA1, 0, pml::SEX_FEMALE, false, false, 0 ),
    PokeTool::SimpleParam( MONSNO_ASIKA1, 0, pml::SEX_MALE, false, false, 0 ),
  };

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  Savedata::MyStatus* myStatus = game_data->GetPlayerStatus();
  pml::PokeParty* party = game_data->GetPlayerParty();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    for ( int index = 0; index < GFL_NELEMS(LEGAL_POKEMON); ++ index ) {
      pml::pokepara::PokemonParam* poke = party->GetMemberPointer(index);
      SetLegalPokemon( poke, myStatus, LEGAL_POKEMON[index] );
    }
  }
#endif //GFL_DEBUGWIN_USE
}


static int fieldDebugMenuBattleTeam = 0;
//-----------------------------------------------------------------------------
/**
 * @brief 不正通過ポケをチームにセット
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemon_SetLegalPokemonForBattleTeam(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );
#if GFL_DEBUGWIN_USE
  static const PokeTool::SimpleParam LEGAL_POKEMON[]  =
  {
    PokeTool::SimpleParam( MONSNO_RIZAADON, 0, pml::SEX_FEMALE, false, false, 0 ),
    PokeTool::SimpleParam( MONSNO_HUSIGIBANA, 0, pml::SEX_MALE, false, false, 0 ),
    PokeTool::SimpleParam( MONSNO_KAMEKKUSU, 0, pml::SEX_FEMALE, false, false, 0 ),
    PokeTool::SimpleParam( MONSNO_HUKUROU1, 0, pml::SEX_MALE, false, false, 0 ),
    PokeTool::SimpleParam( MONSNO_TORA1, 0, pml::SEX_FEMALE, false, false, 0 ),
    PokeTool::SimpleParam( MONSNO_ASIKA1, 0, pml::SEX_MALE, false, false, 0 ),
  };

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  Savedata::MyStatus* myStatus = game_data->GetPlayerStatus();
  Savedata::BOX * sv_box = game_data->GetBOX();
  Savedata::BoxPokemon * sv_box_pokemon = game_data->GetBoxPokemon();
  gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    if( sv_box_pokemon->GetSpaceCountAll( heap ) < 6 )
    {
      item->PlaySystemSound(gfl2::debug::SE_ERROR);
      GFL_PRINT("!!!BOXに空きがない\n");
      return;
    }

    for( u32 i=0; i<Savedata::BOX::TEAM_MAX; i++ )
    {
      pml::pokepara::CoreParam *cp  = GFL_NEW(heap) pml::pokepara::CoreParam( heap );
      SetLegalPokemon( cp, myStatus, LEGAL_POKEMON[i] );

      //BOXにいれて、バトルチーム登録
      u16 boxPos = 0;
      u32 boxTray = sv_box_pokemon->PutPokemonEmptyTrayAll( cp, 0, heap, &boxPos );
      if( boxTray != Savedata::BoxPokemon::PUT_POKEMON_ERROR )
      {
        sv_box->SetTeamPokePos( fieldDebugMenuBattleTeam, i, ((boxTray & 0xFF) << 8) | (boxPos & 0xFF)  );
      }

      GFL_DELETE cp;
    }
  }
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) == true )
  {
    if( --fieldDebugMenuBattleTeam < 0 )
    {
      fieldDebugMenuBattleTeam = 5;
    }
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) == true )
  {
    if( ++fieldDebugMenuBattleTeam >= 6 )
    {
      fieldDebugMenuBattleTeam = 0;
    }
  }

#endif //GFL_DEBUGWIN_USE
}

static wchar_t* DrawDebugPokemon_SetLegalPokemonForBattleTeam( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), fieldDebugMenuBattleTeam );
  return workStr;
#else
  return NULL;
#endif
}
#endif //0

#if GFL_DEBUGWIN_USE
//-----------------------------------------------------------------------------
/**
 * @brief フォームチェック用 BOX Fill 関連
 */
//-----------------------------------------------------------------------------

enum   
  {
    BOXFILL_CHECK_POKE_FORM_VALID = 0,
    BOXFILL_CHECK_POKE_FORM_INVALID_MONS_NO,
    BOXFILL_CHECK_POKE_FORM_INVALID_FORM_MAX,
    BOXFILL_CHECK_POKE_FORM_INVALID_MEGA,
    BOXFILL_CHECK_POKE_FORM_INVALID_GENSI,
    BOXFILL_CHECK_POKE_FORM_INVALID_ULTRA,
  };

// フォームを特定のルールでチェックする
// ルール：メガシンカとゲンシカイキは対象外
// true = 有効 : false = 無効
static int CheckValidPokeForm( MonsNo mons_no, u32 form_no )
{
  if(( mons_no <= MONSNO_NULL ) ||  ( mons_no  >= MONSNO_TAMAGO ))
    {
      return BOXFILL_CHECK_POKE_FORM_INVALID_MONS_NO;
    }

  pml::personal::LoadPersonalData( mons_no, 0 );
  u32 form_max = GetPersonalParam( pml::personal::PARAM_ID_form_max );

  if(form_no >= form_max )
    {
      return BOXFILL_CHECK_POKE_FORM_INVALID_FORM_MAX;
    }
  
  if( pml::personal::CheckMegaForm ( mons_no, form_no ) )
    {
      return BOXFILL_CHECK_POKE_FORM_INVALID_MEGA;
    }

  if( pml::personal::CheckGensiKaiki ( mons_no, form_no ) )
    {
      return BOXFILL_CHECK_POKE_FORM_INVALID_GENSI;
    }
  if(pml::personal::CheckUltraBurst(mons_no, form_no))
    {
      return BOXFILL_CHECK_POKE_FORM_INVALID_ULTRA;
    }


  return BOXFILL_CHECK_POKE_FORM_VALID;

}

// 特定のフォルムを除いて、次のフォルムの取得
static void GetNextPokeAndForm ( MonsNo *mons_no, u32 *form_no)
{
  
  int m_no = (int)(*mons_no);
  u32 f_no = *form_no;

  int is_valid = BOXFILL_CHECK_POKE_FORM_INVALID_MONS_NO;

  f_no += 1;

  while( is_valid != BOXFILL_CHECK_POKE_FORM_VALID )
  {
    is_valid = CheckValidPokeForm( (MonsNo)m_no, f_no );


    if( is_valid == BOXFILL_CHECK_POKE_FORM_INVALID_FORM_MAX )
      {
        m_no += 1;
        f_no = 0;
      } 
    else if( ( is_valid == BOXFILL_CHECK_POKE_FORM_INVALID_MEGA ) || ( is_valid == BOXFILL_CHECK_POKE_FORM_INVALID_GENSI ) || ( is_valid == BOXFILL_CHECK_POKE_FORM_INVALID_ULTRA ) )
      {
        f_no++;
      }
    else if( is_valid == BOXFILL_CHECK_POKE_FORM_INVALID_MONS_NO )
      {
        m_no = MONSNO_NULL;
        f_no = 0;
        break;
      }

  };

  *mons_no = (MonsNo)m_no;
  *form_no = f_no;

}

// フォルムチェック用 1Box fill
static void fill_poke_form_1box( Savedata::BoxPokemon *pBoxPoke, int tray, GameSys::GameData *game_data, gfl2::heap::HeapBase *heap, u32 *start_mons_num, u32 *start_form_num, u32 end_mons_num )
{
  Savedata::MyStatus *mystatus = game_data->GetPlayerStatus();
  u32 myId = mystatus->GetID();
  pml::pokepara::PokemonParam *core  = GFL_NEW(heap) pml::pokepara::PokemonParam( heap, MONSNO_HUSIGIDANE, 50, myId );

  MonsNo mons_no = static_cast<MonsNo>(*start_mons_num);
  u32 form_no = *start_form_num;

  for(int i=0;i<Savedata::BoxPokemon::TRAY_POKE_MAX;i++){

    core->ChangeMonsNo( mons_no, form_no );
    core->ChangeFormNo( form_no );          // ChangeMonsNo()だけではフォルムが変化しないっぽい

    core->SetGetBall( item::BALLID_MONSUTAABOORU );
    if(((1+i) % 3) == 0)
    {
      core->SetItem(1+i);
    }
    core->SetDefaultWaza();

    //親名設定
    mystatus->SetPokemonParent( core, heap );
    core->SetCassetteVersion( System::GetVersion() );
    core->SetLangId( System::GetLang() );
    core->SetDebugEditFlag(true);
    PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(heap, core, PokeTool::PokeGetUtil::CallerType::HOKAKU);
    pBoxPoke->PutPokemon( core, tray, i, heap );

    //    KIJIMA_PRINT(">>>In Box  : Mons no:[%d] Form:[%d]\n", mons_no, form_no );

    // 次のフォルム
    GetNextPokeAndForm( &mons_no, &form_no );

    // End check
    if(( mons_no > end_mons_num ) || (mons_no == MONSNO_NULL) )
      {
        break;
      }
  }

  *start_mons_num = (u32)mons_no;
  *start_form_num = form_no;

  GFL_DELETE core;

}
#endif //GFL_DEBUGWIN_USE

//-----------------------------------------------------------------------------
/**
 * @brief DPまでのポケモンでボックスを埋める(フォルム違いも)
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonBoxFull_All_Form_Still_DP(void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  //  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  Savedata::BOX *pBox             = game_data->GetBOX();
  Savedata::BoxPokemon *pBoxPoke  = game_data->GetBoxPokemon();
  gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  
  u32 start_mons_no = MONSNO_HUSIGIDANE;
  u32 end_mons_no = MONSNO_ARUSEUSU;
  u32 start_form_no = 0;
    
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
    {
      for(int i=0;i<Savedata::BoxPokemon::TRAY_MAX;i++){
        fill_poke_form_1box( pBoxPoke, i, game_data, workHeap, &start_mons_no, &start_form_no, end_mons_no );

        if(( start_mons_no > end_mons_no ) || ( start_mons_no == MONSNO_NULL ) )
          {
            break;
          }
      }
      pBoxPoke->UpdateTrayMax(workHeap);
    }

#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief BW以降のポケモンでボックスを埋める(フォルム違いも)
 */
//-----------------------------------------------------------------------------
static void UpdateDebugPokemonBoxFull_All_Form_After_BW(void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  //  const gfl2::ui::VectorDevice*  cross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  GameSys::GameData* game_data = pGameManager->GetGameData();
  Savedata::BOX *pBox             = game_data->GetBOX();
  Savedata::BoxPokemon *pBoxPoke  = game_data->GetBoxPokemon();
  gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

  u32 start_mons_no = MONSNO_BIKUTHINI;
  u32 end_mons_no = MONSNO_END;
  u32 start_form_no = 0;
  
  
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    for(int i=0;i<Savedata::BoxPokemon::TRAY_MAX;i++){
      fill_poke_form_1box( pBoxPoke, i, game_data, workHeap, &start_mons_no, &start_form_no, end_mons_no );

      if(( start_mons_no > end_mons_no ) || ( start_mons_no == MONSNO_NULL ) )
        {
          break;
        }
    }
    pBoxPoke->UpdateTrayMax(workHeap);

  }

#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief 手持ち Save & Load
 */
//-----------------------------------------------------------------------------
#if GFL_DEBUGWIN_USE
#define DEBUG_SDMC_FILENAME_MAX     32
#define DEBUG_PARTY_SAVE_SLOT_MAX    10
#define DEBUG_PARTY_SAVE_BASE_FILENAME "debug_party_save_0.dat"

// ベースファイル名の'0'の部分にインデックスを加算してファイル名を生成
void MakeSdmcFilename ( const char *baseFilepath,  char *dstFilePath, u32 idx )
{
  //  const char  baseFilepath[] = "debug_party_save_0.dat";
  const char *p = baseFilepath;
  char *dst = dstFilePath;

  for(int i=0; i<DEBUG_SDMC_FILENAME_MAX; i++)
    {
      *dst = *p;

      if( *p == L'\0' )
        {
          break;
        }

      if( *dst == L'0' )
        {
          (*dst) += idx;
        }
      ++dst;
      ++p;
    }
}
#endif // GFL_DEBUGWIN_USE

static wchar_t* DrawDebugPokemon_PartySaveLoad( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), *((int*)userWork) );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugPokemon_PartySave(void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

    pml::PokeParty* t_party = pGameManager->GetGameData()->GetPlayerParty();

    int slot = *((int*)userWork);

    char save_file_name[DEBUG_SDMC_FILENAME_MAX];
    MakeSdmcFilename( DEBUG_PARTY_SAVE_BASE_FILENAME, save_file_name, slot );

    gfl2::fs::DirectSdmcFile* file = gfl2::fs::System::CreateDirectSdmcFile( workHeap, save_file_name, gfl2::fs::Define::OpenMode::WRITE );

    if( file == NULL ) 
      {
        KIJIMA_PRINT( ">>>!!! Party data save failed. [%d]\n", slot );
        return;
      }

    pml::PokeParty::SERIALIZE_DATA  *pSd = GFL_NEW(workHeap) pml::PokeParty::SERIALIZE_DATA();
    size_t ser_size = t_party->SerializeFull( pSd );

    file->Write( pSd, ser_size );

    GFL_SAFE_DELETE(pSd);
    gfl2::fs::System::DestroyDirectSdmcFile( file );
    
    KIJIMA_PRINT( ">>> Party data saved. (to SD card)  %s(%d) \n", save_file_name,  ser_size );

  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) == true )
  {
    int slot = *((int*)userWork);
    slot--;

    if( slot < 0 )
      {
        slot = DEBUG_PARTY_SAVE_SLOT_MAX-1;
      }

    *((int*)userWork) = slot;

  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) == true )
  {
    int slot = *((int*)userWork);
    slot++;

    if( slot >= DEBUG_PARTY_SAVE_SLOT_MAX )
      {
        slot = 0;
      }

    *((int*)userWork) = slot;
  }

#endif
}

static void UpdateDebugPokemon_PartyLoad(void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

    pml::PokeParty* t_party = pGameManager->GetGameData()->GetPlayerParty();

    size_t st_size = pml::PokeParty::SERIALIZE_DATA_SIZE;
    int slot = *((int*)userWork);

    char save_file_name[DEBUG_SDMC_FILENAME_MAX];
    MakeSdmcFilename( DEBUG_PARTY_SAVE_BASE_FILENAME, save_file_name, slot );

    gfl2::fs::DirectSdmcFile* file = gfl2::fs::System::CreateDirectSdmcFile( workHeap, save_file_name, gfl2::fs::Define::OpenMode::READ );

    if( file == NULL ) 
      {
        KIJIMA_PRINT( ">>>!!! Party data load failed. [%d]\n", slot );
        return;
      }

    pml::PokeParty::SERIALIZE_DATA  *pSd = GFL_NEW(workHeap) pml::PokeParty::SERIALIZE_DATA();

    size_t real_read_size = 0;
    file->Read( &real_read_size, pSd, st_size, st_size );

    t_party->DeserializeFull( pSd );

    GFL_SAFE_DELETE(pSd);
    gfl2::fs::System::DestroyDirectSdmcFile( file );

    KIJIMA_PRINT( ">>> Party data loaded. (form SD card)  %s(%d) \n", save_file_name, real_read_size );

  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) == true )
  {
    int slot = *((int*)userWork);
    slot--;

    if( slot < 0 )
      {
        slot = DEBUG_PARTY_SAVE_SLOT_MAX-1;
      }

    *((int*)userWork) = slot;

  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) == true )
  {
    int slot = *((int*)userWork);
    slot++;

    if( slot >= DEBUG_PARTY_SAVE_SLOT_MAX )
      {
        slot = 0;
      }

    *((int*)userWork) = slot;
  }

#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief Box Save & Load
 */
//-----------------------------------------------------------------------------
#if GFL_DEBUGWIN_USE
#define DEBUG_BOX_SAVE_SLOT_MAX    10
#define DEBUG_BOX_SAVE_BASE_FILENAME "debug_box_save_0.dat"
#endif // GFL_DEBUGWIN_USE

static wchar_t* DrawDebugPokemon_BoxSaveLoad( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), *((int*)userWork) );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugPokemon_BoxSave(void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );


  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {
    gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

    Savedata::BoxPokemon *pBoxPoke  = pGameManager->GetGameData()->GetBoxPokemon();

    void *pDataAdr = pBoxPoke->GetData();
    size_t ser_size = pBoxPoke->GetDataSize();
    int slot = *((int*)userWork);

    char save_file_name[DEBUG_SDMC_FILENAME_MAX];
    MakeSdmcFilename( DEBUG_BOX_SAVE_BASE_FILENAME, save_file_name, slot );

    gfl2::fs::DirectSdmcFile* file = gfl2::fs::System::CreateDirectSdmcFile( workHeap, save_file_name, gfl2::fs::Define::OpenMode::WRITE );

    if( file == NULL ) 
      {
        KIJIMA_PRINT( ">>>!!! Box data save failed. [%d]\n", slot );
        return;
      }

    file->Write( pDataAdr, ser_size );
    gfl2::fs::System::DestroyDirectSdmcFile( file );

    KIJIMA_PRINT( ">>> Box data saved. (to SD card)  %s(%d) \n", save_file_name,  ser_size );
    
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) == true )
  {
    int slot = *((int*)userWork);
    slot--;

    if( slot < 0 )
      {
        slot = DEBUG_BOX_SAVE_SLOT_MAX-1;
      }

    *((int*)userWork) = slot;

  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) == true )
  {
    int slot = *((int*)userWork);
    slot++;

    if( slot >= DEBUG_BOX_SAVE_SLOT_MAX )
      {
        slot = 0;
      }

    *((int*)userWork) = slot;
  }


#endif
}


static void UpdateDebugPokemon_BoxLoad(void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
  {

    gfl2::heap::HeapBase* workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

    Savedata::BoxPokemon *pBoxPoke  = pGameManager->GetGameData()->GetBoxPokemon();

    size_t ser_size = pBoxPoke->GetDataSize();
    int slot = *((int*)userWork);


    char save_file_name[DEBUG_SDMC_FILENAME_MAX];
    MakeSdmcFilename( DEBUG_BOX_SAVE_BASE_FILENAME, save_file_name, slot );

    gfl2::fs::DirectSdmcFile* file = gfl2::fs::System::CreateDirectSdmcFile( workHeap, save_file_name, gfl2::fs::Define::OpenMode::READ );

    if( file == NULL ) 
      {
        KIJIMA_PRINT( ">>>!!! Box data load failed. [%d]\n", slot );
        return;
      }

    Savedata::BoxPokemon::BoxTrayData *pBoxTray = GFL_NEW_ARRAY(workHeap) Savedata::BoxPokemon::BoxTrayData[Savedata::BoxPokemon::TRAY_MAX];

    //    size_t heap_sz = gfl2::heap::Manager::GetMemorySize( pBoxTray );
    //    KIJIMA_PRINT( ">>>Heap allocated size (include header). [%d]\n", heap_sz );

    size_t real_read_size;
    file->Read( &real_read_size, pBoxTray, ser_size, ser_size );

    pBoxPoke->SetData( pBoxTray );

    GFL_SAFE_DELETE_ARRAY( pBoxTray );
    gfl2::fs::System::DestroyDirectSdmcFile( file );


    KIJIMA_PRINT( ">>> Box data loaded. (form SD card)  %s(%d) \n", save_file_name, real_read_size );

  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) == true )
  {
    int slot = *((int*)userWork);
    slot--;

    if( slot < 0 )
      {
        slot = DEBUG_BOX_SAVE_SLOT_MAX-1;
      }

    *((int*)userWork) = slot;

  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) == true )
  {
    int slot = *((int*)userWork);
    slot++;

    if( slot >= DEBUG_BOX_SAVE_SLOT_MAX )
      {
        slot = 0;
      }

    *((int*)userWork) = slot;
  }

#endif
}


//------------------------------------------------------------------------------
/**
 * @brief   アイテム生成
 */
//------------------------------------------------------------------------------
static void DebugWin_DebugItemMake( void * userWork, gfl2::debug::DebugWinItem * item )
{
#if GFL_DEBUGWIN_USE
  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();
  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    Field::Debug::EventDebugItemMake::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), true );
    gfl2::debug::DebugWin_CloseWindow();
  }
#endif
}


//------------------------------------------------------------------------------
/**
 * @brief   ロトム関連
 */
//------------------------------------------------------------------------------
static int DebugGetRotomAddValue(void)
{
  const gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();
  if( button->IsHold(gfl2::ui::BUTTON_L) )
  {
    return 10;
  }
  if( button->IsHold(gfl2::ui::BUTTON_R) )
  {
    return 100;
  }
  return 1;
}

static void UpdateDebugRotomFriendship( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Savedata::FieldMenu * sv_fieldmenu = gm->GetGameData()->GetFieldMenu();
  const gfl2::ui::VectorDevice * key = gm->GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  u16 val = sv_fieldmenu->GetRotomFriendship();
  u16 max = App::Tool::RotomFriendship::DebugGetFriendshipMax();

  int add = DebugGetRotomAddValue();

  if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
  {
    if( val >= add )
    {
      val -= add;
    }
    else
    {
      val = 0;
    }
  }
  else if( key->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    if( (val+add) <= max )
    {
      val += add;
    }
    else
    {
      val = max;
    }
  }

  sv_fieldmenu->DebugSetRotomFriendship( val );
#endif
}

static wchar_t* DrawDebugRotomFriendship( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(
    workStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls[%d]",
    item->GetName(),
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenu()->GetRotomFriendship() );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugRotomFriendshipUpFlag( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE

  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  App::FieldMenu::PlayData * fieldmenu = gm->GetGameData()->GetFieldMenuPlayData();
  const gfl2::ui::VectorDevice * key = gm->GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  if( key->IsRepeat(gfl2::ui::DIR_LEFT|gfl2::ui::DIR_RIGHT) )
  {
    fieldmenu->SetRotomTouchFriendshipUpFlag( fieldmenu->IsRotomTouchFriendshipUp()^1 );
  }
#endif
}

static wchar_t* DrawDebugRotomFriendshipUpFlag( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(
    workStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls[%d]",
    item->GetName(),
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenuPlayData()->IsRotomTouchFriendshipUp() );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugRotomChara( void* userWork, gfl2::debug::DebugWinItem* item, Savedata::FieldMenu::RotomCharacter chara )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Savedata::FieldMenu * sv_fieldmenu = gm->GetGameData()->GetFieldMenu();
  const gfl2::ui::VectorDevice * key = gm->GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  u32 val = sv_fieldmenu->GetRotomCharacterCounter( chara );
  int add = DebugGetRotomAddValue();

  if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
  {
    if( val >= add )
    {
      val -= add;
    }
    else
    {
      val = 0;
    }
  }
  else if( key->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    if( (val+add) <= 0xffffffff )
    {
      val += add;
    }
    else
    {
      val = 0xffffffff;
    }
  }

  sv_fieldmenu->DebugSetRotomCharacterCounter( chara, val );
#endif
}

static wchar_t* DrawDebugRotomChara( void* userWork, gfl2::debug::DebugWinItem* item, Savedata::FieldMenu::RotomCharacter chara )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(
    workStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls[%d]",
    item->GetName(),
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenu()->GetRotomCharacterCounter(chara) );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugRotomChara00( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  UpdateDebugRotomChara( userWork, item, Savedata::FieldMenu::ROTOM_CHAR_BELLIGERENT );
#endif
}

static wchar_t* DrawDebugRotomChara00( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return DrawDebugRotomChara( userWork, item, Savedata::FieldMenu::ROTOM_CHAR_BELLIGERENT );
}

static void UpdateDebugRotomChara01( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  UpdateDebugRotomChara( userWork, item, Savedata::FieldMenu::ROTOM_CHAR_FRIENDLY );
#endif
}

static wchar_t* DrawDebugRotomChara01( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return DrawDebugRotomChara( userWork, item, Savedata::FieldMenu::ROTOM_CHAR_FRIENDLY );
}

static void UpdateDebugRotomChara02( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  UpdateDebugRotomChara( userWork, item, Savedata::FieldMenu::ROTOM_CHAR_OPTIMISTIC );
#endif
}

static wchar_t* DrawDebugRotomChara02( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return DrawDebugRotomChara( userWork, item, Savedata::FieldMenu::ROTOM_CHAR_OPTIMISTIC );
}

static void UpdateDebugRotomAnimeLoop( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  App::FieldMenu::PlayData * fieldmenu = gm->GetGameData()->GetFieldMenuPlayData();
  const gfl2::ui::VectorDevice * key = gm->GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  if( key->IsRepeat(gfl2::ui::DIR_LEFT|gfl2::ui::DIR_RIGHT) )
  {
    u32 flg = fieldmenu->DebugIsRotomAnimeLoop() ^ 1;
    fieldmenu->DebugSetRotomAnimeLoop( flg );
  }
#endif
}

static wchar_t* DrawDebugRotomAnimeLoop( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(
    workStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls[%d]",
    item->GetName(),
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenuPlayData()->DebugIsRotomAnimeLoop() );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugRotomCharaChange( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Savedata::FieldMenu * sv_fieldmenu = gm->GetGameData()->GetFieldMenu();
  const gfl2::ui::VectorDevice * key = gm->GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  u16 val = sv_fieldmenu->DebugGetRotomCharacterChangeOffsetCount();
  u16 max = Savedata::FieldMenu::DebugGetRotomCharacterChangeOffset()-1;

  int add = DebugGetRotomAddValue();

  if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
  {
    if( val >= add )
    {
      val -= add;
    }
    else
    {
      val = 0;
    }
  }
  else if( key->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    if( (val+add) <= max )
    {
      val += add;
    }
    else
    {
      val = max;
    }
  }

  sv_fieldmenu->DebugSetRotomCharacterChangeOffsetCount( val );
#endif
}

static wchar_t* DrawDebugRotomCharaChange( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(
    workStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls[%d]",
    item->GetName(),
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenu()->DebugGetRotomCharacterChangeOffsetCount() );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugRotomRotopon( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Savedata::FieldMenu * sv_fieldmenu = gm->GetGameData()->GetFieldMenu();
  const gfl2::ui::VectorDevice * key = gm->GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  u16 val = sv_fieldmenu->DebugGetRotomRotoponStartOffsetCount();
  u16 max = Savedata::FieldMenu::DebugGetRotomRotoponStartOffset()-1;

  int add = DebugGetRotomAddValue();

  if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
  {
    if( val >= add )
    {
      val -= add;
    }
    else
    {
      val = 0;
    }
  }
  else if( key->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    if( (val+add) <= max )
    {
      val += add;
    }
    else
    {
      val = max;
    }
  }

  sv_fieldmenu->DebugSetRotomRotoponStartOffsetCount( val );
#endif
}

static wchar_t* DrawDebugRotomRotopon( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(
    workStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls[%d]",
    item->GetName(),
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenu()->DebugGetRotomRotoponStartOffsetCount() );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugRotomEventFlag( void* userWork, gfl2::debug::DebugWinItem* item, u32 flg )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Field::EventWork * event_work = gm->GetGameData()->GetEventWork();
  const gfl2::ui::VectorDevice * key = gm->GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  if( key->IsRepeat(gfl2::ui::DIR_LEFT|gfl2::ui::DIR_RIGHT) )
  {
    if( event_work->CheckEventFlag( flg ) == false )
    {
      event_work->SetEventFlag( flg );
    }
    else
    {
      event_work->ResetEventFlag( flg );
    }
  }
#endif
}

static wchar_t* DrawDebugRotomEventFlag( void* userWork, gfl2::debug::DebugWinItem* item, u32 flg )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(
    workStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls[%d]",
    item->GetName(),
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork()->CheckEventFlag(flg) );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugRotomEventFlag00( void* userWork, gfl2::debug::DebugWinItem* item )
{
  UpdateDebugRotomEventFlag( userWork, item, SYS_FLAG_ROTOM_SINMITU_1 );
}

static wchar_t* DrawDebugRotomEventFlag00( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return DrawDebugRotomEventFlag( userWork, item, SYS_FLAG_ROTOM_SINMITU_1 );
}

static void UpdateDebugRotomEventFlag01( void* userWork, gfl2::debug::DebugWinItem* item )
{
  UpdateDebugRotomEventFlag( userWork, item, SYS_FLAG_ROTOM_SINMITU_2 );
}

static wchar_t* DrawDebugRotomEventFlag01( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return DrawDebugRotomEventFlag( userWork, item, SYS_FLAG_ROTOM_SINMITU_2 );
}

static void UpdateDebugRotomEventFlag02( void* userWork, gfl2::debug::DebugWinItem* item )
{
  UpdateDebugRotomEventFlag( userWork, item, SYS_FLAG_ROTOM_SINMITU_3 );
}

static wchar_t* DrawDebugRotomEventFlag02( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return DrawDebugRotomEventFlag( userWork, item, SYS_FLAG_ROTOM_SINMITU_3 );
}

static void UpdateDebugRotomRequest( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  App::FieldMenu::PlayData * fieldmenu = gm->GetGameData()->GetFieldMenuPlayData();
  const gfl2::ui::VectorDevice * key = gm->GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  static const Savedata::FieldMenu::RotomRequest tbl[] =
  {
    Savedata::FieldMenu::ROTOM_REQ_NONE,           // なし
    Savedata::FieldMenu::ROTOM_REQ_FINDER_STUDIO,  // ファインダースタジオ
    Savedata::FieldMenu::ROTOM_REQ_MANTAIN_SURF,   // マンタインサーフ
    Savedata::FieldMenu::ROTOM_REQ_LUNASOL_DIVE,   // ルナソルダイブ
    Savedata::FieldMenu::ROTOM_REQ_BATTLE_TREE,    // バトルツリー
    Savedata::FieldMenu::ROTOM_REQ_FES_CIRCLE,     // フェスサークル
  };
  u32 size = GFL_NELEMS( tbl );
  u32 p = 0;

  for( u32 i=0; i<size; i++ )
  {
    if( fieldmenu->DebugGetRotomRequest() == tbl[i] )
    {
      p = i;
      break;
    }
  }

  if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
  {
    if( p == 0 )
    {
      p = size - 1;
    }
    else
    {
      p--;
    }
    fieldmenu->DebugSetRotomRequest( tbl[p] );
  }
  else if( key->IsRepeat(gfl2::ui::DIR_RIGHT) )
  {
    p++;
    if( p >= size )
    {
      p = 0;
    }
    fieldmenu->DebugSetRotomRequest( tbl[p] );
  }
#endif
}

static wchar_t* DrawDebugRotomRequest( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  wchar_t* table[] =
  {
    L"NONE",            // なし
    L"FINDER_STUDIO",   // ファインダースタジオ
    L"MANTAIN_SURF",    // マンタインサーフ
    L"LUNASOL_DIVE",    // ルナソルダイブ
    L"BATTLE_TREE",     // バトルツリー
    L"FES_CIRCLE",      // フェスサークル
  };
  swprintf(
    workStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls[%ls]",
    item->GetName(),
    table[ GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenuPlayData()->DebugGetRotomRequest() ] );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugRotomReportFlag( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  App::FieldMenu::PlayData * fieldmenu = gm->GetGameData()->GetFieldMenuPlayData();
  const gfl2::ui::VectorDevice * key = gm->GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  if( key->IsRepeat(gfl2::ui::DIR_LEFT|gfl2::ui::DIR_RIGHT) )
  {
    if( fieldmenu->DebugGetRotomReportFlag() == false )
    {
      fieldmenu->DebugSetRotomReportFlag( true );
    }
    else
    {
      fieldmenu->DebugSetRotomReportFlag( false );
    }
  }
#endif
}

static wchar_t* DrawDebugRotomReportFlag( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(
    workStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls[%d]",
    item->GetName(),
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenuPlayData()->DebugGetRotomReportFlag() );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugRotomFreeTalkID( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  App::FieldMenu::PlayData * fieldmenu = gm->GetGameData()->GetFieldMenuPlayData();
  const gfl2::ui::VectorDevice * key = gm->GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  u32 p = fieldmenu->DebugGetRotomFreeTalkID();

  if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
  {
    if( p == 0 )
    {
      p = 27;
    }
    else
    {
      p--;
    }
    fieldmenu->DebugSetRotomFreeTalkID( p );
  }
  else if( key->IsRepeat(gfl2::ui::DIR_RIGHT) )
  {
    p++;
    if( p > 27 )
    {
      p = 0;
    }
    fieldmenu->DebugSetRotomFreeTalkID( p );
  }
#endif
}

static wchar_t* DrawDebugRotomFreeTalkID( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(
    workStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls[%d]",
    item->GetName(),
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenuPlayData()->DebugGetRotomFreeTalkID() );
  return workStr;
#else
  return NULL;
#endif
}

static void UpdateDebugRotomGameClearMsgID( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  App::FieldMenu::PlayData * fieldmenu = gm->GetGameData()->GetFieldMenuPlayData();
  const gfl2::ui::VectorDevice * key = gm->GetUiDeviceManager()->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );

  u32 p = fieldmenu->DebugGetRotomGameClearMsgID();

  if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
  {
    if( p == 0 )
    {
      p = 20;
    }
    else
    {
      p--;
    }
    fieldmenu->DebugSetRotomGameClearMsgID( p );
  }
  else if( key->IsRepeat(gfl2::ui::DIR_RIGHT) )
  {
    p++;
    if( p > 20 )
    {
      p = 0;
    }
    fieldmenu->DebugSetRotomGameClearMsgID( p );
  }
#endif
}

static wchar_t* DrawDebugRotomGameClearMsgID( void* userWork, gfl2::debug::DebugWinItem* item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(
    workStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls[%d]",
    item->GetName(),
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenuPlayData()->DebugGetRotomGameClearMsgID() );
  return workStr;
#else
  return NULL;
#endif
}


//-----------------------------------------------------------------------------
/**
 *  @brief  時間の変更
 */
//-----------------------------------------------------------------------------
static void DebugWinFunc_CallFakeTime( void* userWork, gfl2::debug::DebugWinItem* item );
static void DebugWinFunc_CallAlolaTimeReverseMapJump( void* userWork, gfl2::debug::DebugWinItem* item );


//-----------------------------------------------------------------------------
/**
 *  @brief  時間の表示
 */
//-----------------------------------------------------------------------------
static wchar_t* DrawDebugWriteDrawTime( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  GameSys::AlolaTime alolaTime;
  alolaTime.Initialize();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d:%d:%d]", item->GetName(), alolaTime.GetHour(), alolaTime.GetMinute(), alolaTime.GetSecond() );
  return workStr;
}


//=========================================================================================
// フィールドリセット
//=========================================================================================
static void CallGameReStartUpdate( void* userWork, gfl2::debug::DebugWinItem* item )
{
  // Aボタンを押していなければ動作中止
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 現在の位置を記憶
  Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
  EventGameStart::EventDebugFirst::SetStartInfo( pFieldmap->GetZoneID(), pFieldmap->GetPlayerPosition(), pFieldmap->GetPlayerQuaternion() );

  // ウィンドウを閉じる
  gfl2::debug::DebugWin_CloseWindow();

  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventGameClose>( pGameManager->GetGameEventManager() );
}


//=========================================================================================
// 動作モデル用のデバッグ機能 yy-ikeuchi
//=========================================================================================

// 動作モデル作成用
static u32  createCharacterId;

static void CreateTsurizao( void* userWork, gfl2::debug::DebugWinItem* item )
{
  // Aボタンを押していなければ動作中止
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 動作モデルマネージャーを取得
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = static_cast<Field::MoveModel::FieldMoveModelManager*>(userWork);

  // 対象のバイナリロード
  pFieldMoveModelManager->LoadDynamicAsync(Field::MoveModel::FieldMoveModelManager::DYNAMIC_RES_LOAD_INDEX_SUBOBJECT,CHARA_MODEL_ID_OB0002_00);
  while(true)
  {
    if( pFieldMoveModelManager->IsLoaded(CHARA_MODEL_ID_OB0002_00) ){ break; }
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }
  pFieldMoveModelManager->SetupModel(CHARA_MODEL_ID_OB0002_00);

  // サブオブジェクトの設定
  Field::MoveModel::FieldSubobjectHeader header;
  header.characterId = CHARA_MODEL_ID_OB0002_00;
  pFieldMoveModelManager->SetSubobjectToMoveModel( pFieldMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ) , 0 , &header, true );
}

static void CreateDummyTrainer( void* userWork, gfl2::debug::DebugWinItem* item )
{
  // Aボタンを押していなければ動作中止
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 動作モデルマネージャーを取得
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = static_cast<Field::MoveModel::FieldMoveModelManager*>(userWork);

  u32 subobectIndex = 0;

  // サブオブジェクトを作成
  {
    // 対象のバイナリロード
    pFieldMoveModelManager->LoadStaticAsync(CHARA_MODEL_ID_OB0204_00);
    while(true)
    {
      if( pFieldMoveModelManager->IsLoaded(CHARA_MODEL_ID_OB0204_00) ){ break; }
      gfl2::thread::ThreadStatic::CurrentSleep(1);
    }
    pFieldMoveModelManager->SetupModel(CHARA_MODEL_ID_OB0204_00);

    // まだ利用されていないワークを検索
    subobectIndex = pFieldMoveModelManager->GetFieldSubobjectIndexFromFreeSpace();

    // サブオブジェクトの設定
    Field::MoveModel::FieldSubobjectHeader header;
    header.characterId = CHARA_MODEL_ID_OB0204_00;
    pFieldMoveModelManager->InitializeSubobject( (Field::MoveModel::FIELD_SUB_OBJECT_ID)subobectIndex , &header );
  }

  // 動作モデルを作成
  {
    // 対象のバイナリロード
    pFieldMoveModelManager->LoadStaticAsync(CHARA_MODEL_ID_TR0077_00);
    while(true)
    {
      if( pFieldMoveModelManager->IsLoaded(CHARA_MODEL_ID_TR0077_00) ){ break; }
      gfl2::thread::ThreadStatic::CurrentSleep(1);
    }
    pFieldMoveModelManager->SetupModel(CHARA_MODEL_ID_TR0077_00);

    // まだ利用されていないワークを検索
    u32 freeIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromFreeSpace();
    Field::MoveModel::FIELD_MOVE_MODEL_ID modelId     = (Field::MoveModel::FIELD_MOVE_MODEL_ID)freeIndex;
    Field::MoveModel::FieldMoveModel*     pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(modelId);

    // 動作モデルの作成
    Field::MoveModel::FieldMoveModelHeaderWork     headerWork;
    Field::MoveModel::FieldMoveModelHeaderResource headerResource;
    headerWork.eventId                           = static_cast<u32>(FIELD_EVENTID_NULL);
    headerWork.moveCodeId                        = Field::MoveModel::FIELD_MOVE_CODE_NONE;
    headerResource.characterId                   = CHARA_MODEL_ID_TR0077_00;
    headerResource.pDressUpParam                 = NULL;
    headerResource.pCollisionSceneForStaticModel = pGameManager->GetFieldResident()->GetCollisionScene( FieldResident::COLLISION_SCENE_STATIC_MODEL  );
    pFieldMoveModelManager->InitializeMoveModelWork    ( modelId , &headerWork );
    pFieldMoveModelManager->InitializeMoveModelResource( modelId , &headerResource );

    // プレイヤーのSRTを初期SRTに
    Field::MoveModel::FieldMoveModel* pPlayerCharaModel = pFieldMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
    Field::MoveModel::FieldMoveModel* pNpcCharaModel    = pFieldMoveModelManager->GetFieldMoveModel( modelId );
    pNpcCharaModel->GetCharaDrawInstance()->SetPosition    ( pPlayerCharaModel->GetCharaDrawInstance()->GetPosition() + gfl2::math::Vector3(0.0f,0.0f,100.0f) );
    pNpcCharaModel->GetCharaDrawInstance()->SetRotationQuat( pPlayerCharaModel->GetCharaDrawInstance()->GetRotationQuat() );
    pNpcCharaModel->GetCharaDrawInstance()->SetScale       (1.0f, 1.0f, 1.0f);

    // Tスタンスを防ぐために適当にモーションを流す
    pNpcCharaModel->GetCharaDrawInstance()->ChangeAnimation(P2_BASE_FI129_POSE01);
    pNpcCharaModel->GetCharaDrawInstance()->SetAnimationLoop(true);

    // アタッチするジョイント名を取得
    Field::MoveModel::ApplicationDataForSubobject* pSubobjectData = (Field::MoveModel::ApplicationDataForSubobject*)(pFieldMoveModelManager->GetApplicationData( CHARA_MODEL_ID_OB0204_00 ));
    c8 jointName[Field::MoveModel::SUBOBJECT_JOINT_NAME_SIZE];
    for( u32 i=0; i<Field::MoveModel::SUBOBJECT_JOINT_NAME_SIZE; i++ ){ Field::MoveModel::utf16_to_ascii(pSubobjectData->jointName[i],&jointName[i]); }

    // 動作モデルに持たせる
    pNpcCharaModel->SetSubobject( 0,pFieldMoveModelManager->GetFieldSubobject(subobectIndex),jointName );
  }
}

/**
* @brief 動作モデルを作成
*/
static void CreateFieldMoveModel( void* userWork, gfl2::debug::DebugWinItem* item )
{
  // Aボタンを押していなければ動作中止
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 動作モデルマネージャーを取得
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = static_cast<Field::MoveModel::FieldMoveModelManager*>(userWork);

  // 対象のバイナリロード
  pFieldMoveModelManager->LoadStaticAsync(createCharacterId);
  while(true)
  {
    if( pFieldMoveModelManager->IsLoaded(createCharacterId) )
    {
      break;
    }
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }
  pFieldMoveModelManager->SetupModel(createCharacterId);

  // まだ利用されていないワークを検索
  u32 freeIndex = pFieldMoveModelManager->GetFieldSubobjectIndexFromFreeSpace();

  // まだ利用されていないワークを検索
  freeIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromFreeSpace();

  // サブオブジェクト以外を作成
  if( freeIndex == Field::MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    return;
  }

  Field::MoveModel::FIELD_MOVE_MODEL_ID modelId     = (Field::MoveModel::FIELD_MOVE_MODEL_ID)freeIndex;
  Field::MoveModel::FieldMoveModel*     pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(modelId);
  if( pCharaModel && !pCharaModel->IsSetup() )
  {
    // 動作モデルの作成
    Field::MoveModel::FieldMoveModelHeaderWork     headerWork;
    Field::MoveModel::FieldMoveModelHeaderResource headerResource;
    headerWork.eventId                           = static_cast<u32>(FIELD_EVENTID_NULL);
    headerWork.moveCodeId                        = Field::MoveModel::FIELD_MOVE_CODE_NONE;
    headerResource.characterId                   = createCharacterId;
    headerResource.pDressUpParam                 = NULL;
    headerResource.pCollisionSceneForStaticModel = pGameManager->GetFieldResident()->GetCollisionScene( FieldResident::COLLISION_SCENE_STATIC_MODEL  );
    pFieldMoveModelManager->InitializeMoveModelWork    ( modelId , &headerWork );
    pFieldMoveModelManager->InitializeMoveModelResource( modelId , &headerResource );

    // アプリケーションデータをコンバートリストから取得
    const Field::MoveModel::ApplicationDataForCommon* pAppData = pFieldMoveModelManager->GetApplicationData( createCharacterId );

    // プレイヤーのSRTを初期SRTに
    Field::MoveModel::FieldMoveModel* pPlayerCharaModel = pFieldMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
    Field::MoveModel::FieldMoveModel* pNpcCharaModel    = pFieldMoveModelManager->GetFieldMoveModel( modelId );
    pNpcCharaModel->GetCharaDrawInstance()->SetPosition    ( pPlayerCharaModel->GetCharaDrawInstance()->GetPosition() + gfl2::math::Vector3(0.0f,0.0f,100.0f) );
    pNpcCharaModel->GetCharaDrawInstance()->SetRotationQuat( pPlayerCharaModel->GetCharaDrawInstance()->GetRotationQuat() );
    pNpcCharaModel->GetCharaDrawInstance()->SetScale       (1.0f, 1.0f, 1.0f);

    // Tスタンスを防ぐために適当にモーションを流す
    pNpcCharaModel->GetCharaDrawInstance()->ChangeAnimation(0);
    pNpcCharaModel->GetCharaDrawInstance()->SetAnimationLoop(true);
  }
}

//=========================================================================================
// ブルーム用のデバッグ機能 yy-ikeuchi
//=========================================================================================

// ブルーム用
u32                 m_glareEnable;
f32                 m_glareStrength;
f32                 m_glareRange;
f32                 m_glareIntensity;
gfl2::math::Vector4  m_glareWeigth;

/**
* @brief ブルーム用デバッグ変数を取得
*/
static void InitDebugGlareParam( void *userWork )
{
  Field::EnvManager* pEnvManager = static_cast<Field::EnvManager*>(userWork);
  m_glareEnable     = pEnvManager->GetDrawEnvNode()->GetGlareEnable();
  m_glareStrength   = pEnvManager->GetDrawEnvNode()->GetGlareStrength();
  m_glareRange      = pEnvManager->GetDrawEnvNode()->GetGlareRange();
  m_glareIntensity  = pEnvManager->GetDrawEnvNode()->GetGlareIntensity();
  m_glareWeigth     = pEnvManager->GetDrawEnvNode()->GetGlareWeight();
}

/**
* @brief ブルーム用デバッグ変数を適応
*/
static void ApplyDebugGlareParam( void *userWork )
{
  Field::EnvManager* pEnvManager = static_cast<Field::EnvManager*>(userWork);
  pEnvManager->GetDrawEnvNode()->SetGlareEnable   ( m_glareEnable ? true : false );
  pEnvManager->GetDrawEnvNode()->SetGlareStrength (m_glareStrength);
  pEnvManager->GetDrawEnvNode()->SetGlareRange    (m_glareRange);
  pEnvManager->GetDrawEnvNode()->SetGlareIntensity(m_glareIntensity);
  pEnvManager->GetDrawEnvNode()->SetGlareWeight   (m_glareWeigth);
}

//=========================================================================================
// フォグ用のデバッグ機能 yy-ikeuchi
//=========================================================================================

// フォグ用
u32                 m_fogEnable;
f32                 m_fogStrength;
f32                 m_fogNearRange;
f32                 m_fogFarRange;
gfl2::math::Vector4  m_fogColor;

/**
* @brief フォグ用デバッグ変数を取得
*/
static void InitDebugFogParam( void *userWork )
{
  Field::EnvManager* pEnvManager = static_cast<Field::EnvManager*>(userWork);
  m_fogEnable     = pEnvManager->GetDrawEnvNode()->GetFogEnable();
  m_fogStrength   = pEnvManager->GetDrawEnvNode()->GetFogStrength();
  m_fogNearRange  = pEnvManager->GetDrawEnvNode()->GetFogNearLength();
  m_fogFarRange   = pEnvManager->GetDrawEnvNode()->GetFogFarLength();
  m_fogColor      = pEnvManager->GetDrawEnvNode()->GetFogColor();
}

/**
* @brief フォグ用デバッグ変数を適応
*/
static void ApplyDebugFogParam( void *userWork )
{
  Field::EnvManager* pEnvManager = static_cast<Field::EnvManager*>(userWork);
  pEnvManager->GetDrawEnvNode()->SetFogEnable    ( m_fogEnable ? true:false );
  pEnvManager->GetDrawEnvNode()->SetFogStrength  (m_fogStrength);
  pEnvManager->GetDrawEnvNode()->SetFogNearLength(m_fogNearRange);
  pEnvManager->GetDrawEnvNode()->SetFogFarLength (m_fogFarRange);
  pEnvManager->GetDrawEnvNode()->SetFogColor     (m_fogColor);
}

// デバッグメニューで項目を選択中に実行される関数
GFL_NAMESPACE_BEGIN( Item )

/**
* @brief デバッグスイッチを全て初期値にする
*/
static void UpdateDebugSwitchReset( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice*  pCross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );


  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    for( u32 i = 0; i < DebugTypes::Switch::MAX; i++ )
    {
      FIELD_DEBUG_SWITCH_SET( static_cast< DebugTypes::Switch::Value >( i ), DebugTypes::GetSwitchInitialValue( static_cast< DebugTypes::Switch::Value >( i ) ));
    }

    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

  
static void UpdateEaseFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice*  pCross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );


  s32 value = *(gfl2::math::Easing::EaseFunc*)userWork;
  if( pCross->IsRepeat( gfl2::ui::DIR_LEFT ) )
  {
    if( value == 0 )
    {
      value = gfl2::math::Easing::INOUT_BOUNCE;
    }
    else
    {
      value -= 1;
    }
    // xy_snd::PlaySE( SEQ_SE_SLIDE );
  }

  if( pCross->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    if( value >= gfl2::math::Easing::INOUT_BOUNCE )
    {
      value = 0;
    }
    else
    {
      value += 1;
    }
    // xy_snd::PlaySE( SEQ_SE_SLIDE );
  }

  *(gfl2::math::Easing::EaseFunc*)userWork = static_cast<gfl2::math::Easing::EaseFunc>( value );
}

static wchar_t* DrawEaseFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  s32 value = *(gfl2::math::Easing::EaseFunc*)userWork;

  wchar_t* table[] =
  {
    L"LINEAR",

    L"IN_SINE",
    L"OUT_SINE",
    L"INOUT_SINE",

    L"IN_QUAD",
    L"OUT_QUAD",
    L"INOUT_QUAD",

    L"IN_CUBIC",
    L"OUT_CUBIC",
    L"INOUT_CUBIC",

    L"IN_QUART",
    L"OUT_QUART",
    L"INOUT_QUART",

    L"IN_QUINT",
    L"OUT_QUINT",
    L"INOUT_QUINT",

    L"IN_EXPO",
    L"OUT_EXPO",
    L"INOUT_EXPO",

    L"IN_CIRC",
    L"OUT_CIRC",
    L"INOUT_CIRC",

    L"IN_BACK",
    L"OUT_BACK",
    L"INOUT_BACK",

    L"IN_ELASTIC",
    L"OUT_ELASTIC",
    L"INOUT_ELASTIC",

    L"IN_BOUNCE",
    L"OUT_BOUNCE",
    L"INOUT_BOUNCE"
  };

  if( value >= GFL_NELEMS( table ) )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ERROR(%d)]", item->GetName(), value );
  }
  else
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), table[ value ] );
  }
  return workStr;
}


// PosJumpグループ
GFL_NAMESPACE_BEGIN( PosJump )

static wchar_t* DrawNowBlockIndex( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
  if( pFieldmap == NULL )
  {
    return NULL;
  }

  // 自機座標を取得
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pFieldmap->GetMoveModelManager();
  Field::MoveModel::FieldMoveModel* pPlayerCharaModel = pFieldMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
  gfl2::math::Vector3 playerPos;
  if( pPlayerCharaModel == NULL )
  {
    return NULL;
  }

  playerPos = pPlayerCharaModel->GetCharaDrawInstance()->GetPosition();


  // 自機座標から地形ブロック番号を取得
  s32 x, z;
  Field::Terrain::TerrainManager* pTerrainManager = pFieldmap->GetTerrainManager();
  if( pTerrainManager->GetBlockIndex( playerPos, &x, &z ) == false )
  {
    return NULL;
  }

  wchar_t* pWorkStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( pWorkStr, gfl2::debug::DEBUGWIN_NAME_LEN ,L"%ls X[%d] Z[%d]", item->GetName(), x, z );
  return pWorkStr;
}


// ジャンプ実行
static void Jump( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  // gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
  if( pFieldmap == NULL )
  {
    return;
  }

  // 自機を取得
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = static_cast<Field::MoveModel::FieldMoveModelManager*>(userWork);
  Field::MoveModel::FieldMoveModel* pPlayerCharaModel = pFieldMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
  if( pPlayerCharaModel == NULL )
  {
    return;
  }

  // 地形ブロックのサイズを取得
  Field::Terrain::TerrainManager* pTerrainManager = pFieldmap->GetTerrainManager();
  const Field::Terrain::TerrainLayoutData* pLayoutData = pTerrainManager->GetLayoutData();
  f32 blockSize = pLayoutData->GetBlockSize();
  gfl2::math::Vector3 blockStartPosition;
  pLayoutData->GetBlockStartPosition( &blockStartPosition );

  // 飛び先の座標作成
  gfl2::math::Vector3 newPos( pPlayerCharaModel->GetCharaDrawInstance()->GetPosition() );
  newPos.x = blockStartPosition.x + Field::Debug::DebugTypes::s_PosJumpBlockIndexX * blockSize;
  newPos.z = blockStartPosition.z + Field::Debug::DebugTypes::s_PosJumpBlockIndexZ * blockSize;
  pPlayerCharaModel->GetCharaDrawInstance()->SetPosition( newPos );

  // 決定音
  Sound::PlaySE( SEQ_SE_DECIDE1 );
}

GFL_NAMESPACE_END( PosJump )

GFL_NAMESPACE_END( Item )


static void CallReportEvent( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  // gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
  if( pFieldmap == NULL )
  {
    return;
  }
  EventFieldReportPlayerTest::StartEvent(pGameManager);
}



//-------------------------------------------------------------------
/**
 * フィールドアイテム配置関連
 */
//-------------------------------------------------------------------

static u32 createItemID;
static u32 itemShapeID;

/**
* @brief ドロップアイテムを作成
*/
static void CreateDropItem( void* userWork, gfl2::debug::DebugWinItem* item )
{
  // Aボタンを押していなければ動作中止
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }
  // フィールドマップ取得
  Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
  if( pFieldmap == NULL )
  {
    return;
  }

  // 配置マネージャー取得
  Field::PlacementDataManager* pPlacement = pGameManager->GetFieldResident()->GetPlacementDataManager();
  if( pPlacement == NULL )
  {
    return;
  }
  // フィールドアイテムマネージャーを取得
  Field::FieldItemManager* pFieldItemManager = static_cast<Field::FieldItemManager*>(userWork);

  // プレイヤーキャラを取得
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pFieldmap->GetMoveModelManager();
  Field::MoveModel::FieldMoveModel* pPlayerCharaModel = pFieldMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
  if( pPlayerCharaModel == NULL )
  {
    return;
  }
  gfl2::math::Vector3 pos = pPlayerCharaModel->GetCharaDrawInstance()->GetPosition();
  gfl2::math::Vector3 ofs = pPlayerCharaModel->GetCharaDrawInstance()->GetRotationQuat().Transform( gfl2::math::Vector3(0.0f,0.0f,100.0f) );
  u32 index; 
//  pFieldItemManager->CreateDropItemData( pos + ofs, createItemID, index );
  pPlacement->RegistDropItem( createItemID, 1, pos + ofs, Field::FieldItemManager::TYPE_NORMAL );
}


static void DebugPokemonRide( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager* p_gman = static_cast< GameSys::GameManager* >( userWork );

  // Aボタンを押していなければ動作中止
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }
  // デバッグ用自動選択
  Field::RIDE_POKEMON_ID pokeID = Field::EventPokemonRideOn::DebugAutoSelectRidePokemon( p_gman );

  if ( pokeID != Field::RIDE_NO_POKEMON )
  {
    Field::EventPokemonRideOn* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventPokemonRideOn>( p_gman->GetGameEventManager() );
    pEvent->SetPokemonRideOnID( p_gman, pokeID );
    return;
  }
  else if( Field::EventPokemonRideOff::IsPokemonRideOffEnable( p_gman ) == RCR_OK )
  {
    Field::EventPokemonRideOff* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventPokemonRideOff>( p_gman->GetGameEventManager() );
    return;
  }
}

//-------------------------------------------------------------------
/**
 * フィールド`視線トレーナー関連
 */
//-------------------------------------------------------------------
/**
* @brief トレーナーフラグのセットを作成
*/
static void UpdateSetTrainerFlags( void* userWork, gfl2::debug::DebugWinItem* item )
{
  // Aボタンを押していなければ動作中止
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  bool set_f = GFL_BOOL_CAST(userWork);
  EventWork* pEV = pGameManager->GetGameData()->GetEventWork();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }
  if( set_f ){
    for( u32 i = TR_FLAG_AREA_START; i <= TR_FLAG_AREA_END; i++ ){
      pEV->SetEventFlag(i);
    }  
  }else{
    for( u32 i = TR_FLAG_AREA_START; i <= TR_FLAG_AREA_END; i++ ){
      pEV->ResetEventFlag(i);
    }  
  }
}


//-------------------------------------------------------------------
/**
 * フィールド 空を飛ぶ、到着フラグ関連
 */
//-------------------------------------------------------------------
/**
* @brief  空を飛ぶ、到着フラグのセットを作成
*/
static void UpdateSetSkyAndArriveFlags( void* userWork, gfl2::debug::DebugWinItem* item )
{
  // Aボタンを押していなければ動作中止
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  int mode = item->GetScrId()/2;
  int set_f = item->GetScrId()%2;
  EventWork* pEV = pGameManager->GetGameData()->GetEventWork();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }
  if( mode == 0 || mode == 1 ){
    //空を飛ぶフラグ
    pGameManager->GetGameData()->GetWarpDataTools()->DebugSetFlyFlags( pEV, set_f );
  }
  if( mode == 0 || mode == 2 )
  {
    //到着フラグ
    if( set_f ){
      for( u32 i = SYS_FLAG_ARRIVE_F0102R0101; i <= SYS_FLAG_ARRIVE_F0301D0201; i++ ){
        pEV->SetEventFlag(i);
      }
      for( u32 i = SYS_FLAG_ARRIVE_F0102S0201; i <= SYS_FLAG_ARRIVE_F0301T0301; i++ ){
        pEV->SetEventFlag(i);
      }
      for( u32 i = SYS_FLAG_ARRIVE_F0301D0301; i <= SYS_FLAG_ARRIVE_F0401S0601; i++ ){
        pEV->SetEventFlag(i);
      }
      for( u32 i = SYS_FLAG_ARRIVE_F0401S0701; i <= SYS_FLAG_ARRIVE_F0501I0101; i++ ){
        pEV->SetEventFlag(i);
      }
    }else{
      for( u32 i = SYS_FLAG_ARRIVE_F0102R0101; i <= SYS_FLAG_ARRIVE_F0301D0201; i++ ){
        pEV->ResetEventFlag(i);
      }
      for( u32 i = SYS_FLAG_ARRIVE_F0102S0201; i <= SYS_FLAG_ARRIVE_F0301T0301; i++ ){
        pEV->ResetEventFlag(i);
      }
      for( u32 i = SYS_FLAG_ARRIVE_F0301D0301; i <= SYS_FLAG_ARRIVE_F0401S0601; i++ ){
        pEV->ResetEventFlag(i);
      }
      for( u32 i = SYS_FLAG_ARRIVE_F0401S0701; i <= SYS_FLAG_ARRIVE_F0501I0101; i++ ){
        pEV->ResetEventFlag(i);
      }
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  ポケモンダイアリー関連
 */
//-----------------------------------------------------------------------------
static void DebugWinFunc_PokeDiaryDumpCount( void* userWork, gfl2::debug::DebugWinItem* item );

static wchar_t* DebugWinFunc_PokeDiaryList( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_PokeDiaryTypeDraw( void* userWork, gfl2::debug::DebugWinItem* item );
static void DebugWinFunc_PokeDiaryWrite( void* userWork, gfl2::debug::DebugWinItem* item );

static void dumpPokeDiary( void * userWork, Savedata::PokeDiarySave::DiaryType type )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );

  {
    GameSys::GameManager* gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    const gfl2::ui::Button*   btn  = gfl2::debug::DebugWin_GetUiButton();

    if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      Savedata::PokeDiarySave * svPokeDiary = gman->GetGameData()->GetPokeDiarySave();
      svPokeDiary->Dump( type );
    }
  }
}

//-----------------------------------------------------------------------------
static void DebugWinFunc_PokeDiaryDumpCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  dumpPokeDiary( userWork, Savedata::PokeDiarySave::DIARYTYPE_COUNT );
}


/* ------------------------------------------------------------------------- */
/**
 * @brief ポケダイアリーデータ 描画
 *
 * @param userWork
 * @param item
 */
/* ------------------------------------------------------------------------- */
static wchar_t* DebugWinFunc_PokeDiaryList( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>( userWork );
  Savedata::PokeDiarySave * svPokeDiary = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPokeDiarySave();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  Savedata::PokeDiarySave::DiaryType type;
  type = static_cast<Savedata::PokeDiarySave::DiaryType>(pUserWork->datId);

  svPokeDiary->SetStr( workStr, type, pUserWork->labelId );

  return workStr;
}



/* ------------------------------------------------------------------------- */
/**
 * @brief ポケダイアリータイプ 描画
 *
 * @param userWork
 * @param item
 */
/* ------------------------------------------------------------------------- */
static wchar_t* DebugWinFunc_PokeDiaryTypeDraw( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  FieldDebugMenu* dbgmenu = reinterpret_cast<FieldDebugMenu*>(userWork);

  static wchar_t *diaryTypeText[1] = {
    L"COUNT"
  };

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), diaryTypeText[ dbgmenu->m_diaryType ] );

  return workStr;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ポケダイアリー実行 更新
 *
 * @param userWork
 * @param item
 */
/* ------------------------------------------------------------------------- */
static void DebugWinFunc_PokeDiaryWrite( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu* dbgmenu = reinterpret_cast<FieldDebugMenu*>(userWork);
  GameSys::GameManager* gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const gfl2::ui::Button*   btn  = gfl2::debug::DebugWin_GetUiButton();
  Savedata::PokeDiarySave * svPokeDiary = gman->GetGameData()->GetPokeDiarySave();

  if ( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    svPokeDiary->SetDebugData( Savedata::PokeDiarySave::DIARYTYPE_COUNT,
                               (PDID)dbgmenu->m_diaryId,
                              dbgmenu->m_diaryRomCode,
                             (int*)dbgmenu->m_diaryData );
    //item->PlaySound( gfl::debug::SE_ACCEPT );
  }
}


//-----------------------------------------------------------------------------
/*
 *  ふしぎなおくりもの
 */
//-----------------------------------------------------------------------------
static void DebugWinFunc_MisteryGiftSetTestData( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu* dbgmenu = reinterpret_cast<FieldDebugMenu*>(userWork);
  GameSys::GameManager* gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const gfl2::ui::Button*   btn  = gfl2::debug::DebugWin_GetUiButton();

  if ( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::MysteryGiftSave* svGift = gman->GetGameData()->GetSaveData()->GetMysteryGiftSave();
    Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA giftData;
    ::std::memset( &giftData , 0 , sizeof(giftData) );

    Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA* pPoke = (Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*)(giftData.body);
    Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA* pItem = (Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA*)(giftData.body);
    u32* pU32 = (u32*)(giftData.body);

    giftData.dataType = Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke;
    pPoke->monsNo = 25;
    pPoke->level = 50;
    pPoke->getPlace = 40004;
    pPoke->bornPlace = 40004;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 0 , &giftData );

    pPoke->monsNo = 26;
    pPoke->getPlace = 40005;
    pPoke->bornPlace = 40005;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 1 , &giftData );

    giftData.dataType = Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_item;
    pItem->itemNo1 = 1;
    pItem->itemNum1 = 1;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 2 , &giftData );

    pItem->itemNo1 = 2;
    pItem->itemNum1 = 2;
    pItem->itemNo2 = 3;
    pItem->itemNum2 = 3;
    pItem->itemNo3 = 4;
    pItem->itemNum3 = 4;
    pItem->itemNo4 = 5;
    pItem->itemNum4 = 5;
    pItem->itemNo5 = 6;
    pItem->itemNum5 = 6;
    pItem->itemNo6 = 7;
    pItem->itemNum6 = 7;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 3 , &giftData );

    giftData.dataType = Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_bp;
    *pU32 = 500;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 4 , &giftData );

    *pU32 = 6000;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 5 , &giftData );

    giftData.dataType = Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_mame;
    *pU32 = 5;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 6 , &giftData );

    *pU32 = 60;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 7 , &giftData );
  }
}

static void DebugWinFunc_MisteryGiftSetPokeDataMine( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu* dbgmenu = reinterpret_cast<FieldDebugMenu*>(userWork);
  GameSys::GameManager* gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const gfl2::ui::Button*   btn  = gfl2::debug::DebugWin_GetUiButton();

  if ( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::MysteryGiftSave* svGift = gman->GetGameData()->GetSaveData()->GetMysteryGiftSave();
    Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA giftData;
    ::std::memset( &giftData , 0 , sizeof(giftData) );

    Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA* pPoke = (Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*)(giftData.body);
    Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA* pItem = (Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA*)(giftData.body);
    u32* pU32 = (u32*)(giftData.body);

    giftData.dataType = Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke;
    pPoke->monsNo = 25;
    pPoke->level = 50;
    pPoke->getPlace = 40004;
    pPoke->bornPlace = 40004;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 0 , &giftData );

    pPoke->monsNo = 26;
    pPoke->getPlace = 40005;
    pPoke->bornPlace = 40005;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 1 , &giftData );
  }
}


static void DebugWinFunc_MisteryGiftSetPokeDataOthers( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu* dbgmenu = reinterpret_cast<FieldDebugMenu*>(userWork);
  GameSys::GameManager* gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const gfl2::ui::Button*   btn  = gfl2::debug::DebugWin_GetUiButton();

  if ( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::MysteryGiftSave* svGift = gman->GetGameData()->GetSaveData()->GetMysteryGiftSave();
    Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA giftData;
    ::std::memset( &giftData , 0 , sizeof(giftData) );

    Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA* pPoke = (Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*)(giftData.body);
    Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA* pItem = (Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA*)(giftData.body);
    u32* pU32 = (u32*)(giftData.body);

    giftData.dataType = Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke;
    pPoke->monsNo = 25;
    pPoke->level = 50;
    pPoke->parentID = 1234;
    pPoke->getPlace = 40004;
    pPoke->bornPlace = 40004;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 0 , &giftData );

    pPoke->monsNo = 26;
    pPoke->getPlace = 40005;
    pPoke->bornPlace = 40005;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 1 , &giftData );
  }
}



static void DebugWinFunc_MisteryGiftSetPokeDataEgg( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu* dbgmenu = reinterpret_cast<FieldDebugMenu*>(userWork);
  GameSys::GameManager* gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const gfl2::ui::Button*   btn  = gfl2::debug::DebugWin_GetUiButton();

  if ( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::MysteryGiftSave* svGift = gman->GetGameData()->GetSaveData()->GetMysteryGiftSave();
    Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA giftData;
    ::std::memset( &giftData , 0 , sizeof(giftData) );

    Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA* pPoke = (Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*)(giftData.body);
    Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA* pItem = (Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA*)(giftData.body);
    u32* pU32 = (u32*)(giftData.body);

    giftData.dataType = Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke;
    pPoke->monsNo = 25;
    pPoke->level = 50;
    pPoke->egg = 1;
    pPoke->getPlace = 40004;
    pPoke->bornPlace = 40004;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 0 , &giftData );

    pPoke->monsNo = 26;
    pPoke->getPlace = 40005;
    pPoke->bornPlace = 40005;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 1 , &giftData );
  }
}


static void DebugWinFunc_MisteryGiftSetPokeDataWithItem( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu* dbgmenu = reinterpret_cast<FieldDebugMenu*>(userWork);
  GameSys::GameManager* gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const gfl2::ui::Button*   btn  = gfl2::debug::DebugWin_GetUiButton();

  if ( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::MysteryGiftSave* svGift = gman->GetGameData()->GetSaveData()->GetMysteryGiftSave();
    Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA giftData;
    ::std::memset( &giftData , 0 , sizeof(giftData) );

    Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA* pPoke = (Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*)(giftData.body);
    Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA* pItem = (Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA*)(giftData.body);
    u32* pU32 = (u32*)(giftData.body);

    giftData.dataType = Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke;
    pPoke->monsNo = 25;
    pPoke->level = 50;
    pPoke->setItem = 20;
    pPoke->getPlace = 40004;
    pPoke->bornPlace = 40004;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 0 , &giftData );

    pPoke->monsNo = 26;
    pPoke->setItem = 21;
    pPoke->getPlace = 40005;
    pPoke->bornPlace = 40005;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 1 , &giftData );
  }
}


static void DebugWinFunc_MisteryGiftSetItemData( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu* dbgmenu = reinterpret_cast<FieldDebugMenu*>(userWork);
  GameSys::GameManager* gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const gfl2::ui::Button*   btn  = gfl2::debug::DebugWin_GetUiButton();

  if ( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::MysteryGiftSave* svGift = gman->GetGameData()->GetSaveData()->GetMysteryGiftSave();
    Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA giftData;
    ::std::memset( &giftData , 0 , sizeof(giftData) );

    Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA* pPoke = (Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*)(giftData.body);
    Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA* pItem = (Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA*)(giftData.body);
    u32* pU32 = (u32*)(giftData.body);



    giftData.dataType = Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_item;
    pItem->itemNo1 = 1;
    pItem->itemNum1 = 1;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 2 , &giftData );

    pItem->itemNo1 = 2;
    pItem->itemNum1 = 2;
    pItem->itemNo2 = 3;
    pItem->itemNum2 = 3;
    pItem->itemNo3 = 4;
    pItem->itemNum3 = 4;
    pItem->itemNo4 = 5;
    pItem->itemNum4 = 5;
    pItem->itemNo5 = 6;
    pItem->itemNum5 = 6;
    pItem->itemNo6 = 7;
    pItem->itemNum6 = 7;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 3 , &giftData );

  }
}


static void DebugWinFunc_MisteryGiftSetBPData( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu* dbgmenu = reinterpret_cast<FieldDebugMenu*>(userWork);
  GameSys::GameManager* gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const gfl2::ui::Button*   btn  = gfl2::debug::DebugWin_GetUiButton();

  if ( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::MysteryGiftSave* svGift = gman->GetGameData()->GetSaveData()->GetMysteryGiftSave();
    Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA giftData;
    ::std::memset( &giftData , 0 , sizeof(giftData) );

    Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA* pPoke = (Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*)(giftData.body);
    Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA* pItem = (Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA*)(giftData.body);
    u32* pU32 = (u32*)(giftData.body);

    giftData.dataType = Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_bp;
    *pU32 = 500;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 4 , &giftData );

    *pU32 = 6000;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 5 , &giftData );
  }
}



static void DebugWinFunc_MisteryGiftSetMameData( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu* dbgmenu = reinterpret_cast<FieldDebugMenu*>(userWork);
  GameSys::GameManager* gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const gfl2::ui::Button*   btn  = gfl2::debug::DebugWin_GetUiButton();

  if ( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::MysteryGiftSave* svGift = gman->GetGameData()->GetSaveData()->GetMysteryGiftSave();
    Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA giftData;
    ::std::memset( &giftData , 0 , sizeof(giftData) );

    Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA* pPoke = (Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*)(giftData.body);
    Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA* pItem = (Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA*)(giftData.body);
    u32* pU32 = (u32*)(giftData.body);

    giftData.dataType = Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_mame;
    *pU32 = 5;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 6 , &giftData );

    *pU32 = 60;
    ::std::memcpy( giftData.eventName , L"テストデータ" , 14 );
    svGift->SetGiftData( 7 , &giftData );
  }
}


static void DebugWinFunc_MisteryGiftClearTestData( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu* dbgmenu = reinterpret_cast<FieldDebugMenu*>(userWork);
  GameSys::GameManager* gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const gfl2::ui::Button*   btn  = gfl2::debug::DebugWin_GetUiButton();

  if ( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::MysteryGiftSave* svGift = gman->GetGameData()->GetSaveData()->GetMysteryGiftSave();

    u32 pokeNum = svGift->GetPokeGiftNum();
    u32 itemNum = svGift->GetItemGiftNum();
    u32 bpNum = svGift->GetBPGiftNum();
    u32 mameNum = svGift->GetMameGiftNum();

    GFL_PRINT("poke[%d]item[%d]bp[%d]mame[%d]\n",pokeNum,itemNum,bpNum,mameNum);

    pml::pokepara::PokemonParam* pPoke = GFL_NEW( dbgmenu->GetHeap() ) pml::pokepara::PokemonParam( dbgmenu->GetHeap() );
    for( u32 index = 0 ; index < pokeNum ; index++ )
    {
      svGift->GetTopPokeGiftData( pPoke , dbgmenu->GetHeap() );
      GFL_PRINT("remove poke[%d]\n",pPoke->GetMonsNo());
    }

    Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA itemGift;
    std::memset( &itemGift , 0 , sizeof(itemGift) );
    for( u32 index = 0 ; index < itemNum ; index++ )
    {
      svGift->GetTopItemGiftData( &itemGift );
      GFL_PRINT("remove item1[%d][%d]\n",itemGift.itemNo1,itemGift.itemNum1);
      GFL_PRINT("remove item2[%d][%d]\n",itemGift.itemNo2,itemGift.itemNum2);
      GFL_PRINT("remove item3[%d][%d]\n",itemGift.itemNo3,itemGift.itemNum3);
      GFL_PRINT("remove item4[%d][%d]\n",itemGift.itemNo4,itemGift.itemNum4);
      GFL_PRINT("remove item5[%d][%d]\n",itemGift.itemNo5,itemGift.itemNum5);
      GFL_PRINT("remove item6[%d][%d]\n",itemGift.itemNo6,itemGift.itemNum6);
    }

    u32 value = 0;
    for( u32 index = 0 ; index < bpNum ; index++ )
    {
      value = svGift->GetTopBPGiftData();
      GFL_PRINT("remove BP[%d]\n",value);
    }
    for( u32 index = 0 ; index < mameNum ; index++ )
    {
      value = svGift->GetTopMameGiftData();
      GFL_PRINT("remove mame[%d]\n",value);
    }
  }
}




//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//  DebugWinGroup用コンストラクタ、デストラクタ関数
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// 共通デストラクタ関数
static void DestroyGroup( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}
// 歩数グループ作成
static void CreateGroup_CreateStep( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_move_step_count, &FieldEventCheck::m_DebugMoveStepCount, 0, 256, 1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_step_count, &FieldEventCheck::m_DebugStepCount, 0, 256, 1);

  }
}

// 動作モデル 作成
static void CreateGroup_CreateMoveModel( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pDebug->GetMoveModelManager();
    // キャラクターID選択
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_movemodel_id, &createCharacterId, 0, 512, 1);
    // キャラクター作成
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_movemodel_create, pFieldMoveModelManager, CreateFieldMoveModel, NULL );
    // ダミートレーナー作成
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, L"ダミートレーナーを作成" , pFieldMoveModelManager, CreateDummyTrainer, NULL );
    // プレイヤーに釣竿をアタッチ
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, L"プレーヤーに釣竿をアタッチ" , pFieldMoveModelManager, CreateTsurizao, NULL );

  }
}
// フォグ設定
static void CreateGroup_setupFog( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    Field::EnvManager* pEnvManager = pDebug->GetEnvManager();
    gfl2::debug::DebugWinItemValueU32  <u32>* itemU32   = NULL;
    gfl2::debug::DebugWinItemValueFloat<f32>* itemF32   = NULL;
    itemU32 = gfl2::debug::DebugWin_AddItemValueU32   ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_fog_enable, &m_fogEnable    , 0   , 1         , 1     );
    itemU32->SetChangedValueFunc( ApplyDebugFogParam , pEnvManager );
    itemF32 = gfl2::debug::DebugWin_AddItemValueFloat ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_fog_strength, &m_fogStrength  , 0.0f, 10.0f     , 0.1f  );
    itemF32->SetChangedValueFunc( ApplyDebugFogParam , pEnvManager );
    itemF32 = gfl2::debug::DebugWin_AddItemValueFloat ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_fog_near, &m_fogNearRange , 0.0f, 100000.0f , 1.0f  );
    itemF32->SetChangedValueFunc( ApplyDebugFogParam , pEnvManager );
    itemF32 = gfl2::debug::DebugWin_AddItemValueFloat ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_fog_far, &m_fogFarRange  , 0.0f, 100000.0f , 1.0f  );
    itemF32->SetChangedValueFunc( ApplyDebugFogParam , pEnvManager );
    itemF32 = gfl2::debug::DebugWin_AddItemValueFloat ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_fog_col_r, &m_fogColor.x   , 0.0f, 1.0f      , 0.01f );
    itemF32->SetChangedValueFunc( ApplyDebugFogParam , pEnvManager );
    itemF32 = gfl2::debug::DebugWin_AddItemValueFloat ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_fog_col_g, &m_fogColor.y   , 0.0f, 1.0f      , 0.01f );
    itemF32->SetChangedValueFunc( ApplyDebugFogParam , pEnvManager );
    itemF32 = gfl2::debug::DebugWin_AddItemValueFloat ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_fog_col_b, &m_fogColor.z   , 0.0f, 1.0f      , 0.01f );
    itemF32->SetChangedValueFunc( ApplyDebugFogParam , pEnvManager );
  }
}
// ブルーム制御
static void CreateGroup_setupGlare( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    Field::EnvManager* pEnvManager = pDebug->GetEnvManager();
    gfl2::debug::DebugWinItemValueU32  <u32>* itemU32   = NULL;
    gfl2::debug::DebugWinItemValueFloat<f32>* itemF32   = NULL;
    itemU32 = gfl2::debug::DebugWin_AddItemValueU32   ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_glare_enable, &m_glareEnable    , 0   , 1     , 1     );
    itemU32->SetChangedValueFunc( ApplyDebugGlareParam , pEnvManager );
    itemF32 = gfl2::debug::DebugWin_AddItemValueFloat ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_glare_strength, &m_glareStrength  , 0.0f, 10.0f , 0.01f );
    itemF32->SetChangedValueFunc( ApplyDebugGlareParam , pEnvManager );
    itemF32 = gfl2::debug::DebugWin_AddItemValueFloat ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_glare_range, &m_glareRange     , 0.0f, 50.0f , 1.0f  );
    itemF32->SetChangedValueFunc( ApplyDebugGlareParam , pEnvManager );
    itemF32 = gfl2::debug::DebugWin_AddItemValueFloat ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_glare_intensity, &m_glareIntensity , 0.0f, 10.0f , 0.01f );
    itemF32->SetChangedValueFunc( ApplyDebugGlareParam , pEnvManager );
    itemF32 = gfl2::debug::DebugWin_AddItemValueFloat ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_glare_weight_r, &m_glareWeigth.x  , 0.0f, 1.0f  , 0.01f );
    itemF32->SetChangedValueFunc( ApplyDebugGlareParam , pEnvManager );
    itemF32 = gfl2::debug::DebugWin_AddItemValueFloat ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_glare_weight_g, &m_glareWeigth.y  , 0.0f, 1.0f  , 0.01f );
    itemF32->SetChangedValueFunc( ApplyDebugGlareParam , pEnvManager );
    itemF32 = gfl2::debug::DebugWin_AddItemValueFloat ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_glare_weight_b, &m_glareWeigth.z  , 0.0f, 1.0f  , 0.01f );
    itemF32->SetChangedValueFunc( ApplyDebugGlareParam , pEnvManager );
  }
}
// POS制御
static void CreateGroup_PosJump( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pDebug->GetMoveModelManager();

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_posjump_now_block, NULL, NULL, Item::PosJump::DrawNowBlockIndex );
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_posjump_block_x, &Debug::DebugTypes::s_PosJumpBlockIndexX, 0, 29, 1);
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_posjump_block_z, &Debug::DebugTypes::s_PosJumpBlockIndexZ, 0, 29, 1);

    // ジャンプ実行
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_posjump_exec, pFieldMoveModelManager, Item::PosJump::Jump, NULL );
  }
}
// 制御フラグ関連
static void CreateGroup_Flagwork( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    
    // 各小項目を追加
    for( u32 i = 1; i < GFL_NELEMS( sc_SwitchGroupTable ); i++ )
    {
      const SwitchGroupInfo* pInfo = &sc_SwitchGroupTable[i];
      gfl2::debug::DebugWinGroup *pChildGrop = gfl2::debug::DebugWin_AddGroup( pHeap, pInfo->groupName, pGropWork, pInfo->switchListSize );
      for( u32 k = 0; k < pInfo->switchListSize; k++ )
      {
        DebugTypes::Switch::Value switchValue =  static_cast< DebugTypes::Switch::Value >( pInfo->switchList[ k ] );

        gfl2::debug::DebugWin_AddItemValueInt( 
          pChildGrop, 
          pHeap,
          DebugTypes::GetSwitchLabel( switchValue ),
          DebugTypes::GetSwitchValuePtr( switchValue ),
          0,  // 最小値
          DebugTypes::GetSwitchCountMax( switchValue ),  // 最大値
          1   // 変更量
          );
      }
    }

    // リセットボタンを追加
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, L"Reset", NULL, Item::UpdateDebugSwitchReset, NULL );
  }
}

// 時間操作関連.
static void CreateGroup_Time( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pTimeGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pTimeGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    FieldDebugMenu::USER_WORK *pUserWorkTimeControlArray = pDebug->GetUserWorkTimeCtrl();

    // 時間を表示
    gfl2::debug::DebugWin_AddItemUpdateFunc( pTimeGroup, pHeap, L"time", NULL, NULL, DrawDebugWriteDrawTime );

    for( u32 ii=0; ii<GFL_NELEMS(sc_TIMECONTROL); ++ii )
    {
      // Itemクラスが文字列と関数ポインタしか保持しないので、userWorkを利用して強引にラベルIDを保持させる
      pUserWorkTimeControlArray[ii].pThis = pDebug;
      pUserWorkTimeControlArray[ii].labelId = sc_TIMECONTROL[ii].labelId;
      gfl2::debug::DebugWin_AddItemUpdateFunc( pTimeGroup , pHeap , pDebug->GetMessageData(), sc_TIMECONTROL[ii].labelId, &pUserWorkTimeControlArray[ii] , DebugWinFunc_CallFakeTime , NULL );
    }


    // 反転時間の世界にジャンプする。
    gfl2::debug::DebugWin_AddItemUpdateFunc( pTimeGroup , pHeap , pDebug->GetMessageData(), msg_dmenu_time_alola_reverse_jump, pDebug, DebugWinFunc_CallAlolaTimeReverseMapJump , NULL );
  }
}
// イベント起動
static void CreateGroup_StartEvent( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGroupWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroupWork )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::debug::DebugWin_AddItemValueFloat  ( pGroupWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_event_area, &FieldEventCheck::m_DebugTalkCheckCosLimit, 0.0f, 90.0f, 1.0f);

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroupWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_report_effect, pDebug, CallReportEvent, NULL );
  }
}
// 立体視調整
static void CreateGroup_Display( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_stereo_depth_lv, pDebug->GetDepthLevelPtr(), 1.0f, 1000.0f, 1.0f );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pDebug->GetMessageData(), msg_dmenu_field_stereo_factor, pDebug->GetFactorPtr(), 0.01f, 1.0f, 0.01f );
  }
}
// EnvSound
static void CreateGroup_EnvSound( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGroupWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroupWork )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::debug::DebugWin_AddItemValueFloat  ( pGroupWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_env_sound_pan_start_deg, &EnvSoundManager::PAN_CONTROL_START_COS, 0.0f, 180.0f, 0.10f);
    gfl2::debug::DebugWin_AddItemValueFloat  ( pGroupWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_env_sound_pan_end_deg, &EnvSoundManager::PAN_CONTROL_END_COS, 0.0f, 180.0f, 0.10f);
    gfl2::debug::DebugWin_AddItemValueFloat  ( pGroupWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_env_sound_pan_power, &EnvSoundManager::PAN_CONTROL_POWER, -1.0f, 1.0f, 0.10f);
  }
}
// 配置アイテム 生成
static void CreateGroup_Item( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    Field::FieldItemManager* pItemManager = pDebug->GetItemManager();
    // アイテムID選択
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_item_id, &createItemID, 0, 999, 1);
    // キャラクター作成
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, pDebug->GetMessageData(), msg_dmenu_field_item_create, pItemManager, CreateDropItem, NULL );
  }
}

static void DebugWinFunc_ScriptEventWork( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_ScriptEventWorkDraw( void* userWork, gfl2::debug::DebugWinItem* item );
static void DebugWinFunc_ScriptEventWorkForSearch( void* userWork, gfl2::debug::DebugWinItem* item );
static wchar_t* DebugWinFunc_ScriptEventWorkDrawForSearch( void* userWork, gfl2::debug::DebugWinItem* item );

void FieldDebugMenu::SetFlagWorkPtr( gfl2::debug::DebugWinGroup *pGroup, s32 index )
{
  m_pFlagWork[index] = pGroup;
}
void FieldDebugMenu::FreeFlagWorkPtr( s32 index )
{
  if( m_pFlagWork[index] != NULL ){
    gfl2::debug::DebugWin_RemoveGroup( m_pFlagWork[index] );
    m_pFlagWork[index] = NULL;
  }
}

// フラグワーク個別
static void CreateGroup_FlagworkCore( void *userWork, gfl2::debug::DebugWinItem *item, s32 index )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  if( pDebug && pGropWork )
  {

    gfl2::heap::HeapBase *heap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();

    // メモリ削減のため、開いた際に項目分のメモリ確保を行う
    pGropWork->RemoveAllItem();
    pGropWork->ResizeItem( pDebug->GetEventWorkTableSize( index ), heap );
    
    FlagWorkImpl* pFlagWorkImpl = pDebug->GetFlagWorkImpl();
    pFlagWorkImpl->SyncLoadLabel( heap, index );

    for (u32 i = 0; i < pFlagWorkImpl->GetSize(); ++i)
    {
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , L"item", (void*)pFlagWorkImpl->GetLabelPtr(i), FlagWorkImpl::UpdateFlagWorkItem , FlagWorkImpl::DrawFlagWorkItem );
    }

    GFL_RELEASE_PRINT( "CreateGroup Function End, Field debug menu use Heap, TotalFreeSize=[%d]\n", heap->GetTotalFreeSize() );
  }
}



void ConvSearchWord( gfl2::str::StrBuf* pStrBuf, c8* pOut )
{
#if 1
  struct ConvWork
  {
    const gfl2::str::STRCODE* w;
    char c;
  };

  static const ConvWork s_Table[] =
  {
    { L"Ａ", 'A' },
    { L"Ｂ", 'B' },
    { L"Ｃ", 'C' },
    { L"Ｄ", 'D' },
    { L"Ｅ", 'E' },
    { L"Ｆ", 'F' },
    { L"Ｇ", 'G' },
    { L"Ｈ", 'H' },
    { L"Ｉ", 'I' },
    { L"Ｊ", 'J' },
    { L"Ｋ", 'K' },
    { L"Ｌ", 'L' },
    { L"Ｍ", 'M' },
    { L"Ｎ", 'N' },
    { L"Ｏ", 'O' },
    { L"Ｐ", 'P' },
    { L"Ｑ", 'Q' },
    { L"Ｒ", 'R' },
    { L"Ｓ", 'S' },
    { L"Ｔ", 'T' },
    { L"Ｕ", 'U' },
    { L"Ｖ", 'V' },
    { L"Ｗ", 'W' },
    { L"Ｘ", 'X' },
    { L"Ｙ", 'Y' },
    { L"Ｚ", 'Z' },
    { L"０", '0' },
    { L"１", '1' },
    { L"２", '2' },
    { L"３", '3' },
    { L"４", '4' },
    { L"５", '5' },
    { L"６", '6' },
    { L"７", '7' },
    { L"８", '8' },
    { L"９", '9' },
    { L"＿", '_' },
    { L"A", 'A' },
    { L"B", 'B' },
    { L"C", 'C' },
    { L"D", 'D' },
    { L"E", 'E' },
    { L"F", 'F' },
    { L"G", 'G' },
    { L"H", 'H' },
    { L"I", 'I' },
    { L"J", 'J' },
    { L"K", 'K' },
    { L"L", 'L' },
    { L"M", 'M' },
    { L"N", 'N' },
    { L"O", 'O' },
    { L"P", 'P' },
    { L"Q", 'Q' },
    { L"R", 'R' },
    { L"S", 'S' },
    { L"T", 'T' },
    { L"U", 'U' },
    { L"V", 'V' },
    { L"W", 'W' },
    { L"X", 'X' },
    { L"Y", 'Y' },
    { L"Z", 'Z' },
    { L"0", '0' },
    { L"1", '1' },
    { L"2", '2' },
    { L"3", '3' },
    { L"4", '4' },
    { L"5", '5' },
    { L"6", '6' },
    { L"7", '7' },
    { L"8", '8' },
    { L"9", '9' },
    { L"_", '_' }

  };
#endif

  const gfl2::str::STRCODE* pStr = pStrBuf->GetPtr();
  while( *pStr != '\0' )
  {
    for( u32 i = 0; i < GFL_NELEMS( s_Table ); i++ )
    {
      u32 a = static_cast<u32>( *s_Table[i].w );
      u32 b = static_cast<u32>( *pStr );

      if( static_cast<u32>( *s_Table[i].w ) == static_cast<u32>( *pStr ) )
      {
        *pOut = s_Table[i].c;
        pOut++;
        break;
      }
    }
    pStr++;
  }

  *pOut = '\0';
}


b32 FieldDebugMenu::CreateFlagWorkItemForSearch( void *userWork, gfl2::debug::DebugWinItem *item, s32 index, gfl2::str::StrBuf* pStrBuf, size_t breakHeapFreeSize, s32* pHitCountUp )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  b32 isOver = false;

  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *heap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
      
    char test[ 512 ];
    ConvSearchWord( pStrBuf, &test[0] );
    FlagWorkSearchImpl* pFlagWorkImpl = pDebug->GetFlagWorkSearchImpl();

    for (u32 ii = 0; ii < pFlagWorkImpl->GetSize( index ); ++ii)
    {

      wchar_t     *workStr = gfl2::debug::DebugWin_GetWorkStr();
      const c8*   pLabelPtr = pFlagWorkImpl->GetLabelPtr(index, ii);

      if (::std::strstr( pFlagWorkImpl->GetLabelPtrPtrToText( pLabelPtr ), test ) != NULL)
      {
        if( pHitCountUp )
        {
          ( *pHitCountUp ) += 1;
        }


        if( pGropWork->GetItemFreeSize() <= 0 || heap->GetTotalFreeSize() <= breakHeapFreeSize )
        {
          isOver = true;

          // カウントアップしない場合は意味がないので打ち切り
          if( pHitCountUp == NULL )
          {
            return true;
          }
        }
        else
        {
          gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , L"item", (void*)pLabelPtr, FlagWorkSearchImpl::UpdateFlagWorkItem, FlagWorkSearchImpl::DrawFlagWorkItem );
        }
      }
    }
  }

  return isOver;
}

static void DestroyGroup_FlagworkCore( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *heap = pDebug->GetHeap();

    pGropWork->RemoveAllItem();
    pGropWork->ResizeItem( 1, heap );
  }

  pDebug->GetFlagWorkImpl()->DeleteLabel();
}


// フラグ・ワーク検索機能
static void CreateGroup_FlagWorkSearch( void *userWork, gfl2::debug::DebugWinItem *item )
{
  {
    FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
    gfl2::heap::HeapBase *heap = pDebug->GetHeap();

    // 全フラグ文字列をロード
    pDebug->GetFlagWorkSearchImpl()->SyncLoadLabel( pDebug->GetHeap() );
    
    // メモリ削減のため、開いた際に項目分のメモリ確保を行う
    gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
    pGropWork->RemoveAllItem();
    pGropWork->ResizeItem( pDebug->GetEventWorkTotalTableSize(), heap );
  }

  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  FieldDebugFlagWorkSearchProc::Param param;
  param.userWork = userWork;
  param.item = item;

  FieldDebugFlagWorkSearchProc* pProc = NIJI_PROC_CALL< GameSys::GameProcManager, FieldDebugFlagWorkSearchProc>( gameManager->GetProcManager() );
  pProc->SetParam( param );


  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->SetFoceStopFlag( true );

}


static void DestroyGroup_FlagworkSearch( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *heap = pDebug->GetHeap();
    
    pGropWork->RemoveAllItem();
    pGropWork->ResizeItem( 1, heap );
  }
  
  pDebug->GetFlagWorkSearchImpl()->DeleteLabel();
}


static void CreateGroup_FlagWorkEventFlag( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CreateGroup_FlagworkCore( userWork, item, GARC_ScriptDebugSymbol_EventFlag_BIN );
}
static void CreateGroup_FlagWorkSysFlag( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CreateGroup_FlagworkCore( userWork, item, GARC_ScriptDebugSymbol_SysFlag_BIN );
}
static void CreateGroup_FlagWorkVanishFlag( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CreateGroup_FlagworkCore( userWork, item, GARC_ScriptDebugSymbol_VanishFlag_BIN );
}
static void CreateGroup_FlagWorkItemFlag( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CreateGroup_FlagworkCore( userWork, item, GARC_ScriptDebugSymbol_ItemFlag_BIN );
}
static void CreateGroup_FlagWorkEtcFlag( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CreateGroup_FlagworkCore( userWork, item, GARC_ScriptDebugSymbol_EtcFlag_BIN );
}
static void CreateGroup_FlagWorkSceneWork( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CreateGroup_FlagworkCore( userWork, item, GARC_ScriptDebugSymbol_SceneWork_BIN );
}
static void CreateGroup_FlagWorkSysWork( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CreateGroup_FlagworkCore( userWork, item, GARC_ScriptDebugSymbol_SysWork_BIN );
}
static void CreateGroup_FlagWorkOtherWork( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CreateGroup_FlagworkCore( userWork, item, GARC_ScriptDebugSymbol_OtherWork_BIN );
}

// フラグワーク
static void CreateGroup_ScriptFlagworks( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *heap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    {
      gfl2::debug::DebugWinGroup *pChild = NULL;
      s32 ii;

      // pDebug->GetEventWorkTotalTableSize() momiji開始時に2400ぐらい
      pChild = gfl2::debug::DebugWin_AddGroup( heap , L"フラグ・ワーク検索", pGropWork, 1 );
      pChild->SetCreateDestroyGroupFunc( pDebug, CreateGroup_FlagWorkSearch, DestroyGroup_FlagworkSearch );

      ii = GARC_ScriptDebugSymbol_EventFlag_BIN;
      pChild = gfl2::debug::DebugWin_AddGroup( heap , pMsgData , msg_dmenu_flag_ctr_event, pGropWork, 1 );
      pChild->SetCreateDestroyGroupFunc( pDebug, CreateGroup_FlagWorkEventFlag, DestroyGroup_FlagworkCore );
      pDebug->SetFlagWorkPtr( pChild, ii );

      ii = GARC_ScriptDebugSymbol_SysFlag_BIN;
      pChild = gfl2::debug::DebugWin_AddGroup( heap , pMsgData , msg_dmenu_flag_ctr_system, pGropWork, 1 );
      pChild->SetCreateDestroyGroupFunc( pDebug, CreateGroup_FlagWorkSysFlag, DestroyGroup_FlagworkCore );
      pDebug->SetFlagWorkPtr( pChild, ii );
    
      ii = GARC_ScriptDebugSymbol_VanishFlag_BIN;
      pChild = gfl2::debug::DebugWin_AddGroup( heap , pMsgData , msg_dmenu_flag_ctr_vanish, pGropWork, 1 );
      pChild->SetCreateDestroyGroupFunc( pDebug, CreateGroup_FlagWorkVanishFlag, DestroyGroup_FlagworkCore );
      pDebug->SetFlagWorkPtr( pChild, ii );
    
      ii = GARC_ScriptDebugSymbol_ItemFlag_BIN;
      pChild = gfl2::debug::DebugWin_AddGroup( heap , pMsgData , msg_dmenu_flag_ctr_item, pGropWork, 1 );
      pChild->SetCreateDestroyGroupFunc( pDebug, CreateGroup_FlagWorkItemFlag, DestroyGroup_FlagworkCore );
      pDebug->SetFlagWorkPtr( pChild, ii );
    
      ii = GARC_ScriptDebugSymbol_EtcFlag_BIN;
      pChild = gfl2::debug::DebugWin_AddGroup( heap , pMsgData , msg_dmenu_flag_ctr_etc, pGropWork, 1 );
      pChild->SetCreateDestroyGroupFunc( pDebug, CreateGroup_FlagWorkEtcFlag, DestroyGroup_FlagworkCore );
      pDebug->SetFlagWorkPtr( pChild, ii );
    
      ii = GARC_ScriptDebugSymbol_SceneWork_BIN;
      pChild = gfl2::debug::DebugWin_AddGroup( heap , pMsgData , msg_dmenu_flag_ctr_scene_work, pGropWork, 1 );
      pChild->SetCreateDestroyGroupFunc( pDebug, CreateGroup_FlagWorkSceneWork, DestroyGroup_FlagworkCore );
      pDebug->SetFlagWorkPtr( pChild, ii );
    
      ii = GARC_ScriptDebugSymbol_SysWork_BIN;
      pChild = gfl2::debug::DebugWin_AddGroup( heap , pMsgData , msg_dmenu_flag_ctr_system_work, pGropWork, 1 );
      pChild->SetCreateDestroyGroupFunc( pDebug, CreateGroup_FlagWorkSysWork, DestroyGroup_FlagworkCore );
      pDebug->SetFlagWorkPtr( pChild, ii );
    
      ii = GARC_ScriptDebugSymbol_OtherWork_BIN;
      pChild = gfl2::debug::DebugWin_AddGroup( heap , pMsgData , msg_dmenu_flag_ctr_etc_work, pGropWork, 1 );
      pChild->SetCreateDestroyGroupFunc( pDebug, CreateGroup_FlagWorkOtherWork, DestroyGroup_FlagworkCore );
      pDebug->SetFlagWorkPtr( pChild, ii );

      // トレーナー対戦フラグ全リセット
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, heap, pDebug->GetMessageData(), msg_dmenu_eye_trainer_flag_set_all, reinterpret_cast<void*>(1), UpdateSetTrainerFlags, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, heap, pDebug->GetMessageData(), msg_dmenu_eye_trainer_flag_reset_all, reinterpret_cast<void*>(0), UpdateSetTrainerFlags, NULL );

      // 空を飛ぶ/到着フラグ全リセット
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, heap, pDebug->GetMessageData(), msg_dmenu_flag_sky_arrive_all_open, reinterpret_cast<void*>(1), UpdateSetSkyAndArriveFlags, NULL, 1 );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, heap, pDebug->GetMessageData(), msg_dmenu_flag_sky_arrive_all_close, reinterpret_cast<void*>(0), UpdateSetSkyAndArriveFlags, NULL, 0 );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, heap, pDebug->GetMessageData(), msg_dmenu_flag_sorawotobu_all_open, reinterpret_cast<void*>(3), UpdateSetSkyAndArriveFlags, NULL, 3 );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, heap, pDebug->GetMessageData(), msg_dmenu_flag_sorawotobu_all_close, reinterpret_cast<void*>(2), UpdateSetSkyAndArriveFlags, NULL, 2 );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, heap, pDebug->GetMessageData(), msg_dmenu_flag_arrive_all_open, reinterpret_cast<void*>(1), UpdateSetSkyAndArriveFlags, NULL, 5 );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, heap, pDebug->GetMessageData(), msg_dmenu_flag_arrive_all_close, reinterpret_cast<void*>(0), UpdateSetSkyAndArriveFlags, NULL, 4 );
    }
  }
}
static void DestroyGroup_ScriptFlagworks( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    for (u32 ii = 0; ii < GARC_ScriptDebugSymbol_DATA_NUM; ++ii )
    {
      pDebug->FreeFlagWorkPtr(ii);
    }
    pGropWork->RemoveAllItem();
  }
}

// 名前入力
void DebugWinFunc_Data_PlayerIDGet( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  ::Debug::DebugWin::DebugWinWork *work = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Savedata::MyStatus *pMystatus = pGameManager->GetGameData()->GetPlayerStatus();
    u32 id = pMystatus->GetID();
    work->mPlayerIDUpper = id >> 16;
    work->mPlayerIDLower = id & 0xFFFF;
  }
}

void DebugWinFunc_Data_PlayerIDSet( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  ::Debug::DebugWin::DebugWinWork *work = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Savedata::MyStatus *pMystatus = pGameManager->GetGameData()->GetPlayerStatus();
    u32 id = ( work->mPlayerIDLower ) + ( work->mPlayerIDUpper << 16 );
    pMystatus->SetID( id );
  }
}

void DebugWinFunc_Data_PlayerName( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Field::Debug::EventDebugStrInput *pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::Debug::EventDebugStrInput>( pGameManager->GetGameEventManager() );
    gfl2::debug::DebugWin_CloseWindow();
  }
}

void DebugWinFunc_Data_MoneyGet( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Savedata::Misc *pMisc = pGameManager->GetGameData()->GetMisc();
    Field::Debug::DebugTypes::s_DebugMyStatusParam.nGold = pMisc->GetGold();
  }
}

void DebugWinFunc_Data_MoneySet( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Savedata::Misc *pMisc = pGameManager->GetGameData()->GetMisc();
    pMisc->SetGold( Field::Debug::DebugTypes::s_DebugMyStatusParam.nGold );
  }
}

void DebugWinFunc_Data_BtlPtGet( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Savedata::Misc *pMisc = pGameManager->GetGameData()->GetMisc();
    Field::Debug::DebugTypes::s_DebugMyStatusParam.nBtlPt = pMisc->GetBP();
  }
}

void DebugWinFunc_Data_BtlPtSet( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Savedata::Misc *pMisc = pGameManager->GetGameData()->GetMisc();
    pMisc->SetBP( Field::Debug::DebugTypes::s_DebugMyStatusParam.nBtlPt );
  }
}

void DebugWinFunc_Data_GameTimeGet( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Savedata::GameTime *pGameTime = pGameManager->GetGameData()->GetGameTime();
    s64 sec = pGameTime->GetStartSec();
    gfl2::system::Date time;
    time.SetDateTimeToSecond( sec );
    Field::Debug::DebugTypes::s_DebugMyStatusParam.year   = time.GetYear();
    Field::Debug::DebugTypes::s_DebugMyStatusParam.month  = time.GetMonth();
    Field::Debug::DebugTypes::s_DebugMyStatusParam.day    = time.GetDay();
    Field::Debug::DebugTypes::s_DebugMyStatusParam.hour   = time.GetHour();
    Field::Debug::DebugTypes::s_DebugMyStatusParam.minute = time.GetMinute();
    Field::Debug::DebugTypes::s_DebugMyStatusParam.second = time.GetSecond();
  }
}

void DebugWinFunc_Data_GameTimeSet( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Savedata::GameTime *pGameTime = pGameManager->GetGameData()->GetGameTime();
    gfl2::system::Date time(
      Field::Debug::DebugTypes::s_DebugMyStatusParam.year,
      Field::Debug::DebugTypes::s_DebugMyStatusParam.month,
      Field::Debug::DebugTypes::s_DebugMyStatusParam.day,
      Field::Debug::DebugTypes::s_DebugMyStatusParam.hour,
      Field::Debug::DebugTypes::s_DebugMyStatusParam.minute,
      Field::Debug::DebugTypes::s_DebugMyStatusParam.second
      );
    s64 sec = time.GetDateTimeToSecond();
    pGameTime->SetStartSec( sec );
  }
}

void DebugWinFunc_Data_PlayTimeGet( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Savedata::PlayTime *pPlayTime = pGameManager->GetGameData()->GetPlayTime();
    Field::Debug::DebugTypes::s_DebugMyStatusParam.nHour = pPlayTime->GetPlayHour();
    Field::Debug::DebugTypes::s_DebugMyStatusParam.nMin = pPlayTime->GetPlayMinute();
    Field::Debug::DebugTypes::s_DebugMyStatusParam.nSec = pPlayTime->GetPlaySecond();
  }
}

void DebugWinFunc_Data_PlayTimeSet( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Savedata::PlayTime *pPlayTime = pGameManager->GetGameData()->GetPlayTime();
    pPlayTime->SetPlayTime( 
      Field::Debug::DebugTypes::s_DebugMyStatusParam.nHour,
      Field::Debug::DebugTypes::s_DebugMyStatusParam.nMin,
      Field::Debug::DebugTypes::s_DebugMyStatusParam.nSec
      );
  }
}

void DebugWinFunc_Data_MegaRing( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData* pGameData = pGameManager->GetGameData();
    Savedata::MyStatus* pMyStatus = pGameData->GetPlayerStatus();
    bool hasMegaRing =  pMyStatus->HasMegaRing();
    pMyStatus->SetMegaRing( !hasMegaRing );
  }
}

wchar_t* DebugWinFunc_Data_MegaRing_Draw( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  const GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const GameSys::GameData* pGameData = pGameManager->GetGameData();
  const Savedata::MyStatus* pMyStatus = pGameData->GetPlayerStatus();
  const bool hasMegaRing =  pMyStatus->HasMegaRing();

  ::Debug::DebugWin::DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
  gfl2::str::MsgData * pMsgData = debWork->GetMessageData();
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
  pMsgData->GetString( msg_dmenu_data_mega_ring, *workStrBuf );

  wchar_t* workStr = item->GetSysWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", workStrBuf->GetPtr(), ( hasMegaRing ? 1 : 0 ) );
  return workStr;
}

void DebugWinFunc_Data_ZenryokuRing( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData* pGameData = pGameManager->GetGameData();
    Savedata::MyStatus* pMyStatus = pGameData->GetPlayerStatus();
    bool hasZenryokuRing =  pMyStatus->HasZenryokuRing();
    pMyStatus->SetZenryokuRing( !hasZenryokuRing );
  }
}

wchar_t* DebugWinFunc_Data_ZenryokuRing_Draw( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  const GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const GameSys::GameData* pGameData = pGameManager->GetGameData();
  const Savedata::MyStatus* pMyStatus = pGameData->GetPlayerStatus();
  const bool hasZenryokuRing =  pMyStatus->HasZenryokuRing();

  ::Debug::DebugWin::DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
  gfl2::str::MsgData * pMsgData = debWork->GetMessageData();
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
  pMsgData->GetString( msg_dmenu_data_z_ring, *workStrBuf );

  wchar_t* workStr = item->GetSysWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", workStrBuf->GetPtr(), ( hasZenryokuRing ? 1 : 0 ) );
  return workStr;
}

//------------------------------------------------------------------------------
/**
 * @brief CharaViewer呼び出し
 */
//------------------------------------------------------------------------------
static void DebugWinFunc_CallCharaViewer( void * userWork, gfl2::debug::DebugWinItem * item )
{
  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();
  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    Field::Debug::EventDebugCharaViewer::Description desc;
    desc.isFieldControl = true;

    Field::Debug::EventDebugCharaViewer::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), desc );
    gfl2::debug::DebugWin_CloseWindow();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief CharaKisekae呼び出し
 */
//------------------------------------------------------------------------------
static u32 kisekaeContinueCnt = 0;
static void DebugWinFunc_CallCharaKisekae( void * userWork, gfl2::debug::DebugWinItem * item )
{
  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();
  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    Field::Debug::EventDebugKisekae::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager),kisekaeContinueCnt );
    gfl2::debug::DebugWin_CloseWindow();
  }
}

static void DebugWinFunc_CallCharaKisekaeCurrentPosition( void * userWork, gfl2::debug::DebugWinItem * item )
{
  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();
  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    Field::Debug::EventDebugKisekaeCurrentPosition::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager),kisekaeContinueCnt );
    gfl2::debug::DebugWin_CloseWindow();
  }
}

static void DebugWinFunc_CallCharaRideCurrentPosition( void * userWork, gfl2::debug::DebugWinItem * item )
{
  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();
  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    Field::Debug::EventDebugRidePokemon::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager),kisekaeContinueCnt );
    gfl2::debug::DebugWin_CloseWindow();
  }
}

// @note ヘッダファイルが煩雑になるので、メンバ変数はこちらに追加する
class FieldDebugMenu::Impl
{
public:
   
  Impl( void ) : 
    m_pDebugMenuTerrain( NULL )
    ,m_AttrDispEditor( NULL )
    ,m_pMsgDataAttrName( NULL )
    ,m_pCustomMapJumpControl( NULL )
    ,m_pFlagWorkImpl( NULL )
    ,m_pFlagWorkSearchImpl( NULL )
  {
  }

  virtual ~Impl( void )
  {
    GFL_SAFE_DELETE( m_pMsgDataAttrName );
    GFL_SAFE_DELETE( m_pDebugMenuTerrain );
    GFL_SAFE_DELETE( m_AttrDispEditor );
    GFL_SAFE_DELETE( m_pCustomMapJumpControl );
    GFL_SAFE_DELETE( m_pFlagWorkImpl );
    GFL_SAFE_DELETE( m_pFlagWorkSearchImpl );
  }

  Field::Debug::Menu::Terrain* m_pDebugMenuTerrain;
  Field::Debug::Menu::DebugMenuAttrDispEditor* m_AttrDispEditor;
  gfl2::str::MsgData* m_pMsgDataAttrName;


  Field::Debug::CustomMapJumpControl* m_pCustomMapJumpControl;
  FlagWorkImpl*                m_pFlagWorkImpl;
  FlagWorkSearchImpl*          m_pFlagWorkSearchImpl;
};

/**
* @brief コンストラクタ
* フィールドデバッグメニューの構築を実行します
*
* @param  pHeap         メニュー構築に使用するヒープ
* @param  pGameManager  ゲーム管理
*
* @return 無し
*/ 
FieldDebugMenu::FieldDebugMenu( gfl2::heap::HeapBase* pHeap, Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager,
                               Field::EnvManager* pEnvManager, Field::Effect::EffectManager *pEffectManager,
                               Field::FieldItemManager* pItemManager, Field::weather::WeatherControl *pWeatherControl, TrafficManager *pTrafficManager )
: m_pImpl( NULL )
, m_pHeap( pHeap )
, m_pFieldRootGroup( NULL )
, m_pScriptGroup( NULL )
, m_pFlagWorks( NULL )
, m_pTimeGroup( NULL )
, m_pItemItem( NULL )
, m_pPokemonGroup( NULL )
, m_pRotom( NULL )
, m_pFieldMoveModelManager( pFieldMoveModelManager )
, m_pEnvManager( pEnvManager )
, m_pCameraGroup( NULL )
, m_pTraffic( NULL )
, m_pEncount( NULL )
, m_pGimmickEncount( NULL )
, m_pDispInfo( NULL )
, m_pWeather( NULL )
, m_pFishing(NULL)
, m_pSodateya(NULL)
, m_pBattleInst( NULL)
, m_pRotomPower( NULL )
#if defined(GF_ENABLE_DEBUG_EDITOR)
, m_pCharaLayoutEditor( NULL )
, m_pEventCameraEditor( NULL )
#endif // GF_PLATFORM_WIN32
, m_pStaticModel( NULL )
, m_fDepthLevel( 30.0f )
, m_fFactor( 0.15f )
, m_isUseCullingForMoveModel(1)
, m_isUseCullingForStaticModel(1)
, m_isUsePiyoriMaxValue(0)
, m_piyoriMaxValue( 60 )
, m_piyoriAngle( 5 )
, m_isUseJumpAngle(0)
, m_jumpAngle(15)
, m_isUseRunTurnSpeed(0)
, m_isUseSlopeDampingRate(0)
, m_slopeDampingRate(2.75f)
, m_isUseOttotto(false)
, m_isUseKairikyDebugValue(false)
, m_kairikyAngle(89.0f)
, m_kairikyDistance(145.0f)
, m_kairikyInterpFrame(6)
, m_waitSlopeFrame(4)
, m_runTurnSpeed( 180 )
, m_wallStopAngle(23.0f)
, m_rideOffAngle(45.0f)
, m_isUseTranierEyeParam(false)
, m_trainerEyeCylinderOffsetY(0.0f)
, m_trainerEyeCylinderHeight(150.0f)
, m_playerSpeedRateForWalk(1.0f)
, m_playerSpeedRateForRun(1.0f)
, m_kentarosSpeedRateForWalk(1.0f)
, m_kentarosSpeedRateForRun(1.0f)
, m_samehaderSpeedRateForWalk(1.0f)
, m_samehaderSpeedRateForRun(1.0f)
, m_muurandSpeedRateForWalk(1.0f)
, m_muurandSpeedRateForRun(1.0f)
, m_roba2SpeedRateForWalk(1.0f)
, m_roba2SpeedRateForRun(1.0f)
, m_kairikySpeedRateForWalk(1.0f)
, m_kairikySpeedRateForRun(1.0f)
, m_laplaceSpeedRateForWalk(1.0f)
, m_laplaceSpeedRateForRun(1.0f)
, m_isUsePlayerTurnRate(false)
, m_playerTurnRateForDefault(100)
, m_playerTurnRateForRun(60)
, m_kentarosTurnRateForDefault(40)
, m_kentarosTurnRateForRun(15)
, m_samehaderTurnRateForDefault(40)
, m_samehaderTurnRateForRun(10)
, m_muurandTurnRateForDefault(40)
, m_muurandTurnRateForRun(45)
, m_roba2TurnRateForDefault(40)
, m_roba2TurnRateForRun(30)
, m_kairikyTurnRateForDefault(40)
, m_kairikyTurnRateForRun(40)
, m_laplaceTurnRateForDefault(40)
, m_laplaceTurnRateForRun(15)
, m_playerStartTurnRate(60)
, m_playerStartTurnFrame(4)
, m_kentarosStartTurnRate(60)
, m_kentarosStartTurnFrame(4)
, m_samehaderStartTurnRate(30)
, m_samehaderStartTurnFrame(10)
, m_muurandStartTurnRate(60)
, m_muurandStartTurnFrame(4)
, m_roba2StartTurnRate(60)
, m_roba2StartTurnFrame(4)
, m_kairikyStartTurnRate(60)
, m_kairikyStartTurnFrame(4)
, m_laplaceStartTurnRate(30)
, m_laplaceStartTurnFrame(10)
, m_kentarosSuddenStopRate(0.5f)
, m_samehaderSuddenStopRate(1.0f)
, m_muurandSuddenStopRate(0.5f)
, m_roba2SuddenStopRate(0.5f)
, m_kairikySuddenStopRate(0.8f)
, m_laplaceSuddenStopRate(1.0f)
, m_tsumadukiCnt(8)
, m_tsumadukiAcceptFrame(15)
, m_pItemManager( pItemManager )
, m_diaryRomCode(0)   /**< 生成データのロムバージョン*/
, m_diaryType(0)      /**< ダイアリー種別*/
, m_diaryId(0)        /**< ダイアリーID（種別内分類）*/
, m_drawTagNumForAllScene(0)
, m_drawTagNumForTerrainHighModel(0)
, m_drawTagNumForTerrainMiddleModel(0)
, m_drawTagNumForTerrainDistantViewModel(0)
, m_enableAdjustGround(1)
, m_partySaveSlot(0)
, m_boxSaveSlot(0)
{
  for( u32 i = 0; i < GFL_NELEMS(m_drawTagNumTableForTerrainHighModel); i++ )
  {
    m_drawTagNumTableForTerrainHighModel[i].drawTagNum = 0;
    m_drawTagNumTableForTerrainHighModel[i].x = 0;
    m_drawTagNumTableForTerrainHighModel[i].z = 0;
  }

  ::std::memset( m_diaryData , 0 , sizeof(m_diaryData) );

  m_pImpl = GFL_NEW( m_pHeap ) FieldDebugMenu::Impl();

  // メッセージデータ取得
  m_pMsgData = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork)->GetMessageData();

  // アトリビュートName取得
  m_pImpl->m_pMsgDataAttrName = GFL_NEW( m_pHeap ) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_debug_field_attr_name_DAT, m_pHeap, gfl2::str::MsgData::LOAD_PART/*LOAD_FULL*/ );

  // 時間操作関連.
  {
    m_pUserWorkTimeControlArray = GFL_NEW_ARRAY( m_pHeap ) USER_WORK[ GFL_NELEMS(sc_TIMECONTROL) ];
  }

  m_pUserWorkPokeDiaryCount = GFL_NEW_ARRAY( m_pHeap ) USER_WORK[ Savedata::PokeDiarySave::DIARYTYPE_COUNT_MAX ];

  // マップジャンプをTopに追加
  {
    Fieldmap* pfieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
    m_pMapJump = GFL_NEW(m_pHeap) DebugMapJump();
    m_pMapJump->Initialize( m_pHeap, GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetAsyncFileManager(), pfieldmap->GetZoneID(), pfieldmap, m_pMsgData );  // @todo この時点でロードしてしまっている
    m_pMapJump->CreateDebugMenuGroup( gfl2::debug::DebugWin_GetTopGroup() );
  }

  // カスタムマップジャンプをTopに追加
#if ( GFL_HIO_ENABLE || defined(GF_PLATFORM_WIN32) )
  {
    m_pImpl->m_pCustomMapJumpControl = GFL_NEW( m_pHeap ) Field::Debug::CustomMapJumpControl();
    m_pImpl->m_pCustomMapJumpControl->CreateItem( gfl2::debug::DebugWin_GetTopGroup(), m_pHeap, m_pHeap );
  }
#endif

  // カメラ関連をTopに追加
  m_pCameraGroup = GFL_NEW( m_pHeap ) DebugMenuCameraGroup( m_pHeap, gfl2::debug::DebugWin_GetTopGroup(), m_pMsgData );

  // FieldグループをTopに追加
  m_pFieldRootGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_top, NULL , 64 );

  // Script関連をTopに追加
  m_pImpl->m_pFlagWorkImpl = GFL_NEW( m_pHeap ) FlagWorkImpl();
  m_pImpl->m_pFlagWorkSearchImpl = GFL_NEW( m_pHeap ) FlagWorkSearchImpl();
  DebugTestMenu_Script( m_pHeap );

#if 1
  // ポケモンをTopに追加
  {
    gfl2::debug::DebugWinGroup *pokemonGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_pokemon_top, NULL, 64 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pokemonGroup , m_pHeap, m_pMsgData, msg_dmenu_pokemon_add , NULL , DebugWin_DebugPokemonAdd , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_edit, NULL, UpdateDebugPokemonEdit, DrawDebugPokemonEdit );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_del, NULL, UpdateDebugPokemonDel, DrawDebugPokemonDel );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_full, NULL, UpdateDebugPokemonBoxFull, DrawDebugPokemonBoxFull );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_random, NULL, UpdateDebugPokemonBoxRandom, DrawDebugPokemonBoxRandom );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_team1, NULL, UpdateDebugPokemonBattleTeam, DrawDebugPokemonBattleTeam );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_team2, NULL, UpdateDebugPokemonBattleTeamNg, DrawDebugPokemonBattleTeamNg );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_make_ng_egg, NULL, UpdateDebugPokemonSetDameTamago, NULL );
    m_pPokemonGroup = pokemonGroup;
  }

  //  リボン用
  {
    u32 dat = GARC_message_ribbon_DAT;
    m_pRibbonMsgData  = GFL_NEW( m_pHeap ) gfl2::str::MsgData( print::GetMessageArcId(), dat, m_pHeap, gfl2::str::MsgData::LOAD_FULL );

    m_pUserWorkRibbonArray = GFL_NEW_ARRAY( m_pHeap ) USER_WORK[ PokeTool::RIBBON_NO_MAX ];

    m_pRibbonGrop = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_ribbon_rewrite, m_pPokemonGroup, PokeTool::RIBBON_NO_MAX );
    // リボンリスト表示
    for( u32 i = 0; i < PokeTool::RIBBON_NO_MAX; ++i )
    {
      m_pUserWorkRibbonArray[i].pThis   = this;
      m_pUserWorkRibbonArray[i].labelId = i;

      gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRibbonGrop, m_pHeap, m_pRibbonMsgData, i, &m_pUserWorkRibbonArray[i], DebugWinFunc_RibbonList, DebugWinFunc_RibbonListDraw );
    }
  }

  { // バトルチーム：タマゴ関連
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_team_egg, NULL, UpdateDebugPokemonSetBattleTeamTamago, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_team_ngegg, NULL, UpdateDebugPokemonSetBattleTeamDameTamago, NULL );
  }

  // フォームチェック用 BOX Fill
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_full_form_1, NULL, UpdateDebugPokemonBoxFull_All_Form_Still_DP, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_full_form_2, NULL, UpdateDebugPokemonBoxFull_All_Form_After_BW, NULL );
  }

  // 手持ち Save & Load
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_save_party, &m_partySaveSlot, UpdateDebugPokemon_PartySave, DrawDebugPokemon_PartySaveLoad );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_load_party, &m_partySaveSlot, UpdateDebugPokemon_PartyLoad, DrawDebugPokemon_PartySaveLoad );
  }

  // Box Save & Load
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_save_box, &m_boxSaveSlot, UpdateDebugPokemon_BoxSave, DrawDebugPokemon_BoxSaveLoad );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_pokemon_load_box, &m_boxSaveSlot, UpdateDebugPokemon_BoxLoad, DrawDebugPokemon_BoxSaveLoad );
  }
  
  // バイナリ表示
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_18, &PokemonPartyIndex, UpdateDebugPokemonDumpPokemonPrintBinary, DrawDebugPokemon_PrintPokemonNameInParty );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_29, &PokemonPartyIndex, UpdateDebugPokemonDumpPokemonOutputBinary, DrawDebugPokemon_PrintPokemonNameInParty );
    m_pPokemonMakeIndexGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_32, m_pPokemonGroup );
    
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_43, &m_pokeIndexMake_pokemonIndexInParty, UpdatePokeIndexMake_SetPokemon, DrawPokeIndexMake_SetPokemon );
    //gfl2::debug::DebugWin_AddItemValueU8  ( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_33, &m_pokeIndexMake_levelBand, 0, 255);
    gfl2::debug::DebugWin_AddItemValueU16  ( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_34, &m_pokeIndexMake_requestKind, 0, 65535);
    gfl2::debug::DebugWin_AddItemValueU8  ( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_35, &m_pokeIndexMake_requestGender, 0, 255);
    gfl2::debug::DebugWin_AddItemValueU8  ( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_36, &m_pokeIndexMake_requestLevelBand, 0, 255);
    gfl2::debug::DebugWin_AddItemValueU8  ( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_37, &m_pokeIndexMake_requestNoLegend, 0, 255);
    gfl2::debug::DebugWin_AddItemValueU8  ( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_38, &m_pokeIndexMake_searchDisable, 0, 255);
    gfl2::debug::DebugWin_AddItemValueU64  ( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_39, &m_pokeIndexMake_uniqId, 0, 18446744073709551615 );
    gfl2::debug::DebugWin_AddItemValueU64  ( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_40, &m_pokeIndexMake_dataStoreId, 0, 18446744073709551615);
    gfl2::debug::DebugWin_AddItemValueU32  ( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_41, &m_pokeIndexMake_principalId, 0, 4294967295);
    gfl2::debug::DebugWin_AddItemValueU16  ( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_42, &m_pokeIndexMake_messageId, 0, 65535);
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_45, &m_pokeIndexMake_method, UpdatePokeIndexMake_Method, DrawPokeIndexMake_Method );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonMakeIndexGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_44, this, UpdatePokeIndexMake_Generate, NULL );
  }

  //不正チェックサーバーデバッグ用
  {
    m_pPokemonValidateGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_30, m_pPokemonGroup, 64 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_00, NULL, UpdateDebugPokemonSetIllegalMega, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_01, NULL, UpdateDebugPokemonSetIllegalMaxhp, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_03, NULL, UpdateDebugPokemonSetIllegalAtkValue, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_02, NULL, UpdateDebugPokemonSetIllegalLevel, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_04, NULL, UpdateDebugPokemonSetEmptyNickName, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_05, NULL, UpdateDebugPokemonSetEmptyOyaName, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_06, NULL, UpdateDebugPokemonSetIllegalNickNameChar, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_07, NULL, UpdateDebugPokemonSetIllegalOyaNameChar, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_08, NULL, UpdateDebugPokemonSetNoEomNickName, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_09, NULL, UpdateDebugPokemonSetNoEomOyaName, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_24, NULL, UpdateDebugPokemonSet13CharNickName, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_23, NULL, UpdateDebugPokemonSet13CharOyaName, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_10, NULL, UpdateDebugPokemonSetIllegalBasePoint, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_11, NULL, UpdateDebugPokemonSetIllegalWazaNo, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_12, NULL, UpdateDebugPokemonSetIllegalSeikakuNo, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_13, NULL, UpdateDebugPokemonSetForceMale, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_14, NULL, UpdateDebugPokemonSetForceFemale, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_15, NULL, UpdateDebugPokemonSetForceNeutral, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_16, NULL, UpdateDebugPokemonSetIllegalBallID, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_17, NULL, UpdateDebugPokemonSetIllegalMonsNo, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_19, NULL, UpdateDebugPokemonSetIllegalExp, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_20, NULL, UpdateDebugPokemonSetIllegalItem, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_46, NULL, UpdateDebugPokemonSetNoEomPastOyaName, NULL );
    //gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_21, NULL, UpdateDebugPokemonSetIllegalTakenPlace, NULL );
    //gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_22, NULL, UpdateDebugPokemonSetIllegalContactPlace, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_25, NULL, UpdateDebugMyStatusSetEmptyName, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_26, NULL, UpdateDebugMyStatusSetilligalName, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_27, NULL, UpdateDebugMyStatusSetNoEomName, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_28, NULL, UpdateDebugMyStatusSet13CharName, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pPokemonValidateGroup, m_pHeap, m_pMsgData, msg_dmenu_illegal_poke_31, NULL, UpdateDebugPokemon_SetIllegalFormToPokemon, DrawDebugPokemon_SetIllegalFormToPokemon );
  }

  // アイテムをTopに追加
  { 
    m_pItemItem = gfl2::debug::DebugWin_AddItemUpdateFunc( NULL, m_pHeap, m_pMsgData, msg_dmenu_item_make, NULL, DebugWin_DebugItemMake, NULL );
  }

  // ロトムをTopに追加
  { 
    m_pRotom = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_app_rotom, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_event_flag_00, NULL, UpdateDebugRotomEventFlag00, DrawDebugRotomEventFlag00 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_event_flag_01, NULL, UpdateDebugRotomEventFlag01, DrawDebugRotomEventFlag01 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_event_flag_02, NULL, UpdateDebugRotomEventFlag02, DrawDebugRotomEventFlag02 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_friendship_flag, NULL, UpdateDebugRotomFriendshipUpFlag, DrawDebugRotomFriendshipUpFlag );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_friendship, NULL, UpdateDebugRotomFriendship, DrawDebugRotomFriendship );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_chara_change, NULL, UpdateDebugRotomCharaChange, DrawDebugRotomCharaChange );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_chara_00, NULL, UpdateDebugRotomChara00, DrawDebugRotomChara00 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_chara_01, NULL, UpdateDebugRotomChara01, DrawDebugRotomChara01 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_chara_02, NULL, UpdateDebugRotomChara02, DrawDebugRotomChara02 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_rotopon, NULL, UpdateDebugRotomRotopon, DrawDebugRotomRotopon );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_anime_loop, NULL, UpdateDebugRotomAnimeLoop, DrawDebugRotomAnimeLoop );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_report, NULL, UpdateDebugRotomReportFlag, DrawDebugRotomReportFlag );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_request, NULL, UpdateDebugRotomRequest, DrawDebugRotomRequest );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_free_talk, NULL, UpdateDebugRotomFreeTalkID, DrawDebugRotomFreeTalkID );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pRotom, m_pHeap, m_pMsgData, msg_dmenu_app_rotom_gameclear_msg, NULL, UpdateDebugRotomGameClearMsgID, DrawDebugRotomGameClearMsgID );
  }

#endif
#if defined(GF_ENABLE_DEBUG_EDITOR)
  // キャラレイアウトエディター
  {
    Fieldmap* pFieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
    Field::FieldScript::FieldScriptSystem* pScriptSystem = Field::FieldScript::FieldScriptSystem::GetInstance();
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager);
    gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetAsyncFileManager();
    m_pCharaLayoutEditor = GFL_NEW(m_pHeap) Field::Debug::DebugCharaLayoutEditor();
    m_pCharaLayoutEditor->Initialize( m_pHeap, m_pMsgData, pFieldmap, pScriptSystem, pGameManager, pAsyncFileManager );
    //m_pCharaLayoutEditor->CreateDebugMenu( gfl2::debug::DebugWin_GetTopGroup());
  }

  // イベントカメラエディター
  {
    Fieldmap* pFieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
    Field::FieldScript::FieldScriptSystem* pScriptSystem = Field::FieldScript::FieldScriptSystem::GetInstance();
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager);
    m_pEventCameraEditor = GFL_NEW(m_pHeap) Field::Debug::DebugEventCameraEditor();
    m_pEventCameraEditor->Initialize( m_pHeap, m_pMsgData, pFieldmap, pScriptSystem, pGameManager );
    //m_pEventCameraEditor->CreateDebugMenu( gfl2::debug::DebugWin_GetTopGroup());
  }
#endif // GF_PLATFORM_WIN32




  // 情報表示汎用
  {
    Fieldmap* pFieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
    gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetAsyncFileManager();
    ZoneDataLoader* pZoneDataLoader = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldZoneDataLoader();
    m_pDispInfo = GFL_NEW(m_pHeap) DebugDispInfo();
    m_pDispInfo->Initialize( m_pHeap, pAsyncFileManager, pFieldmap, pZoneDataLoader, m_pMsgData );
    m_pDispInfo->CreateDebugMenu(m_pFieldRootGroup);
  }

  // パフォーマンスチェック(CPU)
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_title , m_pFieldRootGroup );
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_17 , &m_enableAdjustGround               , 0, 1, 1);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_00 , &m_performanceForCpuDraw            , 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_01 , &m_performanceForTraverse           , 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_02 , &m_performanceForAllScene           , 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_03 , &m_performanceForMoveModelScene     , 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_04 , &m_performanceForStaticModelScene   , 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_05 , &m_performanceForAdjustGround       , 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_06 , &m_performanceForTerrainScene       , 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_07 , &m_performanceForNaminoriScene      , 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_08 , &m_performanceForRideScene          , 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_09 , &m_performanceForRoba2Scene         , 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_10 , &m_performanceForZone               , 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_11 , &m_performanceForMoveModelPrevUpdate, 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_12 , &m_performanceForMoveModelPostUpdate, 0, 0, 1000000);

    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_13 , &m_drawTagNumForAllScene, 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_14 , &m_drawTagNumForTerrainHighModel, 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, m_pHeap, L"", &m_drawTagNumTableForTerrainHighModel[0], NULL, Draw_DrawTagNumTableForTerrainHighModel );
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_15 , &m_drawTagNumForTerrainMiddleModel, 0, 0, 1000000);
    gfl2::debug::DebugWin_AddItemValueInt  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_performance_check_16 , &m_drawTagNumForTerrainDistantViewModel, 0, 0, 1000000);
  }

  // 制御フラグ関連
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_dbgflg_top, m_pFieldRootGroup, DebugTypes::Switch::MAX ); 
    pGropWork->SetCreateDestroyGroupFunc( this, CreateGroup_Flagwork, DestroyGroup );
  }

  // アトリビュートチェック
  {
    m_pImpl->m_AttrDispEditor = GFL_NEW( m_pHeap ) Field::Debug::Menu::DebugMenuAttrDispEditor();
    
    Fieldmap* pFieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager);
    gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetAsyncFileManager();
    m_pImpl->m_AttrDispEditor->Initialize( m_pHeap, m_pMsgData, m_pImpl->m_pMsgDataAttrName, pFieldmap, pGameManager, pAsyncFileManager );

    m_pImpl->m_AttrDispEditor->CreateDebugMenu( m_pFieldRootGroup );
  }

  // 時間操作関連.
  {
    m_pTimeGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap , m_pMsgData, msg_dmenu_field_time_ctrl_top, m_pFieldRootGroup );
    m_pTimeGroup->SetCreateDestroyGroupFunc( this, CreateGroup_Time, DestroyGroup );
  }

  //天候
  {
     m_pWeather = GFL_NEW(m_pHeap) DebugWeather();
     m_pWeather->Initialize( m_pHeap, pWeatherControl, m_pMsgData );
     m_pWeather->CreateDebugMenu( m_pFieldRootGroup );
  }


  // キャラ作成に関するグループを追加
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_movemodel_top, m_pFieldRootGroup );
    pGropWork->SetCreateDestroyGroupFunc( this, CreateGroup_CreateMoveModel, DestroyGroup );
  }

  // プレイヤーデバッググループを追加
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_player_dbg_top, m_pFieldRootGroup );
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_field_player_dbg_use_ac_assert , &DebugTypes::m_isUseActionCommandCancelAssert, 0,     1,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, L"配置モデルをカリングするかどうか"                  , &m_isUseCullingForStaticModel,    0,      1,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, L"動作モデルをカリングするかどうか"                  , &m_isUseCullingForMoveModel  ,    0,      1,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_field_player_dbg_use_piyo      , &m_isUsePiyoriMaxValue       ,    0,      1,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_field_player_dbg_piyo          , &m_piyoriMaxValue            ,    0,    256,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_field_player_dbg_piyo_angle    , &m_piyoriAngle               ,    0,     90,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, L"段差ジャンプのデバッグ進入角を利用するか"          , &m_isUseJumpAngle            ,    0,      1,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, L"段差ジャンプのデバッグ進入角"                      , &m_jumpAngle                 ,    0,     45,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_field_player_dbg_use_turn_spd  , &m_isUseRunTurnSpeed         ,    0,      1,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_field_player_dbg_turn_spd      , &m_runTurnSpeed              ,    0,    300,    10);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_field_player_dbg_use_slope_rate, &m_isUseSlopeDampingRate     ,    0,      1,     1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_field_player_dbg_slope_rate    , &m_slopeDampingRate          ,    0,  10.0f, 0.25f);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_field_player_dbg_use_ott       , &m_isUseOttotto              ,    0,      1,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, L"カイリキのデバッグ値を利用するか"                  , &m_isUseKairikyDebugValue    ,    0,      1,     1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"カイリキを発動する進入角"                          , &m_kairikyAngle              ,    0,     90,  1.0f);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"カイリキを発動する距離"                            , &m_kairikyDistance           ,    0,    300,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, L"カイリキの吸着にかけるフレーム"                    , &m_kairikyInterpFrame        ,    0,     60,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, L"坂道モーションにかけるフレーム"                    , &m_waitSlopeFrame            ,    0,     30,     1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"壁に当たるとストップする入射角"                    , &m_wallStopAngle             ,    0,     45,     1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"海ライドから降りる入射角"                          , &m_rideOffAngle              ,    0,     90,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, L"視線と当たる円柱を編集するか"                      , &m_isUseTranierEyeParam      ,    0,      1,     1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"視線と当たる円柱のYオフセット"                     , &m_trainerEyeCylinderOffsetY , -500,    500,     1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"視線と当たる円柱の高さ"                            , &m_trainerEyeCylinderHeight  ,    0,   1000,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, L"躓きに移行する連打回数"                            , &m_tsumadukiCnt              ,    0,    127,     1);
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, L"連打と見做す間隔(フレーム数)"                      , &m_tsumadukiAcceptFrame      ,    0,    127,     1);
  }

  // ポケモンライド
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pFieldRootGroup, m_pHeap, m_pMsgData, msg_dmenu_field_pokeride, pGameManager, DebugPokemonRide, NULL );
  }

  // プレイヤーの速度調整
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"速度調整" , m_pFieldRootGroup );
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"プレイヤーのデバッグ速度レート(歩き)"              , &m_playerSpeedRateForWalk    ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"プレイヤーのデバッグ速度レート(走り)"              , &m_playerSpeedRateForRun     ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"ケンタロスのデバッグ速度レート(歩き)"              , &m_kentarosSpeedRateForWalk  ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"ケンタロスのデバッグ速度レート(走り)"              , &m_kentarosSpeedRateForRun   ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"サメハダーのデバッグ速度レート(歩き)"              , &m_samehaderSpeedRateForWalk ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"サメハダーのデバッグ速度レート(走り)"              , &m_samehaderSpeedRateForRun  ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"ムーランドのデバッグ速度レート(歩き)"              , &m_muurandSpeedRateForWalk   ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"ムーランドのデバッグ速度レート(走り)"              , &m_muurandSpeedRateForRun    ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"バンバドロのデバッグ速度レート(歩き)"              , &m_roba2SpeedRateForWalk     ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"バンバドロのデバッグ速度レート(走り)"              , &m_roba2SpeedRateForRun      ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"カイリキーのデバッグ速度レート(歩き)"              , &m_kairikySpeedRateForWalk   ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"カイリキーのデバッグ速度レート(走り)"              , &m_kairikySpeedRateForRun    ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"ラプラス　のデバッグ速度レート(歩き)"              , &m_laplaceSpeedRateForWalk   ,    0,      2,   0.1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"ラプラス　のデバッグ速度レート(走り)"              , &m_laplaceSpeedRateForRun    ,    0,      2,   0.1);
  }

  // プレイヤーの急停止調整
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_sudden_stop_rate_title , m_pFieldRootGroup );
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_sudden_stop_rate_00 , &m_kentarosSuddenStopRate  , 0.0f, 1.0f, 0.1f);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_sudden_stop_rate_01 , &m_samehaderSuddenStopRate , 0.0f, 1.0f, 0.1f);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_sudden_stop_rate_02 , &m_muurandSuddenStopRate   , 0.0f, 1.0f, 0.1f);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_sudden_stop_rate_03 , &m_roba2SuddenStopRate     , 0.0f, 1.0f, 0.1f);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_sudden_stop_rate_04 , &m_kairikySuddenStopRate   , 0.0f, 1.0f, 0.1f);
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_sudden_stop_rate_05 , &m_laplaceSuddenStopRate   , 0.0f, 1.0f, 0.1f);
  }

  // プレイヤーの旋回調整
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_title , m_pFieldRootGroup );
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_00 , &m_isUsePlayerTurnRate         ,    0,       1,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_01 , &m_playerTurnRateForDefault    ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_02 , &m_playerTurnRateForRun        ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_03 , &m_kentarosTurnRateForDefault  ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_04 , &m_kentarosTurnRateForRun      ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_05 , &m_samehaderTurnRateForDefault ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_06 , &m_samehaderTurnRateForRun     ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_07 , &m_muurandTurnRateForDefault   ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_08 , &m_muurandTurnRateForRun       ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_09 , &m_roba2TurnRateForDefault     ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_10 , &m_roba2TurnRateForRun         ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_11 , &m_kairikyTurnRateForDefault   ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_12 , &m_kairikyTurnRateForRun       ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_13 , &m_laplaceTurnRateForDefault   ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_turn_rate_14 , &m_laplaceTurnRateForRun       ,    0,     100,   1);
  }

  // プレイヤーの旋回始め調整
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_title , m_pFieldRootGroup );
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_00 , &m_playerStartTurnRate     ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_01 , &m_playerStartTurnFrame    ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_02 , &m_kentarosStartTurnRate   ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_03 , &m_kentarosStartTurnFrame  ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_04 , &m_samehaderStartTurnRate  ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_05 , &m_samehaderStartTurnFrame ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_06 , &m_muurandStartTurnRate    ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_07 , &m_muurandStartTurnFrame   ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_08 , &m_roba2StartTurnRate      ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_09 , &m_roba2StartTurnFrame     ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_10 , &m_kairikyStartTurnRate    ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_11 , &m_kairikyStartTurnFrame   ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_12 , &m_laplaceStartTurnRate    ,    0,     100,   1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_data_start_turn_rate_13 , &m_laplaceStartTurnFrame   ,    0,     100,   1);
  }

  // 地形関連
  {
    m_pImpl->m_pDebugMenuTerrain = GFL_NEW( m_pHeap ) Field::Debug::Menu::Terrain( m_pHeap, m_pFieldRootGroup, m_pMsgData );
  }
  
  // 配置モデル
  {
    Fieldmap* pFieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
    MyRenderingPipeLine* pRenderingPipeLine = pFieldmap->GetFieldRenderingPipeLineManager()->GetMyRenderingPipeLine();
    gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetAsyncFileManager();
    m_pStaticModel = GFL_NEW(m_pHeap) DebugStaticModel();
    m_pStaticModel->Initialize( m_pHeap, pAsyncFileManager, pFieldmap, pRenderingPipeLine, m_pMsgData );
    m_pStaticModel->CreateDebugMenu(m_pFieldRootGroup);
  }

  // 配置アイテム生成に関するグループを追加
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_item_top, m_pFieldRootGroup );  
    pGropWork->SetCreateDestroyGroupFunc( this, CreateGroup_Item, DestroyGroup );
  }

  // フォグ制御
  {
    InitDebugFogParam( m_pEnvManager );
    gfl2::debug::DebugWinGroup*               pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_fog_top, m_pFieldRootGroup  );
    pGropWork->SetCreateDestroyGroupFunc( this, CreateGroup_setupFog, DestroyGroup );
  }

  // ブルーム制御
  {
    InitDebugGlareParam( m_pEnvManager );
    gfl2::debug::DebugWinGroup*               pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_glare_top, m_pFieldRootGroup  );
    pGropWork->SetCreateDestroyGroupFunc( this, CreateGroup_setupGlare, DestroyGroup );
  }

  // 立体視調整
  {
    gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_stereo_top, m_pFieldRootGroup );
    pGroup->SetCreateDestroyGroupFunc( this, CreateGroup_Display, DestroyGroup );
  }

  // PosJump
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_posjump_top, m_pFieldRootGroup );
    pGropWork->SetCreateDestroyGroupFunc( this, CreateGroup_PosJump, DestroyGroup );
  }

  // 歩数制御
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_step_group, m_pFieldRootGroup );
    pGropWork->SetCreateDestroyGroupFunc( this, CreateGroup_CreateStep, DestroyGroup );
  }


  // イベント起動
  {
    gfl2::debug::DebugWinGroup *pGroupWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_event_top, m_pFieldRootGroup );
    pGroupWork->SetCreateDestroyGroupFunc( this, CreateGroup_StartEvent, DestroyGroup );
  }
  

  // スクリプト?
  {
    Field::FieldScript::FieldScriptSystem   *pSys = Field::FieldScript::FieldScriptSystem::GetInstance();
    gfl2::debug::DebugWin_AddItemValueU32( m_pFieldRootGroup, m_pHeap, m_pMsgData, msg_dmenu_field_message_id, pSys->GetIsDebugMsgLabelPtr(), 0, 1, 1);
  }

  // EnvSound
  {
    gfl2::debug::DebugWinGroup *pGroupWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_env_sound_top, m_pFieldRootGroup );
    pGroupWork->SetCreateDestroyGroupFunc( this, CreateGroup_EnvSound, DestroyGroup );
  }

  // シグナル
  // @todo 現行のターゲットクラスが自機専用になったので一時的にコメントアウト、2月修正予定
#if 0
  {
    m_pSignal = GFL_NEW( m_pHeap ) DebugSignal();
    m_pSignal->Initialize( m_pHeap, m_pMsgData );
    m_pSignal->CreateDebugMenu( m_pFieldRootGroup );
  }
#endif

  // フェード
  {
    m_pFade = GFL_NEW( m_pHeap ) Field::Debug::DebugFade();
    m_pFade->Initialize( m_pHeap, m_pMsgData );
    m_pFade->CreateDebugMenu( m_pFieldRootGroup );
  }

  // エンカウント
  {
    m_pEncount = GFL_NEW( m_pHeap ) DebugEncount();
    m_pEncount->Initialize( m_pHeap, m_pMsgData );
    m_pEncount->CreateDebugMenu( m_pFieldRootGroup );
  }

  // ギミックエンカウント
  {
    m_pGimmickEncount = GFL_NEW( m_pHeap ) DebugGimmickEncount();
    m_pGimmickEncount->Initialize( m_pHeap, m_pMsgData );
    m_pGimmickEncount->CreateDebugMenu( m_pFieldRootGroup );
  }

  // Finder
  {
    m_pPokeFinder = GFL_NEW( m_pHeap ) DebugPokeFinder();
    m_pPokeFinder->Initialize( m_pHeap, m_pFieldRootGroup, m_pMsgData );
  }
  
  // 試練モデル
  {
    m_pTrialModel = GFL_NEW( m_pHeap ) Field::Debug::DebugTrialModel();
    m_pTrialModel->Initialize( m_pHeap, m_pMsgData );
    m_pTrialModel->CreateDebugMenu( m_pFieldRootGroup );
  }

  // 往来
  {
    SetupTrafficManager( pTrafficManager );
  }

  // エフェクト関連
  {
    m_pEffect = GFL_NEW( m_pHeap ) Field::Debug::DebugEffect();
    m_pEffect->Initialize( m_pHeap, pEffectManager, m_pMsgData );
    m_pEffect->CreateDebugMenu( m_pFieldRootGroup );
  }

  // Data
  {
    ::Debug::DebugWin::DebugWinWork *work = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
    gfl2::debug::DebugWinGroup *pDataGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_data_group, m_pFieldRootGroup );
    {
      gfl2::debug::DebugWinGroup *pPlayerID = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_data_player_id, pDataGroup );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pPlayerID, m_pHeap, m_pMsgData, msg_dmenu_data_player_id_00, NULL, DebugWinFunc_Data_PlayerIDGet, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pPlayerID, m_pHeap, m_pMsgData, msg_dmenu_data_player_id_01, NULL, DebugWinFunc_Data_PlayerIDSet, NULL );
      gfl2::debug::DebugWin_AddItemValueU16( pPlayerID, m_pHeap, m_pMsgData , msg_dmenu_data_player_id_02, &(work->mPlayerIDUpper), 0, 0xFFFF, 1);
      gfl2::debug::DebugWin_AddItemValueU16( pPlayerID, m_pHeap, m_pMsgData , msg_dmenu_data_player_id_03, &(work->mPlayerIDLower), 0, 0xFFFF, 1);
    }
    gfl2::debug::DebugWin_AddItemUpdateFunc( pDataGroup, m_pHeap, m_pMsgData, msg_dmenu_data_player_name, NULL, DebugWinFunc_Data_PlayerName, NULL );
    {
      gfl2::debug::DebugWinGroup *pMoney    = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_data_money, pDataGroup );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pMoney, m_pHeap, m_pMsgData, msg_dmenu_data_money_00, NULL, DebugWinFunc_Data_MoneyGet, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pMoney, m_pHeap, m_pMsgData, msg_dmenu_data_money_01, NULL, DebugWinFunc_Data_MoneySet, NULL );
      gfl2::debug::DebugWin_AddItemValueU32( pMoney, m_pHeap, m_pMsgData , msg_dmenu_data_money_02, &(Field::Debug::DebugTypes::s_DebugMyStatusParam.nGold), 0, Savedata::Misc::MY_GOLD_MAX, 1);
    }
    {
      gfl2::debug::DebugWinGroup *pBP       = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_data_battlepoint, pDataGroup );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pBP, m_pHeap, m_pMsgData, msg_dmenu_data_battlepoint_00, NULL, DebugWinFunc_Data_BtlPtGet, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pBP, m_pHeap, m_pMsgData, msg_dmenu_data_battlepoint_01, NULL, DebugWinFunc_Data_BtlPtSet, NULL );
      gfl2::debug::DebugWin_AddItemValueU32( pBP, m_pHeap, m_pMsgData , msg_dmenu_data_battlepoint_02, &(Field::Debug::DebugTypes::s_DebugMyStatusParam.nBtlPt), 0, Savedata::Misc::BP_MAX, 1);
    }
    {
      gfl2::debug::DebugWinGroup *pGameTime = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_data_gametime, pDataGroup );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGameTime, m_pHeap, m_pMsgData, msg_dmenu_data_gametime_00, NULL, DebugWinFunc_Data_GameTimeGet, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGameTime, m_pHeap, m_pMsgData, msg_dmenu_data_gametime_01, NULL, DebugWinFunc_Data_GameTimeSet, NULL );
      gfl2::debug::DebugWin_AddItemValueInt( pGameTime, m_pHeap, m_pMsgData , msg_dmenu_data_gametime_02, &(Field::Debug::DebugTypes::s_DebugMyStatusParam.year), 0, 3000, 1);
      gfl2::debug::DebugWin_AddItemValueInt( pGameTime, m_pHeap, m_pMsgData , msg_dmenu_data_gametime_02, &(Field::Debug::DebugTypes::s_DebugMyStatusParam.month), 1, 12, 1);
      gfl2::debug::DebugWin_AddItemValueInt( pGameTime, m_pHeap, m_pMsgData , msg_dmenu_data_gametime_02, &(Field::Debug::DebugTypes::s_DebugMyStatusParam.day), 1, 31, 1);
      gfl2::debug::DebugWin_AddItemValueInt( pGameTime, m_pHeap, m_pMsgData , msg_dmenu_data_gametime_02, &(Field::Debug::DebugTypes::s_DebugMyStatusParam.hour), 0, 23, 1);
      gfl2::debug::DebugWin_AddItemValueInt( pGameTime, m_pHeap, m_pMsgData , msg_dmenu_data_gametime_02, &(Field::Debug::DebugTypes::s_DebugMyStatusParam.minute), 0, 59, 1);
      gfl2::debug::DebugWin_AddItemValueInt( pGameTime, m_pHeap, m_pMsgData , msg_dmenu_data_gametime_02, &(Field::Debug::DebugTypes::s_DebugMyStatusParam.second), 0, 59, 1);
    }
    {
      gfl2::debug::DebugWinGroup *pPlayTime = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_data_playtime, pDataGroup );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pPlayTime, m_pHeap, m_pMsgData, msg_dmenu_data_playtime_00, NULL, DebugWinFunc_Data_PlayTimeGet, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pPlayTime, m_pHeap, m_pMsgData, msg_dmenu_data_playtime_01, NULL, DebugWinFunc_Data_PlayTimeSet, NULL );
      gfl2::debug::DebugWin_AddItemValueU16( pPlayTime, m_pHeap, m_pMsgData , msg_dmenu_data_playtime_02, &(Field::Debug::DebugTypes::s_DebugMyStatusParam.nHour), 0, Savedata::PlayTime::PTIME_HOUR_MAX, 1);
      gfl2::debug::DebugWin_AddItemValueU8( pPlayTime, m_pHeap, m_pMsgData , msg_dmenu_data_playtime_03, &(Field::Debug::DebugTypes::s_DebugMyStatusParam.nMin), 0, Savedata::PlayTime::PTIME_MINUTE_MAX, 1);
      gfl2::debug::DebugWin_AddItemValueU8( pPlayTime, m_pHeap, m_pMsgData , msg_dmenu_data_playtime_04, &(Field::Debug::DebugTypes::s_DebugMyStatusParam.nSec), 0, Savedata::PlayTime::PTIME_SECOND_MAX, 1);
    }
    gfl2::debug::DebugWin_AddItemUpdateFunc( pDataGroup, m_pHeap, m_pMsgData, msg_dmenu_data_mega_ring, NULL, DebugWinFunc_Data_MegaRing, DebugWinFunc_Data_MegaRing_Draw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pDataGroup, m_pHeap, m_pMsgData, msg_dmenu_data_z_ring, NULL, DebugWinFunc_Data_ZenryokuRing, DebugWinFunc_Data_ZenryokuRing_Draw );
  }

  // フィールドデモ
  {
    Fieldmap* pFieldmap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
    m_pFieldDemo = GFL_NEW(m_pHeap) DebugFieldDemo();
    m_pFieldDemo->Initialize( m_pHeap, m_pMsgData, pFieldmap );
    m_pFieldDemo->CreateDebugMenu(m_pFieldRootGroup);
  }

  // 育て屋
  {
    m_pSodateya = GFL_NEW(m_pHeap) DebugSodateya();
    m_pSodateya->Initialize( m_pHeap, m_pMsgData );
    m_pSodateya->CreateDebugMenu( m_pFieldRootGroup );
  }

  // 釣り
  {
    m_pFishing = GFL_NEW(m_pHeap) DebugFishing();
    m_pFishing->Initialize(m_pHeap, m_pMsgData);
    m_pFishing->CreateDebugMenu(m_pFieldRootGroup);
  }

  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Fieldmap* pFieldmap = pGameManager->GetFieldmap();
    Savedata::BattleInstSave* pSave = pGameManager->GetGameData()->GetBattleInstSave();
    m_pBattleInst = GFL_NEW( m_pHeap) DebugBattleInst();
    m_pBattleInst->Initialize( m_pHeap, pFieldmap, m_pMsgData, pSave);
    m_pBattleInst->CreateDebugMenu(m_pFieldRootGroup);
  }

  //レコード
  {
    Savedata::Record* pRecord = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetRecordSaveData();
    u32* pRecord4Byte = (u32*)pRecord->GetData();
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"Record" , m_pFieldRootGroup , 2 );
    gfl2::debug::DebugWinGroup* p4ByteGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"4Byte" , pGropWork , 100 );
    gfl2::debug::DebugWinGroup* p2ByteGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"2Byte" , pGropWork , 100 );
    for( u32 index = 0 ; index < 100 ; ++index )
    {
      gfl2::debug::DebugWin_AddItemValueU32( p4ByteGroup, m_pHeap, m_pMsgData, msg_dmenu_record_4byte_00 + index, pRecord4Byte , 0, 1000000000, 1);
      pRecord4Byte++;
    }

    u16* pRecord2Byte = (u16*)pRecord4Byte;
    for( u32 index = 0 ; index < 100 ; ++index )
    {
      gfl2::debug::DebugWin_AddItemValueU16( p2ByteGroup, m_pHeap, m_pMsgData, msg_dmenu_record_2byte_00 + index, pRecord2Byte , 0, 65535, 1);
      pRecord2Byte++;
    }
  }


  
  {
    gfl2::debug::DebugWinGroup* pDiaryGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"PokeDiary", m_pFieldRootGroup , 3 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pDiaryGroup , m_pHeap, L"Dump", this, DebugWinFunc_PokeDiaryDumpCount, NULL );

    gfl2::debug::DebugWinGroup* pCountGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"Count", pDiaryGroup , 100 );
    gfl2::debug::DebugWinGroup* pWriteGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"Write", pDiaryGroup );

    {
      // ポケダイアリーCOUNT
      for(u32 i=0;i<Savedata::PokeDiarySave::DIARYTYPE_COUNT_MAX;i++){
        m_pUserWorkPokeDiaryCount[i].pThis   = this;
        m_pUserWorkPokeDiaryCount[i].labelId = i;
        m_pUserWorkPokeDiaryCount[i].datId = static_cast<u32>(Savedata::PokeDiarySave::DIARYTYPE_COUNT);
        gfl2::debug::DebugWin_AddItemUpdateFunc( pCountGroup , m_pHeap , L"", &m_pUserWorkPokeDiaryCount[i],
                            NULL,
                            DebugWinFunc_PokeDiaryList );
      }
    }

    {
      // 初期値
      m_diaryId = 1;
      m_diaryRomCode = PM_VERSION;

      // ポケダイアリー書き込み
      gfl2::debug::DebugWin_AddItemValueU32( pWriteGroup, m_pHeap, L"ID", &m_diaryId , 0, 100, 1);
      gfl2::debug::DebugWin_AddItemValueU32( pWriteGroup, m_pHeap, L"ROM", &m_diaryRomCode , 0, 100, 1);
      gfl2::debug::DebugWin_AddItemValueU32( pWriteGroup, m_pHeap, L"DATA1", &m_diaryData[0] , 0, 65535, 1);
      gfl2::debug::DebugWin_AddItemValueU32( pWriteGroup, m_pHeap, L"DATA2", &m_diaryData[1] , 0, 65535, 1);
      gfl2::debug::DebugWin_AddItemValueU32( pWriteGroup, m_pHeap, L"DATA3", &m_diaryData[2] , 0, 65535, 1);


      // 実行
      gfl2::debug::DebugWin_AddItemUpdateFunc( pWriteGroup , m_pHeap , L"A: Set", this,
                           DebugWinFunc_PokeDiaryWrite,
                           NULL );
    }
  }

  //ふしぎなおくりもの
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"MisteryGift" , m_pFieldRootGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , m_pHeap , L"A: SetTestData", this,
                                             DebugWinFunc_MisteryGiftSetTestData , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , m_pHeap , L"A: SetPokeDataMine", this,
      DebugWinFunc_MisteryGiftSetPokeDataMine , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , m_pHeap , L"A: SetPokeDataOthers", this,
      DebugWinFunc_MisteryGiftSetPokeDataOthers , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , m_pHeap , L"A: SetPokeDataEgg", this,
      DebugWinFunc_MisteryGiftSetPokeDataEgg , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , m_pHeap , L"A: SetPokeDataWithItem", this,
      DebugWinFunc_MisteryGiftSetPokeDataWithItem , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , m_pHeap , L"A: SetItemData", this,
      DebugWinFunc_MisteryGiftSetItemData , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , m_pHeap , L"A: SetBPData", this,
      DebugWinFunc_MisteryGiftSetBPData , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , m_pHeap , L"A: SetMameData", this,
      DebugWinFunc_MisteryGiftSetMameData , NULL );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , m_pHeap , L"A: ClearTestData", this,
                                             DebugWinFunc_MisteryGiftClearTestData , NULL );
  }

  // CharaViewer
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pFieldRootGroup, m_pHeap, m_pMsgData, msg_dmenu_field_call_chara_viewer, this ,DebugWinFunc_CallCharaViewer, NULL);
  }

  // 着せ替えエージングテスト
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_kisekae_title , m_pFieldRootGroup );
    gfl2::debug::DebugWin_AddItemValueU32  ( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_kisekae_cnt                    , &kisekaeContinueCnt, 0,999999, 1);
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_kisekae_start                  , this ,DebugWinFunc_CallCharaKisekae               , NULL,kisekaeContinueCnt);
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_kisekae_start_current_position , this ,DebugWinFunc_CallCharaKisekaeCurrentPosition, NULL,kisekaeContinueCnt);
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_ride_start_current_position    , this ,DebugWinFunc_CallCharaRideCurrentPosition   , NULL,kisekaeContinueCnt);
  }

  // NPC挙動調整 
  {
    gfl2::debug::DebugWinGroup* pNpcGrop = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"NPC挙動調整" , m_pFieldRootGroup );
    
    // パラメータ調整
    {
      gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"パラメータ調整" , pNpcGrop );
      Field::MoveModel::FieldMoveCodeNpc::SETTING* pSetting = Field::MoveModel::FieldMoveCodeNpc::GetSettingData();

      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"注目発動範囲[角度]"              , &pSetting->degreeRangeForInterest,      0,    360,   0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"回転発動範囲[角度]"              , &pSetting->degreeRangeForRotation,      0,    360,   0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"注目発動範囲[距離]"              , &pSetting->distanceRangeForInterest,    0,   5000,   0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"注目発動範囲(ワイド版)[距離]"     , &pSetting->distanceRangeForInterestWide,0,   5000,   0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"注目発動範囲(縮小版)[距離]"       , &pSetting->distanceRangeForInterestShort,0,   5000,   0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"回転発動範囲[距離]"              , &pSetting->distanceRangeForRotation,    0,   5000,   0.1);

      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"注目発動範囲[角度][内側]"         , &pSetting->inSideDegreeRangeForInterest,      0,    360,   0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"回転発動範囲[角度][内側]"         , &pSetting->inSideDegreeRangeForRotation,      0,    360,   0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"注目発動範囲[距離][内側]"         , &pSetting->inSideDistanceRangeForInterest,    0,   5000,   0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"回転発動範囲[距離][内側]"         , &pSetting->inSideDistanceRangeForRotation,    0,   5000,   0.1);

      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"注目発動範囲[角度][最近接]"         , &pSetting->nearestDegreeRangeForInterest,      0,    360,   0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"回転発動範囲[角度][最近接]"         , &pSetting->nearestDegreeRangeForRotation,      0,    360,   0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"注目発動範囲[距離][最近接]"         , &pSetting->nearestDistanceRangeForInterest,    0,   5000,   0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"回転発動範囲[距離][最近接]"         , &pSetting->nearestDistanceRangeForRotation,    0,   5000,   0.1);
      
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"会話終了をリセットする距離",           &pSetting->talkEndResetDistanceRange, 0, 5000, 0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"向きをリセットする距離",               &pSetting->dirResetDistanceRange, 0, 5000, 0.1);
      gfl2::debug::DebugWin_AddItemValueU32( pGropWork,   m_pHeap, L"向きをリセットする待ち時間(最低保障)",   &pSetting->dirResetWaitBase, 0,    5000,   1);
      gfl2::debug::DebugWin_AddItemValueU32( pGropWork,   m_pHeap, L"向きをリセットする待ち時間(乱数)",       &pSetting->dirResetWaitRand, 0,    5000,   1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"会話終了をリセットする距離(縮小版)",     &pSetting->talkEndResetDistanceRangeShort, 0, 5000, 0.1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"向きをリセットする距離(縮小版)",        &pSetting->dirResetDistanceRangeShort, 0, 5000, 0.1);

      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"回転を実行する角度の差分"         , &pSetting->degreeDiffForRotationChange, 0,    360,   0.1);
      gfl2::debug::DebugWin_AddItemValueU32( pGropWork,   m_pHeap, L"回転発動待ち時間"                , &pSetting->enterWaitForInterestToRotation, 0,    5000,   1);
      gfl2::debug::DebugWin_AddItemValueU32( pGropWork,   m_pHeap, L"注目解除にかけるフレーム"         , &pSetting->disableInterestFrame,           0,    5000,   1);
      gfl2::debug::DebugWin_AddItemValueU32( pGropWork,   m_pHeap, L"元の状態に戻す発動待ち時間"       , &pSetting->enterWaitForReturn,             0,    5000,   1);

      gfl2::debug::DebugWin_AddItemValueU32( pGropWork,   m_pHeap, L"回転にかけるフレーム数"           , &pSetting->rotationFrame,             0,    5000,   1);
      gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, L"カリング時再有効化" , &pSetting->isCullingReset, 0, 1, 1 );
    }

    Field::MoveModel::FieldMoveCodeNpc::SETTING* pSetting = Field::MoveModel::FieldMoveCodeNpc::GetSettingData();

		// @todo 3月ROM仮対応、デバッグメニューグループの総数がリストサイズをオーバーしてしまうため、不要なグループを無効化
#if 0
    // デバッグ切り替え
    {
      gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"デバッグ切り替え" , pNpcGrop );
      Field::MoveModel::FieldMoveCodeNpc::DEBUG_SETTING* pDebugSetting = Field::MoveModel::FieldMoveCodeNpc::GetDebugSettingData();
      
      gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, L"切り替え有効" , &pDebugSetting->isUse, 0, 1, 1);
      gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, L"回転無効" , &pDebugSetting->invalidRotation, 0, 1, 1);
      gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, L"注目無効" , &pDebugSetting->invalidInterest, 0, 1, 1);
      gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, L"初期状態戻し無効" , &pDebugSetting->invalidReturn, 0, 1, 1);
      gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, L"注目範囲の拡大" , &pDebugSetting->enableLookWide, 0, 1, 1);
      gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, L"注目範囲の縮小" , &pDebugSetting->enableLookShort, 0, 1, 1);
    }

    // 注目設定上書き
    {
      gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"注目設定上書き" , pNpcGrop );
      Field::MoveModel::FieldMoveCodeNpc::DEBUG_SETTING* pDebugSetting = Field::MoveModel::FieldMoveCodeNpc::GetDebugSettingData();
      
      gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, L"上書き有効" , &pDebugSetting->isMcnfOverWrite, 0, 1, 1);
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"[Neck]_Low" , &pDebugSetting->interestLimit[0]._Low, -180.0f, 180.0f, 0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"[Neck]_High" , &pDebugSetting->interestLimit[0]._High, -180.0f, 180.0f, 0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"[Neck]_Low2" , &pDebugSetting->interestLimit[0]._Low2, -180.0f, 180.0f, 0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"[Neck]_High2" , &pDebugSetting->interestLimit[0]._High2, -180.0f, 180.0f, 0.1f );

      // 初期値が不明瞭なので無効化
      // gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"[Neck]_Speed" , &pDebugSetting->interestLimit[0]._Speed, 0.0f, 1000.0f, 0.1f );
      
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"[Head]_Low" , &pDebugSetting->interestLimit[1]._Low, -180.0f, 180.0f, 0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"[Head]_High" , &pDebugSetting->interestLimit[1]._High, -180.0f, 180.0f, 0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"[Head]_Low2" , &pDebugSetting->interestLimit[1]._Low2, -180.0f, 180.0f, 0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"[Head]_High2" , &pDebugSetting->interestLimit[1]._High2, -180.0f, 180.0f, 0.1f );
      
      // 初期値が不明瞭なので無効化
      // gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, L"[Head]_Speed" , &pDebugSetting->interestLimit[1]._Speed, 0.0f, 1000.0f, 0.1f );
    }
#endif
  }
    // NPC挙動調整 
  {
    gfl2::debug::DebugWinGroup* pFureaiGrop = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"ふれあい挙動調整", m_pFieldRootGroup);
    {
      gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"パラメータ調整", pFureaiGrop);
      Field::MoveModel::FieldMoveCodeFureaiTypeA::SETTING* pSetting = Field::MoveModel::FieldMoveCodeFureaiTypeA::GetSettingData();
      gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"discoverDistance", &pSetting->walk.discoverDistance, 0.0f, 10000.0f, 1.0f);
      gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"lostDistance", &pSetting->walk.lostDistance, 0.0f, 10000.0f, 1.0f);
      gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"runToWalkDistance", &pSetting->walk.runToWalkDistance, 0.0f, 10000.0f, 1.0f);
      gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"walkToRunDistance", &pSetting->walk.walkToRunDistance, 0.0f, 10000.0f, 1.0f);
      gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"endResetDistance", &pSetting->walk.endResetDistance, 0.0f, 10000.0f, 1.0f);
      gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"rotRange", &pSetting->rotRange, 1.0f, 180.0f, 1.0f);
      gfl2::debug::DebugWin_AddItemValueU32( pGropWork,   m_pHeap, L"moveWaitCount", &pSetting->moveWaitCount, 0, 5000, 1);
      gfl2::debug::DebugWin_AddItemValueU32( pGropWork,   m_pHeap, L"moveKeepCount", &pSetting->moveKeepCount, 0, 5000, 1);
      gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"sleepIconDispDistance", &pSetting->sleepIconDispDistance, 0.0, 100000.0f, 1.0f);

      // SOUND
      {
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"min_volume", &pSetting->sound.min_volume, 0.0f, 1.0f, 0.1f);
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"max_volume", &pSetting->sound.max_volume, 0.0f, 1.0f, 0.1f);
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"min_length", &pSetting->sound.min_length, 0.0f, 10000.0f, 1.0f);
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"max_length", &pSetting->sound.max_length, 0.0f, 10000.0f, 1.0f);
        DebugWin_AddItemValueEaseFunc(pGropWork, m_pHeap, L"ease_func", reinterpret_cast<gfl2::math::Easing::EaseFunc*>(&pSetting->sound.ease_func));
      }
    }
    {
      gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"パラメータ調整(会話イベント開始)", pFureaiGrop);
      Field::MoveModel::FieldMoveCodeFureaiTypeA::SETTING* pSetting = Field::MoveModel::FieldMoveCodeFureaiTypeA::GetSettingData();

      {
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"baseDistance[UNKNOWN]", &pSetting->cameNear[0].baseDistance, 0.0f, 10000.0f, 1.0f);
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"baseBuffetDistance[UNKNOWN]", &pSetting->cameNear[0].baseBuffetDistance, 0.0f, 10000.0f, 1.0f);
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"baseMagnetDistance[UNKNOWN]", &pSetting->cameNear[0].baseMagnetDistance, 0.0f, 10000.0f, 1.0f);
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"baseWalkDistance[UNKNOWN]", &pSetting->cameNear[0].baseWalkDistance, 0.0f, 10000.0f, 1.0f);
      }
      {
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"baseDistance[LIKE]", &pSetting->cameNear[1].baseDistance, 0.0f, 10000.0f, 1.0f);
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"baseBuffetDistance[LIKE]", &pSetting->cameNear[1].baseBuffetDistance, 0.0f, 10000.0f, 1.0f);
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"baseMagnetDistance[LIKE]", &pSetting->cameNear[1].baseMagnetDistance, 0.0f, 10000.0f, 1.0f);
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"baseWalkDistance[LIKE]", &pSetting->cameNear[1].baseWalkDistance, 0.0f, 10000.0f, 1.0f);
      }
      {
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"baseDistance[LOVE]", &pSetting->cameNear[2].baseDistance, 0.0f, 10000.0f, 1.0f);
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"baseBuffetDistance[LOVE]", &pSetting->cameNear[2].baseBuffetDistance, 0.0f, 10000.0f, 1.0f);
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"baseMagnetDistance[LOVE]", &pSetting->cameNear[2].baseMagnetDistance, 0.0f, 10000.0f, 1.0f);
        gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"baseWalkDistance[LOVE]", &pSetting->cameNear[2].baseWalkDistance, 0.0f, 10000.0f, 1.0f);
      }

    }
    
		Field::MoveModel::FieldMoveCodeFureaiTypeA::DEBUG_SETTING* pDebugSetting = Field::MoveModel::FieldMoveCodeFureaiTypeA::GetDebugSettingData();
		gfl2::debug::DebugWin_AddItemValueInt(pFureaiGrop, m_pHeap, L"DebugMoveAreaCollision", &pDebugSetting->isMoveAreaCollison, 0, 1, 1 );
		gfl2::debug::DebugWin_AddItemValueInt(pFureaiGrop, m_pHeap, L"isNadeNadeTest", &pDebugSetting->isNadeNadeTest, 0, 1, 1);
		gfl2::debug::DebugWin_AddItemValueFloat(pFureaiGrop, m_pHeap, L"nadenade_distance", &pDebugSetting->nadenade_distance, 0.0f, 10000.0f, 1.0f);
		gfl2::debug::DebugWin_AddItemValueFloat(pFureaiGrop, m_pHeap, L"nadenade_angle", &pDebugSetting->nadenade_angle, -360.0f, 360.0f, 1.0f);
		gfl2::debug::DebugWin_AddItemValueFloat(pFureaiGrop, m_pHeap, L"nadenade_distance_x", &pDebugSetting->nadenade_distance_x, -500.0f, 500.0f, 1.0f);

		gfl2::debug::DebugWin_AddItemValueInt(pFureaiGrop, m_pHeap, L"DebugMoveAreaCollision", &pDebugSetting->isBaseStanceTest, 0, 1, 1);
		gfl2::debug::DebugWin_AddItemValueInt(pFureaiGrop, m_pHeap, L"DebugMoveAreaCollision", &pDebugSetting->base_stance, 0, Field::PokemonFureai::FPF_BASE_STANCE_MAX - 1, 1 );
  }

  // IDくじ
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData , msg_dmenu_id_present, m_pFieldRootGroup);
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, m_pMsgData , msg_dmenu_id_present_force_hit_num, &DebugTypes::s_IdPresentForceHitNum, -1, 5, 1);
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, m_pMsgData , msg_dmenu_id_present_force_number_flag, &DebugTypes::s_IdPresentForceNumber, 0, 1, 1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData , msg_dmenu_id_present_force_number_5, &DebugTypes::s_IdPresentForceNumber5, 0, 9, 1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData , msg_dmenu_id_present_force_number_4, &DebugTypes::s_IdPresentForceNumber4, 0, 9, 1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData , msg_dmenu_id_present_force_number_3, &DebugTypes::s_IdPresentForceNumber3, 0, 9, 1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData , msg_dmenu_id_present_force_number_2, &DebugTypes::s_IdPresentForceNumber2, 0, 9, 1);
    gfl2::debug::DebugWin_AddItemValueU32( pGropWork, m_pHeap, m_pMsgData , msg_dmenu_id_present_force_number_1, &DebugTypes::s_IdPresentForceNumber1, 0, 9, 1);
  }

  // ロトムパワー
  {
    m_pRotomPower = GFL_NEW(m_pHeap) DebugRotomPower();
    m_pRotomPower->Initialize(m_pHeap, m_pMsgData);
    m_pRotomPower->CreateDebugMenu(m_pFieldRootGroup);
  }

  // リセット @note 誤作動防止のため、基本末尾にしてください
  {
    // フィールドリセット
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pFieldRootGroup, m_pHeap, m_pMsgData, msg_dmenu_field_restart_field, this, CallGameReStartUpdate, NULL );
  }

  // カリング
  {
    // パラメータ調整
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"カリング調整", m_pFieldRootGroup);
    Field::MoveModel::FieldMoveModelManager::SETTING* pSetting = Field::MoveModel::FieldMoveModelManager::GetSettingData();

    gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"通常カリングBOXのサイズ", &pSetting->cullingBoxSizeHalf, 0.0f, 10000.0f, 1.0f );
    gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"トレーナー用カリングBOXのサイズ", &pSetting->cullingBoxSizeHalfForTrainer, 0.0f, 10000.0f, 1.0f );
    gfl2::debug::DebugWin_AddItemValueFloat(pGropWork, m_pHeap, L"表示距離", &pSetting->cullingDispLength, 0.0f, 10000.0f, 1.0f );
    gfl2::debug::DebugWin_AddItemUpdateFunc(pGropWork, m_pHeap, L"カリング対象動作モデル表示数", NULL, NULL, Draw_DispMoveModel );

    // pFieldMoveModelManager->GetCountOfDispMax();


  }

  //マンタインサーフレコード
  {
    m_pMantainRecord = GFL_NEW(m_pHeap) Field::Debug::DebugMantain();
    m_pMantainRecord->Initialize(m_pHeap, m_pMsgData);
    m_pMantainRecord->CreateDebugMenu(m_pFieldRootGroup);
  }
}

/**
* @brief デストラクタ
*
* @return 無し
*/
FieldDebugMenu::~FieldDebugMenu( void )
{
  // マンタイン
  GFL_SAFE_DELETE(m_pMantainRecord);
  // ロトムパワー
  GFL_SAFE_DELETE( m_pRotomPower );
  // バトル施設
  GFL_SAFE_DELETE( m_pBattleInst );
  // 育て屋
  GFL_SAFE_DELETE( m_pSodateya );
  // フィールドデモ
  GFL_SAFE_DELETE( m_pFieldDemo );
  // 配置モデル
  GFL_SAFE_DELETE( m_pStaticModel );
  // 釣り
  GFL_SAFE_DELETE( m_pFishing );
  // 天候
  GFL_SAFE_DELETE( m_pWeather );
  // 情報表示汎用
  GFL_SAFE_DELETE( m_pDispInfo );
  // Finder
  GFL_SAFE_DELETE( m_pPokeFinder );
  // Finder
  GFL_SAFE_DELETE( m_pPokeFinder );
  // ギミックエンカウント関連
  GFL_SAFE_DELETE( m_pGimmickEncount );
  // エンカウント関連
  GFL_SAFE_DELETE( m_pEncount );
  // 往来関連
  GFL_SAFE_DELETE( m_pTraffic );
  // TrialModel
  GFL_SAFE_DELETE( m_pTrialModel );
  // エフェクト
  GFL_SAFE_DELETE( m_pEffect );
  // フェード
  GFL_SAFE_DELETE( m_pFade );
  // @todo 現行のターゲットクラスが自機専用になったので一時的にコメントアウト、2月修正予定
#if 0
  // シグナル
  GFL_SAFE_DELETE( m_pSignal );
#endif
#if defined(GF_ENABLE_DEBUG_EDITOR)
  // イベントカメラエディター
  GFL_SAFE_DELETE( m_pCharaLayoutEditor );
  // キャラレイアウトエディター
  GFL_SAFE_DELETE( m_pEventCameraEditor );
#endif // GF_PLATFORM_WIN32
  // カメラ関連
  {
    GFL_SAFE_DELETE( m_pCameraGroup );
  }

  GFL_SAFE_DELETE( m_pImpl->m_AttrDispEditor );

  // Fieldグループ以下を削除する
  if( m_pFieldRootGroup )
  {
    GFL_SAFE_DELETE(m_pMapJump);
    
    if( m_pTimeGroup )
    {
      gfl2::debug::DebugWin_RemoveGroup( m_pTimeGroup );
      m_pTimeGroup = NULL;
    }
    
    gfl2::debug::DebugWin_RemoveGroup( m_pFieldRootGroup );
    m_pFieldRootGroup = NULL;
  }
  if( m_pScriptGroup )
  {
    gfl2::debug::DebugWin_RemoveGroup( m_pScriptGroup );
    m_pScriptGroup = NULL;
  }

  if( m_pItemItem )
  {
    gfl2::debug::DebugWin_GetTopGroup()->RemoveItem( m_pItemItem );
    m_pItemItem = NULL;
  }

  if( m_pRotom )
  {
    gfl2::debug::DebugWin_RemoveGroup( m_pRotom );
    m_pRotom = NULL;
  }

  if( m_pRibbonGrop )
  {
    gfl2::debug::DebugWin_RemoveGroup( m_pRibbonGrop );
    m_pRibbonGrop = NULL;
  }

  if( m_pPokemonGroup )
  {
    gfl2::debug::DebugWin_RemoveGroup( m_pPokemonGroup );
    m_pPokemonGroup = NULL;
  }

  GFL_SAFE_DELETE( m_pRibbonMsgData );
  
  GFL_SAFE_DELETE_ARRAY( m_pUserWorkRibbonArray );
  GFL_SAFE_DELETE_ARRAY( m_pUserWorkPokeDiaryCount );
  GFL_SAFE_DELETE_ARRAY( m_pUserWorkTimeControlArray );
  

  GFL_SAFE_DELETE( m_pImpl );
}

gfl2::debug::DebugWinGroup* FieldDebugMenu::GetCameraRootGroup( void )
{
  if( m_pCameraGroup == NULL )
  {
    return NULL;
  }

  return m_pCameraGroup->GetTopGroup();
}

/**
* @brief 地形関連メニュー管理クラスを取得する
*
* @return メニュー管理クラス
*/
Field::Debug::Menu::Terrain* FieldDebugMenu::GetTerrainMenu( void )
{
  return m_pImpl->m_pDebugMenuTerrain;
}

void FieldDebugMenu::SetupTrafficManager( TrafficManager *pMgr )
{
  m_pTraffic = GFL_NEW( m_pHeap ) DebugTraffic();
  m_pTraffic->Initialize( m_pHeap, pMgr, m_pMsgData );
  m_pTraffic->CreateDebugMenu(m_pFieldRootGroup);
}

gfl2::debug::DebugWinFunc* FieldDebugMenu::DebugWin_AddItemValueEaseFunc( gfl2::debug::DebugWinGroup *parent , gfl2::heap::HeapBase *heap , const wchar_t *name , gfl2::math::Easing::EaseFunc* target )
{
  return gfl2::debug::DebugWin_AddItemUpdateFunc( 
    parent, 
    heap, 
    name,
    target,
    Item::UpdateEaseFunc,
    Item::DrawEaseFunc
    );
}

//
// private
//




//
// @todo スクリプト関連のコード追加位置が煩雑なので整理して欲しい
//

//------------------------------------------------------------------------------
/**
 * @func        DebugWin_DrawTick2
 * @brief       DebugWin_DrawTick2関数(デバッグ用).
 * @author      N.Takeda
 * @date        2015/05/20(水) 17:39:00
 *
 * @param[in,out] void*                       userWork  .
 * @param[in,out] gfl2::debug::DebugWinItem*  item      .
 *
 * @return      wchar_t*  
 */
//------------------------------------------------------------------------------
wchar_t* DebugWin_DrawTick2( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN ,L"%ls" ,item->GetName() );
  return workStr;
#else
  return NULL;
#endif
}

//------------------------------------------------------------------------------
/**
 * @func        DebugWin_ScriptTest
 * @brief       DebugWin_ScriptTest関数(デバッグ用).
 * @author      N.Takeda
 * @date        2015/05/20(水) 17:39:09
 *
 * @param[in,out] void*                       userWork  .
 * @param[in,out] gfl2::debug::DebugWinItem*  item      .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void DebugWin_ScriptTest( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  s32       iId;
  
  if( item->GetScrId() >= 0 )
  {
    iId = item->GetScrId();
  }else{
    /// TEST_ZONE の互換に残す.
    static const s32  DebugScriptId = (60000);
    wchar_t   *str = item->GetName();
    s32       num = (str[0]==' '?0:str[0]-'0')*10 + (str[1]-'0');
    
    if( num == 9 ){
      iId = 2100;
    }
    else if(num>=90){
      iId = num-90+1;
    }else{
      iId = DebugScriptId + num*100;
    }
  }

  //::Debug::DebugWin::DebugWinWork *work = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
  GameSys::GameManager *p_gman = static_cast<GameSys::GameManager*>(userWork);
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_CHANGE_VALUE);
    
    Field::EventScriptCall::CallScript( p_gman, iId, NULL );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
#endif
}

// Group ctor
static void CreateGroup_Script( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>(userWork);
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGropWork )
  {
    gfl2::heap::HeapBase *heap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_00, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_EVENT_JUMP );

#if defined(GF_ENABLE_DEBUG_EDITOR)
    pDebug->GetDebugCharaLayoutEditor()->CreateDebugMenu( pGropWork );
    pDebug->GetDebugEventCameraEditor()->CreateDebugMenu( pGropWork );
#endif
    pDebug->DebugTestMenu_FlagWork( heap, pGropWork );
    
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_01, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_EVENT_JUMP_SUB );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_02, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_SCR_DUMMY );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_03, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_IWASAWA );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_04, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_MURANAKA_RYOTA );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_05, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_SUGINAKA_KATSUNORI );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_06, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_TAKEDA_NOBUTAKA );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_07, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_TAMAGAWA_SATOSHI );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_08, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_WATANABE_YUYA );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_09, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_KOJIMA_YOSHIKAZU );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_10, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_TANI_HIROYUKI );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_11, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_POKECEN_COUNTER );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_12, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_OHHIRA_SYO );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_13, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_HOSAKA_GENYA );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_14, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_ASABA_AKINORI );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_15, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2, SCRID_DEBUG_KODA_RENA );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , heap , pMsgData , msg_dmenu_scrpt_90, gMan , DebugWin_ScriptTest , DebugWin_DrawTick2 );
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, heap, pDebug->GetMessageData(), msg_dmenu_script_heap_check_off_enable, &FieldScript::FieldScriptSystem::DebugHeapCheckOffEnableFlag, 0, 1, 1);
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, heap, pDebug->GetMessageData(), msg_dmenu_script_assert_enable, &ScriptAssert::ScriptAssertEnableFlag, 0, 1, 1);
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, heap, pDebug->GetMessageData(), msg_dmenu_script_hio_enable, &FieldScript::FieldScriptSystem::DebugHioEnableFlag, 0, 1, 1);
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, heap, pDebug->GetMessageData(), msg_dmenu_script_edit_enable, &FieldScript::FieldScriptSystem::DebugSceneEditEnableFlag, 0, 1, 1);

    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, heap, L"DebugEndPositionOutPutEnableFlag", &FieldScript::FieldScriptSystem::DebugEndPositionOutPutEnableFlag, 0, 1, 1);
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, heap, L"DebugEndPositionCheckEnableFlag", &FieldScript::FieldScriptSystem::DebugEndPositionCheckEnableFlag, 0, 1, 1);
  }
}

static void DestroyGroup_Script( void *userWork, gfl2::debug::DebugWinItem *item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );

#if defined(GF_ENABLE_DEBUG_EDITOR)
  pDebug->GetDebugEventCameraEditor()->DeleteDebugMenu();
  pDebug->GetDebugCharaLayoutEditor()->DeleteDebugMenu();
#endif
  pDebug->DebugTestMenu_ClearFlagWork( pDebug->GetHeap() );

  DestroyGroup( userWork, item );
}


//------------------------------------------------------------------------------
/**
 * @func        DebugTestMenu_Script
 * @brief       DebugTestMenu_Script関数(デバッグ用).
 * @author      N.Takeda
 * @date        2015/05/20(水) 17:59:38
 *
 * @param[in,out] gfl2::heap::HeapBase* heap  .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void FieldDebugMenu::DebugTestMenu_Script( gfl2::heap::HeapBase *heap )
{
  for (u32 ii = 0; ii < GARC_ScriptDebugSymbol_DATA_NUM; ++ii )
  {
    m_pFlagWork[ii] = NULL;
  }
  m_pScriptGroup = gfl2::debug::DebugWin_AddGroup( heap , m_pMsgData, msg_dmenu_scrpt_top );
  m_pScriptGroup->SetCreateDestroyGroupFunc( this, CreateGroup_Script, DestroyGroup_Script );
}


//------------------------------------------------------------------------------
/**
 * @func        DebugTestMenu_FlagWork
 * @brief       FlagWork初期設定(デバッグ用).
 * @author      N.Takeda
 * @date        2015/05/26(火) 15:25:35
 *
 * @param[in,out] gfl2::heap::HeapBase* heap  .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void FieldDebugMenu::DebugTestMenu_FlagWork( gfl2::heap::HeapBase *heap, gfl2::debug::DebugWinGroup* parent )
{
  // イベントワーク名バッファ確保
  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq  oreq;
    oreq.arcId = ARCID_SCRIPTDEBUGSYMBOL;
    oreq.heapForFile = heap->GetLowerHandle();
    oreq.heapForArcSrc = heap->GetLowerHandle();
    oreq.heapForReq = heap->GetLowerHandle();
    man->SyncArcFileOpen(oreq);
  }

  for (u32 ii = 0; ii < GARC_ScriptDebugSymbol_DATA_NUM; ++ii )
  {
    void*   pBuff=NULL;
    size_t  ReadSize = 0;

    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq  req;
    req.arcId = ARCID_SCRIPTDEBUGSYMBOL;
    req.datId = ii;
    req.ppBuf = &pBuff; ///< SyncArcFileLoadData 内で確保される
    req.pBufSize = NULL;
    req.pRealReadSize = &ReadSize;
    req.heapForBuf = heap->GetLowerHandle();
    req.heapForReq = heap->GetLowerHandle();
    req.align = 4;
    man->SyncArcFileLoadData(req);
    
    
    GFL_ASSERT( ReadSize % DEBUG_SCR_EACH_SIZE == 0 );
    m_EventWorkTableSize[ii] = ReadSize / DEBUG_SCR_EACH_SIZE;

    GflHeapSafeFreeMemory(pBuff);
  }
  m_pFlagWorks = gfl2::debug::DebugWin_AddGroup( heap , m_pMsgData, msg_dmenu_flag_ctr_top, parent );
  m_pFlagWorks->SetCreateDestroyGroupFunc( this, CreateGroup_ScriptFlagworks, DestroyGroup_ScriptFlagworks );
}


//------------------------------------------------------------------------------
/**
 * @func        DebugTestMenu_ClearFlagWork
 * @brief       FlagWork後始末(デバッグ用).
 * @author      N.Takeda
 * @date        2015/05/26(火) 20:11:44
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void FieldDebugMenu::DebugTestMenu_ClearFlagWork( gfl2::heap::HeapBase *heap )
{
  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  if( m_pFlagWorks )
  {
    gfl2::debug::DebugWin_RemoveGroup( m_pFlagWorks );
    m_pFlagWorks = NULL;
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
    creq.fileName   = NULL;
    creq.arcId = ARCID_SCRIPTDEBUGSYMBOL;
    creq.heapForReq = heap->GetLowerHandle();
    man->SyncArcFileClose(creq);    
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  時間の変更
 */
//-----------------------------------------------------------------------------
static void DebugWinFunc_CallFakeTime( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GFL_UNUSED( item );

  FieldDebugMenu::USER_WORK* pUserWork = reinterpret_cast<FieldDebugMenu::USER_WORK*>(userWork);
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    // 時間の管理
    for( u32 i=0; i<GFL_NELEMS(sc_TIMECONTROL); ++i )
    {
      if( sc_TIMECONTROL[i].labelId == pUserWork->labelId )
      {
        GameSys::AlolaTime alolaTime;
        alolaTime.Initialize();

        switch( sc_TIMECONTROL[i].status ){
        case ENTIME_STOPFAKETIME:
          alolaTime.StopFakeTime();
          break;
        case ENTIME_STARTFAKETIME:
          alolaTime.StartFakeTime( sc_TIMECONTROL[i].rate );
          break;
        case ENTIME_FIXFAKETIME:
          alolaTime.FixFakeTime( sc_TIMECONTROL[i].hour, sc_TIMECONTROL[i].minute );
          break;
        }
        break;
      }
    }
  }
}


/** 
 *  @brief アローラ時間　反転世界にジャンプする。
 */
static void DebugWinFunc_CallAlolaTimeReverseMapJump( void* userWork, gfl2::debug::DebugWinItem* item )
{
  FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  gfl2::math::Vector3 pos = pGameManager->GetFieldmap()->GetPlayerPosition();

  GameSys::AlolaTime::AlolaOtherWorldType type = GameSys::AlolaTime::ALOLA_OTHER_WOLRD_TYPE_MAX;

  if( GameSys::AlolaTime::CheckAlolaOtherWorld( pGameManager->GetGameData() ) )
  {
    type = GameSys::AlolaTime::ALOLA_OTHER_WOLRD_TYPE_MAX;
  }
  else
  {
    static bool is_switch = false;
    if(!is_switch)
    {
      type = GameSys::AlolaTime::ALOLA_OTHER_WOLRD_TYPE_NIKKOU;
      is_switch = true;
    }
    else
    {
      type = GameSys::AlolaTime::ALOLA_OTHER_WOLRD_TYPE_GEKKOU;
      is_switch = false;
    }
  }

  {
    FieldScript::FieldScriptSystem* script = pGameManager->GetFieldScriptSystem();
    if( script->IsRunning()){
      GFL_PRINT( "スクリプトイベント実行中はデバッグマップジャンプできません\n" ); return;
    }
  }
  EventMapJump::CallMapJumpOtherWorldTime( pGameManager, pGameManager->GetFieldmap()->GetZoneID(), pos, type );
  
  // ウィンドウを閉じる
  gfl2::debug::DebugWin_CloseWindow();
}


FlagWorkImpl* FieldDebugMenu::GetFlagWorkImpl( void )
{
  return m_pImpl->m_pFlagWorkImpl;
}


FlagWorkSearchImpl* FieldDebugMenu::GetFlagWorkSearchImpl( void )
{
  return m_pImpl->m_pFlagWorkSearchImpl;
}



GFL_NAMESPACE_END( Debug )
GFL_NAMESPACE_END( Field )


#endif // #if PM_DEBUG
