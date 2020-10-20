#ifndef __GFL_UTIL_PIXELSHADER_H__
#define __GFL_UTIL_PIXELSHADER_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_UtilPixelShader.h
 *	@brief  �V�F�[�_�[�N���X
 *	@author	Masateru Ishiguro
 *	@date		2011.2.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <nn/os.h>
#include <nn/gx.h>
#include "gfl_UtilShader.h"

namespace gfl { namespace grp { namespace util {


	//-----------------------------------------------------------------------------
	/**
	 * @class	VertexColorPixelShader
	 * @brief	���_�J���[�V�F�[�_�[�N���X
	 */
	class VertexColorPixelShader : public IShader
	{
	public:

		VertexColorPixelShader(){}
		virtual ~VertexColorPixelShader(){}

		virtual void vSetLinkProgram( GLuint program_object_id );
		virtual GLuint vGetObjectID() const;
		virtual void vSetUpShader() const;

	private:
		/*!
		:private

		@brief �V�F�[�_��Uniform�̃��P�[�V������ۑ�����z��̃C���f�b�N�X�p�̗񋓑�
		*/

		enum
		{
			// Fragment uniform
			PS_ALPHA_TEST,
			PS_ALPHA_REF,
			PS_ALPHA_FUNC,
			// Fragment uniform : Texture samplerType
			PS_TEXTURE0_SAMPLER_TYPE,
			// Fragment uniform : Fragment lighting
			PS_LIGHTING_ENABLED,
			PS_LIGHTING_AMBIENT,
			// Fragment uniform : Fragment light0
			PS_LIGHT_SOURCE0_ENABLED,
			PS_LIGHT_SOURCE0_AMBIENT,
			PS_LIGHT_SOURCE0_DIFFUSE,
			PS_LIGHT_SOURCE0_POSITION,

			// Fragment uniform : Material
			PS_MATERIAL_AMBIENT,
			PS_MATERIAL_DIFFUSE,
			// Fragment uniform : Texture combiner0
			PS_TEXENV0_SRC_RGB,
			PS_TEXENV0_SRC_ALPHA,
			PS_TEXENV0_OPERAND_RGB,
			PS_TEXENV0_OPERAND_ALPHA,
			PS_TEXENV0_COMBINE_RGB,
			PS_TEXENV0_COMBINE_ALPHA,
      PS_TEXENV0_SRC_CONST,
			// Max uniform locations num
			MAX_UNIFORM_LOCATIONS_NUM
		};

		GLuint			m_ParentProgramObjectID;
		s32				m_UniformLocations[MAX_UNIFORM_LOCATIONS_NUM];
	};

	//-----------------------------------------------------------------------------
	/**
	 * @class	TextureMapPixelShader
	 * @brief	�e�N�X�`���}�b�v�V�F�[�_�[�N���X
	 */
	class TextureMapPixelShader : public IShader
	{
	public:

		TextureMapPixelShader(){}
		virtual ~TextureMapPixelShader(){}

		virtual void vSetLinkProgram( GLuint program_object_id );
		virtual GLuint vGetObjectID() const;
		virtual void vSetUpShader() const;

	private:
		/*!
		:private

		@brief �V�F�[�_��Uniform�̃��P�[�V������ۑ�����z��̃C���f�b�N�X�p�̗񋓑�
		*/
	    
		enum
		{
			// Fragment uniform
			PS_ALPHA_TEST,
			PS_ALPHA_REF,
			PS_ALPHA_FUNC,
			// Fragment uniform : Texture samplerType
			PS_TEXTURE0_SAMPLER_TYPE,
			// Fragment uniform : Fragment lighting
			PS_LIGHTING_ENABLED,
			PS_LIGHTING_AMBIENT,
			// Fragment uniform : Fragment light0
			PS_LIGHT_SOURCE0_ENABLED,
			PS_LIGHT_SOURCE0_AMBIENT,
			PS_LIGHT_SOURCE0_DIFFUSE,
			PS_LIGHT_SOURCE0_POSITION,

			// Fragment uniform : Material
			PS_MATERIAL_AMBIENT,
			PS_MATERIAL_DIFFUSE,
			// Fragment uniform : Texture combiner0
			PS_TEXENV0_SRC_RGB,
			PS_TEXENV0_SRC_ALPHA,
			PS_TEXENV0_OPERAND_RGB,
			PS_TEXENV0_OPERAND_ALPHA,
			PS_TEXENV0_COMBINE_RGB,
			PS_TEXENV0_COMBINE_ALPHA,
			// Max uniform locations num
			MAX_UNIFORM_LOCATIONS_NUM
		};

		GLuint			m_ParentProgramObjectID;
		s32				m_UniformLocations[MAX_UNIFORM_LOCATIONS_NUM];
	};
  
  //-----------------------------------------------------------------------------
	/**
	 * @class	RgbTextureMapPixelShader
	 * @brief	�e�N�X�`���}�b�v�V�F�[�_�[�N���X
	 */
	class RgbTextureMapPixelShader : public IShader
	{
	public:

		RgbTextureMapPixelShader(){}
		virtual ~RgbTextureMapPixelShader(){}

		virtual void vSetLinkProgram( GLuint program_object_id );
		virtual GLuint vGetObjectID() const;
		virtual void vSetUpShader() const;

	private:
		/*!
		:private

		@brief �V�F�[�_��Uniform�̃��P�[�V������ۑ�����z��̃C���f�b�N�X�p�̗񋓑�
		*/
	    
		enum
		{
			// Fragment uniform
			PS_ALPHA_TEST,
			PS_ALPHA_REF,
			PS_ALPHA_FUNC,
			// Fragment uniform : Texture samplerType
			PS_TEXTURE0_SAMPLER_TYPE,
			// Fragment uniform : Fragment lighting
			PS_LIGHTING_ENABLED,
			PS_LIGHTING_AMBIENT,
			// Fragment uniform : Fragment light0
			PS_LIGHT_SOURCE0_ENABLED,
			PS_LIGHT_SOURCE0_AMBIENT,
			PS_LIGHT_SOURCE0_DIFFUSE,
			PS_LIGHT_SOURCE0_POSITION,

			// Fragment uniform : Material
			PS_MATERIAL_AMBIENT,
			PS_MATERIAL_DIFFUSE,
			// Fragment uniform : Texture combiner0
			PS_TEXENV0_SRC_RGB,
			PS_TEXENV0_SRC_ALPHA,
			PS_TEXENV0_OPERAND_RGB,
			PS_TEXENV0_OPERAND_ALPHA,
			PS_TEXENV0_COMBINE_RGB,
			PS_TEXENV0_COMBINE_ALPHA,
			// Max uniform locations num
			MAX_UNIFORM_LOCATIONS_NUM
		};

		GLuint			m_ParentProgramObjectID;
		s32				m_UniformLocations[MAX_UNIFORM_LOCATIONS_NUM];
	};
  
  //-----------------------------------------------------------------------------
	/**
	 * @class   FrontDofTextureMapPixelShader
	 * @brief   ��ʊE�[�x�p�e�N�X�`���}�b�v�V�F�[�_�[�N���X
	 */
	class FrontDofTextureMapPixelShader : public IShader
	{
	public:

		FrontDofTextureMapPixelShader(){}
		virtual ~FrontDofTextureMapPixelShader(){}

		virtual void vSetLinkProgram( GLuint program_object_id );
		virtual GLuint vGetObjectID() const;
		virtual void vSetUpShader() const;

	private:
		/*!
		:private

		@brief �V�F�[�_��Uniform�̃��P�[�V������ۑ�����z��̃C���f�b�N�X�p�̗񋓑�
		*/
	    
		enum
		{
			// Fragment uniform
			PS_ALPHA_TEST,
			PS_ALPHA_REF,
			PS_ALPHA_FUNC,
			// Fragment uniform : Texture samplerType
			PS_TEXTURE0_SAMPLER_TYPE,
			// Fragment uniform : Fragment lighting
			PS_LIGHTING_ENABLED,
			PS_LIGHTING_AMBIENT,
			// Fragment uniform : Fragment light0
			PS_LIGHT_SOURCE0_ENABLED,
			PS_LIGHT_SOURCE0_AMBIENT,
			PS_LIGHT_SOURCE0_DIFFUSE,
			PS_LIGHT_SOURCE0_POSITION,

			// Fragment uniform : Material
			PS_MATERIAL_AMBIENT,
			PS_MATERIAL_DIFFUSE,
			// Fragment uniform : Texture combiner0
			PS_TEXENV0_SRC_RGB,
			PS_TEXENV0_SRC_ALPHA,
			PS_TEXENV0_OPERAND_RGB,
			PS_TEXENV0_OPERAND_ALPHA,
			PS_TEXENV0_COMBINE_RGB,
			PS_TEXENV0_COMBINE_ALPHA,
			// Max uniform locations num
			MAX_UNIFORM_LOCATIONS_NUM
		};

		GLuint			m_ParentProgramObjectID;
		s32				m_UniformLocations[MAX_UNIFORM_LOCATIONS_NUM];
	};
  
  //-----------------------------------------------------------------------------
	/**
	 * @class   BackDofTextureMapPixelShader
	 * @brief   ��ʊE�[�x�p�e�N�X�`���}�b�v�V�F�[�_�[�N���X
	 */
	class BackDofTextureMapPixelShader : public IShader
	{
	public:

		BackDofTextureMapPixelShader(){}
		virtual ~BackDofTextureMapPixelShader(){}

		virtual void vSetLinkProgram( GLuint program_object_id );
		virtual GLuint vGetObjectID() const;
		virtual void vSetUpShader() const;

	private:
		/*!
		:private

		@brief �V�F�[�_��Uniform�̃��P�[�V������ۑ�����z��̃C���f�b�N�X�p�̗񋓑�
		*/
	    
		enum
		{
			// Fragment uniform
			PS_ALPHA_TEST,
			PS_ALPHA_REF,
			PS_ALPHA_FUNC,
			// Fragment uniform : Texture samplerType
			PS_TEXTURE0_SAMPLER_TYPE,
			// Fragment uniform : Fragment lighting
			PS_LIGHTING_ENABLED,
			PS_LIGHTING_AMBIENT,
			// Fragment uniform : Fragment light0
			PS_LIGHT_SOURCE0_ENABLED,
			PS_LIGHT_SOURCE0_AMBIENT,
			PS_LIGHT_SOURCE0_DIFFUSE,
			PS_LIGHT_SOURCE0_POSITION,

			// Fragment uniform : Material
			PS_MATERIAL_AMBIENT,
			PS_MATERIAL_DIFFUSE,
			// Fragment uniform : Texture combiner0
			PS_TEXENV0_SRC_RGB,
			PS_TEXENV0_SRC_ALPHA,
			PS_TEXENV0_OPERAND_RGB,
			PS_TEXENV0_OPERAND_ALPHA,
			PS_TEXENV0_COMBINE_RGB,
			PS_TEXENV0_COMBINE_ALPHA,
			// Max uniform locations num
			MAX_UNIFORM_LOCATIONS_NUM
		};

		GLuint			m_ParentProgramObjectID;
		s32				m_UniformLocations[MAX_UNIFORM_LOCATIONS_NUM];
	};
  
  //-----------------------------------------------------------------------------
	/**
	 * @class	
	 * @brief	
	 */
	class DebugPixelShader : public IShader
	{
	public:

		DebugPixelShader(){}
		virtual ~DebugPixelShader(){}

		virtual void vSetLinkProgram( GLuint program_object_id );
		virtual GLuint vGetObjectID() const;
		virtual void vSetUpShader() const;

	private:
		/*!
		:private

		@brief �V�F�[�_��Uniform�̃��P�[�V������ۑ�����z��̃C���f�b�N�X�p�̗񋓑�
		*/
	    
		enum
		{
			// Fragment uniform
			PS_ALPHA_TEST,
			PS_ALPHA_REF,
			PS_ALPHA_FUNC,
			// Fragment uniform : Texture samplerType
			PS_TEXTURE0_SAMPLER_TYPE,
			// Fragment uniform : Fragment lighting
			PS_LIGHTING_ENABLED,
			PS_LIGHTING_AMBIENT,
			// Fragment uniform : Fragment light0
			PS_LIGHT_SOURCE0_ENABLED,
			PS_LIGHT_SOURCE0_AMBIENT,
			PS_LIGHT_SOURCE0_DIFFUSE,
			PS_LIGHT_SOURCE0_POSITION,

			// Fragment uniform : Material
			PS_MATERIAL_AMBIENT,
			PS_MATERIAL_DIFFUSE,
			// Fragment uniform : Texture combiner0
			PS_TEXENV0_SRC_RGB,
			PS_TEXENV0_SRC_ALPHA,
			PS_TEXENV0_OPERAND_RGB,
			PS_TEXENV0_OPERAND_ALPHA,
			PS_TEXENV0_COMBINE_RGB,
			PS_TEXENV0_COMBINE_ALPHA,
			// Max uniform locations num
			MAX_UNIFORM_LOCATIONS_NUM
		};

		GLuint			m_ParentProgramObjectID;
		s32				m_UniformLocations[MAX_UNIFORM_LOCATIONS_NUM];
	};


}}}

#endif  
 
