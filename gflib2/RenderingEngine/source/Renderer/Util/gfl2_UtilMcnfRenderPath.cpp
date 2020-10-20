#if defined(GF_PLATFORM_WIN32)

#include <Renderingengine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_UtilMcnfRenderPath.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MotionConfigInstanceNode.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

typedef math::Matrix34 (*GetMatrixFunc)(f32 rad);

static math::Vector3 GetAxis(const math::Matrix34& matrix, u32 axisIndex)
{
  switch (axisIndex)
  {
  case 0: return matrix.GetElemAxisX();
  case 1: return matrix.GetElemAxisY();
  case 2: return matrix.GetElemAxisZ();
  }

  return math::Vector3();
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
McnfRenderPath::McnfRenderPath() :
  IRenderPath(),
  m_pVertexFormatList(NULL)
{
}

McnfRenderPath::~McnfRenderPath()
{
  if (m_pVertexFormatList != NULL)
  {
    delete[] m_pVertexFormatList;
  }
}

void McnfRenderPath::Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc)
{
  gfl2::gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
  
  // 初期化設定保存
  m_InitDescription = desc;

  // 頂点フォーマットリスト作成
  m_VertexFormatLimit = desc.maxJointCount * 22;
  m_pVertexFormatList = new DrawUtil::VertexFormat[m_VertexFormatLimit];

  gfl2::gfx::GLMemory::ClearInstanceAllocator();
}

void McnfRenderPath::SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  // レンダーターゲットの設定
	GFGL::SetRenderTarget( pRenderTarget );
	GFGL::SetDepthStencil( pDepthStencil );
}

void McnfRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  (void)pRenderTarget;
  (void)pDepthStencil;

  // レンダーターゲットのクリアはしない
}

// MotionConfigInstanceNodeに対してのVisitor
class MotinConfigInstanceNodeVisitor
{
public:
  MotinConfigInstanceNodeVisitor() {}
  virtual ~MotinConfigInstanceNodeVisitor() {}

  void Visit( scenegraph::instance::ModelInstanceNode* pModel)
  {
    if (pModel == NULL)
    {
      return;
    }

    const scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode = static_cast<const scenegraph::instance::MotionConfigInstanceNode*>(pModel->GetMotionConfigInstanceNode());
    if (pMotionConfigNode == NULL)
    {
      return;
    }

    // 注視：角度制限のデバッグ表示

    // 全ジョイント探索
    for (u32 i = 0; i < pModel->GetJointNum(); ++i)
    {
      scenegraph::instance::JointInstanceNode* pJoint = pModel->GetJointInstanceNode(i);

      // 全注視ノード探索
      for (u32 j = 0; j < pMotionConfigNode->GetInterestCount(); ++j)
      {
        const scenegraph::resource::MotionConfigResourceNode::InterestNode* pInterest = pMotionConfigNode->GetInterest(j);
        if (std::strcmp(pInterest->Name, pJoint->GetName()) != 0)
        {
          continue;
        }

        scenegraph::instance::JointInstanceNode* pJointParent = pJoint->GetParent()->SafeCast<scenegraph::instance::JointInstanceNode>();
        math::Matrix34 worldMatrix = pJointParent->GetWorldMatrix();

        f32 lowRad = pInterest->Low;
        f32 highRad = pInterest->High;
        f32 low2Rad = pInterest->Low2;
        f32 high2Rad = pInterest->High2;

        math::SRT localSRT = pJoint->GetLocalSRT();
        localSRT.rotation = *pMotionConfigNode->GetInterestOldRotation(j);
        worldMatrix = worldMatrix * localSRT.ToMatrixNonInheritanceScale(pJointParent->GetLocalScale());

        const math::Matrix34& currentMatrix = pJoint->GetWorldMatrix();

        // 基準位置
        f32 offset = 50.0f;

        f32 lowRadTmp = (math::Abs(lowRad) > math::Abs(highRad))
          ? lowRad
          : -highRad;

        switch (pInterest->Type)
        {
        case renderingengine::InterestType::X:
          this->DrawX(offset, currentMatrix, worldMatrix, lowRad, highRad);
          break;
        case renderingengine::InterestType::YZ:
          this->DrawYZ(offset, currentMatrix, worldMatrix, lowRadTmp, highRad, low2Rad, high2Rad);
          break;
        case renderingengine::InterestType::XZ:
          this->DrawXZ(offset, currentMatrix, worldMatrix, lowRadTmp, highRad, low2Rad, high2Rad);
          break;
        case renderingengine::InterestType::XY:
          this->DrawXY(offset, currentMatrix, worldMatrix, lowRadTmp, highRad, low2Rad, high2Rad);
          break;
        case renderingengine::InterestType::ZY:
          this->DrawZY(offset, currentMatrix, worldMatrix, lowRadTmp, highRad, low2Rad, high2Rad);
          break;
        case renderingengine::InterestType::Y:
          this->DrawY(offset, currentMatrix, worldMatrix, lowRad, highRad);
          break;
        case renderingengine::InterestType::Z:
          this->DrawZ(offset, currentMatrix, worldMatrix, lowRad, highRad);
          break;
        case renderingengine::InterestType::X_M90:
          this->DrawX(offset, currentMatrix, worldMatrix, lowRad, highRad);
          break;
        case renderingengine::InterestType::Z_180:
          this->DrawZ(offset, currentMatrix, worldMatrix, lowRad, highRad);
          break;
        case renderingengine::InterestType::Y_90:
          {
            f32 rad90 = math::ConvDegToRad(90);
            math::Matrix34 tmpMatrix = currentMatrix * math::Matrix34::GetRotationY(-rad90);
            this->DrawY(offset, tmpMatrix, worldMatrix, lowRad - rad90, highRad - rad90);
          }
          break;
        }
      }
    }
  }

