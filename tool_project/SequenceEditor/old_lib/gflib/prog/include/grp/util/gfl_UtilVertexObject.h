#ifndef __GFL_UTIL_VERTEXOBJECT_H__
#define __GFL_UTIL_VERTEXOBJECT_H__
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
#include <grp/util/gfl_UtilVertex.h>

namespace gfl {
namespace grp {
namespace util {

//! @brief 頂点オブジェクトクラス
class VertexObject
{
  GFL_FORBID_COPY_AND_ASSIGN(VertexObject);	
public:

	VertexObject();
	~VertexObject();

  /**
   * @brief インデックスバッファ生成関数
   * @param size    サイズ
   * @param pData   インデックスデータ(生成時に書き込みたい場合)
   */
	void CreateIndexBuffer( s32 size, u16* pData = NULL );

  /**
   * @brief 頂点バッファ生成関数
   * @param size    サイズ
   * @param pData   頂点データ(生成時に書き込みたい場合)
   */
	void CreateVertexBuffer( s32 size, Vertex* pData = NULL );

  /**
   * @brief インデックスバッファ書き込み関数
   * @param pData   書き込みたいデータ
   * @param size    サイズ
   * @param offset  書き込み開始オフセット
   */
	void WriteIndexBuffer( u16* pData, s32 size, s32 offset = 0 );

  /**
   * @brief 頂点バッファ書き込み関数
   * @param pData   書き込みたい頂点データ
   * @param size    サイズ
   * @param offset  書き込み開始オフセット
   */
	void WriteVertexBuffer( Vertex* pData, s32 size, s32 offset = 0 );

  /**
   * @brief   インデックス数取得関数
   * @return  インデックス数
   */
	inline GLuint GetIndexNum() const { return m_IndexNum; }

  /**
   * @brief   インデックスバッファＩＤ取得関数
   * @return  インデックスバッファＩＤ
   */
	inline GLuint GetIndexBufferId() const { return m_IndexBufferId; }

  /**
   * @brief   頂点バッファＩＤ取得関数
   * @return  頂点バッファＩＤ
   */
	inline GLuint GetVertexBufferId() const { return m_VertexBufferId; }

private:

	s32							m_IndexNum;           //! インデックス数
	GLuint					m_IndexBufferId;      //! インデックスバッファＩＤ
	GLuint					m_VertexBufferId;     //! 頂点バッファＩＤ

};

}  // namespace util
}  // namespace grp
}  // namespace gfl

#endif // __GFL_UTIL_VERTEXOBJECT_H__
