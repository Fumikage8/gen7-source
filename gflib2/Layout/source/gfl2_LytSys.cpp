//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl2_LytSys.cpp
 *  @brief  レイアウトシステム
 *  @author tomoya takahashi   >>  メニューセクション k.ohno
 *  @date   2010.10.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if defined(GF_PLATFORM_CTR)
#include <nw/lyt2.h>
#else
#include <nw/lyt.h>
#include <nw/dev.h>
#endif

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

//#include <grp/include/gfl2_GraphicsSystem.h>

#include <layout/include/gfl2_LytSys.h>
#include <layout/include/gfl2_LytWkBase.h>
#include <layout/include/gfl2_LytRes.h>
#include <layout/include/gfl2_DrawingLytWkManager.h>

//#include <gfl_Std.h>
#include <fs/include/gfl2_fs.h>
//#include <ro/gfl_RoManager.h>

#include <gfx/include/gfl2_GLMemory.h>

#include <layout/include/gfl2_LayoutRenderPath.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)

//-----------------------------------------------------------------------------
/**
 *          定数宣言
 */
//-----------------------------------------------------------------------------
typedef struct
{
  gfl2::lyt::DisplayType  lod;
  Env::CameraType cameraType;
} LytSysLodState;
  
static const LytSysLodState sc_LytSysLodAnmProjIdx[ gfl2::lyt::DISPLAY_BOTH_MAX ] = {
  { gfl2::lyt::DISPLAY_UPPER,    Env::CAMERA_NORMAL },
  { gfl2::lyt::DISPLAY_LOWER,    Env::CAMERA_NORMAL },
  { gfl2::lyt::DISPLAY_UPPER,    Env::CAMERA_RIGHT },
};


//-------------------------------------
///  上下画面に描画するためのデフォルト設定
//=====================================
const LytWkSetUp c_LytWkUpperDefaultSetUp = 
{
  gfl2::math::Vector3(0,LYTSYS_DEFAULT_CAMERA_UPPER_POS_Y,0), 
  gfl2::math::Vector2(1,1), 
  gfl2::math::Vector3(0,0,0), 
  0,
  0
};

const LytWkSetUp c_LytWkLowerDefaultSetUp = 
{
  gfl2::math::Vector3(0,LYTSYS_DEFAULT_CAMERA_LOWER_POS_Y,0), 
  gfl2::math::Vector2(1,1), 
  gfl2::math::Vector3(0,0,0), 
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
  LytResManager( gfl2::heap::NwAllocator* allocator,gfl2::heap::NwAllocator* dev_allocator, u32 lyt_res_max );
  ~LytResManager();

  LytRes * GetClearLytRes( u32 * parent_id ) const;
  LytRes * GetIndexLytRes( u32 parent_id ) const;

  u32 GetUseLytResNum( void ) const;

  LytMultiResID InitMultiResource( gfl2::heap::HeapBase * heap, u32 link_max );

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
  LytWkManager( gfl2::heap::NwAllocator* allocator,gfl2::heap::NwAllocator* dev_allocator, u32 wk_max, DrawingLytWkManager* drawingLytWkManager );
  ~LytWkManager();

  LytWk* GetClearWk( void ) const;
  void UpdateAll( LytWKGroupID groupID );

  void DrawAll( LytWKGroupID groupID );

  void AddLink( LytWk* wk );
  void DelLink( LytWk* wk );
  void SortPriority( LytWk* wk );

  inline u32 GetCreateWkNum( void ) const { return m_createNum; }

  inline DrawingLytWkManager* GetDrawingLytWkManager(void) const { return m_drawingLytWkManager; }
  
private:

  LytWk*  m_array;
  u16     m_arrayNum;
  u16     m_createNum;

  LytWk  m_linkTop;  // リンクリスト

  DrawingLytWkManager* m_drawingLytWkManager;

  bool CheckGroupID( u16 groupID_a, u16 groupID_b ) const;
  
};

//-----------------------------------------------------------------------------
/**
 *          LytSysBaseクラス宣言
 */
//-----------------------------------------------------------------------------
class LytSysBase
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(LytSysBase);


public:
  // システム初期化
  static void InitializeSystem( gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator,gfl2::lyt::LytShaderResource* pLSR );
  // システム破棄
  static void FinalizeSystem( void );

  
  // Defaultフォント設定
  static void SetDefaultFont( gfl2::str::Font* font, const char* name, u32 index );
  // Defaultフォントがあるかチェック
  static bool IsDefaultFont(u32 index);
  // Defaultフォントを取得
  static gfl2::str::Font* GetDefaultFont(u32 index);
  // Defaultフォント名取得
  static const char* GetDefaultFontName(u32 index);
  // Defaultフォント解除
  static void ResetDefaultFontInfo( u32 index );


  // コンストラクタデストラクタ
  LytSysBase( const LytSysSetUp* setup, gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator, DrawingLytWkManager* drawingLytWkManager );
  LytSysBase(void);
  ~LytSysBase();

  // 初期化の流れ
  void Initialize( const LytSysSetUp* setup, gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator, DrawingLytWkManager* drawingLytWkManager );
  void InitializeAsync( const LytSysSetUp* setup, gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator, DrawingLytWkManager* drawingLytWkManager );
  bool InitializeAsyncWait( void );
  inline bool IsInitialized( void ) const { return m_Initialized; }
  
  
  // 情報を取得するインターフェース
  LytWkBaseGlobalData* GetGlobalData(void);
  LytResManager* GetResManager(void) const;
  LytWkManager* GetWkManager(void) const;


  gfl2::heap::NwAllocator* GetHeapAllocator(void) const;
  gfl2::heap::NwAllocator* GetDeviceAllocator(void) const;
  

private:

  // 同時に何個読み込むか？
  static const s32 MULTI_LOAD_ROMFILE_NUM = 3;
  
  // フォント名用バッファサイズ
  static const u32 FONT_NAME_LENGTH = 64;
  
private:

  bool m_Initialized;
  bool m_InitializeSystem;

  gfl2::heap::NwAllocator*         m_allocator;    // メモリアロケータ
  gfl2::heap::NwAllocator*         m_devAllocator;// デバイスメモリアロケータ

  static gfl2::lyt::LytShaderResource* mpRectDrawerShaderRes;
  
  LytWkBaseGlobalData*   m_globalData;      // 全LytWk共有データ
  LytResManager*         m_lytResMan;       // レイアウトリソースマネージャ
  LytWkManager*          m_wkMan;           // ワークマネージャ

  static gfl2::str::Font* m_defaultFont[LYTSYS_DEFAULT_FONT_INDEX_MAX];     // Defaultフォント
  // Defaultフォント名
  // dll内からの文字列リテラル指定に対応するため、ポインタから配列へ変更。
  static char            m_defaultFontName[LYTSYS_DEFAULT_FONT_INDEX_MAX][FONT_NAME_LENGTH];

  // グローバルリソース　ドロワー
  static bool                      m_systemInitialized;
//  static nw::lyt::GraphicsResource m_globalResource;
#if defined(GF_PLATFORM_CTR)  // cafeではdrawerが存在しない
  static nw::lyt::Drawer           m_drawer;
#endif
  static s32                       m_lytSysCount;
};

// ユーザーアロケーター設定
#if defined(GF_PLATFORM_CTR)
#define LYTSYSBASE_CHANGE_USE_ALLOCATOR(wk)  \
  nw::os::IAllocator* p_default_allocator_heap = nw::lyt::Layout::GetAllocator();\
  nw::os::IAllocator* p_default_allocator_device = nw::lyt::Layout::GetDeviceMemoryAllocator();\
  nw::lyt::Layout::SetAllocator( (wk)->GetHeapAllocator() );\
  nw::lyt::Layout::SetDeviceMemoryAllocator( (wk)->GetDeviceAllocator() );
#elif defined(GF_PLATFORM_WIN32)
#define LYTSYSBASE_CHANGE_USE_ALLOCATOR(wk)  \
  nw::ut::IAllocator* p_default_allocator_heap = nw::lyt::Layout::GetAllocator();\
  nw::lyt::Layout::SetAllocator( (wk)->GetHeapAllocator() );
