//======================================================================
/**
 * @file    ResortProc.h
 * @brief   15/05/24
 * @author  k.ohno
 * @data    描画テスト
 */
//======================================================================

#ifndef __RESORT_PROC_H__
#define __RESORT_PROC_H__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "AppLib/include/Util/AppRenderingManager.h"

#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"

#include "Collision/include/gfl2_CollisionModel.h"

#include "App/Resort/include/ResortAppParam.h"
#include "System/include/EnvAccessor.h"

#include "arc_index/islandground.gaix"
#include "arc_index/islandshadow.gaix"
#include "arc_index/islandbin.gaix"

#include "../source/ResortChangeIslandListener.h"

#include "../source/ResortSequenceListener.h"


namespace gfl2{
  namespace renderingengine {
    namespace scenegraph {
			namespace instance{
				class DrawEnvNode;
			}
      namespace resource{
        class ResourceNode;
      }
    }
  }
}


GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)



GFL_NAMESPACE_BEGIN(PokeTool)
class KawaigariParamCareCoreManager;
GFL_NAMESPACE_END(PokeTool)



#define SKYBOX_ISLAND_OPEN_ (1)
#define MODEL_ISLAND_OPEN_ (1)
#define FILE_OFF_OPEN_ (1)
#define ENVNODE_OFF_OPEN_ (1)
#define ALL_OFF_OPEN_ (1)
#define RENDERL_OFF_OPEN_ (1)
#define LAYOUT_OPEN_ (1)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

#if PM_DEBUG
class ResortDebugWin;
#endif
class ResortFrame;
class ResortIslandDraw;
class ResortDrawControl;
class ResortInitLogic;

class ResortProc : public GameSys::GameProc , public ResortChangeIslandListener, public ResortSequenceListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ResortProc); //コピーコンストラクタ＋代入禁止


  static const int _ONEGROUND_SOZAI_NUM = 7;   //ひとつの島の３Ｄ素材
  static const int _ONEGROUND_SOZAI_ALLLV_NUM = 21;   //ひとつの島の３Ｄ素材（ＬＶあわせたもの）


  
public:
  ResortProc(); //コンストラクタ
  virtual ~ResortProc(void);  //デストラクタ

  //============================================================================
  //GameProc
  //============================================================================
  //------------------------------------------------------------------
  /**
    * @brief   PROCの初期化を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief   PROCの終了を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief   PROCのメインを行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief	PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  //--------------------------------------------------------------------------
  /**
   * @brief   切り替え開始
   * @param   index   リゾートインデックス
   *  @return  切りかえれる場合true
   */
  //--------------------------------------------------------------------------
  virtual bool ChangeStart( int index,bool bfade );
  //--------------------------------------------------------------------------
  /**
   * @brief   モデルアニメ開始
   * @param   type  ModelAnim_e
   */
  //--------------------------------------------------------------------------
  virtual void ResortProc::StartModelAnim( int type );

  //--------------------------------------------------------------------------
  /**
   * @brief   ホームメニューオープン
   */
  //--------------------------------------------------------------------------
  virtual void OpenHomeMneu( bool flg );

  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンをタッチした後、メニューフローが終わったときに呼ばれる
   */
  //--------------------------------------------------------------------------
  virtual void ClosePokemonTouchMenu( void );
  
  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンをタッチ出来る状態になっていればtrue 忙しいとfalse
   */
  //--------------------------------------------------------------------------
  virtual bool IsPokemonTouchEnable( void );
  
public:

  void SetAppParam(App::Resort::APP_PARAM* pAppParam);

  void CreateMemory(void);

  void SetupBackgroundRenderCamera( void );


	void UpdateTouch(void);

  
  void UpdateCollision_Height( gfl2::math::Vector3* pVec );

  
