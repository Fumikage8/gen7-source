using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using gfl2.clr.animation;
using gfl2.clr.math;

namespace ModelViewer
{
	public partial class FaceControlForm : Form, IModelMotionResourceChangeListener
	{
		private PokemonModelDataManager m_PokemonModelDataManager;  //!< @brief UI上で扱うPokeonModelDatamanager
		private const float TRANSLATE_LIMIT = 2000.0f;              //!< @brief 移動の限界値(+-TRANSLATE_LIMIT)
    private Color m_DefaultLabelColor;                          // ラベルのデフォルトカラー

		public FaceControlForm()
		{
			InitializeComponent();
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

		private void FaceControlForm_Load(object sender, EventArgs e)
		{
      m_DefaultLabelColor = eyeIndexLabel.BackColor;
		}

		private void SetEnabledComponents(bool enable)
		{
			translateXTextBox.Enabled = enable;
			translateYTextBox.Enabled = enable;
			translateZTextBox.Enabled = enable;
			translateXTrackBar.Enabled = enable;
			translateYTrackBar.Enabled = enable;
			translateZTrackBar.Enabled = enable;
		}

		/**
		 * @brief PokemonModelDataManagerの変更通知受け取り
		 * @param modelDataManager モデルデータマネージャ
		 */
		public void ReceiveNotification(PokemonModelDataManager modelDataManager)
		{
			bool enable = (modelDataManager != null);
			if (m_PokemonModelDataManager != modelDataManager)
			{// 参照するモデルデータが変わったらその都度変更
				m_PokemonModelDataManager = modelDataManager;

				interestPositionEnableCheckBox.Enabled = enable;
				SetEnabledComponents(enable && modelDataManager.IsInterestPositionControllable);
				faceLookEnableCheckBox.Enabled = enable;
				eyeIndexDecrementButton.Enabled = enable;
				eyeIndexLabel.Enabled = enable;
				eyeIndexIncrementButton.Enabled = enable;
				mouthIndexDecrementButton.Enabled = enable;
				mouthIndexLabel.Enabled = enable;
				mouthIndexIncrementButton.Enabled = enable;

				if (enable)
				{
					gfl2.clr.math.Vector translate = modelDataManager.InterestPosition;
					interestPositionEnableCheckBox.Checked = modelDataManager.IsInterestPositionControllable;
					translateXTextBox.Text = translate.x.ToString();
					translateYTextBox.Text = translate.y.ToString();
					translateZTextBox.Text = translate.z.ToString();
					translateXTrackBar.Value = (Int32)(translate.x * (float)translateXTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)translateXTrackBar.Maximum / 2.0f);
					translateYTrackBar.Value = (Int32)(translate.y * (float)translateYTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)translateYTrackBar.Maximum / 2.0f);
					translateZTrackBar.Value = (Int32)(translate.z * (float)translateZTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)translateZTrackBar.Maximum / 2.0f);
					eyeIndexLabel.Text = modelDataManager.EyeIndex.ToString();
					mouthIndexLabel.Text = modelDataManager.MouthIndex.ToString();

          // 目と口のインデックスは1～8だが, 配列のアクセスに合わせて-1で補正
          var usedEyeFlags = m_PokemonModelDataManager.FacePatternUsedFlags[PokemonModelDataManager.FacePatternType.Eye];
          eyeIndexLabel.BackColor = (usedEyeFlags[modelDataManager.EyeIndex - 1]) ? m_DefaultLabelColor : Color.Red;

          var usedMouthFlags = m_PokemonModelDataManager.FacePatternUsedFlags[PokemonModelDataManager.FacePatternType.Mouth];
          mouthIndexLabel.BackColor = (usedMouthFlags[modelDataManager.MouthIndex - 1]) ? m_DefaultLabelColor : Color.Red;
				}
				else
				{
					faceLookEnableCheckBox.Checked = false;
					eyeIndexLabel.Text = "1";
					mouthIndexLabel.Text = "1";
          eyeIndexLabel.BackColor = m_DefaultLabelColor;
          mouthIndexLabel.BackColor = m_DefaultLabelColor;
				}
			}

      // AnimationTimeLineの状態を検知してUIの有効・無効を切り替え
      bool isAnimationEnable = false;
			if (enable)
			{
        isAnimationEnable = m_PokemonModelDataManager.AnimationTimeLine.IsAnimationEnable();
			}

