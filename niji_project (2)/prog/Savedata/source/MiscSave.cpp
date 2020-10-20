//============================================================================================
/**
 * @file    Misc.cpp
 * @brief   未分類セーブデータ
 * @author  taya
 * @date    2012.03.13
 *
 * namespace  savedata::
 *
 *
 * 大きな分類項目でくくれない、未分類のこまかなセーブデータを保存するためのワークです。
 *
 * @fix BTS:828 SCameraSaveDataを追加
 */
//============================================================================================

#include "Savedata/include/MiscSave.h"

#include <debug/include/gfl2_Assert.h>

#include "System/include/PokemonVersion.h"


#include "PokeTool/include/RealRegionPokemon.h"

GFL_NAMESPACE_BEGIN(Savedata)


//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------------------------------------
Misc::Misc(void)
{
  Clear( NULL );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------------------------------------
Misc::~Misc()
{
  // 何もしない
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ワークをクリアする
 */
//--------------------------------------------------------------------------------------------
void Misc::Clear( gfl2::heap::HeapBase * heap )
{
  gfl2::std::MemClear( &m_data, sizeof(m_data) );

  // 学習装置は最初からONの状態（ただしアイテムを持っていないと使えない）
  m_data.gakusyuusoutiFlag = true;


  // 言語別で文字入力の初期モードを設定
  u8 lang = System::GetLang();
  if( lang == POKEMON_LANG_JAPAN || lang == POKEMON_LANG_KOREA )
  {
    m_data.strinputMode = 0;
  }
  else
  {
    m_data.strinputMode = 1;
  }

  for( u8 i=0; i<EVENT_LIST_TYPE_MAX; ++i )
  {
    m_data.eventlist_target_event[i] = EVENT_LIST_TARGET_EVENT_NONE;
  }

  if ( heap )
  {
    //リアル地域ポケモンフォルムナンバー決定
    SetRealRegionFormNo( DecideRealRegion( heap ) );
  }

  // トレーナーパススタンプ
  SetTrainerPassStamp( STAMP_FLAG_ALOLA );

  //１００％レアカラー引くフラグのリセット
  ResetMustRareEncTrigger();

/*
  // フィールドカメラセーブデータ初期化
  field::SCameraSaveData_Init( &m_data.cameraAreaSave );

  // 配信ひみつきち拡張データ初期化
  this->GetSecretBaseDeliveryBaseExtendDataAccessor().ClearData();
*/
}

//----------------------------------------------------------
/**
 * @brief 持っている金額を取得する
 * @param my    自分状態保持ワークへのポインタ
 * @return  u32   持っている金額
 */
//----------------------------------------------------------
u32 Misc::GetGold(void) const
{
  return m_data.gold;
}

//----------------------------------------------------------
/**
 * @brief   お金を設定する
 * @param gold  セットする金額
 * @return  u32   現在の手持ち金額
 */
//----------------------------------------------------------
u32 Misc::SetGold(u32 gold)
{
  if (gold > MY_GOLD_MAX) {
    gold = MY_GOLD_MAX;
  }
  m_data.gold = gold;
  return m_data.gold;
}

//----------------------------------------------------------
/**
 * @brief  お金を増やす
 * @param add   加える金額
 * @return  u32   現在の手持ち金額
 */
//----------------------------------------------------------
u32 Misc::AddGold(u32 add)
{
  if (add > MY_GOLD_MAX) {
    m_data.gold = MY_GOLD_MAX;
  } else {
    m_data.gold += add;
  }
  if (m_data.gold > MY_GOLD_MAX) {
    m_data.gold = MY_GOLD_MAX;
  }
  return m_data.gold;
}

//----------------------------------------------------------
/**
 * @brief    お金を減らす
 * @param sub   引き出す金額
 * @return  u32   現在の手持ち金額
 */
//----------------------------------------------------------
u32 Misc::SubGold(u32 sub)
{
  if (m_data.gold < sub) {
    m_data.gold = 0;
  } else {
    m_data.gold -= sub;
  }
  return m_data.gold;
}

//-----------------------------------------------------------------------------
/**
 * @brief   学習装置の起動フラグをセット
 *
 * @param   flag  true = ON, false = OFF
 */
//-----------------------------------------------------------------------------
void Misc::SetGakusyuusoutiFlag( bool flag )
{
	m_data.gakusyuusoutiFlag = flag;
}

//-----------------------------------------------------------------------------
/**
 * @brief   学習装置の起動フラグを取得
 *
 * @retval  true  = ON
 * @retval  false = OFF
 */
//-----------------------------------------------------------------------------
bool Misc::GetGakusyuusoutiFlag( void ) const
{
	return m_data.gakusyuusoutiFlag;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   バッジ個数を取得する
 */
//--------------------------------------------------------------------------------------------
u8  Misc::GetBadgeCount( void ) const
{
  u32 count = 0;
  u32 badge;
  
  for (badge = m_data.badge; badge != 0; badge >>= 1) 
  {
    if (badge & 1)
    {
      count++;
    }
  }
  
  return count;
}
//--------------------------------------------------------------------------------------------
/**
 * @brief   バッジ個数を設定する
 *
 * @param   count   設定する個数
 */
//--------------------------------------------------------------------------------------------
void  Misc::SetBadgeCount( u8 count )
{
  if( count <= BADGE_COUNT_MAX ){
    u32 badge = 0;
    
    for (u32 i = 0; i < count; i++ )
    {
      badge |= 1 << i;
    }
    
    m_data.badge = badge;
  }
  else{
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------
/**
 * @brief   敗北時に失う金額を計算する
 * @param   level_max   手持ちポケモンで最大のレベル
 * @return  u32         失う金額
 */
//----------------------------------------------------------
u32 Misc::CalcPenaltyMoney( u32 level_max ) const
{
  // 手持ちポケモンのかなの最大レベルｘ４ｘ主人公係数
  // （スタンプの個数で係数は変わる）
  static const u8 sc_StampPenalty[ ISLAND_STAMP_NUM + 1 ] =
  {
    2, 4, 8, 14, 22, 30,
  };
  u32 stamp_num = this->GetIslandStampNum();
  u32 money = level_max * 4 * sc_StampPenalty[ stamp_num ];
  u32 gold = this->GetGold();
  if( gold <= money ){
    return gold;
  }else{
    return money;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メガ進化を見たか？のフラグ列を取得する
 * @param[out]  dst     メガ進化フラグをコピーする領域へのポインタ
 */
//--------------------------------------------------------------------------------------------
void Misc::GetMegaSeeFlags( u8 * dst ) const
{
  gfl2::std::MemCopy( m_data.mega_see_flag, dst, MEGA_SEE_AREA_SIZE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メガ進化を見たか？のフラグ列をセットする
 * @param[in] src     取得するメガ進化フラグ列へのポインタ
 */
//--------------------------------------------------------------------------------------------
void Misc::SetMegaSeeFlag( const u8 * src )
{
  gfl2::std::MemCopy( src, m_data.mega_see_flag, MEGA_SEE_AREA_SIZE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   文字入力画面のパラメータをセット
 *
 * @param   mode  入力モード
 */
//--------------------------------------------------------------------------------------------
void Misc::SetStrInputParam( u32 mode )
{
  m_data.strinputMode = mode;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   文字入力画面のパラメータを取得
 *
 * @param   mode  入力モード
 */
//--------------------------------------------------------------------------------------------
void Misc::GetStrInputParam( u32 * mode ) const
{
  *mode = m_data.strinputMode;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   イベントリストの既読チェック
 *
 * @param   index   イベントのインデックス
 */
//--------------------------------------------------------------------------------------------
bool Misc::IsEventFirstCheck( u32 index ) const
{
  if( index >= EVENT_LIST_BIT_MAX )
  {
    GFL_ASSERT( 0 );
    return true;  // 既読で返す
  }
  if( ( m_data.eventlist_first_check[index/8] & (1<<(index%8)) ) != 0 )
  {
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   イベントリストの既読チェック
 *
 * @param   index   イベントのインデックス
 */
//--------------------------------------------------------------------------------------------
void Misc::SetEventFirstCheck( u32 index )
{
  if( index >= EVENT_LIST_BIT_MAX )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_data.eventlist_first_check[index/8] |= ( 1 << (index%8) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   イベントリストのターゲットイベントIDの取得
 *
 * @param   listType    リストの種類(メイン or サブ)
 *
 * @return  ターゲットイベントID
 *
 * @note    マップに表示するイベントアイコンに対応するイベントIDを取得
 *          EVENT_LIST_TARGET_EVENT_NONE ならアイコンを非表示
 */
//--------------------------------------------------------------------------------------------
u32 Misc::GetTargetEventID( u32 listType ) const
{
  return m_data.eventlist_target_event[listType];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   イベントリストのターゲットイベントIDの設定
 *
 * @param   listType    リストの種類(メイン or サブ)
 * @param   eventID     イベントID
 *
 * @note    マップに表示するイベントアイコンに対応するイベントIDを設定
 *          表示しない場合は EVENT_LIST_TARGET_EVENT_NONE を設定
 */
//--------------------------------------------------------------------------------------------
void Misc::SetTargetEventID( u32 listType, u32 eventID )
{
  m_data.eventlist_target_event[listType] = eventID;
}

//--------------------------------------------------------------------------
/**
 *	@brief  バトルレコーダー初回チュートリアルフラグ設定
 */
//--------------------------------------------------------------------------
void Misc::SetBattleVideoPlayerTutorial( void )
{
  m_data.battle_video_player_tutorial = 1;
}

//--------------------------------------------------------------------------
/**
 *	@brief  バトルレコーダー初回チュートリアルフラグ取得
 * 
 * @return true:チュートリアル終了した false:していない
 */
//--------------------------------------------------------------------------
bool Misc::GetBattleVideoPlayerTutorial( void ) const
{
  return m_data.battle_video_player_tutorial;
}

//--------------------------------------------------------------------------
/**
 *	@brief  かわいがり初回チュートリアルフラグ設定
 */
//--------------------------------------------------------------------------
void Misc::SetKawaigariTutorial( void )
{
  m_data.kawaigari_tutorial = 1;
}

//--------------------------------------------------------------------------
/**
 *	@brief  かわいがり初回チュートリアルフラグ取得
 * 
 * @return true:チュートリアル終了した false:していない
 */
//--------------------------------------------------------------------------
bool Misc::GetKawaigariTutorial( void ) const
{
  return m_data.kawaigari_tutorial;
}


//----------------------------------------------------------
/**
 * @brief     BPを取得する
 * @return    現在のBP
 */
//----------------------------------------------------------
u32 Misc::GetBP(void) const
{
  return m_data.BattlePoint;
}
//----------------------------------------------------------
/**
 * @brief     BPをセットする
 * @param[in]   value   セットする値
 * @return      現在のBP
 */
//----------------------------------------------------------
u32 Misc::SetBP(u32 value)
{
  if ( value > BP_MAX ) value = BP_MAX;
  m_data.BattlePoint = value;
  return value;
}

//----------------------------------------------------------
/**
 * @brief   トレーナーパスのスタンプをセット
 * @param   id  スタンプのフラグID
 */
//----------------------------------------------------------
void Misc::SetTrainerPassStamp( TrainerPassStampID id )
{
  m_data.trainer_pass_stamp |= ( 1 << id );
}

//----------------------------------------------------------
/**
 * @brief   トレーナーパスのスタンプをリセット
 * @param   id  スタンプのフラグID
 */
//----------------------------------------------------------
void Misc::ResetTrainerPassStamp( TrainerPassStampID id )
{
  if( IsTrainerPassStamp( id ) != false )
  {
    m_data.trainer_pass_stamp ^= ( 1 << id );
  }
}

//----------------------------------------------------------
/**
 * @brief   トレーナーパスのスタンプを持っているか
 * @param   id  スタンプのフラグID
 * @retval  true  = 持っている
 * @retval  false = それ以外
 */
//----------------------------------------------------------
bool Misc::IsTrainerPassStamp( TrainerPassStampID id ) const
{
  if( ( m_data.trainer_pass_stamp & (1<<id) ) != 0 )
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------
/**
 * @brief 島めぐりスタンプの所持数を取得する
 * @retval [0, ISLAND_STAMP_NUM]
 */
//----------------------------------------------------------
u32 Misc::GetIslandStampNum( void ) const
{
  static const TrainerPassStampID STAMP_LIST[ ISLAND_STAMP_NUM ] = 
  {
    Savedata::STAMP_FLAG_ISLAND_01_CLEAR,  
    Savedata::STAMP_FLAG_ISLAND_02_CLEAR,  
    Savedata::STAMP_FLAG_ISLAND_03_CLEAR,  
    Savedata::STAMP_FLAG_ISLAND_04_CLEAR,  
    Savedata::STAMP_FLAG_ISLAND_ALL_CLEAR, 
  };

  u32 stampNum = 0;

  for( u32 i=0; i<ISLAND_STAMP_NUM; ++i )
  {
    if( this->IsTrainerPassStamp( STAMP_LIST[i] ) )
    {
      stampNum++;
    }
  }

  return stampNum;
}

//----------------------------------------------------------
/**
  * @brief   最後のkawaigariからの経過日数関連
  */
//----------------------------------------------------------
void Misc::AddKawaigariPastDays(u8 day)
{
  m_data.kawaigariPastDay = gfl2::math::Clamp(m_data.kawaigariPastDay+day,0,255);
}

void Misc::ResetKawaigariPastDays(void)
{
  m_data.kawaigariPastDay = 0;
}

u8   Misc::GetKawaigariPastDays(void)const
{
  return m_data.kawaigariPastDay;
}

/**
  * @brief   アローラオンリーフェイクタイム設定
  */
void Misc::SetAlolaOnlyFakeTime( b8 flag, u8 hour, u8 minute, u8 second )
{
  m_data.alola_only_fake_time         = flag;
  m_data.alola_only_fake_time_hour    = hour;
  m_data.alola_only_fake_time_minute  = minute;
  m_data.alola_only_fake_time_second  = second;
}

//-----------------------------------------------------------------------------
/**
 * @brief  地域ポケモンのフォルムを決定する
 * @param  pHeap
 * @return u8    フォルムナンバー
 */
//-----------------------------------------------------------------------------
u8 Misc::DecideRealRegion( gfl2::heap::HeapBase * pHeap )
{
  u8 form_no = 0;
#if defined(GF_PLATFORM_CTR)
  // SimpleAddressのサイズは4KB以上あるのでスタック確保は避ける
  nn::cfg::CTR::SimpleAddress* simpleAddress = GFL_NEW_LOW(pHeap) nn::cfg::CTR::SimpleAddress();
  if( simpleAddress != NULL )
  {
    // 緯度/経度情報を取得
    nn::cfg::CTR::GetSimpleAddress( simpleAddress );
    u16 longitude = simpleAddress->longitude;
    u16 latitude  = simpleAddress->latitude;
    form_no = PokeTool::RealRegion::DecideRegion( latitude, longitude );
  }
  GFL_SAFE_DELETE( simpleAddress );
#endif
  return form_no;
}

//--------------------------------------------------------------------------
/**
 * @brief   リアル地域ポケモン情報を取得する
 * @return  u32 リアル地域ポケモンのフォルム値
 */
//--------------------------------------------------------------------------
u32 Misc::GetRealRegionFormNo( void ) const
{
  return m_data.realRegionFormWork;
}

//--------------------------------------------------------------------------
/**
 * @brief   リアル地域ポケモン情報を設定する
 * @param[in]  u32 リアル地域ポケモンのフォルム値
 */
//--------------------------------------------------------------------------
void Misc::SetRealRegionFormNo( u32 value )
{
  m_data.realRegionFormWork = value;
}

/**
* @brief   カメラデータ操作
*/
void Misc::SetFieldCameraData( u32 areaId, u32 areaDataNo, u32 oldAreaData )
{
  m_data.fieldCameraAreaId = areaId;
  m_data.fieldCameraAreaDataNo = areaDataNo;
  m_data.fieldCameraAreaOldDataNo = oldAreaData;
}

void Misc::GetFieldCameraData( u32* pAreaId, u32* pAreaDataNo, u32* pOldAreaData ) const
{
  *pAreaId = m_data.fieldCameraAreaId;
  *pAreaDataNo = m_data.fieldCameraAreaDataNo;
  *pOldAreaData = m_data.fieldCameraAreaOldDataNo;
}

void Misc::SetMustRareEncTrigger(void)
{
  m_data.must_rare_encont = 1;
}
void Misc::ResetMustRareEncTrigger(void)
{
  m_data.must_rare_encont = 0;
}
bool Misc::CheckMustRareEncTrigger(void) const
{
  if (m_data.must_rare_encont == 1 ) return true;
  return false;
}

void Misc::SetSurfRecord(u32 corse_id, u32 score)
{
  if (corse_id == Field::COURSE_ID_TUTORIAL) return;
  if (corse_id < Field::COURSE_NUM)
  {
    m_data.SurfRecord[corse_id] = score;
  }
  else
  {
    GFL_ASSERT(0);
  }
}

u32 Misc::GetSurfRecord(u32 corse_id)
{
  if (corse_id == Field::COURSE_ID_TUTORIAL) return 0;
  if (corse_id < Field::COURSE_NUM)
  {
    return m_data.SurfRecord[corse_id];
  }
  GFL_ASSERT(0);
  return 0;
}


//-----------------------------------------------------------------------------
/**
 * @brief   ロトポン関連のアイテムを確認したか
 *
 * @retval  true  = 確認した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool Misc::IsBagRotoponItemEnable(void)
{
  if( m_data.rotopon_item_enable == 1 )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ロトポン関連のアイテムを確認済みにする
 */
//-----------------------------------------------------------------------------
void Misc::SetBagRotoponItemEnable(void)
{
  m_data.rotopon_item_enable = 1;
}

//-----------------------------------------------------------------------------
/**
 * @brief   マイアルバムでUGCのメッセージを表示したか
 *
 * @retval  true  = 表示した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool Misc::IsMyAlbumUGCEnable(void)
{
  if( m_data.myalbum_ugc_enable == 1 )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief   マイアルバムのUGCのメッセージを表示済みにする
 */
//-----------------------------------------------------------------------------
void Misc::SetMyAlbumUGCEnable(void)
{
  m_data.myalbum_ugc_enable = 1;
}



#if 0
//-----------------------------------------------------------------------------
/**
 *	@brief  ライバル名を取得
 *
 *	@param	gfl::str::StrBuf* dst [OUT]文字列格納先
 */
//-----------------------------------------------------------------------------
void Misc::GetRivalName( gfl::str::StrBuf* dst ) const
{ 
  dst->SetStr( m_data.rivalname );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ライバル名を設定
 *
   *	@param	gfl::str::StrBuf* src [IN]代入する文字列
 */
//-----------------------------------------------------------------------------
void Misc::SetRivalName( gfl::str::StrBuf* src )
{ 
  src->PutStr( m_data.rivalname, gamesystem::STRLEN_PERSON_NAME + gamesystem::EOM_LEN );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ライバル名を設定
 *
 *  @param[in]  pTempHeap   作業用ヒープ
 *  @param[in]  sex         自分の性別
 */
//-----------------------------------------------------------------------------
void Misc::SetRivalName( gfl::heap::HeapBase * pTempHeap, u8 sex )
{
  u8 str_id = friend_name_hero;
  if ( sex == PM_MALE ) str_id = friend_name_heroine;

  gfl::str::StrBuf * strbuf = GFL_NEW(pTempHeap) gfl::str::StrBuf(
      gamesystem::STRLEN_PERSON_NAME + gamesystem::EOM_LEN, pTempHeap );
  gfl::str::MsgData* p_msgdata = GFL_NEW(pTempHeap) gfl::str::MsgData( 
      print::GetMessageArcId(), GARC_message_friendname_DAT, pTempHeap, gfl::str::MsgData::LOAD_PART );

  p_msgdata->GetString( str_id, *strbuf );
  this->SetRivalName( strbuf );
  
  GFL_DELETE p_msgdata;
  GFL_DELETE strbuf;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  バッジフラグの取得
 *
 *	@param	u16 badge_id バッジID
 */
//-----------------------------------------------------------------------------
bool Misc::GetBadgeFlag( u16 badge_id ) const
{
  if ( badge_id >= BADGE_COUNT_MAX )
  {
    GFL_ASSERT(0);
    return false;
  }
  
  if( m_data.badge & (1 << badge_id) )
  {
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  バッジフラグの設定
 *
 *	@param	u16 badge_id バッジID
 */
//-----------------------------------------------------------------------------
void Misc::SetBadgeFlag( u16 badge_id )
{
  if ( badge_id >= BADGE_COUNT_MAX )
  {
    GFL_ASSERT(0);
    return;
  }
 
  m_data.badge |= (1 << badge_id);
}

//-----------------------------------------------------------------------------
/**
 *	@brief  バッジフラグの設定
 *
 *	@param	u16 badge_id バッジID
 */
//-----------------------------------------------------------------------------
void Misc::ResetBadgeFlag( u16 badge_id )
{
  if ( badge_id >= BADGE_COUNT_MAX )
  {
    GFL_ASSERT(0);
    return;
  }
 
  u32 bit = (1 << badge_id); // ビットを立てて
  bit = ~bit; // ビット反転
  m_data.badge &= bit; // マスク
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		パソコン画面でリーグ制覇の壁紙追加メッセージを読んだかのフラグを設定
 *
 * @param		none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void Misc::SetPCMsgFlagLeague(void)
{
	m_data.pcLeagueMsgFlag = 1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		パソコン画面でチャンピオン撃破の壁紙追加メッセージを読んだかのフラグを設定
 *
 * @param		none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void Misc::SetPCMsgFlagChampion(void)
{
	m_data.pcChampionMsgFlag = 1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		パソコン画面でリーグ制覇の壁紙追加メッセージを読んだかのフラグを取得
 *
 * @param		none
 *
 * @retval	"true = みた"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool Misc::GetPCMsgFlagLeague(void) const
{
	if( m_data.pcLeagueMsgFlag == 1 ){
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		パソコン画面でチャンピオン撃破の壁紙追加メッセージを読んだかのフラグを取得
 *
 * @param		none
 *
 * @retval	"true = みた"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool Misc::GetPCMsgFlagChampion(void) const
{
	if( m_data.pcChampionMsgFlag == 1 ){
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモン用ランダムシード取得
 * @return	シード値
 */
//--------------------------------------------------------------------------------------------
u64 Misc::GetPokemonCreateRandomSeed(void) const
{
  return m_data.pokemonCreateRandomSeed;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモン用ランダムシード設定
 * @param	  シード値
 */
//--------------------------------------------------------------------------------------------
void Misc::SetPokemonCreateRandomSeed(u64 seed)
{
  m_data.pokemonCreateRandomSeed = seed;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void Misc::Set3DCameraMode( bool isExpert )
{
  m_data.b3DcameraMode = isExpert;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
bool Misc::Get3DCameraModeIsExpert( void ) const
{
  return m_data.b3DcameraMode;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief 下画面チュートリアル既読フラグON
 *
 * @param mode  モード（下画面チュートリアルのモード）
 */
//--------------------------------------------------------------------------------------------
void Misc::SetSubScreenTutorialWatched( u32 mode )
{
  switch( mode )
  {
  case app::sub_screen_tutorial::MODE_SPARRING:
    m_data.sub_screen_tutorial_sparring_watched = true;
    break;
  case app::sub_screen_tutorial::MODE_KAWAIGARI:
    m_data.sub_screen_tutorial_kawaigari_watched  = true;
    break;
  case app::sub_screen_tutorial::MODE_PSS:
    m_data.sub_screen_tutorial_pss_watched  = true;
    break;
  default:
    GFL_ASSERT(0);
  }
}
//--------------------------------------------------------------------------------------------
/**
 * @brief 下画面チュートリアル既読フラグ
 *
 * @param mode  モード（下画面チュートリアルのモード）
 *
 * @return true見た false見ていない
 */
//--------------------------------------------------------------------------------------------
bool Misc::IsSubScreenTutorialWatched( u32 mode ) const
{
  switch( mode )
  {
  case app::sub_screen_tutorial::MODE_SPARRING:
    return m_data.sub_screen_tutorial_sparring_watched;
  case app::sub_screen_tutorial::MODE_KAWAIGARI:
    return m_data.sub_screen_tutorial_kawaigari_watched;
  case app::sub_screen_tutorial::MODE_PSS:
    return m_data.sub_screen_tutorial_pss_watched;
  default:
    GFL_ASSERT(0);
    return false;
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief カメラエリアセーブデータを設定
 */
//--------------------------------------------------------------------------------------------
void Misc::SetCameraSaveData( const field::SCameraSaveData& data )
{
  gfl::std::MemCopy( &data, &m_data.cameraAreaSave, sizeof(field::SCameraSaveData) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief カメラエリアセーブデータを取得
 */
//--------------------------------------------------------------------------------------------
const field::SCameraSaveData& Misc::GetCameraSaveData( void ) const
{
  return m_data.cameraAreaSave;
}


//--------------------------------------------------------------------------
/**
 *	@brief  Oパワーアプリ初回チュートリアルフラグ設定
 */
//--------------------------------------------------------------------------
void Misc::SetApowerTutorial( void )
{
  m_data.apower_tutorial = 1;
}

//--------------------------------------------------------------------------
/**
 *	@brief  Oパワーアプリ初回チュートリアルフラグ取得
 * 
 * @return true:チュートリアル終了した false:していない
 */
//--------------------------------------------------------------------------
bool Misc::GetApowerTutorial( void ) const
{
  return m_data.apower_tutorial;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 隠しアイテムの復活フラグを取得
 *
 * @return true:復活する, false:復活しない
 */
/* ------------------------------------------------------------------------- */
bool Misc::GetHideItemRevivalFlag( void ) const
{ 
  return m_data.hideitem_revival;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 隠しアイテムの復活フラグを設定
 *
 * @param is_revival true:復活する, false:復活しない
 */
/* ------------------------------------------------------------------------- */
void Misc::SetHideItemRevivalFlag( bool is_revival )
{
  m_data.hideitem_revival = is_revival;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief おおぞらをとぶ ショート再生フラグ
 */
/* ------------------------------------------------------------------------- */
bool Misc::GetSkyTripShortFlag( void ) const
{
  return m_data.skytrip_short;
}
/* ------------------------------------------------------------------------- */
/**
 * @brief おおぞらをとぶ ショート再生フラグ
 *
 * @param is_short true:ショートデモ, false:ロングデモ
 */
/* ------------------------------------------------------------------------- */
void Misc::SetSkyTripShortFlag( bool is_short )
{
  m_data.skytrip_short = is_short;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 秘境フラグを取得
 *
 * @return hikyo NIKYO_NOT など
 */
/* ------------------------------------------------------------------------- */
u32 Misc::GetHikyo( void ) const
{
  return m_data.hikyo;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 秘境フラグをセット
 *
 * @param hikyo HIKYO_NOT など
 */
/* ------------------------------------------------------------------------- */
void Misc::SetHikyo( u32 hikyo )
{
  m_data.hikyo = hikyo;
}

//-----------------------------------------------------------------------------
/**
 * @brief  TVナビの初回起動フラグセット
 */
//-----------------------------------------------------------------------------
void Misc::SetTvNaviInitFlag(void)
{
	m_data.tv_navi_init = true;
}

//-----------------------------------------------------------------------------
/**
 * @brief  TVナビの初回起動フラグ取得
 * @return 初回起動フラグ
 */
//-----------------------------------------------------------------------------
bool Misc::GetTvNaviInitFlag(void) const
{
	return m_data.tv_navi_init;
}


//-----------------------------------------------------------------------------
/**
 *	@brief  時をかける賞状ポケモンのMONSNOとニックネームを取得
 *	@param	gfl::str::StrBuf*  dst [OUT]  ニックネーム文字列格納先
 *	@param  pml::Sex*          sex [OUT]  性別格納先
 *	@retval MONSNO_NULLを返すときはdstには何も代入しない。
 */
//-----------------------------------------------------------------------------
MonsNo Misc::GetTokikakePoke( gfl::str::StrBuf* dst, pml::Sex* sex ) const
{
  MonsNo monsNo = static_cast<MonsNo>(m_data.tokikakeMonsNo);
  if(monsNo != MONSNO_NULL)
  {
    *sex = static_cast<pml::Sex>(m_data.tokikakeSex);
    dst->SetStr( m_data.tokikakeNickname );
  }
  return monsNo;
}
//-----------------------------------------------------------------------------
/**
 *	@brief  時をかける賞状ポケモンのMONSNOとニックネームを設定
 *	@param	MonsNo            monsNo [IN]  1以上386デオキシス以下でなくても有効。進化するので。
 *	@param	gfl::str::StrBuf  src    [IN]  代入するニックネーム文字列。
 *	@param  pml::Sex          sex    [IN]  性別。
 *	@retval 正常に設定できたときはtrueを返す。無効なMONSNOのときは設定せずfalseを返す。
 */
//-----------------------------------------------------------------------------
bool Misc::SetTokikakePoke( MonsNo monsNo, const gfl::str::StrBuf& src, pml::Sex sex )
{
  // ルビーサファイアエメラルド以降のポケモンに進化させてるかもしれないので、MONSNOチェックは止めた。
  u16 val = static_cast<u16>(monsNo);

  m_data.tokikakeMonsNo = val;
  m_data.tokikakeSex    = static_cast<u32>(sex);
  src.PutStr( m_data.tokikakeNickname, gamesystem::STRLEN_POKEMON_NAME + gamesystem::EOM_LEN );
  return true;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief エンカウントチュートリアルフラグを設定
 *
 * @param is_enable [IN] true:有効, false:無効
 */
/* ------------------------------------------------------------------------- */
void Misc::SetEncountTutorialEndFlag( bool is_enable )
{
  m_data.encount_tutorial_end = is_enable;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief エンカウントチュートリアルフラグを取得
 *
 * @param true:有効, false:無効
 */
/* ------------------------------------------------------------------------- */
bool Misc::GetEncountTutorialEndFlag( void ) const
{
  return m_data.encount_tutorial_end;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief エンカウントチュートリアル用カウンタを設定 (※2 bit)
 *
 * @param count カウント数
 */
/* ------------------------------------------------------------------------- */
void Misc::SetEncountTutorialCount( u32 count )
{
  m_data.encount_tutorial_count = count;
  HOSAKA_PRINT("ENCOUNT_TUTORIAL::count=%d\n", m_data.encount_tutorial_count );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief エンカウントチュートリアル用カウンタを取得
 *
 * @return カウント数
 */
/* ------------------------------------------------------------------------- */
u32 Misc::GetEncountTutorialCount( void ) const
{
  return m_data.encount_tutorial_count;
}


/**
 * @brief 配信ひみつきち用拡張データへのアクセッサーを取得
 *
 * @return アクセッサー型
 */
const SecretBaseDeliveryBaseExtendDataAccessor Misc::GetSecretBaseDeliveryBaseExtendDataAccessor( void ) const
{
  SecretBaseDeliveryBaseExtendDataAccessor accessor;
  // @note コンスト返しなので、コンスト外しを許容する
  accessor.Initialize(  const_cast< field::secretBase::DELIVERY_BASE_EXTEND_DATA* >( &m_data.secretBaseDeliveryBaseExtendData ) );

  return accessor;
}

/**
 * @brief 配信ひみつきち用拡張データへのアクセッサーを取得
 *
 * @return アクセッサー型
 */
SecretBaseDeliveryBaseExtendDataAccessor Misc::GetSecretBaseDeliveryBaseExtendDataAccessor( void )
{
  SecretBaseDeliveryBaseExtendDataAccessor accessor;
  accessor.Initialize( &m_data.secretBaseDeliveryBaseExtendData );

  return accessor;
}
#endif



//--------------------------------------------------------------------------------------------
/**
 * @brief   Miscセーブデータのコピー　（JoinFestaマップジャンプ対応用）
 */
//--------------------------------------------------------------------------------------------
void Misc::CopyData( const Misc& data, b32 joinFestaReturn )
{
  if( joinFestaReturn )
  {
    u32 fieldCameraAreaOldDataNo = data.m_data.fieldCameraAreaOldDataNo;
    gfl2::std::MemCopy( &(data.m_data), &m_data, this->GetDataSize() );
    m_data.fieldCameraAreaOldDataNo = fieldCameraAreaOldDataNo;
  }
  else
  {
    gfl2::std::MemCopy( &(data.m_data), &m_data, this->GetDataSize() );
  }
}


GFL_NAMESPACE_END(Savedata)
