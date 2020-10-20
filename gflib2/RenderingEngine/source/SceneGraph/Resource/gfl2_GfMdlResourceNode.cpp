//=============================================================================
/**
 * @file    gfl2_GfMdlResourceNode.cpp
 * @brief   GfMdlResourceNodeクラス（モデルのリソースを持つノード）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.6.6    ishiguro  ６４文字チェック追加
 *
 */
//=============================================================================
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <math/include/gfl2_SRT.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

GfMdlResourceNode::GfMdlResourceNode() : NodeTrait<GfMdlResourceNode, ResourceNode>(),
	m_TextureNum(0),
	m_MaterialNum(0),
	m_MeshNum(0),
	m_ShaderNum(0),
	m_JointNum(0),
	m_UserDataSize(0),
  m_pUserData(NULL),
  m_DefaultUserDataSize(0),
  m_pDefaultUserData(NULL),
  m_pElementNameList(NULL),
	m_JointDataList(NULL),
	m_LutTableNum(0),
	m_LutTableDataSize(0),
	m_pLutTableDataTop(NULL),
  m_IsStableJoint(true)
{
	
}

GfMdlResourceNode::~GfMdlResourceNode()
{
	if ( m_pElementNameList )
	{
		this->GetInstanceAllocator()->SystemMemoryFree( m_pElementNameList );
		m_pElementNameList = NULL;
	}

	if ( m_JointDataList )
	{
		this->GetInstanceAllocator()->SystemMemoryFree( m_JointDataList );
		m_JointDataList = NULL;
	}
}

