using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using gfl2.clr.gfx;
using gfl2.clr.math;
using gfl2.clr.renderingengine;
using gfl2.clr.renderingengine.renderer;
using gfl2.clr.renderingengine.renderer.util;
using gfl2.clr.renderingengine.scenegraph;
using gfl2.clr.renderingengine.scenegraph.instance;
using gfl2.clr.renderingengine.scenegraph.resource;
using gfl2.clr.animation;
using gfl2.clr.poke_3d_lib;

namespace SimpleViewer
{
  public enum CameraType
  {
    DEFAULT,
    LEFT,
    RIGHT,
    BACK,
    TOP,
    BOTTOM,
    NumberOf
  }

  public enum ScreenSizeFormat
  {
    Format_400x240
  }

  public partial class RenderForm : Form
  {
    private const Int32 SCREEN_WIDTH = 400;
    private const Int32 SCREEN_HEIGHT = 240;

    private PokemonModelDataManager m_ModeldataManager = null;

    private DisplayDeviceHandle m_DisplayDeviceHandle;					//!< @brief ディスプレイのデバイスハンドル
    private ViewerRenderingPipeLine m_ViewerRenderingPipeLine;	//!< @brief ビューア用レンダリングパイプライン
    public ViewerRenderingPipeLine ViewerRenderingPipeLine
    {
      get { return m_ViewerRenderingPipeLine; }
    }

    private readonly float[] m_InnerCylinderRadiuses;
    private readonly float[] m_OuterCylinderRadiuses;

    private ScreenSizeFormat m_ScreenSizeFormat;

    private Int32 m_clearColorIndex;
    private gfl2.clr.gfx.Color[] m_clearColors;

    private CameraType m_CameraType;
    public CameraType CameraType
    {
      set { m_CameraType = value; }
      get { return m_CameraType; }
    }

    private gfl2.clr.math.Vector m_EyePosition;
    private gfl2.clr.math.Vector m_FocusPostion;
    private gfl2.clr.math.Vector m_ViewVector;

    private DefaultCamera m_DefaultCamera;
    private LeftCamera m_LeftCamera;
    private RightCamera m_RightCamera;
    private BackCamera m_BackCamera;
    private TopCamera m_TopCamera;
    private BottomCamera m_BottomCamera;

    private ICamera[] m_Cameras;

    public void ChangeScreenSize(ScreenSizeFormat format)
    {
      if (m_ScreenSizeFormat == format)
      {
        return;
      }

      m_ScreenSizeFormat = format;

      switch (m_ScreenSizeFormat)
      {
        case ScreenSizeFormat.Format_400x240:
          ClientSize = new System.Drawing.Size(400, 240);
          break;
      }
    }

    public RenderForm()
    {
      InitializeComponent();

      this.ClientSize = new System.Drawing.Size(
          (Int32)SCREEN_WIDTH,
          (Int32)SCREEN_HEIGHT);

      m_Cameras = new ICamera[(Int32)CameraType.NumberOf];

      m_InnerCylinderRadiuses = new float[(Int32)PokemonSize.Enum.NumberOf];
      m_InnerCylinderRadiuses[0] = 93.0f;
      m_InnerCylinderRadiuses[1] = 130.0f;
      m_InnerCylinderRadiuses[2] = 150.0f;
      m_OuterCylinderRadiuses = new float[(Int32)PokemonSize.Enum.NumberOf];
      m_OuterCylinderRadiuses[0] = 100.0f;
      m_OuterCylinderRadiuses[1] = 150.0f;
      m_OuterCylinderRadiuses[2] = 190.0f;

      m_EyePosition = new gfl2.clr.math.Vector(0.0f, 0.0f, 0.0f);
      m_ViewVector = new gfl2.clr.math.Vector(0.0f, 0.0f, 0.0f);

      m_ScreenSizeFormat = ScreenSizeFormat.Format_400x240;

      PokemonModel.CreateTreeNodeColorDictionary();
    }

