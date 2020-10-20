//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_GraphicsSystem.h
 *	@brief  グラフィックスシステム    描画環境構築・管理
 *	@author	tomoya takahashi
 *	@date		2010.10.22
 *	@date		2011.03.23    VRAM領域節約のため、フレームバッファ１つで各画面用の描画を
 *	　　　　　　　　　　　行えるシステムに変更開始
 *
 *
 *	VRAM領域節約のための構造
 *	- 描画先フレームバッファごとのコマンドを別々のコマンドバッファに格納する。
 *	  -#左目右目の描画処理を1つのフレームバッファで行う場合、カラーや深度情報の
 *	  　書き込みが多重に行われないよう、左目用の３D　２Dすべての描画後に　
 *	  　右目用３D　２Dすべての描画を行うように順序良くコマンドをバッファに格納していく必要があります。
 *
 *	　-#しかし、３D描画、２D描画など、別々の描画システムを使用し、レンダリングしていく際に、
 *	　　左目、右目別々に描画処理を行うことは、システムの複雑さにつながり、簡単に言うと面倒な制約になってきます。
 *
 *	　-#そこで、GraphicsSystem内部に、1つのフレームバッファに対する、左右下画面用のコマンドを別々に格納する
 *	　　コマンドバッファを別々に用意し、、EndRenderingのタイミングで、
 *	　　それら別々のコマンド情報を1つにまとめて、レンダリングを実行するシステムを用意しました。
 *
 *	　-#少々わかりにくい構造になりますが、ご了承ください。
 *
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#ifndef __GFL_GRAPHICSSYSTEM_H__
#define __GFL_GRAPHICSSYSTEM_H__
#pragma once


#include <nn.h>
#include <nw.h>

#include <base/gfl_Singleton.h>

#include <debug/gfl_DebugPrint.h>


#include <gfl_Heap.h>
#include <gfl_Macros.h>

#include <grp/gfl_GraphicsType.h>
#include <grp/gfl_DisplayBufferManager.h>
#include <grp/gfl_FrameBufferManager.h>
#include <grp/gfl_CommandListManager.h>

namespace gfl {
namespace grp {






//-----------------------------------------------------------------------------
/**
 *					デバッグ出力
*/
//-----------------------------------------------------------------------------
#define GFL_DEBUG_PRINT_GRAPHICSSYSTEM (gfl::debug::PRINT_FLAG_LEVEL1 | gfl::debug::PRINT_FLAG_CATEGORY_G3D)

// ダブルバッファリング監視
#define GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH (gfl::debug::PRINT_FLAG_LEVEL4 | gfl::debug::PRINT_FLAG_CATEGORY_G3D)

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	動作モード
//=====================================
typedef enum {

  MODE_DOUBLE_BUFFER,   // ダブルバッファリングモード　（推奨）
  MODE_SINGLE_BUFFER,   // シングルバッファリングモード

  MODE_ENUM_END,


  ENUM_FORCE_DWORD(GraphicsSystemBufferMode)

} GraphicsSystemBufferMode;


// コマンドリストタイプ
enum CommandListType
{
  COMMANDLIST_MAIN = 0,     // メインコマンドリスト （OffScreenの処理）
  COMMANDLIST_LEFT,         // 左目用コマンドリスト (左目用処理)
  COMMANDLIST_RIGHT,        // 右目用コマンドリスト (右目用処理)
  COMMANDLIST_BOTTOM,       // 下画面用コマンドリスト (下画面用処理）

  COMMANDLIST_NUM,          // コマンドリスト数
};

enum{
  // システムが最大で保持するバッファ数
  GRAPHICSSYSTEM_USE_DISPLAYBUFFER_NUM = 6,   // ディスプレイバッファ
  GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM = DISPLAY_BOTH_MAX,     // フレームバッファ
  GRAPHICSSYSTEM_USE_COMMANDLIST_NUM = COMMANDLIST_NUM*2, // コマンドリスト
  GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM = 2, // セーブ用コマンドリスト

  GRAPHICSSYSTEM_USER_WORK = 8, // ユーザーが自由に使えるコマンドリスト数

