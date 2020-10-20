//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dLightSet.cpp
 *	@brief  3Dライトセット
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dLight.h>
#include <grp/g3d/gfl_G3dLightSet.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dライトセットクラス
//=====================================

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 */
//-----------------------------------------------------------------------------
LightSet::LightSet(void)
  : m_light_set(NULL)
{
  //GFL_SINGLETON_INSTANCE(System)->GetLightSetManager()->AddObject(this);  // @note 管理したい
}
//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
 */
//-----------------------------------------------------------------------------
LightSet::~LightSet()
{
  Destroy();
  //GFL_SINGLETON_INSTANCE(System)->GetLightSetManager()->RemoveObject(this);  // @note 管理したい
}

//-----------------------------------------------------------------------------
/*
 *  @brief         生成
 */
//-----------------------------------------------------------------------------
void LightSet::Create(
    gfl::heap::NwAllocator*           device_allocator,
    Description*                      description
)
{
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  m_light_set = nw::gfx::LightSet::DynamicBuilder()
      .IsFixedSizeMemory(true)
      .MaxVertexLights(l_description.max_vertex_lights)
      .Create(device_allocator);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         破棄
 */
//-----------------------------------------------------------------------------
void LightSet::Destroy(void)
{
  nw::gfx::SafeDestroy(m_light_set);  // SafeDestroyはNULLを代入してくれる
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ライトを追加する
 */
//-----------------------------------------------------------------------------
b32 LightSet::AddLight(Light* light)
{
  b32 ret = false;
  switch(light->GetType())
  {
  case Light::TYPE_AMBIENT:
    {
      nw::gfx::AmbientLight* nw_light = light->GetNwAmbientLight();
      m_light_set->SetAmbientLight(nw_light);
      ret = true;
      break;
    }
  case Light::TYPE_FRAGMENT:
    {
      nw::gfx::FragmentLight* nw_light = light->GetNwFragmentLight();
      ret = m_light_set->PushFragmentLight(nw_light);
      break;
    }
  case Light::TYPE_HEMI_SPHERE:
    {
      nw::gfx::HemiSphereLight* nw_light = light->GetNwHemiSphereLight();
      m_light_set->SetHemiSphereLight(nw_light);
      ret = true;
      break;
    }
  case Light::TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::VertexLight* nw_light = light->GetNwVertexLight();
      ret = m_light_set->PushVertexLight(nw_light);
#endif 
      break;
    }
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ライトを削除する
 */
//-----------------------------------------------------------------------------
void LightSet::RemoveLight(Light* light)
{
  switch(light->GetType())
  {
  case Light::TYPE_AMBIENT:
    {
      nw::gfx::AmbientLight* nw_light = light->GetNwAmbientLight();
      if( nw_light == m_light_set->GetAmbientLight() )
      {
        m_light_set->SetAmbientLight(NULL);
      }
      break;
    }
  case Light::TYPE_FRAGMENT:
    {
      b32 ret = false;
      nw::gfx::FragmentLight* nw_light = light->GetNwFragmentLight();
      ret = m_light_set->EraseFindFragmentLight(nw_light);
      break;
    }
  case Light::TYPE_HEMI_SPHERE:
    {
      nw::gfx::HemiSphereLight* nw_light = light->GetNwHemiSphereLight();
      if( nw_light == m_light_set->GetHemiSphereLight() )
      {
        m_light_set->SetHemiSphereLight(NULL);
      }
      break;
    }
  case Light::TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      b32 ret = false;
      nw::gfx::VertexLight* nw_light = light->GetNwVertexLight();
      ret = m_light_set->EraseFindVertexLight(nw_light);
#endif 
      break;
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ライトを全て削除する
 */
//-----------------------------------------------------------------------------
void LightSet::RemoveAllLight(void)
{
  m_light_set->ClearAll();
}

 
}  // namespace g3d
}  // namespace grp
}  // namespace gfl

