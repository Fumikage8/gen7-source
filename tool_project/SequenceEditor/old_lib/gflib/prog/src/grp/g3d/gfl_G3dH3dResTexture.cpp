//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dResTexture.cpp
 *	@brief  H3Dテクスチャコンテント
 *	@author	Koji Kawada
 *	@date		2012.08.02
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
#include <grp/g3d/gfl_G3dH3dResTexture.h>
#include <grp/g3d/gfl_G3dH3dCommandUtil.h>



namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					クラス実装
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3Dテクスチャコンテントクラス
//=====================================
H3dResTexture::H3dResTexture(void)
  : H3dContent(),
    m_cont(NULL)
{}

H3dResTexture::~H3dResTexture()
{
  Destroy();
}

void* H3dResTexture::GetPhysicalAddress(void) const
{
  return H3dTextureCommandUtil::GetPhysicalAddress(m_cont->unit0Cmd);  // 0も1も2も同じはず
}
u16   H3dResTexture::GetWidth(void) const
{
  return H3dTextureCommandUtil::GetWidth(m_cont->unit0Cmd);  // 0も1も2も同じはず
}
u16   H3dResTexture::GetHeight(void) const
{
  return H3dTextureCommandUtil::GetHeight(m_cont->unit0Cmd);  // 0も1も2も同じはず
}
gfl::grp::g3d::TextureFormat H3dResTexture::GetFormat(void) const
{
  return static_cast<gfl::grp::g3d::TextureFormat>(m_cont->format);
}
u8    H3dResTexture::GetMipmap(void) const
{
  return m_cont->mipmapSize;
}

const char* H3dResTexture::GetName(void) const
{
  return m_cont->name;
}


void* H3dResTexture::GetVirtualAddress(void) const
{
  return reinterpret_cast<void*>(H3dAddressUtil::GetVirtualAddr(reinterpret_cast<uptr>(GetPhysicalAddress())));
}
gfl::grp::g3d::Texture::Format H3dResTexture::GetFormatAsTexture(void) const
{
  return static_cast<gfl::grp::g3d::Texture::Format>(m_cont->format);
}

// @note ミップマップを考慮していないので注意して下さい。
u32 H3dResTexture::GetByteSize(void) const
{
  u32 byte_size = (static_cast<u32>(gfl::grp::g3d::Texture::GetTextureBitsPerPixel(GetFormatAsTexture()))) * GetHeight() * GetWidth() / 8;
  return byte_size;
}

// ミップマップを考慮した版
u32 H3dResTexture::GetByteSizeWithMipmap(void) const
{
  u32 byte_size = (static_cast<u32>(gfl::grp::g3d::Texture::GetTextureBitsPerPixel(GetFormatAsTexture()))) * GetHeight() * GetWidth() / 8 * ((GetMipmap()>1)?2:1);
  return byte_size;
}


b32 H3dResTexture::createInternal(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const H3dResource* resource, const char* name)
{
  s32 index = resource->GetTextureContentIndex(name);
  return createInternal(heap_allocator, device_allocator, resource, index);
}

b32 H3dResTexture::createInternal(gfl::heap::NwAllocator* /*heap_allocator*/, gfl::heap::NwAllocator* /*device_allocator*/, const H3dResource* resource, s32 index_in_resource)
{
  // resourceを覚えておく
  m_cont_index = index_in_resource;
  m_cont = resource->GetNwTextureContent(index_in_resource);
  return true;
}

void H3dResTexture::destroyInternal(void)
{
  // コンストラクタと同じ初期化
  m_cont_index = H3dResource::INDEX_NONE;
  m_cont = NULL;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

