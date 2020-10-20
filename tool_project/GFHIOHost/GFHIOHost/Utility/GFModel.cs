using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;
using System.Xml.Serialization;


namespace GFHIOHost
{
    /*
     * 
     * */
    [XmlRoot("GfModel")]
    public class GFModel
    {
      [XmlAttribute("version")]
      public string version { get; set; }

      [XmlElement("Source")]
      public GFModel_Source Source  {get; set;}
      
      [XmlElement("ExportOption")]
      public GFModel_ExportOption ExportOption  {get; set;}

      [XmlElement("UserData")]
      public GFModel_UserData UserData  {get; set;}


      [XmlElement("TextureList")]
      public GFModel_TextureList  TextureList {get; set;}

      [XmlElement("ShaderList")]
      public GFModel_ShaderList  ShaderList {get; set;}


      [XmlElement("Materials")]
      public GFModel_Materials  Materials {get; set;}


      [XmlElement("ModelTransform")]
      public GFModel_ModelTransform  ModelTransform {get; set;}

      [XmlElement("ModelAABB")]
      public string  ModelAABB {get; set;}

      [XmlElement("Meshes")]
      public GFModel_Meshes  Meshes {get; set;}

      [XmlElement("Skelton")]
      public GFModel_Skelton  Skelton {get; set;}



      /*
      */
      public static GFModel Load(string path)
      {
          GFModel model = null;

          using (System.IO.FileStream fs = new System.IO.FileStream(path, System.IO.FileMode.Open))
          {
              XmlSerializer serializer = new XmlSerializer(typeof(GFModel));

              model = (serializer.Deserialize(fs) as GFModel);
          }

          return model;
      }
    }

    /*
     * 
     * */
    public class GFModel_Source
    {
        [XmlAttribute("Path")]
        public string Path { get; set; }

        [XmlAttribute("TargetNode")]
        public string TargetNode { get; set; }
    }

    /*
     * 
     * */
    public class GFModel_ExportOption
    {
        [XmlAttribute("NotCombine")]
        public bool NotCombine { get; set; }

        [XmlAttribute("Reduction")]
        public bool Reduction { get; set; }

        [XmlAttribute("Bake")]
        public string Bake { get; set; }

        [XmlAttribute("CurrentUnit")]
        public string CurrentUnit { get; set; }

        [XmlAttribute("MdlFileOut")]
        public string MdlFileOut { get; set; }

        [XmlAttribute("TexFileOut")]
        public string TexFileOut { get; set; }

        [XmlAttribute("ShaderFileOut")]
        public string ShaderFileOut { get; set; }

    
        [XmlAttribute("IncludeShaderFileOut")]
        public string IncludeShaderFileOut {get; set;}
        [XmlAttribute("TransformNodeAnimeEnable")]
        public string TransformNodeAnimeEnable {get; set;}
        [XmlAttribute("EnvLightEnable")]
        public string EnvLightEnable {get; set;}
        [XmlAttribute("EnvCameraEnable")]
        public string EnvCameraEnable {get; set;}
        [XmlAttribute("EnvEnable")]
        public string EnvEnable {get; set;}
        [XmlAttribute("JointAnimEnable")]
        public string JointAnimEnable {get; set;}
        [XmlAttribute("MeshVisEnable")]
        public string MeshVisEnable {get; set;}
        [XmlAttribute("BlendShapeEnable")]
        public string BlendShapeEnable {get; set;}
        [XmlAttribute("UvAnimEnable")]
        public string UvAnimEnable {get; set;}
        [XmlAttribute("ShaderAnimEnable")]
        public string ShaderAnimEnable {get; set;}
        [XmlAttribute("AttributeKeyAnimationEnable")]
        public string AttributeKeyAnimationEnable {get; set;}
        [XmlAttribute("TexturePatternAnimationEnable")]
        public string TexturePatternAnimationEnable {get; set;}
        [XmlAttribute("EnvAnimationEnable")]
        public string EnvAnimationEnable {get; set;}
        [XmlAttribute("LoopMotionEnable")]
        public string LoopMotionEnable {get; set;}
        [XmlAttribute("UvSetAdjust")]
        public string UvSetAdjust {get; set;}

    
    }



    /*
     * 
     * */
    public class GFModel_UserData
    {
      [XmlAttribute("CmHeight")]
      public float CmHeight {get; set;}

      [XmlAttribute("CmAdjustHeight")]
      public float CmAdjustHeight {get; set;}

      [XmlAttribute("BillboardType")]
      public int BillboardType {get; set;}
    }

    /*
     * 
     * */
    public class GFModel_TextureList
    {
      [XmlElement("Texture")]
      public List<GFModel_Texture> Textures {get; set;}
    }

    /*
     * 
     * */
    public class GFModel_Texture
    {
      
      [XmlAttribute("No")]
      public int No {get; set;}
      
      [XmlAttribute("Type")]
      public string Type {get; set;}
      
      [XmlElement("FilePath")]
      public string FilePath  {get; set;}
    }

    /*
     * 
     * */
    public class GFModel_ShaderList
    {
      [XmlElement("Shader")]
      public List<GFModel_Shader> Shaders {get; set;}
    }

    /*
     * 
     * */
    public class GFModel_Shader
    {
      [XmlAttribute("No")]
      public int No {get; set;}

      [XmlAttribute("Name")]
      public string Name {get; set;}

      [XmlElement("VshFilePath")]
      public string VshFilePath {get; set;}
      
      
      [XmlElement("FshFilePath")]
      public string FshFilePath {get; set;}

    }


    /*
     * 
     * */
    public class GFModel_Materials
    {
      [XmlElement("Material")]
      public List<GFModel_Material> Materials {get; set;}
    }

    /*
     * 
     * */
    public class GFModel_Material
    {
      [XmlAttribute("Name")]
      public string Name {get; set;}

      [XmlAttribute("Type")]
      public string Type {get; set;}

      [XmlAttribute("Priority")]
      public int Priority {get; set;}

      [XmlAttribute("MaterialHash")]
      public int MaterialHash {get; set;}
    }


    /*
     * 
     * */
    public class GFModel_ModelTransform
    {
      [XmlElement("GlobalMatrix")]
      public string GlobalMatrix {get; set;}
    }
  
    /*
     * 
     * */
    public class GFModel_Meshes
    {
      [XmlElement("Mesh")]
      public List<GFModel_Mesh> Meshes {get; set;}
    }


    /*
     * 
     * */
    public class GFModel_Mesh
    {
        [XmlAttribute("Name")]
        public string Name { get; set; }

        [XmlAttribute("IsBlendShape")]
        public string IsBlendShape { get; set; }

        [XmlAttribute("IsVisibilityAnimation")]
        public string IsVisibilityAnimation { get; set; }

        [XmlElement("AABB")]
        public string AABB {get; set;}
    }



    /*
     * 
     * */
    public class GFModel_Skelton
    {
      [XmlElement("Node")]
      public List<GFModel_Node> Nodes {get; set;}
    }


    public class GFModel_Node
    {
      [XmlAttribute("Name")]
      public string Name {get; set;}

      [XmlAttribute("NeededRendering")]
      public string NeededRendering {get; set;}


      [XmlAttribute("IsJoint")]
      public string IsJoint {get; set;}

      [XmlAttribute("SegmentScaleCompensate")]
      public string SegmentScaleCompensate {get; set;}

      [XmlAttribute("BillboardType")]
      public string BillboardType {get; set;}
    }




}