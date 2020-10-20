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
	public partial class TransformControlForm : Form, IModelMotionResourceChangeListener
	{
    private readonly List<PokemonModelDataManager> m_PokemonModelDataManagerList;
		private PokemonModelDataManager m_PokemonModelDataManager;		//!< @brief UI上で扱うPokeonModelDatamanager
		private const float HEIGHT_LOWER_LIMIT = 10.0f;
		private const float HEIGHT_UPPER_LIMIT = 2000.0f;
		private const float SCALE_LIMIT = HEIGHT_UPPER_LIMIT / HEIGHT_LOWER_LIMIT;	//!< @brief 拡大縮小の限界値
		private const float ROTATE_LIMIT = 180.0f;										//!< @brief 回転の限界値(+-ROTATE_LIMIT)
		private const float TRANSLATE_LIMIT = 1000.0f;								//!< @brief 移動の限界値(+-TRANSLATE_LIMIT)

    //!< @brief 補正後の高さの種類
    private enum AdjustHeightType
    {
      AdjustHeight,   //!< @brief 通常のAdjustHeight(バトルなどで使用)
      FieldAdjust     //!< @brief フィールド用のAdjustHeight
    }

    //!< @brief AdjustHeight切り替え用ラジオボタン
    private Dictionary<AdjustHeightType, RadioButton> m_AdjustHeightRadioButtonDictionary;
    //!< @brief AdjustHeight操作用UI
    private Dictionary<AdjustHeightType, Control[]> m_AdjustHeightComponentsDictionary;

		/**
		 * @brief コンストラクタ
		 */
		public TransformControlForm(List<PokemonModelDataManager> pokemonModelDataManagerList)
		{
			InitializeComponent();
      m_PokemonModelDataManagerList = pokemonModelDataManagerList;
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
		 * @brief フォーム初期化
		 * @param sender
		 * @param e
		 */
		private void TransformControlForm_Load(object sender, EventArgs e)
		{
      // 各ラジオボタンに対応したDictionaryを作成
      m_AdjustHeightRadioButtonDictionary = new Dictionary<AdjustHeightType, RadioButton>();
      m_AdjustHeightRadioButtonDictionary.Add(AdjustHeightType.AdjustHeight, this.adjustHeightRadioButton);
      m_AdjustHeightRadioButtonDictionary.Add(AdjustHeightType.FieldAdjust, this.fieldAdjustRadioButton);

      m_AdjustHeightComponentsDictionary = new Dictionary<AdjustHeightType, Control[]>();
      m_AdjustHeightComponentsDictionary.Add(
        AdjustHeightType.AdjustHeight,
        new Control[] { adjustHeightTextBox, adjustHeightTrackBar, adjustHeightResetButton }
        );
      m_AdjustHeightComponentsDictionary.Add(
        AdjustHeightType.FieldAdjust,
        new Control[] { fieldAdjustTextBox, fieldAdjustTrackBar, fieldAdjustResetButton }
        );
		}

		/**
		 * @brief PokemonModelDataManagerの変更通知受け取り
		 * @param modelDataManager モデルデータマネージャ
		 */
		public void ReceiveNotification(PokemonModelDataManager modelDataManager)
		{
			if (m_PokemonModelDataManager == modelDataManager)
			{
				return;
			}

			m_PokemonModelDataManager = modelDataManager;
			bool enable = (modelDataManager != null);
			SetEnabledComponents(enable);
			ReceiveSRTChangeNotification(modelDataManager);

      bool isHeightControlEnable = false;

			sizeCategoryLabel.Text = "";
			float cmHeight = 0.0f;
			float adjustHeight = 0.0f;
      float fieldAdjust = 0.0f;
			if (enable)
			{
				cmHeight = modelDataManager.PokemonModel.GetCmHeight();
				adjustHeight = modelDataManager.PokemonModel.GetAdjustHeight();
        fieldAdjust = modelDataManager.FieldAdjust;
				// [cm] -> [m] に変更して表記
				float mHeight = cmHeight / 100.0f;
				float mAdjustHeight = adjustHeight / 100.0f;
        float mFieldAdjust = fieldAdjust / 100.0f;
				cmHeightTextBox.Text = mHeight.ToString();
        adjustHeightTextBox.Text = mAdjustHeight.ToString();
        fieldAdjustTextBox.Text = mFieldAdjust.ToString();

				// カテゴリサイズを表示
				string[] categoryText = { "S", "M", "L" };
				sizeCategoryLabel.Text = categoryText[(Int32)modelDataManager.Size];

        isHeightControlEnable = modelDataManager.IsHeightControlEnable;
			}
			cmHeightTrackBar.Value = (Int32)((cmHeight / HEIGHT_UPPER_LIMIT) * (float)cmHeightTrackBar.Maximum);
      adjustHeightTrackBar.Value = (Int32)((adjustHeight / HEIGHT_UPPER_LIMIT) * (float)adjustHeightTrackBar.Maximum);
      fieldAdjustTrackBar.Value = (Int32)((fieldAdjust / HEIGHT_UPPER_LIMIT) * (float)fieldAdjustTrackBar.Maximum);

      controlHeightCheckBox.Enabled = enable;
      controlHeightCheckBox.Checked = isHeightControlEnable;
			controlHeightCheckBox_CheckedChanged(null, null);	// チェックボックスが変わってなかったときを考慮

      this.allScaleChangeCheckBox.Enabled = enable;
			this.scaleXTextBox.Enabled = enable;
			this.scaleYTextBox.Enabled = enable;
			this.scaleZTextBox.Enabled = enable;
			this.scaleXTrackBar.Enabled = enable;
			this.scaleYTrackBar.Enabled = enable;
			this.scaleZTrackBar.Enabled = enable;
		}

		public void SetEnabledComponents(bool enable)
		{
			cmHeightTextBox.Enabled = enable;
			cmHeightTrackBar.Enabled = enable;
			cmHeightResetButton.Enabled = enable;
			adjustHeightTextBox.Enabled = enable;
			adjustHeightTrackBar.Enabled = enable;
			adjustHeightResetButton.Enabled = enable;
			scaleXLabel.Enabled = enable;
			scaleYLabel.Enabled = enable;
			scaleZLabel.Enabled = enable;
			scaleXTextBox.Enabled = enable;
			scaleYTextBox.Enabled = enable;
			scaleZTextBox.Enabled = enable;
			scaleXTrackBar.Enabled = enable;
			scaleYTrackBar.Enabled = enable;
			scaleZTrackBar.Enabled = enable;
			rotateXLabel.Enabled = enable;
			rotateYLabel.Enabled = enable;
			rotateZLabel.Enabled = enable;
			rotateXTextBox.Enabled = enable;
			rotateYTextBox.Enabled = enable;
			rotateZTextBox.Enabled = enable;
			rotateXTrackBar.Enabled = enable;
			rotateYTrackBar.Enabled = enable;
			rotateZTrackBar.Enabled = enable;
			translateXTextBox.Enabled = enable;
			translateYTextBox.Enabled = enable;
			translateZTextBox.Enabled = enable;
			translateXTrackBar.Enabled = enable;
			translateYTrackBar.Enabled = enable;
			translateZTrackBar.Enabled = enable;
		}

		/**
		 * @brief PokemonModelDataManagerのSRT変更通知受け取り
		 * @param modelDataManager モデルデータマネージャ
		 */
		public void ReceiveSRTChangeNotification(PokemonModelDataManager modelDataManager)
		{
			if (modelDataManager == null)
			{
				return;
			}

			gfl2.clr.math.Vector scale = modelDataManager.Scale;
			scaleXTextBox.Text = scale.x.ToString();
			scaleYTextBox.Text = scale.y.ToString();
			scaleZTextBox.Text = scale.z.ToString();
			scaleXTrackBar.Value = (Int32)(scale.x * (float)scaleXTrackBar.Maximum / SCALE_LIMIT);
			scaleYTrackBar.Value = (Int32)(scale.y * (float)scaleYTrackBar.Maximum / SCALE_LIMIT);
			scaleZTrackBar.Value = (Int32)(scale.z * (float)scaleZTrackBar.Maximum / SCALE_LIMIT);
			gfl2.clr.math.Vector rotate = modelDataManager.EulerRotation;
			rotateXTextBox.Text = rotate.x.ToString();
			rotateYTextBox.Text = rotate.y.ToString();
			rotateZTextBox.Text = rotate.z.ToString();
			rotateXTrackBar.Value = (Int32)(rotate.x * (float)rotateXTrackBar.Maximum / (2.0f * ROTATE_LIMIT) + (float)rotateXTrackBar.Maximum / 2.0f);
			rotateYTrackBar.Value = (Int32)(rotate.y * (float)rotateYTrackBar.Maximum / (2.0f * ROTATE_LIMIT) + (float)rotateYTrackBar.Maximum / 2.0f);
			rotateZTrackBar.Value = (Int32)(rotate.z * (float)rotateZTrackBar.Maximum / (2.0f * ROTATE_LIMIT) + (float)rotateZTrackBar.Maximum / 2.0f);
			gfl2.clr.math.Vector translate = modelDataManager.Position;
			translateXTextBox.Text = translate.x.ToString();
			translateYTextBox.Text = translate.y.ToString();
			translateZTextBox.Text = translate.z.ToString();
			translateXTrackBar.Value = (Int32)(translate.x * (float)translateXTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)translateXTrackBar.Maximum / 2.0f);
			translateYTrackBar.Value = (Int32)(translate.y * (float)translateYTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)translateYTrackBar.Maximum / 2.0f);
			translateZTrackBar.Value = (Int32)(translate.z * (float)translateZTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)translateZTrackBar.Maximum / 2.0f);
		}

		/**
		 * @brief スケール一括調整ON/OFF
		 * @param sender
		 * @param e
		 */
		private void convertScalingAllCheckBox_CheckedChanged(object sender, EventArgs e)
		{

		}

		/**
		 * @brief Scaleの異常値チェック
		 * @param strFrame 文字列で受け取ったscale
		 * @param[out] float 変換後のscale
		 * return bool 成功/失敗
		 */
		private bool ValidateInputScale(string strScale, out float scale)
		{
			bool isSucceeded = float.TryParse(strScale, out scale);
			if (!isSucceeded)
			{
				return false;
			}

			if (scale < 0.0f)
			{
				scale = 0.0f;
			}
			else if (scale > SCALE_LIMIT)
			{
				scale = SCALE_LIMIT;
			}

			return true;
		}

    private void scaleAllTextBox_Validating(object sender, CancelEventArgs e)
    {

    }

		/**
		 * @brief Scale(X)設定
		 * @param sender
		 * @param e
		 */
    private void scaleXTextBox_Validating(object sender, CancelEventArgs e)
    {
			gfl2.clr.math.Vector prevScale = new Vector(m_PokemonModelDataManager.Scale);

			float scale = 0.0f;
			bool isSucceeded = ValidateInputScale(scaleXTextBox.Text, out scale);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				scaleXTextBox.Text = prevScale.x.ToString();
				return;
			}

			// 値の設定
			prevScale.x = scale;
			scaleXTextBox.Text = prevScale.x.ToString();
			scaleXTrackBar.Value = (Int32)(scale * (float)scaleXTrackBar.Maximum / SCALE_LIMIT);
			if (allScaleChangeCheckBox.Checked)
			{
				prevScale.y = scale;
				prevScale.z = scale;
				scaleYTextBox.Text = prevScale.y.ToString();
				scaleYTrackBar.Value = (Int32)(scale * (float)scaleYTrackBar.Maximum / SCALE_LIMIT);
				scaleZTextBox.Text = prevScale.z.ToString();
				scaleZTrackBar.Value = (Int32)(scale * (float)scaleZTrackBar.Maximum / SCALE_LIMIT);
			}

			m_PokemonModelDataManager.SpringResetRequest();
			m_PokemonModelDataManager.Scale = prevScale;
    }
    private void scaleXTextBox_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Enter)
      {
        scaleXTextBox_Validating(sender, null);
      }
    }

		/**
		 * @brief Scale(Y)設定
		 * @param sender
		 * @param e
		 */
    private void scaleYTextBox_Validating(object sender, CancelEventArgs e)
    {
			gfl2.clr.math.Vector prevScale = new Vector(m_PokemonModelDataManager.Scale);

			float scale = 0.0f;
			bool isSucceeded = ValidateInputScale(scaleYTextBox.Text, out scale);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				scaleYTextBox.Text = prevScale.y.ToString();
				return;
			}

			// 値の設定
			prevScale.y = scale;
			scaleYTextBox.Text = prevScale.y.ToString();
			scaleYTrackBar.Value = (Int32)(scale * (float)scaleYTrackBar.Maximum / SCALE_LIMIT);
			if (allScaleChangeCheckBox.Checked)
			{
				prevScale.x = scale;
				prevScale.z = scale;
				scaleXTextBox.Text = prevScale.x.ToString();
				scaleXTrackBar.Value = (Int32)(scale * (float)scaleXTrackBar.Maximum / SCALE_LIMIT);
				scaleZTextBox.Text = prevScale.z.ToString();
				scaleZTrackBar.Value = (Int32)(scale * (float)scaleZTrackBar.Maximum / SCALE_LIMIT);
			}

			m_PokemonModelDataManager.SpringResetRequest();
			m_PokemonModelDataManager.Scale = prevScale;
    }
		private void scaleYTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				scaleYTextBox_Validating(sender, null);
			}
		}

		/**
		 * @brief Scale(Z)設定
		 * @param sender
		 * @param e
		 */
    private void scaleZTextBox_Validating(object sender, CancelEventArgs e)
    {
			gfl2.clr.math.Vector prevScale = new Vector(m_PokemonModelDataManager.Scale);

			float scale = 0.0f;
			bool isSucceeded = ValidateInputScale(scaleZTextBox.Text, out scale);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				scaleZTextBox.Text = prevScale.z.ToString();
				return;
			}

			// 値の設定
			prevScale.z = scale;
			scaleZTextBox.Text = prevScale.z.ToString();
			scaleZTrackBar.Value = (Int32)(scale * (float)scaleZTrackBar.Maximum / SCALE_LIMIT);
			if (allScaleChangeCheckBox.Checked)
			{
				prevScale.x = scale;
				prevScale.y = scale;
				scaleXTextBox.Text = prevScale.x.ToString();
				scaleXTrackBar.Value = (Int32)(scale * (float)scaleXTrackBar.Maximum / SCALE_LIMIT);
				scaleYTextBox.Text = prevScale.y.ToString();
				scaleYTrackBar.Value = (Int32)(scale * (float)scaleYTrackBar.Maximum / SCALE_LIMIT);
			}
			m_PokemonModelDataManager.SpringResetRequest();
			m_PokemonModelDataManager.Scale = prevScale;
    }
    private void scaleZTextBox_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Enter)
      {
        scaleZTextBox_Validating(sender, null);
      }
    }

		/**
		 * @brief Rotateの異常値チェック
		 * @param strFrame 文字列で受け取ったrotate
		 * @param[out] float 変換後のrotate
		 * return bool 成功/失敗
		 */
		private bool ValidateInputRotate(string strRotate, out float rotate)
		{
			bool isSucceeded = float.TryParse(strRotate, out rotate);
			if (!isSucceeded)
			{
				return false;
			}

			if (rotate < -ROTATE_LIMIT)
			{
				rotate = -ROTATE_LIMIT;
			}
			else if (rotate > +ROTATE_LIMIT)
			{
				rotate = ROTATE_LIMIT;
			}

			return true;
		}

		/**
		 * @brief Rotate(X)設定
		 * @param sender
		 * @param e
		 */
    private void rotateXTextBox_Validating(object sender, CancelEventArgs e)
    {
			gfl2.clr.math.Vector prevRotate = new gfl2.clr.math.Vector(m_PokemonModelDataManager.EulerRotation);

			float rotateX = 0.0f;
			bool isSucceeded = ValidateInputRotate(rotateXTextBox.Text, out rotateX);
			
			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				rotateXTextBox.Text = prevRotate.x.ToString();
				return;
			}

			// 値の設定
			gfl2.clr.math.Vector rotate = new gfl2.clr.math.Vector(rotateX, prevRotate.y, prevRotate.z);
			rotate.w = 1.0f;
			m_PokemonModelDataManager.SpringResetRequest();
			m_PokemonModelDataManager.EulerRotation = rotate;
			rotateXTextBox.Text = rotateX.ToString();
			rotateXTrackBar.Value = (Int32)(rotateX * (float)rotateXTrackBar.Maximum / (2.0f * ROTATE_LIMIT)  + (float)rotateXTrackBar.Maximum / 2.0f);
    }
    private void rotateXTextBox_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Enter)
      {
        rotateXTextBox_Validating(sender, null);
      }
    }


		/**
		 * @brief Rotate(Y)設定
		 * @param sender
		 * @param e
		 */
    private void rotateYTextBox_Validating(object sender, CancelEventArgs e)
    {
			gfl2.clr.math.Vector prevRotate = new gfl2.clr.math.Vector(m_PokemonModelDataManager.EulerRotation);

			float rotateY = 0.0f;
			bool isSucceeded = ValidateInputRotate(rotateYTextBox.Text, out rotateY);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				rotateYTextBox.Text = prevRotate.y.ToString();
				return;
			}

			// 値の設定
			gfl2.clr.math.Vector rotate = new gfl2.clr.math.Vector(prevRotate.x, rotateY, prevRotate.z);
			m_PokemonModelDataManager.SpringResetRequest();
			m_PokemonModelDataManager.EulerRotation = rotate;
			rotateYTextBox.Text = rotateY.ToString();
			rotateYTrackBar.Value = (Int32)(rotateY * (float)rotateYTrackBar.Maximum / (2.0f * ROTATE_LIMIT) + (float)rotateYTrackBar.Maximum / 2.0f);
    }
    private void rotateYTextBox_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Enter)
      {
        rotateYTextBox_Validating(sender, null);
      }
    }

		/**
		 * @brief Rotate(Z)設定
		 * @param sender
		 * @param e
		 */
    private void rotateZTextBox_Validating(object sender, CancelEventArgs e)
    {
			gfl2.clr.math.Vector prevRotate = new gfl2.clr.math.Vector(m_PokemonModelDataManager.EulerRotation);

			float rotateZ = 0.0f;
			bool isSucceeded = ValidateInputRotate(rotateZTextBox.Text, out rotateZ);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				rotateZTextBox.Text = prevRotate.z.ToString();
				return;
			}

			// 値の設定
			gfl2.clr.math.Vector rotate = new gfl2.clr.math.Vector(prevRotate.x, prevRotate.y, rotateZ);
			rotate.w = 1.0f;
			m_PokemonModelDataManager.SpringResetRequest();
			m_PokemonModelDataManager.EulerRotation = rotate;
			rotateZTextBox.Text = rotateZ.ToString();
			rotateZTrackBar.Value = (Int32)(rotateZ * (float)rotateZTrackBar.Maximum / (2.0f * ROTATE_LIMIT) + (float)rotateZTrackBar.Maximum / 2.0f);
    }
    private void rotateZTextBox_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Enter)
      {
        rotateZTextBox_Validating(sender, null);
      }
    }

		/**
		 * @brief Translateの異常値チェック
		 * @param strFrame 文字列で受け取ったtranslate
		 * @param[out] float 変換後のtranslate
		 * return bool 成功/失敗
		 */
		private bool ValidateInputTranslate(string strScale, out float translate)
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
			gfl2.clr.math.Vector prevTranslate = new Vector(m_PokemonModelDataManager.Position);

			float translate = 0.0f;
			bool isSucceeded = ValidateInputTranslate(translateXTextBox.Text, out translate);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				translateXTextBox.Text = prevTranslate.x.ToString();
				return;
			}

			// 値の設定
			prevTranslate.x = translate;
			m_PokemonModelDataManager.SpringResetRequest();
			m_PokemonModelDataManager.Position = prevTranslate;
			translateXTextBox.Text = prevTranslate.x.ToString();
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
			gfl2.clr.math.Vector prevTranslate = new Vector(m_PokemonModelDataManager.Position);

			float translate = 0.0f;
			bool isSucceeded = ValidateInputTranslate(translateYTextBox.Text, out translate);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				translateYTextBox.Text = prevTranslate.y.ToString();
				return;
			}

			// 値の設定
			prevTranslate.y = translate;
			m_PokemonModelDataManager.SpringResetRequest();
			m_PokemonModelDataManager.Position = prevTranslate;
			translateYTextBox.Text = prevTranslate.y.ToString();
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
			gfl2.clr.math.Vector prevTranslate = new Vector(m_PokemonModelDataManager.Position);

			float translate = 0.0f;
			bool isSucceeded = ValidateInputTranslate(translateZTextBox.Text, out translate);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				translateZTextBox.Text = prevTranslate.z.ToString();
				return;
			}

			// 値の設定
			prevTranslate.z = translate;
			m_PokemonModelDataManager.SpringResetRequest();
			m_PokemonModelDataManager.Position = prevTranslate;
			translateZTextBox.Text = prevTranslate.z.ToString();
			translateZTrackBar.Value = (Int32)(translate * (float)translateZTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)translateZTrackBar.Maximum / 2.0f);
    }
    private void translateZTextBox_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Enter)
      {
        translateZTextBox_Validating(sender, null);
      }
    }

		/**
		 * @brief Heightの異常値チェック
		 * @param strFrame 文字列で受け取ったheight
		 * @param[out] float 変換後のheight
		 * return bool 成功/失敗
		 */
		private bool ValidateInputHeight(string strHeight, out float cmHeight)
		{
			// テキストボックスから受け取る値はメートル[m]なので,
			// センチメートル[cm]に変換する
			bool isSucceeded = float.TryParse(strHeight, out cmHeight);
			if (!isSucceeded)
			{
				return false;
			}

			// [m] -> [cm]に変換
			cmHeight *= 100.0f;

			if (cmHeight < HEIGHT_LOWER_LIMIT)
			{
				cmHeight = HEIGHT_LOWER_LIMIT;
			}
			else if (cmHeight > HEIGHT_UPPER_LIMIT)
			{
				cmHeight = HEIGHT_UPPER_LIMIT;
			}

			return true;
		}

		private void cmHeightTrackBar_Scroll(object sender, EventArgs e)
		{
			if (cmHeightTrackBar.Value < 1)
			{
				cmHeightTrackBar.Value = 1;
			}

			float cmHeight = HEIGHT_UPPER_LIMIT * (float)cmHeightTrackBar.Value / (float)cmHeightTrackBar.Maximum;
			// [cm] -> [m]に変更
			float mHeight = cmHeight / 100.0f;
			cmHeightTextBox.Text = mHeight.ToString();
			cmHeightTextBox_Validating(sender, null);
		}

		private void adjustHeightTrackBar_Scroll(object sender, EventArgs e)
		{
			if (adjustHeightTrackBar.Value < 1)
			{
				adjustHeightTrackBar.Value = 1;
			}

			float height = HEIGHT_UPPER_LIMIT * (float)adjustHeightTrackBar.Value / (float)adjustHeightTrackBar.Maximum;
			// [cm] -> [m]に変更
			float mAdjustHeight = height / 100.0f;
			adjustHeightTextBox.Text = mAdjustHeight.ToString();
			adjustHeightTextBox_Validating(sender, null);
		}

    private void fieldAdjustTrackBar_Scroll(object sender, EventArgs e)
    {
      if (fieldAdjustTrackBar.Value < 1)
      {
        fieldAdjustTrackBar.Value = 1;
      }

      float height = HEIGHT_UPPER_LIMIT * (float)fieldAdjustTrackBar.Value / (float)fieldAdjustTrackBar.Maximum;
      // [cm] -> [m]に変更
      float mFieldAdjust = height / 100.0f;
      fieldAdjustTextBox.Text = mFieldAdjust.ToString();
      fieldAdjustTextBox_Validating(sender, null);
    }

		private void scaleXTrackBar_Scroll(object sender, EventArgs e)
		{
			float scale = SCALE_LIMIT * (float)scaleXTrackBar.Value / (float)scaleXTrackBar.Maximum;
			scaleXTextBox.Text = scale.ToString();
			scaleXTextBox_Validating(sender, null);
		}

		private void scaleYTrackBar_Scroll(object sender, EventArgs e)
		{
			float scale = SCALE_LIMIT * (float)scaleYTrackBar.Value / (float)scaleYTrackBar.Maximum;
			scaleYTextBox.Text = scale.ToString();
			scaleYTextBox_Validating(sender, null);
		}

		private void scaleZTrackBar_Scroll(object sender, EventArgs e)
		{
			float scale = SCALE_LIMIT * (float)scaleZTrackBar.Value / (float)scaleZTrackBar.Maximum;
			scaleZTextBox.Text = scale.ToString();
			scaleZTextBox_Validating(sender, null);
		}

		private void rotateXTrackBar_Scroll(object sender, EventArgs e)
		{
			float rotate = (2.0f * ROTATE_LIMIT) * (float)(rotateXTrackBar.Value - rotateXTrackBar.Maximum / 2) / (float)rotateXTrackBar.Maximum;
			rotateXTextBox.Text = rotate.ToString();
			rotateXTextBox_Validating(sender, null);
		}

		private void rotateYTrackBar_Scroll(object sender, EventArgs e)
		{
			float rotate = (2.0f * ROTATE_LIMIT) * (float)(rotateYTrackBar.Value - rotateYTrackBar.Maximum / 2) / (float)rotateYTrackBar.Maximum;
			rotateYTextBox.Text = rotate.ToString();
			rotateYTextBox_Validating(sender, null);
		}

		private void rotateZTrackBar_Scroll(object sender, EventArgs e)
		{
			float rotate = (2.0f * ROTATE_LIMIT) * (float)(rotateZTrackBar.Value - rotateZTrackBar.Maximum / 2) / (float)rotateZTrackBar.Maximum;
			rotateZTextBox.Text = rotate.ToString();
			rotateZTextBox_Validating(sender, null);
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

		private void controlHeightCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			bool enable = controlHeightCheckBox.Checked;
			cmHeightTextBox.Enabled = enable;
			cmHeightTrackBar.Enabled = enable;
			adjustHeightTextBox.Enabled = enable;
			adjustHeightTrackBar.Enabled = enable;
      if (enable)
      {// 「高さ調整」が有効の場合は、AdjustHeightとFieldAdjustの有効なほうを選ぶ
        foreach (var dictionary in m_AdjustHeightRadioButtonDictionary)
        {
          dictionary.Value.Enabled = true;      // ラジオボタンを有効化
          Array.ForEach(m_AdjustHeightComponentsDictionary[dictionary.Key],
            control => control.Enabled = dictionary.Value.Checked);
        }

        // AdjustHeightのラジオボタンにひとつもチェックが付いていなかったらチェックを入れる
        if (m_AdjustHeightRadioButtonDictionary.All(data => data.Value.Checked == false))
        {
          m_AdjustHeightRadioButtonDictionary[AdjustHeightType.AdjustHeight].Checked = true;
        }
      }
      else
      {
        foreach (var dictionary in m_AdjustHeightRadioButtonDictionary)
        {
          dictionary.Value.Enabled = false;
        }

        foreach (var dictionary in m_AdjustHeightComponentsDictionary)
        {
          Array.ForEach(dictionary.Value, control => control.Enabled = false);
        }
      }

			// adjustHeightが操作できるようになってるなら, 通常のスケール操作はできないようにする
			allScaleChangeCheckBox.Enabled = !enable;
			scaleXTextBox.Enabled = !enable;
			scaleXTrackBar.Enabled = !enable;
			scaleYTextBox.Enabled = !enable;
			scaleYTrackBar.Enabled = !enable;
			scaleZTextBox.Enabled = !enable;
			scaleZTrackBar.Enabled = !enable;
			// スケール一括調整は誤作動防止のため外しておく
			allScaleChangeCheckBox.Checked = false;

			if (enable)
			{
				cmHeightTextBox_Validating(null, null);
			}

      if (m_PokemonModelDataManager != null)
      {// モデルが選ばれていないときに呼び出される可能性があるのでnullチェック
        m_PokemonModelDataManager.IsHeightControlEnable = enable;
      }
		}

		private void cmHeightTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				cmHeightTextBox_Validating(sender, null);
			}
		}

		private void cmHeightTextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevCmHeight = m_PokemonModelDataManager.PokemonModel.GetCmHeight();

			float cmHeight = 0.0f;
			bool isSucceeded = ValidateInputHeight(cmHeightTextBox.Text, out cmHeight);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				// [cm] -> [m]に直す
				float prevMHeight = prevCmHeight / 100.0f;
				cmHeightTextBox.Text = prevMHeight.ToString();
				return;
			}

			if (cmHeight < HEIGHT_LOWER_LIMIT)
			{
				cmHeight = HEIGHT_LOWER_LIMIT;
			}
			else if (cmHeight > HEIGHT_UPPER_LIMIT)
			{
				cmHeight = HEIGHT_UPPER_LIMIT;
			}

			// [cm] -> [m]に直す
			decimal mHeight = (decimal)cmHeight / 100.0m;
      cmHeight = (float)(mHeight * 100.0m);
			cmHeightTextBox.Text = mHeight.ToString();
			cmHeightTrackBar.Value = (Int32)(cmHeight / HEIGHT_UPPER_LIMIT * (float)cmHeightTrackBar.Maximum);
			float adjustHeight = m_PokemonModelDataManager.PokemonModel.GetAdjustHeight();
      m_PokemonModelDataManager.PokemonModel.SetHeight(cmHeight, adjustHeight);
      float scale = m_PokemonModelDataManager.PokemonModel.CalculateAdjustScale();
      m_PokemonModelDataManager.Scale = new Vector(scale, scale, scale);
      float truncateScale = MathUtil.Truncate(scale, 2);
      scaleXTextBox.Text = truncateScale.ToString();
      scaleYTextBox.Text = truncateScale.ToString();
      scaleZTextBox.Text = truncateScale.ToString();
		}

		private void adjustHeightTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				adjustHeightTextBox_Validating(sender, null);
			}
		}

		private void adjustHeightTextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevCmAdjustHeight = m_PokemonModelDataManager.PokemonModel.GetAdjustHeight();

			float adjustHeight = 0.0f;
			bool isSucceeded = ValidateInputHeight(adjustHeightTextBox.Text, out adjustHeight);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				float prevMAdjustHeight = prevCmAdjustHeight / 100.0f;
				adjustHeightTextBox.Text = prevMAdjustHeight.ToString();
				return;
			}

			if (adjustHeight < HEIGHT_LOWER_LIMIT)
			{
				adjustHeight = HEIGHT_LOWER_LIMIT;
			}
			else if (adjustHeight > HEIGHT_UPPER_LIMIT)
			{
				adjustHeight = HEIGHT_UPPER_LIMIT;
			}

			decimal mAdjustHeight = (decimal)adjustHeight / 100.0m;
      adjustHeight = (float)(mAdjustHeight * 100.0m); 
			adjustHeightTextBox.Text = mAdjustHeight.ToString();
			adjustHeightTrackBar.Value = (Int32)(adjustHeight / HEIGHT_UPPER_LIMIT * (float)adjustHeightTrackBar.Maximum);
      float cmHeight = m_PokemonModelDataManager.PokemonModel.GetCmHeight();
      m_PokemonModelDataManager.PokemonModel.SetHeight(cmHeight, adjustHeight);
      float scale = m_PokemonModelDataManager.PokemonModel.CalculateAdjustScale();
      m_PokemonModelDataManager.Scale = new Vector(scale, scale, scale);
      float truncateScale = MathUtil.Truncate(scale, 2);
      scaleXTextBox.Text = truncateScale.ToString();
      scaleYTextBox.Text = truncateScale.ToString();
      scaleZTextBox.Text = truncateScale.ToString();
		}

    private void fieldAdjustTextBox_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Enter)
      {
        fieldAdjustTextBox_Validating(sender, null);
      }
    }

    private void fieldAdjustTextBox_Validating(object sender, CancelEventArgs e)
    {
      float prevCmAdjustHeight = m_PokemonModelDataManager.PokemonModel.GetAdjustHeight();

      float adjustHeight = 0.0f;
      bool isSucceeded = ValidateInputHeight(fieldAdjustTextBox.Text, out adjustHeight);

      // @todo: 失敗してたら設定前のスケール値に戻す
      if (!isSucceeded)
      {
        float prevMAdjustHeight = prevCmAdjustHeight / 100.0f;
        fieldAdjustTextBox.Text = prevMAdjustHeight.ToString();
        return;
      }

      if (adjustHeight < HEIGHT_LOWER_LIMIT)
      {
        adjustHeight = HEIGHT_LOWER_LIMIT;
      }
      else if (adjustHeight > HEIGHT_UPPER_LIMIT)
      {
        adjustHeight = HEIGHT_UPPER_LIMIT;
      }

      decimal mAdjustHeight = (decimal)adjustHeight / 100.0m;
      adjustHeight = (float)(mAdjustHeight * 100.0m);
      fieldAdjustTextBox.Text = mAdjustHeight.ToString();
      fieldAdjustTrackBar.Value = (Int32)(adjustHeight / HEIGHT_UPPER_LIMIT * (float)adjustHeightTrackBar.Maximum);
      float cmHeight = m_PokemonModelDataManager.PokemonModel.GetCmHeight();
      m_PokemonModelDataManager.PokemonModel.SetHeight(cmHeight, adjustHeight);
      float scale = m_PokemonModelDataManager.PokemonModel.CalculateAdjustScale();
      m_PokemonModelDataManager.Scale = new Vector(scale, scale, scale);
      float truncateScale = MathUtil.Truncate(scale, 2);
      scaleXTextBox.Text = truncateScale.ToString();
      scaleYTextBox.Text = truncateScale.ToString();
      scaleZTextBox.Text = truncateScale.ToString();
      m_PokemonModelDataManager.FieldAdjust = adjustHeight;
    }

		private void allScaleChangeCheckBox_CheckedChanged(object sender, EventArgs e)
		{

		}

		private void cmHeightResetButton_Click(object sender, EventArgs e)
		{
			cmHeightTextBox.Text = (m_PokemonModelDataManager.DefaultCmHeight / 100.0f).ToString();
			cmHeightTextBox_Validating(null, null);
		}

		private void adjustHeightResetButton_Click(object sender, EventArgs e)
		{
			adjustHeightTextBox.Text = (m_PokemonModelDataManager.DefaultAdjustHeight / 100.0f).ToString();
			adjustHeightTextBox_Validating(null, null);
		}

    private void fieldAdjustResetButton_Click(object sender, EventArgs e)
    {
      fieldAdjustTextBox.Text = (m_PokemonModelDataManager.DefaultFieldAdjust / 100.0f).ToString();
      fieldAdjustTextBox_Validating(null, null);
    }

    private void adjustHeightRadioButton_CheckedChanged(object sender, EventArgs e)
    {
      Array.ForEach(
        m_AdjustHeightComponentsDictionary[AdjustHeightType.AdjustHeight],
        control => control.Enabled = m_AdjustHeightRadioButtonDictionary[AdjustHeightType.AdjustHeight].Checked);
      adjustHeightTextBox_Validating(null, null);
    }

    private void fieldAdjustRadioButton_CheckedChanged(object sender, EventArgs e)
    {
      Array.ForEach(
        m_AdjustHeightComponentsDictionary[AdjustHeightType.FieldAdjust],
        control => control.Enabled = m_AdjustHeightRadioButtonDictionary[AdjustHeightType.FieldAdjust].Checked);
      fieldAdjustTextBox_Validating(null, null);
    }

    private void moveAllModelsToOriginButton_Click(object sender, EventArgs e)
    {
      if (m_PokemonModelDataManagerList == null)
      {
        return;
      }

      m_PokemonModelDataManagerList.ForEach(data => data.Position = Vector.GetZero());
    }

    private void lineUpAllModelsButton_Click(object sender, EventArgs e)
    {
      if (m_PokemonModelDataManagerList == null)
      {
        return;
      }

      for (var i = 0; i < m_PokemonModelDataManagerList.Count; ++i)
      {
        m_PokemonModelDataManagerList[i].Position.x = i * 100.0f;
        m_PokemonModelDataManagerList[i].Position.y = 0.0f;
        m_PokemonModelDataManagerList[i].Position.z = 0.0f;
      }
    }
	}
}
