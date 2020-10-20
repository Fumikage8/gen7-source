using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using gfl2.clr.renderingengine.scenegraph.instance;

namespace ModelViewer
{
	public partial class LightControlForm : Form, IModelMotionResourceChangeListener
	{
		private PokemonModelDataManager m_PokemonModelDataManager;		//!< @brief UI上で扱うLightSetNode
		private const float ROTATE_HORIZONTAL_LIMIT = 180.0f;
		private const float ROTATE_VERTICAL_LIMIT = 90.0f;
		private readonly float[] m_LightHorizontalDefaultAngles;
		private readonly float[] m_LightVerticalDefaultAngles;

		/**
		 * @brief コンストラクタ
		 */
		public LightControlForm()
		{
			InitializeComponent();
			m_LightHorizontalDefaultAngles = new float[3] { -45.0f, 0.0f, 45.0f };
			m_LightVerticalDefaultAngles = new float[3] { 70.0f, 50.0f, 30.0f };
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
		private void LightControlForm_Load(object sender, EventArgs e)
		{

		}

		/**
		 * @brief LightSetNodeの変更通知受け取り
		 * @param lightSetNode LightSetNode
		 */
		public void ReceiveNotification(PokemonModelDataManager modelDataManager)
		{
      if (m_PokemonModelDataManager == modelDataManager)
      {
        return;
      }

      m_PokemonModelDataManager = modelDataManager;
      bool enable = (modelDataManager != null);
      rotateXTextBox.Enabled = enable;
      rotateYTextBox.Enabled = enable;
      rotateMinusXButton.Enabled = enable;
      rotateMinusYButton.Enabled = enable;
      rotatePlusXButton.Enabled = enable;
      rotatePlusYButton.Enabled = enable;
      rotateXTrackBar.Enabled = enable;
      rotateYTrackBar.Enabled = enable;

      leftTopButton.Enabled = enable;
      topButton.Enabled = enable;
      rightTopButton.Enabled = enable;
      leftButton.Enabled = enable;
      centerButton.Enabled = enable;
      rightButton.Enabled = enable;
      leftBottomButton.Enabled = enable;
      bottomButton.Enabled = enable;
      rightBottomButton.Enabled = enable;

      if (enable)
      {
	      rotateXTextBox.Text = modelDataManager.LightRotationX.ToString();
	      rotateYTextBox.Text = modelDataManager.LightRotationY.ToString();
	      rotateXTextBox_Validating(null, null);
	      rotateYTextBox_Validating(null, null);
      }
    }

		/**
		 * @brief Rotateの異常値チェック
		 * @param strFrame 文字列で受け取ったrotate
		 * @param[out] float 変換後のrotate
		 * return bool 成功/失敗
		 */
		private bool ValidateInputVerticalRotate(string strRotate, out float rotate)
		{
			bool isSucceeded = float.TryParse(strRotate, out rotate);
			if (!isSucceeded)
			{
				return false;
			}

			if (rotate < -ROTATE_VERTICAL_LIMIT)
			{
				rotate = -ROTATE_VERTICAL_LIMIT;
			}
			else if (rotate > +ROTATE_VERTICAL_LIMIT)
			{
				rotate = ROTATE_VERTICAL_LIMIT;
			}

			return true;
		}

		/**
		 * @brief Rotateの異常値チェック
		 * @param strFrame 文字列で受け取ったrotate
		 * @param[out] float 変換後のrotate
		 * return bool 成功/失敗
		 */
		private bool ValidateInputHorizontalRotate(string strRotate, out float rotate)
		{
			bool isSucceeded = float.TryParse(strRotate, out rotate);
			if (!isSucceeded)
			{
				return false;
			}

			if (rotate < -ROTATE_HORIZONTAL_LIMIT)
			{
				rotate = -ROTATE_HORIZONTAL_LIMIT;
			}
			else if (rotate > +ROTATE_HORIZONTAL_LIMIT)
			{
				rotate = ROTATE_HORIZONTAL_LIMIT;
			}

			return true;
		}

		/**
		 * @brief Rotate(X)設定
		 * @param sender
		 * @param e
		 */
		private void rotateXTextBox_TextChanged(object sender, EventArgs e)
		{

		}

		/**
		 * @brief Rotate(Y)設定
		 * @param sender
		 * @param e
		 */
		private void rotateYTextBox_TextChanged(object sender, EventArgs e)
		{
			
		}

		private void rotateMinusXButton_Click(object sender, EventArgs e)
		{
			m_PokemonModelDataManager.LightRotationX -= 1.0f;
			if (m_PokemonModelDataManager.LightRotationX < -ROTATE_HORIZONTAL_LIMIT)
			{
				m_PokemonModelDataManager.LightRotationX = -ROTATE_HORIZONTAL_LIMIT;
			}
			rotateXTextBox.Text = m_PokemonModelDataManager.LightRotationX.ToString();
		}

		private void rotatePlusXButton_Click(object sender, EventArgs e)
		{
			m_PokemonModelDataManager.LightRotationX += 1.0f;
			if (m_PokemonModelDataManager.LightRotationX > ROTATE_HORIZONTAL_LIMIT)
			{
				m_PokemonModelDataManager.LightRotationX = ROTATE_HORIZONTAL_LIMIT;
			}
			rotateXTextBox.Text = m_PokemonModelDataManager.LightRotationX.ToString();
		}

		private void rotateMinusYButton_Click(object sender, EventArgs e)
		{
			m_PokemonModelDataManager.LightRotationY -= 1.0f;
			if (m_PokemonModelDataManager.LightRotationY < -ROTATE_VERTICAL_LIMIT)
			{
				m_PokemonModelDataManager.LightRotationY = -ROTATE_VERTICAL_LIMIT;
			}
			rotateYTextBox.Text = m_PokemonModelDataManager.LightRotationY.ToString();
		}

		private void rotatePlusYButton_Click(object sender, EventArgs e)
		{
			m_PokemonModelDataManager.LightRotationY += 1.0f;
			if (m_PokemonModelDataManager.LightRotationY > ROTATE_VERTICAL_LIMIT)
			{
				m_PokemonModelDataManager.LightRotationY = ROTATE_VERTICAL_LIMIT;
			}
			rotateYTextBox.Text = m_PokemonModelDataManager.LightRotationY.ToString();
		}

		private void rotateXTrackBar_Scroll(object sender, EventArgs e)
		{
			float rotate = (2.0f * ROTATE_HORIZONTAL_LIMIT) * (float)(rotateXTrackBar.Value - rotateXTrackBar.Maximum / 2) / (float)rotateXTrackBar.Maximum;
			rotateXTextBox.Text = rotate.ToString();
			rotateXTextBox_Validating(sender, null);
		}

		private void rotateYTrackBar_Scroll(object sender, EventArgs e)
		{
			float rotate = (2.0f * ROTATE_VERTICAL_LIMIT) * (float)(rotateYTrackBar.Value - rotateYTrackBar.Maximum / 2) / (float)rotateYTrackBar.Maximum;
			rotateYTextBox.Text = rotate.ToString();
			rotateYTextBox_Validating(sender, null);
		}

		private void leftTopButton_Click(object sender, EventArgs e)
		{
			rotateXTextBox.Text = m_LightHorizontalDefaultAngles[0].ToString();
			rotateYTextBox.Text = m_LightVerticalDefaultAngles[0].ToString();
			rotateXTextBox_Validating(sender, null);
			rotateYTextBox_Validating(sender, null);
		}

		private void topButton_Click(object sender, EventArgs e)
		{
			rotateXTextBox.Text = m_LightHorizontalDefaultAngles[1].ToString();
			rotateYTextBox.Text = m_LightVerticalDefaultAngles[0].ToString();
			rotateXTextBox_Validating(sender, null);
			rotateYTextBox_Validating(sender, null);
		}

		private void rightTopButton_Click(object sender, EventArgs e)
		{
			rotateXTextBox.Text = m_LightHorizontalDefaultAngles[2].ToString();
			rotateYTextBox.Text = m_LightVerticalDefaultAngles[0].ToString();
			rotateXTextBox_Validating(sender, null);
			rotateYTextBox_Validating(sender, null);
		}

		private void leftButton_Click(object sender, EventArgs e)
		{
			rotateXTextBox.Text = m_LightHorizontalDefaultAngles[0].ToString();
			rotateYTextBox.Text = m_LightVerticalDefaultAngles[1].ToString();
			rotateXTextBox_Validating(sender, null);
			rotateYTextBox_Validating(sender, null);
		}

		private void centerButton_Click(object sender, EventArgs e)
		{
			rotateXTextBox.Text = m_LightHorizontalDefaultAngles[1].ToString();
			rotateYTextBox.Text = m_LightVerticalDefaultAngles[1].ToString();
			rotateXTextBox_Validating(sender, null);
			rotateYTextBox_Validating(sender, null);
		}

		private void rightButton_Click(object sender, EventArgs e)
		{
			rotateXTextBox.Text = m_LightHorizontalDefaultAngles[2].ToString();
			rotateYTextBox.Text = m_LightVerticalDefaultAngles[1].ToString();
			rotateXTextBox_Validating(sender, null);
			rotateYTextBox_Validating(sender, null);
		}

		private void leftBottomButton_Click(object sender, EventArgs e)
		{
			rotateXTextBox.Text = m_LightHorizontalDefaultAngles[0].ToString();
			rotateYTextBox.Text = m_LightVerticalDefaultAngles[2].ToString();
			rotateXTextBox_Validating(sender, null);
			rotateYTextBox_Validating(sender, null);
		}

		private void bottomButton_Click(object sender, EventArgs e)
		{
			rotateXTextBox.Text = m_LightHorizontalDefaultAngles[1].ToString();
			rotateYTextBox.Text = m_LightVerticalDefaultAngles[2].ToString();
			rotateXTextBox_Validating(sender, null);
			rotateYTextBox_Validating(sender, null);
		}

		private void rightBottomButton_Click(object sender, EventArgs e)
		{
			rotateXTextBox.Text = m_LightHorizontalDefaultAngles[2].ToString();
			rotateYTextBox.Text = m_LightVerticalDefaultAngles[2].ToString();
			rotateXTextBox_Validating(sender, null);
			rotateYTextBox_Validating(sender, null);
		}

		private void rotateXTextBox_Validating(object sender, CancelEventArgs e)
		{
			float rotateX;
			bool isSucceeded = ValidateInputHorizontalRotate(rotateXTextBox.Text, out rotateX);

			if (!isSucceeded)
			{
				rotateXTextBox.Text = m_PokemonModelDataManager.LightRotationX.ToString();
				return;
			}

			m_PokemonModelDataManager.LightRotationX = rotateX;
			rotateXTrackBar.Value = (Int32)(rotateX * (float)rotateXTrackBar.Maximum / (2.0f * ROTATE_HORIZONTAL_LIMIT) + (float)rotateXTrackBar.Maximum / 2.0f);
			rotateXTextBox.Text = rotateX.ToString();
		}

		private void rotateYTextBox_Validating(object sender, CancelEventArgs e)
		{
			float rotateY;
			bool isSucceeded = ValidateInputVerticalRotate(rotateYTextBox.Text, out rotateY);

			if (!isSucceeded)
			{
				rotateYTextBox.Text = m_PokemonModelDataManager.LightRotationY.ToString();
				return;
			}

			m_PokemonModelDataManager.LightRotationY = rotateY;
			rotateYTrackBar.Value = (Int32)(rotateY * (float)rotateYTrackBar.Maximum / (2.0f * ROTATE_VERTICAL_LIMIT) + (float)rotateYTrackBar.Maximum / 2.0f);
			rotateYTextBox.Text = rotateY.ToString();
		}

		private void rotateXTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				rotateXTextBox_Validating(sender, null);
			}
		}

		private void rotateYTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				rotateYTextBox_Validating(sender, null);
			}
		}
	}
}
