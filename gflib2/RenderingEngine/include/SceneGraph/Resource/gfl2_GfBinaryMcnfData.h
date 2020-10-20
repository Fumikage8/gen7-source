#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GfBinaryMcnfData_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GfBinaryMcnfData_H_INCLUDED

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
 * @class	GfBinaryMcnfData
 * @brief	GfBinaryMdlDataクラス
 */
class GfBinaryMcnfData : public IResourceData
{
public:

	GfBinaryMcnfData();

	virtual ~GfBinaryMcnfData();

	//! @brief データセット
	bool SetMcnfData( c8* pData );

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
		return IResourceData::Type("mcnf");
	}

	//! @brief データを保持しているか？
  virtual bool HasData(void)
  {
    return this->GetData(0) != NULL;
  }

  //! @brief バージョンIDの取得
  virtual u32 GetVersionId( void )
  {
    return 0;
  }

private:

	c8* m_pMcnfDataTop;
};


}}}}

#endif



