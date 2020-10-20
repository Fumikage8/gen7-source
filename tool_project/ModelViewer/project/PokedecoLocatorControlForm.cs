using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using gfl2.clr.renderingengine;
using gfl2.clr.renderingengine.renderer.util;

namespace ModelViewer
{
  public partial class PokedecoLocatorControlForm : Form, IModelMotionResourceChangeListener
  {
    private PokemonModelDataManager m_ModelDataManager;

    public PokedecoLocatorControlForm()
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

    private void PokedecoLocatorControlForm_Load(object sender, EventArgs e)
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

      pokedecoLocatorGridView.Rows.Clear();

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

      if (m_ModelDataManager.PokedecoLocatorInstanceNodes == null)
      {
        changeAllVisibilityCheckBox.Enabled = false;
        changeAllVisibilityCheckBox.Checked = false;
        return;
      }

      bool allVisible = true;
      for (Int32 i = 0; i < modelDataManager.PokedecoLocatorInstanceNodes.Length; ++i)
      {
        bool enabled = m_ModelDataManager.PokedecoLocatorInstanceNodes[i].IsVisible();
        var node = modelDataManager.PokedecoLocatorInstanceNodes[i];
        string name = node.GetName();
        string bbName = m_ModelDataManager.PokedecoLocatorBoundingBoxNames[i];

        pokedecoLocatorGridView.Rows.Add(new object[] { name, bbName, enabled });
        if (!enabled)
        {
          allVisible = false;
        }
      }

      changeAllVisibilityCheckBox.Enabled = true;
      changeAllVisibilityCheckBox.Checked = allVisible;
    }

    private void pokedecoLocatorGridView_CurrentCellDirtyStateChanged(object sender, EventArgs e)
    {
      if (pokedecoLocatorGridView.IsCurrentCellDirty)
      {
        pokedecoLocatorGridView.CommitEdit(DataGridViewDataErrorContexts.Commit);
      }
    }

    private void pokedecoLocatorGridView_CellValueChanged(object sender, DataGridViewCellEventArgs e)
    {
      if (e.RowIndex >= 0 && e.ColumnIndex == 1)
      {// AABBのコンボボックスの設定変更
        string boundingBoxName = this.pokedecoLocatorGridView[e.ColumnIndex, e.RowIndex].FormattedValue as string;
        if (string.IsNullOrEmpty(boundingBoxName))
        {
          return;
        }

        m_ModelDataManager.PokedecoLocatorBoundingBoxNames[e.ColumnIndex] = boundingBoxName;
        var pokeDecoTypeArray = PokemonModelDataManager.PokedecoTypeDictionary[boundingBoxName];
        for (gfl2.clr.renderingengine.PokedecoType.Enum type = 0; type < gfl2.clr.renderingengine.PokedecoType.Enum.NumberOf; ++type)
        {
          Int32 index = Array.FindIndex(pokeDecoTypeArray, data => data == type);
          m_ModelDataManager.PokedecoLocatorInstanceNodes[e.RowIndex].SetPokedecoAABBVisible(type, index >= 0);
        }
      }

      if (e.RowIndex >= 0 && e.ColumnIndex == 2)
      {
        bool isChecked = (bool)pokedecoLocatorGridView[e.ColumnIndex, e.RowIndex].Value;
        m_ModelDataManager.PokedecoLocatorInstanceNodes[e.RowIndex].SetVisible(isChecked);
      }
    }

    private void changeAllVisibilityCheckBox_CheckedChanged(object sender, EventArgs e)
    {
      if (m_ModelDataManager == null)
      {
        return;
      }

      Int32 nodeNum = m_ModelDataManager.PokedecoLocatorInstanceNodes.Length;
      for (Int32 i = 0; i < nodeNum; ++i)
      {
        pokedecoLocatorGridView[2, i].Value = changeAllVisibilityCheckBox.Checked;
      }
    }

    private bool CheckAllVisibility()
    {
      Int32 nodeNum = m_ModelDataManager.PokedecoLocatorInstanceNodes.Length;
      for (Int32 i = 0; i < nodeNum; ++i)
      {
        if ((bool)pokedecoLocatorGridView[2, i].Value == false)
        {
          return false;
        }
      }
      return true;
    }
  }
}
