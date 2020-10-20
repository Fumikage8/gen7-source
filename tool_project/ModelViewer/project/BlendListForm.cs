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
  public partial class BlendListForm : Form, IModelMotionResourceChangeListener
  {
    private PokemonModelDataManager m_ModelDataManager = null;
    private Mcnf.BlendList m_BlendList = null;
    private AnimationDataSetForm m_AnimationDataSetForm = null;
    private int EditRowIndex = -1;
    private int EditColumnIndex = -1;
    private DataGridViewCell m_EditedCell = null;

    /**
     * @brief コンストラクタ
     */
    public BlendListForm(AnimationDataSetForm animationDataSetForm)
    {
      m_AnimationDataSetForm = animationDataSetForm;

      InitializeComponent();

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

    private void OnResizeRawCol()
    {
      var rowCount = this.blendListDataGridView.Rows.Count;
      var colCount = this.blendListDataGridView.Columns.Count;

      // 背景色設定
      var backColor = Color.FromArgb(155, 187, 89);
      for (var col = 0; col < colCount; ++col)
      {
        this.blendListDataGridView[col, 0].Style.BackColor = backColor;

        var quotient = col / ('Z' - 'A' + 1);
        var remainder = col % ('Z' - 'A' + 1);
        var code = 'A' + remainder;
        var quotientStr = (quotient > 0)
          ? quotient.ToString()
          : "";
        this.blendListDataGridView.Columns[col].HeaderText = ((char)(code)).ToString() + quotientStr;
      }
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

      // 表示するものが何も無くなったらコントロールを全て無効にしておく
      var setDisableAll = (m_ModelDataManager != null) && (modelDataManager == null);
      m_ModelDataManager = modelDataManager;
      if (setDisableAll)
      {
        this.SetEnableAll(false);
      }
      // 表示するものが出来たらコントロールを有効にする
      else
      {
        this.SetEnableAll(true);
      }

      // データチェック
      if (m_ModelDataManager == null)
      {
        return;
      }
      if (m_ModelDataManager.ExtraFilePathSet.BlendListXmlPath == null)
      {
        return;
      }
      if (!System.IO.File.Exists(m_ModelDataManager.ExtraFilePathSet.BlendListXmlPath))
      {
        return;
      }
      if (m_ModelDataManager.PokemonModel == null)
      {
        return;
      }

      // BlendList読み込み
			m_BlendList = ExcelXml.ExcelXml.Deserialize<Mcnf.BlendList>(m_ModelDataManager.ExtraFilePathSet.BlendListXmlPath);

      // blendList名のコンボボックス設定
      this.blendListComboBox.Items.Add(System.IO.Path.GetFileName(m_ModelDataManager.ExtraFilePathSet.BlendListXmlPath));
      this.blendListComboBox.SelectedIndex = 0;

      // 要素設定
      var rowCount = m_BlendList.cells.GetLength(0);
      var colCount = m_BlendList.cells.GetLength(1);
      this.blendListDataGridView.Columns.Clear();
      this.blendListDataGridView.Rows.Clear();
      for (var col = 0; col < colCount; ++col)
      {
        this.blendListDataGridView.Columns.Add("", "");
      }
      for (var row = 0; row < rowCount; ++row)
      {
        var cells = new string[colCount];
        for (var col = 0; col < colCount; ++col)
        {
          cells[col] = m_BlendList.cells[row, col];
        }
        this.blendListDataGridView.Rows.Add(cells);
      }

      this.OnResizeRawCol();
    }

		/**
		 * @brief 全てのコントロールを一括設定
		 */
    private void SetEnableAll(bool enabled)
    {
      this.blendListComboBox.Enabled = enabled;
      this.blendListSaveButton.Enabled = enabled;
      this.blendListDataGridView.Enabled = enabled;

      this.blendListComboBox.Text = string.Empty;
      this.blendListComboBox.Items.Clear();
      this.blendListDataGridView.Rows.Clear();
    }

    // 保存ボタンクリック
    private void blendListSaveButton_Click(object sender, EventArgs e)
    {
      // デフォルトの保存ディレクトリをモデルのパスと同じにする
      if (!string.IsNullOrEmpty(m_ModelDataManager.GfmdlFilePath))
      {
        string initialDirectory = System.IO.Path.GetDirectoryName(m_ModelDataManager.GfmdlFilePath);
        initialDirectory = System.IO.Directory.GetParent(initialDirectory).FullName;
        this.blendListSaveFileDialog.InitialDirectory = initialDirectory;
      }

      if (this.blendListSaveFileDialog.ShowDialog() != System.Windows.Forms.DialogResult.OK)
      {
        return;
      }

      // Mcnf作成
      //var rootNode = this.CreateMcnfFromCell();

      // 行数を計算
      var rowCount = this.blendListDataGridView.RowCount;
      var hasValue = false;
      for (var col = 0; col < this.blendListDataGridView.Columns.Count; ++col)
      {
        var value = this.blendListDataGridView[col, rowCount - 1].Value as string;

        if (!string.IsNullOrEmpty(value))
        {
          hasValue = true;
        }
      }
      if (!hasValue)
      {
        --rowCount;
      }

      // シリアライズ
      m_BlendList.ResizeCells(rowCount, this.blendListDataGridView.ColumnCount);
      for (var row = 0; row < m_BlendList.Cell2d.GetLength(0); ++row)
      {
        for (var col = 0; col < m_BlendList.Cell2d.GetLength(1); ++col)
        {
          m_BlendList.Cell2d[row, col] = this.blendListDataGridView[col, row].Value as string;
        }
      }
      ExcelXml.ExcelXml.Serialize<Mcnf.BlendList>(this.blendListSaveFileDialog.FileName, m_BlendList);

      // BlendList読み込み
      // blendList名のコンボボックス設定
      m_ModelDataManager.ExtraFilePathSet.BlendListXmlPath = this.blendListSaveFileDialog.FileName;
      this.blendListComboBox.Items.Clear();
      this.blendListComboBox.Text = string.Empty;
      this.blendListComboBox.Items.Add(System.IO.Path.GetFileName(m_ModelDataManager.ExtraFilePathSet.BlendListXmlPath));
      this.blendListComboBox.SelectedIndex = 0;
    }

    private void blendListDataGridView_CellPainting(object sender, DataGridViewCellPaintingEventArgs e)
    {
      // 一番左だけ
      if (e.ColumnIndex >= 0 || e.RowIndex < 0)
      {
        return;
      }

      // セルを描画する
      e.Paint(e.ClipBounds, DataGridViewPaintParts.All);

      // 行番号を表示する範囲を決める
      var indexRect = e.CellBounds;
      indexRect.Inflate(-2, -2);

      // 行番号を表示する
      TextRenderer.DrawText(
        e.Graphics,
        (e.RowIndex + 1).ToString(),
        e.CellStyle.Font,
        indexRect,
        e.CellStyle.ForeColor,
        TextFormatFlags.Right | TextFormatFlags.VerticalCenter);

      // 描画が完了したことを知らせる
      e.Handled = true;
    }

    private void blendListDataGridView_CellValidating(object sender, DataGridViewCellValidatingEventArgs e)
    {
      if (!this.blendListDataGridView.Enabled)
      {
        return;
      }

      // 数値部分以外は無視
      if (e.ColumnIndex < 1 || e.RowIndex < 1)
      {
        return;
      }

      // 空文字はOK
      if (string.IsNullOrEmpty((string)e.FormattedValue))
      {
        return;
      }

      // int型に変換できなければ入力をキャンセルする
      var result = 0;
      if (int.TryParse((string)e.FormattedValue, out result))
      {
        // マイナスはだめ
        if (result >= 0)
        {
          return;
        }
      }
      

      // 入力をキャンセル
      this.blendListDataGridView.CancelEdit();
      e.Cancel = true;
    }

    private void blendListDataGridView_KeyDown(object sender, KeyEventArgs e)
    {
      if (!this.blendListDataGridView.Enabled)
      {
        return;
      }

      if (e.KeyCode != Keys.Delete)
      {
        return;
      }

      this.blendListDataGridView.CurrentCell.Value = "";
    }

    private void blendListDataGridView_CellStateChanged(object sender, DataGridViewCellStateChangedEventArgs e)
    {
      if (!this.blendListDataGridView.Enabled)
      {
        return;
      }

      if (!e.Cell.Selected)
      {
        return;
      }

      if (e.Cell.RowIndex <= 0 || e.Cell.ColumnIndex <= 0)
      {
        return;
      }
      if (e.Cell.RowIndex >= this.blendListDataGridView.Rows.Count)
      {
        return;
      }
      if (e.Cell.ColumnIndex >= this.blendListDataGridView.Columns.Count)
      {
        return;
      }

      // 開始と終了アニメを設定
      var fromAnimName = this.blendListDataGridView[0, e.Cell.RowIndex].Value as string;
      var toAnimName = this.blendListDataGridView[e.Cell.ColumnIndex, 0].Value as string;

      // 空白なら無視
      if (string.IsNullOrEmpty(fromAnimName) || string.IsNullOrEmpty(toAnimName))
      {
        return;
      }

      var blendFrame = 0;
      var value = this.blendListDataGridView[e.Cell.ColumnIndex, e.Cell.RowIndex].Value as string;
      if (!int.TryParse(value, out blendFrame))
      {
        // allの値を使う
        value = this.blendListDataGridView[e.Cell.ColumnIndex, 1].Value as string;
        int.TryParse(value, out blendFrame);
      }

      var oldIndex = m_AnimationDataSetForm.GetMotionIndex(0);
      m_AnimationDataSetForm.SetMotionClear();
      if (e.Cell.RowIndex > 1)
      {
        m_AnimationDataSetForm.SetMotionChangeString(0, fromAnimName);
      }
      else
      {
        m_AnimationDataSetForm.SetMotionChangeIndex(0, oldIndex);
      }
      m_AnimationDataSetForm.SetMotionChangeString(1, toAnimName);
      m_AnimationDataSetForm.SetBlendFrame(0, blendFrame);

      // １，２番のモーションを有効に設定する
      for (Int32 i = 0; (i < (Int32)m_AnimationDataSetForm.AnimationNum) && (i < 2); ++i)
      {
        m_AnimationDataSetForm.SetCkeckToMotionEnabledCheckBox(i, true);
      }

      // 残りのモーションは全て無効に設定する
      for (Int32 i = 2; i < (Int32)m_AnimationDataSetForm.AnimationNum; ++i)
      {
        m_AnimationDataSetForm.SetCkeckToMotionEnabledCheckBox(i, false);
      }
    }

    private void blendListDataGridView_CellEndEdit(object sender, DataGridViewCellEventArgs e)
    {
      // 現在のセルの位置を保存しておき、下に下げられても直ぐに戻す
      m_EditedCell = this.blendListDataGridView.CurrentCell;
    }

    private void blendListDataGridView_SelectionChanged(object sender, EventArgs e)
    {
      if (m_EditedCell == null)
      {
        return;
      }

      // 編集前のセルに戻す
      this.blendListDataGridView.CurrentCell = m_EditedCell;
      m_EditedCell = null;
    }
  }
}