  GRAPHICSSYSTEM_TOTAL_COMMANDLIST_MAX = GRAPHICSSYSTEM_USE_COMMANDLIST_NUM + GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM + GRAPHICSSYSTEM_USER_WORK,
};

// システムメモリ動作タイプ
enum SystemMemoryMode{
  SYSTEM_MEM_UNSETTLED,       // システム各メモリの前方から確保 (変動のあるメモリ）
  SYSTEM_MEM_FIX,             // システム各メモリの後方から確保（変動のないメモリ）
};

// GPUProfileRESULT
enum GpuProfileResult{
  GPU_PROFILE_BUSY_MICRO_SECOND = 1,      // このマイクロ秒単位にビジーチェックする

  // BUSY
  GPU_PROFILE_BUSY_SHADER0 = 0,           // シェーダープロセッサ0
  GPU_PROFILE_BUSY_COMMAND_VERTEX_LOAD,   // コマンドバッファおよび頂点アレイロードモジュール
  GPU_PROFILE_BUSY_RASTERIZE,             // ラスタライズ
  GPU_PROFILE_BUSY_TRIANGLE_SETUP,        // トライアングルセットアップ
  GPU_PROFILE_BUSY_FRAGMENT,              // フラグメントライティング
  GPU_PROFILE_BUSY_TEXTURE_UNIT,          // テクスチャユニット
  GPU_PROFILE_BUSY_PERFRAGMENT,           // パーフラグメントオペレーション
  GPU_PROFILE_BUSY_TEXTURE_COMB,          // テクスチャコンバイナ

  // VERTEX_SHADER_0
  GPU_PROFILE_VERTEX0_PROGCOUNTER,        // 頂点シェーダー０　プログラムカウンタ
  GPU_PROFILE_VERTEX0_CMD_STOLE,          // 頂点シェーダー０　命令の依存関係によるストールしたクロック数
  GPU_PROFILE_VERTEX0_MOVA_CMD_STOLE,     // 頂点シェーダー０　アドレスレジスタの更新によるストールしたクロック数
  GPU_PROFILE_VERTEX0_CMP_CMD_STOLE,      // 頂点シェーダー０　ステータスレジスタの更新によるストールしたクロック数
  GPU_PROFILE_VERTEX0_FETCH_MISS_STOLE,   // 頂点シェーダー０　プリフェッチのミスによりストールしたクロック数

  // VERTEX_SHADER_1
  GPU_PROFILE_VERTEX1_PROGCOUNTER,        // 頂点シェーダー０　プログラムカウンタ
  GPU_PROFILE_VERTEX1_CMD_STOLE,          // 頂点シェーダー０　命令の依存関係によるストールしたクロック数
  GPU_PROFILE_VERTEX1_MOVA_CMD_STOLE,     // 頂点シェーダー０　アドレスレジスタの更新によるストールしたクロック数
  GPU_PROFILE_VERTEX1_CMP_CMD_STOLE,      // 頂点シェーダー０　ステータスレジスタの更新によるストールしたクロック数
  GPU_PROFILE_VERTEX1_FETCH_MISS_STOLE,   // 頂点シェーダー０　プリフェッチのミスによりストールしたクロック数

  // VERTEX_SHADER_2
  GPU_PROFILE_VERTEX2_PROGCOUNTER,        // 頂点シェーダー０　プログラムカウンタ
  GPU_PROFILE_VERTEX2_CMD_STOLE,          // 頂点シェーダー０　命令の依存関係によるストールしたクロック数
  GPU_PROFILE_VERTEX2_MOVA_CMD_STOLE,     // 頂点シェーダー０　アドレスレジスタの更新によるストールしたクロック数
  GPU_PROFILE_VERTEX2_CMP_CMD_STOLE,      // 頂点シェーダー０　ステータスレジスタの更新によるストールしたクロック数
  GPU_PROFILE_VERTEX2_FETCH_MISS_STOLE,   // 頂点シェーダー０　プリフェッチのミスによりストールしたクロック数

  // VERTEX_SHADER_3
  GPU_PROFILE_VERTEX3_PROGCOUNTER,        // 頂点シェーダー０　プログラムカウンタ
  GPU_PROFILE_VERTEX3_CMD_STOLE,          // 頂点シェーダー０　命令の依存関係によるストールしたクロック数
  GPU_PROFILE_VERTEX3_MOVA_CMD_STOLE,     // 頂点シェーダー０　アドレスレジスタの更新によるストールしたクロック数
  GPU_PROFILE_VERTEX3_CMP_CMD_STOLE,      // 頂点シェーダー０　ステータスレジスタの更新によるストールしたクロック数
  GPU_PROFILE_VERTEX3_FETCH_MISS_STOLE,   // 頂点シェーダー０　プリフェッチのミスによりストールしたクロック数

