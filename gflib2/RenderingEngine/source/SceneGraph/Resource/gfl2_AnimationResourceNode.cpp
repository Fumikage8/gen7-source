#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNode.h>
#include <debug/include/gfl2_Assert.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

static const u32 GFBMOT_SUPPORTED_VERSION_LIST[] =
{
  0x00030000,
  0x00040000,
  0x00050000,
  0x00060000,
};

AnimationResourceNode::AnimationResourceNode() : NodeTrait<AnimationResourceNode, ResourceNode>(),
  m_pAnimData(NULL),
  m_pAnimTop(NULL),
  m_pJointAnimHeader(NULL),
  m_pUvAnimHeader(NULL),
  m_pTexturePatternAnimHeader(NULL),
  m_pShaderAnimHeader(NULL),
  m_pVisibilityAnimHeader(NULL),
  m_pKeyAnimHeader(NULL),
  m_pEnvLightAnimHeader(NULL),
  m_pEnvCameraAnimHeader(NULL),
  m_pEnvFogAnimHeader(NULL),    //kawa20150514
  m_pEnvGlareAnimHeader(NULL)   //kawa20150514
{
}

AnimationResourceNode::~AnimationResourceNode()
{
}

void AnimationResourceNode::SetData( const void* data, u32 size )
{
  (void)size;

  GFL_ASSERT(data);
  uptr u32addr = reinterpret_cast<uptr>(data);
  m_pAnimData = reinterpret_cast<const AnimData*>(u32addr);

  // 対応バージョンか？
  if (!IsGfbmotSupportedVersion(m_pAnimData->version))
  {
    GFL_ASSERT(0);
    return;
  }

  for (u32 i = 0; i < m_pAnimData->dataCount; ++i)
  {
    const DataHeader* pHeader = &m_pAnimData->dataHeaderList[i];

    uptr dataAddr = u32addr + pHeader->offset;
    switch (pHeader->type)
    {
    case AnimType::Top:
      m_pAnimTop = reinterpret_cast<const AnimTop*>(dataAddr);
      break;
    case AnimType::Joint:
      m_pJointAnimHeader = reinterpret_cast<const JointAnimHeader*>(dataAddr);
      break;

      //BlendShape,

    case AnimType::Uv:
      m_pUvAnimHeader = reinterpret_cast<const UvAnimHeader*>(dataAddr);
      break;

    case AnimType::TexturePattern:
      m_pTexturePatternAnimHeader = reinterpret_cast<const TexturePatternAnimHeader*>(dataAddr);
      break;

    case AnimType::Shader:
      m_pShaderAnimHeader = reinterpret_cast<const ShaderAnimHeader*>(dataAddr);
      break;

    case AnimType::Visibility:
      m_pVisibilityAnimHeader = reinterpret_cast<const VisibilityAnimHeader*>(dataAddr);
      break;

    case AnimType::Key:
      m_pKeyAnimHeader = reinterpret_cast<const KeyAnimHeader*>(dataAddr);
      break;

    case AnimType::EnvLight:
      m_pEnvLightAnimHeader = reinterpret_cast<const EnvLightAnimHeader*>(dataAddr);
      break;

    case AnimType::EnvCamera:
      m_pEnvCameraAnimHeader = reinterpret_cast<const EnvCameraAnimHeader*>(dataAddr);
      break;

    //kawa20150514
    case AnimType::EnvFog:
      m_pEnvFogAnimHeader = reinterpret_cast<const EnvFogAnimHeader*>(dataAddr);
      break;

    case AnimType::EnvGlare:
      m_pEnvGlareAnimHeader = reinterpret_cast<const EnvGlareAnimHeader*>(dataAddr);
      break;

    default:
      GFL_ASSERT(0);
      break;
    }
  }
}

u32 AnimationResourceNode::GetFileNameHash() const
{
  if (m_pAnimData->version >= 0x00060000)
  {
    return m_pAnimTop->fileNameHash;
  }

  return 0;
}

bool AnimationResourceNode::IsGfbmotSupportedVersion(u32 version)
{
  for (u32 i = 0; i < sizeof(GFBMOT_SUPPORTED_VERSION_LIST) / sizeof(GFBMOT_SUPPORTED_VERSION_LIST[0]); ++i)
  {
    if (version == GFBMOT_SUPPORTED_VERSION_LIST[i])
    {
      return true;
    }
  }

  return false;
}

}}}}

