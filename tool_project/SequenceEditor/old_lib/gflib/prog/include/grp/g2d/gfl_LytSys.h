#ifndef __GFL_LYTSYS_H__
#define __GFL_LYTSYS_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl_LytSys.h
 *  @brief  レイアウトシステム
 *  @author tomoya takahashi
 *  @date   2010.10.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nn/math.h>

#include <base/gfl_Singleton.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_math.h>
#include <gfl_Fs.h>
#include <str/gfl_Font.h>

#include <grp/g2d/gfl_G2dEnv.h>
#include <grp/g2d/gfl_LytRes.h>

#include <debug/gfl_DebugPrint.h>





namespace gfl {
namespace grp {

//-------------------------------------
/// 前方宣言
//=====================================
class GraphicsSystem;
namespace g2d {
class LytSysBase;
class LytWkManager;
class LytWkBase;
class LytWk;
}

namespace g2d {

//-----------------------------------------------------------------------------
/**
 *        デバッグ出力設定
 */
//-----------------------------------------------------------------------------
#define GFL_LYTSYS_DEBUGPRINT_LEVEL1 (gfl::debug::PRINT_FLAG_LEVEL1|gfl::debug::PRINT_FLAG_CATEGORY_G2D)


//*****************************************************************************
/**
 *          レイアウトシステムクラス
*/
//*****************************************************************************
//----------------------------------------------------------------------------
/**
 *  @brief  定数定義
 */
//-----------------------------------------------------------------------------
typedef enum {

  // リソースID　不正値
  LYTSYS_RESID_ERROR = 0xffff,

  // アニメーション登録失敗
  LYTSYS_ANIMEINDEX_ERROR = 0xffff,

  // ペインヒットチェック失敗
  LYTSYS_PANEINDEX_ERROR = 0xffff,

  // LYTWKグループID　ALL
  LYTSYS_LYTWK_GROUPID_ALL = 0xffff,

  // デフォルトカメラポジション
  LYTSYS_DEFAULT_CAMERA_UPPER_POS_Y = 0,
  LYTSYS_DEFAULT_CAMERA_LOWER_POS_Y = -290,


  ENUM_FORCE_DWORD(LytSysEnum)
} LytSysEnum;


//----------------------------------------------------------------------------
/**
 *  @brief  レイアウトリソースID型
 */
//-----------------------------------------------------------------------------
typedef u32 LytResID;

//----------------------------------------------------------------------------
/**
 *  @brief  レイアウトリソース　アーカイブインデックス型
 */
//-----------------------------------------------------------------------------
typedef u32 LytArcIndex;

//----------------------------------------------------------------------------
/**
 *  @brief  レイアウトリソース　ペイン、マテリアル、グループインデックス型
 */
//-----------------------------------------------------------------------------
typedef u32 LytPaneIndex;

//----------------------------------------------------------------------------
/**
 *  @brief  LYTWKグループID　型
 */
//-----------------------------------------------------------------------------
typedef u16 LytWKGroupID;

//----------------------------------------------------------------------------
/**
 *  @brief  セットアップ構造体定義
 */
//-----------------------------------------------------------------------------
typedef struct {
  u16 lyt_resource_max;   // 管理リソース最大数
  u16 lyt_wk_max;         // 管理LytWk最大数
} LytSysSetUp;


//----------------------------------------------------------------------------
/**
 *  @brief  LytWkセットアップ構造体定義
 */
//-----------------------------------------------------------------------------
typedef struct {
  gfl::math::VEC3   pos;        // 初期位置
  gfl::math::VEC2   scale;      // 初期拡大値
  gfl::math::VEC3   rotate;     // 初期回転角度
  u16               priority;   // 表示プライオリティ
  u16               groupID;    // グループID
} LytWkSetUp;


//-------------------------------------
/// 上下画面に描画するためのデフォルト設定
//（UserEnvを使用していない場合）
//=====================================
extern const LytWkSetUp c_LytWkUpperDefaultSetUp;
extern const LytWkSetUp c_LytWkLowerDefaultSetUp;


//-------------------------------------
/// レイアウトコアシステム
//=====================================
typedef nw::lyt::Layout   LytCore;

//-------------------------------------
/// ペイン　操作用
//=====================================
typedef nw::lyt::Pane             LytPane;
typedef nw::lyt::PaneLinkList     LytPaneList;  // リンクリスト

//-------------------------------------
/// ペイン　種類別
//=====================================
typedef nw::lyt::Picture  LytPicture;   // 画像の表示を行うペイン
typedef nw::lyt::TextBox  LytTextBox;   // 文字列の表示を行うペイン
typedef nw::lyt::Window   LytWindow;    // Windowの表示を行うペイン
typedef nw::lyt::Bounding LytBounding;  // BoundingBoxとして使用するペイン

//-------------------------------------
/// マテリアル情報
//=====================================
typedef nw::lyt::Material LytMaterial;  // マテリアル


/// デフォルトフォントの登録インデックス最大値
const u32 LYTSYS_DEFAULT_FONT_INDEX_MAX = LYTRES_DEFAULT_FONT_MAX;

//----------------------------------------------------------------------------
/**
 *  @brief  レイアウトシステム　クラス　　シングルトンクラス
 *
 */
//-----------------------------------------------------------------------------
class LytSys
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(LytSys);


public:


