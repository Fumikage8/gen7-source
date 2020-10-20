#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {


#if defined(GF_PLATFORM_CTR)
void MaterialResourceNodeFactory::BinaryFileAccessor10::SetAttributeParam( gfx::IGLAllocator *pAllocator, MaterialResourceNode::MaterialResourceData *pMaterialResourceData, const void *pSrc )
{
	const u8					*pSeek = reinterpret_cast<const u8*>(pSrc);
	u8								nameLength;

	std::memset( &pMaterialResourceData->m_Header, 0, sizeof(MaterialResourceNode::Header) );

	{
		std::memcpy( &pMaterialResourceData->m_Header.nameHash, pSeek, sizeof(u32) );
		pSeek += sizeof(u32);
		nameLength = pSeek[0];
		++pSeek;
		std::memcpy( pMaterialResourceData->m_Header.nameBuffer, pSeek, nameLength );
		pSeek += nameLength;
	}

	pMaterialResourceData->m_Header.typeNameHash = gfl2::util::ConstHash< 'C', 'o', 'm', 'b', 'C', 't', 'r'>::value;
	std::strcat( pMaterialResourceData->m_Header.typeNameBuffer, "CombCtr" );

	if ( this->m_VersionId >= GfModelVersion::OptimizeAndAddUserData )
	{
		pSeek += sizeof(u32);
		nameLength = pSeek[0];
		++pSeek;
		pSeek += nameLength;

		{
			std::memcpy( &pMaterialResourceData->m_Header.shaderNameHash, pSeek, sizeof(u32) );
			pSeek += sizeof(u32);
			nameLength = pSeek[0];
			++pSeek;
			std::memcpy( pMaterialResourceData->m_Header.shaderNameBuffer, pSeek, nameLength );
			pSeek += nameLength;
		}

		{
			std::memcpy( &pMaterialResourceData->m_FShaderNameHash, pSeek, sizeof(u32) );
			pSeek += sizeof(u32);
			nameLength = pSeek[0];
			++pSeek;
			std::memcpy( pMaterialResourceData->m_FShaderNameBuffer, pSeek, nameLength );
			pSeek += nameLength;
		}

		if ( this->m_VersionId >= GfModelVersion::AddLutHashBumpNo )
		{
      MaterialResourceNodeFactory::BinaryFileAccessor10::AttributeParamSlim				*pAttributeParamSlim = (MaterialResourceNodeFactory::BinaryFileAccessor10::AttributeParamSlim*)pSeek;
			s32																							readSize = MaterialResourceNodeFactory::BinaryFileAccessor10::AttributeParamSlim::s_FirstSize;

			pMaterialResourceData->m_LutCommandNameHash[0] = pAttributeParamSlim->m_LutTextureNameHash[0];
			pMaterialResourceData->m_LutCommandNameHash[1] = pAttributeParamSlim->m_LutTextureNameHash[1];
			pMaterialResourceData->m_LutCommandNameHash[2] = pAttributeParamSlim->m_LutTextureNameHash[2];
			pMaterialResourceData->m_AttributeParam.m_BumpMapNo = pAttributeParamSlim->m_BumpMapNo;

			if ( this->m_VersionId >= GfModelVersion::SupportConstantColorAnim )
			{
        math::Vector4         f32Color;

				for( u32 stageNo = 0; stageNo < CombinerAttribute::CombinerMax; ++stageNo )
				{
					pMaterialResourceData->m_AttributeParam.m_TextureCombinerConstant[stageNo] = pAttributeParamSlim->m_ConstantColorIndex[stageNo];

          f32Color.Set( pAttributeParamSlim->m_ConstantColor[stageNo].x, pAttributeParamSlim->m_ConstantColor[stageNo].y, pAttributeParamSlim->m_ConstantColor[stageNo].z, pAttributeParamSlim->m_ConstantColor[stageNo].w );
          pMaterialResourceData->m_AttributeParam.m_ConstantColor[stageNo] = f32Color;
				}

				readSize = MaterialResourceNodeFactory::BinaryFileAccessor10::AttributeParamSlim::s_SupportConstantColorAnimSize;

				if ( this->m_VersionId >= GfModelVersion::VertexShaderUpdate15031412 )
				{
					pMaterialResourceData->m_AttributeParam.m_LightSetNo = pAttributeParamSlim->m_LightSetNo;
					pMaterialResourceData->m_AttributeParam.m_VsLightingEnable = pAttributeParamSlim->m_VsLightingEnable;
					pMaterialResourceData->m_AttributeParam.m_PsLightingEnable = pAttributeParamSlim->m_PsLightingEnable;
					pMaterialResourceData->m_AttributeParam.m_FogEnable = pAttributeParamSlim->m_FogEnable;
					pMaterialResourceData->m_AttributeParam.m_FogNo = pAttributeParamSlim->m_FogNo;

					readSize = MaterialResourceNodeFactory::BinaryFileAccessor10::AttributeParamSlim::s_VertexShaderUpdate15031412Size;

					if ( this->m_VersionId >= GfModelVersion::VertexShaderUpdate15041820 )
					{
						for( u32 i = 0; i < 4; ++i )
							pMaterialResourceData->m_VertexColorEnable[i] = pAttributeParamSlim->m_VertexColorEnable[i];
						readSize = MaterialResourceNodeFactory::BinaryFileAccessor10::AttributeParamSlim::s_VertexShaderUpdate15041820Size;

						if ( this->m_VersionId >= GfModelVersion::SupportShadowTexture15090718 )
						{
							pMaterialResourceData->m_AttributeParam.m_UseShadowTexture = pAttributeParamSlim->m_UseShadowTexture;

							readSize = MaterialResourceNodeFactory::BinaryFileAccessor10::AttributeParamSlim::s_SupportShadowTexture15090718Size;

              if ( this->m_VersionId >= GfModelVersion::SupportMaterialColor15101716 )
						  {
                f32Color.Set( pAttributeParamSlim->m_AmbientColor.x, pAttributeParamSlim->m_AmbientColor.y, pAttributeParamSlim->m_AmbientColor.z, pAttributeParamSlim->m_AmbientColor.w );
                pMaterialResourceData->m_AttributeParam.m_MaterialColor.m_Ambient = f32Color;

                f32Color.Set( pAttributeParamSlim->m_EmissionColor.x, pAttributeParamSlim->m_EmissionColor.y, pAttributeParamSlim->m_EmissionColor.z, pAttributeParamSlim->m_EmissionColor.w );
                pMaterialResourceData->m_AttributeParam.m_MaterialColor.m_Emission = f32Color;

                f32Color.Set( pAttributeParamSlim->m_DiffuseColor.x, pAttributeParamSlim->m_DiffuseColor.y, pAttributeParamSlim->m_DiffuseColor.z, pAttributeParamSlim->m_DiffuseColor.w );
                pMaterialResourceData->m_AttributeParam.m_MaterialColor.m_Diffuse = f32Color;

							  readSize = MaterialResourceNodeFactory::BinaryFileAccessor10::AttributeParamSlim::s_SupportMaterialColor15101716Size;
						  }
						}
					}
				}
			}
			
			pSeek += readSize;
		}

		u32					size = sizeof(pMaterialResourceData->m_UserData);
		std::memcpy( &pMaterialResourceData->m_UserData, pSeek, size );
		pSeek += 96;
	}
	else
	{
		std::memcpy( &pMaterialResourceData->m_Header.shaderNameHash, pSeek, sizeof(u32) );
		pSeek += sizeof(u32);
		nameLength = pSeek[0];
		++pSeek;
		std::memcpy( pMaterialResourceData->m_Header.shaderNameBuffer, pSeek, nameLength );
		pSeek += nameLength;
	}

	std::memcpy( &pMaterialResourceData->m_TextureNum, pSeek, sizeof(u32) );
	pSeek += sizeof( pMaterialResourceData->m_TextureNum );

	pMaterialResourceData->m_pTextureInfoList = reinterpret_cast<MaterialResourceNode::TextureInfo*>( pAllocator->SystemMemoryMalloc( sizeof( MaterialResourceNode::TextureInfo ) * pMaterialResourceData->m_TextureNum ) );
	std::memset( pMaterialResourceData->m_pTextureInfoList, 0, sizeof( MaterialResourceNode::TextureInfo ) * pMaterialResourceData->m_TextureNum );

	for( u32 i = 0; i < pMaterialResourceData->m_TextureNum; ++i )
	{
		MaterialResourceNode::TextureInfo& textureInfo = pMaterialResourceData->m_pTextureInfoList[i];

		std::memcpy( &textureInfo.m_TextureNameHash, pSeek, sizeof(u32) );
		pSeek += sizeof(u32);
		nameLength = pSeek[0];
		++pSeek;
		if ( nameLength )
			std::memcpy( textureInfo.m_TextureName, pSeek, nameLength );
		pSeek += nameLength;

		textureInfo.m_SlotNo = pSeek[0];
		++pSeek;

		if ( this->m_VersionId >= GfModelVersion::VertexShaderUpdate15032914 )
		{
			textureInfo.m_Attribute.m_MappingType = pSeek[0];
			++pSeek;
		}

		u32			readSize = 5;
		if ( this->m_VersionId >= GfModelVersion::AddWrapFilerState )
			readSize = 9;
		if ( this->m_VersionId >= GfModelVersion::MipMapBias15072420 )
			readSize = 10;
		std::memcpy( &textureInfo.m_Attribute.m_ScaleU, pSeek, sizeof(f32) * readSize );
		pSeek += sizeof(f32) * readSize;
	}

	u32								readSize = (u32)pSeek - (u32)pSrc;
	if ( readSize % 16 )
	{
		u32			paddingSize = 16 - (readSize %16 );
		pSeek += paddingSize;
	}

	{
		const u32				*pCommandHeader = reinterpret_cast<const u32*>( pSeek );
		pSeek += sizeof(u32) * 8;

		pMaterialResourceData->m_CommandLength = pCommandHeader[0];
		pMaterialResourceData->m_AttributeParam.m_Priority = pCommandHeader[1];
		pMaterialResourceData->m_AttributeParam.m_MaterialHash = pCommandHeader[2];
		pMaterialResourceData->m_BlendTest.m_DrawLayer = static_cast<scenegraph::resource::DrawLayer>(pCommandHeader[3]);

		pMaterialResourceData->m_LutCommandNameHash[0] = pCommandHeader[4];
		pMaterialResourceData->m_LutCommandNameHash[1] = pCommandHeader[5];
		pMaterialResourceData->m_LutCommandNameHash[2] = pCommandHeader[6];

		pMaterialResourceData->m_pMaterialCommandTop = reinterpret_cast<const u8*>( pSeek );
	}
}

