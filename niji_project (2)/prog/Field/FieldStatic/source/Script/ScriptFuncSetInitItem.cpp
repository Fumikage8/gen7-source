//=============================================================================
/**
 * @file  ScriptFuncSetInitItem.cpp
 * @brief 初期化スクリプト用命令セット：アイテム系
 * @date  2012.10.21
 */
//=============================================================================

#include <heap/include/gfl2_Heap.h>

#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "SaveData/include/MyItemSave.h"
#include "SaveData/include/MyItemSaveDef.h"
#include "SaveData/include/ResortSave.h"

#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptWork.h"

#include "Field/FieldStatic/include/script/ScriptFuncSetInit.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

//-----------------------------------------------------------------------------
/**
 *  @brief  アイテムを加える
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::ItemAdd( AMX * amx, const cell * ptr )
{
  u16 item_id = ptr[1];
  u16 num = ptr[2];
  
  FieldScriptSystem* p_this = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_this->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::MyItem* p_item = p_gdata->GetMyItem();
  
  return p_item->Add( item_id, num );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  アイテムを減らす
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::ItemSub( AMX * amx, const cell * ptr )
{
  u16 item_id = ptr[1];
  u16 num = ptr[2];
  
  FieldScriptSystem* p_this = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_this->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::MyItem* p_item = p_gdata->GetMyItem();
  
  return p_item->Sub( item_id, num);
}

//-----------------------------------------------------------------------------
/**
 *  @brief  アイテムの所持数を取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::ItemGetNum( AMX * amx, const cell * ptr )
{
  u16 item_id = ptr[1];
  //u16 num = ptr[2];
  
  FieldScriptSystem* p_this = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_this->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::MyItem* p_item = p_gdata->GetMyItem();
  
  return p_item->GetItemNum( item_id );
}



//-----------------------------------------------------------------------------
/**
 *  @brief  アイテムを加えられるかチェック
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::ItemAddCheck( AMX * amx, const cell * ptr )
{
  u16 item_id = ptr[1];
  u16 num = ptr[2];
  
  FieldScriptSystem* p_this = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_this->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::MyItem* p_item = p_gdata->GetMyItem();
  
  return p_item->AddCheck( item_id, num );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  アイテムが技マシンか判定
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::ItemIsWazaMachine( AMX * amx, const cell * ptr )
{
  u16 item_no = ptr[1];
  return item::ITEM_CheckWazaMachine( item_no );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief アイテムがメガストーンか判定
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ItemIsMegaStone( AMX * amx, const cell * ptr )
{
  u16 item_no = ptr[1];
  return item::ITEM_CheckMegaStone( item_no );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief アイテムのカテゴリを判定
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ItemGetCategory( AMX * amx, const cell * ptr )
{
  u16 item_no = ptr[1];

  if( item::ITEM_CheckWazaMachine( item_no ) ){
    return SCR_ITEM_CATEGORY_WAZA_MASHINE;
  }
  if( item::ITEM_CheckNuts( item_no ) ){
    return SCR_ITEM_CATEGORY_KINOMI;
  }
  if( item::ITEM_CheckMegaStone( item_no ) ){
    return SCR_ITEM_CATEGORY_MEGA_STONE;
  }
  if( item::ITEM_CheckJewel( item_no ) ){
    return SCR_ITEM_CATEGORY_JEWEL;
  }
  if( item::ITEM_CheckPiece( item_no ) ){
    return SCR_ITEM_CATEGORY_Z_PIECE;
  }
  if( item::ITEM_CheckBeads( item_no ) ){
    return SCR_ITEM_CATEGORY_Z_BEADS;
  }
  return SCR_ITEM_CATEGORY_NORMAL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief きのみIDからアイテム番号を取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ItemGetNutsID( AMX *amx, const cell * ptr )
{
  u16 nuts_id = ptr[1];

  return item::ITEM_GetNutsID( nuts_id );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ボールIDからアイテム番号を取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetInit::ItemBallID2ItemID( AMX *amx, const cell * ptr )
{
  item::BALL_ID ball_id = static_cast<item::BALL_ID>( ptr[1] );
  return item::ITEM_BallID2ItemID( ball_id );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ポケットIDを取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PocketGetID( AMX * amx, const cell * ptr )
{
  u16 item_no = ptr[1];

  //#GFNMCat[1412] 道具入手時メッセージをフリースペースポケットに対応
  FieldScriptSystem* p_this = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_this->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::MyItem* p_item = p_gdata->GetMyItem();

  if( p_item->IsRegisterFreeSpace(item_no) == false )
  {
    return item::ITEM_DATA::GetParam( item_no, item::ITEM_DATA::PRM_ID_F_POCKET );
  }
  else
  {
    return BAG_POCKET_FREE;
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  指定ポケットのアイテム総所持数を取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PocketGetTotalCount( AMX * amx, const cell * ptr )
{
  u16 pocket_no = ptr[1];
  
  FieldScriptSystem* p_this = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_this->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::MyItem* p_item = p_gdata->GetMyItem();
  
  return p_item->GetItemTotalCount( pocket_no );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  着せ替えアイテムの追加と削除
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::DressupItemAddDel_( AMX * amx, const cell * ptr )
{
  s32 category = ptr[1];
  s32 item_m = ptr[2];
  s32 item_f = ptr[3];
  bool add_f = ptr[4];
  
  FieldScriptSystem* p_this = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_this->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::Fashion* pFashionSaveData = p_gdata->GetFashion();
  Savedata::Fashion::Category type = Savedata::Fashion::CATEGORY_TOPS;

  switch(category){
  case SCR_DRESSUP_ITEM_CATEGORY_TOPS:
    type = Savedata::Fashion::CATEGORY_TOPS; // トップス
    break;
  case SCR_DRESSUP_ITEM_CATEGORY_BOTTOMS:
    type = Savedata::Fashion::CATEGORY_BOTTOMS; // ボトムス
    break;
  case SCR_DRESSUP_ITEM_CATEGORY_LEGS:
    type = Savedata::Fashion::CATEGORY_LEGS; // レッグ
    break;
  case SCR_DRESSUP_ITEM_CATEGORY_SHOES:
    type = Savedata::Fashion::CATEGORY_SHOES; // シューズ
    break;
  case SCR_DRESSUP_ITEM_CATEGORY_BAG:
    type = Savedata::Fashion::CATEGORY_BAG; // バッグ
    break;
  case SCR_DRESSUP_ITEM_CATEGORY_HAT:
    type = Savedata::Fashion::CATEGORY_HAT; // 帽子
    break;
  case SCR_DRESSUP_ITEM_CATEGORY_ACCEYE:
    type = Savedata::Fashion::CATEGORY_ACCEYE; // めがね
    break;
  case SCR_DRESSUP_ITEM_CATEGORY_ACCHAIR:
    type = Savedata::Fashion::CATEGORY_ACCHAIR; // かみかざり
    break;
  default:
    return false;
  }
  if( p_gdata->GetPlayerStatus()->GetSex() == PM_FEMALE )
  {
    if( add_f ){
      pFashionSaveData->Add( type, item_f );
    }else{
      pFashionSaveData->Del( type, item_f );    
    }
  }
  else
  {
    if( add_f ){
      pFashionSaveData->Add( type, item_m );
    }else{
      pFashionSaveData->Del( type, item_m );    
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
*	@brief  ポケマメの加減算
*
*	@param	type　ポケマメの種類
*	@param	num　 増減数
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokeBeansAddSub( AMX * amx, const cell * ptr )
{
  //Param
  Savedata::ResortSave::PokeBeansEnum type = static_cast<Savedata::ResortSave::PokeBeansEnum>(ptr[1]);
  s32 num  = ptr[2];

  //Alias
  FieldScriptSystem* p_this = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_this->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::ResortSave* p_resort = p_gdata->GetResortSave();

  p_resort->SetPokeBeansNum(type, num);

  return 0;
}

//-----------------------------------------------------------------------------
/**
*	@brief  ポケマメの数を取得
*
*	@param	type　ポケマメの種類
*
*	@return 総所持数
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetInit::PokeBeansNumGet( AMX * amx, const cell * ptr )
{
  //Param
  Savedata::ResortSave::PokeBeansEnum type = static_cast<Savedata::ResortSave::PokeBeansEnum>(ptr[1]);

  //Alias
  FieldScriptSystem* p_this = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_this->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::ResortSave* p_resort = p_gdata->GetResortSave();

  return p_resort->GetPokeBeansNum(type);
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

