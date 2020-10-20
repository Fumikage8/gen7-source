//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl2_LytSys.h
 *  @brief  レイアウトシステム
 *  @author tomoya takahashi
 *  @date   2010.10.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( GFL2_LYTSYS_H_INCLUDED )
#define GFL2_LYTSYS_H_INCLUDED

#pragma once

#if defined(GF_PLATFORM_CTR)
#include <nn.h>
#include <nn/math.h>
#endif

//#include <base/gfl_Singleton.h>
#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_NwAllocator.h>
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <fs/include/gfl2_Fs.h>
#include <str/include/gfl2_Font.h>

#include <layout/include/gfl2_G2dEnv.h>
#include <layout/include/gfl2_LytRes.h>
#include <layout/include/gfl2_LytMultiResID.h>
#include <layout/include/gfl2_LytShaderResource.h>
#include "layout/include/gfl2_LytShaderResource.h"


#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)

//-------------------------------------
/// 前方宣言
//=====================================
class GraphicsSystem;
class LytSysBase;
class LytWkManager;
class LytWkBase;
class LytWk;
class LytMultiResID;
class DrawingLytWkManager;


//-----------------------------------------------------------------------------
/**
 *        デバッグ出力設定
 */
//-----------------------------------------------------------------------------
#define GFL_LYTSYS_DEBUGPRINT_LEVEL1 (gfl2::debug::PRINT_FLAG_LEVEL1|gfl2::debug::PRINT_FLAG_CATEGORY_G2D)


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
  gfl2::lyt::LytShaderResource* pRectDrawerShaderRes;  //RectDrawerShaderのアドレス
  u16 lyt_resource_max;   // 管理リソース最大数
  u16 lyt_wk_max;          // 管理LytWk最大数
} LytSysSetUp;


//----------------------------------------------------------------------------
/**
 *  @brief  LytWkセットアップ構造体定義
 */
