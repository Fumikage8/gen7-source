#pragma once
#if !defined( __GFL_G3DRENDERELEMENTACCESSOR_H__ )
#define __GFL_G3DRENDERELEMENTACCESSOR_H__

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dRenderElementAccessor.h
 *	@brief  3Dレンダーエレメントアクセッサー
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
 *					構造体宣言
 */
//-----------------------------------------------------------------------------

#if GFL_GRP_G3D_H3D_USE

// @brief  メッシュの型チェンジャー
union MeshTypeChanger
{
  //nw::gfx::ResMesh        mesh;      // 4バイト
  void*                   mesh_ptr;  // 4バイト  // nw::gfx::ResMeshは共用体のメンバにはできないので、その唯一のメンバ変数であるResCommonBaseのvoid* mpData;に覚えておく。
  nw::h3d::res::MeshData* h3d_mesh;  // 4バイト
};

// @brief  モデルの型チェンジャー
union ModelTypeChanger
{
  nw::gfx::Model* model;      // 4バイト
  H3dModel*       h3d_model;  // 4バイト
};

#endif  // GFL_GRP_G3D_H3D_USE


  
//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------

#if GFL_GRP_G3D_H3D_USE

// @brief  レンダーエレメントアクセッサー
class RenderElementAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(RenderElementAccessor);

private:
  enum
  {
    // レイヤーID(プログラマが付ける値)
    LAYER_SHIFT = 56,
    LAYER_MASK  = (1<<8)-1,

    // 描画レイヤー
    TRANSLUCENCY_KIND_SHIFT          = 54,
    TRANSLUCENCY_KIND_MASK           = (1<<2)-1,
 
    // Z深度
    PRIOR_MATERIAL_DEPTH_SHIFT = 0,
    PRIOR_DEPTH_DEPTH_SHIFT    = 21,
    DEPTH_MASK                 = (1<<24)-1,

    // マテリアルID
    PRIOR_MATERIAL_MATERIAL_ID_SHIFT = 27,
    PRIOR_MATERIAL_MATERIAL_ID_MASK  = (1<<22)-1,
    PRIOR_DEPTH_MATERIAL_ID_SHIFT    = 0,
    PRIOR_DEPTH_MATERIAL_ID_MASK     = (1<<21)-1,

    // マテリアル優先度(デザイナがメッシュに付ける値)
    PRIOR_MATERIAL_PRIORITY_SHIFT = 49,
    PRIOR_MATERIAL_PRIORITY_MASK  = (1<<4)-1,
    PRIOR_DEPTH_PRIORITY_SHIFT = 45,
    PRIOR_DEPTH_PRIORITY_MASK  = (1<<8)-1,
  };

public:
  RenderElementAccessor(void)
  {
    // NW4Cのgfxの実装が変わっているといけないのでチェックする。
    STATIC_ASSERT( sizeof(nw::gfx::RenderKeyType)==sizeof(bit64) );
    //STATIC_ASSERT( nw::gfx::BasicRenderElement<bit64>::BIT64_TRANSLUCENCY_KIND_SHIFT == TRANSLUCENCY_KIND_SHIFT );  // こう書いてもいいけど

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

    const nw::gfx::RenderKeyType key = element.Key();  // bit64と同じ  // xy_ctrlib/NintendoWare/CTR/include/nw/gfx/gfx_RenderElement.h

    nw::gfx::res::ResMaterial::TranslucencyKind kind = GetTranslucencyKind(element);
    if(kind == nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_TRANSLUCENT)
    {
      material_id = static_cast<u32>( ( ( key >> PRIOR_DEPTH_MATERIAL_ID_SHIFT ) & (PRIOR_DEPTH_MATERIAL_ID_MASK) ) );
    }
    else
    {
      material_id = static_cast<u32>( ( ( key >> PRIOR_MATERIAL_MATERIAL_ID_SHIFT ) & (PRIOR_MATERIAL_MATERIAL_ID_MASK) ) );
    }

    static const u32      MaterialBitMask = (1 << (SortingMaterialIdGenerator::GFX_MATERIAL_ID_BIT_SIZE-1));//２０ビット目をチェック
    
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

  static nw::gfx::RenderKeyType CreateH3dMeshRenderKey(  // bit64と同じ
    u8                                          layer,     // プログラマが付ける値(7ビット+1ビット(下1ビットはモデルかパーティクルかで使用))
    nw::gfx::res::ResMaterial::TranslucencyKind kind,      // デザイナがマテリアルに付ける値
    u8                                          priority,  // デザイナがメッシュに付ける値(TranslucencyKindによって4ビットか8ビットになる)
    f32                                         depth,      // プログラムで求めるZ深度
    u32                                         materialID
  )
  {
    static const u32    H3dMaterialBit = (1 << (SortingMaterialIdGenerator::GFX_MATERIAL_ID_BIT_SIZE-1));//２０ビット目を１
    
    nw::gfx::RenderKeyType key = 0;
    
    key |= (static_cast<nw::gfx::RenderKeyType>(layer & LAYER_MASK))//8
      << LAYER_SHIFT;  // @note 下1ビットは呼び出し元で考慮している前提の記述
    key |= (static_cast<nw::gfx::RenderKeyType>(kind & TRANSLUCENCY_KIND_MASK))//2
      << TRANSLUCENCY_KIND_SHIFT;//54
    
    
    

    if(kind == nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_TRANSLUCENT)
    {
      key |= (static_cast<nw::gfx::RenderKeyType>(priority & PRIOR_DEPTH_PRIORITY_MASK))//8
        << PRIOR_DEPTH_PRIORITY_SHIFT;//45
      
      //! 遠いものが先に描画されるようになります。
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
