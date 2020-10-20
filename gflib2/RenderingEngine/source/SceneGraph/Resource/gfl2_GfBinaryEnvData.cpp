//==============================================================================
/**
 @file    gfl2_GfBinaryEnvData.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.16
 @brief   gfbenvを扱うクラス
 */
//==============================================================================


#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <gfx/include/gfl2_types.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightNode.h>


#define          ARRAY_SIZE(array)    (sizeof(array) / sizeof(array[0]))

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfBinaryEnvData::Impl::Impl():
  m_pBinaryEnvData(NULL),
  m_Version(0),
  m_EnvTextureDataNum(0),
  m_LightSetNum(0),
  m_CameraNum(0),
  m_DataStartBytePos(0),
  m_DataSizeStartBytePos(0)
{

}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfBinaryEnvData::Impl::~Impl()
{

}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
s32 GfBinaryEnvData::Impl::GetSize( void )
{
  s32 size = 0;
  if(m_pBinaryEnvData)
  {
    s32 drawEnvNum        = 1;
    s32 envTextureDataNum = m_EnvTextureDataNum;
    s32 lightSetNum       = m_LightSetNum;
    s32 cameraSetNum      = m_CameraNum;
    size = drawEnvNum + envTextureDataNum + lightSetNum + cameraSetNum;
  }
  return size;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
const void* GfBinaryEnvData::Impl::GetData( u32 index )
{
  if(m_pBinaryEnvData)
  {
    // DrawEnv
    if ( index == 0 )
      return this->getBinaryEnvDataValByC8Pointer(this->getBinaryEnvDataValByU32(m_DataStartBytePos));
    --index;
 
    // EnvTextureData
    if ( index < m_EnvTextureDataNum )
    {
      return this->getBinaryEnvDataValByC8Pointer(this->getBinaryEnvDataValByU32(m_DataStartBytePos+4));
    }
    index -= m_EnvTextureDataNum;

    // LightSet
    if ( index < m_LightSetNum )
    {
      return this->getBinaryEnvDataValByC8Pointer(this->getBinaryEnvDataValByU32(m_DataStartBytePos+4+(4*m_EnvTextureDataNum)+(4*index)));
    }
    index -= m_LightSetNum;
 
    // Camera
    if ( index < m_CameraNum )
    {
      return this->getBinaryEnvDataValByC8Pointer(this->getBinaryEnvDataValByU32(m_DataStartBytePos+4+(4*m_EnvTextureDataNum)+(4*m_LightSetNum)+(4*index)));
    }
    index -= m_CameraNum;
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
u32 GfBinaryEnvData::Impl::GetDataSize( u32 index )
{
  if(m_pBinaryEnvData)
  {
    if ( index == 0 )
      return this->getBinaryEnvDataValByU32(m_DataSizeStartBytePos);
    --index;
 
    if ( index < m_EnvTextureDataNum )
    {
      return this->getBinaryEnvDataValByU32(m_DataSizeStartBytePos+4);
    }
    index -= m_EnvTextureDataNum;
 
    if ( index < m_LightSetNum )
    {
      return this->getBinaryEnvDataValByU32(m_DataSizeStartBytePos+4+(4*m_EnvTextureDataNum)+(4*index));
    }
    index -= m_LightSetNum;
 
    if ( index < m_CameraNum )
    {
      return this->getBinaryEnvDataValByU32(m_DataSizeStartBytePos+4+(4*m_EnvTextureDataNum)+(4*m_LightSetNum)+(4*index));
    }
    index -= m_CameraNum;
  }
  return 0;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
IResourceData::Type GfBinaryEnvData::Impl::GetType( u32 index )
{
  if ( index == 0 )
    return IResourceData::Type("gfenv");
  --index;

  if ( index < m_EnvTextureDataNum )
  {
    return IResourceData::Type("texture");
  }
  index -= m_EnvTextureDataNum;

  if ( index < m_LightSetNum )
  {
    return IResourceData::Type("lightset");
  }
  index -= m_LightSetNum;

  if ( index < m_CameraNum )
  {
    return IResourceData::Type("camera");
  }
  index -= m_CameraNum;

  return IResourceData::Type("none");
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
u32 GfBinaryEnvData::Impl::GetVersionId( void )
{
  return m_Version;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
bool GfBinaryEnvData::Impl::SetBinaryEnvData( c8* pData )
{
  // 扱うデータの情報
  static const c8* signature    = "GFBENV\0\0";
  static const u16 versionMajor = 1;
  static const u16 versionMinor = 0;
   

  bool ret = true;
  m_pBinaryEnvData = pData;

  if(m_pBinaryEnvData)
  {
    do
    {
      // シグネチャ確認
      {
        for(u32 i=0; i<8; ++i)
        {
          if( signature[i] != this->getBinaryEnvDataValByC8(i) )
          {
            ret = false;
            break;
          }
        }
        if(!ret) break;
      }
 
      // バージョン確認
      {
        if(
               versionMajor != this->getBinaryEnvDataValByU16(8)
            || versionMinor != this->getBinaryEnvDataValByU16(10)
        )
        {
          ret = false;
          GFL_ASSERT(0);  // バージョンが違うとハングするので、エラーにしておく。
          break;
        }
      }

      // 日付バージョン
      {
        m_Version = this->getBinaryEnvDataValByU32(16);
      }
 
      // アクセスしやすいように覚えておく
      m_EnvTextureDataNum = this->getBinaryEnvDataValByU16(20);
      m_LightSetNum       = this->getBinaryEnvDataValByU16(22);
      m_CameraNum         = this->getBinaryEnvDataValByU16(24);

      // パディング(2バイト)
 
      m_DataStartBytePos     = 28;
      m_DataSizeStartBytePos = 28+4*(1+m_EnvTextureDataNum+m_LightSetNum+m_CameraNum);
    } ONCE;

    if(!ret)
    {
      // 扱えるデータではない
      m_pBinaryEnvData = NULL;
    }
  }
  else
  {
    ret = false;
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
u16 GfBinaryEnvData::Impl::getBinaryEnvDataValByU16(u32 bytePos)
{
  u16 val = *(reinterpret_cast<u16*>(&(m_pBinaryEnvData[bytePos])));
  return val;
}
u32 GfBinaryEnvData::Impl::getBinaryEnvDataValByU32(u32 bytePos)
{
  u32 val = *(reinterpret_cast<u32*>(&(m_pBinaryEnvData[bytePos])));
  return val;
}
c8 GfBinaryEnvData::Impl::getBinaryEnvDataValByC8(u32 bytePos)
{
  c8 val = *(reinterpret_cast<c8*>(&(m_pBinaryEnvData[bytePos])));
  return val;
}
c8* GfBinaryEnvData::Impl::getBinaryEnvDataValByC8Pointer(u32 bytePos)
{
  c8* val = &(m_pBinaryEnvData[bytePos]);
  return val;
}


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfEnvDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfBinaryEnvData::GfBinaryEnvData() : IResourceData()
{
  
}

GfBinaryEnvData::~GfBinaryEnvData()
{
  
}

//! @brief  データセット
bool GfBinaryEnvData::SetBinaryEnvData( c8* pData )
{
  if (pData == NULL)
  {
    return false;
  }
  return m_Impl.SetBinaryEnvData( pData );
}

//! @brief 全データの数の取得
s32 GfBinaryEnvData::GetSize( void )
{
  return m_Impl.GetSize();
}

//! @brief データの取得
const void* GfBinaryEnvData::GetData( u32 index )
{
  return m_Impl.GetData(index);
}

//! @brief 個々のデータのサイズ取得
u32 GfBinaryEnvData::GetDataSize( u32 index )
{
  return m_Impl.GetDataSize(index);
}

//! @brief タイプデータの取得
IResourceData::Type GfBinaryEnvData::GetType( u32 index )
{
  return m_Impl.GetType(index);
}

//! @brief バージョンIDの取得
u32 GfBinaryEnvData::GetVersionId( void )
{
  return m_Impl.GetVersionId();
}

}}}}

