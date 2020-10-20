#ifndef __GFL_G3DSORTINGMATERIALIDGENERATOR_H__
#define __GFL_G3DSORTINGMATERIALIDGENERATOR_H__
#pragma once

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSortingMaterialIdGenerator.h
 *	@brief  �\�[�g���s�������ʂ���}�e���A��ID�̐������s���N���X
 *	@author	Koji Kawada
 *	@date		2011.08.23
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

/*
NintendoWare/CTR/include/nw/gfx/
gfx_SortingMaterialIdGenerator.h
Revision: 31290
�����ɍ쐬 
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
    //! @brief        �}�e���A���̃\�[�g�p�̃L�[�ƒl�ł��B
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
//! @brief        �\�[�g���s�������ʂ���}�e���A��ID�̐������s���N���X�ł��B
//!
//! @details      �}�e���A��ID�̐����͈ȉ��̗D��x�ōs���܂��B
//!               �o�C�i����r > �V�F�[�_�[��r > �Q�ƃe�[�u����r
//---------------------------------------------------------------------------
class SortingMaterialIdGenerator : public nw::gfx::IMaterialIdGenerator
{
private:
    GFL_FORBID_COPY_AND_ASSIGN(SortingMaterialIdGenerator);

    typedef nw::ut::MoveArray<internal::MaterialKeyValue> MaterialKeyValueArray;
public:
    NW_UT_RUNTIME_TYPEINFO;
        // NintendoWare/CTR/include/nw/ut/ut_RuntimeTypeInfo.h
    
    //! @brief �ݒ���e�ł��B
    struct Description
    {        
        bool isFixedSizeMemory; //!< �ŏ��ɌŒ�T�C�Y�̃��������m�ۂ���t���O�ł��B
        int  maxMaterials;      //!< �}�e���A���̍ő吔�ł��B

        //! @brief �R���X�g���N�^�ł��B
        Description()
         : isFixedSizeMemory(true),
           maxMaterials(128)
        {}
    };

    //! @brief �萔
    enum
    {
      MATERIAL_ID_START_NO = 1,
      GFX_MATERIAL_ID_BIT_SIZE = 21,
      MAX_MATERIALS = (1 << GFX_MATERIAL_ID_BIT_SIZE) -1
          // nw::gfx::BasicRenderElement::BIT64_MATERIAL_RESOURCE_ID_BIT_SIZE , nw::gfx::BasicRenderElement::BIT64_SIMPLE_MATERIAL_ID_BIT_SIZE ��
          // �������ق��̃r�b�g�T�C�Y���S�Ẵr�b�g��1�ɂ����l��H3D�ɗp����BMAX_MATERIALS�̒l�܂ł�gfx�̃}�e���A��ID�ɂ����܂�B
          // H3D�łȂ��}�e���A�����g����materialId�͑S�Ẵr�b�g��1�ɂ������̂���1���������Ƃ���܂ŁA����
          // MATERIAL_ID_START_NO<=materialId<MAX_MATERIALS�ƂȂ�(�C�R�[�����܂܂Ȃ��̂�1���������Ƃ���܂łɂȂ�)�B
#if GFL_GRP_G3D_H3D_USE
      , H3D_MATERIAL_ID = MAX_MATERIALS
#endif  // GFL_GRP_G3D_H3D_USE
    };

    //----------------------------------------
    //! @name ����
    //@{
    

    //! @brief ResMaterial�̃o�C�i�����\�[�X�̃|�C���^����}�e���A��ID�̐������s���N���X���\�z���邽�߂̃N���X�ł��B
    //!
    //!        IsFixedSizeMemory �̏����l�� true �ł��Bfalse �ɕύX����ƁA�e��ő吔�̐ݒ�͖�������܂��B
    class Builder
    {
    public:
        //! @brief �������ȊO�ɂ����������m�ۂ��邩�ǂ����̃t���O��ݒ肵�܂��B
        //! 
        //!        true ���w�肷��ƁA�������̂݌Œ�T�C�Y�̃������m�ۂ��s���܂��B
        //!
        //!        false ���w�肷��ƁA�������ȊO�ɂ��K�v�ɉ����ē��I�Ƀ������m�ۂ��s���܂��B
        Builder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        //! �}�e���A���̍ő吔��ݒ肵�܂��B
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

        //! @brief        �}�e���A��ID�̐����N���X�𐶐����܂��B
        //!
        //! @param[in]    allocator �A���P�[�^�ł��B
        //!
        //! @return       ���������}�e���A��ID�̐����N���X��Ԃ��܂��B
        //!
        nw::gfx::IMaterialIdGenerator* Create(nw::os::IAllocator* allocator);

    private:
        Description m_Description;
    };

    //! @brief        �}�e���A�����󂯎��܂��B
    //!               �����ł̓}�e���A����z��ɒǉ����܂��B
    //!
    //! @param[in]    material �}�e���A���ł��B
    //!
    virtual void Accept(nw::gfx::Material* material);

    //! @brief        �}�e���A���h�c�𐶐����܂��B
    virtual void Generate();

    //@}

private:

    //----------------------------------------
    //! @name �R���X�g���N�^�^�f�X�g���N�^
    //@{
    
    //! �R���X�g���N�^�ł��B
    SortingMaterialIdGenerator(
        nw::os::IAllocator* allocator,
        MaterialKeyValueArray materials,
        MaterialKeyValueArray materialsWorkSpace)
        : nw::gfx::IMaterialIdGenerator(allocator),
          m_Materials(materials),
          m_MaterialsWorkSpace(materialsWorkSpace)
    {
      // NW4C��gfx�̃}�e���A��ID�̃r�b�g�T�C�Y���ς���Ă��Ȃ����`�F�b�N����B
      STATIC_ASSERT( nw::gfx::RenderElement::BIT64_MATERIAL_RESOURCE_ID_BIT_SIZE == 22 );
      STATIC_ASSERT( nw::gfx::RenderElement::BIT64_SIMPLE_MATERIAL_ID_BIT_SIZE == GFX_MATERIAL_ID_BIT_SIZE );  // �������ق�
    }

    //@}

    MaterialKeyValueArray m_Materials;
    MaterialKeyValueArray m_MaterialsWorkSpace;


#if GFL_DEBUG
private:
    // �m�F�o��
    static void PrintMaterialKeyValueArray(MaterialKeyValueArray& materialKeyValueArray);
#endif  // GFL_DEBUG
};

} // namespace g3d
} // namespace grp
} // namespace gfl


#endif // __GFL_G3DSORTINGMATERIALIDGENERATOR_H__