  // POLYGON
  GPU_PROFILE_POLYGON_INPUT_VERTEX,       // トライアングルセットアップへの入力頂点数
  GPU_PROFILE_POLYGON_INPUT_POLYGON,      // トライアングルセットアップへの入力ポリゴン数
  GPU_PROFILE_POLYGON_OUTPUT_POLYGON,     // トライアングルセットアップへの出力ポリゴン数

  // FRAGMENT
  GPU_PROFILE_FRAGMENT_INPUT_FRAGMENT,    // 入力フラグメント数カウンタの値

  // メモリアクセスカウント（バイト数）
  GPU_PROFILE_ACCESS_VRAMA_READ,                 // PICA による VRAM A チャネルのリード
  GPU_PROFILE_ACCESS_VRAMA_WRITE,                // PICA による VRAM A チャネルのライト
  GPU_PROFILE_ACCESS_VRAMB_READ,                 // PICA による VRAM B チャネルのリード
  GPU_PROFILE_ACCESS_VRAMB_WRITE,                // PICA による VRAM B チャネルのライト
  GPU_PROFILE_ACCESS_CMD_VERTEX,                 // コマンドバッファ、頂点アレイ、インデックスアレイのリード
  GPU_PROFILE_ACCESS_TEX_UNIT,                   // テクスチャユニットによる　テクスチャメモリリード
  GPU_PROFILE_ACCESS_DEPTH_READ,                 // フラグメントオペレーション　デプスバッファ　リード
  GPU_PROFILE_ACCESS_DEPTH_WRITE,                // フラグメントオペレーション　デプスバッファ　ライト
  GPU_PROFILE_ACCESS_COLOR_READ,                 // フラグメントオペレーション　カラーバッファ　リード
  GPU_PROFILE_ACCESS_COLOR_WRITE,                // フラグメントオペレーション　カラーバッファ　ライト
  GPU_PROFILE_ACCESS_LCD_UP_DISP_READ,           // 上画面LCDによる　ディスプレイバッファ　リード
  GPU_PROFILE_ACCESS_LCD_DOWN_DISP_READ,         // 下画面LCDによる　ディスプレイバッファ　ライト
  GPU_PROFILE_ACCESS_POST_READ,                  // ポスト転送モジュールによるリード
  GPU_PROFILE_ACCESS_POST_WRITE,                 // ポスト転送モジュールによるライト
  GPU_PROFILE_ACCESS_MEMFILL0_WRITE,             // メモリフィルモージュールのチャネル０によるバッファのライト
  GPU_PROFILE_ACCESS_MEMFILL1_WRITE,             // メモリフィルモージュールのチャネル１によるバッファのライト
  GPU_PROFILE_ACCESS_READPIXCEL,                // glReadPixelsなどによるVRAMリード
  GPU_PROFILE_ACCESS_DMA_VRAM_WRITE,            // DMA転送による、VRAMライト

  GPU_PROFILE_RESULT_MAX,     // 最大値
};


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	コマンドリストセットアップ　ｲﾝﾃﾞｯｸｽ定義
//=====================================
enum CommandListSetUpIndex{

  COMMANDLIST_SETUP_MAIN,     // オフスクリーン用処理が格納されるコマンドバッファ設定
  COMMANDLIST_SETUP_UPPER,    // 上画面用処理が格納されるコマンドバッファ設定
  COMMANDLIST_SETUP_LOWER,    // 下画面用処理が格納されるコマンドバッファ設定

