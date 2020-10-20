//==============================================================================
/**
 * @file	  Record.cpp
 * @brief		ゲーム内カウント処理関連
 * @author  tamada
 * @date    2012.09.28
 */
// =============================================================================

#include "Savedata/include/Record.h"



#include <debug/include/gfl2_DebugPrint.h>
#include  <debug/include/gfl2_Assert.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <SaveData/include/JoinFestaDataSave.h>
#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaGlobalAttractionDefine.h>

GFL_NAMESPACE_BEGIN(Savedata)

static const u32 GLOBAL_RECORD_PAIR_TABLE_NUM = 9;
static const Savedata::Record::RECORD_ID GLOBAL_RECORD_PAIR_TABLE[GLOBAL_RECORD_PAIR_TABLE_NUM][2] =
{
  { Savedata::Record::RECID_GLOBAL_ATTRACTION_CAPTURE ,     Savedata::Record::RECID_CAPTURE_POKE        },
  { Savedata::Record::RECID_GLOBAL_ATTRACTION_GTSTRADE ,    Savedata::Record::RECID_GTS_TRADE_COUNT     },
  { Savedata::Record::RECID_GLOBAL_ATTRACTION_EGG ,         Savedata::Record::RECID_TAMAGO_HATCHING     },
//{ Savedata::Record::RECID_GLOBAL_ATTRACTION_CHAMPION ,    Savedata::Record::RECID_DENDOU_CNT          },
//{ Savedata::Record::RECID_GLOBAL_ATTRACTION_RAREFISHING , Savedata::Record::RECID_FISHING_BUBBLE      }
  /*  momiji  */
  { Savedata::Record::RECID_GLOBAL_ATTRACTION_FESCOIN,      Savedata::Record::RECID_JF_USE_JCOIN        },
  { Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLEFES,    Savedata::Record::RECID_LARGE_BATTLEFES_WIN },
  { Savedata::Record::RECID_GLOBAL_ATTRACTION_MANTABP,      Savedata::Record::RECID_LARGE_MANTAINBP     },
  { Savedata::Record::RECID_GLOBAL_ATTRACTION_FISHINGPOKE,  Savedata::Record::RECID_FISHING_POKEMON     },
  { Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLEROYAL,  Savedata::Record::RECID_BATTLEROYAL_WIN     },
  { Savedata::Record::RECID_GLOBAL_ATTRACTION_EVOLUTION,    Savedata::Record::RECID_POKE_EVOLUTION      },
};


u8 ConvertRecordIDToGlobalAttractionID( int recordID )
{
  u8 globalAttractionID = 0;

  switch( recordID )
  {
    // @fix MMCat[447]グローバルアトラクションIDがmomijiだと13～24になる。
    case Savedata::Record::RECID_GLOBAL_ATTRACTION_CAPTURE:      globalAttractionID = 13; break; // グローバルアトラクション01
    case Savedata::Record::RECID_GLOBAL_ATTRACTION_GTSTRADE:     globalAttractionID = 14; break; // グローバルアトラクション02
    case Savedata::Record::RECID_GLOBAL_ATTRACTION_EGG:          globalAttractionID = 15; break; // グローバルアトラクション03
    case Savedata::Record::RECID_GLOBAL_ATTRACTION_POKEMAME:     globalAttractionID = 16; break; // グローバルアトラクション04
    case Savedata::Record::RECID_GLOBAL_ATTRACTION_FESCOIN:      globalAttractionID = 17; break; // グローバルアトラクション05
    case Savedata::Record::RECID_GLOBAL_ATTRACTION_FIELDSCAN:    globalAttractionID = 18; break; // グローバルアトラクション06
    case Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLEFES:    globalAttractionID = 19; break; // グローバルアトラクション07
    case Savedata::Record::RECID_GLOBAL_ATTRACTION_MANTABP:      globalAttractionID = 20; break; // グローバルアトラクション08
    case Savedata::Record::RECID_GLOBAL_ATTRACTION_FISHINGPOKE:  globalAttractionID = 21; break; // グローバルアトラクション09
    case Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLEROYAL:  globalAttractionID = 22; break; // グローバルアトラクション10
    case Savedata::Record::RECID_GLOBAL_ATTRACTION_EVOLUTION:    globalAttractionID = 23; break; // グローバルアトラクション11
    case Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLETREE:   globalAttractionID = 24; break; // グローバルアトラクション12
  }

  return globalAttractionID;
}


int ConvertRecordIDToIndex( int recordID )
{
  int index = -1;

  u8 globalAttractionID = ConvertRecordIDToGlobalAttractionID( recordID );

  if( globalAttractionID > 0 )
  {
    Savedata::JoinFestaDataSave* pJFSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

    for( int i = 0; i < NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::RECORD_MAX; ++i )
    {
      if( globalAttractionID == pJFSave->GetGlobalAttractionData( static_cast<u8>( i ) )->globalAttractionID )
      {
        index = i;

        break;
      }
    }
  }

  return index;
}