void MaterialResourceNodeFactory::BinaryFileAccessor11::SetAttributeParam( gfx::IGLAllocator *pAllocator, MaterialResourceNode::MaterialResourceData *pMaterialResourceData, const void *pSrc )
{
  const u8					*pSeek = reinterpret_cast<const u8*>(pSrc);
	u8								nameLength;

	std::memset( &pMaterialResourceData->m_Header, 0, sizeof(MaterialResourceNode::Header) );

	{
		std::memcpy( &pMaterialResourceData->m_Header.nameHash, pSeek, sizeof(u32) );
		pSeek += sizeof(u32);
		nameLength = pSeek[0];
		++pSeek;
		std::memcpy( pMaterialResourceData->m_Header.nameBuffer, pSeek, nameLength );
		pSeek += nameLength;
	}

	pMaterialResourceData->m_Header.typeNameHash = gfl2::util::ConstHash< 'C', 'o', 'm', 'b', 'C', 't', 'r'>::value;
	std::strcat( pMaterialResourceData->m_Header.typeNameBuffer, "CombCtr" );

	{
		pSeek += sizeof(u32);
		nameLength = pSeek[0];
		++pSeek;
		pSeek += nameLength;

		{
			std::memcpy( &pMaterialResourceData->m_Header.shaderNameHash, pSeek, sizeof(u32) );
			pSeek += sizeof(u32);
			nameLength = pSeek[0];
			++pSeek;
			std::memcpy( pMaterialResourceData->m_Header.shaderNameBuffer, pSeek, nameLength );
			pSeek += nameLength;
		}

		{
			std::memcpy( &pMaterialResourceData->m_FShaderNameHash, pSeek, sizeof(u32) );
			pSeek += sizeof(u32);
			nameLength = pSeek[0];
			++pSeek;
			std::memcpy( pMaterialResourceData->m_FShaderNameBuffer, pSeek, nameLength );
			pSeek += nameLength;
		}

		{
      MaterialResourceNodeFactory::BinaryFileAccessor11::AttributeParamSlim				*pAttributeParamSlim = (MaterialResourceNodeFactory::BinaryFileAccessor11::AttributeParamSlim*)pSeek;

			pMaterialResourceData->m_LutCommandNameHash[0] = pAttributeParamSlim->m_LutTextureNameHash[0];
			pMaterialResourceData->m_LutCommandNameHash[1] = pAttributeParamSlim->m_LutTextureNameHash[1];
			pMaterialResourceData->m_LutCommandNameHash[2] = pAttributeParamSlim->m_LutTextureNameHash[2];

			pMaterialResourceData->m_AttributeParam.m_BumpMapNo = pAttributeParamSlim->m_BumpMapNo;


			for( u32 stageNo = 0; stageNo < CombinerAttribute::CombinerMax; ++stageNo )
			{
				pMaterialResourceData->m_AttributeParam.m_TextureCombinerConstant[stageNo] = pAttributeParamSlim->m_ConstantColorIndex[stageNo];
        pMaterialResourceData->m_AttributeParam.m_ConstantColor[stageNo] = pAttributeParamSlim->m_ConstantColor[stageNo];
			}

			pMaterialResourceData->m_AttributeParam.m_LightSetNo = pAttributeParamSlim->m_LightSetNo;
			pMaterialResourceData->m_AttributeParam.m_VsLightingEnable = pAttributeParamSlim->m_VsLightingEnable;
			pMaterialResourceData->m_AttributeParam.m_PsLightingEnable = pAttributeParamSlim->m_PsLightingEnable;
			pMaterialResourceData->m_AttributeParam.m_FogEnable = pAttributeParamSlim->m_FogEnable;
			pMaterialResourceData->m_AttributeParam.m_FogNo = pAttributeParamSlim->m_FogNo;

		  for( u32 i = 0; i < 4; ++i )
      {
			  pMaterialResourceData->m_VertexColorEnable[i] = pAttributeParamSlim->m_VertexColorEnable[i];
      }

			pMaterialResourceData->m_AttributeParam.m_UseShadowTexture = pAttributeParamSlim->m_UseShadowTexture;
      pMaterialResourceData->m_AttributeParam.m_MaterialColor.m_Ambient = pAttributeParamSlim->m_AmbientColor;
      pMaterialResourceData->m_AttributeParam.m_MaterialColor.m_Emission = pAttributeParamSlim->m_EmissionColor;
      pMaterialResourceData->m_AttributeParam.m_MaterialColor.m_Diffuse = pAttributeParamSlim->m_DiffuseColor;
			
			pSeek += sizeof(MaterialResourceNodeFactory::BinaryFileAccessor11::AttributeParamSlim);
		}

    {
		  u32					size = sizeof(pMaterialResourceData->m_UserData);
		  std::memcpy( &pMaterialResourceData->m_UserData, pSeek, size );
		  pSeek += size;
    }
	}

	std::memcpy( &pMaterialResourceData->m_TextureNum, pSeek, sizeof(u32) );
	pSeek += sizeof( pMaterialResourceData->m_TextureNum );

	pMaterialResourceData->m_pTextureInfoList = reinterpret_cast<MaterialResourceNode::TextureInfo*>( pAllocator->SystemMemoryMalloc( sizeof( MaterialResourceNode::TextureInfo ) * pMaterialResourceData->m_TextureNum ) );
	std::memset( pMaterialResourceData->m_pTextureInfoList, 0, sizeof( MaterialResourceNode::TextureInfo ) * pMaterialResourceData->m_TextureNum );

	for( u32 i = 0; i < pMaterialResourceData->m_TextureNum; ++i )
	{
		MaterialResourceNode::TextureInfo& textureInfo = pMaterialResourceData->m_pTextureInfoList[i];

		std::memcpy( &textureInfo.m_TextureNameHash, pSeek, sizeof(u32) );
		pSeek += sizeof(u32);
		nameLength = pSeek[0];
		++pSeek;
		if ( nameLength )
			std::memcpy( textureInfo.m_TextureName, pSeek, nameLength );
		pSeek += nameLength;

		textureInfo.m_SlotNo = pSeek[0];
		++pSeek;

		textureInfo.m_Attribute.m_MappingType = pSeek[0];
		++pSeek;

		u32			readSize = 10;//この辺りは見にくいので構造体化する
		std::memcpy( &textureInfo.m_Attribute.m_ScaleU, pSeek, sizeof(f32) * readSize );
		pSeek += sizeof(f32) * readSize;
	}

	u32								readSize = (u32)pSeek - (u32)pSrc;
	if ( readSize % 16 )
	{
		u32			paddingSize = 16 - (readSize %16 );
		pSeek += paddingSize;
	}

	{
		const u32				*pCommandHeader = reinterpret_cast<const u32*>( pSeek );
		pSeek += sizeof(u32) * 8;

		pMaterialResourceData->m_CommandLength = pCommandHeader[0];
		pMaterialResourceData->m_AttributeParam.m_Priority = pCommandHeader[1];
		pMaterialResourceData->m_AttributeParam.m_MaterialHash = pCommandHeader[2];
		pMaterialResourceData->m_BlendTest.m_DrawLayer = static_cast<scenegraph::resource::DrawLayer>(pCommandHeader[3]);

		pMaterialResourceData->m_LutCommandNameHash[0] = pCommandHeader[4];
		pMaterialResourceData->m_LutCommandNameHash[1] = pCommandHeader[5];
		pMaterialResourceData->m_LutCommandNameHash[2] = pCommandHeader[6];

		pMaterialResourceData->m_pMaterialCommandTop = reinterpret_cast<const u8*>( pSeek );
	}
}
#elif defined(GF_PLATFORM_WIN32)
void MaterialResourceNodeFactory::BinaryFileAccessor10::SetAttributeParam( gfx::IGLAllocator *pAllocator, MaterialResourceNode::MaterialResourceData *pMaterialResourceData, const void *pSrc )
{
  GFL_ASSERT( m_VersionId <= GfModelVersion::MeshOptCTR15103116 );

	const u8									*pSeek = reinterpret_cast<const u8*>(pSrc);

	std::memcpy( &pMaterialResourceData->m_Header, pSeek, sizeof(MaterialResourceNode::Header) );
	pSeek += sizeof( pMaterialResourceData->m_Header );

	if ( this->m_VersionId >= GfModelVersion::OptimizeAndAddUserData )
	{
		std::memcpy( &pMaterialResourceData->m_FShaderNameHash, pSeek, sizeof(pMaterialResourceData->m_FShaderNameHash) );
		pSeek += sizeof( pMaterialResourceData->m_FShaderNameHash );

		std::memcpy( &pMaterialResourceData->m_FShaderNameBuffer, pSeek, sizeof(pMaterialResourceData->m_FShaderNameBuffer) );
		pSeek += sizeof( pMaterialResourceData->m_FShaderNameBuffer );
	}

	std::memcpy( &pMaterialResourceData->m_TextureNum, pSeek, sizeof(u32) );
	pSeek += sizeof( pMaterialResourceData->m_TextureNum );

  pMaterialResourceData->m_pTextureInfoList = NULL;
  if (pMaterialResourceData->m_TextureNum > 0)
  {
    pMaterialResourceData->m_pTextureInfoList = reinterpret_cast<MaterialResourceNode::TextureInfo*>( pAllocator->SystemMemoryMalloc( sizeof( MaterialResourceNode::TextureInfo ) * pMaterialResourceData->m_TextureNum ) );
  }

	for( u32 i = 0; i < pMaterialResourceData->m_TextureNum; ++i )
	{
		u32			size = sizeof(MaterialResourceNode::TextureInfo) - sizeof(s32);

		if ( this->m_VersionId >= GfModelVersion::MipMapBias15072420 )
			size = sizeof(MaterialResourceNode::TextureInfo);

		pMaterialResourceData->m_pTextureInfoList[i].m_Attribute.m_MipBias = 0;
		std::memcpy( &pMaterialResourceData->m_pTextureInfoList[i], pSeek, size );
		pSeek += size;
	}

  {// AttributeParam(の設定
    BinaryFileAccessor10::AttributeParam        attributeParam;

		std::memcpy( &attributeParam, pSeek, sizeof(BinaryFileAccessor10::AttributeParam) );

    pMaterialResourceData->m_AttributeParam.m_Priority = attributeParam.m_Priority;
    pMaterialResourceData->m_AttributeParam.m_MaterialHash = attributeParam.m_MaterialHash;
    pMaterialResourceData->m_AttributeParam.m_PsLightingEnable = attributeParam.m_PsLightingEnable;
    pMaterialResourceData->m_AttributeParam.m_VsLightingEnable = attributeParam.m_VsLightingEnable;
    pMaterialResourceData->m_AttributeParam.m_LightSetNo = attributeParam.m_LightSetNo;
    pMaterialResourceData->m_AttributeParam.m_FogEnable = attributeParam.m_FogEnable;
    pMaterialResourceData->m_AttributeParam.m_FogNo = attributeParam.m_FogNo;
    pMaterialResourceData->m_AttributeParam.m_CullMode = attributeParam.m_CullMode;

    pMaterialResourceData->m_AttributeParam.m_MaterialColor.m_Ambient = attributeParam.m_MaterialColor.m_Ambient;
    pMaterialResourceData->m_AttributeParam.m_MaterialColor.m_Diffuse = attributeParam.m_MaterialColor.m_Diffuse;
    pMaterialResourceData->m_AttributeParam.m_MaterialColor.m_Emission = attributeParam.m_MaterialColor.m_Emission;
    pMaterialResourceData->m_AttributeParam.m_MaterialColor.m_Specular0 = attributeParam.m_MaterialColor.m_Specular0;
    pMaterialResourceData->m_AttributeParam.m_MaterialColor.m_Specular1 = attributeParam.m_MaterialColor.m_Specular1;

    for( u32 i = 0; i < CombinerAttribute::ConstantMax; ++i )
      pMaterialResourceData->m_AttributeParam.m_ConstantColor[i] = attributeParam.m_ConstantColor[i];

    pMaterialResourceData->m_AttributeParam.m_EtcColor.m_BlendColor = attributeParam.m_EtcColor.m_BlendColor;
    pMaterialResourceData->m_AttributeParam.m_EtcColor.m_BufferColor = attributeParam.m_EtcColor.m_BufferColor;
    for( u32 i = 0; i < CombinerAttribute::ConstantMax; ++i )
      pMaterialResourceData->m_AttributeParam.m_EtcColor.m_TextureBorderColor[i] = attributeParam.m_EtcColor.m_TextureBorderColor[i];

    pMaterialResourceData->m_AttributeParam.m_BumpMapNo = attributeParam.m_BumpMapNo;
    pMaterialResourceData->m_AttributeParam.m_BumpMapType = attributeParam.m_BumpMapType;
    pMaterialResourceData->m_AttributeParam.m_ZRecastEnable = attributeParam.m_ZRecastEnable;

    for( u32 i = 0; i < CombinerAttribute::TextureMax; ++i )
    {
      pMaterialResourceData->m_AttributeParam.m_TextureAttribute[i] = attributeParam.m_TextureAttribute[i];
      pMaterialResourceData->m_AttributeParam.m_TextureAttributeEnable[i] = attributeParam.m_TextureAttributeEnable[i];
    }

    for( u32 i = 0; i < LookUpTableType::NumberOf; ++i )
    {
      pMaterialResourceData->m_AttributeParam.m_LookUpTableEnable[i] = attributeParam.m_LookUpTableEnable[i];
      pMaterialResourceData->m_AttributeParam.m_LookUpTable[i] = attributeParam.m_LookUpTable[i];
    }

    pMaterialResourceData->m_AttributeParam.m_PrimaryFresnelEnable = attributeParam.m_PrimaryFresnelEnable;
    pMaterialResourceData->m_AttributeParam.m_SecondaryFresnelEnable = attributeParam.m_SecondaryFresnelEnable;
    pMaterialResourceData->m_AttributeParam.m_SpecularColorClamp = attributeParam.m_SpecularColorClamp;
    pMaterialResourceData->m_AttributeParam.m_CookTorrance0Enable = attributeParam.m_CookTorrance0Enable;
    pMaterialResourceData->m_AttributeParam.m_CookTorrance1Enable = attributeParam.m_CookTorrance1Enable;

    for( u32 i = 0; i < CombinerColorType::NumberOf; ++i )
    {
      for( u32 i2 = 0; i2 < CombinerAttribute::CombinerMax; ++i2 )
      {
        pMaterialResourceData->m_AttributeParam.m_TextureCombinerAttribute[i][i2] = attributeParam.m_TextureCombinerAttribute[i][i2];
      }
    }

    for( u32 i = 0; i < CombinerAttribute::CombinerMax; ++i )
    {
      pMaterialResourceData->m_AttributeParam.m_TextureCombinerConstant[i] = attributeParam.m_TextureCombinerConstant[i];
    }

    pMaterialResourceData->m_AttributeParam.m_IsPokemonShader = attributeParam.m_IsPokemonShader;
    pMaterialResourceData->m_AttributeParam.m_UseShadowTexture = attributeParam.m_UseShadowTexture;
    pMaterialResourceData->m_AttributeParam.m_UseObjectSpaceNormalMap = attributeParam.m_UseObjectSpaceNormalMap;
    pMaterialResourceData->m_AttributeParam.m_HighLightTextureEnable = attributeParam.m_HighLightTextureEnable;
    pMaterialResourceData->m_AttributeParam.m_OutLineColorTextureEnable = attributeParam.m_OutLineColorTextureEnable;
    pMaterialResourceData->m_AttributeParam.m_BlurScaleTextureEnable = attributeParam.m_BlurScaleTextureEnable;
    pMaterialResourceData->m_AttributeParam.m_PokeNormalType = attributeParam.m_PokeNormalType;
    pMaterialResourceData->m_AttributeParam.m_BlendType = attributeParam.m_BlendType;
    pMaterialResourceData->m_AttributeParam.m_UpdateShaderParam = attributeParam.m_UpdateShaderParam;
    
		pSeek += sizeof(BinaryFileAccessor10::AttributeParam);
	}

	std::memcpy( &pMaterialResourceData->m_BlendTest, pSeek, sizeof(pMaterialResourceData->m_BlendTest) );
	pSeek += sizeof(pMaterialResourceData->m_BlendTest);

  std::memcpy( &pMaterialResourceData->m_UserData, pSeek, sizeof(pMaterialResourceData->m_UserData) );
	pSeek += sizeof(pMaterialResourceData->m_UserData);
}