  //----------------------------------------------------------------------------
  /**
   *	@brief  レイアウトシステムの初期化
   *	        *ゲーム開始時に一度だけ設定してください。
   */
  //-----------------------------------------------------------------------------
  static void InitializeSystem( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  //----------------------------------------------------------------------------
  /**
   *	@brief  レイアウトシステムの破棄
   */
  //-----------------------------------------------------------------------------
  static void FinalizeSystem( void );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief  Defaultフォント設定
   *      　  ＊ゲーム開始時に一度だけ設定してください。
   *
   *  @param  font                  フォントデータ
   *  @param  name                  リソース名　(例　simple.bcfnt)　レイアウトがアクセスするための名称
   *  @param  index                 インデックス（0 ～　LYTSYS_DEFAULT_FONT_INDEX_MAX)
   *  *ポインタを保持します。
   *  
   */
  //-----------------------------------------------------------------------------
  static void SetDefaultFont( gfl::str::Font* font, const char* name, u32 index = 0 );
  //-----------------------------------------------------------------------------
  /**
   *  @brief  Defaultフォント解除
   *  @param  index                 インデックス
   */
  //-----------------------------------------------------------------------------
  static void ResetDefaultFontInfo( u32 index );



  
  //-----------------------------------------------------------------------------
  /**
  *     コンストラクタ・デストラクタ
  */
  //-----------------------------------------------------------------------------
  LytSys(const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator);
  LytSys( heap::HeapBase* p_heap );   // 非同期初期化モード（デフォルトリソースのスレッド読み込みに対応）
  ~LytSys();

  
  //----------------------------------------------------------------------------
  /**
   *	@brief  非同期初期化（　LytSys（void）で初期化したときのみ必要　）
   *
   */
  //-----------------------------------------------------------------------------
  void InitializeAsync( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );

  //----------------------------------------------------------------------------
  /**
   *	@brief  非同期初期化完了まち
   *	@retval true  非同期初期化
   *	@retval false 非同期初期化中
   */
  //-----------------------------------------------------------------------------
  bool InitializeAsyncWait( void );


  //----------------------------------------------------------------------------
  /**
   *	@brief  初期化が完了しているかチェック
   *	@retval true  完了
   *	@retval false 途中
   */
  //-----------------------------------------------------------------------------
  bool IsInitialized( void ) const;
  

  //----------------------------------------------------------------------------
  /**
   *  @brief    オート描画フラグがONになっている全Wkのデータ更新処理
   *
   *  @param  lytwkGroupID    Updateを行うLYTWKグループID (ペイングループIDとは違うので注意！）
   */
  //-----------------------------------------------------------------------------
  void UpdateAll( LytWKGroupID lytwkGroupID = LYTSYS_LYTWK_GROUPID_ALL );

  //----------------------------------------------------------------------------
  /**
   *  @brief    オート描画フラグがONになっている全Wkの描画処理
   *
   *  @param    graphics          グラフィックスシステム
   *  @param    drawtype          表示面定義
   *  @param    renderTarget      レンダリングターゲットのフレームバッファ
   *  @param    rightRenderTarget 右目用レンダリングターゲット　（DISPLAY_UPPERのときのみ参照）
   *  @param    lytwkGroupID      描画を行うLYTWKグループID(ペイングループIDとは違うので注意！）
   */
  //-----------------------------------------------------------------------------
  void DrawAll( gfl::grp::GraphicsSystem* graphics, DisplayType drawtype, RenderTarget* renderTarget, RenderTarget* rightRenderTarget = NULL, LytWKGroupID lytwkGroupID = LYTSYS_LYTWK_GROUPID_ALL );

  //----------------------------------------------------------------------------
  /**
   *  @brief  UpperLowerに対応したグローバル描画環境データ
   *
   *  @param  描画面　UPPER　LOWER
   *
   *  @return 描画環境データ
   *
   *   *通常LytWKはこのEnvを使用して描画されています。
   *    もちろん、個々のLytWkに別のEnvを設定することも可能です。
   *    LytWk::SetUserEnv( gfl::grp::g2d::Env& upperEnv, gfl::grp::g2d::Env& lowerEnv );
   */
  //-----------------------------------------------------------------------------
  Env* GetGlobalEnv( DisplayType drawtype );

  //----------------------------------------------------------------------------
  /**
   *  @brief  レイアウトリソースの読み込み（圧縮データ対応版）
   *
   *  @param  devAllocator デバイスアロケータ
   *  @param  arcFile   アーカイブファイル
   *  @param  dataID    データID
   *  @param  comp      圧縮データフラグ　true = 圧縮データ
   *  @param  useDefaultFont  Defaultフォントを使用するか？
   *  @param  addTextureNum   後から追加するテクスチャ数
   *
   *  @retval リソースID
   *  @retval LTYSYS_RESID_ERROR  リソースが登録できなかった。
   */
  //-----------------------------------------------------------------------------
  LytResID LoadLytRes( heap::NwAllocator* devAllocator, fs::ArcFile* arcFile, u32 dataID, bool comp, bool useDefaultFont, u32 addTextureNum );

