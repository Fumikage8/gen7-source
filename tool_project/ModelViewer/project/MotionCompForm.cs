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

using System.IO;
using System.Diagnostics;

namespace ModelViewer
{
	/**
	 * @brief モーションコンフィグを設定するフォーム
	 */
  public partial class MotionCompForm : Form, IModelMotionResourceChangeListener
  {
    private PokemonModelDataManager m_ModelDataManager = null;
    private string m_ModelDirectory = string.Empty;
    private string m_EachDaePath = string.Empty;
    private Mcnf.EachDae m_EachDae = null;
    private long m_TotalSize = 0;

    /**
     * @brief コンストラクタ
     */
    public MotionCompForm()
    {
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
      if (m_ModelDataManager.PokemonModel == null)
      {
        return;
      }
      if (string.IsNullOrEmpty(m_ModelDataManager.GfmdlFilePath))
      {
        return;
      }
      if ((m_ModelDataManager.GfmotFilePaths == null) || (m_ModelDataManager.GfmotFilePaths.Length == 0))
      {
        return;
      }

      m_ModelDirectory = Path.GetDirectoryName(m_ModelDataManager.GfmdlFilePath);

      // EachDae.xmlがあるか確認
      m_EachDaePath = Path.Combine(m_ModelDirectory, "EachDae.xml");
      if (File.Exists(m_EachDaePath))
      {
        // デシリアライズ
        m_EachDae = Mcnf.EachDae.Deserialize(m_EachDaePath);
      }
      else
      {
        m_EachDae = new Mcnf.EachDae();
        m_EachDaePath = string.Empty;
      }

      // データを反映
      var gfmotPathList = m_ModelDataManager.GfmotFilePaths;
      m_TotalSize = 0;
      foreach (var gfmotPath in gfmotPathList)
      {
        if (string.IsNullOrEmpty(gfmotPath))
        {
          continue;
        }

        var fileSize = m_ModelDataManager.GfmotBinarySize(this.dataGridView1.Rows.Count);
        m_TotalSize += fileSize;

        var motionName = System.IO.Path.GetFileNameWithoutExtension(gfmotPath);
        var fileSizeKB = fileSize / 1024.0;
        var compLevel = m_EachDae.GetLevel(motionName);

        this.dataGridView1.Rows.Add(new object[] { motionName, fileSizeKB.ToString("0.00") + " KB", compLevel.ToString() });
      }

      // 合計サイズ
      this.totalSizeLabel.Text = string.Format("合計：{0} バイト", m_TotalSize.ToString("#,0"));

      // EachDae.xml
      if (!string.IsNullOrEmpty(m_EachDaePath))
      {
        this.nameComboBox.Items.Add(Path.GetFileName(m_EachDaePath));
        this.nameComboBox.SelectedIndex = 0;
      }
    }

		/**
		 * @brief 全てのコントロールを一括設定
		 */
    private void SetEnableAll(bool enabled)
    {
      this.saveButton.Enabled = enabled;
      this.nameComboBox.Enabled = enabled;
      this.dataGridView1.Enabled = enabled;

      this.nameComboBox.Items.Clear();
      this.nameComboBox.Text = string.Empty;
      this.dataGridView1.Rows.Clear();

      this.totalSizeLabel.Text = "合計：";
    }

