#ifndef __GFL_USE_H__
#define __GFL_USE_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   GflvUse.h
 *  @brief  GameManagerを動作させるために必要なGFLIBの管理
 *  @date   2015.01.30
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <gfx/include/gfl2_GFGL.h>
#include <heap/include/gfl2_Heap.h>
#include "HeapDefine.h"
#include <ui/include/gfl2_UI.h>
#include <fs/include/gfl2_Fs.h>

///#include <System/include/PokemonLanguage.h>
#include "../../Print/include/SystemFont.h"

//-----------------------------------------------------------------------------
//
//  friend関数のGameManager　前方宣言
//
//-----------------------------------------------------------------------------

namespace gfl2{
  namespace math{
    class MTRandom;
    class Random;
  }
  namespace fs{
    class AsyncFileManager;
  }
}

namespace GameSystem {
  class GameManager;
}

namespace System{
  class HomeNixDisp;
  class nijiAllocator;

  namespace event{
    class SystemEventManager;
  }

//-----------------------------------------------------------------------------
//
//          定数宣言
//
//-----------------------------------------------------------------------------

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
 *          @class GflvUse
*/
//-----------------------------------------------------------------------------
class GflvUse {

public:

	//-----------------------------------------------------------------------------
	/*
	 *          内部クラス宣言
	 *          @class GLAllocator
	*/
	//-----------------------------------------------------------------------------
	class GLAllocator : public gfl2::gfx::IGLAllocator
	{
	public:
		
		//----------------------------------------------------------------------------
		/**
		*  @brief  コンストラクタ
		*/
		//-----------------------------------------------------------------------------
		GLAllocator():IGLAllocator(){}

		//----------------------------------------------------------------------------
		/**
		*  @brief  デストラクタ
		*/
		//-----------------------------------------------------------------------------
		virtual			~GLAllocator()
		{
		#if defined(GF_PLATFORM_CTR)
			this->fcramHeap.Finalize(); 
			GFL_DELETE_ARRAY fcramHeapMem;
		#endif
		}
		
		//----------------------------------------------------------------------------
		/**
		*  @brief  コンストラクタ
		*/
		//-----------------------------------------------------------------------------
		void			init(void)
		{
		#if defined(GF_PLATFORM_CTR)
			// デバイスメモリ全体 仮
			gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );
			fcramHeapMem = GFL_NEW_ARRAY(systemHeap) char[FCRAM_HEAMP_SIZE];
			this->fcramHeap.Initialize( (uptr)fcramHeapMem , FCRAM_HEAMP_SIZE );
			
			// VRAMA全体 仮
			this->vramAHeap.Initialize(nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMA), nn::gx::GetVramSize(nn::gx::MEM_VRAMA));
			// VRAMB全体 仮
			this->vramBHeap.Initialize(nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMB), nn::gx::GetVramSize(nn::gx::MEM_VRAMB));
		#endif
		}
#if 0
		//----------------------------------------------------------------------------
		/**
		*  @brief  メインメモリの取得
		*  @param  取得したいメモリサイズ
		*  @param  アライメント
		*  @return メモリ
		*/
		//-----------------------------------------------------------------------------
		virtual void*	SystemMemoryMalloc(u32 size, u32 alignment)
		{
		#if defined(GF_PLATFORM_CTR)
			return fcramHeap.Allocate(size, alignment);
		#else
			return malloc(size);
		#endif
		}
#endif
		//----------------------------------------------------------------------------
		/**
		*  @brief  メインメモリの解放
		*  @param  解放したいメモリのポインタ
		*/
		//-----------------------------------------------------------------------------
		virtual void	SystemMemoryFree(void* const address)
		{
		#if defined(GF_PLATFORM_CTR)
			fcramHeap.Free(address); 
		#else
			free( address );
		#endif
		}

		//----------------------------------------------------------------------------
		/**
		*  @brief  ビデオメモリの取得
		*  @param  取得したいメモリサイズ
		*  @param  アライメント
		*  @return メモリ
		*/
		//-----------------------------------------------------------------------------
		virtual void*	LocalMemoryMalloc(u32 size, u32 alignment)
		{
		#if defined(GF_PLATFORM_CTR)
			// A, B容量が大きいほうを使う
			if (this->vramAHeap.GetAllocatableSize(alignment) > this->vramBHeap.GetAllocatableSize(alignment))
			{
				return this->vramAHeap.Allocate(size, alignment);
			}
			return this->vramBHeap.Allocate(size, alignment);
		#else
			return NULL;
		#endif
		}

		//----------------------------------------------------------------------------
		/**
		*  @brief  ビデオメモリの解放
		*  @param  解放したいメモリのポインタ
		*/
		//-----------------------------------------------------------------------------
		virtual void	LocalMemoryFree(void* const address){ /* todo */ }
		
	private:

	#if defined(GF_PLATFORM_CTR)
		static const int	FCRAM_HEAMP_SIZE = 1024*1024*4;
		char*				fcramHeapMem;
		nn::fnd::ExpHeap	fcramHeap;
		nn::fnd::FrameHeap	vramAHeap;
		nn::fnd::FrameHeap	vramBHeap;
	#endif

	};

