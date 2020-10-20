#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MeshResourceNode.h>
#include <debug/include/gfl2_Assert.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MeshResourceNode::FaceData::FaceData() : 
  gfx::GLMemory(),
#if defined(GF_PLATFORM_CTR)
  m_pVertexBuffer(NULL),
  m_pIndexBuffer(NULL),
  m_VertexBufferSize(0),
  m_IndexBufferSize(0),
#elif defined(GF_PLATFORM_WIN32)
  m_pVertexBuffer(NULL),
  m_pIndexBuffer(NULL),
#endif
  m_MaterialNameHash(0),
  m_JointNum(0),
  m_pJointList(NULL),
  m_VertexNum(0),
  m_IndexNum(0),
  m_JointWeightMax(0)
{
#if defined(GF_PLATFORM_CTR)
  for (u32 i = 0; i < CommandType::Max; ++i)
  {
    m_CommandSize[i] = 0;
    m_pCommandTop[i] = NULL;
  }
#endif
}

MeshResourceNode::FaceData::~FaceData()
{
#if defined(GF_PLATFORM_CTR)
		
#elif defined(GF_PLATFORM_WIN32)
	GFX_SAFE_DELETE( m_pVertexBuffer );
	GFX_SAFE_DELETE( m_pIndexBuffer );
#endif
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MeshResourceNode::MeshResourceNode() : NodeTrait<MeshResourceNode, ResourceNode>(),
	m_pFaceDataList(NULL)
{
	
}

MeshResourceNode::~MeshResourceNode()
{
	if ( m_pFaceDataList )
		delete[] m_pFaceDataList;
}

u32 MeshResourceNode::GetTotalVertexNum() const
{
  u32 totalVertexNum = 0;
  u32 faceNum = GetFaceNum();
  for( u32 i = 0; i < faceNum; i++ )
  {
    const FaceData* pFaceData = GetFaceData( i );
    totalVertexNum += pFaceData->m_VertexNum;
  }

  return totalVertexNum;
}

u32 MeshResourceNode::GetTotalIndexNum() const
{
  u32 totalIndexNum = 0;
  u32 faceNum = GetFaceNum();
  for( u32 i = 0; i < faceNum; i++ )
  {
    const FaceData* pFaceData = GetFaceData( i );
    totalIndexNum += pFaceData->m_IndexNum;
  }

  return totalIndexNum;
}

void MeshResourceNode::SetData( const void* data, int size )
{
	GFL_ASSERT(data);
	const u8									*pSeek = reinterpret_cast<const u8*>(data);

#if defined(GF_PLATFORM_CTR)

	struct Header{
		u32						hashValue;
    c8						nameBuffer[Constant::MESH_NAME_MAX];
		u32						isBlendShape;
		math::AABB		aabb;
		u32						faceNum;
		u32						jointWeightMax;
	};

	const Header				*pHeader = reinterpret_cast<const Header*>( pSeek );

	SetNodeName( pHeader->nameBuffer );
	m_NameHash = pHeader->hashValue;
	m_IsBlendShape = pHeader->isBlendShape;
	m_AABB = pHeader->aabb;
	m_FaceDataNum = pHeader->faceNum;

	if( this->m_VersionId >= GfModelVersion::VertexShaderUpdate15041820 )
		m_JointWeightMax = pHeader->jointWeightMax;
	else
		m_JointWeightMax = 4;

	pSeek += sizeof( Header );

	u32								readSize = (u32)pSeek - (u32)data;
	if ( readSize % 128 )
	{
		u32			paddingSize = 128 - (readSize % 128 );
		pSeek += paddingSize;
	}

	if ( m_pFaceDataList )
			delete[] m_pFaceDataList;
	m_pFaceDataList = new FaceData[ m_FaceDataNum ];

	{
		struct CommandHeader{
			u32							length;
			u32							no;
			u32							max;
			u32							padding;
		};

		CommandHeader			header;

		for( u32 faceNo = 0; faceNo < m_FaceDataNum; ++faceNo )
		{
			for( u32 type = 0; type < FaceData::CommandType::Max; ++type )
			{
				std::memcpy( &header, pSeek, sizeof(CommandHeader) );
				pSeek += sizeof(CommandHeader);

				m_pFaceDataList[faceNo].m_pCommandTop[type] = (u32*)pSeek;//一部コマンドを置き換えます。
				m_pFaceDataList[faceNo].m_CommandSize[type] = header.length;

				pSeek += header.length;
			}
		}

    uptr     topaddr = nngxGetPhysicalAddr( (uptr)data );
    uptr     fcramaddr = nngxGetPhysicalAddr(nn::os::GetDeviceMemoryAddress());
    bit32    fmt  = 0x80000000;
    uptr     scBaseAddr = nngxGetPhysicalAddr( nngxGetVramStartAddr( nn::gx::MEM_VRAMA ) ) >> 3;

    if ( topaddr >= fcramaddr )
      scBaseAddr = fcramaddr >> 3;

		for( u32 faceNo = 0; faceNo < m_FaceDataNum; ++faceNo )
		{
			const u32					*pSeekU32 = reinterpret_cast<const u32*>( pSeek );

			m_pFaceDataList[faceNo].m_JointWeightMax = m_JointWeightMax;
			m_pFaceDataList[faceNo].m_MaterialNameHash = *pSeekU32;
			++pSeekU32;
			pSeek += 4;

			u32			nameLength = *pSeekU32;
			++pSeekU32;
			pSeek += 4;

			std::memcpy( &m_pFaceDataList[faceNo].m_MaterialName, pSeek, nameLength );
			pSeekU32 += nameLength / 4;
			pSeek += nameLength;

			if ( nameLength < sizeof(m_pFaceDataList[faceNo].m_MaterialName) )
				m_pFaceDataList[faceNo].m_MaterialName[nameLength] = 0;

			{//32Byte
				m_pFaceDataList[faceNo].m_JointNum = *pSeek;
				++pSeek;

				m_pFaceDataList[faceNo].m_pJointList = pSeek;
				pSeek += 31;

				pSeekU32 = reinterpret_cast<const u32*>( pSeek );
			}

			m_pFaceDataList[faceNo].m_VertexNum = *pSeekU32;
			++pSeekU32;
			pSeek += 4;

			m_pFaceDataList[faceNo].m_IndexNum = *pSeekU32;
			++pSeekU32;
			pSeek += 4;

			m_pFaceDataList[faceNo].m_VertexBufferSize = *pSeekU32;
			++pSeekU32;
			pSeek += 4;

			m_pFaceDataList[faceNo].m_IndexBufferSize = *pSeekU32;
			++pSeekU32;
			pSeek += 4;
		}

		for( u32 faceNo = 0; faceNo < m_FaceDataNum; ++faceNo )
		{
			m_pFaceDataList[faceNo].m_pVertexBuffer = pSeek;
			pSeek += m_pFaceDataList[faceNo].m_VertexBufferSize;

			m_pFaceDataList[faceNo].m_pIndexBuffer = pSeek;
			pSeek += m_pFaceDataList[faceNo].m_IndexBufferSize;
		}

		//アドレス設定
		for( u32 faceNo = 0; faceNo < m_FaceDataNum; ++faceNo )
		{
			uptr     vaddr = nngxGetPhysicalAddr( (u32)m_pFaceDataList[faceNo].m_pVertexBuffer );
      uptr     iaddr = nngxGetPhysicalAddr( (u32)m_pFaceDataList[faceNo].m_pIndexBuffer );
      uptr     addr = iaddr - scBaseAddr * 8;

			if( this->m_VersionId >= GfModelVersion::SupportVsBoolMap )
			{
				m_pFaceDataList[faceNo].m_pCommandTop[FaceData::CommandType::Draw][2] = fmt | addr;
			}
			else
			{
				m_pFaceDataList[faceNo].m_pCommandTop[FaceData::CommandType::Draw][4] = fmt | addr;
			}

      m_pFaceDataList[faceNo].m_pCommandTop[FaceData::CommandType::VertexAttrEnable][8] = scBaseAddr;
			m_pFaceDataList[faceNo].m_pCommandTop[FaceData::CommandType::VertexAttrEnable][12] = vaddr - scBaseAddr * 8;
			
		}
	}

#elif defined(GF_PLATFORM_WIN32)

struct Header{
		u32						hashValue;
		c8						nameBuffer[Constant::MESH_NAME_MAX];
		u32						isBlendShape;
		math::AABB		aabb;
	};

	const Header				*pHeader = reinterpret_cast<const Header*>( pSeek );

	SetNodeName( pHeader->nameBuffer );
	m_NameHash = pHeader->hashValue;
	m_IsBlendShape = pHeader->isBlendShape;
	m_AABB = pHeader->aabb;

	pSeek += sizeof( Header );

	u32									vertexDataNum;
	std::memcpy( &vertexDataNum, pSeek, sizeof(u32) );
	pSeek += sizeof( vertexDataNum );

	{//頂点宣言
		m_StreamSize = 0;

		for( u32 i = 0; i < vertexDataNum; ++i )
		{
			u32			usageNo;

			std::memcpy( &usageNo, pSeek, sizeof(usageNo) );
			pSeek += sizeof( usageNo );

			{//!@note サイズ最適化
				VertexElement			elm = VertexElement::VERTEX_ELEMENT_DECL( 0, usageNo,	0, VertexType::Float, 4 );
				m_VertexElements.push_back( elm );
				m_StreamSize += sizeof(math::Vector);
			}
		}

		VertexElement			element = VertexElement::VERTEX_ELEMENT_END();
		m_VertexElements.push_back( element );
	}
	
	{//頂点＆インデックスバッファ作成
		std::memcpy( &m_FaceDataNum, pSeek, sizeof(u32) );
		pSeek += sizeof( m_FaceDataNum );

		if ( m_pFaceDataList )
			delete[] m_pFaceDataList;

    gfx::GLMemory::SetInstanceAllocator( this->GetInstanceAllocator() );
		m_pFaceDataList = new FaceData[ m_FaceDataNum ];
    gfx::GLMemory::ClearInstanceAllocator();

		for( u32 faceNo = 0; faceNo < m_FaceDataNum; ++faceNo )
		{
			

			std::memcpy( &m_pFaceDataList[faceNo].m_MaterialNameHash, pSeek, sizeof(u32) );
			pSeek += sizeof( u32 );

      std::memcpy( &m_pFaceDataList[faceNo].m_MaterialName, pSeek, sizeof(c8) * Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX );
			pSeek += sizeof( c8 ) * Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX;

			m_pFaceDataList[faceNo].m_JointNum = *pSeek;
			++pSeek;

			m_pFaceDataList[faceNo].m_pJointList = pSeek;
			pSeek += m_pFaceDataList[faceNo].m_JointNum;

			std::memcpy( &m_pFaceDataList[faceNo].m_VertexNum, pSeek, sizeof(u32) );
			pSeek += sizeof( u32 );

      m_pFaceDataList[faceNo].m_pVertexBuffer = GFGL::CreateVertexBuffer( this->GetInstanceAllocator(), m_StreamSize, m_pFaceDataList[faceNo].m_VertexNum, Usage::WriteOnly, Pool::Default, pSeek );
			pSeek += m_StreamSize * m_pFaceDataList[faceNo].m_VertexNum;

			std::memcpy( &m_pFaceDataList[faceNo].m_IndexNum, pSeek, sizeof(u32) );
			pSeek += sizeof( u32 );

			u32			bufferSize = sizeof( u32 ) * m_pFaceDataList[faceNo].m_IndexNum;
			m_pFaceDataList[faceNo].m_pIndexBuffer = GFGL::CreateIndexBuffer( this->GetInstanceAllocator(), bufferSize, Usage::WriteOnly, Pool::Default, IndexType::Index32, pSeek );
			pSeek += bufferSize;
		}
	}
#endif

}


}}}}

