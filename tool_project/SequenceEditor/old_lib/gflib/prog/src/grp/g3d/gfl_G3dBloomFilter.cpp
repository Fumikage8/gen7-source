//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dBloomFilter.cpp
 *	@brief  3Dブルームフィルタ
 *	@author	Koji Kawada
 *	@date		2011.04.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Base.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dBloomFilter.h>
#include <grp/g3d/g3d_common.gaix>


namespace gfl {
namespace grp {
namespace g3d {

static const char* GFL_GRP_G3D_BLOOM_FILTER_HIGH_LUMINANCE_BOARD_FILE = "rom:/""g3d_common/bloom_filter_high_luminance_board.bcres";
static const char* GFL_GRP_G3D_BLOOM_FILTER_BLOOM_BOARD_FILE          = "rom:/""g3d_common/bloom_filter_bloom_board.bcres";

//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dブルームフィルタクラス
//=====================================

void BloomFilter::InitializeBloomBoardDescriptionForDefault(
    BloomBoardDescription*  description
)
{
  description->small_image_count = 2;
  description->uv = gfl::math::VEC2(0.0f, 0.0f);
  description->rate = 4.0f/16.0f;
}
void BloomFilter::InitializeBloomBoardDescription(
    BloomBoardDescription*  description,
    const u32              small_image_count,
    const gfl::math::VEC2& uv,
    const f32              rate
)
{
  description->small_image_count = small_image_count;
  description->uv = uv;
  description->rate = rate;
}

void BloomFilter::InitializeDescriptionForDefault(
    Description*  description
)
{
  description->high_luminance_board_render_target_width             = 512;
  description->high_luminance_board_render_target_height            = 256;
  description->high_luminance_board_render_target_format            = Texture::FORMAT_RGBA8;
  description->high_luminance_board_render_target_color_memory_area = gfl::grp::MEMORY_AREA_VRAMA;
  description->high_luminance_board_scene_calculate_group_index     = 0;
  description->high_luminance_board_scene_draw_group_index          = 0;
  description->high_luminance_color_weight                          = gfl::grp::ColorF32(0.3333f, 0.3333f, 0.3333f, 1.0f);
  description->high_luminance_min_value                             = 0.3f;
  description->small_image_num = 2;
  description->bloom_board_scene_calculate_group_index = 0;
  description->bloom_board_scene_draw_group_index      = 0;
  description->bloom_board_num                         = 0;
  description->bloom_board_description_array           = NULL;
  description->camera_scene_calculate_group_index = 0;
  description->camera_scene_camera_index          = 1;
  description->frame_buffer_copy_texture_memory_area = gfl::grp::MEMORY_AREA_NONE;  // #if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE==0 のときのみ使用される値。
}
void BloomFilter::InitializeDescription(
    Description*  description,
    const s32                  high_luminance_board_render_target_width,
    const s32                  high_luminance_board_render_target_height,
    const Texture::Format      high_luminance_board_render_target_format,
    const gfl::grp::MemoryArea high_luminance_board_render_target_color_memory_area,
    const s32                  high_luminance_board_scene_calculate_group_index,
    const s32                  high_luminance_board_scene_draw_group_index,
    const gfl::grp::ColorF32&  high_luminance_color_weight,
    const f32                  high_luminance_min_value,
    const u32       small_image_num,
    const s32                     bloom_board_scene_calculate_group_index,
    const s32                     bloom_board_scene_draw_group_index,
    const u32                     bloom_board_num,
    const BloomBoardDescription*  bloom_board_description_array,
    const s32       camera_scene_calculate_group_index,
    const s32       camera_scene_camera_index,
    const gfl::grp::MemoryArea  frame_buffer_copy_texture_memory_area  // #if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE==0 のときのみ使用される値。
)
{
  description->high_luminance_board_render_target_width             = high_luminance_board_render_target_width;
  description->high_luminance_board_render_target_height            = high_luminance_board_render_target_height;
  description->high_luminance_board_render_target_format            = high_luminance_board_render_target_format;
  description->high_luminance_board_render_target_color_memory_area = high_luminance_board_render_target_color_memory_area;
  description->high_luminance_board_scene_calculate_group_index     = high_luminance_board_scene_calculate_group_index;
  description->high_luminance_board_scene_draw_group_index          = high_luminance_board_scene_draw_group_index;
  description->high_luminance_color_weight                          = high_luminance_color_weight;
  description->high_luminance_min_value                             = high_luminance_min_value;
  description->small_image_num = small_image_num;
  description->bloom_board_scene_calculate_group_index = bloom_board_scene_calculate_group_index;
  description->bloom_board_scene_draw_group_index      = bloom_board_scene_draw_group_index;
  description->bloom_board_num                         = bloom_board_num;
  description->bloom_board_description_array           = bloom_board_description_array;
  description->camera_scene_calculate_group_index = camera_scene_calculate_group_index;
  description->camera_scene_camera_index          = camera_scene_camera_index;
  description->frame_buffer_copy_texture_memory_area  = frame_buffer_copy_texture_memory_area;  // #if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE==0 のときのみ使用される値。
}

BloomFilter::BloomFilter(void)
#if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE
  :
#else
  : m_frame_buffer_copy_texture(NULL),
#endif
    m_high_luminance_board_resource(NULL),
    m_high_luminance_board_model(NULL),
    m_high_luminance_board_scene_calculate_group_index(0),
    m_high_luminance_board_scene_draw_group_index(0),
    m_high_luminance_board_render_target(NULL),
    m_small_image_num(0),
    m_small_image_array(NULL),
    m_bloom_board_scene_calculate_group_index(0),
    m_bloom_board_scene_draw_group_index(0),
    m_bloom_board_num(0),
    m_bloom_board_array(NULL),
    m_bloom_board_expanse(1.0f),
    m_bloom_board_intensity(1.0f),
    m_dummy_camera(NULL),
    m_dummy_camera_scene_calculate_group_index(0),
    m_dummy_camera_scene_camera_index(1)
#if GFL_DEBUG
    ,
    m_texture_check_board_resource(NULL),
    m_texture_check_board_model(NULL)
#endif  // #if GFL_DEBUG
{
#if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE
  for( u32 i=0; i<gfl::grp::DISPLAY_BOTH_MAX; ++i )
  {
    m_frame_buffer_render_target_array[i] = NULL;
  }
#endif
}
BloomFilter::~BloomFilter()
{
  Destroy();
}

void BloomFilter::Create(
    gfl::heap::NwAllocator*         heap_allocator,
    gfl::heap::NwAllocator*         device_allocator,
    const gfl::grp::GraphicsSystem* graphics_system,
    const gfl::grp::MemoryArea      frame_buffer_memory_area,
    const Description*              description
)
{
  // description
  Description l_description;
  if( description )
  {
    l_description = *description;
  }
  else
  {
    gfl::grp::g3d::BloomFilter::InitializeDescriptionForDefault(&l_description);
  }

#if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE
  // frame_buffer_render_target
  gfl::grp::RenderTarget* on_screen_render_target_upper = graphics_system->GetCurrentFrameBuffer(gfl::grp::DISPLAY_UPPER);
  for( u32 i=0; i<gfl::grp::DISPLAY_BOTH_MAX; ++i )
  {
    gfl::grp::RenderTarget* on_screen_render_target = graphics_system->GetCurrentFrameBuffer(static_cast<gfl::grp::DisplayType>(i));  // @note フレームバッファは必ず3つあると思っていていいのか？→ないときは左目の情報を使用することで安全になった。
    if( on_screen_render_target == NULL ){
      // 左目の情報を使用する。
      on_screen_render_target = graphics_system->GetCurrentFrameBuffer(gfl::grp::DISPLAY_UPPER);
    }

    gfl::grp::RenderTarget::Description off_screen_description;
    off_screen_description.type         = gfl::grp::RenderTarget::TYPE_OFF_SCREEN_BUFFER;
    off_screen_description.width        = on_screen_render_target_upper->GetWidth();//512;//on_screen_render_target->GetWidth();  // @note 上画面は512x256で、同じバッファを使用しているのに、
    off_screen_description.height       = on_screen_render_target_upper->GetHeight();//256;//on_screen_render_target->GetHeight();  // @note 下画面は320x240が設定されてしまっている。それに対応するために今は直書き。→左目の情報を必ず使用することで安全になった。
    off_screen_description.color_format = on_screen_render_target->GetRenderColorFormat();
    off_screen_description.depth_format = gfl::grp::RENDER_DEPTH_FORMAT_NONE;
    off_screen_description.color_area   = frame_buffer_memory_area;//gfl::grp::MEMORY_AREA_VRAMA;  // @note フレームバッファ3つとも同じメモリエリアでいいか？→呼び出し元で設定してもらうことで安全になった。
        // アドレス指定しているのに、gfl::grp::MEMORY_AREA_NONEでは 
        // gfx_IRenderTarget.cpp:88 Panic:NW:Failed assertion (locationFlag == MEMORY_AREA_VRAMA) || (locationFlag == MEMORY_AREA_VRAMB)
        // というエラーになる。
    off_screen_description.depth_area   = gfl::grp::MEMORY_AREA_NONE;
    off_screen_description.color_address         = on_screen_render_target->GetColorAddress();
    off_screen_description.depth_address         = 0;
    off_screen_description.shadow_kind           = gfl::grp::SHADOW_KIND_NONE;
    off_screen_description.mipmap_size           = 1;
    off_screen_description.color_transfer        = gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP;
    off_screen_description.executing_memory_fill = false;
    off_screen_description.is_dynamic_allocation = false;
    off_screen_description.is_perspective_shadow = true;
    off_screen_description.shadow_z_bias         = 0.0f;
    off_screen_description.shadow_z_scale        = 1.0f;
    off_screen_description.display_type          = static_cast<gfl::grp::DisplayType>(i);
    off_screen_description.fob_id                = 0;
 
    m_frame_buffer_render_target_array[i] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::RenderTarget;
    m_frame_buffer_render_target_array[i]->Create(heap_allocator, device_allocator, &off_screen_description);

    f32 vleft, vtop, vright, vbottom;
    on_screen_render_target->GetViewport(&vleft, &vtop, &vright, &vbottom);
    m_frame_buffer_render_target_array[i]->SetViewport(vleft, vtop, vright, vbottom);
  }
#else
  // m_frame_buffer_copy_texture
  {
    GFL_UNUSED(frame_buffer_memory_area);
    
    if( l_description.frame_buffer_copy_texture_memory_area == gfl::grp::MEMORY_AREA_NONE )
    {
      GFL_ASSERT_MSG( 0, "frame_buffer_copy_texture_memory_areaがgfl::grp::MEMORY_AREA_NONEには対応していません。\n");
      l_description.frame_buffer_copy_texture_memory_area = gfl::grp::MEMORY_AREA_FCRAM;
    }

    //for( u32 i=0; i<gfl::grp::DISPLAY_BOTH_MAX; ++i )
    //{
    //  gfl::grp::RenderTarget* on_screen_render_target = graphics_system->GetCurrentFrameBuffer(static_cast<gfl::grp::DisplayType>(i));  // @note フレームバッファは必ず3つあると思っていていいのか？→未使用変数なので気にしなくていい。
    //  // @note 最もサイズの大きいレンダーターゲットのサイズを記憶し、copy_textureをその大きさで作成する。→最大サイズを直接記入することにした。 
    //}

    gfl::grp::g3d::Texture::Description  copy_texture_description;
    copy_texture_description.type                  = gfl::grp::g3d::Texture::TYPE_IMAGE_TEXTURE;
    copy_texture_description.height                = 512;//256;
    copy_texture_description.width                 = 256;//512;
    copy_texture_description.memory_area           = l_description.frame_buffer_copy_texture_memory_area;
    copy_texture_description.memory_transfer       = gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP;
    copy_texture_description.location_address      = 0;
    copy_texture_description.executing_memory_fill = true;  // 確保したピクセル領域をゼロでクリアするかどうかを設定します。
                                                            // フレームバッファ外のところが明るくならないように、真っ暗(R=G=B=0)で初期化しておく。
    copy_texture_description.is_dynamic_allocation = (copy_texture_description.memory_area==gfl::grp::MEMORY_AREA_FCRAM);  // FCRAMはtrueにしなければならない
    copy_texture_description.mipmap_size           = 1;  // ミップマップサイズを設定します。 ミップマップを使用しないときは 1 を、 ミップマップを使用するときは ミップマップ数 + 1 を指定します。
    copy_texture_description.format                = gfl::grp::g3d::Texture::FORMAT_RGBA8;  // @note レンダーターゲットフォーマットを取得し、それをテクスチャフォーマットに変換してここに代入すること。→このフォーマットのテクスチャが一番豪華なのでこれに描けばいい。
    copy_texture_description.is_perspective_shadow = true;
    copy_texture_description.shadow_z_bias         = 0.0f;
    copy_texture_description.shadow_z_scale        = 1.0f;

    m_frame_buffer_copy_texture = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Texture;
    m_frame_buffer_copy_texture->Create(heap_allocator, device_allocator, &copy_texture_description);
  }
#endif

  // high_luminance_board
  m_high_luminance_board_resource = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource;
  
  {
    void * p_buff = System::GetStaticResource( System::BLOOM_FILTER_HIGH_LUMINANCE_BOARD_BCRES );
    if ( p_buff )
    {
      Resource::Description     desc;

      m_high_luminance_board_resource->AttachBufferAndSetup(device_allocator, p_buff,
                                                    &desc, false, Resource::COMMON_RESOURCE_NONE);
    }
    else
    {
      m_high_luminance_board_resource->LoadAndSetup(device_allocator, GFL_GRP_G3D_BLOOM_FILTER_HIGH_LUMINANCE_BOARD_FILE);
    }
  }

  m_high_luminance_board_model = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model;
  m_high_luminance_board_model->Create(heap_allocator, device_allocator, m_high_luminance_board_resource, 0);

  m_high_luminance_board_scene_draw_group_index = l_description.high_luminance_board_scene_draw_group_index;

  SetHighLuminanceColorWeight(l_description.high_luminance_color_weight);
  SetHighLuminanceMinValue(l_description.high_luminance_min_value);

  // high_luminance_board_render_target
  gfl::grp::RenderTarget::Description render_target_description;
  render_target_description.type         = gfl::grp::RenderTarget::TYPE_OFF_SCREEN_BUFFER;
  render_target_description.width        = l_description.high_luminance_board_render_target_width;
  render_target_description.height       = l_description.high_luminance_board_render_target_height;
  {
    gfl::grp::RenderColorFormat render_color_format;
    switch(l_description.high_luminance_board_render_target_format)
    {
    case gfl::grp::g3d::Texture::FORMAT_RGBA8   : {  render_color_format = RENDER_COLOR_FORMAT_RGBA8;    } break;
    case gfl::grp::g3d::Texture::FORMAT_RGB8    : {  render_color_format = RENDER_COLOR_FORMAT_RGB8;     } break;   
    case gfl::grp::g3d::Texture::FORMAT_RGBA5551: {  render_color_format = RENDER_COLOR_FORMAT_RGBA4;    } break;
    case gfl::grp::g3d::Texture::FORMAT_RGB565  : {  render_color_format = RENDER_COLOR_FORMAT_RGB5_A1;  } break;
    case gfl::grp::g3d::Texture::FORMAT_RGBA4   : {  render_color_format = RENDER_COLOR_FORMAT_RGB565;   } break;
    default: {  GFL_ASSERT_MSG( 0, "レンダーターゲットのフォーマットが異常です。" );  render_color_format = RENDER_COLOR_FORMAT_RGBA8;  } break;
    }

// オフスクリーンバッファのフォーマットにRGB8はないので注意。
// gfx_OffScreenBuffer.cpp:55 Panic:NW:Fatal Error
// illegal texture format for OffScreenBuffer
// OffScreenBuffer::OffScreenBuffer(os::IAllocator* pAllocator, const Description& description, ResPixelBasedTexture resTexture)
//    switch ( resTexture.GetFormatHW() )
//    {
//    case ResPixelBasedTexture::FORMAT_HW_RGBA8:    format = GL_RGBA8_OES; break;
//    case ResPixelBasedTexture::FORMAT_HW_RGBA5551: format = GL_RGB5_A1; break;
//    case ResPixelBasedTexture::FORMAT_HW_RGBA4:    format = GL_RGBA4; break;
//    case ResPixelBasedTexture::FORMAT_HW_RGB565:   format = GL_RGB565; break;
//    default:
//        NW_FATAL_ERROR("illegal texture format for OffScreenBuffer");
//    }

    render_target_description.color_format = render_color_format;
  }
  render_target_description.depth_format          = gfl::grp::RENDER_DEPTH_FORMAT_NONE;
  render_target_description.color_area            = l_description.high_luminance_board_render_target_color_memory_area;
      // gfx_IRenderTarget.cpp:88 Panic:NW:Failed assertion (locationFlag == MEMORY_AREA_VRAMA) || (locationFlag == MEMORY_AREA_VRAMB)
      // というエラーになるので、レンダーターゲットにはVRAMAかVRAMBを用いなければならない。
      // しかし、CTR-SDKでは、下記のようにFCRAMを使ってもいいらしい。
#if 0
/*[質問] シャドウテクスチャについて

田中＠NTSC 2010-04-27 11:19:24  
はい。
レンダリングのターゲットとなるメモリに関してはFCRAMを設定することはできません。
VRAMを使用していただくようお願いいたします。 

田中＠NTSC 2010-05-12 21:00:37  
申し訳ありません。
回答を一部訂正させていただきます。

>FCRAMへのレンダリングの件ですが、
>動作保障されたものではないとのことで使用は禁止とさせていただきます。
再調査した結果、使用していただいても問題がないということが判明しました。
その為、ご利用いただいても問題はありません。
ただ上記したようにglClearは実行できません。
またVRAMへ描画するよりは遅いという点にご注意ください。

→レンダリングのターゲットにFCRAMを使用してもいい、という結論のようだ。*/
#endif
  render_target_description.depth_area            = gfl::grp::MEMORY_AREA_NONE;
  render_target_description.color_address         = 0;
  render_target_description.depth_address         = 0;
  render_target_description.shadow_kind           = gfl::grp::SHADOW_KIND_NONE;
  render_target_description.mipmap_size           = 1;
  render_target_description.color_transfer        = gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP;
  render_target_description.executing_memory_fill = false;
  render_target_description.is_dynamic_allocation = (render_target_description.color_area==gfl::grp::MEMORY_AREA_FCRAM);
      // gfx_ResTexture.cpp:237 Panic:NW:Failed assertion (loadFlag != (NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP)) || resImage.CheckMemoryLocation()
      // というエラーになるので、FCRAMのときはtrueにする。
  render_target_description.is_perspective_shadow = true;
  render_target_description.shadow_z_bias         = 0.0f;
  render_target_description.shadow_z_scale        = 1.0f;
  render_target_description.display_type          = gfl::grp::DISPLAY_NONE;
  render_target_description.fob_id                = 0;

  m_high_luminance_board_render_target = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::RenderTarget;
  m_high_luminance_board_render_target->Create(heap_allocator, device_allocator, &render_target_description);

  // m_high_luminance_board_render_targetはテクスチャ全面がビューポートでよい。

  // small_image
  m_small_image_num = l_description.small_image_num;

  if( m_small_image_num > 0 )
  {
    nw::gfx::res::ResImageTexture res_image_texture = nw::gfx::ResDynamicCast<nw::gfx::res::ResImageTexture>(m_high_luminance_board_render_target->GetNwResTexture());
    s32 high_luminance_board_width  = res_image_texture.GetWidth();  // 縦横が入れ替わっているといけないので、実際にNW4Cが保持している値を得ることにする。
    s32 high_luminance_board_height = res_image_texture.GetHeight();

    m_small_image_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) Texture*[m_small_image_num];
    for( u32 i=0; i<m_small_image_num; ++i )
    {
      Texture::Description texture_description;
      texture_description.type                  = Texture::TYPE_IMAGE_TEXTURE;
      texture_description.height                = high_luminance_board_height /2;
      texture_description.width                 = high_luminance_board_width /2;
      for( u32 j=0; j<i; ++j )
      {
        texture_description.height = texture_description.height /2;
        texture_description.width  = texture_description.width /2;
      } 
      texture_description.memory_area           = l_description.high_luminance_board_render_target_color_memory_area;
      texture_description.memory_transfer       = gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP;
          // NTSC-ONLINE
          // [質問][NW4C]ＣＴＲのカメラで撮影した画像をモデルのテクスチャとして使用するには？
          // 補足ですがこの場合はLocationFlagをFCRAM | NO_COPYにしておいてください。
      texture_description.location_address      = 0;
      texture_description.executing_memory_fill = false;
      texture_description.is_dynamic_allocation = true;  // falseだと
                                                         // gfx_ResTexture.cpp:237 Panic:NW:Failed assertion (loadFlag != (NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP)) || resImage.CheckMemoryLocation()
                                                         // に引っ掛かる。
      texture_description.mipmap_size           = 1;
      texture_description.format                = l_description.high_luminance_board_render_target_format;
      texture_description.is_perspective_shadow = true;
      texture_description.shadow_z_bias         = 0.0f;
      texture_description.shadow_z_scale        = 1.0f;

      m_small_image_array[i] = GFL_NEW(heap_allocator->GetHeapBase()) Texture;
      m_small_image_array[i]->Create(heap_allocator, device_allocator, &texture_description);
    }
  }