      if (!isAnimationEnable)
      {// アニメーションが無効だったら振り向きのチェックボックスのチェックを外す(モーションが流れていないと正常に動かないため)
        interestPositionEnableCheckBox.Checked = false;
      }

      interestPositionEnableCheckBox.Enabled = isAnimationEnable;
      faceLookEnableCheckBox.Enabled = isAnimationEnable;
      translateXTextBox.Enabled = isAnimationEnable;
      translateYTextBox.Enabled = isAnimationEnable;
      translateZTextBox.Enabled = isAnimationEnable;
      translateXTrackBar.Enabled = isAnimationEnable;
      translateYTrackBar.Enabled = isAnimationEnable;
      translateZTrackBar.Enabled = isAnimationEnable;
      eyeIndexDecrementButton.Enabled = isAnimationEnable;
      eyeIndexIncrementButton.Enabled = isAnimationEnable;
      mouthIndexDecrementButton.Enabled = isAnimationEnable;
      mouthIndexIncrementButton.Enabled = isAnimationEnable;
		}

		/**
		 * @brief Translateの異常値チェック
		 * @param strFrame 文字列で受け取ったtranslate
		 * @param[out] float 変換後のtranslate
		 * return bool 成功/失敗
		 */
		private bool ValidateInputeTranslate(string strScale, out float translate)
		{
			bool isSucceeded = float.TryParse(strScale, out translate);
			if (!isSucceeded)
			{
				return false;
			}

			if (translate < -TRANSLATE_LIMIT)
			{
				translate = -TRANSLATE_LIMIT;
			}
			else if (translate > +TRANSLATE_LIMIT)
			{
				translate = TRANSLATE_LIMIT;
			}

			return true;
		}

		/**
		 * @brief Translate(X)設定
		 * @param sender
		 * @param e
		 */
		private void translateXTextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevTranslateX = m_PokemonModelDataManager.InterestPosition.x;

			float translate = 0.0f;
			bool isSucceeded = ValidateInputeTranslate(translateXTextBox.Text, out translate);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				translateXTextBox.Text = prevTranslateX.ToString();
				return;
			}

			// 値の設定
			m_PokemonModelDataManager.InterestPosition.x = translate;
			translateXTextBox.Text = translate.ToString();
			translateXTrackBar.Value = (Int32)(translate * (float)translateXTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)translateXTrackBar.Maximum / 2.0f);
		}
		private void translateXTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				translateXTextBox_Validating(sender, null);
			}
		}

		/**
		 * @brief Translate(Y)設定
		 * @param sender
		 * @param e
		 */
		private void translateYTextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevTranslateY = m_PokemonModelDataManager.InterestPosition.y;

			float translate = 0.0f;
			bool isSucceeded = ValidateInputeTranslate(translateYTextBox.Text, out translate);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				translateYTextBox.Text = prevTranslateY.ToString();
				return;
			}

			// 値の設定
			m_PokemonModelDataManager.InterestPosition.y = translate;
			translateYTextBox.Text = translate.ToString();
			translateYTrackBar.Value = (Int32)(translate * (float)translateYTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)translateYTrackBar.Maximum / 2.0f);
		}
		private void translateYTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				translateYTextBox_Validating(sender, null);
			}
		}

		/**
		 * @brief Translate(Z)設定
		 * @param sender
		 * @param e
		 */
		private void translateZTextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevTranslateZ = m_PokemonModelDataManager.InterestPosition.z;

			float translate = 0.0f;
			bool isSucceeded = ValidateInputeTranslate(translateZTextBox.Text, out translate);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				translateZTextBox.Text = prevTranslateZ.ToString();
				return;
			}

			// 値の設定
			m_PokemonModelDataManager.InterestPosition.z = translate;
			translateZTextBox.Text = translate.ToString();
			translateZTrackBar.Value = (Int32)(translate * (float)translateZTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)translateZTrackBar.Maximum / 2.0f);
		}
		private void translateZTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				translateZTextBox_Validating(sender, null);
			}
		}

		private void translateXTrackBar_Scroll(object sender, EventArgs e)
		{
			float translate = (2.0f * TRANSLATE_LIMIT) * (float)(translateXTrackBar.Value - translateXTrackBar.Maximum / 2) / (float)translateXTrackBar.Maximum;
			translateXTextBox.Text = translate.ToString();
			translateXTextBox_Validating(sender, null);
		}

		private void translateYTrackBar_Scroll(object sender, EventArgs e)
		{
			float translate = (2.0f * TRANSLATE_LIMIT) * (float)(translateYTrackBar.Value - translateYTrackBar.Maximum / 2) / (float)translateYTrackBar.Maximum;
			translateYTextBox.Text = translate.ToString();
			translateYTextBox_Validating(sender, null);
		}

		private void translateZTrackBar_Scroll(object sender, EventArgs e)
		{
			float translate = (2.0f * TRANSLATE_LIMIT) * (float)(translateZTrackBar.Value - translateZTrackBar.Maximum / 2) / (float)translateZTrackBar.Maximum;
			translateZTextBox.Text = translate.ToString();
			translateZTextBox_Validating(sender, null);
		}

		private void interestPositionEnableCheckBox_CheckedChanged(object sender, EventArgs e)
		{
      if (m_PokemonModelDataManager == null)
      {
        return;
      }

			m_PokemonModelDataManager.IsInterestPositionControllable =
				interestPositionEnableCheckBox.Checked;
			SetEnabledComponents(interestPositionEnableCheckBox.Checked);
		}

		private void eyeIndexDecrementButton_Click(object sender, EventArgs e)
		{
			Int32 index = Int32.Parse(eyeIndexLabel.Text);
			--index;
			if (index <= 0)
			{
				index = 8;
			}
			m_PokemonModelDataManager.EyeIndex = index;
			eyeIndexLabel.Text = index.ToString();

      var usedFlags = m_PokemonModelDataManager.FacePatternUsedFlags[PokemonModelDataManager.FacePatternType.Eye];
      eyeIndexLabel.BackColor = (usedFlags[index - 1]) ? m_DefaultLabelColor : Color.Red;
		}

		private void eyeIndexIncrementButton_Click(object sender, EventArgs e)
		{
			Int32 index = Int32.Parse(eyeIndexLabel.Text);
			++index;
			if (index > 8)
			{
				index = 1;
			}
			m_PokemonModelDataManager.EyeIndex = index;
			eyeIndexLabel.Text = index.ToString();
      var usedFlags = m_PokemonModelDataManager.FacePatternUsedFlags[PokemonModelDataManager.FacePatternType.Eye];
      eyeIndexLabel.BackColor = (usedFlags[index - 1]) ? m_DefaultLabelColor : Color.Red;
		}

		private void mouthIndexDecrementButton_Click(object sender, EventArgs e)
		{
			Int32 index = Int32.Parse(mouthIndexLabel.Text);
			--index;
			if (index <= 0)
			{
				index = 8;
			}
			m_PokemonModelDataManager.MouthIndex = index;
			mouthIndexLabel.Text = index.ToString();

      var usedFlags = m_PokemonModelDataManager.FacePatternUsedFlags[PokemonModelDataManager.FacePatternType.Mouth];
      mouthIndexLabel.BackColor = (usedFlags[index - 1]) ? m_DefaultLabelColor : Color.Red;
		}

		private void mouthIndexIncrementButton_Click(object sender, EventArgs e)
		{
			Int32 index = Int32.Parse(mouthIndexLabel.Text);
			++index;
			if (index > 8)
			{
				index = 1;
			}
			m_PokemonModelDataManager.MouthIndex = index;
			mouthIndexLabel.Text = index.ToString();

      var usedFlags = m_PokemonModelDataManager.FacePatternUsedFlags[PokemonModelDataManager.FacePatternType.Mouth];
      mouthIndexLabel.BackColor = (usedFlags[index - 1]) ? m_DefaultLabelColor : Color.Red;
		}

		private void faceLookEnableCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			if (m_PokemonModelDataManager != null)
			{
				m_PokemonModelDataManager.IsFaceControlEnable = faceLookEnableCheckBox.Checked;
				eyeIndexDecrementButton.Enabled = faceLookEnableCheckBox.Checked;
				eyeIndexIncrementButton.Enabled = faceLookEnableCheckBox.Checked;
				mouthIndexDecrementButton.Enabled = faceLookEnableCheckBox.Checked;
				mouthIndexIncrementButton.Enabled = faceLookEnableCheckBox.Checked;
			}
		}
	}
}
