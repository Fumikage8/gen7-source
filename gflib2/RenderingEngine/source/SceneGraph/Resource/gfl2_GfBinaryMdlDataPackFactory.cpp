#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPackFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/*ResourceNode* GfBinaryMdlDataPackFactory::CreateResourceNode(const void* data, int size)
{
  return CreateResourceNode( gfx::GLMemory::GetAllocator(), data, size );
}*/
ResourceNode* GfBinaryMdlDataPackFactory::CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size)
{
  (void)size;

  struct Header
  {
    u32 version;
    u32 modelCount;
    u32 textureCount;
    u32 shaderVertexCount;
    u32 shaderGeometryCount;
    u32 shaderFragmentCount;

    u32 dataOffsetTable[1];

    const c8* GetName(u32 index, u8* out_pSize) const
    {
      uptr data_ptr = reinterpret_cast<uptr>(this);
      data_ptr += dataOffsetTable[index];

      *out_pSize = *reinterpret_cast<const u8*>(data_ptr);
      data_ptr += sizeof(u8);
      return reinterpret_cast<const c8*>(data_ptr);
    }

    const void* GetData(u32 index) const
    {
      u8 len = 0;
      const c8* pNamePtr = this->GetName(index, &len);

      pNamePtr += len;
      u32 data_offset = *reinterpret_cast<const u32*>(pNamePtr);
      uptr data_ptr = reinterpret_cast<uptr>(this);

      data_ptr += data_offset;
      return reinterpret_cast<const c8*>(data_ptr);
    }

  };

  ResourceNode* pRootNode = NULL;

  const Header* pHeader = (const Header*)data;

  // 対応しているバージョンか確認
  if (pHeader->version != 0x00010000)
  {
    GFL_ASSERT(0);
    return NULL;
  }

  u32 modelBaseIndex = 0;
  u32 textureBaseIndex = modelBaseIndex + pHeader->modelCount;
  u32 shaderVertexBaseIndex = textureBaseIndex + pHeader->textureCount;
  u32 shaderGeometryBaseIndex = shaderVertexBaseIndex + pHeader->shaderVertexCount;
  u32 shaderFragmentBaseIndex = shaderGeometryBaseIndex + pHeader->shaderGeometryCount;

  // リソースの構築

  // モデルの読み込み
  if (pHeader->modelCount != 1)
  {
    // モデルの数は必ず１つ
    GFL_ASSERT(0);
    gfx::GLMemory::ClearInstanceAllocator();
    return NULL;
  }

  {
    resource::GfBinaryMdlData resourceData;
    resourceData.SetModelData((c8*)pHeader->GetData(modelBaseIndex + 0));

    pRootNode = resource::ResourceManager::CreateResourceNode(pAllocator, &resourceData);
  }

  // バーテクスシェーダー
  for (u32 i = 0; i < pHeader->shaderVertexCount; ++i)
  {
    resource::GfBinaryShdData resourceData;
    resourceData.SetShaderData((c8*)pHeader->GetData(shaderVertexBaseIndex + i));

    resource::ResourceManager::CreateResourceNode(pAllocator, &resourceData, pRootNode);
  }

  // ジオメトリシェーダー
  for (u32 i = 0; i < pHeader->shaderGeometryCount; ++i)
  {
    resource::GfBinaryShdData resourceData;
    resourceData.SetShaderData((c8*)pHeader->GetData(shaderGeometryBaseIndex + i));

    resource::ResourceManager::CreateResourceNode(pAllocator, &resourceData, pRootNode);
  }

  // フラグメントシェーダー
  for (u32 i = 0; i < pHeader->shaderFragmentCount; ++i)
  {
    resource::GfBinaryShdData resourceData;
    resourceData.SetShaderData((c8*)pHeader->GetData(shaderFragmentBaseIndex + i));

    resource::ResourceManager::CreateResourceNode(pAllocator, &resourceData, pRootNode);
  }

  // テクスチャの読み込み
  for (u32 i = 0; i < pHeader->textureCount; ++i)
  {
    resource::GfBinaryTexData resourceData;
    resourceData.SetTextureData((c8*)pHeader->GetData(textureBaseIndex + i));

    resource::ResourceManager::CreateResourceNode(pAllocator, &resourceData, pRootNode);
  }

  return pRootNode;
}

b32 GfBinaryMdlDataPackFactory::IsSupport( IResourceData::Type type )
{
  static const IResourceData::Type supportType("mdlp");
  return (type == supportType);
}

}}}}


