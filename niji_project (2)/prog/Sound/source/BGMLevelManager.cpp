//=======================================================================================
/**
 * @file BGMLevelManager.cpp
 * @brief サウンド階層管理クラス
 * @author Miyuki Iwasawa 
 * @date 2015.03.11
 */
//=======================================================================================

#include "Sound/include/BGMLevelManager.h"

namespace Sound { 

  //-------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap          インスタンス生成に使用するヒープ
   * @param sound_system  関連付けるサウンドシステム
   * @param sound_heap    管理対象のサウンドヒープ
   * @param settings      動作設定データ
   */
  //-------------------------------------------------------------------------------------
  BGMLevelManager::BGMLevelManager( 
      gfl2::heap::HeapBase* heap, 
      gfl2::sound::SoundSystem* sound_system,
      gfl2::sound::SoundHeap* sound_heap,
      const Settings& settings ) :
    m_soundSystem( sound_system ),
    m_soundHeap( sound_heap ),
    m_levelState( NULL ),
    m_currentLevel( 0 )
  { 
    nw::snd::SoundHeap* nw_sound_heap = sound_heap->GetNwSoundHeap();
    s32 initial_heap_level = nw_sound_heap->GetCurrentLevel();

    m_levelState = GFL_NEW( heap ) LevelState[ BGM_PUSH_COUNT_MAX ];

    for( u32 i=0; i<BGM_PUSH_COUNT_MAX; i++ ) 
    {
      this->InitHeapLevelState( 
          i,
          initial_heap_level,
          settings.soundHandle[i] );
    }
  }

  /**
   * @brief 階層の状態を初期化する
   * @param level               初期化する階層
   * @param initial_heap_level  ヒープレベルの初期値
   * @param sound_handle_index  階層に関連付けるサウンドハンドル
   */
  void BGMLevelManager::InitHeapLevelState(
      u32 level,
      s32 initial_heap_level, 
      u32 sound_handle_index )
  {
    this->ClearHeapLevelState( level, initial_heap_level );

    LevelState* level_state = this->GetHeapLevelState( level );
    level_state->soundHandleIndex = sound_handle_index;
  } 

  /**
   * @brief 階層の状態をクリアする
   * @param level               初期化する階層
   * @param initial_heap_level  ヒープレベルの初期値
   * @param sound_handle_index  階層に関連付けるサウンドハンドル
   */
  void BGMLevelManager::ClearHeapLevelState( u32 level, s32 initial_heap_level )
  {
    LevelState* level_state = this->GetHeapLevelState( level );
    level_state->loadFlag = false;
    level_state->soundItemID = 0;
    level_state->soundHeapLevel_beforeLoad = initial_heap_level;
    level_state->soundHeapLevel_afterLoadCore = initial_heap_level;
    level_state->startInfo.startOffset = -1;
    level_state->startInfo.enableFlag = nw::snd::SoundStartable::StartInfo::ENABLE_START_OFFSET;
    level_state->startInfo.startOffsetType = nw::snd::SoundStartable::StartInfo::START_OFFSET_TYPE_SAMPLE;
    level_state->startInfo.startOffset = -1;
  } 

  //-------------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-------------------------------------------------------------------------------------
  BGMLevelManager::~BGMLevelManager()
  {
    GFL_DELETE_ARRAY m_levelState;
  }

  //-------------------------------------------------------------------------------------
  // 現在のBGM階層レベル
  //-------------------------------------------------------------------------------------
  u32 BGMLevelManager::GetCurrentLevel( void ) const
  {
    return m_currentLevel;
  }

  //-------------------------------------------------------------------------------------
  // 現在のヒープ階層にあるサウンドアイテムIDを取得する
  //-------------------------------------------------------------------------------------
  u32 BGMLevelManager::GetSoundItemIDOnCurrentLevel( void ) const
  {
    LevelState* current_level = this->GetCurrentHeapLevelState();
    return current_level->soundItemID;
  }

  //-------------------------------------------------------------------------------------
  // 現在の階層のサウンドハンドルを取得する
  //-------------------------------------------------------------------------------------
  u32	BGMLevelManager::GetSoundHandleIndexOnCurrentLevel( void ) const
  {
    LevelState* current_level = this->GetCurrentHeapLevelState();
    return current_level->soundHandleIndex;
  }

  // 現在の階層の状態を取得する
  BGMLevelManager::LevelState* BGMLevelManager::GetCurrentHeapLevelState( void ) const
  {
    return this->GetHeapLevelState( m_currentLevel );
  }

  // 指定した階層の状態を取得する
  BGMLevelManager::LevelState* BGMLevelManager::GetHeapLevelState( u32 level ) const
  {
    GFL_ASSERT( level < BGM_PUSH_COUNT_MAX ); //@check iwasawa PushBGMData()で上限を超えないようケアしている為ここには来ない
    return &m_levelState[ level ];
  } 