  void Draw_Rot0(s32 frontAxisIndex, GetMatrixFunc func0, const math::Vector3& basePosition, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad)
  {
    bool inverse = false;
    if (frontAxisIndex < 0)
    {
      frontAxisIndex = -frontAxisIndex;
      inverse = true;
    }
    math::Matrix34 matBegin = func0(lowRad);

    DrawUtil::VertexFormat vf1;
    vf1.pos = worldMatrix.TransformCoord(matBegin.TransformCoord(basePosition));
    vf1.color = desc->jointColor;

    DrawUtil::VertexFormat vfOld = vf1;

    // 適当な分割数でラインを描画
    for (u32 i = 0; i <= this->lineDivCount; ++i)
    {
      DrawUtil::VertexFormat& vfBegin = vertexList[vertexCount++];
      vfBegin = vfOld;

      f32 angle = lowRad + (highRad - lowRad) * (i / (f32)this->lineDivCount);
      math::Matrix34 mat = func0(angle);

      DrawUtil::VertexFormat& vfEnd = vertexList[vertexCount++];
      math::Vector3 p = GetAxis(mat, frontAxisIndex) * 50;
      if (inverse)
      {
        p = -p;
      }
      vfEnd.pos = worldMatrix.TransformCoord(p);//mat.TransformCoord(basePosition));
      vfEnd.color = desc->jointColor;

      vfOld = vfEnd;
    }
  }

