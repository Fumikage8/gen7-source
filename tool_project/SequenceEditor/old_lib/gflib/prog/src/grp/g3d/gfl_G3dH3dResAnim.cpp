//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dResAnim.cpp
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
#include <grp/g3d/gfl_G3dH3dResAnim.h>
#include <grp/g3d/gfl_G3dH3dUtil.h>
#include <grp/g3d/gfl_G3dH3dUserData.h>



namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					クラス実装
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3Dアニメーションコンテントクラス
//=====================================
H3dResAnim::H3dResAnim(void)
  : H3dContent(),
    m_cont(NULL)
{
  STATIC_ASSERT( ANIM_PRIMITIVE_TYPE_FLOAT                != ANIM_PRIMITIVE_TYPE_INVALID );
  STATIC_ASSERT( ANIM_PRIMITIVE_TYPE_INT                  != ANIM_PRIMITIVE_TYPE_INVALID );
  STATIC_ASSERT( ANIM_PRIMITIVE_TYPE_VECTOR2              != ANIM_PRIMITIVE_TYPE_INVALID );
  STATIC_ASSERT( ANIM_PRIMITIVE_TYPE_VECTOR3              != ANIM_PRIMITIVE_TYPE_INVALID );
  STATIC_ASSERT( ANIM_PRIMITIVE_TYPE_TRANSFORM            != ANIM_PRIMITIVE_TYPE_INVALID );
  STATIC_ASSERT( ANIM_PRIMITIVE_TYPE_RGBA_COLOR           != ANIM_PRIMITIVE_TYPE_INVALID );
  STATIC_ASSERT( ANIM_PRIMITIVE_TYPE_TEXTURE              != ANIM_PRIMITIVE_TYPE_INVALID );
  STATIC_ASSERT( ANIM_PRIMITIVE_TYPE_BAKED_TRANSFORM      != ANIM_PRIMITIVE_TYPE_INVALID );
  STATIC_ASSERT( ANIM_PRIMITIVE_TYPE_BAKED_BOOL           != ANIM_PRIMITIVE_TYPE_INVALID );
  STATIC_ASSERT( ANIM_PRIMITIVE_TYPE_FULL_BAKED_TRANSFORM != ANIM_PRIMITIVE_TYPE_INVALID );
}

H3dResAnim::~H3dResAnim()
{
  Destroy();
}

AnimationType H3dResAnim::GetAnimationType(void) const
{
  AnimationType type = ANIMATION_TYPE_NONE;
  switch(m_cont->animType)
  {
  case nw::h3d::AnimType::SKELETAL:   type = ANIMATION_TYPE_SKELETAL;   break;
  case nw::h3d::AnimType::MATERIAL:   type = ANIMATION_TYPE_MATERIAL;   break;
  case nw::h3d::AnimType::VISIBILITY: type = ANIMATION_TYPE_VISIBILITY; break;
  case nw::h3d::AnimType::LIGHT:      type = ANIMATION_TYPE_LIGHT;      break;
  case nw::h3d::AnimType::CAMERA:     type = ANIMATION_TYPE_CAMERA;     break;
  case nw::h3d::AnimType::FOG:        type = ANIMATION_TYPE_FOG;        break;
  }
  return type;
}



s32 H3dResAnim::GetUserDataCount() const
{
  return H3dMetaDataMapUtil::GetMetaDataNum(getMetaDataMap());
}
s32 H3dResAnim::GetUserDataIndex(const char* name) const
{
  STATIC_ASSERT(INDEX_NONE==H3dMetaDataMapUtil::INDEX_NONE);
  return H3dMetaDataMapUtil::GetMetaDataIndex(getMetaDataMap(), name);
}
void H3dResAnim::GetUserData(const s32 user_data_index, H3dUserData* user_data) const
{
  user_data->SetNwMetaData( &( H3dMetaDataMapUtil::GetMetaData(getMetaDataMap(), user_data_index) ) );
}

nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* H3dResAnim::getMetaDataMap(void) const
{
  return m_cont->metaData;
}



u32 H3dResAnim::GetAnimElementNum(void) const
{
  return m_cont->GetAnimElementCount();
}

f32 H3dResAnim::GetFrameSize(void) const
{
  return m_cont->frameLength;
}

f32 H3dResAnim::NormalizeFrame(f32 frame) const
{
  return m_cont->NormalizeFrame(frame);
}

b32 H3dResAnim::IsLoop(void) const
{
  if( m_cont->GetLoopMode() == nw::h3d::AnimLoopMode::LOOP ) return true;
  return false;
}

