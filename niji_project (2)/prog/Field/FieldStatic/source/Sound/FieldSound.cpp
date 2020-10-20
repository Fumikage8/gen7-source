//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldSound.cpp
 *	@brief  フィールドサウンド管理モジュール
 *	@author Miyuki Iwasawa	
 *	@date		2015.06.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/TimeZone.h"
#include "Sound/include/Sound.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/Time/EvTime.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"
#include "FieldScript/include/ScriptAssert.h"

#include "niji_conv_header/field/flagwork/FlagDefine.h"
#include "niji_conv_header/field/zone/zone_id.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/script_event_data.gaix>

namespace Field{

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /**
   *	@brief  コンストラクタ
   */
  //-----------------------------------------------------------------------------
  FieldSound::FieldSound( const GameSys::GameData* p_gdata )
  {
    m_pGameData = p_gdata;
    m_pZoneDataLoader = const_cast<Field::ZoneDataLoader*>(m_pGameData->GetFieldZoneDataLoader());
    m_specialEnvSeNo = SPECIAL_SOUND_NONE;
    m_SpecialBGMSize = 0;
    m_initSeq = 0;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  デストラクタ
   */
  //-----------------------------------------------------------------------------
  FieldSound::~FieldSound()
  {
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  初期化処理
   */
  //-----------------------------------------------------------------------------
  void FieldSound::Initialize( void )
  {
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  破棄処理
   */
  //-----------------------------------------------------------------------------
  void FieldSound::Finalize( void )
  {
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  スペシャルBGM取得
   *
   *	@param  zone_ID  ゾーンID
   */
  //-----------------------------------------------------------------------------
  u32 FieldSound::GetSpecialBGM( ZoneID zoneID, b32 checkPriority )
  {
    const EventWork* ev = m_pGameData->GetEventWork();
    SpecialBGM* pSpecialBGMTable = reinterpret_cast<SpecialBGM*>(m_pSpecialBGMBuffer);

    for( u32 i = 0;i < m_SpecialBGMSize; ++i ){
      const SpecialBGM* data = &pSpecialBGMTable[i];
      if( data->zoneID == zoneID && ev->CheckEventFlag( data->flagNo )){
        if( checkPriority && !data->isHighPriority){
          return SPECIAL_SOUND_NONE;  //プライオリティ負け
        }
        if( this->IsDayTime() ){
          return data->soundDayIdx;
        }else{
          return data->soundNightIdx;
        }
      }
    }

    const u16 * p_wk = ev->GetEventWorkAdrs( WK_SYS_POKECEN_BIRTH );

    if ( (*p_wk) == 1 )
    {
      return STRM_BGM_SILENCE;
    }
    else if ( (*p_wk) == 2 )
    {
      return STRM_BGM_BD_02;
    }
    else if ( (*p_wk) == 3 )
    {
      return STRM_BGM_BD_01;
    }
    return SPECIAL_SOUND_NONE; 
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  スペシャルEnvSE取得
   *
   *	@param  zone_ID  ゾーンID
   */
  //-----------------------------------------------------------------------------
  u32 FieldSound::GetSpecialEnvSE( ZoneID zoneID )
  {
#if 0
    const EventWork* ev = m_pGameData->GetEventWork();
    for( int i = 0;i < GFL_NELEMS(sc_specialEnvSETable); ++i ){
      const SpecialEnvSE* data = &sc_specialEnvSETable[i];
      if( data->zoneID != zoneID ){
        continue;
      }
      if( data->stopFlagNo != 0 && ev->CheckEventFlag( data->stopFlagNo ) == true ){
        continue;
      }
      return data->soundIdx;
    }
#endif
    return SPECIAL_SOUND_NONE; 
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ゾーン スペシャルEnvSEセットアップ
   *
   *	@param  zone_ID  ゾーンID
   */
  //-----------------------------------------------------------------------------
  void FieldSound::PlayZoneSpecialEnvSE( void )
  {
#if 0
    u32 seNo = GetSpecialEnvSE( m_zoneID );
    if( seNo != SPECIAL_SOUND_NONE ){
      Sound::PlayEnvSE( seNo, Sound::BGM_FADE_FAST );
      m_specialEnvSeNo = seNo;
    }
#endif
  }
  //----------------------------------------------------------------------------
  /**
   *	@brief  ゾーン スペシャルEnvSE終了
   *
   *	@param  zone_ID  ゾーンID
   */
  //-----------------------------------------------------------------------------
  void FieldSound::StopZoneSpecialEnvSE( void )
  {
#if 0
    if( m_specialEnvSeNo != SPECIAL_SOUND_NONE ){
      Sound::StopEnvSE( m_specialEnvSeNo, Sound::BGM_FADE_FAST );
      m_specialEnvSeNo = SPECIAL_SOUND_NONE;
    }
#endif
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  フィールドサウンド：昼間かどうか？
   */
  //-----------------------------------------------------------------------------
  b32 FieldSound::IsDayTime( void )
  {
    GameSys::TimeZone::Zone timeZone = GameSys::TimeZone::GetNowTimeZone();
    if( timeZone < GameSys::TimeZone::NIGHT ){
      return true;
    }
    return false;    
  }
  b32 FieldSound::IsDayTimeFromEvTime( void )
  {
    GameSys::TimeZone::Zone timeZone = EvTime::GetAlolaTimeZone( m_pGameData );
    if( timeZone < GameSys::TimeZone::NIGHT ){
      return true;
    }
    return false;
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  ライドBGM判定
   *
   *	@param  gamedata            ゲームデータ
   *	@param  player_move_form    プレイヤー動作フォーム
   *	@param  priority_ride_bgm   trueだとBGM切り替えチェックでライドBGM優先
   */
  //-----------------------------------------------------------------------------
  u32 FieldSound::GetRideBGM( ZoneID zoneID, PlayerForm player_move_form, bool priority_ride_bgm  )
  {
    ZoneDataAccessor zoneDataAccessor( m_pZoneDataLoader->GetZoneData( zoneID ) );
    FieldBGMMode mode = this->GetFieldBGMMode(player_move_form);

    //陸系ライドや波乗り時のBGMが有効なゾーンか？
    if( zoneDataAccessor.IsBGMChangeEnable() == false ){
      return SPECIAL_SOUND_NONE;
    }
    //ライド状態か？
    if( mode == FIELD_BGM_MODE_NORMAL ){
      return SPECIAL_SOUND_NONE;
    }

    //越境型(WATER)はpriority_ride_bgm の影響を受けない
    if( mode == FIELD_BGM_MODE_RIDE_LAND ){
      if( priority_ride_bgm ) return STRM_BGM_POKERIDE01;  //trueならライド

      //現在の曲ナンバーを判定する（ライドBGMならそのまま、ゾーンBGMなら昼夜判定をする）
      if( m_currentBGMID == STRM_BGM_POKERIDE01 ) return STRM_BGM_POKERIDE01;

    }else if( mode == FIELD_BGM_MODE_RIDE_WATER){
      if( priority_ride_bgm ) return STRM_BGM_POKERIDE02;  //trueならライド

      //現在の曲ナンバーを判定する（ライドBGMならそのまま、ゾーンBGMなら昼夜判定をする）
      if( m_currentBGMID == STRM_BGM_POKERIDE02 ) return STRM_BGM_POKERIDE02;
    }
    return SPECIAL_SOUND_NONE;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  現在鳴らすべきBGM 
   *
   *	@param  gamedata            ゲームデータ
   *	@param  player_move_form    プレイヤー動作フォーム
   *	@param  priority_ride_bgm   trueだとBGM切り替えチェックでライドBGM優先
   */
  //-----------------------------------------------------------------------------
  u32 FieldSound::GetFieldBGM( ZoneID zoneID, PlayerForm player_move_form, bool priority_ride_bgm  )
  {
    ZoneDataAccessor zoneDataAccessor( m_pZoneDataLoader->GetZoneData( zoneID ) );
    bool is_ride = false;

    u32 ride_bgm_id = FieldSound::GetRideBGM( zoneID, player_move_form, priority_ride_bgm );
    is_ride = ( ride_bgm_id != SPECIAL_SOUND_NONE );

    //スペシャルBGMの判定
    u32 sp_bgm_id = FieldSound::GetSpecialBGM( zoneID, is_ride );
    if( sp_bgm_id != SPECIAL_SOUND_NONE ) return sp_bgm_id;

    //次にライドBGMの判定
    if( is_ride){
      return ride_bgm_id;
    }

    //歩き
    if( this->IsDayTime() ){
      return zoneDataAccessor.GetBGMIDDay(); 
    }else{
      return zoneDataAccessor.GetBGMIDNight(); 
    }
  }
  u32 FieldSound::GetFieldBGM( PlayerForm player_move_form )
  {
    return GetFieldBGM( m_zoneID, player_move_form, false );
  }
  u32 FieldSound::GetFieldBGM( void )
  {
    return GetFieldBGM( m_zoneID, this->GetPlayerForm(), false );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  フィールドフリームーブ状態でのサウンドステータスをケア
   */
  //-----------------------------------------------------------------------------
  void FieldSound::CareFieldBGMState( void )
  {
    //リクエストが積まれている間はチェックをスルー
    if( Sound::HaveCommand() == false ) 
    {
      u32 pushedCount = Sound::GetPushedBGMCount();
      if( pushedCount > 0 ){
        //フリームーブなのにPushCountが積まれてるのはおかしい
        GFL_ASSERT_MSG( 0,"BGMがPushされたままフリームーブになりました %d\n", pushedCount );
        for( u32 i = 0;i < pushedCount;i++){
          Sound::PopBGMReq(Sound::BGM_FADE_VERY_FAST);
        }
      }
    }
    //環境音ボリュームの制御をチェック　GFNMCat[5472]
    if( Sound::IsRunningPlayerVolumeChange(Sound::SND_PLAYER_IDX_SE_ENV) == false ) //GFNMCat[5590] リクエストが無くなってからチェックする
    {
      f32 envse_volme = Sound::GetPlayerVolume(PLAYER_SE_ENV);
      if( gfl2::math::IsSimilar( envse_volme, 1.0f ) == false ){
        SCRIPT_ASSERT_MSG( 0,"EnvSEVolumeが操作されたままフリームーブになりました %f\n", envse_volme ); 
        ChangeEnvSoundVolume(1.0f,Sound::BGM_FADE_NONE);
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  時間監視によるBGMの切り替えチェック
   */
  //-----------------------------------------------------------------------------
  void FieldSound::UpdateFieldBGM( void )
  {
    u32 nowBGM = GetFieldBGM( m_zoneID, this->GetPlayerForm(), false );  //時間監視の時はライドBGMの優先順位が下がる
    if( m_currentBGMID == nowBGM ){
      return;
    }
    m_currentBGMID = nowBGM;
    Sound::ChangeFieldBGMReq( m_currentBGMID, Sound::BGM_FADE_MIDDLE, Sound::BGM_FADE_MIDDLE );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ゾーンチェンジ　BGMプレイ
   *
   *	@param  gamedata          ゲームデータ
   *	@param  player_move_form  プレイヤー動作フォーム
   *	@param  adjacent_f        地続きゾーンチェンジか？
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ChangeFieldBGMByZoneChange( b32 adjacent_f /*=false*/ )
  {
    u32 bgm_id = 0;
    PlayerForm move_form = this->GetPlayerForm();
    FieldBGMMode mode = this->GetFieldBGMMode();

    bgm_id = GetFieldBGM( m_zoneID, move_form, false );

    // サウンド再生開始
    {
      Sound::FadeFrame fadein_frame = Sound::BGM_FADE_NONE; //デフォルトはフェードインなし

      m_currentBGMID = bgm_id;
      Sound::ChangeFieldBGMReq( bgm_id, Sound::BGM_FADE_MIDDLE, fadein_frame );
    }
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  スタンバイ　NextMapBGM
   */
  //-----------------------------------------------------------------------------
  void FieldSound::PrepareNextFieldBGM( void )
  {
    u32 bgm_id = 0;
    PlayerForm move_form = this->GetPlayerForm();
    FieldBGMMode mode = this->GetFieldBGMMode(move_form);

    //sango BTS799関連対策 ノーマルフォルムでマップ抜け→波乗りフォルムのケースに対応
    if( mode == FIELD_BGM_MODE_RIDE_LAND ){
      //陸系ライドの時はフォルムをノーマルとしてbgmを取得することで陸系ライドBGMを解除する
      move_form = PLAYER_FORM_NORMAL;
    }
    bgm_id = GetFieldBGM( m_zoneID, move_form, true );

    m_currentBGMID = bgm_id;
    Sound::PrepareNextMapBGMReq( bgm_id );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  スタンバイ　プレイ
   */
  //-----------------------------------------------------------------------------
  void FieldSound::PreparePlayNextFieldBGM( void )
  {
    this->ResetTrainerFocusMode( Sound::BGM_FADE_NONE );
    Sound::PreparePlayNextMapBGMReq();
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  開始BGM再生
   */
  //-----------------------------------------------------------------------------
  void FieldSound::StartFirstFieldBGM( void )
  {
    m_pZoneDataLoader = const_cast<Field::ZoneDataLoader*>(m_pGameData->GetFieldZoneDataLoader()); 

    // BGM開始
    u32 bgm_id = GetFieldBGM( m_zoneID, this->GetPlayerForm(), true );
    m_currentBGMID = bgm_id;
    Sound::ChangeBGMReq( bgm_id );
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  イベントBGM開始
   *	@note かならずEndEventBGM()とセットで用いること
   */
  //-----------------------------------------------------------------------------
  void FieldSound::StartEventBGM( u32 bgm_id, Sound::FadeFrame fadeout_frame, Sound::FadeFrame fadein_frame /*= Sound::BGM_FADE_NONE*/ )
  {
    // イベントBGM開始
    Sound::ChangeBGMReq( bgm_id, fadeout_frame, fadein_frame );
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  イベントBGM変更
   *
   *	@note StartEventBGM-EndEventBGMの間でのみ利用可
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ChangeEventBGM( u32 bgm_id, Sound::FadeFrame fadeout_frame, Sound::FadeFrame fadein_frame )
  {
    // イベントBGMを変更
    Sound::ChangeBGMReq( bgm_id, fadeout_frame, fadein_frame );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  イベント終了 BGM復帰
   *	@param  fadeout_frame フェードアウトフレーム
   *	@param  fadein_frame  フェードインフレーム
   *
   *	@note かならずStartEventBGM()とセットで用いること
   */
  //-----------------------------------------------------------------------------
  void FieldSound::EndEventBGM( Sound::FadeFrame fadeout_frame, Sound::FadeFrame fadein_frame )
  {
    // BGM開始
    u32 bgm_id = GetFieldBGM();

    m_currentBGMID = bgm_id;
    Sound::ChangeBGMReq( bgm_id, fadeout_frame, fadein_frame );
    
//    Sound::ReplayEnvSE(Sound::ENVSE_PAUSE_CHANGE);
  }
 
  //----------------------------------------------------------------------------
  /**
   *	@brief  デモBGM終了 BGM復帰
   *	@param  fadeout_frame フェードアウトフレーム
   *	@param  fadein_frame  フェードインフレーム
   *
   *	@note デモでBGMな書き換えらた場合のみに使用
   */
  //-----------------------------------------------------------------------------
  void FieldSound::EndDemoBGM( Sound::FadeFrame fadeout_frame, Sound::FadeFrame fadein_frame )
  {
    // BGM開始
    u32 bgm_id = GetFieldBGM();

    m_currentBGMID = bgm_id;
    Sound::ChangeBGMReq( bgm_id, fadeout_frame, fadein_frame );
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  現在のマップのBGMを鳴らす
   *	@param  fadeout_frame フェードアウトフレーム
   *	@param  fadein_frame  フェードインフレーム
   *
   *	イベント途中でスペシャルBGMへの鳴らし換えを行いたい時などに使う
   *	良くも悪くも環境音には作用しないので、単発呼び出しのみ可
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ChangeFieldBGMNowMap( Sound::FadeFrame fadeout_frame, Sound::FadeFrame fadein_frame )
  {
    // BGM開始
    u32 bgm_id = GetFieldBGM();

    m_currentBGMID = bgm_id;
    Sound::ChangeFieldBGMReq( bgm_id, fadeout_frame, fadein_frame );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  プレイヤーフォルムの取得仮処理
   */
  //-----------------------------------------------------------------------------
  PlayerForm FieldSound::GetPlayerForm( void )
  {
    RIDE_POKEMON_ID ride_poke_id = EventPokemonRideTool::GetPokemonRideOnID( m_pGameManager );

    switch( ride_poke_id ){
    case Field::RIDE_KENTAROSU:
      return PLAYER_FORM_RIDE_KENTAROSU;
    case Field::RIDE_MUURANDO:
      return PLAYER_FORM_RIDE_MUURANDO;
    case Field::RIDE_SAMEHADA:
      return PLAYER_FORM_RIDE_SAMEHADAA;
    case Field::RIDE_LAPLACE:
      return PLAYER_FORM_RIDE_RAPURASU;
    case Field::RIDE_ROBA2:
      return PLAYER_FORM_RIDE_ROBA2;
    case Field::RIDE_KAIRIKY:
      return PLAYER_FORM_RIDE_KAIRIKII;
    default:
      break;
    }
    return PLAYER_FORM_NORMAL;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  プレイヤーフォルムから、フィールドBGMモードに変換
   */
  //-----------------------------------------------------------------------------
  FieldSound::FieldBGMMode FieldSound::GetFieldBGMMode( PlayerForm playerForm )
  {
    switch( playerForm ){
    case PLAYER_FORM_RIDE_KENTAROSU:
    case PLAYER_FORM_RIDE_MUURANDO:
    case PLAYER_FORM_RIDE_ROBA2:
    case PLAYER_FORM_RIDE_KAIRIKII:
      return FIELD_BGM_MODE_RIDE_LAND;
    case PLAYER_FORM_RIDE_SAMEHADAA:
    case PLAYER_FORM_RIDE_RAPURASU:
      return FIELD_BGM_MODE_RIDE_WATER;
    default:
      break;
    }
    return FIELD_BGM_MODE_NORMAL;
  }

  FieldSound::FieldBGMMode FieldSound::GetFieldBGMMode(void)
  {
    PlayerForm playerForm = this->GetPlayerForm();
    return GetFieldBGMMode(playerForm);
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  歩きBGMへの復帰
   *
   *	@note kujiraではPlayerWork->move_formがフォーム移行完了まで更新されないが
   *	      BGMの変更はフォーム変更開始時から始めたいため、フォルム直指定で鳴らすべきBGMを取得している
   *	      (bw世代ではフォーム変更リクエスト時にPlayerWorkを更新かけていた)
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ChangeFieldBGMByPlayerWalk( void )
  {
    u32 bgm_id = GetFieldBGM( PLAYER_FORM_NORMAL );
    m_currentBGMID = bgm_id;
    Sound::ChangeFieldBGMReq( bgm_id, Sound::BGM_FADE_MIDDLE, Sound::BGM_FADE_NONE );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  波乗りBGMへの切り替え
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ChangeFieldBGMByPokeRideSea( void )
  {
    u32 bgm_id = FieldSound::GetSpecialBGM( m_zoneID, true );
    ZoneDataAccessor zoneDataAccessor( m_pZoneDataLoader->GetZoneData( m_zoneID ) );

    if( zoneDataAccessor.IsBGMChangeEnable() && bgm_id == SPECIAL_SOUND_NONE ){
      m_currentBGMID = STRM_BGM_POKERIDE02;
      Sound::ChangeFieldBGMReq( STRM_BGM_POKERIDE02, Sound::BGM_FADE_MIDDLE, Sound::BGM_FADE_NONE );
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ライドBGMへの切り替え
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ChangeFieldBGMByPokeRideLand( void )
  {
    u32 bgm_id = FieldSound::GetSpecialBGM( m_zoneID, true );
    ZoneDataAccessor zoneDataAccessor( m_pZoneDataLoader->GetZoneData( m_zoneID ) );

    if( zoneDataAccessor.IsBGMChangeEnable() && bgm_id == SPECIAL_SOUND_NONE ){
      m_currentBGMID = STRM_BGM_POKERIDE01;
      Sound::ChangeFieldBGMReq( STRM_BGM_POKERIDE01, Sound::BGM_FADE_MIDDLE, Sound::BGM_FADE_NONE );
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  バトル系BGM開始(視線曲/バトル曲/バトルイベント曲)
   */
  //-----------------------------------------------------------------------------
  void FieldSound::StartBattleBGM( u32 bgm_id, Sound::FadeFrame fadeoutFrame, Sound::FadeFrame fadeinFrame /*= Sound::BGM_FADE_NONE*/  )
  {
    Sound::StartBattleBGMReq( bgm_id, fadeoutFrame, fadeinFrame );

    //一度環境音を止める。復帰はフィールド初期化まかせ
//    Sound::StopEnvSEAll();
  }

 //----------------------------------------------------------------------------
  /**
   *	@brief  バトル系BGMチェンジ(視線曲/バトル曲/バトルイベント曲)
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ChangeBattleBGM( u32 bgm_id, Sound::FadeFrame fadeoutFrame /*= Sound::BGM_FADE_MIDDLE*/ , Sound::FadeFrame fadeinFrame /*= Sound::BGM_FADE_NONE*/  )
  {
    Sound::ChangeBattleBGMReq( bgm_id, fadeoutFrame, fadeinFrame );

    //一度環境音を止める。復帰はフィールド初期化まかせ
//    Sound::StopEnvSEAll();
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  バトルBGMから元の曲に戻す
   */
  //-----------------------------------------------------------------------------
  void FieldSound::EndBattleBGM( u32 bgm_id /*= Sound::SOUND_ITEM_ID_NONE*/ )
  {
    if( bgm_id != Sound::SOUND_ITEM_ID_NONE ){
      m_currentBGMID = bgm_id;
    }
    Sound::EndBattleBGMReq( bgm_id );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  バトルBGMから元の曲に戻す:スキップVer
   *	
   *	これを利用した場合、呼び出し元がEndBattleBGMに相当する処理を行う必要があるため
   *	利用は許可制とし、制限する
   */
  //-----------------------------------------------------------------------------
  void FieldSound::EndBattleBGMForSkip( u32 win_bgm_long, u32 win_bgm_short )
  {
    if( Sound::IsBGMPlaying(win_bgm_long) ){
      m_currentBGMID = win_bgm_long;
      return; //変更の必要なし
    }else if( Sound::IsBGMPlaying(win_bgm_short) ){
      m_currentBGMID = win_bgm_short;
      return; //変更の必要なし
    }
    //ショートバージョンのWinBGMに変更して抜ける
    m_currentBGMID = win_bgm_short;
    Sound::ChangeBattleBGMReq( win_bgm_short, Sound::BGM_FADE_VERY_FAST, Sound::BGM_FADE_NONE );
  }
  

  //----------------------------------------------------------------------------
  /**
   *	@brief  バトルBGMから元の曲に戻す 全滅時用
   */
  //-----------------------------------------------------------------------------
  void FieldSound::EndBattleBGMForGameOver( void )
  {
    Sound::EndBattleBGMReqForGameOver();
    //環境音のPause状態などの全ステータスをリセット
//    Sound::ResetEnvSE();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ME再生開始
   */
  //-----------------------------------------------------------------------------
  void FieldSound::PlayME( u32 bgm_id )
  {
    Sound::StartME( bgm_id );
//    Sound::PauseEnvSE( Sound::ENVSE_PAUSE_PUSH );
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ME再生終了待ち
   *
   *	@retval true   再生中
   *	@retval false  再生終了
   *
   *	@note 再生評価をBGMコマンドキューが空になってから行うので注意
   */
  //-----------------------------------------------------------------------------
  bool FieldSound::IsMEPlaying( u32 bgm_id )
  {
    return !Sound::IsMEFinished( bgm_id );
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ME再生後復帰 元のBGMに戻す
   */
  //-----------------------------------------------------------------------------
  void FieldSound::RecoverBGMFromME(void)
  {
    Sound::StartBaseBGMFromME();
//    Sound::ReplayEnvSE( Sound::ENVSE_PAUSE_PUSH );
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ME再生後復帰 別のBGM再生を開始する
   */
  //-----------------------------------------------------------------------------
  void FieldSound::RecoverBGMFromME( u32 bgm_id )
  {
    Sound::StartNewBGMFromME( bgm_id );
//    Sound::ReplayEnvSE( Sound::ENVSE_PAUSE_PUSH );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  BGMボリューム制御
   *
   *	@param  volume  指定ボリューム(1.0がデフォルト)
   *	@param  frame   指定ボリュームに移行するまでのフレーム数(1/60秒換算)
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ChangeBGMVolume( f32 volume, Sound::FadeFrame fade_frame )
  {
    GFL_PRINT("CHG_BGM_VOLUME %f %d\n", volume, fade_frame );
    Sound::ChangeBGMVolume( volume, fade_frame );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  BGMボリューム制御セット 視線トレーナーフォーカス
   *
   *	@param  volume  指定ボリューム(1.0がデフォルト)
   *	@param  frame   指定ボリュームに移行するまでのフレーム数(1/60秒換算)
   */
  //-----------------------------------------------------------------------------
  void FieldSound::SetTrainerFocusMode( f32 volume, Sound::FadeFrame fade_frame )
  {
    GFL_PRINT("SetBGMVolume TrainerForcus %f %d\n", volume, fade_frame );
    Sound::ChangeBGMPlayerVolume( volume, fade_frame );
    m_isValidTrainerFocus = true;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  BGMボリューム制御リセット 視線トレーナーフォーカス
   *	@param  frame   指定ボリュームに移行するまでのフレーム数(1/60秒換算)
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ResetTrainerFocusMode( Sound::FadeFrame fade_frame )
  {
    GFL_PRINT("ResetBGMVolume TrainerForcus %f %d\n", fade_frame );
    Sound::ChangeBGMPlayerVolume( 1.0, fade_frame );
    m_isValidTrainerFocus = false;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  環境音ボリューム制御セット
   *
   *	@param  volume  指定ボリューム(1.0がデフォルト)
   *	@param  frame   指定ボリュームに移行するまでのフレーム数(1/60秒換算)
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ChangeEnvSoundVolume( f32 volume, Sound::FadeFrame fade_frame )
  {
    Sound::ChangePlayerVolume( Sound::SND_PLAYER_IDX_SE_ENV, volume, fade_frame );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  足音ボリューム制御セット
   *
   *	@param  volume  指定ボリューム(1.0がデフォルト)
   *	@param  frame   指定ボリュームに移行するまでのフレーム数(1/60秒換算)
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ChangeFootSoundVolume( f32 volume, Sound::FadeFrame fade_frame )
  {
    Sound::ChangePlayerVolume( Sound::SND_PLAYER_IDX_SE_FT, volume, fade_frame );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  天候ボリューム制御セット
   *
   *	@param  volume  指定ボリューム(1.0がデフォルト)
   *	@param  frame   指定ボリュームに移行するまでのフレーム数(1/60秒換算)
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ChangeWeatherSoundVolume( f32 volume, Sound::FadeFrame fade_frame )
  {
    Sound::ChangePlayerVolume( Sound::SND_PLAYER_IDX_SE_WEATHER, volume, fade_frame );
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  足音グループデータのロードリクエスト
   *
   */
  //-----------------------------------------------------------------------------
  void FieldSound::LoadFootSoundData( gfl2::heap::HeapBase * p_tmp_heap )
  {
    PlayerForm player_move_form = this->GetPlayerForm();
    LoadFootSoundData( player_move_form, p_tmp_heap );
  }
  void FieldSound::LoadFootSoundData( PlayerForm player_move_form, gfl2::heap::HeapBase * p_tmp_heap )
  {
    u32 new_group = GROUP_FIELD_PC;

    switch( player_move_form ){
    case PLAYER_FORM_RIDE_KENTAROSU:
      new_group = GROUP_FIELD_RIDE_KENTAROS;
      break;
    case PLAYER_FORM_RIDE_MUURANDO:
      new_group = GROUP_FIELD_RIDE_MURANDO;
      break;
    case PLAYER_FORM_RIDE_ROBA2:
      new_group = GROUP_FIELD_RIDE_BANBADRO;
      break;
    case PLAYER_FORM_RIDE_KAIRIKII:
      new_group = GROUP_FIELD_RIDE_KAIRIKY;
      break;
    case PLAYER_FORM_RIDE_SAMEHADAA:
      new_group = GROUP_FIELD_RIDE_SAMEHADAR;
      break;
    case PLAYER_FORM_RIDE_RAPURASU:
      new_group = GROUP_FIELD_RIDE_RAPLACE;
      break;
    default:
      break;
    }
#if 0
  GROUP_FIELD_PC               = 720897,  // ( 11 << 16 ) + 1
  GROUP_FIELD_RIDE_KENTAROS    = 720898,  // ( 11 << 16 ) + 2
  GROUP_FIELD_RIDE_MURANDO     = 720899,  // ( 11 << 16 ) + 3
  GROUP_FIELD_RIDE_KAIRIKY     = 720900,  // ( 11 << 16 ) + 4
  GROUP_FIELD_RIDE_BANBADRO    = 720901,  // ( 11 << 16 ) + 5
  GROUP_FIELD_RIDE_RAPLACE     = 720902,  // ( 11 << 16 ) + 6
  GROUP_FIELD_RIDE_SAMEHADAR   = 720903,  // ( 11 << 16 ) + 7
#endif

    ReleaseFootSoundData();
    m_currentFootSound = new_group;

    m_waitFootSound = 0;
    Sound::LoadAndPushSESoundDataAsync( p_tmp_heap->GetLowerHandle(), m_currentFootSound );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  足音グループデータのロード終了待ち
   *
   */
  //-----------------------------------------------------------------------------
  bool FieldSound::IsLoadedFootSoundData( void )
  {
    if( Sound::WaitLoadAndPushSESoundDataAsync() == false ){
      m_waitFootSound++;
      return false;
    }
    GFL_PRINT("FootLoad %d\n", m_waitFootSound );
    return true;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  足音グループデータの破棄
   *
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ReleaseFootSoundData( void )
  {
    if( m_currentFootSound == 0 ) return;

    Sound::PopAndUnloadSESoundData( m_currentFootSound );
    m_currentFootSound = 0;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  リバーブ設定
   *
   */
  //-----------------------------------------------------------------------------
  void FieldSound::SetReverb( gfl2::heap::HeapBase * p_heap )
  {
#if defined(GF_PLATFORM_CTR)
    {
      nw::snd::FxReverb::FilterSize filterSize; ///< フィルタ設定
      nw::snd::FxReverb::Param reverbParam;     ///< リバーブパラメータ
  
      // デフォルトパラメータを設定
      filterSize.m_Comb0 = 1120;
      filterSize.m_Comb1 = 1440;
      filterSize.m_AllPass = 4960;
      reverbParam.m_EarlyReflectionTime = 0;
      reverbParam.m_FusedTime           = 4600;
      reverbParam.m_PreDelayTime          = 1;
      reverbParam.m_Damping               = 0.91f;
      reverbParam.m_EarlyGain             = 0.3f;
      reverbParam.m_FusedGain             = 0.73f;
      reverbParam.m_Coloration            = 0.6;
      reverbParam.m_UseHpfDamping         = 0;
      reverbParam.m_pFilterSize = &filterSize;

      Sound::AppendReverbEffect( p_heap, reverbParam );
    }
#elif defined(GF_PLATFORM_WIN32)
    {
      nw::snd::FxReverbStd::ReverbStdParam reverbParam;     ///< リバーブパラメータ
  
      // デフォルトパラメータを設定
      reverbParam.earlyMode             = nw::snd::FX_REVERB_EARLY_REFLECTION_30MS;
      reverbParam.fusedTime             = 3.0;
      reverbParam.preDelayTime          = 0.2f;
      reverbParam.preDelayTimeMax       = 0.02f;
      reverbParam.damping               = 0.4f;
      reverbParam.earlyGain             = 0.0;
      reverbParam.fusedGain             = 1.0f;
      reverbParam.coloration            = 0.6f;
      reverbParam.outGain               = 1.0f;

      Sound::AppendReverbEffect( p_heap, reverbParam );
    }
#endif

    m_isReberbEnable = true;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  リバーブ設定
   *
   */
  //-----------------------------------------------------------------------------
  void FieldSound::ResetReverb( void )
  {
    if( m_isReberbEnable == false ) return;

    Sound::RemoveReverbEffect( 0 );
    m_isReberbEnable = false;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  FieldSoundデータロード
   */
  //-----------------------------------------------------------------------------
  void FieldSound::Initialize( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* p_heap )
  {
    gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();
    m_pGameManager = p_gman;
    m_pHeap = p_heap;
    m_initSeq = 0;

    //---------------------------------------------------
    // ARCファイルオープン
    //---------------------------------------------------
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId = ARCID_SCRIPT_EVENT_DATA;
      openReq.heapForFile = m_pHeap->GetLowerHandle();
      openReq.heapForArcSrc = m_pHeap->GetLowerHandle();
      openReq.heapForReq = m_pHeap->GetLowerHandle();
      pAsyncFileManager->AddArcFileOpenReq( openReq );
    }
    //---------------------------------------------------
    // ファイル読み込みリクエスト
    //---------------------------------------------------
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

      loadReq.arcId = ARCID_SCRIPT_EVENT_DATA;
      loadReq.datId = GARC_script_event_data_special_bgm_BIN;
      loadReq.ppBuf = &m_pSpecialBGMBuffer;
      loadReq.heapForBuf = m_pHeap;
      loadReq.heapForReq = m_pHeap->GetLowerHandle();
      loadReq.heapForCompressed = NULL;
      loadReq.pBufSize = &m_SpecialBGMBufferSize;
      loadReq.align = 4;
      pAsyncFileManager->AddArcFileLoadDataReq( loadReq );  
    }
  }
  //----------------------------------------------------------------------------
  /**
   *	@brief  FieldSoundデータ待ち
   */
  //-----------------------------------------------------------------------------
  bool FieldSound::InitializeWait( GameSys::GameManager* p_gman )
  {
    gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();
  
    switch( m_initSeq ){
    case 0:
      // 読み込み完了待ち
      if( !pAsyncFileManager->IsArcFileOpenFinished( ARCID_SCRIPT_EVENT_DATA ) ){
        return false;
      }
      if( !pAsyncFileManager->IsArcFileLoadDataFinished( &m_pSpecialBGMBuffer ) ){
        return false;
      }

      //データ格納
      {
        m_SpecialBGMSize = m_SpecialBGMBufferSize/sizeof(SpecialBGM);
      }

      //アーカイブ閉じる
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
        closeReq.arcId = ARCID_SCRIPT_EVENT_DATA;
        closeReq.heapForReq = m_pHeap->GetLowerHandle();
        pAsyncFileManager->AddArcFileCloseReq( closeReq );
      }

      m_initSeq++;
      return false;

    case 1:
      if( !pAsyncFileManager->IsArcFileCloseFinished( ARCID_SCRIPT_EVENT_DATA ) ){
        return false;
      }

      m_initSeq++;
      break;

    default:
      break;
    }
    //読み込みが終了している
    return true;
  }
  //----------------------------------------------------------------------------
  /**
   *	@brief  FieldSoundデータ破棄
   */
  //-----------------------------------------------------------------------------
  void FieldSound::Terminate( void )
  {
    //バッファ開放
    GflHeapSafeFreeMemory(m_pSpecialBGMBuffer);
    m_SpecialBGMSize = 0;
    m_initSeq = 0;
    return;
  }


} // Field

