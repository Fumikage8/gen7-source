//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		FieldItemUse.cpp
 *	@brief  フィールドアイテム私用チェック
 *	@author	Masayuki Onoue
 *	@date		2012.12.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldStatic/include/item/FieldItemUse.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEventManager.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"

#include "FieldScript/include/EventScriptCall.h"

#include  "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include  "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

#include "niji_conv_header/field/script/inc/field_item.inc"
#include "niji_conv_header/field/FieldGimmickTypes.h"

// ロトムパワー
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"

namespace Field {

// ===========================================================================
// ===========================================================================
// 
// @note 以下の構造体にテーブルチェック関数、使用関数、ナンバー変換を登録する
// 
//  ITEM_USE_CHECK_NUMを増やすのも忘れずに
// 
// ===========================================================================
// ===========================================================================
const FieldItemUse::ItemUseCheckTable FieldItemUse::sc_item_use_check_tbl[FieldItemUse::ITEM_USE_CHECK_NUM] =
{
  //学習装置
  //{ FLDITEM_IDX_GAKUSYUUSOUTI, ITEM_GAKUSYUUSOUTI, ItemUse_Gakusyuusouti, ItemCheck_Gakusyuusouti },
  //自転車
  //{ FLDITEM_IDX_ZITENSYA_MACH, ITEM_MAHHAZITENSYA, ItemUse_Zitensya, ItemCheck_ZitensyaMach },
  //{ FLDITEM_IDX_ZITENSYA_DIRT, ITEM_DAATOZITENSYA, ItemUse_Zitensya, ItemCheck_ZitensyaDirt },
  //ダウジングマシン
  //{ FLDITEM_IDX_DAUZINGUMASIN, ITEM_DAUZINGUMASIN, ItemUse_Dauzingumasin, ItemCheck_Dauzingumasin },
  //あまいみつ
  { FLDITEM_IDX_AMAIMITU, ITEM_AMAIMITU, ItemUse_Amaimitu, ItemCheck_Amaimitu },
  //バトルレコーダー
  //{ FLDITEM_IDX_BATORUREKOODAA, ITEM_BATORUREKOODAA, ItemUse_Batorurekoodaa, ItemCheck_Batorurekoodaa },
  // 穴ぬけの紐
  { FLDITEM_IDX_ANANUKENOHIMO, ITEM_ANANUKENOHIMO, ItemUse_Ananukenohimo, ItemCheck_Ananukenohimo },
  // ぼろの釣竿
  //{ FLDITEM_IDX_TURIZAO, ITEM_BORONOTURIZAO, ItemUse_Turizao, ItemCheck_Turizao },
  // いい釣竿
  //{ FLDITEM_IDX_TURIZAO, ITEM_IITURIZAO, ItemUse_Turizao, ItemCheck_Turizao },
  // すごい釣竿
  //{ FLDITEM_IDX_TURIZAO, ITEM_SUGOITURIZAO, ItemUse_Turizao, ItemCheck_Turizao },

  // タマゴふかポン
  { FLDITEM_IDX_TAMAGOHUKAPON, ITEM_TAMAGOHUKAPON, ItemUse_Rotopon, ItemCheck_TamagohukaPon },
  // やすうりポン
  { FLDITEM_IDX_YASUURIPON, ITEM_YASUURIPON, ItemUse_Rotopon, ItemCheck_YasuuriPon },
  // おこづかいポン
  { FLDITEM_IDX_OKODUKAIPON, ITEM_OKODUKAIPON, ItemUse_Rotopon, ItemCheck_OkodukaiPon },
  // けいけんちポン
  { FLDITEM_IDX_KEIKENTIPON, ITEM_KEIKENTIPON, ItemUse_Rotopon, ItemCheck_KeikentiPon },
  // なつきポン
  { FLDITEM_IDX_NATUKIPON, ITEM_NATUKIPON, ItemUse_Rotopon, ItemCheck_NatukiPon },
  // そうぐうポン
  { FLDITEM_IDX_SOUGUUPON, ITEM_SOUGUUPON, ItemUse_Rotopon, ItemCheck_SouguuPon },
  // おんみつポン
  { FLDITEM_IDX_ONMITUPON, ITEM_ONMITUPON, ItemUse_Rotopon, ItemCheck_OnmituPon },
};

/* -------------------------------------------------------------------------*/
/**
 * @brief ワークを初期化する
 *
 * @param p_fieldmap    フィールドマップ
 * @param use_wk        チェック結果を格納するワーク
 */
/* -------------------------------------------------------------------------*/
void FieldItemUse::InitializeWork( Fieldmap* p_fieldmap,ITEM_USE_CHECK_WORK& use_wk )
{
  gfl2::std::MemClear( &use_wk, sizeof(ITEM_USE_CHECK_WORK) );

  // ゾーンIDの取得
  use_wk.zone_id = p_fieldmap->GetZoneID();
  use_wk.p_gdata = p_fieldmap->GetGameManager()->GetGameData();

  //---------------------------
  /// 常に使える
  // 学習装置
  //SetEnableItem(use_wk,FLDITEM_IDX_GAKUSYUUSOUTI);
  
  // バトルレコーダー @note 今回はバトルレコーダー内で内部セーブしないので、セーブしちゃだめな箇所でも常に呼ばれてよい
  //SetEnableItem(use_wk,FLDITEM_IDX_BATORUREKOODAA);

  // 釣竿
  /*
  if( p_player->IsFishingEnable( p_player->GetDir() ) )
  {
    SetEnableItem(use_wk,FLDITEM_IDX_TURIZAO);
  }
  else if( p_player->IsFishingEnableForward( p_player->GetDir() ) )
  {
    SetEnableItem(use_wk,FLDITEM_IDX_TURIZAO);
  }
  */

  //---------------------------
  ///フォルムしばり
  
  //---------------------------
  /// ゾーンデータ関連 
  const ZoneDataLoader* p_zone_loader = use_wk.p_gdata->GetFieldZoneDataLoader();
  ZoneDataAccessor zone_accessor( p_zone_loader->GetZoneData( use_wk.zone_id ) );

  // 穴ぬけの紐
  // @fix GFSBTS[1150]:椅子に座った状態で「あなぬけのひも」を使用すると演出で椅子にめり込む、対処。椅子に座っている場合は使用を禁止する
  if( zone_accessor.IsEscapeEnable() )
  {
    SetEnableItem(use_wk,FLDITEM_IDX_ANANUKENOHIMO);
  }

  //ユニオン/コロシアムなどではアイテム全面禁止
  //@note 通信の同期が取れなくなるため
  /*
  if( zone_accessor.CheckFieldSkillUse() == false )
  {
    use_wk.enable_item = 0;
  }
  else
  */
  {
    // 甘いみつ
    SetEnableItem(use_wk,FLDITEM_IDX_AMAIMITU);
  }

  {
    // ロトポン関連
    // フェスサークル上では使用不可
    // バトルフィールドは下画面ロックされているので見ない
    bool isExecuteJoinFesta = false;
    for (u32 i = 0; i < Fieldmap::GIMMICK_IDX_MAX; ++i)
    {
      u32 gimmickID = p_fieldmap->GetFieldGimmickID(static_cast<Fieldmap::GimmickIndex>(i));
      if (gimmickID == GIMMICK_TYPE_JOIN_FESTA || gimmickID == GIMMICK_TYPE_JOIN_FESTA_ROOM)
      {
        isExecuteJoinFesta = true;
        break;
      }
    }

    if (!isExecuteJoinFesta)
    {
      // 既に使用中のロトポンは使用不可にする
      RotomPower::RotomPowerManager *p_rotom_power_manager = p_fieldmap->GetGameManager()->GetRotomPowerManager();
      u8 disable_item_index = 0;
      // FLDITEM_IDX_とRotomPowerIDの順番が一致している前提
      if (p_rotom_power_manager->IsRotomPower())
      {
        disable_item_index = FLDITEM_IDX_TAMAGOHUKAPON + p_rotom_power_manager->GetRotomPowerID() - 1;
      }
      for (u8 flditem_index(FLDITEM_IDX_TAMAGOHUKAPON); flditem_index < FLDITEM_IDX_MAX; ++flditem_index)
      {
        if (disable_item_index != flditem_index)
        {
          SetEnableItem(use_wk, flditem_index);
        }
      }
    }
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief スキルの使用状況をセットする
 *
 * @param use_wk        ワーク
 * @param item_index   アイテムインデックス
 */
/* -------------------------------------------------------------------------*/
void FieldItemUse::SetEnableItem( ITEM_USE_CHECK_WORK& use_wk ,u8 item_index )
{
  use_wk.enable_item |= (1<<(item_index));
}

/* -------------------------------------------------------------------------*/
/**
 * @brief アイテムが使用できるかチェック
 *
 * @param use_wk        ワーク
 * @param idx   アイテムインデックス
 *
 * @return trueの時使用できる
 */
/* -------------------------------------------------------------------------*/
bool FieldItemUse::IsItemEnable( const ITEM_USE_CHECK_WORK& use_wk ,FLDITEM_IDX idx )
{
  return GFL_BOOL_CAST( use_wk.enable_item & (1<<(idx)) );
}

// ===========================================================================
// ===========================================================================
// ===========================================================================
/* -------------------------------------------------------------------------*/
/**
 * @brief ホログラムツイッター使用チェック
 *
 * @param use_wk  チェック用ワーク
 *
 * @return 使用チェックの戻り値
 */
/* -------------------------------------------------------------------------*/
/*
FieldItemUse::FLDITEM_RET FieldItemUse::ItemCheck_HoroguramuT( const ITEM_USE_CHECK_WORK& use_wk )
{
  // 使用チェック
  if( IsItemEnable(use_wk,FLDITEM_IDX_HOROGURAMUt) ){
    return FLDITEM_RET_USE_OK;
  }

  return FLDITEM_RET_USE_NG;
}
*/

/* -------------------------------------------------------------------------*/
/**
 * @brief ホログラムツイッター実行
 */
/* -------------------------------------------------------------------------*/
/*
void FieldItemUse::ItemUse_HoroguramuT( const ITEM_USE_WORK* p_wk )
{
  if( IsItemEnable(p_wk->check_wk,FLDITEM_IDX_HOROGURAMUt) ){

    EventAppCall* event = GAMESYSTEM_EVENT_CALL<gamesystem::GameEventManager, EventAppCall > ( p_wk->p_evman );
    app::hologrammail::Proc::PARAM * param = static_cast<app::hologrammail::Proc::PARAM *>( event->SetCallTargetApplicationAndAllocParam( EventAppCall::CALL_TARGET_HOLOGRAM_MAIL, sizeof(app::hologrammail::Proc::PARAM) ) );
    param->mode = app::hologrammail::Proc::MODE_USE_ITEM;
    xy_system::bg::GetComponentData( p_wk->p_fieldmap, p_wk->p_zone_loader, &param->bg_component_data );
  }else{
    EventScriptCall::CallScript( p_wk->p_fieldmap->GetGameManager(), SCRID_FIELD_ITEM_USE_ERROR );
  }
}
*/

/* -------------------------------------------------------------------------*/
/**
 * @brief タウンマップ使用チェック
 *
 * @param use_wk  チェック用ワーク
 *
 * @return 使用チェックの戻り値
 */
/* -------------------------------------------------------------------------*/
/*
FieldItemUse::FLDITEM_RET FieldItemUse::ItemCheck_Taunmappu( const ITEM_USE_CHECK_WORK& use_wk )
{
  // 使用チェック
  if( IsItemEnable(use_wk,FLDITEM_IDX_TAUNMAPPU) ){
    return FLDITEM_RET_USE_OK;
  }

  return FLDITEM_RET_USE_NG;
}
*/

/* -------------------------------------------------------------------------*/
/**
 * @brief タウンマップ実行
 */
/* -------------------------------------------------------------------------*/
/*
void FieldItemUse::ItemUse_Taunmappu( const ITEM_USE_WORK* p_wk )
{
  if( IsItemEnable(p_wk->check_wk,FLDITEM_IDX_TAUNMAPPU) )
  {
    EventAppCall* event = GAMESYSTEM_EVENT_CALL<gamesystem::GameEventManager, EventAppCall > ( p_wk->p_evman );
    app::townmap::APP_PARAM * param = static_cast<app::townmap::APP_PARAM *>( event->SetCallTargetApplicationAndAllocParam( EventAppCall::CALL_TARGET_TOWNMAP, sizeof(app::townmap::APP_PARAM) ) );
    param->m_startMode = app::townmap::START_MODE_TOWNMAP;
  }else{
    EventScriptCall::CallScript( p_wk->p_fieldmap->GetGameManager(), SCRID_FIELD_ITEM_USE_ERROR );
  }
}
*/

/* -------------------------------------------------------------------------*/
/**
 * @brief あまいみつ使用チェック
 *
 * @param use_wk  チェック用ワーク
 *
 * @return 使用チェックの戻り値
 */
/* -------------------------------------------------------------------------*/
FieldItemUse::FLDITEM_RET FieldItemUse::ItemCheck_Amaimitu( const ITEM_USE_CHECK_WORK& use_wk )
{
  // 使用チェック
  if( IsItemEnable(use_wk,FLDITEM_IDX_AMAIMITU) ){
    return FLDITEM_RET_USE_OK;
  }

  return FLDITEM_RET_USE_NG;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief あまいみつ実行
 */
/* -------------------------------------------------------------------------*/
void FieldItemUse::ItemUse_Amaimitu( const ITEM_USE_WORK* p_wk )
{
  if( IsItemEnable(p_wk->check_wk,FLDITEM_IDX_AMAIMITU) )
  {
    EventScriptCall::CallScript( p_wk->p_fieldmap->GetGameManager(), SCRID_FIELD_ITEM_AMAIMITSU, NULL );
  }else{
    GFL_ASSERT(0); //,"甘い蜜が使用不可なパターンはバッグ側でメッセージを出す\n");
    //EventScriptCall::CallScript( p_wk->p_fieldmap->GetGameManager(), SCRID_FIELD_ITEM_USE_ERROR );
  }
}


/* -------------------------------------------------------------------------*/
/**
 * @brief 穴ぬけの紐使用チェック
 *
 * @param use_wk  チェック用ワーク
 *
 * @return 使用チェックの戻り値
 */
/* -------------------------------------------------------------------------*/
FieldItemUse::FLDITEM_RET FieldItemUse::ItemCheck_Ananukenohimo( const ITEM_USE_CHECK_WORK& use_wk )
{
  // 使用チェック
  if( IsItemEnable(use_wk,FLDITEM_IDX_ANANUKENOHIMO) ){
    return FLDITEM_RET_USE_OK;
  }

  return FLDITEM_RET_USE_NG;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 穴ぬけの紐実行
 */
/* -------------------------------------------------------------------------*/
void FieldItemUse::ItemUse_Ananukenohimo( const ITEM_USE_WORK* p_wk )
{
  if( IsItemEnable(p_wk->check_wk,FLDITEM_IDX_ANANUKENOHIMO) )
  {
    EventScriptCall::CallScript( p_wk->p_fieldmap->GetGameManager(), SCRID_FIELD_ITEM_ANANUKENOHIMO, NULL );
  }else{
    GFL_ASSERT(0); //,"甘い蜜が使用不可なパターンはバッグ側でメッセージを出す\n");
    //EventScriptCall::CallScript( p_wk->p_fieldmap->GetGameManager(), SCRID_FIELD_ITEM_USE_ERROR );
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 冒険の心得　使用チェック
 *
 * @param use_wk  チェック用ワーク
 *
 * @return 使用チェックの戻り値
 */
/* -------------------------------------------------------------------------*/
/*
FieldItemUse::FLDITEM_RET FieldItemUse::ItemCheck_BoukenNoKokoroe( const ITEM_USE_CHECK_WORK& use_wk )
{
  // 使用チェック
  if( IsItemEnable(use_wk,FLDITEM_IDX_BOUKENNNOKOKOROE) ){
    return FLDITEM_RET_USE_OK;
  }

  return FLDITEM_RET_USE_NG;
}
*/

/* -------------------------------------------------------------------------*/
/**
 * @brief 冒険の心得　実行
 */
/* -------------------------------------------------------------------------*/
/*
void FieldItemUse::ItemUse_BoukenNoKokoroe( const ITEM_USE_WORK* p_wk )
{
  if( IsItemEnable(p_wk->check_wk,FLDITEM_IDX_BOUKENNNOKOKOROE) )
  {
    EventScriptCall::CallScript( p_wk->p_fieldmap->GetGameManager(), SCRID_FIELD_EVENT_ADVENTURE_KNOWLEDGE );
  }
}
*/


/* -------------------------------------------------------------------------*/
/**
 * @brief スキルの使用可能状況を
 *
 * @param use_wk  チェックワーク
 * @param idx     スキルインデックス
 *
 * @return  スキル使用可能状況
 */
/* -------------------------------------------------------------------------*/
FieldItemUse::FLDITEM_RET FieldItemUse::ItemUseCheckByIndex( const ITEM_USE_CHECK_WORK& use_wk ,FLDITEM_IDX idx )
{
  ITEM_CHECK func = GetCheckFunc( idx );

  // チェック関数を実行
  if( func )
  {
    return func(use_wk);
  }
  return FLDITEM_RET_USE_NG;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief スキルの使用可能状況を
 *
 * @param use_wk    チェックワーク
 * @param item_no   アイテム番号
 *
 * @return  アイテム使用可能状況
 */
/* -------------------------------------------------------------------------*/
FieldItemUse::FLDITEM_RET FieldItemUse::ItemUseCheck( const ITEM_USE_CHECK_WORK& use_wk ,u32 item_no )
{
  // アイテムインデックスに変換してからチェック
  return ItemUseCheckByIndex( use_wk, GetItemIndexByItemNo( item_no ) );
}

/* -------------------------------------------------------------------------*/
/**
 * @brief スキルを呼び出す
 *
 * @param p_wk  スキル使用ワーク
 * @param idx   呼び出したいスキルインデックス
 *
 * @return 実行できた場合はtrue
 */
/* -------------------------------------------------------------------------*/
bool FieldItemUse::CallItemByIndex( const ITEM_USE_WORK* p_wk, FLDITEM_IDX idx )
{
  // 関数を実行
  ITEM_USE func = GetItemFunc( idx );
  if( func )
  {
    func(p_wk);
    return true;
  }
  return false;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief アイテムを呼び出す
 *
 * @param p_game_manager  ゲームマネージャ
 * @param item_no         呼び出したいアイテム番号
 *
 * @return 実行できた場合はtrue
 */
/* -------------------------------------------------------------------------*/
bool FieldItemUse::CallItem( GameSys::GameManager* p_game_manager, u32 item_no )
{
  // ワークを初期化
  ITEM_USE_WORK wk;
  wk.item_no = item_no;
  wk.p_fieldmap = p_game_manager->GetFieldmap();
  wk.p_evman  = p_game_manager->GetGameEventManager();

  // 使用可能状態を取得しておく
  InitializeWork( wk.p_fieldmap ,wk.check_wk );

#if 0
  // @fix BTS[4816]:Xメニューからイベントが呼び出された場合に、下画面のロックがかからない問題を修正
  subscreen::FieldMenu* p_menu = wk.p_fieldmap->GetFieldMenu();
  if( p_menu ){
    // 下画面コントロールをカット
    if( p_menu->GetMenuControlFlag() )
    {
      p_menu->SetMenuControlFlag(false);
    }
    // マスクを設定する
    if( !p_menu->GetVisibleLowerMask() )
    {
      p_menu->SetVisibleLowerMask(true);
    }
  }
#endif

  // アイテム実行
  return CallItemByIndex( &wk ,GetItemIndexByItemNo( item_no ) );
}

/* -------------------------------------------------------------------------*/
/**
 * @brief アイテム番号からアイテムインデックスを取得する
 *
 * @param item_no     アイテム番号
 *
 * @return スキルインデックス
 */
/* -------------------------------------------------------------------------*/
FieldItemUse::FLDITEM_IDX FieldItemUse::GetItemIndexByItemNo( u32 item_no )
{
  // 検索
  for( int i = 0; i < ITEM_USE_CHECK_NUM; ++i )
  {
    if( sc_item_use_check_tbl[i].item_no == item_no )
    {
      return sc_item_use_check_tbl[i].idx;
    }
  }

  return FLDITEM_IDX_MAX; 
}

/* -------------------------------------------------------------------------*/
/**
 * @brief アイテム使用関数を取得する
 *
 * @param idx アイテムインデックス
 *
 * @returnアイテム呼び出し関数
 */
/* -------------------------------------------------------------------------*/
FieldItemUse::ITEM_USE FieldItemUse::GetItemFunc( FLDITEM_IDX idx )
{
  for( int i = 0; i < ITEM_USE_CHECK_NUM; ++i )
  {
    if( sc_item_use_check_tbl[i].idx == idx )
    {
      return sc_item_use_check_tbl[i].item_use_func;
    }
  }
  return NULL;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief スキルチェック関数を取得する
 *
 * @param idx スキルインデックス
 *
 * @return チェック関数
 */
/* -------------------------------------------------------------------------*/
FieldItemUse::ITEM_CHECK FieldItemUse::GetCheckFunc( FLDITEM_IDX idx )
{
  // テーブルの範囲チェック
  if( idx >= FLDITEM_IDX_MAX ) {
    GFL_ASSERT( idx < FLDITEM_IDX_MAX );//@check
    return NULL;
  }
  
  for( int i = 0; i < ITEM_USE_CHECK_NUM; ++i )
  {
    if( sc_item_use_check_tbl[i].idx == idx )
    {
      return sc_item_use_check_tbl[i].item_check_func;
    }
  }

  GFL_ASSERT(0);
  return NULL;
}

/* -------------------------------------------------------------------------*/
/**
* @brief たまごふかポン使用チェック
*
* @param use_wk  チェック用ワーク
*
* @return 使用チェックの戻り値
*/
/* -------------------------------------------------------------------------*/
FieldItemUse::FLDITEM_RET FieldItemUse::ItemCheck_TamagohukaPon(const ITEM_USE_CHECK_WORK& use_wk)
{
  // 使用チェック
  if (IsItemEnable(use_wk, FLDITEM_IDX_TAMAGOHUKAPON)){
    return FLDITEM_RET_USE_OK;
  }

  return FLDITEM_RET_USE_NG;
}

/* -------------------------------------------------------------------------*/
/**
* @brief やすうりポン使用チェック
*
* @param use_wk  チェック用ワーク
*
* @return 使用チェックの戻り値
*/
/* -------------------------------------------------------------------------*/
FieldItemUse::FLDITEM_RET FieldItemUse::ItemCheck_YasuuriPon(const ITEM_USE_CHECK_WORK& use_wk)
{
  // 使用チェック
  if (IsItemEnable(use_wk, FLDITEM_IDX_YASUURIPON)){
    return FLDITEM_RET_USE_OK;
  }

  return FLDITEM_RET_USE_NG;
}

/* -------------------------------------------------------------------------*/
/**
* @brief おこづかいポン使用チェック
*
* @param use_wk  チェック用ワーク
*
* @return 使用チェックの戻り値
*/
/* -------------------------------------------------------------------------*/
FieldItemUse::FLDITEM_RET FieldItemUse::ItemCheck_OkodukaiPon(const ITEM_USE_CHECK_WORK& use_wk)
{
  // 使用チェック
  if (IsItemEnable(use_wk, FLDITEM_IDX_OKODUKAIPON)){
    return FLDITEM_RET_USE_OK;
  }

  return FLDITEM_RET_USE_NG;
}

/* -------------------------------------------------------------------------*/
/**
* @brief けいけんちポン使用チェック
*
* @param use_wk  チェック用ワーク
*
* @return 使用チェックの戻り値
*/
/* -------------------------------------------------------------------------*/
FieldItemUse::FLDITEM_RET FieldItemUse::ItemCheck_KeikentiPon(const ITEM_USE_CHECK_WORK& use_wk)
{
  // 使用チェック
  if (IsItemEnable(use_wk, FLDITEM_IDX_KEIKENTIPON)){
    return FLDITEM_RET_USE_OK;
  }

  return FLDITEM_RET_USE_NG;
}

/* -------------------------------------------------------------------------*/
/**
* @brief なつきポン使用チェック
*
* @param use_wk  チェック用ワーク
*
* @return 使用チェックの戻り値
*/
/* -------------------------------------------------------------------------*/
FieldItemUse::FLDITEM_RET FieldItemUse::ItemCheck_NatukiPon(const ITEM_USE_CHECK_WORK& use_wk)
{
  // 使用チェック
  if (IsItemEnable(use_wk, FLDITEM_IDX_NATUKIPON)){
    return FLDITEM_RET_USE_OK;
  }

  return FLDITEM_RET_USE_NG;
}

/* -------------------------------------------------------------------------*/
/**
* @brief そうぐうポン使用チェック
*
* @param use_wk  チェック用ワーク
*
* @return 使用チェックの戻り値
*/
/* -------------------------------------------------------------------------*/
FieldItemUse::FLDITEM_RET FieldItemUse::ItemCheck_SouguuPon(const ITEM_USE_CHECK_WORK& use_wk)
{
  // 使用チェック
  if (IsItemEnable(use_wk, FLDITEM_IDX_SOUGUUPON)){
    return FLDITEM_RET_USE_OK;
  }

  return FLDITEM_RET_USE_NG;
}

/* -------------------------------------------------------------------------*/
/**
* @brief おんみつポン使用チェック
*
* @param use_wk  チェック用ワーク
*
* @return 使用チェックの戻り値
*/
/* -------------------------------------------------------------------------*/
FieldItemUse::FLDITEM_RET FieldItemUse::ItemCheck_OnmituPon(const ITEM_USE_CHECK_WORK& use_wk)
{
  // 使用チェック
  if (IsItemEnable(use_wk, FLDITEM_IDX_ONMITUPON)){
    return FLDITEM_RET_USE_OK;
  }

  return FLDITEM_RET_USE_NG;
}

/* -------------------------------------------------------------------------*/
/**
* @brief ロトポン実行
*/
/* -------------------------------------------------------------------------*/
void FieldItemUse::ItemUse_Rotopon( const ITEM_USE_WORK* p_wk )
{
  // 使用できない場合はバッグで対処しているのでここまで来たら必ず実行
  EventScriptCall::CallScript(p_wk->p_fieldmap->GetGameManager(), SCRID_FIELD_ITEM_ROTOPON, NULL, NULL, p_wk->item_no);
}



} // namespace field

