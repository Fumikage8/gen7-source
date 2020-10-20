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
	/**
	 * @brief モーションコンフィグを設定するフォーム
	 */
  public partial class McnfForm : Form, IModelMotionResourceChangeListener
  {
    private PokemonModelDataManager m_ModelDataManager = null;
    private DataGridViewCell m_InterestEditedCell = null;
    private bool m_InterestFirstClick = false;

    /**
     * @brief コンストラクタ
     */
    public McnfForm()
    {
      InitializeComponent();

      // 注視タイプのコンボボックス設定
      this.interestType.DataSource = Enum.GetValues(typeof(Mcnf.INTEREST_NODE.InterestType));
      this.interestType.ValueType = typeof(Mcnf.INTEREST_NODE.InterestType);

      // エクスプレッションのコンボボックス設定
      this.expressionType.DataSource = Enum.GetValues(typeof(Mcnf.EXPRESSION.AimType));
      this.expressionType.ValueType = typeof(Mcnf.EXPRESSION.AimType);

      // コントロールを全て無効に
      this.SetEnableAll(false);
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
		 * @brief PokemonModelDataManagerの変更通知受け取り
		 */
    public void ReceiveNotification(PokemonModelDataManager modelDataManager)
    {
      // 変更されていないなら無視
      if (m_ModelDataManager == modelDataManager)
      {
        return;
      }

      // モデルが切り替わったらMcnfがあるかのデータチェック
      var hasMcnfData = (modelDataManager != null) && (modelDataManager.McnfRootNodeList.Count > 0) && (modelDataManager.PokemonModel != null);

      // 表示するものがあればコントロールを有効にする
      this.SetEnableAll(hasMcnfData);

      m_ModelDataManager = modelDataManager;

      if (modelDataManager != null)
      {
        this.mcnfCreate.Enabled = true;
      }

      // 表示するものがない
      if (!hasMcnfData)
      {
        return;
      }

      // mcnf名のコンボボックス設定
      foreach (var mcnf in m_ModelDataManager.McnfRootNodeList)
      {
        this.mcnfComboBox.Items.Add(System.IO.Path.GetFileName(mcnf.FileName));
      }
      this.mcnfComboBox.SelectedIndex = 0;
    }

		/**
		 * @brief 全てのコントロールを一括設定
		 */
    private void SetEnableAll(bool enabled)
    {
      this.mcnfComboBox.Enabled = enabled;
      this.mcnfSaveButton.Enabled = enabled;
      this.mcnfTabControl.Enabled = enabled;
      this.interestGridView.Enabled = enabled;
      this.springGridView.Enabled = enabled;
      this.expressionGridView.Enabled = enabled;
      this.mcnfCreate.Enabled = enabled;
      this.checkBoxSpring.Enabled = enabled;

      this.mcnfComboBox.Items.Clear();
      this.mcnfComboBox.Text = string.Empty;

      this.ClearGridView();
    }

    private void ClearGridView()
    {
      this.interestGridView.Rows.Clear();
      this.springGridView.Rows.Clear();
      this.expressionGridView.Rows.Clear();

      this.interestJointName.Items.Clear();
      this.springJointName.Items.Clear();
      this.expressionSrc.Items.Clear();
      this.expressionDst.Items.Clear();
    }

		/**
		 * @brief データビューリストに値を反映
		 */
    private void ApplyDataViewList(Mcnf.Mcnf rootNode, JointInstanceNode[] jointList)
    {
      if (rootNode == null)
      {
        return;
      }

      m_ModelDataManager.AddMotionConfigResource(rootNode.FileName);

      // ジョイント名のコンボボックス設定
      foreach (var joint in jointList)
      {
        var name = joint.GetName();
        this.interestJointName.Items.Add(name);
        this.springJointName.Items.Add(name);
        this.expressionSrc.Items.Add(name);
        this.expressionDst.Items.Add(name);
      }

      // 注視
      if (rootNode.INTEREST != null)
      {
        foreach (var interestNode in rootNode.INTEREST.NODE)
        {
          this.interestGridView.Rows.Add(new object[]{interestNode._Name, interestNode._Type, interestNode._Low, interestNode._High, interestNode._Low2, interestNode._High2, interestNode._Speed});
        }
      }

      // バネ
      if (rootNode.SPRING != null)
      {
        foreach (var springNode in rootNode.SPRING)
        {
          this.springGridView.Rows.Add(new object[]{springNode._Name, springNode._Damping, springNode._Stiffness, springNode._Mass, springNode._Gravity});
        }
      }

      // エクスプレッション
      if (rootNode.EXPRESSION != null)
      {
        foreach (var expressionNode in rootNode.EXPRESSION)
        {
          this.expressionGridView.Rows.Add(new object[]{expressionNode._Type, expressionNode._Src0, expressionNode._Dst});
        }
      }
    }

    private Mcnf.Mcnf CreateMcnfFromCell()
    {
      // 既にファイルが存在するならデシリアライズ、無いなら新規作成
      var rootNode = (System.IO.File.Exists(this.mcnfSaveFileDialog.FileName))
        ? Mcnf.Mcnf.Deserialize(this.mcnfSaveFileDialog.FileName)
        : new Mcnf.Mcnf();
      rootNode.FileName = this.mcnfSaveFileDialog.FileName;

      // 書き換えるデータがあるなら初期化しておく
      // 注視
      if (this.interestGridView.Rows.Count > 0)
      {
        rootNode.INTEREST = new Mcnf.INTEREST();
      }
      // バネ
      if (this.springGridView.Rows.Count > 0)
      {
        rootNode.SPRING = new List<Mcnf.SPRING>();
      }
      // エクスプレッション
      if (this.expressionGridView.Rows.Count > 0)
      {
        rootNode.EXPRESSION = new List<Mcnf.EXPRESSION>();
      }

      // 注視
      for (var i = 0; i < this.interestGridView.Rows.Count; ++i)
      {
        var interestRow = this.interestGridView.Rows[i];

        var interestNode = new Mcnf.INTEREST_NODE();
        interestNode._Name = (string)interestRow.Cells[0].Value;
        interestNode._Type = (Mcnf.INTEREST_NODE.InterestType)interestRow.Cells[1].Value;
        interestNode._Low = int.Parse(interestRow.Cells[2].Value.ToString());
        interestNode._High = int.Parse(interestRow.Cells[3].Value.ToString());
        interestNode._Low2 = int.Parse(interestRow.Cells[4].Value.ToString());
        interestNode._High2 = int.Parse(interestRow.Cells[5].Value.ToString());
        interestNode._Speed = int.Parse(interestRow.Cells[6].Value.ToString());

        rootNode.INTEREST.NODE.Add(interestNode);
      }

      // バネ
      for (var i = 0; i < this.springGridView.Rows.Count; ++i)
      {
        var springRow = this.springGridView.Rows[i];

        var springNode = new Mcnf.SPRING();
        springNode._Name = (string)springRow.Cells[0].Value;
        springNode._Damping = int.Parse(springRow.Cells[1].Value.ToString());
        springNode._Stiffness = int.Parse(springRow.Cells[2].Value.ToString());
        springNode._Mass = int.Parse(springRow.Cells[3].Value.ToString());
        springNode._Gravity = int.Parse(springRow.Cells[4].Value.ToString());

        rootNode.SPRING.Add(springNode);
      }

      // エクスプレッション
      for (var i = 0; i < this.expressionGridView.Rows.Count; ++i)
      {
        var expressionRow = this.expressionGridView.Rows[i];

        var expressionNode = new Mcnf.EXPRESSION();
        expressionNode._Type = (Mcnf.EXPRESSION.AimType)expressionRow.Cells[0].Value;
        expressionNode._Src0 = (string)expressionRow.Cells[1].Value;
        expressionNode._Dst = (string)expressionRow.Cells[2].Value;

        rootNode.EXPRESSION.Add(expressionNode);
      }

      return rootNode;
    }

    // 保存ダイアログを開く
    private bool OpenSaveDialog()
    {
      // デフォルトの保存ディレクトリをモデルのパスと同じにする
      if (!string.IsNullOrEmpty(m_ModelDataManager.GfmdlFilePath))
      {
        string initialDirectory = System.IO.Path.GetDirectoryName(m_ModelDataManager.GfmdlFilePath);
        initialDirectory = System.IO.Directory.GetParent(initialDirectory).FullName;
        this.mcnfSaveFileDialog.InitialDirectory = initialDirectory;
      }

      // デフォルトのファイル名を現在のファイル名にする
      this.mcnfSaveFileDialog.FileName = (!string.IsNullOrEmpty(m_ModelDataManager.ExtraFilePathSet.McnfPathList[0]))
        ? System.IO.Path.GetFileName(m_ModelDataManager.ExtraFilePathSet.McnfPathList[0])
        : (!string.IsNullOrEmpty(m_ModelDataManager.GfmdlFilePath))
          ? System.IO.Path.GetFileNameWithoutExtension(m_ModelDataManager.GfmdlFilePath) + "_motion_kw.mcnf"
          : "model_name.mcnf";

      if (this.mcnfSaveFileDialog.ShowDialog() != System.Windows.Forms.DialogResult.OK)
      {
        return false;
      }

      return true;
    }

    // 保存ボタンクリック
    private void mcnfSaveButton_Click(object sender, EventArgs e)
    {
      // 保存ダイアログを開く
      if (!this.OpenSaveDialog())
      {
        return;
      }

      // Mcnf作成
      var rootNode = this.CreateMcnfFromCell();

      // シリアライズ
      Mcnf.Mcnf.Serialize(this.mcnfSaveFileDialog.FileName, rootNode);
    }

    // 入力値の評価
    private static void CellValueValidating(DataGridView dataGridView, DataGridViewCellValidatingEventArgs e)
    {
      var currentCell = dataGridView.CurrentCell;

      // テキストボックスカラムのみ判定
      if (!(dataGridView.Columns[currentCell.ColumnIndex] is DataGridViewTextBoxColumn))
      {
        return;
      }

      // int型に変換できなければ入力をキャンセルする
      var result = 0;
      if (int.TryParse((string)currentCell.EditedFormattedValue, out result))
      {
        return;
      }

      // 入力をキャンセル
      dataGridView.CancelEdit();
      e.Cancel = true;
    }

    // 注視データグリッドビューの入力値の評価
    private void interestGridView_CellValidating(object sender, DataGridViewCellValidatingEventArgs e)
    {
      if (!this.interestGridView.Enabled)
      {
        return;
      }

      // モデルデータマネージャが設定されていなければ無視
      if (m_ModelDataManager == null)
      {
        return;
      }
      if (m_ModelDataManager.PokemonModel == null)
      {
        return;
      }

      CellValueValidating(this.interestGridView, e);

    }

    // バネデータグリッドビューの入力値の評価
    private void springGridView_CellValidating(object sender, DataGridViewCellValidatingEventArgs e)
    {
      if (!this.springGridView.Enabled)
      {
        return;
      }

      // モデルデータマネージャが設定されていなければ無視
      if (m_ModelDataManager == null)
      {
        return;
      }

      CellValueValidating(this.springGridView, e);

      // データをモデルに反映
    }

    // エクスプレッションデータグリッドビューの入力値の評価
    private void expressionGridView_CellValidating(object sender, DataGridViewCellValidatingEventArgs e)
    {
      if (!this.expressionGridView.Enabled)
      {
        return;
      }

      // モデルデータマネージャが設定されていなければ無視
      if (m_ModelDataManager == null)
      {
        return;
      }

      CellValueValidating(this.expressionGridView, e);

      // データをモデルに反映
    }

    private void mcnfCreate_Click(object sender, EventArgs e)
    {
      // モデルデータマネージャが設定されていなければ無視
      if (m_ModelDataManager == null)
      {
        return;
      }

      var jointNodeList = m_ModelDataManager.PokemonModel.GetJointInstanceNodeList();
      if (jointNodeList == null)
      {
        return;
      }

      var rootNode = Mcnf.McnfUtil.CreateMcnf(jointNodeList);
      if (rootNode == null)
      {
        return;
      }

      // 保存ダイアログを開く
      if (!this.OpenSaveDialog())
      {
        return;
      }

      // シリアライズ
      rootNode.FileName = this.mcnfSaveFileDialog.FileName;
      Mcnf.Mcnf.Serialize(this.mcnfSaveFileDialog.FileName, rootNode);

      // コンボボックス初期化
      if (this.mcnfComboBox.Items.Count == 0)
      {
        this.SetEnableAll(true);
      }

      // mcnf名のコンボボックス設定
      m_ModelDataManager.McnfRootNodeList.Add(rootNode);
      this.mcnfComboBox.Items.Add(System.IO.Path.GetFileName(rootNode.FileName));
      this.mcnfComboBox.SelectedIndex = this.mcnfComboBox.Items.Count - 1;
    }

    private void mcnfComboBox_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (this.mcnfComboBox.SelectedIndex < 0)
      {
        return;
      }

      this.ClearGridView();

      // UIに反映
      this.ApplyDataViewList(m_ModelDataManager.McnfRootNodeList[this.mcnfComboBox.SelectedIndex], m_ModelDataManager.PokemonModel.GetJointInstanceNodeList());
    }

    private void checkBoxSpring_CheckedChanged(object sender, EventArgs e)
    {
      m_ModelDataManager.IsSpringEnable = this.checkBoxSpring.Checked;
    }

    private void applyMcnf()
    {
      // データをモデルに反映
      var rootNode = this.CreateMcnfFromCell();

      if (rootNode == null)
      {
        return;
      }

      if (rootNode.INTEREST == null)
      {
        return;
      }

      if (rootNode.INTEREST.NODE.Count == 0)
      {
        return;
      }

      // 注視設定
      var pokemonModel = m_ModelDataManager.PokemonModel;
      for (var i = 0; i < rootNode.INTEREST.NODE.Count; ++i)
      {
        var node = rootNode.INTEREST.NODE[i];
        var ui = (uint)i;

        pokemonModel.SetInterestType(ui, (gfl2.clr.renderingengine.InterestType.Enum)((int)node._Type));

        pokemonModel.SetInterestLimitAngleLow(ui, (float)(node._Low / 180.0 * Math.PI));
        pokemonModel.SetInterestLimitAngleHigh(ui, (float)(node._High / 180.0 * Math.PI));
        pokemonModel.SetInterestLimitAngleLow2(ui, (float)(node._Low2 / 180.0 * Math.PI));
        pokemonModel.SetInterestLimitAngleHigh2(ui, (float)(node._High2 / 180.0 * Math.PI));
        pokemonModel.SetInterestSpeed(ui, (float)(node._Speed * 0.01 / 180.0 * Math.PI));
      }
    }

    private void interestGridView_CellEndEdit(object sender, DataGridViewCellEventArgs e)
    {
      if (this.interestGridView.RowCount == 1)
      {
        return;
      }

      if (this.interestGridView.Columns[e.ColumnIndex] is DataGridViewComboBoxColumn)
      {
        return;
      }

      // 現在のセルの位置を保存しておき、下に下げられても直ぐに戻す
      m_InterestEditedCell = this.interestGridView.CurrentCell;
    }

    private void interestGridView_SelectionChanged(object sender, EventArgs e)
    {
      if (m_InterestEditedCell == null)
      {
        return;
      }

      // 編集前のセルに戻す
      this.interestGridView.CurrentCell = m_InterestEditedCell;
      m_InterestEditedCell = null;
    }

    private void interestGridView_CellEnter(object sender, DataGridViewCellEventArgs e)
    {
      // このイベントは仕様として古いセルも呼ばれることがあるのでフォーカスが移って初回クリックかどうかを判定
      if (m_InterestFirstClick && (e.ColumnIndex == 0))
      {
        m_InterestFirstClick = false;
        return;
      }

      if (this.interestGridView.Columns[e.ColumnIndex] is DataGridViewComboBoxColumn)
      {
        try
        {
          SendKeys.SendWait("{F4}");
        }
        catch(Win32Exception) // SendKeys throws Win32Exception on Windows service process
        {
          // do nothing
        }
      }
    }

    private void interestGridView_CellValueChanged(object sender, DataGridViewCellEventArgs e)
    {
      // 現在のデータを反映
      this.applyMcnf();
    }

    private void interestGridView_CurrentCellDirtyStateChanged(object sender, EventArgs e)
    {
      if (!(this.interestGridView.Columns[this.interestGridView.CurrentCell.ColumnIndex] is DataGridViewComboBoxColumn))
      {
        return;
      }

      this.interestGridView.CommitEdit(DataGridViewDataErrorContexts.Commit);
    }

    private void mcnfTabControl_Enter(object sender, EventArgs e)
    {
      m_InterestFirstClick = true;
    }
  }
}
