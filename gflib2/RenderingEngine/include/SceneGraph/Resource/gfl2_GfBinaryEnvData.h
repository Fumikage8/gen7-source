//==============================================================================
/**
 @file       gfl2_GfBinaryEnvData.h
 @author     kawada_koji@gamefreak.co.jp
 @date       2015.04.16
 @brief      gfbenvを扱うクラス
 @note       gfl2_GfEnvData.hがgfenvからつくるバイナリをそのままファイルにしたのがgfbenvです。
             gfbenvはgit_program\tool_project\EnvConverterを使ってgfenvから生成されます。
             git_program\tool_project\EnvConverterもgfl2_GfEnvData.hを使用しています。
 @note       xmlを扱うgfl2_GfEnvData.hからバイナリを扱うgfl2_GfBinaryEnvData.hを記述する際に参考にしたのは
             gfl2_GfMotData.hとgfl2_GfBinaryMotData.hです。
 */
//==============================================================================


#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GfBinaryEnvData_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GfBinaryEnvData_H_INCLUDED

#include <stdio.h>
//#include <string>
#include <types/include/gfl2_Typedef.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//-----------------------------------------------------------------------------
/**
 * @class GfBinaryEnvData
 * @brief GfBinaryEnvDataクラス
 */
class GfBinaryEnvData : public IResourceData
{
public:

  GfBinaryEnvData();
  virtual ~GfBinaryEnvData();

  //! @brief  データセット
  bool SetBinaryEnvData( c8* pData );

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
    return IResourceData::Type("gfbenv");
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
  //! @brief  GfBinaryEnvData::Implクラス
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  class Impl
  {
  public:
    Impl();

    ~Impl();

    bool SetBinaryEnvData( c8* pData );

    s32 GetSize( void );
    const void* GetData( u32 index );
    u32 GetDataSize( u32 index );
    IResourceData::Type GetType( u32 index );
    u32 GetVersionId( void );

  private:
    u16 getBinaryEnvDataValByU16(u32 bytePos);
    u32 getBinaryEnvDataValByU32(u32 bytePos);
    c8  getBinaryEnvDataValByC8(u32 bytePos);
    c8* getBinaryEnvDataValByC8Pointer(u32 bytePos);

    c8* m_pBinaryEnvData;
    u32 m_Version;
    u16 m_EnvTextureDataNum;
    u16 m_LightSetNum;
    u16 m_CameraNum;
    u32 m_DataStartBytePos;
    u32 m_DataSizeStartBytePos;
  };

  Impl                m_Impl;
};


}}}}

#endif