#endif

// デフォルトアロケーターに戻す
#if defined(GF_PLATFORM_CTR)
#define LYTSYSBASE_RETURN_ALLOCATOR(wk)  \
  nw::lyt::Layout::SetAllocator( p_default_allocator_heap );\
  nw::lyt::Layout::SetDeviceMemoryAllocator( p_default_allocator_device );
#elif defined(GF_PLATFORM_WIN32)
#define LYTSYSBASE_RETURN_ALLOCATOR(wk)  \
  nw::lyt::Layout::SetAllocator( p_default_allocator_heap );
#endif



//-----------------------------------------------------------------------------
/**
 *    Defaultフォント実態
 */
//-----------------------------------------------------------------------------
gfl2::str::Font* LytSysBase::m_defaultFont[LYTSYS_DEFAULT_FONT_INDEX_MAX]      = {NULL};
char            LytSysBase::m_defaultFontName[LYTSYS_DEFAULT_FONT_INDEX_MAX][LytSysBase::FONT_NAME_LENGTH];// Defaultフォント名
bool            LytSysBase::m_systemInitialized = false;
//nw::lyt::GraphicsResource LytSysBase::m_globalResource;   ///<グローバルリソース
gfl2::lyt::LytShaderResource* LytSysBase::mpRectDrawerShaderRes = NULL;
#if defined(GF_PLATFORM_CTR)  // cafeではdrawerが存在しない
nw::lyt::Drawer           LytSysBase::m_drawer;           ///<ドロワー
#endif
s32             LytSysBase::m_lytSysCount = 0;            ///<生成したレイアウトシステムの数
  
  
//=============================================================================
/**
 *          レイアウトシステムクラス　実装
 */
