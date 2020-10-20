using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;

namespace AnimationConverter
{
  class McnfBinaryHeader
  {
    public enum ConfigType
    {
      Top,
      Interest,
      Spring,
      Ik,
      Expression,
      BlendList,
    }

    public McnfBinaryHeader(ConfigType type, byte[] buffer)
    {
      this.type = type;
      this.buffer = buffer;
    }

    public ConfigType type = ConfigType.Top;
    public byte[] buffer = null;
  }

  class McnfBinaryExporter : IDisposable
  {
    public const ushort FormatVersionMajor = 0;
    public const ushort FormatVersionMinor = 1;

    public const int NameBufferSize = 64;

    public McnfBinaryExporter(string outputPath, Mcnf.Mcnf mcnf, Mcnf.BlendList blendList, string animationDirectory)
    {
      var outputDir = Path.GetDirectoryName(outputPath);
      if (!Directory.Exists(outputDir))
      {
        Directory.CreateDirectory(outputDir);
      }
      this.fs = File.Open(outputPath, FileMode.OpenOrCreate);

      this.bw = new BinaryWriter(this.fs);

      // 上書き用にサイズを０に
      this.bw.BaseStream.SetLength(0);

      this.mcnf = mcnf;
      this.blendList = blendList;
      this.animationDirectory = animationDirectory;
    }

    public void Dispose()
    {
      this.bw.Dispose();
      this.fs.Dispose();
    }

    // バイナリ出力
    public bool ExportBinary()
    {
      // ヘッダー出力
      if (!this.ExportHeader())
      {
        return false;
      }

      // データリスト
      var dataList = new List<McnfBinaryHeader>();

      // アニメーショントップ出力
      var bufTop = this.ExportTop();
      if (bufTop != null)
      {
        dataList.Add(new McnfBinaryHeader(McnfBinaryHeader.ConfigType.Top, bufTop));
      }

      // 視線パラメータ出力
      var bufInterest = this.ExportInterest();
      if (bufInterest != null)
      {
        dataList.Add(new McnfBinaryHeader(McnfBinaryHeader.ConfigType.Interest, bufInterest));
      }

      // バネパラメータ出力
      var bufSpring = this.ExportSpring();
      if (bufSpring != null)
      {
        dataList.Add(new McnfBinaryHeader(McnfBinaryHeader.ConfigType.Spring, bufSpring));
      }

      // IKパラメータ出力
      var bufIk = this.ExportIk();
      if (bufIk != null)
      {
        dataList.Add(new McnfBinaryHeader(McnfBinaryHeader.ConfigType.Ik, bufIk));
      }

      // Expressionパラメータ出力
      var bufExpression = this.ExportExpression();
      if (bufExpression != null)
      {
        dataList.Add(new McnfBinaryHeader(McnfBinaryHeader.ConfigType.Expression, bufExpression));
      }

      // BlendListパラメータ出力
      var bufBlendList = this.ExportBlendList();
      if (bufBlendList != null)
      {
        dataList.Add(new McnfBinaryHeader(McnfBinaryHeader.ConfigType.BlendList, bufBlendList));
      }

      // データ数
      this.bw.Write(dataList.Count);

      var offset = (int)this.bw.BaseStream.Position;
      offset += (4 * 3) * dataList.Count;

      // データヘッダー出力
      foreach (var data in dataList)
      {
        // タイプ
        var type = (int)data.type;
        this.bw.Write(type);

        // サイズ
        this.bw.Write(data.buffer.Length);

        // オフセット
        this.bw.Write(offset);
        offset += data.buffer.Length;
      }

      // データ出力
      foreach (var data in dataList)
      {
        this.bw.Write(data.buffer);
      }

      return true;
    }

    // ヘッダ出力
    public bool ExportHeader()
    {
      if (this.mcnf == null)
      {
        return false;
      }

      // バージョン
      this.bw.Write(FormatVersionMajor);
      this.bw.Write(FormatVersionMinor);

      return true;
    }