private:

  
  static const int SIZE_MY_APP = 0x400000;       /// アプリメモリも容量管理のため分けて管理
  static const int SIZE_MY_APPDEV = 0xF00000;    /// アプリメモリも容量管理のため分けて管理
  
  void fileReadStructSetup(app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, app::util::Heap* pAppHeap, void** pReadBuffAddr);
  bool fileOpenSequece(void);
  bool fileReadSequece(void);
  bool fileCloseSequece(void);

  void debugkey(void);
  void InitCamera(void);
  void BillboardVisible(bool bVisible);

  void BillboardMove(void);

  void InitLayout(void);
  void DeleteLayout(void);

  bool DeletePokemonASync(void);

  bool ModelTouchHitCheck( u16 x, u16 y);
  bool PokeModelTouchHitCheck( u16 x, u16 y);

  bool IsPokemonCameraMode(void);
//  void PokeNearCameraInit(void);
//  void PokeNearCamera(void);

  void PokeNearCameraInit(int no);
  void PokeNearCamera(int no);
  
  void SetBillboardRotation(void);
  void ChangeFadeStart(bool bfade);

  void ChangeGo(void);
  void ChangeDel(void);

  bool ReadBinData(void);
  //void ReadBinData(void);  // ResortDataManagerを使うので、bool ReadBinData(void);に変える。
  void FreeBinData(void);

  void BinLinkLoad(app::util::Heap* pAppHeap);
  bool IsBinLinkLoad(void);
  void* GetBinLinkReadBuff(int linkno);
  u32  GetBinLinkBuffSize( int linkno );
  void BinLinkClose(app::util::Heap* pAppHeap);

  
  
  int m_Seq;       ///InitとEndのシーケンス
  int mFileSeq;
  int m_ModeSeq;
  int m_DebugTime;
	int m_bMapChangeWait;


  struct FileMan{
    app::util::FileAccessor::RequestData* pFH;
    int step;
  };
  

  //読み込んだりソース管理
  static const int READ_DATA_NUM = GARC_islandground_DATA_NUM;
  //使わなくなったのでコメントアウトFileMan mFrs[READ_DATA_NUM];

  //バイナリ管理
  static const int _READBIN = GARC_islandbin_DATA_NUM;
  void* mpReadBuffAddr[_READBIN];


  // ResortDataManagerがオープンしてくれているgarcファイルから、「島とLv」でパックしたBinLinkerファイルを読み込む。
  s32                          m_islandGroundLvPackLoadSeq;
  gfl2::fs::ArcFile::ARCDATID  m_islandGroundLvPackDatId;
  void*                        m_islandGroundLvPackBuf;
  gfl2::fs::ArcFile::ARCDATID getIslandGroundLvPackDatId(u32 island, u32 lv) const;
  u32 getIslandGroundLvStartIndex(gfl2::fs::ArcFile::ARCDATID datId) const;

  
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_collisionResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansModelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pGroundAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pDrawEnvResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pDrawEnvMotionResourceRootNode;

  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pKinomiModelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pKinomiAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pKinomiKiAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pKinomiMiAnimResourceNode;

  void* mpResourceBuffer[GARC_islandground_DATA_NUM];   //読み込んだ素材管理バッファ


