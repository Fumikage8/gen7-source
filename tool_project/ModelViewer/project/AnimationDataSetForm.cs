using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;
using System.Xml.Serialization;
using System.IO;

using gfl2.clr.animation;
using gfl2.clr.renderingengine.scenegraph.resource;
using gfl2.clr.poke_3d_lib;

namespace ModelViewer
{
	/**
	 * アニメーション関連のデータを設定するUIフォーム
	 */
	public partial class AnimationDataSetForm : Form, IModelMotionResourceChangeListener
	{
		private readonly ModelMotionResourceLoader m_ModelMotionResourceLoader;	//!< @brief リソース読み込み窓口
		private PokemonModelDataManager m_PokemonModelDataManager;				//!< @brief UI上で扱うPokeonModelDatamanager
		private readonly UInt32 m_AnimationNum;														//!< @brief アニメーションの総数
    public UInt32 AnimationNum { get { return m_AnimationNum; } }     //!< @brief アニメーションの総数取得

		// モーションに関連したコンポーネント
		private CheckBox[] m_motionEnabledCheckBoxes;					//!< @brief モーション有効化/無効化を切り替えるチェックボックス
		private ComboBox[] m_motionSelectComboBoxes;					//!< @brief モーションを選択するコンボボックス
		private Button[] m_prevMotionSelectButtons;						//!< @brief ひとつ前のモーションを選択するボタン
		private Button[] m_nextMotionSelectButtons;						//!< @brief ひとつ後のモーションを選択するボタン
		private TextBox[] m_motionStartFrameTextBoxes;				//!< @brief 開始フレームを設定するテキストボックス
		private TextBox[] m_motionEndFrameTextBoxes;					//!< @brief 終了フレームを設定するテキストボックス
		private Label[] m_motionCurrentFrameLabels;						//!< @brief 現在のフレームを表示するラベル
		private Label[] m_motionMaxFrameLabels;								//!< @brief 最大フレームを表示するラベル
		private Label[] m_motionFrameLabels;									//!< @brief モーションのフレーム数を表示するラベル
		
		// ブレンドに関連したコンポーネント
		private TextBox[] m_blendFrameSizeTextBoxes;					//!< @brief ブレンドの際のフレーム数を決めるテキストボックス
		private Label[] m_blendCurrentFrameLabels;						//!< @brief 現在のフレームを表示するラベル
		private Label[] m_blendMaxFrameLabels;								//!< @brief ブレンドの最大フレームを表示するラベル
		private Label[] m_blendFrameLabels;										//!< @brief モーションのフレームを表示するラベル

