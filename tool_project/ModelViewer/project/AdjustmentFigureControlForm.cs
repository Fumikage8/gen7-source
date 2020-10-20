using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using gfl2.clr.math;
using gfl2.clr.renderingengine;

namespace ModelViewer
{
	public partial class AdjustmentFigureControlForm : Form, IModelMotionResourceChangeListener
	{
		private PokemonModelDataManager m_ModelDataManager;
		private readonly RadioButton[] m_PokemonSizeRadioButtons;
		private const float TRANSLATE_LIMIT = 1000.0f;								//!< @brief 移動の限界値(+-TRANSLATE_LIMIT)
		private const float DIVISION = 10.0f;

		public AdjustmentFigureControlForm()
		{
			InitializeComponent();
			m_PokemonSizeRadioButtons = new RadioButton[(Int32)PokemonSize.Enum.NumberOf];
			m_PokemonSizeRadioButtons[(Int32)PokemonSize.Enum.S] = pokemonSizeSRadioButton;
			m_PokemonSizeRadioButtons[(Int32)PokemonSize.Enum.M] = pokemonSizeMRadioButton;
			m_PokemonSizeRadioButtons[(Int32)PokemonSize.Enum.L] = pokemonSizeLRadioButton;
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

		public void ReceiveNotification(PokemonModelDataManager modelDataManager)
		{
			if (m_ModelDataManager == modelDataManager)
			{
				return;
			}

			m_ModelDataManager = modelDataManager;
			bool enable = (m_ModelDataManager != null);
			innerTubeVisibleCheckBox.Enabled = enable;
			outerTubeVisibleCheckBox.Enabled = enable;
			modelAABBVisibleCheckBox.Enabled = enable;
			Array.ForEach(m_PokemonSizeRadioButtons, button => button.Enabled = enable);
			if (!enable)
			{
				m_PokemonSizeRadioButtons[0].Checked = true;
				innerColorRTrackBar.Value = 0;
				innerColorGTrackBar.Value = 0;
				innerColorBTrackBar.Value = 0;
				innerColorATrackBar.Value = 0;
				innerColorRTextBox.Text = "";
				innerColorGTextBox.Text = "";
				innerColorBTextBox.Text = "";
				innerColorATextBox.Text = "";
				innerTubeVisibleCheckBox.Checked = false;
				outerColorRTrackBar.Value = 0;
				outerColorGTrackBar.Value = 0;
				outerColorBTrackBar.Value = 0;
				outerColorATrackBar.Value = 0;
				outerColorRTextBox.Text = "";
				outerColorGTextBox.Text = "";
				outerColorBTextBox.Text = "";
				outerColorATextBox.Text = "";
				outerTubeVisibleCheckBox.Checked = false;

				minXTrackBar.Value = 0;
				minYTrackBar.Value = 0;
				minZTrackBar.Value = 0;
				minXTextBox.Text = "";
				minYTextBox.Text = "";
				minZTextBox.Text = "";
				maxXTrackBar.Value = 0;
				maxYTrackBar.Value = 0;
				maxZTrackBar.Value = 0;
				maxXTextBox.Text = "";
				maxYTextBox.Text = "";
				maxZTextBox.Text = "";
				modelAABBVisibleCheckBox.Checked = false;

				return;
			}
			m_PokemonSizeRadioButtons[(Int32)m_ModelDataManager.Size].Checked = true;
			innerColorRTrackBar.Value = (Int32)(DIVISION * m_ModelDataManager.InnerTubeColor.x);
			innerColorGTrackBar.Value = (Int32)(DIVISION * m_ModelDataManager.InnerTubeColor.y);
			innerColorBTrackBar.Value = (Int32)(DIVISION * m_ModelDataManager.InnerTubeColor.z);
			innerColorATrackBar.Value = (Int32)(DIVISION * m_ModelDataManager.InnerTubeColor.w);
			innerColorRTextBox.Text = m_ModelDataManager.InnerTubeColor.x.ToString();
			innerColorGTextBox.Text = m_ModelDataManager.InnerTubeColor.y.ToString();
			innerColorBTextBox.Text = m_ModelDataManager.InnerTubeColor.z.ToString();
			innerColorATextBox.Text = m_ModelDataManager.InnerTubeColor.w.ToString();
			innerTubeVisibleCheckBox.Checked = modelDataManager.IsInnerTubeVisible;

			outerColorRTrackBar.Value = (Int32)(DIVISION * m_ModelDataManager.OuterTubeColor.x);
			outerColorGTrackBar.Value = (Int32)(DIVISION * m_ModelDataManager.OuterTubeColor.y);
			outerColorBTrackBar.Value = (Int32)(DIVISION * m_ModelDataManager.OuterTubeColor.z);
			outerColorATrackBar.Value = (Int32)(DIVISION * m_ModelDataManager.OuterTubeColor.w);
			outerColorRTextBox.Text = m_ModelDataManager.OuterTubeColor.x.ToString();
			outerColorGTextBox.Text = m_ModelDataManager.OuterTubeColor.y.ToString();
			outerColorBTextBox.Text = m_ModelDataManager.OuterTubeColor.z.ToString();
			outerColorATextBox.Text = m_ModelDataManager.OuterTubeColor.w.ToString();
			outerTubeVisibleCheckBox.Checked = modelDataManager.IsOuterTubeVisible;

			SetAABBValue();
		}

		private void SetAABBValue()
		{
			AABB aabb = m_ModelDataManager.PokemonModel.GetModelAABB();
			Vector min = aabb.GetMin();
			Vector max = aabb.GetMax();
			minXTextBox.Text = min.x.ToString();
			minYTextBox.Text = min.y.ToString();
			minZTextBox.Text = min.z.ToString();
			minXTrackBar.Value = (Int32)(min.x * (float)minXTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)minXTrackBar.Maximum / 2.0f);
			minYTrackBar.Value = (Int32)(min.y * (float)minYTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)minYTrackBar.Maximum / 2.0f);
			minZTrackBar.Value = (Int32)(min.z * (float)minZTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)minZTrackBar.Maximum / 2.0f);
			maxXTextBox.Text = max.x.ToString();
			maxYTextBox.Text = max.y.ToString();
			maxZTextBox.Text = max.z.ToString();
			maxXTrackBar.Value = (Int32)(max.x * (float)maxXTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)maxXTrackBar.Maximum / 2.0f);
			maxYTrackBar.Value = (Int32)(max.y * (float)maxYTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)maxYTrackBar.Maximum / 2.0f);
			maxZTrackBar.Value = (Int32)(max.z * (float)maxZTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)maxZTrackBar.Maximum / 2.0f);
		}

		private void innerCylinderVisibleCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			bool enable = innerTubeVisibleCheckBox.Checked;
			if (m_ModelDataManager != null)
			{
				m_ModelDataManager.IsInnerTubeVisible = enable;
			}

			innerColorRTextBox.Enabled = enable;
			innerColorGTextBox.Enabled = enable;
			innerColorBTextBox.Enabled = enable;
			innerColorATextBox.Enabled = enable;
			innerColorRTrackBar.Enabled = enable;
			innerColorGTrackBar.Enabled = enable;
			innerColorBTrackBar.Enabled = enable;
			innerColorATrackBar.Enabled = enable;
		}

		private void outerCylinderVisibleCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			bool enable = outerTubeVisibleCheckBox.Checked;
			if (m_ModelDataManager != null)
			{
				m_ModelDataManager.IsOuterTubeVisible = enable;
			}

			outerColorRTextBox.Enabled = enable;
			outerColorGTextBox.Enabled = enable;
			outerColorBTextBox.Enabled = enable;
			outerColorATextBox.Enabled = enable;
			outerColorRTrackBar.Enabled = enable;
			outerColorGTrackBar.Enabled = enable;
			outerColorBTrackBar.Enabled = enable;
			outerColorATrackBar.Enabled = enable;
		}

		private void innerColorRTrackBar_Scroll(object sender, EventArgs e)
		{
			innerColorRTextBox.Text = ((float)(innerColorRTrackBar.Value) / DIVISION).ToString();
			innerColorRTextBox_Validating(sender, null);
		}

		private void innerColorGTrackBar_Scroll(object sender, EventArgs e)
		{
			innerColorGTextBox.Text = ((float)(innerColorGTrackBar.Value) / DIVISION).ToString();
			innerColorGTextBox_Validating(sender, null);
		}

		private void innerColorBTrackBar_Scroll(object sender, EventArgs e)
		{
			innerColorBTextBox.Text = ((float)(innerColorBTrackBar.Value) / DIVISION).ToString();
			innerColorBTextBox_Validating(sender, null);
		}

		private void innerColorATrackBar_Scroll(object sender, EventArgs e)
		{
			innerColorATextBox.Text = ((float)(innerColorATrackBar.Value) / DIVISION).ToString();
			innerColorATextBox_Validating(sender, null);
		}

		private void outerColorRTrackBar_Scroll(object sender, EventArgs e)
		{
			outerColorRTextBox.Text = ((float)(outerColorRTrackBar.Value) / DIVISION).ToString();
			outerColorRTextBox_Validating(sender, null);
		}

		private void outerColorGTrackBar_Scroll(object sender, EventArgs e)
		{
			outerColorGTextBox.Text = ((float)(outerColorGTrackBar.Value) / DIVISION).ToString();
			outerColorGTextBox_Validating(sender, null);
		}

		private void outerColorBTrackBar_Scroll(object sender, EventArgs e)
		{
			outerColorBTextBox.Text = ((float)(outerColorBTrackBar.Value) / DIVISION).ToString();
			outerColorBTextBox_Validating(sender, null);
		}

		private void outerColorATrackBar_Scroll(object sender, EventArgs e)
		{
			outerColorATextBox.Text = ((float)(outerColorATrackBar.Value) / DIVISION).ToString();
			outerColorATextBox_Validating(sender, null);
		}

		private bool ValidateInputColor(string strColor, out float color)
		{
			bool isSucceeded = float.TryParse(strColor, out color);
			if (!isSucceeded)
			{
				return false;
			}

			if (color < 0.0f)
			{
				color = 0.0f;
			}
			else if (color > 1.0f)
			{
				color = 1.0f;
			}

			return true;
		}

		private void innerColorRTextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevColor = m_ModelDataManager.InnerTubeColor.x;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(innerColorRTextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				innerColorRTextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			m_ModelDataManager.InnerTubeColor.x = color;
			innerColorRTextBox.Text = color.ToString();
			innerColorRTrackBar.Value = (Int32)(color * DIVISION);
		}

		private void innerColorGTextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevColor = m_ModelDataManager.InnerTubeColor.y;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(innerColorGTextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				innerColorGTextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			m_ModelDataManager.InnerTubeColor.y = color;
			innerColorGTextBox.Text = color.ToString();
			innerColorGTrackBar.Value = (Int32)(color * DIVISION);
		}

		private void innerColorBTextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevColor = m_ModelDataManager.InnerTubeColor.z;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(innerColorBTextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				innerColorBTextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			m_ModelDataManager.InnerTubeColor.z = color;
			innerColorBTextBox.Text = color.ToString();
			innerColorBTrackBar.Value = (Int32)(color * DIVISION);
		}

		private void innerColorATextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevColor = m_ModelDataManager.InnerTubeColor.w;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(innerColorATextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				innerColorATextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			m_ModelDataManager.InnerTubeColor.w = color;
			innerColorATextBox.Text = color.ToString();
			innerColorATrackBar.Value = (Int32)(color * DIVISION);
		}

		

		private void outerColorRTextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevColor = m_ModelDataManager.OuterTubeColor.x;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(outerColorRTextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				outerColorRTextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			m_ModelDataManager.OuterTubeColor.x = color;
			outerColorRTextBox.Text = color.ToString();
			outerColorRTrackBar.Value = (Int32)(color * DIVISION);
		}

		private void outerColorGTextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevColor = m_ModelDataManager.OuterTubeColor.y;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(outerColorGTextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				outerColorGTextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			m_ModelDataManager.OuterTubeColor.y = color;
			outerColorGTextBox.Text = color.ToString();
			outerColorGTrackBar.Value = (Int32)(color * DIVISION);
		}

		private void outerColorBTextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevColor = m_ModelDataManager.OuterTubeColor.z;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(outerColorBTextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				outerColorBTextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			m_ModelDataManager.OuterTubeColor.z = color;
			outerColorBTextBox.Text = color.ToString();
			outerColorBTrackBar.Value = (Int32)(color * DIVISION);
		}

		private void outerColorATextBox_Validating(object sender, CancelEventArgs e)
		{
			float prevColor = m_ModelDataManager.OuterTubeColor.w;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(outerColorATextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				outerColorATextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			m_ModelDataManager.OuterTubeColor.w = color;
			outerColorATextBox.Text = color.ToString();
			outerColorATrackBar.Value = (Int32)(color * DIVISION);
		}

		private void innerColorRTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				innerColorRTextBox_Validating(sender, null);
			}
		}

		private void innerColorGTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				innerColorGTextBox_Validating(sender, null);
			}
		}

		private void innerColorBTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				innerColorBTextBox_Validating(sender, null);
			}
		}

		private void innerColorATextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				outerColorRTextBox_Validating(sender, null);
			}
		}

		private void outerColorRTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				outerColorRTextBox_Validating(sender, null);
			}
		}

		private void outerColorGTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				outerColorGTextBox_Validating(sender, null);
			}
		}

		private void outerColorBTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				outerColorBTextBox_Validating(sender, null);
			}
		}

		private void outerColorATextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				outerColorBTextBox_Validating(sender, null);
			}
		}

		private void pokemonSizeSRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			if (m_ModelDataManager != null)
			{
				m_ModelDataManager.Size = PokemonSize.Enum.S;
			}
		}

		private void pokemonSizeMRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			if (m_ModelDataManager != null)
			{
				m_ModelDataManager.Size = PokemonSize.Enum.M;
			}
		}

		private void pokemonSizeLRadioButton_CheckedChanged(object sender, EventArgs e)
		{
			if (m_ModelDataManager != null)
			{
				m_ModelDataManager.Size = PokemonSize.Enum.L;
			}
		}

		private void minXTrackBar_Scroll(object sender, EventArgs e)
		{
			float min = (2.0f * TRANSLATE_LIMIT) * (float)(minXTrackBar.Value - minXTrackBar.Maximum / 2) / (float)minXTrackBar.Maximum;
			minXTextBox.Text = min.ToString();
			minXTextBox_Validating(sender, null);
		}

		private void minYTrackBar_Scroll(object sender, EventArgs e)
		{
			float min = (2.0f * TRANSLATE_LIMIT) * (float)(minYTrackBar.Value - minYTrackBar.Maximum / 2) / (float)minYTrackBar.Maximum;
			minYTextBox.Text = min.ToString();
			minYTextBox_Validating(sender, null);
		}

		private void minZTrackBar_Scroll(object sender, EventArgs e)
		{
			float min = (2.0f * TRANSLATE_LIMIT) * (float)(minZTrackBar.Value - minZTrackBar.Maximum / 2) / (float)minZTrackBar.Maximum;
			minZTextBox.Text = min.ToString();
			minZTextBox_Validating(sender, null);
		}

		private void maxXTrackBar_Scroll(object sender, EventArgs e)
		{
			float max = (2.0f * TRANSLATE_LIMIT) * (float)(maxXTrackBar.Value - maxXTrackBar.Maximum / 2) / (float)maxXTrackBar.Maximum;
			maxXTextBox.Text = max.ToString();
			maxXTextBox_Validating(sender, null);
		}

		private void maxYTrackBar_Scroll(object sender, EventArgs e)
		{
			float max = (2.0f * TRANSLATE_LIMIT) * (float)(maxYTrackBar.Value - maxYTrackBar.Maximum / 2) / (float)maxYTrackBar.Maximum;
			maxYTextBox.Text = max.ToString();
			maxYTextBox_Validating(sender, null);
		}

		private void maxZTrackBar_Scroll(object sender, EventArgs e)
		{
			float max = (2.0f * TRANSLATE_LIMIT) * (float)(maxZTrackBar.Value - maxZTrackBar.Maximum / 2) / (float)maxZTrackBar.Maximum;
			maxZTextBox.Text = max.ToString();
			maxZTextBox_Validating(sender, null);
		}

		private void modelAABBVisibleCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			bool enable = modelAABBVisibleCheckBox.Checked;
			if (m_ModelDataManager != null)
			{
				m_ModelDataManager.IsModelAABBVisible = enable;
			}

			minXTextBox.Enabled = enable;
			minYTextBox.Enabled = enable;
			minZTextBox.Enabled = enable;
			minXTrackBar.Enabled = enable;
			minYTrackBar.Enabled = enable;
			minZTrackBar.Enabled = enable;

			maxXTextBox.Enabled = enable;
			maxYTextBox.Enabled = enable;
			maxZTextBox.Enabled = enable;
			maxXTrackBar.Enabled = enable;
			maxYTrackBar.Enabled = enable;
			maxZTrackBar.Enabled = enable;

			resetAABBButton.Enabled = enable;
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

		private void minXTextBox_Validating(object sender, CancelEventArgs e)
		{
			AABB aabb = m_ModelDataManager.PokemonModel.GetModelAABB();
			Vector prevTranslate = new Vector(aabb.GetMin());
			float upperLimit = aabb.GetMax().x;

			float min = 0.0f;
			bool isSucceeded = ValidateInputeTranslate(minXTextBox.Text, out min);

			if (!isSucceeded)
			{
				minXTextBox.Text = prevTranslate.x.ToString();
				return;
			}

			if (min > upperLimit)
			{
				min = upperLimit;
			}

			// 値の設定
			prevTranslate.x = min;
			aabb.SetMin(prevTranslate);
			m_ModelDataManager.PokemonModel.SetModelAABB(aabb);
			minXTextBox.Text = prevTranslate.x.ToString();
			minXTrackBar.Value = (Int32)(min * (float)minXTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)minXTrackBar.Maximum / 2.0f);
		}

		private void minYTextBox_Validating(object sender, CancelEventArgs e)
		{
			AABB aabb = m_ModelDataManager.PokemonModel.GetModelAABB();
			Vector prevTranslate = new Vector(aabb.GetMin());
			float upperLimit = aabb.GetMax().y;

			float min = 0.0f;
			bool isSucceeded = ValidateInputeTranslate(minYTextBox.Text, out min);

			if (!isSucceeded)
			{
				minYTextBox.Text = prevTranslate.y.ToString();
				return;
			}

			if (min > upperLimit)
			{
				min = upperLimit;
			}

			// 値の設定
			prevTranslate.y = min;
			aabb.SetMin(prevTranslate);
			m_ModelDataManager.PokemonModel.SetModelAABB(aabb);
			minYTextBox.Text = prevTranslate.y.ToString();
			minYTrackBar.Value = (Int32)(min * (float)minYTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)minYTrackBar.Maximum / 2.0f);
		}

		private void minZTextBox_Validating(object sender, CancelEventArgs e)
		{
			AABB aabb = m_ModelDataManager.PokemonModel.GetModelAABB();
			Vector prevTranslate = new Vector(aabb.GetMin());
			float upperLimit = aabb.GetMax().z;

			float min = 0.0f;
			bool isSucceeded = ValidateInputeTranslate(minZTextBox.Text, out min);

			if (!isSucceeded)
			{
				minZTextBox.Text = prevTranslate.z.ToString();
				return;
			}

			if (min > upperLimit)
			{
				min = upperLimit;
			}

			// 値の設定
			prevTranslate.z = min;
			aabb.SetMin(prevTranslate);
			m_ModelDataManager.PokemonModel.SetModelAABB(aabb);
			minZTextBox.Text = prevTranslate.z.ToString();
			minZTrackBar.Value = (Int32)(min * (float)minZTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)minZTrackBar.Maximum / 2.0f);
		}

		private void maxXTextBox_Validating(object sender, CancelEventArgs e)
		{
			AABB aabb = m_ModelDataManager.PokemonModel.GetModelAABB();
			Vector prevTranslate = new Vector(aabb.GetMax());
			float lowerLimit = aabb.GetMin().x;

			float max = 0.0f;
			bool isSucceeded = ValidateInputeTranslate(maxXTextBox.Text, out max);

			if (!isSucceeded)
			{
				maxXTextBox.Text = prevTranslate.x.ToString();
				return;
			}

			if (max < lowerLimit)
			{
				max = lowerLimit;
			}

			// 値の設定
			prevTranslate.x = max;
			aabb.SetMax(prevTranslate);
			m_ModelDataManager.PokemonModel.SetModelAABB(aabb);
			maxXTextBox.Text = prevTranslate.x.ToString();
			maxXTrackBar.Value = (Int32)(max * (float)maxXTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)maxXTrackBar.Maximum / 2.0f);
		}

		private void maxYTextBox_Validating(object sender, CancelEventArgs e)
		{
			AABB aabb = m_ModelDataManager.PokemonModel.GetModelAABB();
			Vector prevTranslate = new Vector(aabb.GetMax());
			float lowerLimit = aabb.GetMin().y;

			float max = 0.0f;
			bool isSucceeded = ValidateInputeTranslate(maxYTextBox.Text, out max);

			if (!isSucceeded)
			{
				maxYTextBox.Text = prevTranslate.y.ToString();
				return;
			}

			if (max < lowerLimit)
			{
				max = lowerLimit;
			}

			// 値の設定
			prevTranslate.y = max;
			aabb.SetMax(prevTranslate);
			m_ModelDataManager.PokemonModel.SetModelAABB(aabb);
			maxYTextBox.Text = prevTranslate.y.ToString();
			maxYTrackBar.Value = (Int32)(max * (float)maxYTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)maxYTrackBar.Maximum / 2.0f);
		}

		private void maxZTextBox_Validating(object sender, CancelEventArgs e)
		{
			AABB aabb = m_ModelDataManager.PokemonModel.GetModelAABB();
			Vector prevTranslate = new Vector(aabb.GetMax());
			float lowerLimit = aabb.GetMin().z;

			float max = 0.0f;
			bool isSucceeded = ValidateInputeTranslate(maxZTextBox.Text, out max);

			if (!isSucceeded)
			{
				maxZTextBox.Text = prevTranslate.z.ToString();
				return;
			}

			if (max < lowerLimit)
			{
				max = lowerLimit;
			}

			// 値の設定
			prevTranslate.z = max;
			aabb.SetMax(prevTranslate);
			m_ModelDataManager.PokemonModel.SetModelAABB(aabb);
			maxZTextBox.Text = prevTranslate.z.ToString();
			maxZTrackBar.Value = (Int32)(max * (float)maxZTrackBar.Maximum / (2.0f * TRANSLATE_LIMIT) + (float)maxZTrackBar.Maximum / 2.0f);
		}

		private void minXTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				minXTextBox_Validating(sender, null);
			}
		}

		private void minYTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				minYTextBox_Validating(sender, null);
			}
		}

		private void minZTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				minZTextBox_Validating(sender, null);
			}
		}

		private void maxXTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				maxXTextBox_Validating(sender, null);
			}
		}

		private void maxYTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				maxYTextBox_Validating(sender, null);
			}
		}

		private void maxZTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				maxZTextBox_Validating(sender, null);
			}
		}

		private void resetAABBButton_Click(object sender, EventArgs e)
		{
			m_ModelDataManager.ResetModelAABB();
			SetAABBValue();
		}
	}
}
