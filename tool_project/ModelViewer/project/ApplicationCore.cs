using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using gfl2.clr.gfx;
using gfl2.clr.math;
using gfl2.clr.animation;
using gfl2.clr.renderingengine;
using gfl2.clr.renderingengine.renderer;
using gfl2.clr.renderingengine.scenegraph;
using gfl2.clr.renderingengine.scenegraph.instance;
using gfl2.clr.renderingengine.scenegraph.resource;
using gfl2.clr.poke_3d_lib;

namespace ModelViewer
{
	public partial class ApplicationCore : Form
	{
		private RenderForm m_RenderForm;																							//!< @brief 描画画面のフォーム
		private AnimationDataSetForm m_AnimationDataSetForm;													//!< @brief データ設定画面のフォーム
		private ModelMotionResourceTreeViewForm m_ModelMotionResourceTreeViewForm;		//!< @brief モデル&モーションの管理ツリーのフォーム
		private AnimationTimeLineControlForm m_AnimationTimeLineControlForm;					//!< @brief タイムライン操作フォーム
		private TransformControlForm m_TransformControlForm;													//!< @brief 配置情報を操作するフォーム
		private LightControlForm m_LightControlForm;																	//!< @brief ライトを操作する
		private CameraControlForm m_CameraControlForm;																//!< @brief カメラを操作する
		private McnfForm m_McnfForm;																                  //!< @brief mcnfの設定をする
		private BlendListForm m_BlendListForm;																        //!< @brief blend_listの設定をする
		private MotionCompForm m_MotionCompForm;																      //!< @brief モーションの圧縮レベル設定をする
		private EffectLocatorControlForm m_EffectLocatorControlForm;									//!< @brief エフェクトロケータの表示切替
		private FaceControlForm m_FaceControlForm;																		//!< @brief 表情を操作する
		private AdjustmentFigureControlForm m_AdjustmentFigureControlForm;						//!< @brief 図形調整
		private ColorShaderControlForm m_ColorShaderControlForm;											//!< @brief カラーシェーダ操作
		private ModelMotionResourceLoader m_ModelMotionResourceLoader;								//!< @brief モデル/モーションのリソース読み込み窓口
		private KeySoundControlForm m_KeySoundControlForm;                            //!< @brief キーサウンド再生切替
    private PokedecoLocatorControlForm m_PokedecoLocatorControlForm;              //!< @brief ポケデコロケータの表示切替
    private DisplayNodeHierarchyForm m_DisplayNodeHierarchyForm;                  //!< @brief 現在のモデルのノード階層を表示
    private CheckMaterialAttributeForm m_CheckMaterialAttributeForm;              //!< @brief マテリアルの情報表示
    private MarkingControlForm m_MarkingControlForm;                              //!< @brief 模様の変更
		private const Int32 ANIMATION_NUM_ENABLED_TO_REGISTER = 100;					//!< @brief PokemonModelDaataManagerに登録できるモデルの総数
		public const Int32 ANIMATION_NUM_ENABLED_TO_SET = 10;									//!< @brief アニメーションの総数
		private List<PokemonModelDataManager> m_PokemonModelDataManagerList;	//!< @brief ポケモンモデルのデータを一括で管理するマネージャ @todo: TreeViewに持っていくべき
		private Int32 m_SelectedModelDatamanagerIndex;						//!< @brief 現在選択中のポケモンモデルデータのインデックス
		public Int32 SelectedIndexOfPokemonModelDataManagerList
		{
			set {
				m_SelectedModelDatamanagerIndex = value;
				m_CameraControlForm.SelectedIndexOfPokemonModelDataManagerList = value;
        m_PokemonModelDataManagerList.ForEach(data => data.SetFocusedToEffectLocators(false));
        m_PokemonModelDataManagerList.ForEach(data => data.SetFocusedToPokedecoLocators(false));
				if (value >= 0)
				{
          m_PokemonModelDataManagerList[value].SetFocusedToEffectLocators(true);
          m_PokemonModelDataManagerList[value].SetFocusedToPokedecoLocators(true);
				}
			}
		}
		public bool IsEmptyPokemonModelDataManagerList()
		{
			return (m_PokemonModelDataManagerList.Count == 0);
		}

		private ResourceNode m_pEnvResourceRootNode;								//!< @brief 描画環境リソース

		//!< @brief 背景データのリソースの種類
		private enum BackgroundResourceType
		{
      sky,
      btl_F_kusa,
      btl_N_kusa,
      btl_G_kusa,
			NumberOf
		}

    //!< @brief 背景データのパス
    private static readonly string[] BACKGROUND_PATH = new string[]
    {
      "data/Background/test_skybox/sky_sunny/converted_data/sky_sunny.wingl.gfbmdl",
      "data/Background/kusa/btl_F/converted_data/btl_F_kusa.wingl.gfbmdl",
      "data/Background/kusa/btl_N/converted_data/btl_N_kusa.wingl.gfbmdl",
      "data/Background/kusa/btl_G/converted_data/btl_G_kusa.wingl.gfbmdl"
    };

