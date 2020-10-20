//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_LytSys.cpp
 *	@brief  レイアウトシステム
 *	@author	tomoya takahashi
 *	@date		2010.10.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <nw/lyt.h>

#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>

#include <grp/gfl_GraphicsSystem.h>

#include <grp/g2d/gfl_LytSys.h>
#include <grp/g2d/gfl_LytWkBase.h>
#include <grp/g2d/gfl_LytRes.h>

#include <gfl_Std.h>
#include <gfl_Fs.h>
#include <ro/gfl_RoManager.h>

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------

namespace gfl {
namespace grp {
namespace g2d {

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
typedef struct
{
  DisplayType     lod;
  Env::CameraType cameraType;
} LytSysLodState;
  
static const LytSysLodState sc_LytSysLodAnmProjIdx[ DISPLAY_BOTH_MAX ] = {
  { DISPLAY_UPPER,    Env::CAMERA_NORMAL },
  { DISPLAY_LOWER,    Env::CAMERA_NORMAL },
  { DISPLAY_UPPER,    Env::CAMERA_RIGHT },
};


//-------------------------------------
///	上下画面に描画するためのデフォルト設定
//=====================================
const LytWkSetUp c_LytWkUpperDefaultSetUp = 
{
  gfl::math::VEC3(0,LYTSYS_DEFAULT_CAMERA_UPPER_POS_Y,0), 
  gfl::math::VEC2(1,1), 
  gfl::math::VEC3(0,0,0), 
  0,
  0
};

const LytWkSetUp c_LytWkLowerDefaultSetUp = 
{
  gfl::math::VEC3(0,LYTSYS_DEFAULT_CAMERA_LOWER_POS_Y,0), 
  gfl::math::VEC2(1,1), 
  gfl::math::VEC3(0,0,0), 
  0,
  0
};

//-----------------------------------------------------------------------------
/**
 *          LytResManagerクラス宣言
 */
//-----------------------------------------------------------------------------
class LytResManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(LytResManager);


public:
  LytResManager( heap::NwAllocator* allocator, u32 lyt_res_max );
  ~LytResManager();

  LytRes* GetClearLytRes( LytResID* idx ) const;
  
  LytRes* GetIndexLytRes( LytResID idx ) const;

  u32 GetUseLytResNum( void ) const;

private:

  // レイアウトリソース配列
  LytRes*   m_lytArray;
  u32       m_lytArrayNum;
};

//-----------------------------------------------------------------------------
/**
 *          LytWkManagerクラス宣言
 */
//-----------------------------------------------------------------------------
class LytWkManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(LytWkManager);

public:
  LytWkManager( heap::NwAllocator* allocator, u32 wk_max );
  ~LytWkManager();

  LytWk* GetClearWk( void ) const;
  void UpdateAll( LytWKGroupID groupID );
  void DrawAll( DisplayType drawtype, RenderTarget* renderTarget, LytWKGroupID groupID );

  void AddLink( LytWk* wk );
  void DelLink( LytWk* wk );
  void SortPriority( LytWk* wk );

  inline u32 GetCreateWkNum( void ) const { return m_createNum; }
  
private:

  LytWk*  m_array;
  u16     m_arrayNum;
  u16     m_createNum;

  LytWk  m_linkTop;  // リンクリスト

