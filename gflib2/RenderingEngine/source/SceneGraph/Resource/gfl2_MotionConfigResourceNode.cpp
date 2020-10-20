#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MotionConfigResourceNode.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

static u32 BMCNF_SUPPORTED_VERSION_LIST[] =
{
  0x00010000,
};

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MotionConfigResourceNode::MotionConfigResourceNode() :
  NodeTrait<MotionConfigResourceNode, ResourceNode>(),
  m_pConfigData(NULL),
  m_pConfigTop(NULL),
  m_pInterestHeader(NULL),
  m_pSpringHeader(NULL),
  m_pIKHeader(NULL),
  m_pExpressionHeader(NULL)
{
}

MotionConfigResourceNode::~MotionConfigResourceNode()
{
}

void MotionConfigResourceNode::SetData(const u8* data)
{
  GFL_ASSERT(data);
  uptr u32addr = reinterpret_cast<uptr>(data);
  m_pConfigData = reinterpret_cast<const ConfigData*>(u32addr);

  // 対応バージョンか？
  if (!IsBmcnfSupportedVersion(m_pConfigData->version))
  {
    GFL_ASSERT(0);
    return;
  }

  for (u32 i = 0; i < m_pConfigData->dataCount; ++i)
  {
    const DataHeader* pHeader = &m_pConfigData->dataHeaderList[i];

    uptr dataAddr = u32addr + pHeader->offset;
    switch (pHeader->type)
    {
    case ConfigType::Top:
      m_pConfigTop = reinterpret_cast<const ConfigTop*>(dataAddr);
      break;
    case ConfigType::Interest:
      m_pInterestHeader = reinterpret_cast<const InterestHeader*>(dataAddr);
      break;
    case ConfigType::Spring:
      m_pSpringHeader = reinterpret_cast<const SpringHeader*>(dataAddr);
      break;
    case ConfigType::IK:
      m_pIKHeader = reinterpret_cast<const IKHeader*>(dataAddr);
      break;
    case ConfigType::Expression:
      m_pExpressionHeader = reinterpret_cast<const ExpressionHeader*>(dataAddr);
      break;
    case ConfigType::BlendList:
      m_pBlendListHeader = reinterpret_cast<const BlendListHeader*>(dataAddr);
      break;

    default:
      GFL_ASSERT(0);
      break;
    }
  }
}

void MotionConfigResourceNode::SetDataInterest(const u8*& pData)
{
  (void)pData;
}

void MotionConfigResourceNode::SetDataSpring(const u8*& pData)
{
  (void)pData;
}

void MotionConfigResourceNode::SetDataIK(const u8*& pData)
{
  (void)pData;
}

void MotionConfigResourceNode::SetDataExpression(const u8*& pData)
{
  (void)pData;
}

bool MotionConfigResourceNode::IsBmcnfSupportedVersion(u32 version)
{
  for (u32 i = 0; i < sizeof(BMCNF_SUPPORTED_VERSION_LIST) / sizeof(BMCNF_SUPPORTED_VERSION_LIST[0]); ++i)
  {
    if (version == BMCNF_SUPPORTED_VERSION_LIST[i])
    {
      return true;
    }
  }

  return false;
}

}}}}

