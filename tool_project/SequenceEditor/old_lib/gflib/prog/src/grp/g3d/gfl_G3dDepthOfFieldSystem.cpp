//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dDepthOfFieldSystem.cpp
 *	@brief  被写界深度システム
 *	@author	Masateru Ishiguro
 *	@date		2011.06.28
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
#include <grp/g3d/gfl_G3dDepthOfFieldSystem.h>

#include "grp/util/gfl_UtilDrawUtil.h"


namespace gfl {
namespace grp {
namespace g3d {
  
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
G3DDepthOfFieldSystem::G3DDepthOfFieldSystem() :
  m_graphics_system( NULL ),
  m_Type( DofType::Front ),
  m_BlurScale( 1.0f )
{
  for( s32 i = 0; i < DISPLAY_BOTH_MAX; ++i )
  {
    m_render_target[i] = NULL;
    m_small_target[i] = NULL;
  }
  
  m_pHalfImage = NULL;
  
  ReSetFocusRange();
  
  SetFocusRange( 0, 50.0f, 1.0f );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
G3DDepthOfFieldSystem::~G3DDepthOfFieldSystem()
{
  DestroyWork();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void G3DDepthOfFieldSystem::CreateWork(gfl::grp::GraphicsSystem* graphics_system, gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, gfl::grp::RenderTarget::Description* p_desc )
{
  gfl::grp::RenderTarget::Description   desc;
  gfl::grp::RenderTarget::Description   smallDesc;
  f32                                   screanWidth, screanHeight;
  f32                                   bufferWidth, bufferHeight;
  f32                                   left, top, right, bottom;
  const gfl::grp::RenderTarget          *p_frame_buffer = NULL;
  gfl::grp::FrameBufferManager          *p_frame_buffer_manager = NULL;
  
  m_graphics_system = graphics_system;

  if ( p_desc )
  {
    desc = *p_desc;
  }
  
  {
    p_frame_buffer_manager = graphics_system->GetFrameBufferManager();
    p_frame_buffer = p_frame_buffer_manager->GetRenderTarget( m_graphics_system->GetCurrentFrameBufferID(gfl::grp::DISPLAY_UPPER) );

    bufferWidth = p_frame_buffer->GetWidth();//512
    bufferHeight = p_frame_buffer->GetHeight();//256
    
    p_frame_buffer->GetViewport(&left, &top, &right, &bottom);
    screanWidth = right - left;//400
    screanHeight = bottom - top;//240
    
    //NN_LOG( "%f, %f, %f, %f, %f, %f\n", left, top, right, bottom, screanW, screanH );
    //0.000000, 0.000000, 400.000000, 240.000000, 512.000000, 256.000000
  }
  
  desc.width = bufferWidth;
  desc.height = bufferHeight;
  desc.depth_format = gfl::grp::RENDER_DEPTH_FORMAT_24_STENCIL8;
  
  desc.display_type = DISPLAY_UPPER;
  m_render_target[DISPLAY_UPPER] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::RenderTarget();
  m_render_target[DISPLAY_UPPER]->Create( heap_allocator, device_allocator, &desc );
  
  desc.color_address = m_render_target[DISPLAY_UPPER]->GetColorAddress();
  desc.depth_address = m_render_target[DISPLAY_UPPER]->GetDepthAddress();
  desc.display_type = DISPLAY_UPPER_RIGHT;
  
  m_render_target[DISPLAY_UPPER_RIGHT] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::RenderTarget();
  m_render_target[DISPLAY_UPPER_RIGHT]->Create( heap_allocator, device_allocator, &desc );
  
  m_render_target[DISPLAY_UPPER_RIGHT]->SetViewport( left, top, right, bottom );
  m_render_target[DISPLAY_UPPER]->SetViewport( left, top, right, bottom );
  
  m_render_target[DISPLAY_LOWER] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::RenderTarget();
    
  desc.color_address = m_render_target[DISPLAY_UPPER]->GetColorAddress();
  desc.depth_address = m_render_target[DISPLAY_UPPER]->GetDepthAddress();
  desc.width = bufferWidth;
  desc.height = bufferHeight;
  desc.depth_format = gfl::grp::RENDER_DEPTH_FORMAT_24_STENCIL8;
  desc.display_type = DISPLAY_LOWER;

  m_render_target[DISPLAY_LOWER]->Create( heap_allocator, device_allocator, &desc );
  
  p_frame_buffer = p_frame_buffer_manager->GetRenderTarget( m_graphics_system->GetCurrentFrameBufferID(gfl::grp::DISPLAY_LOWER) );
  p_frame_buffer->GetViewport(&left, &top, &right, &bottom);
  m_render_target[DISPLAY_LOWER]->SetViewport( left, top, right, bottom );

  m_pHalfImage = m_graphics_system->AllocMemory( MEMORY_AREA_VRAMB, MEM_RENDERBUFFER, (bufferWidth / 2) * (bufferHeight / 2) * 4 );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void G3DDepthOfFieldSystem::DestroyWork(void)
{
  for( int i = 0; i < DISPLAY_BOTH_MAX; ++i )
  {
    GFL_SAFE_DELETE( m_render_target[i] );
    GFL_SAFE_DELETE( m_small_target[i] );
  }
  
  if ( m_pHalfImage )
    m_graphics_system->FreeMemory( MEMORY_AREA_VRAMB, m_pHalfImage );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void G3DDepthOfFieldSystem::MakeBlurImage( const gfl::grp::DisplayType display_type, gfl::grp::GraphicsSystem* graphics_system, gfl::grp::FrameBufferManager* frame_buffer_manager, gfl::grp::g3d::Camera* pCamera )
{
  GFL_ASSERT_STOP_MSG(display_type<DISPLAY_BOTH_MAX, "ディスプレイタイプが%d>=%dとなっています。\n", display_type, DISPLAY_BOTH_MAX);

  //f32                         near = camera->GetNearClip();
  //f32                         far = camera->GetFarClip();
  static const s32            VERTEX_MAX = 4;
  gfl::grp::util::Vertex      vertex[VERTEX_MAX];
  
  const gfl::grp::RenderTarget      *p_frame_buffer;
  
  p_frame_buffer = frame_buffer_manager->GetRenderTarget( graphics_system->GetCurrentFrameBufferID(display_type) );
  
  if( !graphics_system->BindCurrentCommandList( m_render_target[ display_type ] ) )
  {
    //  レンダリングの必要がない。
    return ;
  }
  
  m_render_target[ display_type ]->ClearBuffer( true, gfl::grp::ColorF32(0.0f, 0.0f, 0.0f, 0.0f), false, 1.0f );
  
  {
    f32 left, top, right, bottom;
    
    p_frame_buffer->GetViewport( &left, &top, &right, &bottom );
    
    //f32   u = 240.0f / 256.0f;
    //f32   v = 400.0f / 512.0f;
    f32   u = (bottom - top) / p_frame_buffer->GetHeight();
    f32   v = (right - left) / p_frame_buffer->GetWidth();

    vertex[0].m_Uv.Set( 0.0f,    v );//左上ＧＬ座標
    vertex[1].m_Uv.Set(    u,    v );//右上ＧＬ座標
    vertex[2].m_Uv.Set( 0.0f, 0.0f );//左下ＧＬ座標
    vertex[3].m_Uv.Set(    u, 0.0f );//右下ＧＬ座標
  
    gfl::grp::util::DrawUtil::BeginRender( m_render_target[ display_type ] );

    nn::math::MTX44   identityMatrix;
    nn::math::MTX34   viewMatrix;
    nn::math::MTX44   projMatrix;
    nn::math::MTX44Identity( &identityMatrix );
    
    pCamera->GetViewAndProjectionMatrix( &viewMatrix, &projMatrix);

    {
      gfl::grp::util::DrawUtil::SetProjectionMatrix( projMatrix );
      gfl::grp::util::DrawUtil::SetViewMatrix( nn::math::MTX44(viewMatrix) );
      gfl::grp::util::DrawUtil::SetModelMatrix( identityMatrix );
      
      static const f32      scale = 1.0f;
      static const f32      uv[VERTEX_MAX][2] = {
        { -scale,  scale },
        {  scale,  scale },
        { -scale, -scale },
        {  scale, -scale },
      };
      
      nn::math::MTX44     viewInvMat;
      MTX44Inverse( &viewInvMat, nn::math::MTX44(viewMatrix) );
      
      gfl::math::VEC4     camPos = viewInvMat.GetColumn(3);
      gfl::math::VEC4     centerLay = gfl::math::MathUtil::GetRayVector( &viewMatrix, &projMatrix, 0.0f, 0.0f );
      gfl::math::VEC4     lay[VERTEX_MAX];
      f32                 cos[VERTEX_MAX];

      for( s32 cnt = 0; cnt < VERTEX_MAX; ++cnt )
      {
        lay[cnt] = gfl::math::MathUtil::GetRayVector( &viewMatrix, &projMatrix, uv[cnt][0], uv[cnt][1] );
        cos[cnt] = centerLay.Dot( lay[cnt] );
      }
      
      gfl::grp::util::DrawUtil::MaterialInfo    info;
      
      for( s32 layer = 0; layer < DOF_LAYER_MAX; ++layer )
      {//まずは、ただのプレーンをレンダリングしαを更新。深度テストに合格したところのみαが入る
        
        if ( m_LayerInfo[layer].enable == false )
          continue;
        
        for( s32 cnt = 0; cnt < VERTEX_MAX; ++cnt )
        {
          gfl::math::VEC4     vec = camPos;
          gfl::math::VEC4     len;
          len = lay[cnt] * (m_FocusLength + m_LayerInfo[layer].rangeDistance);
          len /= cos[cnt];
          vec += len;
          vertex[cnt].m_Position = vec;
          
          vertex[cnt].m_Color.Set(  1.0f,  1.0f, 1.0f, m_LayerInfo[layer].blurred );
        }
        
        gfl::grp::util::DrawUtil::SetMaterial( info );
        
        gfl::grp::util::DrawUtil::SetRenderState( gfl::grp::util::DrawUtil::RenderStateType::MODEL_3D );
        
        u32     depthFunc = PICA_DATA_DEPTH_TEST_GREATER;
        
        if ( this->m_Type == DofType::Back )
          depthFunc = PICA_DATA_DEPTH_TEST_LESS;
        
        u32 s_RenderStateCommands[] =
        {
            // デプステストを有効
            // カラーバッファの全ての成分を書き込み可
            NW_FONT_CMD_SET_DEPTH_FUNC_COLOR_MASK(
                true,   // isDepthTestEnabled
                depthFunc,      // depthFunc
                true,   // depthMask
                true,   // red
                true,   // green
                true,   // blue
                true),  // alpha

            // フレームバッファアクセス制御
            NW_FONT_CMD_SET_FBACCESS(
                true,   // colorRead
                true,   // colorWrite
                true,   // depthRead
                false,  // depthWrite
                false,  // stencilRead
                false), // stencilWrite
        };
        
        nngxAdd3DCommand(s_RenderStateCommands, sizeof(s_RenderStateCommands), true);
        
        gfl::grp::util::DrawUtil::DrawVertexStrip( vertex, 4, false );
      }
      
      {//カラーイメージをベッタリコピー
        
        for( s32 cnt = 0; cnt < VERTEX_MAX; ++cnt )
        {
          vertex[cnt].m_Color.Set(  1.0f,  1.0f, 1.0f, 1.0f );
        }
        
        gfl::math::MTX44 identity_matrix;
        gfl::math::MTX44Identity( &identity_matrix );

        gfl::grp::util::DrawUtil::SetViewMatrix( identity_matrix );
        gfl::grp::util::DrawUtil::SetProjectionMatrix( identity_matrix );
        gfl::grp::util::DrawUtil::SetModelMatrix( identity_matrix );

        vertex[0].m_Position.Set(  -1,  1, 0 );
        vertex[1].m_Position.Set(   1,  1, 0 );
        vertex[2].m_Position.Set(  -1, -1, 0 );
        vertex[3].m_Position.Set(   1, -1, 0 );

        gfl::grp::util::DrawUtil::MaterialInfo    materialInfo;
        materialInfo.m_ShadingType = gfl::grp::util::DrawUtil::MaterialInfo::RGBTEXTURE_SHADER;
        gfl::grp::util::DrawUtil::SetMaterial( materialInfo );

        gfl::grp::util::DrawUtil::SetTexture( 0, nngxGetPhysicalAddr(p_frame_buffer->GetColorAddress()), p_frame_buffer->GetHeight(), p_frame_buffer->GetWidth(), GL_RGBA );
        
        gfl::grp::util::DrawUtil::SetRenderState( gfl::grp::util::DrawUtil::RenderStateType::MODEL_3D );
        
        static const u32 s_RenderStateCommands[] =
        {
            // デプステストを無効
            // カラーバッファのαには書き込まない
            NW_FONT_CMD_SET_DEPTH_FUNC_COLOR_MASK(
                false,   // isDepthTestEnabled
                PICA_DATA_DEPTH_TEST_ALWAYS,      // depthFunc
                false,   // depthMask
                true,   // red
                true,   // green
                true,   // blue
                false), // alpha

            // フレームバッファアクセス制御
            NW_FONT_CMD_SET_FBACCESS(
                true,   // colorRead
                true,   // colorWrite
                false,  // depthRead
                false,  // depthWrite
                false,  // stencilRead
                false), // stencilWrite
        };
        
        nngxAdd3DCommand(s_RenderStateCommands, sizeof(s_RenderStateCommands), true);

        gfl::grp::util::DrawUtil::DrawVertexStrip( vertex, 4, false );
      }
    }
    
    gfl::grp::util::DrawUtil::EndRender();
  }
  
  u32 src_address = m_render_target[ display_type ]->GetColorAddress();
  u32 dst_address = (u32)m_pHalfImage;
  s32 src_width = p_frame_buffer->GetHeight();
  s32 src_height = p_frame_buffer->GetWidth();
  s32 format = GL_RGBA8_OES;
  
  //縮小画像を作成
  nngxFlush3DCommand();
  nngxFilterBlockImage( reinterpret_cast<const GLvoid*>(src_address), reinterpret_cast<GLvoid*>(dst_address), src_width, src_height, format );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void G3DDepthOfFieldSystem::DrawBlurImage( const gfl::grp::DisplayType display_type, gfl::grp::GraphicsSystem* graphics_system, gfl::grp::FrameBufferManager* frame_buffer_manager, gfl::grp::g3d::Camera* pCamera )
{
  GFL_ASSERT_STOP_MSG(display_type<DISPLAY_BOTH_MAX, "ディスプレイタイプが%d>=%dとなっています。\n", display_type, DISPLAY_BOTH_MAX);

  static const s32            VERTEX_MAX = 4;
  gfl::grp::util::Vertex      vertex[VERTEX_MAX];
  
  gfl::grp::RenderTarget      *p_frame_buffer;
  
  p_frame_buffer = frame_buffer_manager->GetRenderTarget( graphics_system->GetCurrentFrameBufferID(display_type) );
  
  //nngxFlush3DCommand();
  if( graphics_system->BindCurrentCommandList( p_frame_buffer ) ){
 
    static b32                  fClear = false;
    if ( fClear )
    {
      p_frame_buffer->ClearBuffer( true, gfl::grp::ColorF32(0.0f, 0.0f, 0.0f, 0.0f), true, 1.0f );
    }
    
    for( s32 cnt = 0; cnt < VERTEX_MAX; ++cnt )
    {
      vertex[cnt].m_Color.Set(  1.0f,  1.0f, 1.0f, 1.0f );
    }
    
    f32     left, top, right, bottom;
    f32     screanW, screanH;
    
    screanW = p_frame_buffer->GetWidth();
    screanH = p_frame_buffer->GetHeight();
    p_frame_buffer->GetViewport(&left, &top, &right, &bottom);
    
    //NN_LOG( "%f, %f, %f, %f, %f, %f\n", left, top, right, bottom, screanW, screanH );
    //0.000000, 0.000000, 400.000000, 240.000000, 512.000000, 256.000000
    
    {
      static f32   viewH = (bottom - top);
      static f32   viewW = (right - left);
      static f32   u = viewH / screanH;
      static f32   v = viewW / screanW;
      //f32   u = 1.0f;
      //f32   v = 1.0f;

      vertex[0].m_Uv.Set( 0.0f,    v );//左上ＧＬ座標
      vertex[1].m_Uv.Set(    u,    v );//右上ＧＬ座標
      vertex[2].m_Uv.Set( 0.0f, 0.0f );//左下ＧＬ座標
      vertex[3].m_Uv.Set(    u, 0.0f );//右下ＧＬ座標
    
      gfl::grp::util::DrawUtil::BeginRender( p_frame_buffer );

      nn::math::MTX44   identityMatrix;
      nn::math::MTX34   viewMatrix;
      nn::math::MTX44   projMatrix;
      nn::math::MTX44Identity( &identityMatrix );
      
      pCamera->GetViewAndProjectionMatrix( &viewMatrix, &projMatrix);

      {
        gfl::grp::util::DrawUtil::SetProjectionMatrix( projMatrix );
        gfl::grp::util::DrawUtil::SetViewMatrix( nn::math::MTX44(viewMatrix) );
        gfl::grp::util::DrawUtil::SetModelMatrix( identityMatrix );
        
        static const f32      scale = 1.0f;
        static const f32      uv[VERTEX_MAX][2] = {
          { -scale,  scale },
          {  scale,  scale },
          { -scale, -scale },
          {  scale, -scale },
        };
        
        nn::math::MTX44     viewInvMat;
        MTX44Inverse( &viewInvMat, nn::math::MTX44(viewMatrix) );
        
        gfl::math::VEC4     camPos = viewInvMat.GetColumn(3);
        gfl::math::VEC4     centerLay = gfl::math::MathUtil::GetRayVector( &viewMatrix, &projMatrix, 0.0f, 0.0f );

        for( s32 cnt = 0; cnt < VERTEX_MAX; ++cnt )
        {
          gfl::math::VEC4     lay = gfl::math::MathUtil::GetRayVector( &viewMatrix, &projMatrix, uv[cnt][0], uv[cnt][1] );
          gfl::math::VEC4     vec = camPos;
          f32                 cos = centerLay.Dot( lay );
          lay *= m_FocusLength;
          lay /= cos;
          vec += lay;
          vertex[cnt].m_Position = vec;
        }

        gfl::grp::util::DrawUtil::MaterialInfo    info;
        static b32      fDebug = false;
        if ( fDebug )
        {
          info.m_ShadingType = gfl::grp::util::DrawUtil::MaterialInfo::DEBUG_SHADER;
        }
        else
        {
          if ( this->m_Type == DofType::Front )
            info.m_ShadingType = gfl::grp::util::DrawUtil::MaterialInfo::FRONT_DOFTEXTURE_SHADER;
          else
            info.m_ShadingType = gfl::grp::util::DrawUtil::MaterialInfo::BACK_DOFTEXTURE_SHADER;
        }
        
        gfl::grp::util::DrawUtil::SetMaterial( info );
        
        gfl::grp::util::DrawUtil::SetTexture( 0, nngxGetPhysicalAddr( (u32)m_pHalfImage ), 128, 256, GL_RGBA);
        
        if ( this->m_Type == DofType::Front )
        {//前面は３点サンプリング
          gfl::grp::util::DrawUtil::SetTexture( 1, nngxGetPhysicalAddr( (u32)m_pHalfImage ), 128, 256, GL_RGBA);
          gfl::grp::util::DrawUtil::SetTexture( 2, nngxGetPhysicalAddr( (u32)m_pHalfImage ), 128, 256, GL_RGBA);
          
          f32     offsetU = (u / viewH) * 2.0f;
          f32     offsetV = (v / viewW) * 2.0f;

          offsetU *= m_BlurScale;
          offsetV *= m_BlurScale;

          gfl::grp::util::DrawUtil::SetUvScroll( 0,     0.0f,  offsetV );
          gfl::grp::util::DrawUtil::SetUvScroll( 1, -offsetU, -offsetV );
          gfl::grp::util::DrawUtil::SetUvScroll( 2,  offsetU, -offsetV );
        }
        else
        {
          gfl::grp::util::DrawUtil::SetUvScroll( 0, 0.0f, 0.0f );
        }
        
        gfl::grp::util::DrawUtil::SetRenderState( gfl::grp::util::DrawUtil::RenderStateType::MODEL_3D );
          
        if ( this->m_Type == DofType::Front )
        {//手前ボケならただの上書き
          static const u32 s_RenderStateCommands[] =
          {
              // デプステストを無効
              // カラーバッファのαには書き込まない
              NW_FONT_CMD_SET_DEPTH_FUNC_COLOR_MASK(
                  false,   // isDepthTestEnabled
                  PICA_DATA_DEPTH_TEST_ALWAYS,      // depthFunc
                  false,   // depthMask
                  true,   // red
                  true,   // green
                  true,   // blue
                  false), // alpha

              // フレームバッファアクセス制御
              NW_FONT_CMD_SET_FBACCESS(
                  true,   // colorRead
                  true,   // colorWrite
                  false,  // depthRead
                  false,  // depthWrite
                  false,  // stencilRead
                  false), // stencilWrite
          };

          nngxAdd3DCommand(s_RenderStateCommands, sizeof(s_RenderStateCommands), true);
        }
        else
        {
          static const u32 s_RenderStateCommands[] =
          {
              // デプステストを有効
              // カラーバッファのαには書き込まない
              NW_FONT_CMD_SET_DEPTH_FUNC_COLOR_MASK(
                  true,   // isDepthTestEnabled
                  PICA_DATA_DEPTH_TEST_LESS,      // depthFunc
                  false,   // depthMask
                  true,   // red
                  true,   // green
                  true,   // blue
                  false), // alpha

              // フレームバッファアクセス制御
              NW_FONT_CMD_SET_FBACCESS(
                  true,   // colorRead
                  true,   // colorWrite
                  true,   // depthRead
                  false,  // depthWrite
                  false,  // stencilRead
                  false), // stencilWrite
          };

          nngxAdd3DCommand(s_RenderStateCommands, sizeof(s_RenderStateCommands), true);
        }
        
        gfl::grp::util::DrawUtil::DrawVertexStrip( vertex, 4, false );       
      }
      
      gfl::grp::util::DrawUtil::EndRender();
    }
  }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void G3DDepthOfFieldSystem::SetFocusRange( u32 layer, f32 rangeDistance, f32 blurred )
{
  if ( layer >= DOF_LAYER_MAX)
    return;
  
  m_LayerInfo[layer].enable = true;
  m_LayerInfo[layer].rangeDistance = rangeDistance;
  m_LayerInfo[layer].blurred = blurred;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void G3DDepthOfFieldSystem::ReSetFocusRange()
{
  for( s32 i = 0; i < DOF_LAYER_MAX; ++i )
  {
    m_LayerInfo[i].enable = false;
  }
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

