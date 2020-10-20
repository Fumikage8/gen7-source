//=======================================================================================
/**
 * @file SELevelManager.h
 * @brief SE階層管理クラス
 * @author obata_toshihiro
 * @date 2012.02.25
 *
 * @note 
 * サウンドハンドル, サウンドデータを階層構造で管理する.
 */
//=======================================================================================
#ifndef __SELEVELMANAGER_H__
#define __SELEVELMANAGER_H__
#pragma once


#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "niji_conv_header/sound/SoundDefine.h"
#include "niji_conv_header/sound/SoundMiddleID.h"

namespace gfl2{
  namespace sound{
    class SoundSystem;
    class SoundHeap;
  }
}

namespace Sound {


  class SELevelManager
  {
    GFL_FORBID_COPY_AND_ASSIGN( SELevelManager );


    public:

    static const u32 MAX_LEVEL = 6;
    enum Result{
      LOADING,        // 読み込み中
      LOADED,         // 読み込み済み
      LOAD_FINISH,    // 読み込み完了した瞬間
    };

    //-------------------------------------------------------------------------
    // コンストラクタ・デストラクタ・セットアップ
    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap          インスタンスの生成に使用するヒープ
     * @param sound_system  関連付けるサウンドシステム
     * @param sound_heap    管理対象のサウンドヒープ
     * @param settings      階層管理の動作設定
     */
    SELevelManager( 
        gfl2::heap::HeapBase* heap, 
        gfl2::sound::SoundSystem* sound_system,
        gfl2::sound::SoundHeap* sound_heap );

    /**
     * @brief デストラクタ
     */
    virtual ~SELevelManager();

    //-------------------------------------------------------------------------
    // サウンドデータ
    //-------------------------------------------------------------------------
    b32 LoadSEData( u32 sound_item_id );           // SEサウンドデータを読み込む
    void UnloadSEData( u32 sound_item_id );         // SEサウンドデータを破棄する
    b32 PushSEData( void );                        // サウンドデータを保存する
    b32 PopSEData( u32* pop_sound_itemID = NULL ); // サウンドデータを復元する
    u32 GetCurrentLevel( void ) const;              // 現在の階層レベルを取得する

    b32 LoadSEDataAsync( gfl2::heap::HeapBase * thread_heap, u32 sound_item_id );      // SEサウンドデータを読み込む
    Result WaitLoadSeDataAsync(void);                 // SEサウンドデータ読み込み待ち
     


    private:

    static const u32 ASYNC_LOAD_NONE_ID = 0xffffffff; // 非同期読み込みなし

    // 階層の状態
    struct LevelState {
      b32 loadFlag;                    // サウンドデータをロードしたかどうか
      u32 soundItemID;                  // サウンドアイテムのID
      s32 soundHeapLevel_beforeLoad;    // サウンドデータをロードする前のヒープレベル
      s32 soundHeapLevel_afterLoadCore; // サウンドデータをロードした後のヒープレベル
    }; 

    gfl2::sound::SoundSystem* m_soundSystem;
    gfl2::sound::SoundHeap* m_soundHeap;
    LevelState* m_levelState;
    u32 m_currentLevel;
    u32 m_async_load_sound_item_id;

    void InitHeapLevelState( u32, s32 );
    LevelState* GetCurrentHeapLevelState( void ) const;
    LevelState* GetHeapLevelState( u32 ) const;
  };



} // namespace Sound


#endif // __SELEVELMANAGER_H__