//--------------------------------------------------------------------------
/// 足しこみ
//--------------------------------------------------------------------------
void AddRecord( Savedata::Record::RECORD_ID id, u32 add_value )
{
  // @fix GFNMCat[5605] レコードIDから正しいグローバルアトラクションIDに変換して、更にグローバルアトラクションIDからIndexに変換して対象のセーブデータに反映させる
  int globalAttractionIndex = ConvertRecordIDToIndex( static_cast<int>( id ) );

  Savedata::Record* pRecord = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetRecordSaveData();
  if( id >= Savedata::Record::RECID_GLOBAL_ATTRACTION_CAPTURE && id <= Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLETREE )
  {//グローバルアトラクションレコード
    Savedata::JoinFestaDataSave* pJFSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

    if( globalAttractionIndex >= 0 )
    {
      if( pJFSave->GetGlobalAttractionData( static_cast<u8>( globalAttractionIndex ) )->openState != NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_NOW ||
        pJFSave->GetGlobalAttractionData( static_cast<u8>( globalAttractionIndex ) )->participateState != NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_OK
        )
      {//カウントしない
        return;
      }
    }
    else
    {
      return;
    }
  }

  u32 resultRec = pRecord->AddRec( id , add_value );
  if( id >= Savedata::Record::RECID_GLOBAL_ATTRACTION_CAPTURE && id <= Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLETREE )
  {//グローバルアトラクションレコードをGAセーブデータに設定
    Savedata::JoinFestaDataSave* pJFSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
    if( globalAttractionIndex >= 0 )
    {
      pJFSave->GetGlobalAttractionData( static_cast<u8>( globalAttractionIndex ) )->localRecord = resultRec;
    }
  }

  //グローバルアトラクションと対応しているレコードだったらそちらも処理する
  for( u32 index = 0 ; index < GLOBAL_RECORD_PAIR_TABLE_NUM ; ++index )
  {
    if( id == GLOBAL_RECORD_PAIR_TABLE[index][1] )
    {
      AddRecord( GLOBAL_RECORD_PAIR_TABLE[index][0] , add_value );
      break;
    }
  }
}

//--------------------------------------------------------------------------
/// 指定の値のセット
//--------------------------------------------------------------------------
void SetRecord( Savedata::Record::RECORD_ID id , u32 value )
{
  int globalAttractionIndex = ConvertRecordIDToIndex( static_cast<int>( id ) );

  Savedata::Record* pRecord = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetRecordSaveData();
  if( id >= Savedata::Record::RECID_GLOBAL_ATTRACTION_CAPTURE && id <= Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLETREE )
  {//グローバルアトラクションレコード
    Savedata::JoinFestaDataSave* pJFSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

    if( globalAttractionIndex >= 0 )
    {
      if( pJFSave->GetGlobalAttractionData( static_cast<u8>( globalAttractionIndex ) )->openState != NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_NOW ||
        pJFSave->GetGlobalAttractionData( static_cast<u8>( globalAttractionIndex ) )->participateState != NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_OK
        )
      {//カウントしない
        return;
      }
    }
    else
    {
      return;
    }
  }

  pRecord->SetRec( id , value );

  //グローバルアトラクションと対応しているレコードだったらそちらも処理する
  for( u32 index = 0 ; index < GLOBAL_RECORD_PAIR_TABLE_NUM ; ++index )
  {
    if( id == GLOBAL_RECORD_PAIR_TABLE[index][1] )
    {
      GFL_ASSERT( 0 );//@fix セットはないはず
      //SetRecord( GLOBAL_RECORD_PAIR_TABLE[0][index] , value );
      break;
    }
  }
}

//--------------------------------------------------------------------------
/// インクリメント
//--------------------------------------------------------------------------
void IncRecord( Savedata::Record::RECORD_ID id )
{
  // @fix GFNMCat[5605] レコードIDから正しいグローバルアトラクションIDに変換して、更にグローバルアトラクションIDからIndexに変換して対象のセーブデータに反映させる
  int globalAttractionIndex = ConvertRecordIDToIndex( static_cast<int>( id ) );

  Savedata::Record* pRecord = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetRecordSaveData();
  if( id >= Savedata::Record::RECID_GLOBAL_ATTRACTION_CAPTURE && id <= Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLETREE )
  {//グローバルアトラクションレコード
    Savedata::JoinFestaDataSave* pJFSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

    if( globalAttractionIndex >= 0 )
    {
      if( pJFSave->GetGlobalAttractionData( static_cast<u8>( globalAttractionIndex ) )->openState != NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_NOW ||
        pJFSave->GetGlobalAttractionData( static_cast<u8>( globalAttractionIndex ) )->participateState != NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_OK
        )
      {//カウントしない
        return;
      }
    }
    else
    {
      return;
    }
  }

  u32 resultRec = pRecord->IncRec( id );
  if( id >= Savedata::Record::RECID_GLOBAL_ATTRACTION_CAPTURE && id <= Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLETREE )
  {//グローバルアトラクションレコードをGAセーブデータに設定
    Savedata::JoinFestaDataSave* pJFSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
    if( globalAttractionIndex >= 0 )
    {
      pJFSave->GetGlobalAttractionData( static_cast<u8>( globalAttractionIndex ) )->localRecord = resultRec;
    }
  }

  //グローバルアトラクションと対応しているレコードだったらそちらも処理する
  for( u32 index = 0 ; index < GLOBAL_RECORD_PAIR_TABLE_NUM ; ++index )
  {
    if( id == GLOBAL_RECORD_PAIR_TABLE[index][1] )
    {
      IncRecord( GLOBAL_RECORD_PAIR_TABLE[index][0] );
      break;
    }
  }
}


