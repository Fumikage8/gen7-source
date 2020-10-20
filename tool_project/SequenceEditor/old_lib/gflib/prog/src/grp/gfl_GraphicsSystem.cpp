//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_GraphicsSystem.cpp
 *	@brief  グラフィックスシステム    描画環境構築・管理
 *	@author	tomoya takahashi
 *	@date		2010.10.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <nn.h>

#include <grp/gfl_GraphicsSystem.h>
#include <base/gfl_AreaMan.h>
#include <gfl_Math.h>
#include <gfl_Std.h>
#include <ro/gfl_RoManager.h>

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(gfl::grp::GraphicsSystem);
template class gfl::base::SingletonAccessor<gfl::grp::GraphicsSystem>;
#endif


namespace gfl {
namespace grp {


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
const int ALIGNMENT_SYSTEM_BUFFER     = 4;
const int ALIGNMENT_VERTEX            = 4;
const int ALIGNMENT_TEXTURE           = 128;
const int ALIGNMENT_RENDER_BUFFER     = 64;
const int ALIGNMENT_DISPLAY_BUFFER    = 16;
const int ALIGNMENT_3D_COMMAND_BUFFER = 16;

enum ReadPixcelsState {
  READPIXCELS_REQ_NONE,
  READPIXCELS_REQ_ON_BLOCK,
  READPIXCELS_REQ_ON_LINER,
  READPIXCELS_REQ_WAIT,
};


// 初期化フラグ
bool GraphicsSystem::m_Initialize = false;

// ゲームとして立体視を使用するか？
bool GraphicsSystem::m_GameStereoFlag = true;



//-----------------------------------------------------------------------------
/**
 *					デフォルト初期化設定
*/
//-----------------------------------------------------------------------------

const GraphicsSystemSetUp GraphicsSystemSetupDefault = {
  MODE_DOUBLE_BUFFER,                     // バッファリングモード
  true,                                  // 右目を使用するか。
  false,                                 // 立体視On状態で初期化するか。

  // ディスプレイバッファ情報
  {
    // DisplayBuffer UPPER
    {
      DISPLAY_UPPER,                      // ディスプレイの種類です。
      400,                                // ディスプレイの横幅です。
      240,                                // ディスプレイの縦幅です。
      TRANSFER_MODE_ANTIALIASE_NOT_USED,  // ティスプレイバッファの転送モードです。
      MEMORY_AREA_VRAMA,                  // メモリ確保エリアです。
      RENDER_COLOR_FORMAT_RGB8,          // カラーフォーマットです
      false,                              // 転送時にＸ方向にフリップするかどうかを表します。
      0,                                  // 表示を行う始点Ｘ座標です。
      0                                   // 表示を行う始点Ｙ座標です。
    },
    // DisplayBuffer LOWER
    {
      DISPLAY_LOWER,                      // ディスプレイの種類です。
      320,                                // ディスプレイの横幅です。
      240,                                // ディスプレイの縦幅です。
      TRANSFER_MODE_ANTIALIASE_NOT_USED,  // ティスプレイバッファの転送モードです。
      MEMORY_AREA_VRAMB,                  // メモリ確保エリアです。
      RENDER_COLOR_FORMAT_RGB8,          // カラーフォーマットです
      false,                              // 転送時にＸ方向にフリップするかどうかを表します。
      0,                                  // 表示を行う始点Ｘ座標です。
      0                                   // 表示を行う始点Ｙ座標です。
    },
    // DisplayBuffer UPPER_RIGHT
    {
      DISPLAY_UPPER_RIGHT,                // ディスプレイの種類です。
      400,                                // ディスプレイの横幅です。
      240,                                // ディスプレイの縦幅です。
      TRANSFER_MODE_ANTIALIASE_NOT_USED,  // ティスプレイバッファの転送モードです。
      MEMORY_AREA_VRAMA,                  // メモリ確保エリアです。
      RENDER_COLOR_FORMAT_RGB8,          // カラーフォーマットです
      false,                              // 転送時にＸ方向にフリップするかどうかを表します。
      0,                                  // 表示を行う始点Ｘ座標です。
      0                                   // 表示を行う始点Ｙ座標です。
    },
  },

  // フレームバッファ情報
  // フレームバッファを流用することにより、VRAM領域をそこまで
  // 使わずに描画が可能です。
  {
    // FrameBuffer UPPER
    {
      DISPLAY_UPPER,                      // 上画面用
      400,                                // バッファの横幅です。
      240,                                // バッファの縦幅です。
      RENDER_COLOR_FORMAT_RGBA8,          // カラーバッファフォーマット
      RENDER_DEPTH_FORMAT_24_STENCIL8,    // 深度バッファフォーマット
      MEMORY_AREA_VRAMA,                  // 描画対象のカラーバッファの配置場所です。
      MEMORY_AREA_VRAMB,                  // 描画対象のデプスバッファの配置場所です。
      DISPLAY_NONE,                       // このフレームバッファがオリジナル
      0,                                  //Viewport左です。
      0,                                  //Viewport上です。
      400,                                //Viewport右です。
      240,                                //Viewport下です。
    },

    // FrameBuffer LOWER
    {
      DISPLAY_LOWER,                      // 下画面用
      320,                                // バッファの横幅です。
      240,                                // バッファの縦幅です。
      RENDER_COLOR_FORMAT_RGBA8,          // カラーバッファフォーマット
      RENDER_DEPTH_FORMAT_24_STENCIL8,    // 深度バッファフォーマット
      MEMORY_AREA_NONE,                   // 描画対象のカラーバッファの配置場所です。
      MEMORY_AREA_NONE,                   // 描画対象のデプスバッファの配置場所です。
      DISPLAY_UPPER,                      // UPPERのフレームバッファを流用
      0,                                  //Viewport左です。
      0,                                  //Viewport上です。
      320,                                //Viewport右です。
      240,                                //Viewport下です。
    },

    // FrameBuffer UPPER_RIGHT
    {
      DISPLAY_UPPER_RIGHT,                // 下画面用
      400,                                // バッファの横幅です。
      240,                                // バッファの縦幅です。
      RENDER_COLOR_FORMAT_RGBA8,          // カラーバッファフォーマット
      RENDER_DEPTH_FORMAT_24_STENCIL8,    // 深度バッファフォーマット
      MEMORY_AREA_NONE,                   // 描画対象のカラーバッファの配置場所です。
      MEMORY_AREA_NONE,                   // 描画対象のデプスバッファの配置場所です。
      DISPLAY_UPPER,                      // UPPERのフレームバッファを流用
      0,                                  //Viewport左です。
      0,                                  //Viewport上です。
      400,                                //Viewport右です。
      240,                                //Viewport下です。
    },
  },

  // コマンドリスト設定
  {
    // MAIN
    {
      0x30000,                 // バッファサイズ（コマンドバッファの総サイズ）
      48,                       // キューイングコマンド数
    },

    // UPPER
    {
      0x20000,                 // バッファサイズ（コマンドバッファの総サイズ）
      16,                      // キューイングコマンド数
    },

    // LOWER
    {
      0x20000,                 // バッファサイズ（コマンドバッファの総サイズ）
      16,                      // キューイングコマンド数
    },
  },

  // セーブコマンドリスト設定
  {
    0x80000,                 // バッファサイズ（コマンドバッファの総サイズ）
    16,                      // キューイングコマンド数
  },

  // クリアカラー
  {
    // ClearColor UPPER
    {
      0.20f, 0.20f, 0.20f, 1.0f, 1.0f, 0
    },

    // ClearColor LOWER
    {
      0.20f, 0.20f, 0.20f, 1.0f, 1.0f, 0
    },
  },
};


//-----------------------------------------------------------------------------
/**
 *					メモリコントロールクラス定義
 *					*VRAMメモリの管理のために、このクラスは、0xC60Byteの大きさがあります。
*/
//-----------------------------------------------------------------------------
class MemoryControl
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(MemoryControl);

public:
  static const u32 AREAMAN_BLOCK_SIZE = 512;      // エリアマネージャ　ブロックサイズ (16x16 pixcel　の 16bitカラーのsize）
  static const u32 AREAMAN_BLOCK_NUM  = 0x300000 / AREAMAN_BLOCK_SIZE; // エリアマネージャ　ブロック数

  static const u32 VRAM_ALLOCK_BLOCK_NUM = 256;   // エリアマネージャ　アロケート可能数

  enum ControlType
  {
    APPLICATION_TYPE = 0,     // アプリケーション動作タイプ
    SYSTEM_TYPE,              // システム動作タイプ
  };

  enum AllocWay
  {
    ALLOC_TOP = 0,            // メモリの上側から確保
    ALLOC_BOTTOM,             // メモリの下側から確保
  };

public:
  // コンストラクタ・デストラクタ
  MemoryControl( heap::NwAllocator* deviceAllocator, u32 vramASize, u32 vramBSize );
  ~MemoryControl();

  // アプリケーションメモリ
  void InitializeApplication( heap::NwAllocator* deviceAllocator );
  void FinalizeApplication( void );
  bool IsInitializeApplication(void) const;

  // メモリアロケータ
  void* Alloc( MemoryArea area, MemoryUseType type, u32 size );
  void Free( MemoryArea area, void* addr );

  //　コントロールタイプ変更
  void SetControlType( ControlType type, AllocWay way );
  ControlType GetControlType(void) const;
  AllocWay GetAllocWay(void) const;

  // VRAM残りサイズの取得
  u32 GetVramAFreeSize( void ) const ;
  u32 GetVramBFreeSize( void ) const ;


private:

  ControlType           m_ControlType;  // 動作タイプ
  AllocWay              m_AllocWay;     // 確保方向
  heap::NwAllocator*    m_UseDeviceAllocator;   // 動作タイプのアロケータ
  u32                   m_UseVramAStartBlock;   // 動作タイプのVRAM-A　確保ブロック開始位置
  u32                   m_UseVramANumBlock;     // 動作タイプのVRAM-A　確保ブロック数
  u32                   m_UseVramBStartBlock;   // 動作タイプのVRAM-B　確保ブロック開始位置
  u32                   m_UseVramBNumBlock;     // 動作タイプのVRAM-B　確保ブロック数


  heap::NwAllocator*    m_SystemDeviceAllocator;  // システム デバイスメモリアロケータ
  heap::NwAllocator*    m_ApplyDeviceAllocator;   // アプリ　デバイスメモリアロケータ


  gfl::base::AreaMan    m_AreaManVramA;
  gfl::base::AreaMan    m_AreaManVramB;

  gfl::base::AreaMan::ResultCode m_AllocTblVramA[VRAM_ALLOCK_BLOCK_NUM];
  gfl::base::AreaMan::ResultCode m_AllocTblVramB[VRAM_ALLOCK_BLOCK_NUM];

  // システムメモリーが使用するVRAMサイズ
  u32                   m_SystemVramASize;
  u32                   m_SystemVramBSize;

#if GFL_DEBUG
  s32 m_countDevice;
  s32 m_countVramA;
  s32 m_countVramB;
#endif


private:
  
  gfl::base::AreaMan::ResultCode* GetClearAllocTbl( gfl::base::AreaMan::ResultCode* p_tbl ) const;
  gfl::base::AreaMan::ResultCode* GetAddrAllocTbl( gfl::base::AreaMan::ResultCode* p_tbl, uptr addr ) const;


  // ツール系メンバ
  static u32 GetBlockAlign( u32 align );
  static u32 GetBlockSize( u32 size );

  // ControlTypeのUseメモリを設定
  void SetUseMemory( ControlType control_type );
};

// 実態の宣言
MemoryControl* GraphicsSystem::m_memoryControl;

//-----------------------------------------------------------------------------
/**
 *					バッファコントロールクラス定義
*/
//-----------------------------------------------------------------------------
class BufferControl
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(BufferControl);

public:

  // コンストラクタ・デストラクタ
  BufferControl( heap::NwAllocator* heapAllocator );
  ~BufferControl();

  void Initialize( heap::NwAllocator* heapAllocator, const GraphicsSystemSetUp* setup, bool appUpperRightMode );
  void Finalize( void );

  // CretaeAppRightBufferとDeleteAppRightBufferは、
  void CreateAppRightBuffer( heap::NwAllocator* deviceAllocator, const GraphicsSystemSetUp* setup, GraphicsSystem* p_graphics );
  void DeleteAppRightBufferReq( void );
  bool IsFinishDeleteAppRightBuffer( void ) const;
  bool IsCreateAppRightBuffer( void ) const;

  void StartRendering( DisplayMsk msk, const FrameBufferClearColor* clearColor, const FrameBufferClearFlag* clearFlag = NULL );
  void EndRendering( f32 parallaxDepthRange = gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ONE );
  void EndRenderingEx( DisplayType first, DisplayType second, DisplayType third,
                       f32 parallaxDepthRange = gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ONE );

  void SwapBuffer( void );
  void VSyncAfter( void );
  void VSyncAfterRunAsync( bool skip );

  DisplayBufferID GetCurrentDisplayBufferID( DisplayType display )const;
  FrameBufferID GetCurrentFrameBufferID( DisplayType display )const;
  RenderTarget* GetCurrentFrameBuffer( DisplayType display )const;
  CommandListID GetCurrentCommandListID( DisplayType display )const;
  CommandListID GetCurrentSaveCommandListID(void)const;

  bool BindCurrentCommandList( DisplayType display ) const;

  void StartCommandSave( void );
  void EndCommandSave( void );
  void ReuseCommandSave( void );

  void WaitDone( void );

  void RequestReadPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, u32 mode, RenderTarget::RenderBufferType type = RenderTarget::RENDERBUFFER_TYPE_COLOR );
  void RequestReadLinerPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, RenderTarget::RenderBufferType type = RenderTarget::RENDERBUFFER_TYPE_COLOR );
  void AddReadPixelsCommand( DisplayType display, DisplayType copy_display );
  void UpdateReadPixels( void );
  bool WaitReadPixels( void ) const;

  // スワップバッファフラグの取得とクリア
  bool IsSwapBuffer( void ) const { return m_swapBufferFlag; }
  void ClearSwapBufferFlag( void );

  // シングルバッファフォースフラグ
  void ReqSingleBufferForce( bool flag );

  // 立体視ＯＮ・ＯＦＦリクエスト
  void SetStereoModeReq( bool stereo_on );
  

  //各マネージャの取得
  inline DisplayBufferManager* GetDisplayBufferManager(void)const { return m_displayBufferManager; }
  inline FrameBufferManager* GetFrameBufferManager(void)const { return m_frameBufferManager; }
  inline CommandListManager* GetCommandListManager(void)const { return m_commandListManager; }

  // コマンドリスト実行回数
  inline u32 GetDoneCount( void ) const { return m_doneCount; }


  inline bool IsRenderEnd( DisplayType type, f32 parallaxDepthRange )
  {
    if( type != gfl::grp::DISPLAY_UPPER_RIGHT ){ return true; }
    if( !m_appCreateRightBuffer ){ return false; }
    if( m_appRightBufferDeleteReq != DEL_REQ_NONE ){ return false; }
    if( parallaxDepthRange == gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ZERO ){ return false; }
    return true;
  }

  inline bool IsRenderRightNone( DisplayType type, f32 parallaxDepthRange )
  {
    if( type != gfl::grp::DISPLAY_UPPER_RIGHT ){ return false; }
    if( !m_appCreateRightBuffer ){ return true; }
    if( m_appRightBufferDeleteReq != DEL_REQ_NONE ){ return true; }
    if( m_reqChangeStereoFlag ){ return true; }
    if( parallaxDepthRange == gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ZERO ){ return true; }
    return false;
  }

#if GFL_DEBUG

  void StartGpuProfile( void );
  void StopGpuProfile( void );
  u32 GetGpuProfileResult( GpuProfileResult result ) const;
  const u32* GetGpuProfileResultPtr( GpuProfileResult result ) const;

  inline void SetGpuProfileFlag( bool flag ) { m_gpuProfileFlag = flag; }
  inline bool IsGpuProfileFlag( void ) const { return m_gpuProfileFlag; }

#endif // GFL_DEBUG

