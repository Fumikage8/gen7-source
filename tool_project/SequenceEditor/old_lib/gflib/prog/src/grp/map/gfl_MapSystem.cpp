//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MapSystem.cpp
 *	@brief  マップ管理システム
 *	@author	tomoya takahashi
 *	@date		2011.01.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <grp/map/gfl_MapSystem.h>

namespace gfl{
namespace grp{
namespace map{

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
System::System( void ) : 
  m_Initialize(false),
  m_CreateModel(false),
  m_Visible(true),
  m_Trans(gfl::math::VEC3(0,0,0)),
  m_pArcFile(NULL),
  m_ArcFileOriginal(false),
  m_DataID(0),
  m_pMapData(NULL),
  m_MapDataHeapSize(0),
  m_FileType(0),
  m_pFileControl(NULL),
  m_cpFileControls(NULL),
  m_VramAddrTex(0),
  m_VramAddrPltt(0),
  m_MapTexVramSize(0),
  m_DivLoadSize(0),
  m_LoadStatus(LOAD_STATE_IDLING)
{
  this->ReleaseGlobalTexResource();
}

//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ  + 初期化
 */
//-----------------------------------------------------------------------------
System::System( const SetUpParam& param, gfl::grp::g3d::Scene* p_scene, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap ) : 
  m_Initialize(false),
  m_CreateModel(false),
  m_Visible(true),
  m_Trans(gfl::math::VEC3(0,0,0)),
  m_pArcFile(NULL),
  m_ArcFileOriginal(false),
  m_DataID(0),
  m_pMapData(NULL),
  m_MapDataHeapSize(0),
  m_FileType(0),
  m_pFileControl(NULL),
  m_cpFileControls(NULL),
  m_VramAddrTex(0),
  m_VramAddrPltt(0),
  m_MapTexVramSize(0),
  m_ModelDatas(),
  m_DivLoadSize(0),
  m_LoadStatus(LOAD_STATE_IDLING)
{
  // ワーククリア
  this->ReleaseGlobalTexResource();

  // 初期化処理
  Initialize( param, p_scene, p_sysHeap, p_deviceHeap );
}




//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
System::~System()
{
  Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param	param         環境データ
 *	@param  p_scene       シーンシステム
 *	@param  p_sysHeap     システム制御に使用するヒープ
 *	@param  p_deviceHeap  デバイスヒープ
 */
//-----------------------------------------------------------------------------
void System::Initialize( const SetUpParam& param, gfl::grp::g3d::Scene* p_scene, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap )
{
  // 多重初期化チェック
  if( m_Initialize != false ){
    GFL_ASSERT(0);
    Finalize();
  }

  // モデルモードの保存
  m_ModelMode = param.modelMode;
  
  // シーンシステム初期化
  m_pScene = p_scene;

  // ファイルコントロール保存
  m_cpFileControls = param.cpFileControls;

  // マップバッファ初期化
  InitializeMapBuffer( param, p_sysHeap, p_deviceHeap );

  // VRAMアドレス初期化
  InitializeVramAddr( param, p_sysHeap, p_deviceHeap );

  // リソースバッファ初期化
  InitializeModelBuffer( param, p_sysHeap, p_deviceHeap );

  // 分割読み込み初期化
  InitializeDivLoad( param, p_sysHeap, p_deviceHeap );

  // 初期化完了
  m_Initialize = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  破棄
 */
//-----------------------------------------------------------------------------
void System::Finalize( void )
{
  if( m_Initialize ){
    m_Initialize = false;

    {
      u32 count = 0;
      while( (ResetLoadData() == false) && (count < 10000) ){
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMicroSeconds( 10000 ) );
        ++count;
      }
      GFL_ASSERT_STOP( count < 10000 );
    }

    //アーカイブファイル
    ReleaseArc();

    // 分割読み込み破棄
    FinalizeDivLoad();

    // リソースバッファ破棄
    FinalizeModelBuffer();

    // VRAMアドレス破棄
    FinalizeVramAddr();

    // マップバッファ破棄
    FinalizeMapBuffer();

    m_pScene = NULL;

    Clear();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  更新処理
 */
//-----------------------------------------------------------------------------
void System::Update( void )
{
  // 読み込み処理
  UpdateLoad();
}

//----------------------------------------------------------------------------
/**
 *	@brief  描画更新処理
 */
//-----------------------------------------------------------------------------
void System::UpdateDraw( gfl::grp::g3d::Camera* p_camera )
{
  GFL_UNUSED(p_camera);
}

//----------------------------------------------------------------------------
/**
 *	@brief  読み込みリクエスト
 *
 *	@param	dataID        データID
 *	@param	divLoadFlag   分割読み込みするか。　（true=分割　false=一括）
 *	@param  true  リクエスト成功
 *	@param  false リクエスト失敗
 */
//-----------------------------------------------------------------------------
bool System::ReqLoadData( u32 dataID, b32 divLoadFlag )
{
  u32 loadSize;
  GFL_ASSERT_STOP( m_pArcFile );
  if( !m_pArcFile->WaitSetupForFastMode() ){
    return false;
  }
  if( m_pArcFile->WaitAsyncRead() ){
    return false;
  }
  
  if( divLoadFlag ){
    loadSize = m_DivLoadSize;
  }else{
    loadSize = m_pArcFile->GetDataSize( dataID );
  }

  m_DataID = dataID;
  m_DivLoad.StartFileLoad( m_pArcFile, m_DataID, loadSize );

  m_LoadStatus = LOAD_STATE_START;

  return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  読み込みリクエスト  圧縮データの読み込みリクエスト
 *
 *	@param	dataID        データID
 *	@param	divLoadFlag   分割読み込みするか。　（true=分割　false=一括）
 *	@param  p_tmp_heap    テンポラリ確保用のバッファ
 *
 *	@param  true  リクエスト成功
 *	@param  false リクエスト失敗
 */
//-----------------------------------------------------------------------------
bool System::ReqLoadCompData( u32 dataID, b32 divLoadFlag, gfl::heap::HeapBase* p_tmp_buff )
{
  u32 loadSize;
  GFL_ASSERT_STOP( m_pArcFile );
  if( !m_pArcFile->WaitSetupForFastMode() ){
    return false;
  }
  if( m_pArcFile->WaitAsyncRead() ){
    return false;
  }
  
  if( divLoadFlag ){
    loadSize = m_DivLoadSize;
  }else{
    loadSize = m_pArcFile->GetDataSize( dataID );
  }

  m_DataID = dataID;
  m_DivLoad.StartCompFileLoad( m_pArcFile, m_DataID, loadSize, p_tmp_buff );

  m_LoadStatus = LOAD_STATE_START;

  return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  読み込みリセット
 */
//-----------------------------------------------------------------------------
bool System::ResetLoadData( void )
{
  bool result = false;
  
  // 読み込み情報の破棄
  DeleteModel();
  
  result = m_DivLoad.ResetFileLoad();
  if( result ){
    m_LoadStatus = LOAD_STATE_IDLING;
  }
  return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief  読み込み状態の取得
 *
 *	@retval  LOAD_STATE_IDLING = 0,        // 待機中
 *	@retval  LOAD_STATE_START,             // 読み込み開始
 *	@retval  LOAD_STATE_LOAD,              // 読み込み中
 *	@retval  LOAD_STATE_SETUP,             // セットアップ中
 *	@retval  LOAD_STATE_END,               // 完了
 */
//-----------------------------------------------------------------------------
gfl::grp::map::System::LoadStatus System::GetLoadStatus( void ) const
{
  return m_LoadStatus;
}

//----------------------------------------------------------------------------
/**
 *	@brief  読み込んできたデータを取得
 *
 *	@return 読み込んできたデータ(gflib_cpp/tools/binlinker.rbを使用して生成する。）
 */
//-----------------------------------------------------------------------------
const gfl::base::BinLinkerAccessor& System::GetLoadMemoryPointer(void) const
{
  return m_LinkData;
}

//----------------------------------------------------------------------------
/**
 *	@brief  読み込んできたデータIDを取得
 *
 *	@return データID
 */
//-----------------------------------------------------------------------------
u32 System::GetLoadDataID( void ) const
{
  return m_DataID;
}


//----------------------------------------------------------------------------
/**
 *	@brief  アーカイブデータ設定
 *
 *	@param	  cp_path   アーカイブデータのパス
 *  @param    p_heap    ヒープ
 */
//-----------------------------------------------------------------------------
void System::ResistArc( const wchar_t* cp_path, gfl::heap::HeapBase* p_heap )
{
  if( m_pArcFile ){
    GFL_ASSERT(0);
    ReleaseArc();
  }
  m_pArcFile = GFL_NEW(p_heap) gfl::fs::ArcFile(p_heap, cp_path, 0);
  m_pArcFile->StartSetupForFastMode( p_heap, p_heap );
  m_ArcFileOriginal = true;
}


//----------------------------------------------------------------------------
/**
 *	@brief  アーカイブデータ設定  IDバージョン
 *
 *	@param	  arc_id    アーカイブデータのID
 *  @param    p_heap    ヒープ
 *  @param    file_on_pc  パソコンからの読み込みの場合true, ROMの場合false
 */
//-----------------------------------------------------------------------------
void System::ResistArc( gfl::fs::ArcFile::ARCID arc_id, gfl::heap::HeapBase* p_heap, bool file_on_pc )
{
  if( m_pArcFile ){
    GFL_ASSERT(0);
    ReleaseArc();
  }
  u32 init_flag = 0;
  if( file_on_pc ) {
    init_flag |= gfl::fs::ArcFile::HIO;
  }
  m_pArcFile = GFL_NEW(p_heap) gfl::fs::ArcFile(p_heap, arc_id, init_flag);
  m_pArcFile->StartSetupForFastMode( p_heap, p_heap );
  m_ArcFileOriginal = true;
}


//----------------------------------------------------------------------------
/**
 *	@brief  アーカイブデータ設定
 *
 *	@param	 p_arc 
 */
//-----------------------------------------------------------------------------
void System::ResistArc( gfl::fs::ArcFile* p_arc )
{
  if( m_pArcFile ){
    GFL_ASSERT(0);
    ReleaseArc();
  }
  m_pArcFile = p_arc;
  m_ArcFileOriginal = false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  アーカイブ設定完了待ち
 *
 *  @retval true    完了
 *  @retval false   途中（アーカイブのファーストモードセットアップ中）
 */
//-----------------------------------------------------------------------------
bool System::IsRegistArcFinished( void ) const
{
  if( m_pArcFile ){
    if( m_pArcFile->WaitSetupForFastMode() ){
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  アーカイブデータ破棄
 */
//-----------------------------------------------------------------------------
void System::ReleaseArc( void )
{
  if(m_pArcFile){
    if( m_ArcFileOriginal ){
      GFL_DELETE m_pArcFile;
    }
    m_pArcFile = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  アーカイブデータ取得
 *
 */
//-----------------------------------------------------------------------------
gfl::fs::ArcFile* System::GetArc( void ) const
{
  return m_pArcFile;
}

//----------------------------------------------------------------------------
/**
 *	@brief  グローバルテクスチャリソース　設定
 *  
 *	@param	  p_globalTex   グローバルテクスチャ
 *	@param    index         グローバルテクスチャのインデックス
 */
//-----------------------------------------------------------------------------
void System::ResistGlobalTexResource( gfl::grp::g3d::Resource* p_globalTex, u32 index )
{
  // モデルモードの不一致チェック
  if( !this->isModelMode( MODEL_DEFAULT ) ){GFL_ASSERT_STOP(0);}
  
  if(index < GLOBAL_RESOURCE_MAX){
    m_ModelDatas.def.m_pGlobalTexRes[index] = p_globalTex;
  }else{
    GFL_ASSERT(index < GLOBAL_RESOURCE_MAX); //@check
  }
}

#if GFL_GRP_G3D_H3D_USE
void System::ResistGlobalTexResource( gfl::grp::g3d::H3dResource* p_globalTex, u32 index )
{
  // モデルモードの不一致チェック
  if( !this->isModelMode( MODEL_H3D ) ){GFL_ASSERT_STOP(0);}
  
  if(index < GLOBAL_RESOURCE_MAX){
    m_ModelDatas.h3d.m_pGlobalTexRes[index] = p_globalTex;
  }else{
    GFL_ASSERT(index < GLOBAL_RESOURCE_MAX); //@check
  }
}
#endif // GFL_GRP_G3D_H3D_USE

//----------------------------------------------------------------------------
/**
 *	@brief  グローバルテクスチャリソース　クリア
 */
//-----------------------------------------------------------------------------
void System::ReleaseGlobalTexResource(void)
{
  if( m_ModelMode == MODEL_DEFAULT ){
    for( u32 i=0; i<GLOBAL_RESOURCE_MAX; ++i ){
      m_ModelDatas.def.m_pGlobalTexRes[i] = NULL;
    }
  }
  else{
#if GFL_GRP_G3D_H3D_USE
    for( u32 i=0; i<GLOBAL_RESOURCE_MAX; ++i ){
      m_ModelDatas.h3d.m_pGlobalTexRes[i] = NULL;
    }
#endif // GFL_GRP_G3D_H3D_USE
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  地形表示のON・OFF
 *
 *	@param	flag    true:ON false:OFF
 */
//-----------------------------------------------------------------------------
void System::SetVisible( b32 flag )
{
  m_Visible = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  地形表示のON・OFF　チェック
 *
 *	@retval true      ON
 *	@retval false     OFF
 */
//-----------------------------------------------------------------------------
b32 System::IsVisible( void ) const
{
  return m_Visible;
}

//----------------------------------------------------------------------------
/**
 *	@brief  地形表示位置の設定
 *
 *	@param	  pos   位置
 */
//-----------------------------------------------------------------------------
void System::SetTrans( const gfl::math::VEC3& pos )
{
  m_Trans = pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief  地形表示位置の取得
 *
 *	@return 位置
 */
//-----------------------------------------------------------------------------
const gfl::math::VEC3& System::GetTrans( void ) const
{
  return m_Trans;
}

 
//----------------------------------------------------------------------------
/**
 *	@brief  アトリビュート取得
 *  
 *	@param	  p_attrInfo    アトリビュートインフォ
 *	@param    cp_pos        ３D座標
 *	@param    cp_way        地面方向（Defaultは、(0.0f,-1.0f,0.0f)
 *	@param    map_width     マップブロック幅
 */
//-----------------------------------------------------------------------------
void System::GetAttr( gfl::grp::map::AttrInfo* p_attrInfo, const void* cp_pos, const gfl::math::VEC3* cp_way, f32 map_width )
{
  // アトリビュートインフォの初期化
  FileControl::ClearAttrInfo( p_attrInfo );
  
  // アトリビュートの取得
  if( m_pFileControl ){
    m_pFileControl->GetAttr( this, p_attrInfo, cp_pos, cp_way, map_width );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  地形モデル生成
 *  
 *  @param  p_allocator       アロケータ（デバイスメモリ）
 *  @param  p_model_memory    モデルリソースバッファ
 *  @param  p_tex_memory      テクスチャリソースバッファ
 *  @param  cp_desc           リソース展開設定
 */
//-----------------------------------------------------------------------------
void System::CreateModel( gfl::heap::NwAllocator* p_allocator, void* p_model_memory, void* p_tex_memory, const gfl::grp::g3d::Resource::Description* cp_res_desc, const gfl::grp::g3d::Model::Description* cp_model_desc, u32 common_resource_flag )
{
  bool result;

  // モデルモードの不一致チェック
  if( !this->isModelMode( MODEL_DEFAULT ) ){GFL_ASSERT_STOP(0);}

  
  // 多重登録監視
  if( m_ModelDatas.def.m_pMdlRes->IsSetup() ){
    GFL_ASSERT(0);
    DeleteModel();
  }

  // モデル
  m_ModelDatas.def.m_pMdlRes->AttachBuffer( p_allocator, p_model_memory, cp_res_desc, false );
  if( p_tex_memory ){
    m_ModelDatas.def.m_pTexRes->AttachBufferAndSetup( p_allocator, p_tex_memory, cp_res_desc, false );
    // モデルリソースに、テクスチャリソースを登録
    m_ModelDatas.def.m_pMdlRes->Setup( p_allocator, m_ModelDatas.def.m_pTexRes );
  }


  // グローバルテクスチャのセットアップ
  for( u32 i=0; i<GLOBAL_RESOURCE_MAX; ++i ){
    if( m_ModelDatas.def.m_pGlobalTexRes[i] ){
      m_ModelDatas.def.m_pMdlRes->Setup( p_allocator, m_ModelDatas.def.m_pGlobalTexRes[i] );
    }
  }
  
  // リソース設定
  result = m_ModelDatas.def.m_pMdlRes->Setup( p_allocator, NULL, common_resource_flag );
  if( !result ){
    int nw_result = m_ModelDatas.def.m_pMdlRes->GetResult();
    GFL_PRINT( "ModelResource SetUp Fail %d\n", nw_result );
    m_ModelDatas.def.m_pMdlRes->PrintUnresolvedReference();
    GFL_ASSERT_STOP_MSG( result, "ModelResource SetUp Fail %d\n", nw_result );
  }

  // モデルの構築 リソースの０番にあること！
  m_ModelDatas.def.m_pModel->Create( p_allocator, p_allocator, m_ModelDatas.def.m_pMdlRes, 0, const_cast<gfl::grp::g3d::Model::Description*>(cp_model_desc) );


  // モデル生成完了
  m_CreateModel = true;

  // モデルに位置を登録
  m_ModelDatas.def.m_pModel->SetTranslate( m_Trans );

  // シーンに登録
  m_pScene->AddSceneNode( m_ModelDatas.def.m_pModel );
}
#if GFL_GRP_G3D_H3D_USE
// H3D版
void System::CreateH3dModel( gfl::heap::NwAllocator* p_allocator, void* p_model_memory, void* p_tex_memory, /* const gfl::grp::g3d::H3dResource::Description* cp_res_desc,  const gfl::grp::g3d::H3dModel::Description* cp_model_desc, */ u32 common_resource_flag )
{
  bool result;

  // モデルモードの不一致チェック
  if( !this->isModelMode( MODEL_H3D ) ){GFL_ASSERT_STOP(0);}
  
  // 多重登録監視
  if( m_ModelDatas.h3d.m_pMdlRes->IsSetup() )
  {
    GFL_ASSERT(0);
    DeleteModel();
  }

  // モデル
  m_ModelDatas.h3d.m_pMdlRes->AttachBuffer( p_model_memory/*, cp_res_desc, */ );
  if( p_tex_memory ){
    m_ModelDatas.h3d.m_pTexRes->AttachBuffer( p_tex_memory );
    m_ModelDatas.h3d.m_pTexRes->Setup( p_allocator, p_allocator/*, cp_res_desc, */ );
    // モデルリソースに、テクスチャリソースを登録
    m_ModelDatas.h3d.m_pMdlRes->Setup( p_allocator, p_allocator, m_ModelDatas.h3d.m_pTexRes );
  }


  // グローバルテクスチャのセットアップ
  for( u32 i=0; i<GLOBAL_RESOURCE_MAX; ++i ){
    if( m_ModelDatas.h3d.m_pGlobalTexRes[i] ){
      m_ModelDatas.h3d.m_pMdlRes->Setup( p_allocator, p_allocator, m_ModelDatas.h3d.m_pGlobalTexRes[i] );
    }
  }
  
  // リソース設定
  result = m_ModelDatas.h3d.m_pMdlRes->Setup( p_allocator, p_allocator, NULL, common_resource_flag );
  if( !result ){
    GFL_ASSERT_STOP_MSG( result, "ModelResource SetUp Fail\n" );
  }

  // モデルの構築 リソースの０番にあること！
  m_ModelDatas.h3d.m_pModel->Create( p_allocator, p_allocator, m_ModelDatas.h3d.m_pMdlRes, 0 /*,const_cast<gfl::grp::g3d::Model::Description*>(cp_model_desc)*/ );


  // モデル生成完了
  m_CreateModel = true;

  // モデルに位置を登録
  m_ModelDatas.h3d.m_pModel->SetTranslate( m_Trans );

  // シーンに登録
  m_pScene->AddH3dModelToCalc( m_ModelDatas.h3d.m_pModel, 0 );
  m_pScene->AddH3dModelToDraw( m_ModelDatas.h3d.m_pModel, 0 );
}
#endif // GFL_GRP_G3D_H3D_USE



//----------------------------------------------------------------------------
/**
 *	@brief  地形モデル破棄
 */
//-----------------------------------------------------------------------------
void System::DeleteModel( void )
{
  b32 result;

  if( m_CreateModel ){

    if( m_ModelMode == MODEL_DEFAULT ){
      // シーンから破棄
      m_pScene->RemoveSceneNode( m_ModelDatas.def.m_pModel );
    
      // モデルリソース破棄
      m_ModelDatas.def.m_pModel->Destroy();

      // リソースの破棄
      if( m_ModelDatas.def.m_pTexRes->IsSetup() ){
        result = m_ModelDatas.def.m_pTexRes->DetachBuffer();
        GFL_ASSERT_STOP( result );
      }
      result = m_ModelDatas.def.m_pMdlRes->DetachBuffer();
      GFL_ASSERT_STOP( result );
    }
    else {

#if GFL_GRP_G3D_H3D_USE
      // シーンから破棄
      m_pScene->RemoveH3dModelFromCalcAndAllDraw( m_ModelDatas.h3d.m_pModel );
    
      // モデルリソース破棄
      m_ModelDatas.h3d.m_pModel->Destroy();

      // リソースの破棄
      if( m_ModelDatas.h3d.m_pTexRes->IsSetup() )
      {
        m_ModelDatas.h3d.m_pTexRes->DetachBuffer();
      }
      m_ModelDatas.h3d.m_pMdlRes->DetachBuffer();
#endif // GFL_GRP_G3D_H3D_USE

    }

    // モデル破棄　藩領
    m_CreateModel = false;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  地形モデルの取得
 *
 *	@retval モデルオブジェクト
 */
//-----------------------------------------------------------------------------
gfl::grp::g3d::Model* System::GetModel( void ) const
{
  // モデルモードの不一致チェック
  if( !this->isModelMode( MODEL_DEFAULT ) ){GFL_ASSERT_STOP(0);}
  return m_ModelDatas.def.m_pModel;
}
#if GFL_GRP_G3D_H3D_USE
gfl::grp::g3d::H3dModel* System::GetH3dModel( void ) const
{
  // モデルモードの不一致チェック
  if( !this->isModelMode( MODEL_H3D ) ){GFL_ASSERT_STOP(0);}
  return m_ModelDatas.h3d.m_pModel;
}
#endif // GFL_GRP_G3D_H3D_USE

//----------------------------------------------------------------------------
/**
 *	@brief  地形モデルのモデルリソース取得
 *
 *	@retval モデルリソース
 */
//-----------------------------------------------------------------------------
const gfl::grp::g3d::Resource* System::GetResourceModel( void ) const
{
  // モデルモードの不一致チェック
  if( !this->isModelMode( MODEL_DEFAULT ) ){GFL_ASSERT_STOP(0);}
  return m_ModelDatas.def.m_pMdlRes;
}
#if GFL_GRP_G3D_H3D_USE
const gfl::grp::g3d::H3dResource* System::GetH3dResourceModel( void ) const
{
  // モデルモードの不一致チェック
  if( !this->isModelMode( MODEL_H3D ) ){GFL_ASSERT_STOP(0);}
  return m_ModelDatas.h3d.m_pMdlRes;
}
#endif // GFL_GRP_G3D_H3D_USE

//----------------------------------------------------------------------------
/**
 *	@brief  地形モデルのテクスチャリソース取得
 *
 *	@retval テクスチャリソース
 */
//-----------------------------------------------------------------------------
const gfl::grp::g3d::Resource* System::GetResourceTexture( void ) const
{
  // モデルモードの不一致チェック
  if( !this->isModelMode( MODEL_DEFAULT ) ){GFL_ASSERT_STOP(0);}
  return m_ModelDatas.def.m_pTexRes;
}
#if GFL_GRP_G3D_H3D_USE
const gfl::grp::g3d::H3dResource* System::GetH3dResourceTexture( void ) const
{
  // モデルモードの不一致チェック
  if( !this->isModelMode( MODEL_H3D ) ){GFL_ASSERT_STOP(0);}
  return m_ModelDatas.h3d.m_pTexRes;
}
#endif // GFL_GRP_G3D_H3D_USE


//----------------------------------------------------------------------------
/**
 *	@brief  読み込んだマップブロックのサイズを取得
 */
//-----------------------------------------------------------------------------
s32 System::GetLoadFileSize( void ) const
{
  return m_DivLoad.GetLoadFileSize();
}

//-----------------------------------------------------------------------------
/**
 *    protected メンバ関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  ワークのクリア処理
 */
//-----------------------------------------------------------------------------
void System::Clear( void )
{
  GFL_ASSERT( m_Initialize == false );

  m_Initialize      = false;
  m_ModelMode       = MODEL_DEFAULT;
  m_pScene          = NULL;
  m_Visible          = true;
  m_Trans           = gfl::math::VEC3(0,0,0);
  m_pArcFile        = NULL;
  m_ArcFileOriginal = NULL;
  m_DataID          = 0;
  m_pMapData        = NULL;
  m_MapDataHeapSize = 0;
  m_FileType        = 0;
  m_pFileControl    = NULL;
  m_cpFileControls  = NULL;
  m_VramAddrTex     = 0;
  m_VramAddrPltt    = 0;
  this->ReleaseGlobalTexResource();
  m_DivLoadSize     = 0;
  m_LoadStatus      = LOAD_STATE_IDLING;
}

//----------------------------------------------------------------------------
/**
 *	@brief  マップバッファ　初期化
 *
 *	@param	param         パラメータ
 *	@param  p_sysHeap     システムメモリ用ヒープ
 *	@param  p_deviceHeap  デバイスメモリ用ヒープ
 */
//-----------------------------------------------------------------------------
void System::InitializeMapBuffer( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap )
{
  GFL_UNUSED( p_sysHeap );
  
  m_MapDataHeapSize = param.mapDataHeapSize;
  m_pMapData        = GflHeapAllocMemoryAlign( p_deviceHeap, m_MapDataHeapSize, 128 );
  m_LinkData.Initialize( m_pMapData );
}

//----------------------------------------------------------------------------
/**
 *	@brief  マップバッファ　破棄
 */
//-----------------------------------------------------------------------------
void System::FinalizeMapBuffer( void )
{
  GflHeapFreeMemory( m_pMapData );
  m_pMapData        = NULL;
  m_MapDataHeapSize = 0;
  m_LinkData.Initialize( NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief  モデルバッファ　初期化
 *
 *	@param	param         パラメータ
 *	@param  p_sysHeap     システムメモリ用ヒープ
 *	@param  p_deviceHeap  デバイスメモリ用ヒープ
 */
//-----------------------------------------------------------------------------
void System::InitializeModelBuffer( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap )
{
  GFL_UNUSED( param );
  GFL_UNUSED( p_deviceHeap );
  if( m_ModelMode == MODEL_DEFAULT ){
    m_ModelDatas.def.m_pTexRes = GFL_NEW(p_sysHeap) gfl::grp::g3d::Resource();
    m_ModelDatas.def.m_pMdlRes = GFL_NEW(p_sysHeap) gfl::grp::g3d::Resource();
    m_ModelDatas.def.m_pModel  = GFL_NEW(p_sysHeap) gfl::grp::g3d::Model();
  }
  else{
#if GFL_GRP_G3D_H3D_USE
    m_ModelDatas.h3d.m_pTexRes = GFL_NEW(p_sysHeap) gfl::grp::g3d::H3dResource();
    m_ModelDatas.h3d.m_pMdlRes = GFL_NEW(p_sysHeap) gfl::grp::g3d::H3dResource();
    m_ModelDatas.h3d.m_pModel  = GFL_NEW(p_sysHeap) gfl::grp::g3d::H3dModel();
#endif // GFL_GRP_G3D_H3D_USE
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  モデルバッファ 破棄
 */
//-----------------------------------------------------------------------------
void System::FinalizeModelBuffer( void )
{
  DeleteModel();
  
  if( m_ModelMode == MODEL_DEFAULT ){
    if(m_ModelDatas.def.m_pModel){
      GFL_DELETE m_ModelDatas.def.m_pModel;
      GFL_DELETE m_ModelDatas.def.m_pMdlRes;
      GFL_DELETE m_ModelDatas.def.m_pTexRes; 

      m_ModelDatas.def.m_pModel  = NULL;
      m_ModelDatas.def.m_pMdlRes = NULL;
      m_ModelDatas.def.m_pTexRes = NULL;
    }
  }
  else{
#if GFL_GRP_G3D_H3D_USE
    if(m_ModelDatas.h3d.m_pModel){
      GFL_DELETE m_ModelDatas.h3d.m_pModel;
      GFL_DELETE m_ModelDatas.h3d.m_pMdlRes;
      GFL_DELETE m_ModelDatas.h3d.m_pTexRes; 

      m_ModelDatas.h3d.m_pModel  = NULL;
      m_ModelDatas.h3d.m_pMdlRes = NULL;
      m_ModelDatas.h3d.m_pTexRes = NULL;
    }
#endif // GFL_GRP_G3D_H3D_USE
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  モデルデータ用VRAM確保処理
 *
 *	@param	param         パラメータ
 *	@param  p_sysHeap     システムメモリ用ヒープ
 *	@param  p_deviceHeap  デバイスメモリ用ヒープ
 */
//-----------------------------------------------------------------------------
void System::InitializeVramAddr( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap )
{
  // ハードによって、変わる。
  GFL_UNUSED(p_sysHeap);
  GFL_UNUSED(p_deviceHeap);

  // Defaultのサポートはなし
  GFL_ASSERT(param.texVramSize == 0); //@check
  GFL_ASSERT(param.plttVramSize == 0); //@check
}

//----------------------------------------------------------------------------
/**
 *	@brief  VRAM確保領域のクリア
 */
//-----------------------------------------------------------------------------
void System::FinalizeVramAddr( void )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  分割ロードシステムの初期化
 *
 *	@param	param         パラメータ
 *	@param  p_sysHeap     システムヒープ
 *	@param  p_deviceHeap  デバイスヒープ
 */
//-----------------------------------------------------------------------------
void System::InitializeDivLoad( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap )
{
  GFL_UNUSED( param );
  GFL_UNUSED( p_sysHeap );
  GFL_UNUSED( p_deviceHeap );
  
  m_DivLoadSize = param.divLoadSize;
  m_DivLoad.Initialize( m_pMapData, m_MapDataHeapSize, p_sysHeap, param.divMode, param.threadPri );
}

//----------------------------------------------------------------------------
/**
 *	@brief  分割ロードシステムの破棄
 */
//-----------------------------------------------------------------------------
void System::FinalizeDivLoad( void )
{
  m_DivLoad.Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  分割読み込みの状態遷移
 */
//-----------------------------------------------------------------------------
void System::UpdateLoad( void )
{
  // 読み込み完了チェック
  if( (m_LoadStatus == LOAD_STATE_END) || 
      (m_LoadStatus == LOAD_STATE_IDLING) ){
    return ;
  }
  
  switch( m_LoadStatus ){

  case LOAD_STATE_START:             // 読み込み開始
    m_LoadStatus  = LOAD_STATE_LOAD;
    // break through
    
  case LOAD_STATE_LOAD:              // 読み込み中
    if( m_DivLoad.ContinueFileLoad() != false ){
      // SetUpへ
      m_LoadStatus  = LOAD_STATE_SETUP;

      // シグネチャからファイルコントロール取得
      m_pFileControl = m_cpFileControls->GetFileControl( m_LinkData.GetSignature() );
    }
    break;

  case LOAD_STATE_SETUP:             // セットアップ中
    
    if( m_pFileControl ){
      if( m_pFileControl->SetUp( this ) != false ){
        // セットアップ完了
        m_LoadStatus = LOAD_STATE_END;
      }
    }else{
      m_LoadStatus = LOAD_STATE_END;
    }
    break;

  
  case LOAD_STATE_IDLING:        // 待機中
  case LOAD_STATE_END:           // 完了（データが入っている状態）
  default:
    break;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  モデルモードの一致チェック
 */
//-----------------------------------------------------------------------------
bool System::isModelMode( ModelMode mode ) const 
{
  if( mode == m_ModelMode ){
    return true;
  }
  return false;
}





} // namespace map
} // namespace grp
} // namespace gfl
