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
  public partial class DisplayNodeHierarchyForm : Form, IModelMotionResourceChangeListener
  {
    private PokemonModelDataManager m_PokemonModelDataManager;		//!< @brief UI上で扱うPokeonModelDatamanager

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

    public DisplayNodeHierarchyForm()
    {
      InitializeComponent();

      var nodeColorDictionary = gfl2.clr.poke_3d_lib.PokemonModel.GetTreeNodeBackColorDictionary();
      Int32 index = 0;
      foreach (var dictionary in nodeColorDictionary)
      {
        nodeColorDataGridView.Rows.Add(new object[] {dictionary.Key, ""});
        nodeColorDataGridView[1, index].Style.BackColor = dictionary.Value;
        ++index;
      }
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

      this.nodeHierarchyTreeView.Nodes.Clear();
      m_PokemonModelDataManager = modelDataManager;
      if (m_PokemonModelDataManager == null)
      {
        return;
      }

      if (m_PokemonModelDataManager.PokemonModel == null)
      {
        return;
      }

      TreeNode modelNameTreeNode = m_PokemonModelDataManager.PokemonModel.GetModelNameTreeNode();
      this.nodeHierarchyTreeView.Nodes.Add(modelNameTreeNode);
    }
  }
}
