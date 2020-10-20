using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ModelViewer
{
	public partial class KeySoundControlForm : Form, IModelMotionResourceChangeListener
	{
		private PokemonModelDataManager m_PokemonModelDataManager;
		private CheckBox[] m_KeySoundCheckBoxes;

		public KeySoundControlForm()
		{
			InitializeComponent();
			m_KeySoundCheckBoxes = new CheckBox[10];
			m_KeySoundCheckBoxes[0] = this.AK_EffectStart01CheckBox;
			m_KeySoundCheckBoxes[1] = this.AK_ButuriStart01CheckBox;
			m_KeySoundCheckBoxes[2] = this.AK_SEStart01CheckBox;
			m_KeySoundCheckBoxes[3] = this.AK_SEStart02CheckBox;
			m_KeySoundCheckBoxes[4] = this.AK_SEStart03CheckBox;
			m_KeySoundCheckBoxes[5] = this.AK_SEStart04CheckBox;
			m_KeySoundCheckBoxes[6] = this.AK_PartsSkel01CheckBox;

      // PokemonModel側のキーの順序がここだけ入れ替わった関係で, 
      // VisibleとMaterialだけキーの再生順序が逆になっている
      m_KeySoundCheckBoxes[8] = this.AK_PartsMaterial01CheckBox;
      m_KeySoundCheckBoxes[7] = this.AK_PartsVisible01CheckBox;
      
      m_KeySoundCheckBoxes[9] = this.AK_PartsShader01CheckBox;
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
			if (modelDataManager != null)
			{
				for (UInt32 i = 0; i < m_KeySoundCheckBoxes.Length; ++i)
				{
					if (modelDataManager.PokemonModel.GetKeySoundMatchFlag(i))
					{
						m_KeySoundCheckBoxes[i].BackColor = Color.Red;
					}
					else
					{
						m_KeySoundCheckBoxes[i].BackColor = Color.White;
					}
				}
				
			}

			if (m_PokemonModelDataManager == modelDataManager)
			{
				return;
			}

			this.KeySoundEnabledCheckBox.Enabled = (modelDataManager != null);

			// モデルデータが切り替わったら最初に参照していたモデルの音を鳴らさないようにする
			if (m_PokemonModelDataManager != null)
			{
				for (UInt32 i = 0; i < (UInt32)m_KeySoundCheckBoxes.Length; ++i)
				{
					if (m_PokemonModelDataManager.PokemonModel == null)
					{
						break;
					}
					m_PokemonModelDataManager.PokemonModel.SetEnabledPlaySoundFlag(i, false);
				}
			}
			
			if (!this.KeySoundEnabledCheckBox.Enabled)
			{
				this.KeySoundEnabledCheckBox.Checked = false;
				Array.ForEach(m_KeySoundCheckBoxes, data => data.Checked = false);
			}

			m_PokemonModelDataManager = modelDataManager;
		}

		private bool IsModelDataEmpty()
		{
			if (m_PokemonModelDataManager == null)
			{
				return true;
			}

			if (m_PokemonModelDataManager.PokemonModel == null)
			{
				return true;
			}

			return false;
		}

		private void SetEnabledPlaySoundFlag(UInt32 index, bool enabled)
		{
			if (IsModelDataEmpty())
			{
				return;
			}

			m_PokemonModelDataManager.PokemonModel.SetEnabledPlaySoundFlag(index, enabled);
		}

		private void AK_EffectStart01CheckBox_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledPlaySoundFlag(0, m_KeySoundCheckBoxes[0].Checked);
		}

		private void AK_ButuriStart01CheckBox_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledPlaySoundFlag(1, m_KeySoundCheckBoxes[1].Checked);
		}

		private void AK_SEStart01CheckBox_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledPlaySoundFlag(2, m_KeySoundCheckBoxes[2].Checked);
		}

		private void AK_SEStart02CheckBox_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledPlaySoundFlag(3, m_KeySoundCheckBoxes[3].Checked);
		}

		private void AK_SEStart03CheckBox_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledPlaySoundFlag(4, m_KeySoundCheckBoxes[4].Checked);
		}

		private void AK_SEStart04CheckBox_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledPlaySoundFlag(5, m_KeySoundCheckBoxes[5].Checked);
		}

		private void AK_PartsSkel01CheckBox_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledPlaySoundFlag(6, m_KeySoundCheckBoxes[6].Checked);
		}

		private void AK_PartsVisible01CheckBox_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledPlaySoundFlag(7, m_KeySoundCheckBoxes[7].Checked);
		}

		private void AK_PartsMaterial01CheckBox_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledPlaySoundFlag(8, m_KeySoundCheckBoxes[8].Checked);
		}

		private void AK_PartsShader01CheckBox_CheckedChanged(object sender, EventArgs e)
		{
			SetEnabledPlaySoundFlag(9, m_KeySoundCheckBoxes[9].Checked);
		}

		private void KeySoundEnabledCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			bool isChecked = KeySoundEnabledCheckBox.Checked;
			Array.ForEach(m_KeySoundCheckBoxes, data => data.Enabled = isChecked);

			if (!isChecked)
			{// 有効化のチェックが外されたら全てのキー再生のチェックを外す
				Array.ForEach(m_KeySoundCheckBoxes, data => data.Checked = false);
				return;
			}

			for (UInt32 i = 0; i < (UInt32)m_KeySoundCheckBoxes.Length; ++i)
			{
				bool flag = (m_PokemonModelDataManager != null);
				if (flag)
				{
					flag = m_PokemonModelDataManager.PokemonModel.GetPlaySoundFlag(i);
				}
				m_KeySoundCheckBoxes[i].Checked = flag;
			}
		}
	}
}
