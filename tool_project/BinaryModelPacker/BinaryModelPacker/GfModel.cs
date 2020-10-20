using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace BinaryModelPacker
{
  public class GfModel
  {
    public Texture[] TextureList;
    public Shader[] ShaderList;

    public static GfModel Deserialize(string filepath)
    {
      try
      {
        var serializer = new XmlSerializer(typeof(GfModel));
        using (var fs = System.IO.File.OpenRead(filepath))
        {
          return serializer.Deserialize(fs) as GfModel;
        }
      }
      catch (Exception e)
      {
        Console.WriteLine("!!---------------------------------------");
        Console.WriteLine("Error: failed to deserialize {0}. please confirm the file consistency and not to use non-ascii character in the filepath.", filepath);
        Console.WriteLine(e.Message);
        Console.WriteLine("!!---------------------------------------");
        return null;
      }
    }
  }

  public class Texture
  {
    public string FilePath;

    [XmlAttribute()]
    public string Type;
  }

  public class Shader
  {
    public string VshFilePath;
    public string GshFilePath;
    public string FshFilePath;
  }
}