void H3dResAnim::SetLoop(b32 loop)
{
  if( loop )
  {
    m_cont->flags |= nw::h3d::AnimContentFlag::LOOP;
  }
  else
  {
    m_cont->flags &= ~nw::h3d::AnimContentFlag::LOOP;
  }
}

const char* H3dResAnim::GetTargetName(const s32 anim_element_index) const
{
  if( 0<=anim_element_index && anim_element_index<GetAnimElementNum() )
  {
    return m_cont->animElements[anim_element_index]->targetName;
  }
  else
  {
    GFL_ASSERT_MSG(0, "anim_element_index = %d\n", anim_element_index);
    return NULL;
  }
}
s32 H3dResAnim::GetAnimElementIndex(const char* target_name) const
{
  s32 index = INDEX_NONE;
  if( target_name )
  {
    u32 num = GetAnimElementNum();
    for(s32 i=0; i<num; ++i)
    {
      if( m_cont->animElements[i]->targetName )
      {
        if( strcmp(m_cont->animElements[i]->targetName, target_name)==0 )
        {
          index = i;
          break;
        }
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "target_name = NULL\n");
  }
  return index;
}
H3dResAnim::AnimPrimitiveType H3dResAnim::GetAnimPrimitiveType(const s32 anim_element_index) const
{
  if( 0<=anim_element_index && anim_element_index<GetAnimElementNum() )
  {
    return static_cast<AnimPrimitiveType>(m_cont->animElements[anim_element_index]->primitiveType);
  }
  else
  {
    GFL_ASSERT_MSG(0, "anim_element_index = %d\n", anim_element_index);
    return ANIM_PRIMITIVE_TYPE_INVALID;
  }
}

b32 H3dResAnim::GetFloatValue(const s32 anim_element_index, const f32 frame, f32* value) const
{
  enum
  {
    DIM = 1
  };
  f32 l_value[DIM];
  b32 l_valid[DIM];
  getFloatVectorValue<DIM>(anim_element_index, frame, l_value, l_valid);
  *value = l_value[0];
  return l_valid[0];
}
b32 H3dResAnim::GetFloatValue(const s32 anim_element_index, const s32 frame, f32* value) const
{
  return GetFloatValue(anim_element_index, static_cast<f32>(frame), value);
}

b32 H3dResAnim::GetVector2Value(const s32 anim_element_index, const f32 frame, gfl::math::VEC2* value, BITVEC2* valid) const
{
  enum
  {
    DIM = 2
  };
  f32 l_value[DIM];
  b32 l_valid[DIM];
  getFloatVectorValue<DIM>(anim_element_index, frame, l_value, l_valid);
  value->x = l_value[0];
  value->y = l_value[1];
  valid->x = l_valid[0]?1:0;
  valid->y = l_valid[1]?1:0;
  return (l_valid[0] || l_valid[1]);
}
b32 H3dResAnim::GetVector2Value(const s32 anim_element_index, const s32 frame, gfl::math::VEC2* value, BITVEC2* valid) const
{
  return GetVector2Value(anim_element_index, static_cast<f32>(frame), value, valid);
}

b32 H3dResAnim::GetVector3Value(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid) const
{
  enum
  {
    DIM = 3
  };
  f32 l_value[DIM];
  b32 l_valid[DIM];
  getFloatVectorValue<DIM>(anim_element_index, frame, l_value, l_valid);
  value->x = l_value[0];
  value->y = l_value[1];
  value->z = l_value[2];
  valid->x = l_valid[0]?1:0;
  valid->y = l_valid[1]?1:0;
  valid->z = l_valid[2]?1:0;
  return (l_valid[0] || l_valid[1] || l_valid[2]);
}
b32 H3dResAnim::GetVector3Value(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value, BITVEC3* valid) const
{
  return GetVector3Value(anim_element_index, static_cast<f32>(frame), value, valid);
}


b32 H3dResAnim::GetTransformScaleValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid) const
{
  U32VEC3 flag_not_exist =
  {
    nw::h3d::AnimTransformElementFlag::NOT_EXIST_SCALE_X,
    nw::h3d::AnimTransformElementFlag::NOT_EXIST_SCALE_Y,
    nw::h3d::AnimTransformElementFlag::NOT_EXIST_SCALE_Z
  };
  U32VEC3 flag_constant =
  {
    nw::h3d::AnimTransformElementFlag::CONSTANT_SCALE_X,
    nw::h3d::AnimTransformElementFlag::CONSTANT_SCALE_Y,
    nw::h3d::AnimTransformElementFlag::CONSTANT_SCALE_Z
  };
  return getTransformValue_scale(anim_element_index, frame, value, valid, &flag_not_exist, &flag_constant);
}
b32 H3dResAnim::GetTransformScaleValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value, BITVEC3* valid) const
{
  return GetTransformScaleValue(anim_element_index, static_cast<f32>(frame), value, valid);
}