void MaterialResourceNodeFactory::BinaryFileAccessor11::SetAttributeParam( gfx::IGLAllocator *pAllocator, MaterialResourceNode::MaterialResourceData *pMaterialResourceData, const void *pSrc )
{
  GFL_ASSERT( m_VersionId <= GfModelVersion::PaddingColorIsWhite );

	const u8									*pSeek = reinterpret_cast<const u8*>(pSrc);

	std::memcpy( &pMaterialResourceData->m_Header, pSeek, sizeof(MaterialResourceNode::Header) );
	pSeek += sizeof( pMaterialResourceData->m_Header );

	std::memcpy( &pMaterialResourceData->m_FShaderNameHash, pSeek, sizeof(pMaterialResourceData->m_FShaderNameHash) );
	pSeek += sizeof( pMaterialResourceData->m_FShaderNameHash );

	std::memcpy( &pMaterialResourceData->m_FShaderNameBuffer, pSeek, sizeof(pMaterialResourceData->m_FShaderNameBuffer) );
	pSeek += sizeof( pMaterialResourceData->m_FShaderNameBuffer );

	std::memcpy( &pMaterialResourceData->m_TextureNum, pSeek, sizeof(u32) );
	pSeek += sizeof( pMaterialResourceData->m_TextureNum );

  pMaterialResourceData->m_pTextureInfoList = NULL;
  if (pMaterialResourceData->m_TextureNum > 0)
  {
    pMaterialResourceData->m_pTextureInfoList = reinterpret_cast<MaterialResourceNode::TextureInfo*>( pAllocator->SystemMemoryMalloc( sizeof( MaterialResourceNode::TextureInfo ) * pMaterialResourceData->m_TextureNum ) );
  }

	for( u32 i = 0; i < pMaterialResourceData->m_TextureNum; ++i )
	{
		u32			size = sizeof(MaterialResourceNode::TextureInfo);

		pMaterialResourceData->m_pTextureInfoList[i].m_Attribute.m_MipBias = 0;
		std::memcpy( &pMaterialResourceData->m_pTextureInfoList[i], pSeek, size );
		pSeek += size;
	}

  {// AttributeParamの設定
    MaterialResourceNode::AttributeParam        attributeParam;

		std::memcpy( &pMaterialResourceData->m_AttributeParam, pSeek, sizeof(MaterialResourceNode::AttributeParam) );

		pSeek += sizeof(MaterialResourceNode::AttributeParam);
	}

	std::memcpy( &pMaterialResourceData->m_BlendTest, pSeek, sizeof(pMaterialResourceData->m_BlendTest) );
	pSeek += sizeof(pMaterialResourceData->m_BlendTest);

  std::memcpy( &pMaterialResourceData->m_UserData, pSeek, sizeof(pMaterialResourceData->m_UserData) );
	pSeek += sizeof(pMaterialResourceData->m_UserData);
}

