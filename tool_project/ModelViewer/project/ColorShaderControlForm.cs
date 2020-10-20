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
using gfl2.clr.renderingengine.renderer.util;

namespace ModelViewer
{
	public partial class ColorShaderControlForm : Form, IModelMotionResourceChangeListener
	{
		private PokemonModelDataManager m_ModelDataManager;
		private readonly ModelMotionResourceTreeViewForm m_ModelMotionResourceTreeViewForm;
		private readonly ViewerRenderingPipeLine m_ViewerRenderingPipeLine;
		private const float DIVISION = 10.0f;

		public ColorShaderControlForm(
			ModelMotionResourceTreeViewForm treeView,
			ViewerRenderingPipeLine pipeLine)
		{
			InitializeComponent();
			m_ModelMotionResourceTreeViewForm = treeView;
			m_ViewerRenderingPipeLine = pipeLine;
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
      colorShaderEnableCheckBox.Enabled = enable;
      ambientColorEnableCheckBox.Enabled = enable;

      bool enableColorShader = enable && m_ModelDataManager.PokemonModel.IsEnabledColorShaderTexture();
      purpleButton.Enabled = enableColorShader;
      yellowButton.Enabled = enableColorShader;
      redButton.Enabled = enableColorShader;
      blueButton.Enabled = enableColorShader;

			if (!enable)
			{
				interpColorRTrackBar.Value = 0;
				interpColorGTrackBar.Value = 0;
				interpColorBTrackBar.Value = 0;
				interpColorATrackBar.Value = 0;
				ambientColorTrackBar.Value = 0;
				interpColorRTextBox.Text = "";
				interpColorGTextBox.Text = "";
				interpColorBTextBox.Text = "";
				interpColorATextBox.Text = "";
				ambientColorTextBox.Text = "";
				colorShaderEnableCheckBox.Checked = false;
				ambientColorEnableCheckBox.Checked = false;
				rareColorEnableButton.Enabled = false;
				normalColorEnableButton.Enabled = false;
				return;
			}

			gfl2.clr.poke_3d_lib.PokemonModel pokemonModel = m_ModelDataManager.PokemonModel;
			Vector interpColor = pokemonModel.GetInterpColor();
			Vector ambientColor = pokemonModel.GetAmbientColor();
			interpColorRTrackBar.Value = (Int32)(DIVISION * interpColor.x);
			interpColorGTrackBar.Value = (Int32)(DIVISION * interpColor.y);
			interpColorBTrackBar.Value = (Int32)(DIVISION * interpColor.z);
			interpColorATrackBar.Value = (Int32)(DIVISION * interpColor.w);
			ambientColorTrackBar.Value = (Int32)(DIVISION * ambientColor.w);
			interpColorRTextBox.Text = (interpColorRTrackBar.Value / DIVISION).ToString();
			interpColorGTextBox.Text = (interpColorGTrackBar.Value / DIVISION).ToString();
			interpColorBTextBox.Text = (interpColorBTrackBar.Value / DIVISION).ToString();
			interpColorATextBox.Text = (interpColorATrackBar.Value / DIVISION).ToString();
			ambientColorTextBox.Text = (ambientColorTrackBar.Value / DIVISION).ToString();
			m_ModelDataManager.PokemonModel.SetInterpColor(interpColor);
			m_ModelDataManager.PokemonModel.SetAmbientColor(ambientColor);

			if (colorShaderEnableCheckBox.Checked == pokemonModel.IsEnabledColorShaderTexture())
			{// チェックが同じ状態だったときにちゃんとカラーを有効/無効切替するための処理
				SetEnabledOverrideInterpColor(pokemonModel.IsEnabledColorShaderTexture());
			}

			if (ambientColorEnableCheckBox.Checked == pokemonModel.IsEnabledAmbientColor())
			{// チェックが同じ状態だったときにちゃんとカラーを有効/無効切替するための処理
				SetEnabledOverrideAmbientColor(pokemonModel.IsEnabledAmbientColor());
			}

			colorShaderEnableCheckBox.Checked = pokemonModel.IsEnabledColorShaderTexture();
			ambientColorEnableCheckBox.Checked = pokemonModel.IsEnabledAmbientColor();

			rareColorEnableButton.Enabled = !m_ModelDataManager.IsRare;
			normalColorEnableButton.Enabled = m_ModelDataManager.IsRare;
		}

