//======================================================================
/**
 * @file FieldDebugSodateya.cpp
 * @date 2015/12/19 19:41:07
 * @author onoue_masayuki
 * @brief 育て屋デバッグメニュー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include <Debug/include/gfl2_DebugWinItemTypes.h>

#include "GameSys/include/GameManager.h"

#include "Field/FieldRo/include/Debug/FieldDebugSodateya.h"
#include "Field/FieldStatic/include/Sodateya/FieldSodateya.h"

#include "Debug/DebugEvent/EventDebugPokeMake/include/EventDebugPokeMake.h"

#include "niji_conv_header/message/debug/msg_debug_menu.h"


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 *  @brief  Group ctor
 */
void DebugSodateya::CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugSodateya* pDebug = reinterpret_cast<DebugSodateya*>( userWork );
  gfl2::debug::DebugWinGroup* pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase* pHeap    = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_breed100,         pDebug, DebugSodateya::DebugWinFunc_SodateyaBreed100, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_breed10000,    		pDebug, DebugSodateya::DebugWinFunc_SodateyaBreed10000, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_breed1milion,  		pDebug, DebugSodateya::DebugWinFunc_SodateyaBreed1milion, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_eggwarm,       		pDebug, DebugSodateya::DebugWinFunc_SodateyaEggWarm, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_eggset,        		pDebug, DebugSodateya::DebugWinFunc_SodateyaEggSet, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_eggdel,        		pDebug, DebugSodateya::DebugWinFunc_SodateyaEggDel, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_eggadopt,      		pDebug, DebugSodateya::DebugWinFunc_SodateyaEggAdopt, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_pokeleave,     		pDebug, DebugSodateya::DebugWinFunc_SodateyaPokeLeave, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_poketakeback,  		pDebug, DebugSodateya::DebugWinFunc_SodateyaPokeTakeBack, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_poketakeback2, 		pDebug, DebugSodateya::DebugWinFunc_SodateyaPokeTakeBack2, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_partymemberdel,   pDebug, DebugSodateya::DebugWinFunc_SodateyaPartyMemberDel, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_pokepara1check,  	pDebug, DebugSodateya::DebugWinFunc_SodateyaPokePara1Check, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_pokepara2check,  	pDebug, DebugSodateya::DebugWinFunc_SodateyaPokePara2Check, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup,pHeap,pMsgData, msg_dmenu_sodateya_pokeparaeggcheck, pDebug, DebugSodateya::DebugWinFunc_SodateyaPokeParaEggCheck, NULL );
  }
}

/**
 *  @brief  Group dtor
 */
void DebugSodateya::DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup* pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

DebugSodateya::DebugSodateya()
{}

/**
  *  @brief  初期化
  */
void DebugSodateya::Initialize( gfl2::heap::HeapBase* pHeap, gfl2::str::MsgData* pMsgData )
{
  this->pHeap = pHeap;
  this->pMsgData = pMsgData;
}

/**
  *  @brief  メニューの作成
  */
void DebugSodateya::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( this->pHeap, L"Sodateya", root );
  pGroup->SetCreateDestroyGroupFunc( this, this->CreateDebugGroupFunc, this->DestroyDebugGroupFunc );
}

//育てる
void DebugSodateya::DebugWinFunc_SodateyaBreed100( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_BREED, 100 );
}
void DebugSodateya::DebugWinFunc_SodateyaBreed10000( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_BREED, 10000 );
}
void DebugSodateya::DebugWinFunc_SodateyaBreed1milion( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_BREED, 1000000 );
}
///タマゴ系
void DebugSodateya::DebugWinFunc_SodateyaEggWarm( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_EGG_WARM, 0 );
}
void DebugSodateya::DebugWinFunc_SodateyaEggSet( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_EGG_SET, 0 );
}
void DebugSodateya::DebugWinFunc_SodateyaEggDel( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_EGG_DEL, 0 );
}
void DebugSodateya::DebugWinFunc_SodateyaEggAdopt( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_EGG_ADOPT, 0 );
}
//ポケモン系
void DebugSodateya::DebugWinFunc_SodateyaPokeLeave( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_POKE_LEAVE, 0 );
}
void DebugSodateya::DebugWinFunc_SodateyaPokeTakeBack( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_POKE_TAKEBACK, 0 );
}
void DebugSodateya::DebugWinFunc_SodateyaPokeTakeBack2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_POKE_TAKEBACK, 1 );
}
void DebugSodateya::DebugWinFunc_SodateyaPartyMemberDel( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_MEMBER_DEL, 0 );
}
//PokeParaチェック系
void DebugSodateya::DebugWinFunc_SodateyaPokePara1Check( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_PP_CHECK, 0 );
}
void DebugSodateya::DebugWinFunc_SodateyaPokePara2Check( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_PP_CHECK, 1 );
}
void DebugSodateya::DebugWinFunc_SodateyaPokeParaEggCheck( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugWinFunc_Sodateya( userWork, item, SODATEYA_MENU_PP_CHECK, 2 );
}

