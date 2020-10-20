#ifndef __GFL_G3DH3DRESANIM_H__
#define __GFL_G3DH3DRESANIM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dResAnim.h
 *	@brief  H3Dリソースアニメーション
 *	@author	Koji Kawada
 *	@date		2012.04.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*説明
このH3dResAnimでは
nw::h3d::res::SkeletalAnimContent
nw::h3d::res::MaterialAnimContent
nw::h3d::res::VisibilityAnimContent
nw::h3d::res::LightAnimContent←これはまだ扱っていません
nw::h3d::res::CameraAnimContent←これはまだ扱っていません
nw::h3d::res::FogAnimContent←これはまだ扱っていません
を扱います。
H3dResourceを使うアニメーションです。*/
#endif


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
#include <grp/g3d/gfl_G3dH3dResource.h>
#include <grp/g3d/gfl_G3dH3dContent.h>



namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

// 前方宣言
class H3dUserData;



//-------------------------------------
///	H3Dアニメーションコンテントクラス
//=====================================
class H3dResAnim : public H3dContent
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dResAnim);

  friend class H3dAnimationSlot;


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 定義
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  // @brief  アニメーション要素のバインド先の型
  enum AnimPrimitiveType
  {
    // NintendoWare/CTR/include/nw/h3d/h3d_Const.h
    // struct AnimPrimitiveType
    //     enum enum_t

    ANIM_PRIMITIVE_TYPE_FLOAT                 = nw::h3d::AnimPrimitiveType::FLOAT                ,
    ANIM_PRIMITIVE_TYPE_INT                   = nw::h3d::AnimPrimitiveType::INT                  ,
    ANIM_PRIMITIVE_TYPE_VECTOR2               = nw::h3d::AnimPrimitiveType::VECTOR2              ,
    ANIM_PRIMITIVE_TYPE_VECTOR3               = nw::h3d::AnimPrimitiveType::VECTOR3              ,
    ANIM_PRIMITIVE_TYPE_TRANSFORM             = nw::h3d::AnimPrimitiveType::TRANSFORM            ,
    ANIM_PRIMITIVE_TYPE_RGBA_COLOR            = nw::h3d::AnimPrimitiveType::RGBA_COLOR           ,
    ANIM_PRIMITIVE_TYPE_TEXTURE               = nw::h3d::AnimPrimitiveType::TEXTURE              ,
    ANIM_PRIMITIVE_TYPE_BAKED_TRANSFORM       = nw::h3d::AnimPrimitiveType::BAKED_TRANSFORM      ,
    ANIM_PRIMITIVE_TYPE_BAKED_BOOL            = nw::h3d::AnimPrimitiveType::BAKED_BOOL           ,
    ANIM_PRIMITIVE_TYPE_FULL_BAKED_TRANSFORM  = nw::h3d::AnimPrimitiveType::FULL_BAKED_TRANSFORM ,
    //ANIM_PRIMITIVE_TYPE_BOOL                // 非対応
    ANIM_PRIMITIVE_TYPE_INVALID               = -2
  };

  // @brief  定数
  enum
  {
    INDEX_NONE  = -1  // ないときのインデックス
  };


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 構造体
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // @brief  1ビットベクトル 
  struct BITVEC4
  {
    u32 x: 1;
    u32 y: 1;
    u32 z: 1;
    u32 w: 1;
  };
  typedef BITVEC4 BITVEC3;
  typedef BITVEC4 BITVEC2;

  // @brief  u32ベクトル 
  struct U32VEC3
  {
    u32 x;
    u32 y;
    u32 z;
  };


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // コンストラクタ / デストラクタ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   */
  //-----------------------------------------------------------------------------
  H3dResAnim(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~H3dResAnim();



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 生成 / 破棄
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  // 基底クラスのまま



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ユーザデータ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  s32 GetUserDataCount() const;
  // 【注意】ユーザーデータは先頭に $ (ダラー) を付加した名前のメタデータとして保持されます。
  //         $をつけ忘れないように。
  s32 GetUserDataIndex(const char* name) const;
  void GetUserData(const s32 user_data_index, H3dUserData* user_data) const;

private:
  nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* getMetaDataMap(void) const;


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 設定 / 取得
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションタイプを得る
   */
  //-----------------------------------------------------------------------------
  AnimationType GetAnimationType(void) const;
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーション要素の数を得る
   */
  //-----------------------------------------------------------------------------
  u32 GetAnimElementNum(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         フレーム数を得る
   */
  //-----------------------------------------------------------------------------
  f32 GetFrameSize(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         フレーム数を正規化する
   */
  //-----------------------------------------------------------------------------
  f32 NormalizeFrame(f32 frame) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションがループ再生か否か取得する
   *  @retval        bool            ループのときtrue、ループでないときfalseを返す
   */
  //-----------------------------------------------------------------------------
  b32 IsLoop(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションにループ再生を設定する
   *  @param[in]     loop            ループのときtrue、ループでないときfalseを設定する
   */
  //-----------------------------------------------------------------------------
  void SetLoop(b32 loop);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // AnimElementData
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-------------------------------------
  // 共通
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーション要素のインデックスからバインド先の名前を取得する
   *  @param[in]     anim_element_index  インデックス(0<= <GetAnimElementNum())
   *  @retval        const char*         バインド先の名前
   *                                    インデックスが不正のときNULLを返す
   */
  //-----------------------------------------------------------------------------
  const char* GetTargetName(const s32 anim_element_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーション要素のバインド先の名前からインデックスを取得する
   *  @param[in]     name  バインド先の名前
   *  @retval        s32   インデックス
   *                       ないときINDEX_NONEを返す
   */
  //-----------------------------------------------------------------------------
  s32 GetAnimElementIndex(const char* target_name) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーション要素のバインド先の型を取得する
   *  @param[in]     anim_element_index  インデックス(0<= <GetAnimElementNum())
   *  @retval        AnimPrimitiveType   バインド先の型
   *                                     インデックスが不正のときANIM_PRIMITIVE_TYPE_INVALIDを返す
   */
  //-----------------------------------------------------------------------------
  AnimPrimitiveType GetAnimPrimitiveType(const s32  anim_element_index) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_FLOAT
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     指定したフレームにおける値を得る
   *
   *  @param[in]    anim_element_index  インデックス(0<= <GetAnimElementNum())
   *  @param[in]    frame               フレーム
   *  @param[out]   value               値
   *
   *  @retval    有効な値の場合はtrueを返す。有効な値でない場合はfalseを返す。
   */
  //-----------------------------------------------------------------------------
  b32 GetFloatValue(const s32 anim_element_index, const f32 frame, f32* value) const;
  b32 GetFloatValue(const s32 anim_element_index, const s32 frame, f32* value) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_INT
  //-------------------------------------
  // なし

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_VECTOR2
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     指定したフレームにおける値を得る
   *
   *  @param[in]    anim_element_index  インデックス(0<= <GetAnimElementNum())
   *  @param[in]    frame               フレーム
   *  @param[out]   value               値。
   *                                    valid.xが1のときvalue.xにはきちんとした値が入っている。0のとき不定。
   *                                    valid.yが1のときvalue.yにはきちんとした値が入っている。0のとき不定。
   *  @param[out]   valid               値が有効のとき1、無効のとき0。
   *
   *  @retval    1個でも有効な値がある場合はtrueを返す。1個も有効な値がない場合はfalseを返す。
   */
  //-----------------------------------------------------------------------------
  b32 GetVector2Value(const s32 anim_element_index, const f32 frame, gfl::math::VEC2* value, BITVEC2* valid) const;
  b32 GetVector2Value(const s32 anim_element_index, const s32 frame, gfl::math::VEC2* value, BITVEC2* valid) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_VECTOR3
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     指定したフレームにおける値を得る
   *
   *  @param[in]    anim_element_index  インデックス(0<= <GetAnimElementNum())
   *  @param[in]    frame               フレーム
   *  @param[out]   value               値。
   *                                    valid.xが1のときvalue.xにはきちんとした値が入っている。0のとき不定。
   *                                    valid.yが1のときvalue.yにはきちんとした値が入っている。0のとき不定。
   *                                    valid.zが1のときvalue.zにはきちんとした値が入っている。0のとき不定。
   *  @param[out]   valid               値が有効のとき1、無効のとき0。
   *
   *  @retval    1個でも有効な値がある場合はtrueを返す。1個も有効な値がない場合はfalseを返す。
   */
  //-----------------------------------------------------------------------------
  b32 GetVector3Value(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;
  b32 GetVector3Value(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_TRANSFORM
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     指定したフレームにおける値を得る
   *
   *  @param[in]    anim_element_index  インデックス(0<= <GetAnimElementNum())
   *  @param[in]    frame               フレーム
   *  @param[out]   value               値。
   *                                    valid.xが1のときvalue.xにはきちんとした値が入っている。0のとき不定。
   *                                    valid.yが1のときvalue.yにはきちんとした値が入っている。0のとき不定。
   *                                    valid.zが1のときvalue.zにはきちんとした値が入っている。0のとき不定。
   *  @param[out]   valid               値が有効のとき1、無効のとき0。
   *
   *  @retval    1個でも有効な値がある場合はtrueを返す。1個も有効な値がない場合はfalseを返す。
   */
  //-----------------------------------------------------------------------------
  b32 GetTransformScaleValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;
  b32 GetTransformScaleValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;

  b32 GetTransformRotateValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;
  b32 GetTransformRotateValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;

  b32 GetTransformTranslateValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;
  b32 GetTransformTranslateValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_RGBA_COLOR
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     指定したフレームにおける値を得る
   *
   *  @param[in]    anim_element_index  インデックス(0<= <GetAnimElementNum())
   *  @param[in]    frame               フレーム
   *  @param[out]   value               値。
   *                                    valid.xが1のときvalue.xにはきちんとした値が入っている。0のとき不定。
   *                                    valid.yが1のときvalue.yにはきちんとした値が入っている。0のとき不定。
   *                                    valid.zが1のときvalue.zにはきちんとした値が入っている。0のとき不定。
   *                                    valid.wが1のときvalue.wにはきちんとした値が入っている。0のとき不定。
   *  @param[out]   valid               値が有効のとき1、無効のとき0。
   *
   *  @retval    1個でも有効な値がある場合はtrueを返す。1個も有効な値がない場合はfalseを返す。
   */
  //-----------------------------------------------------------------------------
  b32 GetRgbaColorValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC4* value, BITVEC4* valid) const;
  b32 GetRgbaColorValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC4* value, BITVEC4* valid) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_TEXTURE
  //-------------------------------------
  // なし

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_BAKED_TRANSFORM
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     指定したフレームにおける値を得る
   *
   *  @param[in]    anim_element_index  インデックス(0<= <GetAnimElementNum())
   *  @param[in]    frame               フレーム
   *  @param[out]   value               値
   *
   *  @retval    有効な値の場合はtrueを返す。有効な値でない場合はfalseを返す。
   */
  //-----------------------------------------------------------------------------
  b32 GetBakedTransformScaleValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value) const;
  b32 GetBakedTransformScaleValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value) const;

  b32 GetBakedTransformRotateValue(const s32 anim_element_index, const f32 frame, gfl::math::MTX34* value) const;
  b32 GetBakedTransformRotateValue(const s32 anim_element_index, const s32 frame, gfl::math::MTX34* value) const;

  b32 GetBakedTransformTranslateValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value) const;
  b32 GetBakedTransformTranslateValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_BAKED_BOOL
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     指定したフレームにおける値を得る
   *
   *  @param[in]    anim_element_index  インデックス(0<= <GetAnimElementNum())
   *  @param[in]    frame               フレーム
   *  @param[out]   value               値
   *
   *  @retval    有効な値の場合はtrueを返す。有効な値でない場合はfalseを返す。
   */
  //-----------------------------------------------------------------------------
  b32 GetBakedBoolValue(const s32 anim_element_index, const f32 frame, b32* value) const;
  b32 GetBakedBoolValue(const s32 anim_element_index, const s32 frame, b32* value) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_FULL_BAKED_TRANSFORM
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     指定したフレームにおける値を得る
   *
   *  @param[in]    anim_element_index  インデックス(0<= <GetAnimElementNum())
   *  @param[in]    frame               フレーム
   *  @param[out]   value               値
   *
   *  @retval    有効な値の場合はtrueを返す。有効な値でない場合はfalseを返す。
   */
  //-----------------------------------------------------------------------------
  b32 GetFullBakedTransformValue(const s32 anim_element_index, const f32 frame, gfl::math::MTX34* value) const;
  b32 GetFullBakedTransformValue(const s32 anim_element_index, const s32 frame, gfl::math::MTX34* value) const;


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friendには公開している非公開メンバ
  // 派生クラスがアクセスするメンバ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
protected:
  const nw::h3d::res::AnimContent* GetNwAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::AnimContent*>(m_cont));
  }
  const nw::h3d::res::SkeletalAnimContent* GetNwSkeletalAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::SkeletalAnimContent*>(m_cont));
  }
  const nw::h3d::res::MaterialAnimContent* GetNwMaterialAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::MaterialAnimContent*>(m_cont));
  }
  const nw::h3d::res::VisibilityAnimContent* GetNwVisibilityAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::VisibilityAnimContent*>(m_cont));
  }
  const nw::h3d::res::LightAnimContent* GetNwLightAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::LightAnimContent*>(m_cont));
  }
  const nw::h3d::res::CameraAnimContent* GetNwCameraAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::CameraAnimContent*>(m_cont));
  }
  const nw::h3d::res::FogAnimContent* GetNwFogAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::FogAnimContent*>(m_cont));
  }



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 派生クラスがアクセスするメンバ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
protected:
  nw::h3d::res::AnimContent*  m_cont;  // m_res内のコンテント



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 内部で使うメンバ
  // friendは使用禁止
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief     指定したフレームにおける値を得る
   *
   *  DIM                FLOAT      = 1
   *                     VECTOR2    = 2
   *                     VECTOR3    = 3
   *                     RGBA_COLOR = 4
   *
   *  @param[in]    anim_element_index  インデックス(0<= <GetAnimElementNum())
   *  @param[in]    frame               フレーム
   *  @param[out]   value               値。f32 value[DIM]を渡すこと。
   *                                    valid[i]がtrueのときvalue[i]にはきちんとした値が入っている。
   *                                    valid[i]がfalseのときvalue[i]は不定。
   *  @param[out]   valid               有効な値のときtrue、無効な値のときfalse。b32 valid[DIM]を渡すこと。
   */
  //-----------------------------------------------------------------------------
  template<s32 DIM>
  void getFloatVectorValue(const s32 anim_element_index, const f32 frame, f32* value, b32* valid) const
  {
    // f32 value[DIM];
    // b32 valid[DIM];


    // nw::h3d::res::AnimFloatVectorElementData
    // 
    // NintendoWare/CTR/include/nw/h3d/res/h3d_AnimFloatVectorElementData.h
    //
    // typedef AnimFloatVectorElementData<1> AnimFloatElementData;
    // typedef AnimFloatVectorElementData<2> AnimVector2ElementData;
    // typedef AnimFloatVectorElementData<3> AnimVector3ElementData;
    // typedef AnimFloatVectorElementData<4> AnimColorElementData;
    //
    // NintendoWare/CTR/sources/libraries/h3d/h3d_AnimResourceImpl.h
    //
    // template<int Dim>
    // template<class Context>
    // bit32 AnimFloatVectorElementData<Dim>::CalcAndApply(f32* values, Context context) const
 
    if( 0<=anim_element_index && anim_element_index<GetAnimElementNum() )
    {
      nw::h3d::res::AnimFloatVectorElementData<DIM>* element = static_cast<nw::h3d::res::AnimFloatVectorElementData<DIM>*>(m_cont->animElements[anim_element_index]);
    
      for(s32 i=0; i<DIM; ++i)
      {
        if( !(element->flags & (nw::h3d::AnimFloatVectorElementFlag::NOT_EXIST_0 << i)) )
        {
          value[i] = (element->flags & (nw::h3d::AnimFloatVectorElementFlag::CONSTANT_0 << i)) ?
                  element->component[i].constant : element->component[i].curve->Calc(frame);
          valid[i] = true;
        }
        else
        {
          valid[i] = false;
        }
      }
    }
    else
    {
      GFL_ASSERT_MSG(0, "anim_element_index = %d\n", anim_element_index);
      for(s32 i=0; i<DIM; ++i)
      {
        valid[i] = false;
      }
    }
  }


  //-----------------------------------------------------------------------------
  /**
   *  @brief     指定したフレームにおける値を得る
   *
   *  name               scale
   *                     rotate
   *                     translate
   *
   *  @param[in]    anim_element_index  インデックス(0<= <GetAnimElementNum())
   *  @param[in]    frame               フレーム
   *  @param[out]   value               値。
   *                                    valid.xが1のときvalue.xにはきちんとした値が入っている。0のとき不定。
   *                                    valid.yが1のときvalue.yにはきちんとした値が入っている。0のとき不定。
   *                                    valid.zが1のときvalue.zにはきちんとした値が入っている。0のとき不定。
   *  @param[out]   valid               値が有効のとき1、無効のとき0。
   *
   *  @param[in]    flag_not_exist      nw::h3d::AnimTransformElementFlag::NOT_EXIST_SCALE_Xなど
   *  @param[in]    flag_constant       nw::h3d::AnimTransformElementFlag::CONSTANT_SCALE_Xなど
   *
   *  @retval    1個でも有効な値がある場合はtrueを返す。1個も有効な値がない場合はfalseを返す。
   */
  //-----------------------------------------------------------------------------
  
  // NintendoWare/CTR/sources/libraries/h3d/h3d_AnimResourceImpl.h
  //
  // template <class Context>
  // void AnimTransformElementData::CalcAndApply(math::Transform3* transform3, Context context) const

