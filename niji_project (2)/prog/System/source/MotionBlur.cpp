
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		MotionBlur.cpp
 *	@brief  モーションブラー
 *	@author	tomoya takahashi
 *	@date		2014.03.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "System/include/MotionBlur.h"
#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Mtx.h>
#include <math/include/gfl2_math_convert.h>
#include <math/include/gfl2_MathTriangular.h>

#if PM_DEBUG
#include  <gfx/include/gfl2_PCDebugUtil.h>
#endif // PM_DEBUG

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>


#include "arc_def_index/arc_def.h"

#include "niji_conv_header/motion_blur/motion_blur_garc_ctr.h"  // garc直下のBinlinkerアクセス用
#include "niji_conv_header/motion_blur/motion_blur_ctr.h"       // exports以下の特定のデータアクセス用
#include "niji_conv_header/motion_blur/motion_blur_mot.h"       // モーションヘッダー

#include "System/include/HeapDefine.h"

namespace System
{

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
// アニメーションインデックス
const u32 MotionBlur::BlendAnimationIndex[ MotionBlur::BLEND_MAX ] = 
{
  MOTION_BLUR_BLUR_NORMAL_GFMOT,
  MOTION_BLUR_BLUR_CIRCLE_GFMOT,
  MOTION_BLUR_BLUR_NEAR_GFMOT,
};

const u32 MotionBlur::AnimationIndex[ MotionBlur::ANIM_MAX ] = 
{
  MOTION_BLUR_ANIM_NONE_GFMOT,
  MOTION_BLUR_ANIM_SCALE_GFMOT,
};
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

  // コンストラクタ・デストラクタ
  MotionBlur::MotionBlur(void) : 
    gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath(),
    m_pDevice(NULL),
    m_pDeviceHeap(NULL),
    m_RenderOff(true),
    m_BlendType(BLEND_PLANE),
    m_AnimeType(ANIM_NONE),
    m_cpTargetSurface(NULL),
    m_DrawMsk(false),
    m_IsInitialized(false),
    m_IsSceneRenderPathInitialized(false),
    m_pResourceBuff(NULL),
    m_pResource(NULL),
    m_pModel(NULL),
    m_AnimPackList()
  {
  }

