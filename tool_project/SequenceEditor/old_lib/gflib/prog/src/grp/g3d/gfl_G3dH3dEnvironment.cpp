//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dEnvironment.cpp
 *	@brief  H3D環境
 *	@author	Koji Kawada
 *	@date		2012.04.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_fs.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d
#include <grp/g3d/gfl_G3dH3dEnvironment.h>



namespace gfl {
namespace grp {
namespace g3d {
  
//-----------------------------------------------------------------------------
/**
 *					構造体実装
*/
//-----------------------------------------------------------------------------
  
//----------------------------------------
// 頂点ライト @fix 渕野さん要望：バトル背景のライティング対応
//----------------------------------------
void H3dEnvironment::VertexLightSet::MakeCommand(
    nn::gr::CTR::CommandBufferJumpHelper& dcs,
    const nn::gr::BindSymbolVSInteger& vertexLightCnt,
    const nn::gr::BindSymbolVSFloat& univMtxBind,
    const nn::gr::BindSymbolVSFloat& hslGColBind,
    const nn::gr::BindSymbolVSFloat& hslSColBind,
    const nn::gr::BindSymbolVSFloat& hslSDirBind) const
{
  if ( vertexLightSize == 0 )
    return;
  
  if (this->hemisphereLight.isEnabled)
  {
    dcs.Put(hslGColBind.MakeUniformCommand(
    dcs.GetCurrentPtr(), nw::ut::FloatColor(this->hemisphereLight.groundColor)));
    dcs.Put(hslSColBind.MakeUniformCommand(
    dcs.GetCurrentPtr(), nw::ut::FloatColor(this->hemisphereLight.skyColor)));
    nw::math::VEC4 hslDirAndLeapFactor(this->hemisphereLight.direction);
    hslDirAndLeapFactor[3] = this->hemisphereLight.lerpFactor;
    dcs.Put(hslSDirBind.MakeUniformCommand(dcs.GetCurrentPtr(), hslDirAndLeapFactor));
  }

  dcs.Put(vertexLightCnt.MakeUniformCommand(dcs.GetCurrentPtr(), this->vertexLightSize - 1, 0, 1));

  nn::gr::BindSymbolVSFloat vlBind = univMtxBind;
  vlBind.start += nw::h3d::Constants::UNIV_REGISTER_QUANTITY - 1;
  vlBind.end += nw::h3d::Constants::UNIV_REGISTER_QUANTITY - 1;
  for (int idx = 0; idx < this->vertexLightSize; ++idx)
  {
    //const nw::h3d::res::VertexLightContent& vlc = this->vertexLights[idx];
    nw::h3d::res::VertexLightContent vlc = this->vertexLights[idx];
    
    {// ライトスポットパラメータ(x=SpotExponent, y=SpotCutoffAngle)
      dcs.Put(vlBind.MakeUniformCommand(dcs.GetCurrentPtr(), nw::math::VEC4(vlc.SpotExponent, vlc.SpotCutoffAngle, 0.0f, 0.0f)));
      --vlBind.start; --vlBind.end;
    }

    {// ライト距離減衰パラメータ(x=K0, y=K1, z=K2, w=IsAttenuationDistanceEnabled)
      nn::math::VEC4 attenuation;
      attenuation[0] = vlc.distanceAttenuationConstant;
      attenuation[1] = vlc.distanceAttenuationLinear;
      attenuation[2] = vlc.distanceAttenuationQuadratic;
      attenuation[3] = (vlc.flags & nw::h3d::LightFlag::DISTANCE_ATTENUATION_ENABLED) ? 1.0f : 0.0f;
      dcs.Put(vlBind.MakeUniformCommand(dcs.GetCurrentPtr(), attenuation));
      --vlBind.start; --vlBind.end;
    }

    {// スポットライト方向(w=IsSpotLightEnabled)
      nn::math::VEC4 direction(vlc.transform.translate);
      direction[3] = (vlc.lightType == nw::h3d::LightType::VERTEX_POINT) ? 1.0f : 0.0f;
      dcs.Put(vlBind.MakeUniformCommand(dcs.GetCurrentPtr(), direction));
      --vlBind.start; --vlBind.end;
    }

    {// ライト位置(w=IsDirectionalLight)        
      nn::math::VEC4 position;

      if (vlc.lightType == nw::h3d::LightType::VERTEX_DIRECTIONAL)
        position.Set( vlc.direction.x, vlc.direction.y, vlc.direction.z, 0.0f );
      else
        position.Set( vlc.transform.translate.x, vlc.transform.translate.y, vlc.transform.translate.z, 1.0f );

      dcs.Put(vlBind.MakeUniformCommand(dcs.GetCurrentPtr(), position));
      --vlBind.start; --vlBind.end;
    }
    
    {// ライトディヒューズカラー
      dcs.Put(vlBind.MakeUniformCommand(dcs.GetCurrentPtr(), vlc.diffuse));
      --vlBind.start; --vlBind.end;
    }
    
    {// ライトアンビエントカラー
      dcs.Put(vlBind.MakeUniformCommand(dcs.GetCurrentPtr(), vlc.ambient));
      --vlBind.start; --vlBind.end;
    }
  }
}  

//-----------------------------------------------------------------------------
/**
 *					クラス実装
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3D環境クラス
//=====================================

H3dEnvironment::H3dEnvironment(void)
  : m_buff(NULL),
    m_scene_state(),
    m_pVertexLightSetBuffer( NULL ),
    m_lut_data_enable(NULL),
    m_lut_data_array(NULL),
    m_lut_data_cache_array(NULL),
    m_fog_command_cache_array(NULL),
    m_render_mode_bit_flag(RENDER_MODE_BIT_FLAG_NONE)
{
  // 値が変わっているといけないので、チェックする
  STATIC_ASSERT(nw::gfx::res::ResLightingLookupTable::INPUT_NH==PICA_DATA_FRAG_LIGHT_ENV_NH_DMP);
  STATIC_ASSERT(nw::gfx::res::ResLightingLookupTable::INPUT_VH==PICA_DATA_FRAG_LIGHT_ENV_VH_DMP);
  STATIC_ASSERT(nw::gfx::res::ResLightingLookupTable::INPUT_NV==PICA_DATA_FRAG_LIGHT_ENV_NV_DMP);
  STATIC_ASSERT(nw::gfx::res::ResLightingLookupTable::INPUT_LN==PICA_DATA_FRAG_LIGHT_ENV_LN_DMP);
  STATIC_ASSERT(nw::gfx::res::ResLightingLookupTable::INPUT_SP==PICA_DATA_FRAG_LIGHT_ENV_SP_DMP);
  STATIC_ASSERT(nw::gfx::res::ResLightingLookupTable::INPUT_CP==PICA_DATA_FRAG_LIGHT_ENV_CP_DMP);
  
  STATIC_ASSERT(nw::gfx::res::ResLightingLookupTable::SCALE_1   ==PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_1_0 );
  STATIC_ASSERT(nw::gfx::res::ResLightingLookupTable::SCALE_2   ==PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_2_0 );
  STATIC_ASSERT(nw::gfx::res::ResLightingLookupTable::SCALE_4   ==PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_4_0 );
  STATIC_ASSERT(nw::gfx::res::ResLightingLookupTable::SCALE_8   ==PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_8_0 );
  STATIC_ASSERT(nw::gfx::res::ResLightingLookupTable::SCALE_0_25==PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_0_25);
  STATIC_ASSERT(nw::gfx::res::ResLightingLookupTable::SCALE_0_5 ==PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_0_5 );
}
H3dEnvironment::~H3dEnvironment()
{
  Destroy();
}

void H3dEnvironment::Create(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description& a_desc)
{
  nw::h3d::SceneState::Description l_desc;
  {
    l_desc.maxTextureProjectorSize = a_desc.texture_projector_num_max +1;  // @note +1は下記理由から。これがnw4cのバグでないならこのままで。
        // nw::h3d::SceneStateのGetTextureProjector(int idx)メンバ関数が、{ return m_TexProjectors[idx + 1]; }というふうに+1したものを返してくるので。
    l_desc.maxLightSetSize         = a_desc.light_set_num_max;
    l_desc.maxFogSize              = a_desc.fog_num_max;
  }

  u32 sceneStateSize = nw::h3d::SceneState::CalcSize(l_desc);
  m_buff = GflHeapAllocMemoryAlign(heap_allocator->GetHeapBase(), sceneStateSize, 4);
  m_scene_state.Init(m_buff, l_desc);

  {//@fix 渕野さん要望：バトル背景のライティング対応
    m_pVertexLightSetBuffer = NULL;
    m_scene_state.SetUserPtr( NULL );
    if ( l_desc.maxLightSetSize )
    {
      m_pVertexLightSetBuffer = GFL_NEW_ARRAY(device_allocator->GetHeapBase()) VertexLightSet[l_desc.maxLightSetSize];
      m_VertexLightSets.SetArray( m_pVertexLightSetBuffer, l_desc.maxLightSetSize);
      m_scene_state.SetUserPtr(&m_VertexLightSets);
    }
  }

  // ライト
  {
    m_max_light_sets = a_desc.light_set_num_max;
    if( m_max_light_sets > 0 )
    {
      m_lut_data_enable = GFL_NEW(heap_allocator->GetHeapBase()) gfl::base::Bit(
          heap_allocator->GetHeapBase(), m_max_light_sets*nw::h3d::Constants::LIGHT_QUANTITY);
      m_lut_data_array = GFL_NEW_ARRAY(device_allocator->GetHeapBase()) nw::h3d::res::LutData*[m_max_light_sets];
      for(u32 i=0; i<m_max_light_sets; ++i)
      {
        m_lut_data_array[i] = GFL_NEW_ARRAY(device_allocator->GetHeapBase()) nw::h3d::res::LutData[nw::h3d::Constants::LIGHT_QUANTITY];
      }

      m_lut_data_cache_array = GFL_NEW_ARRAY(device_allocator->GetHeapBase()) LutDataCache**[m_max_light_sets];
      for(u32 i=0; i<m_max_light_sets; ++i)
      {
        m_lut_data_cache_array[i] = GFL_NEW_ARRAY(device_allocator->GetHeapBase()) LutDataCache*[nw::h3d::Constants::LIGHT_QUANTITY];
        for(u32 j=0; j<nw::h3d::Constants::LIGHT_QUANTITY; ++j)
        {
          m_lut_data_cache_array[i][j] = static_cast<LutDataCache*>( GflHeapAllocMemoryAlign(device_allocator->GetHeapBase(), sizeof(LutDataCache), nw::h3d::Alignment::COMMAND_SECTION) );
        }
      }
    }
  }

  // フォグ
  {
    if( a_desc.fog_num_max > 0 )
    {
      m_fog_command_cache_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) const u32*[a_desc.fog_num_max];
      for(u32 i=0; i<a_desc.fog_num_max; ++i)
      {
        m_fog_command_cache_array[i] = NULL;
      }
    }
  }
}

void H3dEnvironment::CreateFromScene(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Scene* scene)
{
  s32 max_cameras       = 0;
  s32 max_light_sets    = 0;
  s32 max_fogs          = 0;
  s32 max_vertex_lights = 0;
  const nw::gfx::SceneEnvironment& env = scene->GetSceneEnvironment(&max_cameras, &max_light_sets, &max_fogs, &max_vertex_lights);

  CreateFromSceneEnvironment(heap_allocator, device_allocator, env, max_cameras, max_light_sets, max_fogs, max_vertex_lights);
}

//使用禁止void H3dEnvironment::CreateAndSetFromScene(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Scene* scene)
//{
//  CreateFromScene(heap_allocator, scene);
//  SetFromScene(scene);
//}

void H3dEnvironment::CreateFromSceneEnvironment(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const nw::gfx::SceneEnvironment& env, s32 max_cameras, s32 max_light_sets, s32 max_fogs, s32 max_vertex_lights)
{
  // SceneのRenderSystemのnw::gfx::RenderContextのnw::gfx::SceneEnvironmentから設定する
  
  Description desc;
  desc.texture_projector_num_max = max_cameras;  // @note カメラ全部の個数を渡すと取り過ぎかな？
  desc.light_set_num_max         = max_light_sets;
  desc.fog_num_max               = max_fogs;

  Create(heap_allocator, device_allocator, desc);
}

void H3dEnvironment::Destroy(void)
{
  if( m_buff )
  {
    GflHeapFreeMemory(m_buff);
    m_buff = NULL;
  }

  // m_scene_stateは放置

  {// ライト
    //@fix 渕野さん要望：バトル背景のライティング対応
    m_VertexLightSets.ReleaseMemory();
    GFL_SAFE_DELETE_ARRAY( m_pVertexLightSetBuffer );

    if( m_lut_data_enable )
    {
      GFL_DELETE m_lut_data_enable;
      m_lut_data_enable = NULL;
    }
    if( m_lut_data_array )
    {
      for(u32 i=0; i<m_max_light_sets; ++i)
      {
        if( m_lut_data_array[i] ) GFL_DELETE_ARRAY m_lut_data_array[i];
      }
      GFL_DELETE_ARRAY m_lut_data_array;
      m_lut_data_array = NULL;
    }
    if( m_lut_data_cache_array )
    {
      for(u32 i=0; i<m_max_light_sets; ++i)
      {
        if( m_lut_data_cache_array[i] )
        {
          for(u32 j=0; j<nw::h3d::Constants::LIGHT_QUANTITY; ++j)
          {
            if( m_lut_data_cache_array[i][j] )
            {
              GflHeapFreeMemory(m_lut_data_cache_array[i][j]);
              m_lut_data_cache_array[i][j] = NULL;
            }
          }
          GFL_DELETE_ARRAY m_lut_data_cache_array[i];
          m_lut_data_cache_array[i] = NULL;
        }
      }
      GFL_DELETE_ARRAY m_lut_data_cache_array;
      m_lut_data_cache_array = NULL;
    }
  }

  // フォグ
  if( m_fog_command_cache_array )
  {
    GFL_DELETE_ARRAY m_fog_command_cache_array;
    m_fog_command_cache_array = NULL;
  }

  m_render_mode_bit_flag = RENDER_MODE_BIT_FLAG_NONE;
}

// @attention この関数は毎フレーム呼ばないとGPUハングアップするかもしれません。
void H3dEnvironment::SetFromScene(const Scene* scene, const nw::gfx::Camera& camera)
{
  // SceneのRenderSystemのnw::gfx::RenderContextのnw::gfx::SceneEnvironmentから設定する
  s32 max_cameras       = 0;
  s32 max_light_sets    = 0;
  s32 max_fogs          = 0;
  s32 max_vertex_lights = 0;
  const nw::gfx::SceneEnvironment& env = scene->GetSceneEnvironment(&max_cameras, &max_light_sets, &max_fogs, &max_vertex_lights);

  SetFromSceneEnvironment(env, camera, max_cameras, max_light_sets, max_fogs, max_vertex_lights);
}

void H3dEnvironment::SetRenderModeBitFlag(u32 bit_flag)
{
  m_render_mode_bit_flag = bit_flag;
}

bit32 H3dEnvironment::GetRenderModeBitFlag(void) const
{
  return m_render_mode_bit_flag;
}

const nn::gr::FragmentLight& H3dEnvironment::GetFragmentLight( s32 lightSetNo ) const
{
  return m_scene_state.GetFragmentLight( lightSetNo );
}

const nw::h3d::fnd::ColorSet& H3dEnvironment::GetLightColor( s32 lightSetNo, int lightIdx ) const
{
  return m_scene_state.GetLightColor( lightSetNo, lightIdx );
}

// @attention この関数は毎フレーム呼ばないとGPUハングアップするかもしれません。
void H3dEnvironment::SetFromSceneEnvironment(const nw::gfx::SceneEnvironment& env, const nw::gfx::Camera& view_camera, s32 max_cameras, s32 max_light_sets, s32 max_fogs, s32 max_vertex_lights)
{
  // 個数はCreate時のDescriptionで確保しておいて下さい。
  
  // ビューマトリクス 
  const gfl::math::MTX34& view_mtx = view_camera.ViewMatrix();


  //////// カメラ //////// 
                                 // @note ライトはgrライブラリを使っているので毎フレーム設定する必要があるかもしれないが、
                                 // カメラは毎フレームでなくてもいいかもしれない。
  for(s32 i=0; i<max_cameras; ++i)
  {
    const nw::gfx::Camera* camera = env.GetCamera(i);
    if( camera )
    {
#if 0
/*      NintendoWare/CTR/sources/libraries/h3d/h3d_ResourceImpl.h
          void
          CameraContent::ApplyTextureProjectorTo(
              SceneState* sceneState,
              u32 texProjIdx) const*/
#endif
      nw::h3d::fnd::TextureProjector& texProj = m_scene_state.GetTextureProjector(i);

