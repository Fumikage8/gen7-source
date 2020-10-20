//=============================================================================================
/**
 * @file    btl_sound.cpp
 * @brief   XY バトルシステム サウンド制御
 * @author  taya
 *
 * @date  2012.04.19  作成
 */
//=============================================================================================
#include <Sound/include/Sound.h>
#include "./btl_sound.h"

GFL_NAMESPACE_BEGIN(btl)

//--------------------------------------------------------------
// ctor / dtor
//--------------------------------------------------------------
BtlSound::BtlSound( void )
 : m_bPinchModeDisable(false)
 , m_bBgmSwitchReq(false)
 , m_bgmPushCounter(0)
 , m_nextBgmNo(0)
 , m_bgmVolume(1.0f)
 , m_fadeBgmMega(false)
 , m_fadeBgmZWaza(false)
{
}
BtlSound::~BtlSound( void )
{

}

//--------------------------------------------------------------
/**
 *  初期化
 */
//--------------------------------------------------------------
void BtlSound::Initialize( void )
{
}

//--------------------------------------------------------------
/**
 *  Main
 */
//--------------------------------------------------------------
void BtlSound::Main( void )
{
  if( m_bBgmSwitchReq )
  {
    if( !this->CheckFadeOnBGM() )
    {
      m_bBgmSwitchReq = false;
      Sound::StartBGMReq( m_nextBgmNo );
    }
  }
}

//--------------------------------------------------------------
/**
 *  BGM 再生
 */
//--------------------------------------------------------------
void BtlSound::StartBGM( u32 bgmNo )
{
  m_bPinchModeDisable = true;
  Sound::StartBGMReq( bgmNo );
}
//--------------------------------------------------------------
/**
 *  現在の BGM をフェードアウトし、次の BGM へ切り替え
 */
//--------------------------------------------------------------
void BtlSound::SwitchBGM( u32 nextBGMNo, u32 fadeFrames )
{
  m_bPinchModeDisable = true;
  m_bBgmSwitchReq = true;
  m_nextBgmNo = nextBGMNo;;
  this->FadeOutBGM( fadeFrames );
}
//--------------------------------------------------------------
/**
 *  BGM 停止
 */
//--------------------------------------------------------------
void BtlSound::StopBGM( void )
{
  Sound::EndBGMReq();
}

//--------------------------------------------------------------
/**
 *  BGM フェードアウト開始
 *
 *  @note ノイズ防止のため10frame = Sound::BGM_FADE_VERY_FAST以上の値を指定すること
 */
//--------------------------------------------------------------
void BtlSound::FadeOutBGM( u32 frames )
{
  Sound::FadeOutBGMReq( static_cast<Sound::FadeFrame>(frames) );
}
//--------------------------------------------------------------
/**
 *  BGM フェードイン開始
 */
//--------------------------------------------------------------
void BtlSound::FadeInBGM( u32 frames )
{
  Sound::FadeInBGMReq( static_cast<Sound::FadeFrame>(frames) );
}
//--------------------------------------------------------------
/**
 * BGM フェード中かどうか判定
 *
 * @retval  bool    フェード中なら true
 */
//--------------------------------------------------------------
bool BtlSound::CheckFadeOnBGM( void )
{
  return GFL_BOOL_CAST( Sound::IsBGMFade() );
}

//--------------------------------------------------------------
/**
 * ピンチBGMへ切り替え
 */
//--------------------------------------------------------------
void BtlSound::StartPinchBGM( void )
{
  if( !m_bPinchModeDisable ){
    //鳴らすチェックはbtlv_coreで行っている。
    //個々のチェックになったので注意

    if( Sound::IsSEPlaying( SEQ_SE_BT_031 ) == false )
    {
      Sound::PlaySE( SEQ_SE_BT_031 );
    }
  }
}
//--------------------------------------------------------------
/**
 * ピンチBGMから通常BGMへ復帰
 */
//--------------------------------------------------------------
void BtlSound::StopPinchBGM( void )
{
  if( !m_bPinchModeDisable ){
    //処理はないが呼ばれている。
  }
}


//--------------------------------------------------------------
/**
 * SE再生
 */
//--------------------------------------------------------------
void BtlSound::PlaySE( u32 SENo )
{
  Sound::PlaySE( SENo );
}
//--------------------------------------------------------------
/**
 * SE終了チェック
 */
