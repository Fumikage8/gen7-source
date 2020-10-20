using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ModelViewer
{
  class Options
  {
    [CommandLine.Option('c', HelpText = "Run ModelViewer as Command Line Mode")]
    public bool IsCommanLineMode { get; set; }

    [CommandLine.Option('i', "input", Required = true, HelpText = "Path to \"exports\" Folder")]
    public string ExportsDirectory { get; set; }

    [CommandLine.OptionArray('m', "motion", HelpText = "List of motion names")]
    public string[] Motions { get; set; }

    [CommandLine.HelpOption]
    public string GetUsage()
    {
      var help = new CommandLine.Text.HelpText();

      help.AddDashesToOption = true;

      help.AddPreOptionsLine("Usage: ModelViewer.exe -c -i [ Exports Directory ] [-m] [Motion List] ");
      help.AddPreOptionsLine("");
      help.AddPreOptionsLine("Options:");

      help.AddOptions(this);

      return help;
    }
  }
}