  void Draw_Rot01_0(s32 frontAxisIndex, GetMatrixFunc func0, GetMatrixFunc func1, const math::Vector3& basePosition, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 rad_2)
  {
    bool inverse = false;
    if (frontAxisIndex < 0)
    {
      frontAxisIndex = -frontAxisIndex;
      inverse = true;
    }
    math::Matrix34 matBegin = func1(rad_2) *  func0(lowRad);

    DrawUtil::VertexFormat vf1;
    vf1.pos = worldMatrix.TransformCoord(matBegin.TransformCoord(basePosition));
    vf1.color = desc->jointColor;

    DrawUtil::VertexFormat vfOld = vf1;

    // 適当な分割数でラインを描画
    for (u32 i = 0; i <= this->lineDivCount; ++i)
    {
      DrawUtil::VertexFormat& vfBegin = vertexList[vertexCount++];
      vfBegin = vfOld;

      f32 angle = lowRad + (highRad - lowRad) * (i / (f32)this->lineDivCount);
      math::Matrix34 mat = func1(rad_2) * func0(angle);

      DrawUtil::VertexFormat& vfEnd = vertexList[vertexCount++];
      math::Vector3 p = GetAxis(mat, frontAxisIndex) * 50;
      if (inverse)
      {
        p = -p;
      }
      vfEnd.pos = worldMatrix.TransformCoord(p);//mat.TransformCoord(basePosition));
      //vfEnd.pos = worldMatrix.TransformCoord(mat.TransformCoord(basePosition));
      vfEnd.color = desc->jointColor;

      vfOld = vfEnd;
    }
  }

  void Draw_Rot01_1(s32 frontAxisIndex, GetMatrixFunc func0, GetMatrixFunc func1, const math::Vector3& basePosition, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 rad_2)
  {
    bool inverse = false;
    if (frontAxisIndex < 0)
    {
      frontAxisIndex = -frontAxisIndex;
      inverse = true;
    }
    math::Matrix34 matBegin = func1(lowRad) *  func0(rad_2);

    DrawUtil::VertexFormat vf1;
    vf1.pos = worldMatrix.TransformCoord(matBegin.TransformCoord(basePosition));
    vf1.color = desc->jointColor;

    DrawUtil::VertexFormat vfOld = vf1;

    // 適当な分割数でラインを描画
    for (u32 i = 0; i <= this->lineDivCount; ++i)
    {
      DrawUtil::VertexFormat& vfBegin = vertexList[vertexCount++];
      vfBegin = vfOld;

      f32 angle = lowRad + (highRad - lowRad) * (i / (f32)this->lineDivCount);
      math::Matrix34 mat = func1(angle) * func0(rad_2);

      DrawUtil::VertexFormat& vfEnd = vertexList[vertexCount++];
      math::Vector3 p = GetAxis(mat, frontAxisIndex) * 50;
      if (inverse)
      {
        p = -p;
      }
      vfEnd.pos = worldMatrix.TransformCoord(p);//mat.TransformCoord(basePosition));
      //vfEnd.pos = worldMatrix.TransformCoord(mat.TransformCoord(basePosition));
      vfEnd.color = desc->jointColor;

      vfOld = vfEnd;
    }
  }

  void DrawXY(f32 offset, const math::Matrix34& currentMatrix, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 low2Rad, f32 high2Rad)
  {
    (void)offset;

    math::Vector3 pos;

    u32 beginIndex = this->vertexCount;
    this->DrawXY_X(pos, worldMatrix, lowRad, highRad, low2Rad);
    DrawUtil::VertexFormat vf0 = this->vertexList[beginIndex];
    DrawUtil::VertexFormat vf1 = this->vertexList[this->vertexCount - 1];
    u32 endIndex = this->vertexCount;

    this->DrawXY_X(pos, worldMatrix, lowRad, highRad, high2Rad);
    DrawUtil::VertexFormat vf2 = this->vertexList[endIndex];
    DrawUtil::VertexFormat vf3 = this->vertexList[this->vertexCount - 1];

    this->DrawXY_Y(pos, worldMatrix, low2Rad, high2Rad, lowRad);
    this->DrawXY_Y(pos, worldMatrix, low2Rad, high2Rad, highRad);

    DrawUtil::VertexFormat vfBase;
    vfBase.pos = worldMatrix.TransformCoord(pos);
    vfBase.color = this->desc->jointColor;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf0;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf1;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf2;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf3;

    // 今どこを向いているか？
    DrawUtil::VertexFormat vfDir;
    vfDir.pos = currentMatrix.TransformCoord(math::Vector3(0, 0, 50));
    vfDir.color.Set(0, 1, 0, 1);
    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vfDir;
  }

