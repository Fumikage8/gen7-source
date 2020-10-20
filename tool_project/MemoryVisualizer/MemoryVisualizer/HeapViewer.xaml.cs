using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
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

namespace MemoryVisualizer
{
  /// <summary>
  /// HeapView.xaml の相互作用ロジック
  /// </summary>
  public partial class HeapViewer : UserControl
  {
    public string filename { get; set; }
    public ulong totalsize { get; set; }
    public FreeSize freesize { get; set; }
    public ulong head { get; set; }
    public ulong address { get; set; }
    private int colorIndex = 0;
    private ObservableDictionary<string, Color> colorDictionary;
    public ObservableCollection<Heap> heaps;
    public ObservableCollection<Log> logs;
    public ObservableCollection<String> skiplist;
    public readonly HashSet<Color> palet = new HashSet<Color>() 
    {
      Colors.PeachPuff,
      Colors.PaleTurquoise,
      Colors.NavajoWhite,
      Colors.Pink,
      Colors.Wheat,
      Colors.PaleGoldenrod,
      Colors.LightPink,
      Colors.PowderBlue,
      Colors.Thistle,
      Colors.LightBlue,
      Colors.Khaki,
      Colors.Violet,
      Colors.Plum,
      Colors.Aquamarine,
      Colors.LightSteelBlue,
      Colors.LightSkyBlue,
      Colors.Silver,
      Colors.SkyBlue,
      Colors.PaleGreen,
      Colors.Orchid,
      Colors.BurlyWood,
      Colors.HotPink,
      Colors.LightSalmon,
      Colors.Tan,
      Colors.LightGreen,
      Colors.Cyan,
      Colors.Magenta,
      Colors.Yellow,
      Colors.DarkSalmon,
      Colors.SandyBrown,
      Colors.LightCoral,
      Colors.Turquoise,
      Colors.Salmon,
      Colors.CornflowerBlue,
      Colors.MediumTurquoise,
      Colors.MediumOrchid,
      Colors.DarkKhaki,
      Colors.MediumPurple,
      Colors.PaleVioletRed,
      Colors.MediumAquamarine,
      Colors.GreenYellow,
      Colors.RosyBrown,
      Colors.Gold,
      Colors.DarkSeaGreen,
      Colors.MediumSlateBlue,
      Colors.Coral,
      Colors.DeepSkyBlue,
      Colors.DodgerBlue,
      Colors.Tomato,
      Colors.DeepPink,
      Colors.Orange,
      Colors.DarkTurquoise,
      Colors.Goldenrod,
      Colors.CadetBlue,
      Colors.YellowGreen,
      Colors.BlueViolet,
      Colors.DarkOrchid,
      Colors.MediumSpringGreen,
      Colors.SlateBlue,
      Colors.Peru,
      Colors.RoyalBlue,
      Colors.DarkOrange,
      Colors.IndianRed,
      Colors.Chartreuse,
      Colors.SpringGreen,
      Colors.LightSeaGreen,
      Colors.SteelBlue,
      Colors.LawnGreen,
      Colors.DarkViolet,
      Colors.MediumVioletRed,
      Colors.MediumSeaGreen,
      Colors.Chocolate,
      Colors.DarkGoldenrod,
      Colors.OrangeRed,
      Colors.LimeGreen,
      Colors.Crimson,
      Colors.Sienna,
      Colors.OliveDrab,
      Colors.DarkMagenta,
      Colors.DarkCyan,
      Colors.SeaGreen,
      Colors.DarkSlateBlue,
      Colors.Teal,
      Colors.Olive,
      Colors.Purple,
      Colors.Blue,
      Colors.Lime,
      //Colors.Red, // reserved
      Colors.Brown,
      Colors.Firebrick,
      Colors.DarkOliveGreen,
      Colors.SaddleBrown,
      Colors.ForestGreen,
      Colors.MediumBlue,
      Colors.Indigo,
      Colors.MidnightBlue,
      Colors.DarkBlue,
      Colors.DarkRed,
      Colors.Maroon,
      Colors.Navy,
      Colors.Green,
      Colors.DarkGreen,
    };