//--------------------------------------------------------------------------
/// レコード値取得
//--------------------------------------------------------------------------
u32 GetRecord( Savedata::Record::RECORD_ID id )
{
  Savedata::Record* pRecord = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetRecordSaveData();
  return pRecord->Get( id );
}





//--------------------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------------------
Record::Record( void )
{
	this->Clear( NULL );
}

//--------------------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------------------
Record::~Record( void )
{
}


//--------------------------------------------------------------------------
/**
 * @brief    データの先頭ポインタを得る関数
 * @return   先頭のポインタ
 */
//--------------------------------------------------------------------------
void* Record::GetData(void)
{
	return reinterpret_cast<void*>(&m_data);
}
//--------------------------------------------------------------------------
/**
 * @brief    データサイズ
 * @retval   バイト
 */
//--------------------------------------------------------------------------
size_t Record::GetDataSize(void)
{
  return sizeof( CoreData );
}

//--------------------------------------------------------------------------
/**
 * @brief セーブデータのクリア
 * @param heap　クリアに使用するテンポラリヒープ
 */
//--------------------------------------------------------------------------
void Record::Clear( gfl2::heap::HeapBase * heap )
{
	GFL_UNUSED(heap);
  gfl2::std::MemClear( GetData(), GetDataSize() );

  m_disable = false;
}


//--------------------------------------------------------------------------
/// 足しこみ
//--------------------------------------------------------------------------
u32 Record::AddRec( RECORD_ID id, u32 add_value )
{
  u32 limit = get_limit( id );
  u32 now   = get_value( id );

  if(check_enable( id, 0, add_value )){
    if( (now+add_value) < limit){
      now = set_value( id, now+add_value );
    } else {
      now = set_value( id, limit );
    }
  }

  return now;
}

//--------------------------------------------------------------------------
/// インクリメント
//--------------------------------------------------------------------------
u32 Record::IncRec( RECORD_ID id )
{
  return AddRec( id, 1 );
}

//--------------------------------------------------------------------------
/// 指定の値のセット
//--------------------------------------------------------------------------
u32 Record::SetRec( RECORD_ID id , u32 value )
{
  u32 limit = get_limit( id );
  u32 now   = get_value( id );

  if(check_enable( id, 1, value )){
    if (value < limit) {
      value = set_value( id, value );
    } else {
      value = set_value( id, limit );
    }
  }else{
    value = now;
  }

  return value;
}

//--------------------------------------------------------------------------
/// 登録済みの値を超えていたら書き換える
//--------------------------------------------------------------------------
u32 Record::SetRecIfLarge( RECORD_ID id, u32 value )
{
  u32 limit = get_limit( id );
  u32 now   = get_value( id );

  if(check_enable( id, 2, value )){
    if(value > limit){
      value = limit;  //上限チェック
    }

    // 記録を超えていれば登録
    if (now < value) {
      value = set_value( id, value );
    }
  }else{
    value = now;
  }

  return value;
}

//--------------------------------------------------------------------------
/// レコード値取得
//--------------------------------------------------------------------------
u32 Record::Get( RECORD_ID id )
{
  u32 ret = get_value( id );

  return ret;
}

//--------------------------------------------------------------------------
/// 1日最大？？？のレコード関連を更新する
//--------------------------------------------------------------------------
void Record::DailyUpdate( void )
{
  // レコードデータ1日更新処理用テーブル
  static const RECORD_ID oneday_update_tbl[][2]={
    { RECID_DAYMAX_EXP,             RECID_DAYCNT_EXP,            }, // 1日に稼いだ経験値の最大値
    { RECID_DAYMAX_KILL,            RECID_DAYCNT_KILL,           }, // 1日に倒したポケモンの最大数
    { RECID_DAYMAX_CAPTURE,         RECID_DAYCNT_CAPTURE,        }, // 1日に捕獲したポケモンの最大数
    { RECID_DAYMAX_TRAINER_BATTLE,  RECID_DAYCNT_TRAINER_BATTLE, }, // 1日にトレーナー戦をした最大回数
    { RECID_DAYMAX_EVOLUTION,       RECID_DAYCNT_EVOLUTION,      }, // 1日にポケモン進化させた最大回数
  };

  // レコードデータ1日更新処理
  for(int i=0;i<GFL_NELEMS(oneday_update_tbl);i++){
    // 最大値を超えている場合は1日MAXのレコードに登録
    SetRecIfLarge( oneday_update_tbl[i][0], Get(oneday_update_tbl[i][1]) );
    // 一日の値はリセット
    SetRec( oneday_update_tbl[i][1], 0 );
  }
}

//--------------------------------------------------------------------------
/// レコードの最大値取得
//--------------------------------------------------------------------------
u32 Record::GetMax( RECORD_ID id )
{
  return get_limit(id);
}


//--------------------------------------------------------------------------
/**
 * @brief レコード項目ごとの上限を取得する
 * @param id    レコード項目指定ID
 * @return  u32   上限の値
 */
