//=======================================================================================
/**
 * @file EnvSEManager.h
 * @brief 環境音管理クラス
 * @author miyuki iwasawa
 * @date 2012.02.25
 *
 * @note 
 * サウンドハンドル, サウンドデータを階層構造で管理する.
 */
//=======================================================================================
#ifndef __ENV_SE_MANAGER_H__
#define __ENV_SE_MANAGER_H__
#pragma once


#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "niji_conv_header/sound/SoundDefine.h"
#include "niji_conv_header/sound/SoundMiddleID.h"

namespace Sound {


  class EnvSEManager
  {
    GFL_FORBID_COPY_AND_ASSIGN( EnvSEManager );


    public:
    
    // 個々の音の発音状態管理
    struct PlayState {
      u32 soundItemID;                  // サウンドアイテムのID
      f32 volume;                       // 再生ボリューム
    }; 

    static const u32 EnvSEManager::MAX_PLAY_NUM = 2;  //最大同時発音数
    static const u32 EnvSEManager::ITEM_NONE = 0xFFFFFFFF;  //無効アイテムID
    static const f32 DEFAULT_VOLUME; //<デフォルトボリューム(float値はコンパイラ仕様でここで初期化できないのでEnvSEManager.cppで初期値定義している)

    //-------------------------------------------------------------------------
    // コンストラクタ・デストラクタ・セットアップ
    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    EnvSEManager(); 

    /**
     * @brief デストラクタ
     */
    virtual ~EnvSEManager();

    //-------------------------------------------------------------------------
    // サウンドデータ
    //-------------------------------------------------------------------------
    void ResetAll( void );                          // ステータスを全リセットする
    b32 Entry( u32 soundID, f32 volume );
    b32 ChangeVolume( u32 soundID, f32 volume );
    void Release( u32 soundID );
  
    void SetPause( EnvSEPauseState state );
    int  ResetPause( EnvSEPauseState state, PlayState* p_out );

    b32 IsActive(void ){ return (m_pauseState == ENVSE_PAUSE_NONE); }

    private:


    u32 m_latestIdx;   //
    u32 m_pauseState;

    PlayState m_state[MAX_PLAY_NUM];


    u32 GetLatestIdx( void ) const; //< 最新の再生情報indexを取得する
    
    void EntryState( u32 idx, u32 soundID, f32 volume );
    void ClearState( u32 idx );     //< 指定した再生ステータスをクリアする
    void ClearStateBySoundID( u32 soundID );
    b32 SetVolumeStatus( u32 soundID, f32 volume );
    int  SearchBlankState( u32 soundID );
    
    b32 IsLoopSE( u32 soundID );
  };



} // namespace xy_snd


#endif // __ENV_SE_MANAGER_H__
