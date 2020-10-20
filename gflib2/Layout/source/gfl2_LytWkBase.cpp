//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl2_LytWkBase.cpp
 *  @brief  レイアウトワークベース
 *  @author tomoya takahshi
 *  @date   2010.10.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if defined( GF_PLATFORM_WIN32 )
#include <nw/gfnd.h>  // add
#endif  // GF_PLATFORM_WIN32

#include <thread/include/gfl2_ThreadStatic.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_DebugTick.h>

#include <layout/include/gfl2_LytWkBase.h>

#include <layout/include/gfl2_LytSys.h>
#include <math/include/gfl2_math.h>

#include <gfx/include/gfl2_GFGL.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------
enum{
  CLACTWK_TICK_PRINT_ANIME_CREATE,
  CLACTWK_TICK_PRINT_ANIME_INIT,
  CLACTWK_TICK_PRINT_ANIME_BIND,
  CLACTWK_TICK_PRINT_MAX,
};

GFL_DEBUG_TICK_SETUP(CLACTWK, CLACTWK_TICK_PRINT_MAX );

#if GFL_DEBUG
  
#define CLWKBASE_TICK_DUMP (0)
  
#else

#define CLWKBASE_TICK_DUMP (0)

#endif


//-----------------------------------------------------------------------------
/**
 *          クラス宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *    ローカル関数
 */
//-----------------------------------------------------------------------------
// ユーザーアロケーター設定
#if defined(GF_PLATFORM_CTR)
#define LYTWKBASE_CHANGE_USER_ALLOCATOR()  \
  nw::os::IAllocator* p_default_allocator_heap = nw::lyt::Layout::GetAllocator();\
  nw::os::IAllocator* p_default_allocator_device = nw::lyt::Layout::GetDeviceMemoryAllocator();\
  nw::lyt::Layout::SetAllocator( m_pUserAllocator );\
  nw::lyt::Layout::SetDeviceMemoryAllocator( m_pUserDeviceAllocator );
#elif defined(GF_PLATFORM_WIN32)
#define LYTWKBASE_CHANGE_USER_ALLOCATOR()  \
  nw::ut::IAllocator* p_default_allocator_heap = nw::lyt::Layout::GetAllocator();\
  nw::lyt::Layout::SetAllocator( m_pUserAllocator );
#endif

// デフォルトアロケーターに戻す
#if defined(GF_PLATFORM_CTR)
#define LYTWKBASE_RETURN_ALLOCATOR()  \
  nw::lyt::Layout::SetAllocator( p_default_allocator_heap );\
  nw::lyt::Layout::SetDeviceMemoryAllocator( p_default_allocator_device );
#elif defined(GF_PLATFORM_WIN32)
#define LYTWKBASE_RETURN_ALLOCATOR()  \
  nw::lyt::Layout::SetAllocator( p_default_allocator_heap );
#endif
  

//=============================================================================
/**
 *          LytWkBase実装
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LytWkBase::LytWkBase( void )
  : m_ppPaneTbl( NULL )
{
  GFL_DEBUG_TICK_SET_FLAG( CLACTWK, CLWKBASE_TICK_DUMP );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LytWkBase::~LytWkBase()
{
  if( m_Initialize )
  {
    Finalize();
  }
}


//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク初期化
 *
 * @param   heap_allocator        アロケーター
 * @param   device_allocator      デバイスアロケーター
 * @param   res                   リソース情報
 * @param   res_multi_id          リソース登録ID
 * @param   lytDataID             レイアウトデータID
 * @param   animeNum              アニメーション最大登録数
 * @param   globalData            共有情報
 * @param   createPaneTbl         ペインテーブル生成
 * @param   createPaneTblWithHash 高速検索用にハッシュキーを作る（消費メモリ増）
 */