//--------------------------------------------------------------------------
u32 Record::get_limit( RECORD_ID id )
{
  /// ↓↓↓ レコード定義を増やしたらここも増やすこと！
  /// レコード最大値テーブル
  static const u8 limit_tbl[RECID_ALL_MAX] = {

    // 4バイトレコードの最大値
    MAX_999_999_999,   // RECID_WALK_COUNT,       歩数                                  999,999,999  0
    MAX_999_999_999,   // RECID_REPORT_COUNT,     レポート回数                          999,999,999
    MAX_999_999_999,   // RECID_CLEAR_TIME,       クリアタイム                          999,59,59
    MAX_999_999_999,   // RECID_BATTLE_COUNT,     全戦闘回数                            999,999,999
    MAX_999_999_999,   // RECID_BTL_ENCOUNT,      エンカウント戦闘回数                  999,999,999
    MAX_999_999_999,   // RECID_BTL_TRAINER,      トレーナー戦闘回数                    999,999,999
    MAX_999_999,       // RECID_CAPTURE_POKE,     捕獲したポケモンの数                  999,999
    MAX_999_999,       // RECID_FISHING_SUCCESS,  釣り上げたポケモンの数                999,999
    MAX_999_999,       // RECID_TAMAGO_HATCHING,  タマゴを孵化させた回数                999,999
    MAX_999_999_999,   // RECID_POKE_EVOLUTION,   進化させたポケモンの匹数              999,999,999  
    MAX_999_999_999,   // RECID_RECOVERY_PC,      ポケセンで休んだ回数                  999,999,999  10
    MAX_999_999_999,   // RECID_COMM_TRADE,       P2P交換をした回数          999,999,999
    MAX_999_999_999,   // RECID_COMM_BATTLE,      P2P対戦をした回数          999,999,999
    MAX_999_999_999,   // RECID_COMM_BTL_WIN,     P2P対戦の勝利数            999,999,999
    MAX_999_999_999,   // RECID_COMM_BTL_LOSE,    P2P対戦の敗北数            999,999,999
    MAX_999_999_999,   // RECID_WIFI_BATTLE,      バトルスポットをした回数        999,999,999
    MAX_999_999_999,   // RECID_WIFI_BTL_WIN,     バトルスポットの勝利数          999,999,999
    MAX_999_999_999,   // RECID_WIFI_BTL_LOSE,    バトルスポットの敗北数          999,999,999
    MAX_999_999_999,   // RECID_SHOPPING_CNT,     買い物をした回数                      999,999,999
    MAX_999_999_999,   // RECID_SHOPPING_MONEY,   買い物で使った合計金額                999,999,999
    MAX_999_999_999,   // RECID_SODATEYA_CNT,     育て屋にポケモンを預けた回数          999,999,999  20
    MAX_999_999_999,   // RECID_KILL_POKE,        倒したポケモンの数                    999,999,999
    MAX_999_999_999,   // RECID_DAYMAX_EXP,       1日に稼いだ経験値最大値               999,999,999
    MAX_999_999_999,   // RECID_DAYCNT_EXP,       1日に稼いだ経験値                     999,999,999  
    MAX_999_999,       // RECID_GTS_PUT,          GTSにポケモンを預けた回数             999,999
    MAX_999_999,       // RECID_NICKNAME,         ポケモンにニックネームを付けた回数    999,999
    MAX_999_999,       // RECID_PREMIUM_BALL,     もらったプレミアボールの数            999,999
    MAX_999_999_999,   // RECID_WIN_BP,           勝ち取ったBP累計                      999,999,999
    MAX_999_999_999,   // RECID_USE_BP,           使ったBP                              999,999,999
    MAX_999_999_999,   // RECID_WEAK_ATTACK,          効果がバツグンのわざが命中した回数        999,999,999  
    MAX_999_999_999,   // RECID_CLOTHING_CNT,         着替えた回数                              999,999,999  30
    MAX_999_999_999,   // RECID_HAIR_ARRANGE,         美容室でヘアアレンジした回数              999,999,999
    MAX_999_999_999,   // RECID_BERRY_HARVEST,        畑で木の実を収穫した回数                  999,999,999
    MAX_999_999_999,   // RECID_GTS_TRADE_COUNT,        GTSで通信交換をした回数                 999,999,999
    MAX_999_999_999,   // RECID_MIRACLE_TRADE_COUNT,    ミラクル交換で通信交換をした回数       999,999,999
    MAX_999_999_999,   // RECID_QUICKMATCH_COUNT,        クイックマッチ回数                       999,999,999
    MAX_999_999_999,   // RECID_RIDE_POKE,               ポケモンライド回数                       999,999,999
    MAX_999_999_999,   // RECID_MAME_PRESENT_CNT,        ポケモンに豆をあげた回数                 999,999,999
    MAX_999_999_999,   // RECID_JF_USE_JCOIN,            ジョインフェスタで使ったコイン累計       999,999,999
    MAX_999_999_999,   // RECID_MAME_GET_CNT,            ポケマメを集めた累計                     999,999,999  
    MAX_999_999_999,   // RECID_BTREE_CHALLENGE,         バトルツリーに挑戦した回数               999,999,999  40
    MAX_999_999,       // RECID_ZWAZA_CNT,				       Ｚワザをくりだした回数                             999,999
    MAX_999_999,       // RECID_THROW_BALL,				       モンスターボールを投げた回数                       999,999
    MAX_999_999,       // RECID_STEAL_ITEM,				       野生のポケモンからアイテムを盗んだ回数             999,999
    MAX_999_999_999,   // RECID_WAZA_CNT,				         技を使用した回数                                   999,999,999
    MAX_999_999_999,   // RECID_SODATEYA_LVUP_CNT,			 育て屋に預けたポケモンの累計ＬＶアップ数           999,999,999
    MAX_999_999_999,   // RECID_ESCAPE,				           野生のポケモンから逃げた回数                       999,999,999
    MAX_999_999,       // RECID_BRAKE_ITEM,				       岩を壊してアイテムを入手した回数                   999,999
    MAX_999_999,       // RECID_HEAL_KIDOU,				       回復アイテムを使用した回数                         999,999
    MAX_999_999_999,   // RECID_NEKONIKOBAN,				     ネコに小判で取得したお金の金額                     999,999,999
    MAX_999_999_999,   // RECID_FINDER_POINT,				     ポケファインダーの総得点                           999,999,999  50
    MAX_999_999,       // RECID_GURUGURU,				         ぐるぐる回った回数                                 999,999
    MAX_999_999,       // RECID_FINDER_RECORD,				   １枚の写真の最高得点                               999,999
    MAX_999_999,       // RECID_PARLE_NADE,				       パルレでなでた回数                                 999,999
    MAX_999_999,       // RECID_POKERESORT,				       ポケリゾートの起動回数                             999,999
    MAX_999_999,       // RECID_POKEMAME_TRADE,				   ポケリゾートでポケマメを交換した数                 999,999
    MAX_999_999,       // RECID_POKEMON_TOUCH,				   ポケリゾートでポケモンをタッチした数               999,999
    MAX_999_999,       // RECID_POKEMAME_IN,				     ポケマメいれにポケマメを入れた数                   999,999
    MAX_999_999,       // RECID_POKERESORT_LVUP,				 ポケリゾートでＬＶをあげたときの累計               999,999
    MAX_999_999,       // RECID_POKERESORT_NATSUKI,		   ポケリゾートでなつき度を上げた累計                 999,999
    MAX_999_999,       // RECID_POKERESORT_TAMAGOFUKA,	 ポケリゾートでタマゴ孵化歩数を下げた累計           999,999  60
    MAX_999_999,       // RECID_POKERESORT_KISO,				 ポケリゾートで基礎ポイントをあげたときの累計       999,999
    MAX_999_999,       // RECID_QR_READ_BATTLE,				   ＱＲ読み込みアプリ：ＱＲバトルチームを読み込んだ数 999,999
    MAX_999_999,       // RECID_BATTLEMOVIE,				     バトルビデオの再生回数                             999,999
    MAX_999_999,       // RECID_BATTLEMOVIE_FIGHT,			 バトルビデオで模擬線をした回数                     999,999
    MAX_999_999,       // RECID_ROTOM,				           ロトム図鑑を触った回数                             999,999  65
    MAX_9_999_999,     // RECID_JF_GEST_TALK_CNT,        ジョインフェスタで交流イベントをした回数           9,999,999 
    MAX_999_999,       // RECID_NUTS,                    集まった木の実を調べた回数                         999,999
    MAX_999_999,       // RECID_MANY_NUTS,               すごく集まった木の実を調べた回数                   999,999
    MAX_999_999,       // RECID_FISHING_ITEM,            アイテムを釣り上げた回数                           999,999
    MAX_999_999_999,   // RECID_ROTOPON_CNT,             ロトポンした回数                                   999,999,999
    MAX_999_999_999,   // RECID_RR_EVENT_FLAG,           レインボーロケット団イベントクリアフラグ           999,999,999
    MAX_999_999_999,   // RECID_NUSIIRU_CNT,             ヌシール取得数                                     999,999,999
    MAX_999_999_999,   // RECID_LARGE_MANTAINBP,         マンタインサーフで稼いだBP                         999,999,999
    MAX_999_999_999,   // RECID_LARGE_BATTLEFES_WIN,     バトルフェスで勝利した回数                         999,999,999

    // 4バイトレコード余りの最大値
    MAX_999_999, MAX_999_999, MAX_999_999, MAX_999_999, MAX_999_999, // LARGE_75 - 79
    MAX_999_999, MAX_999_999, MAX_999_999, MAX_999_999, MAX_999_999, // LARGE_80 - 84
    MAX_999_999, MAX_999_999, MAX_999_999, MAX_999_999, MAX_999_999, // LARGE_85 - 89
    MAX_999_999, MAX_999_999, MAX_999_999, MAX_999_999, MAX_999_999, // LARGE_90 - 94
    MAX_999_999, MAX_999_999, MAX_999_999, MAX_999_999, MAX_999_999, // LARGE_95 - 99

    // ここから２バイトレコードの最大値
    MAX_9999,          // RECID_DENDOU_CNT               殿堂入りした回数                         9,999  0
    MAX_9999,          // RECID_RECOVERY_HOME,           自分の家で休んだ回数                     9,999
    MAX_9999,          // RECID_WAZA_HANERU,             「はねる」を使った回数                   9,999
    MAX_9999,          // RECID_WAZA_WARUAGAKI,          「わるあがき」を使った回数               9,999
    MAX_9999,          // RECID_WAZA_MUKOU,              無効果わざを出した回数                   9,999
    MAX_9999,          // RECID_TEMOTI_KIZETU,           気絶させた手持ちの匹数                   9,999
    MAX_9999,          // RECID_TEMOTI_MAKIZOE,          味方にダメージを与えた回数               9,999
    MAX_9999,          // RECID_NIGERU_SIPPAI,           逃げそこなった回数                       9,999
    MAX_9999,          // RECID_NIGERARETA_FLAG,         逃げられたポケモンの数                   9,999
    MAX_9999,          // RECID_FISHING_FAILURE,         つり損なった回数                         9,999
    MAX_9999,          // RECID_DAYMAX_KILL,             1日に倒したポケモンの最高数              9,999  10
    MAX_9999,          // RECID_DAYCNT_KILL,             1日に倒したポケモンの数                  9,999
    MAX_9999,          // RECID_DAYMAX_CAPTURE,          1日に捕獲したポケモンの最高数            9,999
    MAX_9999,          // RECID_DAYCNT_CAPTURE,          1日に捕獲したポケモンの数                9,999
    MAX_9999,          // RECID_DAYMAX_TRAINER_BATTLE,   1日にトレーナー戦した最高回数            9,999
    MAX_9999,          // RECID_DAYCNT_TRAINER_BATTLE,   1日にトレーナー戦した回数                9,999
    MAX_9999,          // RECID_DAYMAX_EVOLUTION,        1日にポケモン進化させた最高回数          9,999
    MAX_9999,          // RECID_DAYCNT_EVOLUTION,        1日にポケモン進化させた回数              9,999
    MAX_9999,          // RECID_FOSSIL_RESTORE,          ポケモンの化石を復元した回数             9,999
    MAX_9999,          // RECID_CAMERA_CNT,              カメラで写真撮影した回数                 9,999     
    MAX_9999,          // RECID_BTREE_WIN_SINGLE,            ///< バトルツリー最高連勝数・シングル         9,999 20
    MAX_9999,          // RECID_BTREE_WIN_DOUBLE,            ///< バトルツリー最高連勝数・ダブル           9,999
    MAX_9999,          // RECID_BTREE_WIN_MULTI,             ///< バトルツリー最高連勝数・マルチ           9,999
    MAX_9999,          // RECID_IDKUJI_ATARI_CNT,        ＩＤくじに当たった回数                   9,999
    MAX_9999,          // RECID_POINTUP_USE,             ポイントアップを使った回数               9,999
    MAX_9999,          // RECID_KAWAIGARI_CNT,           ポケモンかわいがり画面を開いた回数       9,999
    MAX_999,           // RECID_FISHING_SUCCESSION,      釣り・連続成功回数最高記録               999
    MAX_9999,          // RECID_RARE_COLOUR_ENC_COUNT,         色違いのポケモンに出会った回数     65,535  
    MAX_9999,          // RECID_JF_ATT_CNT,                  ///< ジョインフェスタでアトラクションに参加した回数  9,999
    MAX_9999,          // RECID_JF_SHOP_GET_CNT,             ///< ジョインフェスタでお店を建てた回数              9,999
    MAX_9999,          // RECID_QR_SCAN,                      QRスキャンした回数                                          9,999 30
    MAX_9999,          // RECID_LERAN_WAZA,                   技マシンを覚えさせた回数                                    9,999
    MAX_9999,          // RECID_CAFE,                         ポケモンセンター横のCAFÉで注文した回数                      9,999
    MAX_9999,          // RECID_FINDER,                       自分のプロフィール写真を取り直した回数                      9,999
    MAX_9999,          // RECID_CANCEL_EVOLUTION,             進化キャンセルの回数                                        9,999
    MAX_9999,          // RECID_RANNNYU,                      乱入回数                                                    9,999
    MAX_9999,          // RECID_NAKAYOSHI,                    なかよし度ＭＡＸのポケモンの数                              9,999
    MAX_9999,          // RECID_BATTLEROYAL,                  バトルロイヤルを行った回数                                  9,999
    MAX_9999,          // RECID_MONOHIROI,                    『ものひろい』でアイテムを拾った回数                        9,999
    MAX_9999,          // RECID_MARASADA,                     マラサダを食べた回数                                        9,999 
    MAX_9999,          // RECID_SUGOI_TRAINING,               すごい特訓の回数                                            9,999 40
    MAX_9999,          // RECID_BATTLE_VIKING,                バトルバイキングで食事をした回数                            9,999
    MAX_9999,          // RECID_PARLE_KIDOU,                  ポケパルレ_起動回数                                         9,999
    MAX_9999,          // RECID_BOX_CONTROL,                  ポケモンボックスにアクセスした回数                          9,999
    MAX_9999,          // RECID_LOMILOMI,                     ロミロミをした回数                                          9,999
    MAX_9999,          // RECID_IRIMA_BED,                    イリマのベッドのにおいを嗅いだ回数                          9,999
    MAX_9999,          // RECID_GUZMA_BED,                    グズマのベッドのにおいを嗅いだ回数                          9,999
    MAX_9999,          // RECID_KIAVE_BED,                    キアヴェのベッドのにおいを嗅いだ回数                        9,999
    MAX_9999,          // RECID_SUREIN_BED,                   スレインのベッドのにおいを嗅いだ回数                        9,999
    MAX_9999,          // RECID_MAOS_BED,                     マオのベッドのにおいを嗅いだ回数                            9,999 
    MAX_9999,          // RECID_MAIRE_BED,                    マイレのベッドのにおいを嗅いだ回数                          9,999 50
    MAX_9999,          // RECID_HAPU_BED,                     ハプウのベッドのにおいを嗅いだ回数                          9,999
    MAX_9999,          // RECID_ZAIDANBOSS_BED,               財団代表のベッドのにおいを嗅いだ回数                        9,999
    MAX_9999,          // RECID_DROP_ITEM,                    ドロップアイテム入手回数                                    9,999
    MAX_9999,          // RECID_KIYURE,                       木揺れエンカウント回数                                      9,999
    MAX_9999,          // RECID_DANSA,                        段差ジャンプ回数                                            9,999
    MAX_9999,          // RECID_UMI_ENCOUNT,                  海の特殊エンカウント回数                                    9,999
    MAX_9999,          // RECID_SABAKU_ENCOUNT,               砂漠の特殊エンカウント回数                                  9,999
    MAX_9999,          // RECID_KISEKAE_CNT,                  着せ替えの回数                                              9,999
    MAX_9999,          // RECID_BATTLEROYAL_WIN,              バトルロイヤルの勝利数                                      9,999 
    MAX_9999,          // RECID_RESORT_ITEM,                  ポケリゾートでどうぐを入手した回数                          9,999 60
    MAX_9999,          // RECID_RESORT_WAIWAI,                ポケリゾート：わいわいリゾートに誘ったポケモンの回数        9,999
    MAX_9999,          // RECID_RESORT_POKAPOKA,              ポケリゾート：ぽかぽかリゾートに誘ったポケモンの回数        9,999
    MAX_9999,          // RECID_QR_READ_PICTURE,              ＱＲスキャン：ＱＲコード（図鑑ＱＲ）を読み込んだ回数        9,999
    MAX_9999,          // RECID_QR_READ_POPULAR,              ＱＲスキャン：ＱＲコード（一般）を読み込んだ回数            9,999
    MAX_9999,          // RECID_QR_READ_OUTSIDE,              ＱＲスキャン：ＱＲコード（外部連携系）を読み込んだ回数      9,999
    MAX_9999,          // RECID_FIELDSCAN,                    ＱＲスキャン：フィールドスキャンを発動した回数              9,999
    MAX_9999,          // RECID_PLANT,                        植物からポケモンが飛び出してきた回数                        9,999
    MAX_9999,          // RECID_SHADOW,                       影からポケモンが飛び出してきた回数                          9,999
    MAX_9999,          // RECID_WEED,                         草のシンボルでポケモンと戦った回数                          9,999
    MAX_9999,          // RECID_GROUND,                       土のシンボルでポケモンと戦った回数                          9,999 70
    MAX_9999,          // RECID_KOSOKU,                       逃げるコソクムシに追いついた回数                            9,999
    MAX_9999,          // RECID_YASHIGANI,                    木の実からポケモンが飛び出してきた回数（ヤシガニ）          9,999
    MAX_9999,          // RECID_FISHING_BUBBLE,               泡の出ている釣りスポットで釣りを成功させた回数              9,999
    MAX_9999,          // RECID_PLAYER_BED,                   主人公のベッドのにおいを嗅いだ回数                          9,999
    MAX_9999,          // RECID_GLOBAL_ATTRACTION_CAPTURE,         グローバルアトラクション01　　　　　　　　　 　　　　　　　9,999
    MAX_9999,          // RECID_GLOBAL_ATTRACTION_02,         グローバルアトラクション02　　　　　　　　　 　　　　　　　9,999
    MAX_9999,          // RECID_GLOBAL_ATTRACTION_03,         グローバルアトラクション03　　　　　　　　　 　　　　　　　9,999
    MAX_9999,          // RECID_GLOBAL_ATTRACTION_04,         グローバルアトラクション04　　　　　　　　　 　　　　　　　9,999
    MAX_9999,          // RECID_GLOBAL_ATTRACTION_05,         グローバルアトラクション05　　　　　　　　　 　　　　　　　9,999
    MAX_9999,          // RECID_GLOBAL_ATTRACTION_06,         グローバルアトラクション06　　　　　　　　　 　　　　　　　9,999
    MAX_9999,          // RECID_GLOBAL_ATTRACTION_07,         グローバルアトラクション07　　　　　　　　　 　　　　　　　9,999
    MAX_9999,          // RECID_GLOBAL_ATTRACTION_08,         グローバルアトラクション08　　　　　　　　　 　　　　　　　9,999
    MAX_9999,          // RECID_GLOBAL_ATTRACTION_09,         グローバルアトラクション09　　　　　　　　　 　　　　　　　9,999
    MAX_9999,          // RECID_GLOBAL_ATTRACTION_10,         グローバルアトラクション10　　　　　　　　　 　　　　　　　9,999
    MAX_9999,          // RECID_GLOBAL_ATTRACTION_11,         グローバルアトラクション11　　　　　　　　　 　　　　　　　9,999
    MAX_9999,          // RECID_GLOBAL_ATTRACTION_BATTLETREE,         グローバルアトラクション12　　　　　　　　　 　　　　　　　9,999
    MAX_9999,          // RECID_LILIE_BED,                    リーリエのベッドでリラックスをした回数                       9,999  
    MAX_9999,          // RECID_LUNASOL_DIVE_CNT,                 ///< ルナソルダイブの回数			9,999
    MAX_9999,          // RECID_MANTASURF_RIDE_CNT,               ///< マンタインサーフに乗った回数			9,999
    MAX_9999,          // RECID_FINDERSTUDIO_FILM_CNT,            ///< ファインダースタジオで写真を保存した回数			9,999
    MAX_9999,          // RECID_BATTLEFES_CHALLENGE_CNT,          ///< バトルフェスに挑戦した回数			9,999
    MAX_65535,         // RECID_FIELD_POKE_CNT,                   ///< フィールドポケモンとふれあった回数			65,535
    MAX_65535,         // RECID_SHIREN_OKAWARI_PAZZLE_CNT,        ///< 島３試練後のパズル挑戦回数			65,535
    MAX_65535,         // RECID_SHIREN_OKAWARI_PAZZLE_SUCCESS_CNT,///< 島３試練後のパズルクリア回数			65,535
    MAX_9999,          // RECID_MYALBUM_DECO_CNT,                 ///< マイアルバムデコった回数			9,999
    MAX_9999,          // RECID_FINDER_STUDIO_SHUT_CNT,           ///< ファインダースタジオで撮影した回数			9,999
    MAX_65535,         // RECID_LUNASOL_BESTSCORE,                ///< ルナソルダイブのベストスコア距離		65,535
    MAX_9999,          // RECID_MANTAIN_SURF_BEST_BP,             ///< マンタインサーフベストBP		9,999
    // 2バイトレコード余りの最大値

    MAX_9999,   // RECID_SMALL_95 - 99,

    /// ↑↑↑ レコード定義を増やしたらここも増やすこと！
  };

  /// 最大値タイプのテーブル
  static const u32 max_tbl[MAX_TYPE_MAX]={
    999999999,  // MAX_999_999_999
    9999999,    // MAX_9_999_999
    999999,     // MAX_999_999
    99999,      // MAX_99999
    65535,      // MAX_65535
    9999,       // MAX_9999
    999,        // MAX_999
    7,          // MAX_7
  };

  /// レコードの最大値タイプの取得
  int index = limit_tbl[id];

  // idが定義範囲を超えている
  if(id>=RECID_ALL_MAX){
    return 0;
  }
  // max_tblで引っ張れる数値の範囲を超えている
  if( index>= MAX_TYPE_MAX ){
    return 0;
  }

  // そのRECORD定義で扱える最大値
  return max_tbl[index];
}