		private void interpColorRTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				interpColorRTextBox_Validating(sender, null);
			}
		}

		private void interpColorGTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				interpColorGTextBox_Validating(sender, null);
			}
		}

		private void interpColorBTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				interpColorBTextBox_Validating(sender, null);
			}
		}

		private void interpColorATextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				interpColorATextBox_Validating(sender, null);
			}
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

		private void interpColorRTextBox_Validating(object sender, CancelEventArgs e)
		{
			gfl2.clr.poke_3d_lib.PokemonModel pokemonModel = m_ModelDataManager.PokemonModel;
			Vector interpColor = new Vector(0.0f, 0.0f, 0.0f, 0.0f);
			if (pokemonModel.IsEnabledColorShaderTexture())
			{
				interpColor = pokemonModel.GetInterpColor();
			}

			float prevColor = interpColor.x;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(interpColorRTextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				interpColorRTextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			interpColor.x = color;
			m_ModelDataManager.PokemonModel.SetInterpColor(interpColor);
			interpColorRTextBox.Text = interpColor.x.ToString();
			interpColorRTrackBar.Value = (Int32)(color * DIVISION);
			m_ViewerRenderingPipeLine.SetColorShaderTextureColor(m_ModelDataManager.PokemonModel);
		}

		private void interpColorGTextBox_Validating(object sender, CancelEventArgs e)
		{
			gfl2.clr.poke_3d_lib.PokemonModel pokemonModel = m_ModelDataManager.PokemonModel;
			Vector interpColor = new Vector(0.0f, 0.0f, 0.0f, 0.0f);
			if (pokemonModel.IsEnabledColorShaderTexture())
			{
				interpColor = pokemonModel.GetInterpColor();
			}

			float prevColor = interpColor.y;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(interpColorGTextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				interpColorGTextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			interpColor.y = color;
			m_ModelDataManager.PokemonModel.SetInterpColor(interpColor);
			interpColorGTextBox.Text = interpColor.y.ToString();
			interpColorGTrackBar.Value = (Int32)(color * DIVISION);
			m_ViewerRenderingPipeLine.SetColorShaderTextureColor(m_ModelDataManager.PokemonModel);
		}

		private void interpColorBTextBox_Validating(object sender, CancelEventArgs e)
		{
			gfl2.clr.poke_3d_lib.PokemonModel pokemonModel = m_ModelDataManager.PokemonModel;
			Vector interpColor = new Vector(0.0f, 0.0f, 0.0f, 0.0f);
			if (pokemonModel.IsEnabledColorShaderTexture())
			{
				interpColor = pokemonModel.GetInterpColor();
			}

			float prevColor = interpColor.z;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(interpColorBTextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				interpColorBTextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			interpColor.z = color;
			m_ModelDataManager.PokemonModel.SetInterpColor(interpColor);
			interpColorBTextBox.Text = interpColor.z.ToString();
			interpColorBTrackBar.Value = (Int32)(color * DIVISION);
			m_ViewerRenderingPipeLine.SetColorShaderTextureColor(m_ModelDataManager.PokemonModel);
		}

		private void interpColorATextBox_Validating(object sender, CancelEventArgs e)
		{
			gfl2.clr.poke_3d_lib.PokemonModel pokemonModel = m_ModelDataManager.PokemonModel;
			Vector interpColor = new Vector(0.0f, 0.0f, 0.0f, 0.0f);
			if (pokemonModel.IsEnabledColorShaderTexture())
			{
				interpColor = pokemonModel.GetInterpColor();
			}

			float prevColor = interpColor.w;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(interpColorATextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				interpColorATextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			interpColor.w = color;
			m_ModelDataManager.PokemonModel.SetInterpColor(interpColor);
			interpColorATextBox.Text = interpColor.w.ToString();
			interpColorATrackBar.Value = (Int32)(color * DIVISION);
			m_ViewerRenderingPipeLine.SetColorShaderTextureColor(m_ModelDataManager.PokemonModel);
		}

		private void interpColorRTrackBar_Scroll(object sender, EventArgs e)
		{
			interpColorRTextBox.Text = ((float)(interpColorRTrackBar.Value) / DIVISION).ToString();
			interpColorRTextBox_Validating(sender, null);
		}

		private void interpColorGTrackBar_Scroll(object sender, EventArgs e)
		{
			interpColorGTextBox.Text = ((float)(interpColorGTrackBar.Value) / DIVISION).ToString();
			interpColorGTextBox_Validating(sender, null);
		}

		private void interpColorBTrackBar_Scroll(object sender, EventArgs e)
		{
			interpColorBTextBox.Text = ((float)(interpColorBTrackBar.Value) / DIVISION).ToString();
			interpColorBTextBox_Validating(sender, null);
		}

		private void interpColorATrackBar_Scroll(object sender, EventArgs e)
		{
			interpColorATextBox.Text = ((float)(interpColorATrackBar.Value) / DIVISION).ToString();
			interpColorATextBox_Validating(sender, null);
		}

		private void colorShaderEnableCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			bool enable = colorShaderEnableCheckBox.Checked;

			interpColorRTextBox.Enabled = enable;
			interpColorGTextBox.Enabled = enable;
			interpColorBTextBox.Enabled = enable;
			interpColorATextBox.Enabled = enable;
			interpColorRTrackBar.Enabled = enable;
			interpColorGTrackBar.Enabled = enable;
			interpColorBTrackBar.Enabled = enable;
			interpColorATrackBar.Enabled = enable;

      purpleButton.Enabled = enable;
      redButton.Enabled = enable;
      yellowButton.Enabled = enable;
      blueButton.Enabled = enable;

			SetEnabledOverrideInterpColor(enable);
		}

		private void SetEnabledOverrideInterpColor(bool enable)
		{
			if (m_ModelDataManager == null)
			{
				return;
			}

			gfl2.clr.poke_3d_lib.PokemonModel pokemonModel = m_ModelDataManager.PokemonModel;
			pokemonModel.SetEnabledColorShaderTexture(enable);
			m_ViewerRenderingPipeLine.SetColorShaderTextureColor(m_ModelDataManager.PokemonModel);

			if (!enable)
			{// 無効になった場合は色を0.0fに戻す
				Vector color = pokemonModel.GetInterpColor();
				interpColorRTextBox.Text = color.x.ToString();
				interpColorGTextBox.Text = color.y.ToString();
				interpColorBTextBox.Text = color.z.ToString();
				interpColorATextBox.Text = color.w.ToString();
				interpColorRTextBox_Validating(null, null);
				interpColorGTextBox_Validating(null, null);
				interpColorBTextBox_Validating(null, null);
				interpColorATextBox_Validating(null, null);
			}
		}

		private void SetEnabledOverrideAmbientColor(bool enable)
		{
			if (m_ModelDataManager == null)
			{
				return;
			}

			gfl2.clr.poke_3d_lib.PokemonModel pokemonModel = m_ModelDataManager.PokemonModel;

			pokemonModel.SetEnabledAmbientColor(enable);
			if (enable)
			{
				Vector color = pokemonModel.GetAmbientColor();
				color.x = 1.0f - color.x;
				color.y = 1.0f - color.y;
				color.z = 1.0f - color.z;
				color.w = 1.0f - color.w;
				m_ModelDataManager.PokemonModel.SetAmbientColor(color);
			}
			else
			{
				Vector one = new Vector(1.0f, 1.0f, 1.0f, 1.0f);
				m_ModelDataManager.PokemonModel.SetAmbientColor(one);
			}
		}

		private void ambientColorTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				ambientColorTextBox_Validating(sender, null);
			}
		}

		private void ambientColorTextBox_Validating(object sender, CancelEventArgs e)
		{
      if (m_ModelDataManager == null)
      {
        return;
      }

			gfl2.clr.poke_3d_lib.PokemonModel pokemonModel = m_ModelDataManager.PokemonModel;
			Vector ambientColor = pokemonModel.GetAmbientColor();
			float prevColor = ambientColor.w;
			float color = 0.0f;

			bool isSucceeded = ValidateInputColor(ambientColorTextBox.Text, out color);

			// @todo: 失敗してたら設定前のスケール値に戻す
			if (!isSucceeded)
			{
				ambientColorTextBox.Text = prevColor.ToString();
				return;
			}

			// 値の設定
			float col = (float)color;
			ambientColor.x = 1.0f - col;
			ambientColor.y = 1.0f - col;
			ambientColor.z = 1.0f - col;
			ambientColor.w = 1.0f - col;
			m_ModelDataManager.PokemonModel.SetAmbientColor(ambientColor);
			ambientColorTextBox.Text = col.ToString();
			ambientColorTrackBar.Value = (Int32)(color * DIVISION);
		}

		private void ambientColorTrackBar_Scroll(object sender, EventArgs e)
		{
			ambientColorTextBox.Text = ((float)(ambientColorTrackBar.Value) / DIVISION).ToString();
			ambientColorTextBox_Validating(sender, null);
		}

		private void ambientColorEnableCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			bool enable = ambientColorEnableCheckBox.Checked;

			ambientColorTextBox.Enabled = enable;
			ambientColorTrackBar.Enabled = enable;

			SetEnabledOverrideAmbientColor(enable);
      ambientColorTextBox_Validating(null, null);
		}

		private void rareColorEnableButton_Click(object sender, EventArgs e)
		{
			if (m_ModelDataManager == null)
			{
				return;
			}

      var rareTextureTuples = m_ModelDataManager.GfMdlResourcePathSetDictionary[PokemonModelDataManager.ConvertExtensionType.WinOpenGL].textureRareFilePathTuples;
      if (rareTextureTuples == null)
			{
				MessageBox.Show("レアカラーテクスチャが見つかりません");
				return;
			}

			m_ModelMotionResourceTreeViewForm.ReloadGfResources(m_ModelDataManager, true);
		}

		private void normalColorEnableButton_Click(object sender, EventArgs e)
		{
			if (m_ModelDataManager != null)
			{
				m_ModelMotionResourceTreeViewForm.ReloadGfResources(m_ModelDataManager, false);
			}
		}

		private void ColorShaderControlForm_Load(object sender, EventArgs e)
		{

		}

		private void purpleButton_Click(object sender, EventArgs e)
		{
			interpColorRTextBox.Text = ((float)(5) / DIVISION).ToString();
			interpColorRTextBox_Validating(sender, null);
			interpColorGTextBox.Text = ((float)(0) / DIVISION).ToString();
			interpColorGTextBox_Validating(sender, null);
			interpColorBTextBox.Text = ((float)(10) / DIVISION).ToString();
			interpColorBTextBox_Validating(sender, null);
			interpColorATextBox.Text = ((float)(5) / DIVISION).ToString();
			interpColorATextBox_Validating(sender, null);
		}

		private void yellowButton_Click(object sender, EventArgs e)
		{
			interpColorRTextBox.Text = ((float)(10) / DIVISION).ToString();
			interpColorRTextBox_Validating(sender, null);
			interpColorGTextBox.Text = ((float)(10) / DIVISION).ToString();
			interpColorGTextBox_Validating(sender, null);
			interpColorBTextBox.Text = ((float)(0) / DIVISION).ToString();
			interpColorBTextBox_Validating(sender, null);
			interpColorATextBox.Text = ((float)(5) / DIVISION).ToString();
			interpColorATextBox_Validating(sender, null);
		}

		private void redButton_Click(object sender, EventArgs e)
		{
			interpColorRTextBox.Text = ((float)(10) / DIVISION).ToString();
			interpColorRTextBox_Validating(sender, null);
			interpColorGTextBox.Text = (2.5f / DIVISION).ToString();
			interpColorGTextBox_Validating(sender, null);
			interpColorBTextBox.Text = (2.5f / DIVISION).ToString();
			interpColorBTextBox_Validating(sender, null);
			interpColorATextBox.Text = (7.5f / DIVISION).ToString();
			interpColorATextBox_Validating(sender, null);
		}

		private void blueButton_Click(object sender, EventArgs e)
		{
			interpColorRTextBox.Text = ((float)(5) / DIVISION).ToString();
			interpColorRTextBox_Validating(sender, null);
			interpColorGTextBox.Text = ((float)(10) / DIVISION).ToString();
			interpColorGTextBox_Validating(sender, null);
			interpColorBTextBox.Text = ((float)(10) / DIVISION).ToString();
			interpColorBTextBox_Validating(sender, null);
			interpColorATextBox.Text = ((float)(10) / DIVISION).ToString();
			interpColorATextBox_Validating(sender, null);
		}
	}
}