		/**
		 * @brief コンストラクタ
		 * @param applicationDataManager データ保存用オブジェクト
		 * @param animationTimeLine タイムライン
		 * @param motionNum 再生可能なモーション数
		 */
		public AnimationDataSetForm(ModelMotionResourceLoader modelMotionResourceLoader, UInt32 motionNum)
		{
			m_ModelMotionResourceLoader = modelMotionResourceLoader;
			m_AnimationNum = motionNum;

			InitializeComponent();

			m_motionEnabledCheckBoxes = new CheckBox[motionNum];
			m_motionEnabledCheckBoxes[0] = this.motionEnabledCheckBox0;
			m_motionEnabledCheckBoxes[1] = this.motionEnabledCheckBox1;
			m_motionEnabledCheckBoxes[2] = this.motionEnabledCheckBox2;
			m_motionEnabledCheckBoxes[3] = this.motionEnabledCheckBox3;
			m_motionEnabledCheckBoxes[4] = this.motionEnabledCheckBox4;
			m_motionEnabledCheckBoxes[5] = this.motionEnabledCheckBox5;
			m_motionEnabledCheckBoxes[6] = this.motionEnabledCheckBox6;
			m_motionEnabledCheckBoxes[7] = this.motionEnabledCheckBox7;
			m_motionEnabledCheckBoxes[8] = this.motionEnabledCheckBox8;
			m_motionEnabledCheckBoxes[9] = this.motionEnabledCheckBox9;
			m_motionSelectComboBoxes = new ComboBox[motionNum];
			m_motionSelectComboBoxes[0] = this.motionSelectComboBox0;
			m_motionSelectComboBoxes[1] = this.motionSelectComboBox1;
			m_motionSelectComboBoxes[2] = this.motionSelectComboBox2;
			m_motionSelectComboBoxes[3] = this.motionSelectComboBox3;
			m_motionSelectComboBoxes[4] = this.motionSelectComboBox4;
			m_motionSelectComboBoxes[5] = this.motionSelectComboBox5;
			m_motionSelectComboBoxes[6] = this.motionSelectComboBox6;
			m_motionSelectComboBoxes[7] = this.motionSelectComboBox7;
			m_motionSelectComboBoxes[8] = this.motionSelectComboBox8;
			m_motionSelectComboBoxes[9] = this.motionSelectComboBox9;
			m_prevMotionSelectButtons = new Button[motionNum];
			m_prevMotionSelectButtons[0] = this.prevMotionSelectButton0;
			m_prevMotionSelectButtons[1] = this.prevMotionSelectButton1;
			m_prevMotionSelectButtons[2] = this.prevMotionSelectButton2;
			m_prevMotionSelectButtons[3] = this.prevMotionSelectButton3;
			m_prevMotionSelectButtons[4] = this.prevMotionSelectButton4;
			m_prevMotionSelectButtons[5] = this.prevMotionSelectButton5;
			m_prevMotionSelectButtons[6] = this.prevMotionSelectButton6;
			m_prevMotionSelectButtons[7] = this.prevMotionSelectButton7;
			m_prevMotionSelectButtons[8] = this.prevMotionSelectButton8;
			m_prevMotionSelectButtons[9] = this.prevMotionSelectButton9;
			m_nextMotionSelectButtons = new Button[motionNum];
			m_nextMotionSelectButtons[0] = this.nextMotionSelectButton0;
			m_nextMotionSelectButtons[1] = this.nextMotionSelectButton1;
			m_nextMotionSelectButtons[2] = this.nextMotionSelectButton2;
			m_nextMotionSelectButtons[3] = this.nextMotionSelectButton3;
			m_nextMotionSelectButtons[4] = this.nextMotionSelectButton4;
			m_nextMotionSelectButtons[5] = this.nextMotionSelectButton5;
			m_nextMotionSelectButtons[6] = this.nextMotionSelectButton6;
			m_nextMotionSelectButtons[7] = this.nextMotionSelectButton7;
			m_nextMotionSelectButtons[8] = this.nextMotionSelectButton8;
			m_nextMotionSelectButtons[9] = this.nextMotionSelectButton9;
			m_motionStartFrameTextBoxes = new TextBox[motionNum];
			m_motionStartFrameTextBoxes[0] = this.motionStartFrameTextBox0;
			m_motionStartFrameTextBoxes[1] = this.motionStartFrameTextBox1;
			m_motionStartFrameTextBoxes[2] = this.motionStartFrameTextBox2;
			m_motionStartFrameTextBoxes[3] = this.motionStartFrameTextBox3;
			m_motionStartFrameTextBoxes[4] = this.motionStartFrameTextBox4;
			m_motionStartFrameTextBoxes[5] = this.motionStartFrameTextBox5;
			m_motionStartFrameTextBoxes[6] = this.motionStartFrameTextBox6;
			m_motionStartFrameTextBoxes[7] = this.motionStartFrameTextBox7;
			m_motionStartFrameTextBoxes[8] = this.motionStartFrameTextBox8;
			m_motionStartFrameTextBoxes[9] = this.motionStartFrameTextBox9;
			m_motionEndFrameTextBoxes = new TextBox[motionNum];
			m_motionEndFrameTextBoxes[0] = this.motionEndFrameTextBox0;
			m_motionEndFrameTextBoxes[1] = this.motionEndFrameTextBox1;
			m_motionEndFrameTextBoxes[2] = this.motionEndFrameTextBox2;
			m_motionEndFrameTextBoxes[3] = this.motionEndFrameTextBox3;
			m_motionEndFrameTextBoxes[4] = this.motionEndFrameTextBox4;
			m_motionEndFrameTextBoxes[5] = this.motionEndFrameTextBox5;
			m_motionEndFrameTextBoxes[6] = this.motionEndFrameTextBox6;
			m_motionEndFrameTextBoxes[7] = this.motionEndFrameTextBox7;
			m_motionEndFrameTextBoxes[8] = this.motionEndFrameTextBox8;
			m_motionEndFrameTextBoxes[9] = this.motionEndFrameTextBox9;
			m_motionCurrentFrameLabels = new Label[motionNum];
			m_motionCurrentFrameLabels[0] = this.motionCurrentFrameLabel0;
			m_motionCurrentFrameLabels[1] = this.motionCurrentFrameLabel1;
			m_motionCurrentFrameLabels[2] = this.motionCurrentFrameLabel2;
			m_motionCurrentFrameLabels[3] = this.motionCurrentFrameLabel3;
			m_motionCurrentFrameLabels[4] = this.motionCurrentFrameLabel4;
			m_motionCurrentFrameLabels[5] = this.motionCurrentFrameLabel5;
			m_motionCurrentFrameLabels[6] = this.motionCurrentFrameLabel6;
			m_motionCurrentFrameLabels[7] = this.motionCurrentFrameLabel7;
			m_motionCurrentFrameLabels[8] = this.motionCurrentFrameLabel8;
			m_motionCurrentFrameLabels[9] = this.motionCurrentFrameLabel9;
			m_motionMaxFrameLabels = new Label[motionNum];
			m_motionMaxFrameLabels[0] = this.motionMaxFrameLabel0;
			m_motionMaxFrameLabels[1] = this.motionMaxFrameLabel1;
			m_motionMaxFrameLabels[2] = this.motionMaxFrameLabel2;
			m_motionMaxFrameLabels[3] = this.motionMaxFrameLabel3;
			m_motionMaxFrameLabels[4] = this.motionMaxFrameLabel4;
			m_motionMaxFrameLabels[5] = this.motionMaxFrameLabel5;
			m_motionMaxFrameLabels[6] = this.motionMaxFrameLabel6;
			m_motionMaxFrameLabels[7] = this.motionMaxFrameLabel7;
			m_motionMaxFrameLabels[8] = this.motionMaxFrameLabel8;
			m_motionMaxFrameLabels[9] = this.motionMaxFrameLabel9;
			m_motionFrameLabels = new Label[motionNum];
			m_motionFrameLabels[0] = this.motionFrameLabel0;
			m_motionFrameLabels[1] = this.motionFrameLabel1;
			m_motionFrameLabels[2] = this.motionFrameLabel2;
			m_motionFrameLabels[3] = this.motionFrameLabel3;
			m_motionFrameLabels[4] = this.motionFrameLabel4;
			m_motionFrameLabels[5] = this.motionFrameLabel5;
			m_motionFrameLabels[6] = this.motionFrameLabel6;
			m_motionFrameLabels[7] = this.motionFrameLabel7;
			m_motionFrameLabels[8] = this.motionFrameLabel8;
			m_motionFrameLabels[9] = this.motionFrameLabel9;

			m_blendFrameSizeTextBoxes = new TextBox[motionNum - 1];
			m_blendFrameSizeTextBoxes[0] = this.blendFrameSizeTextBox0_1;
			m_blendFrameSizeTextBoxes[1] = this.blendFrameSizeTextBox1_2;
			m_blendFrameSizeTextBoxes[2] = this.blendFrameSizeTextBox2_3;
			m_blendFrameSizeTextBoxes[3] = this.blendFrameSizeTextBox3_4;
			m_blendFrameSizeTextBoxes[4] = this.blendFrameSizeTextBox4_5;
			m_blendFrameSizeTextBoxes[5] = this.blendFrameSizeTextBox5_6;
			m_blendFrameSizeTextBoxes[6] = this.blendFrameSizeTextBox6_7;
			m_blendFrameSizeTextBoxes[7] = this.blendFrameSizeTextBox7_8;
			m_blendFrameSizeTextBoxes[8] = this.blendFrameSizeTextBox8_9;
			m_blendCurrentFrameLabels = new Label[motionNum - 1];
			m_blendCurrentFrameLabels[0] = this.blendCurrentFrameLabel0_1;
			m_blendCurrentFrameLabels[1] = this.blendCurrentFrameLabel1_2;
			m_blendCurrentFrameLabels[2] = this.blendCurrentFrameLabel2_3;
			m_blendCurrentFrameLabels[3] = this.blendCurrentFrameLabel3_4;
			m_blendCurrentFrameLabels[4] = this.blendCurrentFrameLabel4_5;
			m_blendCurrentFrameLabels[5] = this.blendCurrentFrameLabel5_6;
			m_blendCurrentFrameLabels[6] = this.blendCurrentFrameLabel6_7;
			m_blendCurrentFrameLabels[7] = this.blendCurrentFrameLabel7_8;
			m_blendCurrentFrameLabels[8] = this.blendCurrentFrameLabel8_9;
			m_blendMaxFrameLabels = new Label[motionNum - 1];
			m_blendMaxFrameLabels[0] = this.blendMaxFrameLabel0_1;
			m_blendMaxFrameLabels[1] = this.blendMaxFrameLabel1_2;
			m_blendMaxFrameLabels[2] = this.blendMaxFrameLabel2_3;
			m_blendMaxFrameLabels[3] = this.blendMaxFrameLabel3_4;
			m_blendMaxFrameLabels[4] = this.blendMaxFrameLabel4_5;
			m_blendMaxFrameLabels[5] = this.blendMaxFrameLabel5_6;
			m_blendMaxFrameLabels[6] = this.blendMaxFrameLabel6_7;
			m_blendMaxFrameLabels[7] = this.blendMaxFrameLabel7_8;
			m_blendMaxFrameLabels[8] = this.blendMaxFrameLabel8_9;
			m_blendFrameLabels = new Label[motionNum - 1];
			m_blendFrameLabels[0] = this.blendFrameLabel0_1;
			m_blendFrameLabels[1] = this.blendFrameLabel1_2;
			m_blendFrameLabels[2] = this.blendFrameLabel2_3;
			m_blendFrameLabels[3] = this.blendFrameLabel3_4;
			m_blendFrameLabels[4] = this.blendFrameLabel4_5;
			m_blendFrameLabels[5] = this.blendFrameLabel5_6;
			m_blendFrameLabels[6] = this.blendFrameLabel6_7;
			m_blendFrameLabels[7] = this.blendFrameLabel7_8;
			m_blendFrameLabels[8] = this.blendFrameLabel8_9;
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

		/**
		 * @brief モーション読み込み前のコンポーネントの初期化
		 */
		private void SetEnableComponentsBeforeMotionLoaded()
		{
			// 再読み込みボタンを有効化
			reloadButton.Enabled = true;
		}

		/**
		 * @brief 初期化
		 * @param object
		 * @param EventArgs
		 */
		private void DataSetForm_Load(object sender, EventArgs e)
		{
		}

		/**
		 * @PokemonModelDataManagerの変更通知受け取り
		 */
		public void ReceiveNotification(PokemonModelDataManager modelDataManager)
		{
			AnimationTimeLine animationTimeLine = null;
			if (modelDataManager != null)
			{
				animationTimeLine = modelDataManager.AnimationTimeLine;
			}

      if (m_PokemonModelDataManager != modelDataManager)
			{// 参照するモデルデータが変わったらその都度変更
				m_PokemonModelDataManager = modelDataManager;
				if (modelDataManager == null)
				{
					Array.ForEach(m_motionSelectComboBoxes, component => component.Items.Clear());
					Array.ConvertAll(m_motionEnabledCheckBoxes, component => component.Checked = false);
					Array.ConvertAll(m_motionEnabledCheckBoxes, component => component.Enabled = false);
					Array.ConvertAll(m_motionSelectComboBoxes, component => component.Enabled = false);
					Array.ConvertAll(m_prevMotionSelectButtons, component => component.Enabled = false);
					Array.ConvertAll(m_nextMotionSelectButtons, component => component.Enabled = false);
					Array.ForEach(
						m_motionStartFrameTextBoxes,
						component => { 
							component.Enabled = false;
							component.Text = "0";
						});
					Array.ForEach(
						m_motionEndFrameTextBoxes,
						component =>
						{
							component.Enabled = false;
							component.Text = "0";
						});
					Array.ForEach(
						m_blendFrameSizeTextBoxes,
						component =>
						{
							component.Enabled = false;
							component.Text = "0";
						});
					frameRateAutoSettingCheckBox.Checked = false;
					frameRateAutoSettingCheckBox.Enabled = false;
					resetButton.Enabled = false;
					loadPlayListButton.Enabled = false;
					savePlayListButton.Enabled = false;
				}
				else
				{
					string[] gfmotFilePaths = Array.ConvertAll(
						modelDataManager.GfmotFilePaths,
						data => System.IO.Path.GetFileName(data));

					Array.ConvertAll(m_motionEnabledCheckBoxes, component => component.Enabled = (gfmotFilePaths.Length > 0));

					for (Int32 i = 0; i < m_motionSelectComboBoxes.Length; ++i)
					{
						m_motionSelectComboBoxes[i].Items.Clear();
						m_motionSelectComboBoxes[i].Items.AddRange(gfmotFilePaths);
					}

					AnimationTimeLine timeLine = modelDataManager.AnimationTimeLine;
					frameRateAutoSettingCheckBox.Checked = timeLine.FrameRateAutoSettingEnable;
					frameRateAutoSettingCheckBox.Enabled = true;
					resetButton.Enabled = true;
					loadPlayListButton.Enabled = true;
					savePlayListButton.Enabled = true;


					// モデルが切り替わったときはモーションのチェックを切り替える
					TimeLineData[] dataArray = timeLine.TimeLineDataArray;
					for (Int32 i = 0; i < dataArray.Length; ++i)
					{
						m_motionEnabledCheckBoxes[i].Checked = dataArray[i].IsAvailable;
            m_motionSelectComboBoxes[i].SelectedIndex = -1;
            if (gfmotFilePaths.Length > 0)
            {
              m_motionSelectComboBoxes[i].SelectedIndex = Math.Max(0, dataArray[i].MotionIndex);
            }
						m_motionStartFrameTextBoxes[i].Text = dataArray[i].StartFrame.ToString();
						m_motionEndFrameTextBoxes[i].Text = dataArray[i].EndFrame.ToString();
						if (i < dataArray.Length - 1)
						{
							m_blendFrameSizeTextBoxes[i].Text = dataArray[i].InterpSize.ToString();
						}
					}
				}
			}

      if (m_PokemonModelDataManager != null)
      {
        m_PokemonModelDataManager.WalkEnabled = this.walkCheckBox.Checked;
      }

			ReceiveAnimationTimeLineNotification(animationTimeLine);
		}

		/**
		 * @brief タイムライン変更通知の受け取り
		 */
		private void ReceiveAnimationTimeLineNotification(AnimationTimeLine animationTimeLine)
		{
			if (animationTimeLine == null)
			{
				animationTimeLine = new AnimationTimeLine(ApplicationCore.ANIMATION_NUM_ENABLED_TO_SET, 2.0m, false);
			}

			TimeLineData[] timeLineDataArray = animationTimeLine.TimeLineDataArray;

			Int32 motionIndex;
			decimal motionFrame;
			Int32 index = animationTimeLine.GetCurrentMotion(out motionIndex, out motionFrame);
			if (index >= 0)
			{
				m_motionCurrentFrameLabels[index].Text = motionFrame.ToString();
			}

			// モーションを選択可能/不可を決める
			for (Int32 i = 0; i < timeLineDataArray.Length; ++i)
			{
				bool enabled = timeLineDataArray[i].IsAvailable;
				m_motionSelectComboBoxes[i].Enabled = enabled;
				m_motionStartFrameTextBoxes[i].Enabled = enabled;
				m_motionEndFrameTextBoxes[i].Enabled = enabled && !animationTimeLine.FrameRateAutoSettingEnable;
				m_prevMotionSelectButtons[i].Enabled = enabled;
				m_nextMotionSelectButtons[i].Enabled = enabled;
			}

			if (animationTimeLine.CurrentFrame == 0.0m)
			{// 終わったら全て初期値に戻す
				for (Int32 i = 0; i < timeLineDataArray.Length; ++i)
				{
					m_motionCurrentFrameLabels[i].Text = timeLineDataArray[i].StartFrame.ToString();
					if (i < timeLineDataArray.Length - 1)
					{
						m_blendCurrentFrameLabels[i].Text = "0";
					}
				}
			}

			// UI上のフレームを更新する
			Int32 prevMotionIndex;
			decimal prevMotionFrame;
			decimal interpRatio;
			Int32 prevIndex = animationTimeLine.GetCurrentBlendMotion(out prevMotionIndex, out prevMotionFrame, out interpRatio);
			if (prevIndex >= 0)
			{// 補間中なら補間フレームも更新する
				TimeLineData blendTimeLineData = animationTimeLine.TimeLineDataArray[prevIndex];
				decimal blendFrame = (prevMotionFrame - blendTimeLineData.EndFrame + (blendTimeLineData.FrameSize)) % (blendTimeLineData.FrameSize);
				m_motionCurrentFrameLabels[prevIndex].Text = prevMotionFrame.ToString();
				m_blendCurrentFrameLabels[prevIndex].Text = blendFrame.ToString();
			}

			for (Int32 i = 0; i < timeLineDataArray.Length; ++i)
			{
				TimeLineData data = timeLineDataArray[i];
				m_motionMaxFrameLabels[i].Text = data.EndFrame.ToString();
				m_motionFrameLabels[i].Text = data.FrameSize.ToString();

				if (i < timeLineDataArray.Length - 1)
				{
					m_blendFrameLabels[i].Text = data.InterpSize.ToString();

					m_blendMaxFrameLabels[i].Text = ((data.InterpSize > 0)
            ? (data.InterpSize - 1)
            : data.InterpSize).ToString();
				}
			}
    }

		/**
		 * @brief リロードボタン押下
		 * @param object
		 * @param EventArgs
		 */
		private void reloadButton_Click(object sender, EventArgs e)
		{
			// 補間フレーム自動設定のチェックを外す
			frameRateAutoSettingCheckBox.Checked = false;

			//// 現在読み込んでいるパスを保存
			//string[] motionPaths = new string[m_AnimationNum];
			//for (Int32 i = 0; i < m_AnimationNum; ++i)
			//{
			//	motionPaths[i] = m_ApplicationDataManager.GetGfmotFilePath(i);
			//}
			//m_ApplicationDataManager.ReloadGfmdl();
			//for (Int32 i = 0; i < m_AnimationNum; ++i)
			//{
			//	m_ApplicationDataManager.LoadGfmot(i, motionPaths[i]);
			//}
		}

		/**
		 * @brief リセット
		 * @param object
		 * @param EventArgs
		 */
		private void resetButton_Click(object sender, EventArgs e)
		{
			Array.ForEach(m_motionSelectComboBoxes, box => box.SelectedIndex = 0);
		}

		/**
		 * @brief モーション開始フレームの値の異常値チェック
		 * @param index モーション番号
		 * @return bool 成功/失敗
		 */
		private bool ValidateInputMotionStartFrame(UInt32 index)
		{
			if (m_PokemonModelDataManager == null)
			{
				return false;
			}

			if (m_PokemonModelDataManager.AnimationTimeLine.IsEmpty())
			{
				return false;
			}
			bool isSucceeded = m_PokemonModelDataManager.AnimationTimeLine.SetStartFrame(index, m_motionStartFrameTextBoxes[index].Text);
			if (!isSucceeded)
			{
				MessageBox.Show("0以上で、終了フレームより小さい整数値を入力してください");
				m_motionStartFrameTextBoxes[index].Text = m_PokemonModelDataManager.AnimationTimeLine.GetStartFrame(index).ToString();
			}

			return isSucceeded;
		}

		/**
		 * @brief モーション開始フレーム[0]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionStartFrameTextBox0_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionStartFrame(0);
		}

		/**
		 * @brief モーション開始フレーム[1]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionStartFrameTextBox1_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionStartFrame(1);
		}

		/**
		 * @brief モーション開始フレーム[2]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionStartFrameTextBox2_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionStartFrame(2);
		}

		/**
		 * @brief モーション開始フレーム[3]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionStartFrameTextBox3_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionStartFrame(3);
		}

		/**
		 * @brief モーション開始フレーム[4]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionStartFrameTextBox4_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionStartFrame(4);
		}

		/**
		 * @brief モーション開始フレーム[5]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionStartFrameTextBox5_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionStartFrame(5);
		}

		/**
		 * @brief モーション開始フレーム[6]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionStartFrameTextBox6_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionStartFrame(6);
		}

		/**
		 * @brief モーション開始フレーム[7]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionStartFrameTextBox7_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionStartFrame(7);
		}

		/**
		 * @brief モーション開始フレーム[8]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionStartFrameTextBox8_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionStartFrame(8);
		}

		/**
		 * @brief モーション開始フレーム[9]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionStartFrameTextBox9_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionStartFrame(9);
		}

		/**
		 * @brief モーション終了フレームの値の異常値チェック
		 * @param index モーション番号
		 * @return bool 成功/失敗
		 */
		private bool ValidateInputMotionEndFrame(UInt32 index)
		{
			if (m_PokemonModelDataManager == null)
			{
				return false;
			}

			if (m_PokemonModelDataManager.AnimationTimeLine.IsEmpty())
			{
				return false;
			}

			if (!m_motionEndFrameTextBoxes[index].Enabled)
			{
				return false;
			}

			bool isSucceeded = m_PokemonModelDataManager.AnimationTimeLine.SetEndFrame(index, m_motionEndFrameTextBoxes[index].Text);
			if (!isSucceeded)
			{
				MessageBox.Show("範囲内の整数値を入力してください");
				m_motionEndFrameTextBoxes[index].Text = m_PokemonModelDataManager.AnimationTimeLine.GetEndFrame(index).ToString();
			}

			return isSucceeded;
		}

		/**
		 * @brief モーション終了フレーム[0]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionEndFrameTextBox0_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionEndFrame(0);
		}

		/**
		 * @brief モーション終了フレーム[1]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionEndFrameTextBox1_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionEndFrame(1);
		}

		/**
		 * @brief モーション終了フレーム[2]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionEndFrameTextBox2_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionEndFrame(2);
		}

		/**
		 * @brief モーション終了フレーム[3]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionEndFrameTextBox3_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionEndFrame(3);
		}

		/**
		 * @brief モーション終了フレーム[4]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionEndFrameTextBox4_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionEndFrame(4);
		}

		/**
		 * @brief モーション終了フレーム[5]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionEndFrameTextBox5_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionEndFrame(5);
		}

		/**
		 * @brief モーション終了フレーム[6]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionEndFrameTextBox6_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionEndFrame(6);
		}

		/**
		 * @brief モーション終了フレーム[7]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionEndFrameTextBox7_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionEndFrame(7);
		}

		/**
		 * @brief モーション終了フレーム[8]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionEndFrameTextBox8_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionEndFrame(8);
		}

		/**
		 * @brief モーション終了フレーム[9]変更
		 * @param object
		 * @param EventArgs
		 */
		private void motionEndFrameTextBox9_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputMotionEndFrame(9);
		}

