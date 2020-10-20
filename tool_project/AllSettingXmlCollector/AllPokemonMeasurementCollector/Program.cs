using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using GFFileIO;
using GFMsgPack;
using GFPokemonFile;

namespace AllPokemonMeasurementCollector
{
  public class LoadMeasurementData
  {
    public uint MonsNo { get; set; }
    public uint Sex { get; set; }
    public uint FormNo { get; set; }
    public string Name { get; set; }
    public uint Update { get; set; }
    public uint Draw { get; set; }
    public uint GPU { get; set; }
    public uint Command { get; set; }
  }

  class Program
  {
    static int Main(string[] args)
    {
      if (args.Length == 0)
      {
        Console.WriteLine("負荷計測結果が記述されているファイルを指定してください.");
        return 1;
      }

      string loadMeasurementDataDetail = "";
      using (StreamReader streamReader = new StreamReader(args[0]))
      {// ext_data.txtの中身を一括で読み取り
        loadMeasurementDataDetail = streamReader.ReadToEnd();
      }
      // 改行コードで分割
      string[] loadMeasurementStrings = Regex.Split(loadMeasurementDataDetail, Environment.NewLine);

      List<LoadMeasurementData> loadMeasurementDataList = new List<LoadMeasurementData>();
      foreach (string loadMeasurementString in loadMeasurementStrings)
      {
        if (string.IsNullOrEmpty(loadMeasurementString))
        {
          continue;
        }

        string[] commaSplitStrings = Regex.Split(loadMeasurementString, ",");

        LoadMeasurementData loadMeasurementData = new LoadMeasurementData();

        // [0] : MonsNo
        string[] MonsNos = commaSplitStrings[0].Split('=');
        loadMeasurementData.MonsNo = uint.Parse(MonsNos[1]);

        // [1] : Sex
        string[] Sexes = commaSplitStrings[1].Split('=');
        loadMeasurementData.Sex = uint.Parse(Sexes[1]);

        // [2] : FormNo
        string[] FormNos = commaSplitStrings[2].Split('=');
        loadMeasurementData.FormNo = uint.Parse(FormNos[1]);

        // [3] : Name
        string[] Names = commaSplitStrings[3].Split('=');
        loadMeasurementData.Name = Names[1];

        // [4] : Update
        string[] Updates = commaSplitStrings[4].Split('=');
        loadMeasurementData.Update = uint.Parse(Updates[1]);

        // [5] : Draw
        string[] Draws = commaSplitStrings[5].Split('=');
        loadMeasurementData.Draw = uint.Parse(Draws[1]);

        // [6] : GPU
        string[] GPUs = commaSplitStrings[6].Split('=');
        loadMeasurementData.GPU = uint.Parse(GPUs[1]);

        // [7] : Command
        string[] Commands = commaSplitStrings[7].Split('=');
        loadMeasurementData.Command = uint.Parse(Commands[1]);

        loadMeasurementDataList.Add(loadMeasurementData);
      }

      LoadMeasurementData[] loadMeasurementDataArray = loadMeasurementDataList.ToArray();

      uint maxUpdate = loadMeasurementDataArray.Max(data => data.Update);
      var maxUpdateValue = loadMeasurementDataArray.Where(data => data.Update == maxUpdate).ToArray();
      uint maxDraw = loadMeasurementDataArray.Max(data => data.Draw);
      var maxDrawValue = loadMeasurementDataArray.Where(data => data.Draw == maxDraw).ToArray();
      uint maxGPU = loadMeasurementDataArray.Max(data => data.GPU);
      var maxGPUValue = loadMeasurementDataArray.Where(data => data.GPU == maxGPU).ToArray();
      uint maxCommand = loadMeasurementDataArray.Max(data => data.Command);
      var maxCommandValue = loadMeasurementDataArray.Where(data => data.Command == maxCommand).ToArray();

      using (StreamWriter streamWriter = new StreamWriter("NijiLoadMeasurement.csv"))
      {
        streamWriter.WriteLine("MonsNo, Sex, FormNo, TopJointName, Update, Draw, GPU, Command, UpdateDiff, DrawDiff, GPUDiff");
        foreach (var data in loadMeasurementDataArray)
        {
          streamWriter.Write(data.MonsNo + ", " + data.Sex + ", " + data.FormNo + ", " + data.Name + ", " + data.Update + ", " + data.Draw + ", " + data.GPU + ", " + data.Command + ", ");
          streamWriter.WriteLine((data.Update - loadMeasurementDataArray[0].Update) + ", " + (data.Draw - loadMeasurementDataArray[0].Draw) + ", " + (data.GPU - loadMeasurementDataArray[0].GPU));
        }

        streamWriter.WriteLine("Update Max , MonsNo , " + maxUpdateValue[0].MonsNo + ", Sex , " + maxUpdateValue[0].Sex + ", FormNo , " + maxUpdateValue[0].FormNo + ", Value , " + maxUpdate + ", Diff , " + (maxUpdate - loadMeasurementDataArray[0].Update));
        streamWriter.WriteLine("Draw Max , MonsNo , " + maxDrawValue[0].MonsNo + ", Sex , " + maxDrawValue[0].Sex + ", FormNo , " + maxDrawValue[0].FormNo + ", Value , " + maxDraw + ", Diff , " + (maxDraw - loadMeasurementDataArray[0].Draw));
        streamWriter.WriteLine("GPU Max , MonsNo , " + maxGPUValue[0].MonsNo + ", Sex , " + maxGPUValue[0].Sex + ", FormNo , " + maxGPUValue[0].FormNo + ", Value , " + maxGPU + ", Diff , " + (maxGPU - loadMeasurementDataArray[0].GPU));
        streamWriter.WriteLine("Command Max , MonsNo , " + maxCommandValue[0].MonsNo + ", Sex , " + maxCommandValue[0].Sex + ", FormNo , " + maxCommandValue[0].FormNo + ", Value , " + maxCommand + ", Diff , " + (maxCommand - loadMeasurementDataArray[0].Command));
      }
      return 0;
    }
  }
}