		//!< @brief 背景用のデータリソース
		gfl2.clr.poke_3d_lib.PokemonModel[] m_BackgroundModel;
		private gfl2.clr.renderingengine.scenegraph.resource.ResourceNode[] m_BackgroundModelResourceNode;

		// 入力系
		private Keyboard m_Keyboard;
		private Mouse m_Mouse;

		/**
		 * @brief コンストラクタ
		 */
		public ApplicationCore()
		{
			InitializeComponent();
			m_SelectedModelDatamanagerIndex = -1;	// 初期値は-1

      gfl2.clr.debug.Assert.SetUpAssertLoopFunc(this.Handle);

			m_Keyboard = new Keyboard();
			m_Mouse = new Mouse();

      // TreeNodeで必要になるカラーテーブルを作成
      PokemonModel.CreateTreeNodeColorDictionary();

      gfl2.clr.debug.Assert.m_ApplicationCore = this;
		}

		/**
		 * @brief 初期化
		 * @param object
		 * @param EventArgs
		 */
		private void ModelViewerForm_Load(object sender, EventArgs e)
		{
			// AssembryInfo.cs で、↓のように設定してあることが前提！
			// [assembly: AssemblyVersion("1.0.*")]

			var asm = System.Reflection.Assembly.GetExecutingAssembly();
			var version = asm.GetName().Version;

			int days = version.Build;
			int seconds = version.Revision * 2;

			DateTime baseDate = new DateTime(2000, 1, 1);
			DateTime buildDate = baseDate.AddDays(days);
			DateTime buildDateTime = buildDate.AddSeconds(seconds);

			StringBuilder sb = new StringBuilder();
			sb.Append("Version：");
			sb.AppendLine(buildDateTime.ToString("yyyy'/'MM'/'dd' 'HH':'mm':'ss"));
			this.Text = "GfModelViewer " + sb.ToString();

			GFGL.Initialize();
			gfl2.clr.renderingengine.scenegraph.InitDescription desc = new gfl2.clr.renderingengine.scenegraph.InitDescription(16384);
			SceneGraphManager.Initialize(desc);
			ResourceManager.Initialize();

			m_PokemonModelDataManagerList = new List<PokemonModelDataManager>();

			// ウインドウは描画フォーム以外全部Dockingにする
			m_RenderForm = new RenderForm(m_Keyboard, m_Mouse);
			m_RenderForm.MdiParent = this;

      m_TransformControlForm = new TransformControlForm(m_PokemonModelDataManagerList);
			m_TransformControlForm.MdiParent = this;

			m_LightControlForm = new LightControlForm();
			m_LightControlForm.MdiParent = this;

			m_FaceControlForm = new FaceControlForm();
			m_FaceControlForm.MdiParent = this;

			m_AdjustmentFigureControlForm = new AdjustmentFigureControlForm();
			m_AdjustmentFigureControlForm.MdiParent = this;

			m_KeySoundControlForm = new KeySoundControlForm();
			m_KeySoundControlForm.MdiParent = this;

			// CameraControlFormは, RenderFormの表示が終わらないとカメラが無くて初期化できない
			m_CameraControlForm = new CameraControlForm(m_RenderForm, m_TransformControlForm, m_PokemonModelDataManagerList);
			m_CameraControlForm.MdiParent = this;

			m_ModelMotionResourceTreeViewForm = new ModelMotionResourceTreeViewForm(this, m_CameraControlForm);
			m_ModelMotionResourceTreeViewForm.MdiParent = this;

			m_AnimationDataSetForm = new AnimationDataSetForm(m_ModelMotionResourceLoader, ANIMATION_NUM_ENABLED_TO_SET);
			m_AnimationDataSetForm.MdiParent = this;

      m_AnimationTimeLineControlForm = new AnimationTimeLineControlForm(m_AnimationDataSetForm, "setting.cfg");
      m_AnimationTimeLineControlForm.MdiParent = this;

			// ツリービューがリソース読み込みの起点となるのでここでローダを生成
			m_ModelMotionResourceLoader = new ModelMotionResourceLoader(m_ModelMotionResourceTreeViewForm);

			m_McnfForm = new McnfForm();
			m_McnfForm.MdiParent = this;

			m_BlendListForm = new BlendListForm(m_AnimationDataSetForm);
			m_BlendListForm.MdiParent = this;

			m_MotionCompForm = new MotionCompForm();
			m_MotionCompForm.MdiParent = this;

			m_EffectLocatorControlForm = new EffectLocatorControlForm();
			m_EffectLocatorControlForm.MdiParent = this;

      m_PokedecoLocatorControlForm = new PokedecoLocatorControlForm();
      m_PokedecoLocatorControlForm.MdiParent = this;

      m_DisplayNodeHierarchyForm = new DisplayNodeHierarchyForm();
      m_DisplayNodeHierarchyForm.MdiParent = this;

      m_CheckMaterialAttributeForm = new CheckMaterialAttributeForm();
      m_CheckMaterialAttributeForm.MdiParent = this;

      m_MarkingControlForm = new MarkingControlForm();
      m_MarkingControlForm.MdiParent = this;

			m_AnimationDataSetForm.Location = new Point(m_RenderForm.Location.X, m_RenderForm.Height * 2);
			m_AnimationTimeLineControlForm.Location = new Point(m_RenderForm.Width * 2, 0);
			m_AdjustmentFigureControlForm.Location = new Point(m_AnimationTimeLineControlForm.Location.X + m_AnimationTimeLineControlForm.Width, 0);
			m_FaceControlForm.Location = new Point(m_AdjustmentFigureControlForm.Location.X, m_AdjustmentFigureControlForm.Height);
			m_ModelMotionResourceTreeViewForm.Location = new Point(m_AnimationTimeLineControlForm.Location.X, m_AnimationTimeLineControlForm.Height);
			m_TransformControlForm.Location = new Point(m_AnimationTimeLineControlForm.Location.X, m_ModelMotionResourceTreeViewForm.Location.Y + m_ModelMotionResourceTreeViewForm.Height);
			m_LightControlForm.Location = new Point(m_ModelMotionResourceTreeViewForm.Location.X + m_ModelMotionResourceTreeViewForm.Width, m_AnimationTimeLineControlForm.Height);
			m_CameraControlForm.Location = new Point(m_LightControlForm.Location.X, m_LightControlForm.Location.Y + m_LightControlForm.Height);
			m_McnfForm.Location = new Point(m_LightControlForm.Location.X, m_CameraControlForm.Location.Y + m_CameraControlForm.Height);
			m_BlendListForm.Location = new Point(m_McnfForm.Location.X + 32, m_McnfForm.Location.Y + 32);
			m_MotionCompForm.Location = new Point(m_BlendListForm.Location.X + 32, m_BlendListForm.Location.Y + 32);
			m_EffectLocatorControlForm.Location = new Point(m_LightControlForm.Location.X, m_McnfForm.Location.Y + m_McnfForm.Height);
      m_PokedecoLocatorControlForm.Location = new Point(m_LightControlForm.Location.X, m_McnfForm.Location.Y + m_McnfForm.Height);
      m_CheckMaterialAttributeForm.Location = new Point(m_PokedecoLocatorControlForm.Location.X, m_PokedecoLocatorControlForm.Location.Y);
      m_KeySoundControlForm.Location = new Point(m_EffectLocatorControlForm.Location.X + m_EffectLocatorControlForm.Width, m_EffectLocatorControlForm.Location.Y);
      m_DisplayNodeHierarchyForm.Location = new Point(m_EffectLocatorControlForm.Location.X + m_EffectLocatorControlForm.Width, m_EffectLocatorControlForm.Location.Y);
      m_MarkingControlForm.Location = new Point(0, 0);

			m_RenderForm.Show();
			m_ModelMotionResourceTreeViewForm.Show();
			m_AnimationTimeLineControlForm.Show();
			m_TransformControlForm.Show();
			m_LightControlForm.Show();
			m_FaceControlForm.Show();
			m_AdjustmentFigureControlForm.Show();
			m_KeySoundControlForm.Show();
			m_CameraControlForm.Show();
			m_AnimationDataSetForm.Show();
			m_McnfForm.Show();
			m_BlendListForm.Show();
			m_MotionCompForm.Show();
      //m_PokedecoLocatorControlForm.Show();    // nijiでpokedecoの機能実装がなくなったので非表示に設定
      m_CheckMaterialAttributeForm.Show();
      m_EffectLocatorControlForm.Show();
      m_DisplayNodeHierarchyForm.Show();

			// ColorShaderControlFormは, RenderFormの表示が終わらないとパイプラインが無くて初期化できない
			m_ColorShaderControlForm = new ColorShaderControlForm(m_ModelMotionResourceTreeViewForm, m_RenderForm.ViewerRenderingPipeLine);
			m_ColorShaderControlForm.MdiParent = this;
			m_ColorShaderControlForm.Location = new Point(m_KeySoundControlForm.Location.X, m_KeySoundControlForm.Location.Y);
			m_ColorShaderControlForm.Show();

			m_BackgroundModel = new PokemonModel[(Int32)BackgroundResourceType.NumberOf];
			m_BackgroundModelResourceNode = new ResourceNode[(Int32)BackgroundResourceType.NumberOf];
			// 背景モデルの作成
			for (Int32 i = 0; i < (Int32)BackgroundResourceType.NumberOf; ++i)
			{
				string gfmdlBackgroundPath = BACKGROUND_PATH[i];
	#if DEBUG
				gfmdlBackgroundPath = "../../../" + gfmdlBackgroundPath;
	#endif
				GfBinaryMdlData bgModelResourceData = new GfBinaryMdlData(File.ReadAllBytes(gfmdlBackgroundPath));
				m_BackgroundModelResourceNode[i] = ResourceManager.CreateResourceNode(bgModelResourceData);
				bgModelResourceData.Dispose();
				bgModelResourceData = null;

        // 背景データのシェーダとテクスチャを設定
        string bgModelResourceDirectory = System.IO.Path.GetDirectoryName(gfmdlBackgroundPath);
        string[] resourceFilePaths = System.IO.Directory.GetFiles(bgModelResourceDirectory, "*", SearchOption.AllDirectories);
        m_BackgroundModelResourceNode[i].AddShaderResources(resourceFilePaths);
        m_BackgroundModelResourceNode[i].AddTextureResources(resourceFilePaths);

				m_BackgroundModel[i] = new PokemonModel();
				// cmHeight, adjustHeightのデフォルト値を100.0fとしておく
        m_BackgroundModel[i].Create(m_BackgroundModelResourceNode[i], null, null);
				m_BackgroundModel[i].SetVisible(true);
				m_BackgroundModel[i].SetLightSetNo(0);
				m_BackgroundModel[i].SetLightDirection(new gfl2.clr.math.Vector(0.0f, -1.0f, 0.0f));
        m_BackgroundModel[i].SetHeight(100.0f, 100.0f);
        m_RenderForm.AddBackgroundModel(m_BackgroundModel[i]);
			}
		}

