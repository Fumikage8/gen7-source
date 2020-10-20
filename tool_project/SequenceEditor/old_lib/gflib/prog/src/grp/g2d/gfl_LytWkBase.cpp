//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_LytWkBase.cpp
 *	@brief  レイアウトワークベース
 *	@author	tomoya takahshi
 *	@date		2010.10.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>

#include <grp/g2d/gfl_LytWkBase.h>

#include <grp/g2d/gfl_LytSys.h>
#include <gfl_Math.h>


namespace gfl {
namespace grp {
namespace g2d {


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
  enum{
    CLACTWK_TICK_PRINT_ANIME_CREATE,
    CLACTWK_TICK_PRINT_GET_SHARE_INFO,
    CLACTWK_TICK_PRINT_GET_SHARE_INFO_IN_PANE_NUM,
    CLACTWK_TICK_PRINT_INIT_SHARE,
    CLACTWK_TICK_PRINT_BIND_SHARE,
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
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *    ローカル関数
 */
//-----------------------------------------------------------------------------
//ユーザーアロケーター設定
#define LYTWKBASE_CHANGE_USER_ALLOCATOR()  \
  nw::os::IAllocator* p_default_allocator_heap = nw::lyt::Layout::GetAllocator();\
  nw::os::IAllocator* p_default_allocator_device = nw::lyt::Layout::GetDeviceMemoryAllocator();\
  nw::lyt::Layout::SetAllocator( m_pUserAllocator );\
  nw::lyt::Layout::SetDeviceMemoryAllocator( m_pUserDeviceAllocator );

// デフォルトアロケーターに戻す
#define LYTWKBASE_RETURN_ALLOCATOR()  \
  nw::lyt::Layout::SetAllocator( p_default_allocator_heap );\
  nw::lyt::Layout::SetDeviceMemoryAllocator( p_default_allocator_device );

  
//----------------------------------------------------------------------------
/**
 *	@brief  バインド対象が、アニメーショングループの中に含まれているかチェック
 *	@retval true  含まれてる
 *	@retval false 含まれていない
 */
//-----------------------------------------------------------------------------
bool IsIncludeAnimationGroupRef(
    const nw::lyt::AnimationGroupRef *const  groupRefs,
    const char* pGroupName,
    u32 bindGroupNum
)
{
  for (u16 grpIdx = 0; grpIdx < bindGroupNum; ++grpIdx)
  {
    // 共有対象のペインがバインド指定のグループと同じかチェック
    if( gfl::std::StringCmp( pGroupName, groupRefs[grpIdx].GetName() ) == 0 ){
      return true;
    }
  }

  return false;
}


//=============================================================================
/**
 *					LytWkBase実装
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
LytWkBase::LytWkBase( void ) :
  m_AnimateOpt( nw::lyt::ANIMATEOPT_NOANIMATEINVISIBLE )
{
  GFL_DEBUG_TICK_SET_FLAG(CLACTWK, CLWKBASE_TICK_DUMP );
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
LytWkBase::~LytWkBase()
{
  if( m_Initialize ){
    Finalize();
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief    レイアウトワーク　初期化
 *
 *  @param  heap_allocator        アロケータ－
 *  @param  device_allocator      デバイスアロケータ－
 *	@param  res                   リソース情報
 *	@param  lytDataID             レイアウトデータID
 *	@param  animeNum              アニメーション最大登録数
 *	@param  globalData            共有情報
 *	@param  createPaneTbl         ペインテーブル生成
 *  @param  createPaneTblWithHash 高速検索用にハッシュキーを作る（消費メモリ増）
 */
//-----------------------------------------------------------------------------
void LytWkBase::Initialize( heap::NwAllocator* heap_allocator, heap::NwAllocator* device_allocator, const LytRes* res, u32 lytDataID, u32 animeNum,
                            LytWkBaseGlobalData* globalData, b32 createPaneTblFlag, b32 createPaneTblWithHash )
{
  bool result;
  heap::HeapBase* heap = heap_allocator->GetHeapBase();
  heap::HeapBase* device = device_allocator->GetHeapBase();
  void * addr;
  nw::lyt::ArcResourceAccessor* accessor;

  // 使用するアロケーター登録
  m_pUserAllocator = heap_allocator;
  m_pUserDeviceAllocator = device_allocator;

  // アロケータを指定の物に書き換え
  LYTWKBASE_CHANGE_USER_ALLOCATOR();

  m_globalData = globalData;

  m_LytSys = GFL_NEW(heap) nw::lyt::Layout();
  
  #ifdef GFL_USE_CUSTOM_NW4C
  result = m_LytSys->Build( res->GetResource(lytDataID), res->GetResourceAccessor(), createPaneTblWithHash );
  #else
  result = m_LytSys->Build( res->GetResource(lytDataID), res->GetResourceAccessor() );
  #endif
  GFL_ASSERT_STOP(result);

  m_LytDrawInfo.SetGraphicsResource( m_globalData->p_globalResource );

  if(animeNum>0){
    m_AnimArray = GFL_NEW_ARRAY(heap) LytWkAnime[animeNum];
  }else{
    m_AnimArray = NULL;
  }
  m_AnimMax   = animeNum;
  m_res       = res;

  // オート描画ON
  m_AutoDrawFlag = true;

  // GlobalEnv使用ON
  m_useGlobalEnv = true;

  // ヒットチェックが可能かどうか
  m_hitCheckOk = false;

  // 初期化完了
  m_Initialize = true;

  // カリングON
  m_CullingFlag = true;

  // 初期値は両画面表示
  m_DisplayType = DISPLAY_BOTH_MAX;

  // 初期値ではペインテーブルはNULL
  initializePaneTbl();
  // ペインテーブル生成
  if(createPaneTblFlag){ createPaneTbl(heap,createPaneTblWithHash); }

  // アロケータを元に戻す
  LYTWKBASE_RETURN_ALLOCATOR();
  
}


//----------------------------------------------------------------------------
/**
 *	@brief    レイアウトワーク　破棄
 */
//-----------------------------------------------------------------------------
void LytWkBase::Finalize( void )
{
  int i;
  GFL_ASSERT_STOP(m_LytSys);

  // ペインテーブル破棄
  finalizePaneTbl();

  // アニメーション破棄処理
  for( i=0; i<m_AnimMax; i++ ){
    DeleteAnime(i);
  }
  
  if(m_AnimArray){
    GFL_DELETE_ARRAY m_AnimArray;
  }
  m_AnimArray = NULL;
  m_AnimMax   = 0;


  GFL_SAFE_DELETE( m_LytSys );

  m_pUserDeviceAllocator = NULL;
  m_pUserAllocator = NULL;

  // 破棄完了
  m_Initialize = false;
}

  
//----------------------------------------------------------------------------
/**
 *	@brief    レイアウトワーク　初期化済みチェック
 *
 *	@retval true    初期化完了
 *	@retval false   初期化前
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsInitialized(void) const
{
  return m_Initialize;
}


//----------------------------------------------------------------------------
/**
 *	@brief    レイアウトワーク　更新
 */
//-----------------------------------------------------------------------------
void LytWkBase::Update( void )
{
  // オートアニメーション管理
  UpdateAutoAnime();
  m_LytSys->Animate( m_AnimateOpt );
}

//----------------------------------------------------------------------------
/**
 *	@brief    レイアウトワーク　描画
 *
 *	@param  lod           描画面
 *	@param  cameraType    カメラタイプ
 *	@param  renderTarget  レンダーターゲット
 */
//-----------------------------------------------------------------------------
void LytWkBase::Draw( DisplayType lod, Env::CameraType cameraType, const RenderTarget& renderTarget )
{
  const gfl::grp::g2d::Env* useEnv;

  GFL_ASSERT( lod < DISPLAY_NORMAL_MAX ); //@check

  if( !this->IsDrawDisplayType( lod ) ){
    return ;
  }

  // 使用する環境設定を取得
  if( m_useGlobalEnv ){
    useEnv = &m_globalData->env[lod];
  }else{
    useEnv = m_UserEnv[lod];
  }

  const Camera& camera = useEnv->GetCamera( cameraType );

  // カメラ設定
  {
    
    m_LytDrawInfo.SetProjectionMtx( camera.GetProjectionMtx() );
    m_LytDrawInfo.SetViewMtx( camera.GetViewMtx() );

    if( lod == DISPLAY_UPPER ){
      const Parallax& parallax = useEnv->GetParallax();
      if( cameraType == Env::CAMERA_NORMAL ){
        m_globalData->p_drawer->SetParallax( -parallax.parallax_half, parallax.dLevel, parallax.addDist );
      }else{
        m_globalData->p_drawer->SetParallax( parallax.parallax_half, parallax.dLevel, parallax.addDist );
      }
    }
    else{
      m_globalData->p_drawer->SetParallax( 0.0f, 0.0f, 0.0f );
    }
  }

  // 上画面と下画面で１回でOK？
  m_LytSys->CalculateMtx( m_LytDrawInfo );

  // 描画範囲内チェック
  if( IsCulling( camera ) ){

    // 描画環境の情報をコマンド設定
    useEnv->Add3DCommand( renderTarget );
    
    m_globalData->p_drawer->DrawBegin( m_LytDrawInfo );
    m_globalData->p_drawer->Draw( m_LytSys,  m_LytDrawInfo );
    m_globalData->p_drawer->DrawEnd( m_LytDrawInfo );

    // 一度描画後はヒットチェック可能
    m_hitCheckOk = true;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトワーク　グループIDの設定
 *
 *	@param	groupID   グループID
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetWkGroupID( u16 groupID )
{
  m_WkGroupID = groupID;
}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトワーク  グループIDの取得
 *
 *	@return グループID
 */
//-----------------------------------------------------------------------------
u16 LytWkBase::GetWkGroupID( void ) const
{
  return m_WkGroupID;
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
void LytWkBase::SetUserEnv( const gfl::grp::g2d::Env* upperEnv, const gfl::grp::g2d::Env* lowerEnv )
{
  m_UserEnv[DISPLAY_UPPER] = upperEnv;
  m_UserEnv[DISPLAY_LOWER] = lowerEnv;
  m_useGlobalEnv = false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  グローバルENVを使用するように設定
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetGlobalEnv( void )
{
  if( m_useGlobalEnv == false ){
    m_useGlobalEnv = true;
    m_UserEnv[DISPLAY_UPPER] = NULL;
    m_UserEnv[DISPLAY_LOWER] = NULL;
  }
}




//----------------------------------------------------------------------------
/**
 *	@brief  リンクリスト　1つ前のワーク設定<LytWk型>
 *
 *	@param	１つ前のワーク
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetPrev( LytWk* prev )
{
  m_prev = prev;
}

//----------------------------------------------------------------------------
/**
 *	@brief  リンクリスト　1つ前のワーク取得<LytWk型>
 *
 *	@retval	１つ前のワークreinterpret_cast<LytWk*>として使用してください。
 */
//-----------------------------------------------------------------------------
LytWk* LytWkBase::GetPrev( void ) const
{
  return m_prev;
}

//----------------------------------------------------------------------------
/**
 *	@brief  リンクリスト　1つ次のワーク設定<LytWk型>
 *
 *	@param	１つ次のワーク
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetNext( LytWk* next )
{
  m_next = next;
}

//----------------------------------------------------------------------------
/**
 *	@brief  リンクリスト　1つ次のワーク取得<LytWk型>
 *
 *	@retval	１つ次のワークreinterpret_cast<LytWk*>として使用してください。
 */
//-----------------------------------------------------------------------------
LytWk* LytWkBase::GetNext( void ) const
{
  return m_next;
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
nw::lyt::Pane* LytWkBase::GetPane( u32 paneIndex ) const
{
  nw::lyt::Pane* p_pane;

  // まずは、ペインテーブルから取得
  p_pane = getPaneTblPane( paneIndex );
  if(p_pane){ return p_pane; }
  
  // 無ければ、下から、
  const char * paneName = m_res->GetPaneName(paneIndex);
  if( paneName )
  {
    // 4文字以上のレイアウトがあるのでここは正規の名前での検索必須
    p_pane = m_LytSys->GetRootPane()->FindPaneByName( paneName, true );

    if( m_ppPaneTbl != NULL ){
      m_ppPaneTbl[paneIndex] = p_pane;
    }
  }
  return p_pane;
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
nw::lyt::Picture* LytWkBase::GetPicturePane( u32 paneIndex ) const
{
  nw::lyt::Pane* pane;
  nw::lyt::Picture* picture;

  pane = GetPane(paneIndex);
  if( pane ){
    picture = nw::ut::DynamicCast<nw::lyt::Picture*>(pane);
    return picture;  
  }
  return NULL;
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
nw::lyt::TextBox* LytWkBase::GetTextBoxPane( u32 paneIndex ) const
{
  nw::lyt::Pane* pane;
  nw::lyt::TextBox* textBox;

  pane = GetPane(paneIndex);
  if( pane ){
    textBox = nw::ut::DynamicCast<nw::lyt::TextBox*>(pane);
    return textBox;  
  }
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndexのテキストボックスペイン　ストリングバッファの確保
 *
 *	@param	paneIndex
 */
//-----------------------------------------------------------------------------
void LytWkBase::AllocTextBoxPaneStringBuffer( u16 paneIndex, u16 minLen ) const
{
  nw::lyt::TextBox* pane = this->GetTextBoxPane( paneIndex );
  
  if( pane ){ // @fix KW[7]

    // アロケータを指定の物に書き換え
    LYTWKBASE_CHANGE_USER_ALLOCATOR();
    
    pane->AllocStringBuffer( minLen );

    // アロケータを元に戻す
    LYTWKBASE_RETURN_ALLOCATOR();
  }
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
nw::lyt::Window* LytWkBase::GetWindowPane( u32 paneIndex ) const
{
  nw::lyt::Pane* pane;
  nw::lyt::Window* window;

  pane = GetPane(paneIndex);
  if( pane ){
    window = nw::ut::DynamicCast<nw::lyt::Window*>(pane);
    return window;  
  }
  return NULL;
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
nw::lyt::Bounding* LytWkBase::GetBoundingPane( u32 paneIndex ) const
{
  nw::lyt::Pane* pane;
  nw::lyt::Bounding* bounding;

  pane = GetPane(paneIndex);
  if( pane ){
    bounding = nw::ut::DynamicCast<nw::lyt::Bounding*>(pane);
    return bounding;  
  }
  return NULL;
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
nw::lyt::Material* LytWkBase::GetMaterial( u32 paneIndex ) const
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  const char * paneName = m_res->GetPaneName(paneIndex);
  if( paneName ){
    return rootPane->FindMaterialByName( paneName, true );
  }
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ヒットチェックが可能か取得
 *
 *	@retval true    可能
 *	@retval false   不可能
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsHitCheckOK( void ) const
{
  return m_hitCheckOk;
}


//============================================================================
/**
 *	基本操作
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief    オート描画のON/OFF設定
 *  
 *	@param	  flag    フラグ  true:ON   false:OFF
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetAutoDrawFlag( bool flag )
{
  m_AutoDrawFlag = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief    オート描画のON/OFF 取得
 *  
 *	@retval true    ON
 *	@retval false   OFF
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsAutoDraw( void ) const
{
  return m_AutoDrawFlag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  処理優先順位の設定
 *
 *	@param	pri   優先順位（ 0 > 0xffff )
 *
 *	UpdateAllとDrawAllを行う際の優先順位です。
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetPriority( u16 pri )
{
  m_Priority = pri;
}


//----------------------------------------------------------------------------
/**
 *	@brief  処理優先順位の取得
 *
 *	@retval	優先順位（ 0 > 0xffff )
 *
 *	UpdateAllとDrawAllを行う際の優先順位です。
 */
//-----------------------------------------------------------------------------
u16 LytWkBase::GetPriority( void ) const
{
  return m_Priority;
}





//----------------------------------------------------------------------------
/**
 *	@brief  座標の設定
 *
 *	@param	pos
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetTranslate( const gfl::math::VEC3& pos )
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  rootPane->SetTranslate( pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  座標の取得
 *
 *	@param	p_pos 座標格納先
 */
//-----------------------------------------------------------------------------
void LytWkBase::GetTranslate( gfl::math::VEC3* p_pos ) const
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  *p_pos = rootPane->GetTranslate();
}


//----------------------------------------------------------------------------
/**
 *	@brief  拡大の設定
 *
 *	@param	scale   スケーリング値
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetScale( const gfl::math::VEC2& scale )
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  rootPane->SetScale( scale );
}

//----------------------------------------------------------------------------
/**
 *	@brief  拡大の取得
 *
 *	@param p_scale スケーリング値格納先
 */
//-----------------------------------------------------------------------------
void LytWkBase::GetScale( gfl::math::VEC2* p_scale ) const
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  *p_scale = rootPane->GetScale();
}

//----------------------------------------------------------------------------
/**
 *	@brief  回転の設定
 *
 *	@param	rotate   各軸の回転値
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetRotate( const gfl::math::VEC3& rotate )
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  rootPane->SetRotate( rotate );
}

//----------------------------------------------------------------------------
/**
 *	@brief  回転の取得
 *
 *	@param	p_rotate   各軸の回転値
 */
//-----------------------------------------------------------------------------
void LytWkBase::GetRotate( gfl::math::VEC3* p_rotate ) const
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  *p_rotate = rootPane->GetRotate();
}

//----------------------------------------------------------------------------
/**
 *	@brief  カリングフラグの設定
 *
 *	@param	flag  フラグ
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetCullingFlag( bool flag )
{
  m_CullingFlag = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  カリングフラグの取得
 *
 *	@retval	true  カリング処理ON
 *	@retval false カリング処理OFF
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsCullingFlag( void ) const
{
  return m_CullingFlag;
}


//----------------------------------------------------------------------------
/**
 *	@brief  描画するディスプレイタイプを設定
 *
 *	@param	lod   描画するディスプレイタイプ
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetDrawDispType( DisplayType lod )
{
  if( (lod == DISPLAY_UPPER) || (lod == DISPLAY_LOWER) || (lod == DISPLAY_BOTH_MAX) ){
    m_DisplayType = lod;
  }
  else{
    GFL_ASSERT_MSG( 0, "DisplayType Error" );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  描画するディスプレイタイプの取得
 *
 *	@return 描画するディスプレイタイプ
 */
//-----------------------------------------------------------------------------
DisplayType LytWkBase::GetDrawDispType( void ) const
{
  return m_DisplayType;
}


//============================================================================
/**
 *	アニメーション関連
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  空いているアニメーションインデックスを取得
 *	
 *	@retval 空いているアニメーションインデックス
 *	@retval LYTWKBASE_ANIMEIDX_ERROR    空いてない
 */
//-----------------------------------------------------------------------------
u32 LytWkBase::GetClearAnimeIndex( void ) const
{
  for( int i=0; i<m_AnimMax; i++ ){
    if( m_AnimArray[i].IsInitialized() == false ){
      return i;
    }
  }
  
  return LYTWKBASE_ANIMEIDX_ERROR;
}

//----------------------------------------------------------------------------
/**
 *	@brief    アニメーション情報の生成
 *
 *  @param  allocator     ヒープアロケータ
 *  @param  anmaIdx       生成先アニメーションインデックス
 *	@param  res           リソース情報
 *	@param  lytDataID     アニメーションリソースのデータID
 *	@param  is_bind       バインド状態で設定するか
 */
//-----------------------------------------------------------------------------
void LytWkBase::CreateAnime( u32 anmIdx, const LytRes* res, u32 lytDataID, bool is_bind, bool is_useHash )
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

  // レイアウトのアロケータを使用してアニメーションTransformを生成
  nw::lyt::AnimTransform* animewk = m_LytSys->CreateAnimTransform();

  // 初期化
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_INIT_SHARE, "INIT_SHARE" );
  m_AnimArray[anmIdx].Initialize( m_LytSys, animewk, res, lytDataID, is_useHash, m_LytSys );
  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_INIT_SHARE );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreateAnime Init %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_INIT_SHARE ) );
#endif // CLWKBASE_TICK_DUMP 

  // バインド
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_BIND_SHARE, "BIND" );
  m_AnimArray[anmIdx].Bind( m_LytSys, is_bind, is_useHash );
  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_BIND_SHARE );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreateAnime Bind %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_BIND_SHARE ) );
#endif // CLWKBASE_TICK_DUMP 