    private void dataGridView1_CellValueChanged(object sender, DataGridViewCellEventArgs e)
    {
      if (m_ModelDataManager == null)
      {
        return;
      }
      if (m_ModelDataManager.PokemonModel == null)
      {
        return;
      }
      if (e.ColumnIndex != 2)
      {
        return;
      }

      string nijiToolDir = Environment.GetEnvironmentVariable("NIJI_TOOL_DIR");

			string animationConverterFilePath =
#if DEBUG
      Path.Combine(nijiToolDir, "AnimationConverter", "AnimationConverter.exe");
#else
			Path.GetFullPath("../AnimationConverter/AnimationConverter.exe");
			if (!File.Exists(animationConverterFilePath))
			{
				animationConverterFilePath = Path.Combine(nijiToolDir, "AnimationConverter", "AnimationConverter.exe");
			}
#endif

      // モーションを再コンバートする
			if (!File.Exists(animationConverterFilePath))
			{
        return;
			}

      m_TotalSize -= m_ModelDataManager.GfmotBinarySize(e.RowIndex);

      var compLevelString = this.dataGridView1[e.ColumnIndex, e.RowIndex].FormattedValue as string;
      var compLevel = (int)Mcnf.CompSetting.LevelDict[compLevelString];

			// モーションのバイナリコンバータを起動してバイナリサイズを計測
      var gfmotFilePath = Path.Combine(m_ModelDirectory, Path.ChangeExtension(Path.GetFileName(m_ModelDataManager.GfmotFilePath(e.RowIndex)), ".gfmot"));
      var outputFilePath = m_ModelDataManager.GfmotFilePath(e.RowIndex);
			var startInfo = new ProcessStartInfo(animationConverterFilePath);
			startInfo.WindowStyle = ProcessWindowStyle.Hidden;
      startInfo.Arguments = string.Format("-i {0} -o {1} -comp {2} -poke on", gfmotFilePath, outputFilePath, compLevel);
			Process animationConverterProcess = Process.Start(startInfo);
			animationConverterProcess.WaitForExit();

      // バイナリを読み込みなおす
      m_ModelDataManager.LoadGfmotResource(outputFilePath, e.RowIndex);

      // ファイルサイズ変更
      var fileInfo = new FileInfo(outputFilePath);
      var fileSizeKB = fileInfo.Length / 1024.0;
      this.dataGridView1[1, e.RowIndex].Value = fileSizeKB.ToString("0.00") + " KB";

      m_TotalSize += fileInfo.Length;
      //m_ModelDataManager.SetGfmotBinarySize(e.RowIndex, (int)fileInfo.Length);

      this.totalSizeLabel.Text = string.Format("合計：{0} バイト", m_TotalSize.ToString("#,0"));
    }

    private void dataGridView1_CurrentCellDirtyStateChanged(object sender, EventArgs e)
    {
      if (this.dataGridView1.CurrentCellAddress.X != 2)
      {
        return;
      }

      if (!this.dataGridView1.IsCurrentCellDirty)
      {
        return;
      }

      this.dataGridView1.CommitEdit(DataGridViewDataErrorContexts.Commit);
    }

    private void saveButton_Click(object sender, EventArgs e)
    {
      // デフォルトの保存ディレクトリをモデルのパスと同じにする
      if (!string.IsNullOrEmpty(m_ModelDataManager.GfmdlFilePath))
      {
        string initialDirectory = System.IO.Path.GetDirectoryName(m_ModelDataManager.GfmdlFilePath);
        initialDirectory = System.IO.Directory.GetParent(initialDirectory).FullName;
        this.saveFileDialog.InitialDirectory = initialDirectory;
      }

      this.saveFileDialog.FileName = "EachDae.xml";

      if (this.saveFileDialog.ShowDialog() != System.Windows.Forms.DialogResult.OK)
      {
        return;
      }

      // EachDaeをシリアライズする
      for (var i = 0; i < this.dataGridView1.Rows.Count; ++i)
      {
        var row = this.dataGridView1.Rows[i];
        var name = row.Cells[0].FormattedValue as string;
        var levelText = row.Cells[2].FormattedValue as string;
        var level = Mcnf.CompSetting.LevelDict[levelText];

        m_EachDae.ChangeLevel(name, level);
      }

      m_EachDaePath = Path.Combine(this.saveFileDialog.InitialDirectory, "EachDae.xml");
      Mcnf.EachDae.Serialize(m_EachDaePath, m_EachDae);

      this.nameComboBox.Items.Clear();
      this.nameComboBox.Items.Add(Path.GetFileName(m_EachDaePath));
      this.nameComboBox.SelectedIndex = 0;
    }

    private void dataGridView1_CellEnter(object sender, DataGridViewCellEventArgs e)
    {
      if (this.dataGridView1.Columns[e.ColumnIndex] is DataGridViewComboBoxColumn)
      {
        SendKeys.SendWait("{F4}");
      }
    }
  }
}
