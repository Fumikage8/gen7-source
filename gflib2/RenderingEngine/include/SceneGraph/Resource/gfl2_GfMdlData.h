#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GFMDLDATA_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GFMDLDATA_H_INCLUDED

//後日Windowsアプリ用拡張ライブラリを用意して、そちらで提供いたします。
#if defined(GF_PLATFORM_WIN32)

#include <stdio.h>
#include <string>
#include <memory.h>
#include <types/include/gfl2_Typedef.h>


#include <math/include/gfl2_MathCommon.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_Matrix44.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>


namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//-----------------------------------------------------------------------------
/**
 * @class	GfMdlData
 * @brief	GfMdlDataクラス
 */
class GfMdlData : public IResourceData
{
public:

	struct JointData
	{
		JointData()
		{
			memset( m_Name, 0, sizeof(m_Name) );
			memset( m_ParentName, 0, sizeof(m_ParentName) );
			m_NeededRendering = false;
			m_LocalMatrix = math::Matrix44::GetIdentity();
			m_GlobalMatrix = math::Matrix44::GetIdentity();
		}

		c8									m_Name[Constant::JOINT_NAME_MAX];
		c8									m_ParentName[Constant::JOINT_NAME_MAX];
		math::Matrix44				m_LocalMatrix;
		math::Matrix44				m_GlobalMatrix;
		b32									m_NeededRendering;
    f32                 m_LocalScale[3];
    f32                 m_LocalRotation[3];
    f32                 m_LocalTranslate[3];
	};

	GfMdlData();
	GfMdlData( c8* pData );									//! @note 暗黙の型変換に注意！
	GfMdlData( ::std::string filePath );			//! @note 暗黙の型変換に注意！
	GfMdlData( ::std::wstring wstrFilePath );	//! @note 暗黙の型変換に注意！

	virtual ~GfMdlData();

	//! @brief データからの読み込み
	bool ReadData( c8* pData );

	//! @brief ファイルからの読み込み（相対パス対応）
	bool ReadFile( ::std::string filePath );

	//! @brief 全データの数の取得
	virtual s32 GetSize( void );

	//! @brief データの取得
	virtual const void* GetData( u32 index );

	//! @brief 個々のデータのサイズ取得
	virtual u32 GetDataSize( u32 index );

	//! @brief タイプデータの取得
	virtual Type GetType( u32 index );

	//! @brief ファイルタイプの取得
	virtual Type GetFileType( void )
	{
		return IResourceData::Type("gfmdl");
	}

	//! @brief データを保持しているか？
  virtual bool HasData(void)
  {
    return this->GetData(0) != NULL;
  }

  //! @brief バージョンIDの取得
  virtual u32 GetVersionId( void );

	//-----------------------------------------------------------------------------
	/// @brief	UTF-16をUTF-8に変換
	//-----------------------------------------------------------------------------
	::std::string UTF16toUTF8( const ::std::wstring& str );

private:

	class Impl;
	Impl								*m_pImpl;
};


}}}}

#endif

#endif