b32 H3dResAnim::GetTransformRotateValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid) const
{
  U32VEC3 flag_not_exist =
  {
    nw::h3d::AnimTransformElementFlag::NOT_EXIST_ROTATE_X,
    nw::h3d::AnimTransformElementFlag::NOT_EXIST_ROTATE_Y,
    nw::h3d::AnimTransformElementFlag::NOT_EXIST_ROTATE_Z
  };
  U32VEC3 flag_constant =
  {
    nw::h3d::AnimTransformElementFlag::CONSTANT_ROTATE_X,
    nw::h3d::AnimTransformElementFlag::CONSTANT_ROTATE_Y,
    nw::h3d::AnimTransformElementFlag::CONSTANT_ROTATE_Z
  };
  return getTransformValue_rotate(anim_element_index, frame, value, valid, &flag_not_exist, &flag_constant);
}
b32 H3dResAnim::GetTransformRotateValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value, BITVEC3* valid) const
{
  return GetTransformRotateValue(anim_element_index, static_cast<f32>(frame), value, valid);
}

b32 H3dResAnim::GetTransformTranslateValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid) const
{
  U32VEC3 flag_not_exist =
  {
    nw::h3d::AnimTransformElementFlag::NOT_EXIST_TRANSLATE_X,
    nw::h3d::AnimTransformElementFlag::NOT_EXIST_TRANSLATE_Y,
    nw::h3d::AnimTransformElementFlag::NOT_EXIST_TRANSLATE_Z
  };
  U32VEC3 flag_constant =
  {
    nw::h3d::AnimTransformElementFlag::CONSTANT_TRANSLATE_X,
    nw::h3d::AnimTransformElementFlag::CONSTANT_TRANSLATE_Y,
    nw::h3d::AnimTransformElementFlag::CONSTANT_TRANSLATE_Z
  };
  return getTransformValue_translate(anim_element_index, frame, value, valid, &flag_not_exist, &flag_constant);
}
b32 H3dResAnim::GetTransformTranslateValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value, BITVEC3* valid) const
{
  return GetTransformTranslateValue(anim_element_index, static_cast<f32>(frame), value, valid);
}

b32 H3dResAnim::GetRgbaColorValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC4* value, BITVEC4* valid) const
{
  enum
  {
    DIM = 4
  };
  f32 l_value[DIM];
  b32 l_valid[DIM];
  getFloatVectorValue<DIM>(anim_element_index, frame, l_value, l_valid);
  value->x = l_value[0];
  value->y = l_value[1];
  value->z = l_value[2];
  value->w = l_value[3];
  valid->x = l_valid[0]?true:false;
  valid->y = l_valid[1]?true:false;
  valid->z = l_valid[2]?true:false;
  valid->w = l_valid[3]?true:false;
  return (l_valid[0] || l_valid[1] || l_valid[2] || l_valid[3])?true:false;
}
b32 H3dResAnim::GetRgbaColorValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC4* value, BITVEC4* valid) const
{
  return GetRgbaColorValue(anim_element_index, static_cast<f32>(frame), value, valid);
}


b32 H3dResAnim::GetBakedTransformScaleValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value) const
{
  // NintendoWare/CTR/sources/libraries/h3d/h3d_SkeletalAnimResourceImpl.h
  // void AnimBakedTransformElementData::CalcAndApply( LocalMtx* targetLocalMtx, f32 frame ) const

  b32 ret = false;
  if( 0<=anim_element_index && anim_element_index<GetAnimElementNum() )
  {
    nw::h3d::res::AnimBakedTransformElementData* element = static_cast<nw::h3d::res::AnimBakedTransformElementData*>(m_cont->animElements[anim_element_index]);
    bit32 dummy;

    if (!(element->flags & nw::h3d::AnimBakedTransformElementFlag::NOT_EXIST_SCALE))
    {
      //element->scale->Calc(value, &dummy, frame);
      element->scale.curve->Calc(value, frame);
      ret = true;
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "anim_element_index = %d\n", anim_element_index);
  }
  return ret;
}
b32 H3dResAnim::GetBakedTransformScaleValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value) const
{
  return GetBakedTransformScaleValue(anim_element_index, static_cast<f32>(frame), value);
}

