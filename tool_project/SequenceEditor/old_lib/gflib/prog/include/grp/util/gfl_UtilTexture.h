#ifndef __GFL_UTIL_TEXTURE_H__
#define __GFL_UTIL_TEXTURE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_UtilTexture.h
 *	@brief  �e�N�X�`���N���X
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
	 * @brief	�e�N�X�`���N���X
	 */
	class Texture
	{
    GFL_FORBID_COPY_AND_ASSIGN(Texture);
    friend class DrawUtil;
	public:
		
		~Texture();

    /**
     * @brief   �e�N�X�`���I�u�W�F�N�g���ʎq�擾
     * @return  �e�N�X�`���I�u�W�F�N�g���ʎq
     */
		inline GLuint GetTextureHandle() const;

    /**
     * @brief   �e�N�X�`���^�C�v�擾�֐�
     * @return  �e�N�X�`���^�C�v
     */
		inline GLenum GetTextureType() const;

    /**
     * @brief   �e�N�X�`���̕��擾�֐�
     * @return  �e�N�X�`���̕�
     */
		inline u32 GetWidth() const { return m_Width; }

    /**
     * @brief   �e�N�X�`���̍����擾�֐�
     * @return  �e�N�X�`���̍���
     */
		inline u32 GetHeight() const { return m_Height; }

    /**
     * @brief �e�N�X�`���A�h���X�擾�֐�
     * @return  �e�N�X�`���A�h���X
     */
    inline GLuint GetTextureAddr() const;
    
    /**
     * @brief �e�N�X�`�����z�A�h���X�擾�֐�
     * @return  �e�N�X�`�����z�A�h���X
     */
    inline GLuint GetTextureVirtualAddr() const;
    
    /**
     * @brief �e�N�X�`���t�H�[�}�b�g�擾�֐�
     * @return  �e�N�X�`���t�H�[�}�b�g
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

