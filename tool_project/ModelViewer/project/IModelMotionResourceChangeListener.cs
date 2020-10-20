using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ModelViewer
{
	public interface IModelMotionResourceChangeListener
	{
		/**
		 * @brief モデル/モーションデータの変更通知受理
		 * @param PokemonModelDataManager
		 */
		void ReceiveNotification(PokemonModelDataManager modelDataManager);
	}
}