    public HeapViewer()
    {
      InitializeComponent();

      DataContext = this;
      heaps = new ObservableCollection<Heap>();
      logs = new ObservableCollection<Log>();
      freesize = new FreeSize();
      skiplist = new ObservableCollection<string>();

      {
        var binding = new Binding() { Source = heaps };
        heapBar.SetBinding(ItemsControl.ItemsSourceProperty, binding);
      }

      colorDictionary = new ObservableDictionary<string, Color>();
      colorDictionary.Add("free", Colors.Gray);

      {
        var binding = new Binding() { Source = colorDictionary };
        colorRef.SetBinding(ItemsControl.ItemsSourceProperty, binding);
      }

      {
        var binding = new Binding() { Source = logs };
        LogConsole.SetBinding(ItemsControl.ItemsSourceProperty, binding);
      }

      {
        var binding = new Binding() { Source = skiplist };
        SkipListItemsControl.SetBinding(ItemsControl.ItemsSourceProperty, binding);
      }
    }

    private void PaletLine_MouseEnter(object sender, MouseEventArgs e)
    {
      foreach (var heap in heaps)
      {
        if(heap.color == ((SolidColorBrush)((Line)((DockPanel)sender).FindName("SampleColor")).Stroke).Color)
        {
          heap.color = Colors.Red;
        }
      }
    }

    private void PaletLine_MouseLeave(object sender, MouseEventArgs e)
    {
      foreach (var heap in heaps)
      {
        if (heap.color == Colors.Red)
        {
          heap.color = ((SolidColorBrush)((Line)((DockPanel)sender).FindName("SampleColor")).Stroke).Color;
        }
      }
    }

    public bool LoadInitialHeapDump(System.IO.StreamReader logstream)
    {
      // ヒープが作成されたファイルの名前を取得
      {
        string line;
        string pattern = @"HeapCreated\s*:\s*(\S+)";
        while ((line = logstream.ReadLine()) != null)
        {
          logs.Add(new Log() { value = line, linenum = 0 });
          if (Regex.IsMatch(line, pattern))
          {
            filename = Regex.Match(line, pattern).Groups[1].Value;
            break;
          }
        }
      }

      // ヒープの先頭アドレス取得
      {
        string line;
        string pattern = @"Address\s*:\s*(0x[\dabcdef]+)";
        while ((line = logstream.ReadLine()) != null)
        {
          logs.Add(new Log() { value = line, linenum = 0 });
          if (Regex.IsMatch(line, pattern))
          {
            address = Convert.ToUInt64(Regex.Match(line, pattern).Groups[1].Value, 16);
            break;
          }
        }
      }

      // ヒープの合計サイズ取得
      {
        string line;
        string pattern = @"TotalSize\s*:\s*(0x[\dabcdef]+)";
        while ((line = logstream.ReadLine()) != null)
        {
          logs.Add(new Log() { value = line, linenum = 0 });
          if (Regex.IsMatch(line, pattern))
          {
            totalsize = Convert.ToUInt64(Regex.Match(line, pattern).Groups[1].Value, 16);
            break;
          }
        }
      }

      // ヒープの空きサイズ取得
      {
        string line;
        string pattern = @"FreeSize\s*:\s*(0x[\dabcdef]+)";
        while ((line = logstream.ReadLine()) != null)
        {
          logs.Add(new Log() { value = line, linenum = 0 });
          if (Regex.IsMatch(line, pattern))
          {
            freesize.current = Convert.ToUInt64(Regex.Match(line, pattern).Groups[1].Value, 16);
            break;
          }
        }
      }

      // ログを1行ずつ読み込んでパース
      {
        string line;
        while ((line = logstream.ReadLine()) != null)
        {
          logs.Add(new Log() { value = line, linenum = 0 });
          if (line == "-----------------CTRExpHeap Dump-----------------")
          {
            break;
          }

          ParseHeapInfo(line);
        }
      }

      if (heaps.Count == 0) { return false; }

      // 先頭アドレス取得
      head = heaps[0].address;

      return true;
    }

