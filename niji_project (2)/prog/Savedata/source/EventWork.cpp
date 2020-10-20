//============================================================================
/**
 *
 *  @file   EventWork.cpp
 *  @brief  セーブフラグ、ワーク操作
 *  @author hosaka genya(XY)
 *  @data   2011.05.26
 *  @author tamada  GAMEFREAK inc.
 *  @date   2005.10.22
 *
 */
//============================================================================

#include <pml/include/pml_PokePara.h>

#include "savedata/include/EventWork.h"
#include "util/include/gfl2_std_string.h"
#include "System/include/GflUse.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

namespace Field
{ 
  
//------------------------------------------------------------------
/**
 * @brief セーブしないフラグ
 */
//------------------------------------------------------------------
static u8 EventCtrlFlag[CTRLFLAG_AREA_MAX] = {};

const int EventWork::EVENT_FLAG_REVIVE_ITEM_RESET_PAR = 30;

//-----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
EventWork::EventWork( void )
{ 
  InitWork();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ワーク初期化
 */
//-----------------------------------------------------------------------------
void EventWork::InitWork( void )
{
  GFL_ASSERT( EVENT_WORK_AREA_MAX >= WK_AREA_END - WORKS_TOTAL_MIN );
  GFL_ASSERT( EVENT_FLAG_AREA_MAX >= gfl2::math::RoundUp( FLAGS_TOTAL_MAX, 8) / 8 );

  mCoreData.TimeRequest = false;
  mCoreData.dummy = 0;
  mCoreData.MushiyokeItemNo = 0;

  gfl2::std::MemClear( mCoreData.Work, sizeof(u16) * EVENT_WORK_AREA_MAX );
  gfl2::std::MemClear( mCoreData.Flag, sizeof(u8) * EVENT_FLAG_AREA_MAX );
  gfl2::std::MemClear( mCoreData.DendouiriPartyRecord, sizeof(u16) * (DENDOUIRI_PARTY_TYPE_MAX*pml::PokeParty::MAX_MEMBERS) );

  gfl2::std::MemClear( mCoreData.padding_1, sizeof(u8) * EVENT_WORK_SAVE_PADDING1 );

  mCoreData.SprayCount = 0;
}



//------------------------------------------------------------------
/**
 * @brief IDがフラグのレンジなのかをチェック
 *        noがフラグなのかワークなのかをチェック
 *
 * @param no   フラグナンバー
 *
 * @retval  "1 = フラグ"
 * @retval  "0 = ワーク"
 */
//------------------------------------------------------------------
bool EventWork::CheckIDFlagRenge( u16 no )
{
  if( no < WORKS_TOTAL_MIN ){
    return true;
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief IDがバニッシュフラグのレンジなのかをチェック
 *
 * @param no   フラグナンバー
 *
 * @retval  "1 = バニッシュフラグ"
 * @retval  "0 = それ以外"
 */
//------------------------------------------------------------------
bool EventWork::CheckVanishiFlagRenge( u16 no )
{
  if( no >= FV_AREA_START && no <= FV_AREA_END ){
    return true;
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief イベントフラグをチェックする
 *
 * @param flag_no   フラグナンバー
 *
 * @retval  "1 = フラグON"
 * @retval  "0 = フラグOFF"
 */
//------------------------------------------------------------------
bool EventWork::CheckEventFlag( u16 flag_no ) const
{
  // 特に書き換えるわけではないので、明示的にconst_cast
  u8 * p = const_cast<EventWork*>(this)->GetEventFlagAdrs( flag_no );
  if( p != NULL )
  {
    if( *p & ( 1 << (flag_no % 8) ) ){ return true; }
  }
  return false;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  イベントフラグをセットする
 *
 *  @param  u16 flag_no   フラグナンバー
 */
//-----------------------------------------------------------------------------
void EventWork::SetEventFlag( u16 flag_no )
{
  u8* p = GetEventFlagAdrs( flag_no );
  if( p == NULL ){ return; }
  *p |= 1 << ( flag_no % 8 );
  return;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  フラグをリセットする
 *
 *  @param  u16 flag_no   フラグナンバー
 */
//-----------------------------------------------------------------------------
void EventWork::ResetEventFlag( u16 flag_no )
{
  u8 * p = GetEventFlagAdrs( flag_no );
  if( p == NULL ){ return; }
  *p &= ( 0xff ^ ( 1 << ( flag_no % 8 ) ) );
  return;
}

//--------------------------------------------------------------
/**
 * @brief イベントフラグの領域クリア
 * @param flag_no   フラグナンバー
 * 
 * @noto
 * ベタな記述なので処理速度の問題があるかもしれない
 */
//--------------------------------------------------------------
void EventWork::ClearEventFlags( u16 start_no, u16 end_no )
{
  int no;

  GFL_ASSERT( start_no <= end_no );

  // BW世代からのバグを修正したら元がバグっていたため動かなくなり結局コメント化
  // GFL_ASSERT( ( start_no >= SCFLG_START ) && ( end_no >= SCFLG_START ) );

  for ( no = start_no ; no <= end_no; no ++ )
  {
    ResetEventFlag( no );
  }
}

//--------------------------------------------------------------
/**
 * @brief イベントワークアドレスを取得
 *
 * @param work_no   ワークナンバー
 *
 * @return  "ワークのアドレス"
 *
 * @li  work_no < 0x8000  通常のセーブワーク
 * @li  work_no >= 0x8000 スクリプト制御ワークの中に確保しているワーク
 */
//--------------------------------------------------------------

const u16* EventWork::GetEventWorkAdrs( u16 work_no ) const
{
  if( ( work_no - SVWK_START ) >= EVENT_WORK_AREA_MAX )
  {
    GFL_ASSERT_MSG( 0, "ワークナンバー(0x%04x)が最大数を超えています！\n", work_no );
    return NULL;
  }

  return &mCoreData.Work[ work_no - SVWK_START ];
}


u16* EventWork::GetEventWorkAdrs( u16 work_no )
{
  if( ( work_no - SVWK_START ) >= EVENT_WORK_AREA_MAX )
  {
    GFL_ASSERT_MSG( 0, "ワークナンバー(0x%04x)が最大数を超えています！\n", work_no );
    return NULL;
  }

  return &mCoreData.Work[ work_no - SVWK_START ];
}

//--------------------------------------------------------------
/**
 * @brief イベントーワークに値セット
 * @param work_no   ワークナンバー
 */
//--------------------------------------------------------------
void EventWork::SetEventWork( u16 work_no, u16 value )
{
  u16* p_work = this->GetEventWorkAdrs( work_no );
  *p_work = value;
}

//--------------------------------------------------------------
/**
 * @brief イベントーワークの値ゲット
 * @param work_no   ワークナンバー
 */
//--------------------------------------------------------------
u16 EventWork::GetEventWork( u16 work_no)
{
  u16* p_work = this->GetEventWorkAdrs( work_no );
  return *p_work;
}



//--------------------------------------------------------------
/**
 * @brief   イベントーワークをクリア
 * @param work_no   ワークナンバー
 */
//--------------------------------------------------------------
void EventWork::ClearEventWorks( u16 start_no, u16 end_no )
{
  GFL_ASSERT( start_no <= end_no );
  GFL_ASSERT( SVWK_START <= start_no );
  GFL_ASSERT( end_no < SVWK_START + EVENT_WORK_AREA_MAX );

  gfl2::std::MemClear(
      GetEventWorkAdrs( start_no ),
      sizeof(u16) * (end_no - start_no + 1) );
}


//-----------------------------------------------------------------------------
/**
 *  @brief  アローラ時間フラグのクリア
 */
//-----------------------------------------------------------------------------
void EventWork::ClearTimeFlags( void )
{
  ClearEventFlags( TMFLG_AREA_START, TMFLG_AREA_END );
  mCoreData.TimeRequest = true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  本体（リアル）時間フラグのクリア
 */
//-----------------------------------------------------------------------------
void EventWork::ClearRealTimeFlags( void )
{
  ClearEventFlags( TMFLG_REAL_AREA_START, TMFLG_REAL_AREA_END );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  特殊時間フラグ(ペナルティの効果が逆)のクリア
 *
 *  @note 通常のタイムフラグは 0時またぎでクリアされ、ペナルティ中は0時またぎでクリアされない。
 *        ペナルティの効果が逆のこの特殊フラグは、ペナルティ発生時にフラグ強制クリア
 *        また、ペナルティ中でも0時またぎで必ずフラグがクリアされる
 */
//-----------------------------------------------------------------------------
void EventWork::ClearTimeFlagsPenaltyInverse( void )
{
  // sangoでは niji_conv_header/field/flagwork/FlagDefine.h に格納されていた.
  //@attention nijiでは数が少ないのでマニュアルで定義している
  ResetEventFlag( TMFLG_COUPON_FS );

#if 0
  ClearEventFlags( TMFLG_PI_AREA_START, TMFLG_PI_AREA_END );　//sango
#endif
}


//-----------------------------------------------------------------------------
/**
 *  @brief  復活隠しアイテムフラグのクリア
 */
//-----------------------------------------------------------------------------
void EventWork::ClearReviveItemFlags( void )
{
  if( PopTimeRequest() )
  {
    for( u32 i = FV_FLD_REVIVE_ITEM_AREA_START; i <= FV_FLD_REVIVE_ITEM_AREA_END; ++i )
    {
      if( CheckEventFlag(i) )
      {
        u32 par = EVENT_FLAG_REVIVE_ITEM_RESET_PAR;
#if PM_DEBUG
        par = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::RERIVE_ITEM_PER );
#endif// PM_DEBUG

        // n%で復活
        if( System::GflUse::GetPublicRand( 100 ) <= par )
        {
          ResetEventFlag(i);  ///<復活アイテム
        }
      }
    }

  }
}
 
//----------------------------------------------------------
//    時間更新処理リクエストの取り出し
//    ※現状、隠しアイテム復活用
//----------------------------------------------------------
bool EventWork::PopTimeRequest( void )
{
  bool result = GFL_BOOL_CAST(mCoreData.TimeRequest);
  mCoreData.TimeRequest = false;
  return result;
}


#if 0

//----------------------------------------------------------
//  セーブデータ取得のための関数
//  ※実体はsrc/gamesystem/game_data.c
//----------------------------------------------------------
//--------------------------------------------------------------
/**
 * @brief イベントフラグ・ワーク管理セーブデータの取得
 * @param sv_control_ptr  セーブデータ制御ワークへのポインタ
 * @return  EVENTWORK *
 *
 * @note
 * 本来はGAMEDATA経由ですべてのセーブデータがアクセスできるはずだったが
 * WBでは未完に終わり、開発終盤でこの関数を追加する必要ができたので作成。
 * 出来る限りSAVE_CONTROL_WORK経由のセーブデータアクセスは使いたくないし、
 * チャンスがあれば削除したいのでGMDATA_ID_EVENT_WORKが局所参照となるよう
 * この関数はgame_data.cの中に置いておく。
 */
//--------------------------------------------------------------
EventWork* EventWork::SaveData_GetEventWrok( SAVE_CONTROL_WORK* sv_control_ptr )
{
  return SaveControl_DataPtrGet( sv_control_ptr, GMDATA_ID_EVENT_WORK );
}
#endif


//--------------------------------------------------------------
/**
 * @brief イベントフラグのアドレスを取得する
 *
 * @param flag_no   フラグナンバー
 *
 * @retval  "NULL != フラグアドレス"
 * @retval  "NULL = 未定義"
 */
//--------------------------------------------------------------
u8 * EventWork::GetEventFlagAdrs( u16 flag_no )
{
  if( flag_no == 0 ){
    return NULL;
  }else if( flag_no < SCFLG_START ){
    if( (flag_no / 8) >= EVENT_FLAG_AREA_MAX ){
      GFL_ASSERT_MSG(0, "フラグナンバー(0x%04x)が最大数を超えています！\n", flag_no );
      return NULL;
    }
    return &mCoreData.Flag[flag_no/8];
  }else{

    if( ((flag_no - SCFLG_START) / 8) >= CTRLFLAG_AREA_MAX ){
      GFL_ASSERT_MSG(0, "フラグナンバー(0x%04x)が最大数を超えています！\n", flag_no );
      return NULL;
    }
    return &( EventCtrlFlag[ (flag_no - SCFLG_START) / 8 ] );
  }
}



//-----------------------------------------------------------------------------
/**
 *  @brief  むしよけスプレー使用開始
 */
//-----------------------------------------------------------------------------
void EventWork::SetMushiyokeCount(u16 itemno, u16 count)
{
  mCoreData.SprayCount = count;   // @fix GFNMCat[2535]: 255を超えたので2バイト領域に変更
  mCoreData.MushiyokeItemNo = itemno;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  むしよけスプレー有効歩数を１ずつ減らす
 * @return TRUEのときスプレーが切れている
 */
//-----------------------------------------------------------------------------
bool EventWork::DecMushiyokeCount(void)
{
  // @fix GFNMCat[2535]: 255を超えたので2バイト領域に変更
  if( mCoreData.SprayCount > 0 ){
    --mCoreData.SprayCount;
    if(mCoreData.SprayCount == 0){
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  むしよけスプレーを使えるか？
 */
//-----------------------------------------------------------------------------
bool EventWork::CanUseMushiyoke(void)
{
  // @fix GFNMCat[2535]: 255を超えたので2バイト領域に変更
  if( mCoreData.SprayCount > 0 ){
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  使用中のむしよけスプレーのアイテムNoを取得
 */
//-----------------------------------------------------------------------------
u16 EventWork::GetMushiyokeItemNo(void)
{
  return mCoreData.MushiyokeItemNo;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ジガルデセル パワー取得
 */
//-----------------------------------------------------------------------------
u16 EventWork::GetZigarudeCell( void )
{
  u16* p_work = this->GetEventWorkAdrs( WK_SYS_ZIGARUDE_CELL );
  return *p_work;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ジガルデセル 追加可能か？ momijiでは未使用
 */
//-----------------------------------------------------------------------------
bool EventWork::AddCheckZigarudeCell( u16 value )
{
  GFL_ASSERT(0);
  u16* p_work = this->GetEventWorkAdrs( WK_SYS_ZIGARUDE_CELL );
  u16 result = (*p_work)+value;
  return (result <= Savedata::ZIGARUDE_CELL_MAX);
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ジガルデセル 利用可能か？ momijiでは未使用 
 */
//-----------------------------------------------------------------------------
bool EventWork::SubCheckZigarudeCell( u16 value )
{
  GFL_ASSERT(0);
  u16* p_work = this->GetEventWorkAdrs( WK_SYS_ZIGARUDE_CELL );
  return (*p_work >= value);
}

//-----------------------------------------------------------------------------
 /**
 *  @brief  ジガルデセル 充電
 */
//-----------------------------------------------------------------------------
u16 EventWork::AddZigarudeCell( u16 value )
{
  u16* p_work = this->GetEventWorkAdrs( WK_SYS_ZIGARUDE_CELL );
  u16 result = (*p_work)+value;
  
  if(result <= Savedata::ZIGARUDE_CELL_MAX){
    *p_work += value;
  }else{
    //NMCat[3078]「ジガルデキューブ」内のセル数が特定個数の時に分解を行うと正しく合計数がカウントがされなくなる
    *p_work = Savedata::ZIGARUDE_CELL_MAX;
  }
  return *p_work;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ジガルデセル 利用
 */
//-----------------------------------------------------------------------------
u16 EventWork::SubZigarudeCell( u16 value )
{
  u16* p_work = this->GetEventWorkAdrs( WK_SYS_ZIGARUDE_CELL );
  
  if (*p_work >= value){
    *p_work -= value;
  }else{
    *p_work = 0;  
  }
  return *p_work;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  殿堂入りパーティを記録
 */
//-----------------------------------------------------------------------------
void EventWork::SetDendouiriParty( DendouiriPartyType type, const pml::PokeParty* party )
{
  int count = 0;
  gfl2::std::MemClear( mCoreData.DendouiriPartyRecord[type], sizeof(u16) * pml::PokeParty::MAX_MEMBERS );

  if( type >= DENDOUIRI_PARTY_TYPE_MAX ){
    GFL_ASSERT(0);
    type = DENDOUIRI_PARTY_TYPE_LAST;
  }

  for( u32 i = 0; i < party->GetMemberCount(); i++ ){
    const pml::pokepara::PokemonParam* p_pp = party->GetMemberPointerConst(i);

    if( p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ) continue; //タマゴの場合処理をスキップ
    mCoreData.DendouiriPartyRecord[type][count++] = p_pp->GetMonsNo();
  }
}


//-----------------------------------------------------------------------------
/**
 *  @brief  殿堂入りパーティを取得
 */
//-----------------------------------------------------------------------------
u16* EventWork::GetDendouiriPartyArrayPtr( DendouiriPartyType type )
{
  if( type >= DENDOUIRI_PARTY_TYPE_MAX ){
    GFL_ASSERT(0);
    type = DENDOUIRI_PARTY_TYPE_LAST;
  }
  return mCoreData.DendouiriPartyRecord[type];
}

} // namespace Field