//-----------------------------------------------------------------------------
void LytWkBase::Initialize(
  gfl2::heap::NwAllocator * heap_allocator,
  gfl2::heap::NwAllocator * device_allocator,
  const LytRes * res,
  u32 res_multi_id,
  u32 lytDataID,
  u32 animeNum,
  LytWkBaseGlobalData * globalData,
  b32 createPaneTblFlag,
  b32 createPaneTblWithHash )
{
  bool result;
  gfl2::heap::HeapBase* heap = heap_allocator->GetHeapBase();

  // 使用するアロケーター登録
  m_pUserAllocator = heap_allocator;
  m_pUserDeviceAllocator = device_allocator;

  // アロケータを指定の物に書き換え
  LYTWKBASE_CHANGE_USER_ALLOCATOR();

  m_globalData = globalData;

  m_LytSys = GFL_NEW(heap) nw::lyt::Layout();
  
  result = m_LytSys->Build( res->GetLayoutResource(lytDataID,res_multi_id), res->GetResourceAccessor() );
  GFL_ASSERT_STOP(result);

  m_LytDrawInfo.SetGraphicsResource( m_globalData->p_globalResource );

  if(animeNum>0)
  {
    m_AnimArray = GFL_NEW_ARRAY(heap) LytWkAnime[animeNum];
  }
  else
  {
    m_AnimArray = NULL;
  }
  m_AnimMax = animeNum;

  m_res        = res;
  m_resMultiID = res_multi_id;

  // オート描画ON
  m_AutoDrawFlag = true;

  // GlobalEnv使用ON
  m_useGlobalEnv = true;

  // ヒットチェックが可能かどうか
  m_hitCheckOk = false;

  // 描画中カウント初期化
  m_drawingCount = 0;

  // 初期化完了
  m_Initialize = true;

  // カリングON
  m_CullingFlag = true;

  // 初期値は両画面表示  // ここで言う両面とはSetDrawDispType関数より推測すると全画面(上(左)、下、右のこと)と思われる。
  m_DisplayType = gfl2::lyt::DISPLAY_BOTH_MAX;

  // ペインテーブル生成
  if(createPaneTblFlag)
  {
    createPaneTbl( heap, GFL_BOOL_CAST(createPaneTblWithHash) );
  }

  // アロケータを元に戻す
  LYTWKBASE_RETURN_ALLOCATOR();
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク破棄
 */
//-----------------------------------------------------------------------------
void LytWkBase::Finalize( void )
{
  GFL_ASSERT_MSG(this->IsDrawing()==false, "レイアウト描画中に破棄 LytWkBase(LytWk内のm_base)のアドレス0x%p\n", this);  // ASSERTを抜けたら仕方がないので破棄しに行く。
  if(this->IsDrawing()!=false)
  {
    // ASSERTを抜けた際のケア
    // GPUを進ませるために1フレーム分待つ
    gfl2::thread::ThreadStatic::CurrentSleep(34);  // 34 mili seconds 待つ
  }

  GFL_ASSERT_STOP(m_LytSys);

  // ペインテーブル破棄
  DeletePaneTbl();

  // アニメーション破棄処理
  for( u32 i=0; i<m_AnimMax; i++ )
  {
    DeleteAnime(i);
  }
  
  if( m_AnimArray )
  {
    GFL_DELETE_ARRAY m_AnimArray;
  }
  m_AnimArray = NULL;
  m_AnimMax   = 0;

#if defined(GF_PLATFORM_CTR)
  GFL_SAFE_DELETE( m_LytSys );
#elif defined(GF_PLATFORM_WIN32)
  nw::ut::SafeFreeWithDestruct( m_LytSys, m_pUserAllocator );  // LockContext();
#endif

  m_pUserDeviceAllocator = NULL;
  m_pUserAllocator = NULL;

  // 破棄完了
  m_Initialize = false;
}
  
//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク初期化済みチェック
 *
 * @retval  true  初期化完了
 * @retval  false 初期化前
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsInitialized(void) const
{
  return m_Initialize;
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク更新
 */
//-----------------------------------------------------------------------------
void LytWkBase::Update( void )
{
  // オートアニメーション管理
#if defined( GF_PLATFORM_CTR )
  m_LytSys->Animate();
#elif defined( GF_PLATFORM_WIN32 )
  m_LytSys->AnimateAndUpdateAnimFrame();
#endif
  UpdateAutoAnime();
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク描画
 *
 * @param   lod           描画面
 * @param   cameraType    カメラタイプ
 * @param   renderTarget  レンダーターゲット
 */
//-----------------------------------------------------------------------------
#if defined( GF_PLATFORM_CTR )
bool LytWkBase::Draw( gfl2::lyt::DisplayType lod, Env::CameraType cameraType )
{
  bool isDrawing = false;

  const Env* pUseEnv = NULL;

  if(lod == lyt::DISPLAY_UPPER_RIGHT) lod = lyt::DISPLAY_UPPER;  // nijiでは左目、右目の描画呼び出しが分かれており別々のlodで呼び出されるが、gflibレイアウトはどちらにも上画面の設定を用いる。
  GFL_ASSERT( lod < lyt::DISPLAY_NORMAL_MAX ); //@check

  if( !this->IsDrawDisplayType( lod ) )
  {
    return isDrawing;
  }

  // 使用する環境設定を取得
  if( m_useGlobalEnv ) {
    pUseEnv = &m_globalData->env[lod];
  } else {
    pUseEnv = m_UserEnv[lod];
  }
  const Camera& camera = pUseEnv->GetCamera( cameraType );

  // カメラ設定
  {
    m_LytDrawInfo.SetProjectionMtx( gfl2::math::ConvertGfMtx44ToNwMtx44(camera.GetProjectionMtx()) );
    m_LytDrawInfo.SetViewMtx( gfl2::math::ConvertGfMtx34ToNwMtx34(camera.GetViewMtx()) );

    if( lod == lyt::DISPLAY_UPPER )
    {
      const Parallax& parallax = pUseEnv->GetParallax();
      if( cameraType == Env::CAMERA_NORMAL )
      {
        m_globalData->p_drawer->SetParallax( -parallax.parallax_half, parallax.dLevel, parallax.addDist );
      }
      else
      {
        m_globalData->p_drawer->SetParallax( parallax.parallax_half, parallax.dLevel, parallax.addDist );
      }
    }
    else{
      m_globalData->p_drawer->SetParallax( 0.0f, 0.0f, 0.0f );
    }
  }

  // 上画面と下画面で１回でOK？
  m_LytSys->CalculateMtx( m_LytDrawInfo, true );

  // 描画範囲内チェック
  if( IsCulling( camera ) )
  {
    // 描画環境の情報をコマンド設定
    //pUseEnv->Add3DCommand( renderTarget );

    nn::gr::CommandBufferJumpHelper* jh = gfx::GFGL::GetJumpHelper();
    
    m_globalData->p_drawer->DrawBegin( *jh, m_LytDrawInfo );
    m_globalData->p_drawer->Draw( *jh, m_LytSys,  m_LytDrawInfo );
    m_globalData->p_drawer->DrawEnd( *jh, m_LytDrawInfo );

    // 一度描画後はヒットチェック可能
    m_hitCheckOk = true;

    isDrawing = m_LytSys->GetRootPane()->IsVisible();
  }

  return isDrawing;
}
#elif defined( GF_PLATFORM_WIN32 )
bool LytWkBase::Draw( gfl2::lyt::DisplayType lod, Env::CameraType cameraType )
{
  bool isDrawing = false;
  
  const Env* pUseEnv = NULL;

  if(lod == lyt::DISPLAY_UPPER_RIGHT) lod = lyt::DISPLAY_UPPER;  // nijiでは左目、右目の描画呼び出しが分かれており別々のlodで呼び出されるが、gflibレイアウトはどちらにも上画面の設定を用いる。
  GFL_ASSERT( lod < lyt::DISPLAY_NORMAL_MAX ); //@check

  if( !this->IsDrawDisplayType( lod ) )
  {
    return isDrawing;
  }

  // 使用する環境設定を取得
  if( m_useGlobalEnv ) {
    pUseEnv = &m_globalData->env[lod];
  } else {
    pUseEnv = m_UserEnv[lod];
  }
  const Camera& camera = pUseEnv->GetCamera( cameraType );

  // カメラ設定
  {
    m_LytDrawInfo.SetProjectionMtx( gfl2::math::ConvertGfMtx44ToNwMtx44( camera.GetProjectionMtx() ));
    m_LytDrawInfo.SetViewMtx( gfl2::math::ConvertGfMtx34ToNwMtx34( camera.GetViewMtx() ));
  }

  m_LytSys->CalculateMtx( m_LytDrawInfo );

  // 描画範囲内チェック
  if( IsCulling( camera ) ){

    m_LytSys->Draw( m_LytDrawInfo );

    // 一度描画後はヒットチェック可能
    m_hitCheckOk = true;

    isDrawing = m_LytSys->GetRootPane()->IsVisible();
  }
  
  return isDrawing;
}
#endif

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトワークグループIDの設定
 *
 * @param   groupID   グループID
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetWkGroupID( u16 groupID )
{
  m_WkGroupID = groupID;
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトワークグループIDの取得
 *
 * @return  グループID
 */
//-----------------------------------------------------------------------------
u16 LytWkBase::GetWkGroupID( void ) const
{
  return m_WkGroupID;
}

//-----------------------------------------------------------------------------
/**
 * @brief   UpperLowerに対応したユーザー描画環境データ設定
 *
 * @param   upperEnv  上画面用ENV
 * @param   lowerEnv  下画面用ENV
 *
 *  *描画にGlobalEnvではなく引数のENVを使用するようになります。
 *  *戻すには、SetGlobalEnv()を呼んでください。
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetUserEnv( const Env* upperEnv, const Env* lowerEnv )
{
  m_UserEnv[gfl2::lyt::DISPLAY_UPPER] = upperEnv;
  m_UserEnv[gfl2::lyt::DISPLAY_LOWER] = lowerEnv;
  m_useGlobalEnv = false;
}

//-----------------------------------------------------------------------------
/**
 * @brief   グローバルENVを使用するように設定
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetGlobalEnv( void )
{
  if( m_useGlobalEnv == false ){
    m_useGlobalEnv = true;
    m_UserEnv[gfl2::lyt::DISPLAY_UPPER] = NULL;
    m_UserEnv[gfl2::lyt::DISPLAY_LOWER] = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   リンクリスト: 1つ前のワーク設定<LytWk型>
 *
 * @param   prev  １つ前のワーク
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetPrev( LytWk* prev )
{
  m_prev = prev;
}

//-----------------------------------------------------------------------------
/**
 * @brief   リンクリスト: 1つ前のワーク取得<LytWk型>
 *
 * @retval  １つ前のワークreinterpret_cast<LytWk*>として使用してください。
 */
//-----------------------------------------------------------------------------
LytWk* LytWkBase::GetPrev( void ) const
{
  return m_prev;
}

//-----------------------------------------------------------------------------
/**
 * @brief   リンクリスト: 次のワーク設定<LytWk型>
 *
 * @param   next  次のワーク
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetNext( LytWk* next )
{
  m_next = next;
}

//-----------------------------------------------------------------------------
/**
 * @brief   リンクリスト: 次のワーク取得<LytWk型>
 *
 * @retval  次のワークreinterpret_cast<LytWk*>として使用してください。
 */
//-----------------------------------------------------------------------------
LytWk* LytWkBase::GetNext( void ) const
{
  return m_next;
}


//=============================================================================
/**
 * ペイン操作
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief   ペインを取得
 *
 * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
 *
 * @retval  "NULL != ペイン"
 * @retval  "NULL = ペインが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Pane* LytWkBase::GetPane( u32 paneIndex ) const
{
  // まずは、ペインテーブルから取得
  nw::lyt::Pane* p_pane = getPaneTblPane( paneIndex, m_resMultiID );
  if(p_pane)
  {
    return p_pane;
  }
  
  // 無ければ、下から、
  const char * paneName = m_res->GetPaneName( paneIndex, m_resMultiID );
  if( paneName )
  {
    // 4文字以上のレイアウトがあるのでここは正規の名前での検索必須
    p_pane = m_LytSys->GetRootPane()->FindPaneByName( paneName, true );

    if( m_ppPaneTbl != NULL )
    {
      m_ppPaneTbl[paneIndex] = p_pane;
    }
  }
  return p_pane;
}

//----------------------------------------------------------------------------
/**
 * @brief   ピクチャーペインを取得
 *
 * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
 *
 * @retval  "NULL != ピクチャーペイン"
 * @retval  "NULL = ペインが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Picture* LytWkBase::GetPicturePane( u32 paneIndex ) const
{
  nw::lyt::Pane * pane = GetPane( paneIndex );
  if( pane )
  {
    return nw::ut::DynamicCast<nw::lyt::Picture *>( pane );
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインを取得
 *
 * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
 *
 * @retval  "NULL != テキストボックスペイン"
 * @retval  "NULL = ペインが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::TextBox* LytWkBase::GetTextBoxPane( u32 paneIndex ) const
{
  nw::lyt::Pane * pane = GetPane( paneIndex );
  if( pane )
  {
    return nw::ut::DynamicCast<nw::lyt::TextBox *>( pane );
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインの文字列バッファを確保
 *
 * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
 * @param   minLen        文字数
 */
//-----------------------------------------------------------------------------
void LytWkBase::AllocTextBoxPaneStringBuffer( u32 paneIndex, u32 minLen ) const
{
  nw::lyt::TextBox * pane = this->GetTextBoxPane( paneIndex );
  
  if( pane )  // @fix KW[7]
  {
    // アロケータを指定の物に書き換え
    LYTWKBASE_CHANGE_USER_ALLOCATOR();
    
    pane->AllocStringBuffer( minLen );

    // アロケータを元に戻す
    LYTWKBASE_RETURN_ALLOCATOR();
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインの文字列バッファを確保
 *
 * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
 * @param   minLen        文字数
 */
//-----------------------------------------------------------------------------
void LytWkBase::AllocTextBoxPaneStringBuffer( gfl2::lyt::LytTextBox* pTextBoxPain, u32 minLen ) const
{
  nw::lyt::TextBox * pane = pTextBoxPain;
  
  if( pane )  // @fix KW[7]
  {
    // アロケータを指定の物に書き換え
    LYTWKBASE_CHANGE_USER_ALLOCATOR();
    
    pane->AllocStringBuffer( minLen );

    // アロケータを元に戻す
    LYTWKBASE_RETURN_ALLOCATOR();
  }
}



//-----------------------------------------------------------------------------
/**
 * @brief   ウィンドウペインを取得
 *
 * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
 *
 * @retval  "NULL != ウィンドウペイン"
 * @retval  "NULL = ペインが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Window* LytWkBase::GetWindowPane( u32 paneIndex ) const
{
  nw::lyt::Pane * pane = GetPane( paneIndex );
  if( pane )
  {
    return nw::ut::DynamicCast<nw::lyt::Window *>( pane );
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   バウンディングペインを取得
 *
 * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
 *
 * @retval  "NULL != バウンディングペイン"
 * @retval  "NULL = ペインが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Bounding * LytWkBase::GetBoundingPane( u32 paneIndex ) const
{
  nw::lyt::Pane * pane = GetPane( paneIndex );
  if( pane )
  {
    return nw::ut::DynamicCast<nw::lyt::Bounding *>( pane );
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   部品ペインを取得
 *
 * @param   paneIndex     コンバータから出力されるインデックス ( *_pane.h )
 *
 * @retval  "NULL != 部品ペイン"
 * @retval  "NULL = ペインが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Parts * LytWkBase::GetPartsPane( u32 paneIndex ) const
{
  nw::lyt::Pane * pane = GetPane( paneIndex );
  if( pane )
  {
    return nw::ut::DynamicCast<nw::lyt::Parts *>( pane );
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 * @brief   マテリアル情報を取得
 *
 * @param   materialIndex   コンバータから出力されるインデックス ( *_pane.h )
 *
 * @retval  "NULL != マテリアル情報"
 * @retval  "NULL = マテリアルが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Material* LytWkBase::GetMaterial( u32 materialIndex ) const
{
  const char * paneName = m_res->GetPaneName( materialIndex, m_resMultiID );
  if( paneName ){
    nw::lyt::Pane * rootPane = m_LytSys->GetRootPane();
    return rootPane->FindMaterialByName( paneName, true );
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   部品ペイン内のペインを取得
 *
 * @param   parts         部品ペイン
 * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
 * @param   res_multi_id  partsのリソース登録ID
 *
 * @retval  "NULL != ペイン"
 * @retval  "NULL = ペインが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Pane * LytWkBase::GetPane( nw::lyt::Parts * parts, u32 paneIndex, u32 res_multi_id ) const
{
  const char * paneName = m_res->GetPaneName( paneIndex, res_multi_id );
  if( paneName != NULL )
  {
    // 4文字以上のレイアウトがあるのでここは正規の名前での検索必須
    return parts->GetLayout()->GetRootPane()->FindPaneByName( paneName, true );
  }
  return NULL; 
}

//-----------------------------------------------------------------------------
/**
 * @brief   部品ペイン内のピクチャーペインを取得
 *
 * @param   parts         部品ペイン
 * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
 * @param   res_multi_id  partsのリソース登録ID
 *
 * @retval  "NULL != ピクチャーペイン"
 * @retval  "NULL = ペインが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Picture* LytWkBase::GetPicturePane( nw::lyt::Parts * parts, u32 paneIndex, u32 res_multi_id ) const
{
  nw::lyt::Pane * pane = GetPane( parts, paneIndex, res_multi_id );
  if( pane != NULL )
  {
    return nw::ut::DynamicCast<nw::lyt::Picture *>( pane );
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   部品ペイン内のテキストボックスペインを取得
 *
 * @param   parts         部品ペイン
 * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
 * @param   res_multi_id  partsのリソース登録ID
 *
 * @retval  "NULL != テキストボックスペイン"
 * @retval  "NULL = ペインが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::TextBox* LytWkBase::GetTextBoxPane( nw::lyt::Parts * parts, u32 paneIndex, u32 res_multi_id ) const
{
  nw::lyt::Pane * pane = GetPane( parts, paneIndex, res_multi_id );
  if( pane != NULL )
  {
    return nw::ut::DynamicCast<nw::lyt::TextBox *>( pane );
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   部品ペイン内のテキストボックスペインの文字列バッファを確保
 *
 * @param   parts         部品ペイン
 * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
 * @param   res_multi_id  partsのリソース登録ID
 * @param   minLen        文字数
 */
//-----------------------------------------------------------------------------
void LytWkBase::AllocTextBoxPaneStringBuffer( nw::lyt::Parts * parts, u32 paneIndex, u32 res_multi_id, u32 minLen ) const
{
  nw::lyt::TextBox * pane = this->GetTextBoxPane( parts, paneIndex, res_multi_id );
  
  if( pane )  // @fix KW[7]
  {
    // アロケータを指定の物に書き換え
    LYTWKBASE_CHANGE_USER_ALLOCATOR();
    
    pane->AllocStringBuffer( minLen );

    // アロケータを元に戻す
    LYTWKBASE_RETURN_ALLOCATOR();
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   部品ペイン内のウィンドウペインを取得
 *
 * @param   parts         部品ペイン
 * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
 * @param   res_multi_id  partsのリソース登録ID
 *
 * @retval  "NULL != ウィンドウペイン"
 * @retval  "NULL = ペインが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Window* LytWkBase::GetWindowPane( nw::lyt::Parts * parts, u32 paneIndex, u32 res_multi_id ) const
{
  nw::lyt::Pane * pane = GetPane( parts, paneIndex, res_multi_id );
  if( pane != NULL )
  {
    return nw::ut::DynamicCast<nw::lyt::Window *>( pane );
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   部品ペイン内のバウンディングペインを取得
 *
 * @param   parts         部品ペイン
 * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
 * @param   res_multi_id  partsのリソース登録ID
 *
 * @retval  "NULL != バウンディングペイン"
 * @retval  "NULL = ペインが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Bounding * LytWkBase::GetBoundingPane( nw::lyt::Parts * parts, u32 paneIndex, u32 res_multi_id ) const
{
  nw::lyt::Pane * pane = GetPane( parts, paneIndex, res_multi_id );
  if( pane != NULL )
  {
    return nw::ut::DynamicCast<nw::lyt::Bounding *>( pane );
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   部品ペイン内の部品ペインを取得
 *
 * @param   parts         部品ペイン
 * @param   paneIndex     patrs内のペインインデックス ( *_pane.h )
 * @param   res_multi_id  partsのリソース登録ID
 *
 * @retval  "NULL != 部品ペイン"
 * @retval  "NULL = ペインが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Parts * LytWkBase::GetPartsPane( nw::lyt::Parts * parts, u32 paneIndex, u32 res_multi_id ) const
{
  nw::lyt::Pane * pane = GetPane( parts, paneIndex, res_multi_id );
  if( pane != NULL )
  {
    return nw::ut::DynamicCast<nw::lyt::Parts *>( pane );
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 * @brief   部品ペイン内のマテリアル情報を取得
 *
 * @param   parts         部品ペイン
 * @param   materialIndex patrs内のマテリアルインデックス ( *_pane.h )
 * @param   res_multi_id  partsのリソース登録ID
 *
 * @retval  "NULL != マテリアル情報"
 * @retval  "NULL = マテリアルが存在しない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Material* LytWkBase::GetMaterial( nw::lyt::Parts * parts, u32 materialIndex, u32 res_multi_id ) const
{
  const char * paneName = m_res->GetPaneName( materialIndex, res_multi_id );
  if( paneName ){
    nw::lyt::Pane * rootPane = parts->GetLayout()->GetRootPane();
    return rootPane->FindMaterialByName( paneName, true );
  }
  return NULL;
}


//-----------------------------------------------------------------------------
/**
 * @brief   ヒットチェックが可能か取得
 *
 * @retval  true  可能
 * @retval  false 不可能
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsHitCheckOK( void ) const
{
  return m_hitCheckOk;
}


//=============================================================================
/**
 *  基本操作
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *  @brief    オート描画のON/OFF設定
 *  
 *  @param    flag    フラグ  true:ON   false:OFF
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetAutoDrawFlag( bool flag )
{
  m_AutoDrawFlag = flag;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    オート描画のON/OFF 取得
 *  
 *  @retval true    ON
 *  @retval false   OFF
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsAutoDraw( void ) const
{
  return m_AutoDrawFlag;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  処理優先順位の設定
 *
 *  @param  pri   優先順位（ 0 > 0xffff )
 *
 *  UpdateAllとDrawAllを行う際の優先順位です。
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetPriority( u16 pri )
{
  m_Priority = pri;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  処理優先順位の取得
 *
 *  @retval  優先順位（ 0 > 0xffff )
 *
 *  UpdateAllとDrawAllを行う際の優先順位です。
 */
//-----------------------------------------------------------------------------
u16 LytWkBase::GetPriority( void ) const
{
  return m_Priority;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  座標の設定
 *
 *  @param  pos
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetTranslate( const gfl2::math::Vector3& pos )
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  rootPane->SetTranslate( gfl2::math::ConvertGfVec3ToNwVec3(pos) );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  座標の取得
 *
 *  @param  p_pos 座標格納先
 */
//-----------------------------------------------------------------------------
void LytWkBase::GetTranslate( gfl2::math::Vector3* p_pos ) const
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  *p_pos = gfl2::math::ConvertNwVec3ToGfVec3( rootPane->GetTranslate() );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  拡大の設定
 *
 *  @param  scale   スケーリング値
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetScale( const gfl2::math::Vector2& scale )
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  rootPane->SetScale( gfl2::math::ConvertGfVec2ToNwVec2( scale ) );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  拡大の取得
 *
 *  @param p_scale スケーリング値格納先
 */
//-----------------------------------------------------------------------------
void LytWkBase::GetScale( gfl2::math::Vector2* p_scale ) const
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  *p_scale = gfl2::math::ConvertNwVec2ToGfVec2( rootPane->GetScale() );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  回転の設定
 *
 *  @param  rotate   各軸の回転値
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetRotate( const gfl2::math::Vector3& rotate )
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  rootPane->SetRotate( gfl2::math::ConvertGfVec3ToNwVec3(rotate) );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  回転の取得
 *
 *  @param  p_rotate   各軸の回転値
 */
//-----------------------------------------------------------------------------
void LytWkBase::GetRotate( gfl2::math::Vector3* p_rotate ) const
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  *p_rotate = gfl2::math::ConvertNwVec3ToGfVec3( rootPane->GetRotate() );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  カリングフラグの設定
 *
 *  @param  flag  フラグ
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetCullingFlag( bool flag )
{
  m_CullingFlag = flag;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  カリングフラグの取得
 *
 *  @retval true  カリング処理ON
 *  @retval false カリング処理OFF
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsCullingFlag( void ) const
{
  return m_CullingFlag;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  描画するディスプレイタイプを設定
 *
 *  @param  lod   描画するディスプレイタイプ
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetDrawDispType( gfl2::lyt::DisplayType lod )
{
  if( (lod == gfl2::lyt::DISPLAY_UPPER) || (lod == gfl2::lyt::DISPLAY_LOWER) || (lod == gfl2::lyt::DISPLAY_BOTH_MAX) ){
    m_DisplayType = lod;
  }
  else{
    GFL_ASSERT_MSG( 0, "DisplayType Error" );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  描画するディスプレイタイプの取得
 *
 *  @return 描画するディスプレイタイプ
 */
//-----------------------------------------------------------------------------
gfl2::lyt::DisplayType LytWkBase::GetDrawDispType( void ) const
{
  return m_DisplayType;
}


//=============================================================================
/**
 *  アニメーション関連
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief  空いているアニメーションインデックスを取得
 *  
 *  @retval 空いているアニメーションインデックス
 *  @retval LYTWKBASE_ANIMEIDX_ERROR    空いてない
 */
//-----------------------------------------------------------------------------
u32 LytWkBase::GetClearAnimeIndex( void ) const
{
  for( u32 i=0; i<m_AnimMax; i++ ){
    if( m_AnimArray[i].IsInitialized() == false ){
      return i;
    }
  }
  
  return LYTWKBASE_ANIMEIDX_ERROR;
}

//-----------------------------------------------------------------------------
/**
 * @brief   アニメーション情報生成
 *
 * @param   anmIdx      生成先アニメーションインデックス
 * @param   res         リソースデータ
 * @param   multi_id    リソース登録ID
 * @param   lytDataID   アニメーションリソースのデータID
 * @param   is_enable   アニメを無効状態で関連付ける場合 = false
 * @param   is_useHash  ハッシュを使用するか
 */
//-----------------------------------------------------------------------------
void LytWkBase::CreateAnime( u32 anmIdx, const LytRes* res, u32 multi_id, u32 lytDataID, bool is_enable, bool is_useHash )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE, "CSA" );

  // アロケータを指定の物に書き換え
  LYTWKBASE_CHANGE_USER_ALLOCATOR();
  
  // すでにデータがある場合　破棄処理
  if( m_AnimArray[anmIdx].IsInitialized() )
  {
    DeleteAnime( anmIdx );
  }

  // レイアウトのアロケータを使用してアニメーションTransformを生成
  nw::lyt::AnimTransform* animewk = m_LytSys->CreateAnimTransform<nw::lyt::AnimTransformBasic>();

  // 初期化
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_ANIME_INIT, "INIT_SHARE" );
  m_AnimArray[anmIdx].Initialize( m_LytSys, animewk, res, multi_id, lytDataID, is_useHash, m_LytSys );
  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_ANIME_INIT );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreateAnime Init %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_ANIME_INIT ) );
#endif // CLWKBASE_TICK_DUMP 

  // バインド
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_ANIME_BIND, "BIND" );
  m_AnimArray[anmIdx].Bind( m_LytSys, is_enable, is_useHash );
  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_ANIME_BIND );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreateAnime Bind %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_ANIME_BIND ) );
#endif // CLWKBASE_TICK_DUMP 

  // アロケーターを元に戻す
  LYTWKBASE_RETURN_ALLOCATOR();

  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreateAnime %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE ) );
#endif // CLWKBASE_TICK_DUMP 
}

//-----------------------------------------------------------------------------
/**
 * @brief   部品のアニメーション情報生成
 *
 * @param   parts       部品ペイン
 * @param   anmIdx      生成先アニメーションインデックス
 * @param   res         リソースデータ
 * @param   multi_id    リソース登録ID
 * @param   lytDataID   アニメーションリソースのデータID
 * @param   is_enable   アニメを無効状態で関連付ける場合 = false
 * @param   is_useHash  ハッシュを使用するか
 */
//-----------------------------------------------------------------------------
void LytWkBase::CreatePartsAnime( nw::lyt::Parts * parts, u32 anmIdx, const LytRes* res, u32 multi_id, u32 lytDataID, bool is_enable, bool is_useHash )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE, "CSA" );

  // アロケータを指定の物に書き換え
  LYTWKBASE_CHANGE_USER_ALLOCATOR();
  
  // すでにデータがある場合　破棄処理
  if( m_AnimArray[anmIdx].IsInitialized() ){
    // 破棄処理
    DeleteAnime( anmIdx );
  }

  nw::lyt::Layout * parts_layout = parts->GetLayout();
  GFL_ASSERT( m_LytSys != parts_layout );

  // レイアウトのアロケータを使用してアニメーションTransformを生成
  nw::lyt::AnimTransform * animewk = parts_layout->CreateAnimTransform<nw::lyt::AnimTransformBasic>();

  // 初期化
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_ANIME_INIT, "INIT_SHARE" );
  m_AnimArray[anmIdx].Initialize( parts_layout, animewk, res, multi_id, lytDataID, is_useHash, parts_layout );
  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_ANIME_INIT );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreatePartsAnime Init %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_ANIME_INIT ) );
#endif // CLWKBASE_TICK_DUMP 

  // バインド
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_ANIME_BIND, "BIND" );
  m_AnimArray[anmIdx].Bind( parts_layout, is_enable, is_useHash );
  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_ANIME_BIND );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreatePartsAnime Bind %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_ANIME_BIND ) );
