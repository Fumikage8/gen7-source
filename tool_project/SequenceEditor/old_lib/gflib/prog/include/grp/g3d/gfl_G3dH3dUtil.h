#if !defined( __GFL_G3DH3DUTIL_H__ )
#define __GFL_G3DH3DUTIL_H__
#pragma once

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dUtil.h
 *	@brief  H3D���[�e�B���e�B
 *	@author	Koji Kawada
 *	@date		2012.07.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*����
����H3dUtil�ɂ�
����ƕ֗��ȋ@�\
�ǂ��ł��g����@�\
���W�߂Ă���܂��B*/
#endif


// CTR_SDK
#include <nn/os/CTR/os_MemoryMapCommon.h>

// NW4C
#include <nw/gfx.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>


#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d



namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3D���b�V�����[�e�B���e�B
//=====================================
class H3dMeshUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dMeshUtil);

public:
  // @brief  �R���X�g���N�^
  H3dMeshUtil(void) {}
  // @brief  �f�X�g���N�^
  virtual ~H3dMeshUtil() {}

public:
  // @brief  �`�惌�C���[�𓾂�
  static nw::h3d::TranslucencyLayer::enum_t GetNwTranslucencyLayer(const nw::h3d::res::MeshData& mesh)
  {
    nw::h3d::TranslucencyLayer::enum_t layer = static_cast<nw::h3d::TranslucencyLayer::enum_t>(
      (mesh.key >> nw::h3d::MeshKeyBitField::TRANSLUCENCY_LAYER_SHIFT)
      & nw::h3d::MeshKeyBitField::TRANSLUCENCY_LAYER_MASK );
    return layer;
  }
};

//-------------------------------------
///	H3D���^�f�[�^�}�b�v���[�e�B���e�B
//=====================================
class H3dMetaDataMapUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dMetaDataMapUtil);

public:
  enum
  {
    INDEX_NONE = -1  // �Ȃ��Ƃ��̃C���f�b�N�X
  };

public:
  // @brief  �R���X�g���N�^
  H3dMetaDataMapUtil(void) {}
  // @brief  �f�X�g���N�^
  virtual ~H3dMetaDataMapUtil() {}

public:
  // @brief  ���^�f�[�^�̌��𓾂�
  static s32 GetMetaDataNum(const nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* meta_data_map)
  {
    if( meta_data_map )
    {
      return meta_data_map->size();
    }
    else
    {
      return 0;
    }
  }
  // @brief  ���O���烁�^�f�[�^�̃C���f�b�N�X�𓾂�
  //         name�Ƃ������O�̃��^�f�[�^���Ȃ��Ƃ�INDEX_NONE��Ԃ�
  //         �y���Ӂz���[�U�[�f�[�^�͐擪�� $ (�_���[) ��t���������O�̃��^�f�[�^�Ƃ��ĕێ�����܂��B
  //                 $�����Y��Ȃ��悤�ɁB
  static s32 GetMetaDataIndex(const nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* meta_data_map, const char* name)
  {
    if( meta_data_map )
    {
      u16 index = meta_data_map->FindIndex(name);
      if( index != nw::h3d::fnd::PatriciaDic::NOT_FOUND_INDEX )
      {
        return index;
      }
    }
    return INDEX_NONE;
  }
  // @brief  ���^�f�[�^�𓾂�
  static nw::h3d::res::MetaData& GetMetaData(nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* meta_data_map, const s32 index)
  {
    //if( meta_data_map )�`�F�b�N���Q�ƂȂ̂łł��Ȃ�
    return meta_data_map->At(index);
  }
  // @brief  ���^�f�[�^�𓾂�
  static const nw::h3d::res::MetaData& GetMetaData(const nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* meta_data_map, const s32 index)
  {
    //if( meta_data_map )�`�F�b�N���Q�ƂȂ̂łł��Ȃ�
    return meta_data_map->At(index);
  }
};

//-------------------------------------
///	H3d�A�h���X���[�e�B���e�B
//=====================================
class H3dAddressUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dAddressUtil);

public:
  H3dAddressUtil(void) {}
  ~H3dAddressUtil() {}

/*
NintendoWare/CTR/sources/libraries/h3d/h3d_ResourceImpl.h
        IsContinuousMemory(uptr addr)
        IsVram(uptr addr)
        GetVirtualAddr(uptr addr)
*/

private:
    inline static bool
        IsContinuousMemory(uptr addr)
    {
        const bit32 start = nn::gx::GetPhysicalAddr(NN_OS_ADDR_CONTINUOUS_BEGIN);
        const bit32 end = nn::gx::GetPhysicalAddr(NN_OS_ADDR_RESERVED_BEGIN - 1);
        return (addr >= start && addr <= end ) ? true : false;
    }

    inline static bool
        IsVram(uptr addr)
    {
        const uptr start = nn::gx::GetPhysicalAddr(NN_OS_ADDR_VRAM_BEGIN);
        const uptr end = nn::gx::GetPhysicalAddr(NN_OS_ADDR_VRAM_BEGIN + NN_OS_ADDR_VRAM_SIZE - 1);
        return (addr >= start && addr <= end ) ? true : false;
    }

public:
    // @brief  �����A�h���X����_���A�h���X���擾����
    // @param[in] addr  �����A�h���X
    inline static uptr
        GetVirtualAddr(uptr addr)
    {
        const uptr vramEnd = nn::gx::GetPhysicalAddr(NN_OS_ADDR_VRAM_END - 1);
        if (IsContinuousMemory(addr))
        {
            return NN_OS_PHYSICAL_TO_CONTINUOUS(addr);
        }
        else if (IsVram(addr) || (vramEnd + 1) == addr)
        {
            return addr + NN_OS_ADDR_VRAM_BEGIN - 0x18000000;
        }
        else
        {
            GFL_ASSERT_MSG(false, "[h3d] Address %08x is neither on the device memory nor the VRAM.", addr);  //@check
            return 0;
        }
    }
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

#endif // __GFL_G3DH3DUTIL_H__
