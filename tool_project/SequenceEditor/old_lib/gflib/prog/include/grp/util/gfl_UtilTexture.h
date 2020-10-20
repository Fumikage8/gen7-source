#ifndef __GFL_UTIL_TEXTURE_H__
#define __GFL_UTIL_TEXTURE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_UtilTexture.h
 *	@brief  テクスチャクラス
 *	@author	Masateru Ishiguro
 *	@date		2011.2.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
// gflib
#include <gfl_Macros.h>
#include <math/gfl_MathType.h>
#include <grp/util/gfl_UtilDrawUtil.h>
#include <grp/gfl_CommandListManager.h>

#include <nn/os.h>
#include <nn/gx.h>

namespace gfl {
namespace grp {
namespace util {

	//-----------------------------------------------------------------------------
	/**
	 * @class	Texture
	 * @brief	テクスチャクラス
	 */
	class Texture
	{
    GFL_FORBID_COPY_AND_ASSIGN(Texture);
    friend class DrawUtil;
	public:
		
		~Texture();

    /**
     * @brief   テクスチャオブジェクト識別子取得
     * @return  テクスチャオブジェクト識別子
     */
		inline GLuint GetTextureHandle() const;

    /**
     * @brief   テクスチャタイプ取得関数
     * @return  テクスチャタイプ
     */
		inline GLenum GetTextureType() const;

    /**
     * @brief   テクスチャの幅取得関数
     * @return  テクスチャの幅
     */
		inline u32 GetWidth() const { return m_Width; }

    /**
     * @brief   テクスチャの高さ取得関数
     * @return  テクスチャの高さ
     */
		inline u32 GetHeight() const { return m_Height; }

    /**
     * @brief テクスチャアドレス取得関数
     * @return  テクスチャアドレス
     */
    inline GLuint GetTextureAddr() const;
    
    /**
     * @brief テクスチャ仮想アドレス取得関数
     * @return  テクスチャ仮想アドレス
     */
    inline GLuint GetTextureVirtualAddr() const;
    
    /**
     * @brief テクスチャフォーマット取得関数
     * @return  テクスチャフォーマット
     */
    inline GLuint GetTextureFormat() const;

	private:

		Texture();

    gfl::grp::CommandListManager*     m_pContext;
		GLuint		m_hTexture;
    GLint     m_TextureAddr;
    GLint     m_TextureVirtualAddr;
		u32       m_Width;
		u32       m_Height;
		GLenum		m_Format;
		GLenum		m_Type;
	};

	//-----------------------------------------------------------------------------
	/// @brief	
	//-----------------------------------------------------------------------------
	inline GLuint Texture::GetTextureHandle() const
	{
		return m_hTexture;
	}

	//-----------------------------------------------------------------------------
	/// @brief	
	//-----------------------------------------------------------------------------
	inline GLenum Texture::GetTextureType() const
	{
		return m_Type;
	}

  //-----------------------------------------------------------------------------
	/// @brief
	//-----------------------------------------------------------------------------
	inline GLuint Texture::GetTextureAddr() const
	{
		return m_TextureAddr;
	}
  
  //-----------------------------------------------------------------------------
	/// @brief
	//-----------------------------------------------------------------------------
	inline GLuint Texture::GetTextureVirtualAddr() const
	{
		return m_TextureVirtualAddr;
	}
  
  
  //-----------------------------------------------------------------------------
	/// @brief
	//-----------------------------------------------------------------------------
  inline GLuint Texture::GetTextureFormat() const
  {
    return m_Format;
  }

}}}

#endif