void GfMdlResourceNode::SetData( const void* data, int sizeParam, bool createJointFlag )
{
	GFL_ASSERT(data);
	const u8									*pSeek = reinterpret_cast<const u8*>(data);

	std::memcpy( &m_ShaderNum, pSeek, sizeof(u32) );
	pSeek += sizeof(m_ShaderNum);
	pSeek += sizeof( ElementName ) * m_ShaderNum;

	std::memcpy( &m_TextureNum, pSeek, sizeof(u32) );
	pSeek += sizeof(m_TextureNum);
	pSeek += sizeof( ElementName ) * m_TextureNum;

	std::memcpy( &m_MaterialNum, pSeek, sizeof(u32) );
	pSeek += sizeof(m_MaterialNum);
	pSeek += sizeof( ElementName ) * m_MaterialNum;

	std::memcpy( &m_MeshNum, pSeek, sizeof(u32) );
	pSeek += sizeof(m_MeshNum);
	pSeek += sizeof( ElementName ) * m_MeshNum;

	if ( m_pElementNameList )
	{
		this->GetInstanceAllocator()->SystemMemoryFree( m_pElementNameList );
		m_pElementNameList = NULL;
	}

	{
		s32			num = (m_ShaderNum + m_TextureNum + m_MaterialNum + m_MeshNum);
		if ( num )
			m_pElementNameList = reinterpret_cast<ElementName*>( this->GetInstanceAllocator()->SystemMemoryMalloc( sizeof( ElementName ) * num ) );
	}

	pSeek = reinterpret_cast<const u8*>(data);

	{
		u32		size = sizeof( ElementName ) * m_ShaderNum;
		pSeek += sizeof(u32);
		if ( size )
			std::memcpy( m_pElementNameList, pSeek, size );
		pSeek += size;
	}

	{
		u32		size = sizeof( ElementName ) * m_TextureNum;
		pSeek += sizeof(u32);
		std::memcpy( &m_pElementNameList[m_ShaderNum], pSeek, size );
		pSeek += size;
	}

	{
		u32		size = sizeof( ElementName ) * m_MaterialNum;
		pSeek += sizeof(u32);
		if ( size )
			std::memcpy( &m_pElementNameList[m_ShaderNum+m_TextureNum], pSeek, size );
		pSeek += size;
	}

	{
		u32		size = sizeof( ElementName ) * m_MeshNum;
		pSeek += sizeof(u32);
		if ( size )
			std::memcpy( &m_pElementNameList[m_ShaderNum+m_TextureNum+m_MaterialNum], pSeek, size );
		pSeek += size;
	}

	{
		u32		size = sizeof( math::AABB );
		std::memcpy( &m_AABB, pSeek, size );
		pSeek += size;
	}

	if( this->m_VersionId >= GfModelVersion::EncountKusaUpdate15052316 )
	{
		{
			u32		size = sizeof( math::Matrix44 );
			std::memcpy( &m_TransMat, pSeek, size );
      m_TransMat = m_TransMat.Transpose();
			pSeek += size;
		}
		
		{
			UserDataHeader				userDataHeader;

			u32		size = sizeof( userDataHeader );
			std::memcpy( &userDataHeader, pSeek, size );
			pSeek += size;

			m_UserDataSize = userDataHeader.m_UserDataSize;
			if ( m_UserDataSize )
				m_pUserData = pSeek;
			pSeek += m_UserDataSize;

			if( this->m_VersionId >= GfModelVersion::BillBordModeUpdate15052616 )
			{
				m_DefaultUserDataSize = userDataHeader.m_DefaultUserDataSize;
				if ( m_DefaultUserDataSize )
					m_pDefaultUserData = reinterpret_cast<const DefaultModelUserData*>( pSeek );
				pSeek += m_DefaultUserDataSize;
			}
		}
	}

	{
		struct JointsDataHeader
		{//16バイトアラインサイズ
			s32						size;
			s32						dumy[3];
		};

		const JointsDataHeader*			pJointsDataHeader = reinterpret_cast<const JointsDataHeader*>( pSeek );
		pSeek += sizeof( JointsDataHeader );

		if ( m_JointDataList )
		{
			this->GetInstanceAllocator()->SystemMemoryFree( m_JointDataList );
			m_pElementNameList = NULL;
		}

		m_JointNum = pJointsDataHeader->size;

		if ( m_JointNum )
		{
      if (!createJointFlag)
      {
        // 着せ替え用にジョイントは作成しない
#if defined(GF_PLATFORM_CTR)
        for( u32 i = 0; i < m_JointNum; ++i )
        {
          u8				nameLength;
          u8				parentNameLength;

          nameLength = *pSeek;
        #if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)
          GFL_ASSERT_MSG( nameLength < Constant::JOINT_NAME_MAX, "ジョイント名が%d文字を超えています。リソースを修正してください。", Constant::JOINT_NAME_MAX);
        #endif  //  GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP
          ++pSeek;
          pSeek += nameLength;

          parentNameLength = *pSeek;
        #if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)
          GFL_ASSERT_MSG( parentNameLength < Constant::JOINT_NAME_MAX, "ジョイント名が%d文字を超えています。リソースを修正してください。", Constant::JOINT_NAME_MAX);
        #endif  //  GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP
          ++pSeek;
          pSeek += parentNameLength;
          
          ++pSeek;

          pSeek += sizeof(f32) * 9;
        }
#else
        pSeek += sizeof(JointData) * pJointsDataHeader->size;
#endif
      }
      else
      {
        m_JointDataList = reinterpret_cast<JointData*>( this->GetInstanceAllocator()->SystemMemoryMalloc( sizeof(JointData) * m_JointNum ) );

        //SystemMemoryMallocで0クリアされているので不要
        //std::memset( m_JointDataList, 0, sizeof(JointData) * m_JointNum );
#if defined(GF_PLATFORM_CTR)
        for( u32 i = 0; i < m_JointNum; ++i )
        {
          u8				nameLength;
          u8				parentNameLength;

          nameLength = *pSeek;
        #if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)
          GFL_ASSERT_MSG( nameLength < Constant::JOINT_NAME_MAX, "ジョイント名が%d文字を超えています。リソースを修正してください。", Constant::JOINT_NAME_MAX);
        #endif  //  GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP
          ++pSeek;
          std::memcpy( m_JointDataList[i].m_Name, pSeek, nameLength );
          pSeek += nameLength;

          parentNameLength = *pSeek;
        #if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)
          GFL_ASSERT_MSG( parentNameLength < Constant::JOINT_NAME_MAX, "ジョイント名が%d文字を超えています。リソースを修正してください。", Constant::JOINT_NAME_MAX);
        #endif  //  GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP
          ++pSeek;
          std::memcpy( m_JointDataList[i].m_ParentName, pSeek, parentNameLength );
          pSeek += parentNameLength;
          
          m_JointDataList[i].m_Status[0] = *pSeek;
          ++pSeek;

          std::memcpy( m_JointDataList[i].m_LocalScale, pSeek, sizeof(f32) * 9 );
          pSeek += sizeof(f32) * 9;

          m_JointDataList[i].m_LocalMatrix = math::Matrix34::GetRotationZYX(m_JointDataList[i].m_LocalRotation[2], m_JointDataList[i].m_LocalRotation[1], m_JointDataList[i].m_LocalRotation[0]);
          
          m_JointDataList[i].m_LocalMatrix[0][0] *= m_JointDataList[i].m_LocalScale[0];
          m_JointDataList[i].m_LocalMatrix[1][0] *= m_JointDataList[i].m_LocalScale[0];
          m_JointDataList[i].m_LocalMatrix[2][0] *= m_JointDataList[i].m_LocalScale[0];

          m_JointDataList[i].m_LocalMatrix[0][1] *= m_JointDataList[i].m_LocalScale[1];
          m_JointDataList[i].m_LocalMatrix[1][1] *= m_JointDataList[i].m_LocalScale[1];
          m_JointDataList[i].m_LocalMatrix[2][1] *= m_JointDataList[i].m_LocalScale[1];

          m_JointDataList[i].m_LocalMatrix[0][2] *= m_JointDataList[i].m_LocalScale[2];
          m_JointDataList[i].m_LocalMatrix[1][2] *= m_JointDataList[i].m_LocalScale[2];
          m_JointDataList[i].m_LocalMatrix[2][2] *= m_JointDataList[i].m_LocalScale[2];
          
          m_JointDataList[i].m_LocalMatrix[0][3] = m_JointDataList[i].m_LocalTranslate[0];
          m_JointDataList[i].m_LocalMatrix[1][3] = m_JointDataList[i].m_LocalTranslate[1];
          m_JointDataList[i].m_LocalMatrix[2][3] = m_JointDataList[i].m_LocalTranslate[2];

          m_JointDataList[i].m_GlobalMatrix = m_JointDataList[i].m_LocalMatrix;
          m_JointDataList[i].m_ParentIndex = -1;

          if ( parentNameLength )
          {
            for( u32 i2 = 0; i2 < i; ++i2 )
            {
              if ( std::strcmp( m_JointDataList[i].m_ParentName, m_JointDataList[i2].m_Name ) == 0 )
              {
                m_JointDataList[i].m_GlobalMatrix = m_JointDataList[i2].m_GlobalMatrix * m_JointDataList[i].m_LocalMatrix;
                m_JointDataList[i].m_ParentIndex = i2;
                break;
              }
            }

            if(m_JointDataList[i].m_ParentIndex < 0)
            {
              GFL_PRINT("Warning : Jointの順番が親子で逆転しています。\n");
              m_IsStableJoint = false;
            }
          }
        }
#else
        std::memcpy( m_JointDataList, pSeek, sizeof(JointData) * pJointsDataHeader->size );
#endif
      }
		}

#if defined(GF_PLATFORM_CTR)
		u32								readSize = (u32)pSeek - (u32)data;
		if ( readSize % 16 )
		{
			u32			paddingSize = 16 - (readSize % 16 );
			pSeek += paddingSize;
		}

		{//material common
			u32				*pHeader = (u32*)pSeek;
			m_LutTableNum = pHeader[0];
			m_LutTableDataSize = pHeader[1];

			m_pLutTableDataTop = &pHeader[4];
		}
#endif
	}

#if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)

  for( u32 no = 0; no < this->GetShaderNum(); ++no )
  {
    const c8*                     pName = this->GetShaderName( no );
    GFL_ASSERT_MSG( pName, "文字列格納バッファが見つかりません。データ不正です。" );
    GFL_ASSERT_MSG( std::strlen(pName) < Constant::MODEL_ELEMENT_NAME_MAX, "シェーダー名が%d文字を超えています。リソースを修正してください。", Constant::MODEL_ELEMENT_NAME_MAX);
  }

  for( u32 no = 0; no < this->GetTextureNum(); ++no )
  {
    const c8*                     pName = this->GetTextureName( no );
    GFL_ASSERT_MSG( pName, "文字列格納バッファが見つかりません。データ不正です。" );
    GFL_ASSERT_MSG( std::strlen(pName) < Constant::MODEL_ELEMENT_NAME_MAX, "テクスチャ名が%d文字を超えています。リソースを修正してください。", Constant::MODEL_ELEMENT_NAME_MAX);
  }

  for( u32 no = 0; no < this->GetMaterialNum(); ++no )
  {
    const c8*                     pName = this->GetMaterialName( no );
    GFL_ASSERT_MSG( pName, "文字列格納バッファが見つかりません。データ不正です。" );
    GFL_ASSERT_MSG( std::strlen(pName) < Constant::MODEL_ELEMENT_NAME_MAX, "マテリアル名が%d文字を超えています。リソースを修正してください。", Constant::MODEL_ELEMENT_NAME_MAX);
  }

  for( u32 no = 0; no < this->GetMeshNum(); ++no )
  {
    const c8*                     pName = this->GetMeshName( no );
    GFL_ASSERT_MSG( pName, "文字列格納バッファが見つかりません。データ不正です。" );
    GFL_ASSERT_MSG( std::strlen(pName) < Constant::MODEL_ELEMENT_NAME_MAX, "メッシュ名が%d文字を超えています。リソースを修正してください。", Constant::MODEL_ELEMENT_NAME_MAX);
  }

#endif  //  GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP
}

