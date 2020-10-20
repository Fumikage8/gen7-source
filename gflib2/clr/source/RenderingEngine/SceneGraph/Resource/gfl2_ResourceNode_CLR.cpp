#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_ResourceNode_CLR.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <clr/include/util/gfl2_String_CLR.h>
#include <clr/include/gfx/gfl2_GFGL_CLR.h>

using System::IntPtr;
using System::Runtime::InteropServices::Marshal;
using System::Collections::Generic::List;

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

//いずれＩＯクラスができたら置き換える。
static c8* ReadFile(const char* filename, s32* p_size, s32 align = 16)
{
  FILE *hFile;
  int file_size;
  char *read_buff;
  
  fopen_s(&hFile, filename, "rb");
  
  if (hFile == NULL)
  {
    return NULL;
  }
  
  fseek( hFile, 0, SEEK_END );
  file_size = ftell( hFile );
  *p_size = file_size;
  fseek( hFile, 0, SEEK_SET );
  
  file_size += align;//必ずNULLが入るようにしたい。
  u32 buf_size = file_size + ((-file_size) & (align-1));
  
  read_buff = reinterpret_cast<c8*>(malloc(buf_size));
  memset( read_buff, 0, buf_size );
  fread( read_buff, file_size, 1, hFile );
  fclose( hFile );

  return( read_buff );
}

ResourceNode::ResourceNode( gfl2::renderingengine::scenegraph::resource::ResourceNode *pNative ) :
  m_pShaderDataBuffers(NULL),
  m_ShaderDataNum(0),
  m_pTextureDataBuffers(NULL),
  m_TextureDataNum(0)
{
	m_pNative = pNative;
}

ResourceNode::!ResourceNode()
{
	if (m_pNative)
	{
		delete m_pNative;
		m_pNative = 0;
	}

  if (m_pShaderDataBuffers)
  {
    for (s32 i = 0; i < m_ShaderDataNum; ++i)
    {
      if (m_pShaderDataBuffers[i])
      {
        free(m_pShaderDataBuffers[i]);
        m_pShaderDataBuffers[i] = NULL;
      }
    }
    delete[] m_pShaderDataBuffers;
    m_pShaderDataBuffers = NULL;
  }

  if (m_pTextureDataBuffers)
  {
    for (s32 i = 0; i < m_TextureDataNum; ++i)
    {
      if (m_pTextureDataBuffers[i])
      {
        free(m_pTextureDataBuffers[i]);
        m_pTextureDataBuffers[i] = NULL;
      }
    }
    delete []m_pTextureDataBuffers;
    m_pTextureDataBuffers = NULL;
  }
}

void ResourceNode::AddShaderResources(array<System::String^>^ paths)
{
  List<System::String^>^ pathList = gcnew List<System::String^>();
  for (s32 i = 0; i < paths->Length; ++i)
  {
    if (paths[i]->Contains(".wingl.gfbvsh")
      || paths[i]->Contains(".wingl.gfbgsh")
      || paths[i]->Contains(".wingl.gfbfsh"))
    {
      pathList->Add(paths[i]);
    }
  }

  if (pathList->Count == 0)
  {
    return;
  }

  m_ShaderDataNum = pathList->Count;
  m_pShaderDataBuffers = new c8*[m_ShaderDataNum];

  for (s32 i = 0; i < m_ShaderDataNum; ++i)
  {
    std::string path_ = gfl2::clr::util::String::GetNativeString(pathList[i]);

    s32 fileSize = 0;
    m_pShaderDataBuffers[i] = ReadFile(path_.c_str(), &fileSize);

    gfl2::renderingengine::scenegraph::resource::ResourceNode* resourceNode = NULL;
    gfl2::renderingengine::scenegraph::resource::GfBinaryShdData pData;
    pData.SetShaderData(m_pShaderDataBuffers[i]);
    resourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), &pData);
    m_pNative->AddChild(resourceNode);
  }
}

void ResourceNode::AddTextureResources(array<System::String^>^ paths)
{
  List<System::String^>^ pathList = gcnew List<System::String^>();
  for (s32 i = 0; i < paths->Length; ++i)
  {
    if (paths[i]->Contains(".wingl.btex"))
    {
      pathList->Add(paths[i]);
    }
  }

  if (pathList->Count == 0)
  {
    return;
  }

  m_TextureDataNum = pathList->Count;
  m_pTextureDataBuffers = new c8*[m_TextureDataNum];

  for (s32 i = 0; i < m_TextureDataNum; ++i)
  {
    std::string path_ = gfl2::clr::util::String::GetNativeString(pathList[i]);

    s32 fileSize = 0;
    m_pTextureDataBuffers[i] = ReadFile(path_.c_str(), &fileSize);

    gfl2::renderingengine::scenegraph::resource::ResourceNode* resourceNode = NULL;
    gfl2::renderingengine::scenegraph::resource::GfBinaryTexData pData;
    pData.SetTextureData(m_pTextureDataBuffers[i]);
    resourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), &pData);
    m_pNative->AddChild(resourceNode);
  }
}

}}}}}