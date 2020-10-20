using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ModelViewer
{
	public class ModelMotionResourceLoader
	{
		private readonly ModelMotionResourceTreeViewForm m_ModelMotionResoureceTreeViewForm;

		/**
		 * @brief コンストラクタ
		 * @param modelMotionResoureceTreeViewForm ツリービューを表示するフォーム
		 */
		public ModelMotionResourceLoader(
			ModelMotionResourceTreeViewForm modelMotionResoureceTreeViewForm)
		{
			m_ModelMotionResoureceTreeViewForm = modelMotionResoureceTreeViewForm;
		}

		/**
		 * @brief リソース読み込み後、ツリービューに追加
		 * @param string gfmdlFilePath gfmdl形式のファイルパス
		 * @param string gfmotFilePaths gfmot形式のファイルパス一式
		 * @param frameResolution フレームの分解能
		 * @param loopEnable ループON/OFF
		 * @param isRare レア/通常
		 * @return PokemonModelDataManager ポケモンモデルデータマネージャ
		 */
		public PokemonModelDataManager LoadGfResources(
      string importDirectory,
      GfMdlResourcePathSetDictionary gfmdlResourcePathSetDirectory,
      GfMdlResourcePathSetDictionary shadowGfmdlResourcePathSetDirectory,
			GfmotPathSet gfmotPathSet,
      GfblocPathSet gfblocPathSet,
      ExtraFilePathSet extraFilePathSet,
			decimal frameResolution,
			bool loopEnable,
			bool isRare)
		{
			return m_ModelMotionResoureceTreeViewForm.LoadGfResources(
        importDirectory,
        gfmdlResourcePathSetDirectory,
        shadowGfmdlResourcePathSetDirectory,
        gfmotPathSet,
        gfblocPathSet,
        extraFilePathSet,
				frameResolution,
				loopEnable,
				isRare);
		}
	}
}
