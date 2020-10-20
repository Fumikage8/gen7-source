//=============================================================================
/**
 * @file    gfl2_RenderingPipeLine.h
 * @brief   RenderingPipeLineクラス（描画の流れを管理するクラス）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.4.13    ishiguro  DrawContextを継承先で使えるように変更
 *
 */
//=============================================================================
#ifndef GFLIB2_RENDERINGENGINE_RENDERER_RENDERINGPIPELINE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_RENDERINGPIPELINE_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/renderer/gfl2_IShaderDriver.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

namespace gfl2 { namespace renderingengine { namespace renderer { 

  class RenderingPipeLine
  {
  public:

    //-----------------------------------------------------------
    //! @brief 描画コンテキスト構造体
    //-----------------------------------------------------------
    struct DrawContext
    {
      math::Matrix44  m_Projection;   // 透視変換行列
      math::Matrix34  m_View;    // ビュー変換行列
      gfx::Surface  *m_pRenderTarget;
      gfx::Surface  *m_pDepthStencil;
      u16 m_DisplayWidth;
      u16 m_DisplayHeight;

      DrawContext():
        m_pRenderTarget(NULL),
        m_pDepthStencil(NULL),
        m_DisplayWidth( 0 ),
        m_DisplayHeight( 0 )
      {}
    };

    //-----------------------------------------------------------
    //! @brief 描画パスインターフェイスクラス
    //-----------------------------------------------------------
    class IRenderPath
    {
    public:
      IRenderPath(){}
      virtual ~IRenderPath(){}
      virtual void Update( const DrawContext& rDrawContext )
      {
        (void)rDrawContext;
      };
      virtual void Execute( const DrawContext& rDrawContext ) = 0;
      virtual void CallDisplayList( const DrawContext& rDrawContext )
      {
        // CallDisplayListを実装しない場合はExecuteで代用
        Execute( rDrawContext );
      }
    };

    //-----------------------------------------------------------
    //! @brief シーン描画パスクラス
    //-----------------------------------------------------------
    class SceneRenderPath : public IRenderPath
    {
    public:
      struct InitDescription{
        DrawManager::InitDescription m_DrawManagerDesc;

        InitDescription() : m_DrawManagerDesc( 256, 1024 * 8, 16 ){}//128*8*8 = r02のmap01_11_11が８つ分くらい。
      };

      struct Camera{
        math::Matrix44 m_Projection;    // 透視変換行列
        math::Matrix34 m_View;    // ビュー変換行列
      };

      SceneRenderPath();
      virtual ~SceneRenderPath();

      void Initialize( gfl2::gfx::IGLAllocator * pGLAllocator, const InitDescription& desc );
      void AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode );
      void RemoveDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode );
      void RemoveAllDrawEnv();

      DrawManager* GetDrawManager(){ return m_pDrawManager; }

      virtual const IShaderDriver* ShaderOverride(){ return NULL; }
      virtual const gfx::GLFixedSizeContainer<scenegraph::instance::DrawableNode*>* DrawableNodeOverride(){ return NULL; }

      virtual void SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );
      virtual void ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil );
      virtual const Camera* CameraOverride(){ return NULL; }

      virtual void Execute( const DrawContext& rDrawContext );

      void SetupDraw( const DrawContext& rDrawContext, const gfl2::math::Matrix44* pProj = NULL, const gfl2::math::Matrix34* pView = NULL );
      void DrawManagerDraw(bool fUpdate);
      void ReuseCommand();

    protected:

      void SetDrawableNode();
      void SetCamera(const DrawContext& rDrawContext);

      DrawManager *m_pDrawManager;
      gfx::GLFixedSizeContainer<scenegraph::instance::CullingObjectNode*> m_CullingObjectNodeContainer;
      gfl2::gfx::DisplayList *m_pDisplayList;
    };

    //-----------------------------------------------------------
    //! @brief プリゼントパスクラス
    //-----------------------------------------------------------
    class PresentPath : public IRenderPath
    {
    public:
      typedef void (*DebugDrawCallBackFunc)();
      PresentPath() : IRenderPath(), m_pDebugDrawCallBackFunc(NULL) {}
      virtual ~PresentPath(){}

      virtual void Execute( const DrawContext& rDrawContext );

    private:
      DebugDrawCallBackFunc m_pDebugDrawCallBackFunc;
    };

    //===========================================================
    // 以下、RenderingPipeLine
    //===========================================================
    RenderingPipeLine();
    virtual ~RenderingPipeLine();

    //! @name 描画先設定
    //@{
    inline void SetRenderTarget(gfx::Surface* const pSurface){ m_DrawContext.m_pRenderTarget = pSurface; }
    inline const gfx::Surface* GetRenderTarget(){ return m_DrawContext.m_pRenderTarget; }
    inline void SetDepthStencil(gfx::Surface* const pSurface){ m_DrawContext.m_pDepthStencil = pSurface; }
    inline const gfx::Surface* GetDepthStencil(){ return m_DrawContext.m_pDepthStencil; }
    inline void SetDisplaySize( u16 w, u16 h ){ m_DrawContext.m_DisplayWidth = w; m_DrawContext.m_DisplayHeight = h; }
    //@}

    //! @name マトリクス操作
    //@{
    inline void SetProjectionMatrix( const math::Matrix44& mat ){ m_DrawContext.m_Projection = mat; }
    inline const math::Matrix44& GetProjectionMatrix() const { return m_DrawContext.m_Projection; }

    inline void SetViewMatrix( const math::Matrix34& mat ){ m_DrawContext.m_View = mat; }
    inline const math::Matrix34& GetViewMatrix() const { return m_DrawContext.m_View; }
    //@}

    void Update();
    void Execute();
    void CallDisplayList();

    virtual b32 StartRenderPath() = 0;
    virtual IRenderPath* GetRenderPath() = 0;
    virtual b32 NextRenderPath() = 0;

  protected:
    DrawContext m_DrawContext;
  };

  class DefaultRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
  {
  public:
    DefaultRenderingPipeLine(gfl2::gfx::IGLAllocator * pGLAllocator) : gfl2::renderingengine::renderer::RenderingPipeLine(),
      m_RenderPathCnt(0)
    {
      SceneRenderPath::InitDescription desc;
      m_SceneRenderPath.Initialize(pGLAllocator, desc);
    }

    virtual ~DefaultRenderingPipeLine()
    {

    }

    void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode)
    {
      m_SceneRenderPath.AddDrawEnv(pNode);
    }

  private:
    virtual b32 StartRenderPath()
    {
      m_RenderPathCnt = 0;
      return true;
    }

    virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath()
    {
      switch( m_RenderPathCnt ){
      case 0:
        return &m_SceneRenderPath;
        break;
      case 1:
        //return &m_PresentPath;
        break;
      }
      return NULL;
    }

    virtual b32 NextRenderPath()
    {
      m_RenderPathCnt++;
      if ( m_RenderPathCnt == 2 )
        return false;
      return true;
    }

    gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath m_SceneRenderPath;
    gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath     m_PresentPath;
    u32 m_RenderPathCnt;

  };

}}}

#endif

