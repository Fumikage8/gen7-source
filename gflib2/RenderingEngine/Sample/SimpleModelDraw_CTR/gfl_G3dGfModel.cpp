//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		GfModel.cpp
 *	@brief  シェーダーモデルクラス
 *	@author	Masayuki Onoue
 *	@date		2012.06.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

//#include <system/GfModel.h>
#include <nn.h>
#include <nn/gr.h>

#include <stdio.h>
#include <map>
#include <iterator>
#include <iostream>
#include <vector>
#include <list>

#include <nn/gx/CTR/gx_MacroReg.h>
#include <nn/gr/CTR/gr_Utility.h>
#include <nn/math.h>

#include "gfl_G3dGfModel.h"

#include <gfx/include/gfl2_GFGL.h>

using namespace gfl2;
using namespace gfl2::gfx;

namespace gfl {
namespace grp {
namespace g3d {
  
//---------------------------------------------------------------------------
// #2 モデル描画を行うクラスの実装 : MyModel
//---------------------------------------------------------------------------
void MyModel::SetTexture( s32 no, void* pData )
{
	NN_LOG( "SetTexture %d\n", no );

  struct    Header{
    u32       version;
    u32       dataCount;
  };
  
  struct ChunkHeader{
		u64				m_ID;
		u32				m_Size;
    u32				m_Dummy;
	};
  
  struct ImageHeader//104Byte
	{
		s32						size;
		u16						width;
		u16						height;
		u16						format;
		u16						mipLevel;
		u32						dumy[3];
		c8						path[80];
	};
  
  u8          *pSeek;
  Header      *pHeader;
  ChunkHeader *pChunkHeader;
  ImageHeader *pImageHeader;
  
  pSeek = (u8*)pData;
  
  pHeader = (Header*)pSeek;
  pSeek += sizeof(Header);
  
  pChunkHeader = (ChunkHeader*)pSeek;
  pSeek += sizeof(ChunkHeader);
  
  pImageHeader = (ImageHeader*)pSeek;
  pSeek += sizeof(ImageHeader);
  
  u32     *pTextureCommandTop = &m_TextureCommandTop[no][0];
 
  m_pTextureData = pSeek;
  
  NN_LOG( "%s = %d, %d\n", pImageHeader->path, pImageHeader->width, pImageHeader->height );

	//m_pTextureData = gfl2::gfx::GLMemory::Malloc( 256 * 256 * 4, 128 );
	//memset( m_pTextureData, 0xFF, 256 * 256 * 4 );
  
  u32     addr = nngxGetPhysicalAddr( (u32)m_pTextureData );
	NN_LOG( "texture addr = %08x\n", addr );
  
  //デフォルトではTexture0に設定するコマンド
  // 0x082
	pTextureCommandTop[0] = PICA_CMD_DATA_TEXTURE_SIZE( pImageHeader->width, pImageHeader->height );
  u32 sizeTable[] = { PICA_REG_TEXTURE0_SIZE,
                      PICA_REG_TEXTURE1_SIZE,
                      PICA_REG_TEXTURE2_SIZE };
  pTextureCommandTop[1] = PICA_CMD_HEADER_SINGLE( sizeTable[no] );

  // 0x085
  pTextureCommandTop[2] = PICA_CMD_DATA_TEXTURE_ADDR( addr );
  u32 addrTable[] = { PICA_REG_TEXTURE0_ADDR1,
                      PICA_REG_TEXTURE1_ADDR,
                      PICA_REG_TEXTURE2_ADDR };
  pTextureCommandTop[3] = PICA_CMD_HEADER_SINGLE( addrTable[no] );

  // 0x08e
	pTextureCommandTop[4] = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE( pImageHeader->format );
  u32 formatTable[] = { PICA_REG_TEXTURE0_FORMAT,
                        PICA_REG_TEXTURE1_FORMAT,
                        PICA_REG_TEXTURE2_FORMAT };
  pTextureCommandTop[5] = PICA_CMD_HEADER_SINGLE( formatTable[no] );
}

void MyModel::SetVertexShader( void* pData )
{
  struct    Header{
    u32       version;
    u32       dataCount;
  };
  
  struct ChunkHeader{
		u64				m_ID;
		u32				m_Size;
    u32				m_Dummy;
	};
  
  static const u32	PATH_MAX = 80;
  
  struct FileHeader//!@note 適切な場所から参照
	{//16バイトアラインサイズ
		c8								shaderName[PATH_MAX];
		s32								shaderNameHash;
		s32								shaderNum;
		s32								dumy[2];
	};

	struct ShaderHeader//!@note 適切な場所から参照
	{//16バイトアラインサイズ
		s32						size;
		u32						shaderType;
		s32						dumy[2];
		c8						path[PATH_MAX];
	};
  
  struct CommandHeader//!@note 適切な場所から参照
	{//16バイトアラインサイズ
		u32						size;
		u32						dumy[3];
	};
  
  u8            *pSeek;
  Header        *pHeader;
  ChunkHeader   *pChunkHeader;
  FileHeader    *pFileHeader;
  ShaderHeader  *pShaderHeader;
  CommandHeader  *pCommandHeader;
  
  pSeek = (u8*)pData;
  
  pHeader = (Header*)pSeek;
  pSeek += sizeof(Header);
  
  pChunkHeader = (ChunkHeader*)pSeek;
  pSeek += sizeof(ChunkHeader);
  
  pSeek += sizeof(u32) * 2;//Padding
  
  pFileHeader = (FileHeader*)pSeek;
  pSeek += sizeof(FileHeader);
  
  pShaderHeader = (ShaderHeader*)pSeek;
  pSeek += sizeof(ShaderHeader);
  
  pCommandHeader = (CommandHeader*)pSeek;
  pSeek += sizeof(CommandHeader);
  
  m_VertexShaderSize = pCommandHeader->size;//2512
  m_pVertexShaderData = pSeek;
}

void MyModel::SetGfModel( void* pData )
{
  m_pGfModelData = pData;
  
  u8*             pSeek = (u8*)m_pGfModelData;
  ChunkHeader     *pHeader = (ChunkHeader*)pSeek;
  
  //モデル情報
  pSeek += sizeof(ChunkHeader);
  pSeek += pHeader->m_Size;
  
  pHeader = (ChunkHeader*)pSeek;
  pSeek += sizeof(ChunkHeader);
  
  {
    u8*       pMatSeek = pSeek;
    u32       *pMaterialTop = (u32*)pMatSeek;
    
    pMatSeek += 32;
    
    u32       *pCommandHeader = (u32*)pMatSeek;
    m_MaterialCommandSize = pCommandHeader[0];
    pMatSeek += 16;

    m_pMaterilCommandTop = (u32*)pMatSeek;
  }
  
  pSeek += pHeader->m_Size;
  pHeader = (ChunkHeader*)pSeek;
  pSeek += sizeof(ChunkHeader);
  
  {
    u8*       pMeshSeek = pSeek;
    u32       *pMeshTop = (u32*)pMeshSeek;
    c8*       pMeshName = (c8*)&pMeshSeek[4];
    
    NN_LOG("pMeshName = %s\n", pMeshName );
    
    pMeshSeek += (4+64+4+32+4+4);
    
    u32       *pCommandHeader = (u32*)pMeshSeek;
    u32       commandSize = pCommandHeader[0];
    u32       dataType = pCommandHeader[1];
    
    m_MeshCommandNum = pCommandHeader[2];
    
    for( s32 i = 0; i < m_MeshCommandNum; ++i )
    {
      pCommandHeader = (u32*)pMeshSeek;
      commandSize = pCommandHeader[0];
      dataType = pCommandHeader[1];
      
      pMeshSeek += 4 * 4;
      
      m_MeshCommandSize[i] = commandSize;
      m_pMeshCommandTop[i] = (u32*)pMeshSeek;
      
      pMeshSeek += commandSize;
      
      NN_LOG("MeshCommand[%d/%d] = %d\n", i, m_MeshCommandNum, commandSize );
    }
    
    u32       *pFaceTop = (u32*)pMeshSeek;
    u32       faceDataNum = pFaceTop[0];
    
    pMeshSeek += 16;
    
    for( u32 faceNo = 0; faceNo < faceDataNum; ++faceNo )
    {
      m_pFaceHeader[faceNo] = (FaceHeader*)pMeshSeek;
      
      NN_LOG("MeshName = %s\n", m_pFaceHeader[faceNo]->name );
      
      pMeshSeek += sizeof(FaceHeader);
    }
    
    for( u32 faceNo = 0; faceNo < faceDataNum; ++faceNo )
    {
      m_pVertexBuffer[faceNo] = (u32*)pMeshSeek;
      pMeshSeek += m_pFaceHeader[faceNo]->vertexBufferSize;
      
      m_pIndexBuffer[faceNo] = (u32*)pMeshSeek;
      pMeshSeek += m_pFaceHeader[faceNo]->indexBufferSize;
      
      
      uptr     vaddr = nngxGetPhysicalAddr( (u32)m_pVertexBuffer[faceNo] );
      uptr     iaddr = nngxGetPhysicalAddr( (u32)m_pIndexBuffer[faceNo] );
      
      bit32 fmt  = 0x80000000;
      const uptr scBaseAddr = nngxGetPhysicalAddr( nngxGetVramStartAddr( nn::gx::MEM_VRAMA ) ) >> 3;
      uptr  addr = iaddr - scBaseAddr * 8;
                
      m_pMeshCommandTop[0][12] = vaddr - scBaseAddr * 8;
      m_pMeshCommandTop[2][4] = fmt | addr;
      
      NN_LOG("scBaseAddr = %08x\n", scBaseAddr );
      NN_LOG("VertexAddr = %08x\n", m_pMeshCommandTop[0][12] );//enable command
      NN_LOG("IndexAddr = %08x\n", m_pMeshCommandTop[2][2] );//draw command
    }
  }
  
}

void MyModel::DrawMesh()
{
    
	bool changed = true; // シェーダーが切り替わったら true

	GFGL::AddCommandJump( (u32*)m_pVertexShaderData, m_VertexShaderSize );
	
	/*
	{
		dst->Put(bind.worldMtx.MakeUniformCommand(dst->GetCurrentPtr(), nn::math::MTX34::Identity()));
		dst->Put(bind.normalMtx.MakeUniformCommand(dst->GetCurrentPtr(), normal));
		dst->Put(bind.hslGCol.MakeUniformCommand(dst->GetCurrentPtr(), nw::math::VEC4(0.0f, 0.0f, 0.0f, 1.0f)));
		dst->Put(bind.hslSCol.MakeUniformCommand(dst->GetCurrentPtr(), nw::math::VEC4(0.0f, 0.0f, 0.0f, 1.0f)));
		dst->Put(bind.hslSDir.MakeUniformCommand(dst->GetCurrentPtr(), nw::math::VEC4(0.0f, 1.0f, 0.0f, 0.0f)));
	}*/
        
	GFGL::AddCommandJump( (u32*)m_pMaterilCommandTop, m_MaterialCommandSize);

	{
		GFGL::AddCommandCopy( (u32*)&m_TextureCommandTop[0][0], 3*2 );
		GFGL::AddCommandCopy( (u32*)&m_TextureCommandTop[1][0], 3*2 );
		GFGL::AddCommandCopy( (u32*)&m_TextureCommandTop[2][0], 3*2 );
          
		GFGL::AddCommandJump( (u32*)m_pMeshCommandTop[0], m_MeshCommandSize[0] );

		{
			u32												tempBuffer[1024];
			u32												*pSeek = tempBuffer;

			nn::math::MTX34 mtx34(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f);

      for( u32 regNo = 25; regNo < 85; regNo += 3 )
        pSeek = nn::gr::CTR::MakeUniformCommandVS( pSeek, regNo, mtx34 );

			u32												size = ((u32)pSeek - (u32)tempBuffer) / sizeof(u32);
			GFGL::AddCommandCopy( tempBuffer, size );
    }
          
		GFGL::AddCommandJump( (u32*)m_pMeshCommandTop[2], m_MeshCommandSize[2] );
		GFGL::AddCommandJump( (u32*)m_pMeshCommandTop[1], m_MeshCommandSize[1] );
	}
    
}

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 Createは呼ばれません。
 */
//-----------------------------------------------------------------------------
GfModel::GfModel(void)
{
	m_fH3d = false;
	m_pMyModelList = new MyModel();
}

GfModel::~GfModel()
{
	
}

}}}