//-----------------------------------------------------------------------------
typedef struct {
  gfl2::math::Vector3   pos;       // 初期位置
  gfl2::math::Vector2   scale;     // 初期拡大値
  gfl2::math::Vector3   rotate;    // 初期回転角度
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
typedef nw::lyt::Parts    LytParts;     // 部品ペイン

//-------------------------------------
/// マテリアル情報
//=====================================
typedef nw::lyt::Material LytMaterial;  // マテリアル

typedef nw::lyt::TexMap LytTexMap;   //


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
  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトシステムの初期化
   *
   * @param   heapAllocator     ヒープアロケータ
   * @param   deviceAllocator   デバイスヒープアロケータ
   *
   * @note
   *  ゲーム開始時に一度だけ設定してください。
   *  終了時にFinalizeSystemを必ず呼んでください。
   */
  //-----------------------------------------------------------------------------
  static void InitializeSystem( gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator, gfl2::lyt::LytShaderResource* pLSR );

  //----------------------------------------------------------------------------
  /**
   * @brief   レイアウトシステムの破棄
   */
  //-----------------------------------------------------------------------------
  static void FinalizeSystem( void );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief   デフォルトフォント設定
   *
   * @param   font    フォントデータ
   * @param   name    レイアウトがアクセスするための名称 ( 例　simple.bcfnt )
   * @param   index   インデックス
   */
  //-----------------------------------------------------------------------------
  static void SetDefaultFont( gfl2::str::Font* font, const char* name, u32 index = 0 );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デフォルトフォント解除
   *
   * @param   index   インデックス
   */
  //-----------------------------------------------------------------------------
  static void ResetDefaultFontInfo( u32 index );

  
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *  
   * @param   setup             セットアップ情報
   * @param   heapAllocator     ヒープアロケータ
   * @param   deviceAllocator   デバイスヒープアロケータ
   * @param   drawingLytWkManager  描画中レイアウトワークのマネージャ。NULL可。NULLのとき描画中か否かの管理を行わない。
   */
  //-----------------------------------------------------------------------------
  LytSys( const LytSysSetUp* setup, gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator, DrawingLytWkManager* drawingLytWkManager = NULL );

  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ ( 非同期初期化モード )
   *
   * @param   p_heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  LytSys( gfl2::heap::HeapBase* p_heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  ~LytSys();

  
  //-----------------------------------------------------------------------------
  /**
   * @brief   非同期初期化（　LytSys（void）で初期化したときのみ必要　）
   *
   * @param   setup             セットアップ情報
   * @param   heapAllocator     ヒープアロケータ
   * @param   deviceAllocator   デバイスヒープアロケータ
   * @param   drawingLytWkManager  描画中レイアウトワークのマネージャ。NULL可。NULLのとき描画中か否かの管理を行わない。
   */
  //-----------------------------------------------------------------------------
  void InitializeAsync( const LytSysSetUp* setup, gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator, DrawingLytWkManager* drawingLytWkManager = NULL );

  //-----------------------------------------------------------------------------
  /**
   * @brief   非同期初期化完了まち
   *
   * @retval  true  初期化完了
   * @retval  false 初期化中
   */
  //-----------------------------------------------------------------------------
  bool InitializeAsyncWait( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief   初期化が完了しているかチェック
   *
   * @retval  true  初期化完了
   * @retval  false 初期化中
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

  //-----------------------------------------------------------------------------
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
  void DrawAll( LytWKGroupID lytwkGroupID = LYTSYS_LYTWK_GROUPID_ALL );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  UpperLowerに対応したグローバル描画環境データ
   *
   *  @param  描画面　UPPER　LOWER
   *
   *  @return 描画環境データ
   *
   *   *通常LytWKはこのEnvを使用して描画されています。
   *    もちろん、個々のLytWkに別のEnvを設定することも可能です。
   *    LytWk::SetUserEnv( Env& upperEnv, Env& lowerEnv );
   */
  //-----------------------------------------------------------------------------
  Env* GetGlobalEnv( gfl2::lyt::DisplayType drawtype );

  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースの読み込み ( データ指定版 )
   *
   * @param   resID           親リソースID
   * @param   devAllocator    デバイスアロケータ
   * @param   cp_buff         データ  (内部では、破棄しません。）
   * @param   useDefaultFont  Defaultフォントを使用するか？
   * @param   addTextureNum   後から追加するテクスチャ数
   *
   * @return  リソースID
   *
   * @note
   *  resID = NULL の場合、未登録リソースを使用します。
   *  未登録リソースがない場合、登録数=1の新規リソースを生成します。
   */
  //-----------------------------------------------------------------------------
  LytMultiResID LoadLytRes( const LytMultiResID * resID, gfl2::heap::NwAllocator* devAllocator, const void * cp_buff, bool useDefaultFontt=true, u32 addTextureNum=0 );

  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースの読み込み ( ファイル指定版 )
   *
   * @param    pAsyncFileMan  ファイルマネージャ
   * @param   resID           親リソースID
   * @param   devAllocator    デバイスアロケータ
   * @param   arcID           アーカイブID(arc_tool.lstファイル内に書かれているID(*.garcファイル自体のID(ARCID_*)))
   * @param   dataID          アーカイブデータID(*.gaixファイル内に書かれているID(GARC_*))
   * @param   comp            圧縮されているか
   * @param   useDefaultFont  Defaultフォントを使用するか？
   * @param   addTextureNum   後から追加するテクスチャ数
   *
   * @return  リソースID
   *
   * @note
   *  resID = NULL の場合、未登録リソースを使用します。
   *  未登録リソースがない場合、登録数=1の新規リソースを生成します。
   *
   * @note    Open,Closeは使用する側で行ってください
   */
  //-----------------------------------------------------------------------------
  LytMultiResID LoadLytRes( gfl2::fs::AsyncFileManager* pAsyncFileMan, const LytMultiResID * resID, gfl2::heap::NwAllocator* devAllocator, u32 arcID, u32 dataID, bool comp, bool useDefaultFont=true, u32 addTextureNum=0 );

  //-----------------------------------------------------------------------------
  /**
   * @brief   リソース初期設定
   *
   * @param   heap      ヒープ
   * @param   link_max  アクセサが保持できるアーカイブの数
   *
   * @return  設定されたリソースID
   */
  //-----------------------------------------------------------------------------
  LytMultiResID InitMultiResource( gfl2::heap::HeapBase * heap, u32 link_max );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  レイアウトリソースの破棄
   *
   *  @param  resID     破棄するリソースID
   */
  //-----------------------------------------------------------------------------
  void ReleaseLytRes( const LytMultiResID * resID );

  //-----------------------------------------------------------------------------
  /**
   * @brief   テクスチャリソースを追加
   *
   * @param   srcID   追加するテクスチャデータが入っているリソースID
   * @param   dstID   追加先のリソースID
   *
   * @note
   *  追加されるテクスチャ数はLoadLytRes関数のaddTextureNumに指定。
   *  リソースの親IDが同じ場合、この処理は不要。
   */
  //-----------------------------------------------------------------------------
  void RegistTextureRes( const LytMultiResID * srcID, const LytMultiResID * dstID );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  テクスチャをすべてメモリに配置
   *  
   *  @param  resID   リソースID
   */
  //-----------------------------------------------------------------------------
  void TransferAllTexture( const LytMultiResID * resID );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  テクスチャをメモリに配置
   *  
   *  @param  resID         リソースID
   *  @param  texArcIndex   アーカイブ内のテクスチャインデックス
   */
  //-----------------------------------------------------------------------------
  void TransferIndexTexture( const LytMultiResID * resID, LytArcIndex texArcIndex );

  //-----------------------------------------------------------------------------
  /**
   * @brief   リソースを取得
   *  
   * @param   resID   リソースID
   *
   * @return  リソース
   */
  //-----------------------------------------------------------------------------
  LytRes * GetResource( const LytMultiResID * resID ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   使用中のリソースの総数を取得
   *
   * @return  使用中のリソースの総数
   */
  //-----------------------------------------------------------------------------
  u32 GetUseResourceNum( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトワーク生成
   *
   * @param   resID                 リソースID
   * @param   lytDataID             resID内のレイアウトデータID
   * @param   anmDataTbl            アニメデータIDテーブル ( ない場合はNULL )
   * @param   anmTblNum             アニメデータIDテーブル数
   * @param   anmMax                登録アニメ最大数
   * @param   setup                 レイアウトセットアップデータ
   * @param   is_enable             アニメを無効状態で関連付ける場合 = false
   * @param   createPaneTbl         ペインテーブル生成
   * @param   createPaneTblWithHash 高速検索用にハッシュキーを作る（消費メモリ増）
   *
   * @retval  "NULL != レイアウトワーク"
   * @retval  "NULL = 生成失敗"
   *
   * @note
   *  anmMaxには部品のアニメ数も含める必要がある
   */
  //-----------------------------------------------------------------------------
  LytWk * CreateLytWk(
            const LytMultiResID * resID,
            LytArcIndex lytDataID,
            const LytArcIndex * anmDataTbl,
            u32 anmTblNum,
            u32 anmMax,
            const LytWkSetUp* setup,
            b32 is_enable=true,
            b32 createPaneTbl=false,
            b32 createPaneTblWithHash=false );

  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトワーク生成 ( アロケータ指定版 )
   *
   * @param   allocator             ヒープアロケータ
   * @param   dev_allocator         デバイスヒープアロケータ
   * @param   resID                 リソースID
   * @param   lytDataID             resID内のレイアウトデータID
   * @param   anmDataTbl            アニメデータIDテーブル ( ない場合はNULL )
   * @param   anmTblNum             アニメデータIDテーブル数
   * @param   anmMax                登録アニメ最大数
   * @param   setup                 レイアウトセットアップデータ
   * @param   is_enable             アニメを無効状態で関連付ける場合 = false
   * @param   createPaneTbl         ペインテーブル生成
   * @param   createPaneTblWithHash 高速検索用にハッシュキーを作る（消費メモリ増）
   *
   * @retval  "NULL != レイアウトワーク"
   * @retval  "NULL = 生成失敗"
   *
   * @note
   *  anmMaxには部品のアニメ数も含める必要がある
   */
  //-----------------------------------------------------------------------------
  LytWk * CreateLytWk(
            gfl2::heap::NwAllocator* allocator,
            gfl2::heap::NwAllocator* dev_allocator,
            const LytMultiResID * resID,
            LytArcIndex lytDataID,
            const LytArcIndex * anmDataTbl,
            u32 anmTblNum,
            u32 anmMax,
            const LytWkSetUp* setup,
            b32 is_enable=true,
            b32 createPaneTbl=false,
            b32 createPaneTblWithHash=false );

  //-----------------------------------------------------------------------------
  /**
   * @brief   アニメ生成
   *
   * @param   lytWk                 レイアウトワーク
   * @param   resID                 リソースID
   * @param   anmTbl                アニメデータIDテーブル
   * @param   anmTblNum             アニメデータIDテーブル数
   * @param   anmOffset             アニメ登録開始番号
   * @param   is_enable             アニメを無効状態で関連付ける場合 = false
   * @param   createPaneTblWithHash 高速検索用にハッシュキーを作る（消費メモリ増）
   */
  //-----------------------------------------------------------------------------
  void CreateAnimation( LytWk * lytWk, const LytMultiResID * resID, const LytArcIndex * anmTbl, u32 anmTblNum, u32 anmOffset, b32 is_enable=true, b32 createPaneTblWithHash=false );

  //-----------------------------------------------------------------------------
  /**
   * @brief   部品レイアウトのアニメ生成
   *
   * @param   lytWk                 レイアウトワーク
   * @param   resID                 部品レイアウトのリソースID
   * @param   startAnmNum           登録開始アニメ番号
   * @param   anmTbl                アニメデータIDテーブル
   * @param   anmTblMax             アニメデータIDテーブル数
   * @param   partsTbl              部品ペインテーブル
   * @param   partsTblMax           部品ペインテーブル数
   * @param   is_enable             アニメを無効状態で関連付ける場合 = false
   * @param   createPaneTblWithHash 高速検索用にハッシュキーを作る（消費メモリ増）
   */
  //-----------------------------------------------------------------------------
  void CreatePartsLayoutAnime(
        LytWk * lytwk,
        const LytMultiResID * resID,
        u32 startAnmNum,
        const LytArcIndex * anmTbl,
        u32 anmTblMax,
        LytParts ** partsTbl,
        u32 partsTblMax,
        b32 is_enable=true,
        b32 createPaneTblWithHash=false );

  //-----------------------------------------------------------------------------
  /**
   * @brief   部品レイアウトのアニメ生成（１部品版）
   *
   * @param   lytWk                 レイアウトワーク
   * @param   resID                 部品レイアウトのリソースID
   * @param   startAnmNum           登録開始アニメ番号
   * @param   anmTbl                アニメデータIDテーブル
   * @param   anmTblMax             アニメデータIDテーブル数
   * @param   parts                 部品ペイン
   * @param   is_enable             アニメを無効状態で関連付ける場合 = false
   * @param   createPaneTblWithHash 高速検索用にハッシュキーを作る（消費メモリ増）
   *
   * @return  開始アニメ番号+追加アニメ数
   */
  //-----------------------------------------------------------------------------
  u32 CreatePartsLayoutAnime(
        LytWk * lytwk,
        const LytMultiResID * resID,
        u32 startAnmNum,
        const LytArcIndex * anmTbl,
        u32 anmTblMax,
        LytParts * parts,
        b32 is_enable=true,
        b32 createPaneTblWithHash=false );

  //-----------------------------------------------------------------------------
  /**
   * @brief レイアウトワークをレイアウトマネージャにリンクさせる
   *
   * @param lytWk 追加対象となるレイアウトワーク
   */
  //-----------------------------------------------------------------------------
  void AddLinkWithLayoutWork( LytWk* lytWk );

  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトワーク破棄
   *
   * @param   lytWk   レイアウトワーク
   */
  //-----------------------------------------------------------------------------
  void RemoveLytWk( LytWk* lytWk );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  生成されたレイアウトワーク数の取得
   */
  //-----------------------------------------------------------------------------
  u32 GetLytWkNum( void ) const;


private:
  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトワーク生成
   *
   * @param   allocator             ヒープアロケータ
   * @param   dev_allocator         デバイスヒープアロケータ
   * @param   resID                 リソースID
   * @param   lytDataID             resID内のレイアウトデータID
   * @param   anmMax                登録アニメ最大数
   * @param   setup                 レイアウトセットアップデータ
   * @param   createPaneTbl         ペインテーブル生成
   * @param   createPaneTblWithHash 高速検索用にハッシュキーを作る（消費メモリ増）
   *
   * @retval  "NULL != レイアウトワーク"
   * @retval  "NULL = 生成失敗"
   */
  //-----------------------------------------------------------------------------
  LytWk * BuildLytWk(
            gfl2::heap::NwAllocator* allocator,
            gfl2::heap::NwAllocator* dev_allocator,
            const LytMultiResID * resID,
            LytArcIndex lytDataID,
            u32 anmMax,
            const LytWkSetUp* setup,
            b32 createPaneTbl=false,
            b32 createPaneTblWithHash=false );


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
  friend class DrawingLytWkManager;

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
  void Draw( gfl2::lyt::DisplayType lod, Env::CameraType cameraType );

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
  void SetUserEnv( const Env* upperEnv, const Env* lowerEnv );

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
   *  @brief  描画するディスプレイタイプを設定
   *
   *  @param    lod     描画するディスプレイタイプ
   *  
   *  lod 
   *    DISPLAY_UPPER     上画面に表示
   *    DISPLAY_LOWER     下画面に表示
   *    DISPLAY_BOTH_MAX  両画面に表示
   *    
   */
  //-----------------------------------------------------------------------------
  void SetDrawDispType( gfl2::lyt::DisplayType lod );

  //----------------------------------------------------------------------------
  /**
   *  @brief  描画するディスプレイタイプの取得
   *
   *  @return 描画するディスプレイタイプ
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::DisplayType GetDrawDispType( void ) const;


  //============================================================================
  /**
   *  ペイン操作
   */
  //=============================================================================
  //-----------------------------------------------------------------------------
  /**
   * @brief   ペインを取得
   *
   * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
   *
   * @retval  "NULL != ペイン"
   * @retval  "NULL = 存在しない"
   */
  //-----------------------------------------------------------------------------
  LytPane* GetPane( LytPaneIndex paneIndex ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   ピクチャーペインを取得
   *
   * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
   *
   * @retval  "NULL != ピクチャーペイン"
   * @retval  "NULL = 存在しない"
   */
  //-----------------------------------------------------------------------------
  LytPicture* GetPicturePane( LytPaneIndex paneIndex ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインを取得
   *
   * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
   *
   * @retval  "NULL != テキストボックスペイン"
   * @retval  "NULL = 存在しない"
   */
  //-----------------------------------------------------------------------------
  LytTextBox* GetTextBoxPane( LytPaneIndex paneIndex ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインの文字列バッファを確保
   *
   * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
   * @param   minLen        文字数
   */
  //-----------------------------------------------------------------------------
  void AllocTextBoxPaneStringBuffer( LytPaneIndex paneIndex, u16 minLen );
  void AllocTextBoxPaneStringBuffer( gfl2::lyt::LytTextBox* pLytTextBox, u16 minLen );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ウィンドウペインを取得
   *
   * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
   *
   * @retval  "NULL != ウィンドウペイン"
   * @retval  "NULL = 存在しない"
   */
  //-----------------------------------------------------------------------------
  LytWindow* GetWindowPane( LytPaneIndex paneIndex ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   バウンディングペインを取得
   *
   * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
   *
   * @retval  "NULL != バウンディングペイン"
   * @retval  "NULL = 存在しない"
   */
  //-----------------------------------------------------------------------------
  LytBounding* GetBoundingPane( LytPaneIndex paneIndex ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   部品ペインを取得
   *
   * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
   *
   * @retval  "NULL != 部品ペイン"
   * @retval  "NULL = 存在しない"
   */
  //-----------------------------------------------------------------------------
  LytParts * GetPartsPane( LytPaneIndex paneIndex ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   マテリアル情報を取得
   *
   * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
   *
   * @retval  "NULL != マテリアル情報"
   * @retval  "NULL = マテリアルが存在しない"
   */
  //-----------------------------------------------------------------------------
  LytMaterial* GetMaterial( LytPaneIndex paneIndex ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   部品ペイン内のペインを取得
   *
   * @param   parts         部品ペイン
   * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
   * @param   resID         partsのリソースID
   *
   * @retval  "NULL != ペイン"
   * @retval  "NULL = ペインが存在しない"
   */
  //-----------------------------------------------------------------------------
  LytPane * GetPane( nw::lyt::Parts * parts, u32 paneIndex, const LytMultiResID * resID ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   部品ペイン内のピクチャーペインを取得
   *
   * @param   parts         部品ペイン
   * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
   * @param   resID         partsのリソースID
   *
   * @retval  "NULL != ピクチャーペイン"
   * @retval  "NULL = ペインが存在しない"
   */
  //-----------------------------------------------------------------------------
  LytPicture * GetPicturePane( nw::lyt::Parts * parts, u32 paneIndex, const LytMultiResID * resID ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   部品ペイン内のテキストボックスペインを取得
   *
   * @param   parts         部品ペイン
   * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
   * @param   resID         partsのリソースID
   *
   * @retval  "NULL != テキストボックスペイン"
   * @retval  "NULL = ペインが存在しない"
   */
  //-----------------------------------------------------------------------------
  LytTextBox * GetTextBoxPane( nw::lyt::Parts * parts, u32 paneIndex, const LytMultiResID * resID ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   部品ペイン内のテキストボックスペインの文字列バッファを確保
   *
   * @param   parts         部品ペイン
   * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
   * @param   resID         partsのリソースID
   * @param   minLen        文字数
   */
  //-----------------------------------------------------------------------------
  void AllocTextBoxPaneStringBuffer( nw::lyt::Parts * parts, u32 paneIndex, const LytMultiResID * resID, u32 minLen ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   部品ペイン内のウィンドウペインを取得
   *
   * @param   parts         部品ペイン
   * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
   * @param   resID         partsのリソースID
   *
   * @retval  "NULL != ウィンドウペイン"
   * @retval  "NULL = ペインが存在しない"
   */
  //-----------------------------------------------------------------------------
  LytWindow * GetWindowPane( nw::lyt::Parts * parts, u32 paneIndex, const LytMultiResID * resID ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   部品ペイン内のバウンディングペインを取得
   *
   * @param   parts         部品ペイン
   * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
   * @param   resID         partsのリソースID
   *
   * @retval  "NULL != バウンディングペイン"
   * @retval  "NULL = ペインが存在しない"
   */
  //-----------------------------------------------------------------------------
  LytBounding * GetBoundingPane( nw::lyt::Parts * parts, u32 paneIndex, const LytMultiResID * resID ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   部品ペイン内の部品ペインを取得
   *
   * @param   parts         部品ペイン
   * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
   * @param   resID         partsのリソースID
   *
   * @retval  "NULL != 部品ペイン"
   * @retval  "NULL = ペインが存在しない"
   */
  //-----------------------------------------------------------------------------
  LytParts * GetPartsPane( LytParts * parts, LytPaneIndex paneIndex, const LytMultiResID * resID ) const;

  //----------------------------------------------------------------------------
  /**
   * @brief   部品ペイン内のマテリアル情報を取得
   *
   * @param   parts         部品ペイン
   * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
   * @param   resID         partsのリソースID
   *
   * @retval  "NULL != マテリアル情報"
   * @retval  "NULL = マテリアルが存在しない"
   */
  //-----------------------------------------------------------------------------
  LytMaterial * GetMaterial( LytParts * parts, LytPaneIndex paneIndex, const LytMultiResID * resID ) const;

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

  //-----------------------------------------------------------------------------
  /**
   *  @brief  ヒットチェックが可能か取得
   *
   *  @retval true  可能
   *  @retval false 不可能    一度描画する必要があります。
   */
  //-----------------------------------------------------------------------------
  bool IsHitCheckOK(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   全ペインヒットチェック
   *
   * @param   x             スクリーン座標系でのX座標 ( タッチパネル座標 )
   * @param   y             スクリーン座標系でのY座標 ( タッチパネル座標 )
   * @param   boundingOnly  バウンディングペインのみチェックする:true
   *
   * @retval  最初にヒットしたペイン
   * @retval  NULL  ヒットしない
   *
   * @note
   *  ヒットチェックを行う前に必ず一度Drawされている必要があります。
   *  また、1つ前の描画位置でヒットチェックが行われます。
   *
   * @note
   *  回転には対応しておりません。
   *
   * @note
   *  部品内のペインについては、最初に部品ペインにヒットするため不要。
   */
  //-----------------------------------------------------------------------------
  LytPane* GetHitPaneAll( s32 x, s32 y, bool boundingOnly ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   全ペインヒットチェック ( インデックスを返す )
   *
   * @param   x             スクリーン座標系でのX座標 ( タッチパネル座標 )
   * @param   y             スクリーン座標系でのY座標 ( タッチパネル座標 )
   * @param   boundingOnly  バウンディングペインのみチェックする:true
   *
   * @retval  最初にヒットしたペインのインデックス
   * @retval  LYTSYS_PANEINDEX_ERROR  ヒットしない
   *
   * @note
   *  ヒットチェックを行う前に必ず一度Drawされている必要があります。
   *  また、1つ前の描画位置でヒットチェックが行われます。
   *
   * @note
   *  回転には対応しておりません。
   *
   * @note
   *  部品内のペインについては、最初に部品ペインにヒットするため不要。
   */
  //-----------------------------------------------------------------------------
  LytPaneIndex GetHitPaneIndexAll( s32 x, s32 y, bool boundingOnly ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   GroupPaneヒットチェック
   *
   * @param   groupIndex    GroupのIndex
   * @param   x             スクリーン座標系でのX座標 ( タッチパネル座標 )
   * @param   y             スクリーン座標系でのY座標 ( タッチパネル座標 )
   * @param   boundingOnly  バウンディングペインのみチェックする:true
   *
   * @retval  最初にヒットしたペイン
   * @retval  NULL  ヒットしない
   *
   * @note
   *  ヒットチェックを行う前に必ず一度Drawされている必要があります。
   *  また、1つ前の描画位置でヒットチェックが行われます。
   *
   * @note
   *  回転には対応しておりません。
   */
  //-----------------------------------------------------------------------------
  LytPane* GetHitPaneGroup( LytPaneIndex groupIndex, s32 x, s32 y, bool boundingOnly ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   GroupPaneヒットチェック ( インデックスを返す )
   *
   * @param   groupIndex    GroupのIndex
   * @param   x             スクリーン座標系でのX座標 ( タッチパネル座標 )
   * @param   y             スクリーン座標系でのY座標 ( タッチパネル座標 )
   * @param   boundingOnly  バウンディングペインのみチェックする:true
   *
   * @retval  最初にヒットしたペインのインデックス
   * @retval  LYTSYS_PANEINDEX_ERROR  ヒットしない
   *
   * @note
   *  ヒットチェックを行う前に必ず一度Drawされている必要があります。
   *  また、1つ前の描画位置でヒットチェックが行われます。
   *
   * @note
   *  回転には対応しておりません。
   */
  //-----------------------------------------------------------------------------
  LytPaneIndex GetHitPaneIndexGroup( LytPaneIndex groupIndex, s32 x, s32 y, bool boundingOnly ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   単体Paneとのヒットチェック ( インデックス指定版 )
   *
   * @param   x       スクリーン座標系でのX座標 ( タッチパネル座標 )
   * @param   y       スクリーン座標系でのY座標 ( タッチパネル座標 )
   * @param   index   ペインインデックス
   *
   * @retval  true    ヒットしている。 
   * @retval  false   ヒットしていない。
   *
   * @note
   *  ヒットチェックを行う前に必ず一度Drawされている必要があります。
   *  また、1つ前の描画位置でヒットチェックが行われます。
   *
   * @note
   *  回転には対応しておりません。
   */
  //-----------------------------------------------------------------------------
  bool GetHitPane( s32 x, s32 y, LytPaneIndex index ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   単体Paneとのヒットチェック ( ペイン指定版 )
   *
   * @param   x       スクリーン座標系でのX座標 ( タッチパネル座標 )
   * @param   y       スクリーン座標系でのY座標 ( タッチパネル座標 )
   * @param   index   ペインインデックス
   *
   * @retval  true    ヒットしている。 
   * @retval  false   ヒットしていない。
   *
   * @note
   *  ヒットチェックを行う前に必ず一度Drawされている必要があります。
   *  また、1つ前の描画位置でヒットチェックが行われます。
   *
   * @note
   *  回転には対応しておりません。
   */
  //-----------------------------------------------------------------------------
  bool GetHitPane( s32 x, s32 y, LytPane * pane ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   マテリアルのテクスチャを差し替える
   *
   * @param   materialIndex   マテリアルのインデックス
   * @param   mapIndex        １マテリアル内の何番目のテクスチャを変更するか？
   * @param   cp_res          リソースワーク
   * @param   res_mult_id     リソース登録ID
   * @param   texArcIndex     テクスチャのアーカイブインデックス
   */
  //-----------------------------------------------------------------------------
  void SetMaterialImage( LytPaneIndex materialIndex, u32 mapIndex, const LytRes* cp_res, u32 res_multi_id, LytArcIndex texArcIndex );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  リソース情報を取得
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
  void SetTranslate( const gfl2::math::Vector3& pos );

  //----------------------------------------------------------------------------
  /**
   *  @brief  座標の取得
   *
   *  @param  p_pos   座標格納先
   */
  //-----------------------------------------------------------------------------
  void GetTranslate( gfl2::math::Vector3* p_pos ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  拡大の設定
   *
   *  @param  scale   スケーリング値
   */
  //-----------------------------------------------------------------------------
  void SetScale( const gfl2::math::Vector2& scale );

  //----------------------------------------------------------------------------
  /**
   *  @brief  拡大の取得
   *
   *  @param p_scale スケーリング値格納先
   */
  //-----------------------------------------------------------------------------
  void GetScale( gfl2::math::Vector2* p_scale ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  回転の設定
   *
   *  @param  rotate   各軸の回転値
   */
  //-----------------------------------------------------------------------------
  void SetRotate( const gfl2::math::Vector3& rotate );

  //----------------------------------------------------------------------------
  /**
   *  @brief  回転の取得
   *
   *  @param  p_rotate   各軸の回転値格納先
   */
  //-----------------------------------------------------------------------------
  void GetRotate( gfl2::math::Vector3* p_rotate ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  カリングフラグの設定
   *
   *  @param  flag  フラグ
   *
   *  カリングをfalseにするということは、
   *  左、右、下すべての画面でレンダリング処理が実行されるということですので、
   *  処理負荷に注意してください。
   */
  //-----------------------------------------------------------------------------
  void SetCullingFlag( bool flag );

  //----------------------------------------------------------------------------
  /**
   *  @brief  カリングフラグの取得
   *
   *  @retval  true  カリング処理ON
   *  @retval false カリング処理OFF
   */
  //-----------------------------------------------------------------------------
  bool IsCullingFlag( void ) const;


  //============================================================================
  /**
   *  アニメーション関連
   */
  //=============================================================================

  //-----------------------------------------------------------------------------
  /**
   * @brief   アニメーションの追加登録
   *
   * @param   anmResID  アニメーションリソースID
   * @param   resID     リソースID
   *
   * @retval  "LYTSYS_ANIMEINDEX_ERROR != 登録番号"
   * @retval  "LYTSYS_ANIMEINDEX_ERROR = 登録失敗"
   */
  //-----------------------------------------------------------------------------
  u32 CreateAnime( LytArcIndex anmDataID, const LytMultiResID * resID );

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
   *  @brief    非表示のペインに対して行列計算を行うかどうかを設定する
   *
   *  @return   none
   */
  //-----------------------------------------------------------------------------
  void SetInvisiblePaneCalculateMtx( bool flg );

  //----------------------------------------------------------------------------
  /**
   *  @brief    コンバート時に埋め込まれたフォント幅を取得する。
   *            実際は下記帳ユーザーデータにg2d_fwとして埋め込まれた実数
   *
   *  @return   f32 幅。0の時は取得失敗
   */
  //-----------------------------------------------------------------------------
  f32 GetFontWidthWhenConvert( LytPaneIndex index );


  //----------------------------------------------------------------------------
  /**
   *  @brief  描画中か
   *  @return  trueのとき描画中(描画中はLytSys::RemoveLytWkで破棄してはならない)
   *  @note   LytWkBase(m_base)のm_drawingCountを見張っている
   */
  //----------------------------------------------------------------------------
  bool IsDrawing(void) const;

  //! @name  描画中カウント(friendであるDrawingLytWkManagerからしかアクセスしない)
  //@{
private:
  void IncreaseDrawingCount(void);
  void DecreaseDrawingCount(void);
  //@}


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
  void Initialize( gfl2::heap::NwAllocator* allocator, gfl2::heap::NwAllocator* dev_allocator, LytWkManager* p_wkman );
public:
  LytWkBase* GetLytWkBase( void ) const;


private:
  void Finalize( void );

  LytWkManager* m_parent;

  LytWkBase*  m_base;
};


GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

#endif  // GFL2_LYTSYS_H_INCLUDED
