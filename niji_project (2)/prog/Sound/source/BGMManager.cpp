//=======================================================================================
/**
 * @brief BGM管理クラス
 * @file  BGMManager.h
 * @author obata
 * @date   2011.06.29
 */
//========================================================================================

#include "Sound/include/PMSoundSystem.h"
#include "Sound/include/BGMManager.h"
#include "Sound/include/BGMPrepareThread.h" 


namespace Sound { 


  // コマンドキューの大きさ
  const u32 BGMManager::COMMAND_QUEUE_SIZE = 64; 

#ifdef BGMMANAGER_DEBUG_PRINT
  int BGMManager::DebugPrintBGMLog = 0;
#endif

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap         作業領域として使用するヒープ
   * @param sound_system 関連付けるサウンドシステム
   */
  //---------------------------------------------------------------------------
  BGMManager::BGMManager( 
      gfl2::heap::HeapBase* heap, 
      Sound::PMSoundSystem* sound_system ) : 
    m_heapForWork( heap ),
    m_soundSystem( sound_system ),
    m_prepareThread( NULL ),
    m_commandQueue( heap, COMMAND_QUEUE_SIZE ),
    m_commandSeq( 0 ),
    m_lastPlayedSoundID( SMID_NULL ),
    m_lastPushedSoundID( SMID_NULL ),
    m_lastPushedCount( 0 ),
    m_lastPreparedSoundID( SMID_NULL ),
    m_currentPrepareSoundID( SMID_NULL ),
    m_currentPlayedSoundID( SMID_NULL ),
    m_currentExecutedCommandBlockID( SMID_NULL )
  {
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  BGMManager::~BGMManager()
  {
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief BGM 管理情報をクリアする
   */
  //---------------------------------------------------------------------------
  void BGMManager::ClearManageState( void )
  {
    m_commandQueue.Clear();
    m_lastPlayedSoundID = SMID_NULL;
    m_lastPushedSoundID = SMID_NULL;
    m_lastPushedCount = 0;
    m_lastPreparedSoundID = SMID_NULL;
    m_currentPlayedSoundID = SMID_NULL;
    m_currentExecutedCommandBlockID = SMID_NULL;

    for(int i = 0;i < BGM_PUSH_COUNT_MAX; ++i ){
      m_pushedSoundID[i] = SMID_NULL;
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief サウンドシステムを取得する
   */
  //---------------------------------------------------------------------------
  PMSoundSystem* BGMManager::GetSoundSystem( void ) const
  {
    return m_soundSystem;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 全てのコマンドが実行された時点での, 最後に再生されたサウンド
   */
  //---------------------------------------------------------------------------
  u32 BGMManager::GetLastPlayedSoundID( void ) const
  {
    return m_lastPlayedSoundID;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 全てのコマンドが実行された時点での, 退避中のサウンド数
   */
  //---------------------------------------------------------------------------
  u32 BGMManager::GetLastPushedCount( void ) const
  {
    return m_lastPushedCount;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 全てのコマンドが実行された時点での, 最後に退避されたサウンド
   */
  //---------------------------------------------------------------------------
  u32 BGMManager::GetLastPushedSoundID( void ) const
  {
    return m_lastPushedSoundID;
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief 全てのコマンドが実行された時点での, 最後にロードされたサウンド
   */
  //---------------------------------------------------------------------------
  u32 BGMManager::GetLastPreparedSoundID( void ) const
  {
    return m_lastPreparedSoundID;
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief 処理が終了していないコマンドを持っているか？
   * @retval true  終了していないコマンドがある
   * @retval false 全てのコマンドが終了した
   */
  //---------------------------------------------------------------------------
  b32 BGMManager::HaveCommand( void ) const
  {
    return !m_commandQueue.IsEmpty();
  }

  /**
   * @brief コマンドをコマンドキューに追加する
   * @param command 追加するコマンドデータ
   */
  void BGMManager::AddCommand( const CommandData& command )
  {
    if( m_commandQueue.IsFull() ) 
    {
      GFL_PRINT( "WARNING! Soundコマンドキューが一杯です。 岩澤までご報告ください。\n" );
      if( command.command == COMMAND_BLOCK_END ){
        //Endを検出したら既に積んでしまった分を削除。それまではスルー
        GFL_ASSERT(0);
        this->DeleteCommandLastBlock( COMMAND_BLOCK_ID_NONE );
      }
#if 0
      while( m_commandQueue.IsFull() ) {
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
        m_soundSystem->Update();
        this->Update();
      }
#endif
      return;
    }
    m_commandQueue.Enqueue( command );
  }
  
  /**
   * @brief 指定したコマンドが現れるまで, コマンドをデキューする
   * @param end_command スキップ終了コマンド
   * @retval true  end_command を発見した
   * @retval false end_command が見当たらず, キューが空になった
   */
  b32 BGMManager::SkipCommand( Command end_command )
  {
    while( !m_commandQueue.IsEmpty() )
    {
      if( m_commandQueue.GetData(0).command == end_command ) 
      {
        return true;
      }
      else
      {
        m_commandQueue.Dequeue();
      }
    }
    return false;
  }
  /**
   * @brief 指定したコマンドが現れるまで, コマンドを末尾からデキューする
   * @param end_command スキップ終了コマンド
   * @retval true  end_command を発見した
   * @retval false end_command が見当たらず, キューが空になった
   */
  b32 BGMManager::SkipCommandInverse( Command end_command )
  {
    while( !m_commandQueue.IsEmpty() )
    {
      if( m_commandQueue.GetLastData().command == end_command ){
        m_commandQueue.DequeueInverse();
        return true;
      }
      m_commandQueue.DequeueInverse();
    }
    return false;
  }
  
  /**
   * @brief  末尾のブロックグループのコマンドをデキューする
   * @retval true  end_command を発見した
   * @retval false end_command が見当たらず, キューが空になった
   *
   * @note  既に消化され始めているブロックには手を付けない。あくまでBLOCK_START-BLOCK_END単位で削除
   */
  b32 BGMManager::DeleteCommandLastBlock( u32 block_group )
  {
    int i;
    u32 group = COMMAND_BLOCK_GROUP_ERROR;
    u32 command_num = m_commandQueue.GetDataNum();
    
    if( command_num == 0 ) return false;  //キューが空なので何もしない

    //末尾から順にBLOCK_STARTを探す
    for( i = command_num-1 ; i>=0; i-- )
    {
      const CommandData& check_command = m_commandQueue.GetData( i );
      if( check_command.command == COMMAND_BLOCK_START ) {
        group = check_command.blockGroup;
        break;
      }
    }
    if( group == COMMAND_BLOCK_GROUP_ERROR ){
      return false; //BLOCK_STARTが見つからなかった
    }
    if( block_group != COMMAND_BLOCK_GROUP_NONE &&
        block_group != group ){
      return false; //捜している種類のグループではなかった
    }
    //ブロックごと削除
    BGM_PRINT( "->BGMComBlockDel: g=%d l=0x%08x,0x%08x", block_group, m_lastPlayedSoundID, m_lastPushedSoundID );
    this->SkipCommandInverse( COMMAND_BLOCK_START );
    return true;
  }


  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: フェードイン
   * @param fade_in_frame フェードインフレーム数
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_FADE_IN( u32 fade_in_frame )
  {
    CommandData com_data;
    com_data.command = COMMAND_FADE_IN;
    com_data.fadeInFrame = fade_in_frame;
    this->AddCommand( com_data );
  }

  /**
   * @brief コマンド処理: フェードイン
   * @param command_data 処理対象のコマンドデータ
   * @param command_seq  コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_FADE_IN( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_FADE_IN );
    this->FadeInBGM( command_data.fadeInFrame );
    return true;
  }

  /**
   * @brief BGMのフェードインを開始する
   * @param fade_in_frame フェードインフレーム数
   */
  void BGMManager::FadeInBGM( u32 fade_in_frame )
  {
    m_soundSystem->PauseReleaseBGM( fade_in_frame ); 
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: フェードアウト
   * @param fade_out_frame フェードアウトフレーム数
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_FADE_OUT( u32 fade_out_frame )
  {
    CommandData com_data;
    com_data.command = COMMAND_FADE_OUT;
    com_data.fadeOutFrame = fade_out_frame;
    this->AddCommand( com_data );
  }

  /**
   * @brief コマンド処理: フェードアウト
   * @param command_data 処理対象のコマンドデータ
   * @param command_seq  コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_FADE_OUT( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_FADE_OUT );
    this->FadeOutBGM( command_data.fadeOutFrame );
    return true;
  }

  /**
   * @brief BGMのフェードアウトを開始する
   * @param fade_out_frame フェードアウトフレーム数
   */
  void BGMManager::FadeOutBGM( u32 fade_out_frame )
  {
    if( m_currentPrepareSoundID != SMID_NULL ){
      //最後に準備したBGMはスキップされた.Prepaed状態でPauseすると永遠にフェードが終わらない現象対策
      GFL_PRINT("FadeOutBGM Skip. LastPrepareSound is not play. last=%d, current=%d\n",m_currentPrepareSoundID, m_lastPlayedSoundID );
      return;    
    }
    m_soundSystem->PauseBGM( fade_out_frame ); 
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: フェード終了待ち
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_FADE_WAIT( void )
  {
    CommandData com_data;
    com_data.command = COMMAND_FADE_WAIT;
    this->AddCommand( com_data );
  }

  /**
   * @brief コマンド処理: フェード終了待ち
   * @param command_data 処理対象のコマンドデータ
   * @param command_seq  コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_FADE_WAIT( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_FADE_WAIT );
    return ( m_soundSystem->IsBGMFade() == false );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: ポーズ
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_PAUSE( void )
  {
    CommandData com_data;
    com_data.command = COMMAND_PAUSE;
    com_data.suspend = true;  //実行後にライブラリmainを通して内部ステータスを反映させるため一度処理を返す
    this->AddCommand( com_data );
  }

  /**
   * @brief コマンド処理: ポーズ
   * @param command_data  処理対象のコマンドデータ
   * @param command_seq   コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_PAUSE( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_PAUSE );
    this->PauseBGM();
    return true;
  }

  /**
   * @brief BGMを一時停止する
   */
  void BGMManager::PauseBGM( void )
  {
    m_soundSystem->PauseBGM(0);
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: ポーズ解除
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_PAUSE_RELEASE( void )
  {
    CommandData com_data;
    com_data.command = COMMAND_PAUSE_RELEASE;
    this->AddCommand( com_data );
  }

  /**
   * @brief コマンド処理: ポーズ解除
   * @param command_data 処理対象のコマンドデータ
   * @param command_seq  コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_PAUSE_RELEASE( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_PAUSE_RELEASE );
    this->PauseReleaseBGM();
    return true;
  }

  /**
   * @brief BGMの一時停止を解除する
   */
  void BGMManager::PauseReleaseBGM( void )
  {
    m_soundSystem->PauseReleaseBGM(0);
  }


  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: サウンド退避
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_PUSH( void )
  {
    CommandData com_data;

    if( m_lastPushedCount >= (BGM_PUSH_COUNT_MAX-1) ){
      GFL_PRINT( "WARNING!! BGM 退避数オーバー\n" );
      GFL_ASSERT(0); // 退避数オーバー
      return;
    }
    com_data.command = COMMAND_PUSH;
    this->AddCommand( com_data );
   
    m_pushedSoundID[++m_lastPushedCount] = m_lastPlayedSoundID;
    m_lastPushedSoundID = m_lastPlayedSoundID;
    m_lastPlayedSoundID = SMID_NULL;
  }

  /**
   * @brief コマンド処理: サウンド退避
   * @param command_data 処理対象のコマンドデータ
   * @param command_seq  コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_PUSH( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_PUSH );
    this->PushBGM();
    m_currentPlayedSoundID = SMID_NULL;
    return true;
  }

  /**
   * @brief BGMを退避する
   */
  void BGMManager::PushBGM( void )
  {
    if( m_soundSystem->GetMaxPushBGMCount() <= m_soundSystem->GetPushedBGMCount() ) {
      GFL_PRINT( "WARNING!! BGM 退避数オーバー\n" );
      GFL_ASSERT(0); // 退避数オーバー
      return;
    }
    if( m_soundSystem->IsBGMValid() ){
      /*
       * BGM完全停止状態で呼ぶとバグるので、有効なBGMがある時のみ処理
       * PushをスキップしてPopBGM()が呼ばれても内部でスキップされ直接の副作用はないが
       * PushPopのスタック状態によっては、段数の整合性が崩れるケースがあるので原則NG
       */
      m_soundSystem->PushBGM();
    }else{
      GFL_PRINT("WARNING!! 退避する有効なBGMがない。BGM完全停止状態でPush処理は呼べません\n");
      #if PM_DEBUG
      if( m_soundSystem->IsBGMControlEnable() ){
        GFL_ASSERT( 0 );	
      }
      #endif
    }
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: サウンド復帰
   * @param fade_in_frame フェードインフレーム数
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_POP( u32 fade_in_frame )
  {
    if( 0 == m_lastPushedCount ){
      GFL_ASSERT( 0 );
      return;
    }

    CommandData com_data;
    com_data.command = COMMAND_POP;
    com_data.soundID = m_lastPushedSoundID;
    com_data.fadeInFrame = fade_in_frame;
    this->AddCommand( com_data );
    
    m_lastPlayedSoundID = m_lastPushedSoundID;
    m_pushedSoundID[m_lastPushedCount] = SMID_NULL;
    m_lastPushedSoundID = m_pushedSoundID[--m_lastPushedCount];
  }

  /**
   * @brief コマンド処理: サウンド復帰
   * @param command_data 処理対象のコマンドデータ
   * @param command_seq  コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_POP( const CommandData& command_data, u32* command_seq )
  {
    GFL_ASSERT( command_data.command == COMMAND_POP );

    switch( *command_seq )
    {
    case 0:
      if( m_soundSystem->GetPushedBGMCount() <= 0 ) 
      {
        GFL_PRINT( "WARNING!! 復帰すべき BGM がありません。\n" );
        #if PM_DEBUG
        if( m_soundSystem->IsBGMControlEnable() ){
          GFL_ASSERT(0);
        }
        #endif
        return true;
      } 
      else
      {
        m_soundSystem->StopBGM( 0 );
				if(command_data.fadeInFrame){
					m_soundSystem->PopBGM( command_data.fadeInFrame );
				} else {
          GFL_PRINT("WARNING!! POP_BGM_NOT_PLAY\n");
					m_soundSystem->PopBGMOnly();
				}
        m_currentPlayedSoundID = command_data.soundID;
        ( *command_seq )++;
      }
      return false;

    case 1:
      if( m_soundSystem->IsBGMFade() == false )
      {
        return true;
      }
      return false;

    default:
      GFL_ASSERT(0);
      return true;
    }
  }
 
  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: 非同期読み込み開始
   * @param soundID 読み込むサウンド
   * @param startMilliSec  再生開始位置(ミリ秒)
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_PREPARE_START( u32 soundID , u32 startMilliSec )
  {
    CommandData com_data;
    com_data.command = COMMAND_PREPARE_START;
    com_data.soundID = soundID;
    com_data.startMilliSec = startMilliSec;
    this->AddCommand( com_data );

    m_lastPreparedSoundID = soundID;
  }

  /**
   * @brief コマンド処理: 非同期読み込み開始
   * @param command_data 処理対象のコマンドデータ
   * @param command_seq  コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_PREPARE_START( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_PREPARE_START );
    m_soundSystem->PrepareBGM( command_data.soundID , command_data.startMilliSec );
    m_currentPrepareSoundID = command_data.soundID;
    return true;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: 非同期読み込み終了待ち
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_PREPARE_WAIT( void )
  {
    CommandData com_data;
    com_data.command = COMMAND_PREPARE_WAIT;
    this->AddCommand( com_data );
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: 続き再生 非同期読み込み開始
   * @param soundID 読み込むサウンド
   *
   * Sangoイントロデモ専用 再生位置を引きついで、次のBGMに切り替えるプリペア処理を開始
   * 機能的には前BGMの最後の再生サンプル数を保存し、そのサンプル位置から次のBGMを再生する
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_PREPARE_START_CONTINUE( u32 soundID )
  {
    CommandData com_data;
    com_data.command = COMMAND_PREPARE_START_CONTINUE;
    com_data.soundID = soundID;
    this->AddCommand( com_data );

    m_lastPreparedSoundID = soundID;
  }

  /**
   * @brief コマンド処理:続き再生非同期読み込み開始
   * @param command_data 処理対象のコマンドデータ
   * @param command_seq  コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   *
   * Sangoイントロデモ専用 再生位置を引きついで、次のBGMに切り替えるプリペア処理を開始
   * 機能的には前BGMの最後の再生サンプル数を保存し、そのサンプル位置から次のBGMを再生する
   */
  b32 BGMManager::Command_PREPARE_START_CONTINUE( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_PREPARE_START_CONTINUE );
    
    m_soundSystem->PrepareBGMContinue( command_data.soundID );
    return true;
  }


  /**
   * @brief コマンド処理: 非同期読み込み終了待ち
   * @param command_data 処理対象のコマンドデータ
   * @param command_seq  コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_PREPARE_WAIT( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_PREPARE_WAIT );
    return m_soundSystem->IsBGMPrepared();
  }
  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: 読み込んだサウンドの再生
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_PREPARE_PLAY( void )
  {
    CommandData com_data;
    com_data.command = COMMAND_PREPARE_PLAY;
    com_data.soundID = m_lastPreparedSoundID;
    this->AddCommand( com_data );

    m_lastPlayedSoundID = m_lastPreparedSoundID;
    m_lastPreparedSoundID = SMID_NULL;
  }

  /**
   * @brief コマンド処理: 読み込んだサウンドの再生
   * @param command_data 処理対象のコマンドデータ
   * @param command_seq  コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_PREPARE_PLAY( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_PREPARE_PLAY );
    m_soundSystem->PlayPreparedBGM( 0 );
    m_currentPlayedSoundID = command_data.soundID;
    m_currentPrepareSoundID = SMID_NULL;

    return true;
  }
  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: コマンドスキップ
   * @param soundID スキップ条件
   *
   * @note
   * 全てのコマンドが実行された時点で再生中のサウンドが soundID でない場合,
   * ( このコマンドが実行されるまでに, 異なるサウンドの再生コマンドが積まれた場合など )
   * 次の SKIP_END コマンドまでのコマンドを実行せずに破棄する.
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_SKIP( u32 soundID )
  {
    CommandData com_data;
    com_data.command = COMMAND_SKIP;
    com_data.soundID = soundID;
    this->AddCommand( com_data );
  }

  /**
   * @brief コマンド処理: コマンドスキップ
   * @param command_data  処理対象のコマンドデータ
   * @param command_seq  コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_SKIP( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_SKIP );

    // 最終的に再生されるべきサウンドであればスキップしない
    if( ( command_data.soundID == m_lastPlayedSoundID ) ||
        ( command_data.soundID == m_lastPushedSoundID ) ) 
    {
      return true;
    }
    BGM_PRINT( "BGMComSkip: 0x%08x l=0x%08x,0x%08x\n", command_data.soundID, m_lastPlayedSoundID, m_lastPushedSoundID );
    
    b32 skip_result = this->SkipCommand( COMMAND_SKIP_END );
    if( !skip_result ) {
      GFL_PRINT( "SKIP コマンドに対応する SKIP_END コマンドがありません。\n" );
      GFL_ASSERT(0);
    }
    return true;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: コマンドスキップ終了
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_SKIP_END( void )
  {
    CommandData com_data;
    com_data.command = COMMAND_SKIP_END;
    this->AddCommand( com_data );
  }

  /**
   * @brief コマンド処理: コマンドスキップ終了
   * @param command_data  処理対象のコマンドデータ
   * @param command_seq   コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_SKIP_END( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_SKIP_END );
    return true;
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: コマンドブロック開始
   * @param block_group コマンドブロックのグループ番号
   * @param blockID     コマンドブロックのID
   *
   * @note 
   * BLOCK_START, BLOCK_END コマンドで囲まれた一連のコマンドを
   * 一つのコマンドブロックとして扱う.
   * 各コマンドブロックは, ブロックグループとブロックIDを持つ.
   *
   * 自分より前に、同じブロックグループのコマンドブロックが存在した場合
   * 自分より前の同一グループのコマンドをブロック単位で削除してから自分を積む
   *
   * 最後に実行されたコマンドブロックIDが自身と同じなら,
   * 後続のコマンドブロックは, 先着のコマンドブロックと同じ処理を行うことから,
   * 自身のコマンド処理を実行せずに破棄する.
   *
   * 自身のブロックの直後が同じブロックグループだった場合,
   * 先着のコマンドブロック処理は, 後続のコマンドブロックにより上書きされるため
   * 自身のコマンド処理を実行せずに破棄する.
   *
   * ゾーンチェンジ, 自転車など, プレイヤの操作により
   * 無限にコマンドが積まれてしまう可能性がある操作については
   * 各操作を同じコマンドブロックグループとして登録することにより,
   * コマンドキューのオーバーフローを防ぐ.
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_BLOCK_START( u32 block_group, u32 blockID, b32 same_skip )
  {
    CommandData com_data;

    //既存のキューの中身をチェックして、直前が同一グループなら後勝ちで前を削除してしまう
    if( blockID != COMMAND_BLOCK_ID_NONE ){
      this->DeleteCommandLastBlock( block_group );
    }
    com_data.command = COMMAND_BLOCK_START;
    com_data.blockGroup = block_group;
    com_data.blockID = blockID;
    com_data.sameSkip = same_skip;
    this->AddCommand( com_data );
  }

  /**
   * @brief コマンド処理: コマンドブロックの開始
   * @param command_data  処理対象のコマンドデータ
   * @param command_seq   コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_BLOCK_START( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_BLOCK_START );

    u32 command_num = m_commandQueue.GetDataNum();

    // 自身のブロック終了位置を探す
    u32 block_end_command_index = 0;
    for( u32 i=0; i<command_num; i++ )
    {
      if( m_commandQueue.GetData(i).command == COMMAND_BLOCK_END ) {
        block_end_command_index = i;
        break;
      }
    }

    if( block_end_command_index == 0 ) {
      GFL_PRINT( "BLOCK_START コマンドに対応する BLOCK_END コマンドがありません。\n" );
      return true;
    }

    // 最後に実行されたコマンドブロックIDが自身と同じコマンドブロックIDなら,
    // 自身のブロックをスキップする
    if( command_data.sameSkip ){
      if( ( command_data.blockID != COMMAND_BLOCK_ID_NONE ) && 
          ( command_data.blockID == m_currentExecutedCommandBlockID ) )
      {  
        BGM_PRINT( "BGMComSameSkipB: 0x%08x\n", command_data.blockID );
        this->SkipCommand( COMMAND_BLOCK_END );
        return true;
      }
    }
    // 自身のブロックの直後に同じブロックグループのブロックがあるなら, 
    // 自身のブロックをスキップする
    if( command_data.blockGroup != COMMAND_BLOCK_GROUP_NONE )
    {
      u32 check_command_index = block_end_command_index + 1;
      if( check_command_index < command_num )
      {
        const CommandData& check_command = m_commandQueue.GetData( check_command_index );
        if( ( check_command.command == COMMAND_BLOCK_START ) &&
            ( check_command.blockGroup == command_data.blockGroup ) )
        {
          BGM_PRINT( "BGMComSameSkipA: s=0x%08x a=0x%08x, g=%d\n", command_data.blockID, check_command.blockID, check_command.blockGroup );
          this->SkipCommand( COMMAND_BLOCK_END );
          return true;
        }
      }
    }

    m_currentExecutedCommandBlockID = command_data.blockID;
    return true;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief コマンド追加: コマンドブロック終了
   */
  //---------------------------------------------------------------------------
  void BGMManager::AddCommand_BLOCK_END( void )
  {
    CommandData com_data;
    com_data.command = COMMAND_BLOCK_END;
    this->AddCommand( com_data );
  }

  /**
   * @brief コマンド処理: コマンドブロックの終了
   * @param command_data  処理対象のコマンドデータ
   * @param command_seq   コマンドの処理シーケンス
   * @retval true  コマンド処理終了
   * @retval false コマンド処理継続
   */
  b32 BGMManager::Command_BLOCK_END( const CommandData& command_data, u32* command_seq )
  {
    GFL_UNUSED(command_seq);
    GFL_ASSERT( command_data.command == COMMAND_BLOCK_END );
    return true;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 更新処理
   */
  //---------------------------------------------------------------------------
  void BGMManager::Update( void )
  {
    while( !m_commandQueue.IsEmpty() )
    {
      const CommandData com_data = m_commandQueue.GetData(0);
      b32 com_end = this->ExecuteCommand( com_data, &m_commandSeq ); 
      if( com_end ) 
      {
        b32 suspend = com_data.suspend; 
        m_commandQueue.Dequeue();
        m_commandSeq = 0;

        if( suspend ) break; //次のフレームに処理を持ち越す
      }
      else 
      {
        break;  // コマンド処理が継続するなら, 次のフレームに持ち越す
      }
    }
  }


#ifdef BGMMANAGER_DEBUG_PRINT
  const char* BGMManager::CommandName[ COMMAND_NUM ] = 
  {
    "FADE_IN",
    "FADE_OUT",
    "FADE_WAIT",
    "PAUSE",
    "PAUSE_RELEASE",
    "PUSH",
    "POP",
    "PREPARE_START",
    "PREPARE_WAIT",
    "PREPARE_PLAY",
    "SKIP",
    "SKIP_END",
    "BLOCK_START",
    "BLOCK_END",
  };
#endif 

  //---------------------------------------------------------------------------
  /**
   * @brief コマンドを実行する
   * @param com_data 実行するコマンドデータ
   * @param com_seq  コマンドの処理シーケンス
   */
  //---------------------------------------------------------------------------
  b32 BGMManager::ExecuteCommand( const CommandData& com_data, u32* com_seq )
  {
    static b32 ( BGMManager::*COMMAND_TABLE[ COMMAND_NUM ] )( const CommandData&, u32* ) =
    {
      &BGMManager::Command_FADE_IN,
      &BGMManager::Command_FADE_OUT,
      &BGMManager::Command_FADE_WAIT,
      &BGMManager::Command_PAUSE,
      &BGMManager::Command_PAUSE_RELEASE,
      &BGMManager::Command_PUSH,
      &BGMManager::Command_POP,
      &BGMManager::Command_PREPARE_START,
      &BGMManager::Command_PREPARE_WAIT,
      &BGMManager::Command_PREPARE_PLAY,
      &BGMManager::Command_SKIP,
      &BGMManager::Command_SKIP_END,
      &BGMManager::Command_BLOCK_START,
      &BGMManager::Command_BLOCK_END,
      &BGMManager::Command_PREPARE_START_CONTINUE,
    };

#ifdef BGMMANAGER_DEBUG_PRINT
    if( (*com_seq) == 0 ){
      BGM_PRINT( "BGMCom: %s c(0x%06x) l(0x%06x,0x%06x,%d) lo(0x%06x,0x%06x,0x%06x,0x%06x), g(%d,0x%06x,%d, s=0x%06x) f(%d,%d)\n",
        CommandName[ com_data.command ], m_currentPlayedSoundID,
        m_lastPlayedSoundID, m_lastPushedSoundID, m_lastPushedCount,
        m_pushedSoundID[0], m_pushedSoundID[1], m_pushedSoundID[2], m_pushedSoundID[3],
        com_data.blockGroup, com_data.blockID, com_data.sameSkip, com_data.soundID ,
        com_data.fadeInFrame, com_data.fadeOutFrame
        );
    }
#endif
    return ( this->*COMMAND_TABLE[ com_data.command ] )( com_data, com_seq );
  } 


} // namespace xy_snd
