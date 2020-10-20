//=============================================================================
/**
 * @brief BGM管理クラス
 * @file  BGMManager.h
 * @author obata
 * @date   2011.06.29
 */
//=============================================================================
#ifndef __BGMMANAGER_H__
#define __BGMMANAGER_H__
#pragma once


#include <heap/include/gfl2_Heap.h>
#include <base/include/gfl2_Queue.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "niji_conv_header/sound/SoundDefine.h"
#include "niji_conv_header/sound/SoundMiddleID.h"


#if PM_DEBUG
  // デバッグプリントのスイッチ
  #define BGMMANAGER_DEBUG_PRINT
#endif  //#if PM_DEBUG

namespace Sound {

  class PMSoundSystem;
  class BGMPrepareThread;


  class BGMManager 
  {
    GFL_FORBID_COPY_AND_ASSIGN( BGMManager ); 

    public: 

    //-------------------------------------------------------------------------
    // コンストラクタ・デストラクタ
    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap          作業ワークの確保に使用するヒープ
     * @param sound_system  関連付けるサウンドシステム
     */
    BGMManager( gfl2::heap::HeapBase* heap, Sound::PMSoundSystem* sound_system );

    /**
     * @brief デストラクタ
     */
    ~BGMManager();


    //-------------------------------------------------------------------------
    // 基本動作
    //-------------------------------------------------------------------------
    /**
     * @brief 更新処理
     */
    void Update( void );

    /**
     * @brief BGM 管理情報をクリアする
     */
    void ClearManageState( void );


    //-------------------------------------------------------------------------
    // コマンドの追加
    //-------------------------------------------------------------------------
    static const u32 COMMAND_BLOCK_ID_NONE = 0;
  
    /// コマンドグループ
    static const u32 COMMAND_BLOCK_GROUP_NONE = 0;
    static const u32 COMMAND_BLOCK_GROUP_FIELD = 1;
    static const u32 COMMAND_BLOCK_GROUP_APP = 2;
    static const u32 COMMAND_BLOCK_GROUP_BATTLE = 3;
    static const u32 COMMAND_BLOCK_GROUP_ERROR = 0xFFFFFFFF;  //無効値

    void AddCommand_FADE_IN( u32 fade_in_frame );
    void AddCommand_FADE_OUT( u32 fade_out_frame );
    void AddCommand_FADE_WAIT( void );
    void AddCommand_PAUSE( void );
    void AddCommand_PAUSE_RELEASE( void );
    void AddCommand_PUSH( void );
    void AddCommand_POP( u32 fade_in_frame );
    void AddCommand_PREPARE_START( u32 soundID , u32 startMilliSec = 0 );
    void AddCommand_PREPARE_START_CONTINUE( u32 soundID );
    void AddCommand_PREPARE_WAIT( void );
    void AddCommand_PREPARE_PLAY( void );
    void AddCommand_SKIP( u32 soundID );
    void AddCommand_SKIP_END( void );
    void AddCommand_BLOCK_START(
        u32 block_group = COMMAND_BLOCK_GROUP_NONE,
        u32 blockID = COMMAND_BLOCK_ID_NONE,
        b32 same_skip = true );
    void AddCommand_BLOCK_END( void );


    //-------------------------------------------------------------------------
    // 取得
    //-------------------------------------------------------------------------
    /**
     * @brief 処理が終了していないコマンドを持っているか？
     * @retval true  終了していないコマンドがある
     * @retval false 全てのコマンドが終了した
     */
    b32 HaveCommand( void ) const;

    /**
     * @brief サウンドシステムを取得する
     */
    PMSoundSystem* GetSoundSystem( void ) const;

    u32 GetLastPlayedSoundID( void ) const;   // 全てのコマンドが実行された時点での, 最後に再生されたサウンド
    u32 GetLastPushedSoundID( void ) const;   // 全てのコマンドが実行された時点での, 最後に退避されたサウンド
    u32 GetLastPushedCount( void ) const;     // 全てのコマンドが実行された時点での, 退避中のサウンドの数
    u32 GetLastPreparedSoundID( void ) const; // 全てのコマンドが実行された時点での, 最後にロードされたサウンド



    private:

    enum Command {
      COMMAND_FADE_IN,
      COMMAND_FADE_OUT,
      COMMAND_FADE_WAIT,
      COMMAND_PAUSE,
      COMMAND_PAUSE_RELEASE,
      COMMAND_PUSH,
      COMMAND_POP,
      COMMAND_PREPARE_START,
      COMMAND_PREPARE_WAIT,
      COMMAND_PREPARE_PLAY,
      COMMAND_SKIP,
      COMMAND_SKIP_END,
      COMMAND_BLOCK_START,
      COMMAND_BLOCK_END,
      COMMAND_PREPARE_START_CONTINUE,
      COMMAND_NUM,
      COMMAND_NULL,
    };

