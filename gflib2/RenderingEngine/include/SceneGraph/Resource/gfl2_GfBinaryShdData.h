﻿#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GfBinaryShdData_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GfBinaryShdData_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>

#include <math/include/gfl2_MathCommon.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>


namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//-----------------------------------------------------------------------------
/**
 * @class	GfBinaryShdData
 * @brief	GfBinaryShdDataクラス
 */
class GfBinaryShdData : public IResourceData
{
public:

	struct ChunkHeader{
		ChunkHeader( u64 id, u32 size ){ m_ID = id;m_Size = size;}
		u64				m_ID;
		u32				m_Size;
	};

	GfBinaryShdData();

	virtual ~GfBinaryShdData();

	//! @brief データセット
	bool SetShaderData( c8* pData );

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
		return IResourceData::Type("gfbtex");
	}

	//! @brief データを保持しているか？
  virtual bool HasData(void)
  {
    return this->GetData(0) != NULL;
  }

  //! @brief バージョンIDの取得
  virtual u32 GetVersionId( void );

private:

	//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  GfBinaryShdData::Implクラス
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	class Impl
  {
  public:
	  Impl():
      m_pShaderDataTop(NULL),
      m_Version(0),
      m_DataNum(0)
	  {}

	  ~Impl();

	  bool setShaderData( c8* pData );

	  s32 GetSize( void );
	  const void* GetData( u32 index );
	  u32 GetDataSize( u32 index );
	  IResourceData::Type GetType( u32 index );
	  u32 GetVersionID(){ return m_Version; }

	  c8											*m_pShaderDataTop;
	  u32											m_Version;
	  u32											m_DataNum;
  };

	Impl								m_Impl;
};


}}}}

#endif



