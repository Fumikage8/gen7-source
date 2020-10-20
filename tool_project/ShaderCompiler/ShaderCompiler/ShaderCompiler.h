#if !defined(ShaderCompiler_H_INCLUDED)
#define ShaderCompiler_H_INCLUDED
#pragma once

#include <stdio.h>
#include <vector>
#include <string>
#include <types/include/gfl2_Typedef.h>

class ShaderCompiler
{
public:

	struct FileHeader//!@todo 適切な場所から参照
	{//16バイトアラインサイズ
		static const u32	SHADER_NAME_MAX = 64;
		c8								shaderName[SHADER_NAME_MAX];
		s32								shaderNameHash;
		s32								shaderNum;
		s32								dumy[2];
	};

	struct ShaderHeader//!@todo 適切な場所から参照
	{//16バイトアラインサイズ
		static const u32						PATH_MAX = 64;

		s32						size;
		u32						shaderType;
		s32						dumy[2];
		c8						path[PATH_MAX];
	};

	class BinaryMaker
	{
	public:
		BinaryMaker(){}
		~BinaryMaker(){}

		void Write( const void* pSrc, u32 size )
		{
			const u8* pData = reinterpret_cast<const u8*>(pSrc);
			m_Buffer.insert(m_Buffer.end(), pData, pData + size);
		}

		u8* CreateFixedBuffer()
		{
			u8* pOut = new u8[m_Buffer.size()];
			memcpy(pOut, &m_Buffer[0], m_Buffer.size());

			return pOut;
		}

		u32 GetBinarySize()
		{
			return static_cast<u32>( m_Buffer.size() );
		}

		std::vector<u8>				m_Buffer;
	};

	ShaderCompiler();
	~ShaderCompiler();

	enum Type{
		Vertex = 0,
		Pixel,
		Geometry,
		Max
	};

	void ReadFile( Type type, std::string filePath );
	void SetSource( Type type, const char* pSrc );
	void CompileHLSL(std::string shaderTypeName, std::vector<std::string> argList );
	void CompileGLSL(std::string shaderTypeName, std::vector<std::string> argList );
	void CompileCTR(std::string shaderTypeName, std::vector<std::string> argList );
	void WriteFile( std::string filePath );
	std::string GetError(){ return m_ErrorCode; }

	static char* ReadFile( const char* filename, int* p_size, int align );
	static void WriteFile( const char* filename, const void* p_data, int size );
	static void GetFilePath( char* p_out, const char* filename );

private:
	std::string				m_Source[Type::Max];
	std::string				m_ErrorCode;
	std::string				m_FileName;
	std::string				m_ShaderName;
	BinaryMaker				m_BinaryMaker;

};

#endif