  bool CheckGroupID( u16 groupID_a, u16 groupID_b ) const;
  
};

//-----------------------------------------------------------------------------
/**
 *					LytSysBaseクラス宣言
*/
//-----------------------------------------------------------------------------
class LytSysBase
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(LytSysBase);


public:
  // システム初期化
  static void InitializeSystem( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  // システム破棄
  static void FinalizeSystem( void );

  
  // Defaultフォント設定
  static void SetDefaultFont( gfl::str::Font* font, const char* name, u32 index );
  // Defaultフォントがあるかチェック
  static bool IsDefaultFont(u32 index);
  // Defaultフォントを取得
  static gfl::str::Font* GetDefaultFont(u32 index);
  // Defaultフォント名取得
  static const char* GetDefaultFontName(u32 index);
  // Defaultフォント解除
  static void ResetDefaultFontInfo( u32 index );


  // コンストラクタデストラクタ
  LytSysBase( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  LytSysBase(void);
  ~LytSysBase();

  // 初期化の流れ
  void Initialize( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  void InitializeAsync( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  bool InitializeAsyncWait( void );
  inline bool IsInitialized( void ) const { return m_Initialized; }
  
  
  // 情報を取得するインターフェース
  LytWkBaseGlobalData* GetGlobalData(void);
  LytResManager* GetResManager(void) const;
  LytWkManager* GetWkManager(void) const;


  gfl::heap::NwAllocator* GetHeapAllocator(void) const;
  gfl::heap::NwAllocator* GetDeviceAllocator(void) const;
  

private:

  // 同時に何個読み込むか？
  static const s32 MULTI_LOAD_ROMFILE_NUM = 3;
  
  // フォント名用バッファサイズ
  static const u32 FONT_NAME_LENGTH = 64;
  
private:

  bool m_Initialized;
  bool m_InitializeSystem;

  gfl::heap::NwAllocator*         m_allocator;    // メモリアロケータ
  gfl::heap::NwAllocator*         m_devAllocator;// デバイスメモリアロケータ

  
  LytWkBaseGlobalData*   m_globalData;      // 全LytWk共有データ
  LytResManager*         m_lytResMan;       // レイアウトリソースマネージャ
  LytWkManager*          m_wkMan;           // ワークマネージャ

  static gfl::str::Font* m_defaultFont[LYTSYS_DEFAULT_FONT_INDEX_MAX];     // Defaultフォント
  // Defaultフォント名
  // dll内からの文字列リテラル指定に対応するため、ポインタから配列へ変更。
  static char            m_defaultFontName[LYTSYS_DEFAULT_FONT_INDEX_MAX][FONT_NAME_LENGTH];

  // グローバルリソース　ドロワー
  static bool                      m_systemInitialized;
  static nw::lyt::GraphicsResource m_globalResource;
  static nw::lyt::Drawer           m_drawer;
  static gfl::fs::RomFile          m_RomFile;
  static s32                       m_lytSysCount;
};
//ユーザーアロケーター設定
#define LYTSYSBASE_CHANGE_USE_ALLOCATOR(wk)  \
  nw::os::IAllocator* p_default_allocator_heap = nw::lyt::Layout::GetAllocator();\
  nw::os::IAllocator* p_default_allocator_device = nw::lyt::Layout::GetDeviceMemoryAllocator();\
  nw::lyt::Layout::SetAllocator( (wk)->GetHeapAllocator() );\
  nw::lyt::Layout::SetDeviceMemoryAllocator( (wk)->GetDeviceAllocator() );

// デフォルトアロケーターに戻す
#define LYTSYSBASE_RETURN_ALLOCATOR(wk)  \
  nw::lyt::Layout::SetAllocator( p_default_allocator_heap );\
  nw::lyt::Layout::SetDeviceMemoryAllocator( p_default_allocator_device );



//-----------------------------------------------------------------------------
/**
 *    Defaultフォント実態
 */
//-----------------------------------------------------------------------------
gfl::str::Font* LytSysBase::m_defaultFont[LYTSYS_DEFAULT_FONT_INDEX_MAX]      = {NULL};
char            LytSysBase::m_defaultFontName[LYTSYS_DEFAULT_FONT_INDEX_MAX][LytSysBase::FONT_NAME_LENGTH];// Defaultフォント名
bool            LytSysBase::m_systemInitialized = false;
nw::lyt::GraphicsResource LytSysBase::m_globalResource;   ///<グローバルリソース
nw::lyt::Drawer           LytSysBase::m_drawer;           ///<ドロわー
gfl::fs::RomFile          LytSysBase::m_RomFile;          ///<ロムファイルス
s32             LytSysBase::m_lytSysCount = 0;            ///<生成したレイアウトシステムの数

  
  
//=============================================================================
/**
 *					レイアウトシステムクラス　実装
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトシステムの初期化
 *	        *ゲーム開始時に一度だけ設定してください。
 */
//-----------------------------------------------------------------------------
void LytSys::InitializeSystem( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  LytSysBase::InitializeSystem( heapAllocator, deviceAllocator );
}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトシステムの破棄
 */
//-----------------------------------------------------------------------------
void LytSys::FinalizeSystem( void )
{
  LytSysBase::FinalizeSystem();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  Defaultフォント設定
 *      　  ＊ゲーム開始時に一度だけ設定してください。
 *
 *  @param  font                  フォントデータ
 *  @param  name                  リソース名　(例　simple.bcfnt)　レイアウトがアクセスするための名称
 *  @param  index                 インデックス
 */
//-----------------------------------------------------------------------------
void LytSys::SetDefaultFont( gfl::str::Font* font, const char* name, u32 index )
{
  LytSysBase::SetDefaultFont( font, name, index );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  Defaultフォント解除
 *  @param  index                 インデックス
 */
//-----------------------------------------------------------------------------
void LytSys::ResetDefaultFontInfo( u32 index )
{
  LytSysBase::ResetDefaultFontInfo( index );
}


//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 *	
 *	@param	setup   セットアップ情報
 */
//-----------------------------------------------------------------------------
LytSys::LytSys( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  gfl::heap::HeapBase* heap = heapAllocator->GetHeapBase();
  
  // LytSysBase生成
  m_base = GFL_NEW(heap) LytSysBase( setup, heapAllocator, deviceAllocator );
}

//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
LytSys::LytSys( heap::HeapBase* p_heap )   // 非同期初期化モード（デフォルトリソースのスレッド読み込みに対応）
{
  // LytSysBase生成
  m_base = GFL_NEW(p_heap) LytSysBase();
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
LytSys::~LytSys()
{
  GFL_DELETE m_base;
}


//----------------------------------------------------------------------------
/**
 *	@brief  非同期初期化（　LytSys（void）で初期化したときのみ必要　）
 *
 *	@retval true  同期完了
 *	@retval false 同期中
 */
//-----------------------------------------------------------------------------
void LytSys::InitializeAsync( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  m_base->InitializeAsync( setup, heapAllocator, deviceAllocator );
}


//----------------------------------------------------------------------------
/**
 *	@brief  非同期初期化完了まち
 *	@retval true  非同期初期化
 *	@retval false 非同期初期化中
 */
//-----------------------------------------------------------------------------
bool LytSys::InitializeAsyncWait( void )
{
  return m_base->InitializeAsyncWait();
}


//----------------------------------------------------------------------------
/**
 *	@brief  初期化が完了しているかチェック
 *	@retval true  完了
 *	@retval false 途中
 */
//-----------------------------------------------------------------------------
bool LytSys::IsInitialized( void ) const
{
  return m_base->IsInitialized();
}

//----------------------------------------------------------------------------
/**
 *	@brief  オート描画フラグがONになっている全Wkのデータ更新処理
 *
 *	@param  lytwkGroupID    Updateを行うレイアウトワークグループID
 */
//-----------------------------------------------------------------------------
void LytSys::UpdateAll( LytWKGroupID lytwkGroupID )
{
  GFL_ASSERT( m_base->IsInitialized() );
  
  m_base->GetWkManager()->UpdateAll( lytwkGroupID );
}


//----------------------------------------------------------------------------
/**
 *	@brief  オート描画フラグがONになっている全Wkの描画処理
 *
 *  @param    graphics          グラフィックスシステム
 *	@param	  drawtype          表示面定義
 *	@param    renderTarget      レンダリングターゲットのフレームバッファ
 *	@param    rightRenderTarget 右目用レンダリングターゲット　（DISPLAY_UPPERのときのみ参照）
 *	@param    lytwkGroupID      描画を行うLYTWKグループID(ペイングループIDとは違うので注意！）
 */
//-----------------------------------------------------------------------------
void LytSys::DrawAll( gfl::grp::GraphicsSystem* graphics, DisplayType drawtype, RenderTarget* renderTarget, RenderTarget* rightRenderTarget, LytWKGroupID lytwkGroupID )
{
  GFL_ASSERT( m_base->IsInitialized() );

  if( graphics->BindCurrentCommandList( renderTarget ) )
  {
    renderTarget->ActivateBuffer();
    m_base->GetWkManager()->DrawAll( drawtype, renderTarget, lytwkGroupID );
  }

  if( (drawtype == DISPLAY_UPPER) && (rightRenderTarget != NULL) ){
    if( graphics->BindCurrentCommandList( rightRenderTarget ) )
    {
      rightRenderTarget->ActivateBuffer();
      m_base->GetWkManager()->DrawAll( DISPLAY_UPPER_RIGHT, rightRenderTarget, lytwkGroupID );
    }
  }
}

/*コマンドキャッシュ使用例
void LytSys::DrawAll( DisplayType drawtype, FrameBuffer* frameBuffer, FrameBuffer* rightFrameBuffer )
{
  gfl::grp::GraphicsSystem* p_grpSys = GFL_SINGLETON_INSTANCE(GraphicsSystem);
  
  // セーブ
  p_grpSys->StartCommandSave();
  m_base->GetWkManager()->DrawAll( drawtype, frameBuffer );
  p_grpSys->EndCommandSave();


  // フレームバッファのバインド
  frameBuffer->Bind();
  p_grpSys->ReuseCommandSave();

  if( (drawtype == DISPLAY_UPPER) && (rightFrameBuffer != NULL) ){
    // フレームバッファのバインド
    rightFrameBuffer->Bind();
    p_grpSys->ReuseCommandSave();
  }
}
*/

//----------------------------------------------------------------------------
/**
 *	@brief  UpperLowerに対応した描画環境データ
 *
 *	@param	描画面　UPPER　LOWER
 *
 *	@return 描画環境データ
 */
//-----------------------------------------------------------------------------
Env* LytSys::GetGlobalEnv( DisplayType drawtype )
{
  LytWkBaseGlobalData* globalData = m_base->GetGlobalData();
  if( drawtype < DISPLAY_NORMAL_MAX ){
    return &globalData->env[drawtype];
  }
  GFL_ASSERT(0);
  return &globalData->env[0];
}


//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトリソースの読み込み
 *
 *	@param  devAllocator デバイスアロケータ
 *	@param  cp_buff          データ  (内部では、破棄しません。）
 *	@param  useDefaultFont  Defaultフォントを使用するか？
 *	@param  addTextureNum   後から追加するテクスチャ数
 *
 *	@retval リソースID
 *	@retval LTYSYS_RESID_ERROR  リソースが登録できなかった。
 */
//-----------------------------------------------------------------------------
LytResID LytSys::LoadLytRes( heap::NwAllocator* devAllocator, const void * cp_buff, bool useDefaultFont, u32 addTextureNum )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;
  LytResID     idx;

  res = lytResMan->GetClearLytRes( &idx );
  if( res == NULL ){
    return LYTSYS_RESID_ERROR;
  }

  // メモリの確保先を設定
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);
  

  // 読み込み処理
  res->Load( devAllocator, cp_buff, addTextureNum );

  // Defaultフォントの設定
  if( useDefaultFont ){
    for( u32 i=0; i<LYTSYS_DEFAULT_FONT_INDEX_MAX; ++i ){
      if( LytSysBase::IsDefaultFont(i) ){
        res->SetDefaultFont( LytSysBase::GetDefaultFont(i), LytSysBase::GetDefaultFontName(i), i );
      }
    }
  }

  // メモリの確保先をもとに戻す
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
  
  return idx;
}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトリソースの読み込み（圧縮データ対応版）
 *
 *	@param  devAllocator デバイスアロケータ
 *	@param  arcFile   アーカイブファイル
 *	@param  dataID    データID
 *	@param	comp			圧縮データフラグ　true = 圧縮データ
 *	@param  useDefaultFont  Defaultフォントを使用するか？
 *	@param  addTextureNum   後から追加するテクスチャ数
 *
 *	@retval リソースID
 *	@retval LTYSYS_RESID_ERROR  リソースが登録できなかった。
 */
//-----------------------------------------------------------------------------
LytResID LytSys::LoadLytRes( heap::NwAllocator* devAllocator, fs::ArcFile* arcFile, u32 dataID, bool comp, bool useDefaultFont, u32 addTextureNum )
{
	LytResID	resID;

  // メモリの確保先を設定
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

	if( comp == false ){
		resID = LoadLytRes( devAllocator, arcFile, dataID, useDefaultFont, addTextureNum );
	}else{
		resID = LoadLytResComp( devAllocator, arcFile, dataID, useDefaultFont, addTextureNum );
	}

  // メモリの確保先をもとに戻す
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);

	return resID;
}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトリソースの読み込み（非圧縮データ）
 *
 *	@param  devAllocator デバイスアロケータ
 *	@param  arcFile   アーカイブファイル
 *	@param  dataID    データID
 *	@param  useDefaultFont  Defaultフォントを使用するか？
 *	@param  addTextureNum   後から追加するテクスチャ数
 *
 *	@retval リソースID
 *	@retval LTYSYS_RESID_ERROR  リソースが登録できなかった。
 */
//-----------------------------------------------------------------------------
LytResID LytSys::LoadLytRes( heap::NwAllocator* devAllocator, fs::ArcFile* arcFile, u32 dataID, bool useDefaultFont, u32 addTextureNum )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;
  LytResID     idx;

  res = lytResMan->GetClearLytRes( &idx );
  if( res == NULL ){
    return LYTSYS_RESID_ERROR;
  }

  // メモリの確保先を設定
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

  // 読み込み処理
  res->Load( devAllocator, arcFile, dataID, addTextureNum );

  // Defaultフォントの設定
  if( useDefaultFont ){
    for( u32 i=0; i<LYTSYS_DEFAULT_FONT_INDEX_MAX; ++i ){
      if( LytSysBase::IsDefaultFont(i) ){
        res->SetDefaultFont( LytSysBase::GetDefaultFont(i), LytSysBase::GetDefaultFontName(i), i );
      }
    }
  }

  // メモリの確保先をもとに戻す
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
  
  return idx;
}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトリソースの読み込み（圧縮データ）
 *
 *	@param  devAllocator デバイスアロケータ
 *	@param  arcFile   アーカイブファイル
 *	@param  dataID    データID
 *	@param  useDefaultFont  Defaultフォントを使用するか？
 *	@param  addTextureNum   後から追加するテクスチャ数
 *
 *	@retval リソースID
 *	@retval LTYSYS_RESID_ERROR  リソースが登録できなかった。
 */
//-----------------------------------------------------------------------------
LytResID LytSys::LoadLytResComp( heap::NwAllocator* devAllocator, fs::ArcFile* arcFile, u32 dataID, bool useDefaultFont, u32 addTextureNum )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;
  LytResID     idx;

  res = lytResMan->GetClearLytRes( &idx );
  if( res == NULL ){
    return LYTSYS_RESID_ERROR;
  }

  // メモリの確保先を設定
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

  // 読み込み処理
  res->LoadComp( devAllocator, arcFile, dataID, addTextureNum );

  // Defaultフォントの設定
  if( useDefaultFont ){
    for( u32 i=0; i<LYTSYS_DEFAULT_FONT_INDEX_MAX; ++i ){
      if( LytSysBase::IsDefaultFont(i) ){
        res->SetDefaultFont( LytSysBase::GetDefaultFont(i), LytSysBase::GetDefaultFontName(i), i );
      }
    }
  }

  // メモリの確保先をもとに戻す
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
  
  return idx;
}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトリソースの破棄
 *
 *	@param  resID     破棄するリソースID
 */
//-----------------------------------------------------------------------------
void LytSys::ReleaseLytRes( LytResID resID )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;

  res = lytResMan->GetIndexLytRes( resID );
  if( res ){
    res->Release();
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  dstIDのリソースにsrcIDのテクスチャリソースをすべて追加する。
 *
 *	@param	srcID     共有テクスチャデータが入っているリソースID
 *	@param	dstID     共有テクスチャデータを追加するリソースID
 *
 * ＊追加されるテクスチャの枚数をLoadLytRes関数のaddTextureNumに渡して管さい。
 *	
 */
//-----------------------------------------------------------------------------
void LytSys::RegistTextureRes( LytResID srcID, LytResID dstID, gfl::grp::MemoryArea area )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  const LytRes* srcRes;
  LytRes* dstRes;

  // メモリの確保先を設定
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

  srcRes = lytResMan->GetIndexLytRes( srcID );
  dstRes = lytResMan->GetIndexLytRes( dstID );
  if( dstRes && srcRes ){
    // テクスチャResist
    dstRes->RegistTexture( srcRes, area );
  }

  // メモリの確保先をもとに戻す
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
}


// ここでメモリ配置しなかったテクスチャは、デバイスメモリに配置されます。
//-----------------------------------------------------------------------------
/**
 *  @brief  リソースID内にあるテクスチャをすべてメモリに配置する
 *  
 *  @param  resID   リソースID
 *  @param  area    転送先メモリ　（FCRAM/VRAMA/VRAMB）
 */
//-----------------------------------------------------------------------------
void LytSys::TransferAllTexture( LytResID resID, gfl::grp::MemoryArea area )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;

  // メモリの確保先を設定
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

  res = lytResMan->GetIndexLytRes( resID );
  if( res ){
    res->TransferAllTexture( area );
  }

  // メモリの確保先をもとに戻す
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
}

//-----------------------------------------------------------------------------
/**
 *  @brief  リソースID内にあるテクスチャをすべてメモリに配置する
 *  
 *  @param  resID         リソースID
 *  @param  texArcIndex   アーカイブ内のテクスチャインデックス
 *  @param  area          転送先メモリ　（FCRAM/VRAMA/VRAMB）
 */
//-----------------------------------------------------------------------------
void LytSys::TransferIndexTexture( LytResID resID, LytArcIndex texArcIndex, gfl::grp::MemoryArea area )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;

  // メモリの確保先を設定
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

  res = lytResMan->GetIndexLytRes( resID );
  if( res ){
    res->TransferIndexTexture( texArcIndex, area );
  }

  // メモリの確保先をもとに戻す
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
}


//-----------------------------------------------------------------------------
/**
 *  @brief  リソースIDのリソースを取得する。
 *  
 *  @param  resID         リソースID
 *
 *  @retval リソースAccessor
 */
//-----------------------------------------------------------------------------
LytRes* LytSys::GetResource( LytResID resID ) const
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;