  COMMANDLIST_SETUP_NUM,
} ;

//-------------------------------------
///	グラフィックシステム　セットアップ
//
//Initializeを行うと、システム側で自動で以下のバッファを確保します。
//  ■buffer_modeをMODE_DOUBLE_BUFFERにした場合
//    displayBufferを立体視動作の場合6個　通常の場合4個
//    commandListを8個
//    Save用commandListを2個
//    確保します。
//
//
//  ■buffer_modeをMODE_SINGLE_BUFFERにした場合
//    displayBufferを立体視動作の場合3個　通常の場合2個
//    commandListを4個
//    Save用commandListを1個
//    確保します。
//=====================================
typedef struct {
  GraphicsSystemBufferMode  buffer_mode;        // バッファリングモード
  b32                       useUpperRight;      // 立体視するか?　一度でも立体視にするときはtrue
  b32                       stereoOn;           // 立体視On状態で初期化するか。
  DisplayBufferSetUp        displaySetup[DISPLAY_BOTH_MAX];   // ティスプレイバッファ初期化データ 順番重要です。
  RenderBufferSetUp         frameSetup[DISPLAY_BOTH_MAX];   // フレームバッファ初期化データ
  CommandListSetUp          commandSetup[COMMANDLIST_SETUP_NUM];// コマンドリスト初期化データ
  CommandListSetUp          saveCommandSetup;   // セーブ用コマンドリスト初期化データ（コマンドの再利用に使用）
  FrameBufferClearColor     clearColor[DISPLAY_NORMAL_MAX];   // フレームバッファクリアカラー
  FrameBufferClearFlag      clearFlag[DISPLAY_NORMAL_MAX];   // フレームバッファクリアフラグ
} GraphicsSystemSetUp;

// 基本セットアップ情報
extern const GraphicsSystemSetUp GraphicsSystemSetupDefault;


//-------------------------------------
///	バッファ管理クラス
//=====================================
class BufferControl;

//-------------------------------------
///	グラフィックメモリ管理クラス
//=====================================
class MemoryControl;

//-----------------------------------------------------------------------------
/**
 *					グラフィックスシステム　クラス宣言
*/
//-----------------------------------------------------------------------------
class GraphicsSystem : public base::Singleton<GraphicsSystem>
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(GraphicsSystem);

public:
  // 立体視の視差の補正値の定義
  static const f32 PARALLAX_DEPTH_RANGE_ZERO = 0.0f;  // 視差0(立体視なし)
  static const f32 PARALLAX_DEPTH_RANGE_ONE  = 1.0f;  // 視差1(立体視標準)

public:
  //-----------------------------------------------------------------------------
  /**
   *    @brief  デフォルトセットアップデータを使用した生成
   *
   *    @param  heapAllocator       GraphicsSystemが使用するヒープメモリアロケータ
   *    @param  deviceAllocator     GraphicsSystemが使用するデバイスメモリアロケータ
   *    @param  systemVramASize      システムが使用するVRAM-Aサイズ
   *    @param  systemVramBSize      システムが使用するVRAM-Bサイズ
   */
  //-----------------------------------------------------------------------------
  GraphicsSystem( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator, u32 systemVramASize = 0x300000, u32 systemVramBSize = 0x300000 );
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
  GraphicsSystem( heap::NwAllocator* allocator, heap::NwAllocator* deviceAllocator, const GraphicsSystemSetUp* setup, u32 systemVramASize = 0x300000, u32 systemVramBSize = 0x300000 );

  //-----------------------------------------------------------------------------
  /**
   *    @brief  デストラクタ
   */
  //-----------------------------------------------------------------------------
  ~GraphicsSystem();

  //-----------------------------------------------------------------------------
  /**
   *    @brief  グラフィックスシステムが初期化ずみかチェック
   *
   *    @retval true  初期化済み
   *    @retval false 未初期化
   */
  //-----------------------------------------------------------------------------
  static bool IsInitialize( void );

