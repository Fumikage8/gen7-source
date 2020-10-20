using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Forms;
using hio;

namespace MemoryVisualizer
{
  /// <summary>
  /// MainWindow.xaml の相互作用ロジック
  /// </summary>
  public partial class MainWindow : Window
  {
    private hio.ISCTRDLL dll { get; set; }
    private System.IO.StreamReader heaplog { get; set; }

    public MainWindow()
    {
      InitializeComponent();
      dll = new hio.ISCTRDLL();
    }

    private void MenuItem_OpenLogFile_Click(object sender, RoutedEventArgs e)
    {
      var dialog = new OpenFileDialog();
      dialog.Title = "ファイルを開く";
      dialog.Filter = "全てのファイル(*.*)|*.*";
      HeapViewer.heaps.Clear();

      if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
      {
        try 
        {
          heaplog = new System.IO.StreamReader(dialog.OpenFile());
        }
        catch (System.IO.IOException exception)
        {
          System.Windows.MessageBox.Show(exception.Message, "MemoryVisualizer", MessageBoxButton.OK, MessageBoxImage.Error);
          return;
        }

        HeapViewer.LoadInitialHeapDump(heaplog);
      }
    }

    private void MenuItem_OpenCCI_Click(object sender, RoutedEventArgs e)
    {
      var dialog = new OpenFileDialog();
      dialog.Title = "ファイルを開く";
      dialog.Filter = "CTR CARD IMAGE(*.cci)|*.cci";

      if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
      {
        if (!dll.IsDLLLoaded())
        {
          System.Windows.MessageBox.Show("IS-CTR DLLの読み込みに失敗しました", "MemoryVisualizer", MessageBoxButton.OK, MessageBoxImage.Error);
          return;
        }

        if (!dll.Open())
        {
          System.Windows.MessageBox.Show("ハードウェアが見つかりませんでした", "MemoryVisualizer", MessageBoxButton.OK, MessageBoxImage.Error);
          return;
        }

        if (!dll.EnableHIO())
        {
          System.Windows.MessageBox.Show("Host IOを有効にできませんでした", "MemoryVisualizer", MessageBoxButton.OK, MessageBoxImage.Error);
          return;
        }

        dll.DownloadGo(dialog.FileName);

        if (!dll.OpenSHMEM(0))
        {
          System.Windows.MessageBox.Show("共有メモリのオープンに失敗しました", "MemoryVisualizer", MessageBoxButton.OK, MessageBoxImage.Error);
          return;
        }
      }
    }

    private void StepPlayButton_Click(object sender, RoutedEventArgs e)
    {
      if (heaplog == null)
      {
        return;
      }

      StepPlay(heaplog);
    }

    private void StepPlay(System.IO.StreamReader reader)
    {
      UInt32 count;
      try
      {
        count = UInt32.Parse(MultiLoadTextBox.Text);
      }
      catch (FormatException)
      {
        System.Windows.MessageBox.Show("数値のパースに失敗しました" + Environment.NewLine + "複数行読み込みの設定を確認してください", "MemoryVisualizer", MessageBoxButton.OK, MessageBoxImage.Error);
        return;
      }

      bool skip = false;
      string line = null;
      for (var i = 0; i < UInt32.Parse(MultiLoadTextBox.Text); i++)
      {
        if ((line = reader.ReadLine()) != null)
        {
          skip = HeapViewer.LoadHeapDumpLine(line);
        }
        else
        {
          System.Windows.MessageBox.Show("ログファイルの最後まで到達しました", "MemoryVisualizer", MessageBoxButton.OK, MessageBoxImage.Information);
          return;
        }
      }

      if (skip)
      {
        StepPlay(reader);
      }
    }
  }
}