public:
  //!同時に受け付けることができる読み込みリクエストの数
  static const int FILEMANAGER_MAX_REQUEST_COUNT = 192;
  //!同時に非同期読み込みを実行する数
  static const int FILEMANAGER_MAX_READ_COUNT = 5;

  //!ウィンドウハンドル
  static gfl2::gfx::DisplayDeviceHandle  m_HandleList[gfl2::gfx::CtrDisplayNo::NumberOf];
  //!ディスプレイリスト
  static gfl2::gfx::DisplayList			*m_pDisplayLists[2];
  //!GFGL用のアロケータ
  static nijiAllocator*					 m_pAllocator;


  //-----------------------------------------------------------------------------
  /**
   *  GameManagerだけは、メンバに参照可能。
   */
  //-----------------------------------------------------------------------------
  friend class GameSystem::GameManager;


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
   *  @brief  乱数取得(0～U32_MAX)
   *
   *  @return 乱数
   */
  //-----------------------------------------------------------------------------
  static u32 GetPublicRand( void );
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
   *  @brief  ポケモン生成乱数取得64bit版（0?U64_MAX)
   *
   *  @return 乱数
   *
   *  @note タマゴ生成用seedなど、乱数オブジェクトの初期化に用いるseed値などに利用
   */
  //-----------------------------------------------------------------------------
  static u64 GetPokemonCreateRand64( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ポケモン生成乱数初期化
   *  @param  乱数
   *  @param  seed
   */
  //-----------------------------------------------------------------------------
  static void PokemonCreateRandomInit(u32 seed);

  //----------------------------------------------------------------------------
  /**
   *  @brief  アーカイブファイルをPCから読み込むことが可能か？
   *
   *  @retval true    可能
   *  @retval false   不可能　→　可能にするには、デバッガ設定をdev2にしてください。
   */
  //-----------------------------------------------------------------------------
  static bool IsArcHioMode( void );

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
   *   @brief GFLIB 更新
   */
  //-----------------------------------------------------------------------------
  static void Update( void );


  //-----------------------------------------------------------------------------
  /**
   *   @brief GFLIB VSync処理
   */
  //-----------------------------------------------------------------------------
  static void VSyncWait( void );    ///< VSync待ち
  static void VSync( void );        ///< VSync後処理

  static void FatalErrorDisp( void ); ///< Fatalエラー画面の表示

  //----------------------------------------------------------------------------
  /**
   *  @brief  グラフィックスシステムの実行が完了してるかチェックする。
   */
  //-----------------------------------------------------------------------------
  static bool IsGpuIdle( void );
 
  //----------------------------------------------------------------------------
  /**
   *  @brief  ホームボタン＆電源ボタンの復帰処理
   */
  //-----------------------------------------------------------------------------
  static void ReturnProsessing(void);

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
   * @attention  必ず成功するので気を付けて 特にセーブが終わってから呼ぶこと
   */
  //------------------------------------------------------------------
  static void SetSoftResetRequest(void);

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
    gfl2::math::MTRandom*             m_pPokemonCreateRandom;                ///< ポケモン生成乱数
    gfl2::math::Random*               m_pRandom;                   ///< 乱数
//    gfl2::grp::Fade*                m_pFade;                   ///< フェード

    gfl2::fs::AsyncFileManager*       m_romAsyncFileManager;     ///< ファイルリードマネージャ
    gfl2::fs::NameTable*              m_romArcFileTable;         ///< arcファイルリスト
#if GFL_DEBUG
    gfl2::fs::NameTable*              m_romHioArcFileTable;      ///< CTRのHIOのarcファイルリスト
    gfl2::fs::NameTable*              m_romArcSrcFileTable;      ///< gascファイルリスト
#endif

    //gfl::base::AutoStackManager*    m_autoStackManager;         ///< AutoStackManager
    ///gfl2::ro::RoManager*              m_roManager;              ///<動的モジュールマネージャー@saito

    event::SystemEventManager*      m_pSystemEventManager;    ///< システムイベント

    ///Language*                       m_pLanguage;              ///< 言語コード
    ///view_print::SystemFont*              m_pSystemFont;            ///< システムフォント
    gfl2::heap::HeapBase*           m_pSystemFontHeap;        ///< フォント用ヒープ
    gfl2::heap::HeapBase*           m_pSystemFontDevHeap;     ///< フォント用デバイスヒープ

    s32   m_LastVCount;   ///< 1つ前VCount監視

    bool   CPUframeOverFlag;    ///< 処理落ちフラグ
    bool   SoftResetFlg;   ///<ソフトリセットを起こすフラグ
    
    //sango追加
    bool   mScreenCapturePostPermission;
  };
  static SingltonLibrary m_Singlton;  ///< Singltonデータ

#if defined(GF_PLATFORM_CTR)
 #if GFL_ENABLE_ASSERT
  #if !(PM_DEBUG_PRINT)
  static const char* m_pAssertMsgBuffer;  ///< スレッド内アサート文字列格納バッファ
  static nn::os::CriticalSection m_pCriticalSectionForAssertMsgBuffer;  ///< スレッド内アサートメッセージバッファ用クリティカルセクション
  #endif // PM_DEBUG_PRINT
 #endif // GFL_ENABLE_ASSERT
#endif //defined(GF_PLATFORM_CTR)

  //! Defaultのグラフィックスセットアップ情報
//  static const gfl::grp::GraphicsSystemSetUp m_DefaultGraphicsSetUp;
  // GraphicsSystem常駐メモリサイズ
  static const u32 DEFAULT_GRAPHICS_SYSTEM_CONTROL_VRAMA_SIZE = 0x100000-0x3000;  ///< VRAMAのシステム使用サイズ
  static const u32 DEFAULT_GRAPHICS_SYSTEM_CONTROL_VRAMB_SIZE = 0x1a0000;         ///< VRAMBのシステム使用サイズ

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

  //ファイルシステムの初期化と終了
  static void FileSystemInitialize(gfl2::heap::HeapBase* heap);
  static void FileSystemTerminate(void);


};


} // xy_system

#endif  // __GFL_USE_H__

