using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Input;
using gfl2.clr.gfx;
using gfl2.clr.math;
using gfl2.clr.renderingengine;
using gfl2.clr.renderingengine.renderer;
using gfl2.clr.renderingengine.renderer.util;
using gfl2.clr.renderingengine.scenegraph;
using gfl2.clr.renderingengine.scenegraph.instance;
using gfl2.clr.renderingengine.scenegraph.resource;
using gfl2.clr.animation;

namespace ModelViewer
{
	public enum CameraType
	{
		DEFAULT,
		STATUS,
		KW,
		BATTLE,
		NumberOf
	}

	public enum ScreenSizeFormat
	{
		Format_400x240,
		Format_600x360,
		Format_800x480,
	};

	/**
	 * @brief 描画画面を構築するフォーム
	 */
	public partial class RenderForm : Form, IModelMotionResourceChangeListener
	{
		private const Int32 SCREEN_WIDTH = 400;
		private const Int32 SCREEN_HEIGHT = 240;
		private PokemonModelDataManager m_ModelDataManager = null;
		private DisplayDeviceHandle m_DisplayDeviceHandle;					//!< @brief ディスプレイのデバイスハンドル
		private ViewerRenderingPipeLine m_ViewerRenderingPipeLine;	//!< @brief ビューア用レンダリングパイプライン
		public ViewerRenderingPipeLine ViewerRenderingPipeLine
		{
			get { return m_ViewerRenderingPipeLine; }
		}
		private Int32 m_clearColorIndex;														//!< @brief 画面クリア時の色のインデックス
		private gfl2.clr.gfx.Color[] m_clearColors;									//!< @brief 画面クリア時の色

		private CameraType m_CameraType;
		public CameraType CameraType
		{
			set { m_CameraType = value; }

			get { return m_CameraType; }//!< @todo: getは外したい
		}
		private DefaultCamera m_DefaultCamera;		//!< @brief デフォルトカメラ
		private BattleCamera m_BattleCamera;			//!< @brief バトルカメラ
		public BattleCamera BattleCamera
		{
			get { return m_BattleCamera; }
		}
		private ICamera[] m_Cameras;							//!< @brief 使用するカメラ

		//!< @brief リソース削除フラグ
		private bool m_IsResourceDeleted;
		public bool IsResourceDeleted
		{
			set { m_IsResourceDeleted = value; }
		}

    //!< @brief 被写界深度の有効/無効フラグ
    private bool m_IsEnabledDof;

		/**
		 * @brief 背景データの追加
		 * @param PokemonModel モデルデータ
		 */
		public void AddBackgroundModel(gfl2.clr.poke_3d_lib.PokemonModel model)
		{
			m_ViewerRenderingPipeLine.AddBackgroundModel(model);
		}

		/**
		 * @brief 背景データの削除
		 * @param PokemonModel モデルデータ
		 */
		public void RemoveBackgroundModel(gfl2.clr.poke_3d_lib.PokemonModel model)
		{
			m_ViewerRenderingPipeLine.RemoveBackgroundModel(model);
		}

		/**
		 * @brief 現在のカメラの状態をリセット
		 * @param modelDataManagerList モデルデータのリスト
		 * @param index 現在選択されているモデルのインデックス
		 */
		public void ResetCamera(PokemonModelDataManager modelDataManager)
		{
			m_Cameras[(Int32)m_CameraType].Reset(modelDataManager);
		}

		private bool m_IsFocusPointVisible;				//!< @brief 注視点の描画ON/OFFフラグ
		public bool IsFocusPointVisible
		{
			set { m_IsFocusPointVisible = value; }
		}

		private gfl2.clr.math.Vector m_EyePosition;
		private gfl2.clr.math.Vector m_FocusPosition;
		private gfl2.clr.math.Vector m_ViewVector;

		private readonly float[] m_InnerTubeRadiuses;
		private readonly float[] m_OuterTubeRadiuses;

		private ScreenSizeFormat m_ScreenSizeFormat;

		private readonly Keyboard m_Keyboard;
		private readonly Mouse m_Mouse;

		private Label[] m_MotionBinarySizeLabels;
    private int[] m_MotionBinarySizes;

		private Int32 m_AllBinarySize;

		/**
		 * @brief 座標情報などのON/OFF切り替え
		 * @param enable 有効/無効
		 */
		public void SetEnabledInformationVisibility(bool enable)
		{
			performanceTimeLabel.Visible = enable;
			eyePositionLabel.Visible = enable;
			focusPositionLabel.Visible = enable;
			upDirectionLabel.Visible = enable;
			allBinarySizeLabel.Visible = enable;
			Array.ForEach(m_MotionBinarySizeLabels, data => data.Visible = enable);
			battleBinarySizeLabel.Visible = enable;
      fpsLabel.Visible = enable;
			walkSpeedLabel.Visible = enable;
			walkSpeedLabel2.Visible = enable;
      polygonCountLabel.Visible = enable;
      materialCountLabel.Visible = enable;
      jointCountLabel.Visible = enable;
		}

