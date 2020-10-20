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
  public partial class MarkingControlForm : Form, IModelMotionResourceChangeListener
  {
    private PokemonModelDataManager m_ModelDataManager;

    public MarkingControlForm()
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
      if (m_ModelDataManager == modelDataManager)
      {
        return;
      }

      // モデルが切り替わったら一旦0にしておく
      // (本来であればPokemonModelからgetter()を使うべきだが, getterを追加したくないのでこの対処)
      mark1TextBox.Text = "0";
      mark2TextBox.Text = "0";
      mark3TextBox.Text = "0";
      mark4TextBox.Text = "0";

      bool enabled = (modelDataManager != null);
      mark1TextBox.Enabled = enabled;
      mark2TextBox.Enabled = enabled;
      mark3TextBox.Enabled = enabled;
      mark4TextBox.Enabled = enabled;
      setMarkingsButton.Enabled = enabled;

      m_ModelDataManager = modelDataManager;
    }

    private bool ValidateInputMarkValue(string strValue, out Byte markValue)
    {
      bool isSucceeded = Byte.TryParse(strValue, out markValue);
      if (!isSucceeded)
      {
        return false;
      }

      return true;
    }

    private void mark1TextBox_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Enter)
      {
        mark1TextBox_Validating(sender, null);
      }
    }

    private void mark1TextBox_Validating(object sender, CancelEventArgs e)
    {
      Byte markValue = 0;
      bool isSucceeded = ValidateInputMarkValue(mark1TextBox.Text, out markValue);
      if (!isSucceeded)
      {// 失敗していたら0にしておく
        mark1TextBox.Text = "0";
      }
    }

    private void mark2TextBox_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Enter)
      {
        mark2TextBox_Validating(sender, null);
      }
    }

    private void mark2TextBox_Validating(object sender, CancelEventArgs e)
    {
      Byte markValue = 0;
      bool isSucceeded = ValidateInputMarkValue(mark2TextBox.Text, out markValue);
      if (!isSucceeded)
      {// 失敗していたら0にしておく
        mark2TextBox.Text = "0";
      }
    }

    private void mark3TextBox_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Enter)
      {
        mark3TextBox_Validating(sender, null);
      }
    }

    private void mark3TextBox_Validating(object sender, CancelEventArgs e)
    {
      Byte markValue = 0;
      bool isSucceeded = ValidateInputMarkValue(mark3TextBox.Text, out markValue);
      if (!isSucceeded)
      {// 失敗していたら0にしておく
        mark3TextBox.Text = "0";
      }
    }

    private void mark4TextBox_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Enter)
      {
        mark4TextBox_Validating(sender, null);
      }
    }

    private void mark4TextBox_Validating(object sender, CancelEventArgs e)
    {
      Byte markValue = 0;
      bool isSucceeded = ValidateInputMarkValue(mark4TextBox.Text, out markValue);
      if (!isSucceeded)
      {// 失敗していたら0にしておく
        mark4TextBox.Text = "0";
      }
    }

    private void setMarkingsButton_Click(object sender, EventArgs e)
    {
      Byte mark1 = GetMarkFrame1();
      Byte mark2 = GetMarkFrame2();
      Byte mark3 = GetMarkFrame3();
      Byte mark4 = GetMarkFrame4();

      m_ModelDataManager.PokemonModel.SetMarkingFrames(mark1, mark2, mark3, mark4);
    }

    public Byte GetMarkFrame1()
    {
      Byte mark;
      bool isSucceeded = false;

      isSucceeded = ValidateInputMarkValue(mark1TextBox.Text, out mark);
      if (!isSucceeded)
      {
        return 0;
      }

      return mark;
    }

    public Byte GetMarkFrame2()
    {
      Byte mark;
      bool isSucceeded = false;

      isSucceeded = ValidateInputMarkValue(mark2TextBox.Text, out mark);
      if (!isSucceeded)
      {
        return 0;
      }

      return mark;
    }

    public Byte GetMarkFrame3()
    {
      Byte mark;
      bool isSucceeded = false;

      isSucceeded = ValidateInputMarkValue(mark3TextBox.Text, out mark);
      if (!isSucceeded)
      {
        return 0;
      }

      return mark;
    }

    public Byte GetMarkFrame4()
    {
      Byte mark;
      bool isSucceeded = false;

      isSucceeded = ValidateInputMarkValue(mark4TextBox.Text, out mark);
      if (!isSucceeded)
      {
        return 0;
      }

      return mark;
    }
  }
}
