#ifndef __GFL_G3DSORTINGMATERIALIDGENERATOR_H__
#define __GFL_G3DSORTINGMATERIALIDGENERATOR_H__
#pragma once

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSortingMaterialIdGenerator.h
 *	@brief  ソートを行った結果からマテリアルIDの生成を行うクラス
 *	@author	Koji Kawada
 *	@date		2011.08.23
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

/*
NintendoWare/CTR/include/nw/gfx/
gfx_SortingMaterialIdGenerator.h
Revision: 31290
を元に作成 
 */

#include <nw/gfx.h>

namespace gfl
{
namespace grp
{
namespace g3d
{

namespace internal
{
    //---------------------------------------------------------------------------
    //! @brief        マテリアルのソート用のキーと値です。
    //---------------------------------------------------------------------------
    struct MaterialKeyValue
    {
        u32 uniqueId;
        u32 key;
        u32 subKey;
        nw::gfx::Material* material;

        MaterialKeyValue(void)
        {
          uniqueId = 0;
          key      = 0;
          subKey   = 0;
          material = 0;
        }
    };
}

//---------------------------------------------------------------------------
//! @brief        ソートを行った結果からマテリアルIDの生成を行うクラスです。
//!
//! @details      マテリアルIDの生成は以下の優先度で行われます。
//!               バイナリ比較 > シェーダー比較 > 参照テーブル比較
//---------------------------------------------------------------------------
class SortingMaterialIdGenerator : public nw::gfx::IMaterialIdGenerator
{
private:
    GFL_FORBID_COPY_AND_ASSIGN(SortingMaterialIdGenerator);

    typedef nw::ut::MoveArray<internal::MaterialKeyValue> MaterialKeyValueArray;
public:
    NW_UT_RUNTIME_TYPEINFO;
        // NintendoWare/CTR/include/nw/ut/ut_RuntimeTypeInfo.h
    
    //! @brief 設定内容です。
    struct Description
    {        
        bool isFixedSizeMemory; //!< 最初に固定サイズのメモリを確保するフラグです。
        int  maxMaterials;      //!< マテリアルの最大数です。

        //! @brief コンストラクタです。
        Description()
         : isFixedSizeMemory(true),
           maxMaterials(128)
        {}
    };

    //! @brief 定数
    enum
    {
      MATERIAL_ID_START_NO = 1,
      GFX_MATERIAL_ID_BIT_SIZE = 21,
      MAX_MATERIALS = (1 << GFX_MATERIAL_ID_BIT_SIZE) -1
          // nw::gfx::BasicRenderElement::BIT64_MATERIAL_RESOURCE_ID_BIT_SIZE , nw::gfx::BasicRenderElement::BIT64_SIMPLE_MATERIAL_ID_BIT_SIZE の
          // 小さいほうのビットサイズ分全てのビットを1にした値はH3Dに用いる。MAX_MATERIALSの値まではgfxのマテリアルIDにおさまる。
          // H3Dでないマテリアルが使えるmaterialIdは全てのビットを1にしたものから1を引いたところまで、即ち
          // MATERIAL_ID_START_NO<=materialId<MAX_MATERIALSとなる(イコールを含まないので1を引いたところまでになる)。
#if GFL_GRP_G3D_H3D_USE
      , H3D_MATERIAL_ID = MAX_MATERIALS
#endif  // GFL_GRP_G3D_H3D_USE
    };

    //----------------------------------------
    //! @name 生成
    //@{
    

    //! @brief ResMaterialのバイナリリソースのポインタからマテリアルIDの生成を行うクラスを構築するためのクラスです。
    //!
    //!        IsFixedSizeMemory の初期値は true です。false に変更すると、各種最大数の設定は無視されます。
    class Builder
    {
    public:
        //! @brief 生成時以外にもメモリを確保するかどうかのフラグを設定します。
        //! 
        //!        true を指定すると、生成時のみ固定サイズのメモリ確保を行います。
        //!
        //!        false を指定すると、生成時以外にも必要に応じて動的にメモリ確保が行われます。
        Builder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        //! マテリアルの最大数を設定します。
        Builder& MaxMaterials(int maxMaterials)
        {
            if( maxMaterials <= MAX_MATERIALS )
            {
              m_Description.maxMaterials = maxMaterials;
            }
            else
            {
              GFL_ASSERT_MSG(0, "%d > MAX_MATERIALS\n", maxMaterials);
              m_Description.maxMaterials = MAX_MATERIALS;
            }
            return *this;
        }

        //! @brief        マテリアルIDの生成クラスを生成します。
        //!
        //! @param[in]    allocator アロケータです。
        //!
        //! @return       生成したマテリアルIDの生成クラスを返します。
        //!
        nw::gfx::IMaterialIdGenerator* Create(nw::os::IAllocator* allocator);

    private:
        Description m_Description;
    };

    //! @brief        マテリアルを受け取ります。
    //!               ここではマテリアルを配列に追加します。
    //!
    //! @param[in]    material マテリアルです。
    //!
    virtual void Accept(nw::gfx::Material* material);

    //! @brief        マテリアルＩＤを生成します。
    virtual void Generate();

    //@}

private:

    //----------------------------------------
    //! @name コンストラクタ／デストラクタ
    //@{
    
    //! コンストラクタです。
    SortingMaterialIdGenerator(
        nw::os::IAllocator* allocator,
        MaterialKeyValueArray materials,
        MaterialKeyValueArray materialsWorkSpace)
        : nw::gfx::IMaterialIdGenerator(allocator),
          m_Materials(materials),
          m_MaterialsWorkSpace(materialsWorkSpace)
    {
      // NW4CのgfxのマテリアルIDのビットサイズが変わっていないかチェックする。
      STATIC_ASSERT( nw::gfx::RenderElement::BIT64_MATERIAL_RESOURCE_ID_BIT_SIZE == 22 );
      STATIC_ASSERT( nw::gfx::RenderElement::BIT64_SIMPLE_MATERIAL_ID_BIT_SIZE == GFX_MATERIAL_ID_BIT_SIZE );  // 小さいほう
    }

    //@}

    MaterialKeyValueArray m_Materials;
    MaterialKeyValueArray m_MaterialsWorkSpace;


#if GFL_DEBUG
private:
    // 確認出力
    static void PrintMaterialKeyValueArray(MaterialKeyValueArray& materialKeyValueArray);
#endif  // GFL_DEBUG
};

} // namespace g3d
} // namespace grp
} // namespace gfl


#endif // __GFL_G3DSORTINGMATERIALIDGENERATOR_H__