  void DrawXY_X(const math::Vector3& basePosition, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 rad_2)
  {
    this->Draw_Rot01_0(2, math::Matrix34::GetRotationX, math::Matrix34::GetRotationY, basePosition, worldMatrix, lowRad, highRad, rad_2);
  }

  void DrawXY_Y(const math::Vector3& basePosition, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 rad_2)
  {
    this->Draw_Rot01_1(2, math::Matrix34::GetRotationX, math::Matrix34::GetRotationY, basePosition, worldMatrix, lowRad, highRad, rad_2);
  }

  void DrawXZ(f32 offset, const math::Matrix34& currentMatrix, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 low2Rad, f32 high2Rad)
  {
    math::Vector3 pos;

    u32 beginIndex = this->vertexCount;
    this->DrawXZ_X(pos, worldMatrix, lowRad, highRad, low2Rad);
    DrawUtil::VertexFormat vf0 = this->vertexList[beginIndex];
    DrawUtil::VertexFormat vf1 = this->vertexList[this->vertexCount - 1];
    u32 endIndex = this->vertexCount;

    this->DrawXZ_X(pos, worldMatrix, lowRad, highRad, high2Rad);
    DrawUtil::VertexFormat vf2 = this->vertexList[endIndex];
    DrawUtil::VertexFormat vf3 = this->vertexList[this->vertexCount - 1];

    this->DrawXZ_Y(pos, worldMatrix, low2Rad, high2Rad, lowRad);
    this->DrawXZ_Y(pos, worldMatrix, low2Rad, high2Rad, highRad);

    DrawUtil::VertexFormat vfBase;
    vfBase.pos = worldMatrix.TransformCoord(pos);
    vfBase.color = this->desc->jointColor;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf0;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf1;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf2;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf3;

    // 今どこを向いているか？
    DrawUtil::VertexFormat vfDir;
    vfDir.pos = currentMatrix.TransformCoord(math::Vector3(0, -50, 0));
    vfDir.color.Set(0, 1, 0, 1);
    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vfDir;
  }

  void DrawXZ_X(const math::Vector3& basePosition, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 rad_2)
  {
    this->Draw_Rot01_0(-1, math::Matrix34::GetRotationX, math::Matrix34::GetRotationZ, basePosition, worldMatrix, lowRad, highRad, rad_2);
  }

  void DrawXZ_Y(const math::Vector3& basePosition, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 rad_2)
  {
    this->Draw_Rot01_1(-1, math::Matrix34::GetRotationX, math::Matrix34::GetRotationZ, basePosition, worldMatrix, lowRad, highRad, rad_2);
  }
  void DrawX(f32 offset, const math::Matrix34& currentMatrix, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad)
  {
    math::Vector3 pos;

    u32 beginIndex = this->vertexCount;
    this->Draw_Rot0(2, math::Matrix34::GetRotationX, pos, worldMatrix, lowRad, highRad);
    DrawUtil::VertexFormat vf0 = this->vertexList[beginIndex];
    DrawUtil::VertexFormat vf1 = this->vertexList[this->vertexCount - 1];

    DrawUtil::VertexFormat vfBase;
    vfBase.pos = worldMatrix.GetElemPosition();
    vfBase.color = this->desc->jointColor;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf0;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf1;

    // 今どこを向いているか？
    DrawUtil::VertexFormat vfDir;
    vfDir.pos = currentMatrix.TransformCoord(math::Vector3(0, 0, 50));
    vfDir.color.Set(0, 1, 0, 1);
    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vfDir;
  }