  // bloom_board
  m_bloom_board_scene_draw_group_index = l_description.bloom_board_scene_draw_group_index;
  
  m_bloom_board_num = l_description.bloom_board_num;

  if( m_bloom_board_num > 0 )
  {
    m_bloom_board_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) BloomBoard[m_bloom_board_num];

    BloomBoardDescription* l_bloom_board_description_array = GFL_NEW_LOW_ARRAY( heap_allocator->GetHeapBase() ) BloomBoardDescription[m_bloom_board_num];
    if( l_description.bloom_board_description_array )
    {
      for( u32 i=0; i<m_bloom_board_num; ++i )
      {
        l_bloom_board_description_array[i] = l_description.bloom_board_description_array[i];
      }
    }
    else
    {
      for( u32 i=0; i<m_bloom_board_num; ++i )
      {
        gfl::grp::g3d::BloomFilter::InitializeBloomBoardDescriptionForDefault( &(l_bloom_board_description_array[i]) );
      }
    }
    
    {
      void*                   buff = System::GetStaticResource( System::BLOOM_FILTER_BLOOM_BOARD_BCRES );
      Resource::Description   desc;

      for( u32 i=0; i<m_bloom_board_num; ++i )
      {
        m_bloom_board_array[i].resource = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource;

        if ( buff )
          m_bloom_board_array[i].resource->AttachBufferAndSetup(device_allocator, buff, &desc, false, Resource::COMMON_RESOURCE_NONE);
        else
          m_bloom_board_array[i].resource->LoadAndSetup(device_allocator, GFL_GRP_G3D_BLOOM_FILTER_BLOOM_BOARD_FILE);

        m_bloom_board_array[i].model = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model;
        m_bloom_board_array[i].model->Create(heap_allocator, device_allocator, m_bloom_board_array[i].resource, 0);

        m_bloom_board_array[i].small_image_count = l_bloom_board_description_array[i].small_image_count;
        m_bloom_board_array[i].uv                = l_bloom_board_description_array[i].uv;
        m_bloom_board_array[i].rate              = l_bloom_board_description_array[i].rate;
      }
    }