		/**
		 * @brief モーションブレンドの際のフレームの値の異常値チェック
		 * @param index 配列上のモーション番号
		 * @return bool 成功/失敗
		 */
		private bool ValidateInputBlendFrame(UInt32 index)
		{
			if (m_PokemonModelDataManager == null)
			{
				return false;
			}

			if (m_PokemonModelDataManager.AnimationTimeLine.IsEmpty())
			{
				return false;
			}
			bool isSucceeded = m_PokemonModelDataManager.AnimationTimeLine.SetBlendFrame(index, m_blendFrameSizeTextBoxes[index].Text);
			if (!isSucceeded)
			{
				MessageBox.Show("1以上で範囲内の整数値を入力してください");
				m_blendFrameSizeTextBoxes[index].Text = m_PokemonModelDataManager.AnimationTimeLine.GetBlendFrame(index).ToString();
			}

			return isSucceeded;
		}

		/**
		 * @brief モーションブレンドフレーム[0]変更
		 * @param object
		 * @param EventArgs
		 */
		private void blendFrameSizeTextBox0_1_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputBlendFrame(0);
		}

		/**
		 * @brief モーションブレンドフレーム[1]変更
		 * @param object
		 * @param EventArgs
		 */
		private void blendFrameSizeTextBox1_2_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputBlendFrame(1);
		}

		/**
		 * @brief モーションブレンドフレーム[2]変更
		 * @param object
		 * @param EventArgs
		 */
		private void blendFrameSizeTextBox2_3_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputBlendFrame(2);
		}

		/**
		 * @brief モーションブレンドフレーム[3]変更
		 * @param object
		 * @param EventArgs
		 */
		private void blendFrameSizeTextBox3_4_TextChanged(object sender, EventArgs e)
		{
			ValidateInputBlendFrame(3);
		}

		/**
		 * @brief モーションブレンドフレーム[4]変更
		 * @param object
		 * @param EventArgs
		 */
		private void blendFrameSizeTextBox4_5_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputBlendFrame(4);
		}

		/**
		 * @brief モーションブレンドフレーム[5]変更
		 * @param object
		 * @param EventArgs
		 */
		private void blendFrameSizeTextBox5_6_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputBlendFrame(5);
		}

		/**
		 * @brief モーションブレンドフレーム[6]変更
		 * @param object
		 * @param EventArgs
		 */
		private void blendFrameSizeTextBox6_7_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputBlendFrame(6);
		}

		/**
		 * @brief モーションブレンドフレーム[7]変更
		 * @param object
		 * @param EventArgs
		 */
		private void blendFrameSizeTextBox7_8_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputBlendFrame(7);
		}

		/**
		 * @brief モーションブレンドフレーム[8]変更
		 * @param object
		 * @param EventArgs
		 */
		private void blendFrameSizeTextBox8_9_TextChanged(object sender, EventArgs e)
		{
			//ValidateInputBlendFrame(8);
		}

		/**
		 * @brief 補間フレーム自動設定のON/OFF切替
		 * @param object
		 * @param EventArgs
		 */
		private void frameRateAutoSettingCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			if (m_PokemonModelDataManager == null)
			{
				return;
			}

			AnimationTimeLine timeLine = m_PokemonModelDataManager.AnimationTimeLine;
			TimeLineData[] dataArray = timeLine.TimeLineDataArray;
			timeLine.SetFrameRateAutoSettingEnable(frameRateAutoSettingCheckBox.Checked);

			// チェックに応じて終了フレームの設定の有効/無効を切り替え
			for (Int32 i = 0; i < dataArray.Length; ++i)
			{
				if (!dataArray[i].IsAvailable)
				{
					continue;
				}

				m_motionEndFrameTextBoxes[i].Enabled = !frameRateAutoSettingCheckBox.Checked;
			}

			if (!frameRateAutoSettingCheckBox.Checked)
			{// チェックつけられてないならここで終わり
				return;
			}

			// チェックが付けられたら修正された変更フレームを設定
			Int32 availableMotionNum = Array.FindAll(dataArray, data => data.IsAvailable == true).Length;
			Int32 availableMotionCount = 0;
			for (Int32 i = 0; i < dataArray.Length; ++i)
			{
				if (!dataArray[i].IsAvailable)
				{
					continue;
				}

				m_motionStartFrameTextBoxes[i].Text = dataArray[i].StartFrame.ToString();
				ValidateStartFrame(i);

				Int32 nextIndex = timeLine.FindFirstAvailableIndex(i);
				if (nextIndex < 0)
				{// 次のフレームを探して見つからなかったら終了
					break;
				}

				// 補間フレームがあるなら設定
				float interpSize = (float)dataArray[i].InterpSize;
				float endFrame = (float)dataArray[i].FrameSize - 1.0f - interpSize;
				if ((endFrame <= 0.0f) && (availableMotionCount < availableMotionCount - 1))
				{// 終了フレームがマイナスになってしまったら補間フレーム1にしておく
					endFrame = (float)(dataArray[i].FrameSize) - 1.0f;
					interpSize = 1;
				}
				m_motionEndFrameTextBoxes[i].Text = endFrame.ToString();
				ValidateEndFrame(i);
				m_blendFrameSizeTextBoxes[i].Text = interpSize.ToString();
				ValidateBlendFrame(i);
			}
		}

		private void ChangeMotion(Int32 index, Int32 motionIndex)
		{
			if (m_PokemonModelDataManager == null)
			{
				return;
			}

			// 「使わない」の項目が先頭にあるので-1
			// @todo: チェックボックスで有効/無効を切り替えられるようにしたらここの修正が絶対に必要になる
      m_PokemonModelDataManager.ResetLoopMotion();
			m_PokemonModelDataManager.SetMotion(index, motionIndex);

			AnimationTimeLine timeLine = m_PokemonModelDataManager.AnimationTimeLine;
			decimal startFrame = timeLine.TimeLineDataArray[index].StartFrame;
			decimal endFrame = timeLine.TimeLineDataArray[index].EndFrame;
			decimal interpSize = timeLine.TimeLineDataArray[index].InterpSize;

			if (timeLine.FrameRateAutoSettingEnable)
			{// 自動補間が有効の場合はフレームを調整する
				endFrame -= interpSize;
			}

      if (timeLine.LoopEnable)
      {
        if (endFrame > timeLine.TimeLineDataArray[index].FrameSize - 2)
        {
          endFrame = timeLine.TimeLineDataArray[index].FrameSize - 2;
        }
      }

			m_motionStartFrameTextBoxes[index].Text = startFrame.ToString();
			ValidateStartFrame(index);
			m_motionEndFrameTextBoxes[index].Text = endFrame.ToString();
			ValidateEndFrame(index);
			if (index < timeLine.TimeLineDataArray.Length - 1)
			{
				m_blendFrameSizeTextBoxes[index].Text = interpSize.ToString();
				ValidateBlendFrame(index);
			}

			// モーションが切り替わったので、タイムラインのフレームを0に戻す
			timeLine.Reset();

			// ラベルの表示が前回の更新のままとまっているので、再設定された値に変更する
			TimeLineData[] dataArray = timeLine.TimeLineDataArray;
			for (Int32 i = 0; i < dataArray.Length; ++i)
			{
				m_motionCurrentFrameLabels[i].Text = dataArray[i].StartFrame.ToString();
				if (i < dataArray.Length - 1)
				{
					m_blendCurrentFrameLabels[i].Text = "0";
				}
			}
		}

		private void motionSelectComboBox0_SelectedIndexChanged(object sender, EventArgs e)
		{
			ChangeMotion(0, m_motionSelectComboBoxes[0].SelectedIndex);
		}

		private void motionSelectComboBox1_SelectedIndexChanged(object sender, EventArgs e)
		{
			ChangeMotion(1, m_motionSelectComboBoxes[1].SelectedIndex);
		}

		private void motionSelectComboBox2_SelectedIndexChanged(object sender, EventArgs e)
		{
			ChangeMotion(2, m_motionSelectComboBoxes[2].SelectedIndex);
		}

		private void motionSelectComboBox3_SelectedIndexChanged(object sender, EventArgs e)
		{
			ChangeMotion(3, m_motionSelectComboBoxes[3].SelectedIndex);
		}

		private void motionSelectComboBox4_SelectedIndexChanged(object sender, EventArgs e)
		{
			ChangeMotion(4, m_motionSelectComboBoxes[4].SelectedIndex);
		}

		private void motionSelectComboBox5_SelectedIndexChanged(object sender, EventArgs e)
		{
			ChangeMotion(5, m_motionSelectComboBoxes[5].SelectedIndex);
		}

		private void motionSelectComboBox6_SelectedIndexChanged(object sender, EventArgs e)
		{
			ChangeMotion(6, m_motionSelectComboBoxes[6].SelectedIndex);
		}

		private void motionSelectComboBox7_SelectedIndexChanged(object sender, EventArgs e)
		{
			ChangeMotion(7, m_motionSelectComboBoxes[7].SelectedIndex);
		}

		private void motionSelectComboBox8_SelectedIndexChanged(object sender, EventArgs e)
		{
			ChangeMotion(8, m_motionSelectComboBoxes[8].SelectedIndex);
		}

		private void motionSelectComboBox9_SelectedIndexChanged(object sender, EventArgs e)
		{
			ChangeMotion(9, m_motionSelectComboBoxes[9].SelectedIndex);
		}

    public void SetMotionClear()
    {
      foreach (var motionSelectComboBox in m_motionSelectComboBoxes)
      {
        motionSelectComboBox.SelectedIndex = 0;
      }
    }
    public int GetMotionIndex(int comboBoxIndex)
    {
      return m_motionSelectComboBoxes[comboBoxIndex].SelectedIndex;
    }
    public void SetMotionChangeIndex(int comboBoxIndex, int selectedIndex)
    {
      m_motionSelectComboBoxes[comboBoxIndex].SelectedIndex = selectedIndex;
    }
    public void SetMotionChangeString(int comboBoxIndex, string selectedName)
    {
      // 同じ名前が含まれるモーションを探す
      for (var i = 0; i < m_motionSelectComboBoxes[0].Items.Count; ++i)
      {
        var itemName = m_motionSelectComboBoxes[0].Items[i] as string;

        if (itemName.Contains(selectedName))
        {
          this.SetMotionChangeIndex(comboBoxIndex, i);
          break;
        }
      }
    }
    public void SetBlendFrame(int comboBoxIndex, int blendFrame)
    {
			AnimationTimeLine timeLine = m_PokemonModelDataManager.AnimationTimeLine;
			if (!timeLine.TimeLineDataArray[comboBoxIndex].IsAvailable)
			{
				return;
			}
      frameRateAutoSettingCheckBox.Checked = true;
      m_blendFrameSizeTextBoxes[comboBoxIndex].Text = blendFrame.ToString();
			ValidateBlendFrame(comboBoxIndex);
    }

		private bool ValidateBlendFrame(Int32 index)
		{
			AnimationTimeLine timeLine = m_PokemonModelDataManager.AnimationTimeLine;
			TimeLineData[] dataArray = timeLine.TimeLineDataArray;
			UInt32 prevBlendFrame = dataArray[index].InterpSize;

			UInt32 blendFrame = 0;
			bool isSucceeded = UInt32.TryParse(m_blendFrameSizeTextBoxes[index].Text, out blendFrame);
			if (!isSucceeded)
			{
				m_blendFrameSizeTextBoxes[index].Text = prevBlendFrame.ToString();
				return false;
			}

			decimal blendFrameUpperLimit = dataArray[index].FrameSize - 1;
			if (timeLine.FrameRateAutoSettingEnable)
			{
				blendFrameUpperLimit -= 1.0m;
			}

			if (blendFrame < 0)
			{
				blendFrame = 0;
			}
			else if (blendFrame > blendFrameUpperLimit)
			{
				blendFrame = (UInt32)blendFrameUpperLimit;
				m_blendFrameSizeTextBoxes[index].Text = blendFrame.ToString();
			}

			dataArray[index].InterpSize = blendFrame;

			if (timeLine.FrameRateAutoSettingEnable)
			{// 自動フレーム補間が有効の場合は値を設定する
				m_motionEndFrameTextBoxes[index].Text =
					(dataArray[index].FrameSize - blendFrame - 1).ToString();
				ValidateEndFrame(index);
			}

			timeLine.Reset();
			return true;
		}

		private void blendFrameSizeTextBox0_1_Validating(object sender, CancelEventArgs e)
		{
			ValidateBlendFrame(0);
		}

		private void blendFrameSizeTextBox0_1_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				blendFrameSizeTextBox0_1_Validating(sender, null);
			}
		}

		private void blendFrameSizeTextBox1_2_Validating(object sender, CancelEventArgs e)
		{
			ValidateBlendFrame(1);
		}

		private void blendFrameSizeTextBox1_2_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				blendFrameSizeTextBox1_2_Validating(sender, null);
			}
		}

		private void blendFrameSizeTextBox2_3_Validating(object sender, CancelEventArgs e)
		{
			ValidateBlendFrame(2);
		}

		private void blendFrameSizeTextBox2_3_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				blendFrameSizeTextBox2_3_Validating(sender, null);
			}
		}

		private void blendFrameSizeTextBox3_4_Validating(object sender, CancelEventArgs e)
		{
			ValidateBlendFrame(3);
		}

		private void blendFrameSizeTextBox3_4_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				blendFrameSizeTextBox3_4_Validating(sender, null);
			}
		}

		private void blendFrameSizeTextBox4_5_Validating(object sender, CancelEventArgs e)
		{
			ValidateBlendFrame(4);
		}

		private void blendFrameSizeTextBox4_5_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				blendFrameSizeTextBox4_5_Validating(sender, null);
			}
		}

		private void blendFrameSizeTextBox5_6_Validating(object sender, CancelEventArgs e)
		{
			ValidateBlendFrame(5);
		}

		private void blendFrameSizeTextBox5_6_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				blendFrameSizeTextBox5_6_Validating(sender, null);
			}
		}

		private void blendFrameSizeTextBox6_7_Validating(object sender, CancelEventArgs e)
		{
			ValidateBlendFrame(6);
		}

		private void blendFrameSizeTextBox6_7_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				blendFrameSizeTextBox6_7_Validating(sender, null);
			}
		}

		private void blendFrameSizeTextBox7_8_Validating(object sender, CancelEventArgs e)
		{
			ValidateBlendFrame(7);
		}

		private void blendFrameSizeTextBox7_8_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				blendFrameSizeTextBox7_8_Validating(sender, null);
			}
		}

		private void blendFrameSizeTextBox8_9_Validating(object sender, CancelEventArgs e)
		{
			ValidateBlendFrame(8);
		}

		private void blendFrameSizeTextBox8_9_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				blendFrameSizeTextBox8_9_Validating(sender, null);
			}
		}

    public void InputEndFrame(Int32 index, decimal frame)
    {
      if (m_PokemonModelDataManager == null)
      {
        return;
      }

      if (!m_PokemonModelDataManager.LoopEnable)
      {
        return;
      }

      if (frame < 1.0m)
      {
        return;
      }

      m_motionEndFrameTextBoxes[index].Text = frame.ToString();
    }

		private bool ValidateStartFrame(Int32 index)
		{
			TimeLineData data = m_PokemonModelDataManager.AnimationTimeLine.TimeLineDataArray[index];
			decimal prevFrame = data.StartFrame;

			decimal frame = 0;
			bool isSucceeded = decimal.TryParse(m_motionStartFrameTextBoxes[index].Text, out frame);
			if (!isSucceeded)
			{
				m_motionStartFrameTextBoxes[index].Text = prevFrame.ToString();
				return false;
			}

			if (frame >= data.EndFrame)
			{
				frame = data.EndFrame - 1.0m;
			}

			if (frame < 0)
			{
				frame = 0;
			}

			m_PokemonModelDataManager.AnimationTimeLine.TimeLineDataArray[index].StartFrame = frame;
			m_PokemonModelDataManager.AnimationTimeLine.Reset();

			return true;
		}

		private void motionStartFrameTextBox0_Validating(object sender, CancelEventArgs e)
		{
			ValidateStartFrame(0);
		}

		private void motionStartFrameTextBox0_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionStartFrameTextBox0_Validating(sender, null);
			}
		}

		private void motionStartFrameTextBox1_Validating(object sender, CancelEventArgs e)
		{
			ValidateStartFrame(1);
		}

		private void motionStartFrameTextBox1_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionStartFrameTextBox1_Validating(sender, null);
			}
		}

		private void motionStartFrameTextBox2_Validating(object sender, CancelEventArgs e)
		{
			ValidateStartFrame(2);
		}

		private void motionStartFrameTextBox2_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionStartFrameTextBox2_Validating(sender, null);
			}
		}

		private void motionStartFrameTextBox3_Validating(object sender, CancelEventArgs e)
		{
			ValidateStartFrame(3);
		}

		private void motionStartFrameTextBox3_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionStartFrameTextBox3_Validating(sender, null);
			}
		}

		private void motionStartFrameTextBox4_Validating(object sender, CancelEventArgs e)
		{
			ValidateStartFrame(4);
		}

		private void motionStartFrameTextBox4_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionStartFrameTextBox4_Validating(sender, null);
			}
		}

		private void motionStartFrameTextBox5_Validating(object sender, CancelEventArgs e)
		{
			ValidateStartFrame(5);
		}

		private void motionStartFrameTextBox5_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionStartFrameTextBox5_Validating(sender, null);
			}
		}

		private void motionStartFrameTextBox6_Validating(object sender, CancelEventArgs e)
		{
			ValidateStartFrame(6);
		}

		private void motionStartFrameTextBox6_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionStartFrameTextBox6_Validating(sender, null);
			}
		}

		private void motionStartFrameTextBox7_Validating(object sender, CancelEventArgs e)
		{
			ValidateStartFrame(7);
		}

		private void motionStartFrameTextBox7_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionStartFrameTextBox7_Validating(sender, null);
			}
		}

		private void motionStartFrameTextBox8_Validating(object sender, CancelEventArgs e)
		{
			ValidateStartFrame(8);
		}

		private void motionStartFrameTextBox8_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionStartFrameTextBox8_Validating(sender, null);
			}
		}

		private void motionStartFrameTextBox9_Validating(object sender, CancelEventArgs e)
		{
			ValidateStartFrame(9);
		}

		private void motionStartFrameTextBox9_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionStartFrameTextBox9_Validating(sender, null);
			}
		}

		private bool ValidateEndFrame(Int32 index)
		{
			TimeLineData data = m_PokemonModelDataManager.AnimationTimeLine.TimeLineDataArray[index];
			decimal prevFrame = data.EndFrame;

			decimal frame = 0;
			bool isSucceeded = decimal.TryParse(m_motionEndFrameTextBoxes[index].Text, out frame);
			if (!isSucceeded)
			{
				m_motionEndFrameTextBoxes[index].Text = prevFrame.ToString();
				return false;
			}

			if (frame <= data.StartFrame)
			{
				frame = data.StartFrame + 1.0m;
			}

			if (frame >= data.FrameSize)
			{
				frame = data.FrameSize - 1.0m;
			}

      if (m_PokemonModelDataManager.AnimationTimeLine.LoopEnable)
      {
        if (frame >= m_PokemonModelDataManager.AnimationTimeLine.TimeLineDataArray[index].FrameSize - 2)
        {
          frame = m_PokemonModelDataManager.AnimationTimeLine.TimeLineDataArray[index].FrameSize - 2;
        }
      }

			m_PokemonModelDataManager.AnimationTimeLine.TimeLineDataArray[index].EndFrame = frame;
			m_PokemonModelDataManager.AnimationTimeLine.Reset();

			return true;
		}

		private void motionEndFrameTextBox0_Validating(object sender, CancelEventArgs e)
		{
			ValidateEndFrame(0);
		}

		private void motionEndFrameTextBox0_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionEndFrameTextBox0_Validating(sender, null);
			}
		}

		private void motionEndFrameTextBox1_Validating(object sender, CancelEventArgs e)
		{
			ValidateEndFrame(1);
		}

		private void motionEndFrameTextBox1_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionEndFrameTextBox1_Validating(sender, null);
			}
		}

		private void motionEndFrameTextBox2_Validating(object sender, CancelEventArgs e)
		{
			ValidateEndFrame(2);
		}

		private void motionEndFrameTextBox2_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionEndFrameTextBox2_Validating(sender, null);
			}
		}

		private void motionEndFrameTextBox3_Validating(object sender, CancelEventArgs e)
		{
			ValidateEndFrame(3);
		}

		private void motionEndFrameTextBox3_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionEndFrameTextBox3_Validating(sender, null);
			}
		}

		private void motionEndFrameTextBox4_Validating(object sender, CancelEventArgs e)
		{
			ValidateEndFrame(4);
		}

		private void motionEndFrameTextBox4_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionEndFrameTextBox4_Validating(sender, null);
			}
		}

		private void motionEndFrameTextBox5_Validating(object sender, CancelEventArgs e)
		{
			ValidateEndFrame(5);
		}

		private void motionEndFrameTextBox5_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionEndFrameTextBox5_Validating(sender, null);
			}
		}

		private void motionEndFrameTextBox6_Validating(object sender, CancelEventArgs e)
		{
			ValidateEndFrame(6);
		}

		private void motionEndFrameTextBox6_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionEndFrameTextBox6_Validating(sender, null);
			}
		}

		private void motionEndFrameTextBox7_Validating(object sender, CancelEventArgs e)
		{
			ValidateEndFrame(7);
		}

		private void motionEndFrameTextBox7_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionEndFrameTextBox7_Validating(sender, null);
			}
		}

		private void motionEndFrameTextBox8_Validating(object sender, CancelEventArgs e)
		{
			ValidateEndFrame(8);
		}

		private void motionEndFrameTextBox8_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionEndFrameTextBox8_Validating(sender, null);
			}
		}

		private void motionEndFrameTextBox9_Validating(object sender, CancelEventArgs e)
		{
			ValidateEndFrame(9);
		}

		private void motionEndFrameTextBox9_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				motionEndFrameTextBox9_Validating(sender, null);
			}
		}

		private void savePlayListButton_Click(object sender, EventArgs e)
		{
			savePlayListFileDialog.InitialDirectory = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) + "\\playlist\\";
			if (savePlayListFileDialog.ShowDialog() != System.Windows.Forms.DialogResult.OK)
			{
        return;
      }

			FileStream fileStream = new FileStream(savePlayListFileDialog.FileName, FileMode.Create);
			StreamWriter streamWriter = new StreamWriter(fileStream);

			for (Int32 i = 0; i < m_motionSelectComboBoxes.Length; ++i)
			{
				string motionText = m_motionSelectComboBoxes[i].Text;
				motionText = System.IO.Path.GetFileNameWithoutExtension(motionText);
        string trimmedMotionText = PokemonModelDataManager.s_pmXXXX_YY.Replace(motionText, string.Empty);
				string outputMotionText = "";
				if (m_motionEnabledCheckBoxes[i].Checked && !string.IsNullOrEmpty(trimmedMotionText))
				{
          outputMotionText = trimmedMotionText;
				}
				streamWriter.WriteLine(outputMotionText);
			}

			streamWriter.Close();
			fileStream.Close();
		}

		private void loadPlayListButton_Click(object sender, EventArgs e)
		{
			openPlayListFileDialog.InitialDirectory = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) + "\\playlist\\";
			if (openPlayListFileDialog.ShowDialog() != System.Windows.Forms.DialogResult.OK)
			{
				return;
			}

			FileStream fileStream = new FileStream(openPlayListFileDialog.FileName, FileMode.Open);
			StreamReader streamReader = new StreamReader(fileStream);

      Int32 index = 0;
			for (index = 0; streamReader.Peek() > 0; ++index)
			{
				// モーションの最大数を超えたらその時点で終了
				if (index >= m_motionSelectComboBoxes.Length)
				{
					break;
				}

				string motionText = streamReader.ReadLine();
				if (string.IsNullOrEmpty(motionText))
				{
					m_motionEnabledCheckBoxes[index].Checked = false;
					m_motionSelectComboBoxes[index].SelectedIndex = 0;
					continue;
				}

				Int32 itemIndex = -1;
				for (Int32 i = 0; i < m_motionSelectComboBoxes[index].Items.Count; ++i)
				{
					string motionName = m_motionSelectComboBoxes[index].Items[i].ToString();
          motionName = System.IO.Path.GetFileNameWithoutExtension(motionName);
          string trimmedMotionName = PokemonModelDataManager.s_pmXXXX_YY.Replace(motionName, string.Empty);
					if (motionText == trimmedMotionName)
					{
						itemIndex = i;
						break;
					}
				}

				m_motionEnabledCheckBoxes[index].Checked = (itemIndex >= 0);

				if (itemIndex < 0)
				{
					itemIndex = 0;		// モーション名がなかったらとりあえず0にしてインデックスを設定
				}
				m_motionSelectComboBoxes[index].SelectedIndex = itemIndex;
			}

      // モーションが設定されていたらインデックスを0に戻しておく
      for (Int32 i = index; i < m_motionSelectComboBoxes.Length; ++i)
      {
        m_motionSelectComboBoxes[i].SelectedIndex = 0;
      }
      // インデックスを0に戻した上で, ファイルに設定されていないモーションは無効化
      for (Int32 i = index; i < m_motionEnabledCheckBoxes.Length; ++i)
      {
        m_motionEnabledCheckBoxes[i].Checked = false;
      }

			streamReader.Close();
			fileStream.Close();
		}

		private void SetEnabledMotion(Int32 index, bool enabled)
		{
			if (m_PokemonModelDataManager == null)
			{
				return;
			}

			AnimationTimeLine timeLine = m_PokemonModelDataManager.AnimationTimeLine;
			TimeLineData[] dataArray = timeLine.TimeLineDataArray;
			dataArray[index].IsAvailable = enabled;

			m_motionStartFrameTextBoxes[index].Enabled = enabled;
			m_motionEndFrameTextBoxes[index].Enabled = enabled;

      if (index < m_AnimationNum - 1)
      {
        ValidateBlendFrame(index);
      }

			// モーションのブレンドを設定するTextBoxの有効・無効の設定
			Array.ForEach(m_blendFrameSizeTextBoxes, component => component.Enabled = false);
			Int32 prevIndex = -1;
			for (Int32 i = 0; i < dataArray.Length; ++i)
			{
				if (!dataArray[i].IsAvailable)
				{
					continue;
				}

				if (prevIndex >= 0)
				{
					m_blendFrameSizeTextBoxes[prevIndex].Enabled = true;
				}

				prevIndex = i;
			}

      // 最後のフレームの補間フレームを0に設定する
      if (0 <= prevIndex && prevIndex < dataArray.Length - 1)
      {
        m_blendFrameSizeTextBoxes[prevIndex].Text = "0";
        ValidateBlendFrame(prevIndex);
      }
		}

    public void SetCkeckToMotionEnabledCheckBox(Int32 index, bool check)
    {
      m_motionEnabledCheckBoxes[index].Checked = check;
    }

		private void motionEnabledCheckBox0_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledMotion(0, m_motionEnabledCheckBoxes[0].Checked);
		}

		private void motionEnabledCheckBox1_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledMotion(1, m_motionEnabledCheckBoxes[1].Checked);
		}

		private void motionEnabledCheckBox2_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledMotion(2, m_motionEnabledCheckBoxes[2].Checked);
		}

		private void motionEnabledCheckBox3_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledMotion(3, m_motionEnabledCheckBoxes[3].Checked);
		}

		private void motionEnabledCheckBox4_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledMotion(4, m_motionEnabledCheckBoxes[4].Checked);
		}

		private void motionEnabledCheckBox5_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledMotion(5, m_motionEnabledCheckBoxes[5].Checked);
		}

		private void motionEnabledCheckBox6_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledMotion(6, m_motionEnabledCheckBoxes[6].Checked);
		}

		private void motionEnabledCheckBox7_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledMotion(7, m_motionEnabledCheckBoxes[7].Checked);
		}

		private void motionEnabledCheckBox8_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledMotion(8, m_motionEnabledCheckBoxes[8].Checked);
		}

		private void motionEnabledCheckBox9_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledMotion(9, m_motionEnabledCheckBoxes[9].Checked);
		}

		private void BackwardSelectedMotion(Int32 index)
		{
			Int32 selectedIndex = m_motionSelectComboBoxes[index].SelectedIndex;
			--selectedIndex;
			if (selectedIndex < 0)
			{
				selectedIndex = m_motionSelectComboBoxes[index].Items.Count - 1;
			}
			m_motionSelectComboBoxes[index].SelectedIndex = selectedIndex;
		}

		private void prevMotionSelectButton0_Click(object sender, EventArgs e)
		{
			BackwardSelectedMotion(0);
		}

		private void prevMotionSelectButton1_Click(object sender, EventArgs e)
		{
			BackwardSelectedMotion(1);
		}

		private void prevMotionSelectButton2_Click(object sender, EventArgs e)
		{
			BackwardSelectedMotion(2);
		}

		private void prevMotionSelectButton3_Click(object sender, EventArgs e)
		{
			BackwardSelectedMotion(3);
		}

		private void prevMotionSelectButton4_Click(object sender, EventArgs e)
		{
			BackwardSelectedMotion(4);
		}

		private void prevMotionSelectButton5_Click(object sender, EventArgs e)
		{
			BackwardSelectedMotion(5);
		}

		private void prevMotionSelectButton6_Click(object sender, EventArgs e)
		{
			BackwardSelectedMotion(6);
		}

		private void prevMotionSelectButton7_Click(object sender, EventArgs e)
		{
			BackwardSelectedMotion(7);
		}

		private void prevMotionSelectButton8_Click(object sender, EventArgs e)
		{
			BackwardSelectedMotion(8);
		}

		private void prevMotionSelectButton9_Click(object sender, EventArgs e)
		{
			BackwardSelectedMotion(9);
		}

		private void ForwardSelectedMotion(Int32 index)
		{
			Int32 selectedIndex = m_motionSelectComboBoxes[index].SelectedIndex;
			++selectedIndex;
			if (selectedIndex >= m_motionSelectComboBoxes[index].Items.Count)
			{
				selectedIndex = 0;
			}
			m_motionSelectComboBoxes[index].SelectedIndex = selectedIndex;
		}

		private void nextMotionSelectButton0_Click(object sender, EventArgs e)
		{
			ForwardSelectedMotion(0);
		}

		private void nextMotionSelectButton1_Click(object sender, EventArgs e)
		{
			ForwardSelectedMotion(1);
		}

		private void nextMotionSelectButton2_Click(object sender, EventArgs e)
		{
			ForwardSelectedMotion(2);
		}

		private void nextMotionSelectButton3_Click(object sender, EventArgs e)
		{
			ForwardSelectedMotion(3);
		}

		private void nextMotionSelectButton4_Click(object sender, EventArgs e)
		{
			ForwardSelectedMotion(4);
		}

		private void nextMotionSelectButton5_Click(object sender, EventArgs e)
		{
			ForwardSelectedMotion(5);
		}

		private void nextMotionSelectButton6_Click(object sender, EventArgs e)
		{
			ForwardSelectedMotion(6);
		}

		private void nextMotionSelectButton7_Click(object sender, EventArgs e)
		{
			ForwardSelectedMotion(7);
		}

		private void nextMotionSelectButton8_Click(object sender, EventArgs e)
		{
			ForwardSelectedMotion(8);
		}

		private void nextMotionSelectButton9_Click(object sender, EventArgs e)
		{
			ForwardSelectedMotion(9);
		}

    public void SetMotion(string motion, int frame)
    {
      // 0番スロットのモーションを有効に
      SetEnabledMotion(0, true);

      // モーション設定
      SetMotionChangeString(0, motion);
      ChangeMotion(0, m_motionSelectComboBoxes[0].SelectedIndex);

      // フレーム設定
      motionStartFrameTextBox0.Text = frame.ToString();
      if (!ValidateStartFrame(0))
      {
        Console.WriteLine("error: specified frame index is out of range.");
      }
    }
	}
}