//  AppLib::Fade::DemoFade* m_pDemoFade;
//  gfl2::heap::HeapBase *mpFadeHeap;

  
  gfl2::heap::HeapBase* m_pLocalDeviceHeap;                   // 専用のヒープ、寿命も管理する　
  System::nijiAllocator* m_pLocalDeviceHeapAllocator;      // 専用のアロケーター、寿命も管理する　

  gfl2::heap::HeapBase* m_pIslandGroundLvDevHeap;

  gfl2::heap::HeapBase* mpRootHeap;  // Procで使うヒープの大元
  app::util::Heap* mpAppHeap;   //メモリ
  gfl2::heap::HeapBase* mpLogicHeap;
  gfl2::heap::HeapBase* mpTempHeap;  // 小物用一時ヒープ。確保したらすぐ解放するようなものにしか使わない。
  gfl2::heap::HeapBase* mpPokeModelHeap;
  System::nijiAllocator* mpPokeModelNijiAllocator;

  App::Resort::ResortFrame* m_pResortFrame;   //レイアウトフレーム 上下両用

  App::Resort::ResortIslandDraw* mpResortIslandDraw;  //島の描画、ＯＢＪこみ

  App::Resort::ResortDrawControl* mpResortDrawControl;  //
  
  App::Resort::ResortInitLogic*  mpLogic;
  
  App::Resort::APP_PARAM* mpAppParam;     //初期化パラメータ

  gfl2::collision::CollisionModel* m_pCollisionModel;    //地面コリジョン

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_drawEnvNode;  // カメラを含む環境
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;  // カメラを含む環境
  
  System::nijiAllocator* m_pNijiAllocator;  //レンダリングに必要なアロケータ
  app::util::AppRenderingManager* m_appRenderingManager;    ///APPレンダリングマネージャー
  app::util::AppRenderingManager::Description m_Description;  //APPレンダリングマネージャーの設定

  static const int _SHADOW_READBUFF_MAX = 5;
  void* mpShadowReadBuffAddr[_SHADOW_READBUFF_MAX];
  
  void* m_dataBuf;
  gfl2::fs::BinLinkerAccessor         m_binLinker;

  System::EnvAccessor         m_EnvAccessor;    ///< DrawEnv共通処理アクセサー
  
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pLight; //ライト
  int m_TestTimer;                                    ///かわいがりアニメの間を取るためのタイマー。アルファ用


  //カメラ管理
  f32 BILLBOARD_COMP;
  f32 ISLAND_DIST_INIT;
  f32 ISLAND_HEIGHT_INIT;

  f32 m_IslandDist;  //注視点までの距離
  f32 island_camera_rotate;  //左右というか 回転
  gfl2::math::Vector3 m_TargetPos;    ///注視点保存用

  gfl2::math::Vector3 m_PokeTargetPos;    ///ポケモン注視点保存用
  

  int m_asyncCreateDestroyPokeModelStep;    //ポケモンモデル生成用
  int m_PokeDeleteStep;   ///ポケモンモデル削除用

  int mChangeStep;
  int mChangeNo;
  
  int m_TouchPokeIndex;      //タッチしたポケモンのインデックス番号

  applib::frame::Manager *mpFrameManager;
  gfl2::heap::HeapBase *mpManagerHeap;
  //使っていないのでコメントアウトPokeTool::KawaigariParamCareCoreManager* mpCareCoreParamManager;
 
  bool asyncCreatePokeModel(void);

  gfl2::gfx::IGLAllocator* mpOldAllocator;   //セットされているアロケータを記憶するもの

  int mbFadeFlg;
  
#if PM_DEBUG
  ResortDebugWin* mResortDebugWin;
  gfl2::heap::HeapBase* m_pLocalDebugDeviceHeap;
#endif
};  //class BattleProc









GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

/*
▼場所
[フィールドR+Xメニュー]→[Field]→[木の実]

位置番号についてはこのメールに添付されている「きのみ配置管理表」をご確認ください。
なお、きのみ配置管理表の最新版は下記位置にコミットされています。
sango_field\convert\nuts\config\field_nuts_point.xlsx

よろしくお願いいたします。

------------------------
生田　淳也





0000000000000000000000000000000000000000000000000000000000000000000000000000000000000

■kinomi00.cskla
木の揺れ


■kinomi00_anim_ki.cmata

フレーム
　　　　0:土
　　　　1:土（水やり後）
　　　　2:芽
　　　　3:芽（水やり後）
　　　　4:小木
　　　　5:小木（水やり後）
　　　　6:大木
　　　　7:大木（水やり後）


■kinomi00_anim_mi.cmata

フレーム
　　　　0:なし
　　　　1:花
　　　　2～15：花予備
　　　　16～82：木の実１番～67番
　　　　83～95：実予備
*/



#endif  //__RESORT_PROC_H__