		/**
		 * @brief 終了
		 * @param object
		 * @param EventArgs
		 */
		private void ApplicationCore_FormClosed(object sender, FormClosedEventArgs e)
		{
			Terminate();
		}

		/**
		 * @brief info.datの出力
		 */
		private void OutputInfoDat()
		{
			//自分自身の実行ファイルのパスを取得する
			string outputTmpPath = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location).Replace("\\", "/") + "/tmp/";
#if DEBUG
			outputTmpPath = Environment.GetEnvironmentVariable("NIJI_TOOL_DIR").Replace("\\", "/") + "NijiModelViewer/tmp/";
#endif
			if (!System.IO.Directory.Exists(outputTmpPath))
			{
				MessageBox.Show("出力先のtmpディレクトリが存在しません。 Path = " + outputTmpPath);
				return;
			}
			string outputInfoDatPath = outputTmpPath + "info_dat/";
			if (!System.IO.Directory.Exists(outputInfoDatPath))
			{
				System.IO.Directory.CreateDirectory(outputInfoDatPath);
			}

			// gfmdlのファイル名から各種モデルの名前のディレクトリを作成
			PokemonModelDataManager modelDataManager = m_PokemonModelDataManagerList[m_SelectedModelDatamanagerIndex];

			bool isSucceeded = 
				modelDataManager.OutputInfoDat(
					outputInfoDatPath,
					m_RenderForm.GetCurrentViewMatrix(),
					m_RenderForm.GetCurrentFieldOfView(),
					m_RenderForm.GetCurrentNearClip(),
					m_RenderForm.GetCurrentFarClip(),
          backgroundToolStripMenuItem.Checked,
          m_MarkingControlForm.GetMarkFrame1(),
          m_MarkingControlForm.GetMarkFrame2(),
          m_MarkingControlForm.GetMarkFrame3(),
          m_MarkingControlForm.GetMarkFrame4()
				);