		public Matrix GetCurrentViewMatrix()
		{
			return m_Cameras[(Int32)m_CameraType].View;
		}

		public Matrix GetCurrentProjectionMatrix()
		{
			return m_Cameras[(Int32)m_CameraType].Projection;
		}

		public float GetCurrentFieldOfView()
		{
			return m_Cameras[(Int32)m_CameraType].FieldOfView;
		}

		public float GetCurrentFarClip()
		{
			return m_Cameras[(Int32)m_CameraType].FarClip;
		}

		public float GetCurrentNearClip()
		{
			return m_Cameras[(Int32)m_CameraType].NearClip;
		}

		/**
		 * @brief 座標情報などのON/OFF切り替え
		 * @param format 解像度の種類
		 */
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
				case ScreenSizeFormat.Format_600x360:
					ClientSize = new System.Drawing.Size(600, 360);
					break;
				case ScreenSizeFormat.Format_800x480:
					ClientSize = new System.Drawing.Size(800, 480);
					break;
			}
		}

		/**
		 * @brief コンストラクタ
		 * @param animationTimeLine タイムライン
		 * @param applicationDataManager データマネージャ
		 */
		public RenderForm(
			Keyboard keyboard,
			Mouse mouse)
		{
			m_Keyboard = keyboard;
			m_Mouse = mouse;

			InitializeComponent();

      // ウインドウスタイルによってクライアント領域の幅が変わることがあるので補正
      this.ClientSize = new System.Drawing.Size(
        (Int32)SCREEN_WIDTH,
        (Int32)SCREEN_HEIGHT);
			m_Cameras = new ICamera[(Int32)CameraType.NumberOf];

			m_IsFocusPointVisible = false;

			// 円柱の半径を設定(@todo: 外部ファイルで設定できるようにしたほうがいい)
			m_InnerTubeRadiuses = new float[(Int32)PokemonSize.Enum.NumberOf];
			m_InnerTubeRadiuses[0] = 93.0f;
			m_InnerTubeRadiuses[1] = 130.0f;
			m_InnerTubeRadiuses[2] = 150.0f;
			m_OuterTubeRadiuses = new float[(Int32)PokemonSize.Enum.NumberOf];
			m_OuterTubeRadiuses[0] = 100.0f;
			m_OuterTubeRadiuses[1] = 150.0f;
			m_OuterTubeRadiuses[2] = 190.0f;

			m_EyePosition = new gfl2.clr.math.Vector(0.0f, 0.0f, 0.0f);
			m_ViewVector = new gfl2.clr.math.Vector(0.0f, 0.0f, 0.0f);

			m_ScreenSizeFormat = ScreenSizeFormat.Format_400x240;

			m_IsResourceDeleted = false;

			m_MotionBinarySizeLabels = new Label[(Int32)GfmotPathSet.MotionType.Numberof];
			m_MotionBinarySizeLabels[(Int32)GfmotPathSet.MotionType.BATTLE] = this.battleBinarySizeLabel;
			m_MotionBinarySizeLabels[(Int32)GfmotPathSet.MotionType.KW] = this.kwBinarySizeLabel;
			m_MotionBinarySizeLabels[(Int32)GfmotPathSet.MotionType.FIELD] = this.fieldBinarySizeLabel;
			m_MotionBinarySizes = new Int32[(Int32)GfmotPathSet.MotionType.Numberof];

      m_IsEnabledDof = false;
		}

		/**
		 * @brief OnInitに相当
		 * @param object
		 * @param EventArgs
		 */
		private void ViewForm_Load(object sender, EventArgs e)
		{
			m_clearColorIndex = 0;
			m_clearColors = new gfl2.clr.gfx.Color[4];
			m_clearColors[0] = new gfl2.clr.gfx.Color(0.5f, 0.5f, 0.5f, 0.0f);
			m_clearColors[1] = new gfl2.clr.gfx.Color(0.3f, 0.3f, 0.3f, 0.0f);
			m_clearColors[2] = new gfl2.clr.gfx.Color(0.0f, 0.0f, 0.0f, 0.0f);
			m_clearColors[3] = new gfl2.clr.gfx.Color(0.75f, 0.75f, 0.75f, 0.0f);

			UInt32 widthDiff = SCREEN_WIDTH - GFGL.GetNativeClientWidth(this.Handle);
			UInt32 heightDiff = SCREEN_HEIGHT - GFGL.GetNativeClientHeight(this.Handle);
			this.ClientSize = new System.Drawing.Size(
				(Int32)(SCREEN_WIDTH + widthDiff),
				(Int32)(SCREEN_HEIGHT + heightDiff));

      m_DisplayDeviceHandle = GFGL.CreateDisplay(this.Handle, Format.Enum.A8B8G8R8, Format.Enum.D24S8, MultiSample.Enum.None);

      string path = Application.ExecutablePath;
      // カラーシェーダ用の画面塗りつぶし用リソースの読み込み
      // @todo: リソースは本来バイナリで読み込むが, 2015.01.16の判断で,
      // Windows版は暫定でexeと同階層にリソース一式を配置,
      // 3DS版の実機ビューアはモデル, シェーダ, テクスチャのバイナリを渡すことにする.
      string colorShaderPlaneFilePath;
      string colorShaderPlaneVertexShaderPath;
      string colorShaderPlanePixelShaderPath;
      string colorShaderPlaneTexturePath;
      string edgeCheckPlaneFilePath;
      string edgeCheckPlaneVertexShaderPath;
      string edgeCheckPlanePixelShaderPath;
      string edgeCheckPlaneTexturePath;
      string edgeGfbmdlPath;
      string[] edgeShaderPaths = new string[2];
      string[] edgeTexturePaths = new string[2];
      string gfbmdlPathNormal;
      string[] shaderPathsNormal = new string[2];
      string texturePathNormal;
      string gfbmdlPathId;
      string[] shaderPathsId = new string[4];
      string texturePathId;
      string dofPlaneFilePath;
      string dofPlaneVertexShaderPath;
      string dofFrontPlanePixelShaderPath;
      string dofBackPlanePixelShaderPath;
      string dofPlaneTexturePath;

#if DEBUG
      colorShaderPlaneFilePath = "../../../data/ColorShaderGfmdl/colorshaderplane.wingl.gfbmdl";
      colorShaderPlaneVertexShaderPath = "../../../data/ColorShaderGfmdl/FillScrean.wingl.gfbvsh";
      colorShaderPlanePixelShaderPath = "../../../data/ColorShaderGfmdl/Comb1_Manual.wingl.gfbfsh";
      colorShaderPlaneTexturePath = "../../../data/ColorShaderGfmdl/colorshadertexture.wingl.btex";

      edgeCheckPlaneFilePath = "../../../data/EdgeColorCheck/edgecolorcheckplane.wingl.gfbmdl";
      edgeCheckPlaneVertexShaderPath = "../../../data/EdgeColorCheck/FillScrean.wingl.gfbvsh";
      edgeCheckPlanePixelShaderPath = "../../../data/EdgeColorCheck/Comb1_Manual.wingl.gfbfsh";
      edgeCheckPlaneTexturePath = "../../../data/EdgeColorCheck/edgecolorchecktexture.wingl.btex";

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

      dofPlaneFilePath = "../../../data/Dof/DofShader.wingl.gfbmdl";
      dofPlaneVertexShaderPath = "../../../data/Dof/Default.wingl.gfbvsh";
      dofFrontPlanePixelShaderPath = "../../../data/Dof/CombFront_Manual.wingl.gfbfsh";
      dofBackPlanePixelShaderPath = "../../../data/Dof/CombBack_Manual.wingl.gfbfsh";
      dofPlaneTexturePath = "../../../data/Dof/EdgeMapImage.wingl.btex";
#else
      colorShaderPlaneFilePath = "data/ColorShaderGfmdl/colorshaderplane.wingl.gfbmdl";
      colorShaderPlaneVertexShaderPath = "data/ColorShaderGfmdl/FillScrean.wingl.gfbvsh";
      colorShaderPlanePixelShaderPath = "data/ColorShaderGfmdl/Comb1_Manual.wingl.gfbfsh";
      colorShaderPlaneTexturePath = "data/ColorShaderGfmdl/colorshadertexture.wingl.btex";

      edgeCheckPlaneFilePath = "data/EdgeColorCheck/edgecolorcheckplane.wingl.gfbmdl";
      edgeCheckPlaneVertexShaderPath = "data/EdgeColorCheck/FillScrean.wingl.gfbvsh";
      edgeCheckPlanePixelShaderPath = "data/EdgeColorCheck/Comb1_Manual.wingl.gfbfsh";
      edgeCheckPlaneTexturePath = "data/EdgeColorCheck/edgecolorchecktexture.wingl.btex";

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

      dofPlaneFilePath = "data/Dof/DofShader.wingl.gfbmdl";
      dofPlaneVertexShaderPath = "data/Dof/Default.wingl.gfbvsh";
      dofFrontPlanePixelShaderPath = "data/Dof/CombFront_Manual.wingl.gfbfsh";
      dofBackPlanePixelShaderPath = "data/Dof/CombBack_Manual.wingl.gfbfsh";
      dofPlaneTexturePath = "data/Dof/EdgeMapImage.wingl.btex";
#endif

      // 現在のハンドルからサーフェスを取得
      Surface targetSurface = GFGL.GetBackBuffer(m_DisplayDeviceHandle);

      m_ViewerRenderingPipeLine = new ViewerRenderingPipeLine(
        colorShaderPlaneFilePath,
        colorShaderPlaneVertexShaderPath,
        colorShaderPlanePixelShaderPath,
        colorShaderPlaneTexturePath,
        edgeCheckPlaneFilePath,
        edgeCheckPlaneVertexShaderPath,
        edgeCheckPlanePixelShaderPath,
        edgeCheckPlaneTexturePath,
        edgeGfbmdlPath,
        edgeShaderPaths,
        edgeTexturePaths,
        gfbmdlPathNormal,
        shaderPathsNormal,
        texturePathNormal,
        gfbmdlPathId,
        shaderPathsId,
        texturePathId,
        dofPlaneFilePath,
        dofPlaneVertexShaderPath,
        dofFrontPlanePixelShaderPath,
        dofBackPlanePixelShaderPath,
        dofPlaneTexturePath,
				SCREEN_WIDTH,
				SCREEN_HEIGHT,
        targetSurface);
      DrawUtil.Initialize(1280, 720);

			// 射影行列を描画マネージャに設定
			m_DefaultCamera = new DefaultCamera((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 40.0f);
			m_BattleCamera = new BattleCamera((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 30.0f);
			m_CameraType = ModelViewer.CameraType.DEFAULT;
			m_Cameras[(Int32)CameraType.DEFAULT] = m_DefaultCamera;
			m_Cameras[(Int32)CameraType.STATUS] = new StatusCamera((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 40.0f);
			m_Cameras[(Int32)CameraType.KW] = new KwCamera((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 20.0f);
			m_Cameras[(Int32)CameraType.BATTLE] = m_BattleCamera;

			m_Cameras[(Int32)m_CameraType].Reset(null);

			m_ViewerRenderingPipeLine.SetProjectionMatrix(m_DefaultCamera.Projection);

			m_ViewerRenderingPipeLine.ChangeModelVisibility(true);
			m_ViewerRenderingPipeLine.ChangeWireFrameVisibility(false);
			m_ViewerRenderingPipeLine.ChangeJointVisibility(false);
			m_ViewerRenderingPipeLine.ChangeGridVisibility(true);
			m_ViewerRenderingPipeLine.SetLightPositionVisible(false);
      m_ViewerRenderingPipeLine.ChangeGaugeVisibility(false);
      SetEnabledInformationVisibility(false);

			// キー音を鳴らすための初期化(ウインドウハンドルが必要なのでここで行う)
			string keySoundFilePathFilePath;
#if DEBUG
			keySoundFilePathFilePath = "../../../data/Sound/cursor.wav";
#else
			keySoundFilePathFilePath = "data/Sound/cursor.wav";
			if (!System.IO.File.Exists(keySoundFilePathFilePath))
			{
				keySoundFilePathFilePath = "../../../data/Sound/cursor.wav";
			}
#endif
			gfl2.clr.poke_3d_lib.PokemonModel.InitializeKeySound(keySoundFilePathFilePath);

			// スクリーンサイズを変更(800x480)
			this.ChangeScreenSize(ScreenSizeFormat.Format_800x480);
		}

		/**
		 * @brief 変更通知
		 * @param modelDataManager モデルデータ
		 */
		public void ReceiveNotification(PokemonModelDataManager modelDataManager)
		{
			for (Int32 i = 0; i < (Int32)GfmotPathSet.MotionType.Numberof; ++i)
			{
				var binarySize = 0;
				if (modelDataManager != null)
				{
					binarySize = modelDataManager.CalculateBinarySize((GfmotPathSet.MotionType)i);
				}

				if (m_MotionBinarySizes[i] != binarySize)
				{
					var labelMessage = m_MotionBinarySizeLabels[i].Text;
					var indexDemiliter = labelMessage.IndexOf(':');
					labelMessage = labelMessage.Substring(0, indexDemiliter + 1);
					m_MotionBinarySizeLabels[i].Text = labelMessage + binarySize.ToString("#,0");
					m_MotionBinarySizes[i] = binarySize;
				}
			}

			var allBinarySize = 0;
			if (modelDataManager != null)
			{
				allBinarySize = modelDataManager.BinarySize;
			}

			if (m_AllBinarySize != allBinarySize)
			{
				var labelMessage = allBinarySizeLabel.Text;
				var indexDemiliter = labelMessage.IndexOf(':');
				labelMessage = labelMessage.Substring(0, indexDemiliter + 1);
				allBinarySizeLabel.Text = labelMessage + allBinarySize.ToString("#,0");
				m_AllBinarySize = allBinarySize;
			}

      if (m_ModelDataManager != modelDataManager)
      {
        m_ModelDataManager = modelDataManager;
				m_Cameras[(Int32)m_CameraType].Reset(m_ModelDataManager);
      }

      if (m_ModelDataManager != null)
      {
        var walkSpeedPerSec = m_ModelDataManager.WalkSpeedOriginal * 30.0f / 100.0f; // 秒速(m/sec)に直す

        walkSpeedLabel.Text = string.Format("歩き速度 : {0:0.00}(m/sec)", walkSpeedPerSec);
        walkSpeedLabel2.Text = string.Format("歩き速度(スケール済み) : {0:0.00}(m/sec)", walkSpeedPerSec * m_ModelDataManager.Scale.z);
        polygonCountLabel.Text = string.Format("ポリゴン数 : {0}", m_ModelDataManager.PokemonModel.GetPolygonCount());
        materialCountLabel.Text = string.Format("マテリアル数 : {0}", m_ModelDataManager.PokemonModel.GetMaterialCount());
        jointCountLabel.Text = string.Format("ジョイント数 : {0}", m_ModelDataManager.PokemonModel.GetJointCount());
      }
      else
      {
        walkSpeedLabel.Text = string.Format("歩き速度");
        walkSpeedLabel2.Text = string.Format("歩き速度2");
        polygonCountLabel.Text = string.Format("ポリゴン数");
        materialCountLabel.Text = string.Format("マテリアル数");
        jointCountLabel.Text = string.Format("ジョイント数");
      }
		}

		/**
		 * @brief 描画対象モデル追加
		 * @param PokemonModel 描画対象モデル
		 */
		public void AddDrawableModel(gfl2.clr.poke_3d_lib.PokemonModel model)
		{
			m_ViewerRenderingPipeLine.AddDrawableModel(model);
		}

		/**
		 * @brief 描画対象モデル削除
		 * @param PokemonModel 描画対象モデル
		 */
		public void RemoveDrawableModel(gfl2.clr.poke_3d_lib.PokemonModel model)
		{
			m_ViewerRenderingPipeLine.RemoveDrawableModel(model);
		}

		/**
		 * @brief DragEnterイベントハンドラ
		 * @param object
		 * @param System.Windows.Forms.DragEventArgs
		 */
		private void ViewForm_DragEnter(object sender, System.Windows.Forms.DragEventArgs e)
		{
			// ビューへのドラッグ&ドロップは一旦封印
			/*
			m_dropFilePath = "";
			e.Effect = System.Windows.Forms.DragDropEffects.None;

			if (m_AnimationTimeLine.IsPlayingAnimation())
			{// 再生中なら変更は許可しない
				return;
			}

			// ViewForm内に入ってきたものがファイルなら許可のアイコンに変更
			if (e.Data.GetDataPresent(System.Windows.Forms.DataFormats.FileDrop))
			{
				string[] filePaths = (string[])e.Data.GetData(System.Windows.Forms.DataFormats.FileDrop);
				if (!System.IO.File.Exists(filePaths[0]))
				{// ファイルがなければココで終了
					return;
				}

				string extension = System.IO.Path.GetExtension(filePaths[0]);
				if(extension != ".gfmdl" && extension != ".gfmot")
				{// 拡張子がおかしかったらココで終了
					return;
				}

				m_dropFilePath = filePaths[0];
				e.Effect = System.Windows.Forms.DragDropEffects.All;
			}
			*/
		}

		/**
		 * @brief DragDropイベントハンドラ
		 * @param object
		 * @param System.Windows.Forms.DragEventArgs
		 */
		private void ViewForm_DragDrop(object sender, System.Windows.Forms.DragEventArgs e)
		{
			// ビューへのドラッグ&ドロップは一旦封印
			/*
			string extension = System.IO.Path.GetExtension(m_dropFilePath);
			if (extension == ".gfmot")
			{
				if (m_ApplicationDataManager.GfmdlFilePath == "(None)")
				{
					MessageBox.Show("gfmdlファイルを読み込んでから、gfmotファイルをドラッグ&ドロップしてください");
					return;
				}
				bool isSucceeded = m_ApplicationDataManager.LoadGfMotFromDragDrop(m_dropFilePath);
				if (!isSucceeded)
				{
					MessageBox.Show("gfmotファイルを読み込めませんでした");
				}
			}
			else if (extension == ".gfmdl")
			{
				bool isSucceeded = m_ApplicationDataManager.LoadGfmdl(m_dropFilePath);
				if (!isSucceeded)
				{
					MessageBox.Show("gfmdlファイルを読み込めませんでした");
				}
			}
			else
			{// DragEnterで弾いているのでココには来ないはずだが念のため
				MessageBox.Show("不正なファイルです. ViewForm.cs ViewForm_DragDrop()");
				return;
			}
			*/
		}

		/**
		 * @brief 終了処理(ApplicationCoreに呼び出してもらう)
		 */
		public void Terminate()
		{
			// キー音を鳴らすための終了処理
			gfl2.clr.poke_3d_lib.PokemonModel.TerminateKeySound();
			m_ViewerRenderingPipeLine.Dispose();

			// リソースマネージャ, シーングラフの順番
			ResourceManager.Terminate();
			SceneGraphManager.Terminate();

			DrawUtil.Terminate();
			GFGL.Finish();
			GFGL.SetActiveDisplay(null);
			GFGL.DestroyDisplay(m_DisplayDeviceHandle);
			GFGL.Terminate();
		}

		/**
		 * @brief OnTermに相当
		 * @param object
		 * @param FormClosedEventArgs
		 */
		private void ViewForm_FormClosed(object sender, FormClosedEventArgs e)
		{
		}

		/**
		 * @brief OnUpdateに相当
		 */
		public void OnUpdate()
		{
			// リソースが削除されたフレームでは更新しない
			if (m_IsResourceDeleted)
			{
				return;
			}

			m_Cameras[(Int32)m_CameraType].Update();

			//Matrix projectionMatrix = Matrix.GetIdentity();
			Matrix projectionMatrix = m_Cameras[(Int32)m_CameraType].Projection;
			m_ViewerRenderingPipeLine.SetProjectionMatrix(projectionMatrix);
			//Matrix viewMatrix = Matrix.GetIdentity();
			Matrix viewMatrix = m_Cameras[(Int32)m_CameraType].View;
			m_ViewerRenderingPipeLine.SetViewMatrix(viewMatrix);

      // 背景に渡す行列を計算
      gfl2.clr.math.Matrix backgroundProjection = gfl2.clr.math.Matrix.GetPerspective(
        gfl2.clr.math.Util.ConvDegToRad(30.0f),
        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
        1.1f,
        100000.0f);
      m_ViewerRenderingPipeLine.SetCameraDataToBackgroundPath(
        true,
        backgroundProjection,
        viewMatrix);

			Matrix mtxInvView = viewMatrix.Inverse();
			m_EyePosition.x = 0.0f;
			m_EyePosition.y = 0.0f;
			m_EyePosition.z = 0.0f;
			m_EyePosition.w = 1.0f;
			m_EyePosition = mtxInvView * m_EyePosition;
			m_ViewVector.x = -viewMatrix.GetElem(2, 0);
			m_ViewVector.y = -viewMatrix.GetElem(2, 1);
			m_ViewVector.z = -viewMatrix.GetElem(2, 2);
			m_ViewVector.w = 0.0f;
			m_FocusPosition = m_EyePosition + m_ViewVector * m_Cameras[(Int32)m_CameraType].CameraInterval;

			// デバッグ情報の更新
			m_ViewerRenderingPipeLine.SetFocusPointVisible(m_IsFocusPointVisible);
			m_ViewerRenderingPipeLine.SetFocusPosition(m_FocusPosition);

			if (m_ModelDataManager != null)
			{
				m_ViewerRenderingPipeLine.SetModelPosition(m_ModelDataManager.Position);
				m_ViewerRenderingPipeLine.SetPokemonSize(m_ModelDataManager.Size);
				m_ViewerRenderingPipeLine.SetInterestPositionVisible(m_ModelDataManager.IsInterestPositionControllable);
				m_ViewerRenderingPipeLine.SetInterestPosition(m_ModelDataManager.InterestPosition);
				m_ViewerRenderingPipeLine.SetInnerTubeVisible(m_ModelDataManager.IsInnerTubeVisible);
				m_ViewerRenderingPipeLine.SetInnerTubeColor(m_ModelDataManager.InnerTubeColor);
				m_ViewerRenderingPipeLine.SetOuterTubeVisible(m_ModelDataManager.IsOuterTubeVisible);
				m_ViewerRenderingPipeLine.SetOuterTubeColor(m_ModelDataManager.OuterTubeColor);
				m_ViewerRenderingPipeLine.SetIsModelAABBVisible(m_ModelDataManager.IsModelAABBVisible);

				// スケールに合わせてAABBをリサイズ
				AABB aabb = m_ModelDataManager.PokemonModel.GetModelAABB();
				gfl2.clr.math.Vector min = new gfl2.clr.math.Vector(aabb.GetMin());
				gfl2.clr.math.Vector max = new gfl2.clr.math.Vector(aabb.GetMax());
				min.x *= m_ModelDataManager.Scale.x;
				min.y *= m_ModelDataManager.Scale.y;
				min.z *= m_ModelDataManager.Scale.z;
				max.x *= m_ModelDataManager.Scale.x;
				max.y *= m_ModelDataManager.Scale.y;
				max.z *= m_ModelDataManager.Scale.z;
				// 位置を現在のモデルの位置に合わせる
				min.x += m_ModelDataManager.Position.x;
				min.y += m_ModelDataManager.Position.y;
				min.z += m_ModelDataManager.Position.z;
				max.x += m_ModelDataManager.Position.x;
				max.y += m_ModelDataManager.Position.y;
				max.z += m_ModelDataManager.Position.z;
				m_ViewerRenderingPipeLine.SetModelAABB(min, max);
				m_ViewerRenderingPipeLine.SetModelAABBColor(m_ModelDataManager.ModelAABBColor);
			}
			else
			{
				m_ViewerRenderingPipeLine.SetInterestPositionVisible(false);
				m_ViewerRenderingPipeLine.SetInnerTubeVisible(false);
				m_ViewerRenderingPipeLine.SetOuterTubeVisible(false);
				m_ViewerRenderingPipeLine.SetIsModelAABBVisible(false);
				m_ViewerRenderingPipeLine.SetLightPositionVisible(false);
			}

			performanceTimeLabel.Text = String.Format(
				"CPU負荷 : {0}, GPU負荷 : {1}",
				m_ViewerRenderingPipeLine.GetCpuTimeAverage(),
				m_ViewerRenderingPipeLine.GetGpuTimeAverage());
			eyePositionLabel.Text = String.Format("視点座標 : ({0}, {1}, {2})", m_EyePosition.x, m_EyePosition.y, m_EyePosition.z);
			focusPositionLabel.Text = String.Format("注視点座標 : ({0}, {1}, {2})", m_FocusPosition.x, m_FocusPosition.y, m_FocusPosition.z);
			upDirectionLabel.Text = String.Format("視点 - 注視点間の距離 : {0}", m_Cameras[(Int32)m_CameraType].CameraInterval);

			m_ViewerRenderingPipeLine.Update();
		}

		/**
		 * @brief OnDrawに相当
		 */
		public void OnDraw()
		{
			// リソースが削除されたフレームでは描画しない
			if (m_IsResourceDeleted)
			{
				m_IsResourceDeleted = false;
				return;
			}

			m_ViewerRenderingPipeLine.SetClearColor(m_clearColors[m_clearColorIndex]);
			GFGL.SetActiveDisplay(m_DisplayDeviceHandle);

			Surface backBuffer = GFGL.GetBackBuffer(m_DisplayDeviceHandle);
			Surface depthBuffer = GFGL.GetDepthStencilBuffer(m_DisplayDeviceHandle);

			m_ViewerRenderingPipeLine.SetRenderTarget(backBuffer);
			m_ViewerRenderingPipeLine.SetDepthStencil(depthBuffer);
			m_ViewerRenderingPipeLine.Execute();
		}

		/**
		 * @brief 入力の更新(本来はここにあるべき処理ではないのでいずれ分離)
		 */
		public void UpdateInput()
		{
			if (!this.Focused)
			{
				return;
			}

			if ((m_Keyboard.IsKeyPressed(Key.LeftAlt) || m_Keyboard.IsKeyPressed(Key.RightAlt)) && m_Keyboard.IsKeyPushed(Key.B))
			{
				m_clearColorIndex = (m_clearColorIndex + 1) % (m_clearColors.Length);
			}

			if (m_Keyboard.IsKeyPushed(Key.Space))
			{
				m_Cameras[(Int32)m_CameraType].ResetToDefaultPosition(m_ModelDataManager);
			}

      if (m_Keyboard.IsKeyPressed(Key.RightCtrl) && m_Keyboard.IsKeyPushed(Key.D))
      {
        m_IsEnabledDof = !m_IsEnabledDof;
        m_ViewerRenderingPipeLine.SetEnabledDof(m_IsEnabledDof);
      }

      if (m_Keyboard.IsKeyPressed(Key.RightCtrl) && m_Keyboard.IsKeyPushed(Key.F))
      {
        m_ViewerRenderingPipeLine.ChangeDofType(DofType.Enum.Front);
      }
      else if (m_Keyboard.IsKeyPressed(Key.RightCtrl) && m_Keyboard.IsKeyPushed(Key.B))
      {
        m_ViewerRenderingPipeLine.ChangeDofType(DofType.Enum.Back);
      }
		}

    public void SetFPS(double fps)
    {
      this.fpsLabel.Text = string.Format("FPS : {0:0.00}", fps);
    }

		/**
		 * @brief マウスボタン押下時の処理
		 * @param object
		 * @param System.Windows.Forms.MouseEventArgs
		 */
		private void ViewForm_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
		}

		/**
		 * @brief マウス移動時の処理
		 * @param object
		 * @param System.Windows.Forms.MouseEventArgs
		 */
		private void ViewForm_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (m_CameraType == ModelViewer.CameraType.BATTLE)
			{
				return;
			}

			if (!(m_Keyboard.IsKeyPressed(Key.LeftAlt) || m_Keyboard.IsKeyPressed(Key.RightAlt)))
			{
				return;
			}

			int offsetX;
			int offsetY;

			ICamera camera = m_Cameras[(Int32)m_CameraType];

			// マウスの左ボタンでカメラを回す
			if (m_Mouse.IsButtonPressed(MouseButtons.Left))
			{
				m_Mouse.GetMoveOffset(out offsetX, out offsetY);
        decimal decimalOffsetX;
        decimal decimalOffsetY;
        decimal coef = 1.48m;     // Maya上で垂直方向に90度回したときと同じくらいの回転の仕方をする係数
        decimalOffsetX = (decimal)offsetX * coef;
        decimalOffsetY = (decimal)offsetY * coef;
        decimalOffsetX *= 0.28m;
        decimalOffsetY *= 0.28m;

				// 差分を取得してカメラを回す
        camera.CameraRotationX += 2.0f * (float)gfl2.clr.math.Util.Pai * (float)decimalOffsetX / (float)720;

				// 上下方向は完全にπ/2(90°)だけ向いてしまうと見えなくなってしまうのでギリギリで止める
        camera.CameraRotationY += 2.0f * (float)gfl2.clr.math.Util.Pai * (float)decimalOffsetY / (float)360;
				float verticalLimit = (gfl2.clr.math.Util.Pai - 0.01f) / 2.0f;
			}

			// マウスの中ボタンで平行移動
			if (m_Mouse.IsButtonPressed(MouseButtons.Middle))
			{
				m_Mouse.GetMoveOffset(out offsetX, out offsetY);
        decimal decimalOffsetX;
        decimal decimalOffsetY;
        decimal coef = (decimal)Math.Min(Math.Max(10.0f, camera.CameraInterval), 5000.0f) / 150.0m;
        decimalOffsetX = (decimal)offsetX * coef;
        decimalOffsetY = (decimal)offsetY * coef;
        decimalOffsetX *= 0.11m;
        decimalOffsetY *= 0.11m;

        camera.CameraTranslationX -= (float)decimalOffsetX;
				camera.CameraTranslationY += (float)decimalOffsetY;
			}

			// マウスの右ボタンで拡大/縮小
			if (m_Mouse.IsButtonPressed(MouseButtons.Right))
			{
				m_Mouse.GetMoveOffset(out offsetX, out offsetY);
        decimal decimalOffsetX;
        decimal decimalOffsetY;
        decimal coef = (decimal)Math.Min(Math.Max(150.0f, camera.CameraInterval), 5000.0f) / 150.0m;
        decimalOffsetX = (decimal)offsetX * coef;
        decimalOffsetY = (decimal)offsetY * coef;
        decimalOffsetX *= 0.17m;
        decimalOffsetY *= 0.17m;

				camera.CameraInterval += (float)decimalOffsetX;
				camera.CameraInterval += (float)decimalOffsetY;
			}

            camera.Update();
		}

		/**
		 * @brief マウスボタンリリース時の処理(今のところ未使用)
		 * @param object
		 * @param System.Windows.Forms.MouseEventArgs
		 */
		private void ViewForm_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
		}

		/**
		 * @brief マウスホイール回転時の処理
		 * @param object
		 * @param System.Windows.Forms.MouseEventArgs
		 */
		private void ViewForm_MouseWheel(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (m_CameraType != ModelViewer.CameraType.DEFAULT)
			{
				return;
			}
			m_DefaultCamera.CameraInterval += (float)(-e.Delta * SystemInformation.MouseWheelScrollDelta) / 200.0f;
		}

		/**
		 * @brief モデル表示のON/OFF切替
		 */
		public void ChangeModelVisibility(bool enabled)
		{
			m_ViewerRenderingPipeLine.ChangeModelVisibility(enabled);
		}

		/**
		 * @brief ワイヤーフレーム表示のON/OFF切替
		 */
		public void ChangeWireFrameVisibility(bool enabled)
		{
			m_ViewerRenderingPipeLine.ChangeWireFrameVisibility(enabled);
		}

		/**
		 * @brief ジョイント表示のON/OFF切替
		 */
		public void ChangeJointVisibility(bool enabled)
		{
			m_ViewerRenderingPipeLine.ChangeJointVisibility(enabled);
		}

		/**
		 * @brief グリッド表示のON/OFF切替
		 */
		public void ChangeGridVisibility(bool enabled)
		{
			m_ViewerRenderingPipeLine.ChangeGridVisibility(enabled);
		}

		/**
		 * @brief ゲージ表示のON/OFF切替
		 */
		public void ChangeGaugeVisibility(bool enabled)
		{
			m_ViewerRenderingPipeLine.ChangeGaugeVisibility(enabled);
		}

		/**
		 * @brief 注視範囲のON/OFF切替
		 */
		public void ChangeInterestRangeVisibility(bool enabled)
		{
			m_ViewerRenderingPipeLine.ChangeInterestRangeVisibility(enabled);
		}

		/**
		 * @brief アウトラインのON/OFF切替
		 */
		public void ChangeOutLineVisibility(bool enabled)
		{
			m_ViewerRenderingPipeLine.ChangeOutLineVisibility(enabled);
		}

		/**
		 * @brief ライト位置表示のON/OFF切替
		 */
		public void ChangeLightPositionVisibility(bool enabled)
		{
			m_ViewerRenderingPipeLine.SetLightPositionVisible(enabled);
		}

		/**
		 * @brief 背景表示のON/OFF切替
		 */
		public void ChangeBackgroundVisibility(bool enabled)
		{
			m_ViewerRenderingPipeLine.ChangeBackgroundVisibility(enabled);
		}

    /**
     * @brief 全画面で表示するテクスチャ切り替え
     */
    public void ChangeFillScreenTexture(Int32 index)
    {
      m_ViewerRenderingPipeLine.ChangeFillTexture(index);
    }

		/**
		 * @brief ウインドウプロシージャ上書き
		 * @param ref Message メッセージ
		 */
		protected override void WndProc(ref Message m)
		{
			const Int32 WM_NCLBUTTONDBLCLK = 0x00A3;

			if (m.Msg == WM_NCLBUTTONDBLCLK)
			{// タイトルバーのダブルクリックをシカト
				return;
			}

			base.WndProc(ref m);
		}

    public void PrintScreen(string fileName)
    {
      PCDebugUtil.PrintScreen(fileName, m_DisplayDeviceHandle);
    }

    public void PrintScreenAsTga(string fileName)
    {
      PCDebugUtil.PrintScreenAsTga(fileName, m_DisplayDeviceHandle);
    }
	}
}
