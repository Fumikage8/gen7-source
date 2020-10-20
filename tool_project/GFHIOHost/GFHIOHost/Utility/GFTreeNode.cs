using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GFHIOHost
{
    /*
     * 
     * */
    public class GFTreeNode : TreeNode
    {
        public string HeaderDescription { get; set; }
        public string FilePath { get; set; }


        public GFTreeNode(string text)
            : base(text)
        {

        }


        public virtual void SetupFromFile(string path)
        {
            System.IO.FileInfo info = new System.IO.FileInfo(path);

            SetupFromFileInfo(info);
        }



        protected void SetupFromFileInfo(System.IO.FileInfo info)
        {
            Text = info.FullName;
            FilePath = info.FullName;

            Nodes.Add(@"file")
                .Nodes.Add(info.LastWriteTime.ToString())
                .Parent.Nodes.Add(String.Format(@"{0:#,0}[B]", info.Length))
                ;
        }
    }




    /*
     * 
     * */
    public class GFTreeNode_ModelBase : GFTreeNode
    {
        public GFTreeNode_MotionBase Motion { get; set; }
        


        public GFTreeNode_ModelBase(string text)
            : base(text)
        {

        }

        public void SetupMotionFromFile(string path)
        {
            if (Motion != null)
            {
                Nodes.Remove(Motion);
            }
            Motion = GFTreeNode_MotionBase.CreateFromFile(path);
            Nodes.Add(Motion);
        }


        public static GFTreeNode_ModelBase CreateFromFile(string path)
        {
            GFTreeNode_ModelBase    node = null;
            string                  extension   = System.IO.Path.GetExtension(path).ToLower();

            if(extension.Equals(GFTreeNode_ModelBin.Extension))
            {
                /*   gfbmdlp   */
                node = new GFTreeNode_ModelBin(path);
                node.SetupFromFile(path);
            }
            else if(extension.Equals(GFTreeNode_ModelAscii.Extension))
            {
                /*   gfmdl   */
                node = new GFTreeNode_ModelAscii(path);
                node.SetupFromFile(path);
            }


            return node;
        }
    };
    /*
     * 
     * */
    public class GFTreeNode_ModelBin : GFTreeNode_ModelBase
    {
        public static string Extension { get { return @".gfbmdlp"; } }

        public GFTreeNode_ModelBin(string text)
            : base(text)
        {

        }

        public override void SetupFromFile(string path)
        {
            this.Nodes.Clear();
            base.SetupFromFile(path);
        }
    };

    
    /*
     * 
     * */
    public class GFTreeNode_ModelAscii : GFTreeNode_ModelBase
    {
        public static string Extension { get { return @".gfmdl"; } }
        public GFModel Model { get { return model; } }

        /*
         *
         * */
        protected GFModel model = null;


        public GFTreeNode_ModelAscii(string text)
            : base(text)
        {

        }

        public override void SetupFromFile(string path)
        {
            try
            {
                model = GFModel.Load(path);

                this.Nodes.Clear();
                base.SetupFromFile(path);

                {
                    TreeNode    rootNode = this.Nodes.Add(@"property");

                    {
                        /*   version   */
                        rootNode.Nodes.Add(String.Format(@"version: {0}", model.version));
                    }
                    {
                        /*   Source   */
                        rootNode.Nodes.Add(@"Source")
                            .Nodes.Add(String.Format(@"Path: {0}", model.Source.Path))
                            ;
                    }
                    {
                        /*   TextureList   */
                        if(model.TextureList != null)
                        {
                            TreeNode    listNode    = rootNode.Nodes.Add(@"TextureList");

                            foreach(GFModel_Texture tex in model.TextureList.Textures)
                            {
                                listNode.Nodes.Add(String.Format(@"{0}: {1}", tex.No, tex.FilePath));
                            }
                        }
                    }
                    {
                        /*  ShaderList  */
                        if(model.ShaderList != null)
                        {
                            TreeNode    listNode    = rootNode.Nodes.Add(@"ShaderList");

                            foreach(GFModel_Shader shader in model.ShaderList.Shaders)
                            {
                                TreeNode shaderNode = listNode.Nodes.Add(String.Format(@"{0}: {1}", shader.No, shader.Name));

                                if(!String.IsNullOrEmpty(shader.VshFilePath))
                                {
                                    shaderNode.Nodes.Add(String.Format(@"vsh: {0}", shader.VshFilePath));
                                }
                                if(!String.IsNullOrEmpty(shader.FshFilePath))
                                {
                                    shaderNode.Nodes.Add(String.Format(@"vsh: {0}", shader.FshFilePath));
                                }
                            }
                        }
                    }
                    {
                        /*   materials   */
                        if (model.Materials != null)
                        {
                            TreeNode listNode = rootNode.Nodes.Add(@"Materials");

                            foreach(GFModel_Material mat in model.Materials.Materials)
                            {
                                listNode.Nodes.Add(mat.Name);
                            }
                        }
                    }
                    {
                        /*   meshes   */
                        if (model.Meshes != null)
                        {
                            TreeNode listNode = rootNode.Nodes.Add(@"Meshes");

                            foreach(GFModel_Mesh mesh in model.Meshes.Meshes)
                            {
                                listNode.Nodes.Add(mesh.Name);
                            }
                        }
                    }
                    {
                        /*   skelton   */
                        if (model.Skelton != null)
                        {
                            TreeNode listNode = rootNode.Nodes.Add(@"Skelton");

                            foreach (GFModel_Node node in model.Skelton.Nodes)
                            {
                                listNode.Nodes.Add(node.Name);
                            }
                        }
                    }
                }
            }
            catch
            {

            }
        }
    };



    /*
     * 
     * */
    public class GFTreeNode_MotionBase : GFTreeNode
    {
        
        public GFTreeNode_MotionBase(string text)
            : base(text)
        {

        }


        public static GFTreeNode_MotionBase CreateFromFile(string path)
        {
            GFTreeNode_MotionBase node = null;
            string extension = System.IO.Path.GetExtension(path).ToLower();

            if (extension.Equals(GFTreeNode_MotionBin.Extension))
            {
                /*   gfbmot   */
                node = new GFTreeNode_MotionBin(path);
                node.SetupFromFile(path);
            }
            else if (extension.Equals(GFTreeNode_MotionAscii.Extension))
            {
                /*   gfmot   */
                node = new GFTreeNode_MotionAscii(path);
                node.SetupFromFile(path);
            }


            return node;
        }
    }


    /*
     * 
     * */
    public class GFTreeNode_MotionBin : GFTreeNode_MotionBase
    {
        public static string Extension { get { return @".gfbmot"; } }

        public GFTreeNode_MotionBin(string text)
            : base(text)
        {

        }

        public override void SetupFromFile(string path)
        {
            this.Nodes.Clear();
            base.SetupFromFile(path);
        }
    };

    /*
     * 
     * */
    public class GFTreeNode_MotionAscii : GFTreeNode_MotionBase
    {
        public static string Extension { get { return @".gfmot"; } }
        public GFMotion Motion { get { return motion; } }

        /*
         *
         * */
        protected GFMotion motion = null;


        public GFTreeNode_MotionAscii(string text)
            : base(text)
        {

        }

        public override void SetupFromFile(string path)
        {
            try
            {
                motion = GFMotion.Load(path);

                this.Nodes.Clear();
                base.SetupFromFile(path);

                {
                    TreeNode rootNode = this.Nodes.Add(@"property");

                    {
                        /*   version   */
                        rootNode.Nodes.Add(String.Format(@"ver: {0}", motion.version));
                    }
                    {
                        /*   Source   */
                        rootNode.Nodes.Add(@"Source")
                            .Nodes.Add(String.Format(@"Path: {0}", motion.Source.Path))
                            ;
                    }
                    {
                        /*   SkeletalAnimation   */
                        if (motion.SkeletalAnimation != null)
                        {
                            TreeNode listNode = rootNode.Nodes.Add(@"SkeletalAnimation");

                            foreach (GFMotion_SkeletalAnimationNode animNode in motion.SkeletalAnimation.Nodes)
                            {
                                listNode.Nodes.Add(animNode.Name);
                            }
                        }
                    }
                    {
                        /*   Skelton   */
                        if (motion.Skelton != null)
                        {
                            TreeNode listNode = rootNode.Nodes.Add(@"Skelton");

                            foreach (GFMotion_SkeltonNode skNode in motion.Skelton.Nodes)
                            {
                                listNode.Nodes.Add(skNode.Name);
                            }
                        }
                    }
                }
            }
            catch
            {

            }
        }
    };

}