      switch (camera->GetViewUpdater()->Kind())
      {
      case nw::anim::res::ResCameraAnim::VIEW_UPDATER_AIM:
          {
              texProj.viewMatrix.SetupLookAt(camera->GetPosition(), camera->GetTwist(), camera->GetTargetPosition());
          }
          break;
      case nw::anim::res::ResCameraAnim::VIEW_UPDATER_LOOKAT:
          {
              texProj.viewMatrix.SetupLookAt(camera->GetPosition(), camera->GetUpwardVector(), camera->GetTargetPosition());
          }
          break;
      case nw::anim::res::ResCameraAnim::VIEW_UPDATER_ROTATE:
          {
              nn::math::VEC3 rotateDeg(
                  NN_MATH_RAD_TO_DEG(camera->GetViewRotate().x),
                  NN_MATH_RAD_TO_DEG(camera->GetViewRotate().y),
                  NN_MATH_RAD_TO_DEG(camera->GetViewRotate().z));
              texProj.viewMatrix.SetupCameraRotate(camera->GetPosition(), rotateDeg);
          }
          break;
      default:
          GFL_ASSERT_STOP_MSG(0, "NW_FATAL_ERROR()\n");
      }
      switch (camera->GetProjectionUpdater()->Kind())
      {
      case nw::anim::res::ResCameraAnim::PROJECTION_UPDATER_PERSPECTIVE:
          {
              const nw::gfx::ResPerspectiveProjectionUpdater perspective =
                  nw::gfx::ResStaticCast<const nw::gfx::ResPerspectiveProjectionUpdater>(camera->GetProjectionUpdater()->GetResource());
              texProj.near = camera->GetNear();
              texProj.far = camera->GetFar();
              texProj.SetupPerspective(
                  perspective.GetFovy(),
                  perspective.GetAspectRatio(),
                  nn::math::VEC2(0.5f, 0.5f),   // @note マテリアルに設定してあるuvのSRTとも違うみたいだし、これ何の値？
                  nn::math::VEC2(0.5f, 0.5f));  //       uvのSRTは反映されていたので、これはサンプルのままでいいだろうか？
          }
          break;
      case nw::anim::res::ResCameraAnim::PROJECTION_UPDATER_ORTHO:
          {
              const nw::gfx::ResOrthoProjectionUpdater ortho =
                  nw::gfx::ResStaticCast<const nw::gfx::ResOrthoProjectionUpdater>(camera->GetProjectionUpdater()->GetResource());
              texProj.near = camera->GetNear();
              texProj.far = camera->GetFar();
              texProj.SetupOrtho(
                  -ortho.GetRect().GetHeight() * ortho.GetRect().GetAspectRatio() / 2,
                  ortho.GetRect().GetHeight() * ortho.GetRect().GetAspectRatio() / 2,
                  -ortho.GetRect().GetHeight() / 2,
                  ortho.GetRect().GetHeight() / 2,
                  nn::math::VEC2(0.5f, 0.5f),   // @note マテリアルに設定してあるuvのSRTとも違うみたいだし、これ何の値？
                  nn::math::VEC2(0.5f, 0.5f));  //       uvのSRTは反映されていたので、これはサンプルのままでいいだろうか？
          }
          break;
      case nw::anim::res::ResCameraAnim::PROJECTION_UPDATER_FRUSTUM:
          {
              const nw::gfx::ResFrustumProjectionUpdater frustum =
                  nw::gfx::ResStaticCast<const nw::gfx::ResFrustumProjectionUpdater>(camera->GetProjectionUpdater()->GetResource());
              texProj.near = camera->GetNear();
              texProj.far = camera->GetFar();
              texProj.SetupFrustum(
                  frustum.GetRect().GetCenter().x - frustum.GetRect().GetWidth()/2,
                  frustum.GetRect().GetCenter().x + frustum.GetRect().GetWidth()/2,
                  frustum.GetRect().GetCenter().y - frustum.GetRect().GetHeight()/2,
                  frustum.GetRect().GetCenter().y + frustum.GetRect().GetHeight()/2,
                  camera->GetNear(),
                  nn::math::VEC2(0.5f, 0.5f),   // @note マテリアルに設定してあるuvのSRTとも違うみたいだし、これ何の値？
                  nn::math::VEC2(0.5f, 0.5f));  //       uvのSRTは反映されていたので、これはサンプルのままでいいだろうか？
          }
          break;
      default:
          GFL_ASSERT_STOP_MSG(0, "NW_FATAL_ERROR()\n");
      }
 
