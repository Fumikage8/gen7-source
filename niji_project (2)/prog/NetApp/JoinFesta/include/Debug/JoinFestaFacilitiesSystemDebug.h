//==============================================================================
/**
 * @file	JoinFestaFacilitiesSystemDebug.h
 * @brief	ジョインフェスタ施設システムデバッグ機能
 * @author	ichiraku_katsuhiko
 * @date	2015/10/22　12:47
 */
// =============================================================================
#ifndef __JOIN_FESTA_FACILITES_SYSTEM_DEBUG_H__
#define __JOIN_FESTA_FACILITES_SYSTEM_DEBUG_H__
#pragma once

// ライブラリ
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

// niji
#include "GameSys/include/GameManager.h"
#include "System/include/HeapDefine.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/ScriptWork.h"
#include "App/Shop/include/ShopAppParam.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"

// JFモジュール
#include "NetApp/JoinFesta/include/JoinFestaDataTable/JoinFestaDataTable.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesSystem.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesData.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesModel.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesModelResourceManager.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesMoveModel.h"
//  ジョインフェスタリソースマネージャ
#include "NetApp/JoinFesta/include/JoinFestaResourceManager/JoinFestaResourceManager.h"
// 施設更新イベント
#include "NetApp/JoinFesta/include/Event/JoinFestaFacilitiesModelUpdateEvent.h"
// ジョインフェスタ施設ユーティリティ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaFacilitiesUtil/JoinFestaFacilitiesUtil.h"

#include <Debug/DebugWin/include/DebugWinSystem.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"
#include  <niji_conv_header/message/msg_jf_shop_name.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

#ifdef JF_DEBUG_MENU
class JoinFestaFacilitiesSystemDebug
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaFacilitiesSystemDebug);

