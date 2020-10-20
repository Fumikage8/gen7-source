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
    [XmlRoot("GfMotion")]
    public class GFMotion
    {
      [XmlAttribute("version")]
      public string version { get; set; }

      [XmlAttribute("FrameSize")]
      public int FrameSize { get; set; }

      [XmlAttribute("IsLoop")]
      public bool IsLoop { get; set; }

      [XmlAttribute("CurrrentUnit")]
      public string CurrrentUnit { get; set; }


      [XmlElement("Source")]
      public GFMotion_Source Source  {get; set;}

      [XmlElement("ModelAABB")]
      public string ModelAABB  {get; set;}

      [XmlElement("SkeltalAnimation")]  /*  gfmot内では 'Skeltal'Animation    */
      public GFMotion_SkeletalAnimation SkeletalAnimation  {get; set;}

      [XmlElement("Skelton")]
      public GFMotion_Skelton Skelton  {get; set;}




      /*
      */
      public static GFMotion Load(string path)
      {
          GFMotion motion = null;

          using (System.IO.FileStream fs = new System.IO.FileStream(path, System.IO.FileMode.Open))
          {
              XmlSerializer serializer = new XmlSerializer(typeof(GFMotion));
              motion = (serializer.Deserialize(fs) as GFMotion);
          }
          return motion;
      }

    }

    /*
     * 
     * */
    public class GFMotion_Source
    {
        [XmlAttribute("Path")]
        public string Path { get; set; }

        [XmlAttribute("TargetNode")]
        public string TargetNode { get; set; }
    }

    /*
     * 
     * */
    public class GFMotion_SkeletalAnimation
    {
      [XmlElement("Node")]
      public List<GFMotion_SkeletalAnimationNode> Nodes  {get; set;}
    }

    /*
     * 
     * */
    public class GFMotion_SkeletalAnimationNode
    {
        [XmlAttribute("Name")]
        public string Name { get; set; }
    }

    /*
     * 
     * */
    public class GFMotion_Skelton
    {
      [XmlElement("Node")]
      public List<GFMotion_SkeltonNode> Nodes  {get; set;}
    }


    /*
     * 
     * */
    public class GFMotion_SkeltonNode
    {
        [XmlAttribute("Name")]
        public string Name  {get; set;}

        [XmlAttribute("NeededRendering")]
        public string NeededRendering {get; set;}

        [XmlAttribute("IsJoint")]
        public string IsJoint {get; set;}

        [XmlAttribute("SegmentScaleCompensate")]
        public string SegmentScaleCompensate  {get; set;}

        [XmlAttribute("BillboardType")]
        public string BillboardType {get; set;}
    }






}