//ポケモンを育てる
void DebugSodateya::DebugWinFunc_Sodateya( void* userWork, gfl2::debug::DebugWinItem* item, SodateyaMenuID menu, int param )
{
  DebugSodateya* dbgmenu = reinterpret_cast<DebugSodateya*>(userWork);
  GameSys::GameManager*    p_gman   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  gfl2::ui::Button*	       btn        = p_gman->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  Field::FieldSodateya* pSodateya = p_gman->GetFieldmap()->GetSodateya();
  Savedata::Sodateya* pSodateyaSave = p_gdata->GetSodateyaSave();
  pml::PokeParty* party = p_gdata->GetPlayerParty();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT);
  Savedata::SodateyaID id = Savedata::SODATEYA_ID_I02;

  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    switch( menu ){
    case SODATEYA_MENU_BREED:
      {
        for(int i = 0;i < param;i++){
          pSodateya->PokeBreed();
        }
      }
      break;
    case SODATEYA_MENU_EGG_WARM:
      {
        for( int i = 0;i < party->GetMemberCount(); i++ ){
          pml::pokepara::PokemonParam* pp = party->GetMemberPointer( i );
          if( pp->IsEgg( pml::pokepara::CHECK_ONLY_LEGAL_EGG ) ){
            pp->SetOriginalFamiliarity( 0 );
          }
        }
      }
      break;
    case SODATEYA_MENU_EGG_SET:
      {
        if( pSodateya->LoveCheck( id ) == SODATEYA_LOVE_WORST ||
            pSodateyaSave->IsEggExist( id ) != false ){
          return;
        }
        pSodateyaSave->EggSet( id );
      }
      break;
    case SODATEYA_MENU_EGG_DEL:
      {
        if( pSodateyaSave->IsEggExist( id ) != false ){
          pSodateyaSave->EggClear( id );
          break;
        }
      }
      return;
    case SODATEYA_MENU_EGG_ADOPT:
      {
        if( pSodateyaSave->IsEggExist( id ) == false ||
            pSodateya->EggAdopt( id ) == false ){
          return;
        }
      }
      break;
    case SODATEYA_MENU_POKE_LEAVE:
      {
        int party_num = party->GetMemberCountEx(pml::PokeParty::POKE_COUNT_TYPE_BATTLE_ENABLE);
        bool leave_f = false;
        if( pSodateyaSave->PokeNumGet( id ) >= 2 ){
          return;
        }
        //手持ちとボックスを合わせたポケモン数（タマゴを含まない）が3匹以上でないと預けられない
        if( party_num < 2 ){
          return;
        }
        if( party_num < 3 ){
          //戦えるポケモンが3匹未満なのでボックスもチェック
          Savedata::BoxPokemon* p_boxpoke = p_gdata->GetBoxPokemon();
          if( p_boxpoke->GetPokemonCountAll( p_heap, false ) == 0 ){
            return;
          }
        }
        //預けられる
        for( int i = 0;i < party->GetMemberCount(); i++ ){
          pml::pokepara::PokemonParam* pp = party->GetMemberPointer( i );
          if( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false && pp->GetHp() > 0 ){
            pSodateya->PokeLeave( id, i );
            leave_f = true;
            break;
          }
        }
        if( leave_f == false ) return;
      }
      break;
    case SODATEYA_MENU_POKE_TAKEBACK:
      {
        if( party->GetMemberCount() >= pml::PokeParty::MAX_MEMBERS ||
            pSodateyaSave->PokeNumGet( id ) <= param ){
          return;
        }
        pSodateya->PokeTakeBack( id, param );
      }
      break;
    case SODATEYA_MENU_MEMBER_DEL:
      {
        if( party->GetMemberCountEx(pml::PokeParty::POKE_COUNT_TYPE_BATTLE_ENABLE) >= 2 ){
          party->RemoveMember( party->GetMemberCount()-1 );
          break;
        }
        return;
      }
    case SODATEYA_MENU_PP_CHECK:
      {
        if( param < 2 ){
          if( pSodateyaSave->PokeNumGet( id ) <= param ) return;
        }else{
          if( pSodateya->LoveCheck( id ) == SODATEYA_LOVE_WORST ) return;
        }

        debug::PokeMake::APP_PARAM* pAppParam = GFL_NEW(p_heap) debug::PokeMake::APP_PARAM;
        pAppParam->param = param;
        pAppParam->poke = pSodateya->DebugPokeParaMake( id, param, p_heap->GetLowerHandle());

        EventDebugPokeMake* pEventDebugPokeMake = Field::Debug::EventDebugPokeMake::CallDebugSodateyaPokeEdit( p_gman, param, true ,pAppParam, DebugSodateya::PPCheck );

        //EventAppCall* event = GAMESYSTEM_EVENT_CALL<gamesystem::GameEventManager, EventAppCall>( p_gman->GetGameEventManager() );
        //xy_debug::DebugPokeMake::APP_PARAM* p_param =
        //  static_cast<xy_debug::DebugPokeMake::APP_PARAM*>(
        //      event->SetCallTargetApplicationAndAllocParam(
        //        static_cast<EventAppCall::CallTarget>(
        //          EventAppCall::CALL_TARGET_SODATEYA_POKE1_CHECK+param),
        //          sizeof(xy_debug::DebugPokeMake::APP_PARAM),
        //          debug_cb_sodateya_pp_check, NULL
        //        )
        //      );
            //dbgmenu->CloseMenu();
        gfl2::debug::DebugWin_CloseWindow();
      }
      break;
    }
    //item->PlaySound( gfl2::debug::SE_ACCEPT );
    
    // 閉じる
    ////dbgmenu->CloseMenu();
  }
  return;
}

