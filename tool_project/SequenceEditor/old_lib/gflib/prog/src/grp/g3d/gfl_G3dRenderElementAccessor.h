#pragma once
#if !defined( __GFL_G3DRENDERELEMENTACCESSOR_H__ )
#define __GFL_G3DRENDERELEMENTACCESSOR_H__

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dRenderElementAccessor.h
 *	@brief  3D�����_�[�G�������g�A�N�Z�b�T�[
 *	@author	Koji Kawada
 *	@date		2012.07.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
 */
//-----------------------------------------------------------------------------

#if GFL_GRP_G3D_H3D_USE

// @brief  ���b�V���̌^�`�F���W���[
union MeshTypeChanger
{
  //nw::gfx::ResMesh        mesh;      // 4�o�C�g
  void*                   mesh_ptr;  // 4�o�C�g  // nw::gfx::ResMesh�͋��p�̂̃����o�ɂ͂ł��Ȃ��̂ŁA���̗B��̃����o�ϐ��ł���ResCommonBase��void* mpData;�Ɋo���Ă����B
  nw::h3d::res::MeshData* h3d_mesh;  // 4�o�C�g
};

// @brief  ���f���̌^�`�F���W���[
union ModelTypeChanger
{
  nw::gfx::Model* model;      // 4�o�C�g
  H3dModel*       h3d_model;  // 4�o�C�g
};

#endif  // GFL_GRP_G3D_H3D_USE


  
//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
 */
//-----------------------------------------------------------------------------

#if GFL_GRP_G3D_H3D_USE

// @brief  �����_�[�G�������g�A�N�Z�b�T�[
class RenderElementAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(RenderElementAccessor);

private:
  enum
  {
    // ���C���[ID(�v���O���}���t����l)
    LAYER_SHIFT = 56,
    LAYER_MASK  = (1<<8)-1,

    // �`�惌�C���[
    TRANSLUCENCY_KIND_SHIFT          = 54,
    TRANSLUCENCY_KIND_MASK           = (1<<2)-1,
 
    // Z�[�x
    PRIOR_MATERIAL_DEPTH_SHIFT = 0,
    PRIOR_DEPTH_DEPTH_SHIFT    = 21,
    DEPTH_MASK                 = (1<<24)-1,

    // �}�e���A��ID
    PRIOR_MATERIAL_MATERIAL_ID_SHIFT = 27,
    PRIOR_MATERIAL_MATERIAL_ID_MASK  = (1<<22)-1,
    PRIOR_DEPTH_MATERIAL_ID_SHIFT    = 0,
    PRIOR_DEPTH_MATERIAL_ID_MASK     = (1<<21)-1,

    // �}�e���A���D��x(�f�U�C�i�����b�V���ɕt����l)
    PRIOR_MATERIAL_PRIORITY_SHIFT = 49,
    PRIOR_MATERIAL_PRIORITY_MASK  = (1<<4)-1,
    PRIOR_DEPTH_PRIORITY_SHIFT = 45,
    PRIOR_DEPTH_PRIORITY_MASK  = (1<<8)-1,
  };

public:
  RenderElementAccessor(void)
  {
    // NW4C��gfx�̎������ς���Ă���Ƃ����Ȃ��̂Ń`�F�b�N����B
    STATIC_ASSERT( sizeof(nw::gfx::RenderKeyType)==sizeof(bit64) );
    //STATIC_ASSERT( nw::gfx::BasicRenderElement<bit64>::BIT64_TRANSLUCENCY_KIND_SHIFT == TRANSLUCENCY_KIND_SHIFT );  // ���������Ă���������

    STATIC_ASSERT( nw::gfx::RenderElement::BIT64_LAYER_SHIFT == LAYER_SHIFT );
    STATIC_ASSERT( nw::gfx::RenderElement::BIT64_LAYER_MASK == LAYER_MASK );

    STATIC_ASSERT( nw::gfx::RenderElement::BIT64_TRANSLUCENCY_KIND_SHIFT == TRANSLUCENCY_KIND_SHIFT );
    STATIC_ASSERT( nw::gfx::RenderElement::BIT64_TRANSLUCENCY_KIND_BIT_SIZE == 2 );
 
    STATIC_ASSERT( nw::gfx::RenderElement::BIT64_DEPTH_MASK == DEPTH_MASK );
    
    STATIC_ASSERT( nw::gfx::RenderElement::BIT64_MATERIAL_RESOURCE_ID_BIT_SIZE == 22 );
    STATIC_ASSERT( nw::gfx::RenderElement::BIT64_SIMPLE_MATERIAL_ID_BIT_SIZE == 21 );

    STATIC_ASSERT( nw::gfx::RenderElement::BIT64_MATERIAL_PRIORITY_MASK == PRIOR_MATERIAL_PRIORITY_MASK );
    STATIC_ASSERT( nw::gfx::RenderElement::BIT64_SIMPLE_MATERIAL_PRIORITY_MASK == PRIOR_DEPTH_PRIORITY_MASK );
  }

  virtual ~RenderElementAccessor(void){}

public:
  static b32 IsH3dRenderElement(const nw::gfx::RenderElement& element)
  {
    u32 material_id = 0;

    const nw::gfx::RenderKeyType key = element.Key();  // bit64�Ɠ���  // xy_ctrlib/NintendoWare/CTR/include/nw/gfx/gfx_RenderElement.h

    nw::gfx::res::ResMaterial::TranslucencyKind kind = GetTranslucencyKind(element);
    if(kind == nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_TRANSLUCENT)
    {
      material_id = static_cast<u32>( ( ( key >> PRIOR_DEPTH_MATERIAL_ID_SHIFT ) & (PRIOR_DEPTH_MATERIAL_ID_MASK) ) );
    }
    else
    {
      material_id = static_cast<u32>( ( ( key >> PRIOR_MATERIAL_MATERIAL_ID_SHIFT ) & (PRIOR_MATERIAL_MATERIAL_ID_MASK) ) );
    }

    static const u32      MaterialBitMask = (1 << (SortingMaterialIdGenerator::GFX_MATERIAL_ID_BIT_SIZE-1));//�Q�O�r�b�g�ڂ��`�F�b�N
    
    return (material_id & MaterialBitMask);
    //return (material_id == SortingMaterialIdGenerator::H3D_MATERIAL_ID);
  }

  static nw::gfx::res::ResMaterial::TranslucencyKind GetTranslucencyKind(const nw::gfx::RenderElement& element)
  {
    nw::gfx::res::ResMaterial::TranslucencyKind kind = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_OPAQUE;
    
    const nw::gfx::RenderKeyType key = element.Key();

    kind = static_cast<nw::gfx::res::ResMaterial::TranslucencyKind>( ( key >> TRANSLUCENCY_KIND_SHIFT ) & (TRANSLUCENCY_KIND_MASK) );

    return kind;
  }

  static nw::gfx::RenderKeyType CreateH3dMeshRenderKey(  // bit64�Ɠ���
    u8                                          layer,     // �v���O���}���t����l(7�r�b�g+1�r�b�g(��1�r�b�g�̓��f�����p�[�e�B�N�����Ŏg�p))
    nw::gfx::res::ResMaterial::TranslucencyKind kind,      // �f�U�C�i���}�e���A���ɕt����l
    u8                                          priority,  // �f�U�C�i�����b�V���ɕt����l(TranslucencyKind�ɂ����4�r�b�g��8�r�b�g�ɂȂ�)
    f32                                         depth,      // �v���O�����ŋ��߂�Z�[�x
    u32                                         materialID
  )
  {
    static const u32    H3dMaterialBit = (1 << (SortingMaterialIdGenerator::GFX_MATERIAL_ID_BIT_SIZE-1));//�Q�O�r�b�g�ڂ��P
    
    nw::gfx::RenderKeyType key = 0;
    
    key |= (static_cast<nw::gfx::RenderKeyType>(layer & LAYER_MASK))//8
      << LAYER_SHIFT;  // @note ��1�r�b�g�͌Ăяo�����ōl�����Ă���O��̋L�q
    key |= (static_cast<nw::gfx::RenderKeyType>(kind & TRANSLUCENCY_KIND_MASK))//2
      << TRANSLUCENCY_KIND_SHIFT;//54
    
    
    

    if(kind == nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_TRANSLUCENT)
    {
      key |= (static_cast<nw::gfx::RenderKeyType>(priority & PRIOR_DEPTH_PRIORITY_MASK))//8
        << PRIOR_DEPTH_PRIORITY_SHIFT;//45
      
      //! �������̂���ɕ`�悳���悤�ɂȂ�܂��B
      key |= nw::gfx::RenderElement::ReverseDepth24bitQuantizer()(depth)//24
        << PRIOR_DEPTH_DEPTH_SHIFT;//21
      
      materialID |= H3dMaterialBit;
      key |= (static_cast<nw::gfx::RenderKeyType>(materialID & PRIOR_DEPTH_MATERIAL_ID_MASK))//21
        << PRIOR_DEPTH_MATERIAL_ID_SHIFT;//0
    }
    else
    {
      key |= (static_cast<nw::gfx::RenderKeyType>(priority & PRIOR_MATERIAL_PRIORITY_MASK))//4
        << PRIOR_MATERIAL_PRIORITY_SHIFT;//49
      key |= (static_cast<nw::gfx::RenderKeyType>(H3dMaterialBit & PRIOR_MATERIAL_MATERIAL_ID_MASK))//22
        << PRIOR_MATERIAL_MATERIAL_ID_SHIFT;//27
      
      key |= (static_cast<nw::gfx::RenderKeyType>(materialID & 0xFFFFFFFF));//32
    }

    return key;
  }

};

#endif  // GFL_GRP_G3D_H3D_USE


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DRENDERELEMENTACCESSOR_H__
