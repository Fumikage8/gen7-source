//=======================================================================================
/**
 * @file SELevelManager.cpp
 * @brief サウンド階層管理クラス
 * @author obata_toshihiro
 * @date 2011.06.27
 */
//=======================================================================================

#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>
#include <thread/include/gfl2_Thread.h>
#include <sound/include/gfl2_SoundHeap.h>
#include <sound/include/gfl2_SoundSystem.h>

#include "Sound/include/SELevelManager.h"

namespace Sound { 


  //-------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap          インスタンス生成に使用するヒープ
   * @param sound_system  関連付けるサウンドシステム
   * @param sound_heap    管理対象のサウンドヒープ
   */
  //-------------------------------------------------------------------------------------
  SELevelManager::SELevelManager( 
      gfl2::heap::HeapBase* heap, 
      gfl2::sound::SoundSystem* sound_system, 
      gfl2::sound::SoundHeap* sound_heap ) :
    m_soundSystem( sound_system ),
    m_soundHeap( sound_heap ),
    m_levelState( NULL ),
    m_currentLevel( 0 ),
    m_async_load_sound_item_id(ASYNC_LOAD_NONE_ID)
  { 
    nw::snd::SoundHeap* nw_sound_heap = sound_heap->GetNwSoundHeap();
    s32 initial_heap_level = nw_sound_heap->GetCurrentLevel();

    m_levelState = GFL_NEW( heap ) LevelState[ MAX_LEVEL ]; 

    for( u32 i=0; i<MAX_LEVEL; i++ ) {
      this->InitHeapLevelState( i, initial_heap_level );
    }
  }

  /**
   * @brief 階層の状態を初期化する
   * @param level               初期化する階層
   * @param initial_heap_level  ヒープレベルの初期値
   * @param sound_handle_index  階層に関連付けるサウンドハンドル
   */
  void SELevelManager::InitHeapLevelState(
      u32 level,
      s32 initial_heap_level )
  {
    LevelState* level_state = this->GetHeapLevelState( level );
    level_state->loadFlag = false;
    level_state->soundItemID = 0;
    level_state->soundHeapLevel_beforeLoad = initial_heap_level;
    level_state->soundHeapLevel_afterLoadCore = initial_heap_level;
  } 

  //-------------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-------------------------------------------------------------------------------------
  SELevelManager::~SELevelManager()
  {
    GFL_DELETE_ARRAY m_levelState;
  }

  // 現在の階層の状態を取得する
  SELevelManager::LevelState* SELevelManager::GetCurrentHeapLevelState( void ) const
  {
    return this->GetHeapLevelState( m_currentLevel );
  }

  // 指定した階層の状態を取得する
  SELevelManager::LevelState* SELevelManager::GetHeapLevelState( u32 level ) const
  {
    GFL_ASSERT( level < MAX_LEVEL );  //@check iwasawa 130329 PushSEData()で上限を越えたらASSERT_STOPにしたので、このアサートには来ない
                                      //PushSEData()のASSERT_STOPをASSERTに変える場合、ケアを実装する必要がある
    return &m_levelState[ level ];
  } 

  //-------------------------------------------------------------------------------------
  /**
   * @brief SEのサウンドデータを読み込む
   * @param sound_item_id  読み込むサウンドデータのID
   * @retval データをロードした
   * @retval データをロードしなかった( 既に現在の階層でロード済み )
   */
  //-------------------------------------------------------------------------------------
  b32 SELevelManager::LoadSEData( u32 sound_item_id )
  {
    LevelState* current_state = this->GetCurrentHeapLevelState();
    if( current_state->loadFlag ) {
      return false;
    }

    // 関連する全てのデータをロード
    b32 load_result = m_soundSystem->LoadSoundData(
        m_soundHeap,
        sound_item_id, 
        gfl2::sound::SoundSystem::LOAD_ALL );

    // 読み込んだ後のヒープレベルを記憶
    nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
    current_state->soundHeapLevel_afterLoadCore = nw_sound_heap->SaveState();  

    // 読み込んだ結果を記憶
    current_state->loadFlag = load_result;
    current_state->soundItemID = sound_item_id;
    return current_state->loadFlag;
  }