public:
  JoinFestaFacilitiesSystemDebug( JoinFestaFacilitiesSystem* pSystem, JoinFestaResourceManager* pResourceManager )
    : m_debugGroup(NULL)
    , m_facilitiesCreateGroup(NULL)
    , m_pSystem(pSystem)
    , m_pResourceManager(pResourceManager)
    , m_facilitiesIndex(0)
    , m_facilitiesId(0)
    , m_facilitiesColor(0)
    , m_facilitiesCharacter(0)
    , m_textureIndex(0)
    , m_debugDeviceManager(NULL)
    , m_pDebugButton(NULL)
  {
    GameSys::GameManager*             pGameManager      = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Field::FieldGimmickJoinFesta*     pJfGimmick        = static_cast<Field::FieldGimmickJoinFesta*>(pGameManager->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
    gfl2::debug::DebugWinGroup*       pJFDebugGroupTop  = pJfGimmick->GetDebugGroup();
    ::Debug::DebugWin::DebugWinWork*  devWork           = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
    gfl2::str::MsgData*               pMsgData          = devWork->GetMessageData();
    gfl2::heap::HeapBase*             pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);

    /*  deviceManager  */
    m_debugDeviceManager  = pGameManager->GetUiDeviceManager();
    m_pDebugButton        = m_debugDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    /*  グループ「施設系」  */
    m_debugGroup  = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_jf_facilities_top, pJFDebugGroupTop );

    /*  グループ「施設作成」  */
    m_facilitiesCreateGroup  = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_jf_facilities_create_top, m_debugGroup );

    // 「施設作成」直下
    {
      // 「施設位置」
      gfl2::debug::DebugWin_AddItemValueU32( m_facilitiesCreateGroup, pHeap, pMsgData, msg_dmenu_jf_facilities_create_00, &m_facilitiesIndex, 0, 6, 1 );
      // 「施設ID」
      gfl2::debug::DebugWin_AddItemUpdateFunc( m_facilitiesCreateGroup, pHeap, pMsgData, msg_dmenu_jf_facilities_create_01, this, DebugFunc_FacilitiesIdUpdate, DebugFunc_FacilitiesIdDraw );
      // 「施設カラー」
      gfl2::debug::DebugWin_AddItemValueU32( m_facilitiesCreateGroup, pHeap, pMsgData, msg_dmenu_jf_facilities_create_02, &m_facilitiesColor, 0, 3, 1 );
      // 「施設店員」
      gfl2::debug::DebugWin_AddItemValueU32( m_facilitiesCreateGroup, pHeap, pMsgData, msg_dmenu_jf_facilities_create_03, &m_facilitiesCharacter, 0, 11, 1 );

      // 「作成」
      gfl2::debug::DebugWin_AddItemUpdateFunc( m_facilitiesCreateGroup, pHeap, pMsgData, msg_dmenu_jf_facilities_create_04, this, DebugFunc_FacilitiesCreateUpdate );
    }

    // 「施設系」直下
    {
      gfl2::debug::DebugWinItemValueU32  <u32>* itemU32   = NULL;

      // 「施設Index」
      gfl2::debug::DebugWin_AddItemValueU32( m_debugGroup, pHeap, pMsgData, msg_dmenu_jf_facilities_00, &m_facilitiesIndex, 0, 6, 1 );
      // 「テクスチャIndex」
      gfl2::debug::DebugWin_AddItemValueU32( m_debugGroup, pHeap, pMsgData, msg_dmenu_jf_facilities_01, &m_textureIndex, 0, 20, 1 );
      // 「施設テクスチャ差し替え」
      gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugGroup, pHeap, pMsgData, msg_dmenu_jf_facilities_02, this, DebugFunc_FacilitiesChangeTexture );
      // 「城テクスチャ差し替え」
      gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugGroup, pHeap, pMsgData, msg_dmenu_jf_facilities_03, this, DebugFunc_CastleChangeTexture );
      // 「強制抽選スロット」
      itemU32 = gfl2::debug::DebugWin_AddItemValueU32( m_debugGroup, pHeap, pMsgData, msg_dmenu_jf_facilities_04, &m_forceSlot, 0, 10 );
      itemU32->SetChangedValueFunc( DebugFunc_ForceSlotChange, this );
    }

  }

  virtual ~JoinFestaFacilitiesSystemDebug()
  {
    // グループ破棄
    if( m_facilitiesCreateGroup != NULL )
    {
      gfl2::debug::DebugWin_RemoveGroup( m_facilitiesCreateGroup );
      m_facilitiesCreateGroup = NULL;
    }
    if( m_debugGroup != NULL )
    {
      gfl2::debug::DebugWin_RemoveGroup( m_debugGroup );
      m_debugGroup = NULL;
    }
  }


