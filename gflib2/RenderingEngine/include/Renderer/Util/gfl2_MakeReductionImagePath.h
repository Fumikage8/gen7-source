#if !defined(GFL2_RENDERINGENGINE_RENDERER_UTIL_MAKEREDUCTIONIMAGEPATH_H_INCLUDED)
#define GFL2_RENDERINGENGINE_RENDERER_UTIL_MAKEREDUCTIONIMAGEPATH_H_INCLUDED
#pragma once

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <gfx/include/gfl2_DrawUtil.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

//-----------------------------------------------------------
//! @brief �k���摜�`��p�X�N���X
//-----------------------------------------------------------
class MakeReductionImagePath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
  /**
   * @brief �k���摜�쐬�p�X�̏��������
   */
  struct InitDescription
  {
    u32 m_ScreenWidth;                            //!< @brief �X�N���[���̉���
    u32 m_ScreenHeight;                           //!< @brief �X�N���[���̏c��
    const gfl2::gfx::Texture* m_pSourceTexture;   //!< @brief ���T�C�Y�Ώۂ̃e�N�X�`��
    InitDescription() : m_ScreenWidth(0), m_ScreenHeight(0), m_pSourceTexture(NULL){}
  };

  MakeReductionImagePath();
  virtual ~MakeReductionImagePath();

  void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

  void Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc);

  /**
   * @brief �k���Ώۂ̃e�N�X�`����ݒ�
   * @param pSourceTexture �k���Ώۂ̃e�N�X�`��
   */
  void SetSourceTexture(const gfl2::gfx::Texture* pSourceTexture);

  /**
   * @brief �k����̃e�N�X�`�����擾
   * @return const gfl2::gfx::Texture* �k����̃e�N�X�`��
   */
  const gfl2::gfx::Texture* GetDestinationTexture() const;

private:
  u32 m_ScreenWidth;                                //!< @brief �X�N���[���̉���
  u32 m_ScreenHeight;                               //!< @brief �X�N���[���̏c��
  u32 m_FrameBufferWidth;                           //!< @brief �t���[���o�b�t�@�̉���
  u32 m_FrameBufferHeight;                          //!< @brief �t���[���o�b�t�@�̏c��
  static const u32 VERTEX_DATA_ARRAY_LENGTH = 6;    //!< @brief ��`��`�悷�邽�߂̒��_��
  gfl2::gfx::DrawUtil::VertexFormat m_VertexDataArray[VERTEX_DATA_ARRAY_LENGTH];  // !< @brief ��`��`�悷�邽�߂̒��_�z��
  const gfl2::gfx::Texture* m_pSourceTexture;       //!< @brief �`��ΏۂƂȂ�e�N�X�`��
  const gfl2::gfx::Texture* m_pDestinationTexture;  //!< @brief ��ʂ��R�s�[����p�̃e�N�X�`��
};

}}}}

#endif