void DebugSodateya::PPCheck( debug::PokeMake::APP_PARAM* p_param, GameSys::GameManager* p_gman )
{
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::Sodateya* pSodateyaSave = p_gdata->GetSodateyaSave();
  Savedata::SodateyaID id = Savedata::SODATEYA_ID_I02;

  if( p_param->param == FieldSodateya::DEBUG_PP_MAKE_EGG ){
    //タマゴ生成乱数を進める
    pSodateyaSave->EggClear( id );
  }

  // PokemonMakeの終了時_tempPokeのMonsNoが0だったら格納しない
  if( p_param->result == false ){
    GFL_SAFE_DELETE( p_param->poke );
    return; //確認しただけでキャンセル
  }
  FieldSodateya* p_sodateya = p_gman->GetFieldmap()->GetSodateya();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT);

  if( p_param->param == FieldSodateya::DEBUG_PP_MAKE_EGG ){
    //パラメータが正常なタマゴか？
    if( p_param->poke->IsEgg( pml::pokepara::CHECK_ONLY_LEGAL_EGG ) != false ){
      // 手持ちに空きがあるなら追加、無い場合はBOXに追加
      pml::PokeParty* poke_party = p_gdata->GetPlayerParty();
      if(poke_party->GetMemberCount()<6){  // 手持ちに増やす
        poke_party->AddMember( *p_param->poke );
      }else{
        int ret = p_gdata->GetBoxPokemon()->PutPokemonEmptyTrayAll( p_param->poke, 0, p_heap );
        GFL_PRINT( "box add ret=%d\n", ret );
      }
    }
  }else{
    //パラメータがタマゴの時は書き戻さない
    if( p_param->poke->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false ){
      //育て屋セーブに書き戻し
      p_sodateya->DebugPokeParaSet( id, p_param->param, p_param->poke );
    }
  }
  GFL_SAFE_DELETE( p_param->poke );
}

GFL_NAMESPACE_END(Field)
GFL_NAMESPACE_END(Debug)

#endif //PM_DEBUG