  // アロケーターを元に戻す
  LYTWKBASE_RETURN_ALLOCATOR();

  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreateAnime %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE ) );
#endif // CLWKBASE_TICK_DUMP 
}
  
//----------------------------------------------------------------------------
/**
 *	@brief    共有アニメーションの生成
 *
 *	@param	  anmStartIdx 生成開始アニメーションインデックス
 *	@param    res         リソース情報
 *	@param    lytDataID   アニメーションリソースのデータID
 *	@param    maxNum      登録最大数（０であれば、すべて登録）
 *	@param    maxNum      登録最大数（0だとすべて登録）
 *	@param    groupIndex  登録グループインデックス（LYTSYS_PANEINDEX_ERRORだとすべて登録）
 *	@param    paneIndex   登録グループ内ペインインデックス（LYTSYS_PANEINDEX_ERRORだとすべて登録）
 *
 *	@return 登録数
 */
//-----------------------------------------------------------------------------
u32 LytWkBase::CreateShareAnime( u32 anmStartIdx, const LytRes* res, u32 lytDataID, u32 maxNum, u32 groupIndex, u32 paneIndex )
{
  u32 shareGroup;
  u32 total_create = 0;
  nw::lyt::Pane* pSrcPane = NULL;
  nw::lyt::Group* pGroup = NULL;
  const char * pGroupName = NULL;
  bool is_pane_end = false;

  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE, "CSA" );