  res = lytResMan->GetIndexLytRes( resID );
  return res;
}

//----------------------------------------------------------------------------
/**
 *	@brief  格納しているリソースの総数をチェック
 *
 *	@retval リソースの総数
 */
//-----------------------------------------------------------------------------
u32 LytSys::GetUseResourceNum( void ) const
{
  LytResManager* lytResMan = m_base->GetResManager();
  return lytResMan->GetUseLytResNum();
}


//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトワーク生成
 *
 *	@param  resID       レイアウトリソースID
 *	@param  lytDataID   resID内のレイアウトデータID
 *	@param  anmResIDTbl アニメーションデータIDテーブル      アニメのない場合はNULL
 *	@param  anmTblNum   アニメーションデータIDテーブル数    アニメのない場合は0
 *	@param  anmMax      登録アニメーション最大数
 *	@param  wkdata      レイアウトワーク基本情報
 *	@param  anmBind     アニメーションのバインドを行う。
 *  @param  createPaneTbl     ペインテーブル生成
 *
 *  @retval レイアウトワーク
 *  @retval NULL              生成失敗
 */
//-----------------------------------------------------------------------------
LytWk* LytSys::CreateLytWk( LytResID resID, LytArcIndex lytDataID, const LytArcIndex* anmDataIDTbl, u32 anmTblNum, u32 anmMax, const LytWkSetUp* setup, b32 anmBind, b32 createPaneTbl, b32 createPaneTblWithHash )
{
  return CreateLytWk( m_base->GetHeapAllocator(), m_base->GetDeviceAllocator(), resID, lytDataID, anmDataIDTbl, anmTblNum, anmMax, setup, anmBind, createPaneTbl, createPaneTblWithHash );
}

// Allocator指定版
LytWk* LytSys::CreateLytWk( gfl::heap::NwAllocator* p_heap, gfl::heap::NwAllocator* p_device, LytResID resID, LytArcIndex lytDataID, const LytArcIndex* anmDataIDTbl, u32 anmTblNum, u32 anmMax, const LytWkSetUp* setup, b32 anmBind, b32 createPaneTbl, b32 createPaneTblWithHash )
{
  LytWk* wk = BuildLytWk( p_heap, p_device, resID, lytDataID, anmMax, setup, createPaneTbl, createPaneTblWithHash );

  if( wk != NULL ){
    BuildAnimation( wk, resID, anmDataIDTbl, anmTblNum, anmBind, createPaneTblWithHash );
  }
  return wk;
}


//-----------------------------------------------------------------------------
/**
 * @brief レイアウトワーク生成（レイアウトインスタンスの生成部）
 */
//-----------------------------------------------------------------------------
LytWk* LytSys::BuildLytWk( gfl::heap::NwAllocator* p_heap, gfl::heap::NwAllocator* p_device, LytResID resID, LytArcIndex lytDataID, u32 anmMax, const LytWkSetUp* setup, b32 createPaneTbl, b32 createPaneTblWithHash )
{
  LytWk*  wk     = m_base->GetWkManager()->GetClearWk();
  LytRes* lytRes = m_base->GetResManager()->GetIndexLytRes( resID );
  GFL_ASSERT_STOP(lytRes);

  if( wk != NULL )
  {
    LytWkBase* base = wk->GetLytWkBase();

    base->Initialize( p_heap, p_device, lytRes, lytDataID, anmMax, m_base->GetGlobalData(), createPaneTbl, createPaneTblWithHash );

    // setupの内容を反映
    {
      base->SetTranslate( setup->pos );
      base->SetScale( setup->scale );
      base->SetRotate( setup->rotate );
      base->SetPriority( setup->priority );
      GFL_ASSERT( setup->groupID != LYTSYS_LYTWK_GROUPID_ALL );
      base->SetWkGroupID( setup->groupID );
    }
  }

  return wk;
}


//-----------------------------------------------------------------------------
/**
 * @brief  レイアウトワーク生成（アニメーション設定の生成部）
 */
//-----------------------------------------------------------------------------
void LytSys::BuildAnimation( LytWk* lytWk, LytResID resID, const LytArcIndex* anmDataIDTbl, u32 anmTblNum, b32 anmBind, b32 createPaneTblWithHash )
{
  // アニメーションの設定
  CreateAnimation( lytWk, resID, anmDataIDTbl, 0, anmTblNum, anmBind, createPaneTblWithHash );

  // リストに追加
  AddLinkWithLayoutWork( lytWk );
}


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
void LytSys::CreateAnimation(  LytWk* lytWk, LytResID resID, const LytArcIndex* anmDataIDTbl, u32 anmOffset, u32 anmTblNum, b32 anmBind, b32 createPaneTblWithHash )
{
  LytRes*    lytRes = m_base->GetResManager()->GetIndexLytRes( resID );
  LytWkBase* base   = lytWk->GetLytWkBase();
  GFL_ASSERT_STOP(lytRes);
  GFL_ASSERT_STOP(base);

  for( int i=anmOffset; i<anmTblNum; i++ ){
    base->CreateAnime( i, lytRes, anmDataIDTbl[i], anmBind, createPaneTblWithHash );
  }
}


//----------------------------------------------------------------------------
/**
 * @brief レイアウトワークをレイアウトマネージャにリンクさせる
 *
 * @param lytWk 追加対象となるレイアウトワーク
 */
