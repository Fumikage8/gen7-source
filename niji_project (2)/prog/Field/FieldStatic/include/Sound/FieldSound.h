//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldSound.h
 *	@brief  フィールドサウンド管理モジュール
 *	@author Miyuki Iwasawa	
 *	@date		2015.06.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELDSOUND_H__
#define __FIELDSOUND_H__


// 前方宣言
namespace GameSys{
  class GameManager;
  class GameData;
}

#include "Sound/include/Sound.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "niji_conv_header/sound/SoundDefine.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

namespace Field{

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//----------------------------------------------------------------------------

  class FieldSound
  {
    GFL_FORBID_COPY_AND_ASSIGN(FieldSound); //コピーコンストラクタ＋代入禁止
  
  public:
    enum FieldBGMMode{
      FIELD_BGM_MODE_NORMAL,
      FIELD_BGM_MODE_RIDE_LAND,
      FIELD_BGM_MODE_RIDE_WATER,
    };
  private:
    struct SpecialBGM{
      u16     zoneID;
      u8      isHighPriority;
      u8      padding;
      s32     flagNo;
      u32     soundDayIdx;
      u32     soundNightIdx;
    };

    struct SpecialEnvSE{
      u16 stopFlagNo; //指定フラグがOnなら止める。SpBGMと逆なので注意
      u16 zoneID;
      u32 soundIdx;
    };

  private:
    static const u32 PLAYER_NO_CONTACT = 1;
    static const u32 SPECIAL_SOUND_NONE = 0xFFFFFFFF;  //無効値

  public:

    // コンストラクタ　デストラクタ
    FieldSound( const GameSys::GameData* p_gdata );
    ~FieldSound();

    //----------------------------------------------------------------------------
    /**
     *	@brief  初期化処理
     */
    //-----------------------------------------------------------------------------
    void Initialize( void );
    
    //----------------------------------------------------------------------------
    /**
     *	@brief  破棄処理
     */
    //-----------------------------------------------------------------------------
    void Finalize( void );
    
    //----------------------------------------------------------------------------
    /**
     *	@brief  現在のゾーンIDセット
     *
     *	@param  zone_ID  ゾーンID
     */
    //-----------------------------------------------------------------------------
    void SetCurrentZoneID( ZoneID zoneID ){ m_zoneID = zoneID; }
    
    //----------------------------------------------------------------------------
    /**
     *	@brief  スペシャルBGM取得
     *
     *	@param  zone_ID  ゾーンID
     */
    //-----------------------------------------------------------------------------
    u32 GetSpecialBGM( ZoneID zoneID, b32 checkPriority );
    
    //----------------------------------------------------------------------------
    /**
     *	@brief  スペシャルEnvSE取得
     *
     *	@param  zone_ID  ゾーンID
     */
    //-----------------------------------------------------------------------------
    u32 GetSpecialEnvSE( ZoneID zoneID );
    
    //----------------------------------------------------------------------------
    /**
     *	@brief  ゾーン スペシャルEnvSEセットアップ
     *
     *	@param  zone_ID  ゾーンID
     */
    //-----------------------------------------------------------------------------
    void PlayZoneSpecialEnvSE( void );
  