  //----------------------------------------------------------------------------
  /**
   *  @brief  レイアウトリソースの読み込み（非圧縮データ）
   *
   *  @param  devAllocator    デバイスアロケータ
   *  @param  arcFile         アーカイブファイル
   *  @param  dataID          データID
   *  @param  useDefaultFont  Defaultフォントを使用するか？
   *  @param  addTextureNum   後から追加するテクスチャ数
   *
   *  @retval リソースID
   */
  //-----------------------------------------------------------------------------
  LytResID LoadLytRes( heap::NwAllocator* devAllocator, fs::ArcFile* arcFile, u32 dataID, bool useDefaultFont=true, u32 addTextureNum = 0 );

  //----------------------------------------------------------------------------
  /**
   *  @brief  レイアウトリソースの読み込み（圧縮データ）
   *
   *  @param  devAllocator デバイスアロケータ
   *  @param  arcFile   アーカイブファイル
   *  @param  dataID    データID
   *  @param  useDefaultFont  Defaultフォントを使用するか？
   *  @param  addTextureNum   後から追加するテクスチャ数
   *
   *  @retval リソースID
   *  @retval LTYSYS_RESID_ERROR  リソースが登録できなかった。
   */
  //-----------------------------------------------------------------------------
  LytResID LoadLytResComp( heap::NwAllocator* devAllocator, fs::ArcFile* arcFile, u32 dataID, bool useDefaultFont=true, u32 addTextureNum = 0 );


  //----------------------------------------------------------------------------
  /**
   *  @brief  バイナリデータ直接渡し。
   *
   *  @param  devAllocator    デバイスアロケータ
   *  @param  cp_buff          データ  (内部では、破棄しません。）
   *  @param  useDefaultFont  Defaultフォントを使用するか？
   *  @param  addTextureNum   後から追加するテクスチャ数
   *
   *  @retval リソースID
   */
  //-----------------------------------------------------------------------------
  LytResID LoadLytRes( heap::NwAllocator* devAllocator, const void * cp_buff, bool useDefaultFont=true, u32 addTextureNum = 0 );

  //----------------------------------------------------------------------------
  /**
   *  @brief  レイアウトリソースの破棄
   *
   *  @param  resID     破棄するリソースID
   */
  //-----------------------------------------------------------------------------
  void ReleaseLytRes( LytResID resID );

  //----------------------------------------------------------------------------
  /**
   *  @brief  dstIDのリソースにsrcIDのテクスチャリソースをすべて追加する。
   *
   *  @param  srcID     共有テクスチャデータが入っているリソースID
   *  @param  dstID     共有テクスチャデータを追加するリソースID
   *
   * ＊追加されるテクスチャの枚数をLoadLytRes関数のaddTextureNumに渡して管さい。
   * ＊dstIDのリソースの前にsrcIDを破棄してはいけません。
   *
   */
  //-----------------------------------------------------------------------------
  void RegistTextureRes( LytResID srcID, LytResID dstID, gfl::grp::MemoryArea area = gfl::grp::MEMORY_AREA_FCRAM );


  // ここでメモリ配置しなかったテクスチャは、デバイスメモリに配置されます。
  //-----------------------------------------------------------------------------
  /**
   *  @brief  リソースID内にあるテクスチャをすべてメモリに配置する
   *
   *  @param  resID   リソースID
   *  @param  area    転送先メモリ　（FCRAM/VRAMA/VRAMB）
   *
   *  2011/05/31 VRAM FCRAM全て対応したようです。
   */
  //-----------------------------------------------------------------------------
  void TransferAllTexture( LytResID resID, gfl::grp::MemoryArea area );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  リソースID内にあるテクスチャをすべてメモリに配置する
   *
   *  @param  resID         リソースID
   *  @param  texArcIndex   アーカイブ内のテクスチャインデックス
   *  @param  area          転送先メモリ　（FCRAM/VRAMA/VRAMB）
   *  2011/05/31 VRAM FCRAM全て対応したようです。
   */
  //-----------------------------------------------------------------------------
  void TransferIndexTexture( LytResID resID, LytArcIndex texArcIndex, gfl::grp::MemoryArea area );


