#ifndef __GFL_USE_H__
#define __GFL_USE_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   GflUse.h
 *  @brief  GameManagerを動作させるために必要なGFLIBの管理
 *  @date   2015.01.30
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <gfx/include/gfl2_GFGL.h>
#include <heap/include/gfl2_Heap.h>
#include <system/include/HeapDefine.h>
#include <ui/include/gfl2_UI.h>
#include <fs/include/gfl2_Fs.h>

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

#include <System/include/PokemonLanguage.h>
#include <Print/include/SystemFont.h>
#include "niji_reference_files/Script/SystemPawnTypes.h"

//-----------------------------------------------------------------------------
//
//  friend関数のGameManager　前方宣言
//
//-----------------------------------------------------------------------------

namespace gfl2{
  namespace math{
    class SFMTRandom;
  }
  namespace fs{
    class AsyncFileManager;
  }
}

namespace GameSys {
  class GameManager;
}

namespace ExtSavedata {
  class NijiExtSaveSystem;
}

namespace System{
  class HomeNixDisp;
  class nijiAllocator;
  class SystemEventManager;

//-----------------------------------------------------------------------------
//
//          定数宣言
//
//-----------------------------------------------------------------------------

#define NIJI_DYNAMIC_HEADER_ALERTLIMIT 0x0200000                                   // 動的ヘッダシステム : ヒープ使用量警告の閾値
#define NIJI_DYNAMIC_HEADER_FILEAMOUNTMAX 16                                       // 動的ヘッダシステム : 同時に読み込めるヘッダファイルの数



// アサート用アプレットの準備をするか否か
#if defined(GF_PLATFORM_CTR)
#if !defined( GF_CONFIG_FINAL )

// この定義が有効で、デバッガ未接続時のみアプレット表示 @note マスター版使用不可関数を使用します、FAINALでは必ず無効にしてください
#define VALID_ASSEART_APPLET

#endif
#endif

//-----------------------------------------------------------------------------
/**
 * @brief デバッグ用SDカードアクセス定義
 *
 */
//-----------------------------------------------------------------------------
#define GFL_ENABLE_SDMC    (0)

//-----------------------------------------------------------------------------
//
//          構造体宣言
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/*
 *          クラス宣言
 *          @class GflUse
*/
//-----------------------------------------------------------------------------
class GflUse {

public:

#if PM_DEBUG
  struct RandDebug
  {
    enum Value
    {
      NONE,         // デバッグ操作無し
      FIX_MIN,      // 最小値固定
      FIX_MAX,      // 最大値固定

      MAX_EFFECTIVE_VAL = FIX_MAX
    };
  };
#endif

public:
  static nijiAllocator* GetResidentAllocator(){ return m_pAllocator; }
  static nijiAllocator* GetEventDeviceAllocator(){ return m_pEventDeviceAllocator; }
#if PM_DEBUG
	static nijiAllocator* GetDebugAllocator(){ return m_pAllocatorForDebug; }
#endif

public:
  //!同時に受け付けることができる読み込みリクエストの数
  static const int FILEMANAGER_MAX_REQUEST_COUNT = 192;
  //!同時に非同期読み込みを実行する数
  static const int FILEMANAGER_MAX_READ_COUNT = 5;


  //!ウィンドウハンドル
  static gfl2::gfx::Surface* GetRenderTargetColor( gfl2::gfx::CtrDisplayNo disp ){ return gfl2::gfx::GFGL::GetBackBuffer(m_HandleList[disp]); }
  static gfl2::gfx::Surface* GetRenderTargetDepth( gfl2::gfx::CtrDisplayNo disp ){ return gfl2::gfx::GFGL::GetDepthStencilBuffer(m_HandleList[disp]); }
  static gfl2::gfx::DisplayDeviceHandle  m_HandleList[gfl2::gfx::CtrDisplayNo::NumberOf];
  //!ディスプレイリスト
  static gfl2::gfx::DisplayList			*m_pDisplayLists[2];

#if defined(GF_PLATFORM_WIN32)
  //!Textureテンポラリー用アロケーター
  static nijiAllocator*					 m_pTextureFactoryTemporraryAllocator;
#endif
  

  //-----------------------------------------------------------------------------
  /**
   *  GameManagerだけは、メンバに参照可能。
   */
  //-----------------------------------------------------------------------------
  friend class GameSys::GameManager;


  // 自由に使用可能
public:

  //-----------------------------------------------------------------------------
  /*
   *    乱数
   */
  //-----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  /**
   *  @brief  乱数取得
   *
   *  @param  max_range 取得する最大値（0 <= n < max_range）
   *
   *  @return 乱数
   */
  //-----------------------------------------------------------------------------
  static u32 GetPublicRand( u32 max_range );