			if (!isSucceeded)
			{
				MessageBox.Show("info.datの出力に失敗しました.");
			}
		}

		/** 
		 * @brief 各バイナリファイルサイズ出力
		 */
		private void OutputBinarySize()
		{
			m_PokemonModelDataManagerList[m_SelectedModelDatamanagerIndex].OutputBinarySize();
		}

		public void MainLoop()
		{
			// 入力デバイスの更新
			m_Keyboard.Update();
			m_Mouse.Update();

			if (m_Keyboard.IsKeyPushed(System.Windows.Input.Key.F8))
			{
				//if (this.Focused)
				{// F5が押されたらinfo.datを出力
					if (m_PokemonModelDataManagerList.Count > 0)
					{
						OutputInfoDat();
						// 暫定で追加した各バイナリファイルの大きさを取得する処理
						OutputBinarySize();
					}
				}
			}

      if (Form.ActiveForm == this)
      {
        if (m_Keyboard.IsKeyPushed(System.Windows.Input.Key.F6))
        {
          var localDate = DateTime.Now;
          string screenShotName = "screenshot_" + localDate.ToString("yyyy_MM_dd_HH_mm_ss");
          m_RenderForm.PrintScreenAsTga(screenShotName + ".tga");
        }

        if (m_Keyboard.IsKeyPressed(System.Windows.Input.Key.RightAlt) && m_Keyboard.IsKeyPushed(System.Windows.Input.Key.P))
        {
          m_MarkingControlForm.Show();
        }      

        // Gキーを押下したらグリッドON/OFFのショートカット
        if (m_Keyboard.IsKeyPushed(System.Windows.Input.Key.G))
        {
          gridVisibilityToolStripMenuItem.Checked = !gridVisibilityToolStripMenuItem.Checked;
          gridVisibilityToolStripMenuItem_Click(null, null);
        }
      }

			// PokemonModelDataManagerのタイムラインの更新
			if (m_PokemonModelDataManagerList != null)
			{
				m_PokemonModelDataManagerList.ForEach(data => data.Update());

				if (m_SelectedModelDatamanagerIndex >= 0)
				{
					m_PokemonModelDataManagerList[m_SelectedModelDatamanagerIndex].NotifyToListeners();
				}
			}
			
			// モーション再生が必要なオブジェクトの更新が終わってからトラバース
			SceneGraphManager.TraverseModel(m_RenderForm.GetCurrentViewMatrix());
            

			if (m_PokemonModelDataManagerList != null)
			{
				// PokemonModelDataManagerのタイムラインの更新
				m_PokemonModelDataManagerList.ForEach(data => data.UpdateJointController());
			}

			m_RenderForm.UpdateInput();
			m_RenderForm.OnUpdate();
			m_RenderForm.OnDraw();
		}