const c8* GfMdlResourceNode::GetShaderName( u32 no ) const 
{
	return m_pElementNameList[no].m_Name;
}

const c8* GfMdlResourceNode::GetTextureName( u32 no ) const 
{
	no += m_ShaderNum;
	return m_pElementNameList[no].m_Name;
}

const c8* GfMdlResourceNode::GetMaterialName( u32 no ) const 
{
	no += m_ShaderNum + m_TextureNum;
	return m_pElementNameList[no].m_Name;
}

const c8* GfMdlResourceNode::GetMeshName( u32 no ) const 
{
	no += m_ShaderNum + m_TextureNum + m_MaterialNum;
	return m_pElementNameList[no].m_Name;
}

const GfMdlResourceNode::JointData* GfMdlResourceNode::GetJointData( u32 no ) const
{
	return &m_JointDataList[no];
}

util::FnvHash GfMdlResourceNode::GetShaderNameHash( u32 no ) const 
{
	return m_pElementNameList[no].m_NameHash;
}

util::FnvHash GfMdlResourceNode::GetTextureNameHash( u32 no ) const 
{
	no += m_ShaderNum;
	return m_pElementNameList[no].m_NameHash;
}

util::FnvHash GfMdlResourceNode::GetMaterialNameHash( u32 no ) const 
{
	no += m_ShaderNum + m_TextureNum;
	return m_pElementNameList[no].m_NameHash;
}

util::FnvHash GfMdlResourceNode::GetMeshNameHash( u32 no ) const 
{
	no += m_ShaderNum + m_TextureNum + m_MaterialNum;
	return m_pElementNameList[no].m_NameHash;
}

const u32* GfMdlResourceNode::GetLutTableCommand( u32 nameHash ) const
{
	u32				*pSeek32 = m_pLutTableDataTop;
	u32				*pCommand = NULL;

	for( u32 i = 0; i < m_LutTableNum; ++i )
	{
		u32				thisHash = *pSeek32;
		if ( thisHash == nameHash )
		{
			pCommand = &pSeek32[4];
			break;
		}

		pSeek32 += 4;
		pSeek32 += m_LutTableDataSize / sizeof(u32);
	}

	return pCommand;
}

}}}}