    for( u32 i=0; i<m_bloom_board_num; ++i )
    {
      nw::gfx::Material*    material     = m_bloom_board_array[i].model->GetNwModel()->GetMaterial(0);
      nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
      nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(0);
      nw::gfx::ResTexture dummy_texture                      = res_texture_mapper.GetTexture().Dereference();
      dummy_texture.Cleanup();
      if( m_bloom_board_array[i].small_image_count == 0 )
      {
        // コマンド蓄積
        res_texture_mapper.SetTexture(m_high_luminance_board_render_target->GetNwResTexture());
            // NintendoWare for CTR 関数リファレンスマニュアル
            // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
      }
      else
      {
        // コマンド蓄積
        res_texture_mapper.SetTexture(m_small_image_array[m_bloom_board_array[i].small_image_count-1]->GetNwResTexture());
            // NintendoWare for CTR 関数リファレンスマニュアル
            // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
      }
      res_material.SetTextureMappersHash(0x0);

      // uv
      res_material = material->GetTextureCoordinatorResMaterial();
      nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(0); 
      // コマンド蓄積
      res_texture_coordinator.SetTranslate(
          gfl::math::VEC2(
            m_bloom_board_array[i].uv.x * m_bloom_board_expanse,
            m_bloom_board_array[i].uv.y * m_bloom_board_expanse
          )
      );
      res_material.SetTextureCoordinatorsHash(0x0);
      
      // rate
      res_material = material->GetMaterialColorResMaterial();
      nw::gfx::ResMaterialColor res_material_color = res_material.GetMaterialColor();
      res_material_color.SetConstant0(
          m_bloom_board_array[i].rate * m_bloom_board_intensity,
          m_bloom_board_array[i].rate * m_bloom_board_intensity,
          m_bloom_board_array[i].rate * m_bloom_board_intensity
      );
      res_material.SetMaterialColorHash(0x0);
    }

