#include <RenderingEngine/include/Renderer/Util/gfl2_MakeReductionImagePath.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CombinerAttribute.h>
#include <gfx/include/gfl2_DrawUtil.h>
#include <math/include/gfl2_MathUtil.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

/**
 * @brief �R���X�g���N�^
 */
MakeReductionImagePath::MakeReductionImagePath() :
  m_ScreenWidth(0),
  m_ScreenHeight(0),
  m_FrameBufferWidth(0),
  m_FrameBufferHeight(0),
  m_pSourceTexture(NULL),
  m_pDestinationTexture(NULL)
{
  memset(m_VertexDataArray, 0, sizeof(m_VertexDataArray));
}

/**
 * @brief ������
 * @param pAllocator �e�N�X�`�����쐬���邽�߂̃A���P�[�^
 * @param desc �������ɕK�v�Ȑݒ�f�[�^
 * @note ���T�C�Y�ΏۂƂȂ�e�N�X�`����ݒ肷��
 */
void MakeReductionImagePath::Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc)
{
  GFL_ASSERT(!m_pDestinationTexture && "MakeReductionImagePath::Initialize() doesn't work correctly. This is already initialized.");
  m_ScreenWidth = desc.m_ScreenWidth;
  m_ScreenHeight = desc.m_ScreenHeight;
  m_FrameBufferWidth = m_ScreenWidth;
  m_FrameBufferHeight = m_ScreenHeight;
  f32 createBufferWidth = m_FrameBufferWidth;
  f32 createBufferHeight = m_FrameBufferHeight;

#if defined(GF_PLATFORM_CTR)
  // CTR�ł�2�ׂ̂���̍ő�̑傫�����������ăt���[���o�b�t�@�̑傫�����w��
  for (u32 size = 8; size < 1024; size *= 2)
  {
    if (size >= m_FrameBufferWidth)
    {
      m_FrameBufferWidth = size;
      break;
    }
  }
  for (u32 size = 8; size < 1024; size *= 2)
  {
    if (size >= m_FrameBufferHeight)
    {
      m_FrameBufferHeight = size;
      break;
    }
  }

  // �t���[���o�b�t�@�쐬���̉���/�c�������ւ���
  createBufferWidth = m_FrameBufferHeight;
  createBufferHeight = m_FrameBufferWidth;
#endif

  // ���T�C�Y�Ώۂ̃e�N�X�`����ݒ�
  m_pSourceTexture = desc.m_pSourceTexture;

  GFL_ASSERT((m_ScreenWidth > 0) && (m_ScreenHeight > 0) && "MakeReductionImagePath::Initialize() doesn't work correctly. ScreenWidth and ScreenHeight must be larger than 0.");
  // ��ʂ��L���v�`������p�̃e�N�X�`���𐶐�
  m_pDestinationTexture = gfl2::gfx::GFGL::CreateTexture(
    pAllocator,
    createBufferWidth,
    createBufferHeight,
    1,
    gfl2::gfx::Usage::RenderTarget,
    gfl2::gfx::Format::A8R8G8B8,
    gfl2::gfx::Pool::Default
  );	
}

/**
 * @brief �f�X�g���N�^
 */
MakeReductionImagePath::~MakeReductionImagePath()
{
  // �k���摜�쐬�p�̃e�N�X�`����j��
  GFL_DELETE(m_pDestinationTexture);

  // �k���Ώۂ̃e�N�X�`��SourceTexture�͊O������n�����e�N�X�`���Ȃ̂ŉ�����Ȃ�
}

/**
 * @brief �k���Ώۂ̃e�N�X�`����ݒ�
 * @param pSourceTexture �k���Ώۂ̃e�N�X�`��
 */
void MakeReductionImagePath::SetSourceTexture(const gfl2::gfx::Texture* pSourceTexture)
{
  m_pSourceTexture = pSourceTexture;
}

