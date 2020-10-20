#if !defined( __GFL_G3DH3DUTIL_H__ )
#define __GFL_G3DH3DUTIL_H__
#pragma once

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dUtil.h
 *	@brief  H3Dユーティリティ
 *	@author	Koji Kawada
 *	@date		2012.07.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*説明
このH3dUtilには
あると便利な機能
どこでも使える機能
を集めてあります。*/
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
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3Dメッシュユーティリティ
//=====================================
class H3dMeshUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dMeshUtil);

public:
  // @brief  コンストラクタ
  H3dMeshUtil(void) {}
  // @brief  デストラクタ
  virtual ~H3dMeshUtil() {}

public:
  // @brief  描画レイヤーを得る
  static nw::h3d::TranslucencyLayer::enum_t GetNwTranslucencyLayer(const nw::h3d::res::MeshData& mesh)
  {
    nw::h3d::TranslucencyLayer::enum_t layer = static_cast<nw::h3d::TranslucencyLayer::enum_t>(
      (mesh.key >> nw::h3d::MeshKeyBitField::TRANSLUCENCY_LAYER_SHIFT)
      & nw::h3d::MeshKeyBitField::TRANSLUCENCY_LAYER_MASK );
    return layer;
  }
};

//-------------------------------------
///	H3Dメタデータマップユーティリティ
//=====================================
class H3dMetaDataMapUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dMetaDataMapUtil);

public:
  enum
  {
    INDEX_NONE = -1  // ないときのインデックス
  };

public:
  // @brief  コンストラクタ
  H3dMetaDataMapUtil(void) {}
  // @brief  デストラクタ
  virtual ~H3dMetaDataMapUtil() {}

public:
  // @brief  メタデータの個数を得る
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
  // @brief  名前からメタデータのインデックスを得る
  //         nameという名前のメタデータがないときINDEX_NONEを返す
  //         【注意】ユーザーデータは先頭に $ (ダラー) を付加した名前のメタデータとして保持されます。
  //                 $をつけ忘れないように。
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
  // @brief  メタデータを得る
  static nw::h3d::res::MetaData& GetMetaData(nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* meta_data_map, const s32 index)
  {
    //if( meta_data_map )チェックが参照なのでできない
    return meta_data_map->At(index);
  }
  // @brief  メタデータを得る
  static const nw::h3d::res::MetaData& GetMetaData(const nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* meta_data_map, const s32 index)
  {
    //if( meta_data_map )チェックが参照なのでできない
    return meta_data_map->At(index);
  }
};

//-------------------------------------
///	H3dアドレスユーティリティ
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
    // @brief  物理アドレスから論理アドレスを取得する
    // @param[in] addr  物理アドレス
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