      texProj.flags |= nw::h3d::TextureProjectorFlag::DIRTY;
    }
  }

  // @note フラグメントライトしか反映していないけど、頂点ライトとか使っている？ 抜けがないようにASSERTくらいはいれておきたいが。
  //////// ライト ////////
  for(s32 lightSetId=0; lightSetId<max_light_sets; ++lightSetId)
  {
    const nw::gfx::LightSet* light_set = env.GetLightSet(lightSetId);
    if( light_set )
    {
      // アンビエントライト
      const nw::gfx::AmbientLight* global_ambient = light_set->GetAmbientLight();
      if( global_ambient )
      {
        m_scene_state.SetGlobalAmbient(lightSetId, nw::ut::Color8(global_ambient->GetResAmbientLight().GetAmbient()));
      }
      else
      {
        m_scene_state.SetGlobalAmbient(lightSetId, nw::ut::Color8(0,0,0,0));
      }
    
      {// フラグメントライト
        s32 lightIdx=0;
        nw::gfx::FixedFragmentLightArray::const_iterator end = light_set->GetFragmentLightEnd();
        for( nw::gfx::FixedFragmentLightArray::const_iterator ite = light_set->GetFragmentLightBegin(); ite != end; ++ite )
        {
          //m_scene_state.GetFragmentLight(i).globalAmbientR = 0;  // m_scene_state.SetGlobalAmbientとm_scene_state.GetFragmentLight(i).globalAmbientRは
          //m_scene_state.GetFragmentLight(i).globalAmbientG = 0;  // 別物か？ 別物の場合はCreativeStudioではどこで設定するものか？
          //m_scene_state.GetFragmentLight(i).globalAmbientB = 0;  // →[回答] m_scene_state.SetGlobalAmbientと他の値から
                                                                   //          m_scene_state.GetFragmentLight(i).globalAmbientRを設定しているので、
                                                                   //          別物ではなく一部のようなので設定しなくてよさそう。

          // フラグメントライト
          if( (*ite) )
          {
            m_scene_state.SetDiffuse(lightSetId, lightIdx, nw::ut::Color8((*ite)->GetResFragmentLight().GetDiffuse()));
            m_scene_state.SetAmbient(lightSetId, lightIdx, nw::ut::Color8((*ite)->GetResFragmentLight().GetAmbient()));
            m_scene_state.SetSpecular0(lightSetId, lightIdx, nw::ut::Color8((*ite)->GetResFragmentLight().GetSpecular0()));
            m_scene_state.SetSpecular1(lightSetId, lightIdx, nw::ut::Color8((*ite)->GetResFragmentLight().GetSpecular1()));

            m_scene_state.GetFragmentLight(lightSetId).isEnable[lightIdx] = (*ite)->GetResFragmentLight().IsLightEnabled();
                // このm_scene_state.GetFragmentLight(i).isEnable[j]への設定を
                // 「毎フレームやらない」で「描画前に1回だけ設定」とかにしているとGPUハングアップするかもしれない。
                // gfxと併用しているときはgrライブラリに対する設定【m_scene_state.GetFragmentLight(i)といったgrを得ているもの】は
                // 毎フレームしないといけないのかもしれない。

            /*
            {
              // SetPositionに設定するのは、ライトの位置ではなくライトの方向なので、これは間違いだからコメントアウト。
              gfl::math::VEC3 pos;
              nw::gfx::TransformNode* transform_node = reinterpret_cast<nw::gfx::TransformNode*>(*ite);
              //GFL_PRINT("pos(%.3f, %.3f, %.3f)\n", pos.x, pos.y, pos.z);
              transform_node->Transform().GetTranslate( &pos );
              m_scene_state.GetFragmentLight(i).source[j].SetPosition(pos, true);
            }
            */
        #if 0
            {
              // h3d_State.h
              // ライト方向の設定は GetFragmentLight() から取得した nn::gr::FragmentLight クラスに設定
              const gfl::math::VEC3& dir = (*ite)->GetResFragmentLight().GetDirection();
              //GFL_PRINT("dir(%.3f, %.3f, %.3f)\n", dir.x, dir.y, dir.z);         // プリント出力するとGPUハングアップする？
              m_scene_state.GetFragmentLight(lightSetId).source[lightIdx].SetPosition(-dir, true);  // 向きが逆になるみたい
            }
        #else
            {
              // NintendoWare/CTR/sources/libraries/h3d/h3d_ResourceImpl.h
              // void
              // FragmentLightContent::ApplyTo(
              //     SceneState* sceneState,
              //     u32 lightSetIdx,
              //     u32 lightIdx,
              //     const nn::math::MTX34& view) const

              // 初期化
              {
                // スポットライトなし
                m_scene_state.GetFragmentLight(lightSetId).isEnableSpot[lightIdx] = false;
                m_scene_state.SetAngleSampler(lightSetId, lightIdx, NULL);

                // ディレクショナルライトなし
                gfl::math::VEC3 pos;
                nn::math::VEC3Transform(&pos, view_mtx, (*ite)->WorldTransform().GetTranslate());
                m_scene_state.GetFragmentLight(lightSetId).source[lightIdx].SetPosition(pos, false);

                // DISTANCE_ATTENUATION_ENABLEDなし
                m_scene_state.GetFragmentLight(lightSetId).isEnableDistAttn[lightIdx] = false;
                m_scene_state.SetDistanceSampler(lightSetId, lightIdx, NULL);

                // TWO_SIDE_DIFFUSE_ENABLEDなし
                m_scene_state.GetFragmentLight(lightSetId).source[lightIdx].isEnableTwoSideDiffuse = false;
              }

              // 個別
              {
                nw::gfx::res::ResLight::Kind kind = static_cast<nw::gfx::res::ResLight::Kind>((*ite)->GetResFragmentLight().GetLightKind());
                switch(kind)
                {
                  // ディレクショナルライト
                case nw::gfx::res::ResLight::KIND_DIRECTIONAL:
                  {
                    gfl::math::VEC3 dir;
                    nn::math::VEC3TransformNormal(&dir, view_mtx, -(*ite)->Direction());
                    m_scene_state.GetFragmentLight(lightSetId).source[lightIdx].SetPosition(dir, true);  // 向きが逆になるみたい
                    break;
                  }
                  // ポイントライト
                case nw::gfx::res::ResLight::KIND_POINT:
                  {
                    // ディレクショナルライトなしのときの設定がそのまま使われることになる
                    break;
                  }
                  // スポットライト
                case nw::gfx::res::ResLight::KIND_SPOT:
                  {
                    m_scene_state.GetFragmentLight(lightSetId).isEnableSpot[lightIdx] = true;

                    gfl::math::VEC3 dir;
                    nn::math::VEC3TransformNormal(&dir, view_mtx, (*ite)->Direction());
                    m_scene_state.GetFragmentLight(lightSetId).source[lightIdx].SetSpotDirection(dir);

                    // 角度減衰用の LUT を設定します。
                    nw::gfx::res::ResLightingLookupTable lighting_lut = (*ite)->GetResFragmentLight().GetAngleSampler();
                    if( lighting_lut.IsValid() )
                    {
                      nw::gfx::res::ResLookupTable lut = lighting_lut.GetSampler();

                      if( setLutData(lut, lightSetId, lightIdx) )
                      {
                        //m_scene_state.SetAngleSampler(i, j, &(m_lut_data_array[i][j]));
                        m_scene_state.SetAngleSampler(lightSetId, lightIdx, &(m_lut_data_cache_array[lightSetId][lightIdx]->lutData));
                      }
                      else
                      {
                        m_scene_state.SetAngleSampler(lightSetId, lightIdx, NULL);
                      }

                      m_scene_state.GetFragmentLight(lightSetId).lutConfigSP.input = static_cast<PicaDataFragLightEnvLutInput>(lighting_lut.GetInput());  // コンストラクタのSTATIC_ASSERTで変換できることを確認してある
                      m_scene_state.GetFragmentLight(lightSetId).lutConfigSP.scale = static_cast<PicaDataFragLightEnvLutScale>(lighting_lut.GetScale());  // コンストラクタのSTATIC_ASSERTで変換できることを確認してある
                      m_scene_state.GetFragmentLight(lightSetId).lutConfigSP.isAbs = lighting_lut.IsAbs();
                    }
                    break;
                  }
                  // その他
                default:
                  {
                    GFL_ASSERT(0);
                    break;
                  }
                }

                // DISTANCE_ATTENUATION_ENABLEDのとき
                if( (*ite)->GetResFragmentLight().GetFlags() & nw::gfx::res::ResFragmentLight::FLAG_DISTANCE_ATTENUATION_ENABLED )
                {
                  // 距離減衰用の LUT を設定します。
                  m_scene_state.GetFragmentLight(lightSetId).isEnableDistAttn[lightIdx] = true;
                  m_scene_state.GetFragmentLight(lightSetId).source[lightIdx].SetDistAttnStartEnd(
                      (*ite)->GetResFragmentLight().GetDistanceAttenuationStart(), (*ite)->GetResFragmentLight().GetDistanceAttenuationEnd() );
                  nw::gfx::res::ResLookupTable lut = (*ite)->GetResFragmentLight().GetDistanceSampler();

                  if( setLutData(lut, lightSetId, lightIdx) )
                  {
                    //m_scene_state.SetDistanceSampler(i, j, &(m_lut_data_array[i][j]));
                    m_scene_state.SetDistanceSampler(lightSetId, lightIdx, &(m_lut_data_cache_array[lightSetId][lightIdx]->lutData));
                  }
                }

                // TWO_SIDE_DIFFUSE_ENABLEDのとき
                if( (*ite)->GetResFragmentLight().GetFlags() & nw::gfx::res::ResFragmentLight::FLAG_TWO_SIDE_DIFFUSE_ENABLED )
                {
                  m_scene_state.GetFragmentLight(lightSetId).source[lightIdx].isEnableTwoSideDiffuse = true;
                }
              }
            }
        #endif
          }
          else
          {
            m_scene_state.GetFragmentLight(lightSetId).isEnable[lightIdx] = false;
          }

          ++lightIdx;
        }

        for(s32 k=lightIdx; k<nw::h3d::Constants::LIGHT_QUANTITY; ++k)
        {
          // 設定のないライトの無効化
          m_scene_state.GetFragmentLight(lightSetId).isEnable[k] = false;
        }
      }
      
      // ここでは、SceneState クラスのユーザーポインタを利用して頂点ライトのパラメータを受け取っています。
      VertexLightSetArray* vertexLightSets = reinterpret_cast<VertexLightSetArray*>(m_scene_state.GetUserPtr());
      if (vertexLightSets)
      {// 頂点ライト
        int vertexLightCnt = 0;
        for (nw::gfx::VertexLightArray::const_iterator light = light_set->GetVertexLightBegin(); light != light_set->GetVertexLightEnd(); ++light, ++vertexLightCnt)
        {
          if( vertexLightCnt >= VertexLightSet::MAX_VERTEX_LIGHT_QUANTITY)
          {
            GFL_ASSERT_MSG(0, "頂点ライトが多すぎます。\n");
            break;
          }
          
          nw::h3d::res::VertexLightContent& resVertexLight = (*vertexLightSets)[lightSetId].vertexLights[vertexLightCnt];

          resVertexLight.ambient = (*light)->GetResVertexLight().GetAmbient();
          resVertexLight.diffuse = (*light)->GetResVertexLight().GetDiffuse();
          nn::math::VEC3TransformNormal( &resVertexLight.direction, view_mtx, -(*light)->Direction() );
          resVertexLight.transform.translate = (*light)->WorldTransform().GetTranslate();

          if ((*light)->GetResVertexLight().IsDistanceAttenuationEnabled())
          {
            const nw::math::VEC3& attenuation = (*light)->GetResVertexLight().GetDistanceAttenuation();
            resVertexLight.distanceAttenuationConstant = attenuation.x;
            resVertexLight.distanceAttenuationLinear = attenuation.y;
            resVertexLight.distanceAttenuationQuadratic = attenuation.z;
          }

          switch( (*light)->GetResVertexLight().GetLightKind() ){
          case nw::gfx::res::ResVertexLight::KIND_DIRECTIONAL:
            resVertexLight.lightType = nw::h3d::LightType::VERTEX_DIRECTIONAL;
            break;
          case nw::gfx::res::ResVertexLight::KIND_POINT:
            resVertexLight.lightType = nw::h3d::LightType::VERTEX_POINT;
            break;
          case nw::gfx::res::ResVertexLight::KIND_SPOT:
            resVertexLight.lightType = nw::h3d::LightType::VERTEX_SPOT;
            break;
          default:
            break;
          }

          if ((*light)->GetResVertexLight().GetLightKind() != nw::gfx::res::ResVertexLight::KIND_SPOT)
          {
            const nw::math::VEC2& factor = (*light)->GetResVertexLight().GetSpotFactor();
            resVertexLight.SpotExponent = factor.x;
            resVertexLight.SpotCutoffAngle = factor.y;
          }
        }
        // 頂点ライトはライト数を設定するので無効化は必要ありません。
        (*vertexLightSets)[lightSetId].vertexLightSize = vertexLightCnt;
        
        //ISHIGURO_PRINT( "lightSetId = %d, vertexLightCnt = %d\n", lightSetId, vertexLightCnt );
        
        const nw::gfx::HemiSphereLight* hsLight = light_set->GetHemiSphereLight();
        if (hsLight)
        {// 半球ライト
          nw::h3d::res::HemiSphereLightContent& resHemiSphereLight =
          (*vertexLightSets)[lightSetId].hemisphereLight;

          resHemiSphereLight.direction = hsLight->GetResHemiSphereLight().GetDirection();
          resHemiSphereLight.groundColor = hsLight->GetResHemiSphereLight().GetGroundColor();
          resHemiSphereLight.skyColor = hsLight->GetResHemiSphereLight().GetSkyColor();
          resHemiSphereLight.lerpFactor = hsLight->GetResHemiSphereLight().GetLerpFactor();
        }
        else
        {// 設定のないライトの無効化
          (*vertexLightSets)[lightSetId].hemisphereLight.isEnabled = false;
        }
      }

      // nw::h3d::res::MaterialData::MakeCommand() を利用している場合にライトのカラーを変更する場合は、
      // nw::h3d::SceneState::FLAG_FRAGMENT_LIGHT_COLOR_MUTABLE を利用してください。
      // このフラグは nw::h3d::res::MaterialData::MakeCommandWithoutCache() を利用している場合、
      // 意味を持ちません。
      m_scene_state.EnableFlags(lightSetId, nw::h3d::SceneState::FLAG_FRAGMENT_LIGHT_COLOR_MUTABLE);
    }
  }
  
