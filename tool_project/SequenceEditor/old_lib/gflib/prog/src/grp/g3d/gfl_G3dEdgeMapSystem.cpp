//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dEdgeMapSystem.cpp
 *	@brief  エッジシステム
 *	@author	Masateru Ishiguro
 *	@date		2011.04.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>

//  gflib grp
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dEdgeMapSystem.h>
#include <grp/g3d/g3d_common.gaix>

#include "grp/g3d/gfl_G3dH3dUserData.h"
#include "core/gfl_MathBase.h"


namespace gfl {
namespace grp {
namespace g3d {

#define			MATERIAL_ID(a, b, c, d)		(d << 24) | (c << 16) | (b << 8) | (a << 0)
  
//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void EdgeMapPreRenderCallbackSlot::Invoke(nw::gfx::Model* model, nw::gfx::ResMesh mesh, nw::gfx::RenderContext* renderContext)
{
  if ( m_fEnable == false )
    return;
  
  nw::gfx::Material                     *setting_material;
  const s32                             index = mesh.GetMaterialIndex();
  nw::gfx::Material                     *target_material = model->GetMaterial( index );
  nw::gfx::res::ResMaterial             target_res_material = target_material->GetOriginal();
  b32                                   fTextureEnable = false;
  nw::gfx::res::ResTexture              target_texture;
  s32                                   edgeType = EdgeType::DefaultEdgeTypeID;
  
  {//シェーダーの選択
    nw::gfx::ResPixelBasedTextureMapper   target_res_tex_mapper;
    
    {
      s32   userDataIndex= target_res_material.GetUserDataIndex("EdgeMapAlphaMask");

      if ( userDataIndex >= 0 )
      {
        nw::ut::ResMetaData                   res_meta_data = target_res_material.GetUserData(userDataIndex);

        UserData    user_data;
        user_data.SetNwResMetaData(res_meta_data);
        s32         maskType = user_data.GetIntValue(0);

        if ( maskType != EdgeMapAlphaMask::None )
        {
          target_res_tex_mapper = target_res_material.GetTextureMappers( maskType - EdgeMapAlphaMask::Texture0 );
        }
      }
    }

    if ( target_res_tex_mapper.IsValid() )
    {
      target_texture = target_res_tex_mapper.GetTexture();
      if ( target_texture.IsValid() && target_texture.Dereference().IsValid() )
        fTextureEnable = true;
    }

    {
      s32   edgeModelNo = ( fTextureEnable ) ? ALPHA_EDGE_MODEL_VCOL_A : DEFAULT_EDGE_MODEL_VCOL_A;
      s32   userDataIndex= target_res_material.GetUserDataIndex("EdgeType");

      if ( userDataIndex >= 0 )
      {
        nw::ut::ResMetaData                   res_meta_data = target_res_material.GetUserData(userDataIndex);

        UserData    user_data;
        user_data.SetNwResMetaData(res_meta_data);
        edgeType = user_data.GetIntValue(0);
        
        switch( edgeType ){
        case EdgeType::VColorEdgeTypeID:
          edgeModelNo = ( fTextureEnable ) ? ALPHA_DISABLE_NORMAL_EDGE_MODEL_VCOL_A : DISABLE_NORMAL_EDGE_MODEL_VCOL_A;
          break;
        case EdgeType::Erace:
          edgeModelNo = ( fTextureEnable ) ? ALPHA_DISABLE_NORMAL_EDGE_DISABLE_MODEL_VCOL_A : DISABLE_NORMAL_EDGE_DISABLE_MODEL_VCOL_A;
          break;
        case EdgeType::OwnColor:// 自前で塗る（のでマテリアル切り替えを行わない
          return;
        case EdgeType::DefaultEdgeTypeID:
        default:
          break;
        }
      }

      //edgeModelNo = DISABLE_NORMAL_EDGE_DISABLE_MODEL_VCOL_A;
      setting_material = m_pGfxEdgeModel[edgeModelNo]->GetNwModel()->GetMaterial(0);
    }
  }
  
  
  if ( m_MorphingEnable )
  {//モーフィングに対応
    
  }

  if ( fTextureEnable )
  {//α抜きテクスチャ設定
    nw::gfx::ResMaterial                  setting_res_material = setting_material->GetTextureMapperResMaterial();
    nw::gfx::ResPixelBasedTextureMapper   setting_res_tex_mapper = setting_res_material.GetTextureMappers(0);

    // ターゲットカラーテクスチャ設定（α抜きに使用します。）
    setting_res_tex_mapper.SetTexture( target_texture );

    // テクスチャを変更した場合はテクスチャマッパーのハッシュを０に設定します。
    setting_res_material.SetTextureMappersHash(0x0);
  }
  
  {//深度テスト情報コピー
    nw::gfx::ResMaterial            setting_res_material = setting_material->GetFragmentOperationResMaterial();
    nw::gfx::ResFragmentOperation   settingFragmentOperation = setting_res_material.GetFragmentOperation();
    nw::gfx::ResDepthOperation      settingResDepthOperation = settingFragmentOperation.GetDepthOperation();    
    nw::gfx::ResFragmentOperation   targetFragmentOperation = target_res_material.GetFragmentOperation();
    nw::gfx::ResDepthOperation      targetResDepthOperation = targetFragmentOperation.GetDepthOperation();
    
    u32                                     frag = targetResDepthOperation.GetFlags();
    nw::gfx::ResDepthOperation::TestFunc    func = targetResDepthOperation.GetTestFunc();
    
    if ( edgeType == EdgeType::None )
    {
      func = nw::gfx::ResDepthOperation::TESTFUNC_NEVER; // 透過（描画しない）
    }
    
    settingResDepthOperation.SetFlags( frag );
    settingResDepthOperation.SetTestFunc( func );
    
    setting_res_material.SetFragmentOperationHash(0x0);
  }
  
  {//αテスト設定コピー
    nw::gfx::ResMaterial            setting_res_material = setting_material->GetAlphaTestResMaterial();
    nw::gfx::ResFragmentShader      settingResFragmentShader = setting_res_material.GetFragmentShader();
    nw::gfx::ResAlphaTest           settingResAlphaTest = settingResFragmentShader.GetAlphaTest();
    nw::gfx::ResFragmentShader      targetResFragmentShader = target_res_material.GetFragmentShader();
    nw::gfx::ResAlphaTest           targetResAlphaTest = targetResFragmentShader.GetAlphaTest();
    
    b32                                     frag = targetResAlphaTest.IsTestEnabled();
    nw::gfx::ResAlphaTest::TestFunc         func = targetResAlphaTest.GetTestFunc();
    f32                                     ref = targetResAlphaTest.GetTestReference();
    
    settingResAlphaTest.SetTestEnabled( frag );
    settingResAlphaTest.SetTestFunc( func );
    settingResAlphaTest.SetTestReference( ref );
    
    setting_res_material.SetAlphaTestHash(0x0);
  }
  
  {//カリングモードコピー
    nw::gfx::ResMaterial            setting_res_material = setting_material->GetRasterizationResMaterial();
    nw::gfx::ResRasterization       settingResRasterization = setting_res_material.GetRasterization();
    nw::gfx::ResRasterization       targetResRasterization = target_res_material.GetRasterization();
    
    nw::gfx::res::ResRasterization::CullingMode     mode;
    mode = targetResRasterization.GetCullingMode();
      
    settingResRasterization.SetCullingMode( mode );
    
    setting_res_material.SetRasterizationHash(0x0);
  }
  
  //ResFragmentOperation fragmentOperation
  //ResDepthOperation resDepthOperation = fragmentOperation.GetDepthOperation();
  
  // マテリアルを差し替えます。
  renderContext->SetMaterial(setting_material);
  renderContext->ActivateContext();
  
  // RENDERMODE_IGNORE_MATERIAL を用いて以降のマテリアル設定を行いません。
  renderContext->SetRenderMode(nw::gfx::RenderContext::RENDERMODE_IGNORE_MATERIAL | nw::gfx::RenderContext::RENDERMODE_IGNORE_SHADER); 
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void EdgeMapPreRenderCallbackSlot::Invoke(H3dModel* model, s32 material_index, H3dCommand* command, H3dEnvironment* env)
{
  if ( m_fEnable == false )
    return;
  
  //ISHIGURO_PRINT("EdgeMapPreRenderCallbackSlot %s\n", model->GetMaterialName(material_index) );
  
  H3dModel*    m_pSettingModel;
  s32          edgeType = EdgeType::DefaultEdgeTypeID;
  b32             fTextureEnable = false;
  s32          maskType;
  
  {//シェーダーの選択
    s32             edgeModelNo = EdgeType::DefaultEdgeTypeID;
    s32             userDataIndex;
    H3dUserData     user_data;
    
    {//α抜きテクスチャがあるか
      userDataIndex = model->GetMaterialUserDataIndex( material_index, "$EdgeMapAlphaMask" );

      if ( userDataIndex >= 0 )
      {
        model->GetMaterialUserData( material_index, userDataIndex, &user_data );
        maskType = user_data.GetIntValue( 0 );

        if ( maskType != EdgeMapAlphaMask::None )
        {
          if ( model->IsMaterialTextureMapperTextureValid( material_index, maskType - EdgeMapAlphaMask::Texture0 ) )
          {
            fTextureEnable = true;
          }
        }
      }
    }
    
    {//エッジタイプ取得と決定
      {
        userDataIndex = model->GetMaterialUserDataIndex( material_index, "$EdgeType" );

        if ( userDataIndex >= 0 )
        {//指定がある場合
          model->GetMaterialUserData( material_index, userDataIndex, &user_data );
          edgeType = user_data.GetIntValue(0);
        }
      }

      switch( edgeType ){
      case EdgeType::VColorEdgeTypeID:
        edgeModelNo = ( fTextureEnable ) ? ALPHA_DISABLE_NORMAL_EDGE_MODEL_VCOL_A : DISABLE_NORMAL_EDGE_MODEL_VCOL_A;
        break;
      case EdgeType::Erace:
        edgeModelNo = ( fTextureEnable ) ? ALPHA_DISABLE_NORMAL_EDGE_DISABLE_MODEL_VCOL_A : DISABLE_NORMAL_EDGE_DISABLE_MODEL_VCOL_A;
        break;
      case EdgeType::OwnColor:// 自前で塗る（のでマテリアル切り替えを行わない
        {
          model->SetMaterialStencilTestPassOperation(material_index, H3dStencilOperationExConst::STENCILOP_KEEP);
          return;
        }
      case EdgeType::DefaultEdgeTypeID:
      default:
        edgeModelNo = ( fTextureEnable ) ? ALPHA_EDGE_MODEL_VCOL_A : DEFAULT_EDGE_MODEL_VCOL_A;//デフォルト
        break;
      }
    }
    
    m_pSettingModel = m_pH3dEdgeModel[edgeModelNo];
  }
  
  if ( fTextureEnable )
  {//α抜きテクスチャ設定
    s32                                 textureNo = (maskType - EdgeMapAlphaMask::Texture0);
    m_pSettingModel->SetMaterialTextureMapperTexture( 0, 0, *model, material_index, textureNo );
  }
  
  if ( edgeType == EdgeType::None )
  {// 透過（描画しない）
    m_pSettingModel->SetMaterialDepthTestEnable( 0, true );
    m_pSettingModel->SetMaterialDepthTestMaskEnable( 0, true );
    m_pSettingModel->SetMaterialDepthTestFunction( 0, H3dDepthTestExConst::TESTFUNC_NEVER );
  }
  else
  {//深度テスト情報コピー
    b32 enable = model->IsMaterialDepthTestEnable( material_index );
    m_pSettingModel->SetMaterialDepthTestEnable( 0, enable);
    
    enable = model->IsMaterialDepthTestMaskEnable( material_index );
    m_pSettingModel->SetMaterialDepthTestMaskEnable( 0, enable );
    
    H3dDepthTestExConst::TestFunc func = model->GetMaterialDepthTestFunction( material_index); 
    m_pSettingModel->SetMaterialDepthTestFunction( 0, func);
  }
  
  {//αテスト設定コピー
    b32 enable = model->IsMaterialAlphaTestEnable( material_index );
    m_pSettingModel->SetMaterialAlphaTestEnable( 0, enable);
    
    H3dAlphaTestExConst::TestFunc func = model->GetMaterialAlphaTestFunction(material_index);
    m_pSettingModel->SetMaterialAlphaTestFunction( 0, func );

    u8 ref = model->GetMaterialAlphaTestReference(material_index);
    m_pSettingModel->SetMaterialAlphaTestReference( 0, ref );
  }
  
  {//カリングモードコピー
    H3dRasterizationExConst::CullingMode cull = model->GetMaterialRasterizationCullingMode(material_index); 
    m_pSettingModel->SetMaterialRasterizationCullingMode( 0, cull);
  }
  
  {//マトリックス設定
    nn::math::MTX34     wMat = model->GetWorldMatrix();
    nn::math::MTX34     nMat = model->GetNormalMatrix();
  
    if ( model->IsSkeltalModel() )
    {
      wMat = nn::math::MTX34::Identity();
    }
    
    m_pSettingModel->SetWorldMatrix( wMat );
    m_pSettingModel->SetNormalMatrix( nMat );
  }
  
  // マテリアルを差し替えます。
  m_pSettingModel->MakeMaterialCommand( command, env, 0 );
  
  // レンダーモードをマテリアル設定無視にする
  env->SetRenderModeBitFlag( H3dEnvironment::RENDER_MODE_BIT_FLAG_IGNORE_MATERIAL );
  
#if GFL_DEBUG
  {//自分が描画されたフレームを適応先モデルより取得し設定する。
    u32       drawFrameCnt = model->GetDrawFrameCount();
    m_pSettingModel->SetDrawFrameCount( drawFrameCnt );
  }
#endif  
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void EdgeMapPostRenderCallbackSlot::Invoke(nw::gfx::Model*, nw::gfx::ResMesh, nw::gfx::RenderContext* renderContext)
{
  if ( m_fEnable == false )
    return;
  
  // レンダーモードをデフォルトに戻し、マテリアル設定が通常通り行われるようにします。
  renderContext->SetRenderMode(nw::gfx::RenderContext::RENDERMODE_DEFAULT);
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void EdgeMapPostRenderCallbackSlot::Invoke(H3dModel* model, s32 material_index, H3dCommand* command, H3dEnvironment* env)
{
  if ( m_fEnable == false )
    return;
  
  {
    s32   userDataIndex = model->GetMaterialUserDataIndex( material_index, "$EdgeType" );

    if ( userDataIndex >= 0 )
    {//指定がある場合
      H3dUserData     user_data;
      model->GetMaterialUserData( material_index, userDataIndex, &user_data );
      s32     edgeType = user_data.GetIntValue(0);
      
      if ( edgeType == EdgeType::OwnColor )
        model->SetMaterialStencilTestPassOperation(material_index, H3dStencilOperationExConst::STENCILOP_REPLACE);
    }
  }
    
  
  
  // レンダーモードをデフォルトに戻し、マテリアル設定が通常通り行われるようにします。
  env->SetRenderModeBitFlag( H3dEnvironment::RENDER_MODE_BIT_FLAG_DEFAULT );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------  
const char* G3DEdgeMapSystem::GetIDEdgeEnableTagName()
{
  return "IDEdgeEnable";//@note 自動生成ヘッダなどに埋め込む
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
const char* G3DEdgeMapSystem::GetEdgeIDTagName()
{
  return "EdgeID";//@note 自動生成ヘッダなどに埋め込む
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
G3DEdgeMapSystem::G3DEdgeMapSystem()
{
  
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
G3DEdgeMapSystem::~G3DEdgeMapSystem()
{
  DestroyWork();
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeMapSystem::MorphingEnable( f32 fEnable )
{
  m_PreRenderCallbackSlot.SetMorphingEnable( fEnable );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeMapSystem::createGfxModel(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator)
{
  {
    void * buff = System::GetStaticResource( System::NORMAL_TEXTURE_BCTEX );
    if(buff){
      m_GfxCommonResource[CommonResourceType::NormalTexture].AttachBufferAndSetup( device_allocator, buff, NULL, false );
    }else{
      m_GfxCommonResource[CommonResourceType::NormalTexture].LoadAndSetup( device_allocator, "rom:/g3d_common/NormalTexture.bctex" );
    }
    buff = System::GetStaticResource( System::NORMAL_TO_UV_SHADER_BCSDR );
    if(buff){
      m_GfxCommonResource[CommonResourceType::NormalToUvShader].AttachBufferAndSetup( device_allocator, buff, NULL, false );
    }else{
      m_GfxCommonResource[CommonResourceType::NormalToUvShader].LoadAndSetup( device_allocator, "rom:/g3d_common/NormalToUvShader.bcsdr" );
    }
    
    const char* pFilePath[EDGE_MODEL_MAX] = {
      "rom:/g3d_common/EdgeMapVColA.bcmdl",//法線＋頂点カラー
      "rom:/g3d_common/EdgeMapAlphaVColA.bcmdl",//テクスチャα抜き対応シェーダー
      "rom:/g3d_common/EdgeMapVColADisableNormal.bcmdl",//頂点カラーのみ
      "rom:/g3d_common/EdgeMapAlphaVColADisableNormal.bcmdl",//テクスチャα抜き対応シェーダー
      "rom:/g3d_common/EdgeMapDisableNormalVColA.bcmdl",//法線無し＋頂点カラー無しの塗りつぶしシェーダー
      "rom:/g3d_common/EdgeMapAlphaDisableNormalVColA.bcmdl"//テクスチャα抜き対応シェーダー
    };
    const System::ResourceEnum pFileEnum[EDGE_MODEL_MAX] = {
      System::EDGE_MAP_VCOLA_BCMDL,//法線＋頂点カラー
      System::EDGE_MAP_ALPHAVCOLA_BCMDL,//テクスチャα抜き対応シェーダー
      System::EDGE_MAP_VCOLA_DISABLE_NORMAL_BCMDL,//頂点カラーのみ
      System::EDGE_MAP_ALPHA_VCOLA_DISABLE_NORMAL_BCMDL,//テクスチャα抜き対応シェーダー
      System::EDGE_MAP_DISABLE_NORMAL_VCOLA_BCMDL,//法線無し＋頂点カラー無しの塗りつぶしシェーダー
      System::EDGE_MAP_ALPHA_DISABLE_NORMAL_VCOLA_BCMDL//テクスチャα抜き対応シェーダー
    };

    for( s32 i = 0; i < EDGE_MODEL_MAX; ++i )
    {
      buff = System::GetStaticResource( pFileEnum[i] );
      if(buff){
        m_GfxResource[i].AttachBufferAndSetup( device_allocator, buff, NULL, false );
      }else{
        m_GfxResource[i].LoadAndSetup( device_allocator, pFilePath[i] );
      }
    }
  }

  for( s32 i = 0; i < EDGE_MODEL_MAX; ++i )
  {
    m_GfxResource[i].Setup( device_allocator , &m_GfxCommonResource[CommonResourceType::NormalTexture] ); 
    m_GfxResource[i].Setup( device_allocator , &m_GfxCommonResource[CommonResourceType::NormalToUvShader] ); 
    
    m_GfxEdgeModel[i].Create( heap_allocator, device_allocator, &m_GfxResource[i], 0);
    m_PreRenderCallbackSlot.SetEdgeModel( i, &m_GfxEdgeModel[i] );
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeMapSystem::createH3dModel(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, b32 f_delete_memory_control)
{
  struct LocalFunc{
    static void *ReadData( gfl::heap::NwAllocator *device_allocator, const char* pFilePath )
    {//自前データロード
      void                                 *buffer;
      gfl::fs::ReadFile   read_file;
      bool                open_ret = read_file.Open( pFilePath );
      u32                 size  = read_file.GetSize();
      u32 size_align = read_file.GetSizeAlign();  // テクスチャをロードするには128Byteアライメントを行う必要があるので、
      size_align     = 128;                       // 何をロードするかでアライメントを分ける必要があるかも
      buffer = GflHeapAllocMemoryAlign( device_allocator->GetHeapBase(), size, size_align );
      s32 read_ret = read_file.Read( buffer, size );  // sizeをsize_alignに合わせたアライメントにしておく必要があるかも
      read_file.Close();

      return buffer;
    }
  };
  
  {//リソース読み込み
    
    const char* pCommonFilePath[ CommonResourceType::NumberOf ] = {
      "rom:/g3d_common/NormalTexture.bctex.h3d",
      "rom:/g3d_common/NormalToUvShader.bcsdr.h3d"
    };
    const System::ResourceEnum pCommonFileIndex[ CommonResourceType::NumberOf ] = {
      System::NORMAL_TEXTURE_BCTEX_H3D,
      System::NORMAL_TO_UV_SHADER_BCSDR_H3D,
    };
    
    const char* pFilePath[EDGE_MODEL_MAX] = {
      "rom:/g3d_common/EdgeMapVColA.bcmdl.h3d",//法線＋頂点カラー
      "rom:/g3d_common/EdgeMapAlphaVColA.bcmdl.h3d",//テクスチャα抜き対応シェーダー
      "rom:/g3d_common/EdgeMapVColADisableNormal.bcmdl.h3d",//頂点カラーのみ
      "rom:/g3d_common/EdgeMapAlphaVColADisableNormal.bcmdl.h3d",//テクスチャα抜き対応シェーダー
      "rom:/g3d_common/EdgeMapDisableNormalVColA.bcmdl.h3d",//法線無し＋頂点カラー無しの塗りつぶしシェーダー
      "rom:/g3d_common/EdgeMapAlphaDisableNormalVColA.bcmdl.h3d"//テクスチャα抜き対応シェーダー
    };
    const System::ResourceEnum pFileIndex[EDGE_MODEL_MAX] = {
      System::EDGE_MAP_VCOLA_BCMDL_H3D,//法線＋頂点カラー
      System::EDGE_MAP_ALPHAVCOLA_BCMDL_H3D,//テクスチャα抜き対応シェーダー
      System::EDGE_MAP_VCOLA_DISABLE_NORMAL_BCMDL_H3D,//頂点カラーのみ
      System::EDGE_MAP_ALPHA_VCOLA_DISABLE_NORMAL_BCMDL_H3D,//テクスチャα抜き対応シェーダー
      System::EDGE_MAP_DISABLE_NORMAL_VCOLA_BCMDL_H3D,//法線無し＋頂点カラー無しの塗りつぶしシェーダー
      System::EDGE_MAP_ALPHA_DISABLE_NORMAL_VCOLA_BCMDL_H3D//テクスチャα抜き対応シェーダー
    };
    
    u32 common_resource_flag = H3dResource::COMMON_RESOURCE_NONE | H3dResource::COMMON_RESOURCE_H3D_DEFAULT_SHADER_BIT;

    void * buff;
    
    for( s32 i = 0; i < CommonResourceType::NumberOf; i++ )
    {
      buff = System::GetStaticResource( pCommonFileIndex[i] );
      if(buff){
        m_pH3dCommonData[ i ] = buff;
        m_H3dCommonResourceReleaseFlag[ i ] = false;
      }
      else{
        m_pH3dCommonData[ i ] = LocalFunc::ReadData( device_allocator, pCommonFilePath[i] );
        m_H3dCommonResourceReleaseFlag[ i ] = true;
      }
      m_H3dCommonResource[i].AttachBuffer( m_pH3dCommonData[ i ] );
      
      if ( i == CommonResourceType::NormalTexture )
      {
        H3dResource::MemoryLocation      location;
        location.raw_section = gfl::grp::MEMORY_AREA_VRAMA;
        m_H3dCommonResource[i].Map( heap_allocator, device_allocator, &location );
      }
      
      m_H3dCommonResource[i].Setup( heap_allocator, device_allocator, NULL, common_resource_flag );
    }

    for( s32 i = 0; i < EDGE_MODEL_MAX; ++i )
    {
      buff = System::GetStaticResource( pFileIndex[i] );
      if(buff){
        m_pH3dResourceData[ i ] = buff;
        m_H3dResourceReleaseFlag[ i ] = false;
      }else{
        m_pH3dResourceData[ i ] = LocalFunc::ReadData( device_allocator, pFilePath[i] );
        m_H3dResourceReleaseFlag[ i ] = true;
      }
      m_H3dResource[i].AttachBuffer( m_pH3dResourceData[ i ] );
      m_H3dResource[i].Setup( heap_allocator, device_allocator, &m_H3dCommonResource[CommonResourceType::NormalTexture], common_resource_flag );
      m_H3dResource[i].Setup( heap_allocator, device_allocator, &m_H3dCommonResource[CommonResourceType::NormalToUvShader], common_resource_flag );
      
      H3dModel::Description     desc;
      desc.delete_memory_control = f_delete_memory_control;
      m_H3dEdgeModel[i].Create( heap_allocator, device_allocator, &m_H3dResource[i], 0, &desc );
      
      m_PreRenderCallbackSlot.SetEdgeModel( i, &m_H3dEdgeModel[i] );
    }
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeMapSystem::CreateWork(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, gfl::grp::RenderTarget::Description* p_desc, gfl::grp::RenderTarget::Description* p_low_desc, b32 f_delete_memory_control )
{ 
  gfl::grp::RenderTarget::Description   desc;

  if ( p_desc )
  {
    desc = *p_desc;
  }
  
  {
    desc.width = EDGE_MAP_WIDTH;
    desc.height = EDGE_MAP_HEIGHT;
    desc.depth_format = gfl::grp::RENDER_DEPTH_FORMAT_24_STENCIL8;

    desc.display_type = DISPLAY_UPPER;
    m_render_target[CommandType::Upper] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::RenderTarget();
    m_render_target[CommandType::Upper]->Create( heap_allocator, device_allocator, &desc );
  }
  
  {
    desc.color_address = m_render_target[CommandType::Upper]->GetColorAddress();
    desc.depth_address = m_render_target[CommandType::Upper]->GetDepthAddress();
    desc.display_type = DISPLAY_UPPER_RIGHT;

    m_render_target[CommandType::UpperRight] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::RenderTarget();
    m_render_target[CommandType::UpperRight]->Create( heap_allocator, device_allocator, &desc );
  }
  
  m_render_target[CommandType::UpperRight]->SetViewport( 0, 0, 400, 240 );
  m_render_target[CommandType::Upper]->SetViewport( 0, 0, 400, 240 );
  
  m_render_target[CommandType::Lower] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::RenderTarget();
  if ( p_low_desc )
  {
    desc = *p_low_desc;
    desc.color_address = m_render_target[CommandType::Upper]->GetColorAddress();
    desc.depth_address = m_render_target[CommandType::Upper]->GetDepthAddress();
    desc.width = EDGE_MAP_WIDTH;
    desc.height = EDGE_MAP_HEIGHT;
    desc.depth_format = gfl::grp::RENDER_DEPTH_FORMAT_24_STENCIL8;
    desc.display_type = DISPLAY_LOWER;
  
    m_render_target[CommandType::Lower]->Create( heap_allocator, device_allocator, &desc );
    m_render_target[CommandType::Lower]->SetViewport( 0, 0, 320, 240 );
  }
  
  {//オフスクリーン用
    desc.display_type = DISPLAY_NONE;

    m_render_target[CommandType::None] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::RenderTarget();
    m_render_target[CommandType::None]->Create( heap_allocator, device_allocator, &desc );
  }
  
  createGfxModel(heap_allocator, device_allocator);
  createH3dModel(heap_allocator, device_allocator, f_delete_memory_control);
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeMapSystem::DestroyWork(void)
{
  for( int i = 0; i < CommandType::NumberOf; ++i )
  {
    GFL_SAFE_DELETE( m_render_target[i] );
  }
  
  //m_H3dCommonResource[ CommonResourceType::NormalTexture ].DeleteSomeSection();
  for( s32 i = 0; i < CommonResourceType::NumberOf; i++ )
  {
    m_H3dCommonResource[i].DetachBuffer(true);
    if( m_H3dCommonResourceReleaseFlag[i] ){
      GflHeapSafeFreeMemory( m_pH3dCommonData[i] );
      m_H3dCommonResourceReleaseFlag[i] = false;
    }else{
      m_pH3dCommonData[i] = NULL;
    }
  }

  for( s32 i = 0; i < EDGE_MODEL_MAX; ++i )
  {
    m_H3dResource[i].DetachBuffer(true);
    if( m_H3dResourceReleaseFlag[i] ){
      GflHeapSafeFreeMemory( m_pH3dResourceData[i] );
      m_H3dResourceReleaseFlag[i] = false;
    }else{
      m_pH3dResourceData[i] = NULL;
    }
  }
}

void G3DEdgeMapSystem::RenderCallbackEnable( b32 f )
{
  m_PreRenderCallbackSlot.RenderCallbackEnable( f );
  m_PostRenderCallbackSlot.RenderCallbackEnable( f );
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