//--------------------------------------------------------------------------
/// 値格納
//--------------------------------------------------------------------------
u32 Record::set_value( RECORD_ID id, u32 value )
{
  if (id < RECID_LARGE_MAX) {
    m_data.large_rec[id] = value;
  } else if (id < RECID_ALL_MAX ) {
    m_data.small_rec[id - RECID_LARGE_MAX] = value;
  }else{
//    GFL_ASSERT_MSG(0, "record: id error %d\n", id);
    GFL_ASSERT(0);
    return 0;
  }
  return get_value( id );
}

//--------------------------------------------------------------------------
/// 値取得
//--------------------------------------------------------------------------
u32  Record::get_value( RECORD_ID id )
{
  if (id < RECID_LARGE_MAX) {
    return m_data.large_rec[id];
  } else if (id < RECID_ALL_MAX ) {
    return m_data.small_rec[id - RECID_LARGE_MAX];
  }
//  GFL_ASSERT_MSG(0, "record: id error %d\n", id);
  GFL_ASSERT(0);
  return 0;
}

//--------------------------------------------------------------------------
/// 書き込み禁止のON/OFF( true:書き込み禁止 )
//--------------------------------------------------------------------------
void Record::SetDisableRecord( bool flag )
{
  m_disable = flag;
}

//--------------------------------------------------------------------------
/// 書き込み禁止期間チェック( true:書き込める false:書き込めない）
//--------------------------------------------------------------------------
bool Record::check_enable( int id, int operate, u32 value )
{
  if(m_disable){
#if PM_DEBUG
    const char *operate_tbl[]={ "add", "set", "setIfLarge"  };
    GFL_ASSERT_MSG( 0,"%d:Disable Write %s %d\n", id, operate_tbl[operate], value );
#endif
    return false;
  }
  return true;
}




GFL_NAMESPACE_END(Savedata)