    public void LoadModel(string exportsDirectoryPath)
    {
      m_ModelMotionResourceTreeViewForm.EmulateDragDrop(exportsDirectoryPath);
    }

    public void SimpleExec(string exportsDirectoryPath, string motion, int frame)
    {
      // グリッドを非表示に
      m_RenderForm.ChangeGridVisibility(false);

      m_AnimationDataSetForm.SetMotion(motion, frame);

      // PokemonModelDataManagerのタイムラインの更新
      if (m_PokemonModelDataManagerList != null)
      {
        m_PokemonModelDataManagerList.ForEach(data => data.Update());

        if (m_SelectedModelDatamanagerIndex >= 0)
        {
          m_PokemonModelDataManagerList[m_SelectedModelDatamanagerIndex].NotifyToListeners();
        }
      }

      SceneGraphManager.TraverseModel(m_RenderForm.GetCurrentViewMatrix());

      m_RenderForm.OnUpdate();
      m_RenderForm.OnDraw();

      var outputImageDirectory = exportsDirectoryPath.Replace("exports", "screenshots");

      if (!System.IO.Directory.Exists(outputImageDirectory))
      {
        System.IO.Directory.CreateDirectory(outputImageDirectory);
      }

      var filename = motion + "_frame" + frame.ToString() + "_DEFAULT.bmp";

      var outputImageFile = System.IO.Path.Combine(outputImageDirectory, filename);

      m_RenderForm.PrintScreen(outputImageFile);
    }

		private void Terminate()
		{
			for (Int32 i = 0; i < (Int32)BackgroundResourceType.NumberOf; ++i)
			{
				if (m_BackgroundModelResourceNode[i] != null)
				{
					m_RenderForm.RemoveBackgroundModel(m_BackgroundModel[i]);
					m_BackgroundModelResourceNode[i].Dispose();
					m_BackgroundModelResourceNode[i] = null;
				}

				if (m_BackgroundModel[i] != null)
				{
					m_BackgroundModel[i].Destroy();
					m_BackgroundModel[i].Dispose();
					m_BackgroundModel[i] = null;
				}
			}

			// PokemonModelを有しているPokemonModelDataManagerを最初に解放
			if (m_PokemonModelDataManagerList != null)
			{
				m_PokemonModelDataManagerList.ForEach(data => data.Dispose());
				m_PokemonModelDataManagerList = null;
			}

			if (m_pEnvResourceRootNode != null)
			{
				m_pEnvResourceRootNode.Dispose();
				m_pEnvResourceRootNode = null;
			}

			m_RenderForm.Terminate();
		}

		/**
		 * @brief キー入力(Escで終了する処理のみ)
		 * @param object
		 * @param KeyEventArgs
		 */
		private void ModelViewerForm_KeyDown(object sender, KeyEventArgs e)
		{
			switch (e.KeyData)
			{
				case Keys.Escape:
					this.Close();
					break;
			}
		}

		/**
		 * @brief リソースの破棄
		 */
		private void DisposeAllResources()
		{
			if (m_PokemonModelDataManagerList != null)
			{
				m_PokemonModelDataManagerList.ForEach(data => data.Dispose());
			}
			SelectedIndexOfPokemonModelDataManagerList = -1;
		}

