//=======================================================================================
/**
 * @file EnvSEManager.cpp
 * @brief 環境音管理クラス
 * @author miyuki iwasawa
 * @date 2011.06.27
 */
//=======================================================================================

#include "Sound/include/EnvSEManager.h"
#include "niji_conv_header/sound/SoundDefine.h"
#include "niji_conv_header/sound/SoundMiddleID.h"

namespace Sound { 

  typedef struct ENVSE_DATA{
    u32   soundID;
    b32   loop_f; //ループ音か？
  }ENVSE_DATA;
  const f32 EnvSEManager::DEFAULT_VOLUME = 1.0f; //<デフォルトボリューム

  //-------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap          インスタンス生成に使用するヒープ
   * @param sound_system  関連付けるサウンドシステム
   * @param sound_heap    管理対象のサウンドヒープ
   */
  //-------------------------------------------------------------------------------------
  EnvSEManager::EnvSEManager( void )
  { 
    this->ResetAll();
  }

  /**
   * @brief デストラクタ
   */
  //-------------------------------------------------------------------------------------
  EnvSEManager::~EnvSEManager()
  {
    this->ResetAll();
  }

  //-------------------------------------------------------------------------------------
  /**
   * @brief EnvSE ステータスを全リセットする
   */
  //-------------------------------------------------------------------------------------
  void EnvSEManager::ResetAll( void )
  {
    for( int i = 0;i < MAX_PLAY_NUM; ++i ){
      this->ClearState(i); 
    }
    m_latestIdx = 0;
    m_pauseState = ENVSE_PAUSE_NONE;
  }
  
  //-------------------------------------------------------------------------------------
  /**
   * @brief EnvSE ポーズステータスをセットする
   */
  //-------------------------------------------------------------------------------------
  void EnvSEManager::SetPause( EnvSEPauseState state )
  {
    m_pauseState |= state;
  }
  
  //-------------------------------------------------------------------------------------
  /**
   * @brief EnvSE ポーズステータスをリセットする
   */
  //-------------------------------------------------------------------------------------
  int EnvSEManager::ResetPause( EnvSEPauseState state, PlayState* p_out  )
  {
    int num = 0;

    if( !(m_pauseState & state) ) return 0;

    m_pauseState &= ~state;

    //復帰させるべきSEを返す
    if( m_pauseState != ENVSE_PAUSE_NONE ) return 0;

    for( int i = 0;i < MAX_PLAY_NUM; ++i ){
      if( m_state[i].soundItemID != ITEM_NONE ){
        p_out[num++] = m_state[i];
      }
    }
    return num;
  }
  
  //-------------------------------------------------------------------------------------
  /**
   * @brief EnvSE 再生登録 
   */
  //-------------------------------------------------------------------------------------
  b32 EnvSEManager::Entry( u32 soundID, f32 volume )
  {
    if( this->IsLoopSE( soundID ) ){
      //ループSEなら登録処理
      int idx = this->SearchBlankState( soundID );
      if( idx < 0 ){
        return false; //登録失敗
      }
      this->EntryState( idx, soundID, volume );
    }
    //ポーズ状態でなければ再生リクエスト
    return this->IsActive();
  }
  
  //-------------------------------------------------------------------------------------
  /**
   * @brief EnvSE 登録解放
   */
  //-------------------------------------------------------------------------------------
  void EnvSEManager::Release( u32 soundID )
  {
    this->ClearStateBySoundID( soundID );
  }
  
  //-------------------------------------------------------------------------------------
  /**
   * @brief EnvSE ボリュームステータスを変更する
   */
  //-------------------------------------------------------------------------------------
  b32 EnvSEManager::ChangeVolume( u32 soundID, f32 volume )
  {
    if( this->SetVolumeStatus( soundID, volume ) ){
      return this->IsActive();
    }
    return false;
  }

  //-------------------------------------------------------------------------------------
  /**
   * @brief 最新の再生情報indexを取得する
   */
  //-------------------------------------------------------------------------------------
  u32 EnvSEManager::GetLatestIdx( void ) const
  {
    return m_latestIdx;
  }
  
  //-------------------------------------------------------------------------------------
  /**
   * @brief EnvSE ステータスを登録する
   */
  //-------------------------------------------------------------------------------------
  void EnvSEManager::EntryState( u32 idx, u32 soundID, f32 volume )
  {
    m_state[idx].soundItemID = soundID;
    m_state[idx].volume = volume;

    m_latestIdx = idx;
  }
  
  //-------------------------------------------------------------------------------------
  /**
   * @brief EnvSE ボリュームステータスを変更する
   */
  //-------------------------------------------------------------------------------------
  b32 EnvSEManager::SetVolumeStatus( u32 soundID, f32 volume )
  {
    if( !this->IsLoopSE( soundID ) ){
      return true; //ループSEでなければステータス変更処理の必要がない
    }
    for( int i = 0; i < MAX_PLAY_NUM; ++i ){
      if( m_state[i].soundItemID == soundID ){
        m_state[i].volume = volume;
        return true;
      }
    }
    return false;
  }
  
  //-------------------------------------------------------------------------------------
  /**
   * @brief EnvSE 指定した再生ステータスをリセットする index指定
   */
  //-------------------------------------------------------------------------------------
  void EnvSEManager::ClearState( u32 idx )
  {
    m_state[idx].soundItemID = ITEM_NONE;
    m_state[idx].volume = DEFAULT_VOLUME;
  }
  
  //-------------------------------------------------------------------------------------
  /**
   * @brief EnvSE 指定した再生ステータスをリセットする サウンドIDからサーチ
   */
  //-------------------------------------------------------------------------------------
  void EnvSEManager::ClearStateBySoundID( u32 soundID )
  {
    for( int i = 0; i < MAX_PLAY_NUM; ++i ){
      if( m_state[i].soundItemID == soundID ){
        int idx = i^1;
        this->ClearState(i);

        if( m_state[idx].soundItemID != ITEM_NONE ){
          m_latestIdx = idx;
        }else{
          m_latestIdx = 0;
        }
      }
    }
  }
  
  
  //-------------------------------------------------------------------------------------
  /**
   * @brief EnvSE 管理領域の空きを探す
   */
  //-------------------------------------------------------------------------------------
  int EnvSEManager::SearchBlankState( u32 soundID )
  {
    int i = 0;
    for( i = 0; i < MAX_PLAY_NUM; ++i ){
      if( m_state[i].soundItemID == soundID ){
        return i; //登録済み
      }
      if( m_state[i].soundItemID == ITEM_NONE ){
        return i;
      }
    }
    return -1;
  }
  
  //-------------------------------------------------------------------------------------
  /**
   * @brief EnvSE ループ音かチェック
   */
  //-------------------------------------------------------------------------------------
  b32 EnvSEManager::IsLoopSE( u32 soundID )
  {
    return false;
  }

} // Sound