  //----------------------------------------------------------------------------
  /**
   *  @brief  乱数取得(0-U32_MAX)
   *
   *  @return 乱数
   */
  //-----------------------------------------------------------------------------
  static u32 GetPublicRand( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  乱数取得
   *
   *  @param  max_range 取得する最大値（0 <= n < max_range）
   *
   *  @return 乱数
   */
  //-----------------------------------------------------------------------------
  static u64 GetPublicRand64( u64 max_range );

  //----------------------------------------------------------------------------
  /**
   *  @brief  乱数取得(0-U64_MAX)
   *
   *  @return 乱数
   */
  //-----------------------------------------------------------------------------
  static u64 GetPublicRand64( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ポケモン生成乱数取得
   *  @param  max_range 取得する最大値（0 <= n < max_range）
   *  @return 乱数
   */
  //-----------------------------------------------------------------------------
  static u32 GetPokemonCreateRand( u32 max_range );
  //----------------------------------------------------------------------------
  /**
   *  @brief  ポケモン生成乱数取得（0?U32_MAX)
   *
   *  @return 乱数
   */
  //-----------------------------------------------------------------------------
  static u32 GetPokemonCreateRand( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ポケモン生成乱数取得64bit版（0-U64_MAX)
   *
   *  @return 乱数
   *
   *  @note タマゴ生成用seedなど、乱数オブジェクトの初期化に用いるseed値などに利用
   */
  //-----------------------------------------------------------------------------
  static u64 GetPokemonCreateRand64( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ポケモン生成乱数取得
   *  @param  max_range 取得する最大値（0 <= n < max_range）
   *  @return 乱数
   */
  //-----------------------------------------------------------------------------
  static u64 GetPokemonCreateRand64( u64 max_range );

  //----------------------------------------------------------------------------
  /**
   *  @brief  暗号用途の乱数を生成します
   *  @detail 暗号用途の場合にのみ使用してください
   */
  //-----------------------------------------------------------------------------
  static void GenerateRandomBytes(void* pDst, size_t size);

  //----------------------------------------------------------------------------
  /**
   *  @brief  アーカイブファイルをPCから読み込むことが可能か？
   *
   *  @retval true    可能
   *  @retval false   不可能　→　可能にするには、デバッガ設定をdev2にしてください。
   */
  //-----------------------------------------------------------------------------
  //static bool IsArcHioMode( void );  // nijiではFSに用意してあるHIO有効無効関数を使うので、これはコメントアウト。

  //----------------------------------------------------------------------------
  /**
   *  @brief  システムフォントの取得
   */
  //-----------------------------------------------------------------------------
  static print::SystemFont* GetSystemFont(){ return m_Singlton.m_pSystemFont; }

  // GFL_USEの管理
public:
  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープマネージャの初期化    nninitStartUpで行うため
   */
  //-----------------------------------------------------------------------------
  static void InitializeHeapManager( void );

  //-----------------------------------------------------------------------------
  /**
   *   @brief GFLIB　使用準備
   */
  //-----------------------------------------------------------------------------
  static void Initialize( void );

  //-----------------------------------------------------------------------------
  /**
   *   @brief GFLIB　破棄
   */
  //-----------------------------------------------------------------------------
  static void Finalize( void );

  //-----------------------------------------------------------------------------
  /**
   *   @brief GFLIB 更新(ProcのUpdate前)
   */
  //-----------------------------------------------------------------------------
  static void UpdateBeforeProc( void );

  //-----------------------------------------------------------------------------
  /**
   *   @brief GFLIB 更新(ProcのUpdate後)
   */
  //-----------------------------------------------------------------------------
  static void UpdateAfterProc( void );

  //-----------------------------------------------------------------------------
  /**
   *   @brief アプレット更新
   */
  //-----------------------------------------------------------------------------
  static void UpdateAppletSystem( void );

  //-----------------------------------------------------------------------------
  /**
   *   @brief GFLIB VSync処理
   */
  //-----------------------------------------------------------------------------
  static void VSyncWait( void );    ///< VSync待ち
  static void VSync( void );        ///< VSync後処理


  //----------------------------------------------------------------------------
  /**
   *  @brief  グラフィックスシステムの実行が完了してるかチェックする。
   */
  //-----------------------------------------------------------------------------
  static bool IsGpuIdle( void );
 


  //------------------------------------------------------------------
  /**
   * @brief   ソフトリセットリクエスト確認
   * @return リクエストがあると１になる
   */
  //------------------------------------------------------------------
  static bool IsSoftResetRequest(void);
  //------------------------------------------------------------------
  /**
   * @brief   画面を消す（ソフトリセット用）
   */
  //------------------------------------------------------------------
  static void SoftResetGraphicSystemClear(void);
  static void VramClear(void);///< VRAMクリア
  //------------------------------------------------------------------
  /**
   * @brief   ソフトリセットリクエスト
   * @caution  必ず成功するので気を付けて 特にセーブが終わってから呼ぶこと
   */
  //------------------------------------------------------------------
  static void SetSoftResetRequest(void);

  //------------------------------------------------------------------
  /**
   * @brief   常駐GARCをPCから読み込みなおす用にリロード
   */
  //------------------------------------------------------------------
  static void ReloadGarc();

  //------------------------------------------------------------------
  /**
   * @brief   CTR本体リージョンコードの取得
   * @caution  nn::cfg::CTR::CfgRegionCodeと一致させている
   */
  //------------------------------------------------------------------
  static HardwareRegion GetHardwareRegion(void);

  //------------------------------------------------------------------
  /**
   * @brief   CTR本体リージョンコードの初期化
   * @caution  nn::cfg::CTR::CfgRegionCodeと一致させている
   */
  //------------------------------------------------------------------
  static void InitHardwareRegion(void);

#if PM_DEBUG
  //------------------------------------------------------------------
  /**
   * @brief   CTR本体リージョンコードの変数アドレス取得
   */
  //------------------------------------------------------------------
  static s32* DebugGetHardwareRegionPointer(void);

  //------------------------------------------------------------------
  /**
  * @brief   乱数デバッグ制御の変数アドレス取得
  */
  //------------------------------------------------------------------
  static System::GflUse::RandDebug::Value* DebugGetRandDebugPointer( void ) { return &m_RandDebugMode; }

#endif  //PM_DEBUG



  //------------------------------------------------------------------
  /**
    *  @brief    eshopアプリが有効であるか
    */
  //------------------------------------------------------------------
  static bool IsEshopAvailable(void);


  //------------------------------------------------------------------
  /**
   * @brief   eshopページへの遷移リクエスト
   * @parma   [in]  bToPatchPage  true:パッチページへ（※）   false:タイトルページへ
   * @retval  true:   リクエストが受理され、この後アプリケーションが終了し、eshopのPatchPageへ遷移する
   * @retval  false:  リクエストは受理されなかった（eshopアプリがないため遷移不可：本体更新が必要の旨表示することがリファレンスで推奨されている）
   * @note    アプリケーションの更新をリマスターで運用している場合は、ニンテンドーeショップにタイトルの「パッチ」ページが存在しません。
   */
  //------------------------------------------------------------------
  static bool TransitToEshopRequest(const bool bToPatchPage);

  static bool IsTransitToEshopTitlePageRequest(void);
  static bool IsTransitToEshopPatchPageRequest(void);

  //-----------------------------------------------------------------------------
  /**
   *   Miiverseへ写真投稿の可否を設定する
   *   trueで投稿可能 falseで投稿不可能になります。   
   */
  //-----------------------------------------------------------------------------
  static void SetScreenCapturePostPermission( bool isEnable );
  //-----------------------------------------------------------------------------
  /**
   *   Miiverseへ写真投稿の可否を設定するの取得
   *   trueで投稿可能 falseで投稿不可能です。
   */
  //-----------------------------------------------------------------------------
  static bool IsScreenCapturePostPermission( void );



private:
  //-----------------------------------------------------------------------------
  /**
   *    常駐GFLIB
   */
  //-----------------------------------------------------------------------------

  class AppletObj;

  //! singletonライブラリ
  struct SingltonLibrary
  {
    gfl2::ui::DeviceManager*        m_pUiDeviceManager;       ///< UIデバイスマネージャ
//    gfl2::grp::GraphicsSystem*      m_pGraphicsSystem;        ///< グラフィックスシステム
//    gfl2::heap::NwAllocator*        m_pGraphicsHeapMemAllocator;  ///< グラフィック　ヒープアロケータ
//    gfl2::heap::NwAllocator*        m_pGraphicsDeviceMemAllocator;  ///< グラフィックス　デバイスヒープアロケータ
    gfl2::math::SFMTRandom*               m_pRandom;                   ///< 乱数
//    gfl2::grp::Fade*                m_pFade;                   ///< フェード

    gfl2::fs::AsyncFileManager*       m_romAsyncFileManager;     ///< ファイルリードマネージャ
    gfl2::fs::NameTable*              m_romArcFileTable;         ///< arcファイルリスト
#if PM_DEBUG
    gfl2::fs::NameTable*              m_hioArcFileTable;         ///< CTRのHIOのarcファイルリスト
    gfl2::fs::NameTable*              m_arcSrcFileTable;         ///< gascファイルリスト
    gfl2::fs::NameTable*              m_directSdmcArcFileTable;  ///< DirectSdmcにあるgarcファイル関連
#endif

    //gfl::base::AutoStackManager*    m_autoStackManager;         ///< AutoStackManager
    gfl2::ro::RoManager*              m_roManager;              ///<動的モジュールマネージャー

    System::SystemEventManager*      m_pSystemEventManager;    ///< システムイベント
    NetLib::NijiNetworkSystem*       m_pNijiNetworkSystem;      ///< 通信システム

    ExtSavedata::NijiExtSaveSystem* m_pNijiExtSaveSystem;
    Language*                       m_pLanguage;              ///< 言語コード

    gfl2::heap::NwAllocator * m_pMemAllocator;  //layout
    gfl2::heap::NwAllocator * m_pDevAllocator;

    print::SystemFont*              m_pSystemFont;            ///< システムフォント
    gfl2::heap::HeapBase*           m_pSystemFontHeap;        ///< フォント用ヒープ
    gfl2::heap::HeapBase*           m_pSystemFontDevHeap;     ///< フォント用デバイスヒープ

    gfl2::heap::HeapBase*           m_pCharaModelHeap;     ///< キャラモデル用ヒープ

    s32   m_LastVCount;   ///< 1つ前VCount監視

    bool   CPUframeOverFlag;    ///< 処理落ちフラグ
    bool   SoftResetFlg;   ///<ソフトリセットを起こすフラグ
    
    bool   m_bTransitToEshopTitlePageRequest;   /**<  eshop titlePageへの遷移リクエスト  */
    bool   m_bTransitToEshopPatchPageRequest;   /**<  eshop patchPageへの遷移リクエスト  */
    
    
    //sango追加
    bool   mScreenCapturePostPermission;

    //niji追加
    HardwareRegion m_hardowareRegion; ///<CTRの本体リージョンコード
  };

  static SingltonLibrary m_Singlton;  ///< Singltonデータ

#if defined(VALID_ASSEART_APPLET)
  static const char*                    m_pAssertMsgBuffer;                    ///< スレッド内アサート文字列格納バッファ
  static gfl2::thread::CriticalSection  m_pCriticalSectionForAssertMsgBuffer;  ///< スレッド内アサートメッセージバッファ用クリティカルセクション
  static b32                            m_IsAssertCallApplet;                  ///< trueならアサートをアプレットで表示する、デバッガー起動ならtrue、CommonHeapCreate以降で有効
#endif //defined(GF_PLATFORM_CTR)

  //! Defaultのグラフィックスセットアップ情報
//  static const gfl::grp::GraphicsSystemSetUp m_DefaultGraphicsSetUp;
  // GraphicsSystem常駐メモリサイズ
  static const u32 DEFAULT_GRAPHICS_SYSTEM_CONTROL_VRAMA_SIZE = 0x100000-0x3000;  ///< VRAMAのシステム使用サイズ
  static const u32 DEFAULT_GRAPHICS_SYSTEM_CONTROL_VRAMB_SIZE = 0x1a0000;         ///< VRAMBのシステム使用サイズ

  //!GFGL用のアロケータ
  static nijiAllocator*					 m_pAllocator;

#if PM_DEBUG
  static nijiAllocator*			     m_pAllocatorForDebug;
  static nijiAllocator*			     m_pAllocatorForDebugDrawUtil;
  static void*                   m_pBufferForDebugDrawUtil;
  static RandDebug::Value        m_RandDebugMode;
#endif

  //!EVENT_DEVICEのアロケータ
  static nijiAllocator*					 m_pEventDeviceAllocator;


private:
#if 0
  static void createGraphicsSystemTitle( void ); ///< タイトル用としてグラフィックスシステムを初期化　メモリは下からとる。
  static void createGraphicsSystem( const gfl::grp::GraphicsSystemSetUp& setup, u32 vramA, u32 vramB, gfl::heap::HeapBase* p_heap, gfl::heap::HeapBase* p_device, bool heap_low, const gfl::math::VEC4& color );  ///< グラフィックスシステムの初期化
  static void deleteGraphicsSystem( void ); ///< グラフィックスシステムの破棄
  static bool isCreateGraphicsSystem( void ); ///< グラフィックスシステムが常駐しているか？
#endif
  static void checkHeap( void );  ///< ヒープチェック

  static void AssertLoopFunc( const char* buff,b32 bStop ); ///< ASSERTループ内コールバック処理
  static bool LoopFunc( const char* pbuff );      ///< ループ内コールバック処理
  static bool IsJpnFontLoadRegion(void);          ///< 本体リージョン確認

  //常駐ヒープ初期化と開放
  static void CommonHeapCreate(void);
  static void CommonHeapFree(void);

  // キャラモデルシステムの初期化と終了
  static void InitializeCharaModelSystem();
  static void TerminateCharaModelSystem();
  static void ReloadCharaModelSystem();

  // アサート設定
  static void AssertSetup( void );


  //ファイルシステムの初期化と終了
private:
  static void FileSystemInitialize(gfl2::heap::HeapBase* heap);
public:
  static void FileSystemTerminate(void);
};


} // xy_system

#endif  // __GFL_USE_H__

