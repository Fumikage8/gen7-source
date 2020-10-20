using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ModelViewer
{
	public class ModelMotionSettingData
	{
		// PokemonModelに関するデータ
		public string m_GfmdlFilePath;
		public string[] m_GfmotFilePaths;

		// AnimationTimeLineに関するデータ
		public bool m_LoopEnable;
		public decimal m_FrameResolution;
		public string[] m_SelectedGfmotFilePaths;
		public decimal[] m_StartFrames;
		public decimal[] m_EndFrames;
		public decimal[] m_InterpSizes;
	}
}