//----------------------------------------------------------------------------
void LytSys::AddLinkWithLayoutWork( LytWk* lytWk )
{
  m_base->GetWkManager()->AddLink( lytWk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトワーク破棄
 *
 *	@param  lytResID  レイアウトリソースID
 *	@param  anmResID  アニメーションリソースID
 *	@param  wkdata    レイアウトワーク基本情報
 *
 */
//-----------------------------------------------------------------------------
void LytSys::RemoveLytWk( LytWk* lytWk )
{
  LytWkManager* lytWkMan = m_base->GetWkManager();
  LytResManager* lytResMan = m_base->GetResManager();
  LytWkBase* base = lytWk->GetLytWkBase();
  
  // リストから削除
  lytWkMan->DelLink( lytWk );

  // アニメーションの破棄
  {
    int i;
    u32 anime_max = base->GetAnimeMax();
    for( i=0; i<anime_max; i++ ){
      base->DeleteAnime( i );
    }
  }

  // レイアウトワーク破棄
  base->Finalize();
}


//----------------------------------------------------------------------------
/**
 *	@brief  生成されたレイアウトワーク数の取得
 */
//-----------------------------------------------------------------------------
u32 LytSys::GetLytWkNum( void ) const
{
  LytWkManager* lytWkMan = m_base->GetWkManager();
  return lytWkMan->GetCreateWkNum();
}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトシステムベース取得
 */
//-----------------------------------------------------------------------------
LytSysBase* LytSys::GetLytSysBase(void) const
{
  return m_base;
}



//=============================================================================
/**
 *					レイアウトシステム ワーククラス　実装
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief    使用可能状態取得
 *
 *	@retval true    使用可能
 *	@retval false   使用不可能
 */
//-----------------------------------------------------------------------------
bool LytWk::IsInitialized(void)
{
  return m_base->IsInitialized();
}



//----------------------------------------------------------------------------
/**
 *	@brief    個別Update(AutoDrawFlagがfalseのときのみ有効)
 */
//-----------------------------------------------------------------------------
void LytWk::Update( void )
{
  if( m_base->IsAutoDraw() == false ){
    m_base->Update();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    個別描画(AutoDrawFlagがfalseのときのみ有効)
 *
 *	@param	  drawtype    LYTSYS_DRAW_～
 *	@param    renderTarget レンダリングターゲットのフレームバッファ
 *	@param    rightRenderTarget  右目用レンダリングターゲット　（DISPLAY_UPPERのときのみ参照）
 */
//-----------------------------------------------------------------------------
void LytWk::Draw( gfl::grp::GraphicsSystem* graphics, DisplayType drawtype, RenderTarget* renderTarget, RenderTarget* rightRenderTarget )
{
  if( m_base->IsAutoDraw() == false ){
    // drawtypeが不正なら描画しない
    if(drawtype >= DISPLAY_BOTH_MAX){  // Coverity[12721] 範囲外参照
      GFL_ASSERT(drawtype < DISPLAY_BOTH_MAX); //@check
      return ;
    }

    if( graphics->BindCurrentCommandList(renderTarget) )
    {
      renderTarget->ActivateBuffer();
      m_base->Draw( sc_LytSysLodAnmProjIdx[drawtype].lod, sc_LytSysLodAnmProjIdx[drawtype].cameraType, *renderTarget );
    }


    if( (drawtype == DISPLAY_UPPER) && (rightRenderTarget != NULL) ){
      if( graphics->BindCurrentCommandList(rightRenderTarget) )
      {
        rightRenderTarget->ActivateBuffer();
        m_base->Draw( sc_LytSysLodAnmProjIdx[DISPLAY_UPPER_RIGHT].lod, sc_LytSysLodAnmProjIdx[DISPLAY_UPPER_RIGHT].cameraType, *rightRenderTarget );
      }
    }


    //nngxFlush3DCommand();
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  UpperLowerに対応したユーザー描画環境データ設定
 *
 *	@param	upperEnv    上画面用ENV
 *	@param	lowerEnv    下画面用ENV
 *
 *	*描画にGlobalEnvではなく引数のENVを使用するようになります。
 *	*戻すには、SetGlobalEnv()を呼んでください。
 *
 */
//-----------------------------------------------------------------------------
void LytWk::SetUserEnv( const gfl::grp::g2d::Env* upperEnv, const gfl::grp::g2d::Env* lowerEnv )
{
  m_base->SetUserEnv( upperEnv, lowerEnv );
}

//----------------------------------------------------------------------------
/**
 *	@brief  グローバルENVを使用するように設定
 */
//-----------------------------------------------------------------------------
void LytWk::SetGlobalEnv( void )
{
  m_base->SetGlobalEnv();
}


//----------------------------------------------------------------------------
/**
 *	@brief  LytWkGroupIDを設定する
 *
 *	@param	groupID グループID
 */
//-----------------------------------------------------------------------------
void LytWk::SetLytWkGroupID( LytWKGroupID groupID )
{
  GFL_ASSERT( groupID != LYTSYS_LYTWK_GROUPID_ALL );
  m_base->SetWkGroupID( groupID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  LytWkGroupIDを取得する
 *
 *	@return グループID
 */
//-----------------------------------------------------------------------------
LytWKGroupID LytWk::GetLytWkGroupID( void ) const
{
  return m_base->GetWkGroupID();
}


//----------------------------------------------------------------------------
/**
 *	@brief  描画するディスプレイタイプを設定
 *
 *	@param	  lod     描画するディスプレイタイプ
 */
//-----------------------------------------------------------------------------
void LytWk::SetDrawDispType( DisplayType lod )
{
  m_base->SetDrawDispType( lod );
}

//----------------------------------------------------------------------------
/**
 *	@brief  描画するディスプレイタイプの取得
 *
 *	@return 描画するディスプレイタイプ
 */
//-----------------------------------------------------------------------------
DisplayType LytWk::GetDrawDispType( void ) const
{
  return m_base->GetDrawDispType();
}


//----------------------------------------------------------------------------
/**
 *	@brief  非表示ペインのアニメーション更新を省略するフラグ
 *	（デフォルトON）
 */
//-----------------------------------------------------------------------------
void LytWk::SetVisiblePaneNotAnimeOption( bool flag )
{
  m_base->SetVisiblePaneNotAnimeOption( flag );
}

//----------------------------------------------------------------------------
/**
 *	@brief  非表示ペインのアニメーション更新を省略するフラグチェック
 *	@retval true  有効
 *	@retval false 無効
 */
//-----------------------------------------------------------------------------
bool LytWk::IsVisiblePaneNotAnimeOption( void ) const
{
  return m_base->IsVisiblePaneNotAnimeOption();
}




//============================================================================
/**
 *	ペイン操作
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndexのペインを取得
 *
 *	@param	paneIndex GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
 *
 *	@retval ペイン
 *	@retval NULL    Indexが不正
 */
//-----------------------------------------------------------------------------
LytPane* LytWk::GetPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetPane( paneIndex );
}


//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndexのピクチャーペインを取得
 *
 *	@param	paneIndex GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
 *
 *	@retval ピクチャーペイン
 *	@retval NULL Indexが不正か、PicturePaneでない
 */
//-----------------------------------------------------------------------------
LytPicture* LytWk::GetPicturePane( LytPaneIndex paneIndex ) const
{
  return m_base->GetPicturePane( paneIndex );
}

//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndexのテキストボックスペインを取得
 *
 *	@param	paneIndex GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
 *
 *	@retval テキストボックスペイン
 *	@retval NULL Indexが不正か、TextBoxPaneでない
 */
//-----------------------------------------------------------------------------
LytTextBox* LytWk::GetTextBoxPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetTextBoxPane( paneIndex );
}


//----------------------------------------------------------------------------
/**
 *	@brief  PeneIndexのテキストボックスペインの文字列バッファのメモリを確保
 *
 *	@param	paneIndex     ペインインデックス
 *	@param  minLen        確保文字列数
 */
//-----------------------------------------------------------------------------
void LytWk::AllocTextBoxPaneStringBuffer( LytPaneIndex paneIndex, u16 minLen )
{
  m_base->AllocTextBoxPaneStringBuffer( paneIndex, minLen );
}

//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndexのwindowペインを取得
 *
 *	@param	paneIndex GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
 *
 *	@retval windowペイン
 *	@retval NULL Indexが不正かwindowPaneでない
 */
//-----------------------------------------------------------------------------
LytWindow* LytWk::GetWindowPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetWindowPane( paneIndex );
}

//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndexのバウンディングボックスペインを取得
 *
 *	@param	paneIndex GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
 *
 *	@retval バウンディングボックスペイン
 *	@retval NULL Indexが不正かboundingPaneでない
 */
//-----------------------------------------------------------------------------
LytBounding* LytWk::GetBoundingPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetBoundingPane( paneIndex );
}

//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndexのマテリアルを取得
 *
 *	@param	paneIndex GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
 *
 *	@retval マテリアル情報
 *	@retval NULL  Indexが不正かマテリアルではない
 */
//-----------------------------------------------------------------------------
LytMaterial* LytWk::GetMaterial( LytPaneIndex paneIndex ) const
{
  return m_base->GetMaterial( paneIndex );
}


//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndexのグループのペインリストを取得
 *
 *	@param	groupIndex  グループインデックス
 *
 *	@return ペインリスト
 */
//-----------------------------------------------------------------------------
LytPaneList* LytWk::GetPaneList( LytPaneIndex groupIndex ) const
{
  nw::lyt::Group* group = m_base->GetGroup( groupIndex );
  if( group ){
    return &group->GetPaneList();
  }
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ヒットチェックが可能か取得
 *
 *	@retval true  可能
 *	@retval false 不可能    一度描画する必要があります。
 */
//-----------------------------------------------------------------------------
bool LytWk::IsHitCheckOK(void) const
{
  return m_base->IsHitCheckOK();
}

//----------------------------------------------------------------------------
/**
 *	@brief  全Paneヒットチェック
 *
 *	@param	スクリーン座標系でのｘ、ｙ（タッチパネル座標そのまま）
 *	@param  boundingOnly  バウンディングペインのみチェックする:true
 *
 *	@retval 最初にヒットしたペイン
 *	@retval NULL  ヒットしない
 *
 *	*ヒットチェックを行う前に必ず一度Drawされている必要があります。
 *	　また、1つ前の描画位置でヒットチェックが行われます。
 *
 *	*回転には対応しておりません。
 */
//-----------------------------------------------------------------------------
LytPane* LytWk::GetHitPaneAll( s32 x, s32 y, bool boundingOnly ) const
{
  // ヒットチェックが可能かチェック
  if( m_base->IsHitCheckOK() == false ){
    GFL_ASSERT(0);
    return NULL;
  }
  
  LytPane* rootPane = m_base->GetCore()->GetRootPane();
  gfl::math::VEC2 pos = gfl::math::VEC2(static_cast<float>(x-160), static_cast<float>(-y+120));

  
  nw::lyt::PaneList & paneList = rootPane->GetChildList();
  nw::lyt::PaneList::Iterator it      = paneList.GetBeginIter();
  nw::lyt::PaneList::Iterator it_end  = paneList.GetEndIter();

  
  while( it != it_end ){

    // boundingOnly チェック
    if( boundingOnly ){
      // Boundingでなければチェックしない
      if( nw::ut::DynamicCast<nw::lyt::Bounding*>(&(*it)) == NULL ){
        it ++;
        continue;
      }
    }

    if( nw::lyt::IsContain( &(*it), pos ) ){
      return &(*it);
    }

    it ++;
  }
  
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  全Paneヒットチェック
 *
 *	@param	スクリーン座標系でのｘ、ｙ（タッチパネル座標そのまま）
 *	@param  boundingOnly  バウンディングペインのみチェックする:true
 *
 *	@retval 最初にヒットしたペインのインデックス
 *	@retval LYTSYS_PANEINDEX_ERROR  ヒットしない
 *
 *	*ヒットチェックを行う前に必ず一度Drawされている必要があります。
 *	　また、1つ前の描画位置でヒットチェックが行われます。
 *
 *	*回転には対応しておりません。
 */
//-----------------------------------------------------------------------------
LytPaneIndex LytWk::GetHitPaneIndexAll( s32 x, s32 y, bool boundingOnly ) const
{
  const LytPane* pane = GetHitPaneAll(x, y, boundingOnly);
  const LytRes* res = m_base->GetLytRes();
  LytPaneIndex ret = LYTSYS_PANEINDEX_ERROR;
  
  if( pane ){
    ret = res->GetPaneNameIndex( pane->GetName() );
    // pene名から、インデックスを取得
    if( ret == LYTRES_PANENAME_ERROR ){
      return LYTSYS_PANEINDEX_ERROR;
    }
  } 
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  GroupPaneヒットチェック
 *
 *  @param  GroupのIndex　GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
 *	@param	スクリーン座標系でのｘ、ｙ（タッチパネル座標そのまま）
 *	@param  boundingOnly  バウンディングペインのみチェックする:true
 *
 *	@retval 最初にヒットしたペイン
 *	@retval NULL  ヒットしない
 *
 *	*ヒットチェックを行う前に必ず一度Drawされている必要があります。
 *	　また、1つ前の描画位置でヒットチェックが行われます。
 *
 *	*回転には対応しておりません。
 */
//-----------------------------------------------------------------------------
LytPane* LytWk::GetHitPaneGroup( LytPaneIndex groupIndex, s32 x, s32 y, bool boundingOnly ) const
{
  // ヒットチェックが可能かチェック
  if( m_base->IsHitCheckOK() == false ){
    GFL_ASSERT(0);
    return NULL;
  }


  gfl::math::VEC2 pos = gfl::math::VEC2(static_cast<float>(x-160), static_cast<float>(-y+120));
  nw::lyt::Layout* layout = m_base->GetCore();
  nw::lyt::GroupContainer* groupContainer = layout->GetGroupContainer();
  nw::lyt::Group* group;
  const LytRes* res = m_base->GetLytRes();
  const char* name;

  name = res->GetPaneName( groupIndex );
  if( name == NULL ){
    return NULL;
  }
  
  group = groupContainer->FindGroupByName( name );
  if( group == NULL ){
    return NULL;
  }


  nw::lyt::PaneLinkList & paneList = group->GetPaneList();
  nw::lyt::PaneLinkList::Iterator it      = paneList.GetBeginIter();
  nw::lyt::PaneLinkList::Iterator it_end  = paneList.GetEndIter();

  
  while( it != it_end ){

    // boundingOnly チェック
    if( boundingOnly ){
      // Boundingでなければチェックしない
      if( nw::ut::DynamicCast<nw::lyt::Bounding*>(it->target) == NULL ){
        it ++;
        continue;
      }
    }

    if( nw::lyt::IsContain( it->target, pos ) ){
      return it->target;
    }

    it ++;
  }
  
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  GroupPaneヒットチェック
 *
 *  @param  GroupのIndex　GFL_LytSys_ResrouceConv.rbから出力される.._pane.hのインデックス
 *	@param	スクリーン座標系でのｘ、ｙ（タッチパネル座標そのまま）
 *	@param  boundingOnly  バウンディングペインのみチェックする:true
 *
 *	@retval 最初にヒットしたペインのインデックス
 *	@retval LYTSYS_PANEINDEX_ERROR  ヒットしない
 *
 *	*ヒットチェックを行う前に必ず一度Drawされている必要があります。
 *	　また、1つ前の描画位置でヒットチェックが行われます。
 *
 *	*回転には対応しておりません。
 */
//-----------------------------------------------------------------------------
LytPaneIndex LytWk::GetHitPaneIndexGroup( LytPaneIndex groupIndex, s32 x, s32 y, bool boundingOnly ) const
{
  const LytPane* pane = GetHitPaneGroup(groupIndex, x, y, boundingOnly);
  const LytRes* res = m_base->GetLytRes();
  LytPaneIndex ret = LYTSYS_PANEINDEX_ERROR;
  
  if( pane ){
    ret = res->GetPaneNameIndex( pane->GetName() );
    // pene名から、インデックスを取得
    if( ret == LYTRES_PANENAME_ERROR ){
      return LYTSYS_PANEINDEX_ERROR;
    }
  }
  return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief  単体Paneとのヒットチェック
 *
 *	@param	スクリーン座標系でのｘ、ｙ（タッチパネル座標そのまま）
 *	@param  ペインインデックス
 *
 *	@retval true    ヒットしている。 
 *	@retval false   ヒットしていない。
 *
 *	*ヒットチェックを行う前に必ず一度Drawされている必要があります。
 *	　また、1つ前の描画位置でヒットチェックが行われます。
 *
 *	*回転には対応しておりません。
 */
//-----------------------------------------------------------------------------
bool LytWk::GetHitPane( s32 x, s32 y, LytPaneIndex index ) const
{
  LytPane* p_pane = m_base->GetPane( index );
  gfl::math::VEC2 pos = gfl::math::VEC2(static_cast<float>(x-160), static_cast<float>(-y+120));

  if( nw::lyt::IsContain( p_pane, pos ) ){
    return true;
  }

  return false;
}


//----------------------------------------------------------------------------
/**
 *	@brief  マテリアルのテクスチャを差し替える。
 *
 *	@param	paneIndex   マテリアルのインデックス
 *	@param  mapIndex    １マテリアル内の何番目のテクスチャを変更するか？
 *	@param  cp_res      リソースワーク
 *	@param  texArcIndex テクスチャのアーカイブインデックス
 */
//-----------------------------------------------------------------------------
void LytWk::SetMaterialImage( LytPaneIndex materialIndex, u32 mapIndex, const LytRes* cp_res, LytArcIndex texArcIndex )
{
  LytMaterial* p_material;

  // マテリアル取得
  p_material = this->GetMaterial( materialIndex );

  if( p_material ){

    // テクスチャインデックス制限オーバー
    if( mapIndex < p_material->GetTexMapNum() )
    {
      // 張替え
      p_material->SetTexMap( mapIndex, cp_res->GetTexture(texArcIndex) );
    }
    else{
      GFL_ASSERT(0);
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  リソース情報を取得
 *  
 *  @param  resID         リソースID
 *
 *  @retval リソース
 */
//-----------------------------------------------------------------------------
const LytRes* LytWk::GetResource( void ) const
{
  return m_base->GetLytRes();
}





//----------------------------------------------------------------------------
/**
 *	@brief  オート描画フラグ　設定　
 *
 *	@param	flag  フラグ　true:ON　false:OFF
 */
//-----------------------------------------------------------------------------
void LytWk::SetAutoDrawFlag( bool flag )
{
  m_base->SetAutoDrawFlag(flag);
}

//----------------------------------------------------------------------------
/**
 *	@brief    オート描画のON/OFF 取得
 *  
 *	@retval true    ON
 *	@retval false   OFF
 */
//-----------------------------------------------------------------------------
bool LytWk::IsAutoDraw( void ) const
{
  return m_base->IsAutoDraw();
}

//----------------------------------------------------------------------------
/**
 *	@brief  処理優先順位の設定
 *
 *	@param	pri   優先順位（ 0 > 0xffff )
 *
 *	*ただし、デプステストなしで、描画を行う場合には、
 *　後から描画されたワークが手前に描画されるため注意。
 *
 *	UpdateAllとDrawAllを行う際の優先順位です。
 */
//-----------------------------------------------------------------------------
void LytWk::SetPriority( u16 pri )
{
  m_base->SetPriority( pri );
  m_parent->SortPriority( this );
}

//----------------------------------------------------------------------------
/**
 *	@brief  処理優先順位の取得
 *
 *	@retval	優先順位（ 0 > 0xffff )
 *
 *	*ただし、デプステストなしで、描画を行う場合には、
 *　後から描画されたワークが手前に描画されるため注意。
 *
 *	UpdateAllとDrawAllを行う際の優先順位です。
 */
//-----------------------------------------------------------------------------
u16 LytWk::GetPriority( void ) const
{
  return m_base->GetPriority();
}

//----------------------------------------------------------------------------
/**
 *	@brief  座標の設定
 *
 *	@param	pos
 */
//-----------------------------------------------------------------------------
void LytWk::SetTranslate( const gfl::math::VEC3& pos )
{
  m_base->SetTranslate( pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  座標の取得
 *
 *	@param  p_pos   座標格納先
 */
//-----------------------------------------------------------------------------
void LytWk::GetTranslate( gfl::math::VEC3* p_pos ) const
{
  m_base->GetTranslate(p_pos);
}


//----------------------------------------------------------------------------
/**
 *	@brief  拡大の設定
 *
 *	@param	scale   スケーリング値
 */
//-----------------------------------------------------------------------------
void LytWk::SetScale( const gfl::math::VEC2& scale )
{
  m_base->SetScale( scale );
}

//----------------------------------------------------------------------------
/**
 *	@brief  拡大の取得
 *
 *	@retval scale スケーリング値
 */
//-----------------------------------------------------------------------------
void LytWk::GetScale( gfl::math::VEC2* p_scale ) const
{
  m_base->GetScale( p_scale );
}


//----------------------------------------------------------------------------
/**
 *	@brief  回転の設定
 *
 *	@param	rotate   各軸の回転値
 */
//-----------------------------------------------------------------------------
void LytWk::SetRotate( const gfl::math::VEC3& rotate )
{
  m_base->SetRotate( rotate );
}

//----------------------------------------------------------------------------
/**
 *	@brief  回転の取得
 *
 *	@retval	rotate   各軸の回転値
 */
//-----------------------------------------------------------------------------
void LytWk::GetRotate( gfl::math::VEC3* p_rotate ) const
{
  m_base->GetRotate( p_rotate );
}

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
void LytWk::SetCullingFlag( bool flag )
{
  m_base->SetCullingFlag(flag);
}

//----------------------------------------------------------------------------
/**
 *	@brief  カリングフラグの取得
 *
 *	@retval	true  カリング処理ON
 *	@retval false カリング処理OFF
 */
//-----------------------------------------------------------------------------
bool LytWk::IsCullingFlag( void ) const
{
  return m_base->IsCullingFlag();
}


//----------------------------------------------------------------------------
/**
 *	@brief  アニメーションの追加登録
 *
 *	@param	anmResID  アニメーションリソースID
 *
 *	@retval 登録Index
 *	@retval LYTSYS_ANIMEINDEX_ERROR 登録失敗
 */
//-----------------------------------------------------------------------------
u32 LytWk::CreateAnime( LytArcIndex anmDataID )
{
  const LytRes* lytRes = m_base->GetLytRes();
  u32 anmIdx;
  
  if( lytRes ){
    anmIdx = m_base->GetClearAnimeIndex();
    if( anmIdx != LYTWKBASE_ANIMEIDX_ERROR ){
      m_base->CreateAnime( anmIdx, lytRes, anmDataID );
      return anmIdx;
    }
  }

  return LYTSYS_ANIMEINDEX_ERROR;
}



//----------------------------------------------------------------------------
/**
 *	@brief  共有アニメーションの登録    LytPaneIndexを指定して、個別登録するタイプ
 *
 *  @param  startAnimeIndex 登録開始アニメーションインデックス
 *	@param	anmResID        アニメーションリソースID
 *	@param  maxNum          登録最大数（0だとすべて登録）
 *	@param  groupIndex      登録グループインデックス（LYTSYS_PANEINDEX_ERRORだとすべて登録）
 *	@param  paneIndex       登録グループ内ペインインデックス（LYTSYS_PANEINDEX_ERRORだとすべて登録）
 *
 *	@retval 登録数  (startAnimeIndexから戻り値分のインデックスにアニメーションが追加されました。）
 *
 *	*アニメーションワークが連続で確保できないとASSERTでとまります。
 *	*登録数など把握しながら使用してください。
 *
 *	*共有アニメーションとは、
 *	　anmResIDのアニメーションを複数のペインが共通で使用することができます。
 *	LayoutEditor側で、アニメーションを使用することができるペインを指定しておき、
 *	この関数でアニメーションを作成してください。
 *
 *	共有アニメーションという名前ですが、アニメーションフレームの管理は独立しています。
 *	startAnimeIndex+戻り値分のインデックスを指定してアニメーションのフレーム管理を行ってください。
 *
 *	＊大本となっているアニメーションは通常のCreateAnimeで生成してください。
 */
//-----------------------------------------------------------------------------
u32 LytWk::CreateShareAnime( u32 startAnimeIndex, LytArcIndex anmDataID, u32 maxNum, LytPaneIndex groupIndex, LytPaneIndex paneIndex )
{
  const LytRes* lytRes = m_base->GetLytRes();
  
  if( lytRes ){
    return m_base->CreateShareAnime( startAnimeIndex, lytRes, anmDataID, maxNum, groupIndex, paneIndex );
  }
  return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  アニメーションの削除
 *
 *	@param	anmIdx  削除するアニメーションインデックス
 */
//-----------------------------------------------------------------------------
void LytWk::DeleteAnime( u32 anmIdx )
{
  m_base->DeleteAnime( anmIdx );
}

//----------------------------------------------------------------------------
/**
 *	@brief    アニメーション情報があるかチェック
 *
 *  @param  anmaIdx       アニメーションインデックス
 */
//-----------------------------------------------------------------------------
bool LytWk::IsAnime( u32 anmIdx ) const
{
  return m_base->IsAnime( anmIdx );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションの反映
 *
 *  @param  anmIdx    アニメーションIDX
 *  @param  is_auto_unbind   one time アニメーションの場合、アニメーション完了後、unbindする。
 */
//-----------------------------------------------------------------------------
void LytWk::BindAnime( u32 anmIdx, bool is_auto_unbind )
{
  m_base->BindAnime( anmIdx, is_auto_unbind );
}


//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションの反映解除
 *
 *  @param  anmIdx    登録先IDX
 */
//-----------------------------------------------------------------------------
void LytWk::UnBindAnime( u32 anmIdx )
{
  m_base->UnBindAnime( anmIdx );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    全アニメーションの反映解除
 */
//-----------------------------------------------------------------------------
void LytWk::UnBindAnimeAll( void )
{
  m_base->UnBindAnimeAll();
}


//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションがバインドされているかチェック
 *
 *  @retval true    バインドされている。
 *  @retval false   バインドされていない。
 */
//-----------------------------------------------------------------------------
bool LytWk::IsBindAnime( u32 anmIdx ) const
{
  return m_base->IsBindAnime( anmIdx );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    オートアニメーション設定
 *
 *  @param  anmIdx    アニメーションIDX
 *  @param  flag      ON/OFFフラグ
 */
//-----------------------------------------------------------------------------
void LytWk::SetAutoAnime( u32 anmIdx, bool flag )
{
  m_base->SetAutoAnime( anmIdx, flag );
}


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
bool LytWk::IsAutoAnime( u32 anmIdx ) const
{
  return m_base->IsAutoAnime( anmIdx );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    オートアニメーション スピード設定　（全オートアニメ共通）
 *
 *  @param  speed     アニメーションスピード
 */
//-----------------------------------------------------------------------------
void LytWk::SetAutoAnimeSpeed( f32 speed )
{
  m_base->SetAutoAnimeSpeed( speed );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    オートアニメーション スピード設定　（個別設定）
 *
 *  @param  anmIdx    アニメーションインデックス
 *  @param  speed     アニメーションスピード
 */
//-----------------------------------------------------------------------------
void LytWk::SetAutoAnimeSpeed( u32 anmIdx, f32 speed )
{
  m_base->SetAutoAnimeSpeed( anmIdx, speed );
}


//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションフレーム設定
 *
 *  @param  anmIdx    アニメーションIDX
 *  @param  frame     アニメーションフレーム
 */
//-----------------------------------------------------------------------------
void LytWk::SetAnimeFrame( u32 anmIdx, f32 frame )
{
  m_base->SetAnimeFrame( anmIdx, frame );
}

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
bool LytWk::AddAnimeFrame( u32 anmIdx, f32 frame )
{
  return m_base->AddAnimeFrame( anmIdx, frame );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションフレーム取得
 *
 *  @param  anmIdx    アニメーションIDX
 */
//-----------------------------------------------------------------------------
f32 LytWk::GetAnimeFrame( u32 anmIdx ) const
{
  return m_base->GetAnimeFrame( anmIdx );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーション最大フレーム取得
 *
 *  @param  anmIdx    アニメーションIDX
 */
//-----------------------------------------------------------------------------
f32 LytWk::GetAnimeMaxFrame( u32 anmIdx ) const
{
  return m_base->GetAnimeMaxFrame( anmIdx );
}

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
bool LytWk::IsAnimeLoop( u32 anmIdx ) const
{
  return m_base->IsAnimeLoop(anmIdx);
}

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
bool LytWk::IsAnimeEnd( u32 anmIdx ) const
{
  return m_base->IsAnimeEnd( anmIdx );
}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトコアシステム　（ベースライブラリのクラス）取得
 *
 *	@return ベースライブラリのクラス
 */
//-----------------------------------------------------------------------------
LytCore* LytWk::GetLayoutCore( void ) const
{
  return m_base->GetCore();
}


//----------------------------------------------------------------------------
/**
 *	@brief    非表示のペインに対して行列計算を行うかどうかを設定する
 *
 *	@return		none
 */
//-----------------------------------------------------------------------------
void LytWk::SetInvisiblePaneCalculateMtx( bool flg )
{
	m_base->SetInvisiblePaneCalculateMtx(flg);
}

//----------------------------------------------------------------------------
/**
 *	@brief    コンバート時に埋め込まれたフォント幅を取得する。
 *            実際は下記帳ユーザーデータにg2d_fwとして埋め込まれた実数
 *
 *	@return		f32 幅。0の時は取得失敗
 */
//-----------------------------------------------------------------------------
f32 LytWk::GetFontWidthWhenConvert( LytPaneIndex index )
{
  LytTextBox *textPane = GetTextBoxPane(index);
  
  if( textPane )
  {
    const nw::lyt::ExtUserData *extData = textPane->FindExtUserDataByName("g2d_fw");
    if( extData )
    {
      return extData->GetFloatArray()[0];
    }
  }
  return 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ デフォルトコンストラクター
 */
//-----------------------------------------------------------------------------
LytWk::LytWk(void)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
LytWk::~LytWk()
{
  Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param	allocator     アロケータ
 */
//-----------------------------------------------------------------------------
void LytWk::Initialize( gfl::heap::NwAllocator* allocator, LytWkManager* p_wkman )
{
  heap::HeapBase* heap = allocator->GetHeapBase();

  GFL_ASSERT( m_base == NULL );

  m_base = GFL_NEW(heap) LytWkBase();
  m_parent = p_wkman;
}

//----------------------------------------------------------------------------
/**
 *	@brief  破棄
 */
//-----------------------------------------------------------------------------
void LytWk::Finalize( void )
{
  if( m_base ){
    // ベース破棄
    GFL_DELETE m_base;
    m_base = NULL;
    m_parent = NULL;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトワークベース取得
 *
 *	@retval レイアウトワークベース
 */
//-----------------------------------------------------------------------------
LytWkBase* LytWk::GetLytWkBase( void ) const
{
  return m_base;
}


//=============================================================================
/**
 *					レイアウトシステム ベースクラス　実装
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  システム初期化
 *
 *	@param	heapAllocator
 *	@param	deviceAllocator 
 */
//-----------------------------------------------------------------------------
void LytSysBase::InitializeSystem( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  if(m_systemInitialized == false){
  
    gfl::heap::HeapBase* heap = heapAllocator->GetHeapBase();

    // Laytouライブラリ初期化
    nw::lyt::Initialize(heapAllocator, deviceAllocator);
    
    {
      const wchar_t* resourcePath = 0;

      m_globalResource.StartSetup();
      
      for (int i = 0;
          (resourcePath = m_globalResource.GetResourcePath(i)) != NULL;
           ++i)
      {
        void * buff;
        s32 result;
        u32 size;

        GFL_PRINT_EX( GFL_LYTSYS_DEBUGPRINT_LEVEL1, "path = %s addr = %d\n", resourcePath, resourcePath );

        m_RomFile.Open( resourcePath, gfl::fs::OPEN_MODE_READ );
        
        GFL_ASSERT( m_RomFile.GetSize() > 0 );

        // サイズ取得
        size = static_cast<u32>(m_RomFile.GetSize());

        // バッファ作成 このメモリはlytが管理する。
        buff = nw::lyt::Layout::AllocDeviceMemory( size, 128 );
        GFL_ASSERT_STOP(buff);

        result = m_RomFile.Read( buff, size );
        m_RomFile.Close();

        GFL_PRINT_EX( GFL_LYTSYS_DEBUGPRINT_LEVEL1, "readFile size = %d buff = 0x%x result = %d\n", size, buff, result );


        if( result < 0 ){
          // "can not read\n"
          GFL_ASSERT(0);
        }
        m_globalResource.SetResource(i, buff, size, true);
      }
      m_globalResource.FinishSetup();
    }

    // drawer初期化
    m_drawer.Initialize(m_globalResource);

    nngxUpdateState(NN_GX_STATE_ALL);

  
    // システムの初期化完了
    m_systemInitialized = true;
    m_lytSysCount       = 0;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  システム破棄
 */
//-----------------------------------------------------------------------------
void LytSysBase::FinalizeSystem( void )
{
  if( m_systemInitialized )
  {
    GFL_ASSERT_STOP( m_lytSysCount == 0 );
    
    // drawer開放
    m_drawer.Finalize();
    m_globalResource.Finalize();
    // Laytouライブラリ アロケーター情報クリア
    nw::lyt::Initialize(NULL, NULL);

    m_systemInitialized = false;
    m_lytSysCount       = 0;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  Defaultフォント 設定
 *
 *  @param  font                  フォントデータ
 *  @param  name                  リソース名　(例　simple.bcfnt)　レイアウトがアクセスするための名称
 */
//-----------------------------------------------------------------------------
void LytSysBase::SetDefaultFont( gfl::str::Font* font, const char* name, u32 index )
{
  if( index < LYTSYS_DEFAULT_FONT_INDEX_MAX ){
    m_defaultFont[index]       = font;
    strcpy( m_defaultFontName[index], name);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  Defaultフォントがあるかチェック
 *
 *	@retval true  ある
 *	@retval false ない
 */
//-----------------------------------------------------------------------------
bool LytSysBase::IsDefaultFont(u32 index)
{
  if( index < LYTSYS_DEFAULT_FONT_INDEX_MAX ){
    if( m_defaultFont[index] ){
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  Defaultフォント取得
 *
 *	@retval Defaultフォント
 */
//-----------------------------------------------------------------------------
gfl::str::Font* LytSysBase::GetDefaultFont(u32 index)
{
  if( index < LYTSYS_DEFAULT_FONT_INDEX_MAX ){
    return m_defaultFont[index];
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  Defaultフォント名取得
 *
 *	@return Defaultフォント名
 */
//-----------------------------------------------------------------------------
const char* LytSysBase::GetDefaultFontName(u32 index)
{
  if( index < LYTSYS_DEFAULT_FONT_INDEX_MAX ){
    return m_defaultFontName[index];
  }
  return NULL; 
}


//----------------------------------------------------------------------------
/**
 *	@brief  追加されたフォント情報を消す
 *  @param  フォントを指しているindex
 */
//-----------------------------------------------------------------------------
void LytSysBase::ResetDefaultFontInfo(u32 index)
{
  if( index < LYTSYS_DEFAULT_FONT_INDEX_MAX ){
    m_defaultFont[index] = NULL;
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ  ファイル非同期読み込みVer
 *
 *	@param	セットアップ情報
 */
//-----------------------------------------------------------------------------
LytSysBase::LytSysBase() : m_Initialized(false), m_InitializeSystem(false)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 *
 *	@param	セットアップ情報
 */
//-----------------------------------------------------------------------------
LytSysBase::LytSysBase( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator ) : m_Initialized(false), m_InitializeSystem(false)
{
  this->Initialize( setup, heapAllocator, deviceAllocator );
}


//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトシステムベース初期化　同期Ver
 *
 *	@param	setup
 *	@param	heapAllocator
 *	@param	deviceAllocator 
 */
//-----------------------------------------------------------------------------
void LytSysBase::Initialize( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  gfl::heap::HeapBase* heap = heapAllocator->GetHeapBase();
  
  // メモリアロケーター生成
  m_allocator = heapAllocator;
  m_devAllocator = deviceAllocator;

  // 旧式の使い方も担保
  if(!m_systemInitialized){
    InitializeSystem( m_allocator, m_devAllocator );
    m_InitializeSystem = true;
  }
  
  // マネージャの生成
  m_lytResMan   = GFL_NEW(heap) LytResManager( heapAllocator, setup->lyt_resource_max );
  m_wkMan       = GFL_NEW(heap) LytWkManager( heapAllocator, setup->lyt_wk_max );


  // グローバルデータ生成
  m_globalData = GFL_NEW(heapAllocator->GetHeapBase()) LytWkBaseGlobalData();
  m_globalData->p_globalResource  = &m_globalResource;
  m_globalData->p_drawer          = &m_drawer;

  // 下画面カメラ設定初期値設定
  {
    m_globalData->env[DISPLAY_LOWER].SetCameraParam( Env::CAMERA_NORMAL, Env::DefaultProjection[DISPLAY_LOWER], Env::DefaultView[DISPLAY_LOWER] );
  }



  // 初期化完了
  m_Initialized = true;
  
  // 生成されたレイアウトシステムの数を数える。
  ++m_lytSysCount;
}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトシステムベース　非同期読み込み開始
 *
 *	@param	setup
 *	@param	heapAllocator
 *	@param	deviceAllocator 
 */
//-----------------------------------------------------------------------------
void LytSysBase::InitializeAsync( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  // メモリアロケーター生成
  m_allocator = heapAllocator;
  m_devAllocator = deviceAllocator;

  // 旧式の使い方も担保
  if(!m_systemInitialized){
    InitializeSystem( m_allocator, m_devAllocator );
    m_InitializeSystem = true;
  }
  
  // マネージャの生成
  m_lytResMan   = GFL_NEW(heapAllocator->GetHeapBase()) LytResManager( heapAllocator, setup->lyt_resource_max );
  m_wkMan       = GFL_NEW(heapAllocator->GetHeapBase()) LytWkManager( heapAllocator, setup->lyt_wk_max );


  // グローバルデータ生成
  m_globalData = GFL_NEW(heapAllocator->GetHeapBase()) LytWkBaseGlobalData();
  
  m_globalData->p_globalResource  = &m_globalResource;
  m_globalData->p_drawer          = &m_drawer;

}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトシステムベース　非同期更新処理
 *
 *	@retval true  非同期初期化完了
 *	@retval false 途中
 */
//-----------------------------------------------------------------------------
bool LytSysBase::InitializeAsyncWait( void )
{
  // 初期化完了チェック
  if(m_Initialized){
    return true;
  }
  
  // 下画面カメラ設定初期値設定
  {
    m_globalData->env[DISPLAY_LOWER].SetCameraParam( Env::CAMERA_NORMAL, Env::DefaultProjection[DISPLAY_LOWER], Env::DefaultView[DISPLAY_LOWER] );
  }


  // 初期化完了
  m_Initialized = true;

  // 生成されたレイアウトシステムの数を数える。
  ++m_lytSysCount;

  return true;
}



//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
LytSysBase::~LytSysBase()
{

  // 先されたレイアウトシステムの数を数える。
  --m_lytSysCount;

  // グローバルデータ破棄
  GFL_DELETE m_globalData;
  
  // マネージャ破棄
  GFL_DELETE m_lytResMan;
  GFL_DELETE m_wkMan;     

  // このシステムがInitializeSystemをしたので、
  // FinalizeSystemを行う。
  if( m_InitializeSystem ){
    FinalizeSystem();
    m_InitializeSystem = false;
  }

}


//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトワークが共有して使用するワーク取得
 *
 *	@return 共有ワーク
 */
//-----------------------------------------------------------------------------
LytWkBaseGlobalData* LytSysBase::GetGlobalData(void)
{
  return m_globalData;
}

//----------------------------------------------------------------------------
/**
 *	@brief  リソースマネージャ取得
 *
 *	@return リソースマネージャ
 */
//-----------------------------------------------------------------------------
LytResManager* LytSysBase::GetResManager(void) const
{
  return m_lytResMan;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ワークマネージャ取得
 *
 *	@return ワークマネージャ
 */
//-----------------------------------------------------------------------------
LytWkManager* LytSysBase::GetWkManager(void) const
{
  return m_wkMan;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ヒープアロケータの取得
 *	@return ヒープアロケータ
 */
//-----------------------------------------------------------------------------
gfl::heap::NwAllocator* LytSysBase::GetHeapAllocator(void) const
{
  return m_allocator;
}

//----------------------------------------------------------------------------
/**
 *	@brief  デバイスアロケータ取得
 *
 *	@return デバイスアロケータ
 */
//-----------------------------------------------------------------------------
gfl::heap::NwAllocator* LytSysBase::GetDeviceAllocator(void) const
{
  return m_devAllocator;
}


//=============================================================================
/**
 *    LytWkManager実装
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 *
 *  @param  allocator   アロケータ
 *	@param	wk_max      ワーク最大数
 */
//-----------------------------------------------------------------------------
LytWkManager::LytWkManager( heap::NwAllocator* allocator, u32 wk_max ) : 
  m_linkTop()
{
  gfl::heap::HeapBase* heap = allocator->GetHeapBase();
  int i;
  
  m_array    = GFL_NEW(heap) LytWk[wk_max];
  m_arrayNum  = wk_max;
  m_createNum = 0;

  // 初期化
  for( i=0; i<m_arrayNum; i++ ){
    m_array[i].Initialize( allocator, this );
  }

  m_linkTop.Initialize(allocator, this);

  //  リンクリスト初期化
  m_linkTop.GetLytWkBase()->SetPrev( &m_linkTop );
  m_linkTop.GetLytWkBase()->SetNext( &m_linkTop );
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
LytWkManager::~LytWkManager()
{
  // LytWk配列破棄
  if( m_array ){
    //破棄処理
    GFL_DELETE_ARRAY m_array;
    m_array = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief クリアワークの取得
 *
 *  @retval クリアワーク  もうない場合NULL
 */
//-----------------------------------------------------------------------------
LytWk* LytWkManager::GetClearWk( void ) const
{
  for( int i=0; i<m_arrayNum; i++){
    if( m_array[i].IsInitialized() == false ){
      return &m_array[i];
    }
  }

  // クリアワークなし
  GFL_ASSERT(0);
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  オート描画される全ワークを更新する。
 */
//-----------------------------------------------------------------------------
void LytWkManager::UpdateAll( LytWKGroupID groupID )
{
  LytWk* p_now;
  
  // 全LytWkリストをLOOP
  p_now = m_linkTop.GetLytWkBase()->GetNext();
  while( p_now != &m_linkTop ){

    // オート描画されるワークならば、更新
    if( p_now->GetLytWkBase()->IsAutoDraw() ){
      if( CheckGroupID( groupID, p_now->GetLytWkBase()->GetWkGroupID() ) == true ){
        p_now->GetLytWkBase()->Update();
      }
    }

    // 次のワーク
    p_now = p_now->GetLytWkBase()->GetNext();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  オート描画設定されている全ワークを描画
 */
//-----------------------------------------------------------------------------
void LytWkManager::DrawAll( DisplayType drawtype, RenderTarget* renderTarget, LytWKGroupID groupID )
{
  LytWk* p_now;

  // drawtypeが不正なら描画しない
  if(drawtype >= DISPLAY_BOTH_MAX){
    GFL_ASSERT(drawtype < DISPLAY_BOTH_MAX); //@check
    return ;
  }

  // 全LytWkリストをLOOP
  p_now = m_linkTop.GetLytWkBase()->GetNext();
  while( p_now != &m_linkTop ){

    // オート描画されるワークならば、更新
    if( p_now->GetLytWkBase()->IsAutoDraw() ){
      
      if( CheckGroupID( groupID, p_now->GetLytWkBase()->GetWkGroupID() ) == true ){

        p_now->GetLytWkBase()->Draw( 
            sc_LytSysLodAnmProjIdx[drawtype].lod, sc_LytSysLodAnmProjIdx[drawtype].cameraType, *renderTarget );
        
      }
    }

    // 次のワーク
    p_now = p_now->GetLytWkBase()->GetNext();
  }
  //nngxFlush3DCommand();
}

//----------------------------------------------------------------------------
/**
 *	@brief  リンクリストにワークを登録
 *
 *	@param	wk  ワーク
 */
//-----------------------------------------------------------------------------
void LytWkManager::AddLink( LytWk* wk )
{
  // すでにリンクがはられてないかチェック
  if( wk->GetLytWkBase()->GetPrev() != NULL ){
    GFL_ASSERT( wk->GetLytWkBase()->GetPrev() == NULL ); //@check
    return ;
  }
  if( wk->GetLytWkBase()->GetNext() != NULL ){ 
    GFL_ASSERT( wk->GetLytWkBase()->GetNext() == NULL ); //@check
    return ;
  }

  u16 wk_priority = wk->GetPriority();
  u16 next_priority;
    
  // priority 0>0xffff　の順に登録
  // 先頭の情報かチェック
  if( m_linkTop.GetLytWkBase()->GetNext() == &m_linkTop )
  {
    // 先頭に登録
    m_linkTop.GetLytWkBase()->SetNext( wk );
    m_linkTop.GetLytWkBase()->SetPrev( wk );

    wk->GetLytWkBase()->SetNext( &m_linkTop );
    wk->GetLytWkBase()->SetPrev( &m_linkTop );
  }
  else
  {
    LytWk* next = m_linkTop.GetLytWkBase()->GetNext();
    LytWk* prev = &m_linkTop;

    // けつまでPriorityチェック
    while( next != &m_linkTop ){

      next_priority = next->GetPriority();
      if( wk_priority < next_priority ){
        // ここに設定する。
        break;
      }

      prev = next;
      next = next->GetLytWkBase()->GetNext();
    }

    wk->GetLytWkBase()->SetNext( next );
    wk->GetLytWkBase()->SetPrev( next->GetLytWkBase()->GetPrev() );
    prev->GetLytWkBase()->SetNext( wk );
    next->GetLytWkBase()->SetPrev( wk );
  }

  // 生成数カウント
  ++m_createNum;
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  リンクリストからワークを削除
 *
 *	@param	wk  ワーク
 */
//-----------------------------------------------------------------------------
void LytWkManager::DelLink( LytWk* wk )
{
  // まだ未生成の場合は何もおこなわずに終わる
  if( (wk->GetLytWkBase()->GetPrev() == NULL) && (wk->GetLytWkBase()->GetNext() == NULL) ){
    return;
  }

  // すでにlinkから外れているか、linkが壊れた。
  if( (wk->GetLytWkBase()->GetNext() == NULL) || (wk->GetLytWkBase()->GetPrev() == NULL) ){
    GFL_ASSERT(0); //@check
    return ;
  }

  // 生成数カウント
  --m_createNum;
  
  LytWk* prev = wk->GetLytWkBase()->GetPrev();
  LytWk* next = wk->GetLytWkBase()->GetNext();
  
  prev->GetLytWkBase()->SetNext( next );
  next->GetLytWkBase()->SetPrev( prev );

  wk->GetLytWkBase()->SetNext( NULL );
  wk->GetLytWkBase()->SetPrev( NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief  Priorityによるソートを行う。
 *
 *	@param  wk  ソートしなおしてほしいワーク
 */
//-----------------------------------------------------------------------------
void LytWkManager::SortPriority( LytWk* wk )
{
  DelLink(wk);
  AddLink(wk);
}

//----------------------------------------------------------------------------
/**
 *	@brief  グループIDの一致チェック
 *
 *	@param	groupID_a   グループID  A
 *	@param	groupID_b   グループID　B
 *
 *	@retval true    一致
 *	@retval false   不一致
 */
//-----------------------------------------------------------------------------
bool LytWkManager::CheckGroupID( u16 groupID_a, u16 groupID_b ) const
{
  if( (groupID_a == LYTSYS_LYTWK_GROUPID_ALL) ){
    return true;
  }
  if( (groupID_b == LYTSYS_LYTWK_GROUPID_ALL) ){
    return true;
  }
  if( groupID_a == groupID_b ){
    return true;
  }
  return false;
}


//=============================================================================
/**
 *          LytResManagerクラス 実装
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief    コンストラクタ
 */
//-----------------------------------------------------------------------------
LytResManager::LytResManager( heap::NwAllocator* allocator, u32 lyt_res_max )
{
  heap::HeapBase* heap = allocator->GetHeapBase();
  
  m_lytArray = GFL_NEW_ARRAY(heap) LytRes[lyt_res_max]();
  m_lytArrayNum = lyt_res_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief    デストラクタ
 */
//-----------------------------------------------------------------------------
LytResManager::~LytResManager()
{
  // 配列破棄
  GFL_DELETE_ARRAY m_lytArray;
}

//----------------------------------------------------------------------------
/**
 *	@brief  使用していないリソースワークを取得
 *
 *	@param	idx ワークのインデックス可能先（NULLも可）
 *
 *	@return 使用していないワークポインタ(ないときNULL)
 */
//-----------------------------------------------------------------------------
LytRes* LytResManager::GetClearLytRes( LytResID* idx  ) const
{
  // 空のワークを検索
  for( int i=0; i<m_lytArrayNum; i++ ){
    if( m_lytArray[i].IsDataIn() == false ){
      if( idx != NULL ){ *idx = i; }
      return &m_lytArray[i];
    }
  }

  // 空のワークなし
  GFL_ASSERT( 0 );

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  idxのリソースを取得
 *
 *	@param	idx   インデックス
 *
 *	@return リソース
 */
//-----------------------------------------------------------------------------
LytRes* LytResManager::GetIndexLytRes( LytResID idx ) const
{
  GFL_ASSERT( idx < m_lytArrayNum ); //@check
  if(idx >= m_lytArrayNum){ return NULL; }
  return &m_lytArray[idx];
}

//----------------------------------------------------------------------------
/**
 *	@brief  使用中のリソースの数を返す
 *
 *	@return
 */
//-----------------------------------------------------------------------------
u32 LytResManager::GetUseLytResNum( void ) const
{
  u32 count = 0;
  // 空のワークを検索
  for( int i=0; i<m_lytArrayNum; i++ ){
    if( m_lytArray[i].IsDataIn() ){
      ++count;
    }
  }

  return count;
}


} /* end of namespace g2d */
} /* end of namespace grp */
} /* end of namespace gfl */