  //////// フォグ //////// 
  for(s32 i=0; i<max_fogs; ++i)
  {
    const nw::gfx::Fog* gfx_fog = env.GetFog(i);

    if( gfx_fog )
    {
      const nw::gfx::res::ResFog gfx_res_fog = gfx_fog->GetResFog();
      
      nn::gr::CTR::Fog& gr_fog = m_scene_state.GetFog(i);

      m_fog_command_cache_array[i] = gfx_res_fog.GetFogSampler().GetCommandCache();

      gfl::grp::ColorU8 color_u8 = ConvertColorF32ToColorU8(gfx_res_fog.GetColor());
      gr_fog.colorR = color_u8.r;
      gr_fog.colorG = color_u8.g;
      gr_fog.colorB = color_u8.b;
      
      gr_fog.isEnable = true;
      gr_fog.isEnableZFlip = gfx_res_fog.IsZFlip();
     
      /*
      if (fog->GetResFog().IsAttenuateDistance())
      {
          resFog.flags |= nw::h3d::FogFlag::ATTENUATE_DISTANCE;
      }

      resFog.fogType = static_cast<nw::h3d::FogType::enum_t>(fogUpdater.GetFogUpdaterType() - 1);
      resFog.minDepth = fogUpdater.GetMinFogDepth();
      resFog.maxDepth = fogUpdater.GetMaxFogDepth();
      resFog.density = fogUpdater.GetDensity();

          cameraNear,
          cameraFar,
          m_SceneState->GetWScale(),
          invProj);

      これらを使っていないが、使った後の参照テーブルを直接使っているので大丈夫のはず。
      */
    }
  }
}

