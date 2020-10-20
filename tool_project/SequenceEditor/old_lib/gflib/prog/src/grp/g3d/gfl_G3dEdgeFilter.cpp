//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dEdgeFilter.cpp
 *	@brief  エッジフィルター
 *	@author	Masateru Ishiguro
 *	@date		2011.04.26
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
#include <grp/g3d/gfl_G3dEdgeFilter.h>
#include <grp/g3d/g3d_common.gaix>


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
G3DEdgeFilter::G3DEdgeFilter(void):
  m_Type( DEFAULT_EDGE ),
  m_Scale(1.0f)
{
  for( s32 i = 0; i < TYPE_MAX; ++i )
  {
    for( s32 i2 = 0; i2 < MAX_PATH; ++i2 )
    {
      m_pEdgeModel[i][i2] = NULL;
    }
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
G3DEdgeFilter::~G3DEdgeFilter()
{
  DestroyWork();
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeFilter::CreateWork( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator )
{
  {
    void * buff = System::GetStaticResource( System::EDGE_FILLTER_BCRES );
    if(buff){
      m_Resource[DEFAULT_EDGE].AttachBufferAndSetup( device_allocator, buff, NULL, false );
    }else{
      m_Resource[DEFAULT_EDGE].LoadAndSetup( device_allocator, "rom:/g3d_common/EdgeFillter.bcres" );
    }
    buff = System::GetStaticResource( System::VCOLOR_EDGE_FILLTER_BCRES );
    if(buff){
      m_Resource[VCOLOR_EDGE].AttachBufferAndSetup( device_allocator, buff, NULL, false );
    }else{
      m_Resource[VCOLOR_EDGE].LoadAndSetup( device_allocator, "rom:/g3d_common/VColorEdgeFillter.bcres" );
    }
    buff = System::GetStaticResource( System::IDEDGE_FILLTER_BCRES );
    if(buff){
      m_Resource[ID_EDGE].AttachBufferAndSetup( device_allocator, buff, NULL, false );
    }else{
      m_Resource[ID_EDGE].LoadAndSetup( device_allocator, "rom:/g3d_common/IDEdgeFillter.bcres" );
    }
    buff = System::GetStaticResource( System::EDGE_FILLTER_NORMAL_BCRES );
    if(buff){
      m_Resource[NORMAL_EDGE].AttachBufferAndSetup( device_allocator, buff, NULL, false );
    }else{
      m_Resource[NORMAL_EDGE].LoadAndSetup( device_allocator, "rom:/g3d_common/EdgeFillterNormal.bcres" );
    }
    
#if GFL_DEBUG
    buff = System::GetStaticResource( System::EDGE_DEBUG_RGB_BCRES );
    if(buff){
      m_Resource[DEBUG_RGB].AttachBufferAndSetup( device_allocator, buff, NULL, false );
    }else{
      m_Resource[DEBUG_RGB].LoadAndSetup( device_allocator, "rom:/g3d_common/EdgeDebugRGB.bcres" );
    }
    buff = System::GetStaticResource( System::EDGE_DEBUG_A_BCRES );
    if(buff){
      m_Resource[DEBUG_A].AttachBufferAndSetup( device_allocator, buff, NULL, false );
    }else{
      m_Resource[DEBUG_A].LoadAndSetup( device_allocator, "rom:/g3d_common/EdgeDebugA.bcres" );
    }
    buff = System::GetStaticResource( System::EDGE_DEBUG_R_BCRES );
    if(buff){
      m_Resource[DEBUG_R].AttachBufferAndSetup( device_allocator, buff, NULL, false );
    }else{
      m_Resource[DEBUG_R].LoadAndSetup( device_allocator, "rom:/g3d_common/EdgeDebugR.bcres" );
    }
#endif // GFL_DEBUG
    buff = System::GetStaticResource( System::INSIDE_EDGE_FILLTER_BCRES );
    if(buff){
      m_Resource[INSIDE_EDGE].AttachBufferAndSetup( device_allocator, buff, NULL, false );
    }else{
      m_Resource[INSIDE_EDGE].LoadAndSetup( device_allocator, "rom:/g3d_common/InsideEdgeFillter.bcres" );
    }
    buff = System::GetStaticResource( System::INSIDE_EDGE_FILLTER_BCRES );
    if(buff){
      m_Resource[DOUBLE_LAYERED_EDGE].AttachBufferAndSetup( device_allocator, buff, NULL, false );
    }else{
      m_Resource[DOUBLE_LAYERED_EDGE].LoadAndSetup( device_allocator, "rom:/g3d_common/InsideEdgeFillter.bcres" );
    }
  }
  
  for( s32 i = 0; i < TYPE_MAX; ++i )
  {
    m_pEdgeModel[i][0] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model();
    m_pEdgeModel[i][0]->Create( heap_allocator, device_allocator, &m_Resource[i], 0);
    m_pEdgeModel[i][0]->SetVisible(false);
  }
  
  m_pEdgeModel[INSIDE_EDGE][1] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model();
  m_pEdgeModel[INSIDE_EDGE][1]->Create( heap_allocator, device_allocator, &m_Resource[INSIDE_EDGE], 1);
  m_pEdgeModel[INSIDE_EDGE][1]->SetVisible(false);
  
  m_pEdgeModel[DOUBLE_LAYERED_EDGE][1] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model();
  m_pEdgeModel[DOUBLE_LAYERED_EDGE][1]->Create( heap_allocator, device_allocator, &m_Resource[DOUBLE_LAYERED_EDGE], 1);
  m_pEdgeModel[DOUBLE_LAYERED_EDGE][1]->SetVisible(false);
  
  m_pEdgeModel[DOUBLE_LAYERED_EDGE][2] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model();
  m_pEdgeModel[DOUBLE_LAYERED_EDGE][2]->Create( heap_allocator, device_allocator, &m_Resource[DEFAULT_EDGE], 0);
  m_pEdgeModel[DOUBLE_LAYERED_EDGE][2]->SetVisible(false);
    
  ScaleEdgeWidth( 1.0f );//初期化
}


//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeFilter::DestroyWork(void)
{
  for( s32 i = 0; i < TYPE_MAX; ++i )
  {
    for( s32 i2 = 0; i2 < MAX_PATH; ++i2 )
    {
      GFL_SAFE_DELETE( m_pEdgeModel[i][i2] );
    }
  }
  
  if ( m_DepthTarget.IsValid() )
  {    
    m_DepthTarget.Cleanup();
    m_DepthTarget.DynamicDestroy();
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeFilter::SetColorTarget( nw::gfx::res::ResTexture target )
{
  m_ColorTarget = target;
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeFilter::CreateDepthTarget( gfl::heap::NwAllocator* heap_allocator, const gfl::grp::GraphicsSystem* graphics_system )
{
  gfl::grp::FrameBufferManager          *frame_buffer_manager = graphics_system->GetFrameBufferManager();
  gfl::grp::RenderTarget                *left_render_target = frame_buffer_manager->GetRenderTarget( graphics_system->GetCurrentFrameBufferID(gfl::grp::DISPLAY_UPPER) );

  m_DepthTarget = nw::gfx::res::ResImageTexture::DynamicBuilder()
      .Height(left_render_target->GetWidth())
      .Width(left_render_target->GetHeight())
      .MipmapSize( 1 )
      .LocationFlag( left_render_target->GetDepthMemoryArea() | MEMORY_TRANSFER_NO_COPY_FCRAM_DMP )
      .LocationAddress( left_render_target->GetDepthAddress() )
      .Format( nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA8 )
      .ExecutingMemoryFill(false)
      .DynamicAllocation(false)
      .Create(heap_allocator);

  m_DepthTarget.Setup(heap_allocator, nw::gfx::ResGraphicsFile(NULL));
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeFilter::SetTexture( nw::gfx::res::ResTexture texture, s32 layerNo )
{
  for( s32 i = 0; i < TYPE_MAX; ++i )
  {
    for( s32 no = 0; no < MAX_PATH; ++no )
    {
      gfl::grp::g3d::Model        *pModel = m_pEdgeModel[i][no];
      if ( pModel == NULL )       continue;
      
      if ( layerNo >= 0 && no != layerNo )
        continue;

      nw::gfx::Material* material = pModel->GetNwModel()->GetMaterial( 0 );

      // テクスチャマッパー
      nw::gfx::ResMaterial res_material = material->GetTextureMapperResMaterial();

      for( int material_no = 0; material_no < 3; ++material_no )
      {
        nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(material_no);

        // テクスチャ
        res_tex_mapper.SetTexture(texture);  
      }
      // テクスチャを変更した場合はテクスチャマッパーのハッシュを０に設定します。
      res_material.SetTextureMappersHash(0x0);
    }
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeFilter::ScaleEdgeWidth( f32 scale, gfl::grp::DisplayType type )
{
  f32     screanWidth = ( type == gfl::grp::DISPLAY_LOWER ) ? LOW_OFFSCREAN_WIDTH : OFFSCREAN_WIDTH;
  f32     screanHeight = ( type == gfl::grp::DISPLAY_LOWER ) ? LOW_OFFSCREAN_HEIGHT : OFFSCREAN_HEIGHT;
  
  ScaleEdgeWidth( screanWidth, screanHeight, scale );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeFilter::ScaleEdgeWidth( f32 screanWidth, f32 screanHeight, f32 scale )
{
  m_Scale = scale;
  
  //0.9375f <- 240/256
  //0.78125f <- 400/512
  
  for( s32 i = 0; i < TYPE_MAX; ++i )
  {
    for( s32 no = 0; no < MAX_PATH; ++no )
    {
      gfl::grp::g3d::Model        *pModel = m_pEdgeModel[i][no];
      if ( pModel == NULL )       continue;
    
      nw::gfx::Material* material = pModel->GetNwModel()->GetMaterial( 0 );

      // テクスチャマッパー
      nw::gfx::ResMaterial res_material = material->GetTextureMapperResMaterial();

      nw::gfx::ResTextureCoordinator  res_tex_coord[3];
      res_tex_coord[0] = res_material.GetTextureCoordinators( 0 );
      res_tex_coord[1] = res_material.GetTextureCoordinators( 1 );
      res_tex_coord[2] = res_material.GetTextureCoordinators( 2 );
      
      res_tex_coord[0].SetScale( nw::math::VEC2( screanHeight / TEXTURE_HEIGHT, screanWidth / TEXTURE_WIDTH ) );
      res_tex_coord[1].SetScale( nw::math::VEC2( screanHeight / TEXTURE_HEIGHT, screanWidth / TEXTURE_WIDTH ) );
      res_tex_coord[2].SetScale( nw::math::VEC2( screanHeight / TEXTURE_HEIGHT, screanWidth / TEXTURE_WIDTH ) );
      
      switch( i ){
      case INSIDE_EDGE:
      case DOUBLE_LAYERED_EDGE:
        if ( no == 0 )
        {//左右サンプリング
          res_tex_coord[1].SetTranslate( nw::math::VEC2( 1.0f / screanHeight * scale, 0.0f ) );
          res_tex_coord[2].SetTranslate( nw::math::VEC2( -(1.0f / screanHeight * scale), 0.0f ) );
        }
        else if ( no == 1 )
        {//上下サンプリング
          res_tex_coord[1].SetTranslate( nw::math::VEC2( 0.0f, 1.0f / screanWidth * scale ) );
          res_tex_coord[2].SetTranslate( nw::math::VEC2( 0.0f, -(1.0f / screanWidth * scale) ) );
        }
        else
        {
          res_tex_coord[1].SetTranslate( nw::math::VEC2( 1.0f / screanHeight * scale, 0.0f ) );
          res_tex_coord[2].SetTranslate( nw::math::VEC2( 0.0f, 1.0f / screanWidth * scale ) );
        }
        break;
      default:
        res_tex_coord[1].SetTranslate( nw::math::VEC2( 1.0f / screanHeight * scale, 0.0f ) );
        res_tex_coord[2].SetTranslate( nw::math::VEC2( 0.0f, 1.0f / screanWidth * scale ) );
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeFilter::SetDisplayType( gfl::grp::DisplayType type )
{
  ScaleEdgeWidth( m_Scale, type );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeFilter::SetEdgeColor( f32 r, f32 g, f32 b, f32 a )
{
  for( s32 i = 0; i < TYPE_MAX; ++i )
  {
    for( s32 no = 0; no < MAX_PATH; ++no )
    {
      gfl::grp::g3d::Model        *pModel = m_pEdgeModel[i][no];
      if ( pModel == NULL )       continue;
      
      nw::gfx::Material               *material = pModel->GetNwModel()->GetMaterial( 0 );
      nw::gfx::ResMaterial            res_material = material->GetTextureMapperResMaterial();
      nw::gfx::ResTextureCoordinator  res_tex_coord;
      nw::gfx::ResMaterialColor       res_color = res_material.GetMaterialColor();

      res_color.SetConstant1( r, g, b, a );
    }
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DEdgeFilter::SetEdgeType( EdgeType type )
{
  for( s32 no = 0; no < MAX_PATH; ++no )
  {
    gfl::grp::g3d::Model    *pModel;
    
    pModel = m_pEdgeModel[m_Type][no];
    if ( pModel )
    {
      pModel->SetVisible(false);
    }
    
    pModel = m_pEdgeModel[type][no];
    if ( pModel )
    {
      pModel->SetVisible(true);
    }
  }
  
  m_Type = type;
  
  switch( m_Type ){
  case DEFAULT_EDGE:   //! デフォルトエッジ
  case VCOLOR_EDGE:        //! 頂点カラーエッジ
  case NORMAL_EDGE:        //! 法線エッジ
#if GFL_DEBUG
  case DEBUG_RGB:          //! デバッグ用
  case DEBUG_A:            //! デバッグ用
#endif
    this->SetTexture( m_ColorTarget );
    break;
  case ID_EDGE:            //! ＩＤエッジ
#if GFL_DEBUG
  case DEBUG_R:            //! デバッグ用
#endif
  case INSIDE_EDGE:        //! 内側エッジ
    this->SetTexture( m_DepthTarget );
    break;
  case DOUBLE_LAYERED_EDGE:
    this->SetTexture( m_DepthTarget, 0 );
    this->SetTexture( m_DepthTarget, 1 );
    this->SetTexture( m_ColorTarget, 2 );
    break;
  }
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