  // シェアペイン数取得
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_GET_SHARE_INFO, "SI" );
  void * p_animRes = res->GetAnimeResource( lytDataID );
  nw::lyt::AnimResource animRes( p_animRes );
  shareGroup = animRes.GetAnimationShareInfoNum();
  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_GET_SHARE_INFO );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "ResShareInfoNum %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_GET_SHARE_INFO ) );
#endif 


  for(int i=0; i<shareGroup; i++){

    // シェアグループ内のペイン数取得
    GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_GET_SHARE_INFO_IN_PANE_NUM, "SIIP" );
    bool isAnimeOk = LytWkAnime::GetResShareInfoInPaneNum( m_LytSys, animRes, res, i, &pSrcPane, &pGroup, &pGroupName, groupIndex );
    GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_GET_SHARE_INFO_IN_PANE_NUM );
#if CLWKBASE_TICK_DUMP 
    GFL_PRINT( "GetResShareInfoInPaneNum %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_GET_SHARE_INFO_IN_PANE_NUM ) );
#endif // CLWKBASE_TICK_DUMP 


    if(isAnimeOk)
    {
      is_pane_end = false;

      nw::lyt::PaneLinkList& paneList = pGroup->GetPaneList();

      for (nw::lyt::PaneLinkList::Iterator it = paneList.GetBeginIter(); it != paneList.GetEndIter(); ++it)
      {
        // GroupIndexとPaneIndexの指定があるか？
        if( (groupIndex != LYTSYS_PANEINDEX_ERROR) && (paneIndex != LYTSYS_PANEINDEX_ERROR) ){
          if( gfl::std::StringCmp( it->target->GetName(), res->GetPaneName( paneIndex ) ) != 0 ){
            continue;
          }
          is_pane_end = true;
        }

        // オーバーチェック
        if( m_AnimMax > anmStartIdx + total_create ){

          // 連続領域である必要がある。
          if( m_AnimArray[anmStartIdx + total_create].IsInitialized() ){
            GFL_ASSERT(0);
            return total_create;
          }

          // アロケータを指定の物に書き換え
          LYTWKBASE_CHANGE_USER_ALLOCATOR();

          
          // 生成
          GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_INIT_SHARE, "IS" );
          bool result = m_AnimArray[ anmStartIdx + total_create ].InitializeShare( m_LytSys, res, p_animRes, i, it, pSrcPane, pGroup );
          GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_INIT_SHARE );
#if CLWKBASE_TICK_DUMP 
          GFL_PRINT( "InitializeShare %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_INIT_SHARE ) );
#endif // CLWKBASE_TICK_DUMP 

          if( result ){
            // バインド
            GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_BIND_SHARE, "IS" );
            m_AnimArray[ anmStartIdx + total_create ].BindShare( m_LytSys, res, pSrcPane );
            GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_BIND_SHARE );
#if CLWKBASE_TICK_DUMP 
            GFL_PRINT( "BindShare %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_BIND_SHARE ) );
#endif // CLWKBASE_TICK_DUMP 
            
            total_create ++;

            // 生成数が最大になったら終わり
            if( (maxNum > 0) && (total_create >= maxNum) ){
              // アロケーターを元に戻す。
              LYTWKBASE_RETURN_ALLOCATOR();

              GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE );
#if CLWKBASE_TICK_DUMP 
              GFL_PRINT( "CreateShareAnime %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE ) );
#endif 

              return total_create;
            }
          }
          // アロケーターを元に戻す。
          LYTWKBASE_RETURN_ALLOCATOR();
        }else{
          // ワークオーバー
          GFL_ASSERT( 0 );
          return total_create;
        }

        // 指定ペイン　終了
        if( is_pane_end ){
          break;
        }
      }
    }
  }

  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreateShareAnime %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE ) );