  //-----------------------------------------------------------------------------
  /**
   *  @brief  リソースIDのリソースを取得する。
   *
   *  @param  resID         リソースID
   *
   *  @retval リソースAccessor
   */
  //-----------------------------------------------------------------------------
  LytRes* GetResource( LytResID resID ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  格納しているリソースの総数をチェック
   *
   *	@retval リソースの総数
   */
  //-----------------------------------------------------------------------------
  u32 GetUseResourceNum( void ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  レイアウトワーク生成
   *
   *  @param  p_heap            ヒープアロケータ
   *  @param  p_device          デバイスヒープアロケータ
   *  @param  resID             レイアウトリソースID
   *  @param  lytDataID         resID内のレイアウトデータID
   *  @param  anmResIDTbl       アニメーションデータIDテーブル      アニメのない場合はNULL
   *  @param  anmTblNum         アニメーションデータIDテーブル数    アニメのない場合は0
   *  @param  anmMax            登録アニメーション最大数（※）
   *  @param  wkdata            レイアウトワーク基本情報
   *  @param  anmBind           アニメーションのバインドを行う。
   *  @param  createPaneTbl     ペインテーブル生成
   *
   *  @note
   *  （※）anmMax = 登録アニメーション最大数について
   *        共有アニメーションを利用しない場合、anmTblNumと同じ数を指定してください。
   *        共有アニメーションを利用する場合、animTblNum + 共有アニメーションの数 を指定してください。
   *  アロケーター指定版を使用すると、LytWk関連のヒープを全て指定したヒープを使用して確保するようになります。
   *
   *  @retval レイアウトワーク
   *  @retval NULL              生成失敗
   */
  //-----------------------------------------------------------------------------
  LytWk* CreateLytWk( LytResID resID, LytArcIndex lytDataID, const LytArcIndex* anmDataIDTbl, u32 anmTblNum, u32 anmMax, const LytWkSetUp* setup, b32 anmBind = true, b32 createPaneTbl = false, b32 createPaneTblWithHash = false );
  LytWk* CreateLytWk( gfl::heap::NwAllocator* p_heap, gfl::heap::NwAllocator* p_device, LytResID resID, LytArcIndex lytDataID, const LytArcIndex* anmDataIDTbl, u32 anmTblNum, u32 anmMax, const LytWkSetUp* setup, b32 anmBind = true, b32 createPaneTbl = false, b32 createPaneTblWithHash = false );
  //
  LytWk* BuildLytWk( gfl::heap::NwAllocator* p_heap, gfl::heap::NwAllocator* p_device, LytResID resID, LytArcIndex lytDataID, u32 anmMax, const LytWkSetUp* setup, b32 createPaneTbl=false, b32 createPaneTblWithHash=false );
  void   BuildAnimation( LytWk* wk, LytResID resID, const LytArcIndex* anmDataIDTbl, u32 anmTblNum, b32 anmBind=true, b32 createPaneTblWithHash=false );

  //----------------------------------------------------------------------------
  /**
   * @brief アニメーション生成
   *
   * @param lytWk                  対象レイアウトワーク
   * @param resID                  リソースID
   * @param anmDataIDTbl           アニメーションデータ
   * @param snmOffset              アニメーションテーブル参照オフセット
   * @param anmTblNum              アニメーションテーブル数
   * @param anmBind                アニメーションをバインドするならtrueを指定する
   * @param createPaneTblWithHash  生成時にハッシュテーブルを利用するならtrueを指定する
   */
  //----------------------------------------------------------------------------
  void CreateAnimation(  LytWk* lytWk, LytResID resID, const LytArcIndex* anmDataIDTbl, u32 anmOffset, u32 anmTblNum, b32 anmBind, b32 createPaneTblWithHash );

  //----------------------------------------------------------------------------
  /**
   * @brief レイアウトワークをレイアウトマネージャにリンクさせる
   *
   * @param lytWk 追加対象となるレイアウトワーク
   */
  //----------------------------------------------------------------------------
  void AddLinkWithLayoutWork( LytWk* lytWk );

  //----------------------------------------------------------------------------
  /**
   *  @brief  レイアウトワーク破棄
   *
   *  @param  lytResID  レイアウトリソースID
   *  @param  anmResID  アニメーションリソースID
   *  @param  wkdata    レイアウトワーク基本情報
   */
  //-----------------------------------------------------------------------------
  void RemoveLytWk( LytWk* lytWk );

  //----------------------------------------------------------------------------
  /**
   *	@brief  生成されたレイアウトワーク数の取得
   */
  //-----------------------------------------------------------------------------
  u32 GetLytWkNum( void ) const;


  // 使用不可能
  LytSysBase* GetLytSysBase(void) const;

private:

  LytSysBase* m_base;
};



//----------------------------------------------------------------------------
/**
 *  @brief  レイアウトワーク　クラス
 */
//-----------------------------------------------------------------------------
class LytWk
{

  friend class LytSys;
  friend class LytWkManager;

  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(LytWk);


public:

  //----------------------------------------------------------------------------
  /**
   *  @brief    使用可能状態取得
   *
   *  @retval true    使用可能
   *  @retval false   使用不可能
   */
  //-----------------------------------------------------------------------------
  bool IsInitialized(void);


  //----------------------------------------------------------------------------
  /**
   *  @brief    個別Update(AutoDrawFlagがfalseのときのみ有効)
   */
  //-----------------------------------------------------------------------------
  void Update( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief    個別描画(AutoDrawFlagがfalseのときのみ有効)
   *
   *  @param    graphics    グラフィックスシステム
   *  @param    drawtype    UPPER　LOWER
   *  @param    renderTarget レンダリングターゲットのフレームバッファ
   *  @param    rightRenderTarget  右目用レンダリングターゲット　（DISPLAY_UPPERのときのみ参照）
   */
  //-----------------------------------------------------------------------------
  void Draw( gfl::grp::GraphicsSystem* graphics, DisplayType drawtype, RenderTarget* renderTarget, RenderTarget* rightRenderTarget = NULL );


  //----------------------------------------------------------------------------
  /**
   *  @brief  UpperLowerに対応したユーザー描画環境データ設定
   *
   *  @param  upperEnv    上画面用ENV
   *  @param  lowerEnv    下画面用ENV
   *
   *  *描画にGlobalEnvではなく引数のENVを使用するようになります。
   *  *戻すには、SetGlobalEnv()を呼んでください。
   *
   */
  //-----------------------------------------------------------------------------
  void SetUserEnv( const gfl::grp::g2d::Env* upperEnv, const gfl::grp::g2d::Env* lowerEnv );

  //----------------------------------------------------------------------------
  /**
   *  @brief  グローバルENVを使用するように設定
   */
  //-----------------------------------------------------------------------------
  void SetGlobalEnv( void );


  //----------------------------------------------------------------------------
  /**
   *  @brief  LytWkGroupIDを設定する
   *
   *  @param  groupID グループID
   */
  //-----------------------------------------------------------------------------
  void SetLytWkGroupID( LytWKGroupID groupID );

  //----------------------------------------------------------------------------
  /**
   *  @brief  LytWkGroupIDを取得する
   *
   *  @return グループID
   */
  //-----------------------------------------------------------------------------
  LytWKGroupID GetLytWkGroupID( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  描画するディスプレイタイプを設定
   *
   *	@param	  lod     描画するディスプレイタイプ
   *  
   *  lod 
   *    DISPLAY_UPPER     上画面に表示
   *    DISPLAY_LOWER     下画面に表示
   *    DISPLAY_BOTH_MAX  両画面に表示
   *    
   */
  //-----------------------------------------------------------------------------
  void SetDrawDispType( DisplayType lod );

  //----------------------------------------------------------------------------
  /**
   *	@brief  描画するディスプレイタイプの取得
   *
   *	@return 描画するディスプレイタイプ
   */
  //-----------------------------------------------------------------------------
  DisplayType GetDrawDispType( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  非表示ペインのアニメーション更新を省略するフラグ
   *	（デフォルトON）
   */
  //-----------------------------------------------------------------------------
  void SetVisiblePaneNotAnimeOption( bool flag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  非表示ペインのアニメーション更新を省略するフラグチェック
   *	@retval true  有効
   *	@retval false 無効
   */
  //-----------------------------------------------------------------------------
  bool IsVisiblePaneNotAnimeOption( void ) const;


  //============================================================================
  /**
   *  ペイン操作
   */
  //=============================================================================
  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndexのペインを取得
   *
   *  @param  paneIndex GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
   *
   *  @retval ペイン
   *  @retval NULL    Indexが不正
   */
  //-----------------------------------------------------------------------------
  LytPane* GetPane( LytPaneIndex paneIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndexのピクチャーペインを取得
   *
   *  @param  paneIndex GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
   *
   *  @retval ピクチャーペイン
   *  @retval NULL Indexが不正か、PicturePaneでない
   */
  //-----------------------------------------------------------------------------
  LytPicture* GetPicturePane( LytPaneIndex paneIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndexのテキストボックスペインを取得
   *
   *  @param  paneIndex GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
   *
   *  @retval テキストボックスペイン
   *  @retval NULL Indexが不正か、TextBoxPaneでない
   */
  //-----------------------------------------------------------------------------
  LytTextBox* GetTextBoxPane( LytPaneIndex paneIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  PeneIndexのテキストボックスペインの文字列バッファのメモリを確保
   *
   *	@param	paneIndex     ペインインデックス
   *	@param  minLen        確保文字列数
   */
  //-----------------------------------------------------------------------------
  void AllocTextBoxPaneStringBuffer( LytPaneIndex paneIndex, u16 minLen );

  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndexのwindowペインを取得
   *
   *  @param  paneIndex GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
   *
   *  @retval windowペイン
   *  @retval NULL Indexが不正かwindowPaneでない
   */
  //-----------------------------------------------------------------------------
  LytWindow* GetWindowPane( LytPaneIndex paneIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndexのバウンディングボックスペインを取得
   *
   *  @param  paneIndex GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
   *
   *  @retval バウンディングボックスペイン
   *  @retval NULL Indexが不正かboundingPaneでない
   */
  //-----------------------------------------------------------------------------
  LytBounding* GetBoundingPane( LytPaneIndex paneIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndexのマテリアルを取得
   *
   *  @param  paneIndex GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
   *
   *  @retval マテリアル情報
   *  @retval NULL  Indexが不正かマテリアルではない
   */
  //-----------------------------------------------------------------------------
  LytMaterial* GetMaterial( LytPaneIndex paneIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndexのグループのペインリストを取得
   *
   *  @param  groupIndex  グループインデックス
   *
   *  @return ペインリスト
   */
  //-----------------------------------------------------------------------------
  LytPaneList* GetPaneList( LytPaneIndex groupIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒットチェックが可能か取得
   *
   *  @retval true  可能
   *  @retval false 不可能    一度描画する必要があります。
   */
  //-----------------------------------------------------------------------------
  bool IsHitCheckOK(void) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  全Paneヒットチェック
   *
   *  @param  スクリーン座標系でのｘ、ｙ（タッチパネル座標そのまま）
   *  @param  boundingOnly  バウンディングペインのみチェックする:true
   *
   *  @retval 最初にヒットしたペイン
   *  @retval NULL  ヒットしない
   *
   *  *ヒットチェックを行う前に必ず一度Drawされている必要があります。
   *  　また、1つ前の描画位置でヒットチェックが行われます。
   *
   *  *回転には対応しておりません。
   */
  //-----------------------------------------------------------------------------
  LytPane* GetHitPaneAll( s32 x, s32 y, bool boundingOnly ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  全Paneヒットチェック
   *
   *  @param  スクリーン座標系でのｘ、ｙ（タッチパネル座標そのまま）
   *  @param  boundingOnly  バウンディングペインのみチェックする:true
   *
   *  @retval 最初にヒットしたペインのインデックス
   *  @retval LYTSYS_PANEINDEX_ERROR  ヒットしない
   *
   *  *ヒットチェックを行う前に必ず一度Drawされている必要があります。
   *  　また、1つ前の描画位置でヒットチェックが行われます。
   *
   *  *回転には対応しておりません。
   */
  //-----------------------------------------------------------------------------
  LytPaneIndex GetHitPaneIndexAll( s32 x, s32 y, bool boundingOnly ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  GroupPaneヒットチェック
   *
   *  @param  GroupのIndex　GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
   *  @param  View座標系でのｘ、ｙ（画面中心が原点）
   *  @param  boundingOnly  バウンディングペインのみチェックする:true
   *
   *  @retval 最初にヒットしたペイン
   *  @retval NULL  ヒットしない
   *
   *  *ヒットチェックを行う前に必ず一度Drawされている必要があります。
   *  　また、1つ前の描画位置でヒットチェックが行われます。
   *
   *  *回転には対応しておりません。
   */
  //-----------------------------------------------------------------------------
  LytPane* GetHitPaneGroup( LytPaneIndex groupIndex, s32 x, s32 y, bool boundingOnly ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  GroupPaneヒットチェック
   *
   *  @param  GroupのIndex　GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
   *  @param  スクリーン座標系でのｘ、ｙ（タッチパネル座標そのまま）
   *  @param  boundingOnly  バウンディングペインのみチェックする:true
   *
   *  @retval 最初にヒットしたペインのインデックス
   *  @retval LYTSYS_PANEINDEX_ERROR  ヒットしない
   *
   *  *ヒットチェックを行う前に必ず一度Drawされている必要があります。
   *  　また、1つ前の描画位置でヒットチェックが行われます。
   *
   *  *回転には対応しておりません。
   */
  //-----------------------------------------------------------------------------
  LytPaneIndex GetHitPaneIndexGroup( LytPaneIndex groupIndex, s32 x, s32 y, bool boundingOnly ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  単体Paneとのヒットチェック
   *
   *  @param  スクリーン座標系でのｘ、ｙ（タッチパネル座標そのまま）
   *  @param  ペインインデックス
   *
   *  @retval true    ヒットしている。
   *  @retval false   ヒットしていない。
   *
   *  *ヒットチェックを行う前に必ず一度Drawされている必要があります。
   *  　また、1つ前の描画位置でヒットチェックが行われます。
   *
   *  *回転には対応しておりません。
   */
  //-----------------------------------------------------------------------------
  bool GetHitPane( s32 x, s32 y, LytPaneIndex index ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  マテリアルのテクスチャを差し替える。
   *
   *  @param  paneIndex   マテリアルのインデックス
   *  @param  mapIndex    １マテリアル内の何番目のテクスチャを変更するか？
   *  @param  cp_res      リソースワーク
   *  @param  texArcIndex テクスチャのアーカイブインデックス
   */
  //-----------------------------------------------------------------------------
  void SetMaterialImage( LytPaneIndex materialIndex, u32 mapIndex, const LytRes* cp_res, LytArcIndex texArcIndex );



  //-----------------------------------------------------------------------------
  /**
   *  @brief  リソース情報を取得
   *
   *  @param  resID         リソースID
   *
   *  @retval リソース
   */
  //-----------------------------------------------------------------------------
  const LytRes* GetResource( void ) const;




  //============================================================================
  /**
   *  基本操作
   */
  //=============================================================================
  //----------------------------------------------------------------------------
  /**
   *  @brief    オート描画のON/OFF設定
   *
   *  @param    flag    フラグ  true:ON   false:OFF
   */
  //-----------------------------------------------------------------------------
  void SetAutoDrawFlag( bool flag );


  //----------------------------------------------------------------------------
  /**
   *  @brief    オート描画のON/OFF 取得
   *
   *  @retval true    ON
   *  @retval false   OFF
   */
  //-----------------------------------------------------------------------------
  bool IsAutoDraw( void ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  処理優先順位の設定
   *
   *  @param  pri   優先順位（ 0 > 0xffff )
   *
   *  *ただし、デプステストなしで、描画を行う場合には、
   *　後から描画されたワークが手前に描画されるため注意。
   *
   *  UpdateAllとDrawAllを行う際の優先順位です。
   */
  //-----------------------------------------------------------------------------
  void SetPriority( u16 pri );

  //----------------------------------------------------------------------------
  /**
   *  @brief  処理優先順位の取得
   *
   *  @retval 優先順位（ 0 > 0xffff )
   *
   *  *ただし、デプステストなしで、描画を行う場合には、
   *　後から描画されたワークが手前に描画されるため注意。
   *
   *  UpdateAllとDrawAllを行う際の優先順位です。
   */
  //-----------------------------------------------------------------------------
  u16 GetPriority( void ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  座標の設定
   *
   *  @param  pos
   */
  //-----------------------------------------------------------------------------
  void SetTranslate( const gfl::math::VEC3& pos );

  //----------------------------------------------------------------------------
  /**
   *  @brief  座標の取得
   *
   *  @param  p_pos   座標格納先
   */
  //-----------------------------------------------------------------------------
  void GetTranslate( gfl::math::VEC3* p_pos ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  拡大の設定
   *
   *  @param  scale   スケーリング値
   */
  //-----------------------------------------------------------------------------
  void SetScale( const gfl::math::VEC2& scale );

  //----------------------------------------------------------------------------
  /**
   *  @brief  拡大の取得
   *
   *  @param p_scale スケーリング値格納先
   */
  //-----------------------------------------------------------------------------
  void GetScale( gfl::math::VEC2* p_scale ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  回転の設定
   *
   *  @param  rotate   各軸の回転値
   */
  //-----------------------------------------------------------------------------
  void SetRotate( const gfl::math::VEC3& rotate );

  //----------------------------------------------------------------------------
  /**
   *  @brief  回転の取得
   *
   *  @param  p_rotate   各軸の回転値格納先
   */
  //-----------------------------------------------------------------------------
  void GetRotate( gfl::math::VEC3* p_rotate ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  カリングフラグの設定
   *
   *	@param	flag  フラグ
   *
   *	カリングをfalseにするということは、
   *	左、右、下すべての画面でレンダリング処理が実行されるということですので、
   *	処理負荷に注意してください。
   */
  //-----------------------------------------------------------------------------
  void SetCullingFlag( bool flag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  カリングフラグの取得
   *
   *	@retval	true  カリング処理ON
   *	@retval false カリング処理OFF
   */
  //-----------------------------------------------------------------------------
  bool IsCullingFlag( void ) const;

  //============================================================================
  /**
   *  アニメーション関連
   */
  //=============================================================================

  //----------------------------------------------------------------------------
  /**
   *  @brief  アニメーションの追加登録
   *
   *  @param  anmResID  アニメーションリソースID
   *
   *  @retval 登録Index
   *  @retval LYTSYS_ANIMEINDEX_ERROR 登録失敗
   */
  //-----------------------------------------------------------------------------
  u32 CreateAnime( LytArcIndex anmDataID );


  //----------------------------------------------------------------------------
  /**
   *  @brief  共有アニメーションの登録    LytPaneIndexを指定して、個別登録するタイプ
   *
   *  @param  startAnimeIndex 登録開始アニメーションインデックス
   *  @param  anmResID        アニメーションリソースID
   *  @param  maxNum          登録最大数（0だとすべて登録）
   *  @param  groupIndex      登録グループインデックス（LYTSYS_PANEINDEX_ERRORだとすべて登録）
   *  @param  paneIndex       登録グループ内ペインインデックス（LYTSYS_PANEINDEX_ERRORだとすべて登録）
   *                          （groupIndexを指定しないと、paneIndexは効果がありません。）
   *
   *  @retval 登録数  (startAnimeIndexから戻り値分のインデックスにアニメーションが追加されました。）
   *
   *  *アニメーションワークが連続で確保できないとASSERTでとまります。
   *  *登録数など把握しながら使用してください。
   *
   *  *共有アニメーションとは、
   *  　anmResIDのアニメーションを複数のペインが共通で使用することができます。
   *  LayoutEditor側で、アニメーションを使用することができるペインを指定しておき、
   *  この関数でアニメーションを作成してください。
   *
   *  共有アニメーションという名前ですが、アニメーションフレームの管理は独立しています。
   *  startAnimeIndex+戻り値分のインデックスを指定してアニメーションのフレーム管理を行ってください。
   *
   *  ＊大本となっているアニメーションは通常のCreateAnimeで生成してください。
   */
  //-----------------------------------------------------------------------------
  u32 CreateShareAnime( u32 startAnimeIndex, LytArcIndex anmDataID, u32 maxNum=0, LytPaneIndex groupIndex=LYTSYS_PANEINDEX_ERROR, LytPaneIndex paneIndex=LYTSYS_PANEINDEX_ERROR );

  //----------------------------------------------------------------------------
  /**
   *  @brief  アニメーションの削除
   *
   *  @param  anmIdx  削除するアニメーションインデックス
   */
  //-----------------------------------------------------------------------------
  void DeleteAnime( u32 anmIdx );

  //----------------------------------------------------------------------------
  /**
   *  @brief    アニメーション情報があるかチェック
   *
   *  @param  anmaIdx       アニメーションインデックス
   */
  //-----------------------------------------------------------------------------
  bool IsAnime( u32 anmIdx ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    アニメーションの反映
   *
   *  @param  anmIdx        アニメーションIDX
   *  @param  is_auto_unbind   one time アニメーションの場合、アニメーション完了後、unbindする。
   */
  //-----------------------------------------------------------------------------
  void BindAnime( u32 anmIdx, bool is_auto_unbind = true );

  //-----------------------------------------------------------------------------
  /**
   *  @brief    アニメーションの反映解除
   *
   *  @param  anmIdx    登録先IDX
   */
  //-----------------------------------------------------------------------------
  void UnBindAnime( u32 anmIdx );

  //-----------------------------------------------------------------------------
  /**
   *  @brief    全アニメーションの反映解除
   */
  //-----------------------------------------------------------------------------
  void UnBindAnimeAll( void );


  //-----------------------------------------------------------------------------
  /**
   *  @brief    アニメーションがバインドされているかチェック
   *
   *  @retval true    バインドされている。
   *  @retval false   バインドされていない。
   */
  //-----------------------------------------------------------------------------
  bool IsBindAnime( u32 anmIdx ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    オートアニメーション設定
   *
   *  @param  anmIdx    アニメーションIDX
   *  @param  flag      ON/OFFフラグ
   */
  //-----------------------------------------------------------------------------
  void SetAutoAnime( u32 anmIdx, bool flag );

  //-----------------------------------------------------------------------------
  /**
   *  @brief    オートアニメーションかチェック
   *
   *  @param  anmIdx    アニメーションIDX
   *
   *  @retval true    オートアニメ
   *  @retval false   非オートアニメ
   */
  //-----------------------------------------------------------------------------
  bool IsAutoAnime( u32 anmIdx ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    オートアニメーション スピード設定　（全オートアニメ共通）
   *
   *  @param  speed     アニメーションスピード
   */
  //-----------------------------------------------------------------------------
  void SetAutoAnimeSpeed( f32 speed );


  //-----------------------------------------------------------------------------
  /**
   *  @brief    オートアニメーション スピード設定　（個別設定）
   *
   *  @param  speed     アニメーションスピード
   */
  //-----------------------------------------------------------------------------
  void SetAutoAnimeSpeed( u32 anmIdx, f32 speed );


  //-----------------------------------------------------------------------------
  /**
   *  @brief    アニメーションフレーム設定
   *
   *  @param  anmIdx    アニメーションIDX
   *  @param  frame     アニメーションフレーム
   */
  //-----------------------------------------------------------------------------
  void SetAnimeFrame( u32 anmIdx, f32 frame );

  //-----------------------------------------------------------------------------
  /**
   *  @brief    アニメーションフレーム　カウント
   *
   *  @param  anmIdx    アニメーションIDX
   *  @param  frame     アニメーションフレーム
   *
   *  @retval true    アニメーション終了（LOOP設定のときは常にfalse）
   *  @retval false   アニメーション途中
   */
  //-----------------------------------------------------------------------------
  bool AddAnimeFrame( u32 anmIdx, f32 frame );

  //-----------------------------------------------------------------------------
  /**
   *  @brief    アニメーションフレーム取得
   *
   *  @param  anmIdx    アニメーションIDX
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimeFrame( u32 anmIdx ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    アニメーション最大フレーム取得
   *
   *  @param  anmIdx    アニメーションIDX
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimeMaxFrame( u32 anmIdx ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ループアニメーションかチェック
   *
   *  @param  anmIdx    アニメーションIDX
   *
   *  @retval true    ループアニメーション
   *  @retval false   ループアニメーションじゃない
   */
  //-----------------------------------------------------------------------------
  bool IsAnimeLoop( u32 anmIdx ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    アニメーションの完了チェック
   *
   *  @param  anmIdx    アニメーションIDX
   *
   *  @retval true    完了
   *  @retval false   途中
   */
  //-----------------------------------------------------------------------------
  bool IsAnimeEnd( u32 anmIdx ) const;






  //----------------------------------------------------------------------------
  /**
   *  @brief  レイアウトコアシステム　（ベースライブラリのクラス）取得
   *
   *  @return ベースライブラリのクラス
   */
  //-----------------------------------------------------------------------------
  LytCore* GetLayoutCore( void ) const;


	//----------------------------------------------------------------------------
	/**
	 *	@brief    非表示のペインに対して行列計算を行うかどうかを設定する
	 *
	 *	@return		none
	 */
	//-----------------------------------------------------------------------------
	void SetInvisiblePaneCalculateMtx( bool flg );

	//----------------------------------------------------------------------------
	/**
	 *	@brief    コンバート時に埋め込まれたフォント幅を取得する。
	 *            実際は下記帳ユーザーデータにg2d_fwとして埋め込まれた実数
	 *
	 *	@return		f32 幅。0の時は取得失敗
	 */
	//-----------------------------------------------------------------------------
	f32 GetFontWidthWhenConvert( LytPaneIndex index );

private:
  //-----------------------------------------------------------------------------
  /**
  *     以下の関数使用禁止
  */
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
  /**
  *     コンストラクタ・デストラクタ
  *     このクラスは、かならずLytSys::CreateLytWkを使用して生成すること。
  */
  //-----------------------------------------------------------------------------
  LytWk(void);
  ~LytWk();
  void Initialize( gfl::heap::NwAllocator* allocator, LytWkManager* p_wkman );
  LytWkBase* GetLytWkBase( void ) const;

private:
  void Finalize( void );

  LytWkManager* m_parent;

  LytWkBase*  m_base;
};




} /* end of namespace g2d */
} /* end of namespace grp */
} /* end of namespace gfl */

#endif // __GFL_LYTSYS_H__
