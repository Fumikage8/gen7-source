#include <renderer/include/gfl2_OutLinePostRenderPath.h>

#include <gfx/include/gfl2_GFGLUtil.h>
#include <debug/include/gfl2_Assert.h>

#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>

#if defined(GF_PLATFORM_CTR)
	#include <nn/fs.h>
	#include <nn/hio.h>
#else
	#include <gfx/include/WinOpenGL/gfl2_WinOpenGLStateObject.h>
#endif


////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////
////////    以下のクラスがこのクラスを継承しています。
////////    System::NijiOutLinePostSceneRenderPath
////////    app::util::AppOutLinePostRenderPath
////////    このpoke_3d::renderer::OutLinePostRenderPathクラスを変更した際は
////////    上記のクラスに影響がないか必ず確認して下さい。
////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////


using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

struct LocalFunc{
	static void *ReadFile( const char* pFilePath )
  {//自前データロード
#if defined(GF_PLATFORM_CTR)
    nn::fs::FileReader file( pFilePath );
    u32 fileSize = file.GetSize();
    void* pShaderBinary = GLMemory::Malloc(fileSize, 128);
    file.Read(pShaderBinary, fileSize);
		return pShaderBinary;
#else
		FILE		*hFile;
		int			file_size;
		char		*read_buff;

		fopen_s( &hFile, pFilePath, "rb" );

		if ( hFile == NULL ){
			GFL_ASSERT(0);
			return NULL;
		}

		fseek( hFile, 0, SEEK_END );
		file_size = ftell( hFile );
		fseek( hFile, 0, SEEK_SET );

		u32		align = 16;
		file_size += align;//必ずNULLが入るようにしたい。
		u32 buf_size = file_size + ((-file_size) & (align-1));

		read_buff = reinterpret_cast<c8*>( GLMemory::Malloc(buf_size) );
		memset( read_buff, 0, buf_size );
		fread( read_buff, file_size, 1, hFile );
		fclose( hFile );

		return read_buff;
#endif
  }
};

namespace poke_3d { namespace renderer {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
OutLinePostRenderPath::OutLinePostRenderPath() : 
  IRenderPath()
{
  for( u32 i = 0; i < EdgeMapSceneRenderPath::EdgeColor::Max; i++)
  {
    m_pTexture[i] = NULL;
    m_pBinModelRootNode[i] = NULL;
    m_pModelInstanceNode[i] = NULL;
    m_EdgeColorEnableTable[i] = true;
  }


}

void OutLinePostRenderPath::Initialize(
    gfl2::gfx::IGLAllocator* pAllocator, 
    c8* gfbmdlBinaryNormal,
    c8** shaderBinariesNormal,
    s32 shaderNumNormal,
    c8* textureBinaryNormal,
    c8* gfbmdlBinaryId,
    c8** shaderBinariesId,
    s32 shaderNumId,
    c8* textureBinaryId
  )
{
	{
		{
			scenegraph::resource::GfBinaryMdlData							resourceData;

			resourceData.SetModelData( gfbmdlBinaryNormal );

			m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::Normal] = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &resourceData );	
		}

		{
			for( s32 shaderNo = 0; shaderNo < shaderNumNormal; ++shaderNo )
			{
				scenegraph::resource::GfBinaryShdData							resourceData;

				resourceData.SetShaderData( shaderBinariesNormal[shaderNo] );

				scenegraph::resource::ResourceNode		*pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &resourceData );
				m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::Normal]->AddChild( pBinShdNode );
			}
		}

		{
			scenegraph::resource::GfBinaryTexData							texResourceData;
			scenegraph::resource::ResourceNode								*pBinTexRootNode;

			texResourceData.SetTextureData( textureBinaryNormal );

			//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
			pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &texResourceData );
			m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::Normal]->AddChild( pBinTexRootNode );
		}
	}

	{
		{
			scenegraph::resource::GfBinaryMdlData							resourceData;

			resourceData.SetModelData( gfbmdlBinaryId );

			m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::ID] = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &resourceData );	
		}

		{
			for( s32 shaderNo = 0; shaderNo < shaderNumId; ++shaderNo )
			{
				scenegraph::resource::GfBinaryShdData							resourceData;

				resourceData.SetShaderData( shaderBinariesId[shaderNo] );

				scenegraph::resource::ResourceNode		*pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &resourceData );
				m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::ID]->AddChild( pBinShdNode );
			}
		}

		{
			scenegraph::resource::GfBinaryTexData							texResourceData;
			scenegraph::resource::ResourceNode								*pBinTexRootNode;

			texResourceData.SetTextureData( textureBinaryId );

			//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
			pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &texResourceData );
			m_pBinModelRootNode[EdgeMapSceneRenderPath::EdgeColor::ID]->AddChild( pBinTexRootNode );
		}
	}

  for( u32 i = 0; i < EdgeMapSceneRenderPath::EdgeColor::Max; ++i )
  {
    m_pModelInstanceNode[i] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( pAllocator, m_pBinModelRootNode[i] );

    scenegraph::instance::DrawableNode::UpdateDrawTagInfo updateDrawTagInfo;
    m_pModelInstanceNode[i]->UpdateDrawTag(updateDrawTagInfo);
    scenegraph::SceneGraphManager::RemoveChild(m_pModelInstanceNode[i]);
  }