#endif 

  return total_create;
}

//----------------------------------------------------------------------------
/**
 *	@brief    アニメーション情報の破棄
 *
 *  @param  anmaIdx       破棄するアニメーションインデックス
 */
//-----------------------------------------------------------------------------
void LytWkBase::DeleteAnime( u32 anmIdx )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }
  
  if( m_AnimArray[anmIdx].IsInitialized() ){
    if( m_AnimArray[anmIdx].IsShareAnime() == false ){
      // バインドの解除
      m_AnimArray[anmIdx].UnBind( m_LytSys );
      // レイアウトのアロケータを使用してアニメーションTransformを破棄
      m_LytSys->DeleteAnimTransform( m_AnimArray[anmIdx].GetAnimeWork() );
    }else{
      // バインドの解除
      m_AnimArray[anmIdx].UnBindShare( m_LytSys );
    }

    // 破棄
    m_AnimArray[anmIdx].Finalize();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    アニメーションインデックス最大値取得
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
 *	@brief    アニメーション情報があるかチェック
 *
 *  @param  anmaIdx       アニメーションインデックス
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
 *  @param  anmIdx    アニメーションIDX
 *  @param  is_auto_unbind   one time アニメーションの場合、アニメーション完了後、unbindする。
 */
//-----------------------------------------------------------------------------
void LytWkBase::BindAnime( u32 anmIdx, bool is_auto_unbind )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    m_AnimArray[anmIdx].SetEnable( m_LytSys, true );
    m_AnimArray[anmIdx].SetAutoUnbind(is_auto_unbind);
    this->SetAnimeFrame( anmIdx, 0 );
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションの反映解除
 *
 *  @param  anmIdx    登録先IDX
 */
//-----------------------------------------------------------------------------
void LytWkBase::UnBindAnime( u32 anmIdx )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    m_AnimArray[anmIdx].SetEnable( m_LytSys, false );
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
  
  for( i=0; i<m_AnimMax; ++i ){
    UnBindAnime(i);
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションがバインドされているかチェック
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsBindAnime( u32 anmIdx ) const
{
  if( anmIdx >= m_AnimMax ){
    return false;
  }
  
  if( m_AnimArray[anmIdx].IsInitialized() ){
    if( m_AnimArray[anmIdx].IsEnable() ){
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    オートアニメーション設定
 *
 *  @param  anmIdx    アニメーションIDX
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

  if( m_AnimArray[anmIdx].IsInitialized() ){
    m_AnimArray[anmIdx].SetAutoAnime( flag );
  }
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
bool LytWkBase::IsAutoAnime( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return false;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
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
  for( int i=0; i<m_AnimMax; i++ ){
    if( m_AnimArray[i].IsInitialized() ){
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

  if( m_AnimArray[anmIdx].IsInitialized() ){
    m_AnimArray[anmIdx].SetAutoAnimeSpeed( speed );
  }
}


//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションフレーム設定
 *
 *  @param  anmIdx    アニメーションIDX
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

  if( m_AnimArray[anmIdx].IsInitialized() ){
    m_AnimArray[anmIdx].GetAnimeWork()->SetFrame( frame );
  }
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
bool LytWkBase::AddAnimeFrame( u32 anmIdx, f32 frame )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return true;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    return m_AnimArray[anmIdx].AddAnimeFrame( frame );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーションフレーム取得
 *
 *  @param  anmIdx    アニメーションIDX
 */
//-----------------------------------------------------------------------------
f32 LytWkBase::GetAnimeFrame( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return 0.0f;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    return m_AnimArray[anmIdx].GetAnimeWork()->GetFrame();
  }
  return 0.0f;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    アニメーション最大フレーム取得
 *
 *  @param  anmIdx    アニメーションIDX
 */
//-----------------------------------------------------------------------------
f32 LytWkBase::GetAnimeMaxFrame( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return 0;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    return m_AnimArray[anmIdx].GetAnimeWork()->GetFrameMax();
  }
  return 0;
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
bool LytWkBase::IsAnimeLoop( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return false;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    return m_AnimArray[anmIdx].GetAnimeWork()->IsLoopData();
  }
  return false;
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



//----------------------------------------------------------------------------
/**
 *	@brief  非表示ペインのアニメーション更新を省略するフラグ
 *	（デフォルトON）
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetVisiblePaneNotAnimeOption( bool flag )
{
  if( flag ){
    m_AnimateOpt = nw::lyt::ANIMATEOPT_NOANIMATEINVISIBLE;
  }
  else{
    m_AnimateOpt = static_cast<nw::lyt::AnimateOpt>(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  非表示ペインのアニメーション更新を省略するフラグチェック
 *	@retval true  有効
 *	@retval false 無効
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsVisiblePaneNotAnimeOption( void ) const
{
  if( m_AnimateOpt == 0 ){
    return false;
  }
  return true;
}


//----------------------------------------------------------------------------
/**
 *	@brief  可視範囲内チェック
 *
 *	@param	camera  カメラワーク
 *
 *	@retval true    可視範囲内
 *	@retval false   可視範囲外
 */
//-----------------------------------------------------------------------------
b32 LytWkBase::IsCulling( const Camera& camera ) const
{
  if( !m_CullingFlag ){
    return true;
  }
  
  nw::ut::Rect rect = m_LytSys->GetLayoutRect();
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  gfl::math::MTX34 matrix_tmp = rootPane->GetGlobalMtx();
  gfl::math::MTX44 screen_a = camera.GetProjectionMtx();
  gfl::math::MTX34 screen_b;
  gfl::math::MTX34 matrix;
  f32 w;

  
  gfl::math::VEC3 check;
  gfl::math::VEC3 ans;

  //MTX44->MTX34
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


  // 9点チェック うち1つでも範囲内なら描画。
  for( u32 i = 0; i<9; ++i ){
    GetRectCheckPos( rect, i, &check );

    // スクリーン座標系まで持っていく。
    gfl::math::VEC3Transform(&check, &matrix_tmp, &check);

    // スクリーン行列への変換では、横・縦が置き換わる。
    ans.x = check.y;
    ans.y = check.x;
    ans.z = check.z;
    gfl::math::VEC3Transform(&ans, &screen_b, &check);
    w = (check.x * screen_a._30) + (check.y * screen_a._31) + (check.z * screen_a._32) + screen_a._33;
    ans.x /= w;
    ans.y /= w;

    if( (gfl::math::Abs(ans.x) <= 1.0f) && (gfl::math::Abs(ans.y) <= 1.0f) ){
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  矩形位置情報を取得
 *
 *	@param	rect    矩形情報
 *	@param	p_pos   位置情報
 */
//-----------------------------------------------------------------------------
void LytWkBase::GetRectCheckPos( const nw::ut::Rect& rect, u32 idx, gfl::math::VEC3* p_pos )
{
  p_pos->z = 1.0f;
  
  switch( idx ){
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
    p_pos->x = rect.left + ((rect.right - rect.left)/2.0f);
    p_pos->y = rect.top;
    break;

  case 5:
    p_pos->x = rect.left + ((rect.right - rect.left)/2.0f);
    p_pos->y = rect.bottom;
    break;

  case 6:
    p_pos->x = rect.left;
    p_pos->y = rect.top + ((rect.bottom - rect.top)/2.0f);
    break;

  case 7:
    p_pos->x = rect.right;
    p_pos->y = rect.top + ((rect.bottom - rect.top)/2.0f);
    break;

  case 8:
    p_pos->x = rect.left + ((rect.right - rect.left)/2.0f);
    p_pos->y = rect.top + ((rect.bottom - rect.top)/2.0f);
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ペインテーブル初期化
 */
//-----------------------------------------------------------------------------
void LytWkBase::initializePaneTbl( void )
{
  m_ppPaneTbl = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ペインテーブル破棄
 */
//-----------------------------------------------------------------------------
void LytWkBase::finalizePaneTbl( void )
{
  GflHeapSafeFreeMemory(m_ppPaneTbl);
}

//----------------------------------------------------------------------------
/**
 *	@brief  ペインテーブル生成
 */
//-----------------------------------------------------------------------------
void LytWkBase::createPaneTbl( gfl::heap::HeapBase* p_heap, bool isUseHash )
{
  if( m_ppPaneTbl == NULL ){
    m_ppPaneTbl = static_cast<nw::lyt::Pane**>(GflHeapAllocMemory( p_heap, sizeof(nw::lyt::Pane*) * m_res->GetPaneNum() ));

    nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();

    //@suzuki------------------
    u32 paneNum = m_res->GetPaneNum();

    #ifdef GFL_USE_CUSTOM_NW4C
    if( isUseHash )
    {
      for( u32 i=0; i<paneNum; i++ ){
        m_ppPaneTbl[i] = m_LytSys->FindPaneByNameWithHash( m_res->GetPaneName(i) );
      }
    }
    else
    {
      for( u32 i=0; i<paneNum; i++ ){
        m_ppPaneTbl[i] = rootPane->FindPaneBySimpleName( m_res->GetPaneName(i), true );
      }
    }
    #else
    {
      for( u32 i=0; i<paneNum; i++ ){
        m_ppPaneTbl[i] = rootPane->FindPaneByName( m_res->GetPaneName(i), true );
      }
    }
    #endif


    //@suzuki------------------
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ペインを取得
 *
 *	@param	paneIndex   インデックス
 *
 *	@return ペイン
 */
//-----------------------------------------------------------------------------
nw::lyt::Pane* LytWkBase::getPaneTblPane( u32 paneIndex ) const
{
  if( m_ppPaneTbl == NULL ) {
    return NULL;
  }
  
  if( m_res->GetPaneNum() > paneIndex ){
    return m_ppPaneTbl[ paneIndex ];
  }
  GFL_ASSERT( paneIndex < m_res->GetPaneNum() ); //@check

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
 *	@brief    描画インフォの取得
 *
 *	@return 描画インフォ
 */
//-----------------------------------------------------------------------------
const nw::lyt::DrawInfo* LytWkBase::GetDrawInfo(void) const
{
  return &m_LytDrawInfo;
}

//----------------------------------------------------------------------------
/**
 *	@brief    DrawInfoに非表示のペインに対して行列計算を行うかどうかを設定する
 *
 *	@return		none
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetInvisiblePaneCalculateMtx( bool flg )
{
	m_LytDrawInfo.SetInvisiblePaneCalculateMtx( flg );
}


//----------------------------------------------------------------------------
/**
 *	@brief  描画する必要があるかチェック
 *
 *	@retval true    必要がある。
 *	@retval false   必要がない。
 */
//-----------------------------------------------------------------------------
b32 LytWkBase::IsDrawDisplayType( DisplayType lod ) const
{
  GFL_ASSERT( lod < DISPLAY_NORMAL_MAX ); //@check

  // 両面描画
  if( m_DisplayType == DISPLAY_BOTH_MAX ){
    return true;
  }

  if( lod == m_DisplayType ){
    return true;
  }
  return false;
}


//============================================================================
/**
 *	Group操作
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief  IndexのGroupの取得
 *
 *  @retval Groupのインスタンス
 */
//-----------------------------------------------------------------------------
nw::lyt::Group* LytWkBase::GetGroup( u32 paneIndex ) const
{
  nw::lyt::GroupContainer* group_container  = m_LytSys->GetGroupContainer();

  if( group_container == NULL ){
    GFL_ASSERT( group_container ); //@check
    return NULL;
  }
  
  const char * paneName = m_res->GetPaneName(paneIndex);
  if( paneName == NULL ){
    return NULL;
  }

  return group_container->FindGroupByName( paneName );
}

//----------------------------------------------------------------------------
/**
 *	@brief    参照レイアウトリソース取得
 *
 *	@return   レイアウトリソース
 */
//-----------------------------------------------------------------------------
const LytRes* LytWkBase::GetLytRes(void) const
{
  return m_res;
}




//============================================================================
/**
 *  隠蔽メンバ関数
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  オートアニメーション　更新処理
 */
//-----------------------------------------------------------------------------
void LytWkBase::UpdateAutoAnime( void )
{
  int i;
  
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
          }
        }
      }
    }
  }
}


//=============================================================================
/**
 *					LytWkAnime実装
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
LytWkAnime::LytWkAnime( void )
{
  m_Initialize  = false;
  m_AutoAnime   = false;
  m_AutoAnimeSpeed = 1.0f;
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
LytWkAnime::~LytWkAnime()
{
  if( m_Initialize ){
    GFL_ASSERT(0);
    Finalize();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param  lytwk           NWレイアウトワーク
 *	@param  animTransform   設定トランスフォーム
 *	@param  res             リソース
 *	@param  lytDataID       リソースデータID
 */
//-----------------------------------------------------------------------------
void LytWkAnime::Initialize( nw::lyt::Layout* lytwk, nw::lyt::AnimTransform* animTransform,
                             const LytRes* res, u32 lytDataID,
                             bool isUsingHash, nw::lyt::Layout* layout )
{
  m_AnimTransform = animTransform;

  // アニメーションリソースのアクセサ
  m_AnimRes.Set( res->GetAnimeResource( lytDataID ) );

  // もしグループ情報があるのであればグループ数分のアニメーションリンクができるように数を指定する。
  const u16 bindGroupNum = m_AnimRes.GetGroupNum();
  u16 animNum = 0;
  if(bindGroupNum == 0){
    animNum = m_AnimRes.GetResourceBlock()->animContNum; 
  }else{
    const nw::lyt::AnimationGroupRef *const groupRefs = m_AnimRes.GetGroupArray();
    for (int grpIdx = 0; grpIdx < bindGroupNum; ++grpIdx)
    {
      // グループにあるペイン全てで必要になるアニメーションの個数を数えます。
      nw::lyt::Group *const pGroup = lytwk->GetGroupContainer()->FindGroupByName(groupRefs[grpIdx].GetName());
      if (pGroup == NULL)
      {
        // "Group not found
        GFL_ASSERT(0);
        continue;
      }

      #ifdef GFL_USE_CUSTOM_NW4C
      animNum += m_AnimRes.CalcAnimationNum(pGroup, m_AnimRes.IsDescendingBind(),isUsingHash, layout);
      #else
      animNum += m_AnimRes.CalcAnimationNum(pGroup, m_AnimRes.IsDescendingBind());
      #endif
    }
  }

  // バインドするアニメーション数を明示的に指定してリソースをセットします。
  m_AnimTransform->SetResource( m_AnimRes.GetResourceBlock(), res->GetResourceAccessor(), animNum );
  
  m_Share       = false;
  m_Initialize  = true;
  m_BindFlag    = false;
  m_AutoAnime   = false;
  m_AutoAnimeSpeed = 1.0f;
  m_Enable      = false;
}





//----------------------------------------------------------------------------
/**
 *	@brief  初期化　共有アニメーションの初期化　＆　自動バインド
 *
 *	@param  res             リソース
 *	@param  lytDataID       リソースデータID
 *	@param  shareIdx        シェアペインインデックス
 *	@param  shareIdx        シェアペインインデックス
 *
 *	@retval true    登録成功
 *	@retval false   登録失敗（基準Paneだった）
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::InitializeShare( nw::lyt::Layout* lytwk, const LytRes* res, void * p_animeRes, u32 shareIdx, nw::lyt::PaneLinkList::Iterator it, nw::lyt::Pane* pSrcPane, nw::lyt::Group* pGroup )
{
  m_AnimTransform = NULL;
  // アニメーションリソースのアクセサ
  m_AnimRes.Set( p_animeRes );

  // 対象Paneを検索
  {
    const u16 animShareInfoNum = m_AnimRes.GetAnimationShareInfoNum();
    const u16 bindGroupNum = m_AnimRes.GetGroupNum();
    
    // 最大値チェック
    if( animShareInfoNum <= shareIdx ){
      GFL_ASSERT(0);
      return false;
    }

    if (pSrcPane == NULL)
    {
      // "Source pane of animation-share is not found"
      GFL_ASSERT(0);
      return false;
    }

    nw::lyt::internal::AnimPaneTree animPaneTree(pSrcPane, m_AnimRes);
  
    
    if (! animPaneTree.IsEnabled()) // 共有元ペインにアニメーションが無い?
    {
      GFL_ASSERT(0);
      return false;
    }

    // 対象のグループがあるか
    if (pGroup == NULL)
    {
      // "Target group of animation-share is not found:"
      GFL_ASSERT(0);
      return false;
    }


    m_Pane = NULL;
    if( it->target != pSrcPane ){
      m_Pane = it->target;
    }
  }

  if( m_Pane == NULL ){
    return false;
  }

  m_Share       = true;
  m_ShareIndex  = shareIdx;
  m_Initialize  = true;
  m_BindFlag    = false;
  m_AutoAnime   = false;
  m_AutoAnimeSpeed = 1.0f;
  m_Enable      = false;

  return true;
}


//----------------------------------------------------------------------------
/**
 *	@brief  通常バインド処理
 *
 *	@param	lytwk ワーク
 */
//-----------------------------------------------------------------------------
void LytWkAnime::Bind( nw::lyt::Layout* lytwk, bool is_enable, bool is_useHash )
{
  if( m_BindFlag ){
    GFL_ASSERT( !m_BindFlag ); //@check
    return ;
  }
  
  // グループの有無でバインドを変える
  // もしグループ情報があるのであればグループを使用してバインド
  const u16 bindGroupNum = m_AnimRes.GetGroupNum();
  if(bindGroupNum == 0)
  {
    #ifdef GFL_USE_CUSTOM_NW4C
    if( is_useHash ){
      lytwk->GetRootPane()->BindAnimationWithHash(m_AnimTransform, lytwk, true, !is_enable);
    }
    else
    #endif
    {
      lytwk->GetRootPane()->BindAnimation(m_AnimTransform, true, !is_enable);
    }
  }
  else{
    const nw::lyt::AnimationGroupRef *const groupRefs = m_AnimRes.GetGroupArray();
    for (int grpIdx = 0; grpIdx < bindGroupNum; ++grpIdx)
    {
      // グループにあるペイン全てで必要になるアニメーションの個数を数えます。
      nw::lyt::Group *const pGroup = lytwk->GetGroupContainer()->FindGroupByName(groupRefs[grpIdx].GetName());
      if (pGroup == NULL)
      {
        // "Group not found
        GFL_ASSERT(0);
        continue;
      }


      #ifdef GFL_USE_CUSTOM_NW4C
      if( is_useHash ){
        nw::lyt::BindAnimationWithHash(pGroup, m_AnimTransform, lytwk, m_AnimRes.IsDescendingBind(), !is_enable);
      }
      else
      #endif
      {
        nw::lyt::BindAnimation(pGroup, m_AnimTransform, m_AnimRes.IsDescendingBind(), !is_enable);
      }
    }
  }

  m_BindFlag = true;
  m_Enable = is_enable;
}

//----------------------------------------------------------------------------
/**
 *	@brief  共有アニメーションのバインド処理
 *
 *	@param	lytwk ワーク
 *	@param  res   リソースクラス
 */
//-----------------------------------------------------------------------------
void LytWkAnime::BindShare( nw::lyt::Layout* lytwk, const LytRes* res, nw::lyt::Pane * pSrcPane )
{
  if( m_Share == false ){
    GFL_ASSERT(0);
    return ;
  }
  if( m_BindFlag ){  // バインドずみ
    GFL_ASSERT(0);
    return ;
  }

  if (pSrcPane == NULL)
  {
    // "Source pane of animation-share is not found"
    GFL_ASSERT(0);
    return;
  }

  nw::lyt::internal::AnimPaneTree animPaneTree(pSrcPane, m_AnimRes);
  // srcPaneNameと異なるペインに対してアニメーションをバインド
  m_AnimTransform = animPaneTree.Bind(lytwk, m_Pane, res->GetResourceAccessor());
  m_Pane->SetAnimationEnable( m_AnimTransform, false, false );


  m_BindFlag = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  通常アンバインド処理
 *
 *	@param	lytwk ワーク
 */
//-----------------------------------------------------------------------------
void LytWkAnime::UnBind( nw::lyt::Layout* lytwk )
{
  if( m_Share == false ){
    if( m_BindFlag ){
      lytwk->UnbindAnimation( m_AnimTransform );
      m_BindFlag = false;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  共有アニメーションのアンバインド処理
 *
 *	@param	lytwk ワーク
 */
//-----------------------------------------------------------------------------
void LytWkAnime::UnBindShare( nw::lyt::Layout* lytwk )
{
  if( m_BindFlag ){
    if( m_Share && (m_AnimTransform!=NULL) ){
      m_Pane->UnbindAnimation( m_AnimTransform, false );
      lytwk->DeleteAnimTransform( m_AnimTransform );
      m_AnimTransform = NULL;
      m_BindFlag = false;
    }
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  アニメーションの有効・無効設定
 */
//-----------------------------------------------------------------------------
void LytWkAnime::SetEnable( nw::lyt::Layout* lytwk, bool flag )
{
  if(m_AnimTransform){
    m_Enable = flag;
    if( !m_Share ){
      lytwk->GetRootPane()->SetAnimationEnable( m_AnimTransform, flag ); 
    }else{
      m_Pane->SetAnimationEnable( m_AnimTransform, flag, false ); 
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  アニメーションが有効かチェック
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsEnable( void ) const
{
  return m_Enable;
}

//----------------------------------------------------------------------------
/**
 *	@brief  リソースから　Shareグループを取得
 *
 *	@param	res           リソース
 *	@param	lytDataID     リソースデータID
 *	@param  shareGroupIdx Shareグループインデックス
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::GetResShareInfoInPaneNum( const nw::lyt::Layout* lytwk, const nw::lyt::AnimResource& animRes, const LytRes* res, u32 shareGroupIdx, nw::lyt::Pane** ppSrcPane, nw::lyt::Group** ppGroup, const char ** ppGroupName, u32 groupIndex )
{
  const u16 animShareInfoNum = animRes.GetAnimationShareInfoNum();
  
  // 最大値チェック
  if( animShareInfoNum <= shareGroupIdx ){
    GFL_ASSERT(0);
    return false;
  }

  const nw::lyt::AnimationShareInfo *const animShareInfoAry = animRes.GetAnimationShareInfoArray();


  *ppGroupName = animShareInfoAry[shareGroupIdx].GetTargetGroupName();
  // 指定グループ？
  if( groupIndex != LYTSYS_PANEINDEX_ERROR ){
    if( gfl::std::StringCmp( *ppGroupName, res->GetPaneName( groupIndex ) ) != 0 ){
      return false;
    }
  }
  
  // このグループがアニメーションの関連に入っているか？
  if( animRes.GetGroupNum() > 0 ) {
    // 自分のグループが関連グループに入っているかチェック
    // 関連グループに含まれてない場合は共有しない
    const bool bInclude = IsIncludeAnimationGroupRef(
        animRes.GetGroupArray(),
        *ppGroupName,
        animRes.GetGroupNum() );
    if( !bInclude ){
      return false;
    }
  }

  *ppGroup = lytwk->GetGroupContainer()->FindGroupByName(*ppGroupName);
  if (*ppGroup == NULL)
  {
    // "Target group of animation-share is not found:"
    GFL_ASSERT_MSG(0, "group name %s\n", animShareInfoAry[shareGroupIdx].GetTargetGroupName());
    return false;
  }

  // そのシェアペインをもつグループのペイン最大数を求める
  #ifdef GFL_USE_CUSTOM_NW4C
  *ppSrcPane = lytwk->GetRootPane()->FindPaneBySimpleName(animShareInfoAry[shareGroupIdx].GetSrcPaneName());
  #else
  *ppSrcPane = lytwk->GetRootPane()->FindPaneByName(animShareInfoAry[shareGroupIdx].GetSrcPaneName());
  #endif

  if (*ppSrcPane == NULL)
  {
    // "Source pane of animation-share is not found"
    GFL_PRINT( "pane name %s group name %s\n", animShareInfoAry[shareGroupIdx].GetSrcPaneName(), animShareInfoAry[shareGroupIdx].GetTargetGroupName() );
    GFL_ASSERT_MSG(0, "pane name %s\n", animShareInfoAry[shareGroupIdx].GetSrcPaneName());
    return false;
  }

  nw::lyt::internal::AnimPaneTree animPaneTree(*ppSrcPane, animRes);
  if (! animPaneTree.IsEnabled()) // 共有元ペインにアニメーションが無い?
  {
    // "srcPane not Animation"
    return false;
  }

  return true;
}



//----------------------------------------------------------------------------
/**
 *	@brief  shareグループ内での、グループIDを取得
 *
 *  @param  lytwk         レイアウトワーク
 *	@param	res           リソース
 *	@param	lytDataID     リソースデータID
 *	@param  shareGroupIdx Shareグループインデックス
 *	@param  cp_panename   ペイン名
 *
 *	@return グループ内のペインインデックス
 */
//-----------------------------------------------------------------------------
s32 LytWkAnime::GetResShareGroupInPaneIndex( const nw::lyt::Layout* lytwk, const LytRes* res, const nw::lyt::AnimResource& animRes, u32 shareGroupIdx, const char* cp_panename, nw::lyt::Pane** ppSrcPane, nw::lyt::Group** ppGroup )
{
  const u16 animShareInfoNum = animRes.GetAnimationShareInfoNum();
  
  // 最大値チェック
  if( animShareInfoNum <= shareGroupIdx ){
    GFL_ASSERT(0);
    return -1;
  }

  // そのシェアペインをもつグループのペイン最大数を求める
  const nw::lyt::AnimationShareInfo *const animShareInfoAry = animRes.GetAnimationShareInfoArray();
  
  if(*ppSrcPane == NULL)
  {
    #ifdef GFL_USE_CUSTOM_NW4C
    *ppSrcPane = lytwk->GetRootPane()->FindPaneBySimpleName( animShareInfoAry[shareGroupIdx].GetSrcPaneName() );
    #else
    *ppSrcPane = lytwk->GetRootPane()->FindPaneByName( animShareInfoAry[shareGroupIdx].GetSrcPaneName() );
    #endif
  }
  if (*ppSrcPane == NULL)
  {
    // "Source pane of animation-share is not found"
    GFL_ASSERT(0);
    return 0;
  }

  nw::lyt::internal::AnimPaneTree animPaneTree(*ppSrcPane, animRes);
  if (! animPaneTree.IsEnabled()) // 共有元ペインにアニメーションが無い?
  {
    return 0;
  }

  if(*ppGroup == NULL){
    *ppGroup = lytwk->GetGroupContainer()->FindGroupByName(animShareInfoAry[shareGroupIdx].GetTargetGroupName());
  }
  if (*ppGroup == NULL)
  {
    // "Target group of animation-share is not found:"
    GFL_ASSERT(0);
    return 0;
  }

  nw::lyt::PaneLinkList& paneList = (*ppGroup)->GetPaneList();

  u32 wkcount = 0;
  for (nw::lyt::PaneLinkList::Iterator it = paneList.GetBeginIter(); it != paneList.GetEndIter(); ++it, ++wkcount)
  {
    nw::lyt::Pane* p_pane = it->target;

    if( gfl::std::StringCmp( p_pane->GetName(), cp_panename ) == 0 ){
      return wkcount;
    }
  }

  return -1;
}

//----------------------------------------------------------------------------
/**
 *	@brief  破棄
 */
//-----------------------------------------------------------------------------
void LytWkAnime::Finalize( void )
{
  if( m_Initialize ){

    GFL_ASSERT( m_BindFlag == false );

    m_AnimTransform = NULL;
    m_Initialize = false;

    m_Share = false;
    m_Pane = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  初期化づみチェック
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsInitialized( void ) const
{
  return m_Initialize;
}

//----------------------------------------------------------------------------
/**
 *	@brief  アニメーションフレームを進める
 *
 *	@param	frame 進めるフレーム数
 *
 *	@retval true      アニメーション終了
 *	@retval false     アニメーション途中
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::AddAnimeFrame( f32 frame )
{
  bool ret = false;
  if( m_AnimTransform ){
    if( m_Enable ){
      f32 frame_now = m_AnimTransform->GetFrame();
      f32 frame_max = m_AnimTransform->GetFrameMax();
      bool loop = m_AnimTransform->IsLoopData();
      
      //frameが2.0で、最終フレームが奇数等の場合
      //最終フレームを飛ばした瞬間に※１でtrueが返り、オートアンバインドになってしまうため
      //最後だけframeを1.0fにする
      if( static_cast<int>(frame) % 2 == 0 //偶数
          && (static_cast<int>(frame_max)-1) % 2 == 1  //奇数
          && frame_now + frame > frame_max-1 ){
        frame = 1.0f;
      }

      frame_now += frame;

      if( frame_now < 0 ){
        if( !loop ){
          frame_now = 0;
        }else{
          frame_now += frame_max;
        }
      }else if( frame_max <= frame_now ){

        if( !loop ){  //※１
          frame_now = frame_max-1.0f;
          ret = true;
        }else{
          frame_now -= frame_max;
        }
      }
      m_AnimTransform->SetFrame( frame_now );
    }
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  アニメーションフレームの終了をチェック
 *
 *	@retval true      アニメーション終了
 *	@retval false     アニメーション途中
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsAnimeEnd( void ) const
{
  if( m_AnimTransform ){
    f32 frame_now = m_AnimTransform->GetFrame();
    f32 frame_max = m_AnimTransform->GetFrameMax();
    bool loop = m_AnimTransform->IsLoopData();

    // UnBind中か?
    if( !m_Enable ){
      return true;
    }
    
    if( (frame_now >= (frame_max-1.0f)) && (!loop) ){
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  オートアニメーション設定
 *
 *	@param	flag  true  オートアニメON    false オートアニメ
 */
//-----------------------------------------------------------------------------
void LytWkAnime::SetAutoAnime( bool flag )
{
  m_AutoAnime = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  オートアニメーション設定　取得
 *
 *	@retval true    オートアニメON
 *	@retval false   オートアニメOFF
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsAutoAnime( void ) const
{
  return m_AutoAnime;
} 

//----------------------------------------------------------------------------
/**
 *	@brief Lytのアニメーションクラスを取得
 */
//-----------------------------------------------------------------------------
nw::lyt::AnimTransform* LytWkAnime::GetAnimeWork( void ) const
{
  return m_AnimTransform;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  共有アニメーションかチェック
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsShareAnime(void) const
{
  return m_Share;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  バインドされているかチェック
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsBindAnime( void ) const
{
  return m_BindFlag;
}




} // g2d
} // grp
} // gfl
