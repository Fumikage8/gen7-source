#ifndef __GFL_UTIL_VERTEX_H__
#define __GFL_UTIL_VERTEX_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_UtilVertexObject.h
 *	@brief  頂点オブジェクトクラス
 *	@author	Masateru Ishiguro
 *	@date		2011.2.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
// gflib
#include <gfl_Macros.h>
#include <math/gfl_MathType.h>
#include <grp/gfl_GraphicsSystem.h>
#include <grp/util/gfl_UtilDrawUtil.h>

namespace gfl {
namespace grp {
namespace util {

//! @brief 頂点クラス
class Vertex
{
  GFL_FORBID_COPY_AND_ASSIGN(Vertex);
  friend class DrawUtil;
public:
	gfl::math::VEC3		m_Position;
	gfl::math::VEC4		m_Color;
	gfl::math::VEC2		m_Uv;
	gfl::math::VEC3		m_Normal;

  ~Vertex(){}
  Vertex(){}
private:
};

}  // namespace util
}  // namespace grp
}  // namespace gfl

#endif // __GFL_UTIL_VERTEXOBJECT_H__