private:

  enum{
    DEL_REQ_NONE,
    DEL_REQ_RENDERING_OFF,
    DEL_REQ_RENDERING_END_WAIT,
    DEL_REQ_RENDERING_END,
  };

private:

  // doubleバッファリングモードフラグ
  b8                  m_doubleBufferFlag;
  // スワップバッファフラグ
  b8                  m_swapBufferFlag;

  // 強制シングルバッファリングモードフラグ
  b8                  m_singleBufferForceFlag:2;
  b8                  m_reqSingleBufferForce:2;
  b8                  m_reqSingleBufferForceFlag:4;

  // マネージャ
  DisplayBufferManager* m_displayBufferManager;     // ディスプレイバッファマネージャ
  FrameBufferManager*   m_frameBufferManager;       // フレームバッファマネージャ
  CommandListManager*   m_commandListManager;       // コマンドリストマネージャ

  // システム保持バッファ
  DisplayBufferID       m_displayBufferTbl[GRAPHICSSYSTEM_USE_DISPLAYBUFFER_NUM];
  FrameBufferID         m_frameBufferTbl[DISPLAY_BOTH_MAX]; // 左、下、右のフレームバッファID
  CommandListID         m_commandListTbl[GRAPHICSSYSTEM_USE_COMMANDLIST_NUM];
  CommandListID         m_saveCommandListTbl[GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM];
  u32                   m_useDisplayBufferNum;
  u32                   m_useFrameBufferNum;
  u32                   m_useCommandListNum;
  u32                   m_useSaveCommandListNum;
  u32                   m_useDisplayNum;          // 用意されているディスプレイの数
  b8                    m_useRightDisplay;        // 右目を使用するとして用意されているのか
  b8                    m_appCreateRightBuffer;   // 右目用バッファをアプリケーションが作成した状態のとき,TRUE
  u8                    m_appRightBufferDeleteReq;

  // レンダリング管理
  b8                    m_renderFlag;             // レンダリング期間を管理するフラグ
  DisplayMsk            m_renderMsk;              // ディスプレイマスク

  // スワップバッファ管理
  u32                   m_swapDispCount;                      // スワップカウンタ
  u32                   m_swapCommandCount;                    // スワップカウンタ
  CommandListID         m_curentCommandList[COMMANDLIST_NUM]; // カレントコマンドリスト
  CommandListID         m_curentSaveCommandList;              // カレントセーブコマンドリスト
  DisplayBufferID       m_curentDisplay[DISPLAY_BOTH_MAX];    // カレントディスプレイバッファ
  DisplayBufferID       m_rendEndDisplay[DISPLAY_BOTH_MAX];   // レンダリング終了したディスプレイバッファ
  DisplayBufferID       m_swapDisplay[DISPLAY_BOTH_MAX];      // スワップディスプレイバッファ
  CommandListID         m_swapCommandList;                    // スワップコマンドリスト

  // コマンドリスト実行回数
  u32                   m_doneCount;

  // StereoMode変更Request
  bool  m_reqChangeStereoFlag;
  bool  m_reqChangeStereoMode;


  // ReadPixelsRequest
  u32                             m_reqReadPixcels;
  RenderTarget::RenderBufferType  m_reqReadPixcelsTypeTop;
  RenderTarget::RenderBufferType  m_reqReadPixcelsTypeTopRight;
  RenderTarget::RenderBufferType  m_reqReadPixcelsTypeBottom;
  void*                           m_reqReadPixcelsMemoryTop;
  void*                           m_reqReadPixcelsMemoryTopRight;
  void*                           m_reqReadPixcelsMemoryBottom;



#if GFL_DEBUG

  // スワップバッファフラグ
  bool m_gpuProfileFlag;
  bool m_gpuProfileRun;

  GLuint m_gpuProfileResultBusy[4];               // BUSY
  GLuint m_gpuProfileResultVertexShader[4][5];    // VertexShader
  GLuint m_gpuProfileResultPolygon[3];            // Polygon
  GLuint m_gpuProfileResultFragment;              // Fragment
  GLuint m_gpuProfileResultMemAccess[18];         // メモリアクセス

  u32     m_gpuProfileResultWork[ GPU_PROFILE_RESULT_MAX ]; // 前回の結果格納先
  u32     m_gpuProfileResultLastWork[ GPU_PROFILE_RESULT_MAX ]; // 前回の取得結果

#endif // GFL_DEBUG

private:

  void SetCurrentSwapCountBuffer(void);
  void ClearCurrentCommandList( void );
  void SetSwapTargetRenderDisplay(void);
  void ClearSwapID(void);
  void ClearSwapCommandID(void);
  void ClearRenderEndID(void);
  void Swap(void);

  void DeleteAppRightBuffer( void );

  void createDisplayBuffer( gfl::heap::NwAllocator* heapAllocator, u32 index, bool double_buffer, const DisplayBufferSetUp* cp_setup );
  void deleteDisplayBuffer( u32 index );

  void createFrameBuffer( gfl::heap::NwAllocator* heapAllocator, const RenderBufferSetUp* cp_setup );
  void deleteFrameBuffer( u32 index );


  void createCommandList( gfl::heap::NwAllocator* heapAllocator, u32 index, bool double_buffer, const GraphicsSystemSetUp* setup );
  void deleteCommandList( u32 index );

};


//=============================================================================
/**
 *					グラフィックスシステム実装
*/
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *    @brief  デフォルトセットアップデータを使用した生成
 *
 *    @param  heapAllocator       ヒープメモリアロケータ
 *    @param  deviceAllocator     デバイスメモリアロケータ
 *    @param  systemVramASize      システムが使用するVRAM-Aサイズ
 *    @param  systemVramBSize      システムが使用するVRAM-Bサイズ
 */
//-----------------------------------------------------------------------------
GraphicsSystem::GraphicsSystem( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator, u32 systemVramASize, u32 systemVramBSize )
{
  Initialize( heapAllocator, deviceAllocator, systemVramASize, systemVramBSize, &GraphicsSystemSetupDefault, false );
}

//-----------------------------------------------------------------------------
/**
 *    @brief  指定セットアップデータを使用した生成
 *
 *    @param  heapAllocator       ヒープメモリアロケータ
 *    @param  deviceAllocator     デバイスメモリアロケータ
 *    @param  setup               セットアップ情報
 *    @param  systemVramASize      システムが使用するVRAM-Aサイズ
 *    @param  systemVramBSize      システムが使用するVRAM-Bサイズ
 */
//-----------------------------------------------------------------------------
GraphicsSystem::GraphicsSystem( heap::NwAllocator* allocator, heap::NwAllocator* deviceAllocator, const GraphicsSystemSetUp* setup, u32 systemVramASize, u32 systemVramBSize )
{
  Initialize( allocator, deviceAllocator, systemVramASize, systemVramBSize, setup, false );
}

//-----------------------------------------------------------------------------
/**
 *    @brief  指定セットアップデータを使用した生成
 *
 *    @param  heapAllocator       ヒープメモリアロケータ
 *    @param  deviceAllocator     デバイスメモリアロケータ
 *    @param  setup               セットアップ情報
 *    @param  appUpperRightMode   右目のバッファを生成する。　TRUE
 *    @param  systemVramASize      システムが使用するVRAM-Aサイズ
 *    @param  systemVramBSize      システムが使用するVRAM-Bサイズ
 */
//-----------------------------------------------------------------------------
GraphicsSystem::GraphicsSystem( heap::NwAllocator* allocator, heap::NwAllocator* deviceAllocator, const GraphicsSystemSetUp* setup, bool appUpperRightMode, u32 systemVramASize, u32 systemVramBSize )
{
  Initialize( allocator, deviceAllocator, systemVramASize, systemVramBSize, setup, appUpperRightMode );
}


//-----------------------------------------------------------------------------
/**
 *    @brief  デストラクタ
 */
//-----------------------------------------------------------------------------
GraphicsSystem::~GraphicsSystem()
{
  Finalize();
}


//-----------------------------------------------------------------------------
/**
 *    @brief  グラフィックスシステムが初期化ずみかチェック
 *
 *    @retval true  初期化済み
 *    @retval false 未初期化
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsInitialize( void )
{
  return m_Initialize;
}


//-----------------------------------------------------------------------------
/**
 *    @brief  ゲームとして、立体視を使用するのかの設定
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::SetGameStereoFlag( bool flag )
{
  m_GameStereoFlag = flag;
}

bool GraphicsSystem::IsGameStereoFlag( void )
{
  return m_GameStereoFlag;
}


//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param	allocator           ヒープアロケータ
 *	@param	deviceAllocator     デバイスアロケータ
 *	@param  systemVramASize     VRAM-Aサイズ
 *	@param  systemVramBSize     VRAM-Bサイズ
 *	@param	setup               セットアップデータ
 *	@param  appUpperRight       右目ディスプレイバッファを強制的に確保するか？
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::Initialize( heap::NwAllocator* allocator, heap::NwAllocator* deviceAllocator, u32 systemVramASize, u32 systemVramBSize, const GraphicsSystemSetUp* setup, bool appUpperRight )
{
  heap::HeapBase* heap = allocator->GetHeapBase();

  if(m_memoryControl != NULL){
    GFL_ASSERT( m_memoryControl == NULL );
    Finalize();
  }

  // アロケータの保存
  m_heapAllocator = allocator;
  m_deviceAllocator = deviceAllocator;

  // SetUpデータ保存
  std::MemCopy( setup, &m_setup, sizeof(GraphicsSystemSetUp) );

  // メモリマネージャの生成
  m_memoryControl = GFL_NEW(heap) MemoryControl( deviceAllocator, systemVramASize, systemVramBSize );

  // gx
  if (nngxInitialize(AllocateGraphicsMemory, DeallocateGraphicsMemory) == GL_FALSE)
  {
    // GxInitialize 失敗
    GFL_ASSERT_STOP(0);
  }


  // バッファコントローラー生成
  m_bufferControl = GFL_NEW(heap) BufferControl( allocator );

  // バッファコントローラーの初期化は、メモリの上から取る
  m_memoryControl->SetControlType( MemoryControl::SYSTEM_TYPE, MemoryControl::ALLOC_TOP );
  {
    m_bufferControl->Initialize( allocator, setup, appUpperRight );
  }
  m_memoryControl->SetControlType( MemoryControl::SYSTEM_TYPE, MemoryControl::ALLOC_TOP );


  // 立体視ON
  if( setup->stereoOn ){
    nngxSetDisplayMode( NN_GX_DISPLAYMODE_STEREO );
  }else{
    nngxSetDisplayMode( NN_GX_DISPLAYMODE_NORMAL );
  }

  // 初期化時に
  // VRAMの全クリアを行う。　ソフトウェアリセット対策
  {
    gfl::grp::CommandListManager * comListMan = this->GetCommandListManager();
    gfl::grp::CommandListID comListID = this->GetCurrentCommandListID();
    {
      comListMan->Clear( comListID );
      comListMan->Bind( comListID );
      {
        // VRAMをクリアするコマンドをつむ
        nngxAddMemoryFillCommand( 
            reinterpret_cast<GLvoid*>(nngxGetVramStartAddr(NN_GX_MEM_VRAMA)), nngxGetVramSize(NN_GX_MEM_VRAMA), 0, 32,
            reinterpret_cast<GLvoid*>(nngxGetVramStartAddr(NN_GX_MEM_VRAMB)), nngxGetVramSize(NN_GX_MEM_VRAMB), 0, 32 );
        comListMan->RunAsync( comListID );
        comListMan->WaitDone();
        comListMan->Clear( comListID );
      }
    }
  }

  // VsyncWait
  nngxSwapBuffers(NN_GX_DISPLAY_BOTH);
  nngxWaitVSync(NN_GX_DISPLAY_BOTH);
  nngxStartLcdDisplay();



  //　初期化
  m_Initialize = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  削除処理
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::Finalize( void )
{
  // m_memoryControlが最後に破棄されるのでこれでチェック
  if( m_memoryControl != NULL ){

    //　初期化
    m_Initialize = false;

    // バッファコントロー破棄
    GFL_DELETE m_bufferControl;
    m_bufferControl = NULL;

    //VSyncWait
    nngxWaitVSync(NN_GX_DISPLAY_BOTH);

    // nngx終了
    nngxFinalize();

    // メモリマネージャ破棄
    GFL_DELETE m_memoryControl;
    m_memoryControl = NULL;


    m_heapAllocator = NULL;
    m_deviceAllocator = NULL;

    //GFL_PRINT( "nngxFinalize\n" );
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  画面構成を変更する。
 *
 *	@param	setup 変更する画面構成のセットアップ情報
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::ChangeSetUp( const GraphicsSystemSetUp* setup, bool appUpperRight )
{
  MemoryControl::ControlType type = m_memoryControl->GetControlType();
  MemoryControl::AllocWay    way = m_memoryControl->GetAllocWay();

  // Systemバッファから確保
  // 断片化対策として、下からとる。
  m_memoryControl->SetControlType( MemoryControl::SYSTEM_TYPE, MemoryControl::ALLOC_BOTTOM );
  {
    // メモリ破棄
    m_bufferControl->Finalize();

    m_heapAllocator->SetIsAllocLow( true );
    m_bufferControl->Initialize( m_heapAllocator, setup, appUpperRight );
    m_heapAllocator->SetIsAllocLow( false );
  }
  // メモリコントロールを元に戻す。
  m_memoryControl->SetControlType( type, way );

  // SetUpデータ保存
  std::MemCopy( setup, &m_setup, sizeof(GraphicsSystemSetUp) );

  // 立体視ON
  if( setup->stereoOn ){
    nngxSetDisplayMode( NN_GX_DISPLAYMODE_STEREO );
  }else{
    nngxSetDisplayMode( NN_GX_DISPLAYMODE_NORMAL );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーション用のデバイスメモリ、VRAM管理を初期化
 *
 *	@param	deviceAllocator           アプリケーションのグラフィックで使用するデバイスメモリのアロケータ
 *
 *	VRAM領域には、SystemMemoryで予約されている部分以外の全てが割り当てられます。
 *
 *	この関数を呼ぶと、アプリケーションメモリに切り替わります。
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::InitializeApplicationMemory( heap::NwAllocator* deviceAllocator )
{
  m_memoryControl->InitializeApplication( deviceAllocator );
}


//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーション用のデバイスメモリ、VRAM管理を破棄
 *
 *	VRAMに解放忘れがあるとAssartでとまります。
 *
 *	これ以降で、確保されるメモリは、システムメモリから確保されます。
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::FinalizeApplicationMemory( void )
{
  m_memoryControl->FinalizeApplication();
}

//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーション側で、拡張した立体視バッファがあるかチェック
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsInitializedAppUpperRightBuffer( void ) const
{
  return m_bufferControl->IsCreateAppRightBuffer();
}

//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーション側で、拡張で、立体視をＯＮにする。
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::InitializeAppUpperRightBuffer( heap::NwAllocator* deviceAllocator )
{
  // バファコントロールに拡張の立体視バッファを生成
  m_bufferControl->CreateAppRightBuffer( deviceAllocator, &m_setup, this );
  this->BindCurrentCommandList();
}

//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーション側で、拡張で確保したバッファを解放する。
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::ReqDeleteAppUpperRightBuffer( bool is_stereo_off )
{
  // 念のため、立体視をＯＦＦに
  if( is_stereo_off ){
    SetStereoModeReq(false);
  }
  
  // リクエスト発行
  m_bufferControl->DeleteAppRightBufferReq();
}

//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーション側で、拡張で確保したバッファが解放されたかをチェック
 *	@return
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsDeleteAppUpperRightBuffer( void ) const
{
  return m_bufferControl->IsFinishDeleteAppRightBuffer();
}



//----------------------------------------------------------------------------
/**
 *	@brief  システム内のメモリ確保先をシステムメモリに切り替える。
 *
 *	@param  mode    システムメモリのモード
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::BindSystemMemory( SystemMemoryMode mode )
{
  if( mode == SYSTEM_MEM_UNSETTLED ){

    // 変動のあるメモリ確保
    m_memoryControl->SetControlType( MemoryControl::SYSTEM_TYPE, MemoryControl::ALLOC_BOTTOM );

  }else{

    // 変動のないメモリ確保
    m_memoryControl->SetControlType( MemoryControl::SYSTEM_TYPE, MemoryControl::ALLOC_TOP );

  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  システム内のメモリ確保先をアプリケーションメモリに戻す。
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::UnBindSystemMemory( void )
{
  if( m_memoryControl->IsInitializeApplication() ){

    // アプリケーションメモリを指定
    m_memoryControl->SetControlType( MemoryControl::APPLICATION_TYPE, MemoryControl::ALLOC_TOP );
  }else{

    // 変動のないメモリとして初期化
    m_memoryControl->SetControlType( MemoryControl::SYSTEM_TYPE, MemoryControl::ALLOC_TOP );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  メモリ確保先がシステムメモリになっているか確認する。
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsBindSystemMemory(void) const
{
  if( m_memoryControl->GetControlType() == MemoryControl::SYSTEM_TYPE ){
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------
/**
 *	@brief  立体視のON/OFFをリクエストします。 実際の反映は、VSyncAfterになります。
 *
 *	@param	stereo_on   ON:true   OFF:false
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::SetStereoModeReq( bool stereo_on )
{
  if( IsUseRightDisplay() ){
    m_bufferControl->SetStereoModeReq(stereo_on);
  }
}

#if GFL_DEBUG
//立体視が有効の時しかモードが切り替えられないのでデバッグ用に用意
void GraphicsSystem::SetStereoModeReqDebug( bool stereo_on )
{
  m_bufferControl->SetStereoModeReq(stereo_on);
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief  シングルバッファーにするフラグ
 *
 *	@param	single_buffer_on   ON:true   OFF:false
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::SetSingleBufferReq( bool single_buffer_on )
{
  m_bufferControl->ReqSingleBufferForce( single_buffer_on );
}


//----------------------------------------------------------------------------
/**
 *	@brief    通常レンダリング開始処理
 *
 *	@param  msk   描画を行う面設定
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::StartRendering( DisplayMsk msk )
{
  m_bufferControl->StartRendering( msk, m_setup.clearColor, m_setup.clearFlag );
}


//----------------------------------------------------------------------------
/**
 *	@brief    レンダリング後処理
 *
 *	@param    parallaxDepthRange  立体視の視差の補正値
 *	                               (gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_か
 *                                  gfl::grp::g3d::StereoCameraのGetDepthRange関数の戻り値を渡して下さい。)
 *
 *	＊スワップするバッファIDの保存などを行います。
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::EndRendering( f32 parallaxDepthRange )
{
  m_bufferControl->EndRendering( parallaxDepthRange );
}



//----------------------------------------------------------------------------
/**
 *	@brief  各画面のコマンドバッファの実行順序指定付き　レンダリング後処理
 *
 *  @param  first     1番にコマンド実行をする面
 *  @param  second    2番にコマンド実行をする面
 *  @param  third     3番にコマンド実行をする面
 *	@param  parallaxDepthRange  立体視の視差の補正値
 *	                             (gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_か
 *                                gfl::grp::g3d::StereoCameraのGetDepthRange関数の戻り値を渡して下さい。)
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::EndRenderingEx( DisplayType first, DisplayType second, DisplayType third, f32 parallaxDepthRange )
{
  m_bufferControl->EndRenderingEx( first, second, third, parallaxDepthRange );
}


//----------------------------------------------------------------------------
/**
 *	@brief    スワップバッファー処理
 *
 *	この関数を行った後、直ぐにVSyncWaitを行ってください。
 *
 *	■ダブルバッファリングモードの場合
 *	  １．前フレームで開始したの実描画処理の完了を待ちます。
 *	  ２．前のフレームで描画した面をLODに反映します。
 *
 *	■シングルバッファリングモードの場合
 *	　１．今フレームで生成したコマンドを実行します。
 *	　２．描画の完了を待ちます。
 *	　３．描画結果をLODに反映します。
 *
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::SwapBuffer( void )
{
  m_bufferControl->SwapBuffer();
#if GFL_DEBUG  
  //描画が完了し次のフレームということになる。
  //前フレームの描画インスタンスは破棄できる。
  ++s_DrawFrameCount;
#endif    
}


//----------------------------------------------------------------------------
/**
 *	@brief  コマンドリストの実行が完了するまで待つ
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::WaitDone( void )
{
  m_bufferControl->WaitDone();
}


//----------------------------------------------------------------------------
/**
 *	@brief    VSync待ち　処理
 *	@param  display   ディスプレイタイプ  （VSyncを待つ画面 DISPLAY_UPPER or DISPLAY_LOWER or DISPLAY_BOTH_MAX ）
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::WaitVSync( DisplayType display )
{
  static const u32 sc_NngxDisplay[ DISPLAY_BOTH_MAX+1 ] = {
    NN_GX_DISPLAY0,
    NN_GX_DISPLAY1,
    NN_GX_DISPLAY0,
    NN_GX_DISPLAY_BOTH,
  };

  if( display <= DISPLAY_BOTH_MAX )
  {

    nngxWaitVSync( sc_NngxDisplay[display] );
  }
  else
  {
    GFL_ASSERT(0);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  スワップバッファ　リクエストが出ているかチェック
 *
 *	@retval true  出ている。    （WaitVSyncしてからVSyncAfterを呼ぶ必要がある。）
 *	@retval false 出ていない。
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsSwapBuffer( void ) const
{
  return m_bufferControl->IsSwapBuffer();
}

//----------------------------------------------------------------------------
/**
 *	@brief    VSync後処理
 *
 *	この関数を呼ぶ前に必ずVSyncWaitを行ってください。
 *
 *	■ダブルバッファリングモードの場合
 *	  ３．今フレームで生成した3Dコマンドを実行します。
 *	  ４．ディスプレイバッファ、コマンドリストの書き込み先を変更します。
 *
 *	■シングルバッファリングモードの場合
 *	　特になし
 *
 *	@param  skip    今回のコマンド実行をスキップ
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::VSyncAfter( bool skip )
{
  // スワップバッファが行われた場合に行う処理
  if( m_bufferControl->IsSwapBuffer() ){

    // 実際のコマンド実行開始処理
    m_bufferControl->VSyncAfter();

    // キャプチャ後に行いたい、コマンド実行
    m_bufferControl->VSyncAfterRunAsync( skip );

    // スワップバッファフラグのクリア
    m_bufferControl->ClearSwapBufferFlag();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ディスプレイのクリア命令を発行する。
 *
 *	@param	display       ディスプレイ
 *	@param  b_color       カラーのクリアを行うか？(true:行う）
 *	@param  clear_color   クリアカラー
 *	@param  b_depth       デプスのクリアを行うか？(true:行う）
 *	@param  clear_depth   クリアデプス値
 *	@param  b_stencil     ステンシルのクリアを行うか？(true:行う)
 *	@param  clear_stencil クリアステンシル値
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::ClearBuffer( DisplayType display, b32 b_color, const gfl::grp::ColorF32& clear_color, b32 b_depth, f32 clear_depth, b32 b_stencil, u8 clear_stencil )
{
  RenderTarget* p_render = this->GetCurrentFrameBuffer( display );

  // クリアバッファを行う。
  if( this->BindCurrentCommandList(p_render) ){
    p_render->ClearBuffer( b_color, clear_color, b_depth, clear_depth, b_stencil, clear_stencil );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief    システムが操作しているカレントのディスプレイバッファIDを取得
 *
 *	@param    display   面指定
 *
 *	@return   カレントのディスプレイバッファID
 */
