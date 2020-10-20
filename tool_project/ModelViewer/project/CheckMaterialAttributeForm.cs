using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using gfl2.clr.poke_3d_lib;

namespace ModelViewer
{
  public partial class CheckMaterialAttributeForm : Form, IModelMotionResourceChangeListener
  {
    private PokemonModelDataManager m_PokemonModelDataManager;

    //!< @brief マテリアル名をキートした、マテリアルの情報が格納されている連想配列
    private Dictionary<string, PokemonModelMaterialAttribute> m_PokemonModelMaterialAttributeDictionary;

    //!< @brief 参照テーブルの番号にあわせた説明文
    private readonly Dictionary<ReferenceTableTexturePaths.ReferenceTable, string> s_ReferenceTableCommentDictionary =
      new Dictionary<ReferenceTableTexturePaths.ReferenceTable, string>
      {
        { ReferenceTableTexturePaths.ReferenceTable.RR, "参照テーブル(R)" },
        { ReferenceTableTexturePaths.ReferenceTable.RG, "参照テーブル(G)" },
        { ReferenceTableTexturePaths.ReferenceTable.RB, "参照テーブル(B)" }
      };

    //!< @brief カリングの設定にあわせた説明文
    private readonly Dictionary<string, string> s_CullingCommentDictionary =
      new Dictionary<string, string>
      {
        { "None", "両面表示" },
        { "CCW", "表面カリング" },
        { "CW", "裏面カリング" }
      };

    public CheckMaterialAttributeForm()
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

    public void ReceiveNotification(PokemonModelDataManager modelDataManager)
    {
      if (m_PokemonModelDataManager == modelDataManager)
      {
        return;
      }

      m_PokemonModelDataManager = modelDataManager;
      materialAttributeDataGridView.Rows.Clear();
      selectMaterialTreeView.Nodes.Clear();

      if (m_PokemonModelDataManager == null)
      {
        m_PokemonModelMaterialAttributeDictionary = null;
        return;
      }

      m_PokemonModelMaterialAttributeDictionary = m_PokemonModelDataManager.PokemonModel.CreatePokemonModelMaterialAttributes();

      // ツリービューに各マテリアルの名前を追加
      foreach (var attribute in m_PokemonModelMaterialAttributeDictionary)
      {
        TreeNode treeNode = new TreeNode(attribute.Key);
        selectMaterialTreeView.Nodes.Add(treeNode);
      }
      selectMaterialTreeView.SelectedNode = selectMaterialTreeView.Nodes[0];

      // 選択されている状態の最初のノードの情報を表示
      var materialAttribute = m_PokemonModelMaterialAttributeDictionary.First().Value;
    }

    private void selectMaterialTreeView_AfterSelect(object sender, TreeViewEventArgs e)
    {
      // 現在表示されているノード情報を消去してから, 選択されたノードの情報を表示
      materialAttributeDataGridView.Rows.Clear();
      string materialName = e.Node.FullPath;
      var materialAttribute = m_PokemonModelMaterialAttributeDictionary[materialName];
      materialAttributeDataGridView.Rows.Add(new object[] { "プリセット名", m_PokemonModelDataManager.PresetnameDictionary[materialName] });
      materialAttributeDataGridView.Rows.Add(new object[] { "IDエッジ番号", materialAttribute.m_IdEdgeNo });
      materialAttributeDataGridView.Rows.Add(new object[] { "ライトセット番号", materialAttribute.m_LightSetNo });
      materialAttributeDataGridView.Rows.Add(new object[] { "カリング設定", s_CullingCommentDictionary[materialAttribute.m_CullModeName] });

      // 参照テーブルのテクスチャのパスを記載
      var referenceTable = m_PokemonModelDataManager.ReferenceTableDictionary[materialName];
      foreach (ReferenceTableTexturePaths.ReferenceTable type in Enum.GetValues(typeof(ReferenceTableTexturePaths.ReferenceTable)))
      {
        string comment = s_ReferenceTableCommentDictionary[type];
        string fileName = referenceTable[type];
        materialAttributeDataGridView.Rows.Add(new object[] { comment, fileName });
      }
    }
  }
}