private:

  static void DebugFunc_FacilitiesChangeTexture(void* _pThis, gfl2::debug::DebugWinItem* pItem)
  {
    JoinFestaFacilitiesSystemDebug*  pThis = static_cast<JoinFestaFacilitiesSystemDebug*>(_pThis);
    if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
    {
      pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );

      pThis->m_pSystem->GetFacilityModel( pThis->m_facilitiesIndex )->ChangeTexture( pThis->m_textureIndex );
    }
  }
  static void DebugFunc_CastleChangeTexture(void* _pThis, gfl2::debug::DebugWinItem* pItem)
  {
    JoinFestaFacilitiesSystemDebug*  pThis = static_cast<JoinFestaFacilitiesSystemDebug*>(_pThis);
    if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
    {
      pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );

      pThis->m_pSystem->GetCastleModel()->ChangeTexture( pThis->m_textureIndex );
    }
  }

  // 更新:施設ID
  static void DebugFunc_FacilitiesIdUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem)
  {
    JoinFestaFacilitiesSystemDebug* pThis = static_cast<JoinFestaFacilitiesSystemDebug*>(userWork);

    int step = 1;

    if( pThis->m_pDebugButton->IsHold( gfl2::ui::BUTTON_R ) ) { step *= 10; }
    if( pThis->m_pDebugButton->IsHold( gfl2::ui::BUTTON_L ) ) { step *= 100; }

    if(pThis->m_pDebugButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
    {
      pThis->m_facilitiesId += step;
      if( pThis->m_facilitiesId >= NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_MAX )
      {
        pThis->m_facilitiesId = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_MAX-1;
      }
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
    if(pThis->m_pDebugButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
    {
      pThis->m_facilitiesId -= step;
      if( pThis->m_facilitiesId < 0 ){ pThis->m_facilitiesId=0; }
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }
  // 描画：施設ID
  static wchar_t* DebugFunc_FacilitiesIdDraw( void* userWork, gfl2::debug::DebugWinItem *item )
  {
    JoinFestaFacilitiesSystemDebug* pThis = static_cast<JoinFestaFacilitiesSystemDebug*>(userWork);

    wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
    gfl2::str::StrBuf* workStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();

    const NetApp::JoinFesta::JoinFestaDataTable* p_jfDataTable = pThis->m_pSystem->GetDataTable();
    const NetApp::JoinFesta::ShopItemInfo* pShop = p_jfDataTable->GetShopItemInfo( pThis->m_facilitiesId );

    // 施設名用のmstxt使う
    gfl2::str::MsgData* pMsgData = pThis->m_pResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFShopName );
    pMsgData->GetString( pShop->messageID_ShopName, *workStrBuf );

    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d:%ls]", item->GetName(), pThis->m_facilitiesId, workStrBuf->GetPtr() );
    return workStr;
  }

  // 施設作成
  static void DebugFunc_FacilitiesCreateUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem)
  {
    JoinFestaFacilitiesSystemDebug* pThis = static_cast<JoinFestaFacilitiesSystemDebug*>(userWork);
    const NetApp::JoinFesta::JoinFestaDataTable* p_jfDataTable = pThis->m_pSystem->GetDataTable();
    const NetApp::JoinFesta::ShopItemInfo* pShop = p_jfDataTable->GetShopItemInfo( pThis->m_facilitiesId );

    if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
    {
      Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

      // 染物屋だった場合は、色が自動で決まる
      {
        if( pShop->shopType == JoinFestaScript::FACILITIES_TYPE_DYEING )
        {
          const JoinFestaScript::FacilitiesColor color = NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::ConvertDyeingColor( pThis->m_facilitiesId );
          pThis->m_facilitiesColor = static_cast<u32>(color);
        }
      }

      // 施設上書き
      pSave->FacilitiesChange( static_cast<JoinFestaScript::FacilitiesIndex>(pThis->m_facilitiesIndex),
                               static_cast<u8>(pThis->m_facilitiesId),
                               pThis->m_facilitiesCharacter,
                               pThis->m_facilitiesColor );

      // 施設更新イベント
      GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetApp::JoinFesta::JoinFestaFacilitiesModelUpdateEvent>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameEventManager() );
    }
  }

  // 強制抽選スロット
  static void DebugFunc_ForceSlotChange(void* userWork)
  {
    JoinFestaFacilitiesSystemDebug* pThis = static_cast<JoinFestaFacilitiesSystemDebug*>(userWork);
    if( pThis->m_forceSlot > 0 )
    {
#if PM_DEBUG
      pThis->m_pSystem->GetFacilitiesData( static_cast<JoinFestaScript::FacilitiesIndex>(pThis->m_facilitiesIndex) )->DEBUG_SetForceSlot( pThis->m_forceSlot );
#endif
    }
  }


  gfl2::debug::DebugWinGroup* m_debugGroup;
  gfl2::debug::DebugWinGroup* m_facilitiesCreateGroup;
  JoinFestaFacilitiesSystem*  m_pSystem;
  JoinFestaResourceManager*   m_pResourceManager;     ///< ジョインフェスタリソースマネージャ
  u32                         m_facilitiesIndex;  // 施設インデックス(0～7)
  s32                         m_facilitiesId;     // 施設ID
  u32                         m_facilitiesColor;  // 施設カラー
  u32                         m_facilitiesCharacter;  // 施設店員
  u32                         m_textureIndex;     // テクスチャインデックス
  u32                         m_forceSlot;     // 強制抽選スロット
  gfl2::ui::DeviceManager*    m_debugDeviceManager;
  gfl2::ui::Button*           m_pDebugButton;

};


#endif  /*  JF_DEBUG_MENU  */




GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)


#endif // __JOIN_FESTA_FACILITES_SYSTEM_DEBUG_H__
