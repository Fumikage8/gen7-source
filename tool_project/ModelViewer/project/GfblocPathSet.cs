using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ModelViewer
{
  public class GfblocPathSet
  {
    //!< @brief ロケータの種類
    public enum LocatorType
    {
      EFFECT,
      POKEDECO
    }

    private Dictionary<LocatorType, string> m_GfblocFilePathDictionary;
    
    public GfblocPathSet()
    {
      m_GfblocFilePathDictionary = new Dictionary<LocatorType, string>();

      foreach (LocatorType type in Enum.GetValues(typeof(LocatorType)))
      {
        m_GfblocFilePathDictionary[type] = "";
      }
    }

    public string this[LocatorType type]
    {
      set { m_GfblocFilePathDictionary[type] = value; }
      get { return m_GfblocFilePathDictionary[type]; }
    }
  }
}