#endif

/*ResourceNode* MaterialResourceNodeFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size );
}*/
ResourceNode* MaterialResourceNodeFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
	GFL_ASSERT(data);
  gfx::GLMemory::SetInstanceAllocator( pAllocator );
	MaterialResourceNode				*pNode = new MaterialResourceNode();


  

	pNode->SetVersionId( this->m_DataVersionId );
	
  switch( m_DataVersionId ){
  case GfModelVersion::FirstVersion:
	case GfModelVersion::OptimizeAndAddUserData:
	case GfModelVersion::AddWrapFilerState:
	case GfModelVersion::AddLutHashBumpNo:
	case GfModelVersion::SupportVsBoolMap:
	case GfModelVersion::SupportConstantColorAnim:
	case GfModelVersion::VertexShaderUpdate15031412://! フィールド向けデフォルトシェーダー対応
	case GfModelVersion::VertexShaderUpdate15032914://! デフォルトシェーダーのスフィアマップ対応
	case GfModelVersion::TextureCacheOpt15032918://! テクスチャキャッシュの有効利用
	case GfModelVersion::PokeShaderLutLightingFix://! Pokeシェーダー使用時のLut調整対応
	case GfModelVersion::VertexShaderUpdate15041820://! Pokeシェーダー最適化
	case GfModelVersion::EncountKusaUpdate15052316://! エンカウント草モデル対応
	case GfModelVersion::BillBordModeUpdate15052616://! ビルボードモデル対応
	case GfModelVersion::SegmentScale15061117://! セグメントスケール対応
  case GfModelVersion::BillboardType15072416://! Billboard Data Added (2Bit, 0-3)
	case GfModelVersion::MipMapBias15072420://! MipMapBias対応
	case GfModelVersion::MultiUvAdjust15090112://! ＵＶ番号自動設定マルチＵＶサポート
	case GfModelVersion::SupportShadowTexture15090718://! 影テクスチャ付きモデルのカラーシェーダーサポート
  case GfModelVersion::SupportMaterialColor15101716://! マテリアルカラー対応（Emission, Ambient, Diffuse）
  case GfModelVersion::MeshOptCTR15103116://! ファイヤーシェーダー用メッシュのストリップ化はCTRのみとする
    {
      BinaryFileAccessor10      binaryFileAccessor10( m_DataVersionId );
      pNode->SetData( &binaryFileAccessor10, data );
    }
    break;
  case GfModelVersion::ColorU8CTR15111314://! ColorU8によるデータ圧縮  
  case GfModelVersion::FireShaderScaleCTR15112017://! 頂点カラーによる1.0以上のファイヤーシェーダースケール対応
  case GfModelVersion::PaddingColorIsWhite://! 頂点カラーを持たないメッシュのパディングを白に変更
    {
      BinaryFileAccessor11      binaryFileAccessor11( m_DataVersionId );
      pNode->SetData( &binaryFileAccessor11, data );
    }
    break;
  default:
    GFL_ASSERT( 0 );
    break;
  };

  gfx::GLMemory::ClearInstanceAllocator();
  
  pNode->CreateStateObject();

	SceneGraphManager::AddChild( pNode );

	return pNode;
}

b32 MaterialResourceNodeFactory::IsSupport( IResourceData::Type type )
{
	static const IResourceData::Type		supportType("material");
	return (type == supportType);
}

}}}}