#if defined(GF_PLATFORM_CTR)
	{
		//u32								materialNum = m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::ID]->GetMaterialNum();
		math::Vector2			uvList[2][3] = {
			{
				math::Vector2( 0.0f, 0.0f ),
				math::Vector2( 0.0039059999f, 0.0f ),
				math::Vector2( -0.0039059999f, 0.0f ),
			},

			{
				math::Vector2( 0.0f, 0.0f ),
				math::Vector2( 0.0f, 0.0019530000f ),
				math::Vector2( 0.0f, -0.0019530000f )
			}
		};

		for( u32 matNo = 0; matNo < 2; ++matNo )
		{
			scenegraph::instance::MaterialInstanceNode								*pMatInstance = m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::ID]->GetMaterialInstanceNode( matNo );

			for( u32 texNo = 0; texNo < 3; ++texNo )
			{
				scenegraph::resource::MaterialResourceNode::TextureInfo		*pTextureInfo = pMatInstance->GetTextureInfo( texNo );

				pTextureInfo->m_Attribute.m_TranslateU = uvList[matNo][texNo].x;
				pTextureInfo->m_Attribute.m_TranslateV = uvList[matNo][texNo].y;
			}
		}
	}
#endif
}

OutLinePostRenderPath::~OutLinePostRenderPath()
{
	for( u32 i = 0; i < EdgeMapSceneRenderPath::EdgeColor::Max; ++i )
	{
		delete m_pModelInstanceNode[i];
		delete m_pBinModelRootNode[i];
	}
}

