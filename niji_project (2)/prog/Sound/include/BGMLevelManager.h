//=======================================================================================
/**
 * @file BGMLevelManager.h
 * @brief BGM階層管理クラス
 * @author obata_toshihiro
 * @date 2012.02.25
 *
 * @note 
 * サウンドハンドル, サウンドデータを階層構造で管理する.
 */
//=======================================================================================
#ifndef __BGMLEVELMANAGER_H__
#define __BGMLEVELMANAGER_H__
#pragma once

#include <sound/include/gfl2_SoundSystem.h>
#include <sound/include/gfl2_SoundHeap.h>
#include <sound/include/gfl2_SoundDataManagerThread.h>

#include "niji_conv_header/sound/SoundDefine.h"

namespace Sound {


  class BGMLevelManager
  {
    GFL_FORBID_COPY_AND_ASSIGN( BGMLevelManager );


    public:

    //-------------------------------------------------------------------------
    // コンストラクタ・デストラクタ・セットアップ
    //-------------------------------------------------------------------------
    /**
     * @brief 動作設定
     */
    struct Settings {
      u32 soundHandle[ BGM_PUSH_COUNT_MAX ]; // 各階層に割り当てるサウンドハンドル
    }; 

    /**
     * @brief コンストラクタ
     * @param heap          インスタンスの生成に使用するヒープ
     * @param sound_system  関連付けるサウンドシステム
     * @param sound_heap    管理対象のサウンドヒープ
     * @param settings      階層管理の動作設定
     */
    BGMLevelManager( 
        gfl2::heap::HeapBase* heap, 
        gfl2::sound::SoundSystem* sound_system,
        gfl2::sound::SoundHeap* sound_heap,
        const Settings& settings );

    /**
     * @brief デストラクタ
     */
    virtual ~BGMLevelManager();

    //-------------------------------------------------------------------------
    // 階層状態取得
    //-------------------------------------------------------------------------
    u32 GetCurrentLevel( void ) const;                   // 現在の階層レベル
    u32 GetSoundItemIDOnCurrentLevel( void ) const;      // サウンドアイテムID
    u32	GetSoundHandleIndexOnCurrentLevel( void ) const; // サウンドハンドルのインデックス

    //-------------------------------------------------------------------------
    // サウンド操作
    //-------------------------------------------------------------------------
    b32 LoadBGMData( u32 sound_item_id );    // BGMサウンドデータを読み込む
    void UnloadBGMData( u32 sound_item_id );  // BGMサウンドデータを破棄する
    // サウンドデータを保存する
    b32 PushBGMData( const nw::snd::SoundStartable::StartInfo* start_info );
    // サウンドデータを復元する
    b32 PopBGMData( 
      u32* pop_sound_itemID = NULL,
      u32* pop_sound_handleID = NULL,
      nw::snd::SoundStartable::StartInfo* pop_sound_start_info = NULL );  



    private:

    // 階層の状態
    struct LevelState {
      b32 loadFlag;                                // サウンドデータをロードしたかどうか
      u32 soundItemID;                              // サウンドアイテムのID
      s32 soundHeapLevel_beforeLoad;                // サウンドデータをロードする前のヒープレベル
      s32 soundHeapLevel_afterLoadCore;             // 退避させるデータをロードした後のヒープレベル
      u32 soundHandleIndex;                         // 再生に使用するサウンドハンドルのインデックス
      nw::snd::SoundStartable::StartInfo startInfo; // Push 時の状態を保存する
    }; 

    gfl2::sound::SoundSystem* m_soundSystem;
    gfl2::sound::SoundHeap* m_soundHeap;
    LevelState* m_levelState;
    u32 m_currentLevel;

    void InitHeapLevelState( u32, s32, u32 );
    void ClearHeapLevelState( u32, s32 );
    LevelState* GetCurrentHeapLevelState( void ) const;
    LevelState* GetHeapLevelState( u32 ) const;
  };



} // namespace Sound


#endif // __BGMLEVELMANAGER_H__
