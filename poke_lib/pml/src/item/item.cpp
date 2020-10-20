//============================================================================================
/**
 * @file    item.cpp
 * @brief   アイテムデータ処理
 * @author  Hiroyuki Nakamura
 * @date    11.05.23
 *
 * class    item::ITEM_DATA::
 */
//============================================================================================
#include <debug/include/gfl2_Assert.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>

#include "pml/include/system/pml_Library.h"
#include "pml/include/item/item.h"

#include <niji_conv_header/poke_lib/wazano_def.h>


namespace item {


// 手持ちポケモンに使用するアイテム
typedef struct {
  u8  sleep_rcv:1;        // 眠り回復
  u8  poison_rcv:1;       // 毒回復
  u8  burn_rcv:1;         // 火傷回復
  u8  ice_rcv:1;          // 氷回復
  u8  paralyze_rcv:1;     // 麻痺回復
  u8  panic_rcv:1;        // 混乱回復
  u8  meromero_rcv:1;     // メロメロ回復
  u8  ability_guard:1;    // 能力ガード

  u8  death_rcv:1;        // 瀕死回復
  u8  alldeath_rcv:1;     // 全員瀕死回復
  u8  lv_up:1;            // レベルアップ
  u8  evolution:1;        // 進化
  u8  atc_up:4;           // 攻撃力アップ

  u8  def_up:4;           // 防御力アップ
  u8  spa_up:4;           // 特攻アップ

  u8  spd_up:4;           // 特防アップ
  u8  agi_up:4;           // 素早さアップ

  u8  hit_up:4;           // 命中率アップ
  u8  critical_up:2;      // クリティカル率アップ
  u8  pp_up:1;            // PPアップ
  u8  pp_3up:1;           // PPアップ（３段階）

  u8  pp_rcv:1;           // PP回復
  u8  allpp_rcv:1;        // PP回復（全ての技）
  u8  hp_rcv:1;           // HP回復
  u8  hp_exp:1;           // HP努力値アップ
  u8  pow_exp:1;          // 攻撃努力値アップ
  u8  def_exp:1;          // 防御努力値アップ
  u8  agi_exp:1;          // 素早さ努力値アップ
  u8  spa_exp:1;          // 特攻努力値アップ

  u8  spd_exp:1;          // 特防努力値アップ
  u8  exp_limit:1;        // 努力値限界制御
  u8  friend_exp1:1;      // なつき度１
  u8  friend_exp2:1;      // なつき度２
  u8  friend_exp3:1;      // なつき度３

  u8  dmy_bit:3;          // ダミー（余り）

  s8  prm_hp_exp;         // HP努力値
  s8  prm_pow_exp;        // 攻撃努力値
  s8  prm_def_exp;        // 防御努力値
  s8  prm_agi_exp;        // 素早さ努力値
  s8  prm_spa_exp;        // 特攻努力値
  s8  prm_spd_exp;        // 特防努力値
  u8  prm_hp_rcv;         // HP回復値
  u8  prm_pp_rcv;         // pp回復値
  s8  prm_friend1;        // なつき度1
  s8  prm_friend2;        // なつき度2
  s8  prm_friend3;        // なつき度3

  u8  dmy_buf[2];         // ダミー（余り）
}ITEMWORK_RCV;


static int GetWorkRecoverItem( ITEM_DATA::ITEM_PRM_ID prmID, ITEMWORK_RCV * wk );


//============================================================================================
//	グローバル変数
//============================================================================================
static u8 * ItemParamFieldPocket = NULL;		// フィールドポケットデータ展開場所

#include "niji_conv_header/poke_lib/item/itemtype.cdat"   // 各分類テーブル
#include "niji_conv_header/poke_lib/item/waza_mcn.cdat"   // 技マシンで覚えられる技テーブル


//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ（空データ）
 *
 * @param   clearFlag   true = データクリア
 */
//--------------------------------------------------------------------------------------------
ITEM_DATA::ITEM_DATA( bool clearFlag ) :
  id( ITEM_DUMMY_DATA )
{
  if( clearFlag == true ){
    gfl2::std::MemClear( &prm, sizeof(ITEM_PRM) );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ（データ作成+）
 *
 * @param   itemID    アイテム番号
 * @param   pHeap     ヒープ
 */
//--------------------------------------------------------------------------------------------
ITEM_DATA::ITEM_DATA( int itemID, gfl2::heap::HeapBase * pHeap )
{
  Create( itemID, pHeap );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------------------------------------
ITEM_DATA::~ITEM_DATA(void)
{
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテムデータ作成
 *
 * @param   itemID    アイテム番号
 * @param   pHeap     ヒープ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void ITEM_DATA::Create( int itemID, gfl2::heap::HeapBase * pHeap )
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq arcReq;
  arcReq.fileName = pml::Library::m_setting->garcPath_ItemData;
  arcReq.heapForFile = pHeap->GetLowerHandle();
  arcReq.heapForReq = pHeap->GetLowerHandle();
  /*
    2016/08/25  Yu Muto
    GF_DEVELOP_HIO_RESOURCE!=0の場合は、
    全てのリソースをHIO経由で読み込むモードへの対応のためデフォルト設定（マネージャ設定に依存）とします
  */
#if GF_DEVELOP_HIO_RESOURCE
#else
  arcReq.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_MAIN;
#endif
  pml::Library::m_pmlFileManager->SyncArcFileOpen(arcReq);

  gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq readReq;
  readReq.fileName = pml::Library::m_setting->garcPath_ItemData;
  readReq.bufSize = sizeof(ITEM_PRM);
  readReq.datId = itemID;
  readReq.pBuf = &prm;
  pml::Library::m_pmlFileManager->SyncArcFileLoadDataBuf( readReq );

  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
  closeReq.fileName = pml::Library::m_setting->garcPath_ItemData;
  pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );

  id = itemID;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテムデータ作成（ファイル指定）
 *
 * @param   itemID    アイテム番号
 * @param   arcFile   ファイル
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void ITEM_DATA::Create( int itemID, const char *garcPath  )
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq readReq;
  readReq.fileName = garcPath;
  readReq.bufSize = sizeof(ITEM_PRM);
  readReq.datId = itemID;
  readReq.pBuf = &prm;
  pml::Library::m_pmlFileManager->SyncArcFileLoadDataBuf( readReq );

  id = itemID;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   パラメータ取得
 *
 * @param   prmID     パラメータID
 *
 * @return  パラメータ
 */
//--------------------------------------------------------------------------------------------
int ITEM_DATA::GetParam( ITEM_PRM_ID prmID ) const
{
  switch( prmID ){
  case ITEM_DATA::PRM_ID_ITEMNUMBER:  // アイテム番号
    return id;

  case ITEM_DATA::PRM_ID_PRICE:       // 買値
    return ( prm.price * 10 );
  case ITEM_DATA::PRM_ID_EQUIP:       // 装備効果
    return prm.eqp;
  case ITEM_DATA::PRM_ID_ATTACK:      // 威力
    return prm.atc;
  case ITEM_DATA::PRM_ID_TUIBAMU_EFF: // ついばむ効果
    return prm.tuibamu_eff;
  case ITEM_DATA::PRM_ID_NAGE_EFF:    // なげつける効果
    return prm.nage_eff;
  case ITEM_DATA::PRM_ID_NAGE_ATC:    // なげつける威力
    return prm.nage_atc;
  case ITEM_DATA::PRM_ID_SIZEN_ATC:   // しぜんのめぐみ威力
    return prm.sizen_atc;
  case ITEM_DATA::PRM_ID_SIZEN_TYPE:  // しぜんのめぐみタイプ
    return prm.sizen_type;
  case ITEM_DATA::PRM_ID_IMP:         // 重要
    return prm.imp;
  case ITEM_DATA::PRM_ID_CNV:         // 便利ボタン
    return prm.cnv_btn;
  case ITEM_DATA::PRM_ID_F_POCKET:    // FIELD保存先（ポケット番号）
    return prm.fld_pocket;
  case ITEM_DATA::PRM_ID_B_POCKET:    // BATTLE保存先（ポケット番号）
    return prm.btl_pocket;
  case ITEM_DATA::PRM_ID_F_FUNC:      // FIELD機能
    return prm.field_func;
  case ITEM_DATA::PRM_ID_B_FUNC:      // BATTLE機能
    return prm.battle_func;
  case ITEM_DATA::PRM_ID_WORK_TYPE:   // ワークタイプ
    return prm.work_type;
  case ITEM_DATA::PRM_ID_ITEM_TYPE:   // アイテム種類
    return prm.type;
  case ITEM_DATA::PRM_ID_SPEND:       // 消費するか（戦闘装備品用）
    return prm.spend;
  case ITEM_DATA::PRM_ID_USE_SPEND:   // 使用時に消費するか ( 消費しない場合 = 1 )
    return prm.use_no_spend;
  case ITEM_DATA::PRM_ID_SORT:        // ソート番号
    return prm.sort;

  default:                            // 汎用ワーク
    switch( prm.work_type ){
    case ITEM_DATA::WKTYPE_NORMAL:    // 通常
      return prm.work[0];
    case ITEM_DATA::WKTYPE_POKEUSE:   // ポケモンに使う
      return GetWorkRecoverItem( prmID, (ITEMWORK_RCV *)prm.work );
    case ITEM_DATA::WKTYPE_BALL:      // モンスターボール
      return prm.work[0];
    }
    break;
  }

  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   回復アイテムのパラメータ取得
 *
 * @param   prmID     パラメータID
 * @param   wk        ワーク
 *
 * @return  パラメータ
 */
//--------------------------------------------------------------------------------------------
static int GetWorkRecoverItem( ITEM_DATA::ITEM_PRM_ID prmID, ITEMWORK_RCV * wk )
{
  switch( prmID ){
  case ITEM_DATA::PRM_ID_SLEEP_RCV:             // 眠り回復
    return (int)wk->sleep_rcv;
  case ITEM_DATA::PRM_ID_POISON_RCV:            // 毒回復
    return (int)wk->poison_rcv;
  case ITEM_DATA::PRM_ID_BURN_RCV:              // 火傷回復
    return (int)wk->burn_rcv;
  case ITEM_DATA::PRM_ID_ICE_RCV:               // 氷回復
    return (int)wk->ice_rcv;
  case ITEM_DATA::PRM_ID_PARALYZE_RCV:          // 麻痺回復
    return (int)wk->paralyze_rcv;
  case ITEM_DATA::PRM_ID_PANIC_RCV:             // 混乱回復
    return (int)wk->panic_rcv;
  case ITEM_DATA::PRM_ID_MEROMERO_RCV:          // メロメロ回復
    return (int)wk->meromero_rcv;
  case ITEM_DATA::PRM_ID_ABILITY_GUARD:         // 能力ガード
    return (int)wk->ability_guard;
  case ITEM_DATA::PRM_ID_DEATH_RCV:             // 瀕死回復
    return (int)wk->death_rcv;
  case ITEM_DATA::PRM_ID_ALL_DEATH_RCV:         // 全員瀕死回復
    return (int)wk->alldeath_rcv;
  case ITEM_DATA::PRM_ID_LV_UP:                 // レベルアップ
    return (int)wk->lv_up;
  case ITEM_DATA::PRM_ID_EVOLUTION:             // 進化
    return (int)wk->evolution;
  case ITEM_DATA::PRM_ID_ATTACK_UP:             // 攻撃力アップ
    return (int)wk->atc_up;
  case ITEM_DATA::PRM_ID_DEFENCE_UP:            // 防御力アップ
    return (int)wk->def_up;
  case ITEM_DATA::PRM_ID_SP_ATTACK_UP:          // 特攻アップ
    return (int)wk->spa_up;
  case ITEM_DATA::PRM_ID_SP_DEFENCE_UP:         // 特防アップ
    return (int)wk->spd_up;
  case ITEM_DATA::PRM_ID_AGILITY_UP:            // 素早さアップ
    return (int)wk->agi_up;
  case ITEM_DATA::PRM_ID_HIT_UP:                // 命中率アップ
    return (int)wk->hit_up;
  case ITEM_DATA::PRM_ID_CRITICAL_UP:           // クリティカル率アップ
    return (int)wk->critical_up;
  case ITEM_DATA::PRM_ID_PP_UP:                 // PPアップ
    return (int)wk->pp_up;
  case ITEM_DATA::PRM_ID_PP_3UP:                // PPアップ（３段階）
    return (int)wk->pp_3up;
  case ITEM_DATA::PRM_ID_PP_RCV:                // PP回復
    return (int)wk->pp_rcv;
  case ITEM_DATA::PRM_ID_ALL_PP_RCV:            // PP回復（全ての技）
    return (int)wk->allpp_rcv;
  case ITEM_DATA::PRM_ID_HP_RCV:                // HP回復
    return (int)wk->hp_rcv;
  case ITEM_DATA::PRM_ID_HP_EXP:                // HP努力値アップ
    return (int)wk->hp_exp;
  case ITEM_DATA::PRM_ID_POWER_EXP:             // 攻撃努力値アップ
    return (int)wk->pow_exp;
  case ITEM_DATA::PRM_ID_DEFENCE_EXP:           // 防御努力値アップ
    return (int)wk->def_exp;
  case ITEM_DATA::PRM_ID_AGILITY_EXP:           // 素早さ努力値アップ
    return (int)wk->agi_exp;
  case ITEM_DATA::PRM_ID_SP_ATTACK_EXP:         // 特攻努力値アップ
    return (int)wk->spa_exp;
  case ITEM_DATA::PRM_ID_SP_DEFENCE_EXP:        // 特防努力値アップ
    return (int)wk->spd_exp;
  case ITEM_DATA::PRM_ID_EXP_LIMIT_FLAG:        // 努力値限界制御
    return (int)wk->exp_limit;
  case ITEM_DATA::PRM_ID_FRIEND1:               // なつき度１
    return (int)wk->friend_exp1;
  case ITEM_DATA::PRM_ID_FRIEND2:               // なつき度２
    return (int)wk->friend_exp2;
  case ITEM_DATA::PRM_ID_FRIEND3:               // なつき度３
    return (int)wk->friend_exp3;
  case ITEM_DATA::PRM_ID_HP_EXP_POINT:          // HP努力値の値
    return (int)wk->prm_hp_exp;
  case ITEM_DATA::PRM_ID_POWER_EXP_POINT:       // 攻撃努力値の値
    return (int)wk->prm_pow_exp;
  case ITEM_DATA::PRM_ID_DEFENCE_EXP_POINT:     // 防御努力値の値
    return (int)wk->prm_def_exp;
  case ITEM_DATA::PRM_ID_AGILITY_EXP_POINT:     // 素早さ努力値の値
    return (int)wk->prm_agi_exp;
  case ITEM_DATA::PRM_ID_SP_ATTACK_EXP_POINT:   // 特攻努力値の値
    return (int)wk->prm_spa_exp;
  case ITEM_DATA::PRM_ID_SP_DEFENCE_EXP_POINT:  // 特防努力値の値
    return (int)wk->prm_spd_exp;
  case ITEM_DATA::PRM_ID_HP_RCV_POINT:          // HP回復値の値
    return (int)wk->prm_hp_rcv;
  case ITEM_DATA::PRM_ID_PP_RCV_POINT:          // pp回復値の値
    return (int)wk->prm_pp_rcv;
  case ITEM_DATA::PRM_ID_FRIEND1_POINT:         // なつき度1の値
    return (int)wk->prm_friend1;
  case ITEM_DATA::PRM_ID_FRIEND2_POINT:         // なつき度2の値
    return (int)wk->prm_friend2;
  case ITEM_DATA::PRM_ID_FRIEND3_POINT:         // なつき度3の値
    return (int)wk->prm_friend3;
  }
  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテム名取得 ( MsgData指定 )
 *
 * @param   md      MsgData
 * @param   buf     アイテム名格納先バッファ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
#if 0 // @note WordSetを使用して取得してください
void ITEM_DATA::ITEM_GetItemName( gfl2::str::MsgData * md, gfl2::str::StrBuf * buf )
{
  md->GetString( id, *buf );
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテム名取得
 *
 * @param   buf     アイテム名格納先バッファ
 * @param   pHeap   ヒープ
 *
 * @return  none
 *
 * @li      以下の3つの関数の集合体
 *            gfl2::str::MsgData * ITEM_CreateItemNameMsgData(...);
 *            void ITEM_GetItemName(...);
 *            void ITEM_DeleteItemNameMsgData(...);
 */
//--------------------------------------------------------------------------------------------
#if 0 // @note WordSetを使用して取得してください
void ITEM_DATA::ITEM_GetItemName( gfl2::str::StrBuf * buf, gfl2::heap::HeapBase * pHeap )
{
  gfl2::str::MsgData * md = ITEM_CreateItemNameMsgData( gfl2::str::MsgData::LOAD_PART, pHeap );
  ITEM_GetItemName( md, buf );
  ITEM_DeleteItemNameMsgData( md );
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテム説明取得 ( MsgData指定 )
 *
 * @param   md      MsgData
 * @param   buf     アイテム名格納先バッファ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
//void ITEM_DATA::ITEM_GetInfo( gfl2::str::MsgData * md, gfl2::str::StrBuf * buf )
//{
//  md->GetString( id, *buf );
//}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテム説明取得
 *
 * @param   buf     アイテム説明格納先バッファ
 * @param   pHeap   ヒープ
 *
 * @return  none
 *
 * @li      以下の3つの関数の集合体
 *            gfl2::str::MsgData * ITEM_CreateInfoMsgData(...);
 *            void ITEM_GetInfo(...);
 *            void ITEM_DeleteInfoMsgData(...);
 */
//--------------------------------------------------------------------------------------------
/*
void ITEM_DATA::ITEM_GetInfo( gfl2::str::StrBuf * buf, gfl2::heap::HeapBase * pHeap )
{
  gfl2::str::MsgData * md = ITEM_CreateInfoMsgData( gfl2::str::MsgData::LOAD_PART, pHeap );
  ITEM_GetInfo( md, buf );
  ITEM_DeleteInfoMsgData( md );
}
*/
//--------------------------------------------------------------------------------------------
/**
 * @brief   常駐アイテムデータ作成
 *
 * @param   heap   ヒープ
 *
 * @return  none
 *
 * @li	グローバルなポインタに展開するので、複数回やらないように注意！
 */
//--------------------------------------------------------------------------------------------
void ITEM_DATA::CreateExpandData( gfl2::heap::HeapBase * heap )
{
	GFL_ASSERT_STOP( ItemParamFieldPocket == NULL ); //@fix メモリ破壊 or プログラムミス

//	nn::os::Tick start = nn::os::Tick::GetSystemCurrent();

  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq arcReq;
    arcReq.fileName = pml::Library::m_setting->garcPath_FieldPocket;
    arcReq.heapForFile = heap->GetLowerHandle();
    arcReq.heapForReq = heap->GetLowerHandle();
    /*
      2016/08/25  Yu Muto
      GF_DEVELOP_HIO_RESOURCE!=0の場合は、
      全てのリソースをHIO経由で読み込むモードへの対応のためデフォルト設定（マネージャ設定に依存）とします
    */
#if GF_DEVELOP_HIO_RESOURCE
#else
    arcReq.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_MAIN;
#endif
    pml::Library::m_pmlFileManager->SyncArcFileOpen(arcReq);
  }

  { // フィールドポケットデータ
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq readReq;
    readReq.fileName   = pml::Library::m_setting->garcPath_FieldPocket;
    readReq.datId      = 0;
    readReq.ppBuf      = reinterpret_cast<void**>( &ItemParamFieldPocket );
    readReq.heapForBuf = heap;
    readReq.align      = 4;
    pml::Library::m_pmlFileManager->SyncArcFileLoadData( readReq );
  }

  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.fileName = pml::Library::m_setting->garcPath_FieldPocket;
    pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );
  }

/*
  // 23678 くらい
	nn::os::Tick end = nn::os::Tick::GetSystemCurrent();

	nn::fnd::TimeSpan ss = start.ToTimeSpan();
	nn::fnd::TimeSpan es = end.ToTimeSpan();

	s64	sec = es.GetMicroSeconds() - ss.GetMicroSeconds();

	GFL_PRINT( "■ CreatePocketData\n" );
	GFL_PRINT( "sec = %lld\n", sec );
	GFL_PRINT( "■■■■■■■■■\n" );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   常駐アイテムデータ削除
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void ITEM_DATA::DeleteExpandData(void)
{
  GflHeapSafeFreeMemory( ItemParamFieldPocket );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   パラメータ取得（常駐データから）
 *
 * @param   item		アイテム番号
 * @param		prmID		パラメータID
 *
 * @return  パラメータ
 *
 * @li	以下、有効パラメータID
 *				PRM_ID_F_POCKET		フィールドポケット番号
 */
//--------------------------------------------------------------------------------------------
int ITEM_DATA::GetParam( u32 item, ITEM_DATA::ITEM_PRM_ID prmID )
{
	if( item > ITEM_DATA_MAX ){
		GFL_ASSERT( 0 );  //@check 引数異常は補正する
		return 0;
	}

	// フィールドポケット番号
	if( prmID == PRM_ID_F_POCKET ){
		u8	dat = ItemParamFieldPocket[item/2];
		if( ( item & 1 ) == 0 ){
			return ( ( dat >> 4 ) & 0x0f );
		}else{
			return ( dat & 0x0f );
		}
	}

	GFL_ASSERT( 0 );  //@check プログラムミス検出用
	return 0;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテム名のMsgDataを作成
 *
 * @param   type    読み込みタイプ
 * @param   pHeap   ヒープ
 *
 * @return  gfl2::str::MsgData
 */
//--------------------------------------------------------------------------------------------
#if 0 // @note WordSetを使用して取得してください
gfl2::str::MsgData * ITEM_CreateItemNameMsgData( gfl2::str::MsgData::LoadType type, gfl2::heap::HeapBase * pHeap )
{
  gfl2::str::MsgData * md = GFL_NEW( pHeap ) 
    gfl2::str::MsgData( 
        pml::Library::m_setting->garcPath_Message,
        pml::Library::m_setting->msgDataID_ItemName,
        pHeap, type );
  return md;
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテム名のMsgDataを削除
 *
 * @param   md      MsgData
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
#if 0 // @note WordSetを使用して取得してください
void ITEM_DeleteItemNameMsgData( gfl2::str::MsgData * md )
{
  GFL_DELETE  md;
}
#endif



//============================================================================================
//  ボール関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * アイテムID -> ボールID 変換
 *
 * @param   item    アイテムNo
 *
 * @retval  BALL_ID   ボールID（指定されたアイテムIDがボール以外の場合、BALLID_NULL）
 */
//--------------------------------------------------------------------------------------------
BALL_ID ITEM_GetBallID( u16 item )
{
  for( u32 i=0; i<GFL_NELEMS(ItemTable_BALL); i++ )
  {
    if( item == ItemTable_BALL[i] )
    {
      return static_cast<BALL_ID>( i+1 );
    }
  }
  return BALLID_NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * ボールID -> アイテムNo 変換
 *
 * @param   BALL_ID   ボールID
 * @retval  itemID    アイテムNo（指定されたアイテムIDがボール以外の場合、ITEM_DUMMY_DATA
 */
//--------------------------------------------------------------------------------------------
u16 ITEM_BallID2ItemID( BALL_ID ballID )
{
  if( ballID == BALLID_NULL ||
      ballID > BALLID_MAX )
  {
    return ITEM_DUMMY_DATA;
  }
  return ItemTable_BALL[ ballID-1 ];
}


//============================================================================================
//  技マシン関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief   技マシンかどうか
 *
 * @param   item  アイテム番号
 *
 * @retval  "true = 技マシン"
 * @retval  "false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_CheckWazaMachine( u16 item )
{
  if( ITEM_GetWazaMashineNo( item ) != ITEM_WAZAMACHINE_ERROR )
  {
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   技マシンで覚えられる技を取得
 *
 * @param   item  アイテム番号
 *
 * @return  技番号
 */
//--------------------------------------------------------------------------------------------
u16 ITEM_GetWazaNo( u16 item )
{
  for( u32 i=0; i<GFL_NELEMS(ItemTable_WAZA_MACHINE); i++ )
  {
    if( item == ItemTable_WAZA_MACHINE[i] )
    {
      return WazaMachineNo[i];
    }
  }
  for( u32 i=0; i<GFL_NELEMS(ItemTable_HIDEN_MACHINE); i++ )
  {
    if( item == ItemTable_HIDEN_MACHINE[i] )
    {
      return HidenMachineNo[i];
    }
  }
  return WAZANO_NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   選択された技がポケモン交換に適さない技かどうか（秘伝技かどうか）
 *
 * @param   waza  技番号
 *
 * @retval  "true = 交換に適さない技(秘伝技"
 * @retval  "false = 交換に適する技(秘伝技以外"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_CheckHidenWaza( u16 waza )
{
  for( u32 i=0; i<GFL_NELEMS(HidenMachineNo); i++ )
  {
    if( waza == HidenMachineNo[i] )
    {
      return true;
    }
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   技マシン番号取得
 *
 * @param   item  アイテム番号
 *
 * @return  技マシン番号
 *
 * @li  秘伝マシンは通常の技マシンの後にカウントされる
 */
//--------------------------------------------------------------------------------------------
u32 ITEM_GetWazaMashineNo( u16 item )
{
  u32 normalMax = GFL_NELEMS( ItemTable_WAZA_MACHINE );

  for( u32 i=0; i<normalMax; i++ )
  {
    if( item == ItemTable_WAZA_MACHINE[i] )
    {
      return i;
    }
  }

  u32 hidenNo = ITEM_GetHidenMashineNo( item );
  if( hidenNo != ITEM_WAZAMACHINE_ERROR )
  {
    return ( hidenNo + normalMax );
  }

  return ITEM_WAZAMACHINE_ERROR;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   秘伝マシン番号取得
 *
 * @param   item  アイテム番号
 *
 * @return  秘伝マシン番号
 */
//--------------------------------------------------------------------------------------------
u32 ITEM_GetHidenMashineNo( u16 item )
{
  for( u32 i=0; i<GFL_NELEMS(ItemTable_HIDEN_MACHINE); i++ )
  {
    if( item == ItemTable_HIDEN_MACHINE[i] )
    {
      return i;
    }
  }
  return ITEM_WAZAMACHINE_ERROR;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   通常技マシン数取得
 *
 * @param   none
 *
 * @return  技マシン数
 */
//--------------------------------------------------------------------------------------------
u32 ITEM_GetNormalWazaMashineMax(void)
{
	return GFL_NELEMS( ItemTable_WAZA_MACHINE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   秘伝マシン数取得
 *
 * @param   none
 *
 * @return  秘伝マシン数
 */
//--------------------------------------------------------------------------------------------
u32 ITEM_GetHidenMashineMax(void)
{
	return GFL_NELEMS( ItemTable_HIDEN_MACHINE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   全技マシン数取得（通常＋秘伝）
 *
 * @param   none
 *
 * @return  通常技マシン数と秘伝マシン数の合計
 */
//--------------------------------------------------------------------------------------------
u32 ITEM_GetWazaMashineMax(void)
{
	return ( GFL_NELEMS(ItemTable_WAZA_MACHINE) + GFL_NELEMS(ItemTable_HIDEN_MACHINE) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   技マシン番号から技を取得
 *
 * @param   no	技マシン番号
 *
 * @return  技番号
 */
//--------------------------------------------------------------------------------------------
u16 ITEM_GetWazaMashineWaza( u32 no )
{
  GFL_ASSERT( no < ITEM_GetWazaMashineMax() );

  // 秘伝
  if( no >= GFL_NELEMS(ItemTable_WAZA_MACHINE) )
  {
    return HidenMachineNo[no-GFL_NELEMS(ItemTable_WAZA_MACHINE)];
  }

	return WazaMachineNo[no];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   技マシン番号からアイテム番号を取得
 *
 * @param   no	技マシン番号
 *
 * @return  アイテム番号
 */
//--------------------------------------------------------------------------------------------
u16 ITEM_GetWazaMashineItemNo( u32 no )
{
  GFL_ASSERT( no < ITEM_GetWazaMashineMax() );

  // 秘伝
  if( no >= GFL_NELEMS(ItemTable_WAZA_MACHINE) )
  {
    return ItemTable_HIDEN_MACHINE[no-GFL_NELEMS(ItemTable_WAZA_MACHINE)];
  }

  return ItemTable_WAZA_MACHINE[no];
}


//============================================================================================
//  木の実関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * アイテムが木の実かどうかをチェック
 *
 * @param item  アイテム番号
 *
 * @retval  "true = 木の実"
 * @retval  "false = 木の実以外"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_CheckNuts( u16 item )
{
  u8 no = ITEM_GetNutsNo( item );
  if( no != 0xff )
  {
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * 木の実の番号を取得
 *
 * @param item  アイテム番号
 *
 * @return  木の実番号
 */
//--------------------------------------------------------------------------------------------
u8 ITEM_GetNutsNo( u16 item )
{
  for( u32 i=0; i<GFL_NELEMS(ItemTable_NUTS); i++ ){
    if( item == ItemTable_NUTS[i] ){
      return i;
    }
  }
  return 0xff;    // エラー
}

//--------------------------------------------------------------------------------------------
/**
 * 指定IDの木の実のアイテム番号を取得
 *
 * @param id    木の実のID
 *
 * @return  アイテム番号
 */
//--------------------------------------------------------------------------------------------
u16 ITEM_GetNutsID( u8 id )
{
  if( id >= GFL_NELEMS(ItemTable_NUTS) ){
    return ITEM_DUMMY_DATA;  // エラー
  }
  return ItemTable_NUTS[id];
}

//--------------------------------------------------------------------------------------------
/**
 * 木の実の数を取得
 *
 * @param   none
 *
 * @return  木の実の数
 */
//--------------------------------------------------------------------------------------------
u32 ITEM_GetNutsMax(void)
{
  return GFL_NELEMS( ItemTable_NUTS );
}


//============================================================================================
//  その他
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief メガストーンかどうかを判定
 *
 * @param item  アイテム番号
 *
 * @retval  true=メガストーン, false=それ以外
 */
//--------------------------------------------------------------------------------------------
bool ITEM_CheckMegaStone( u16 item )
{
  for( u32 i=0; i<GFL_NELEMS(ItemTable_MEGA_STONE); i++ )
  {
    if( item == ItemTable_MEGA_STONE[i] )
    {
      return true;
    }
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * アイテムがジュエルかどうかをチェック
 *
 * @param item  アイテム番号
 *
 * @retval  "true = ジュエル"
 * @retval  "false = ジュエル以外"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_CheckJewel( u16 item )
{
	for( u32 i=0; i<GFL_NELEMS(ItemTable_JEWEL); i++ ){
		if( item == ItemTable_JEWEL[i] ){
			return true;
		}
	}
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * アイテムがピースかどうかをチェック
 *
 * @param item  アイテム番号
 *
 * @retval true   ピース
 * @retval false  ピース以外"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_CheckPiece( u16 item )
{
  for( u32 i=0; i<GFL_NELEMS(ItemTable_PIECE); i++ ){
		if( item == ItemTable_PIECE[i] ){
			return true;
		}
	}
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    ITEM_GetPieceTable
 * @brief   ピースのテーブルを取得
 * @date    2015.11.04
 *
 * @param   max   テーブル要素数格納場所
 *
 * @return  テーブル
 */
//-----------------------------------------------------------------------------
const u16 * ITEM_GetPieceTable( u32 * max )
{
  *max = GFL_NELEMS(ItemTable_PIECE);
  return ItemTable_PIECE;
}

//-----------------------------------------------------------------------------
/**
 * @func    ITEM_CheckBeads
 * @brief   アイテムがビーズかをチェック
 * @date    2015.11.04
 *
 * @retval  true  = ビーズ
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ITEM_CheckBeads( u16 item )
{
  for( u32 i=0; i<GFL_NELEMS(ItemTable_BEADS); i++ )
  {
    if( item == ItemTable_BEADS[i] )
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    ITEM_GetBeadsTable
 * @brief   ビーズのテーブルを取得
 * @date    2016.01.13
 *
 * @param   max   テーブル要素数格納場所
 *
 * @return  テーブル
 */
//-----------------------------------------------------------------------------
const u16 * ITEM_GetBeadsTable( u32 * max )
{
  *max = GFL_NELEMS(ItemTable_BEADS);
  return ItemTable_BEADS;
}

//-----------------------------------------------------------------------------
/**
 * @func    ITEM_GetRotoponTable
 * @brief   ロトポンのテーブルを取得
 * @date    2017.02.01
 *
 * @param   max   テーブル要素数格納場所
 *
 * @return  テーブル
 */
//-----------------------------------------------------------------------------
const u16 * ITEM_GetRotoponTable( u32 * max )
{
  *max = GFL_NELEMS(ItemTable_ROTOPON);
  return ItemTable_ROTOPON;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテムをポケモンに持たせられるか
 *
 * @param   item    アイテム番号
 *
 * @retval  "true = 可"
 * @retval  "false = 不可"
 */
//--------------------------------------------------------------------------------------------
bool ITEM_CheckPokeAdd( u16 item )
{
  if( item == ITEM_KONPEBOORU ||
      item == ITEM_PAAKUBOORU ){
    return false;
  }
  return true;
}


} // item