//=============================================================================

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
void LytSys::InitializeSystem( gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator,gfl2::lyt::LytShaderResource* pLSR )
{
  LytSysBase::InitializeSystem( heapAllocator, deviceAllocator, pLSR );
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトシステムの破棄
 */
//-----------------------------------------------------------------------------
void LytSys::FinalizeSystem( void )
{
  LytSysBase::FinalizeSystem();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デフォルトフォント設定
 *
 * @param   font    フォントデータ
 * @param   name    レイアウトがアクセスするための名称 ( 例　simple.bcfnt )
 * @param   index   インデックス
 */
//-----------------------------------------------------------------------------
void LytSys::SetDefaultFont( gfl2::str::Font* font, const char* name, u32 index )
{
  LytSysBase::SetDefaultFont( font, name, index );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デフォルトフォント解除
 *
 * @param   index   インデックス
 */
//-----------------------------------------------------------------------------
void LytSys::ResetDefaultFontInfo( u32 index )
{
  LytSysBase::ResetDefaultFontInfo( index );
}


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
LytSys::LytSys( const LytSysSetUp* setup, gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator, DrawingLytWkManager* drawingLytWkManager )
{
  gfl2::heap::HeapBase* heap = heapAllocator->GetHeapBase();
  
  // LytSysBase生成
  m_base = GFL_NEW(heap) LytSysBase( setup, heapAllocator, deviceAllocator, drawingLytWkManager );
}

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ ( 非同期初期化モード )
 *
 * @param   p_heap  ヒープ
 */
//-----------------------------------------------------------------------------
LytSys::LytSys( gfl2::heap::HeapBase* p_heap )
{
  // LytSysBase生成
  m_base = GFL_NEW(p_heap) LytSysBase();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LytSys::~LytSys()
{
  GFL_DELETE m_base;
}


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
void LytSys::InitializeAsync( const LytSysSetUp* setup, gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator, DrawingLytWkManager* drawingLytWkManager )
{
  m_base->InitializeAsync( setup, heapAllocator, deviceAllocator, drawingLytWkManager );
}

//-----------------------------------------------------------------------------
/**
 * @brief   非同期初期化完了まち
 *
 * @retval  true  初期化完了
 * @retval  false 初期化中
 */
//-----------------------------------------------------------------------------
bool LytSys::InitializeAsyncWait( void )
{
  return m_base->InitializeAsyncWait();
}

//-----------------------------------------------------------------------------
/**
 * @brief   初期化が完了しているかチェック
 *
 * @retval  true  初期化完了
 * @retval  false 初期化中
 */
//-----------------------------------------------------------------------------
bool LytSys::IsInitialized( void ) const
{
  return m_base->IsInitialized();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  オート描画フラグがONになっている全Wkのデータ更新処理
 *
 *  @param  lytwkGroupID    Updateを行うレイアウトワークグループID
 */
//-----------------------------------------------------------------------------
void LytSys::UpdateAll( LytWKGroupID lytwkGroupID )
{
  GFL_ASSERT( m_base->IsInitialized() );
  
  m_base->GetWkManager()->UpdateAll( lytwkGroupID );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  オート描画フラグがONになっている全Wkの描画処理
 *
 *  @param    graphics          グラフィックスシステム
 *  @param    drawtype          表示面定義
 *  @param    renderTarget      レンダリングターゲットのフレームバッファ
 *  @param    rightRenderTarget 右目用レンダリングターゲット　（DISPLAY_UPPERのときのみ参照）
 *  @param    lytwkGroupID      描画を行うLYTWKグループID(ペイングループIDとは違うので注意！）
 */
//-----------------------------------------------------------------------------
void LytSys::DrawAll( LytWKGroupID lytwkGroupID )
{
  GFL_ASSERT( m_base->IsInitialized() );

//  GraphicsSystem::inst().BeginScene();

  m_base->GetWkManager()->DrawAll( lytwkGroupID );

//  GraphicsSystem::inst().EndScene();
}

/*コマンドキャッシュ使用例
void LytSys::DrawAll( DisplayType drawtype, FrameBuffer* frameBuffer, FrameBuffer* rightFrameBuffer )
{
  gfl::lyt::GraphicsSystem* p_grpSys = GFL_SINGLETON_INSTANCE(GraphicsSystem);
  
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

//-----------------------------------------------------------------------------
/**
 *  @brief  UpperLowerに対応した描画環境データ
 *
 *  @param  描画面　UPPER　LOWER
 *
 *  @return 描画環境データ
 */
//-----------------------------------------------------------------------------
Env* LytSys::GetGlobalEnv( gfl2::lyt::DisplayType drawtype )
{
  LytWkBaseGlobalData* globalData = m_base->GetGlobalData();
  if( drawtype < gfl2::lyt::DISPLAY_NORMAL_MAX ){
    return &globalData->env[drawtype];
  }
  GFL_ASSERT(0);
  return &globalData->env[0];
}


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
LytMultiResID LytSys::LoadLytRes( const LytMultiResID * resID, gfl2::heap::NwAllocator* devAllocator, const void * cp_buff, bool useDefaultFont, u32 addTextureNum )
{
  LytResManager * lytResMan = m_base->GetResManager();
  LytRes * res;
  LytMultiResID make_res;

  if( resID == NULL )
  {
    u32 id;
    res = lytResMan->GetClearLytRes( &id );
    make_res.SetParentID( id );
  }
  else
  {
    res = lytResMan->GetIndexLytRes( resID->GetParentID() );
    make_res = *resID;
  }

  GFL_ASSERT_STOP( res );

  // メモリの確保先を設定
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

  // 読み込み処理
  {
    u32 id = res->Load( devAllocator, cp_buff, addTextureNum );
    make_res.SetMultiID( id );
  }

  // Defaultフォントの設定
  if( useDefaultFont )
  {
    for( u32 i=0; i<LYTSYS_DEFAULT_FONT_INDEX_MAX; ++i )
    {
      if( LytSysBase::IsDefaultFont(i) )
      {
        res->SetDefaultFont( LytSysBase::GetDefaultFont(i), LytSysBase::GetDefaultFontName(i), i );
      }
    }
  }

  // メモリの確保先をもとに戻す
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
  
  return make_res;
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトリソースの読み込み ( ファイル指定版 )
 *
 * @param   pAsyncFileMan   ファイルマネージャ
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
LytMultiResID LytSys::LoadLytRes( gfl2::fs::AsyncFileManager* pAsyncFileMan, const LytMultiResID * resID, gfl2::heap::NwAllocator* devAllocator, u32 arcID, u32 dataID, bool comp, bool useDefaultFont, u32 addTextureNum )
{
  LytResManager * lytResMan  = m_base->GetResManager();
  LytRes * res;
  LytMultiResID make_res;

  if( resID == NULL )
  {
    u32 id;
    res = lytResMan->GetClearLytRes( &id );
    make_res.SetParentID( id );
  }
  else
  {
    res = lytResMan->GetIndexLytRes( resID->GetParentID() );
    make_res = *resID;
  }
  GFL_ASSERT_STOP( res );

  // メモリの確保先を設定
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

  // 読み込み処理
  {
    u32 id = res->Load( pAsyncFileMan, devAllocator, arcID, dataID, comp, addTextureNum );
    make_res.SetMultiID( id );
  }

  // Defaultフォントの設定
  if( useDefaultFont )
  {
    for( u32 i=0; i<LYTSYS_DEFAULT_FONT_INDEX_MAX; ++i )
    {
      if( LytSysBase::IsDefaultFont(i) )
      {
        res->SetDefaultFont( LytSysBase::GetDefaultFont(i), LytSysBase::GetDefaultFontName(i), i );
      }
    }
  }

  // メモリの確保先をもとに戻す
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);

  return make_res;
}

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
LytMultiResID LytSys::InitMultiResource( gfl2::heap::HeapBase * heap, u32 link_max )
{
  LytResManager * lytResMan = m_base->GetResManager();
  return lytResMan->InitMultiResource( heap, link_max );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  レイアウトリソースの破棄
 *
 *  @param  resID     破棄するリソースID
 */
//-----------------------------------------------------------------------------
void LytSys::ReleaseLytRes( const LytMultiResID * resID )
{
  LytResManager * lytResMan = m_base->GetResManager();
  LytRes * res = lytResMan->GetIndexLytRes( resID->GetParentID() );
  if( res )
  {
    res->ReleaseMultiResource();
  }
}

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
void LytSys::RegistTextureRes( const LytMultiResID * srcID, const LytMultiResID * dstID )
{
  LytResManager * lytResMan = m_base->GetResManager();

  u32 src_parent = srcID->GetParentID();
  u32 dst_parent = dstID->GetParentID();

  if( src_parent == dst_parent )
  {
    GFL_ASSERT_MSG( 0, "親IDが同じなので必要ありません。\n" );
    return;
  }

  const LytRes * srcRes = lytResMan->GetIndexLytRes( src_parent );
  LytRes * dstRes       = lytResMan->GetIndexLytRes( dst_parent );

  if( dstRes && srcRes ){
    // メモリの確保先を設定
    LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);
    // テクスチャResist
    dstRes->RegistTexture( dstID->GetMultiID(), srcRes, srcID->GetMultiID() );
    // メモリの確保先をもとに戻す
    LYTSYSBASE_RETURN_ALLOCATOR(m_base);
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief  テクスチャをすべてメモリに配置
 *  
 *  @param  resID   リソースID
 */
//-----------------------------------------------------------------------------
void LytSys::TransferAllTexture( const LytMultiResID * resID )
{
  LytResManager * lytResMan = m_base->GetResManager();
  LytRes * res = lytResMan->GetIndexLytRes( resID->GetParentID() );

  if( res ){
    // メモリの確保先を設定
    LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);
    // 配置
    res->TransferAllTexture();
    // メモリの確保先をもとに戻す
    LYTSYSBASE_RETURN_ALLOCATOR(m_base);
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief  テクスチャをメモリに配置
 *  
 *  @param  resID         リソースID
 *  @param  texArcIndex   アーカイブ内のテクスチャインデックス
 */
//-----------------------------------------------------------------------------
void LytSys::TransferIndexTexture( const LytMultiResID * resID, LytArcIndex texArcIndex )
{
  LytResManager * lytResMan = m_base->GetResManager();
  LytRes * res = lytResMan->GetIndexLytRes( resID->GetParentID() );
  if( res ){
    // メモリの確保先を設定
    LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);
    // 配置
    res->TransferIndexTexture( resID->GetMultiID(), texArcIndex );
    // メモリの確保先をもとに戻す
    LYTSYSBASE_RETURN_ALLOCATOR(m_base);
  }

}

//-----------------------------------------------------------------------------
/**
 * @brief   リソースを取得
 *  
 * @param   resID   リソースID
 *
 * @return  リソース
 */
//-----------------------------------------------------------------------------
LytRes * LytSys::GetResource( const LytMultiResID * resID ) const
{
  LytResManager * lytResMan  = m_base->GetResManager();
  return lytResMan->GetIndexLytRes( resID->GetParentID() );
}

//-----------------------------------------------------------------------------
/**
 * @brief   使用中のリソースの総数を取得
 *
 * @return  使用中のリソースの総数
 */
//-----------------------------------------------------------------------------
u32 LytSys::GetUseResourceNum( void ) const
{
  LytResManager* lytResMan = m_base->GetResManager();
  return lytResMan->GetUseLytResNum();
}


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
LytWk * LytSys::CreateLytWk(
  const LytMultiResID * resID,
  LytArcIndex lytDataID,
  const LytArcIndex * anmDataTbl,
  u32 anmTblNum,
  u32 anmMax,
  const LytWkSetUp * setup,
  b32 is_enable,
  b32 createPaneTbl,
  b32 createPaneTblWithHash )
{
  return CreateLytWk( m_base->GetHeapAllocator(), m_base->GetDeviceAllocator(), resID, lytDataID, anmDataTbl, anmTblNum, anmMax, setup, is_enable, createPaneTbl, createPaneTblWithHash );
}

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
LytWk * LytSys::CreateLytWk(
  gfl2::heap::NwAllocator * allocator,
  gfl2::heap::NwAllocator * dev_allocator,
  const LytMultiResID * resID,
  LytArcIndex lytDataID,
  const LytArcIndex * anmDataTbl,
  u32 anmTblNum,
  u32 anmMax,
  const LytWkSetUp * setup,
  b32 is_enable,
  b32 createPaneTbl,
  b32 createPaneTblWithHash )
{
  // レイアウト生成
  LytWk * wk = BuildLytWk( allocator, dev_allocator, resID, lytDataID, anmMax, setup, createPaneTbl, createPaneTblWithHash );
  if( wk == NULL )
  {
    return NULL;
  }

  // アニメーションの設定
  CreateAnimation( wk, resID, anmDataTbl, anmTblNum, 0, is_enable, createPaneTblWithHash );
  // リストに追加
  AddLinkWithLayoutWork( wk );

  return wk;
}

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
LytWk * LytSys::BuildLytWk( gfl2::heap::NwAllocator* allocator, gfl2::heap::NwAllocator* dev_allocator, const LytMultiResID * resID, LytArcIndex lytDataID, u32 anmMax, const LytWkSetUp* setup, b32 createPaneTbl, b32 createPaneTblWithHash )
{
  LytWk * wk = m_base->GetWkManager()->GetClearWk();
  LytRes * lytRes = m_base->GetResManager()->GetIndexLytRes( resID->GetParentID() );
  GFL_ASSERT_STOP(lytRes);

  if( wk != NULL )
  {
    LytWkBase * base = wk->GetLytWkBase();

    base->Initialize( allocator, dev_allocator, lytRes, resID->GetMultiID(), lytDataID, anmMax, m_base->GetGlobalData(), createPaneTbl, createPaneTblWithHash );

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
void LytSys::CreateAnimation( LytWk * lytWk, const LytMultiResID * resID, const LytArcIndex * anmTbl, u32 anmTblNum, u32 anmOffset, b32 is_enable, b32 createPaneTblWithHash )
{
  LytRes * lytRes = m_base->GetResManager()->GetIndexLytRes( resID->GetParentID() );
  LytWkBase * base = lytWk->GetLytWkBase();
  GFL_ASSERT_STOP( lytRes );
  GFL_ASSERT_STOP( base );

  for( u32 i=anmOffset; i<(anmTblNum+anmOffset); i++ )
  {
    base->CreateAnime( i, lytRes, resID->GetMultiID(), anmTbl[i], GFL_BOOL_CAST(is_enable), GFL_BOOL_CAST(createPaneTblWithHash) );
  }
}

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
void LytSys::CreatePartsLayoutAnime(
  LytWk * lytwk,
  const LytMultiResID * resID,
  u32 startAnmNum,
  const LytArcIndex * anmTbl,
  u32 anmTblMax,
  LytParts ** partsTbl,
  u32 partsTblMax,
  b32 is_enable,
  b32 createPaneTblWithHash )
{
  for( u32 i=0; i<partsTblMax; i++ )
  {
    startAnmNum = CreatePartsLayoutAnime( lytwk, resID, startAnmNum, anmTbl, anmTblMax, partsTbl[i], is_enable, createPaneTblWithHash );
  }
}

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
u32 LytSys::CreatePartsLayoutAnime(
  LytWk * lytwk,
  const LytMultiResID * resID,
  u32 startAnmNum,
  const LytArcIndex * anmTbl,
  u32 anmTblMax,
  LytParts * parts,
  b32 is_enable,
  b32 createPaneTblWithHash )
{
  LytRes * lytRes = m_base->GetResManager()->GetIndexLytRes( resID->GetParentID() );
  LytWkBase * base = lytwk->GetLytWkBase();
  GFL_ASSERT_STOP( lytRes );
  GFL_ASSERT_STOP( base );

  u32 offs = startAnmNum;

  for( u32 i=0; i<anmTblMax; i++ )
  {
    base->CreatePartsAnime( parts, offs, lytRes, resID->GetMultiID(), anmTbl[i], GFL_BOOL_CAST(is_enable), GFL_BOOL_CAST(createPaneTblWithHash) );
    offs++;
  }
  return offs;
}

//-----------------------------------------------------------------------------
/**
 * @brief レイアウトワークをレイアウトマネージャにリンクさせる
 *
 * @param lytWk 追加対象となるレイアウトワーク
 */
//-----------------------------------------------------------------------------
void LytSys::AddLinkWithLayoutWork( LytWk* lytWk )
{
  m_base->GetWkManager()->AddLink( lytWk );
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク破棄
 *
 * @param   lytWk   レイアウトワーク
 */
//-----------------------------------------------------------------------------
void LytSys::RemoveLytWk( LytWk* lytWk )
{
  LytWkManager* lytWkMan = m_base->GetWkManager();
  LytWkBase* base = lytWk->GetLytWkBase();
  
  // リストから削除
  lytWkMan->DelLink( lytWk );

  // アニメーションの破棄
  {
    u32 i;
    u32 anime_max = base->GetAnimeMax();
    for( i=0; i<anime_max; i++ ){
      base->DeleteAnime( i );
    }
  }

  // レイアウトワーク破棄
  base->Finalize();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  生成されたレイアウトワーク数の取得
 */
//-----------------------------------------------------------------------------
u32 LytSys::GetLytWkNum( void ) const
{
  LytWkManager* lytWkMan = m_base->GetWkManager();
  return lytWkMan->GetCreateWkNum();
}


//=============================================================================
/**
 *          レイアウトシステム ワーククラス　実装
*/
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief    使用可能状態取得
 *
 *  @retval true    使用可能
 *  @retval false   使用不可能
 */
//-----------------------------------------------------------------------------
bool LytWk::IsInitialized(void)
{
  return m_base->IsInitialized();
}

//-----------------------------------------------------------------------------
/**
 *  @brief    個別Update(AutoDrawFlagがfalseのときのみ有効)
 */
//-----------------------------------------------------------------------------
void LytWk::Update( void )
{
  if( m_base->IsAutoDraw() == false ){
    m_base->Update();
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief    個別描画(AutoDrawFlagがfalseのときのみ有効)
 *
 *  @param    drawtype    LYTSYS_DRAW_～
 *  @param    renderTarget レンダリングターゲットのフレームバッファ
 *  @param    rightRenderTarget  右目用レンダリングターゲット　（DISPLAY_UPPERのときのみ参照）
 */
//-----------------------------------------------------------------------------
void LytWk::Draw(gfl2::lyt::DisplayType lod, Env::CameraType cameraType)
{
#if 0
  if( !m_base->IsAutoDraw() )
  {
    glDisable( GL_DEPTH_TEST );
    glDepthMask( GL_FALSE );
    glDisable( GL_CULL_FACE );

    m_base->Draw();
  }
#endif
  //if( !m_base->IsAutoDraw() )
  {
    bool isDrawing = m_base->Draw( lod, cameraType );
    if(isDrawing)
    {
      DrawingLytWkManager* drawingLytWkManager = m_parent->GetDrawingLytWkManager();
      if(drawingLytWkManager)
      {
        drawingLytWkManager->AddLytWk(this);
      }
    }
  }
}

//-----------------------------------------------------------------------------
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
void LytWk::SetUserEnv( const Env* upperEnv, const Env* lowerEnv )
{
  m_base->SetUserEnv( upperEnv, lowerEnv );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  グローバルENVを使用するように設定
 */
//-----------------------------------------------------------------------------
void LytWk::SetGlobalEnv( void )
{
  m_base->SetGlobalEnv();
}


//-----------------------------------------------------------------------------
/**
 *  @brief  LytWkGroupIDを設定する
 *
 *  @param  groupID グループID
 */
//-----------------------------------------------------------------------------
void LytWk::SetLytWkGroupID( LytWKGroupID groupID )
{
  GFL_ASSERT( groupID != LYTSYS_LYTWK_GROUPID_ALL );
  m_base->SetWkGroupID( groupID );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  LytWkGroupIDを取得する
 *
 *  @return グループID
 */
//-----------------------------------------------------------------------------
LytWKGroupID LytWk::GetLytWkGroupID( void ) const
{
  return m_base->GetWkGroupID();
}


//-----------------------------------------------------------------------------
/**
 *  @brief  描画するディスプレイタイプを設定
 *
 *  @param    lod     描画するディスプレイタイプ
 */
//-----------------------------------------------------------------------------
void LytWk::SetDrawDispType( gfl2::lyt::DisplayType lod )
{
  m_base->SetDrawDispType( lod );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  描画するディスプレイタイプの取得
 *
 *  @return 描画するディスプレイタイプ
 */
//-----------------------------------------------------------------------------
gfl2::lyt::DisplayType LytWk::GetDrawDispType( void ) const
{
  return m_base->GetDrawDispType();
}


//=============================================================================
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
LytPane* LytWk::GetPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetPane( paneIndex );
}

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
LytPicture* LytWk::GetPicturePane( LytPaneIndex paneIndex ) const
{
  return m_base->GetPicturePane( paneIndex );
}

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
LytTextBox* LytWk::GetTextBoxPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetTextBoxPane( paneIndex );
}

//-----------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインの文字列バッファを確保
 *
 * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
 * @param   minLen        文字数
 */
//-----------------------------------------------------------------------------
void LytWk::AllocTextBoxPaneStringBuffer( LytPaneIndex paneIndex, u16 minLen )
{
  m_base->AllocTextBoxPaneStringBuffer( paneIndex, minLen );
}

void LytWk::AllocTextBoxPaneStringBuffer( gfl2::lyt::LytTextBox* pLytTextBox, u16 minLen )
{
  m_base->AllocTextBoxPaneStringBuffer( pLytTextBox, minLen );
}

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
LytWindow* LytWk::GetWindowPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetWindowPane( paneIndex );
}

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
LytBounding* LytWk::GetBoundingPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetBoundingPane( paneIndex );
}

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
LytParts* LytWk::GetPartsPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetPartsPane( paneIndex );
}

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
LytMaterial* LytWk::GetMaterial( LytPaneIndex paneIndex ) const
{
  return m_base->GetMaterial( paneIndex );
}

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
LytPane * LytWk::GetPane( nw::lyt::Parts * parts, u32 paneIndex, const LytMultiResID * resID ) const
{
  return m_base->GetPane( parts, paneIndex, resID->GetMultiID() );
}

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
LytPicture * LytWk::GetPicturePane( nw::lyt::Parts * parts, u32 paneIndex, const LytMultiResID * resID ) const
{
  return m_base->GetPicturePane( parts, paneIndex, resID->GetMultiID() );
}

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
LytTextBox * LytWk::GetTextBoxPane( nw::lyt::Parts * parts, u32 paneIndex, const LytMultiResID * resID ) const
{
  return m_base->GetTextBoxPane( parts, paneIndex, resID->GetMultiID() );
}

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
void LytWk::AllocTextBoxPaneStringBuffer( nw::lyt::Parts * parts, u32 paneIndex, const LytMultiResID * resID, u32 minLen ) const
{
  m_base->AllocTextBoxPaneStringBuffer( parts, paneIndex, resID->GetMultiID(), minLen );
}

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
LytWindow * LytWk::GetWindowPane( nw::lyt::Parts * parts, u32 paneIndex, const LytMultiResID * resID ) const
{
  return m_base->GetWindowPane( parts, paneIndex, resID->GetMultiID() );
}

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
LytBounding * LytWk::GetBoundingPane( nw::lyt::Parts * parts, u32 paneIndex, const LytMultiResID * resID ) const
{
  return m_base->GetBoundingPane( parts, paneIndex, resID->GetMultiID() );
}

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
LytParts * LytWk::GetPartsPane( LytParts * parts, LytPaneIndex paneIndex, const LytMultiResID * resID ) const
{
  return m_base->GetPartsPane( parts, paneIndex, resID->GetMultiID() );
}

//-----------------------------------------------------------------------------
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
LytMaterial * LytWk::GetMaterial( LytParts * parts, LytPaneIndex paneIndex, const LytMultiResID * resID ) const
{
  return m_base->GetMaterial( parts, paneIndex, resID->GetMultiID() );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  PaneIndexのグループのペインリストを取得
 *
 *  @param  groupIndex  グループインデックス
 *
 *  @return ペインリスト
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

//-----------------------------------------------------------------------------
/**
 *  @brief  ヒットチェックが可能か取得
 *
 *  @retval true  可能
 *  @retval false 不可能    一度描画する必要があります。
 */
//-----------------------------------------------------------------------------
bool LytWk::IsHitCheckOK(void) const
{
  return m_base->IsHitCheckOK();
}

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
LytPane* LytWk::GetHitPaneAll( s32 x, s32 y, bool boundingOnly ) const
{
  // ヒットチェックが可能かチェック
  if( m_base->IsHitCheckOK() == false ){
    GFL_ASSERT(0);
    return NULL;
  }
  
  LytPane* rootPane = m_base->GetCore()->GetRootPane();
  nw::math::VEC2 pos = nw::math::VEC2(static_cast<float>(x-160), static_cast<float>(-y+120));
  
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
LytPaneIndex LytWk::GetHitPaneIndexAll( s32 x, s32 y, bool boundingOnly ) const
{
  const LytPane* pane = GetHitPaneAll(x, y, boundingOnly);
  const LytRes* res = m_base->GetLytRes();
  u32 multi_id = m_base->GetLytResMultiID();
  LytPaneIndex ret = LYTSYS_PANEINDEX_ERROR;
  
  if( pane ){
    ret = res->GetPaneNameIndex( pane->GetName(), multi_id );
    // pene名から、インデックスを取得
    if( ret == LYTRES_PANENAME_ERROR ){
      return LYTSYS_PANEINDEX_ERROR;
    }
  } 
  return ret;
}

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
LytPane* LytWk::GetHitPaneGroup( LytPaneIndex groupIndex, s32 x, s32 y, bool boundingOnly ) const
{
  // ヒットチェックが可能かチェック
  if( m_base->IsHitCheckOK() == false ){
    GFL_ASSERT(0);
    return NULL;
  }

  nw::math::VEC2 pos = nw::math::VEC2(static_cast<float>(x-160), static_cast<float>(-y+120));
  nw::lyt::Layout* layout = m_base->GetCore();
  nw::lyt::GroupContainer* groupContainer = layout->GetGroupContainer();
  const LytRes* res = m_base->GetLytRes();
  u32 multi_id = m_base->GetLytResMultiID();

  const char* name = res->GetPaneName( groupIndex, multi_id );
  if( name == NULL ){
    return NULL;
  }
  
  nw::lyt::Group* group = groupContainer->FindGroupByName( name );
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
LytPaneIndex LytWk::GetHitPaneIndexGroup( LytPaneIndex groupIndex, s32 x, s32 y, bool boundingOnly ) const
{
  const LytPane* pane = GetHitPaneGroup(groupIndex, x, y, boundingOnly);
  const LytRes* res = m_base->GetLytRes();
  u32 multi_id = m_base->GetLytResMultiID();
  LytPaneIndex ret = LYTSYS_PANEINDEX_ERROR;
  
  if( pane ){
    ret = res->GetPaneNameIndex( pane->GetName(), multi_id );
    // pene名から、インデックスを取得
    if( ret == LYTRES_PANENAME_ERROR ){
      return LYTSYS_PANEINDEX_ERROR;
    }
  }
  return ret;
}

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
bool LytWk::GetHitPane( s32 x, s32 y, LytPaneIndex index ) const
{
  return GetHitPane( x, y, m_base->GetPane(index) );
}

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
bool LytWk::GetHitPane( s32 x, s32 y, LytPane * pane ) const
{
  nw::math::VEC2 pos = nw::math::VEC2(static_cast<float>(x-160), static_cast<float>(-y+120));

  if( nw::lyt::IsContain( pane, pos ) ){
    return true;
  }

  return false;
}

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
void LytWk::SetMaterialImage( LytPaneIndex materialIndex, u32 mapIndex, const LytRes* cp_res, u32 res_multi_id, LytArcIndex texArcIndex )
{
  // マテリアル取得
  LytMaterial* p_material = this->GetMaterial( materialIndex );

  if( p_material )
  {
    // テクスチャインデックス制限オーバー
    if( mapIndex < p_material->GetTexMapNum() )
    {
      // 張替え
      p_material->SetTexMap( mapIndex, cp_res->GetTexture(texArcIndex,res_multi_id) );
    }
    else
    {
      GFL_ASSERT(0);
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  リソース情報を取得
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
 *  @brief  オート描画フラグ　設定　
 *
 *  @param  flag  フラグ　true:ON　false:OFF
 */
//-----------------------------------------------------------------------------
void LytWk::SetAutoDrawFlag( bool flag )
{
  m_base->SetAutoDrawFlag(flag);
}

//----------------------------------------------------------------------------
/**
 *  @brief    オート描画のON/OFF 取得
 *  
 *  @retval true    ON
 *  @retval false   OFF
 */
//-----------------------------------------------------------------------------
bool LytWk::IsAutoDraw( void ) const
{
  return m_base->IsAutoDraw();
}

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
void LytWk::SetPriority( u16 pri )
{
  m_base->SetPriority( pri );
  m_parent->SortPriority( this );
}

//----------------------------------------------------------------------------
/**
 *  @brief  処理優先順位の取得
 *
 *  @retval  優先順位（ 0 > 0xffff )
 *
 *  *ただし、デプステストなしで、描画を行う場合には、
 *　後から描画されたワークが手前に描画されるため注意。
 *
 *  UpdateAllとDrawAllを行う際の優先順位です。
 */
//-----------------------------------------------------------------------------
u16 LytWk::GetPriority( void ) const
{
  return m_base->GetPriority();
}

//----------------------------------------------------------------------------
/**
 *  @brief  座標の設定
 *
 *  @param  pos
 */
//-----------------------------------------------------------------------------
void LytWk::SetTranslate( const gfl2::math::Vector3& pos )
{
  m_base->SetTranslate( pos );
}

//----------------------------------------------------------------------------
/**
 *  @brief  座標の取得
 *
 *  @param  p_pos   座標格納先
 */
//-----------------------------------------------------------------------------
void LytWk::GetTranslate( gfl2::math::Vector3* p_pos ) const
{
  m_base->GetTranslate(p_pos);
}


//----------------------------------------------------------------------------
/**
 *  @brief  拡大の設定
 *
 *  @param  scale   スケーリング値
 */
//-----------------------------------------------------------------------------
void LytWk::SetScale( const gfl2::math::Vector2& scale )
{
  m_base->SetScale( scale );
}

//----------------------------------------------------------------------------
/**
 *  @brief  拡大の取得
 *
 *  @retval scale スケーリング値
 */
//-----------------------------------------------------------------------------
void LytWk::GetScale( gfl2::math::Vector2* p_scale ) const
{
  m_base->GetScale( p_scale );
}


//----------------------------------------------------------------------------
/**
 *  @brief  回転の設定
 *
 *  @param  rotate   各軸の回転値
 */
//-----------------------------------------------------------------------------
void LytWk::SetRotate( const gfl2::math::Vector3& rotate )
{
  m_base->SetRotate( rotate );
}

//----------------------------------------------------------------------------
/**
 *  @brief  回転の取得
 *
 *  @retval  rotate   各軸の回転値
 */
//-----------------------------------------------------------------------------
void LytWk::GetRotate( gfl2::math::Vector3* p_rotate ) const
{
  m_base->GetRotate( p_rotate );
}

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
void LytWk::SetCullingFlag( bool flag )
{
  m_base->SetCullingFlag(flag);
}

//----------------------------------------------------------------------------
/**
 *  @brief  カリングフラグの取得
 *
 *  @retval  true  カリング処理ON
 *  @retval false カリング処理OFF
 */
//-----------------------------------------------------------------------------
bool LytWk::IsCullingFlag( void ) const
{
  return m_base->IsCullingFlag();
}


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
u32 LytWk::CreateAnime( LytArcIndex anmDataID, const LytMultiResID * resID )
{
  const LytRes* lytRes = m_base->GetLytRes();
  u32 anmIdx;
  
  if( lytRes ){
    anmIdx = m_base->GetClearAnimeIndex();
    if( anmIdx != LYTWKBASE_ANIMEIDX_ERROR ){
      m_base->CreateAnime( anmIdx, lytRes, resID->GetMultiID(), anmDataID );
      return anmIdx;
    }
  }

  return LYTSYS_ANIMEINDEX_ERROR;
}

//----------------------------------------------------------------------------
/**
 *  @brief  アニメーションの削除
 *
 *  @param  anmIdx  削除するアニメーションインデックス
 */
//-----------------------------------------------------------------------------
void LytWk::DeleteAnime( u32 anmIdx )
{
  m_base->DeleteAnime( anmIdx );
}

//----------------------------------------------------------------------------
/**
 *  @brief    アニメーション情報があるかチェック
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
 *  @brief  レイアウトコアシステム　（ベースライブラリのクラス）取得
 *
 *  @return ベースライブラリのクラス
 */
//-----------------------------------------------------------------------------
LytCore* LytWk::GetLayoutCore( void ) const
{
  return m_base->GetCore();
}

//----------------------------------------------------------------------------
/**
 *  @brief    非表示のペインに対して行列計算を行うかどうかを設定する
 *
 *  @return    none
 */
//-----------------------------------------------------------------------------
void LytWk::SetInvisiblePaneCalculateMtx( bool flg )
{
  m_base->SetInvisiblePaneCalculateMtx(flg);
}

//----------------------------------------------------------------------------
/**
 *  @brief    コンバート時に埋め込まれたフォント幅を取得する。
 *            実際は下記帳ユーザーデータにg2d_fwとして埋め込まれた実数
 *
 *  @return    f32 幅。0の時は取得失敗
 */
//-----------------------------------------------------------------------------
f32 LytWk::GetFontWidthWhenConvert( LytPaneIndex index )
{
  LytTextBox *textPane = GetTextBoxPane(index);
  
  if( textPane )
  {
    const nw::lyt::res::ExtUserData *extData = textPane->FindExtUserDataByName("g2d_fw");
    if( extData )
    {
      return extData->GetFloatArray()[0];
    }
  }
  return 0;
}


//----------------------------------------------------------------------------
/**
 *  @brief  描画中か
 *  @return  trueのとき描画中(描画中はLytSys::RemoveLytWkで破棄してはならない)
 *  @note   LytWkBase(m_base)のm_drawingCountを見張っている
 */
//----------------------------------------------------------------------------
bool LytWk::IsDrawing(void) const
{
  return m_base->IsDrawing();
}

//! @name  描画中カウント(friendであるDrawingLytWkManagerからしかアクセスしない)
//@{
void LytWk::IncreaseDrawingCount(void)
{
  m_base->IncreaseDrawingCount();
}
void LytWk::DecreaseDrawingCount(void)
{
  m_base->DecreaseDrawingCount();
}
//@}


//----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
LytWk::LytWk(void)
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  デストラクタ
 */
//-----------------------------------------------------------------------------
LytWk::~LytWk()
{
  Finalize();
}

//----------------------------------------------------------------------------
/**
 *  @brief  初期化
 *
 *  @param  allocator     アロケータ
 */
//-----------------------------------------------------------------------------
void LytWk::Initialize( gfl2::heap::NwAllocator* allocator, gfl2::heap::NwAllocator* dev_allocator, LytWkManager* p_wkman )
{
  gfl2::heap::HeapBase* heap = dev_allocator->GetHeapBase();

  GFL_ASSERT( m_base == NULL );

  m_base = GFL_NEW(heap) LytWkBase();
  m_parent = p_wkman;
}

//----------------------------------------------------------------------------
/**
 *  @brief  破棄
 */
//-----------------------------------------------------------------------------
void LytWk::Finalize( void )
{
  // (this->IsDrawing()==false)が偽だったら、レイアウト描画中に破棄となる。そのチェックはLytWkBase(m_base)側で行っているので、LytWk側では行わない。
  DrawingLytWkManager* drawingLytWkManager = m_parent->GetDrawingLytWkManager();  // ここでポインタを取得しておいて・・・

  if( m_base ){
    // ベース破棄
    GFL_DELETE m_base;
    m_base = NULL;
    m_parent = NULL;
  }

  // ・・・ここで強制的に登録解除する
  {
    if(drawingLytWkManager)
    {
      drawingLytWkManager->RemoveLytWkForce(this);
    }
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  レイアウトワークベース取得
 *
 *  @retval レイアウトワークベース
 */
//-----------------------------------------------------------------------------
LytWkBase* LytWk::GetLytWkBase( void ) const
{
  return m_base;
}


//=============================================================================
/**
 *          レイアウトシステム ベースクラス　実装
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *  @brief  システム初期化
 *
 *  @param  heapAllocator
 *  @param  deviceAllocator 
 */
//-----------------------------------------------------------------------------
void LytSysBase::InitializeSystem( gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator, gfl2::lyt::LytShaderResource* pLSR )
{
  if(m_systemInitialized == false){
  
    //gfl2::heap::HeapBase* heap = heapAllocator->GetHeapBase();
    mpRectDrawerShaderRes = pLSR;

#if defined(GF_PLATFORM_CTR)
    // Laytouライブラリ初期化
    nw::lyt::Initialize(heapAllocator, deviceAllocator);

    {
  //    GFL_ASSERT(mpRectDrawerShader);
//      m_globalResource.Setup( mpRectDrawerShader, mRectDrawerShaderSize );
    }

    // drawer初期化    // cafeではdrawerが存在しない
    mpRectDrawerShaderRes->Setup();
    m_drawer.Initialize(*mpRectDrawerShaderRes->GetGraphicsResource());

    nngxUpdateState(NN_GX_STATE_ALL);

#elif defined(GF_PLATFORM_WIN32)
    // Laytouライブラリ初期化
    nw::lyt::Initialize( heapAllocator );

    {
      //GraphicsSystem::inst().LockContext();
      {
//        m_globalResource.Setup( 512 );
        mpRectDrawerShaderRes->Setup();
      }
      //GraphicsSystem::inst().UnlockContext();
    }
#endif

    // システムの初期化完了
    m_systemInitialized = true;
    m_lytSysCount       = 0;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  システム破棄
 */
//-----------------------------------------------------------------------------
void LytSysBase::FinalizeSystem( void )
{
  if( m_systemInitialized )
  {
    GFL_ASSERT_STOP( m_lytSysCount == 0 );
    
#if defined(GF_PLATFORM_CTR)
    // drawer開放
    m_drawer.Finalize();
    mpRectDrawerShaderRes->Release();
    //  m_globalResource.Finalize();
    // Laytouライブラリ アロケーター情報クリア
    nw::lyt::Initialize(NULL, NULL);
#elif defined(GF_PLATFORM_WIN32)
    //m_globalResource.Finalize();
    // Laytouライブラリ アロケーター情報クリア
    nw::lyt::Initialize( NULL );
#endif  // GF_PLATFORM_CTR

    m_systemInitialized = false;
    m_lytSysCount       = 0;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  Defaultフォント 設定
 *
 *  @param  font                  フォントデータ
 *  @param  name                  リソース名　(例　simple.bcfnt)　レイアウトがアクセスするための名称
 */
//-----------------------------------------------------------------------------
void LytSysBase::SetDefaultFont( gfl2::str::Font* font, const char* name, u32 index )
{
  if( index < LYTSYS_DEFAULT_FONT_INDEX_MAX ){
    m_defaultFont[index]       = font;
#if defined( GF_PLATFORM_CTR )
    strcpy( m_defaultFontName[index], name);
#elif defined( GF_PLATFORM_WIN32 )
    strcpy_s( m_defaultFontName[index], name);
#endif
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  Defaultフォントがあるかチェック
 *
 *  @retval true  ある
 *  @retval false ない
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
 *  @brief  Defaultフォント取得
 *
 *  @retval Defaultフォント
 */
//-----------------------------------------------------------------------------
gfl2::str::Font* LytSysBase::GetDefaultFont(u32 index)
{
  if( index < LYTSYS_DEFAULT_FONT_INDEX_MAX ){
    return m_defaultFont[index];
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief  Defaultフォント名取得
 *
 *  @return Defaultフォント名
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
 *  @brief  追加されたフォント情報を消す
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
 *  @brief  コンストラクタ  ファイル非同期読み込みVer
 *
 *  @param  セットアップ情報
 */
//-----------------------------------------------------------------------------
LytSysBase::LytSysBase()
  : m_Initialized(false)
  , m_InitializeSystem(false)
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 *
 *  @param  セットアップ情報
 *  @param  drawingLytWkManager  描画中レイアウトワークのマネージャ。NULL可。NULLのとき描画中か否かの管理を行わない。
 */
//-----------------------------------------------------------------------------
LytSysBase::LytSysBase( const LytSysSetUp* setup, gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator, DrawingLytWkManager* drawingLytWkManager )
  : m_Initialized(false)
  , m_InitializeSystem(false)
{
  mpRectDrawerShaderRes = setup->pRectDrawerShaderRes;
  this->Initialize( setup, heapAllocator, deviceAllocator, drawingLytWkManager );
}


//----------------------------------------------------------------------------
/**
 *  @brief  レイアウトシステムベース初期化　同期Ver
 *
 *  @param  setup
 *  @param  heapAllocator
 *  @param  deviceAllocator 
 *  @param  drawingLytWkManager  描画中レイアウトワークのマネージャ。NULL可。NULLのとき描画中か否かの管理を行わない。
 */
//-----------------------------------------------------------------------------
void LytSysBase::Initialize( const LytSysSetUp* setup, gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator, DrawingLytWkManager* drawingLytWkManager )
{
  gfl2::heap::HeapBase* devheap = heapAllocator->GetHeapBase();
  
  // メモリアロケーター生成
  m_allocator = heapAllocator;
  m_devAllocator = deviceAllocator;

  // モジュールごとの初期化は禁止
  if(!m_systemInitialized){
    GFL_ASSERT( 0 );
    InitializeSystem( m_allocator, m_devAllocator, setup->pRectDrawerShaderRes );
    m_InitializeSystem = true;
  }
  
  // マネージャの生成
  m_lytResMan   = GFL_NEW(devheap) LytResManager( heapAllocator,deviceAllocator, setup->lyt_resource_max );
  m_wkMan       = GFL_NEW(devheap) LytWkManager( heapAllocator,deviceAllocator, setup->lyt_wk_max, drawingLytWkManager );


  // グローバルデータ生成
  m_globalData = GFL_NEW(deviceAllocator->GetHeapBase()) LytWkBaseGlobalData();
  m_globalData->p_globalResource  = mpRectDrawerShaderRes->GetGraphicsResource();//&m_globalResource;
#if defined(GF_PLATFORM_CTR)  // cafeではdrawerが存在しない
  m_globalData->p_drawer          = &m_drawer;
#endif

  // 下画面カメラ設定初期値設定
  m_globalData->env[gfl2::lyt::DISPLAY_LOWER].SetCameraParam(
    Env::CAMERA_NORMAL,
    Env::DefaultProjection[gfl2::lyt::DISPLAY_LOWER],
    Env::DefaultView[gfl2::lyt::DISPLAY_LOWER] );

  // 初期化完了
  m_Initialized = true;
  
  // 生成されたレイアウトシステムの数を数える。
  ++m_lytSysCount;
}

//----------------------------------------------------------------------------
/**
 *  @brief  レイアウトシステムベース　非同期読み込み開始
 *
 *  @param  setup
 *  @param  heapAllocator
 *  @param  deviceAllocator 
 *  @param  drawingLytWkManager  描画中レイアウトワークのマネージャ。NULL可。NULLのとき描画中か否かの管理を行わない。
 */
//-----------------------------------------------------------------------------
void LytSysBase::InitializeAsync( const LytSysSetUp* setup, gfl2::heap::NwAllocator* heapAllocator, gfl2::heap::NwAllocator* deviceAllocator, DrawingLytWkManager* drawingLytWkManager )
{
  // メモリアロケーター生成
  m_allocator = heapAllocator;
  m_devAllocator = deviceAllocator;

  // モジュールごとの初期化は禁止
  if(!m_systemInitialized){
    GFL_ASSERT( 0 );
    InitializeSystem( m_allocator, m_devAllocator ,setup->pRectDrawerShaderRes);
    m_InitializeSystem = true;
  }

  // マネージャの生成
  m_lytResMan   = GFL_NEW(deviceAllocator->GetHeapBase()) LytResManager( heapAllocator,deviceAllocator, setup->lyt_resource_max );
  m_wkMan       = GFL_NEW(deviceAllocator->GetHeapBase()) LytWkManager( heapAllocator,deviceAllocator, setup->lyt_wk_max, drawingLytWkManager );

  // グローバルデータ生成
  m_globalData = GFL_NEW(deviceAllocator->GetHeapBase()) LytWkBaseGlobalData();
  
  m_globalData->p_globalResource  = mpRectDrawerShaderRes->GetGraphicsResource();//&m_globalResource;
#if defined(GF_PLATFORM_CTR)  // cafeではdrawerが存在しない
  m_globalData->p_drawer          = &m_drawer;
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  レイアウトシステムベース　非同期更新処理
 *
 *  @retval true  非同期初期化完了
 *  @retval false 途中
 */
//-----------------------------------------------------------------------------
bool LytSysBase::InitializeAsyncWait( void )
{
  // 初期化完了チェック
  if(m_Initialized){
    return true;
  }
  
  // 下画面カメラ設定初期値設定
  m_globalData->env[gfl2::lyt::DISPLAY_LOWER].SetCameraParam(
    Env::CAMERA_NORMAL,
    Env::DefaultProjection[gfl2::lyt::DISPLAY_LOWER],
    Env::DefaultView[gfl2::lyt::DISPLAY_LOWER] );

  // 初期化完了
  m_Initialized = true;

  // 生成されたレイアウトシステムの数を数える。
  ++m_lytSysCount;

  return true;
}


//----------------------------------------------------------------------------
/**
 *  @brief  デストラクタ
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

  // モジュールごとの初期化は禁止
  if( m_InitializeSystem ){
    GFL_ASSERT( 0 );
    FinalizeSystem();
    m_InitializeSystem = false;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  レイアウトワークが共有して使用するワーク取得
 *
 *  @return 共有ワーク
 */
//-----------------------------------------------------------------------------
LytWkBaseGlobalData* LytSysBase::GetGlobalData(void)
{
  return m_globalData;
}

//----------------------------------------------------------------------------
/**
 *  @brief  リソースマネージャ取得
 *
 *  @return リソースマネージャ
 */
//-----------------------------------------------------------------------------
LytResManager* LytSysBase::GetResManager(void) const
{
  return m_lytResMan;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ワークマネージャ取得
 *
 *  @return ワークマネージャ
 */
//-----------------------------------------------------------------------------
LytWkManager* LytSysBase::GetWkManager(void) const
{
  return m_wkMan;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ヒープアロケータの取得
 *  @return ヒープアロケータ
 */
//-----------------------------------------------------------------------------
gfl2::heap::NwAllocator* LytSysBase::GetHeapAllocator(void) const
{
  return m_allocator;
}

//----------------------------------------------------------------------------
/**
 *  @brief  デバイスアロケータ取得
 *
 *  @return デバイスアロケータ
 */
//-----------------------------------------------------------------------------
gfl2::heap::NwAllocator* LytSysBase::GetDeviceAllocator(void) const
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
 *  @brief  コンストラクタ
 *
 *  @param  allocator   アロケータ
 *  @param  wk_max      ワーク最大数
 *  @param  drawingLytWkManager  描画中レイアウトワークのマネージャ。NULL可。NULLのとき描画中か否かの管理を行わない。
 */
//-----------------------------------------------------------------------------
LytWkManager::LytWkManager( gfl2::heap::NwAllocator* allocator,gfl2::heap::NwAllocator* dev_allocator,  u32 wk_max, DrawingLytWkManager* drawingLytWkManager ) : 
  m_linkTop(),
  m_drawingLytWkManager(drawingLytWkManager)
{
  //GFL_ASSERT(drawingLytWkManager);  // drawingLytWkManagerはNULL可なのでこのASSERTは抜けてもらっていい。何故ASSERTを入れているかというとnijiでレイアウトの破棄タイミングを管理していないものがないようにするため。

  gfl2::heap::HeapBase* heap = dev_allocator->GetHeapBase();
  int i;
  
  m_array    = GFL_NEW(heap) LytWk[wk_max];
  m_arrayNum  = wk_max;
  m_createNum = 0;

  // 初期化
  for( i=0; i<m_arrayNum; i++ ){
    m_array[i].Initialize( allocator,dev_allocator, this );
  }

  m_linkTop.Initialize(allocator,dev_allocator, this);

  //  リンクリスト初期化
  m_linkTop.GetLytWkBase()->SetPrev( &m_linkTop );
  m_linkTop.GetLytWkBase()->SetNext( &m_linkTop );
}

//----------------------------------------------------------------------------
/**
 *  @brief  デストラクタ
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
 *  @brief  オート描画される全ワークを更新する。
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
 *  @brief  オート描画設定されている全ワークを描画
 */
//-----------------------------------------------------------------------------

void LytWkManager::DrawAll( LytWKGroupID groupID )
{
#if 0
  LytWk* pNow = NULL;

  // drawtypeが不正なら描画しない
  //if(drawtype >= lyt::DISPLAY_BOTH_MAX){
    //GFL_ASSERT(drawtype < lyt::DISPLAY_BOTH_MAX); //@check
    //return ;
  //}
  glDisable( GL_DEPTH_TEST );
  glDepthMask( GL_FALSE );
  glDisable( GL_CULL_FACE );

  // 全LytWkリストをLOOP
  pNow = m_linkTop.GetLytWkBase()->GetNext();
  while( pNow != &m_linkTop )
  {
    // オート描画されるワークならば、更新
    if( pNow->GetLytWkBase()->IsAutoDraw() )
    {
      if( CheckGroupID( groupID, pNow->GetLytWkBase()->GetWkGroupID() ) )
      {
        pNow->GetLytWkBase()->Draw();
      }
    }

    // 次のワーク
    pNow = pNow->GetLytWkBase()->GetNext();
  }
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  リンクリストにワークを登録
 *
 *  @param  wk  ワーク
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
 *  @brief  リンクリストからワークを削除
 *
 *  @param  wk  ワーク
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
 *  @brief  Priorityによるソートを行う。
 *
 *  @param  wk  ソートしなおしてほしいワーク
 */
//-----------------------------------------------------------------------------
void LytWkManager::SortPriority( LytWk* wk )
{
  DelLink(wk);
  AddLink(wk);
}

//----------------------------------------------------------------------------
/**
 *  @brief  グループIDの一致チェック
 *
 *  @param  groupID_a   グループID  A
 *  @param  groupID_b   グループID　B
 *
 *  @retval true    一致
 *  @retval false   不一致
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
 *  @brief    コンストラクタ
 */
//-----------------------------------------------------------------------------
LytResManager::LytResManager( gfl2::heap::NwAllocator* allocator, gfl2::heap::NwAllocator* dev_allocator, u32 lyt_res_max )
{
  gfl2::heap::HeapBase* heap = dev_allocator->GetHeapBase();
  
  m_lytArray = GFL_NEW_ARRAY(heap) LytRes[lyt_res_max]();
  m_lytArrayNum = lyt_res_max;
}

//----------------------------------------------------------------------------
/**
 *  @brief    デストラクタ
 */
//-----------------------------------------------------------------------------
LytResManager::~LytResManager()
{
  // 配列破棄
  GFL_DELETE_ARRAY m_lytArray;
}

//-----------------------------------------------------------------------------
/**
 * @brief   未使用リソースワークを取得
 *
 * @param   parent_id   リソース親IDの格納場所 ( NULL可 )
 *
 * @retval  "NULL != 未使用リソースワーク"
 * @retval  "NULL = 未使用ワークなし"
 */
//-----------------------------------------------------------------------------
LytRes * LytResManager::GetClearLytRes( u32 * parent_id  ) const
{
  // 空のワークを検索
  for( u32 i=0; i<m_lytArrayNum; i++ )
  {
    if( m_lytArray[i].IsDataIn() == false )
    {
      if( parent_id != NULL )
      {
        *parent_id = i;
      }
      return &m_lytArray[i];
    }
  }

  // 空のワークなし
  GFL_ASSERT( 0 );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   指定親IDのリソースワークを取得
 *
 * @param   id  リソース親ID
 *
 * @retval  "NULL != リソースワーク"
 * @retval  "NULL = ID範囲外"
 */
//-----------------------------------------------------------------------------
LytRes * LytResManager::GetIndexLytRes( u32 parent_id ) const
{
  if( parent_id >= m_lytArrayNum )
  {
    GFL_ASSERT( 0 );
    return NULL;
  }
  return &m_lytArray[parent_id];
}

//----------------------------------------------------------------------------
/**
 *  @brief  使用中のリソースの数を返す
 *
 *  @return
 */
//-----------------------------------------------------------------------------
u32 LytResManager::GetUseLytResNum( void ) const
{
  u32 count = 0;
  // 空のワークを検索
  for( u32 i=0; i<m_lytArrayNum; i++ ){
    if( m_lytArray[i].IsDataIn() ){
      ++count;
    }
  }

  return count;
}

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
LytMultiResID LytResManager::InitMultiResource( gfl2::heap::HeapBase * heap, u32 link_max )
{
  u32 id;
  LytRes * res = GetClearLytRes( &id );
  LytMultiResID resID;

  if( res == NULL )
  {
    GFL_ASSERT( 0 );
    return resID;
  }
  res->InitMultiResource( heap, link_max );

  resID.SetParentID( id );
  resID.SetMultiID( 0 );

  return resID;
}


GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)