    GFL_DELETE_ARRAY l_bloom_board_description_array;
  }

  // dummy_camera
  m_dummy_camera_scene_calculate_group_index = l_description.camera_scene_calculate_group_index;
  m_dummy_camera_scene_camera_index          = l_description.camera_scene_camera_index;

  m_dummy_camera = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Camera;
  gfl::grp::g3d::Camera::Description camera_description;
  m_dummy_camera->Create(device_allocator, &camera_description);


#if GFL_DEBUG
  // texture_check_board
  CreateTextureCheckBoardModel(heap_allocator, device_allocator);
#endif  // #if GFL_DEBUG
}

void BloomFilter::Destroy(void)
{
#if GFL_DEBUG
  // texture_check_board
  DestroyTextureCheckBoardModel();
#endif  // #if GFL_DEBUG


  if( m_dummy_camera )
  {
    GFL_DELETE m_dummy_camera;
    m_dummy_camera = NULL;
  }

  if( m_bloom_board_array )
  {
    for( u32 i=0; i<m_bloom_board_num; ++i )
    {
      if( m_bloom_board_array[i].model )
      {
        GFL_DELETE m_bloom_board_array[i].model;
        m_bloom_board_array[i].model = NULL;
      }
      if( m_bloom_board_array[i].resource )
      {
        GFL_DELETE m_bloom_board_array[i].resource;
        m_bloom_board_array[i].resource = NULL;
      }
    }
    GFL_DELETE_ARRAY m_bloom_board_array;
    m_bloom_board_array = NULL;
  }

  if( m_small_image_array )
  {
    for( u32 i=0; i<m_small_image_num; ++i )
    {
      if( m_small_image_array[i] )
      {
        GFL_DELETE m_small_image_array[i];
        m_small_image_array[i] = NULL;
      }
    }
    GFL_DELETE_ARRAY m_small_image_array;
    m_small_image_array = NULL;
  }

  if( m_high_luminance_board_render_target )
  {
    GFL_DELETE m_high_luminance_board_render_target;
    m_high_luminance_board_render_target = NULL;
  }

  if( m_high_luminance_board_model )
  {
    GFL_DELETE m_high_luminance_board_model;
    m_high_luminance_board_model = NULL;
  }
  if( m_high_luminance_board_resource )
  {
    GFL_DELETE m_high_luminance_board_resource;
    m_high_luminance_board_resource = NULL;
  }

#if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE
  for( u32 i=0; i<gfl::grp::DISPLAY_BOTH_MAX; ++i )
  {
    if( m_frame_buffer_render_target_array[i] )
    {
      GFL_DELETE m_frame_buffer_render_target_array[i];
      m_frame_buffer_render_target_array[i] = NULL;
    }
  }
#else
  if( m_frame_buffer_copy_texture )
  {
    GFL_DELETE m_frame_buffer_copy_texture;
    m_frame_buffer_copy_texture = NULL;
  }
#endif
}

void BloomFilter::CopySmallerAll(void)
{
  if( m_small_image_num > 0 )
  {
    CopySmaller( m_high_luminance_board_render_target->GetNwResTexture(), m_small_image_array[0]->GetNwResTexture() );
  }
  for( u32 i=1; i<m_small_image_num; ++i )
  {
    CopySmaller( m_small_image_array[i-1]->GetNwResTexture(), m_small_image_array[i]->GetNwResTexture() );
  }
}

void BloomFilter::SetPlainImageFrameBufferToHighLuminanceBoard(
    gfl::grp::GraphicsSystem*       graphics_system,
    const gfl::grp::DisplayType     plain_image_display_type
)
{
#if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE
  GFL_ASSERT_STOP_MSG( plain_image_display_type<gfl::grp::DISPLAY_BOTH_MAX, "ディスプレイタイプ%d>=%dが異常です。\n", plain_image_display_type, gfl::grp::DISPLAY_BOTH_MAX );

  GFL_UNUSED(graphics_system);

  nw::gfx::Material*    material     = m_high_luminance_board_model->GetNwModel()->GetMaterial(0);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(0);
  //nw::gfx::ResTexture dummy_texture                      = res_texture_mapper.GetTexture().Dereference();
  //dummy_texture.Cleanup();
  // コマンド蓄積
  res_texture_mapper.SetTexture(m_frame_buffer_render_target_array[plain_image_display_type]->GetNwResTexture());
      // NintendoWare for CTR 関数リファレンスマニュアル
      // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
  res_material.SetTextureMappersHash(0x0);

  // uv
  f32 vleft, vtop, vright, vbottom;
  m_frame_buffer_render_target_array[plain_image_display_type]->GetViewport(&vleft, &vtop, &vright, &vbottom);
  gfl::math::VEC2 scale(
      (vbottom-vtop)/m_frame_buffer_render_target_array[plain_image_display_type]->GetHeight(),  // 縦横が逆になっている。
      (vright-vleft)/m_frame_buffer_render_target_array[plain_image_display_type]->GetWidth()
  );
  //gfl::math::VEC2 translate(  // 基点が(vtop=0,vleft=0)から動かないならこれでもいいが、動いた場合は下記のようにしないといけない。
  //    0.0f,  // 縦横が逆になっている。
  //    -(1.0f-scale.y)/scale.y
  //);
  f32 vt = ( vtop>0.0f)? vtop:0.0f;  // 基点がマイナスの場合はそのマイナス値が画像サイズの端に合うようになるみたい。
  f32 vl = (vleft>0.0f)?vleft:0.0f;  // 問：(終点-基点)が画像サイズを越えた場合はどうなるのだろうか？ → 答：反対側から出て来る。ただ、縦方向で反対側から出て来たとき、横にずれるという謎が残る。横方向は検証していない。
  gfl::math::VEC2 translate(
      - vt / m_frame_buffer_render_target_array[plain_image_display_type]->GetHeight() / scale.x,  // 縦横が逆になっている。
      - vl / m_frame_buffer_render_target_array[plain_image_display_type]->GetWidth()  / scale.y  // scaleのxyは既に逆にしてある。
  );
  // @note ここで変更を反映せずに、それぞれのSet関数内で反映したほうがいいかもしれない。
  for( u32 i=0; i<m_bloom_board_num; ++i )
  {
    // small_image_count
    nw::gfx::Material*    l_material     = m_bloom_board_array[i].model->GetNwModel()->GetMaterial(0);
    nw::gfx::ResMaterial  l_res_material = l_material->GetTextureMapperResMaterial();
    nw::gfx::ResPixelBasedTextureMapper l_res_texture_mapper = l_res_material.GetTextureMappers(0);
    //nw::gfx::ResTexture dummy_texture                        = l_res_texture_mapper.GetTexture().Dereference();
    //dummy_texture.Cleanup();  // この時点で設定されているテクスチャはデフォルトのテクスチャではないのでCleanupしてはならない。
    if( m_bloom_board_array[i].small_image_count == 0 )
    {
      // コマンド蓄積
      l_res_texture_mapper.SetTexture(m_high_luminance_board_render_target->GetNwResTexture());
          // NintendoWare for CTR 関数リファレンスマニュアル
          // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
    }
    else
    {
      // コマンド蓄積
      l_res_texture_mapper.SetTexture(m_small_image_array[m_bloom_board_array[i].small_image_count-1]->GetNwResTexture());
          // NintendoWare for CTR 関数リファレンスマニュアル
          // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
    }
    l_res_material.SetTextureMappersHash(0x0);

    // uv
    l_res_material = l_material->GetTextureCoordinatorResMaterial();
    nw::gfx::ResTextureCoordinator l_res_texture_coordinator = l_res_material.GetTextureCoordinators(0); 
    gfl::math::VEC2 t(
        m_bloom_board_array[i].uv.x * m_bloom_board_expanse + translate.x,
        m_bloom_board_array[i].uv.y * m_bloom_board_expanse + translate.y
    );
    // コマンド蓄積
    l_res_texture_coordinator.SetScale(scale);
    l_res_texture_coordinator.SetTranslate(t);
    l_res_material.SetTextureCoordinatorsHash(0x0);

    // rate
    l_res_material = l_material->GetMaterialColorResMaterial();
    nw::gfx::ResMaterialColor l_res_material_color = l_res_material.GetMaterialColor(); 
    // コマンド蓄積か?
    l_res_material_color.SetConstant0(
        m_bloom_board_array[i].rate * m_bloom_board_intensity,
        m_bloom_board_array[i].rate * m_bloom_board_intensity,
        m_bloom_board_array[i].rate * m_bloom_board_intensity
    );
    l_res_material.SetMaterialColorHash(0x0);
  }
#else
  gfl::grp::RenderTarget*   render_target  = graphics_system->GetCurrentFrameBuffer( plain_image_display_type );
  nw::gfx::res::ResTexture  res_texture    =  m_frame_buffer_copy_texture->GetNwResTexture();

  // オンスクリーンバッファであるフレームバッファの絵をテクスチャにコピーする
  {
    if( graphics_system->BindCurrentCommandList( plain_image_display_type ) )
    {
      nngxFlush3DCommand();
      
      // src
      u32 src_address = render_target->GetColorAddress();

      // dst
      nw::gfx::res::ResImageTexture     res_image_texture      =  nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(res_texture);
      nw::gfx::res::ResPixelBasedImage  res_pixel_based_image  =  res_image_texture.GetImage();
      u8* dst_address = res_pixel_based_image.GetImageData();

#if 0
/*[質問][NW4C] モデルに使用しているテクスチャの画像データを取得するには？

Altron_060 2010-10-25 09:10:42  
お世話になっております。

モデルに使用しているテクスチャの画像データを取得したいのですが、
マテリアルを指定し、テクスチャマッパーから取得できるテクスチャは
nw::gfx::res::ResTextureクラスのため、画像データを取得する関数がなく、
画像の取得ができません。

良い方法があればお教えいただけないでしょうか。
よろしくお願いいたします。 

田中＠NTSC 2010-10-25 16:01:58  
テクスチャマッパーのResTextureはResRefernceTexutreになっています。
普通のテクスチャであればDereferenceし、
ResImageTextureにキャストしてデータを得ればよいと思います。

具体的に書くと下記のようになります。

nw::gfx::ResTexture oTexture = oMaterial.GetTextureMappers(0).GetTexture();

nw::gfx::ResImageTexture oImageTexture = nw::ut::ResDynamicCast<nw::gfx::ResImageTexture>(oTexture.Dereference());
if(oImageTexture.IsValid()){
nw::gfx::ResPixelBasedImage oImage = oImageTexture.GetImage();
    NN_LOG("name = %s, addr = %p", oImageTexture.GetName(), oImage.GetImageData());
}*/
#endif

#if 0
    // 400*240を512*256にコピー
    // 定数  // @note 定数でなくきちんとフレームバッファの値を取得すること→3DSである限りは大丈夫。
    u32 format_size = 4;
    u32 width = (plain_image_display_type==gfl::grp::DISPLAY_LOWER)?(320):(400);
    u32 size = width * 240 * format_size;

    static const int BLOCK_SIZE = 8*8;
    static const int DOT_BYTE_SIZE = 4;
    // 8 block line > 240(30) + 16(2)
    nngxAddBlockImageCopyCommand( 
        reinterpret_cast<const GLvoid*>(src_address),
        BLOCK_SIZE * DOT_BYTE_SIZE * 30,
        0,
        reinterpret_cast<GLvoid*>(dst_address),
        BLOCK_SIZE * DOT_BYTE_SIZE * 30,
        BLOCK_SIZE * DOT_BYTE_SIZE * 2,
        size );
#else
      // 512*256を512*256にコピー
      // 定数  // @note 定数でなくきちんとフレームバッファの値を取得すること→3DSである限りは大丈夫。
      u32 format_size = 4;
      u32 width = (plain_image_display_type==gfl::grp::DISPLAY_LOWER)?(320):(400);
      u32 size = width * 240 * format_size;

      static const int BLOCK_SIZE = 8*8;
      static const int DOT_BYTE_SIZE = 4;
      // 8 block line > 240(30) + 16(2)
      nngxAddBlockImageCopyCommand( 
          //reinterpret_cast<const GLvoid*>(src_address) + (512-400),
          reinterpret_cast<const GLvoid*>( src_address + BLOCK_SIZE * DOT_BYTE_SIZE * 32 * (512-400)/8 ),
          BLOCK_SIZE * DOT_BYTE_SIZE * 30,
          BLOCK_SIZE * DOT_BYTE_SIZE * 2,
          reinterpret_cast<GLvoid*>(dst_address),
          BLOCK_SIZE * DOT_BYTE_SIZE * 30,
          BLOCK_SIZE * DOT_BYTE_SIZE * 2,
          size );
#endif

      nngxFlush3DCommand();
    }
  }

  // テクスチャをhigh_luminance板に貼り付ける
  {
    nw::gfx::Material*    material     = m_high_luminance_board_model->GetNwModel()->GetMaterial(0);
    nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
    nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(0);
    //nw::gfx::ResTexture dummy_texture                      = res_texture_mapper.GetTexture().Dereference();
    //dummy_texture.Cleanup();
    // コマンド蓄積
    res_texture_mapper.SetTexture(res_texture);
        // NintendoWare for CTR 関数リファレンスマニュアル
        // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
    res_material.SetTextureMappersHash(0x0);
 
    // uv
    f32 vleft, vtop, vright, vbottom;
    render_target->GetViewport(&vleft, &vtop, &vright, &vbottom);
    gfl::math::VEC2 scale(  // @note 定数でなくきちんとフレームバッファの値を取得すること→3DSである限りは大丈夫。
        240 / 256.0f,  // 縦横が逆になっている。
        ( (plain_image_display_type==gfl::grp::DISPLAY_LOWER)?(320):(400) ) / 512.0f
    );
    f32 vt = ( vtop>0.0f)? vtop:0.0f;  // 基点がマイナスの場合はそのマイナス値が画像サイズの端に合うようになるみたい。
    f32 vl = (vleft>0.0f)?vleft:0.0f;  // 問：(終点-基点)が画像サイズを越えた場合はどうなるのだろうか？ → 答：反対側から出て来る。ただ、縦方向で反対側から出て来たとき、横にずれるという謎が残る。横方向は検証していない。
    gfl::math::VEC2 translate(  // @note 定数でなくきちんとフレームバッファの値を取得すること  // @note そのせいで下画面ずれてます→3DSである限りは大丈夫。
        0.0f,  // 縦横が逆になっている。
        - (512.0f - 400.0f) / 512.0f / scale.y  // scaleのxyは既に逆にしてある。
    );
    // @note ここで変更を反映せずに、それぞれのSet関数内で反映したほうがいいかもしれない。
    for( u32 i=0; i<m_bloom_board_num; ++i )
    {
      // small_image_count
      nw::gfx::Material*    l_material     = m_bloom_board_array[i].model->GetNwModel()->GetMaterial(0);
      nw::gfx::ResMaterial  l_res_material = l_material->GetTextureMapperResMaterial();
      nw::gfx::ResPixelBasedTextureMapper l_res_texture_mapper = l_res_material.GetTextureMappers(0);
      //nw::gfx::ResTexture dummy_texture                        = l_res_texture_mapper.GetTexture().Dereference();
      //dummy_texture.Cleanup();  // この時点で設定されているテクスチャはデフォルトのテクスチャではないのでCleanupしてはならない。
      if( m_bloom_board_array[i].small_image_count == 0 )
      {
        // コマンド蓄積
        l_res_texture_mapper.SetTexture(m_high_luminance_board_render_target->GetNwResTexture());
            // NintendoWare for CTR 関数リファレンスマニュアル
            // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
      }
      else
      {
        // コマンド蓄積
        l_res_texture_mapper.SetTexture(m_small_image_array[m_bloom_board_array[i].small_image_count-1]->GetNwResTexture());
            // NintendoWare for CTR 関数リファレンスマニュアル
            // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
      }
      l_res_material.SetTextureMappersHash(0x0);

      // uv
      l_res_material = l_material->GetTextureCoordinatorResMaterial();
      nw::gfx::ResTextureCoordinator l_res_texture_coordinator = l_res_material.GetTextureCoordinators(0); 
      gfl::math::VEC2 t(
          m_bloom_board_array[i].uv.x * m_bloom_board_expanse + translate.x,
          m_bloom_board_array[i].uv.y * m_bloom_board_expanse + translate.y
      );
      // コマンド蓄積
      l_res_texture_coordinator.SetScale(scale);
      l_res_texture_coordinator.SetTranslate(t);
      l_res_material.SetTextureCoordinatorsHash(0x0);

      // rate
      l_res_material = l_material->GetMaterialColorResMaterial();
      nw::gfx::ResMaterialColor l_res_material_color = l_res_material.GetMaterialColor(); 
      // コマンド蓄積か?
      l_res_material_color.SetConstant0(
          m_bloom_board_array[i].rate * m_bloom_board_intensity,
          m_bloom_board_array[i].rate * m_bloom_board_intensity,
          m_bloom_board_array[i].rate * m_bloom_board_intensity
      );
      l_res_material.SetMaterialColorHash(0x0);
    }
  }
#endif
}

void BloomFilter::SetPlainImageOffScreenBufferToHighLuminanceBoard(const gfl::grp::RenderTarget* plain_image_off_screen_buffer)
{
  nw::gfx::Material*    material     = m_high_luminance_board_model->GetNwModel()->GetMaterial(0);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(0);
  //nw::gfx::ResTexture dummy_texture                      = res_texture_mapper.GetTexture().Dereference();
  //dummy_texture.Cleanup();
  // コマンド蓄積
  res_texture_mapper.SetTexture(plain_image_off_screen_buffer->GetNwResTexture());
      // NintendoWare for CTR 関数リファレンスマニュアル
      // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
  res_material.SetTextureMappersHash(0x0);

#if 0
  //オフスクリーンなので、ビューポートのどの範囲が見えているとかないので、uvを変更する意味がない気がする。
  // uv
  const gfl::grp::Viewport* viewport = plain_image_off_screen_buffer->GetViewport();
  f32 vleft, vtop, vright, vbottom;
  viewport->GetBound(&vleft, &vtop, &vright, &vbottom);
  gfl::math::VEC2 scale(
      (vright-vleft)/plain_image_off_screen_buffer->GetWidth(),
      (vbottom-vtop)/plain_image_off_screen_buffer->GetHeight()
  );
  for( u32 i=0; i<m_bloom_board_num; ++i )
  {
    nw::gfx::Material*    l_material     = m_bloom_board_array[i].model->GetNwModel()->GetMaterial(0);
    nw::gfx::ResMaterial  l_res_material = l_material->GetTextureCoordinatorResMaterial();
    nw::gfx::ResTextureCoordinator l_res_texture_coordinator = l_res_material.GetTextureCoordinators(0); 
    // コマンド蓄積
    l_res_texture_coordinator.SetScale(scale);
    l_res_material.SetTextureCoordinatorsHash(0x0);
  }
#else
  // しかし、m_bloom_board_expanseとm_bloom_board_intensityを追加したので、その変更を反映しなければならない。
  // さらに、small_image_countも変更できるようにしたので、その変更も反映しなければならない。
  // @note ここで変更を反映せずに、それぞれのSet関数内で反映したほうがいいかもしれない。
  for( u32 i=0; i<m_bloom_board_num; ++i )
  {
    // small_image_count
    nw::gfx::Material*    l_material     = m_bloom_board_array[i].model->GetNwModel()->GetMaterial(0);
    nw::gfx::ResMaterial  l_res_material = l_material->GetTextureMapperResMaterial();
    nw::gfx::ResPixelBasedTextureMapper l_res_texture_mapper = l_res_material.GetTextureMappers(0);
    //nw::gfx::ResTexture dummy_texture                        = l_res_texture_mapper.GetTexture().Dereference();
    //dummy_texture.Cleanup();  // この時点で設定されているテクスチャはデフォルトのテクスチャではないのでCleanupしてはならない。
    if( m_bloom_board_array[i].small_image_count == 0 )
    {
      // コマンド蓄積
      l_res_texture_mapper.SetTexture(m_high_luminance_board_render_target->GetNwResTexture());
          // NintendoWare for CTR 関数リファレンスマニュアル
          // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
    }
    else
    {
      // コマンド蓄積
      l_res_texture_mapper.SetTexture(m_small_image_array[m_bloom_board_array[i].small_image_count-1]->GetNwResTexture());
          // NintendoWare for CTR 関数リファレンスマニュアル
          // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
    }
    l_res_material.SetTextureMappersHash(0x0);

    // uv
    l_res_material = l_material->GetTextureCoordinatorResMaterial();
    nw::gfx::ResTextureCoordinator l_res_texture_coordinator = l_res_material.GetTextureCoordinators(0); 
    gfl::math::VEC2 t(
        m_bloom_board_array[i].uv.x * m_bloom_board_expanse,
        m_bloom_board_array[i].uv.y * m_bloom_board_expanse
    );
    // コマンド蓄積
    l_res_texture_coordinator.SetTranslate(t);
    l_res_material.SetTextureCoordinatorsHash(0x0);

    // rate
    l_res_material = l_material->GetMaterialColorResMaterial();
    nw::gfx::ResMaterialColor l_res_material_color = l_res_material.GetMaterialColor(); 
    // コマンド蓄積か?
    l_res_material_color.SetConstant0(
        m_bloom_board_array[i].rate * m_bloom_board_intensity,
        m_bloom_board_array[i].rate * m_bloom_board_intensity,
        m_bloom_board_array[i].rate * m_bloom_board_intensity
    );
    l_res_material.SetMaterialColorHash(0x0);
  }
#endif
}

void BloomFilter::SetHighLuminanceColorWeight(const gfl::grp::ColorF32& c)
{
  m_high_luminance_board_model->SetMaterialColorConstant(0, 0, c);
}
void BloomFilter::GetHighLuminanceColorWeight(gfl::grp::ColorF32* c) const
{
  m_high_luminance_board_model->GetMaterialColorConstant(0, 0, c);
}

void BloomFilter::SetHighLuminanceMinValue(const f32 v)
{
  m_high_luminance_board_model->SetMaterialAlphaTestReference(0, v);
}
f32  BloomFilter::GetHighLuminanceMinValue(void) const
{
  return m_high_luminance_board_model->GetMaterialAlphaTestReference(0);
}

void BloomFilter::SetBloomBoardExpanse(const f32 bloom_board_expanse)
{
  m_bloom_board_expanse = bloom_board_expanse;

  // このBloomFilterの呼び出し元のSceneがDrawの際に必ず
  // SetPlainImageFrameBufferToHighLuminanceBoard
  // SetPlainImageOffScreenBufferToHighLuminanceBoard
  // を呼び出してくれるので、そこでこの値が反映される。
}
f32  BloomFilter::GetBloomBoardExpanse(void) const
{
  return m_bloom_board_expanse;
}

void BloomFilter::SetBloomBoardIntensity(const f32 bloom_board_intensity)
{
  m_bloom_board_intensity = bloom_board_intensity;

  // このBloomFilterの呼び出し元のSceneがDrawの際に必ず
  // SetPlainImageFrameBufferToHighLuminanceBoard
  // SetPlainImageOffScreenBufferToHighLuminanceBoard
  // を呼び出してくれるので、そこでこの値が反映される。
}
f32  BloomFilter::GetBloomBoardIntensity(void) const
{
  return m_bloom_board_intensity;
}

void BloomFilter::SetBloomBoardSmallImageCount(const u32 bloom_board_index, const u32 small_image_count)
{
  if( small_image_count > m_small_image_num )
  {
    GFL_ASSERT_MSG(0, "small_image_count %d > m_small_image_num %d\n", small_image_count, m_small_image_num);
    return;
  }
  else if( bloom_board_index >= m_bloom_board_num )
  {
    GFL_ASSERT_MSG(0, "bloom_board_index %d >= m_bloom_board_num %d\n", bloom_board_index, m_bloom_board_num);
    return;
  }
  else
  {
    m_bloom_board_array[bloom_board_index].small_image_count = small_image_count;
  }
}
u32  BloomFilter::GetBloomBoardSmallImageCount(const u32 bloom_board_index) const
{
  if( bloom_board_index >= m_bloom_board_num )
  {
    GFL_ASSERT_MSG(0, "bloom_board_index %d >= m_bloom_board_num %d\n", bloom_board_index, m_bloom_board_num);
    return 0;
  }
  else
  {
    return m_bloom_board_array[bloom_board_index].small_image_count;
  }
}


void BloomFilter::CopySmaller( const nw::gfx::res::ResTexture src_texture, nw::gfx::res::ResTexture dst_texture )
{
  nw::gfx::res::ResImageTexture    src_res_image_texture     = nw::gfx::ResDynamicCast<nw::gfx::res::ResImageTexture>(src_texture);
  nw::gfx::res::ResPixelBasedImage src_res_pixel_based_image = src_res_image_texture.GetImage();
  u32 src_address = src_res_pixel_based_image.GetLocationAddress();
  if( !src_address )
  {
    src_address = src_res_pixel_based_image.GetImageAddress();
  }

  nw::gfx::res::ResImageTexture    dst_res_image_texture     = nw::gfx::ResDynamicCast<nw::gfx::res::ResImageTexture>(dst_texture);
  nw::gfx::res::ResPixelBasedImage dst_res_pixel_based_image = dst_res_image_texture.GetImage();
  u32 dst_address = dst_res_pixel_based_image.GetLocationAddress();
  if( !dst_address )
  {
    dst_address = dst_res_pixel_based_image.GetImageAddress();
  }

  s32 src_width  = src_res_image_texture.GetWidth();
  s32 src_height = src_res_image_texture.GetHeight();
  nw::gfx::res::ResPixelBasedTexture::FormatHW src_dst_format = static_cast<nw::gfx::res::ResPixelBasedTexture::FormatHW>(src_res_image_texture.GetFormatHW());

  GLenum format;
  switch( src_dst_format )
  {
  case nw::gfx::ResPixelBasedTexture::FORMAT_HW_RGBA8:  {  format = GL_RGBA8_OES;  }  break;
  case nw::gfx::ResPixelBasedTexture::FORMAT_HW_RGB8:   {  format = GL_RGB8_OES;   }  break;
  case nw::gfx::ResPixelBasedTexture::FORMAT_HW_RGB565: {  format = GL_RGB565;     }  break;
  case nw::gfx::ResPixelBasedTexture::FORMAT_HW_RGBA4:  {  format = GL_RGBA4;      }  break;
  default: {  GFL_ASSERT_MSG( 0, "テクスチャのフォーマットが異常です。" );  format = GL_RGBA4;  }  break;
  }

  // コマンド蓄積
  nngxFlush3DCommand();
  nngxFilterBlockImage( reinterpret_cast<const GLvoid*>(src_address), reinterpret_cast<GLvoid*>(dst_address), src_width, src_height, format );
      // CTR-SDK 関数リファレンスマニュアル
      // ブロックイメージのアンチエイリアスフィルタ転送コマンドをカレントのコマンドリストに蓄積します。
}




#if GFL_DEBUG
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// check_boardはデバッグ用途

static const char* GFL_GRP_G3D_BLOOM_FILTER_TEXTURE_CHECK_BOARD_FILE = "rom:/""g3d_common/texture_check_board_no_alpha.bcres";

void BloomFilter::CreateTextureCheckBoardModel(
    gfl::heap::NwAllocator*  heap_allocator,
    gfl::heap::NwAllocator*  device_allocator
)
{
  m_texture_check_board_resource = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource;
  
  void * buff = System::GetStaticResource( System::TEXTURE_CHECK_BOARD_NO_ALPHA_BCRES );
  
  if ( buff )
  {
    Resource::Description     desc;
    
    m_texture_check_board_resource->AttachBufferAndSetup(device_allocator, buff,
                                                  &desc, false, Resource::COMMON_RESOURCE_NONE);
  }
  else
  {     
    m_texture_check_board_resource->LoadAndSetup(device_allocator, GFL_GRP_G3D_BLOOM_FILTER_TEXTURE_CHECK_BOARD_FILE);
  }

  m_texture_check_board_model = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model;
  m_texture_check_board_model->Create(heap_allocator, device_allocator, m_texture_check_board_resource, 0);
}

void BloomFilter::DestroyTextureCheckBoardModel(void)
{
  if( m_texture_check_board_model )
  {
    GFL_DELETE m_texture_check_board_model;
    m_texture_check_board_model = NULL;
  }

  if( m_texture_check_board_resource )
  {
    GFL_DELETE m_texture_check_board_resource;
    m_texture_check_board_resource = NULL;
  }
}

#if 0
// @note m_frame_buffer_copy_textureを用意したので、これはかなり古い記述になります。コメントアウトを解除しただけでは動かないかもしれません。
// @note うまく表示されない原因を突き止めたい。
//うまく表示されないので、使用禁止。
void   BloomFilter::SetImageFrameBufferToTextureCheckBoard(const gfl::grp::DisplayType image_display_type)
{
  //nngxSplitDrawCmdlist();  // この後フレームバッファをテクスチャとして使用するので必須
  // は呼び出し元であるgfl::grp::g3d::Scene::SetImageFrameBufferToBloomTextureCheckBoardで実行している。

  nw::gfx::Material*    material     = m_texture_check_board_model->GetNwModel()->GetMaterial(0);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(0);
  //nw::gfx::ResTexture dummy_texture                      = res_texture_mapper.GetTexture().Dereference();
  //dummy_texture.Cleanup();
  // コマンド蓄積
  res_texture_mapper.SetTexture(m_frame_buffer_render_target_array[image_display_type]->GetNwResTexture());
      // NintendoWare for CTR 関数リファレンスマニュアル
      // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
  res_material.SetTextureMappersHash(0x0);

#if 0
  // uv
  const gfl::grp::Viewport* viewport = m_frame_buffer_render_target_array[image_display_type]->GetViewport();
  f32 vleft, vtop, vright, vbottom;
  viewport->GetBound(&vleft, &vtop, &vright, &vbottom);
  gfl::math::VEC2 scale(
      (vbottom-vtop)/m_frame_buffer_render_target_array[image_display_type]->GetHeight(),  // 縦横が逆になっている。
      (vright-vleft)/m_frame_buffer_render_target_array[image_display_type]->GetWidth()
  );
  gfl::math::VEC2 translate(
      0.0f,  // 縦横が逆になっている。
      -(1.0f-scale.y)/scale.y
  );
  {
    nw::gfx::ResMaterial  l_res_material = material->GetTextureCoordinatorResMaterial();
    nw::gfx::ResTextureCoordinator l_res_texture_coordinator = l_res_material.GetTextureCoordinators(0); 
    // コマンド蓄積
    l_res_texture_coordinator.SetScale(scale);
    l_res_texture_coordinator.SetTranslate(translate);
    l_res_material.SetTextureCoordinatorsHash(0x0);
  }
#endif
}
#endif

void   BloomFilter::SetImageOffScreenBufferToTextureCheckBoard(const gfl::grp::RenderTarget* image_off_screen_buffer)
{
  nw::gfx::Material*    material     = m_texture_check_board_model->GetNwModel()->GetMaterial(0);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(0);
  //nw::gfx::ResTexture dummy_texture                      = res_texture_mapper.GetTexture().Dereference();
  //dummy_texture.Cleanup();  // これをありにして、毎フレームこの関数を呼び出して同じimage_off_screen_bufferをセットしていると、2回目でエラーになる。
  // コマンド蓄積
  res_texture_mapper.SetTexture(image_off_screen_buffer->GetNwResTexture());
      // NintendoWare for CTR 関数リファレンスマニュアル
      // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
  res_material.SetTextureMappersHash(0x0);

#if 0
  //オフスクリーンなので、ビューポートのどの範囲が見えているとかないので、uvを変更する意味がない気がする。
  //もしuvを変更するなら、呼び出し元に指定してもらって変更するのがいいだろう。
  // uv
  const gfl::grp::Viewport* viewport = image_off_screen_buffer->GetViewport();
  f32 vleft, vtop, vright, vbottom;
  viewport->GetBound(&vleft, &vtop, &vright, &vbottom);
  gfl::math::VEC2 scale(
      (vright-vleft)/image_off_screen_buffer->GetWidth(),
      (vbottom-vtop)/image_off_screen_buffer->GetHeight()
  );
  {
    nw::gfx::ResMaterial  l_res_material = material->GetTextureCoordinatorResMaterial();
    nw::gfx::ResTextureCoordinator l_res_texture_coordinator = l_res_material.GetTextureCoordinators(0); 
    // コマンド蓄積
    l_res_texture_coordinator.SetScale(scale);
    l_res_material.SetTextureCoordinatorsHash(0x0);
  }
#endif
}

void   BloomFilter::SetImageOffScreenBufferToTextureCheckBoard(const gfl::grp::RenderTarget* image_off_screen_buffer, const gfl::math::VEC2& uv_scale, const gfl::math::VEC2& uv_translate)
{
  nw::gfx::Material*    material     = m_texture_check_board_model->GetNwModel()->GetMaterial(0);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(0);
  //nw::gfx::ResTexture dummy_texture                      = res_texture_mapper.GetTexture().Dereference();
  //dummy_texture.Cleanup();  // これをありにして、毎フレームこの関数を呼び出して同じimage_off_screen_bufferをセットしていると、2回目でエラーになる。
  // コマンド蓄積
  res_texture_mapper.SetTexture(image_off_screen_buffer->GetNwResTexture());
      // NintendoWare for CTR 関数リファレンスマニュアル
      // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。
  res_material.SetTextureMappersHash(0x0);

  // uv
  {
    nw::gfx::ResMaterial  l_res_material = material->GetTextureCoordinatorResMaterial();
    nw::gfx::ResTextureCoordinator l_res_texture_coordinator = l_res_material.GetTextureCoordinators(0); 
    // コマンド蓄積
    l_res_texture_coordinator.SetScale(uv_scale);
    l_res_texture_coordinator.SetTranslate(uv_translate);
    l_res_material.SetTextureCoordinatorsHash(0x0);
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif  // #if GFL_DEBUG

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

