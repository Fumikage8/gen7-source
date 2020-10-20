using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using gfl2.clr.renderingengine.renderer.util;

namespace ModelViewer
{
	public partial class EffectLocatorControlForm : Form, IModelMotionResourceChangeListener
	{
		private PokemonModelDataManager m_ModelDataManager;

		public EffectLocatorControlForm()
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

		private void EffectLocatorControlForm_Load(object sender, EventArgs e)
		{

		}

		public void ReceiveNotification(PokemonModelDataManager modelDataManager)
		{
			// 変更されていないなら無視
			if (m_ModelDataManager == modelDataManager)
			{
				return;
			}

			// データチェック
			m_ModelDataManager = modelDataManager;
			
			// ロケータのフラグを全て一旦クリア
			effectLocatorGridView.Rows.Clear();

			if (m_ModelDataManager == null)
			{
				changeAllVisibilityCheckBox.Enabled = false;
				changeAllVisibilityCheckBox.Checked = false;
				return;
			}

			if (m_ModelDataManager.PokemonModel == null)
			{
				changeAllVisibilityCheckBox.Enabled = false;
				changeAllVisibilityCheckBox.Checked = false;
				return;
			}

      if (m_ModelDataManager.EffectLocatorInstanceNodes == null)
      {
        changeAllVisibilityCheckBox.Enabled = false;
        changeAllVisibilityCheckBox.Checked = false;
        return;
      }

			bool allVisible = true;
      for (Int32 i = 0; i < modelDataManager.EffectLocatorInstanceNodes.Length; ++i)
			{
        bool enabled = m_ModelDataManager.EffectLocatorInstanceNodes[i].IsVisible();
        var node = modelDataManager.EffectLocatorInstanceNodes[i];
				string name = node.GetName();
				effectLocatorGridView.Rows.Add(new object[] { name, enabled });
				if (!enabled)
				{
					allVisible = false;
				}
			}
			changeAllVisibilityCheckBox.Enabled = true;
			changeAllVisibilityCheckBox.Checked = allVisible;
		}

		private void effectLocatorGridView_CurrentCellDirtyStateChanged(object sender, EventArgs e)
		{
			if (effectLocatorGridView.IsCurrentCellDirty)
			{
				effectLocatorGridView.CommitEdit(DataGridViewDataErrorContexts.Commit);
			}
		}

		private void effectLocatorGridView_CellValueChanged(object sender, DataGridViewCellEventArgs e)
		{
			if (e.RowIndex >= 0 && e.ColumnIndex == 1)
			{
				bool isChecked = (bool)effectLocatorGridView[e.ColumnIndex, e.RowIndex].Value;
        m_ModelDataManager.EffectLocatorInstanceNodes[e.RowIndex].SetVisible(isChecked);
			}
		}

		private void changeAllVisibilityCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			if (m_ModelDataManager == null)
			{
				return;
			}

      Int32 nodeNum = m_ModelDataManager.EffectLocatorInstanceNodes.Length;
			for (Int32 i = 0; i < nodeNum; ++i)
			{
				effectLocatorGridView[1, i].Value = changeAllVisibilityCheckBox.Checked;
			}
		}

		private bool CheckAllVisibility()
		{
      Int32 nodeNum = m_ModelDataManager.EffectLocatorInstanceNodes.Length;
			for (Int32 i = 0; i < nodeNum; ++i)
			{
				if ((bool)effectLocatorGridView[1, i].Value == false)
				{
					return false;
				}
			}
			return true;
		}
	}
}