/**
 * @brief �k����̃e�N�X�`�����擾
 * @return const gfl2::gfx::Texture* �k����̃e�N�X�`��
 */
const gfl2::gfx::Texture* MakeReductionImagePath::GetDestinationTexture() const
{
  return m_pDestinationTexture;
}

void MakeReductionImagePath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  GFL_ASSERT((m_pDestinationTexture != NULL) && "MakeReductionImagePath::Execute() doesn't work correctly. MakeReductionImagePath is not initialized.");

  if (!m_pSourceTexture)
  {// �k���Ώۂ̃\�[�X�摜���ݒ肳��Ă��Ȃ���Ή������Ȃ�
    return;
  }

  const gfx::Surface* pTargetSurface = m_pDestinationTexture->GetSurface();

  gfl2::gfx::GFGL::SetRenderTarget(m_pDestinationTexture->GetSurface());
	gfx::GFGL::SetDepthStencil(NULL);

  // DrawUtil�ɐݒ肳��Ă��錻�݂̃e�N�X�`���ƃX�N���[���̃T�C�Y���L�^(�����I����Ɍ��ɖ߂�)
  const gfl2::gfx::Texture* originalDrawUtilTexture = gfl2::gfx::DrawUtil::GetTexture();
  u32 originalDrawUtilScreenWidth = gfl2::gfx::DrawUtil::GetScreenWidth();
  u32 originalDrawUtilScreenHeight = gfl2::gfx::DrawUtil::GetScreenHeight();

  gfl2::gfx::GFGL::BeginScene();

  gfl2::gfx::DrawUtil::BeginRender();

  gfl2::gfx::GFGL::SetViewport(0, 0, m_FrameBufferWidth, m_FrameBufferHeight);
  gfl2::gfx::GFGL::SetScissor(0, 0, m_FrameBufferWidth, m_FrameBufferHeight);

  gfl2::gfx::DrawUtil::SetScreenSize(m_FrameBufferWidth, m_FrameBufferHeight);
  // �`��O�ɕ`��Ώۂ̃����_�[�^�[�Q�b�g�����ŃN���A
  gfl2::gfx::GFGL::ClearRenderTarget(pTargetSurface, gfl2::gfx::Color(0.0f, 0.0f, 0.0f, 1.0f));

  gfl2::gfx::DepthStencilStateObject::StateData depthStateData;
  depthStateData.m_DepthTestEnable = false;
  depthStateData.m_DepthWriteEnable = false;
  gfl2::gfx::DrawUtil::SetDepthTestState(depthStateData);

  gfl2::gfx::DrawUtil::SetBlendTestState(
    false,
    gfl2::gfx::BlendOp::Add,
    gfl2::gfx::BlendFunc::One,
    gfl2::gfx::BlendFunc::One,
    gfl2::gfx::BlendFunc::Zero,
    gfl2::gfx::BlendFunc::Zero);
  gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
  gfl2::gfx::DrawUtil::SetTexture(m_pSourceTexture);
  gfl2::gfx::DrawUtil::DrawRect(0.0f, 0.0f, m_FrameBufferWidth, m_FrameBufferHeight);

  gfl2::gfx::DrawUtil::EndRender();

  gfl2::gfx::GFGL::EndScene();

  // �`�悪�I������̂Ŗ{���̃����_�[�^�[�Q�b�g�ɖ߂�
  gfl2::gfx::GFGL::SetRenderTarget(rDrawContext.m_pRenderTarget);
  gfl2::gfx::GFGL::SetDepthStencil(rDrawContext.m_pDepthStencil);

  // �`�悪�I������̂�DrawUtil�̃e�N�X�`���ƃX�N���[���T�C�Y�����ɖ߂�
  gfl2::gfx::DrawUtil::SetTexture(originalDrawUtilTexture);
  gfl2::gfx::DrawUtil::SetScreenSize(originalDrawUtilScreenWidth, originalDrawUtilScreenHeight);
}

}}}}