#define GFL_GRP_G3D_H3D_RES_ANIM_GET_TRANSFORM_VALUE(name) \
  b32 getTransformValue_##name(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid, U32VEC3* flag_not_exist, U32VEC3* flag_constant) const \
  { \
    if( 0<=anim_element_index && anim_element_index<GetAnimElementNum() ) \
    { \
      nw::h3d::res::AnimTransformElementData* element = static_cast<nw::h3d::res::AnimTransformElementData*>(m_cont->animElements[anim_element_index]); \
      if( !(element->flags & flag_not_exist->x) ) \
      { \
        value->x = (element->flags & flag_constant->x) ? \
                element->name##X.constant : element->name##X.curve->Calc(frame); \
        valid->x = 1; \
      } \
      else \
      { \
        valid->x = 0; \
      } \
      if( !(element->flags & flag_not_exist->y) ) \
      { \
        value->y = (element->flags & flag_constant->y) ? \
                element->name##Y.constant : element->name##Y.curve->Calc(frame); \
        valid->y = 1; \
      } \
      else \
      { \
        valid->y = 0; \
      } \
      if( !(element->flags & flag_not_exist->z) ) \
      { \
        value->z = (element->flags & flag_constant->z) ? \
                element->name##Z.constant : element->name##Z.curve->Calc(frame); \
        valid->z = 1; \
      } \
      else \
      { \
        valid->z = 0; \
      } \
    } \
    else \
    { \
      GFL_ASSERT_MSG(0, "anim_element_index = %d\n", anim_element_index); \
      valid->x = 0; \
      valid->y = 0; \
      valid->z = 0; \
    } \
    return (valid->x || valid->y || valid->z)?true:false; \
  }

  GFL_GRP_G3D_H3D_RES_ANIM_GET_TRANSFORM_VALUE(scale)
  GFL_GRP_G3D_H3D_RES_ANIM_GET_TRANSFORM_VALUE(rotate)
  GFL_GRP_G3D_H3D_RES_ANIM_GET_TRANSFORM_VALUE(translate)


};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE



#endif // __GFL_G3DH3DRESANIM_H__