  //-------------------------------------------------------------------------------------
  /**
   * @brief トップ階層にあるSEのサウンドデータを破棄する
   * @param sound_item_id  破棄するサウンドデータのID
   */
  //-------------------------------------------------------------------------------------
  void SELevelManager::UnloadSEData( u32 sound_item_id )
  {
    LevelState* current_state = this->GetCurrentHeapLevelState();

    if( !current_state->loadFlag ) {
      return;  // 破棄するデータがない
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
   * @brief SE サウンドデータを保存する
   * @retval true   保存できた
   * @retval false  保存できなかった
   */
  //-------------------------------------------------------------------------------------
  b32 SELevelManager::PushSEData( void )
  {
    u32 next_level = m_currentLevel + 1;  //次の階層

    if( next_level >= MAX_LEVEL ) {
      GFL_ASSERT_STOP( 0 ); //@fix iwasawa 130329 ケアは諦めて上限を超えて積もうとしたらストップ

      //次の階層はないので、初期化はスキップする(↑ストップせずにケアするにはGetHeapLevelState()でNULLを返し、NULLをケアする必要がある)
      m_currentLevel = next_level;
      return false;
    } 

    // 次の階層を初期化
    LevelState* current_state = this->GetCurrentHeapLevelState(); 
    this->InitHeapLevelState( next_level, current_state->soundHeapLevel_afterLoadCore ); 
    m_currentLevel = next_level;

    return true;
  } 

  //-------------------------------------------------------------------------------------
  /**
   * @brief SE サウンドデータを復元する
   * @param [out] pop_sound_itemID 復元後の階層にあるサウンドデータのID
   * @return 復元できたか？
   * @retval true   復元できた
   * @retval false  復元できなかった
   */
  //-------------------------------------------------------------------------------------
  b32 SELevelManager::PopSEData( u32* pop_sound_itemID )
  {
    if( m_currentLevel == 0 ) {
      GFL_ASSERT_MSG( 0, "Popするデータが無い\n" );  //@check iwasawa ケア済みなのでこのまま
      return false;
    }

    // 現階層のヒープ状態を破棄し, 次の階層のヒープ状態を復元
    u32 next_level = m_currentLevel - 1;
    LevelState* next_state = this->GetHeapLevelState( next_level );
    nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
    nw_sound_heap->LoadState( next_state->soundHeapLevel_afterLoadCore );  // サウンドデータを読み込んだ状態に戻す
    m_currentLevel = next_level;

    // 復元情報を返す
    if( pop_sound_itemID ) { *pop_sound_itemID = next_state->soundItemID; }
    return true;
  }

  //-------------------------------------------------------------------------------------
  /**
   * @brief 現在の階層レベルを取得する
   */
  //-------------------------------------------------------------------------------------
  u32 SELevelManager::GetCurrentLevel( void ) const
  {
    return m_currentLevel;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  SEサウンドデータを読み込む
   *
   * @param thread_heap     スレッドヒープ
   * @param sound_item_id   読み込むサウンドデータのID
   *
   * @retval true   読み込み開始
   * @retval false  読み込み開始失敗　（前回の読み込みがまだ続いている）
   */
  //-----------------------------------------------------------------------------
  b32 SELevelManager::LoadSEDataAsync( gfl2::heap::HeapBase * thread_heap, u32 sound_item_id )      // 
  {
    LevelState* current_state = this->GetCurrentHeapLevelState();
    if( current_state->loadFlag ) {
      return true;
    }

    // 関連する全てのデータをロード
    b32 load_result = m_soundSystem->LoadSoundDataAsync(
        m_soundHeap,
        thread_heap,
        gfl2::thread::Thread::THREAD_PRI_NORMAL - 1,
        sound_item_id, 
        gfl2::sound::SoundSystem::LOAD_ALL );

    if( load_result ){
      m_async_load_sound_item_id = sound_item_id;
    }

    return load_result;
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  非同期読み込み待ち
   *
   *	@retval true  読み込み完了
   *	@retval false 読み込み途中
   */
  //-----------------------------------------------------------------------------
  SELevelManager::Result SELevelManager::WaitLoadSeDataAsync(void)                 // SEサウンドデータ読み込み待ち
  {
    LevelState* current_state = this->GetCurrentHeapLevelState();

    if( m_async_load_sound_item_id == ASYNC_LOAD_NONE_ID ){
      return LOADED;
    }
    
    if( current_state->loadFlag ) {
      return LOADED;
    }
    if( m_soundSystem->WaitLoadSoundDataAsync() ){
      // 読み込んだ後のヒープレベルを記憶
      nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
      current_state->soundHeapLevel_afterLoadCore = nw_sound_heap->SaveState();  

      GFL_ASSERT_STOP( m_soundSystem->GetLoadSoundDataAsyncResult() ); //@check iwasawa 130329 ケアを諦めている場所なのでこのまま
      // 読み込んだ結果を記憶
      current_state->loadFlag = true;
      current_state->soundItemID = m_async_load_sound_item_id;
      m_async_load_sound_item_id = ASYNC_LOAD_NONE_ID;
      return LOAD_FINISH;
    }

    return LOADING;
  }


} // namespace Sound