		/**
		 * @brief 要素の削除
		 * @param index 削除対象のインデックス
		 * @return bool 成功/失敗
		 */
		public bool RemoveFromPokemonModelDataManagerList(Int32 index)
		{
			if (m_PokemonModelDataManagerList[index].AnimationTimeLine.IsPlayingAnimation())
			{// 再生中なら停止させる(削除の最中にアニメーションが再生されないようにする)
				m_PokemonModelDataManagerList[index].AnimationTimeLine.Stop();
			}

      PokemonModel shadowModel = m_PokemonModelDataManagerList[index].ShadowModel;
      if (shadowModel != null)
      {
        m_RenderForm.RemoveDrawableModel(shadowModel);
      }

      PokemonModel idModel = m_PokemonModelDataManagerList[index].IdModel;
      if (idModel != null)
      {
        m_RenderForm.RemoveDrawableModel(idModel);
      }

			PokemonModel pokemonModel = m_PokemonModelDataManagerList[index].PokemonModel;
			// 該当のModelRenderPathを削除
			m_RenderForm.RemoveDrawableModel(pokemonModel);

			Int32 colorShaderTextureNum = pokemonModel.GetColorTextureNum();
			m_RenderForm.ViewerRenderingPipeLine.RemoveColorTexture(pokemonModel);

			// エッジ描画から該当のモデルデータを外す
			m_RenderForm.ViewerRenderingPipeLine.RemoveEdgeRenderTarget(pokemonModel);

			// 該当のモデルをモデルの描画パスから外す

			m_PokemonModelDataManagerList[index].Dispose();
			m_PokemonModelDataManagerList.RemoveAt(index);

			if (index >= m_PokemonModelDataManagerList.Count)
			{
				m_SelectedModelDatamanagerIndex = m_PokemonModelDataManagerList.Count - 1;
			}

			if (m_PokemonModelDataManagerList.Count == 0)
			{
				m_RenderForm.ReceiveNotification(null);
				m_AnimationTimeLineControlForm.ReceiveNotification(null);
				m_AnimationDataSetForm.ReceiveNotification(null);
				m_TransformControlForm.ReceiveNotification(null);
				m_LightControlForm.ReceiveNotification(null);
				m_McnfForm.ReceiveNotification(null);
				m_BlendListForm.ReceiveNotification(null);
        m_MotionCompForm.ReceiveNotification(null);
				m_EffectLocatorControlForm.ReceiveNotification(null);
				m_FaceControlForm.ReceiveNotification(null);
				m_AdjustmentFigureControlForm.ReceiveNotification(null);
				m_ColorShaderControlForm.ReceiveNotification(null);
				m_EffectLocatorControlForm.ReceiveNotification(null);
				m_KeySoundControlForm.ReceiveNotification(null);
        m_PokedecoLocatorControlForm.ReceiveNotification(null);
        m_DisplayNodeHierarchyForm.ReceiveNotification(null);
        m_CheckMaterialAttributeForm.ReceiveNotification(null);
        m_MarkingControlForm.ReceiveNotification(null);
				SelectedIndexOfPokemonModelDataManagerList = -1;
			}

			// インデックスの設定が終了したら, カラーシェーダのパスのインデックスを修正する
			for (Int32 i = index; i < m_PokemonModelDataManagerList.Count; ++i)
			{
				PokemonModel model = m_PokemonModelDataManagerList[i].PokemonModel;
				model.PackColorShaderTextureIndex(colorShaderTextureNum);
			}

			// 削除したことをRenderFormに通知. 削除したフレームでは描画をいったん止める.
			m_RenderForm.IsResourceDeleted = true;

			return true;
		}

		/**
		 * @brief PokemonModelDataManagerのセットアップ
		 * @param gfmdlFilePath gfmdl形式のファイルパス
		 * @param gfmotFilePaths gfmot形式のファイルパス群
		 * @param frameResolution フレームの分解能
		 * @param loopEnable ループON/OFF
		 * @return PokemonModelDataManager セットアップ完了したDataManager
		 */
		private PokemonModelDataManager SetUpPokemonModelDataManager(
      string importDirectory,
      GfMdlResourcePathSetDictionary gfmdlResourcePathSetDirectory,
      GfMdlResourcePathSetDictionary shadowGfmdlResourcePathSetDirectory,
			GfmotPathSet gfmotPathSet,
      GfblocPathSet gfblocPathSet,
      ExtraFilePathSet extraFilePathSet,
			decimal frameResolution,
			bool loopEnable,
			bool isRare)
		{
      if (!System.IO.File.Exists(gfmdlResourcePathSetDirectory[PokemonModelDataManager.ConvertExtensionType.WinOpenGL].gfmdlFilePath))
			{
				return null;
			}

			PokemonModelDataManager pokemonModelDataManager = new PokemonModelDataManager(
        importDirectory,
        gfmdlResourcePathSetDirectory,
        shadowGfmdlResourcePathSetDirectory,
        gfmotPathSet,
        gfblocPathSet,
        extraFilePathSet,
				ANIMATION_NUM_ENABLED_TO_SET,
				ANIMATION_NUM_ENABLED_TO_REGISTER,
				frameResolution,
				loopEnable,
				isRare);

			// カラーシェーダ用テクスチャの設定
			m_RenderForm.ViewerRenderingPipeLine.AddColorTexture(
				pokemonModelDataManager.PokemonModel);

			// 描画パイプラインにエッジを描画するモデルのノードを追加
			m_RenderForm.ViewerRenderingPipeLine.AddEdgeRenderTarget(
				pokemonModelDataManager.PokemonModel);

			// UIにモデルデータを追加
			AddModelManagerToUserInterfaces(pokemonModelDataManager);

			return pokemonModelDataManager;
		}

