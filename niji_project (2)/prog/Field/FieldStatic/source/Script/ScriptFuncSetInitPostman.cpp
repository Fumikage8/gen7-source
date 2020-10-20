//=============================================================================
/**
 * @file  ScriptFuncSetInitPostman.cpp
 * @brief 初期化スクリプト用命令セット：配達員系
 * @date  2016.02.21
 */
//=============================================================================

#include <heap/include/gfl2_Heap.h>

#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/SaveData.h"
#include "Savedata/include/MyStatus.h"
#include "Savedata/include/Record.h"
#include "Savedata/include/MysteryGiftSave.h"
#include "Savedata/include/ResortSave.h"

#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptWork.h"

#include "Field/FieldStatic/include/script/ScriptFuncSetInit.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )



/* ------------------------------------------------------------------------- */
/**
 * @brief 配達員　贈り物数を取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PostmanGetGiftCount( AMX * amx, const cell * ptr )
{
  PostmanGiftType type = static_cast<PostmanGiftType>(ptr[1]);

  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = pSys->GetGameManager();
  Savedata::MysteryGiftSave * pGiftSave = p_gman->GetGameData()->GetSaveData()->GetMysteryGiftSave();

  int poke_num = pGiftSave->GetPokeGiftNum();
  int item_num = pGiftSave->GetItemGiftNum();
  int bp_num   = pGiftSave->GetBPGiftNum();
  int mame_num = pGiftSave->GetMameGiftNum();
  int kind_num = 0;
  int all_num = 0;

  if( poke_num ){
    kind_num++;
    all_num += poke_num;
  }

  if( item_num ){
    kind_num++;
    all_num += item_num;
  }

  if( bp_num ){
    kind_num++;
    all_num += bp_num;
  }

  if( mame_num ){
    kind_num++;
    all_num += mame_num;
  }

  switch( type ){
  case POSTMAN_GIFT_TYPE_POKEMON:
    return poke_num;
  case POSTMAN_GIFT_TYPE_ITEM:
    return item_num;
  case POSTMAN_GIFT_TYPE_BP:
    return bp_num;
  case POSTMAN_GIFT_TYPE_POKEMAME:
    return mame_num;
  case POSTMAN_GIFT_TYPE_KIND:
    return kind_num;
  case POSTMAN_GIFT_TYPE_ALL:
    return all_num;
  default:
    break;
  }
  return 0;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief 配達員　贈り物のパラメータを取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PostmanGetGiftParam( AMX * amx, const cell * ptr )
{
  PostmanGiftType type = static_cast<PostmanGiftType>(ptr[1]);
  s32 param_id = ptr[2];

  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = pSys->GetGameManager();
  Savedata::MysteryGiftSave * pGiftSave = p_gman->GetGameData()->GetSaveData()->GetMysteryGiftSave();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  s32 ret = 0;

  switch( type ){
  case POSTMAN_GIFT_TYPE_POKEMON:
    {
      if( pGiftSave->GetPokeGiftNum() == 0 ){
        GFL_ASSERT(0);
        return 0;
      }
      pml::pokepara::PokemonParam* p_pp = GFL_NEW(p_heap) pml::pokepara::PokemonParam(p_heap );
      pGiftSave->GetTopPokeGiftData( p_pp, p_heap, false );

      switch(param_id){
      case POSTMAN_POKEMON_PARAM_MONSNO:
        if( p_pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) ){
          ret = MONSNO_TAMAGO;
        }else{
          ret = p_pp->GetMonsNo();
        }      
        break;
      case POSTMAN_POKEMON_PARAM_ITEMNO:
        ret = pGiftSave->GetTopPokeGiftDataItem(false);
        break;
      default:
        GFL_ASSERT(0);
        break;
      }
      GFL_DELETE(p_pp);

    }
    break;
  case POSTMAN_GIFT_TYPE_ITEM:
    {
      if( pGiftSave->GetItemGiftNum() == 0 ){
        GFL_ASSERT(0);
        return 0;
      }
      Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA item_data;
      pGiftSave->GetTopItemGiftData(&item_data,false);
      switch(param_id){
      case POSTMAN_ITEM_PARAM_ITEMNO:
        ret = item_data.itemNo1;
        break;
      case POSTMAN_ITEM_PARAM_ITEM_NUM:
        ret = item_data.itemNum1;
        break;
      case POSTMAN_ITEM_PARAM_KIND_NUM:
        {
          ret  = (item_data.itemNum1>0);
          ret += (item_data.itemNum2>0);
          ret += (item_data.itemNum3>0);
          ret += (item_data.itemNum4>0);
          ret += (item_data.itemNum5>0);
          ret += (item_data.itemNum6>0);
        }
        break;
      default:
        GFL_ASSERT(0);
        break;
      }
    }
    break;
  case POSTMAN_GIFT_TYPE_BP:
    {
      if( pGiftSave->GetBPGiftNum() == 0 ){
        GFL_ASSERT(0);
        return 0;
      }
      ret = pGiftSave->GetTopBPGiftData(false);
    }
    break;
  case POSTMAN_GIFT_TYPE_POKEMAME:
    {
      if( pGiftSave->GetMameGiftNum() == 0 ){
        GFL_ASSERT(0);
        return 0;
      }
      ret = pGiftSave->GetTopMameGiftData(false);
    }
    break;
  default:
    break;
  }
  return ret;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 配達員　贈り物の中身をセーブに反映し、カードを受け取り済みにする（ポケモン本体以外）
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PostmanSetGiftReceive( AMX * amx, const cell * ptr )
{
  PostmanGiftType type = static_cast<PostmanGiftType>(ptr[1]);
  bool multi_f = GFL_BOOL_CAST(ptr[2]);

  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = pSys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::MysteryGiftSave * pGiftSave = p_gdata->GetSaveData()->GetMysteryGiftSave();
  Savedata::MyItem* sv_item = p_gdata->GetMyItem();

  int count = 1;

  switch( type ){
  case POSTMAN_GIFT_TYPE_POKEMON:
    {
      //ポケモンはまとめて受け取りが無い　付属アイテムの追加処理はpostman.pで行う
      u16 itemno = pGiftSave->GetTopPokeGiftDataItem(true);
      return itemno;
    }
  case POSTMAN_GIFT_TYPE_ITEM:
    {
      Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA item_data;
      int ret = 0;
      if(multi_f){
        count = pGiftSave->GetItemGiftNum();
      }
      for( int i = 0; i < count; i++ ){
        pGiftSave->GetTopItemGiftData(&item_data,true);
        ret += sv_item->Add( item_data.itemNo1, item_data.itemNum1 );
        ret += sv_item->Add( item_data.itemNo2, item_data.itemNum2 );
        ret += sv_item->Add( item_data.itemNo3, item_data.itemNum3 );
        ret += sv_item->Add( item_data.itemNo4, item_data.itemNum4 );
        ret += sv_item->Add( item_data.itemNo5, item_data.itemNum5 );
        ret += sv_item->Add( item_data.itemNo6, item_data.itemNum6 );
      }
      return count;
    }

  case POSTMAN_GIFT_TYPE_BP:
    {
      if(multi_f){
        count = pGiftSave->GetBPGiftNum();
      }
      Savedata::Misc* p_misc = p_gdata->GetMisc();
      u32 bp = 0;
      for( int i = 0; i < count; i++ ){
        bp += pGiftSave->GetTopBPGiftData(true);
      }
      p_misc->SetBP( p_misc->GetBP()+bp ); //上限補正は中でやってくれる

      if( bp > Savedata::Misc::BP_MAX ){
        bp = Savedata::Misc::BP_MAX; //NMCat[2367] まとめて受け取る事で追加個数がカンストしたときはカンスト値を返す
      }
      return bp;
    }
    
  case POSTMAN_GIFT_TYPE_POKEMAME:
    {
      if(multi_f){
        count = pGiftSave->GetMameGiftNum();
      }
      Savedata::ResortSave* p_resort = p_gdata->GetResortSave();
      u32 mame = 0;

      for( int i = 0; i < count; i++ ){
        mame += pGiftSave->GetTopMameGiftData(true);
      }
      p_resort->SetPokeBeansNum( Savedata::ResortSave::POKEBEANS_NIJI, mame );

      if( mame > Savedata::ResortSave::POKEBEANS_KIND_NUM_MAX ){
        mame = Savedata::ResortSave::POKEBEANS_KIND_NUM_MAX; //NMCat[2367] 追加個数がカンストしたときはカンスト値を返す
      }
      return mame;
    }

  default:
    GFL_ASSERT(0);
    break;
  }
  return count;
}



/* ------------------------------------------------------------------------- */
/**
 * @brief 配達員　贈り物を受け取り済みに設定する
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::PostmanSetGiftUsedFlag( AMX * amx, const cell * ptr )
{
  PostmanGiftType type = static_cast<PostmanGiftType>(ptr[1]);
  bool multi_f = GFL_BOOL_CAST(ptr[2]);

  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = pSys->GetGameManager();
  Savedata::MysteryGiftSave * pGiftSave = p_gman->GetGameData()->GetSaveData()->GetMysteryGiftSave();
  int count = 1;

  switch( type ){
  case POSTMAN_GIFT_TYPE_POKEMON:
    {
      if(multi_f){
        count = pGiftSave->GetPokeGiftNum();
      }
      for( int i = 0; i < count; i++ ){
        (void)pGiftSave->GetTopPokeGiftDataType(true);
      }
    }
    break;
  case POSTMAN_GIFT_TYPE_ITEM:
    {
      Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA item_data;
      if(multi_f){
        count = pGiftSave->GetItemGiftNum();
      }
      for( int i = 0; i < count; i++ ){
        pGiftSave->GetTopItemGiftData(&item_data,true);
      }
    }
    break;
  case POSTMAN_GIFT_TYPE_BP:
    {
      if(multi_f){
        count = pGiftSave->GetBPGiftNum();
      }
      for( int i = 0; i < count; i++ ){
        (void)pGiftSave->GetTopBPGiftData(true);
      }
    }
    break;
  case POSTMAN_GIFT_TYPE_POKEMAME:
    {
      if(multi_f){
        count = pGiftSave->GetMameGiftNum();
      }
      for( int i = 0; i < count; i++ ){
        (void)pGiftSave->GetTopMameGiftData(true);
      }
    }
    break;
  default:
    break;
  }
  return 0;
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

