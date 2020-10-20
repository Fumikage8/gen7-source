//======================================================================
/**
 * @file DendouDemoPlayerModel.h
 * @date 2016/03/01 12:33:11
 * @author  nagihashi_toru(移植), Hiroyuki Nakamura
 * @brief 殿堂入りデモ プレイヤーモデル関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined( __DENDOU_DEMO_PLAYERMODEL_H__ )
#define __DENDOU_DEMO_PLAYERMODEL_H__

#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

namespace app { namespace tool { class CharaSimpleModel; } }
namespace Savedata { class MyStatus; }
namespace poke_3d { namespace model { class DressUpModel; class CharaModelFactory; class DressUpModelResourceManager; } }

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	app::dendou_demo
 * @brief			殿堂入りデモ
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace app {
namespace dendou_demo {

//--------------------------------------------------------------------------------------------
/**
 * @brief	プレイヤーモデル
 */
//--------------------------------------------------------------------------------------------
class PlayerModel {
public:
  struct Description {
		app::util::Heap * heap;
    gfl2::fs::AsyncFileManager * asyncFileManager;
    app::util::AppRenderingManager * renderingManager;
  };
public:
	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		コンストラクタ
	 *
	 * @param		description 設定
	 */
	//--------------------------------------------------------------------------------------------
	PlayerModel( const Description & description );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		デストラクタ
	 */
	//--------------------------------------------------------------------------------------------
	virtual ~PlayerModel();

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		アップデート
	 *
	 * @param		none
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
	void Update(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		モデル作成
	 *
	 * @param		myst								プレイヤーデータ
	 * @param		resident_resource		ゲーム常駐リソース
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
	void Create(
        const Savedata::MyStatus * myst );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		モデル削除
	 *
	 * @param		none
	 *
	 * @retval	"false = 削除中"
	 * @retval	"true = それ以外"
	 */
	//--------------------------------------------------------------------------------------------
	bool Delete(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		作成されたか
	 *
	 * @param		none
	 *
	 * @retval	"true = 完了"
	 * @retval	"false = それ以外"
	 */
	//--------------------------------------------------------------------------------------------
	bool IsCreated(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		表示切り替え
	 *
	 * @param		flg		true = 表示, false = 非表示
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
	void SetVisible( bool flg );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		終了モーション設定
	 *
	 * @param		none
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
	void SetEndMotion(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		終了モーションの次のモーションを設定
	 *
	 * @param		none
	 *
	 * @return	none
	 *
	 *	※いらなくなったので空に
	 */
	//--------------------------------------------------------------------------------------------
	void SetEndMotionNext(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		モデル設定
	 *
	 * @param		none
	 *
	 * @retval	"true = 設定完了"
	 * @retval	"false = それ以外"
	 */
	//--------------------------------------------------------------------------------------------
	bool Setup(void);

private:
	app::util::Heap * pHeap;							//!< ヒープ
  gfl2::fs::AsyncFileManager * asyncFileManager; //!< ファイルリードマネージャ
  app::util::AppRenderingManager * renderingManager; //!<レンダリングパイプライン
  gfl2::heap::HeapBase*               pModelFactoryHeap;
  gfl2::heap::HeapBase*               pModelResourceManagerHeap;

  poke_3d::model::DressUpModel * pModel;
  poke_3d::model::CharaModelFactory*                    pCharaModelFactory;
  poke_3d::model::DressUpModelResourceManager*          pDressUpModelResourceManager;
  poke_3d::model::DressUpParam                          dressUpParam;
  u32 modelID;
  u32 waitMotID;
  u32 deleteSeq;
  u32 setupSeq;
  u32 updateSeq;
  u32 counter;
  u32 max_counter;
  u32 actionMotID;
  u32 actionMotPack;
};


}	// dendou_demo
}	// app

#endif	// __DENDOU_DEMO_PLAYERMODEL_H__