		/**
		 * @brief gfmdl, gfmot形式のリソースをロード
		 * @param gfmdlFilePath gfmdl形式のファイルパス
		 * @param gfmotFilePaths gfmot形式のファイルパス群
		 * @param frameResolution フレームの分解能
		 * @param loopEnable ループON/OFF
		 * @return bool 成功/失敗
		 */
		public PokemonModelDataManager LoadGfResources(
      string importDirectory,
      GfMdlResourcePathSetDictionary gfmdlResourcePathSetDirectory,
      GfMdlResourcePathSetDictionary shadowGfmdlResourcePathSetDirectory,
			GfmotPathSet gfmotPathSet,
      GfblocPathSet gfblocPathSet,
      ExtraFilePathSet extraFilePathSet,
			decimal frameResolution,
			bool loopEnable,
			bool isRare)
		{
			
			PokemonModelDataManager pokemonModelDataManager = SetUpPokemonModelDataManager(
        importDirectory,
        gfmdlResourcePathSetDirectory,
        shadowGfmdlResourcePathSetDirectory,
        gfmotPathSet,
        gfblocPathSet,
        extraFilePathSet,
				frameResolution,
				loopEnable,
				isRare);

			// 最後にリストに追加
			pokemonModelDataManager.Position.x =
				m_PokemonModelDataManagerList.Count * PokemonModelDataManager.PositionInterval;
			m_PokemonModelDataManagerList.Add(pokemonModelDataManager);

      // 描画順を考慮し, 影モデル, 本体モデル, IDモデルの順に設定
      if (pokemonModelDataManager.ShadowModel != null)
      {
        m_RenderForm.AddDrawableModel(pokemonModelDataManager.ShadowModel);
      }
      m_RenderForm.AddDrawableModel(pokemonModelDataManager.PokemonModel);
      if (pokemonModelDataManager.IdModel != null)
      {
        m_RenderForm.AddDrawableModel(pokemonModelDataManager.IdModel);
      }

			return pokemonModelDataManager;
		}

		/**
		 * @brief gfmdl, gfmot形式のリソースをロード
		 * @param gfmdlFilePath gfmdl形式のファイルパス
		 * @param gfmotFilePaths gfmot形式のファイルパス群
		 * @param frameResolution フレームの分解能
		 * @param loopEnable ループON/OFF
		 * @return bool 成功/失敗
		 */
		public PokemonModelDataManager ReloadGfResources(
      string importDirectory,
      GfMdlResourcePathSetDictionary gfmdlResourcePathSetDirectory,
      GfMdlResourcePathSetDictionary shadowGfmdlResourcePathSetDirectory,
      GfmotPathSet gfmotPathSet,
      GfblocPathSet gfblocPathSet,
      ExtraFilePathSet extraFilePathSet,
      decimal frameResolution,
			bool loopEnable,
			bool isRare,
			Int32 indexOfList)
		{
			PokemonModelDataManager pokemonModelDataManager = SetUpPokemonModelDataManager(
        importDirectory,
        gfmdlResourcePathSetDirectory,
        shadowGfmdlResourcePathSetDirectory,
        gfmotPathSet,
        gfblocPathSet,
        extraFilePathSet,
				frameResolution,
				loopEnable,
				isRare);

			m_PokemonModelDataManagerList.Insert(indexOfList, pokemonModelDataManager);

      // 描画順を考慮し, 影モデル, 本体モデル, IDモデルの順に設定
      if (pokemonModelDataManager.ShadowModel != null)
      {
        m_RenderForm.AddDrawableModel(pokemonModelDataManager.ShadowModel);
      }
      m_RenderForm.AddDrawableModel(pokemonModelDataManager.PokemonModel);
      if (pokemonModelDataManager.IdModel != null)
      {
        m_RenderForm.AddDrawableModel(pokemonModelDataManager.IdModel);
      }

			return pokemonModelDataManager;
		}

		/**
		 * @brief 各種UIにモデルの変更を通知
		 * @param modelDataManager UI上にデータを表示したいモデルデータ
		 */
		private void AddModelManagerToUserInterfaces(PokemonModelDataManager modelDataManager)
		{
			modelDataManager.AddListener(m_RenderForm);
			modelDataManager.AddListener(m_AnimationDataSetForm);
			modelDataManager.AddListener(m_AnimationTimeLineControlForm);
			modelDataManager.AddListener(m_TransformControlForm);
			modelDataManager.AddListener(m_LightControlForm);
			modelDataManager.AddListener(m_McnfForm);
			modelDataManager.AddListener(m_BlendListForm);
      modelDataManager.AddListener(m_MotionCompForm);
			modelDataManager.AddListener(m_EffectLocatorControlForm);
			modelDataManager.AddListener(m_FaceControlForm);
			modelDataManager.AddListener(m_AdjustmentFigureControlForm);
			modelDataManager.AddListener(m_ColorShaderControlForm);
			modelDataManager.AddListener(m_KeySoundControlForm);
      modelDataManager.AddListener(m_PokedecoLocatorControlForm);
      modelDataManager.AddListener(m_DisplayNodeHierarchyForm);
      modelDataManager.AddListener(m_CheckMaterialAttributeForm);
      modelDataManager.AddListener(m_MarkingControlForm);
		}

    public void SetFPS(double fps)
    {
      m_RenderForm.SetFPS(fps);
    }