  //-----------------------------------------------------------------------------
  /**
   *    @brief  ゲームとして、立体視を使用するのかの設定
   */
  //-----------------------------------------------------------------------------
  static void SetGameStereoFlag( bool flag );
  static bool IsGameStereoFlag( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  画面構成を変更する。
   *
   *	@param	setup 変更する画面構成のセットアップ情報
   *
   *	フレームバッファ、ディスプレイバッファ、コマンドリスト全て破棄します。
   *	この処理は、アプリケーションから違うアプリケーションへの画面転換時などで
   *	使われることを想定しています。
   */
  //-----------------------------------------------------------------------------
  void ChangeSetUp( const GraphicsSystemSetUp* setup, bool appUpperRightMode = false );


  //----------------------------------------------------------------------------
  /**
   *	@brief  アプリケーション用のデバイスメモリ、VRAM管理を初期化
   *
   *	@param	deviceAllocator           アプリケーションのグラフィックで使用するデバイスメモリのアロケータ
   *
   *	VRAM領域には、SystemMemoryで予約されている部分以外の全てが割り当てられます。
   *
   *	この関数を呼ぶと、アプリケーションメモリに切り替わります。
   *
   */
  //-----------------------------------------------------------------------------
  void InitializeApplicationMemory( heap::NwAllocator* deviceAllocator );


  //----------------------------------------------------------------------------
  /**
   *	@brief  アプリケーション用のデバイスメモリ、VRAM管理を破棄
   *
   *	VRAMに解放忘れがあるとAssartでとまります。
   *
   *	これ以降で、確保されるメモリは、システムメモリから確保されます。
   *
   *	@retval true  破棄完了
   *	@retval false 破棄未完了
   */
  //-----------------------------------------------------------------------------
  void FinalizeApplicationMemory( void );


  //-----------------------------------------------------------------------------
  /**
   *    アプリケーション　ごとに、立体視のメモリを確保、解放する際に使用する関数群
   */
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *    @brief  指定セットアップデータを使用した生成  アプリケーション個別に立体視バッファ確保する版
   *
   *    @param  heapAllocator       ヒープメモリアロケータ
   *    @param  deviceAllocator     デバイスメモリアロケータ
   *    @param  setup               セットアップ情報
   *    @param  appUpperRightMode   右目のバッファを生成する。　TRUE
   *    @param  systemVramASize      システムが使用するVRAM-Aサイズ
   *    @param  systemVramBSize      システムが使用するVRAM-Bサイズ
   */
  //-----------------------------------------------------------------------------
  GraphicsSystem( heap::NwAllocator* allocator, heap::NwAllocator* deviceAllocator, const GraphicsSystemSetUp* setup, bool appUpperRightMode, u32 systemVramASize = 0x300000, u32 systemVramBSize = 0x300000 );

  //----------------------------------------------------------------------------
  /**
   *	@brief  アプリケーション側で、拡張した立体視バッファがあるかチェック
   */
  //-----------------------------------------------------------------------------
  bool IsInitializedAppUpperRightBuffer( void ) const;
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  アプリケーション側で、拡張で、立体視をＯＮにする。
   */
  //-----------------------------------------------------------------------------
  void InitializeAppUpperRightBuffer( heap::NwAllocator* deviceAllocator );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  アプリケーション側で、拡張で確保したバッファを解放する。
   */
  //-----------------------------------------------------------------------------
  void ReqDeleteAppUpperRightBuffer( bool is_stereo_off = true );

  //----------------------------------------------------------------------------
  /**
   *	@brief  アプリケーション側で、拡張で確保したバッファが解放されたかをチェック
   *	@return
   */
  //-----------------------------------------------------------------------------
  bool IsDeleteAppUpperRightBuffer( void ) const;
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
  /**
   *	@brief  システム内のメモリ確保先をシステムメモリに切り替える。
   *
   *	@param  mode    システムメモリのモード
   *
   *	SystemMemoryMode
   *   SYSTEM_MEM_UNSETTLED,       // 変動のあるメモリ (前方から確保）
   *   SYSTEM_MEM_FIX,             // 変動のないメモリ（後方から確保）
   */
  //-----------------------------------------------------------------------------
  void BindSystemMemory( SystemMemoryMode mode );

  //----------------------------------------------------------------------------
  /**
   *	@brief  システム内のメモリ確保先をアプリケーションメモリに戻す。
   */
  //-----------------------------------------------------------------------------
  void UnBindSystemMemory( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  メモリ確保先がシステムメモリになっているか確認する。
   */
  //-----------------------------------------------------------------------------
  bool IsBindSystemMemory(void) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  立体視のON/OFFをリクエストします。 実際の反映は、VSyncAfterになります。
   *
   *	@param	stereo_on   ON:true   OFF:false
   */
  //-----------------------------------------------------------------------------
  void SetStereoModeReq( bool stereo_on );
#if GFL_DEBUG
  //立体視が有効の時しかモードが切り替えられないのでデバッグ用に用意
  void SetStereoModeReqDebug( bool stereo_on );
#endif
  // 立体視つかえる？
  bool IsUseRightDisplay( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  シングルバッファーにするフラグ
   *
   *	@param	single_buffer_on   ON:true   OFF:false
   */
  //-----------------------------------------------------------------------------
  void SetSingleBufferReq( bool single_buffer_on );

  //----------------------------------------------------------------------------
  /**
   *	@brief    通常レンダリング開始処理
   *
   *	@param  msk   描画を行う面設定
   */
  //-----------------------------------------------------------------------------
  void StartRendering( DisplayMsk msk = DISPLAY_MSK_BOTH );

  //----------------------------------------------------------------------------
  /**
   *	@brief    レンダリング後処理
   *
   *  @param    parallaxDepthRange  立体視の視差の補正値
   *	                               (gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_か
   *                                  gfl::grp::g3d::StereoCameraのGetDepthRange関数の戻り値を渡して下さい。)
   *
   *	＊カラー情報のディスプレイバッファ転送、スワップするディスプレイバッファ、コマンドリストIDの保存などを行います。
   *	＊StartRenderingのmskで指定した面のみ更新します。
   */
  //-----------------------------------------------------------------------------
  void EndRendering( f32 parallaxDepthRange = PARALLAX_DEPTH_RANGE_ONE );


  //----------------------------------------------------------------------------
  /**
   *	@brief  各画面のコマンドバッファの実行順序指定付き　レンダリング後処理
   *
   *  @param  first     1番にコマンド実行をする面
   *  @param  second    2番にコマンド実行をする面
   *  @param  third     3番にコマンド実行をする面
   *  @param  parallaxDepthRange  立体視の視差の補正値
   *	                             (gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_か
   *                                gfl::grp::g3d::StereoCameraのGetDepthRange関数の戻り値を渡して下さい。)
   */
  //-----------------------------------------------------------------------------
  void EndRenderingEx( DisplayType first, DisplayType second, DisplayType third, f32 parallaxDepthRange = PARALLAX_DEPTH_RANGE_ONE );

  //----------------------------------------------------------------------------
  /**
   *	@brief    スワップバッファー処理
   *
   *	*この関数を行った後、直ぐにVSyncWaitを行ってください。
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
  void SwapBuffer( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  コマンドリストの実行が完了するまで待つ
   */
  //-----------------------------------------------------------------------------
  void WaitDone( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief    VSync待ち　処理
   *
   *	@param  display   ディスプレイタイプ  （VSyncを待つ画面 DISPLAY_UPPER or DISPLAY_LOWER or DISPLAY_BOTH_MAX ）
   */
  //-----------------------------------------------------------------------------
  static void WaitVSync( DisplayType display = DISPLAY_BOTH_MAX );

  //----------------------------------------------------------------------------
  /**
   *	@brief  スワップバッファ　リクエストが出ているかチェック
   *
   *	@retval true  出ている。    （WaitVSyncしてからVSyncAfterを呼ぶ必要がある。）
   *	@retval false 出ていない。
   */
  //-----------------------------------------------------------------------------
  bool IsSwapBuffer( void ) const ;

  //----------------------------------------------------------------------------
  /**
   *	@brief    VSync後処理
   *
   *	*この関数を呼ぶ前に必ずVSyncWaitを行ってください。
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
  void VSyncAfter( bool skip = false );

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
  void ClearBuffer( DisplayType display, b32 b_color, const gfl::grp::ColorF32& clear_color, b32 b_depth, f32 clear_depth, b32 b_stencil = false, u8 clear_stencil = 0 );

  //----------------------------------------------------------------------------
  /**
   *	@brief    システムが操作しているカレントのディスプレイバッファIDを取得
   *
   *	@param    display   面指定
   *
   *	@return   カレントのディスプレイバッファID
   */
  //-----------------------------------------------------------------------------
  DisplayBufferID GetCurrentDisplayBufferID( DisplayType display )const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    システムが操作しているカレントのフレームバッファIDを取得
   *
   *	@param    display   面指定
   *
   *	@return   カレントのフレームバッファID
   */
  //-----------------------------------------------------------------------------
  FrameBufferID GetCurrentFrameBufferID( DisplayType display )const;
  RenderTarget* GetCurrentFrameBuffer( DisplayType display )const;

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
  CommandListID GetCurrentCommandListID( DisplayType displayType = DISPLAY_NONE )const;

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
  bool BindCurrentCommandList( DisplayType displayType = DISPLAY_NONE );

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
  bool BindCurrentCommandList( const gfl::grp::RenderTarget* renderTarget );

  //----------------------------------------------------------------------------
  /**
   *	@brief    システムが操作しているカレントのセーブ用コマンドリストIDを取得
   *
   *	@return   カレントのセーブ用コマンドリストID
   */
  //-----------------------------------------------------------------------------
  CommandListID GetCurrentSaveCommandListID(void)const;


  //-----------------------------------------------------------------------------
  /**
   *  コマンドキャッシュ（セーブしたコマンドを再利用）を行う関数
   *
   *  使用用途　立体視表示での左右目用レンダリングに使用するコマンドを再利用する。
   *
   *  使用方法
   *    立体視では、カメラの情報のコマンド以外は右目・左目で同じコマンドになるため、
   *    共通部分をはじめにSave用コマンドリストに保存し、左右のレンダリングに再利用することが可能となります。
   *
   *    1.共通部分のコマンドを蓄積
   *      StartCommandSave();
   *      {
   *        (カメラの情報は設定しない）
   *        Scene->Draw();    // シーンの描画
   *      }
   *      EndCommandSave();
   *
   *    2.実際の描画
   *
   *      StartRendering();
   *      {
   *        // 左目の描画
   *        LeftCamera->Set();  // 左目用カメラ設定のコマンドを発行
   *        LeftRenderTarget->Set();// レンダリングターゲットの設定
   *        ReuseCommandSave(); // 共通部分のコマンドを再利用
   *
   *        // 右目の描画
   *        RightCamera->Set();  // 右目用カメラ設定のコマンドを発行
   *        RightRenderTarget->Set();// レンダリングターゲットの設定
   *        ReuseCommandSave(); // 共通部分のコマンドを再利用
   *      }
   *      EndRendering();
   */
  //-----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  /**
   *	@brief    セーブ用コマンドリストへのコマンド蓄積開始
   */
  //-----------------------------------------------------------------------------
  void StartCommandSave( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief    セーブ用コマンドリストへのコマンド蓄積終了
   */
  //-----------------------------------------------------------------------------
  void EndCommandSave( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief    セーブ用コマンドリストへ蓄積された最新の内容をカレントコマンドリストに設定
   */
  //-----------------------------------------------------------------------------
  void ReuseCommandSave( void );

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
  void SetClearColor( DisplayType display, f32 red, f32 green, f32 blue, f32 alpha );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief     デフォルトクリアフラグの設定
   * @param display     面指定　（DISPLAY_UPPER or DISPLAY_LOWER）
   * @param color       カラーバッファクリア  有効　or 無効
   * @param depth       深度バッファクリア  有効　or 無効
   * @param stencil     ステンシルバッファクリア  有効　or 無効
   */
  //-----------------------------------------------------------------------------
  void SetClearColorFlag( DisplayType display, b32 color, f32 depth, f32 stencil );

  //----------------------------------------------------------------------------
  /**
   *	@brief    ディスプレイバッファマネージャの取得
   */
  //-----------------------------------------------------------------------------
  DisplayBufferManager* GetDisplayBufferManager( void )const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    フレームバッファマネージャの取得
   */
  //-----------------------------------------------------------------------------
  FrameBufferManager* GetFrameBufferManager( void )const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    コマンドリストマネージャの取得
   */
  //-----------------------------------------------------------------------------
  CommandListManager* GetCommandListManager( void )const;



  //----------------------------------------------------------------------------
  /**
   *	@brief  グラフィックスセットアップ数
   *
   *	@return グラフィックスセットアップデータ
   */
  //-----------------------------------------------------------------------------
  const GraphicsSystemSetUp& GetSetup( void ) const { return m_setup; }


  //----------------------------------------------------------------------------
  /**
   *	@brief  コマンドリスト実行回数（回り込み注意！）
   *
   *	@return コマンドリスト実行回数
   */
  //-----------------------------------------------------------------------------
  u32 GetCommandListDoneCount( void ) const ;


  //----------------------------------------------------------------------------
  /**
   *	@brief    ReadPixelsに必要なメモリサイズを取得する。
   */
  //-----------------------------------------------------------------------------
  u32 GetReadPixelsMemorySize( RenderColorFormat colorFormat, u32 width, u32 height ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    カラーバッファのピクセル情報の取得Request
   *
   *	@param  memoryTop     上画面左格納先　（不要な場合NULL）
   *	@param  memoryBottom  下画面格納先  （不要な場合NULL）
   *	@param  type          フレームバッファタイプ  (gfl::grp::RenderTarget::RENDERBUFFER_TYPE_COLOR or gfl::grp::RenderTarget::RENDERBUFFER_TYPE_DEPTH)
   *	@param  memoryTopRight上画面右格納先　（不要な場合NULL）
   */
  //-----------------------------------------------------------------------------
  // BLOCKモード
  void RequestReadPixels( void* memoryTop, void* memoryBottom, RenderTarget::RenderBufferType type = RenderTarget::RENDERBUFFER_TYPE_COLOR );
  void RequestReadPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, RenderTarget::RenderBufferType type = RenderTarget::RENDERBUFFER_TYPE_COLOR );
  // リニアモード
  void RequestReadLinerPixels( void* memoryTop, void* memoryBottom, RenderTarget::RenderBufferType type = RenderTarget::RENDERBUFFER_TYPE_COLOR );
  void RequestReadLinerPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, RenderTarget::RenderBufferType type = RenderTarget::RENDERBUFFER_TYPE_COLOR );

  //----------------------------------------------------------------------------
  /**
   *	@brief    カラーバッファのピクセル情報の取得完了待ち
   *
   *	@retval true    完了
   *	@retval false   途中
   */
  //-----------------------------------------------------------------------------
  bool WaitReadPixels( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief    VRAM残り領域を取得
   */
  //-----------------------------------------------------------------------------
  u32 GetVramAAllocatableSize( void ) const;
  u32 GetVramBAllocatableSize( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief    メモリ確保関数
   */
  //-----------------------------------------------------------------------------
  static void* AllocMemory( MemoryArea area, MemoryUseType type, u32 size );
  static void FreeMemory( MemoryArea area, void* p_memory );
  
  static u32 GetAlignment( MemoryUseType type );//@fix 有泉さん要望：ＶＲＡＭアドレス指定配置対応



#if GFL_DEBUG
  //----------------------------------------------------------------------------
  /**
   *	@brief    コマンドリストの状態をダンプ
   */
  //-----------------------------------------------------------------------------
  void DumpCommandList(void)const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  GpuProfileフラグを設定
   *
   *	@param	flag  フラグ
   */
  //-----------------------------------------------------------------------------
  void SetGpuProfileFlag( bool flag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  GpuProfileフラグを取得
   */
  //-----------------------------------------------------------------------------
  bool IsGpuProfileFlag( void ) const;

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
  u32 GetGpuProfileResult( GpuProfileResult result ) const;

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
  const u32* GetGpuProfileResultPtr( GpuProfileResult result ) const;
  
#if GFL_DEBUG    
  static u32 GetDrawFrameCount();
private:
  static u32 s_DrawFrameCount;
#endif

#endif

private:


private:

  // アロケータの保存
  gfl::heap::NwAllocator* m_heapAllocator;
  gfl::heap::NwAllocator* m_deviceAllocator;

  // セットアップ情報
  GraphicsSystemSetUp m_setup;

  // バッファコントロールクラス
  BufferControl*        m_bufferControl;

  // メモリ管理部
  static MemoryControl* m_memoryControl;

  // 初期化フラグ
  static bool m_Initialize;
  static bool m_GameStereoFlag;
private:

  void Initialize( heap::NwAllocator* allocator, heap::NwAllocator* deviceAllocator, u32 systemVramASize, u32 systemVramBSize, const GraphicsSystemSetUp* setup, bool appUpperRight );
  void Finalize( void );

  static void* AllocateGraphicsMemory(GLenum area, GLenum aim, GLuint id, GLsizei size);
  static void DeallocateGraphicsMemory(GLenum area, GLenum aim, GLuint id, void* addr);


};



} /* namespace end grp */
} /* namespace end gfl */


/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( gfl::grp::GraphicsSystem );


#endif // __GFL_GRAPHICSSYSTEM_H__