//-----------------------------------------------------------------------------
DisplayBufferID GraphicsSystem::GetCurrentDisplayBufferID( DisplayType display )const
{
  return m_bufferControl->GetCurrentDisplayBufferID(display);
}

//----------------------------------------------------------------------------
/**
 *	@brief    システムが操作しているカレントのフレームバッファIDを取得
 *
 *	@param    display   面指定
 *
 *	@return   カレントのフレームバッファID
 */
//-----------------------------------------------------------------------------
FrameBufferID GraphicsSystem::GetCurrentFrameBufferID( DisplayType display )const
{
  return m_bufferControl->GetCurrentFrameBufferID( display );
}

//----------------------------------------------------------------------------
/**
 *	@brief    システムが操作しているカレントのフレームバッファを取得
 *
 *	@param    display   面指定
 *
 *	@return   カレントのフレームバッファ
 */
//-----------------------------------------------------------------------------
RenderTarget* GraphicsSystem::GetCurrentFrameBuffer( DisplayType display )const
{
  return m_bufferControl->GetCurrentFrameBuffer(display);
}


//----------------------------------------------------------------------------
/**
 *	@brief    システムが操作しているカレントのコマンドリストIDを取得
 *
 *	@param  displayType 　取得したいコマンドリストに対応した面
 *
 *	@return   カレントのコマンドリストID
 *
 *	displayType
 *	  DISPLAY_UPPER           左目 or 上画面用のコマンドリスト
 *	  DISPLAY_UPPER_RIGHT     右目用のコマンドリスト
 *	  DISPLAY_LOWER           下画面用のコマンドリスト
 *	  DISPLAY_NONE            オフスクリーン処理用のコマンドリスト（影など）
 */
//-----------------------------------------------------------------------------
CommandListID GraphicsSystem::GetCurrentCommandListID(DisplayType displayType)const
{
  return m_bufferControl->GetCurrentCommandListID(displayType);
}

//----------------------------------------------------------------------------
/**
 *	@brief    システムが操作しているカレントのコマンドリストをバインドする
 *
 *	@param	displayType バインドしたいコマンドリストに対応した面
 *
 *	displayType
 *	  DISPLAY_UPPER           左目 or 上画面用のコマンドリスト
 *	  DISPLAY_UPPER_RIGHT     右目用のコマンドリスト
 *	  DISPLAY_LOWER           下画面用のコマンドリスト
 *	  DISPLAY_NONE            オフスクリーン処理用のコマンドリスト（影など）
 *
 *	@retval true  コマンドリストがバインドできた。
 *	@retval false コマンドリストがバインドできなかった！(立体視じゃないときにUPPER_RIGHTが指定された）
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::BindCurrentCommandList( DisplayType displayType )
{
  return m_bufferControl->BindCurrentCommandList( displayType );
}

//----------------------------------------------------------------------------
/**
 *	@brief    レンダーターゲットと関連したコマンドバッファをバインドする
 *
 *	@param	renderTarget 　描画先にしたいレンダーターゲット
 *
 *	@retval true  コマンドリストがバインドできた。
 *	@retval false コマンドリストがバインドできなかった！(立体視じゃないときにUPPER_RIGHTが指定された）
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::BindCurrentCommandList( const gfl::grp::RenderTarget* renderTarget )
{
  if( renderTarget ){
    return m_bufferControl->BindCurrentCommandList( renderTarget->GetDisplayType() );
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief    システムが操作しているカレントのセーブ用コマンドリストIDを取得
 *
 *	@return   カレントのセーブ用コマンドリストID
 */
//-----------------------------------------------------------------------------
CommandListID GraphicsSystem::GetCurrentSaveCommandListID(void)const
{
  return m_bufferControl->GetCurrentSaveCommandListID();
}

//----------------------------------------------------------------------------
/**
 *	@brief    セーブ用コマンドリストへのコマンド蓄積開始
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::StartCommandSave( void )
{
  m_bufferControl->StartCommandSave();
}

//----------------------------------------------------------------------------
/**
 *	@brief    セーブ用コマンドリストへのコマンド蓄積終了
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::EndCommandSave( void )
{
  m_bufferControl->EndCommandSave();
}

//----------------------------------------------------------------------------
/**
 *	@brief    セーブ用コマンドリストへ蓄積された最新の内容をカレントコマンドリストに設定
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::ReuseCommandSave( void )
{
  m_bufferControl->ReuseCommandSave();
}



//----------------------------------------------------------------------------
/**
 *	@brief    デフォルトクリアカラーの設定
 *
 *  @param  display   面指定　（DISPLAY_UPPER or DISPLAY_LOWER）
 *	@param	red       R   0.0～1.0
 *	@param	green     G   0.0～1.0
 *	@param	blue      B   0.0～1.0
 *	@param	alpha     A   0.0～1.0
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::SetClearColor( DisplayType display, f32 red, f32 green, f32 blue, f32 alpha )
{
  if( display < DISPLAY_NORMAL_MAX )
  {
    m_setup.clearColor[display].red     = red;
    m_setup.clearColor[display].green   = green;
    m_setup.clearColor[display].blue    = blue;
    m_setup.clearColor[display].alpha   = alpha;
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//-----------------------------------------------------------------------------
/**
  * @brief     デフォルトクリアフラグの設定
  * @param display     面指定　（DISPLAY_UPPER or DISPLAY_LOWER）
  * @param color       カラーバッファクリア  有効　or 無効
  * @param depth       深度バッファクリア  有効　or 無効
  * @param stencil     ステンシルバッファクリア  有効　or 無効
  */
