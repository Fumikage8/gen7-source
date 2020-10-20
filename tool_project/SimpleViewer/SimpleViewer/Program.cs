using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
using gfl2.clr.gfx;
using gfl2.clr.renderingengine;
using gfl2.clr.renderingengine.scenegraph;
using gfl2.clr.renderingengine.scenegraph.instance;
using gfl2.clr.renderingengine.scenegraph.resource;

namespace SimpleViewer
{

  class Option
  {
    public Option(string[] args)
    {
      if (args == null)
      {
        this.IsInvalid = true;
        return;
      }

      if (args.Length == 0)
      {
        this.IsInvalid = true;
        return;
      }

      this.Parse(args);
      this.SetResourceDirectory();

    }

    private void SetResourceDirectory()
    {
      if (System.IO.File.Exists(this.InputPath))
      {
        string BasePath = System.IO.Path.GetDirectoryName(this.InputPath);

        // Set Shader Directory
        this.ShaderDirectoryPath = BasePath + "\\Shaders";
        if (!System.IO.Directory.Exists(this.ShaderDirectoryPath))
        {
          this.IsInvalid = true;
        }

        // Set Texture Directory
        this.TextureDirectoryPath = BasePath + "\\Textures";
        if (!System.IO.Directory.Exists(this.ShaderDirectoryPath))
        {
          this.IsInvalid = true;
        }

        if (this.OutputPath == null)
        {
          this.OutputPath = this.InputPath + ".bmp";
        }
      }
      else
      {
        this.IsInvalid = false;
      }
    }

    private void Parse(string[] args)
    {
      for ( var i = 0; i < args.Length; i++)
      {
        switch (args[i])
        {
          case "-i":
            try
            {
              this.InputPath = args[i + 1];
            }
            catch (IndexOutOfRangeException)
            {
              this.IsInvalid = true;
            }
            break;

          case "-o":
            try
            {
              this.OutputPath = args[i + 1];
            }
            catch (IndexOutOfRangeException)
            {
              this.IsInvalid = true;
            }
            break;

          case "-c":
            CameraType type = CameraType.DEFAULT;
            if (CameraTypeLabel.TryGetValue(args[i + 1], out type))
            {
              this.CameraType = type;
            }
            break;
        }
      }
    }

    private static Dictionary<string, CameraType> CameraTypeLabel =
      new Dictionary<string, CameraType>
      {
        {"DEFAULT", CameraType.DEFAULT},
        {"LEFT", CameraType.LEFT},
        {"RIGHT", CameraType.RIGHT},
        {"BACK", CameraType.BACK},
        {"TOP", CameraType.TOP},
        {"BOTTOM", CameraType.BOTTOM}
      };

    public string InputPath { get; set;  }
    public string ShaderDirectoryPath { get; set;  }
    public string TextureDirectoryPath { get; set;  }
    public string OutputPath { get; set; }
    public bool IsInvalid { get; set; }

    public CameraType CameraType { get; set; }
  }

  static class Program
  {
    /// <summary>
    /// アプリケーションのメイン エントリ ポイントです。
    /// </summary>
    [STAThread]
    static int Main(string[] args)
    {
      // parse arguments
      var option = new Option(args);

      if (option.IsInvalid)
      {
        return 1;
      }

      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);

      RenderForm renderForm = new RenderForm();

      GFGL.Initialize();
      gfl2.clr.renderingengine.scenegraph.InitDescription desc = new gfl2.clr.renderingengine.scenegraph.InitDescription(16384);
      SceneGraphManager.Initialize(desc);
      ResourceManager.Initialize();

      renderForm.Show();

      renderForm.CameraType = option.CameraType;
      renderForm.LoadPokemon(option.InputPath, option.ShaderDirectoryPath, option.TextureDirectoryPath);
      renderForm.OnUpdate();
      renderForm.OnDraw();

      renderForm.PrintScreen(option.OutputPath);

      renderForm.Close();

      return 0;
    }
  }
}