  //-------------------------------------------------------------------------------------
  /**
   * @brief BGMのサウンドデータを読み込む
   * @param sound_item_id  読み込むサウンドデータのID
   * @retval データをロードした
   * @retval データをロードしなかった( 既に現在の階層でロード済み )
   */
  //-------------------------------------------------------------------------------------
  b32 BGMLevelManager::LoadBGMData( u32 sound_item_id )
  {
    LevelState* current_state = this->GetCurrentHeapLevelState();
    if( current_state->loadFlag ) {
      return false;
    }
    // BGMは全てストリームサウンドなので読み込みは行わない
    // 現階層のサウンドアイテムIDのみを記憶する
    current_state->soundItemID = sound_item_id; 
    return true;
  }

  //-------------------------------------------------------------------------------------
  /**
   * @brief トップ階層にあるBGMのサウンドデータを破棄する
   * @param sound_item_id  破棄するサウンドデータのID
   */
  //-------------------------------------------------------------------------------------
  void BGMLevelManager::UnloadBGMData( u32 sound_item_id )
  {
    LevelState* current_state = this->GetCurrentHeapLevelState();

    // 破棄するデータがない
    if( !current_state->loadFlag ) {
      return;  
    }

    GFL_ASSERT_MSG( //@check iwasawa ケア済み
        current_state->soundItemID == sound_item_id,
        "現在の階層にあるサウンドデータではありません。not 0x%x but 0x%x",
        sound_item_id, current_state->soundItemID );

    if( ( current_state->loadFlag ) &&
        ( current_state->soundItemID == sound_item_id ) ) 
    {
      nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
      nw_sound_heap->LoadState( current_state->soundHeapLevel_beforeLoad );
      current_state->loadFlag = false;
    }
  }

  //-------------------------------------------------------------------------------------
  /**
   * @brief BGM サウンドデータを保存する
   * @param start_info  復元時に適用する開始情報
   * @retval true   保存できた
   * @retval false  保存できなかった
   */
  //-------------------------------------------------------------------------------------
  b32 BGMLevelManager::PushBGMData( 
      const nw::snd::SoundStartable::StartInfo* start_info )
  {
    GFL_ASSERT( m_currentLevel < BGM_PUSH_COUNT_MAX ); //@fix ケア済み
    if( !( m_currentLevel < BGM_PUSH_COUNT_MAX ) ) {
      return false;
    } 

    // 現階層のヒープ状態を保存
    LevelState* current_state = this->GetCurrentHeapLevelState(); 
    if( current_state->loadFlag ) {
      nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
      nw_sound_heap->LoadState( current_state->soundHeapLevel_afterLoadCore );  // 不要なデータを捨てる
    }

    // 現在階層の再生情報を保存
    if( start_info ) {
      m_levelState[ m_currentLevel ].startInfo = *start_info; 
    }

    // 次の階層を初期化
    u32 next_level = m_currentLevel + 1;
    this->ClearHeapLevelState( next_level, current_state->soundHeapLevel_afterLoadCore ); 
    m_currentLevel = next_level;

    return true;
  } 

  //-------------------------------------------------------------------------------------
  /**
   * @brief BGM サウンドデータを復元する
   * @param [out] pop_sound_itemID      復元後の階層にあるサウンドデータのID
   * @param [out] pop_sound_handleID    復元後の階層を操作するハンドル
   * @param [out] pop_sound_start_info  復元後のサウンドの再生情報
   * @return 復元できたか？
   * @retval true   復元できた
   * @retval false  復元できなかった
   */
  //-------------------------------------------------------------------------------------
  b32 BGMLevelManager::PopBGMData( 
      u32* pop_sound_itemID,
      u32* pop_sound_handleID,
      nw::snd::SoundStartable::StartInfo* pop_sound_start_info )
  {
    GFL_ASSERT( 0 < m_currentLevel ); //@check iwasawa ケア済み
    if( m_currentLevel == 0 ) {
      return false;
    }

    // 現階層のヒープ状態を破棄し, 次の階層のヒープ状態を復元
    u32 next_level = m_currentLevel - 1;
    LevelState* next_state = this->GetHeapLevelState( next_level );
    if( next_state->loadFlag ) {
      nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
      nw_sound_heap->LoadState( next_state->soundHeapLevel_afterLoadCore );  // シーケンス、バンクのみ存在する状態に戻す
      m_soundSystem->LoadSoundData( next_state->soundItemID, gfl2::sound::SoundSystem::LOAD_WAVE_ARC ); // 波形を読み直す
    }
    m_currentLevel = next_level;

    // 復元情報を返す
    if( pop_sound_itemID ) { *pop_sound_itemID = next_state->soundItemID; }
    if( pop_sound_handleID ) { *pop_sound_handleID = next_state->soundHandleIndex; }
    if( pop_sound_start_info ) { *pop_sound_start_info = next_state->startInfo; } 
    return true;
  }


} // namespace Sound 