  MotionBlur::~MotionBlur()
  {
    Terminate();
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  初期化
   *  @param  p_device    デバイスヒープ
   *  @param  p_fileread  ファイルリードマネージャー
   *  @param  useTextureTarget  テンポラリのカラーバッファとして使用するFadeManagerの面(gfl2::Fade::FadeManager::COLOR_BUFFER_UPPER or  gfl2::Fade::FadeManager::COLOR_BUFFER_LOWER)
   */
  //-----------------------------------------------------------------------------
  void MotionBlur::InitializeMotionBlur( gfl2::gfx::IGLAllocator*  p_deviceAllocator, gfl2::heap::HeapBase* p_device, gfl2::fs::AsyncFileManager* p_fileread, u32 useTextureTarget )
  {
    GFL_ASSERT( m_pDevice == NULL );

    m_pDevice     = p_deviceAllocator;
    m_pDeviceHeap = p_device;


    // Pathの初期化
    if(m_IsSceneRenderPathInitialized == false) // インスタンスが破棄されるまでに1回でよい。
    {
      // コンテナ
      m_DrawableNodeContainer.CreateBuffer(1, m_pDevice);

      gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
      desc.m_DrawManagerDesc.m_DrawEnvStackSize = 1;
      desc.m_DrawManagerDesc.m_DrawTagStackSize = 1;
      desc.m_DrawManagerDesc.m_NodeStackSize    = 1;
      this->Initialize( m_pDevice, desc );
      m_IsSceneRenderPathInitialized = true; 
    }


    // MotionBlur初期化
    m_pFileReadManager  = p_fileread;
    m_UseTextureTarget = useTextureTarget;

    m_BlendControlParam.type = TYPE_SET;
    m_BlendControlParam.end = 0.50f;
    m_BlendControlParam.frame = 0;
    m_BlendControlNowBlend = 0.50f;
    m_BlendControlStartBlend = 0.50f;
    m_BlendControlFrame = 0;

    m_ColorControlParam.type = TYPE_SET;
    m_ColorControlParam.end = gfl2::math::Vector3(0,0,0);
    m_ColorControlParam.frame = 0;
    m_ColorControlNowColor = gfl2::math::Vector3(0,0,0);
    m_ColorControlStartColor = gfl2::math::Vector3(0,0,0);
    m_ColorControlFrame = 0;

    //---------------------------------------------------
    // ARCファイルオープン
    //---------------------------------------------------
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId = ARCID_MOTION_BLUR;
      openReq.heapForFile = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD);
      openReq.heapForArcSrc = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD);
      openReq.heapForReq = p_device->GetLowerHandle();
      m_pFileReadManager->AddArcFileOpenReq( openReq );
    }
    //---------------------------------------------------
    // ファイル読み込みリクエスト
    //---------------------------------------------------
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

      loadReq.arcId = ARCID_MOTION_BLUR;
      loadReq.datId = 0;
      loadReq.ppBuf = &m_pResourceBuff;
      loadReq.heapForBuf = p_device;
      loadReq.heapForReq = p_device->GetLowerHandle();
      loadReq.heapForCompressed = p_device->GetLowerHandle();
      loadReq.pBufSize = NULL;
      loadReq.align = 128;
      m_pFileReadManager->AddArcFileLoadDataReq( loadReq );
    }


    // カメラ生成 正射影
    {
      f32 orthoL =-200.0f;     // ニアクリッピング面の視錐台左辺のX座標
      f32 orthoR = 200.0f;     // ニアクリッピング面の視錐台右辺のX座標
      f32 orthoB =-120.0f;     // ニアクリッピング面の視錐台下辺のY座標
      f32 orthoT = 120.0f;     // ニアクリッピング面の視錐台上辺のY座標
      m_camera.m_View = gfl2::math::Matrix34(
        gfl2::math::Vector4( 1.0f, 0.0f, 0.0f, 0.0f),
        gfl2::math::Vector4( 0.0f, 1.0f, 0.0f, 0.0f),
        gfl2::math::Vector4( 0.0f, 0.0f, 1.0f, -10.0f) );

      nw::math::MTX44 proj;
      gfl2::math::MTX44OrthoPivot( 
        &proj,
        orthoL,
        orthoR,
        orthoB,
        orthoT,
        0.0f,
        1000.0f,
        gfl2::math::PIVOT_UPSIDE_TO_TOP );
      m_camera.m_Projection = gfl2::math::ConvertNwMtx44ToGfMtx44( proj );
    }

    // キーとなるノード生成
    {
      gfl2::renderingengine::scenegraph::instance::TransformNode *node = gfl2::renderingengine::scenegraph::instance::TransformNode::Create( m_pDevice );
      node->SetWorldMatrix( gfl2::math::Matrix34::GetIdentity() );
      node->SetLocalScale( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
      m_node = node;
    }

    // アニメーションパックリスト
    {
      m_AnimPackList.Initialize( p_device, 1 );
    }

  } 

  //----------------------------------------------------------------------------
  /**
   *	@brief  初期化更新処理
   *
   *	@retval true  完了
   *	@retval false 途中
   */
  //-----------------------------------------------------------------------------
  bool MotionBlur::InitializeMotionBlurUpdate( void )
  {
    // Initializeが呼ばれていない。
    if( !m_pDevice ){
      GFL_ASSERT(0);
      return true;
    }

    // 読み込み完了待ち
    if( !m_pFileReadManager->IsArcFileOpenFinished( ARCID_MOTION_BLUR ) ){
      return false;
    }
    if( !m_pFileReadManager->IsArcFileLoadDataFinished( &m_pResourceBuff ) ){
      return false;
    }

    // すでにセットアップ済み
    if( m_pResource ){
      return true;
    }

    // リンカーに設定
    m_BinLinker.Initialize( m_pResourceBuff );
    m_BinLinker.Initialize( m_BinLinker.GetData( BL_IDX_MOTION_BLUR_GARC_CTR_MOTION_BLUR_CTR_PACK ) );

    // 読み込み完了　セットアップ
    // モデルリソース
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
      resourceData.SetModelData( static_cast<c8*>( m_BinLinker.GetData(BL_IDX_MOTION_BLUR_CTR_MOTION_BLUR_CTR_GFBMDLP) ) );
      m_pResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pDevice, &resourceData );
    }

    // アニメーション生成
    {
      c8 * pAnime;
      pAnime = reinterpret_cast<c8*>(m_BinLinker.GetData(BL_IDX_MOTION_BLUR_CTR_MOTION_BLUR_GFBMOTP));
      m_AnimPackList.LoadData( 0, m_pDevice, m_pDeviceHeap, pAnime );
    }

    // モデル生成
    {
      m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pDevice, m_pResource );
      m_pModelInstanceNodeDrawableNode = reinterpret_cast<gfl2::renderingengine::scenegraph::instance::DrawableNode*>(m_pModelInstanceNode);
      m_pModel = GFL_NEW(m_pDeviceHeap) poke_3d::model::BaseModel();
      m_pModel->Create( m_pDevice, m_pDeviceHeap, m_pModelInstanceNode );
      m_pModel->SetAnimationPackList( &m_AnimPackList );

      // シーンから外す。
      m_node->AddChild(m_pModelInstanceNode);
    }

    // WIPEからテクスチャのバッファを取得
    {
      m_pOldRenderBuffer = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->GetColorBufferTexture( m_UseTextureTarget );

      //testコード コピー先を自分で作る。
#if 0 
      m_pOldRenderBuffer = gfl2::gfx::GFGL::CreateTexture(
        m_pDevice,
        256, 512, 1,
        gfl2::gfx::Usage::RenderTarget,
        gfl2::gfx::Format::A8R8G8B8,
        gfl2::gfx::Pool::System
      );
#endif

    }

    // モデルのテクスチャを差し替える。
    {
      // モデルにテクスチャを適応
      gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* materialNode = m_pModelInstanceNode->GetMaterialInstanceNode( 0 );
      const c8* pFactoryName;
      pFactoryName = gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory::GetFactoryTypeName();
      gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory;
      pTextureResourceNodeFactory = reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory*>( gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory( gfl2::renderingengine::scenegraph::resource::IResourceData::Type( pFactoryName ) ) );
      m_pOldRenderBufferResource = reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode*>(pTextureResourceNodeFactory->CreateResourceNode(m_pDevice, "capture",m_pOldRenderBuffer,false));
      materialNode->SetTextureResourceNode(0, m_pOldRenderBufferResource);

#if defined(GF_PLATFORM_WIN32)
      // Win32ではUV値を90度回転させる。
      m_pModel->SetMaterialTextureCoordinatorRotateUV(0, 0, GFL_MATH_DEG_TO_RAD(90));
      // UV U成分を平行移動
      gfl2::math::Vector2 trans( 0.0f, -0.218750f );
      m_pModel->SetMaterialTextureCoordinatorTranslateUV(0, 0, trans);
#endif
    }


    m_IsInitialized = true;

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  破棄
   */
  //-----------------------------------------------------------------------------
  void MotionBlur::Terminate(void)
  {
    if( !m_IsInitialized ) return;

    GFL_ASSERT_STOP( m_RenderOff );


    // モデル破棄
    if(m_pModel) GFL_DELETE m_pModel; m_pModel = NULL;
    m_pModelInstanceNode = NULL;
    m_pModelInstanceNodeDrawableNode = NULL;

    // アニメーション破棄
    m_AnimPackList.Finalize();

    // テクスチャ破棄
    if(m_pOldRenderBufferResource) GFL_DELETE m_pOldRenderBufferResource; m_pOldRenderBufferResource = NULL;
    m_pOldRenderBuffer = NULL;


    // リソース破棄
    if(m_pResource) GFL_DELETE m_pResource; m_pResource = NULL;
    
    if(m_pResourceBuff) GflHeapFreeMemory(m_pResourceBuff); m_pResourceBuff = NULL;


    m_BinLinker.Initialize(NULL);

    // キーとなるノード破棄
    if( m_node ) GFL_DELETE m_node; m_node = NULL;


    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_MOTION_BLUR;
      closeReq.heapForReq = m_pDeviceHeap->GetLowerHandle();
      m_pFileReadManager->SyncArcFileClose( closeReq );
    }

    m_pDevice = NULL;

    m_DrawMsk = false;
    m_IsLoop = false;

    m_IsInitialized = false;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  アニメーション更新処理
   */
  //-----------------------------------------------------------------------------
  void MotionBlur::UpdateMotionBlur( void )
  {
    if( !m_IsInitialized ) return ;

    BlendControl();
    ColorControl();

    if( m_pModel->HasAnimationController() )
    {
      m_pModel->UpdateAnimation();
    }

    // 更新
    gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFastFromChild( m_node);

  }

  // 
  //----------------------------------------------------------------------------
  /**
   *	@brief  表示ON
   *	@param	blend       ブレンドタイプ
   *	@param	anime       アニメーションタイプ
   *	@param	cpTargetSurface 描画ターゲットとなるサーフェース
   *  @param  is_Loop     ループ再生するか？
   */
  //-----------------------------------------------------------------------------
  void MotionBlur::RenderStart( BlendType blend, AnimeType anime, const gfl2::gfx::Surface* cpTargetSurface, bool is_loop )
  {
    if( !m_IsInitialized ) return ;

    // 元々レンダリングOFFのとき
    if(m_RenderOff){
      // シーンにモデルを登録
	    m_DrawableNodeContainer.push_back( m_pModelInstanceNodeDrawableNode );

      //countアップ開始
      m_DrawMsk = false;
    }

    m_RenderOff = false;

    if( blend < BLEND_MAX ){
      m_BlendType = blend;
    }
    else{
      GFL_ASSERT( blend < BLEND_MAX );
      m_BlendType = BLEND_PLANE;
    }

    if( anime < ANIM_MAX ){
      m_AnimeType = anime;
    }
    else{
      GFL_ASSERT( anime < ANIM_MAX );
      m_AnimeType = ANIM_NONE;
    }
    m_IsLoop = is_loop;
    m_cpTargetSurface = cpTargetSurface;

    // モデルにアニメーションを適応
    m_pModel->ChangeAnimation( AnimationIndex[m_AnimeType], 0 );
    m_pModel->ChangeAnimation( BlendAnimationIndex[m_BlendType], 1 );
    m_pModel->SetAnimationStepFrame( 1.0f, 0 );
    m_pModel->SetAnimationStepFrame( 1.0f, 1 );
    m_pModel->SetAnimationLoop( m_IsLoop, 0 );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief    アニメーションの完了を待つ
   *
   *	@retval true  完了
   *	@retval false 途中
   */
  //-----------------------------------------------------------------------------
  b32 MotionBlur::IsAnimeEnd( void )
  {
    if( !m_IsInitialized ) return true;

    if(m_RenderOff){
      return true;
    }
    return m_pModel->IsAnimationLastFrame( 0 );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  表示OFFか？
   */
  //-----------------------------------------------------------------------------
  bool MotionBlur::IsRenderOff(void)
  {
    if( !m_IsInitialized ) return true;
    return m_RenderOff;
  }

  // 
  //----------------------------------------------------------------------------
  /**
   *	@brief  表示OFF (Finalizeの1フレーム前に必ずコール）
   */
  //-----------------------------------------------------------------------------
  void MotionBlur::RenderOff(void)
  {
    if( !m_IsInitialized ) return;

    if( m_RenderOff == false ){
      // シーンからモデルを削除
	    m_DrawableNodeContainer.remove( m_pModelInstanceNodeDrawableNode );
      m_cpTargetSurface = NULL;
    }

    m_RenderOff = true;
  }

  /**
   *  @brief 描画実行
   */
  void MotionBlur::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
  {
    UpdateMotionBlur();
    SetCamera(rDrawContext);
    SetDrawableNode();
    m_pDrawManager->Update();
  }
  void MotionBlur::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
  {
    // レンダリングすべき面かチェック
    if( !m_IsInitialized ) return;
    if( m_RenderOff ) return ;
    if( rDrawContext.m_pRenderTarget != m_cpTargetSurface ) return ;

    // キャプチャーを一回待つ。
    if( m_DrawMsk == false )
    {
      m_DrawMsk = true;
    }
    else
    {

      // 描画
      SetupDraw( rDrawContext );
      DrawManagerDraw(false);
    }

    // レンダリング結果のコピー命令
    gfl2::gfx::GFGL::AddReadColorBufferToTextureCommand( m_pOldRenderBuffer );

#if PM_DEBUG
    /*
#if defined(GF_PLATFORM_WIN32)
    void * addr = m_pOldRenderBuffer->Lock();
    gfl2::gfx::PCDebugUtil::PrintBlockImage( addr, "screenshot/colorbuffer_block.bmp", 512, 256, 4, m_pDevice->GetHeap() );
    gfl2::gfx::PCDebugUtil::PrintImage( addr, "screenshot/colorbuffer_liner.bmp", 512, 256, 4, m_pDevice->GetHeap() );
    m_pOldRenderBuffer->UnLock();
#elif defined(GF_PLATFORM_CTR)
    void * addr = m_pOldRenderBuffer->Lock();
    gfl2::gfx::PCDebugUtil::PrintBlockImage( addr, "screenshot/colorbuffer_block_ctr.bmp", 512, 256, 4, m_pDevice->GetHeap() );
    gfl2::gfx::PCDebugUtil::PrintImage( addr, "screenshot/colorbuffer_liner_ctr.bmp", 512, 256, 4, m_pDevice->GetHeap() );
    m_pOldRenderBuffer->UnLock();
#endif
    */
#endif // PM_DEBUG
  }

  /**
   *  @brief 描画物の設定
   */
	const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* MotionBlur::DrawableNodeOverride()
  {
    return &m_DrawableNodeContainer;
  }

  /**
   *  @brief カメラ上書き
   */
  const gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* MotionBlur::CameraOverride( void )
  {
    return &m_camera; 
  }


  // コマンドの再利用モード(使用できない）
	void MotionBlur::CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
  {
    //@note 再利用は非対応
    return ;
  }

  // レンダーターゲットをクリアしない。
  void MotionBlur::ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
  {
    //空実装。つまり何もしない＝クリアしない。
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  ブレンド比コントロールを開始
   */
  //-----------------------------------------------------------------------------
  void MotionBlur::SetBlendControl( const Param& param )
  {
    if( !m_IsInitialized ) return;

    m_BlendControlParam = param;
    m_BlendControlParam.end = gfl2::math::Clamp(param.end+0.50f, 0.0f, 1.0f); // 0.0f ～　1.0fの範囲に変更する。
    m_BlendControlStartBlend = m_BlendControlNowBlend;
    m_BlendControlFrame = 0;

    // SETの場合この場で値を反映する。
    if( m_BlendControlParam.type == TYPE_SET ){
      m_BlendControlNowBlend = m_BlendControlParam.end;
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ブラー比コントロールアニメの完了チェック
   *
   *	@retval true  終了
   *	@retval false 途中
   */
  //-----------------------------------------------------------------------------
  bool MotionBlur::IsEndBlendControl( void ) const
  {
    if( !m_IsInitialized ) return true;
    
    if( m_BlendControlFrame >= m_BlendControlParam.frame ){
      return true;
    }
    return false;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  今のブレンド比を取得
   */
  //-----------------------------------------------------------------------------
  f32 MotionBlur::GetBlendControl( void ) const
  {
    if( !m_IsInitialized ) return 0.0f;
    return m_BlendControlNowBlend;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ブラーカラーアニメの設定
   */
  //-----------------------------------------------------------------------------
  void MotionBlur::SetColorFilter( const ColorParam& param )
  {
    if( !m_IsInitialized ) return;

    m_ColorControlParam = param;
    m_ColorControlStartColor = m_ColorControlNowColor;
    m_ColorControlFrame = 0;

    // SETの場合この場で値を反映する。
    if( m_ColorControlParam.type == TYPE_SET ){
      m_ColorControlNowColor = m_ColorControlParam.end;
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ブラーカラーアニメの完了チェック
   */
  //-----------------------------------------------------------------------------
  bool MotionBlur::IsEndColorFilter( void ) const
  {
    if( !m_IsInitialized ) return true;
    
    if( m_ColorControlFrame >= m_ColorControlParam.frame ){
      return true;
    }
    return false;
  }


  // 
  //----------------------------------------------------------------------------
  /**
   *	@brief  ブレンド比コントロール
   */
  //-----------------------------------------------------------------------------
  void MotionBlur::BlendControl(void)
  {
    static f32 (*pFunc[TYPE_END])( f32, f32 , f32 , s32 , s32 ) =
    {
      &CalcSet,
      &CalcLinear,
      &CalcSpline,
      &CalcQuad,
      &CalcLog10,
    };

    // フレームコントロール
    if( !this->IsEndBlendControl() ){
      m_BlendControlFrame ++;

      // 最終フレームになったら、最終の値にする。
      if( m_BlendControlFrame == m_BlendControlParam.frame ){
        m_BlendControlNowBlend = m_BlendControlParam.end;
      }
      else{

        if( m_BlendControlParam.type < TYPE_END ){
          m_BlendControlNowBlend = pFunc[ m_BlendControlParam.type ]( m_BlendControlParam.end, m_BlendControlStartBlend, m_BlendControlNowBlend, 
              m_BlendControlFrame, m_BlendControlParam.frame );
        }
        else{
          GFL_ASSERT(0);  // 補間タイプが不正
          m_BlendControlNowBlend = pFunc[ TYPE_SET ]( m_BlendControlParam.end, m_BlendControlStartBlend, m_BlendControlNowBlend, 
              m_BlendControlFrame, m_BlendControlParam.frame );
        }
      }

      f32 add = m_BlendControlNowBlend;
      add = gfl2::math::Clamp( add, 0.0f, 1.0f );

      gfl2::math::Vector4 color;
      color = m_pModel->GetMaterialColorConstant(0,0);
      color.SetW(add);
      m_pModel->SetMaterialColorConstant(0,0,color);
    }
  }

  // 
  //----------------------------------------------------------------------------
  /**
   *	@brief  カラーフィルターコントロール
   */
  //-----------------------------------------------------------------------------
  void MotionBlur::ColorControl(void)
  {
    static f32 (*pFunc[TYPE_END])( f32, f32 , f32 , s32 , s32 ) =
    {
      &CalcSet,
      &CalcLinear,
      &CalcSpline,
      &CalcQuad,
      &CalcLog10,
    };

    // フレームコントロール
    if( !this->IsEndColorFilter() ){
      m_ColorControlFrame ++;

      // 最終フレームになったら、最終の値にする。
      if( m_ColorControlFrame == m_ColorControlParam.frame ){
        m_ColorControlNowColor = m_ColorControlParam.end;
      }
      else{

        Type type = TYPE_SET;

        if( m_ColorControlParam.type < TYPE_END ){
          type = m_ColorControlParam.type;
        }
        else{
          GFL_ASSERT(0);  // 補間タイプが不正
        }

        m_ColorControlNowColor.x = pFunc[ type ]( m_ColorControlParam.end.x, m_ColorControlStartColor.x, m_ColorControlNowColor.x, 
            m_ColorControlFrame, m_ColorControlParam.frame );

        m_ColorControlNowColor.y = pFunc[ type ]( m_ColorControlParam.end.y, m_ColorControlStartColor.y, m_ColorControlNowColor.y, 
            m_ColorControlFrame, m_ColorControlParam.frame );

        m_ColorControlNowColor.z = pFunc[ type ]( m_ColorControlParam.end.z, m_ColorControlStartColor.z, m_ColorControlNowColor.z, 
            m_ColorControlFrame, m_ColorControlParam.frame );
      }

      // 加算 color0
      // 減算 color1
      gfl2::math::Vector4 color0;
      gfl2::math::Vector4 color1;
      color0 = m_pModel->GetMaterialColorConstant(0,0);
      color1 = m_pModel->GetMaterialColorConstant(0,1);

      gfl2::math::Vector3 add = m_ColorControlNowColor;

      if( add.x >= 0.0f )
      {
        add.x = gfl2::math::Clamp( add.x, 0.0f, 1.0f );
        color0.x = add.x;
        color1.x = 0.0f;
      }
      else
      {
        add.x = gfl2::math::Clamp( -add.x, 0.0f, 1.0f );
        color1.x = add.x;
        color0.x = 0.0f;
      }

      if( add.y >= 0.0f )
      {
        add.y = gfl2::math::Clamp( add.y, 0.0f, 1.0f );
        color0.y = add.y;
        color1.y = 0.0f;
      }
      else
      {
        add.y = gfl2::math::Clamp( -add.y, 0.0f, 1.0f );
        color1.y = add.y;
        color0.y = 0.0f;
      }

      if( add.z >= 0.0f )
      {
        add.z = gfl2::math::Clamp( add.z, 0.0f, 1.0f );
        color0.z = add.z;
        color1.z = 0.0f;
      }
      else
      {
        add.z = gfl2::math::Clamp( -add.z, 0.0f, 1.0f );
        color1.z = add.z;
        color0.z = 0.0f;
      }

      m_pModel->SetMaterialColorConstant(0,0,color0);
      m_pModel->SetMaterialColorConstant(0,1,color1);
    }
  }

  // 各タイプの関数
  f32 MotionBlur::CalcSet( f32 end, f32 /*start*/, f32 /*now*/, s32 /*frame*/, s32 /*frame_max*/ )
  {
    return end;
  }

  f32 MotionBlur::CalcLinear( f32 end, f32 start, f32 /*now*/, s32 frame, s32 frame_max )
  {
    f32 rate = static_cast<f32>(frame) / static_cast<f32>(frame_max);
    rate = gfl2::math::Clamp(rate, 0.0f, 1.0f);
    return start + ((end - start) * rate);
  }

  f32 MotionBlur::CalcSpline( f32 end, f32 start, f32 /*now*/, s32 frame, s32 frame_max )
  {
    f32 rate = (-gfl2::math::CosDeg( ((180.0f * static_cast<f32>(frame)) / static_cast<f32>(frame_max)) ) * 0.50f) + 0.50f;
    rate = gfl2::math::Clamp(rate, 0.0f, 1.0f);
    return start + ((end - start) * rate);
  }

  f32 MotionBlur::CalcQuad( f32 end, f32 start, f32 /*now*/, s32 frame, s32 frame_max )
  {
    f32 n = static_cast<f32>(frame) / static_cast<f32>(frame_max);
    f32 rate = (n > 0.0f) ? n * n : 0.0f;
    rate = gfl2::math::Clamp(rate, 0.0f, 1.0f);
    return start + ((end - start) * rate);
  }

  f32 MotionBlur::CalcLog10( f32 end, f32 start, f32 /*now*/, s32 frame, s32 frame_max )
  {
    f32 n = static_cast<f32>(frame) / static_cast<f32>(frame_max);
    f32 rate = (n > 0.0f) ? log10f( 1.0f + (n*9.0f) ) : 0.0f;
    rate = gfl2::math::Clamp(rate, 0.0f, 1.0f);
    return start + ((end - start) * rate);
  }
  
}