//-----------------------------------------------------------------------------
void GraphicsSystem::SetClearColorFlag( DisplayType display, b32 color, f32 depth, f32 stencil )
{
  if( display < DISPLAY_NORMAL_MAX )
  {
    m_setup.clearFlag[display].f_color     = color;
    m_setup.clearFlag[display].f_depth     = depth;
    m_setup.clearFlag[display].f_stencil   = stencil;
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    ディスプレイバッファマネージャの取得
 */
//-----------------------------------------------------------------------------
DisplayBufferManager* GraphicsSystem::GetDisplayBufferManager( void )const
{
  return m_bufferControl->GetDisplayBufferManager();
}

//----------------------------------------------------------------------------
/**
 *	@brief    フレームバッファマネージャの取得
 */
//-----------------------------------------------------------------------------
FrameBufferManager* GraphicsSystem::GetFrameBufferManager( void )const
{
  return m_bufferControl->GetFrameBufferManager();
}

//----------------------------------------------------------------------------
/**
 *	@brief    コマンドリストマネージャの取得
 */
//-----------------------------------------------------------------------------
CommandListManager* GraphicsSystem::GetCommandListManager( void )const
{
  return m_bufferControl->GetCommandListManager();
}

//----------------------------------------------------------------------------
/**
 *	@brief  コマンドリスト実行回数（回り込み注意！）
 *
 *	@return コマンドリスト実行回数
 */
//-----------------------------------------------------------------------------
u32 GraphicsSystem::GetCommandListDoneCount( void ) const
{
  return m_bufferControl->GetDoneCount();
}

//----------------------------------------------------------------------------
/**
 *	@brief    ReadPixelsに必要なメモリサイズを取得する。
 */
//-----------------------------------------------------------------------------
u32 GraphicsSystem::GetReadPixelsMemorySize( RenderColorFormat colorFormat, u32 width, u32 height ) const
{
  u32 memorySize = 0;

  switch( colorFormat )
  {
  case RENDER_COLOR_FORMAT_NONE:
    memorySize = 0;
    break;
  case RENDER_COLOR_FORMAT_RGBA8:
    memorySize = 4;
    break;
  case RENDER_COLOR_FORMAT_RGB8:
    memorySize = 3;
    break;
  case RENDER_COLOR_FORMAT_RGBA4:
    memorySize = 2;
    break;
  case RENDER_COLOR_FORMAT_RGB5_A1:
    memorySize = 2;
    break;
  case RENDER_COLOR_FORMAT_RGB565:
    memorySize = 2;
    break;
  default:
    GFL_ASSERT_MSG( 0, "colorFormat does not exist" );
  }

  return ( width * height * memorySize );
}


//----------------------------------------------------------------------------
/**
 *	@brief    カラーバッファのピクセル情報の取得Request
 *
 *	@param  memoryTop     上画面左格納先　（不要な場合NULL）
 *  @param  memoryTopRight上画面右格納先　（不要な場合NULL）
 *	@param  memoryBottom  下画面格納先    （不要な場合NULL）
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::RequestReadPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, RenderTarget::RenderBufferType type )

{
  m_bufferControl->RequestReadPixels( memoryTop, memoryTopRight, memoryBottom, READPIXCELS_REQ_ON_BLOCK, type );
}
// Up と Bottomのみのとき
void GraphicsSystem::RequestReadPixels( void* memoryTop, void* memoryBottom, RenderTarget::RenderBufferType type )

{
  m_bufferControl->RequestReadPixels( memoryTop, NULL, memoryBottom, READPIXCELS_REQ_ON_BLOCK, type );
}

//リニアモード
void GraphicsSystem::RequestReadLinerPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, RenderTarget::RenderBufferType type )

{
  m_bufferControl->RequestReadPixels( memoryTop, memoryTopRight, memoryBottom, READPIXCELS_REQ_ON_LINER, type );
}
// Up と Bottomのみのとき
void GraphicsSystem::RequestReadLinerPixels( void* memoryTop, void* memoryBottom, RenderTarget::RenderBufferType type )

{
  m_bufferControl->RequestReadPixels( memoryTop, NULL, memoryBottom, READPIXCELS_REQ_ON_LINER, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief    カラーバッファのピクセル情報の取得完了待ち
 *	@retval true    完了
 *	@retval false   途中
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::WaitReadPixels( void ) const
{
  return m_bufferControl->WaitReadPixels();
}



//----------------------------------------------------------------------------
/**
 *	@brief    VRAM-A残り領域を取得
 */
//-----------------------------------------------------------------------------
u32 GraphicsSystem::GetVramAAllocatableSize( void ) const
{
  return m_memoryControl->GetVramAFreeSize();
}

//----------------------------------------------------------------------------
/**
 *	@brief    VRAM-B残り領域を取得
 */
//-----------------------------------------------------------------------------
u32 GraphicsSystem::GetVramBAllocatableSize( void ) const
{
  return m_memoryControl->GetVramBFreeSize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  メモリ確保
 *
 *	@param	area  メモリエリア
 *	@param	type  使用タイプ
 *	@param	size  サイズ
 *
 *	@return メモリ
 */
//-----------------------------------------------------------------------------
void* GraphicsSystem::AllocMemory( MemoryArea area, MemoryUseType type, u32 size )
{
  return m_memoryControl->Alloc( area, type, size );
}

//----------------------------------------------------------------------------
/**
 *	@brief  メモリ解放
 *
 *	@param	area        エリア
 *	@param  p_memory    メモリ
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::FreeMemory( MemoryArea area, void* p_memory )
{
  if( m_memoryControl != NULL ){
    m_memoryControl->Free( area, p_memory );
  }
}


#if GFL_DEBUG
//----------------------------------------------------------------------------
/**
 *	@brief    コマンドリストの状態をダンプ
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::DumpCommandList(void)const
{
  GFL_PRINT( "COMMANDLIST MAIN\n" );
  if( m_bufferControl->BindCurrentCommandList( DISPLAY_NONE ) ){
    m_bufferControl->GetCommandListManager()->Dump();
  }

  GFL_PRINT( "COMMANDLIST LEFT\n" );
  if( m_bufferControl->BindCurrentCommandList( DISPLAY_UPPER ) ){
    m_bufferControl->GetCommandListManager()->Dump();
  }

  GFL_PRINT( "COMMANDLIST RIGHT\n" );
  if( m_bufferControl->BindCurrentCommandList( DISPLAY_UPPER_RIGHT ) ){
    m_bufferControl->GetCommandListManager()->Dump();
  }

  GFL_PRINT( "COMMANDLIST LOWER\n" );
  if( m_bufferControl->BindCurrentCommandList( DISPLAY_LOWER ) ){
    m_bufferControl->GetCommandListManager()->Dump();
  }

  GFL_PRINT( "SAVE COMMANDLIST" );
  m_bufferControl->GetCommandListManager()->Bind( m_bufferControl->GetCurrentSaveCommandListID() );
  m_bufferControl->GetCommandListManager()->Dump();


  // 元に戻す。
  m_bufferControl->BindCurrentCommandList( DISPLAY_NONE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  GpuProfileフラグを設定
 *
 *	@param	flag  フラグ
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::SetGpuProfileFlag( bool flag )
{
  m_bufferControl->SetGpuProfileFlag(flag);
}

//----------------------------------------------------------------------------
/**
 *	@brief  GpuProfileフラグを取得
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsGpuProfileFlag( void ) const
{
  return m_bufferControl->IsGpuProfileFlag();
}

//----------------------------------------------------------------------------
/**
 *	@brief  1つ前のレンダリングでのGPUプロファイリング結果取得
 *
 *	@param  GpuProfileResult
 *
 *	@return 結果の数値
 */
//-----------------------------------------------------------------------------
u32 GraphicsSystem::GetGpuProfileResult( GpuProfileResult result ) const
{
  return m_bufferControl->GetGpuProfileResult( result );
}

//----------------------------------------------------------------------------
/**
 *	@brief  1つ前のレンダリングでのGPUプロファイリング結果取得
 *
 *	@param  GpuProfileResult
 *
 *	@return 結果の数値
 */
//-----------------------------------------------------------------------------
const u32* GraphicsSystem::GetGpuProfileResultPtr( GpuProfileResult result ) const
{
  return m_bufferControl->GetGpuProfileResultPtr( result );
}

#endif



//----------------------------------------------------------------------------
/**
 *	@brief  グラフィックスメモリアロケータ
 *
 *	@param	area    メモリエリア
 *	@param	aim     使用用途
 *	@param	id      未使用
 *	@param	size    サイズ
 *
 *	@return アドレス
 */
//-----------------------------------------------------------------------------
void* GraphicsSystem::AllocateGraphicsMemory(GLenum area, GLenum aim, GLuint id, GLsizei size)
{
  GFL_UNUSED( id );

  if(m_memoryControl != NULL){
    return m_memoryControl->Alloc( static_cast<MemoryArea>(area), static_cast<MemoryUseType>(aim), static_cast<u32>(size) );
  }else{
    GFL_ASSERT( m_memoryControl != NULL ); //@check
    return NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  グラフィックスメモリデアロケータ
 *
 *	@param	area    メモリエリア
 *	@param	aim     使用用途
 *	@param	id      未使用
 *	@param	size    サイズ
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::DeallocateGraphicsMemory(GLenum area, GLenum aim, GLuint id, void* addr)
{
  GFL_UNUSED( id );
  GFL_UNUSED( aim );

  if( m_memoryControl != NULL ){
    m_memoryControl->Free( static_cast<MemoryArea>(area), addr );
  }else{
    GFL_ASSERT( m_memoryControl != NULL ); //@check
  }
}




// 
//----------------------------------------------------------------------------
/**
 *	@brief 立体視つかえる？
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsUseRightDisplay( void ) const
{
  if( (m_setup.useUpperRight || m_bufferControl->IsCreateAppRightBuffer()) && m_GameStereoFlag ){
    return true;
  }
  return false;
}

#if GFL_DEBUG  
u32   GraphicsSystem::s_DrawFrameCount = 0;

u32 GraphicsSystem::GetDrawFrameCount()
{
  return s_DrawFrameCount;
}
#endif

//=============================================================================
/**
 *					バッファコントロールクラス定義
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 *
 *	@param	heapAllocator           ヒープアロケータ
 *	@param	setup                   セットアップデータ
 */
//-----------------------------------------------------------------------------
BufferControl::BufferControl(heap::NwAllocator* heapAllocator )
{

  // 各種マネージャ作成
  m_displayBufferManager  = GFL_NEW(heapAllocator->GetHeapBase()) DisplayBufferManager( heapAllocator, GRAPHICSSYSTEM_USE_DISPLAYBUFFER_NUM );
  m_frameBufferManager    = GFL_NEW(heapAllocator->GetHeapBase()) FrameBufferManager( heapAllocator, GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM );
  m_commandListManager    = GFL_NEW(heapAllocator->GetHeapBase()) CommandListManager( heapAllocator, GRAPHICSSYSTEM_TOTAL_COMMANDLIST_MAX );

  m_doneCount = 0;


#if GFL_DEBUG
  m_gpuProfileRun = false;
  m_gpuProfileFlag = false;
  for(u32 i=0; i<GPU_PROFILE_RESULT_MAX; ++i){
    m_gpuProfileResultWork[i] = 0;
    m_gpuProfileResultLastWork[i] = 0;
  }
#endif // GFL_DEBUG
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
BufferControl::~BufferControl()
{
  this->Finalize();

  // 各種バッファ破棄
  GFL_DELETE m_commandListManager;
  m_commandListManager = NULL;
  GFL_DELETE m_frameBufferManager;
  m_frameBufferManager = NULL;
  GFL_DELETE m_displayBufferManager;
  m_displayBufferManager = NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  バッファコントロール　初期化
 *
 *  @param  heapAllocator ヒープアロケータ
 *	@param	setup         セットアップ
 */
//-----------------------------------------------------------------------------
void BufferControl::Initialize( heap::NwAllocator* heapAllocator, const GraphicsSystemSetUp* setup, bool appUpperRight )
{
  int i;


  // appUpperRightモードの場合。
  // m_useRightDisplay = false
  // m_useDisplayNum   = DISPLAY_BOTH_MAX
  // 右目ディスプレイバッファ　=　生成
  // 右目フレームバッファ      = 生成しない
  // 右目コマンドバッファ      = 生成しない

  // 右目使用するか？
  m_useRightDisplay = setup->useUpperRight;
  if( m_useRightDisplay || appUpperRight ){
    m_useDisplayNum = DISPLAY_BOTH_MAX;
  }else{
    m_useDisplayNum = DISPLAY_NORMAL_MAX;
  }

  m_singleBufferForceFlag = false;
  m_reqSingleBufferForce = false;

  // モードチェック
  if( setup->buffer_mode == MODE_DOUBLE_BUFFER ){
    m_doubleBufferFlag       = true;
    m_useDisplayBufferNum     = GRAPHICSSYSTEM_USE_DISPLAYBUFFER_NUM;
    m_useFrameBufferNum       = GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM;
    m_useCommandListNum       = GRAPHICSSYSTEM_USE_COMMANDLIST_NUM;
    m_useSaveCommandListNum   = GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM;
  }else{
    m_doubleBufferFlag       = false;
    m_useDisplayBufferNum     = GRAPHICSSYSTEM_USE_DISPLAYBUFFER_NUM/2;
    m_useFrameBufferNum       = GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM;
    m_useCommandListNum       = GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2;
    m_useSaveCommandListNum   = GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM/2;
  }


  // ディスプレイバッファ確保
  for( i=0; i<DISPLAY_BOTH_MAX; i++ ){

    // 生成の必要があるバッファかチェック
    if( m_useDisplayNum <= i ){

      m_displayBufferTbl[i] = DISPLAYBUFFER_ID_ERROR;
      m_displayBufferTbl[i+DISPLAY_BOTH_MAX] = DISPLAYBUFFER_ID_ERROR;

    }else{

      this->createDisplayBuffer( heapAllocator, i, m_doubleBufferFlag, &setup->displaySetup[i] );
    }
  }

  // 各画面用のフレームバッファの確保ワークの初期化
  for( i=0; i<GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM; i++ ){
    m_frameBufferTbl[i] = FRAMEBUFFER_ID_ERROR;
  }

  // 各画面用のフレームバッファの確保
  for( i=0; i<GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM; i++ ){
    // DisplayTypeの範囲チェック & 重複チェック
    GFL_ASSERT_STOP( setup->frameSetup[i].targetDisplay < DISPLAY_BOTH_MAX );
    GFL_ASSERT_STOP( m_frameBufferTbl[setup->frameSetup[i].targetDisplay] == FRAMEBUFFER_ID_ERROR );

    // 右目用が必要か？
    if( (m_useRightDisplay == false) && (setup->frameSetup[i].targetDisplay == DISPLAY_UPPER_RIGHT) ){
      continue;
    }
    this->createFrameBuffer( heapAllocator, &setup->frameSetup[i] );
  }

  // コマンドリスト
  {
    for( i=0; i<GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2; i++ ){

      // 右目用が必要か？
      if( (m_useRightDisplay == false) && (i == COMMANDLIST_RIGHT) ){
        m_commandListTbl[i] = COMMANDLIST_ID_ERROR;
        m_commandListTbl[i + (GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2)] = COMMANDLIST_ID_ERROR;
        TOMOYA_PRINT( "GraphicsSystem right commandbuffer not create\n" );
        continue;
      }
      this->createCommandList( heapAllocator, i, m_doubleBufferFlag, setup );
    }
  }

  // セーブ用コマンドリスト
  for( i=0; i<GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM; i++ ){
    if( i<m_useSaveCommandListNum ){
      m_saveCommandListTbl[i] = m_commandListManager->CreateBuffer( setup->saveCommandSetup );
    }else{
      m_saveCommandListTbl[i] = COMMANDLIST_ID_ERROR;
    }
  }


  // レンダリング設定の初期化
  {
    m_renderFlag          = false;
  }

  // スワップバッファ管理
  {
    m_swapDispCount = 0;
    m_swapCommandCount = 0;

    // まだ何も書いていないので初回はスワップできない。
    for( i=0; i<DISPLAY_BOTH_MAX; i++ ){
      m_curentDisplay[i] = DISPLAYBUFFER_ID_ERROR;
    }
    ClearSwapID();
    ClearRenderEndID();

    // swapCountに対応したバッファIDをカレントに設定
    SetCurrentSwapCountBuffer();
    ClearCurrentCommandList();

    // 新コマンドリストのバインド
    m_commandListManager->Bind( m_curentCommandList[COMMANDLIST_MAIN] );

    m_swapBufferFlag = false;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  バッファコントロール　破棄
 */
//-----------------------------------------------------------------------------
void BufferControl::Finalize( void )
{
  // 確保した、各バッファを破棄
  // セーブコマンドリスト　破棄
  for( u32 i=0; i<GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM; ++i ){
    if(m_saveCommandListTbl[i] != COMMANDLIST_ID_ERROR){
      m_commandListManager->DeleteBuffer( m_saveCommandListTbl[i] );
      m_saveCommandListTbl[i] = COMMANDLIST_ID_ERROR;
    }
  }

  // コマンドリスト　破棄
  for( u32 i=0; i<GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2; ++i ){
    this->deleteCommandList(i);
  }

  // フレームバッファ　破棄
  for( u32 i=0; i<DISPLAY_BOTH_MAX; ++i ){
    this->deleteFrameBuffer(i);
  }

  // ディスプレイバッファ　破棄
  for( u32 i=0; i<GRAPHICSSYSTEM_USE_DISPLAYBUFFER_NUM; ++i ){
    this->deleteDisplayBuffer(i);
  }


  // メンバ変数のクリア
  m_singleBufferForceFlag = false;
  m_reqSingleBufferForce  = false;
  m_doubleBufferFlag      = false;
  m_swapBufferFlag        = false;
  m_useDisplayBufferNum   = 0;
  m_useFrameBufferNum     = 0;
  m_useCommandListNum     = 0;
  m_useSaveCommandListNum = 0;
  m_useDisplayNum         = 0;          // 用意されているディスプレイの数
  m_useRightDisplay       = false;      // 右目を使用するとして用意されているのか
  m_appCreateRightBuffer  = false;      // 右目用バッファをアプリケーションが作成した状態のとき,TRUE
  m_renderFlag            = 0;          // レンダリング期間を管理するフラグ
  m_renderMsk             = static_cast<gfl::grp::DisplayMsk>(0);          // ディスプレイマスク
  m_swapCommandCount      = 0;          // スワップカウンタ
  m_swapDispCount         = 0;


  for( u32 i=0; i<COMMANDLIST_NUM; ++i ){
    m_curentCommandList[i] = COMMANDLIST_ID_ERROR;
  }
  m_curentSaveCommandList = 0;

  for( u32 i=0; i<DISPLAY_BOTH_MAX; ++i )
  {
    m_curentDisplay[i]   = DISPLAYBUFFER_ID_ERROR;
    m_rendEndDisplay[i]  = DISPLAYBUFFER_ID_ERROR;
    m_swapDisplay[i]     = DISPLAYBUFFER_ID_ERROR;
  }
  m_swapCommandList = 0;                    // スワップコマンドリスト

}

//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーションによる、後からの立体視バッファ確保
 */
//-----------------------------------------------------------------------------
void BufferControl::CreateAppRightBuffer( heap::NwAllocator* deviceAllocator, const GraphicsSystemSetUp* setup, GraphicsSystem* p_graphics )
{
  // すでに右目バッファがある場合は、何もしない。
  if( m_useRightDisplay ){
    return ;
  }

  // 右目用バファを生成し、右目レンダリングを開始する。
  m_useRightDisplay = true;
  // appUpperRight用のコンストラクタで生成していない。
  GFL_ASSERT_STOP( m_useDisplayNum == DISPLAY_BOTH_MAX );

  // フレームバッファの確保
  this->createFrameBuffer( deviceAllocator, &setup->frameSetup[DISPLAY_UPPER_RIGHT] );

  // コマンドリスト
  this->createCommandList( deviceAllocator, DISPLAY_UPPER_RIGHT, m_doubleBufferFlag, setup );

  // アプリケーションによって、右目バッファが生成された。
  m_appCreateRightBuffer = true;

  
  // 立体視用のバッファを設定する。
  this->SetCurrentSwapCountBuffer();
}

//----------------------------------------------------------------------------
/**
 *	@brief  右目のバッファ破棄リクエスト
 */
//-----------------------------------------------------------------------------
void BufferControl::DeleteAppRightBufferReq( void )
{
  // 破棄中の際リクエストの場合、無視する。
  if( m_appRightBufferDeleteReq != DEL_REQ_NONE ){
    return ;
  }

  // そもそも、バッファが確保されていない場合は、無視する。
  if( !m_appCreateRightBuffer ){
    return ;
  }
  
  m_appRightBufferDeleteReq = DEL_REQ_RENDERING_OFF;
}

//----------------------------------------------------------------------------
/**
 *	@brief  右目のバッファ破棄完了まち
 *
 *	@retval true    完了
 *	@retval false   途中
 */
//-----------------------------------------------------------------------------
bool BufferControl::IsFinishDeleteAppRightBuffer( void ) const
{
  return (m_appRightBufferDeleteReq == DEL_REQ_NONE);
}

//----------------------------------------------------------------------------
/**
 *	@brief  右目のバッファが生成されているかチェック
 *
 *	@retval true
 *	@retval false
 */
//-----------------------------------------------------------------------------
bool BufferControl::IsCreateAppRightBuffer( void ) const
{
  return m_appCreateRightBuffer;
}

//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーションによる、後からの立体視バッファの解放
 */
//-----------------------------------------------------------------------------
void BufferControl::DeleteAppRightBuffer( void )
{
  // 確保されたバッファがあるかチェック
  if( m_appCreateRightBuffer == false ){
    // 確保してない！
    return ;
  }
  m_appCreateRightBuffer = false;

  // コマンドリスト
  this->deleteCommandList( DISPLAY_UPPER_RIGHT );

  // フレームバッファ
  this->deleteFrameBuffer( DISPLAY_UPPER_RIGHT );

  // 右目用バファ無にしておく。
  m_useRightDisplay = false;

  // カレントのバッファを更新
  this->SetCurrentSwapCountBuffer();
}


//----------------------------------------------------------------------------
/**
 *	@brief  レンダリング開始
 *
 *	@param  msk         ディスプレイマスク
 *  @param  ClearColor クリアカラー
 */
//-----------------------------------------------------------------------------
void BufferControl::StartRendering( DisplayMsk msk, const FrameBufferClearColor* clearColor, const FrameBufferClearFlag* clearFlag )
{
  int i;
  if(m_renderFlag){
    GFL_ASSERT( m_renderFlag == false ); //@check
    EndRendering();
  }

  // 各画面向け、フレームバッファクリア処理を格納
  for( i=0; i<DISPLAY_BOTH_MAX; ++i ){
    int index;
    if( msk & (1<<i) ){
      if( (i==DISPLAY_UPPER) || (i==DISPLAY_UPPER_RIGHT) ){
        index = DISPLAY_UPPER;
      }else{
        index = DISPLAY_LOWER;
      }
      if( BindCurrentCommandList( static_cast<gfl::grp::DisplayType>(i) ) ){  // 各画面用のコマンドリストをバインド
        if ( clearFlag )
        {
          m_frameBufferManager->BindAndClear( m_frameBufferTbl[i], clearColor[index], clearFlag[index] );
        }
        else
        {
          FrameBufferClearFlag    allEnableFlag;
          m_frameBufferManager->BindAndClear( m_frameBufferTbl[i], clearColor[index], allEnableFlag );
        }
      }
    }
  }

  // コマンドリストバインド
  BindCurrentCommandList( DISPLAY_NONE );  // 各画面用のコマンドリストをバインド

  m_renderFlag = true;
  m_renderMsk  = msk;
}



//----------------------------------------------------------------------------
/**
 *	@brief  レンダリング　後処理
 *
 *	@param    parallaxDepthRange  立体視の視差の補正値
 *	                               (gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_か
 *                                  gfl::grp::g3d::StereoCameraのGetDepthRange関数の戻り値を渡して下さい。)
 */
//-----------------------------------------------------------------------------
void BufferControl::EndRendering( f32 parallaxDepthRange )
{
  EndRenderingEx( DISPLAY_UPPER, DISPLAY_UPPER_RIGHT, DISPLAY_LOWER, parallaxDepthRange );
}


//----------------------------------------------------------------------------
/**
 *	@brief    各ディスプレイバッファに転送するフレームバッファを指定したレンダリング後処理
 *
 *  @param  first     1番にコマンド実行をする面
 *  @param  second    2番にコマンド実行をする面
 *  @param  third     3番にコマンド実行をする面
 *  @param  parallaxDepthRange  立体視の視差の補正値
 *	                             (gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_か
 *                                gfl::grp::g3d::StereoCameraのGetDepthRange関数の戻り値を渡して下さい。)
 */
//-----------------------------------------------------------------------------
void BufferControl::EndRenderingEx( DisplayType first, DisplayType second, DisplayType third, f32 parallaxDepthRange )
{
  int i;

  if( m_renderFlag == false ) {
    return ;
  }


  // カレントフレームの情報をカレントディスプレイバッファに転送する処理を
  // それぞれのコマンドリストに格納
  for( i=0; i<m_useDisplayNum; ++i ){

    if( m_renderMsk & (1<<i) ){   // レンダリングを行った面かチェック

      // 右目バッファ破棄リクエスト中は、右目へのレンダリング処理は、行わない。
      if( IsRenderEnd(static_cast<gfl::grp::DisplayType>(i), parallaxDepthRange) ){

        if( BindCurrentCommandList( static_cast<gfl::grp::DisplayType>(i) ) ){
          const RenderTarget* renderTarget = m_frameBufferManager->GetRenderTarget( m_frameBufferTbl[i] );
          nngxFlush3DCommand();

          // キャプチャを実行
          AddReadPixelsCommand( static_cast<gfl::grp::DisplayType>(i), static_cast<gfl::grp::DisplayType>(i) );

          // SBTS[2120] GFSBTS[933]
          // 上画面 左目描画の場合で、右目レンダリングをしない状態であれば、
          // 右目用バッファに左目画像をキャプチャ
          if( ( m_useDisplayNum > gfl::grp::DISPLAY_NORMAL_MAX ) // 立体視ありの画面数
              && ( i == gfl::grp::DISPLAY_UPPER ) // 左目画像
              && ( !IsRenderEnd(gfl::grp::DISPLAY_UPPER_RIGHT, parallaxDepthRange) )  // 右目レンダリングなし
              )
          {
            // 右目に左目画像をキャプチャ
            AddReadPixelsCommand( gfl::grp::DISPLAY_UPPER_RIGHT, static_cast<gfl::grp::DisplayType>(i) );
          }


          // 描画面用のコマンドバッファをバインド
          m_displayBufferManager->TransferRenderImage( m_curentDisplay[i], *renderTarget );

          /*
          //----------
          // 立体視なしのとき、上左画面のコマンドバッファに上右画面への転送コマンドを追加する
          if(    ( m_useDisplayNum > gfl::grp::DISPLAY_NORMAL_MAX )
              && ( i == gfl::grp::DISPLAY_UPPER )
              && ( parallaxDepthRange == gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ZERO ) 
              && ( m_appRightBufferDeleteReq == DEL_REQ_NONE ) )
          {
            // 右目でも、左目のディスプレイバッファを使ってレンダリング
            // スワップバッファ対象。
            m_rendEndDisplay[gfl::grp::DISPLAY_UPPER_RIGHT] = m_curentDisplay[i];
            
            //AddReadPixelsCommand( gfl::grp::DISPLAY_UPPER_RIGHT, static_cast<gfl::grp::DisplayType>(i) );
            //m_displayBufferManager->TransferRenderImage( m_curentDisplay[gfl::grp::DISPLAY_UPPER_RIGHT], *renderTarget );
          }
          */
          // スワップバッファ対象。
          m_rendEndDisplay[i] = m_curentDisplay[i];
        }
      }

      // 右目レンダリング準備中は、左目の画面を出す。
      if( IsRenderRightNone(static_cast<gfl::grp::DisplayType>(i), parallaxDepthRange) ){
        // 右目の描画がないときは、左目のディスプレイを設定
        // スワップバッファ対象。
        m_rendEndDisplay[i] = m_curentDisplay[gfl::grp::DISPLAY_UPPER];
      }
    }
  }

  // Mainコマンドに統合
  // まだ、ディスプレイバッファの内容を反映コマンドが蓄積されるが、
  // それは、Mainコマンドに入れる。
  {
    DisplayType commandSchedule[ 3 ] = { first, second, third };

    // MAIN　CommandListをバインド
    BindCurrentCommandList( DISPLAY_NONE );

    for( i=0; i<3; ++i ){
      if( m_renderMsk & (1<<commandSchedule[i]) ){


        // バッファがあるかチェック
        if( GetCurrentCommandListID( commandSchedule[i] ) == COMMANDLIST_ID_ERROR ){
          continue;
        }

        // 右目バッファ破棄リクエスト中は、右目へのレンダリング処理は、行わない。
        if( IsRenderEnd(static_cast<gfl::grp::DisplayType>(commandSchedule[i]), parallaxDepthRange) ){
#if GFL_DEBUG
          {
            u32 restbuf, restreq;
            // コマンドリストの残り領域チェック
            m_commandListManager->GetBufferStatus( GetCurrentCommandListID( commandSchedule[i] ), NULL, NULL, &restbuf, &restreq );

            if( restbuf < 0x1000 ){
              GFL_PRINT( "warning display %d commandList buffsize less 0x%x\n", commandSchedule[i], restbuf );
            }
            if( restreq < 8 ){
  //            GFL_PRINT( "warning display %d commandList reqcount less %d\n", commandSchedule[i], restreq ); // 延々と出続けてテストできないので
            }

          }
#endif
          {
            m_commandListManager->AddCommand( GetCurrentCommandListID( commandSchedule[i] ), false );
          }
        }
      }
    }
  }

  // 次のスワップディスプレイに指定
  // 次の実行コマンドIDに指定
  m_swapCommandList = GetCurrentCommandListID( DISPLAY_NONE );


  // 右目バッファ破棄処理　DEL_REQ_RENDERING_OFF　→　DEL_REQ_RENDERING_END_WAIT
  if( m_appRightBufferDeleteReq == DEL_REQ_RENDERING_OFF ){ m_appRightBufferDeleteReq = DEL_REQ_RENDERING_END_WAIT; }

#if GFL_DEBUG
  {
    u32 restbuf, restreq;
    // コマンドリストの残り領域チェック
    m_commandListManager->GetBufferStatus( m_swapCommandList, NULL, NULL, &restbuf, &restreq );

    if( restbuf < 0x1000 ){
      GFL_PRINT( "warning MAIN commandList buffsize less 0x%x\n", restbuf );
    }
    if( restreq < 8 ){
      GFL_PRINT( "warning MAIN commandList reqcount less %d\n", restreq );
    }


    // コマンドリストの残り領域チェック
    m_commandListManager->GetBufferStatus( m_curentSaveCommandList, NULL, NULL, &restbuf, &restreq );

    if( restbuf < 0x1000 ){
      GFL_PRINT( "warning SAVE commandList buffsize less 0x%x\n", restbuf );
    }
    if( restreq < 8 ){
      GFL_PRINT( "warning SAVE commandList reqcount less %d\n", restreq );
    }

  }
#endif

  // キャッシュフラッシュを行います。
  //
  // フラッシュが必要なのは、コマンドバッファの領域や
  // h3d::MaterialData::MakeCommandAndCache が持っているコマンドキャッシュの領域ですが、
  // 小さな領域に分けて複数回フラッシュするよりも、
  // デバイスメモリ全体に対して 1 回にまとめてフラッシュした方が高速です。
  nngxUpdateBuffer( reinterpret_cast<void*>(nn::os::GetDeviceMemoryAddress()),  nn::os::GetDeviceMemorySize() );
  
  // レンダリング完了
  m_renderFlag = false;
}


//----------------------------------------------------------------------------
/**
 *	@brief  コマンドリストの実行が完了するまで待つ
 */
//-----------------------------------------------------------------------------
void BufferControl::WaitDone( void )
{
  m_commandListManager->WaitDone();
}

//----------------------------------------------------------------------------
/**
 *	@brief  スワップバッファ
 *
   *	この関数を行った後、直ぐにVSyncWaitを行ってください。
   *
   *	■ダブルバッファリングモードの場合
   *	  １．前フレームで開始したの実描画処理の完了を待ちます。
   *	  ２．前のフレームで描画した面をLODに反映します。
   *
   *	■シングルバッファリングモードの場合
   *	　１．今フレームで生成したコマンドを実行します。
   *	　２．描画の完了を待ちます。
   *	　３．描画結果をLODに反映します。
 */
//-----------------------------------------------------------------------------
void BufferControl::SwapBuffer( void )
{
  // 強制シングルバッファ切り替え
  if( m_reqSingleBufferForce ){
    m_singleBufferForceFlag = m_reqSingleBufferForceFlag;
    m_reqSingleBufferForce = false;
  }
  
  // ダブルバッファ
  if( m_doubleBufferFlag && !m_singleBufferForceFlag ){

    // すでに走っているコマンドがあるのであれば終わるまでまつ
    if( m_commandListManager->IsRuning() ){
      CommandListID runID = m_commandListManager->GetRuningCommandID();

      // コマンドＷａｉｔ
      m_commandListManager->WaitDone();
    }

    // 全バッファをスワップ
    Swap();

    // 今フレーム描画完了したものをスワップディスプレイバッファに格納
    SetSwapTargetRenderDisplay();
  }
  // シングルバッファ
  else{
    // レンダリング終了ディスプレイをこの場でスワップする。
    SetSwapTargetRenderDisplay();

    // コマンド実行
    if(m_swapCommandList != COMMANDLIST_ID_ERROR){
      m_commandListManager->RunAsync( m_swapCommandList );
      GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH, "RunCommand %d\n", m_swapCommandList );
      // コマンドＷａｉｔ
      m_commandListManager->WaitDone();
    }

    // 全バッファをスワップ
    Swap();
  }

  m_swapBufferFlag = true;

  // コマンドリスト実行回数
  m_doneCount = m_doneCount + 1;

  // 右目バッファ破棄処理　DEL_REQ_RENDERING_END_WAIT　→　DEL_REQ_RENDERING_END
  if( m_appRightBufferDeleteReq == DEL_REQ_RENDERING_END_WAIT ){ m_appRightBufferDeleteReq = DEL_REQ_RENDERING_END; }

  /* これ以降づづくべき処理があります。　VSync後　かならず、VSyncAfter関数を呼ぶこと */
}



//----------------------------------------------------------------------------
/**
 *	@brief  VSyncAfter期間処理
 *	この関数を呼ぶ前に必ずVSyncWaitを行ってください。
 *
 *	■ダブルバッファリングモードの場合
 *	  ３．今フレームで生成した3Dコマンドを実行します。
 *	  ４．ディスプレイバッファ、コマンドリストの書き込み先を変更します。
 *
 *	■シングルバッファリングモードの場合
 *	　特になし
 */
//-----------------------------------------------------------------------------
void BufferControl::VSyncAfter( void )
{
  // アプリケーションが生成した、バッファの破棄処理。
  if( m_appRightBufferDeleteReq == DEL_REQ_RENDERING_END ){
    DeleteAppRightBuffer();
    m_appRightBufferDeleteReq = DEL_REQ_NONE;
  }
  

  //
  if( m_doubleBufferFlag && !m_singleBufferForceFlag ){
    m_swapCommandCount = 1-m_swapCommandCount;
    m_swapDispCount    = 1-m_swapDispCount;
  }

  // スワップバッファカウントにあったバッファＩＤを設定
  SetCurrentSwapCountBuffer();
  ClearCurrentCommandList();

  // 新コマンドリストのバインド
  m_commandListManager->Bind( m_curentCommandList[COMMANDLIST_MAIN] );


  // キャプチャ処理の状態管理
  // コマンドが実行されないと、キャプチャが完了したことにならない。
  // ここで、完了を待つ必要がある。
  UpdateReadPixels();


  // StereoMode変更
  if( m_reqChangeStereoFlag ){
    if( m_reqChangeStereoMode ){
      nngxSetDisplayMode( NN_GX_DISPLAYMODE_STEREO );
    }else{
      nngxSetDisplayMode( NN_GX_DISPLAYMODE_NORMAL );
    }
    m_reqChangeStereoFlag = false;
  }

#if GFL_DEBUG
  // GPUProfile実行
  this->StopGpuProfile();
  this->StartGpuProfile();
#endif // GFL_DEBUG
}


//----------------------------------------------------------------------------
/**
 *	@brief  VSyncAfter期間処理
 *	この関数を呼ぶ前に必ずVSyncWaitを行ってください。
 */
//-----------------------------------------------------------------------------
void BufferControl::VSyncAfterRunAsync( bool skip )
{
  // ダブルバッファ 実行開始
  if( m_doubleBufferFlag && !m_singleBufferForceFlag ){

    // コマンド実行
    if(m_swapCommandList != COMMANDLIST_ID_ERROR){
      if( skip == false ){
        m_commandListManager->RunAsync( m_swapCommandList );
      }
      GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH, "RunCommand %d\n", m_swapCommandList );
    }
  }


  // それぞれswapするバッファのＩＤをクリア
  ClearSwapCommandID();
}



//----------------------------------------------------------------------------
/**
 *	@brief  カレントのディスプレイバッファＩＤを取得
 *
 *	@param	display   ディスプレイタイプ
 *
 *	@return カレントのディスプレイバッファＩＤ
 */
//-----------------------------------------------------------------------------
DisplayBufferID BufferControl::GetCurrentDisplayBufferID( DisplayType display )const
{
  if(display >= DISPLAY_BOTH_MAX){
    GFL_ASSERT( display < DISPLAY_BOTH_MAX ); //@check
    return DISPLAYBUFFER_ID_ERROR;
  }

  if( (display == DISPLAY_UPPER_RIGHT) && (m_useDisplayNum <= display) ){
    // そのディスプレイは用意されていません。
    GFL_ASSERT( 0 );
    return DISPLAYBUFFER_ID_ERROR;
  }

  return m_curentDisplay[ display ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  カレントのフレームバッファＩＤを取得
 *
 *	@param	display   ディスプレイタイプ
 *
 *	@return カレントのフレームバッファＩＤ
 */
//-----------------------------------------------------------------------------
FrameBufferID BufferControl::GetCurrentFrameBufferID( DisplayType display )const
{
  if(display >= GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM){
    GFL_ASSERT( 0 );
    return FRAMEBUFFER_ID_ERROR;
  }
  return m_frameBufferTbl[display];
}

//----------------------------------------------------------------------------
/**
 *	@brief  カレントのフレームバッファを取得
 *
 *	@param	display   ディスプレイ
 *
 *	@return カレントのフレームバッファ
 */
//-----------------------------------------------------------------------------
RenderTarget* BufferControl::GetCurrentFrameBuffer( DisplayType display ) const
{
  if(display >= GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM){
    GFL_ASSERT( 0 );
    return NULL;
  }
  return m_frameBufferManager->GetRenderTarget( m_frameBufferTbl[display] );
}

//----------------------------------------------------------------------------
/**
 *	@brief  カレントのコマンドリストＩＤを取得
 *
 *	@return コマンドリストＩＤ
 */
//-----------------------------------------------------------------------------
CommandListID BufferControl::GetCurrentCommandListID(DisplayType displayType)const
{
  // NONEならMainのコマンドリストを返す。
  if( displayType == DISPLAY_NONE ){
    return m_curentCommandList[COMMANDLIST_MAIN];
  }

  // ディスプレイタイプの処理を格納するコマンドリストIDを返す。
  static const CommandListType DisplayToCommandList[ DISPLAY_BOTH_MAX ] ={
    COMMANDLIST_LEFT,
    COMMANDLIST_BOTTOM,
    COMMANDLIST_RIGHT,
  };

  if( displayType < DISPLAY_BOTH_MAX )
  {
    return m_curentCommandList[ DisplayToCommandList[displayType] ];
  }
  else
  {
    GFL_ASSERT(0);
    return m_curentCommandList[ DisplayToCommandList[0] ];
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  カレントのセーブ用コマンドリストIDを取得
 *
 *	@return セーブ用コマンドリストID
 */
//-----------------------------------------------------------------------------
CommandListID BufferControl::GetCurrentSaveCommandListID(void)const
{
  return m_curentSaveCommandList;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ディスプレイタイプのコマンドリストをバインドする。
 *
 *	@param	display ディスプレイタイプ
 */
//-----------------------------------------------------------------------------
bool BufferControl::BindCurrentCommandList( DisplayType display ) const
{
  CommandListID commandID = GetCurrentCommandListID( display );

  if( commandID != COMMANDLIST_ID_ERROR ){
    m_commandListManager->Bind( commandID );
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  コマンドセーブ開始
 */
//-----------------------------------------------------------------------------
void BufferControl::StartCommandSave( void )
{
  m_commandListManager->StartCommandSave( m_curentSaveCommandList );
}

//----------------------------------------------------------------------------
/**
 *	@brief  コマンドセーブ終了
 */
//-----------------------------------------------------------------------------
void BufferControl::EndCommandSave( void )
{
  m_commandListManager->EndCommandSave();
}

//----------------------------------------------------------------------------
/**
 *	@brief    セーブ用コマンドリストへ蓄積された最新の内容をカレントコマンドリストに設定
 */
//-----------------------------------------------------------------------------
void BufferControl::ReuseCommandSave( void )
{
  m_commandListManager->ReuseCommandSave( m_curentSaveCommandList, false );
}


//----------------------------------------------------------------------------
/**
 *	@brief    カラーバッファのピクセル情報の取得Request
 *
 *	@param  memoryTop     上左画面格納先　（不要な場合NULL）
 *	@param  memoryTopRight上右画面格納先　（不要な場合NULL）
 *	@param  memoryBottom  下画面格納先  （不要な場合NULL）
 */
//-----------------------------------------------------------------------------
void BufferControl::RequestReadPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, u32 mode, RenderTarget::RenderBufferType type )
{
  m_reqReadPixcels              = mode;

  if( memoryTop ){
    m_reqReadPixcelsMemoryTop         = memoryTop;
    m_reqReadPixcelsTypeTop           = type;
  }

  if( memoryTopRight ){
    m_reqReadPixcelsMemoryTopRight    = memoryTopRight;
    m_reqReadPixcelsTypeTopRight      = type;
  }

  if( memoryBottom ){
    m_reqReadPixcelsMemoryBottom      = memoryBottom;
    m_reqReadPixcelsTypeBottom        = type;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    ピクセルアウトプットコマンドを追加
 *
 *	@param	display   描画面
 */
//-----------------------------------------------------------------------------
void BufferControl::AddReadPixelsCommand( DisplayType display, DisplayType copy_display )
{
  if( m_reqReadPixcels == READPIXCELS_REQ_ON_BLOCK ){

    if( ((display== DISPLAY_UPPER)) && m_reqReadPixcelsMemoryTop ){
      if( GetCurrentFrameBufferID(copy_display) != FRAMEBUFFER_ID_ERROR ){
        m_frameBufferManager->OutputImage( GetCurrentFrameBufferID(copy_display), m_reqReadPixcelsTypeTop, m_reqReadPixcelsMemoryTop );
      }
    }
    else if( ((display== DISPLAY_UPPER_RIGHT)) && m_reqReadPixcelsMemoryTopRight ){

      if( GetCurrentFrameBufferID(copy_display) != FRAMEBUFFER_ID_ERROR ){
        m_frameBufferManager->OutputImage( GetCurrentFrameBufferID(copy_display), m_reqReadPixcelsTypeTopRight, m_reqReadPixcelsMemoryTopRight );
      }
    }
    else if( ((display== DISPLAY_LOWER)) && m_reqReadPixcelsMemoryBottom ){
      if( GetCurrentFrameBufferID(copy_display) != FRAMEBUFFER_ID_ERROR ){
        m_frameBufferManager->OutputImage( GetCurrentFrameBufferID(copy_display), m_reqReadPixcelsTypeBottom, m_reqReadPixcelsMemoryBottom );
      }
    }

  }
  else if( m_reqReadPixcels == READPIXCELS_REQ_ON_LINER )
  {
    if( ((display== DISPLAY_UPPER)) && m_reqReadPixcelsMemoryTop ){
      if( GetCurrentFrameBufferID(copy_display) != FRAMEBUFFER_ID_ERROR ){
        m_frameBufferManager->OutputB2LImage( GetCurrentFrameBufferID(copy_display), m_reqReadPixcelsTypeTop, m_reqReadPixcelsMemoryTop );
      }
    }
    else if( ((display== DISPLAY_UPPER_RIGHT)) && m_reqReadPixcelsMemoryTopRight ){
      if( GetCurrentFrameBufferID(copy_display) != FRAMEBUFFER_ID_ERROR ){
        m_frameBufferManager->OutputB2LImage( GetCurrentFrameBufferID(copy_display), m_reqReadPixcelsTypeTopRight, m_reqReadPixcelsMemoryTopRight );
      }
    }
    else if( ((display== DISPLAY_LOWER)) && m_reqReadPixcelsMemoryBottom ){
      if( GetCurrentFrameBufferID(copy_display) != FRAMEBUFFER_ID_ERROR ){
        m_frameBufferManager->OutputB2LImage( GetCurrentFrameBufferID(copy_display), m_reqReadPixcelsTypeBottom, m_reqReadPixcelsMemoryBottom );
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    ピクセルアウトプット　ワーク管理
 */
//-----------------------------------------------------------------------------
void BufferControl::UpdateReadPixels( void )
{
  //キャプチャRequestを発行は、完了　（　EndRenderingで発行は行う。　）
  if( (m_reqReadPixcels == READPIXCELS_REQ_ON_BLOCK) || (m_reqReadPixcels == READPIXCELS_REQ_ON_LINER) ){
    m_reqReadPixcels = READPIXCELS_REQ_WAIT;
    m_reqReadPixcelsMemoryTop     = NULL;
    m_reqReadPixcelsMemoryBottom  = NULL;
  }
  //１フレーム後キャプチャ完了待ち状態を解除
  else if( m_reqReadPixcels == READPIXCELS_REQ_WAIT ){
    m_reqReadPixcels = READPIXCELS_REQ_NONE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    カラーバッファのピクセル情報の取得完了待ち
 *	@retval true    完了
 *	@retval false   途中
 */
//-----------------------------------------------------------------------------
bool BufferControl::WaitReadPixels( void ) const
{
  if( m_reqReadPixcels != READPIXCELS_REQ_NONE ){
    return false;
  }
  return true;
}



//----------------------------------------------------------------------------
/**
 *	@brief    スワップバッファフラグのクリア
 */
//-----------------------------------------------------------------------------
void BufferControl::ClearSwapBufferFlag( void )
{
  m_swapBufferFlag = false;
}

// シングルバッファフォースフラグ
void BufferControl::ReqSingleBufferForce( bool flag )
{
  m_reqSingleBufferForce = true;
  m_reqSingleBufferForceFlag = flag;
}

// 立体視ＯＮ・ＯＦＦリクエスト
void BufferControl::SetStereoModeReq( bool stereo_on )
{
  m_reqChangeStereoFlag = true;
  m_reqChangeStereoMode = stereo_on;
}

#if GFL_DEBUG

//----------------------------------------------------------------------------
/**
 *	@brief  GPUProfile　開始
 */
//-----------------------------------------------------------------------------
void BufferControl::StartGpuProfile( void )
{
  if( m_gpuProfileFlag == false ){
    return ;
  }
  if( m_gpuProfileRun == false ){

    nngxSetProfilingParameter( NN_GX_PROFILING_BUSY_SAMPLING_TIME_MICRO_SECOND, GPU_PROFILE_BUSY_MICRO_SECOND );
    nngxSetProfilingParameter( NN_GX_PROFILING_BUSY_SAMPLING_COUNT, 0 );
    nngxStartProfiling( NN_GX_PROFILING_BUSY );

    m_gpuProfileRun = true;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  GPUProfile　終了
 */
//-----------------------------------------------------------------------------
void BufferControl::StopGpuProfile( void )
{
  if( m_gpuProfileRun == false ){
    return ;
  }

  nngxStopProfiling( NN_GX_PROFILING_BUSY );

  m_gpuProfileRun = false;

  // 全パラメータ取得
  {
    nngxGetProfilingResult( NN_GX_PROFILING_BUSY, m_gpuProfileResultBusy );
    nngxGetProfilingResult( NN_GX_PROFILING_VERTEX_SHADER0, m_gpuProfileResultVertexShader[0] );
    nngxGetProfilingResult( NN_GX_PROFILING_VERTEX_SHADER1, m_gpuProfileResultVertexShader[1] );
    nngxGetProfilingResult( NN_GX_PROFILING_VERTEX_SHADER2, m_gpuProfileResultVertexShader[2] );
    nngxGetProfilingResult( NN_GX_PROFILING_VERTEX_SHADER3, m_gpuProfileResultVertexShader[3] );
    nngxGetProfilingResult( NN_GX_PROFILING_POLYGON, m_gpuProfileResultPolygon );
    nngxGetProfilingResult( NN_GX_PROFILING_FRAGMENT, &m_gpuProfileResultFragment );
    nngxGetProfilingResult( NN_GX_PROFILING_MEMORY_ACCESS, m_gpuProfileResultMemAccess );
  }

  // そのまま格納してよいもの。
  {
    // BUSY
    for( u32 i=0; i<4; ++i ){
      m_gpuProfileResultWork[ GPU_PROFILE_BUSY_SHADER0 + (i*2) + 0 ] = m_gpuProfileResultBusy[i] >> 16;
      m_gpuProfileResultWork[ GPU_PROFILE_BUSY_SHADER0 + (i*2) + 1 ] = m_gpuProfileResultBusy[i] & 0xffff;
    }
  }

  // 前回と今回の差分を格納する物。
  {
    // VertexShader
    for( u32 i=0; i<4; ++i ){
      for( u32 j=0; j<5; ++j ){
        m_gpuProfileResultWork[ GPU_PROFILE_VERTEX0_PROGCOUNTER + (i*5) + j ] = m_gpuProfileResultVertexShader[i][j] - m_gpuProfileResultLastWork[ GPU_PROFILE_VERTEX0_PROGCOUNTER + (i*5) + j ];
        m_gpuProfileResultLastWork[ GPU_PROFILE_VERTEX0_PROGCOUNTER + (i*5) + j ] = m_gpuProfileResultVertexShader[i][j];
      }
    }

    // Polygon
    for( u32 i=0; i<3; ++i ){
      m_gpuProfileResultWork[GPU_PROFILE_POLYGON_INPUT_VERTEX + i] = m_gpuProfileResultPolygon[i] - m_gpuProfileResultLastWork[GPU_PROFILE_POLYGON_INPUT_VERTEX + i];
      m_gpuProfileResultLastWork[GPU_PROFILE_POLYGON_INPUT_VERTEX + i] = m_gpuProfileResultPolygon[i];
    }

    // Fragment
    m_gpuProfileResultWork[GPU_PROFILE_FRAGMENT_INPUT_FRAGMENT] = m_gpuProfileResultFragment - m_gpuProfileResultLastWork[GPU_PROFILE_FRAGMENT_INPUT_FRAGMENT];
    m_gpuProfileResultLastWork[GPU_PROFILE_FRAGMENT_INPUT_FRAGMENT] = m_gpuProfileResultFragment;

    // MemoryAccess
    for( u32 i=0; i<18; ++i ){
      m_gpuProfileResultWork[GPU_PROFILE_ACCESS_VRAMA_READ + i] = (m_gpuProfileResultMemAccess[i]*16) - (m_gpuProfileResultLastWork[GPU_PROFILE_ACCESS_VRAMA_READ + i] * 16);
      m_gpuProfileResultLastWork[GPU_PROFILE_ACCESS_VRAMA_READ + i] = m_gpuProfileResultMemAccess[i];
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  GPUProfile　結果取得
 *
 *	@param	result  RESULTタイプ
 *
 *	@return 前回のレンダリング結果
 */
//-----------------------------------------------------------------------------
u32 BufferControl::GetGpuProfileResult( GpuProfileResult result ) const
{
  if( result < GPU_PROFILE_RESULT_MAX ){
    return m_gpuProfileResultWork[result];
  }
  GFL_ASSERT( result < GPU_PROFILE_RESULT_MAX ); //@check
  return 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief  1つ前のレンダリングでのGPUプロファイリング結果取得
 *
 *	@param  GpuProfileResult
 *
 *	@return 結果の数値
 *  @lim    カウンタがオーバーフローした瞬間　間違った数値が帰ってきます。
 */
//-----------------------------------------------------------------------------
const u32* BufferControl::GetGpuProfileResultPtr( GpuProfileResult result ) const
{
  if( result < GPU_PROFILE_RESULT_MAX ){
    return &m_gpuProfileResultWork[result];
  }
  GFL_ASSERT( result < GPU_PROFILE_RESULT_MAX ); //@check
  return &m_gpuProfileResultWork[0];
}

#endif // GFL_DEBUG



//----------------------------------------------------------------------------
/**
 *	@brief  swapCountに対応したバッファIDをカレントに設定
 */
//-----------------------------------------------------------------------------
void BufferControl::SetCurrentSwapCountBuffer(void)
{
  int i;
  int offset;

  // ディスプレイバッファ
  {
    offset = m_swapDispCount * DISPLAY_BOTH_MAX;

    for( i=0; i<m_useDisplayNum; ++i ){
      m_curentDisplay[i] = m_displayBufferTbl[ i + offset ];
    }
  }

  // コマンドリスト
  {
    offset = m_swapCommandCount * COMMANDLIST_NUM;

    // コマンドリストクリア
    for( i=0; i<COMMANDLIST_NUM; ++i ){
      m_curentCommandList[i]     = m_commandListTbl[ offset + i ];
    }
    m_curentSaveCommandList = m_saveCommandListTbl[ m_swapCommandCount ];
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  カレントのコマンドリストをクリアする
 */
//-----------------------------------------------------------------------------
void BufferControl::ClearCurrentCommandList( void )
{
  u32 i;
  // コマンドリストクリア
  for( i=0; i<COMMANDLIST_NUM; ++i ){
    if( m_curentCommandList[i] != COMMANDLIST_ID_ERROR  ){
      m_commandListManager->Clear( m_curentCommandList[i] );
    }
  }
  m_commandListManager->Clear( m_curentSaveCommandList );
}

//----------------------------------------------------------------------------
/**
 *	@brief  レンダリングしたディスプレイをスワップバッファするターゲットのディスプレイに設定
 */
//-----------------------------------------------------------------------------
void BufferControl::SetSwapTargetRenderDisplay(void)
{
  std::MemCopy( m_rendEndDisplay, m_swapDisplay, sizeof(DisplayBufferID)*DISPLAY_BOTH_MAX );
  ClearRenderEndID();
}


//----------------------------------------------------------------------------
/**
 *	@brief  ディスプレイバッファスワップ処理
 */
//-----------------------------------------------------------------------------
void BufferControl::Swap(void)
{
  int i;
  GLenum display = NN_GX_DISPLAY_BOTH;
  bool upper = false;
  bool lower = false;

  GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH, "SwapBuffer " );

  // ディスプレイバッファ情報をアクティブにする。
  for( i=0; i<DISPLAY_BOTH_MAX; i++ ){
    if( m_swapDisplay[i] != DISPLAYBUFFER_ID_ERROR )
    {
      //GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH, " %d", m_swapDisplay[i] );
      m_displayBufferManager->ActivateBuffer( m_swapDisplay[i], static_cast<DisplayType>(i) );
      if( (i==DISPLAY_UPPER) || (i==DISPLAY_UPPER_RIGHT) ){
        upper = true;
      }else{
        lower = true;
      }
    }
  }

  GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH, "\n" );

  // 両画面チェック
  if( upper || lower ){
    if( upper && lower ){
      display = NN_GX_DISPLAY_BOTH;
    }else if( upper ){
      display = NN_GX_DISPLAY0;
    }else if( lower ){
      display = NN_GX_DISPLAY1;
    }

    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH, "swap 0x%x\n", display );

    nngxSwapBuffers(display);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  ディスプレイバッファ　の生成
 *
 *	@param	heapAllocator     アロケータ
 *	@param	index             インデックス
 *	@param	double_buffer     ダブルバッファモードか？
 *	@param	cp_setup          セットアップパラメータ
 */
//-----------------------------------------------------------------------------
void BufferControl::createDisplayBuffer( gfl::heap::NwAllocator* heapAllocator, u32 index, bool double_buffer, const DisplayBufferSetUp* cp_setup )
{
  // single番目のバッファ
  m_displayBufferTbl[index] = m_displayBufferManager->CreateBuffer( *cp_setup );

  // double番目のバッファ
  if( double_buffer ){
    m_displayBufferTbl[index+DISPLAY_BOTH_MAX] = m_displayBufferManager->CreateBuffer( *cp_setup );
    m_displayBufferManager->ActivateBuffer( m_displayBufferTbl[index+DISPLAY_BOTH_MAX], cp_setup->displayType );
  }else{
    m_displayBufferTbl[index+DISPLAY_BOTH_MAX] = DISPLAYBUFFER_ID_ERROR;
    m_displayBufferManager->ActivateBuffer( m_displayBufferTbl[index], cp_setup->displayType );

  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ディスプレイバッファの破棄
 */
//-----------------------------------------------------------------------------
void BufferControl::deleteDisplayBuffer( u32 index )
{
  if(m_displayBufferTbl[index] != DISPLAYBUFFER_ID_ERROR){
    m_displayBufferManager->DeleteBuffer( m_displayBufferTbl[index] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  フレームバッファの生成
 */
//-----------------------------------------------------------------------------
void BufferControl::createFrameBuffer( gfl::heap::NwAllocator* heapAllocator, const RenderBufferSetUp* cp_setup )
{
  GFL_ASSERT( cp_setup );
  GFL_ASSERT_STOP( cp_setup->targetDisplay < DISPLAY_BOTH_MAX );
  GFL_ASSERT_STOP( m_frameBufferTbl[cp_setup->targetDisplay] == FRAMEBUFFER_ID_ERROR );
  m_frameBufferTbl[cp_setup->targetDisplay] = m_frameBufferManager->CreateBuffer( heapAllocator, *cp_setup );
}

//----------------------------------------------------------------------------
/**
 *	@brief  フレームバッファの破棄
 */
//-----------------------------------------------------------------------------
void BufferControl::deleteFrameBuffer( u32 index )
{
  if(m_frameBufferTbl[index] != COMMANDLIST_ID_ERROR){
    m_frameBufferManager->DeleteBuffer( m_frameBufferTbl[index] );
    m_frameBufferTbl[index] = FRAMEBUFFER_ID_ERROR;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  コマンドリストの生成
 */
//-----------------------------------------------------------------------------
void BufferControl::createCommandList( gfl::heap::NwAllocator* heapAllocator, u32 index, bool double_buffer, const GraphicsSystemSetUp* setup )
{
  static const u32 setup_index[] = {
    COMMANDLIST_SETUP_MAIN,
    COMMANDLIST_SETUP_UPPER,
    COMMANDLIST_SETUP_UPPER,
    COMMANDLIST_SETUP_LOWER,
  };

  m_commandListTbl[index] = m_commandListManager->CreateBuffer( setup->commandSetup[ setup_index[index] ] );


  // ダブルバッファリング用バッファの格納
  if(m_doubleBufferFlag){
    m_commandListTbl[index + (GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2)] = m_commandListManager->CreateBuffer( setup->commandSetup[ setup_index[index] ] );
  }else{
    m_commandListTbl[index + (GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2)] = COMMANDLIST_ID_ERROR;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  コマンドリストの破棄
 */
//-----------------------------------------------------------------------------
void BufferControl::deleteCommandList( u32 index )
{
  if(m_commandListTbl[index] != COMMANDLIST_ID_ERROR){
    m_commandListManager->DeleteBuffer( m_commandListTbl[index] );
    m_commandListTbl[index] = COMMANDLIST_ID_ERROR;
  }

  if(m_doubleBufferFlag){
    if(m_commandListTbl[index + (GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2)] != COMMANDLIST_ID_ERROR){
      m_commandListManager->DeleteBuffer( m_commandListTbl[index + (GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2)] );
      m_commandListTbl[index + (GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2)] = COMMANDLIST_ID_ERROR;
    }
  }
}




//----------------------------------------------------------------------------
/**
 *	@brief  スワップするバッファＩＤをクリア
 */
//-----------------------------------------------------------------------------
void BufferControl::ClearSwapID(void)
{
  int i;
  for( i=0; i<DISPLAY_BOTH_MAX; i++ ){
    m_swapDisplay[i] = DISPLAYBUFFER_ID_ERROR;
  }
  ClearSwapCommandID();
}

void BufferControl::ClearSwapCommandID(void)
{
  m_swapCommandList = COMMANDLIST_ID_ERROR;
}

//----------------------------------------------------------------------------
/**
 *	@brief  レンダリング終了　ディスプレイバッファIDのクリア
 */
//-----------------------------------------------------------------------------
void BufferControl::ClearRenderEndID(void)
{
  int i;
  for( i=0; i<DISPLAY_BOTH_MAX; i++ ){
    m_rendEndDisplay[i] = DISPLAYBUFFER_ID_ERROR;
  }
}






//=============================================================================
/**
 *					メモリコントロールクラス定義
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 *
 *	@param  deviceAllocator   システム用デバイスアロケータ
 *	@param  vramASize         システム用VRAM-Aサイズ
 *	@param  vramBSize         システム用VRAM-Bサイズ
 */
//-----------------------------------------------------------------------------
MemoryControl::MemoryControl(heap::NwAllocator* deviceAllocator, u32 vramASize, u32 vramBSize ) :
  m_AreaManVramA( AREAMAN_BLOCK_NUM, deviceAllocator->GetHeapBase() ),
  m_AreaManVramB( AREAMAN_BLOCK_NUM, deviceAllocator->GetHeapBase() )
{
  // アロックテーブル初期化
  gfl::std::MemFill( m_AllocTblVramA, 0xff, sizeof(gfl::base::AreaMan::ResultCode)*VRAM_ALLOCK_BLOCK_NUM );
  gfl::std::MemFill( m_AllocTblVramB, 0xff, sizeof(gfl::base::AreaMan::ResultCode)*VRAM_ALLOCK_BLOCK_NUM );

  // アロケータ
  m_SystemDeviceAllocator = deviceAllocator;
  m_ApplyDeviceAllocator  = NULL;

  // vramサイズ AREAMAN_BLOCK_SIZEの倍数に調整
  m_SystemVramASize = vramASize + (vramASize % AREAMAN_BLOCK_SIZE);
  m_SystemVramBSize = vramBSize + (vramBSize % AREAMAN_BLOCK_SIZE);


  // アプリケーションメモリがない場合は、システムメモリから確保
  // 固定メモリから確保するようにする。
  this->SetUseMemory( SYSTEM_TYPE );
  // メモリ確保方向
  m_AllocWay        = ALLOC_TOP;

}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
MemoryControl::~MemoryControl()
{
#if GFL_DEBUG
  bool error = false;

  if( m_countDevice != 0 ){
    GFL_PRINT( "GraphicsSystem deviceCount %d\n", m_countDevice );
    error = true;
  }

  if( m_countVramA != 0 ){
    GFL_PRINT( "GraphicsSystem vramACount %d\n", m_countVramA );
    error = true;
  }

  if( m_countVramB != 0 ){
    GFL_PRINT( "GraphicsSystem vramBCount %d\n", m_countVramB );
    error = true;
  }

  if( error ){
    GFL_ASSERT(0);
  }
#endif
}



//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーションメモリ設定の初期化
 *
 *	@param	deviceAllocator   アプリケーションで使用するデバイスメモリ
 */
//-----------------------------------------------------------------------------
void MemoryControl::InitializeApplication( heap::NwAllocator* deviceAllocator )
{
  GFL_ASSERT( m_ApplyDeviceAllocator == NULL  );  // 多重呼び出し禁止
  m_ApplyDeviceAllocator = deviceAllocator;

  this->SetUseMemory( APPLICATION_TYPE );
  m_AllocWay = ALLOC_TOP; // アプリケーションは、トップから。
}

//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーションメモリ設定の破棄
 */
//-----------------------------------------------------------------------------
void MemoryControl::FinalizeApplication( void )
{
  m_ApplyDeviceAllocator = NULL;

  if(m_ControlType == APPLICATION_TYPE){

    // アプリケーションVRAM領域がきれいな状態かチェック
#if GFL_DEBUG
    u32 vrama_free_block = m_AreaManVramA.GetContiniusFreeBlockNum( m_UseVramAStartBlock, m_UseVramANumBlock );
    u32 vramb_free_block = m_AreaManVramB.GetContiniusFreeBlockNum( m_UseVramBStartBlock, m_UseVramBNumBlock );
    bool error = false;
    
    if( vrama_free_block != m_UseVramANumBlock ){
      GFL_PRINT( "GraphicsSystem Application VRAMA Remain Memory blocknum %d\n", m_UseVramANumBlock - vrama_free_block );
      error = true;
    }
    if( vramb_free_block != m_UseVramBNumBlock ){
      GFL_PRINT( "GraphicsSystem Application VRAMB Remain Memory blocknum %d\n", m_UseVramBNumBlock - vramb_free_block );
      error = true;
    }
    GFL_ASSERT_MSG( !error, "VRAMの解放忘れがあります\n" );
#endif // GFL_DEBUG

    this->SetUseMemory( SYSTEM_TYPE );
    m_AllocWay = ALLOC_TOP;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  アプリケーションメモリ設定がされているか？
 *
 *	@retval true  されている。
 *	@retval false されていない。
 */
//-----------------------------------------------------------------------------
bool MemoryControl::IsInitializeApplication(void) const
{
  if( m_ApplyDeviceAllocator != NULL ){
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------
/**
 *	@brief  Memory領域確保
 *
 *	@param	area    Memoryエリア
 *	@param  type    使用用途
 *	@param	size    サイズ
 *
 *	@return アドレス
 */
//-----------------------------------------------------------------------------
void* MemoryControl::Alloc( MemoryArea area, MemoryUseType type, u32 size )
{
  u32 alignment;
  void * resultAddr = NULL;

  // 使用用途ごとのアライメント取得
  alignment = GraphicsSystem::GetAlignment( type );


  // メモリ確保
  switch (area)
  {
  case MEMORY_AREA_FCRAM:
    // FCRAM の場合は SDK サポートの拡張ヒープから割り当てるため管理しない
    if( m_AllocWay == ALLOC_TOP ){
      resultAddr = GflHeapAllocMemoryAlign( m_UseDeviceAllocator->GetHeapBase(), size, alignment );
    }else{
      resultAddr = GflHeapAllocMemoryLowAlign( m_UseDeviceAllocator->GetHeapBase(), size, alignment );
    }

#if GFL_DEBUG
    m_countDevice ++;
#endif
    break;
  case MEMORY_AREA_VRAMA:
    {
      gfl::base::AreaMan::ResultCode* p_clearWk = GetClearAllocTbl( m_AllocTblVramA );
      uptr  addr;


      // AreaManのブロックサイズとalignmentをチェックし、
      // エリアブロック単位でのアライメントを計算
      m_AreaManVramA.SetAlign( GetBlockAlign(alignment) );

      // VRAMのブロックを取得
      if( m_AllocWay == ALLOC_TOP ){
        *p_clearWk = m_AreaManVramA.ReserveArea( GetBlockSize(size), m_UseVramAStartBlock, m_UseVramANumBlock );
      }else{

        u32 searchStartBlock;
        u32 searchBlockNum;

        // ディスプレイバッファは、0x180000未満のアドレスに配置される必要がある。
        // （CTRのハード仕様）
        if( (type == MEM_DISPLAYBUFFER) && (m_UseVramAStartBlock+m_UseVramANumBlock) >= GetBlockSize(0x180000) ){
          searchStartBlock = GetBlockSize(0x180000) - 1;
          searchBlockNum   = GetBlockSize(0x180000);
        }else{
          searchStartBlock = (m_UseVramAStartBlock+m_UseVramANumBlock) - 1;
          searchBlockNum    = m_UseVramANumBlock;
        }

        *p_clearWk = m_AreaManVramA.ReserveAreaLo( GetBlockSize(size), searchStartBlock, searchBlockNum );
      }

      addr = m_AreaManVramA.ResCodeToBlockPos( *p_clearWk );

      // 確保に失敗していないかチェック
      if(*p_clearWk == gfl::base::AreaMan::RESULT_FAILED){
        // サイズオーバー
        GFL_ASSERT_MSG( 0, "VRAMA over" );
        addr = 0; // 強制的に転送
      }
      addr *= AREAMAN_BLOCK_SIZE;

#if GFL_DEBUG
      if( (type == MEM_DISPLAYBUFFER) && ((addr + size) > 0x180000) ){

        // ディスプレイバッファ　の　サイズオーバー
        GFL_ASSERT(0);
        addr = 0;
      }
#endif
      addr += nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMA);
      resultAddr = reinterpret_cast<void*>(addr);
    }
#if GFL_DEBUG
    m_countVramA ++;
#endif
    break;
  case MEMORY_AREA_VRAMB:
    {
      gfl::base::AreaMan::ResultCode* p_clearWk = GetClearAllocTbl( m_AllocTblVramB );
      uptr  addr;

      // AreaManのブロックサイズとalignmentをチェックし、
      // エリアブロック単位でのアライメントを計算
      m_AreaManVramB.SetAlign( GetBlockAlign(alignment) );

      // VRAMのブロックを取得
      if( m_AllocWay == ALLOC_TOP ){
        *p_clearWk = m_AreaManVramB.ReserveArea( GetBlockSize(size), m_UseVramBStartBlock, m_UseVramBNumBlock );
      }else{

        u32 searchStartBlock;
        u32 searchBlockNum;

        // ディスプレイバッファは、0x180000未満のアドレスに配置される必要がある。
        // （CTRのハード仕様）
        if( (type == MEM_DISPLAYBUFFER) && ((m_UseVramBStartBlock+m_UseVramBNumBlock) >= GetBlockSize(0x180000)) ){
          searchStartBlock = GetBlockSize(0x180000) - 1;
          searchBlockNum    = GetBlockSize(0x180000);
        }else{
          searchStartBlock  = (m_UseVramBStartBlock+m_UseVramBNumBlock) - 1;
          searchBlockNum    = m_UseVramBNumBlock;
        }

        *p_clearWk = m_AreaManVramB.ReserveAreaLo( GetBlockSize(size), searchStartBlock, searchBlockNum );
      }

      addr = m_AreaManVramB.ResCodeToBlockPos( *p_clearWk );

      // 確保に失敗していないかチェック
      if(*p_clearWk == gfl::base::AreaMan::RESULT_FAILED){
        // サイズオーバー
        GFL_ASSERT_MSG( 0, "VRAMB over" );
        addr = 0; // 強制的に転送
      }
      addr *= AREAMAN_BLOCK_SIZE;

#if GFL_DEBUG
      if( (type == MEM_DISPLAYBUFFER) && ((addr + size) > 0x180000) ){
        // ディスプレイバッファ　の　サイズオーバー
        GFL_ASSERT(0);
        addr = 0;
      }
#endif
      addr += nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMB);
      resultAddr = reinterpret_cast<void*>(addr);
    }
#if GFL_DEBUG
    m_countVramB ++;
#endif
    break;
  default:
    GFL_ASSERT(0);
    break;
  }

#if GFL_DEBUG
  switch( area ){
  case MEMORY_AREA_FCRAM:
    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM, "alloc memory type = FCRAM, size = 0x%x, align = %d, addr = 0x%x\n", size, alignment, resultAddr );
    break;
  case MEMORY_AREA_VRAMA:
    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM, "alloc memory type = VRAMA, size = 0x%x, align = %d, addr = 0x%x\n", size, alignment, resultAddr );
    break;
  case MEMORY_AREA_VRAMB:
    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM, "alloc memory type = VRAMB, size = 0x%x, align = %d, addr = 0x%x\n", size, alignment, resultAddr );
    break;
  }
#endif

  GFL_ASSERT(resultAddr);

  return resultAddr;
}

//----------------------------------------------------------------------------
/**
 *	@brief  フリー
 *
 *	@param	area    メモリエリア
 *	@param	addr    アドレス
 */
//-----------------------------------------------------------------------------
void MemoryControl::Free( MemoryArea area, void* addr )
{
#if GFL_DEBUG
  switch( area ){
  case MEMORY_AREA_FCRAM:
    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM, "free memory type = FCRAM, addr = 0x%x vramA=%d vramB=%d\n", addr, m_countVramA, m_countVramB );
    break;
  case MEMORY_AREA_VRAMA:
    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM, "free memory type = VRAMA, addr = 0x%x vramA=%d vramB=%d\n", addr, m_countVramA, m_countVramB );
    break;
  case MEMORY_AREA_VRAMB:
    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM, "free memory type = VRAMB, addr = 0x%x vramA=%d vramB=%d\n", addr, m_countVramA, m_countVramB );
    break;
  }
#endif

  switch (area)
  {
  case MEMORY_AREA_FCRAM:
    // 破棄
    GflHeapFreeMemory( addr );
#if GFL_DEBUG
    m_countDevice --;
#endif
    break;

  case MEMORY_AREA_VRAMA:
    {
      uptr adress = reinterpret_cast<uptr>(addr);
      adress -= nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMA);


      gfl::base::AreaMan::ResultCode* p_addrWk = GetAddrAllocTbl( m_AllocTblVramA, adress );
      if( p_addrWk == NULL ){
        // 不明なアドレス。
        GFL_ASSERT( 0 );
      }else{
        m_AreaManVramA.Release( *p_addrWk );
        *p_addrWk = gfl::base::AreaMan::RESULT_FAILED;
      }
    }
#if GFL_DEBUG
    m_countVramA --;
#endif
    break;

  case MEMORY_AREA_VRAMB:
    {
      uptr adress = reinterpret_cast<uptr>(addr);
      adress -= nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMB);


      gfl::base::AreaMan::ResultCode* p_addrWk = GetAddrAllocTbl( m_AllocTblVramB, adress );
      if( p_addrWk == NULL ){
        // 不明なアドレス。
        GFL_ASSERT( 0 );
      }else{
        m_AreaManVramB.Release( *p_addrWk );
        *p_addrWk = gfl::base::AreaMan::RESULT_FAILED;
      }
    }
#if GFL_DEBUG
    m_countVramB --;
#endif
    break;

  default:
    GFL_ASSERT(0);
    break;
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  コントロールタイプの設定
 *
 *	@param	type  タイプ
 */
//-----------------------------------------------------------------------------
void MemoryControl::SetControlType( MemoryControl::ControlType type, MemoryControl::AllocWay way )
{
  this->SetUseMemory( type );
  m_AllocWay = way;
}

//----------------------------------------------------------------------------
/**
 *	@brief  コントロールタイプの取得
 *
 *	@return コントロールタイプ
 */
//-----------------------------------------------------------------------------
MemoryControl::ControlType MemoryControl::GetControlType(void) const
{
  return m_ControlType;
}

//----------------------------------------------------------------------------
/**
 *	@brief  メモリ確保方向の取得
 *
 *	@return メモリ確保方向
 */
//-----------------------------------------------------------------------------
MemoryControl::AllocWay MemoryControl::GetAllocWay(void) const
{
  return m_AllocWay;
}


//----------------------------------------------------------------------------
/**
 *	@brief  VRAM-A　最大確保可能サイズの取得
 *
 *	@return 最大確保可能サイズ
 */
//-----------------------------------------------------------------------------
u32 MemoryControl::GetVramAFreeSize( void ) const
{
  return m_AreaManVramA.GetContiniusFreeBlockNum( m_UseVramAStartBlock, m_UseVramANumBlock ) * AREAMAN_BLOCK_SIZE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  VRAM-B　最大確保可能サイズの取得
 *
 *	@return 最大確保可能サイズ
 */
//-----------------------------------------------------------------------------
u32 MemoryControl::GetVramBFreeSize( void ) const
{
  return m_AreaManVramB.GetContiniusFreeBlockNum( m_UseVramBStartBlock, m_UseVramBNumBlock ) * AREAMAN_BLOCK_SIZE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  VRAM管理のリセット処理
 *  @fix 有泉さん要望：ＶＲＡＭアドレス指定配置対応
 */
//-----------------------------------------------------------------------------
u32 GraphicsSystem::GetAlignment( MemoryUseType type )
{
  u32 alignment = ALIGNMENT_SYSTEM_BUFFER;
  switch (type)
  {
  case MEM_SYSTEM:
      alignment = ALIGNMENT_SYSTEM_BUFFER;
      break;
  case MEM_TEXTURE:
      alignment = ALIGNMENT_TEXTURE;
      break;
  case MEM_VERTEXBUFFER:
      alignment = ALIGNMENT_VERTEX;
      break;
  case MEM_RENDERBUFFER:
      alignment = ALIGNMENT_RENDER_BUFFER;
      break;
  case MEM_DISPLAYBUFFER:
      alignment = ALIGNMENT_DISPLAY_BUFFER;
      break;
  case MEM_COMMANDBUFFER:
      alignment = ALIGNMENT_3D_COMMAND_BUFFER;
      break;
  default:
      GFL_ASSERT(0);
      break;
  }

  return alignment;
}


//----------------------------------------------------------------------------
/**
 *	@brief  空のアロケートテーブルを取得
 *
 *	@param	p_tbl   テーブル
 *
 *	@return 空のアロケートテーブルポインタ
 */
//-----------------------------------------------------------------------------
gfl::base::AreaMan::ResultCode* MemoryControl::GetClearAllocTbl( gfl::base::AreaMan::ResultCode* p_tbl ) const
{
  for( int i=0; i<VRAM_ALLOCK_BLOCK_NUM; i++ ){
    if( p_tbl[i] == gfl::base::AreaMan::RESULT_FAILED ){
      return &p_tbl[i];
    }
  }
  // 確保できるテーブルなし
  GFL_ASSERT_STOP(0);
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  VRAMアドレス　から　開始する　アロックテーブルを取得
 *
 *	@param	p_tbl アロックテーブル
 *	@param	addr  アドレス
 *
 *	@return VRAMアドレスから開始するアロックテーブル
 */
//-----------------------------------------------------------------------------
gfl::base::AreaMan::ResultCode* MemoryControl::GetAddrAllocTbl( gfl::base::AreaMan::ResultCode* p_tbl, uptr addr ) const
{
  u32 start_block = addr / AREAMAN_BLOCK_SIZE;

  for( int i=0; i<VRAM_ALLOCK_BLOCK_NUM; i++ ){
    if( m_AreaManVramA.ResCodeToBlockPos(p_tbl[i]) == start_block ){
      return &p_tbl[i];
    }
  }
  // 確保できるテーブルなし
  GFL_ASSERT_STOP(0);
  return NULL;
}



// ツール系メンバ
//----------------------------------------------------------------------------
/**
 *	@brief  バイト単位のアライメントからAreaManのブロックアライメントを計算
 *
 *	@param	u32 align   バイト単位のアライメント
 *
 *	@return ブロック単位のアライメント
 */
//-----------------------------------------------------------------------------
u32 MemoryControl::GetBlockAlign( u32 align )
{
  u32 block_align = 1;

  if( align > AREAMAN_BLOCK_SIZE ){
    block_align = align / AREAMAN_BLOCK_SIZE;
    // あまりがあれば、繰上げ
    if( (align % AREAMAN_BLOCK_SIZE) != 0 ){
      block_align += 1;
    }
  }
  return block_align;
}

//----------------------------------------------------------------------------
/**
 *	@brief  バイト単位のsizeから、確保したいブロック数を計算
 *
 *	@param	u32 size   バイト単位のsize
 *
 *	@return ブロック単位の確保size
 */
//-----------------------------------------------------------------------------
u32 MemoryControl::GetBlockSize( u32 size )
{
  u32 block_size = 1;

  if( size >= AREAMAN_BLOCK_SIZE ){
    block_size = size / AREAMAN_BLOCK_SIZE;
    if( (size % AREAMAN_BLOCK_SIZE) > 0 ){
      block_size += 1; // 繰上げ
    }
  }
  return block_size;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ControlTypeに対応した、Useメモリ情報を設定
 *
 *	@param	control_type  設定するコントロールタイプ
 */
//-----------------------------------------------------------------------------
void MemoryControl::SetUseMemory( MemoryControl::ControlType control_type )
{
  u32 systemVramABlockNum = ((m_SystemVramASize % AREAMAN_BLOCK_SIZE) == 0) ? (m_SystemVramASize / AREAMAN_BLOCK_SIZE) : (m_SystemVramASize / AREAMAN_BLOCK_SIZE) + 1;
  u32 systemVramBBlockNum = ((m_SystemVramBSize % AREAMAN_BLOCK_SIZE) == 0) ? (m_SystemVramBSize / AREAMAN_BLOCK_SIZE) : (m_SystemVramBSize / AREAMAN_BLOCK_SIZE) + 1;

  if(control_type == SYSTEM_TYPE){
    m_UseDeviceAllocator  = m_SystemDeviceAllocator;
    m_UseVramAStartBlock  = 0;
    m_UseVramANumBlock    = systemVramABlockNum;
    m_UseVramBStartBlock  = 0;
    m_UseVramBNumBlock    = systemVramBBlockNum;
    m_ControlType         = control_type;

  }
  else if( m_ApplyDeviceAllocator != NULL ){
    m_UseDeviceAllocator  = m_ApplyDeviceAllocator;
    m_UseVramAStartBlock  = systemVramABlockNum;
    m_UseVramANumBlock    = AREAMAN_BLOCK_NUM - m_UseVramAStartBlock;
    m_UseVramBStartBlock  = systemVramBBlockNum;
    m_UseVramBNumBlock    = AREAMAN_BLOCK_NUM - m_UseVramBStartBlock;
    m_ControlType         = control_type;
  }
  else {
    GFL_ASSERT_MSG(0, "アプリケーションメモリが初期化されていません。\n");
    // フリーズ回避
    m_UseDeviceAllocator  = m_SystemDeviceAllocator;
    m_UseVramAStartBlock  = 0;
    m_UseVramANumBlock    = systemVramABlockNum;
    m_UseVramBStartBlock  = 0;
    m_UseVramBNumBlock    = systemVramBBlockNum;
    m_ControlType         = SYSTEM_TYPE;
  }
}


} /* namespace end grp */
} /* namespace end gfl */
