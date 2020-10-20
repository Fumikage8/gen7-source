#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GfBinaryLocData_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GfBinaryLocData_H_INCLUDED

#include <stdio.h>
//#include <string>
//#include <memory.h>
#include <types/include/gfl2_Typedef.h>

#include <math/include/gfl2_MathCommon.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>


namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//-----------------------------------------------------------------------------
/**
 * @class	GfBinaryLocData
 * @brief	GfBinaryLocDataクラス
 */
class GfBinaryLocData : public IResourceData
{
public:

	struct ChunkHeader{
		ChunkHeader( u64 id, u32 size ){ m_ID = id;m_Size = size; m_Padding = 0xFFFFFFFF; }
		u64				m_ID;
		u32				m_Size;
		u32				m_Padding;
	};

	GfBinaryLocData();

	virtual ~GfBinaryLocData();

	//! @brief データセット
	bool SetLocData( c8* pData );

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
		return IResourceData::Type("gfbloc");
	}

	//! @brief データを保持しているか？
  virtual bool HasData(void)
  {
    return this->GetData(0) != NULL;
  }

  //! @brief バージョンIDの取得
  virtual u32 GetVersionId( void );

private:

	class Impl;
	Impl								*m_pImpl;
};


}}}}

#endif