    private void ViewForm_Load(object sender, EventArgs e)
    {
      m_clearColorIndex = 0;
      m_clearColors = new gfl2.clr.gfx.Color[4];
      m_clearColors[0] = new gfl2.clr.gfx.Color(0.5f, 0.5f, 0.5f, 1.0f);
      m_clearColors[1] = new gfl2.clr.gfx.Color(0.3f, 0.3f, 0.3f, 1.0f);
      m_clearColors[2] = new gfl2.clr.gfx.Color(0.0f, 0.0f, 0.0f, 1.0f);
      m_clearColors[3] = new gfl2.clr.gfx.Color(0.75f, 0.75f, 0.75f, 1.0f);

      UInt32 widthDiff = SCREEN_WIDTH - GFGL.GetNativeClientWidth(this.Handle);
      UInt32 heightDiff = SCREEN_HEIGHT - GFGL.GetNativeClientHeight(this.Handle);
      this.ClientSize = new System.Drawing.Size(
          (Int32)(SCREEN_WIDTH + widthDiff),
          (Int32)(SCREEN_HEIGHT + heightDiff));

      m_DisplayDeviceHandle = GFGL.CreateDisplay(this.Handle, Format.Enum.A8B8G8R8, Format.Enum.D24S8, MultiSample.Enum.None);

      string path = Application.ExecutablePath;
      string colorShaderPlaneFilePath;
      string colorShaderPlaneVertexShaderPath;
      string colorShaderPlanePixelShaderPath;
      string colorShaderPlaneTexturePath;
      string edgeGfbmdlPath;
      string[] edgeShaderPaths = new string[2];
      string[] edgeTexturePaths = new string[2];
      string gfbmdlPathNormal;
      string[] shaderPathsNormal = new string[2];
      string texturePathNormal;
      string gfbmdlPathId;
      string[] shaderPathsId = new string[4];
      string texturePathId;

#if DEBUG
			colorShaderPlaneFilePath = "../../../data/ColorShaderGfmdl/colorshaderplane.wingl.gfbmdl";
      colorShaderPlaneVertexShaderPath = "../../../data/ColorShaderGfmdl/FillScrean.wingl.gfbvsh";
      colorShaderPlanePixelShaderPath = "../../../data/ColorShaderGfmdl/Comb1_Manual.wingl.gfbfsh";
      colorShaderPlaneTexturePath = "../../../data/ColorShaderGfmdl/colorshadertexture.wingl.btex";
			edgeGfbmdlPath = "../../../data/EdgeMapVColAShader/exports/EdgeMapVColA.wingl.gfbmdl";
      edgeShaderPaths[0] = "../../../data/EdgeMapVColAShader/exports/Shaders/NormalToUv.wingl.gfbvsh";
			edgeShaderPaths[1] = "../../../data/EdgeMapVColAShader/exports/Shaders/Comb2_Manual.wingl.gfbfsh";
			edgeTexturePaths[0] = "../../../data/EdgeMapVColAShader/exports/Textures/NormalTexture.wingl.btex";
      edgeTexturePaths[1] = "../../../data/EdgeMapVColAShader/exports/Textures/ZInvNormalTexture.wingl.btex";

			gfbmdlPathNormal = "../../../data/EdgeMap/exports/EdgeMapShader.wingl.gfbmdl";
			shaderPathsNormal[0] = "../../../data/EdgeMap/exports/Shaders/Comb3_Manual.wingl.gfbvsh";
			shaderPathsNormal[1] = "../../../data/EdgeMap/exports/Shaders/Comb3_Manual.wingl.gfbfsh";
			texturePathNormal = "../../../data/EdgeMap/exports/Texture/EdgeMapImage.wingl.tga";

			gfbmdlPathId = "../../../data/IDEdgeMap/exports/IDEdgeMap.wingl.gfbmdl";
			shaderPathsId[0] = "../../../data/IDEdgeMap/exports/Shader/Comb2_Manual.wingl.gfbvsh";
			shaderPathsId[1] = "../../../data/IDEdgeMap/exports/Shader/Comb2_Manual.wingl.gfbfsh";
			shaderPathsId[2] = "../../../data/IDEdgeMap/exports/Shader/Comb3_Manual.wingl.gfbvsh";
			shaderPathsId[3] = "../../../data/IDEdgeMap/exports/Shader/Comb3_Manual.wingl.gfbfsh";
			texturePathId = "../../../data/IDEdgeMap/exports/Texture/MapImage.wingl.tga";
#else
      colorShaderPlaneFilePath = "data/ColorShaderGfmdl/colorshaderplane.wingl.gfbmdl";
      colorShaderPlaneVertexShaderPath = "data/ColorShaderGfmdl/FillScrean.wingl.gfbvsh";
      colorShaderPlanePixelShaderPath = "data/ColorShaderGfmdl/Comb1_Manual.wingl.gfbfsh";
      colorShaderPlaneTexturePath = "data/ColorShaderGfmdl/colorshadertexture.wingl.btex";
      edgeGfbmdlPath = "data/EdgeMapVColAShader/exports/EdgeMapVColA.wingl.gfbmdl";
      edgeShaderPaths[0] = "data/EdgeMapVColAShader/exports/Shaders/NormalToUv.wingl.gfbvsh";
      edgeShaderPaths[1] = "data/EdgeMapVColAShader/exports/Shaders/Comb2_Manual.wingl.gfbfsh";
      edgeTexturePaths[0] = "data/EdgeMapVColAShader/exports/Textures/NormalTexture.wingl.btex";
      edgeTexturePaths[1] = "data/EdgeMapVColAShader/exports/Textures/ZInvNormalTexture.wingl.btex";

      gfbmdlPathNormal = "data/EdgeMap/exports/EdgeMapShader.wingl.gfbmdl";
      shaderPathsNormal[0] = "data/EdgeMap/exports/Shaders/Comb3_Manual.wingl.gfbvsh";
      shaderPathsNormal[1] = "data/EdgeMap/exports/Shaders/Comb3_Manual.wingl.gfbfsh";
      texturePathNormal = "data/EdgeMap/exports/Texture/EdgeMapImage.wingl.tga";

      gfbmdlPathId = "data/IDEdgeMap/exports/IDEdgeMap.wingl.gfbmdl";
      shaderPathsId[0] = "data/IDEdgeMap/exports/Shader/Comb2_Manual.wingl.gfbvsh";
      shaderPathsId[1] = "data/IDEdgeMap/exports/Shader/Comb2_Manual.wingl.gfbfsh";
      shaderPathsId[2] = "data/IDEdgeMap/exports/Shader/Comb3_Manual.wingl.gfbvsh";
      shaderPathsId[3] = "data/IDEdgeMap/exports/Shader/Comb3_Manual.wingl.gfbfsh";
      texturePathId = "data/IDEdgeMap/exports/Texture/MapImage.wingl.tga";
#endif

      m_ViewerRenderingPipeLine = new ViewerRenderingPipeLine(
          colorShaderPlaneFilePath,
          colorShaderPlaneVertexShaderPath,
          colorShaderPlanePixelShaderPath,
          colorShaderPlaneTexturePath,
          edgeGfbmdlPath,
          edgeShaderPaths,
          edgeTexturePaths,
          gfbmdlPathNormal,
          shaderPathsNormal,
          texturePathNormal,
          gfbmdlPathId,
          shaderPathsId,
          texturePathId,
          SCREEN_WIDTH,
          SCREEN_HEIGHT);

      DrawUtil.Initialize(m_DisplayDeviceHandle);

      // 射影行列を描画マネージャに設定
      m_DefaultCamera = new DefaultCamera((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 40.0f);
      m_LeftCamera = new LeftCamera((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 40.0f);
      m_RightCamera = new RightCamera((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 40.0f);
      m_BackCamera = new BackCamera((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 40.0f);
      m_TopCamera = new TopCamera((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 40.0f);
      m_BottomCamera = new BottomCamera((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 40.0f);

      m_Cameras[(Int32)CameraType.DEFAULT] = m_DefaultCamera;
      m_Cameras[(Int32)CameraType.LEFT] = m_LeftCamera;
      m_Cameras[(Int32)CameraType.RIGHT] = m_RightCamera;
      m_Cameras[(Int32)CameraType.BACK] = m_BackCamera;
      m_Cameras[(Int32)CameraType.TOP] = m_TopCamera;
      m_Cameras[(Int32)CameraType.BOTTOM] = m_BottomCamera;

      m_Cameras[(Int32)m_CameraType].Reset(null);

      m_ViewerRenderingPipeLine.SetProjectionMatrix(m_Cameras[(Int32)m_CameraType].Projection);

      m_ViewerRenderingPipeLine.ChangeModelVisibility(true);
      m_ViewerRenderingPipeLine.ChangeWireFrameVisibility(false);
      m_ViewerRenderingPipeLine.ChangeJointVisibility(false);
      m_ViewerRenderingPipeLine.ChangeGridVisibility(false);
      m_ViewerRenderingPipeLine.SetLightPositionVisible(false);
      m_ViewerRenderingPipeLine.ChangeGaugeVisibility(false);

      this.ChangeScreenSize(ScreenSizeFormat.Format_400x240);
    }

    public void OnUpdate()
    {
      m_Cameras[(Int32)m_CameraType].Update();

      // set projection matrix
      Matrix projectionMatrix = m_Cameras[(Int32)m_CameraType].Projection;
      m_ViewerRenderingPipeLine.SetProjectionMatrix(projectionMatrix);

      // set view matrix
      Matrix viewMatrix = m_Cameras[(Int32)m_CameraType].View;
      m_ViewerRenderingPipeLine.SetViewMatrix(viewMatrix);

      Matrix mtxInvView = viewMatrix.Inverse();
      m_EyePosition.x = 0.0f;
      m_EyePosition.y = 0.0f;
      m_EyePosition.z = 0.0f;
      m_EyePosition.w = 0.0f;

      m_EyePosition = mtxInvView * m_EyePosition;
      m_ViewVector.x = -viewMatrix.GetElem(2, 0);
      m_ViewVector.y = -viewMatrix.GetElem(2, 1);
      m_ViewVector.z = -viewMatrix.GetElem(2, 2);
      m_ViewVector.w = 0.0f;

      m_FocusPostion = m_EyePosition + m_ViewVector * m_Cameras[(Int32)m_CameraType].CameraInterval;

      if (m_ModeldataManager != null)
      {
        m_ViewerRenderingPipeLine.SetModelPosition(m_ModeldataManager.Position);
        m_ViewerRenderingPipeLine.SetPokemonSize(m_ModeldataManager.Size);
        m_ViewerRenderingPipeLine.SetInterestPosition(m_ModeldataManager.InterestPosition);
        m_ViewerRenderingPipeLine.SetInnerCylinderColor(m_ModeldataManager.InnerCylinderColor);
        m_ViewerRenderingPipeLine.SetOuterCylinderColor(m_ModeldataManager.OuterCylinderColor);

        // resize AABB with scale
        AABB aabb = m_ModeldataManager.PokemonModel.GetModelAABB();
        gfl2.clr.math.Vector min = new gfl2.clr.math.Vector(aabb.GetMin());
        gfl2.clr.math.Vector max = new gfl2.clr.math.Vector(aabb.GetMax());

        min.x *= m_ModeldataManager.Scale.x;
        min.y *= m_ModeldataManager.Scale.y;
        min.z *= m_ModeldataManager.Scale.z;

        max.x *= m_ModeldataManager.Scale.x;
        max.y *= m_ModeldataManager.Scale.y;
        max.z *= m_ModeldataManager.Scale.z;

        // set current position to model position
        min.x += m_ModeldataManager.Position.x;
        min.y += m_ModeldataManager.Position.y;
        min.z += m_ModeldataManager.Position.z;

        max.x += m_ModeldataManager.Position.x;
        max.y += m_ModeldataManager.Position.y;
        max.z += m_ModeldataManager.Position.z;

        m_ViewerRenderingPipeLine.SetModelAABB(min, max);
        m_ViewerRenderingPipeLine.SetModelAABBColor(m_ModeldataManager.ModelAABBColor);
      }

      m_ViewerRenderingPipeLine.Update();
    }

    public void LoadPokemon(string gfbmdlFile, string shaderDirectory, string textureDirectory)
    {
      m_ModeldataManager = new PokemonModelDataManager(gfbmdlFile, shaderDirectory, textureDirectory);

      m_ViewerRenderingPipeLine.AddDrawableModel(m_ModeldataManager.PokemonModel);

      // set textures for color shader
      m_ViewerRenderingPipeLine.AddColorTexture(m_ModeldataManager.PokemonModel);
      m_ViewerRenderingPipeLine.SetColorShaderTextureColor(m_ModeldataManager.PokemonModel);

      // add edge draw node to view pipeline
      m_ViewerRenderingPipeLine.AddEdgeRenderTarget(m_ModeldataManager.PokemonModel);

      m_Cameras[(Int32)m_CameraType].Reset(m_ModeldataManager);

      SceneGraphManager.TraverseModel(m_Cameras[(Int32)m_CameraType].View);
    }

    public void OnDraw()
    {
      m_ViewerRenderingPipeLine.SetClearColor(m_clearColors[m_clearColorIndex]);
      GFGL.SetActiveDisplay(m_DisplayDeviceHandle);

      Surface backBuffer = GFGL.GetBackBuffer(m_DisplayDeviceHandle);
      Surface depthBuffer = GFGL.GetDepthStencilBuffer(m_DisplayDeviceHandle);

      m_ViewerRenderingPipeLine.SetRenderTarget(backBuffer);
      m_ViewerRenderingPipeLine.SetDepthStencil(depthBuffer);
      m_ViewerRenderingPipeLine.Execute();
    }

    private void ViewForm_Closed(object sender, FormClosedEventArgs e)
    {
      m_ModeldataManager.Dispose();
      m_ViewerRenderingPipeLine.Dispose();

      ResourceManager.Terminate();
      SceneGraphManager.Terminate();

      DrawUtil.Terminate();
      GFGL.Finish();
      GFGL.SetActiveDisplay(null);
      GFGL.DestroyDisplay(m_DisplayDeviceHandle);
      GFGL.Terminate();
    }

    public void PrintScreen(string fileName)
    {
      PCDebugUtil.PrintScreen(fileName, m_DisplayDeviceHandle);
    }

  }
}
