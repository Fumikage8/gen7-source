using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ModelViewer
{
  public class ExtraFilePathSet
  {
    public ExtraFilePathSet()
    {
      McnfPathList = new string[] { string.Empty };
      BlendListXmlPath = "";
      EachDaeXmlPath = "";
    }

    public ExtraFilePathSet(string[] mcnfPathList, string blendListXmlPath, string eachDaeXmlPath)
    {
      McnfPathList = mcnfPathList;
      BlendListXmlPath = string.IsNullOrEmpty(blendListXmlPath) ? "" : blendListXmlPath;
      EachDaeXmlPath = string.IsNullOrEmpty(eachDaeXmlPath) ? "" : eachDaeXmlPath;
    }

    public string[] McnfPathList { get; set; }
    public string BlendListXmlPath { get; set; }
    public string EachDaeXmlPath { get; set; }
  }
}