		private void modelVisibilityToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.ChangeModelVisibility(modelVisibilityToolStripMenuItem.Checked);
			// この方法で消してしまうと, アウトラインも出なくなってしまうのでまずい.
			//m_PokemonModelDataManagerList.ForEach(data => data.SetVisible(modelVisibilityToolStripMenuItem.Checked));
		}

		private void wireFrameVisibilityToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.ChangeWireFrameVisibility(wireFrameVisibilityToolStripMenuItem.Checked);
		}

		private void jointVisibilityToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.ChangeJointVisibility(jointVisibilityToolStripMenuItem.Checked);
		}

		private void gridVisibilityToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.ChangeGridVisibility(gridVisibilityToolStripMenuItem.Checked);
		}

		private void gaugeVisibilityToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.ChangeGaugeVisibility(gaugeVisibilityToolStripMenuItem.Checked);
		}

	  private void interestRangeToolStripMenuItem_Click(object sender, EventArgs e)
	  {
	    m_RenderForm.ChangeInterestRangeVisibility(interestRangeToolStripMenuItem.Checked);
	  }

		private void focusPointToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.IsFocusPointVisible = focusPointToolStripMenuItem.Checked;
		}

		private void outLineToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.ChangeOutLineVisibility(outLineToolStripMenuItem.Checked);
		}

		private void informationToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.SetEnabledInformationVisibility(informationToolStripMenuItem.Checked);
		}

		private void Screen400x240ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.ChangeScreenSize(ScreenSizeFormat.Format_400x240);
		}

		private void Screen600x360ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.ChangeScreenSize(ScreenSizeFormat.Format_600x360);
		}

		private void Screen800x480ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.ChangeScreenSize(ScreenSizeFormat.Format_800x480);
		}

		private void idModelToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_PokemonModelDataManagerList.ForEach(data => data.SetIdModelVisible(idModelToolStripMenuItem.Checked));
		}

		private void lightPositionToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.ChangeLightPositionVisibility(lightPositionToolStripMenuItem.Checked);
		}

		private void backgroundToolStripMenuItem_Click(object sender, EventArgs e)
		{
			m_RenderForm.ChangeBackgroundVisibility(backgroundToolStripMenuItem.Checked);
		}

    private void animationDataSetFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_AnimationDataSetForm.Visible = animationDataSetFormToolStripMenuItem.Checked;
    } 

    private void animationTimeLineControlFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_AnimationTimeLineControlForm.Visible = animationTimeLineControlFormToolStripMenuItem.Checked;
    }

    private void modelMotionResourceTreeViewFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_ModelMotionResourceTreeViewForm.Visible = modelMotionResourceTreeViewFormToolStripMenuItem.Checked;
    }

    private void transformControlFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_TransformControlForm.Visible = transformControlFormToolStripMenuItem.Checked;
    }

    private void lightControlFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_LightControlForm.Visible = lightControlFormToolStripMenuItem.Checked;
    }

    private void cameraControlFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_CameraControlForm.Visible = cameraControlFormToolStripMenuItem.Checked;
    }

    private void mcnfFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_McnfForm.Visible = mcnfFormToolStripMenuItem.Checked;
    }

    private void effectLocatorControlFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_EffectLocatorControlForm.Visible = effectLocatorControlFormToolStripMenuItem.Checked;
    }

    private void keySoundControlFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_KeySoundControlForm.Visible = keySoundControlFormToolStripMenuItem.Checked;
    }

    private void AdjustFigureControlFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_AdjustmentFigureControlForm.Visible = AdjustFigureControlFormToolStripMenuItem.Checked;
    }

    private void faceControlFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_FaceControlForm.Visible = faceControlFormToolStripMenuItem.Checked;
    }

    private void colorShaderControlFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_ColorShaderControlForm.Visible = colorShaderControlFormToolStripMenuItem.Checked;
    }

    private void blendListFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_BlendListForm.Visible = blendListFormToolStripMenuItem.Checked;
    }

    private void motionCompFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_MotionCompForm.Visible = motionCompFormToolStripMenuItem.Checked;
    }

    private void pokedecoLocatorControlFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_PokedecoLocatorControlForm.Visible = pokedecoLocatorControlFormToolStripMenuItem.Checked;
    }

    private void displayNodeHierarchyFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_DisplayNodeHierarchyForm.Visible = displayNodeHierarchyFormToolStripMenuItem.Checked;
    }

    private void checkMaterialAttributeFormToolStripMenuItem_Click(object sender, EventArgs e)
    {
      m_CheckMaterialAttributeForm.Visible = checkMaterialAttributeFormToolStripMenuItem.Checked;
    }

    private void noEdgeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SelectEdgeTypeFromMenuBar(sender);
      m_RenderForm.ChangeFillScreenTexture(-1);
      m_RenderForm.ViewerRenderingPipeLine.ChangeFillScreenType(FillScreenConstantType.Enum.RGB);
    }

    private void normalColorEdgeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SelectEdgeTypeFromMenuBar(sender);
      m_RenderForm.ChangeFillScreenTexture(0);
      m_RenderForm.ViewerRenderingPipeLine.ChangeFillScreenType(FillScreenConstantType.Enum.RGB);
    }

    private void iDEdgeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SelectEdgeTypeFromMenuBar(sender);
      m_RenderForm.ChangeFillScreenTexture(1);
      m_RenderForm.ViewerRenderingPipeLine.ChangeFillScreenType(FillScreenConstantType.Enum.RGB);
    }

    private void SelectEdgeTypeFromMenuBar(object sender)
    {
      foreach (ToolStripMenuItem item in edgeColorToolStripMenuItem.DropDownItems)
      {
        if (object.ReferenceEquals(item, sender))
        {
          item.CheckState = CheckState.Indeterminate;
        }
        else
        {
          item.CheckState = CheckState.Unchecked;
        }
      }
    }
	}
}