    // トップ出力
    private byte[] ExportTop()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        return null;
      }
    }

    // 視線パラメータ出力
    private byte[] ExportInterest()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        if (this.mcnf == null)
        {
          return null;
        }

        var interest = this.mcnf.INTEREST;
        if (interest == null)
        {
          return null;
        }
        if ((interest.NODE == null) || (interest.NODE.Count == 0))
        {
          return null;
        }

        // ノード数
        msr.Write(interest.NODE.Count);

        foreach (var node in interest.NODE)
        {
          var name = Common.GetNameBuffer(node._Name, NameBufferSize);
          msr.Write(name);
          msr.Write((int)node._Type);
          msr.Write(DegToRad(node._Speed * 0.01f));
          msr.Write(DegToRad(node._Low));
          msr.Write(DegToRad(node._High));
          msr.Write(DegToRad(node._Low2));
          msr.Write(DegToRad(node._High2));
        }

        return ms.ToArray();
      }
    }

    // バネパラメータ出力
    private byte[] ExportSpring()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        if (this.mcnf == null)
        {
          return null;
        }

        var sprintList = this.mcnf.SPRING;
        if ((sprintList == null) || (sprintList.Count == 0))
        {
          return null;
        }

        // ノード数
        msr.Write(sprintList.Count);

        foreach (var spring in sprintList)
        {
          var name = Common.GetNameBuffer(spring._Name, NameBufferSize);
          msr.Write(name);
          msr.Write(spring._Damping * 0.01f);
          msr.Write(spring._Stiffness * 0.01f);
          msr.Write(spring._Mass * 0.1f);
          msr.Write(spring._Gravity * 0.1f);
        }

        return ms.ToArray();
      }
    }

    // IKパラメータ出力
    private byte[] ExportIk()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        if (this.mcnf == null)
        {
          return null;
        }

        var ikList = this.mcnf.IK;
        if ((ikList == null) || (ikList.Count == 0))
        {
          return null;
        }

        // ノード数
        msr.Write(ikList.Count);

        foreach (var ik in ikList)
        {
          var chain = Common.GetNameBuffer(ik._Chain, NameBufferSize);
          msr.Write(chain);

          var joint = Common.GetNameBuffer(ik._Joint, NameBufferSize);
          msr.Write(joint);

          var effector = Common.GetNameBuffer(ik._Effector, NameBufferSize);
          msr.Write(effector);

          var effectorChild = Common.GetNameBuffer(ik._EffectorChild, NameBufferSize);
          msr.Write(effectorChild);

          var tip = Common.GetNameBuffer(ik._Tip, NameBufferSize);
          msr.Write(tip);

          msr.Write((int)ik._Type);
          msr.Write(ik._Hold ? 1 : 0);
          msr.Write(ik._Land ? 1 : 0);
        }

        return ms.ToArray();
      }
    }

    // Expressionパラメータ出力
    private byte[] ExportExpression()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        if (this.mcnf == null)
        {
          return null;
        }

        var expressionList = this.mcnf.EXPRESSION;
        if ((expressionList == null) || (expressionList.Count == 0))
        {
          return null;
        }

        // ノード数
        msr.Write(expressionList.Count);

        foreach (var expression in expressionList)
        {
          var dst = Common.GetNameBuffer(expression._Dst, NameBufferSize);
          msr.Write(dst);

          var src0 = Common.GetNameBuffer(expression._Src0, NameBufferSize);
          msr.Write(src0);

          msr.Write((int)expression._Type);
        }

        return ms.ToArray();
      }
    }

    // BlendListパラメータ出力
    private byte[] ExportBlendList()
    {
      using (var ms = new MemoryStream())
      using (var msr = new BinaryWriter(ms))
      {
        if (this.blendList == null)
        {
          return null;
        }

        var cells = this.blendList.Cell2d;
        if ((cells == null) || (cells.GetLength(0) == 0) || (cells.GetLength(1) == 0))
        {
          return null;
        }

        var fromCount = cells.GetLength(0) - 1;
        var toCount = cells.GetLength(1) - 1;

        var fromNameList = new string[fromCount];
        var fromHashLists = new List<int>[fromCount];

        var toNameList = new string[toCount];
        var toHashLists = new List<int>[toCount];

        for (int i = 0; i < fromCount; ++i)
        {
          fromNameList[i] = cells[i + 1, 0];
        }
        for (int i = 0; i < toCount; ++i)
        {
          toNameList[i] = cells[0, i + 1];
        }

        // fromのハッシュ値初期化
        InitializeHashLists(fromHashLists, fromNameList);

        // toのハッシュ値初期化
        InitializeHashLists(toHashLists, toNameList);

        // from, to数書き込み
        msr.Write(GetHashCount(fromHashLists));
        msr.Write(GetHashCount(toHashLists));

        // fromのハッシュ値書き込み
        foreach (var fromHashList in fromHashLists)
        {
          foreach (var hash in fromHashList)
          {
            msr.Write(hash);
          }
        }

        // toのハッシュ値書き込み
        foreach (var toHashList in toHashLists)
        {
          foreach (var hash in toHashList)
          {
            msr.Write(hash);
          }
        }

        // ブレンド数テーブル書き込み
        for (int fromCellIndex = 1; fromCellIndex < cells.GetLength(0); ++fromCellIndex)
        {
          foreach (var fromHashList in fromHashLists[fromCellIndex - 1])
          {
            for (int toCellIndex = 1; toCellIndex < cells.GetLength(1); ++toCellIndex)
            {
              var valueStr = cells[fromCellIndex, toCellIndex];
              if (string.IsNullOrEmpty(valueStr))
              {
                valueStr = cells[1, toCellIndex];
              }
              var value = int.Parse(valueStr);
              foreach (var toHashList in toHashLists[toCellIndex - 1])
              {
                msr.Write((byte)value);
              }
            }
          }
        }

        Common.Align4Byte(msr);
        
        return ms.ToArray();
      }
    }

    private void InitializeHashLists(List<int>[] hashLists, string[] nameList)
    {
      // 念のためハッシュの衝突チェック
      //var checkHashDict = new Dictionary<int, string>();

      // 念のため名前の重複チェック
      //var checkNameDict = new Dictionary<string, int>();

      for (int i = 0; i < nameList.Length; ++i)
      {
        hashLists[i] = new List<int>();
        var name = nameList[i];

        // 名前重複チェック
        //if (checkNameDict.ContainsKey(name))
        //{
        //  var errorMessage = string.Format("アニメーション名が重複しています！ {0}[{1}], {2}[{3}]", name, checkNameDict[name], name, i);
        //  throw new System.Exception(errorMessage);
        //}
        //checkNameDict.Add(name, i);

        if (name == "all")
        {
          hashLists[i].Add(-1);
          continue;
        }

        // 該当するファイルを検索
        var matchList = Directory.GetFiles(this.animationDirectory, "*" + name + ".gfmot");
        if (matchList.Length == 0)
        {
          // 見つからない
          hashLists[i].Add(-1);
          continue;
        }

        foreach (var match in matchList)
        {
          var fileName = Path.GetFileNameWithoutExtension(match);
          var hash = Common.CreateHash(fileName);

          hashLists[i].Add(hash);

          //if (checkHashDict.ContainsKey(hash))
          //{
          //  var errorMessage = string.Format("ハッシュ値が重複しています！ {0}[{1}], {2}[{3}]", checkHashDict[hash], hash, fileName, hash);
          //  throw new System.Exception(errorMessage);
          //}

          //checkHashDict.Add(hash, fileName);
        }
      }
    }

    private int GetHashCount(List<int>[] hashLists)
    {
      int total = 0;
      foreach (var hashList in hashLists)
      {
        total += hashList.Count;
      }

      return total;
    }

    private static int GetMatchFileIndex(string[] matchList, string filename)
    {
      if (matchList.Length == 0)
      {
        return -1;
      }
      if (matchList.Length == 1)
      {
        return 0;
      }

      for (var i = 0; i < matchList.Length; ++i)
      {
        matchList[i] = Path.GetFileNameWithoutExtension(matchList[i]);
      }

      // 共通文字が無くなるまで文字列を削除していく
      while (true)
      {
        var allSame = true;
        var firstValue = matchList[0].First();
        for (var i = 1; i < matchList.Length; ++i)
        {
          if (firstValue != matchList[i].First())
          {
            allSame = false;
            break;
          }
        }

        if (!allSame)
        {
          break;
        }

        // 1文字削除
        for (var i = 0; i < matchList.Length; ++i)
        {
          matchList[i] = matchList[i].Substring(1);
        }
      }

      for (var i = 0; i < matchList.Length; ++i)
      {
        if (matchList[i].StartsWith(filename))
        {
          return i;
        }
      }

      return -1;
    }

    private static float DegToRad(float deg)
    {
      return (float)Math.PI * deg / 180.0f;
    }

    private FileStream fs = null;
    private BinaryWriter bw = null;
    private Mcnf.Mcnf mcnf = null;
    private Mcnf.BlendList blendList = null;
    private string animationDirectory = null;
  }
}
