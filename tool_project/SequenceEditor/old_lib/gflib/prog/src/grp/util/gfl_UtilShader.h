#ifndef __GFL_UTIL_SHADER_H__
#define __GFL_UTIL_SHADER_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_UtilShader.h
 *	@brief  シェーダークラス
 *	@author	Masateru Ishiguro
 *	@date		2011.2.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gfl_Macros.h>
#include <nn/os.h>
#include <nn/gx.h>

namespace gfl { namespace grp { namespace util {

	typedef s32		ShaderParameterHandle;

	//-----------------------------------------------------------------------------
	/**
	 * @class	IShader
	 * @brief	シェーダーインターフェイス
	 */
	class IShader{
	public:
		virtual ~IShader(){}

		virtual void vSetLinkProgram( GLuint program_object_id ) = 0;
		virtual GLuint vGetObjectID() const = 0;
		virtual void vSetUpShader() const = 0;

	protected:
		IShader(){}
	};

	//-----------------------------------------------------------------------------
	/**
	 * @class	ShaderObject
	 * @brief	頂点シェーダー、ジオメトリシェーダー、ピクセルシェーダーを持つオブジェクト
	 */
	class ShaderObject{
	public:

		ShaderObject();
		~ShaderObject();

		static GLuint CreateVertexShaderFromMemory( const void* pSrc, size_t fileSize );

		void CreateProgram();
		void AttachVertexShader( GLuint shader );
    inline void AttachStaticVertexShader( u32 *p_shader, u32 size );
		void AttachGeometryShader( GLuint shader );
		void AttachPixelShader( IShader* pShader );
		void BindAttribLocation( int index, const char* param_name);//@note 一定の規則にしたがってシェーダーを記述すればこの関数は不要。
		void LinkProgram();
		void DeleteProgram();

		//const char* GetParameterName(const ShaderParameterHandle& parameter) const;
		//GLenum GetParameterType(const ShaderParameterHandle& parameter) const;

		ShaderParameterHandle GetParameterByName(const char* name) const;
		//void SetFloatArray(const ShaderParameterHandle& parameter, const float *fdata, u_int count);
		//void SetVector(const ShaderParameterHandle& parameter, const Math::Vector& vector);
		//void SetVectorArray(const ShaderParameterHandle& parameter, const Math::Vector& vector, u_int count);
		//void SetMatrix(const ShaderParameterHandle& parameter, const Math::Matrix& matrix);

		inline GLuint GetProgramId() const;
    inline void SetStaticShader();
		inline const IShader* GetPixelShader() const;

	private:
		GLuint				m_ProgramId;
		GLuint				m_VertexShaderID;
		GLuint				m_GeometryShaderID;
		IShader				*m_pPixelShader;
    u32           *m_pStaticVertexShader;
    u32           m_StaticShaderSize;

		GLuint				m_VsShaderId;
		GLuint				m_PsShaderId;
		GLuint				m_GShaderId;
	};

	//-----------------------------------------------------------------------------
	/// @brief	
	//-----------------------------------------------------------------------------
	inline GLuint ShaderObject::GetProgramId() const
	{
		return m_ProgramId;
	}

	//-----------------------------------------------------------------------------
	/// @brief	
	//-----------------------------------------------------------------------------
	inline const IShader* ShaderObject::GetPixelShader() const
	{
		return m_pPixelShader;
	}

  //-----------------------------------------------------------------------------
	/// @brief
	//-----------------------------------------------------------------------------
  inline void ShaderObject::AttachStaticVertexShader( u32 *p_shader, u32 size )
  {
    m_pStaticVertexShader = p_shader;
    m_StaticShaderSize = size;
  }

  //-----------------------------------------------------------------------------
	/// @brief
	//-----------------------------------------------------------------------------
  inline void ShaderObject::SetStaticShader()
  {
    nngxAdd3DCommand( m_pStaticVertexShader, m_StaticShaderSize, GL_TRUE );
  }

}}}

#endif  
 