b32 H3dResAnim::GetBakedTransformRotateValue(const s32 anim_element_index, const f32 frame, gfl::math::MTX34* value) const
{
  b32 ret = false;
  if( 0<=anim_element_index && anim_element_index<GetAnimElementNum() )
  {
    nw::h3d::res::AnimBakedTransformElementData* element = static_cast<nw::h3d::res::AnimBakedTransformElementData*>(m_cont->animElements[anim_element_index]);
    bit32 dummy;

    if (!(element->flags & nw::h3d::AnimBakedTransformElementFlag::NOT_EXIST_ROTATE))
    {
      nn::math::QUAT quat;
      //element->rotate->Calc(&quat, &dummy, frame);
      element->rotate.curve->Calc(&quat, frame);
      nn::math::QUATToMTX34(value, &quat);
      ret = true;
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "anim_element_index = %d\n", anim_element_index);
  }
  return ret;
}
b32 H3dResAnim::GetBakedTransformRotateValue(const s32 anim_element_index, const s32 frame, gfl::math::MTX34* value) const
{
  return GetBakedTransformRotateValue(anim_element_index, static_cast<f32>(frame), value);
}

b32 H3dResAnim::GetBakedTransformTranslateValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value) const
{
  b32 ret = false;
  if( 0<=anim_element_index && anim_element_index<GetAnimElementNum() )
  {
    nw::h3d::res::AnimBakedTransformElementData* element = static_cast<nw::h3d::res::AnimBakedTransformElementData*>(m_cont->animElements[anim_element_index]);
    bit32 dummy;

    if (!(element->flags & nw::h3d::AnimBakedTransformElementFlag::NOT_EXIST_TRANSLATE))
    {
      //element->translate->Calc(value, &dummy, frame);
      element->translate.curve->Calc(value, frame);
      ret = true;
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "anim_element_index = %d\n", anim_element_index);
  }
  return ret;
}
b32 H3dResAnim::GetBakedTransformTranslateValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value) const
{
  return GetBakedTransformTranslateValue(anim_element_index, static_cast<f32>(frame), value);
}


b32 H3dResAnim::GetBakedBoolValue(const s32 anim_element_index, const f32 frame, b32* value) const
{
  // NintendoWare/CTR/sources/libraries/h3d/h3d_AnimResourceImpl.h
  // void AnimBakedBoolElementData::CalcAndApplyImpl( bool* value, f32 frame ) const

  b32 ret = false;
  if( 0<=anim_element_index && anim_element_index<GetAnimElementNum() )
  {
    nw::h3d::res::AnimBakedBoolElementData* element = static_cast<nw::h3d::res::AnimBakedBoolElementData*>(m_cont->animElements[anim_element_index]);

    *value = element->curve.Calc(frame);
    ret = true;
  }
  else
  {
    GFL_ASSERT_MSG(0, "anim_element_index = %d\n", anim_element_index);
  }
  return ret;
}
b32 H3dResAnim::GetBakedBoolValue(const s32 anim_element_index, const s32 frame, b32* value) const
{
  return GetBakedBoolValue(anim_element_index, static_cast<f32>(frame), value);
}


b32 H3dResAnim::GetFullBakedTransformValue(const s32 anim_element_index, const f32 frame, gfl::math::MTX34* value) const
{
  // NintendoWare/CTR/sources/libraries/h3d/h3d_AnimResourceImpl.h
  // void AnimFullBakedTransformElementData::CalcAndApplyImpl( math::MTX34* value, f32 frame ) const
  
  b32 ret = false;
  if( 0<=anim_element_index && anim_element_index<GetAnimElementNum() )
  {
    nw::h3d::res::AnimFullBakedTransformElementData* element = static_cast<nw::h3d::res::AnimFullBakedTransformElementData*>(m_cont->animElements[anim_element_index]);

    element->transform.Calc(value, frame);
    ret = true;
  }
  else
  {
    GFL_ASSERT_MSG(0, "anim_element_index = %d\n", anim_element_index);
  }
  return ret;
}
b32 H3dResAnim::GetFullBakedTransformValue(const s32 anim_element_index, const s32 frame, gfl::math::MTX34* value) const
{
  return GetFullBakedTransformValue(anim_element_index, static_cast<f32>(frame), value);
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