    struct CommandData 
    {
      Command command;    // コマンドの種類
      u32 soundID;        // 操作対象のサウンド
      u32 fadeOutFrame;   // フェードアウトフレーム数
      u32 fadeInFrame;    // フェードインフレーム数
      u32 blockGroup;     // コマンドブロックのグループ
      u32 blockID;        // コマンドブロックのID
      u32 startMilliSec;  // 開始ミリ秒
      u8  sameSkip;       // trueなら最後の再生曲と次のリクエスト曲が同一の場合にリクエストをスキップ
      u8  suspend;        // trueならコマンド実行&Dequeue後に一度mainループに処理を返す
      CommandData( void ) : 
        command( COMMAND_NULL ), 
        soundID( SMID_NULL ),
        fadeOutFrame( 0 ),
        fadeInFrame( 0 ),
        blockGroup( 0 ),
        blockID( 0 ),  
        startMilliSec( 0 ),
        sameSkip( true ),
        suspend( false )
      {}
    };

    static const u32 COMMAND_QUEUE_SIZE;

    gfl2::heap::HeapBase* m_heapForWork;  // 作業領域に使用するヒープ
    PMSoundSystem* m_soundSystem;
    BGMPrepareThread* m_prepareThread; 
    gfl2::base::Queue<CommandData> m_commandQueue;
    u32 m_commandSeq;          // コマンドの処理シーケンス
    u32 m_lastPlayedSoundID;   // 全てのコマンドが実行された時点での, 最後に再生されたサウンド
    u32 m_lastPushedSoundID;   // 全てのコマンドが実行された時点での, 最後に退避されたサウンド
    u32 m_lastPushedCount;     // 全てのコマンドが実行された時点での, 退避中のサウンドの数
    u32 m_lastPreparedSoundID; // 全てのコマンドが実行された時点での, 最後にロードされたサウンド
    u32 m_currentPrepareSoundID;  // 現時点で準備中のサウンドID
    u32 m_currentPlayedSoundID;          // 現時点での, 最後に再生されたサウンド
    u32 m_currentExecutedCommandBlockID; // 現時点での, 最後に実行されたコマンドブロックID
    u32 m_pushedSoundID[ BGM_PUSH_COUNT_MAX ]; //PushされたサウンドID

    void FadeInBGM( u32 );
    void FadeOutBGM( u32 );
    void PauseBGM( void );
    void PauseReleaseBGM( void );
    void PushBGM( void );

    void AddCommand( const CommandData& ); 
    b32 SkipCommand( Command );
    b32 SkipCommandInverse( Command );
    b32 DeleteCommandLastBlock( u32 block_group );

    b32 ExecuteCommand( const CommandData&, u32* );
    b32 Command_FADE_IN( const CommandData&, u32* );
    b32 Command_FADE_OUT( const CommandData&, u32* );
    b32 Command_FADE_WAIT( const CommandData&, u32* );
    b32 Command_PAUSE( const CommandData&, u32* );
    b32 Command_PAUSE_RELEASE( const CommandData&, u32* );
    b32 Command_PUSH( const CommandData&, u32* );
    b32 Command_POP( const CommandData&, u32* );
    b32 Command_PREPARE_START( const CommandData&, u32* );
    b32 Command_PREPARE_START_CONTINUE( const CommandData&, u32* );
    b32 Command_PREPARE_WAIT( const CommandData&, u32* );
    b32 Command_PREPARE_PLAY( const CommandData&, u32* );
    b32 Command_SKIP( const CommandData&, u32* );
    b32 Command_SKIP_END( const CommandData&, u32* );
    b32 Command_BLOCK_START( const CommandData&, u32* );
    b32 Command_BLOCK_END( const CommandData&, u32* );

#ifdef BGMMANAGER_DEBUG_PRINT
    private:
    static const char* CommandName[ COMMAND_NUM ];
    public:
    static int DebugPrintBGMLog; // BGM制御ログ出力
#endif
  };

#ifdef BGMMANAGER_DEBUG_PRINT
  #define BGM_PRINT(...) {if(BGMManager::DebugPrintBGMLog)GFL_PRINT(__VA_ARGS__);}
//  #define BGM_PRINT(...) {GFL_PRINT(__VA_ARGS__);}
#else
  #define BGM_PRINT(...) /**/
#endif


} //namespace Sound


#endif // __BGMMANAGER_H__ 