/*
xy_ctrlib/CTR_SDK/include/nn/gr/CTR/gr_Fog.h
*/
bit32* H3dEnvironment::MakeFogAllCommand(s32 index, bit32* command) const
{
  const nn::gr::CTR::Fog& gr_fog = m_scene_state.GetFog(index);
  
  // 設定用コマンド
  command = MakeFogConfigCommand(index, command);

  // フォグの参照テーブルコマンド
  if(gr_fog.isEnable)
  {
    command = MakeFogTableCommand(index, command);
  }
  return command;
}

bit32* H3dEnvironment::MakeFogConfigCommand(s32 index, bit32* command) const
{
  const nn::gr::CTR::Fog& gr_fog = m_scene_state.GetFog(index);
  return gr_fog.MakeConfigCommand(command);
}

bit32* H3dEnvironment::MakeFogTableCommand(s32 index, bit32* command) const
{
  ::std::memcpy( command, m_fog_command_cache_array[index], sizeof(u32) * ( nn::gr::CTR::Fog::FOG_LOOKUP_TABLE_ELEMENT_NUM + 4 ) );
  return command + ( nn::gr::CTR::Fog::FOG_LOOKUP_TABLE_ELEMENT_NUM + 4 );
}

bit32* H3dEnvironment::MakeFogDisableCommand(s32 index, bit32* command) const
{
  return nn::gr::CTR::Fog::MakeDisableCommand(command);
}