    public bool LoadHeapDumpLine(string line)
    {
      logs.Add(new Log() { value = line, linenum = 0 });
      LogScrollViewer.ScrollToBottom();

      // ALC... ログの処理
      {
        string pattern = @"ALC\s+(0x[\dabcdef]+)\s+(0x[\dabcdef]+)\s+(\S+)\s+(\d+)\s+(\d+)";
        foreach (Match m in Regex.Matches(line, pattern))
        {
          Color color;
          if (!colorDictionary.TryGetValue(m.Groups[3].Value, out color))
          {
            color = palet.ElementAt(colorIndex);
            colorDictionary.Add(m.Groups[3].Value, color);
            colorIndex++;
          }

          heaps.Add(new Heap()
          {
            address = Convert.ToUInt64(m.Groups[1].Value, 16) - address,
            size = Convert.ToUInt64(m.Groups[2].Value, 16),
            filename = m.Groups[3].Value,
            line = Convert.ToUInt32(m.Groups[4].Value),
            color = color,
            totalsize = totalsize
          });

          freesize.current -= Convert.ToUInt64(m.Groups[2].Value, 16);
        }
      }

      // DEL... ログの処理
      {
        string pattern = @"DEL\s+(0x[\dabcdef]+)\s+(0x[\dabcdef]+)\s+(\S+)\s+(\d+)\s+(\d+)";
        foreach (Match m in Regex.Matches(line, pattern))
        {
          Heap matched = null;
          foreach (var heap in heaps.Where(x => x.address == Convert.ToUInt64(m.Groups[1].Value, 16) - address))
          {
            matched = heap;
          }

          if (matched != null)
          {
            freesize.current += matched.size;
            heaps.Remove(matched);
          }
        }
      }

      // RES... ログの処理
      {
        string pattern = @"RES\s+(0x[\dabcdef]+)\s+(0x[\dabcdef]+)\s+(\S+)\s+(\d+)\s+(\d+)";
        foreach (Match m in Regex.Matches(line, pattern))
        {
          foreach (var heap in heaps.Where(x => x.address == Convert.ToUInt64(m.Groups[1].Value, 16) - address))
          {
            freesize.current += heap.size;
            freesize.current -= Convert.ToUInt64(m.Groups[2].Value, 16);
            heap.size = Convert.ToUInt64(m.Groups[2].Value, 16);
          }
        }
      }

      foreach (var m in skiplist)
      {
        if (line.Contains(m))
        {
          return true;
        }
      }

      return false;
    }

    private void ParseHeapInfo(string line)
    {
      // 各ヒープ情報をパース
      {
        string pattern = @"(0x[\dabcdef]+)\s+(0x[\dabcdef]+)\s+(\S+)\s+(\d+)\s+(\d+)";
        foreach (Match m in Regex.Matches(line, pattern))
        {
          Color color;
          if (!colorDictionary.TryGetValue(m.Groups[3].Value, out color))
          {
            color = palet.ElementAt(colorIndex);
            colorDictionary.Add(m.Groups[3].Value, color);
            colorIndex++;
          }

          heaps.Add(new Heap() { address = Convert.ToUInt64(m.Groups[1].Value, 16) - address, 
                                 size = Convert.ToUInt64(m.Groups[2].Value, 16), 
                                 filename = m.Groups[3].Value, 
                                 line = Convert.ToUInt32(m.Groups[4].Value),
                                 color = color,
                                 totalsize = totalsize});
        }
      }
    }

    private void MenuItem_AddToSkipList_Click(object sender, RoutedEventArgs e)
    {
      string pattern = @"(ALC|DEL|RES)\s+(0x[\dabcdef]+)\s+(0x[\dabcdef]+)\s+(\S+)\s+(\d+)\s+(\d+)";
      var menuitem = sender as MenuItem;
      TextBox tb = null;

      if (menuitem != null)
      {
        tb = ((ContextMenu)menuitem.Parent).PlacementTarget as TextBox;
      }

      foreach (Match m in Regex.Matches(tb.Text, pattern))
      {
        if (!skiplist.Contains(m.Groups[4].Value))
        {
          skiplist.Add(m.Groups[4].Value);
        }
      }
    }