    //----------------------------------------------------------------------------
    /**
     *	@brief  ゾーン スペシャルEnvSE終了
     *
     *	@param  zone_ID  ゾーンID
     */
    //-----------------------------------------------------------------------------
    void StopZoneSpecialEnvSE( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  フィールドサウンド：昼間かどうか？
     */
    //-----------------------------------------------------------------------------
    b32 IsDayTime( void );
    b32 IsDayTimeFromEvTime( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  ライドBGM判定
     *	@param  priority_ride_bgm   trueだとBGM切り替えチェックでライドBGM優先
     */
    //-----------------------------------------------------------------------------
    u32 GetRideBGM( ZoneID zoneID, PlayerForm player_move_form, bool priority_ride_bgm  );
    
    //----------------------------------------------------------------------------- 
    //----------------------------------------------------------------------------
    /**
     *	@brief  現在鳴らすべきBGM 
     *
     *	@param  gamedata            ゲームデータ
     *	@param  player_move_form    プレイヤー動作フォーム
     *	@param  priority_ride_bgm   trueだとBGM切り替えチェックでライドBGM優先
     */
    //-----------------------------------------------------------------------------
    u32 GetFieldBGM( ZoneID zoneID, PlayerForm player_move_form, bool priority_ride_bgm  );
    u32 GetFieldBGM( PlayerForm player_move_form );
    u32 GetFieldBGM( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  フィールドフリームーブ状態でのサウンドステータスをケア
     */
    //-----------------------------------------------------------------------------
    void CareFieldBGMState( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  時間監視によるBGMの切り替えチェック
     */
    //-----------------------------------------------------------------------------
    void UpdateFieldBGM( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  ゾーンチェンジ　BGMプレイ
     *
     *	@param  gamedata          ゲームデータ
     *	@param  player_move_form  プレイヤー動作フォーム
     *	@param  adjacent_f        地続きゾーンチェンジか？
     */
    //-----------------------------------------------------------------------------
    void ChangeFieldBGMByZoneChange( b32 adjacent_f =false );

    //----------------------------------------------------------------------------
    /**
     *	@brief  現在のマップのBGMを鳴らす
     *	@param  fadeout_frame フェードアウトフレーム
     *	@param  fadein_frame  フェードインフレーム
     *
     *	イベント途中でスペシャルBGMへの鳴らし換えを行いたい時などに使う
     */
    //-----------------------------------------------------------------------------
    void ChangeFieldBGMNowMap( Sound::FadeFrame fadeout_frame =  Sound::BGM_FADE_SLOW, Sound::FadeFrame fadein_frame = Sound::BGM_FADE_MIDDLE );

    //----------------------------------------------------------------------------
    /**
     *	@brief  歩きBGMへの復帰
     */
    //-----------------------------------------------------------------------------
    void ChangeFieldBGMByPlayerWalk( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  フォルム別BGMへの切り替え
     */
    //-----------------------------------------------------------------------------
    PlayerForm GetPlayerForm( void );
    FieldBGMMode GetFieldBGMMode( PlayerForm playerForm );
    FieldBGMMode GetFieldBGMMode(void);
    void ChangeFieldBGMByPokeRideSea( void );
    void ChangeFieldBGMByPokeRideLand( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  スタンバイ　NextFieldBGM
     */
    //-----------------------------------------------------------------------------
    void PrepareNextFieldBGM( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  スタンバイ　プレイ
     */
    //-----------------------------------------------------------------------------
    void PreparePlayNextFieldBGM( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  開始BGM再生
     */
    //-----------------------------------------------------------------------------
    void StartFirstFieldBGM( void );
  
    //----------------------------------------------------------------------------
    /**
     *	@brief  イベントBGM開始
     *
     *	@note かならずEndEventBGM()とセットで用いること
     */
    //-----------------------------------------------------------------------------
    void StartEventBGM( u32 bgm_id, Sound::FadeFrame fadeoutFrame, Sound::FadeFrame fadeinFrame = Sound::BGM_FADE_NONE );
  
    //----------------------------------------------------------------------------
    /**
     *	@brief  イベントBGM変更
     *
     *	@note StartEventBGM-EndEventBGMの間でのみ利用可
     */
    //-----------------------------------------------------------------------------
    void ChangeEventBGM( u32 bgm_id, Sound::FadeFrame fadeoutFrame = Sound::BGM_FADE_SLOW, Sound::FadeFrame fadeinFrame = Sound::BGM_FADE_MIDDLE );
    
    //----------------------------------------------------------------------------
    /**
     *	@brief  イベントBGM終了
     *	@param  fadeout_frame フェードアウトフレーム
     *	@param  fadein_frame  フェードインフレーム
     *
     *	@note かならずStartEventBGM()とセットで用いること
     */
    //-----------------------------------------------------------------------------
    void EndEventBGM( Sound::FadeFrame fadeoutFrame = Sound::BGM_FADE_SLOW, Sound::FadeFrame fadeinFrame = Sound::BGM_FADE_MIDDLE );

    //----------------------------------------------------------------------------
    /**
     *	@brief  デモBGM終了 BGM復帰
     *	@param  fadeout_frame フェードアウトフレーム
     *	@param  fadein_frame  フェードインフレーム
     *
     *	@note デモでBGMな書き換えらた場合のみに使用
     */
    //-----------------------------------------------------------------------------
    void EndDemoBGM( Sound::FadeFrame fadeoutFrame, Sound::FadeFrame fadeinFrame );


    //----------------------------------------------------------------------------
    /**
     *	@brief  バトル系BGM開始(視線曲/バトル曲)
     */
    //-----------------------------------------------------------------------------
    void StartBattleBGM( u32 bgm_id, Sound::FadeFrame fadeoutFrame = Sound::BGM_FADE_VERY_FAST, Sound::FadeFrame fadeinFrame = Sound::BGM_FADE_NONE  );

    //----------------------------------------------------------------------------
    /**
     *	@brief  バトル系BGMチェンジ(視線曲/バトル曲)
     */
    //-----------------------------------------------------------------------------
    void ChangeBattleBGM( u32 bgm_id, Sound::FadeFrame fadeoutFrame = Sound::BGM_FADE_MIDDLE, Sound::FadeFrame fadeinFrame = Sound::BGM_FADE_NONE  );

    //----------------------------------------------------------------------------
    /**
     *	@brief  バトルBGMからフィールド曲に戻す
     *
     *  @note  bgm_idがSOUND_ITEM_ID_NONEの場合はStartBattleBGMReq再生時点の曲を途中から再生
     *         有効なBGMIDを指定した場合、Pop処理後に新しいBGMを流す
     */
    //-----------------------------------------------------------------------------
    void EndBattleBGM( u32 bgm_id = Sound::SOUND_ITEM_ID_NONE );

    //----------------------------------------------------------------------------
    /**
    *	@brief  バトルBGMから元の曲に戻す:フィールドBGMに戻す処理をスキップするVer
    *	
    *	これを利用した場合、呼び出し元がEndBattleBGMに相当する処理を行う必要があるため
    *	利用は許可制とし、制限する
    */
    //-----------------------------------------------------------------------------
    void FieldSound::EndBattleBGMForSkip( u32 win_bgm_long, u32 win_bgm_short );

    //----------------------------------------------------------------------------
    /**
     *	@brief  バトルBGMから元の曲に戻す 全滅時用
     */
    //-----------------------------------------------------------------------------
    void EndBattleBGMForGameOver( void );
  
    //----------------------------------------------------------------------------
    /**
     *	@brief  ME再生開始
     */
    //-----------------------------------------------------------------------------
    void PlayME( u32 bgm_id );
  
    //----------------------------------------------------------------------------
    /**
     *	@brief  ME再生中チェック
     *
     *	@retval true   再生中
     *	@retval false  再生終了
     *
     *	@note 再生評価をBGMコマンドキューが空になってから行うので注意
     */
    //-----------------------------------------------------------------------------
    bool IsMEPlaying( u32 bgm_id );
  
    //----------------------------------------------------------------------------
    /**
     *	@brief  ME再生後復帰
     */
    //-----------------------------------------------------------------------------
    void RecoverBGMFromME(void);
  
    //----------------------------------------------------------------------------
    /**
     *	@brief  ME再生後復帰 別のBGM再生を開始する
     */
    //-----------------------------------------------------------------------------
    void RecoverBGMFromME( u32 bgm_id );
  
    //----------------------------------------------------------------------------
    /**
     *	@brief  BGMボリューム制御
     *
     *	@param  volume  指定ボリューム(1.0がデフォルト)
     *	@param  frame   指定ボリュームに移行するまでのフレーム数(1/60秒換算)
     */
    //-----------------------------------------------------------------------------
    void ChangeBGMVolume( f32 volume, Sound::FadeFrame fade_frame );

     //----------------------------------------------------------------------------
    /**
     *	@brief  BGMボリューム制御セット 視線トレーナーフォーカス
     *
     *	@param  volume  指定ボリューム(1.0がデフォルト)
     *	@param  frame   指定ボリュームに移行するまでのフレーム数(1/60秒換算)
     */
    //-----------------------------------------------------------------------------
    void SetTrainerFocusMode( f32 volume, Sound::FadeFrame fade_frame );

    //----------------------------------------------------------------------------
    /**
     *	@brief  BGMボリューム制御リセット 視線トレーナーフォーカス
     *	@param  frame   指定ボリュームに移行するまでのフレーム数(1/60秒換算)
     */
    //-----------------------------------------------------------------------------
    void ResetTrainerFocusMode( Sound::FadeFrame fade_frame );

    //----------------------------------------------------------------------------
    /**
     *	@brief  BGMボリューム制御が有効か？　 視線トレーナーフォーカス
     */
    //-----------------------------------------------------------------------------
    bool IsValidTrainerFocusMode( void ){ return m_isValidTrainerFocus; };

    //----------------------------------------------------------------------------
    /**
     *	@brief  環境音ボリューム制御セット
     *
     *	@param  volume  指定ボリューム(1.0がデフォルト)
     *	@param  frame   指定ボリュームに移行するまでのフレーム数(1/60秒換算)
     */
    //-----------------------------------------------------------------------------
    void ChangeEnvSoundVolume( f32 volume, Sound::FadeFrame fade_frame );

    //----------------------------------------------------------------------------
    /**
     *	@brief  足音ボリューム制御セット
     *
     *	@param  volume  指定ボリューム(1.0がデフォルト)
     *	@param  frame   指定ボリュームに移行するまでのフレーム数(1/60秒換算)
     */
    //-----------------------------------------------------------------------------
    void ChangeFootSoundVolume( f32 volume, Sound::FadeFrame fade_frame );

    //----------------------------------------------------------------------------
    /**
     *	@brief  天候音ボリューム制御セット
     *
     *	@param  volume  指定ボリューム(1.0がデフォルト)
     *	@param  frame   指定ボリュームに移行するまでのフレーム数(1/60秒換算)
     */
    //-----------------------------------------------------------------------------
    void ChangeWeatherSoundVolume( f32 volume, Sound::FadeFrame fade_frame );

    //----------------------------------------------------------------------------
    /**
    *	@brief  足音グループデータのロードリクエスト
    */
    //-----------------------------------------------------------------------------
    void LoadFootSoundData( gfl2::heap::HeapBase * p_tmp_heap );
    void LoadFootSoundData( PlayerForm player_move_form, gfl2::heap::HeapBase * p_tmp_heap );
    bool IsLoadedFootSoundData( void );
    void ReleaseFootSoundData( void );


    //----------------------------------------------------------------------------
    /**
    *	@brief  リバーブ
    */
    //-----------------------------------------------------------------------------
    void SetReverb( gfl2::heap::HeapBase * p_heap );
    void ResetReverb( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  FieldSoundデータロード
     */
    //-----------------------------------------------------------------------------
    void Initialize( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* pHeap );

    //----------------------------------------------------------------------------
    /**
     *	@brief  FieldSoundデータ待ち
     */
    //-----------------------------------------------------------------------------
    bool InitializeWait( GameSys::GameManager* p_gman );

    //----------------------------------------------------------------------------
    /**
     *	@brief  FieldSoundデータ破棄
     */
    //-----------------------------------------------------------------------------
    void Terminate( void );

  private:
    GameSys::GameManager* m_pGameManager;
    const GameSys::GameData* m_pGameData;
    Field::ZoneDataLoader* m_pZoneDataLoader;

    int     m_initSeq;
    bool    m_isReberbEnable;
    u32     m_specialEnvSeNo;
    u32     m_currentBGMID;
    ZoneID  m_zoneID;
    u32     m_currentFootSound;
    u32     m_waitFootSound;

    gfl2::heap::HeapBase* m_pHeap;
    void* m_pSpecialBGMBuffer;
    u32   m_SpecialBGMBufferSize;
    u32   m_SpecialBGMSize;

    bool    m_isValidTrainerFocus;

  };

} // Field

#endif	// __FIELDSOUND_H__