H3dEnvironment::VertexLightSet* H3dEnvironment::GetVertexLightSet( s32 index ) const
{
  if ( m_pVertexLightSetBuffer)
    return &m_pVertexLightSetBuffer[index];
  else
    return NULL;
}

s32 H3dEnvironment::getLutDataEnableIndex(s32 light_set_index, s32 light_index)
{
  return ( nw::h3d::Constants::LIGHT_QUANTITY*light_set_index + light_index );
}

b32 H3dEnvironment::setLutData(nw::gfx::res::ResLookupTable lut, s32 light_set_index, s32 light_index)
{
  b32 lut_data_enable = false;
  
  if( lut.IsValid() )
  {
    nw::gfx::res::ResImageLookupTable image_lut = lut.Dereference();
    s32 command_count = image_lut.GetCommandCacheCount();
    //GFL_PRINT("cache count %d aaaaaaaaaaaaaaaaaa\n", command_count);
    bit32* command_array = image_lut.GetCommandCache();
    if( command_count > 0 )
    {
      m_lut_data_enable->On(getLutDataEnableIndex(light_set_index, light_index));

      m_lut_data_array[light_set_index][light_index].flags = 0;
      m_lut_data_array[light_set_index][light_index].flags |= (image_lut.IsAbs())?(nw::h3d::LutFlag::IS_ABS):(0);
      m_lut_data_array[light_set_index][light_index].cmd = nw::h3d::fnd::Command(command_array, command_count);  // 参照テーブルのコマンド部分はgfxとh3dで共有する
      //GFL_PRINT("command size=%d ByteSize=%d aaaaaaaaaaaaaaaaaaaa\n", m_lut_data_array[light_set_index][light_index].cmd.size(), m_lut_data_array[light_set_index][light_index].cmd.GetByteSize());
      m_lut_data_array[light_set_index][light_index].name = NULL;  // 名無し
    
      lut_data_enable = true;




      //aaaaaaaaaaaaaaaaaaaaa
      // アライメントをそろえるためにバッファにコピーします。
      NW_ASSERT(command_count <= LUTDATA_BUFFER_SIZE);
      nw::h3d::fnd::SubroutineCmdMaker scm(reinterpret_cast<bit32*>(m_lut_data_cache_array[light_set_index][light_index]->lutDataBuffer));
      scm.CopyFrom(command_array, command_count / 4);
      scm.Make(&(m_lut_data_cache_array[light_set_index][light_index]->lutData.cmd));
      NW_ASSERT(m_lut_data_cache_array[light_set_index][light_index]->lutData.cmd.GetByteSize() <= (LUTDATA_BUFFER_SIZE + SUBROUTINE_JUMP_COMMAND_SIZE));
      m_lut_data_cache_array[light_set_index][light_index]->lutData.flags = (image_lut.IsAbs())?(nw::h3d::LutFlag::IS_ABS):(0);
      m_lut_data_cache_array[light_set_index][light_index]->lutData.name = NULL;  // 名無し
      // 参照テーブルのコマンド部分はgfxとh3dで共有したかったのに、別バッファになってしまっている。




    }
  }

  if( !lut_data_enable )
  {
    m_lut_data_enable->Off(getLutDataEnableIndex(light_set_index, light_index));
  }

  return lut_data_enable;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