    private void MenuItem_RemoveFromSkipList_Click(object sender, RoutedEventArgs e)
    {
      var menuitem = sender as MenuItem;
      TextBox tb = null;

      if (menuitem != null)
      {
        tb = ((ContextMenu)menuitem.Parent).PlacementTarget as TextBox;
      }

      string pattern = @"(ALC|DEL|RES)\s+(0x[\dabcdef]+)\s+(0x[\dabcdef]+)\s+(\S+)\s+(\d+)\s+(\d+)";
      foreach (Match m in Regex.Matches(tb.Text, pattern))
      {
        if (skiplist.Contains(m.Groups[4].Value))
        {
          skiplist.Remove(m.Groups[4].Value);
        }
      }
    }
  }

  public class Heap : INotifyPropertyChanged
  {
    public ulong address { get; set; }
    private ulong _size { get; set; }
    public ulong size
    {
      get { return _size; }
      set
      {
        if (value != _size)
        {
          _size = value;
          OnPropertyChanged("size");
        }
      }
    }
    public string filename { get; set; }
    public uint line { get; set; }
    public ulong totalsize { get; set; }
    private Color _color;
    public Color color
    {
      get { return _color; }
      set
      {
        if (value != _color)
        {
          _color = value;
          OnPropertyChanged("color");
        }
      }
    }

    public event PropertyChangedEventHandler PropertyChanged;

    protected virtual void OnPropertyChanged(string propertyName)
    {
      var handler = PropertyChanged;
      if (handler != null)
      {
        handler(this, new PropertyChangedEventArgs(propertyName));
      }
    }
  }

  public class FreeSize : INotifyPropertyChanged
  {
    public FreeSize()
    { 
      max = 0;
      min = ulong.MaxValue;
    }

    private ulong _current { get; set; }
    public ulong current
    {
      get { return _current; }
      set
      {
        if (value != _current)
        {
          _current = value;
          OnPropertyChanged("current");
          if (_current > max) { max = _current; OnPropertyChanged("max"); }
          if (_current < min) { min = _current; OnPropertyChanged("min"); }
        }
      }
    }
    public ulong min { get; private set; }
    public ulong max { get; private set; }

    public event PropertyChangedEventHandler PropertyChanged;

    protected virtual void OnPropertyChanged(string propertyName)
    {
      var handler = PropertyChanged;
      if (handler != null)
      {
        handler(this, new PropertyChangedEventArgs(propertyName));
      }
    }
  }

  public class Log
  {
    public string value { get; set; }
    public int linenum { get; set; }
  }

  public class AddrToGridOriginConverter : IMultiValueConverter
  {
    public object Convert(object[] values, Type type, object parameter, System.Globalization.CultureInfo culture)
    {
      return (1000 * System.Convert.ToDouble(values[0]) / System.Convert.ToDouble(values[1]));
    }

    public object[] ConvertBack(object value, Type[] types, object parameter, System.Globalization.CultureInfo culture)
    {
      throw new NotSupportedException();
    }
  }

  public class SizeToGridWidthConverter : IMultiValueConverter
  {
    public object Convert(object[] values, Type type, object parameter, System.Globalization.CultureInfo culture)
    {
      return Math.Max((uint)(1000 * System.Convert.ToDouble(values[0]) / System.Convert.ToDouble(values[1])), 1.0d);
    }

    public object[] ConvertBack(object values, Type[] types, object parameter, System.Globalization.CultureInfo culture)
    {
      throw new NotSupportedException();
    }
  }

  public class ColorToBrushConverter : IValueConverter
  {
    public object Convert(object value, Type type, object parameter, System.Globalization.CultureInfo culture)
    {
      return new SolidColorBrush((Color)value);
    }

    public object ConvertBack(object value, Type type, object parameter, System.Globalization.CultureInfo culture)
    {
      return ((SolidColorBrush)value).Color;
    }
  }
}