void OutLinePostRenderPath::SetEdgeMapTexture( u32 type, const gfl2::gfx::Texture* pTexture)
{
	m_pTexture[type] = pTexture;
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////
////////    以下のクラスがこのクラスを継承しています。
////////    System::NijiOutLinePostSceneRenderPath
////////    app::util::AppOutLinePostRenderPath
////////    このpoke_3d::renderer::OutLinePostRenderPathクラスを変更した際は
////////    上記のクラスに影響がないか必ず確認して下さい。
////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void OutLinePostRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
	GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
	GFGL::SetDepthStencil( NULL );

	GFGL::BeginScene();

	const gfx::Surface::SurfaceDesc			&rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
	GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);

	{
		RenderState			renderState;
		math::Matrix34		dummyMat34 = math::Matrix34::GetIdentity();
		math::Matrix44		dummyMat44 = math::Matrix44::GetIdentity();
			
		renderState.SetProjectionMatrix( &dummyMat44 );
		renderState.SetViewMatrix( &dummyMat34 );
		renderState.SetWorldMatrix( &dummyMat34 );

    if( this->IsEnable( EdgeMapSceneRenderPath::EdgeColor::Normal ) )
		{
			MeshDrawTag			*pTag = m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::Normal]->GetMeshDrawTag( 0 );

			for( u32 texNo = 0; texNo < 3; ++texNo )
      {
#ifdef GF_PLATFORM_CTR
        // CTRの下画面でエッジがずれることへの対応
        gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo* texInfo
            = m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::Normal]->GetMaterialInstanceNode(0)->GetTextureInfo(texNo);
        if(texInfo && m_pTexture[EdgeMapSceneRenderPath::EdgeColor::Normal])
        {
          u32 height = (m_pTexture[EdgeMapSceneRenderPath::EdgeColor::Normal]->GetSurface() != NULL)
            ? m_pTexture[EdgeMapSceneRenderPath::EdgeColor::Normal]->GetSurface()->GetSurfaceDesc().bufferHeight
            : m_pTexture[EdgeMapSceneRenderPath::EdgeColor::Normal]->GetTextureDesc().height;

          texInfo->m_Attribute.m_ScaleV = (static_cast<f32>(rDesc.width)) / height;  // 例: 400/512
        }
#endif  // GF_PLATFORM_CTR
				pTag->SetTexture( texNo, m_pTexture[EdgeMapSceneRenderPath::EdgeColor::Normal] );
      }

			pTag->Draw( renderState );
		}

    if( this->IsEnable( EdgeMapSceneRenderPath::EdgeColor::ID ) && m_pTexture[EdgeMapSceneRenderPath::EdgeColor::ID] )
		{//IDエッジ
      static const u32                materialMax = 2;
      static const util::FnvHash			materialName[materialMax] = {
        util::FnvHash( reinterpret_cast<u8*>( const_cast<char*>("Comb2") ) ),
        util::FnvHash( reinterpret_cast<u8*>( const_cast<char*>("Comb3") ) )
      };

      //メッシュとマテリアルは１対１
      GFL_ASSERT( m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::ID]->GetMaterialNum() == m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::ID]->GetMehsNum() );
      GFL_ASSERT( m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::ID]->GetMaterialNum() == materialMax );

      MeshDrawTag                     *pOT[materialMax] = { NULL, NULL };

      
      for( u32 pushCnt = 0; pushCnt < materialMax; ++pushCnt )
      {
        u32         checkValue = materialName[pushCnt].value;//Comb2 -> Comb3 の順に整えます。

        for( u32 materialNo = 0; materialNo < materialMax; ++materialNo )
			  {
				  MeshDrawTag			                                                            *pTag = m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::ID]->GetMeshDrawTag( materialNo );
          const gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode     *pMaterial = pTag->GetMaterialInstanceNode();

          if ( pMaterial->GetNameHash() == checkValue )
          {
            pOT[pushCnt] = pTag;
            break;
          }
        }
      }

      for( u32 materialNo = 0; materialNo < materialMax; ++materialNo )
			{
				MeshDrawTag			                                                            *pTag = pOT[materialNo];
        const gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode     *pMaterial = pTag->GetMaterialInstanceNode();

				for( u32 texNo = 0; texNo < 3; ++texNo )
        {
#ifdef GF_PLATFORM_CTR
          // CTRの下画面でエッジがずれることへの対応
          gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo* texInfo = const_cast<gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo*>( pMaterial->GetTextureInfo(texNo) );

          if(texInfo)
          {
            u32 width = (m_pTexture[EdgeMapSceneRenderPath::EdgeColor::ID]->GetSurface() != NULL)
              ? m_pTexture[EdgeMapSceneRenderPath::EdgeColor::ID]->GetSurface()->GetSurfaceDesc().bufferWidth
							: m_pTexture[EdgeMapSceneRenderPath::EdgeColor::ID]->GetTextureDesc().width;

						u32 height = (m_pTexture[EdgeMapSceneRenderPath::EdgeColor::ID]->GetSurface() != NULL)
              ? m_pTexture[EdgeMapSceneRenderPath::EdgeColor::ID]->GetSurface()->GetSurfaceDesc().bufferHeight
              : m_pTexture[EdgeMapSceneRenderPath::EdgeColor::ID]->GetTextureDesc().height;

						f32			fwidth = static_cast<f32>(rDesc.width);
						f32			fheight = static_cast<f32>(rDesc.height);

            texInfo->m_Attribute.m_ScaleV = fwidth / height;  // 例: 400/512
						texInfo->m_Attribute.m_ScaleU = fheight / width;  // 例: 240/256

						texInfo->m_Attribute.m_TranslateU = 0.0f;
						texInfo->m_Attribute.m_TranslateV = 0.0f;

            if ( materialNo == 0 )
            {//Comb2はＵ方向差分をレンダリング
						  switch( texNo ){
						  case 1:
							  texInfo->m_Attribute.m_TranslateU = 1.0f / fheight;
							  break;
						  case 2:
							  texInfo->m_Attribute.m_TranslateU = -(1.0f / fheight);
							  break;
						  }
            }
            else
            {//Comb3はＶ方向差分をレンダリング
              switch( texNo ){
						  case 1:
							  texInfo->m_Attribute.m_TranslateV = 1.0f / fwidth ;
							  break;
						  case 2:
							  texInfo->m_Attribute.m_TranslateV = -(1.0f / fwidth );
							  break;
						  }
            }
          }
#endif  // GF_PLATFORM_CTR
					pTag->SetTexture( texNo, m_pTexture[EdgeMapSceneRenderPath::EdgeColor::ID] );
        }

				pTag->Draw( renderState );
			}
		}
	}

	GFGL::SetTexture( 0, NULL );//次のフレームでは最初にRTとして使用するので、バインドをはずしておく。
	GFGL::SetTexture( 1, NULL );//次のフレームでは最初にRTとして使用するので、バインドをはずしておく。
	GFGL::SetTexture( 2, NULL );//次のフレームでは最初にRTとして使用するので、バインドをはずしておく。

	GFGL::EndScene();
	GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );
}

void OutLinePostRenderPath::SetEnable( u32 edgeColor, b32 flag )
{
  if( edgeColor >= EdgeMapSceneRenderPath::EdgeColor::Max )
  {
    return;
  }

  m_EdgeColorEnableTable[ edgeColor ] = flag;
}

b32 OutLinePostRenderPath::IsEnable( u32 edgeColor ) const
{
  if( edgeColor >= EdgeMapSceneRenderPath::EdgeColor::Max )
  {
    return false;
  }

  return m_EdgeColorEnableTable[ edgeColor ];
}

void OutLinePostRenderPath::SetEdgeColor( u8 r, u8 g, u8 b )
{
  scenegraph::resource::MaterialResourceNode::AttributeParam			*pAttr = m_pModelInstanceNode[EdgeMapSceneRenderPath::EdgeColor::Normal]->GetMaterialInstanceNode( 0 )->GetAttributeParam();

  pAttr->m_ConstantColor[1].r = r;
  pAttr->m_ConstantColor[1].g = g;
  pAttr->m_ConstantColor[1].b = b;
}

}}
