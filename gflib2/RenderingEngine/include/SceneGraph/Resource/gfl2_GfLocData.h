#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GfLocDATA_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GfLocDATA_H_INCLUDED

//後日Windowsアプリ用拡張ライブラリを用意して、そちらで提供いたします。
#if defined(GF_PLATFORM_WIN32)

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>

#include <math/include/gfl2_Vector.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//-----------------------------------------------------------------------------
/**
 * @class	GfLocData
 * @brief	GfLocDataクラス
 */
class GfLocData : public IResourceData
{
public:

	GfLocData( c8* pData );
	virtual ~GfLocData();

	//! @brief 全データの数の取得
	virtual s32 GetSize( void );

	// Write Binary File:
	virtual bool WriteFile(std::string filePath);

	//! @brief データの取得
	virtual const void* GetData( u32 index );

	//! @brief 個々のデータのサイズ取得
	virtual u32 GetDataSize( u32 index );

	//! @brief タイプデータの取得
	virtual Type GetType( u32 index );

	//! @brief ファイルタイプの取得
	virtual Type GetFileType( void )
	{
		return IResourceData::Type("gfloc");
	}

	//! @brief データを保持しているか？
  virtual bool HasData(void)
  {
    return (this->GetData(0) != NULL);
  }

  //! @brief バージョンIDの取得
  virtual u32 GetVersionId( void )
  {
    return 0;
  }

private:
	GfLocData():IResourceData(){}

	class Impl;
	Impl								*m_pImpl;
};


}}}}

#endif

#endif