//--------------------------------------------------------------
bool BtlSound::IsSEFinished( u32 SENo )
{
  return GFL_BOOL_CAST( Sound::IsSEFinished( SENo ) );
}


//--------------------------------------------------------------
/**
 * ME再生
 */
//--------------------------------------------------------------
void BtlSound::StartME( u32 MENo )
{
  Sound::StartME( MENo );
  ++m_bgmPushCounter;
  BTL_PRINT( "[btlSnd: %p] Me(%d)を鳴らす Counter =%d\n", this, MENo, m_bgmPushCounter );
}
//--------------------------------------------------------------
/**
 * ME終了待ち
 */
//--------------------------------------------------------------
bool BtlSound::WaitMEFinish( u32 MENo )
{
  bool result = GFL_BOOL_CAST( Sound::IsMEFinished( MENo ) );
  return result;
}
//--------------------------------------------------------------
/**
 * ME再生中チェック
 *
 * @note MEが鳴り始めたタイミングをチェックする関数です。
 *       BGMOffデバッグ有効時は即座にtrueが返るため
 *       ME再生終了を検知するには WaitMEFinsish()を使ってください。
 */
//--------------------------------------------------------------
bool BtlSound::IsMEPlaying( u32 MENo )
{
#if PM_DEBUG
  //撮影用BGMOffデバッグ機能/全サウンドOff利用中は速攻でtrueを返す
  DEBUG_SOUND_OFF_RET(true);

  if( Sound::IsBGMControlEnable() == false ) {
    return true;
  }
#endif //PM_DEBUG

  return !Sound::IsMEFinished( MENo );
}

void BtlSound::RestartBaseBGM( void )
{
  Sound::StartBaseBGMFromME();
  --m_bgmPushCounter;
  BTL_PRINT( "[btlSnd: %p] ME終わりで戻す Layer %d\n", this, m_bgmPushCounter );
}
void BtlSound::StartNewBGMFromME( u32 bgmNo )
{
  Sound::StartNewBGMFromME( bgmNo );
  --m_bgmPushCounter;
  BTL_PRINT( "[btlSnd: %p] ME終わりで戻す Layer %d\n", this, m_bgmPushCounter );
}

//--------------------------------------------------------------
/**
 *  再生BGMレイヤーを初期状態に強制的に戻す
 */
//--------------------------------------------------------------
void BtlSound::ForceFlatBgmLayer( void )
{
  while( m_bgmPushCounter > 0 )
  {
    Sound::PopBGMOnlyReq();
    --m_bgmPushCounter;
    GFL_ASSERT_MSG( 0, "BGMレイヤー数が元に戻っていない %d ME再生復帰処理/BGMダイレクト系コマンドを使用していないかなど要確認\n", m_bgmPushCounter ); //@check iwasawa このループでケアされている
  }
}

//------------------------------------------------------------------------------------
/**
 * プレーヤー毎ボリュームの設定（WB時代に存在していた処理：kuriraで必要になるかどうか未検証だが、ハリボテだけ用意しておく）
 */
//------------------------------------------------------------------------------------
void BtlSound::StopAllSE( void )
{
  Sound::StopAllSE();
}

//技、鳴き声再生時のBGM制御

//技エフェクト発動
#define BGM_VOLUME_DOWN_RATIO ( 0.85f )  //マスターボリューム(1.0で100%)
#define BGM_VOLUME_DOWN_FRAME ( 20 )  //EFFVM_VOLUME_DOWN_RATIOの％に何フレームでなるかを指定（60で1秒）
#define BGM_VOLUME_UP_FRAME ( 20 )    //100％に何フレームでなるかを指定（60で1秒）
//ポケモンの鳴き声
#define BGM_VOLUME_DOWN_RATIO_PV ( 0.75f )  //マスターボリューム(1.0で100%)
#define BGM_VOLUME_DOWN_RATIO_PV_PIKA5 ( 0.75f )  //マスターボリューム(1.0で100%)
#define BGM_VOLUME_DOWN_FRAME_PV ( 10 )  //EFFVM_VOLUME_DOWN_RATIOの％に何フレームでなるかを指定（60で1秒）
#define BGM_VOLUME_UP_FRAME_PV ( 10 )    //100％に何フレームでなるかを指定（60で1秒）