  void DrawY(f32 offset, const math::Matrix34& currentMatrix, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad)
  {
    math::Vector3 pos;

    u32 beginIndex = this->vertexCount;
    this->Draw_Rot0(0, math::Matrix34::GetRotationY, pos, worldMatrix, lowRad, highRad);
    DrawUtil::VertexFormat vf0 = this->vertexList[beginIndex];
    DrawUtil::VertexFormat vf1 = this->vertexList[this->vertexCount - 1];

    DrawUtil::VertexFormat vfBase;
    vfBase.pos = worldMatrix.TransformCoord(pos);
    vfBase.color = this->desc->jointColor;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf0;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf1;

    // 今どこを向いているか？
    DrawUtil::VertexFormat vfDir;
    vfDir.pos = currentMatrix.TransformCoord(math::Vector3(50, 0, 0));
    vfDir.color.Set(0, 1, 0, 1);
    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vfDir;
  }

  void DrawZ(f32 offset, const math::Matrix34& currentMatrix, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad)
  {
    math::Vector3 pos;

    u32 beginIndex = this->vertexCount;
    this->Draw_Rot0(0, math::Matrix34::GetRotationZ, pos, worldMatrix, lowRad, highRad);
    DrawUtil::VertexFormat vf0 = this->vertexList[beginIndex];
    DrawUtil::VertexFormat vf1 = this->vertexList[this->vertexCount - 1];

    DrawUtil::VertexFormat vfBase;
    vfBase.pos = worldMatrix.TransformCoord(pos);
    vfBase.color = this->desc->jointColor;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf0;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf1;

    // 今どこを向いているか？
    DrawUtil::VertexFormat vfDir;
    vfDir.pos = currentMatrix.TransformCoord(math::Vector3(50, 0, 0));
    vfDir.color.Set(0, 1, 0, 1);
    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vfDir;
  }

  void DrawYZ(f32 offset, const math::Matrix34& currentMatrix, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 low2Rad, f32 high2Rad)
  {
    math::Vector3 pos;

    u32 beginIndex = this->vertexCount;
    this->DrawYZ_Y(pos, worldMatrix, lowRad, highRad, low2Rad);
    DrawUtil::VertexFormat vf0 = this->vertexList[beginIndex];
    DrawUtil::VertexFormat vf1 = this->vertexList[this->vertexCount - 1];
    u32 endIndex = this->vertexCount;

    this->DrawYZ_Y(pos, worldMatrix, lowRad, highRad, high2Rad);
    DrawUtil::VertexFormat vf2 = this->vertexList[endIndex];
    DrawUtil::VertexFormat vf3 = this->vertexList[this->vertexCount - 1];

    this->DrawYZ_Z(pos, worldMatrix, low2Rad, high2Rad, lowRad);
    this->DrawYZ_Z(pos, worldMatrix, low2Rad, high2Rad, highRad);

    DrawUtil::VertexFormat vfBase;
    vfBase.pos = worldMatrix.TransformCoord(pos);
    vfBase.color = this->desc->jointColor;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf0;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf1;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf2;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf3;

    // 今どこを向いているか？
    DrawUtil::VertexFormat vfDir;
    vfDir.pos = currentMatrix.TransformCoord(math::Vector3(50, 0, 0));
    vfDir.color.Set(0, 1, 0, 1);
    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vfDir;
  }

  void DrawYZ_Y(const math::Vector3& basePosition, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 rad_2)
  {
    this->Draw_Rot01_0(0, math::Matrix34::GetRotationY, math::Matrix34::GetRotationZ, basePosition, worldMatrix, lowRad, highRad, rad_2);
  }

  void DrawYZ_Z(const math::Vector3& basePosition, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 rad_2)
  {
    this->Draw_Rot01_1(0, math::Matrix34::GetRotationY, math::Matrix34::GetRotationZ, basePosition, worldMatrix, lowRad, highRad, rad_2);
  }