#endif // CLWKBASE_TICK_DUMP 

  // アロケーターを元に戻す
  LYTWKBASE_RETURN_ALLOCATOR();

  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreatePartsAnime %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE ) );
#endif // CLWKBASE_TICK_DUMP 
}

//----------------------------------------------------------------------------
/**
 * @brief   アニメーション情報の破棄
 *
 * @param   anmIdx  破棄するアニメーションインデックス
 */
//-----------------------------------------------------------------------------
void LytWkBase::DeleteAnime( u32 anmIdx )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  LytWkAnime * anm = &m_AnimArray[anmIdx];
  
  if( anm->IsInitialized() )
  {
    // バインドの解除
    anm->UnBind();
    // レイアウトのアロケータを使用してアニメーションTransformを破棄
    anm->GetLayout()->DeleteAnimTransform( anm->GetAnimeWork() );
    // 破棄
    anm->Finalize();
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief    アニメーションインデックス最大値取得
 *
 *  @retval   アニメーションインデックス最大値
 */
//-----------------------------------------------------------------------------
u32 LytWkBase::GetAnimeMax( void ) const
{
  return m_AnimMax;
}

//----------------------------------------------------------------------------
/**
 *  @brief    アニメーション情報があるかチェック
 *
 * @param   anmIdx  アニメーションインデックス
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsAnime( u32 anmIdx ) const
{
  if( anmIdx < m_AnimMax ){
    return m_AnimArray[ anmIdx ].IsInitialized();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションの反映
 *
 *  @param  anmIdx          アニメーションインデックス
 *  @param  is_auto_unbind  one time アニメーションの場合、アニメーション完了後、unbindする。
 */
//-----------------------------------------------------------------------------
void LytWkBase::BindAnime( u32 anmIdx, bool is_auto_unbind )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  if( m_AnimArray[anmIdx].IsInitialized() )
  {
    m_AnimArray[anmIdx].SetEnable( true );
    m_AnimArray[anmIdx].SetAutoUnbind(is_auto_unbind);
    this->SetAnimeFrame( anmIdx, 0 );
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションの反映解除
 *
 *  @param  anmIdx          アニメーションインデックス
 */
//-----------------------------------------------------------------------------
void LytWkBase::UnBindAnime( u32 anmIdx )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  if( m_AnimArray[anmIdx].IsInitialized() )
  {
    m_AnimArray[anmIdx].SetEnable( false );
    this->SetAnimeFrame( anmIdx, 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションの反映解除
 */
//-----------------------------------------------------------------------------
void LytWkBase::UnBindAnimeAll( void )
{
  u32 i;
  
  for( i=0; i<m_AnimMax; ++i )
  {
    UnBindAnime(i);
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションがバインドされているかチェック
 *
 *  @param  anmIdx          アニメーションインデックス
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsBindAnime( u32 anmIdx ) const
{
  if( anmIdx >= m_AnimMax ){
    return false;
  }
  
  if( m_AnimArray[anmIdx].IsInitialized() )
  {
    if( m_AnimArray[anmIdx].IsEnable() )
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    オートアニメーション設定
 *
 *  @param  anmIdx       アニメーションインデックス
 *  @param  flag      ON/OFFフラグ
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetAutoAnime( u32 anmIdx, bool flag )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  if( m_AnimArray[anmIdx].IsInitialized() )
  {
    m_AnimArray[anmIdx].SetAutoAnime( flag );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief    オートアニメーションかチェック
 *
 *  @param  anmIdx          アニメーションインデックス
 *
 *  @retval true    オートアニメ
 *  @retval false   非オートアニメ
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsAutoAnime( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return false;
  }

  if( m_AnimArray[anmIdx].IsInitialized() )
  {
    return m_AnimArray[anmIdx].IsAutoAnime();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    オートアニメーション スピード設定　（全オートアニメ共通）
 *
 *  @param  speed     アニメーションスピード
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetAutoAnimeSpeed( f32 speed )
{
  // 全アニメーションのオートアニメーションスピードを設定
  for( u32 i=0; i<m_AnimMax; i++ )
  {
    if( m_AnimArray[i].IsInitialized() )
    {
      m_AnimArray[i].SetAutoAnimeSpeed( speed );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief    オートアニメーション スピード設定　（個別設定）
 *
 *  @param  anmIdx    アニメーションインデックス
 *  @param  speed     アニメーションスピード
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetAutoAnimeSpeed( u32 anmIdx, f32 speed )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  if( m_AnimArray[anmIdx].IsInitialized() )
  {
    m_AnimArray[anmIdx].SetAutoAnimeSpeed( speed );
  }
}


//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションフレーム設定
 *
 *  @param  anmIdx    アニメーションインデックス
 *  @param  frame     アニメーションフレーム
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetAnimeFrame( u32 anmIdx, f32 frame )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  if( m_AnimArray[anmIdx].IsInitialized() )
  {
    if( frame < 0.0f )
    {
      GFL_ASSERT( 0 );
      frame = 0.0f;
    }
    else if( frame > m_AnimArray[anmIdx].GetAnimeWork()->GetFrameMax() )
    {
      GFL_ASSERT( 0 );
      frame = m_AnimArray[anmIdx].GetAnimeWork()->GetFrameMax();
    }
    m_AnimArray[anmIdx].GetAnimeWork()->SetFrame( frame );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションフレーム　カウント
 *
 *  @param  anmIdx    アニメーションインデックス
 *  @param  frame     アニメーションフレーム
 *
 *  @retval true    アニメーション終了（LOOP設定のときは常にfalse）
 *  @retval false   アニメーション途中
 */
//-----------------------------------------------------------------------------
bool LytWkBase::AddAnimeFrame( u32 anmIdx, f32 frame )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return true;
  }

  if( m_AnimArray[anmIdx].IsInitialized() )
  {
    return m_AnimArray[anmIdx].AddAnimeFrame( frame );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションフレーム取得
 *
 *  @param  anmIdx    アニメーションインデックス
 */
//-----------------------------------------------------------------------------
f32 LytWkBase::GetAnimeFrame( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return 0.0f;
  }

  if( m_AnimArray[anmIdx].IsInitialized() )
  {
    return m_AnimArray[anmIdx].GetAnimeWork()->GetFrame();
  }
  return 0.0f;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーション最大フレーム取得
 *
 *  @param  anmIdx    アニメーションインデックス
 */
//-----------------------------------------------------------------------------
f32 LytWkBase::GetAnimeMaxFrame( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return 0;
  }

  if( m_AnimArray[anmIdx].IsInitialized() )
  {
    return m_AnimArray[anmIdx].GetAnimeWork()->GetFrameMax();
  }
  return 0;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    ループアニメーションかチェック
 *
 *  @param  anmIdx    アニメーションインデックス
 *
 *  @retval true    ループアニメーション
 *  @retval false   ループアニメーションじゃない
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsAnimeLoop( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return false;
  }

  if( m_AnimArray[anmIdx].IsInitialized() )
  {
    return m_AnimArray[anmIdx].GetAnimeWork()->IsLoopData();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションの完了チェック
 *
 *  @param  anmIdx    アニメーションインデックス
 *
 *  @retval true    完了
 *  @retval false   途中
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsAnimeEnd( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return true;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    return m_AnimArray[anmIdx].IsAnimeEnd();
  }
  return true;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  可視範囲内チェック
 *
 *  @param  camera  カメラワーク
 *
 *  @retval true    可視範囲内
 *  @retval false   可視範囲外
 */
//-----------------------------------------------------------------------------
b32 LytWkBase::IsCulling( const Camera& camera ) const
{
  if( !m_CullingFlag ){
    return true;
  }
  nw::ut::Rect rect = m_LytSys->GetLayoutRect();
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  gfl2::math::Matrix34 matrix_tmp = gfl2::math::ConvertNwMtx34ToGfMtx34( rootPane->GetGlobalMtx() );
  gfl2::math::Matrix44 screen_a = camera.GetProjectionMtx();
  gfl2::math::Matrix34 screen_b;
  gfl2::math::Matrix34 matrix;
  f32 w;
  
  gfl2::math::Vector3 check;
  gfl2::math::Vector3 ans;

  //MTX44->MTX34
  for( int i=0; i<3; ++i )
  {
    for( int j=0; j<4; ++j )
    {
      screen_b.SetElem( i, j, screen_a.GetElem( i, j ) );
    }
  }
#if 0
  screen_b._00 = screen_a._00;
  screen_b._10 = screen_a._10;
  screen_b._20 = screen_a._20;
  screen_b._01 = screen_a._01;
  screen_b._11 = screen_a._11;
  screen_b._21 = screen_a._21;
  screen_b._02 = screen_a._02;
  screen_b._12 = screen_a._12;
  screen_b._22 = screen_a._22;
  screen_b._03 = screen_a._03;
  screen_b._13 = screen_a._13;
  screen_b._23 = screen_a._23;
#endif


  // 9点チェック うち1つでも範囲内なら描画。
  for( u32 i = 0; i<9; ++i ){
    GetRectCheckPos( rect, i, &check );

    // スクリーン座標系まで持っていく。
    check = matrix_tmp.TransformCoord( check );

    // スクリーン行列への変換では、横・縦が置き換わる。
    ans.x = check.y;
    ans.y = check.x;
    ans.z = check.z;
    check = screen_b.TransformCoord( check );
#if 1
    w = (check.x * screen_a.GetElem(3, 0)) + (check.y * screen_a.GetElem(3, 1)) + (check.z * screen_a.GetElem(3, 2)) + screen_a.GetElem(3, 3);
#else
    w = (check.x * screen_a._30) + (check.y * screen_a._31) + (check.z * screen_a._32) + screen_a._33;
#endif
    ans.x /= w;
    ans.y /= w;

    if( (gfl2::math::Abs(ans.x) <= 1.0f) && (gfl2::math::Abs(ans.y) <= 1.0f) ){
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief   矩形位置情報を取得
 *
 * @param   rect   矩形情報
 * @param   id     取得ID
 * @param   p_pos  位置情報
 */
//-----------------------------------------------------------------------------
void LytWkBase::GetRectCheckPos( const nw::ut::Rect& rect, u32 id, gfl2::math::Vector3* p_pos )
{
  p_pos->z = 1.0f;
  
  switch( id ){
  case 0:
    p_pos->x = rect.left;
    p_pos->y = rect.top;
    break;

  case 1:
    p_pos->x = rect.right;
    p_pos->y = rect.top;
    break;

  case 2:
    p_pos->x = rect.left;
    p_pos->y = rect.bottom;
    break;

  case 3:
    p_pos->x = rect.right;
    p_pos->y = rect.bottom;
    break;
    
  case 4:
    p_pos->x = rect.left + ( rect.right - rect.left ) / 2.0f;
    p_pos->y = rect.top;
    break;

  case 5:
    p_pos->x = rect.left + ( rect.right - rect.left ) / 2.0f;
    p_pos->y = rect.bottom;
    break;

  case 6:
    p_pos->x = rect.left;
    p_pos->y = rect.top + ( rect.bottom - rect.top ) / 2.0f;
    break;

  case 7:
    p_pos->x = rect.right;
    p_pos->y = rect.top + ( rect.bottom - rect.top ) / 2.0f;
    break;

  case 8:
    p_pos->x = rect.left + ( rect.right - rect.left ) / 2.0f;
    p_pos->y = rect.top + ( rect.bottom - rect.top ) / 2.0f;
    break;

  default:
    break;
  }
}


//=============================================================================
/**
 * ペインテーブル:
 *    旧GFLIBの構造だと部品内ペインを持ちようがないので、
 *    親レイアウトのペインのみをテーブルに保持。
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief   ペインテーブル生成
 *
 * @param   m_heap      ヒープ
 * @param   isUseHash   ハッシュを使用するか
 */
//-----------------------------------------------------------------------------
void LytWkBase::createPaneTbl( gfl2::heap::HeapBase * p_heap, bool isUseHash )
{
  if( m_ppPaneTbl == NULL )
  {
    u32 pane_num = m_res->GetPaneNum( m_resMultiID );

    m_ppPaneTbl = static_cast<nw::lyt::Pane **>( GflHeapAllocMemory(p_heap,sizeof(nw::lyt::Pane *)*pane_num) );

    nw::lyt::Pane * rootPane = m_LytSys->GetRootPane();
    for( u32 i=0; i<pane_num; i++ )
    {
      m_ppPaneTbl[i] = rootPane->FindPaneByName( m_res->GetPaneName(i,m_resMultiID), true );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   ペインテーブル破棄
 */
//-----------------------------------------------------------------------------
void LytWkBase::DeletePaneTbl(void)
{
  GflHeapSafeFreeMemory( m_ppPaneTbl );
}

//-----------------------------------------------------------------------------
/**
 * @brief   ペインを取得
 *
 * @param   paneIndex     インデックス
 * @param   res_multi_id  リソース登録ID
 *
 * @retval  "NULL != ペイン"
 * @retval  "NULL = テーブルにない"
 */
//-----------------------------------------------------------------------------
nw::lyt::Pane * LytWkBase::getPaneTblPane( u32 paneIndex, u32 res_multi_id ) const
{
  // テーブルなし or 親レイアウトのリソース以外
  if( m_ppPaneTbl == NULL || res_multi_id != m_resMultiID )
  {
    return NULL;
  }
  
  if( m_res->GetPaneNum(res_multi_id) > paneIndex )
  {
    return m_ppPaneTbl[paneIndex];
  }

  GFL_ASSERT( 0 );
  return NULL;
}


//-----------------使用注意-----------
//-----------------------------------------------------------------------------
/**
 *  @brief  コアシステムの取得
 */
//-----------------------------------------------------------------------------
nw::lyt::Layout* LytWkBase::GetCore(void) const
{
  return m_LytSys;
}

//----------------------------------------------------------------------------
/**
 *  @brief    描画インフォの取得
 *
 *  @return 描画インフォ
 */
//-----------------------------------------------------------------------------
const nw::lyt::DrawInfo* LytWkBase::GetDrawInfo(void) const
{
  return &m_LytDrawInfo;
}

//----------------------------------------------------------------------------
/**
 *  @brief    DrawInfoに非表示のペインに対して行列計算を行うかどうかを設定する
 *
 *  @return    none
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetInvisiblePaneCalculateMtx( bool flg )
{
  m_LytDrawInfo.SetInvisiblePaneCalculateMtx( flg );
}


//----------------------------------------------------------------------------
/**
 *  @brief  描画する必要があるかチェック
 *
 *  @retval true    必要がある。
 *  @retval false   必要がない。
 */
//-----------------------------------------------------------------------------
b32 LytWkBase::IsDrawDisplayType( gfl2::lyt::DisplayType lod ) const
{
  if(lod == lyt::DISPLAY_UPPER_RIGHT) lod = lyt::DISPLAY_UPPER;  // nijiでは左目、右目の描画呼び出しが分かれており別々のlodで呼び出されるが、gflibレイアウトはどちらにも上画面の設定を用いる。
  GFL_ASSERT( lod < gfl2::lyt::DISPLAY_NORMAL_MAX ); //@check

  // 両面描画  // ここで言う両面とはSetDrawDispType関数より推測すると全画面(上(左)、下、右のこと)と思われる。
  if( m_DisplayType == gfl2::lyt::DISPLAY_BOTH_MAX ){
    return true;
  }

  if( lod == m_DisplayType ){
    return true;
  }
  return false;
}


//=============================================================================
/**
 *  Group操作
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief   Groupの取得
 *
 * @param   layout        レイアウト
 * @param   paneIndex     ペインインデックス
 * @param   res_multi_id  リソース登録ID
 *
 * @retval  Groupのインスタンス
 */
//-----------------------------------------------------------------------------
nw::lyt::Group * LytWkBase::GetGroup( nw::lyt::Layout * layout, u32 paneIndex, u32 res_multi_id ) const
{
  nw::lyt::GroupContainer * group_container = layout->GetGroupContainer();

  if( group_container == NULL )
  {
    GFL_ASSERT( 0 );
    return NULL;
  }
  
  const char * paneName = m_res->GetPaneName( paneIndex, res_multi_id );
  if( paneName == NULL )
  {
    return NULL;
  }

  return group_container->FindGroupByName( paneName );
}

//-----------------------------------------------------------------------------
/**
 * @brief   Groupの取得
 *
 * @param   paneIndex     ペインインデックス
 *
 * @retval  Groupのインスタンス
 */
//-----------------------------------------------------------------------------
nw::lyt::Group * LytWkBase::GetGroup( u32 paneIndex ) const
{
  return GetGroup( m_LytSys, paneIndex, m_resMultiID );
}

//-----------------------------------------------------------------------------
/**
 * @brief   Groupの取得 ( 部品ペイン版 )
 *
 * @param   parts         部品ペイン
 * @param   paneIndex     ペインインデックス
 * @param   res_multi_id  リソース登録ID
 *
 * @retval  Groupのインスタンス
 */
//-----------------------------------------------------------------------------
nw::lyt::Group * LytWkBase::GetGroup( nw::lyt::Parts * parts, u32 paneIndex, u32 res_multi_id ) const
{
  return GetGroup( parts->GetLayout(), paneIndex, res_multi_id );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    参照レイアウトリソース取得
 *
 *  @return   レイアウトリソース
 */
//-----------------------------------------------------------------------------
const LytRes* LytWkBase::GetLytRes(void) const
{
  return m_res;
}

//-----------------------------------------------------------------------------
/**
 * @brief   参照レイアウトのリソース登録IDを取得
 *
 * @return  リソース登録ID
 */
//-----------------------------------------------------------------------------
u32 LytWkBase::GetLytResMultiID(void) const
{
  return m_resMultiID;
}


//============================================================================
/**
 *  隠蔽メンバ関数
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  オートアニメーション　更新処理
 */
//-----------------------------------------------------------------------------
void LytWkBase::UpdateAutoAnime( void )
{
  u32 i;
  
  for( i=0; i<m_AnimMax; i++ )
  {
    if( m_AnimArray[i].IsInitialized() ){
      if( m_AnimArray[i].IsBindAnime() ){
        if( m_AnimArray[i].IsAutoAnime() ){
          if( m_AnimArray[i].AddAnimeFrame( m_AnimArray[i].GetAutoAnimeSpeed() ) ){
            // ループ終了　＆　オートアンバインドフラグが立っている
            if( m_AnimArray[i].IsAutoUnbind() ){
              // オートアンバインド処理
              this->UnBindAnime(i);
              m_AnimArray[i].SetAutoUnbind(false);
            }
            // オートアニメ終了
            m_AnimArray[i].SetAutoAnime( false );
          }
        }
      }
    }
  }
}


//! @name  描画中カウント(friendであるLytWkからしかアクセスしない)
//@{
//----------------------------------------------------------------------------
/**
 *  @brief  描画中か
 *  @return  trueのとき描画中(描画中はFinalizeで破棄してはならない)
 *  @note   m_drawingCountを見張っている
 */
//----------------------------------------------------------------------------
bool LytWkBase::IsDrawing(void) const
{
  return (m_drawingCount > 0);
}
//----------------------------------------------------------------------------
/**
 *  @brief  描画中カウントを増やす
 *  @note   m_drawingCountを増やす
 */ 
//----------------------------------------------------------------------------
void LytWkBase::IncreaseDrawingCount(void)
{
  ++m_drawingCount;
}
//----------------------------------------------------------------------------
/**
 *  @brief  描画中カウントを減らす
 *  @note   m_drawingCountを減らす
 */ 
//----------------------------------------------------------------------------
void LytWkBase::DecreaseDrawingCount(void)
{
  if(m_drawingCount > 0)
  {
    --m_drawingCount;
  }
  else  // if(m_drawingCount == 0)
  {
    GFL_ASSERT(0);  // カウントを減らせなくなっているのでおかしい。気付いてくれればケアなしでいい。
  }
}
//@}




//=============================================================================
/**
 *          LytWkAnime実装
*/
//=============================================================================
//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LytWkAnime::LytWkAnime(void)
  : m_Initialize( false )
  , m_AutoAnime( false )
  , m_BindFlag( false )
  , m_Enable( false )
  , m_IsAutoUnbind( false )
  , m_AnimTransform( NULL )
  , m_AutoAnimeSpeed( 1.0f )
  , m_layout( NULL )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LytWkAnime::~LytWkAnime()
{
  if( m_Initialize ){
    GFL_ASSERT(0);
    Finalize();
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   初期化
 *
 * @param   layout        レイアウト
 * @param   animTransform アニメクラス
 * @param   res           リソース
 * @param   multi_id      リソース登録ID
 * @param   lytDataID     リソースデータID
 * @param   isUsingHash   ハッシュを使用するか
 * @param   custom_lyt    カスタマイズNWで使用するレイアウト
 */
//-----------------------------------------------------------------------------
void LytWkAnime::Initialize(
  nw::lyt::Layout * layout,
  nw::lyt::AnimTransform * animTransform,
  const LytRes* res,
  u32 multi_id,
  u32 lytDataID,
  bool isUsingHash,
  nw::lyt::Layout * custom_lyt )
{
  m_AnimTransform = animTransform;

  // アニメーションリソースのアクセサ
  m_AnimRes.Set( res->GetAnimeResource(lytDataID,multi_id) );

  // もしグループ情報があるのであればグループ数分のアニメーションリンクができるように数を指定する。
  const u16 bindGroupNum = m_AnimRes.GetGroupNum();
  u16 animNum = 0;
  if(bindGroupNum == 0){
    animNum = m_AnimRes.GetResourceBlock()->animContNum; 
  }else{
    const nw::lyt::res::AnimationGroupRef *const groupRefs = m_AnimRes.GetGroupArray();
    for( int grpIdx=0; grpIdx<bindGroupNum; ++grpIdx )
    {
      // グループにあるペイン全てで必要になるアニメーションの個数を数えます。
      nw::lyt::Group *const pGroup = layout->GetGroupContainer()->FindGroupByName(groupRefs[grpIdx].GetName());
      if( pGroup == NULL )
      {
        // "Group not found
        GFL_ASSERT(0);
        continue;
      }

      animNum += m_AnimRes.CalcAnimationNum(pGroup, m_AnimRes.IsDescendingBind());
    }
  }

  // バインドするアニメーション数を明示的に指定してリソースをセットします。
  m_AnimTransform->SetResource( m_AnimRes.GetResourceBlock(), res->GetResourceAccessor(), animNum );
  
  m_Initialize  = true;
  m_BindFlag    = false;
  m_AutoAnime   = false;
  m_AutoAnimeSpeed = 1.0f;
  m_Enable      = false;
}

//-----------------------------------------------------------------------------
/**
 * @brief   破棄
 */
//-----------------------------------------------------------------------------
void LytWkAnime::Finalize( void )
{
  if( m_Initialize )
  {
    GFL_ASSERT( m_BindFlag == false );

    m_AnimTransform = NULL;
    m_Initialize = false;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   初期化済みチェック
 *
 * @retval  "true = 初期化済み"
 * @retval  "false = それ以外"
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsInitialized(void) const
{
  return m_Initialize;
}

//-----------------------------------------------------------------------------
/**
 * @brief  バインド処理
 *
 * @param   layout      レイアウト
 * @param   is_enable   アニメを無効状態で関連付ける場合 = false
 * @param   is_useHash  ハッシュを使用するか
 */
//-----------------------------------------------------------------------------
void LytWkAnime::Bind( nw::lyt::Layout * layout, bool is_enable, bool is_useHash )
{
  if( m_BindFlag )
  {
    GFL_ASSERT( 0 );
    return;
  }
  
  // グループの有無でバインドを変える
  // もしグループ情報があるのであればグループを使用してバインド
  const u16 bindGroupNum = m_AnimRes.GetGroupNum();
  if( bindGroupNum == 0 )
  {
    layout->GetRootPane()->BindAnimation( m_AnimTransform, true, is_enable );
  }
  else
  {
    const nw::lyt::res::AnimationGroupRef *const groupRefs = m_AnimRes.GetGroupArray();
    for ( int grpIdx = 0; grpIdx < bindGroupNum; ++grpIdx )
    {
      // グループにあるペイン全てで必要になるアニメーションの個数を数えます。
      nw::lyt::Group *const pGroup = layout->GetGroupContainer()->FindGroupByName(groupRefs[grpIdx].GetName());
      if (pGroup == NULL)
      {
        // "Group not found
        GFL_ASSERT(0);
        continue;
      }
      nw::lyt::BindAnimation(pGroup, m_AnimTransform, is_enable);
    }
  }

  m_BindFlag = true;
  m_Enable   = is_enable;
  m_layout   = layout;
}

//-----------------------------------------------------------------------------
/**
 * @brief   バインド解除
 */
//-----------------------------------------------------------------------------
void LytWkAnime::UnBind(void)
{
  if( m_BindFlag ){
    m_layout->UnbindAnimation( m_AnimTransform );
    m_BindFlag = false;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief  バインドされているかチェック
 *
 * @retval  "true = バインドされている"
 * @retval  "false = それ以外"
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsBindAnime(void) const
{
  return m_BindFlag;
}

//-----------------------------------------------------------------------------
/**
 * @brief   アニメの有効/無効設定
 *
 * @param   flag  true = 有効, false = 無効
 */
//-----------------------------------------------------------------------------
void LytWkAnime::SetEnable( bool flag )
{
  if( m_AnimTransform ){
    m_Enable = flag;
    m_AnimTransform->SetEnable( flag );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   アニメが有効かチェック
 *
 * @retval  "true = 有効"
 * @retval  "false = 無効"
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsEnable(void) const
{
  return m_Enable;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  アニメーションフレームを進める
 *
 *  @param  frame 進めるフレーム数
 *
 *  @retval true      アニメーション終了
 *  @retval false     アニメーション途中
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::AddAnimeFrame( f32 frame )
{
  if( m_AnimTransform )
  {
    if( m_Enable )
    {
      f32 frame_now = m_AnimTransform->GetFrame();
      f32 frame_max = m_AnimTransform->GetFrameMax();

      // ループアニメ以外
      if( m_AnimTransform->IsLoopData() == false )
      {
        // 終了フレームか
        if( ( frame > 0.0f && frame_now == frame_max ) ||
            ( frame < 0.0f && frame_now == 0.0f ) )
        {
          return true;
        }

        frame_now += frame;
        // 終了フレームを超えたら補正
        if( frame_now < 0.0f )
        {
          frame_now = 0.0f;
        }
        else if( frame_now > frame_max )
        {
          frame_now = frame_max;
        }
      }
      // ループアニメ
      else
      {
        frame_now += frame;
        // 終了フレームを超えたらループ
        if( frame_now <= 0.0f )
        {
          frame_now += frame_max;
        }
        else if( frame_now >= frame_max )
        {
          frame_now -= frame_max;
        }
      }

      m_AnimTransform->SetFrame( frame_now );
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  アニメーションフレームの終了をチェック
 *
 *  @retval true      アニメーション終了
 *  @retval false     アニメーション途中
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsAnimeEnd( void ) const
{
  if( m_AnimTransform ){
    // 有効か？
    if( m_Enable == false ){
      return true;
    }

    // オートアニメが有効か？
    if( IsAutoAnime() != false )
    {
      return false;
    }

    f32 frame_now = m_AnimTransform->GetFrame();
    f32 frame_max = m_AnimTransform->GetFrameMax();
    bool loop = m_AnimTransform->IsLoopData();

    if( (frame_now >= frame_max) && (!loop) ){
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  オートアニメーション設定
 *
 *  @param  flag  true  オートアニメON    false オートアニメ
 */
//-----------------------------------------------------------------------------
void LytWkAnime::SetAutoAnime( bool flag )
{
  m_AutoAnime = flag;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  オートアニメーション設定　取得
 *
 *  @retval true    オートアニメON
 *  @retval false   オートアニメOFF
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsAutoAnime( void ) const
{
  return m_AutoAnime;
} 

//-----------------------------------------------------------------------------
/**
 * @brief   アニメーションクラスを取得
 *
 * @return  アニメーションクラス
 */
//-----------------------------------------------------------------------------
nw::lyt::AnimTransform * LytWkAnime::GetAnimeWork(void) const
{
  return m_AnimTransform;
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトを取得
 *
 * @return  レイアウト
 */
//-----------------------------------------------------------------------------
nw::lyt::Layout * LytWkAnime::GetLayout(void) const
{
  return m_layout;
}


GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)
