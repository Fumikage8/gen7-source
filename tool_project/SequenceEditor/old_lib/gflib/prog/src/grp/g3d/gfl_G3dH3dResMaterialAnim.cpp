//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dResMaterialAnim.cpp
 *	@brief  H3Dアニメーションコンテント
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
#include <grp/g3d/gfl_G3dH3dResMaterialAnim.h>



namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					クラス実装
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3Dマテリアルアニメーションコンテントクラス
//=====================================
H3dResMaterialAnim::H3dResMaterialAnim(void)
  : H3dResAnim(),
    m_texpat_buff(NULL)
{}

H3dResMaterialAnim::~H3dResMaterialAnim()
{
  Destroy();
}

b32 H3dResMaterialAnim::SetupTexturePatternAnimTexture(H3dResource* other_resource)
{
  b32 ret = false;

  if( m_texpat_buff )
  {
    // テクスチャパターンアニメーションがある場合

    // 既に設定されているバインド情報は削除されません。 複数回 Bind() を実行した場合、まだバインドされていないテクスチャのみバインドを試みます。
    const nw::h3d::res::MaterialAnimContent* mat_anim_cont = GetNwMaterialAnimContent();
    nw::h3d::Result result = m_texpat_bind_state.Bind(mat_anim_cont, other_resource->GetNwResourceBinary());
 
    // バインド結果
    ////////{
    ////////  // これは今回引数で渡したother_resourceに関しての結果であり、全テクスチャがバインドされているかの結果ではないので、ここでは使えない。
    ////////  s32 res_desc = result.GetDescription();
    ////////  if( res_desc == nw::h3d::Result::RESULT_OK ) ret = true;
    ////////}
    {
      ret = true;
      for(u32 i=0; i<mat_anim_cont->GetTextureListCount(); ++i)
      {
        if( !(m_texpat_bind_state.GetTexture(i)) )
        {
          ret = false;
          break;
        }
      }
    }
  }
  else
  {
    // テクスチャパターンアニメーションがない場合
    // セットしなくても完成品なので、trueを返す
    ret = true;
  }
  
  return ret;
}

b32 H3dResMaterialAnim::createInternal(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const H3dResource* resource, const char* name)
{
  s32 index = resource->GetMaterialAnimContentIndex(name);
  return createInternal(heap_allocator, device_allocator, resource, index);
}

b32 H3dResMaterialAnim::createInternal(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* /*device_allocator*/, const H3dResource* resource, s32 index_in_resource)
{
  // resourceを覚えておく
  m_cont_index = index_in_resource;
  m_cont = resource->GetNwMaterialAnimContent(index_in_resource);

  {
    u32 num = getTexturePatternAnimTextureNum();  // この関数はm_contの設定が終わっていないと使えないので、設定順に注意すること。
    if( num > 0 )
    {
      m_texpat_buff = GflHeapAllocMemoryAlign(
          heap_allocator->GetHeapBase(),
          nw::h3d::TexturePatternAnimBindState::CalcSize(num), 4);
      m_texpat_bind_state.Init(m_texpat_buff, num);
    }
  }

  return true;
}

void H3dResMaterialAnim::destroyInternal(void)
{
  if(m_texpat_buff) GflHeapFreeMemory(m_texpat_buff);

  // コンストラクタと同じ初期化
  m_cont_index = H3dResource::INDEX_NONE;
  m_cont = NULL;
  m_texpat_buff = NULL;
}

u32 H3dResMaterialAnim::getTexturePatternAnimTextureNum(void) const
{
  u32 ret = 0;
  const nw::h3d::res::MaterialAnimContent* mat_cont = GetNwMaterialAnimContent();
  if( mat_cont->HasTexturePatternAnim() )
  {
    ret = mat_cont->GetTextureListCount();
    GFL_ASSERT_MSG(ret>0, "HasTexturePatternAnimがtrueなのにGetTextureListCountが0個です。\n");  //@check
  }
  else
  {
    GFL_ASSERT_MSG(mat_cont->GetTextureListCount()==0, "HasTexturePatternAnimがfalseなのにGetTextureListCountが0個より大。\n");  //@check
  }
  return ret;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