  void DrawZY(f32 offset, const math::Matrix34& currentMatrix, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 low2Rad, f32 high2Rad)
  {
    math::Vector3 pos;

    u32 beginIndex = this->vertexCount;
    this->DrawZY_Y(pos, worldMatrix, lowRad, highRad, low2Rad);
    DrawUtil::VertexFormat vf0 = this->vertexList[beginIndex];
    DrawUtil::VertexFormat vf1 = this->vertexList[this->vertexCount - 1];
    u32 endIndex = this->vertexCount;

    this->DrawZY_Y(pos, worldMatrix, lowRad, highRad, high2Rad);
    DrawUtil::VertexFormat vf2 = this->vertexList[endIndex];
    DrawUtil::VertexFormat vf3 = this->vertexList[this->vertexCount - 1];

    this->DrawZY_Z(pos, worldMatrix, low2Rad, high2Rad, lowRad);
    this->DrawZY_Z(pos, worldMatrix, low2Rad, high2Rad, highRad);

    DrawUtil::VertexFormat vfBase;
    vfBase.pos = worldMatrix.TransformCoord(pos);
    vfBase.color = this->desc->jointColor;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf0;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf1;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf2;

    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vf3;

    // 今どこを向いているか？
    DrawUtil::VertexFormat vfDir;
    vfDir.pos = currentMatrix.TransformCoord(math::Vector3(50, 0, 0));
    vfDir.color.Set(0, 1, 0, 1);
    this->vertexList[this->vertexCount++] = vfBase;
    this->vertexList[this->vertexCount++] = vfDir;
  }

  void DrawZY_Y(const math::Vector3& basePosition, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 rad_2)
  {
    this->Draw_Rot01_0(0, math::Matrix34::GetRotationZ, math::Matrix34::GetRotationY, basePosition, worldMatrix, lowRad, highRad, rad_2);
  }

  void DrawZY_Z(const math::Vector3& basePosition, const math::Matrix34& worldMatrix, f32 lowRad, f32 highRad, f32 rad_2)
  {
    this->Draw_Rot01_1(0, math::Matrix34::GetRotationZ, math::Matrix34::GetRotationY, basePosition, worldMatrix, lowRad, highRad, rad_2);
  }

  McnfRenderPath::InitDescription* desc;                // 初期化設定
  DrawUtil::VertexFormat* vertexList;   // 頂点リスト
  u32 vertexLimit;                      // 頂点最大数

  u32 vertexCount;                      // 頂点数
  u32 lineDivCount;                     // ラインの分割数
};

void McnfRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  // シザー、カリング、フィルの設定
  DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
  DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);

  // マテリアル設定
  DrawUtil::SetMaterialAlpha(1.0f);
  
  // 簡易描画のデプステストを無しに設定する
  gfl2::gfx::DepthStencilStateObject::StateData stateData;
  stateData.m_DepthTestEnable = false;
  DrawUtil::SetDepthTestState(stateData);

  // 描画開始
	GFGL::BeginScene();
  DrawUtil::ClearScissor();

  // 簡易描画開始
  DrawUtil::BeginRender();

  // プロジェクション行列設定
  DrawUtil::SetProjectionMatrix(rDrawContext.m_Projection);
  // ビュー行列設定
  DrawUtil::SetViewMatrix(rDrawContext.m_View);
  // モデル行列設定
  DrawUtil::SetModelMatrix(gfl2::math::Matrix34::GetIdentity());

  // ノードを巡回して頂点リストを作成する
  scenegraph::Traverser::Visitor<scenegraph::instance::ModelInstanceNode, MotinConfigInstanceNodeVisitor>				visitor;
  visitor.desc = &m_InitDescription;
  visitor.desc->jointColor.x = 255.0f / 255.0f;
  visitor.desc->jointColor.y = 192.0f / 255.0f;
  visitor.desc->jointColor.z = 203.0f / 255.0f;
  visitor.vertexList = m_pVertexFormatList;
  visitor.vertexLimit = m_VertexFormatLimit;
  visitor.vertexCount = 0;
  visitor.lineDivCount = 10;
  scenegraph::SceneGraphManager::Traverse<scenegraph::instance::ModelInstanceNode, MotinConfigInstanceNodeVisitor>( &visitor );

  // 頂点リスト描画
  DrawUtil::DrawLines(m_pVertexFormatList, visitor.vertexCount, false);

  // 描画終了
  DrawUtil::EndRender();
	GFGL::EndScene();
}

}}}}

#endif