//メガシンカ発動
#define BGM_VOLUME_DOWN_RATIO_MEGA_EVO ( 0.75 )  //マスターボリューム(1.0で100%)
#define BGM_VOLUME_DOWN_FRAME_MEGA_EVO ( 30 )  //EFFVM_VOLUME_DOWN_RATIOの％に何フレームでなるかを指定（60で1秒）
#define BGM_VOLUME_UP_FRAME_MEGA_EVO   ( 30 )    //100％に何フレームでなるかを指定（60で1秒）

//全力技
#define BGM_VOLUME_DOWN_RATIO_Z_WAZA ( 0.5 )  //マスターボリューム(1.0で100%)
#define BGM_VOLUME_DOWN_FRAME_Z_WAZA ( 30 )  //EFFVM_VOLUME_DOWN_RATIOの％に何フレームでなるかを指定（60で1秒）
#define BGM_VOLUME_UP_FRAME_Z_WAZA   ( 30 )    //100％に何フレームでなるかを指定（60で1秒）

void BtlSound::BgmVolumeDown_Waza(void)
{
  if( m_fadeBgmZWaza )
  {
    //全力技中は、そちらのBGMフェードを優先する
    return;
  }
  Sound::ChangeBGMVolume( BGM_VOLUME_DOWN_RATIO , BGM_VOLUME_DOWN_FRAME );
  m_fadeBgmWaza = true;
  m_bgmVolume = BGM_VOLUME_DOWN_RATIO;
}

void BtlSound::BgmVolumeReset_Waza(void)
{
  if( m_fadeBgmWaza == false )
  {
    return;
  }
  Sound::ChangeBGMVolume( 1.0f , BGM_VOLUME_UP_FRAME );
  m_fadeBgmWaza = false;
  m_bgmVolume = 1.0f;
}

void BtlSound::BgmVolumeDown_Voice(bool isPikach5 )
{
  if( m_fadeBgmMega || m_fadeBgmWaza || m_fadeBgmZWaza )
  {
    return;
  }

  if( isPikach5 )
  {
    Sound::ChangeBGMVolume( BGM_VOLUME_DOWN_RATIO_PV_PIKA5 , BGM_VOLUME_DOWN_FRAME_PV );
  }
  else
  {
    Sound::ChangeBGMVolume( BGM_VOLUME_DOWN_RATIO_PV , BGM_VOLUME_DOWN_FRAME_PV );
  }
  m_fadeBgmVoice = true;
  m_bgmVolume = BGM_VOLUME_DOWN_RATIO_PV;
}

void BtlSound::BgmVolumeReset_Voice(void)
{
  if( m_fadeBgmVoice == false )
  {
    return;
  } 
  Sound::ChangeBGMVolume( 1.0f , BGM_VOLUME_UP_FRAME_PV );
  m_fadeBgmVoice = false;
  m_bgmVolume = 1.0f;
}

//メガシンカ
void BtlSound::BgmVolumeDown_MegaEvo(void)
{
  m_fadeBgmMega = true;
  Sound::ChangeBGMVolume( BGM_VOLUME_DOWN_RATIO_MEGA_EVO , BGM_VOLUME_DOWN_FRAME_MEGA_EVO );

  m_bgmVolume = BGM_VOLUME_DOWN_RATIO_MEGA_EVO;
}

void BtlSound::BgmVolumeReset_MegaEvo(void)
{
  m_fadeBgmMega = false;
  Sound::ChangeBGMVolume( 1.0f , BGM_VOLUME_UP_FRAME_MEGA_EVO );

  m_bgmVolume = 1.0f;
}

//全力技
void BtlSound::BgmVolumeDown_ZWaza(void)
{
  m_fadeBgmZWaza = true;
  Sound::ChangeBGMVolume( BGM_VOLUME_DOWN_RATIO_Z_WAZA , BGM_VOLUME_DOWN_FRAME_Z_WAZA );

  m_bgmVolume = BGM_VOLUME_DOWN_RATIO_Z_WAZA;
}
void BtlSound::BgmVolumeReset_ZWaza(void)
{
  m_fadeBgmZWaza = false;
  Sound::ChangeBGMVolume( 1.0f , BGM_VOLUME_UP_FRAME_Z_WAZA );

  m_bgmVolume = 1.0f;
}

GFL_NAMESPACE_END(btl)

