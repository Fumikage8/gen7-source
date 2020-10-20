using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace DressUpConverter
{
  class Def
  {
    public enum ConvertType
    {
      BaseJoint,  // ベースジョイントのコンバート
      BaseJointMotion,  // ベースジョイントのモーションコンバート
      PartsJointMotion, // パーツジョイントのモーションコンバート

      // ここから各パーツ
      Accbadge,
      Acceye,
      Acchair,
      Bag,
      //Base,    // コンバート必要ないはず
      Bngl,
      Body,
      Bottoms,
      //Default, // コンバート必要ないはず
      Face,
      Hair,
      Hat,
      Legs,
      Shoes,
      Tops,
      Situ,
    }

    public static readonly Dictionary<string, ConvertType> ConvertTypeDict = new Dictionary<string, ConvertType>()
    {
      {ConvertType.BaseJoint.ToString().ToLower(), ConvertType.BaseJoint},
      {ConvertType.BaseJointMotion.ToString().ToLower(), ConvertType.BaseJointMotion},
      {ConvertType.PartsJointMotion.ToString().ToLower(), ConvertType.PartsJointMotion},

      // ここから各パーツ
      {ConvertType.Accbadge.ToString().ToLower(), ConvertType.Accbadge},
      {ConvertType.Acceye.ToString().ToLower(), ConvertType.Acceye},
      {ConvertType.Acchair.ToString().ToLower(), ConvertType.Acchair},
      {ConvertType.Bag.ToString().ToLower(), ConvertType.Bag},
      {ConvertType.Bngl.ToString().ToLower(), ConvertType.Bngl},
      {ConvertType.Body.ToString().ToLower(), ConvertType.Body},
      {ConvertType.Bottoms.ToString().ToLower(), ConvertType.Bottoms},
      {ConvertType.Face.ToString().ToLower(), ConvertType.Face},
      {ConvertType.Hair.ToString().ToLower(), ConvertType.Hair},
      {ConvertType.Hat.ToString().ToLower(), ConvertType.Hat},
      {ConvertType.Legs.ToString().ToLower(), ConvertType.Legs},
      {ConvertType.Shoes.ToString().ToLower(), ConvertType.Shoes},
      {ConvertType.Tops.ToString().ToLower(), ConvertType.Tops},
      {ConvertType.Situ.ToString().ToLower(), ConvertType.Situ},
    };
  }

  class Program
  {
    static int Main(string[] args)
    {
      var convTypeString = args[0]; // コンバートタイプ
      var outputPath = args[1]; // 出力パス

      var outputDir = Path.GetDirectoryName(outputPath);
      if (!Directory.Exists(outputDir))
      {
        Directory.CreateDirectory(outputDir);
      }

      // コンバートタイプに含まれないならおかしい
      if (!Def.ConvertTypeDict.ContainsKey(convTypeString))
      {
        return -1;
      }
      var convType = Def.ConvertTypeDict[convTypeString];

      var result = 0;
      if (convType == Def.ConvertType.BaseJointMotion)
      {
        // ベースモーションのコンバート
        var baseMotionPath = args[2];
        var baseModelPath = args[3];
        var partsMotionPath = args[4];

        result = ConvBaseJointMotion(outputPath, baseMotionPath, baseModelPath, partsMotionPath);
      }
      else if (convType == Def.ConvertType.PartsJointMotion)
      {
        // パーツモーションのコンバート
        var baseModelPath = args[2];
        var partsModelPath = args[3];
        var partsMotionPath = args[4];

        result = ConvPartsJointMotion(outputPath, baseModelPath, partsModelPath, partsMotionPath);
      }
      else
      {
        var baseModelPath = args[2];
        if (convType == Def.ConvertType.BaseJoint)
        {
          // ベースジョイントモデルコンバート
          var partsModelPathList = args.Where((arg, index) => index > 2).ToArray();
          result = ConvBaseJoint(outputPath, baseModelPath, partsModelPathList);
        }
        else
        {
          // パーツモデルコンバート
          var partsModelPath = args[3];
          result = ConvParts(outputPath, convType, baseModelPath, partsModelPath);
        }
      }
    
      return result;
    }

    static int ConvBaseJointMotion(string outputPath, string baseMotionPath, string baseModelPath, string partsMotionPath)
    {
      var baseMotion = new GfMotion(baseMotionPath);
      var baseModel = new GfModel(baseModelPath);
      var partsMotion = new GfMotion(partsMotionPath);

      // モーションのマージ
      baseMotion.MargeMotion(baseModel, partsMotion);

      // 変更内容を保存
      baseMotion.Save(outputPath);

      return 0;
    }

    static int ConvPartsJointMotion(string outputPath, string baseModelPath, string partsModelPath, string partsMotionPath)
    {
      var baseModel = new GfModel(baseModelPath);
      var partsModel = new GfModel(partsModelPath);
      var partsMotion = new GfMotion(partsMotionPath);

      // モーションの加工
      partsMotion.ConvPartsMotion(baseModel, partsModel);

      // 変更内容を保存
      partsMotion.Save(outputPath);

      return 0;
    }

    static int ConvBaseJoint(string outputPath, string baseModelPath, string[] partsModelPathList)
    {
      var baseModel = new GfModel(baseModelPath);

      foreach (var partsModelPath in partsModelPathList)
      {
        var partsModel = new GfModel(partsModelPath);

        // スケルトンをコンバート
        baseModel.ConvertBaseJoint(partsModel);
      }

      // 変更内容を保存
      baseModel.Save(outputPath);

      return 0;
    }

    static int ConvParts(string outputPath, Def.ConvertType convType, string baseModelPath, string partsModelPath)
    {
      var partsModel = new GfModel(partsModelPath);

      // アクセサリー系は原点に頂点を戻す必要がある
      if ((convType == Def.ConvertType.Accbadge) || (convType == Def.ConvertType.Acceye) || (convType == Def.ConvertType.Acchair))
      {
        if (!partsModel.ConvertAcc(convType))
        {
          return 1;
        }
      }
      // スケルトンをコンバート
      else
      {
        var baseModel = new GfModel(baseModelPath);

        if (!partsModel.ConvertSkelton(convType, baseModel))
        {
          return 1;
        }
      }

      // 変更内容を保存
      partsModel.Save(outputPath);

      return 0;
    }
  }
}
