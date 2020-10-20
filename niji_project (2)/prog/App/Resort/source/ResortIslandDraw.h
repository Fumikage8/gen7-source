//======================================================================
/**
 * @file    ResortIslandDraw.h
 * @brief   島の描画
 * @author  k.ohno
 * @data    15/09/05
 */
//======================================================================

#ifndef __RESORT_ISLAND_DRAW_H__
#define __RESORT_ISLAND_DRAW_H__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "AppLib/include/Util/AppRenderingManager.h"

#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"
#include "PokeTool/include/PokeTool.h"

#include "Collision/include/gfl2_CollisionModel.h"

#include "App/Resort/include/ResortAppParam.h"
#include "System/include/EnvAccessor.h"

#include "ResortTouchToFlowListener.h"

#include "arc_index/islandground.gaix"

#include "Savedata/include/ResortSave.h"
#include "niji_conv_header\app\island\walk_hotspa_islandbin.h"
#include "niji_conv_header\app\island\camera_islandbin.h"
#include "niji_conv_header\app\island\kinomipos_islandbin.h"

#include "niji_conv_header\app\island\moveflg_islandbin.h"

#include "niji_conv_header\app\island\walk_farm_islandbin.h"
#include "niji_conv_header\app\island\walk_adv_islandbin.h"
#include "niji_conv_header\app\island\walk_gim_islandbin.h"
#include "niji_conv_header\app\island\walk_friend_islandbin.h"
#include "niji_conv_header\app\island\standpos_islandbin.h"


#include "niji_conv_header\app\island\farm_touch_islandbin.h"
#include "niji_conv_header\app\island\hotspa_touch_islandbin.h"
#include "niji_conv_header\app\island\friendship_touch_islandbin.h"
#include "niji_conv_header\app\island\adventure_touch_islandbin.h"
#include "niji_conv_header\app\island\gim_touch_islandbin.h"
#include "niji_conv_header\app\island\beans_pos_islandbin.h"
//#include "niji_conv_header\app\island\poke_camera_islandbin.h"
#include "niji_conv_header\app\island\poke_camera_s_islandbin.h"
#include "niji_conv_header\app\island\poke_camera_i_islandbin.h"




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



GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)
class BaseCamera;
class BaseModel;
GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)


GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Util)
class AppModel;
GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(AppLib)

GFL_NAMESPACE_BEGIN(PokeTool)
class PokeModelSystem;
class PokeModel;
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
class ResortInitLogic;  //参照

enum{
  MODEL_CREATE,
  MODEL_ADD,
  MODEL_REMOVE,
  MODEL_DELETE,
};


enum{
  BEANS_BOTTLE_ANIM_ADD,  //まめびんがある
  BEANS_BOTTLE_ANIM_PICKUP, //ひろった
  BEANS_BOTTLE_ANIM_NONE,    //無い
};



enum{
  BEANS_ANIM_GROUND,  //地面
  BEANS_ANIM_FALL,    //木から落ちてる所
  BEANS_ANIM_PICKUP,  //拾っている所
  BEANS_ANIM_NONE,    //表示して無い
  BEANS_ANIM_START,  //ふらせる
};



struct ModelData{
  poke_3d::model::BaseModel* pModel;
  int seq;  //シーケンス状態
  int anim; //描画状態
};


struct KinomikiModelData{
  poke_3d::model::BaseModel* pModel;
  int flower; //花状態
  int mi;//実の番号
  bool bAdd;//RenderringManagerに追加したらtrue
  bool bview;
};


class ResortIslandDraw
{
  GFL_FORBID_COPY_AND_ASSIGN(ResortIslandDraw); //コピーコンストラクタ＋代入禁止


  static const int _POKEMON_NEAR_FRAME = 10;
	static const int _POKE_CAMERA_MAX=600;
  static const int _FUKIDASHI_UPDATE=30;

public:
  //! @brief  テクスチャリソースノードゲッター
  //! @note   gfl2::renderingengine::scenegraph::Traverser::VisitorのImplementに渡すクラスとしての役割と
  //!         テクスチャリソースノードを取得するクラスとしての役割を持つ。
  class TextureResourceNodeGetter
  {
    GFL_FORBID_COPY_AND_ASSIGN(TextureResourceNodeGetter);

  public:
    //! @brief  gfl2::renderingengine::scenegraph::Traverser::VisitorのImplementに必要なメンバ関数
    void Visit(gfl2::renderingengine::scenegraph::DagNode* dagNode);
  
  public:
    //! @brief  dagNode内に存在するテクスチャリソースノードを取得する(最初の1個を見付けた時点で探すのを止める)
    gfl2::renderingengine::scenegraph::resource::TextureResourceNode* GetTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* dagNode);

  public:
    TextureResourceNodeGetter(void);
    virtual ~TextureResourceNodeGetter();

  private:
    gfl2::renderingengine::scenegraph::resource::TextureResourceNode* getTextureResourceNode(void);

  private:
    gfl2::renderingengine::scenegraph::resource::TextureResourceNode* m_textureResourceNode;
  };

  
public:
  ResortIslandDraw(); //コンストラクタ
  virtual ~ResortIslandDraw(void);  //デストラクタ

  //------------------------------------------------------------------
  /**
   * @brief   初期化を行う関数
   * @return  初期化完了 true
   */
  //------------------------------------------------------------------
  bool InitFunc(void);

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  bool EndFunc(void);

  //------------------------------------------------------------------
  /**
    * @brief   PROCのメインを行う関数
    */
  //------------------------------------------------------------------
  void UpdateFunc(int debugflg);
  void UpdateFuncBefore(void);


  //------------------------------------------------------------------
  /**
    * @brief   コントロール
    */
  //------------------------------------------------------------------
  enum{
    CONTROL_NONE,
    CONTROL_DISABLE,
    CONTROL_TOUCH_TREE,
    CONTROL_TOUCH_TREE_FIRST,
    CONTROL_TOUCH_BOX,
    CONTROL_TOUCH_BOARD,
    CONTROL_TOUCH_BOTTLE,
    CONTROL_TOUCH_ADV_KANBAN,
    CONTROL_TOUCH_GIM,
    CONTROL_TOUCH_GIM_KANBAN,
    CONTROL_TOUCH_HOTSPA_KANBAN,
    CONTROL_TOUCH_FARM_KANBAN,
    CONTROL_TOUCH_GIM_KANBAN2,
    CONTROL_TOUCH_HOTSPA_KANBAN2,
    CONTROL_TOUCH_FARM_KANBAN2,
    CONTROL_TOUCH_GIM_KANBAN3,
    CONTROL_TOUCH_HOTSPA_KANBAN3,
    CONTROL_TOUCH_FARM_KANBAN3,
    CONTROL_TOUCH_ADV_CHECK,
    CONTROL_TOUCH_BEANS_BOTTLE,
    CONTROL_TOUCH_POKEMON,
    CONTROL_TOUCH_FARM_HARVEST,
  };

  enum{
    CONTROL_BOARD1 = 0,
    CONTROL_BOARD2,
    CONTROL_BOARD3,
  };
  


  
#if PM_DEBUG
  int ControlFunc(bool bDebugCameraMode,bool flg);
#else
  int ControlFunc(bool flg);
#endif

  
public:

  bool IsBeansBoxTouchAnimLastFrame(void);
  void BeansBoxTouchAnimEnd(void);
  bool IsTreasureBoxTouchAnimLastFrame(void);
  void SetAdventureInEnd(void);


  void SetInitCameraData(void* pData);
  void SetHotSpaMove(void* pReadBuffAddr);
  void SetFarmMove(void* pReadBuffAddr);
  void SetFriendMove(void* pReadBuffAddr);
  void SetGimMove(void* pReadBuffAddr);
  void SetAdvMove(void* pReadBuffAddr);
  void SetKinomiPos(void* pReadBuffAddr);
	void SetKanbanPos(void* pReadBuffAddr);
  void SetPokeMoveFlg(void* pReadBuffAddr);
  void SetTouchAdvBin(void* pReadBuffAddr);
  void SetTouchGimBin(void* pReadBuffAddr);
  void SetStandPosBin(void* pReadBuffAddr);

  void SetFarmTouchBin(void* pReadBuffAddr);
  void SetHotSpaTouchBin(void* pReadBuffAddr);
  void SetFriendShipTouchBin(void* pReadBuffAddr);
  void SetAdventureTouchBin(void* pReadBuffAddr);
  void SetGimTouchBin(void* pReadBuffAddr);
  void SetBeansPosBin(void* pReadBuffAddr);
//  void SetPokeCameraBin(void* pReadBuffAddr);
  void SetPokeCameraSimpleBin(void* pReadBuffAddr);
  void SetPokeCameraIslandBin(void* pReadBuffAddr);

  
  void BillPokemonChange(void);

  void SetLogic( ResortInitLogic* pLogic );

  void SetParam(App::Resort::APP_PARAM* pAppParam, app::util::AppRenderingManager* pApR,void** pResource, app::util::Heap* pHeap,
                gfl2::heap::HeapBase* pPokeModelHeap, System::nijiAllocator* pPokeModelNijiAllocator,
                System::nijiAllocator* pniji, System::nijiAllocator* /*使っていないのでコメントアウトplocal*/);

  inline void SetListener(ResortTouchToFlowListener* pResortTouchToFlowListener){  mpResortTouchToFlowListener=pResortTouchToFlowListener; };
  
  void CreateMemory(void);

  void SetupBackgroundRenderCamera( void );

  void PokemonTouchAnimEnd(void);

  bool SetSelectPokemonMulScaleTable(int frame);
  bool SetApperPokemonMulScaleTable(int index, int frame);

  bool IsBeansPickuping(void);

  
  void SetPokemonMulScale(int index,float scale);


  void PutBeansBottle(void);
  bool BeansBottleTouchAnim(void);
  void BeansBottleCreate(void);
  void BeansBottleRemove(void);
  bool BeansBottleDelete(void);
  void BeansBottleUpdate(void);

  bool IsBeansBottleTouchLastFrame(void);
  void BeansBottleTouchEnd(void);

  void BeansCreate(void);
  void BeansReAdd(void);
  void BeansPause(void);
  void BeansRemove(void);
  bool BeansDelete(void);


  void KinomiCreate(void);
  void KinomiUpdate(void);
  void KinomiRemove(void);
  bool KinomiDelete(void);

  void KinomiPause();
  void KinomiReAdd();

  
  void BillCreate(void);
  void BillReAdd(void);
  void BillPause(void);
  void BillRemove(void);
  bool BillDelete(void);
  void TreasurePokemonInAnim(void);
  void TreasurePokemonOutAnim(void);
  bool TreasurePokemonAnimEnd(void);
  void TreasurePokemonNormalAnim(void);


  void GroundCreate(void);
  void GroundReAdd(void);
  void GroundPause(void);
  void GroundRemove(void);
  bool GroundDelete(void);

	int UpdateTouch(void);
  void UpdatePokemonMove(int debugno);
  bool IsPokemonMoveSeq(void);


  void UpdateCollision_Height( gfl2::math::Vector3* pVec );


  //--------------------------------------------------------------------------
  /**
   * @brief   リゾートの変更
   * @param   dir   移動方向(0: 左、1: 右)
   */
  //--------------------------------------------------------------------------
  void FeedResort( u8 dir );

  //--------------------------------------------------------------------------
  /**
   * @brief   リゾートのキー操作を許可するかどうか
   * @param   true許可  false非許可
   */
  //--------------------------------------------------------------------------
  void SetKeyEnable( bool bKeyEnable ){  mbKeyEnable = bKeyEnable;  };


  void CreateAll(void);
  void RemoveAll(void);
  bool DeleteAll(void);
  
  void InitCamera(void);

  void SetPokemonWalkEnd(void);
  void InitSetMoveTable(void);

  //ポケモン吹き出し変更
  void FukidashiChange(int x,int id);

private:
  bool GroundBeansHitCheck( gfl2::ui::TouchPanel * tp );

private:
	int GetBillMax(void);
  void CalcCamera( gfl2::math::Vector3* pTargetPos, f32 rotate, f32 dist,f32 cameraHeight,f32 fovy);
  void SetCamera( gfl2::math::Vector3* pTargetPos,gfl2::math::Vector3* pMyPos, f32 fovy);

  void SetBeansAnim(int x, float fx, float fz,bool banim);
  void PutBeans(bool banim);
  
  //使っていないのでコメントアウトvoid fileReadStructSetup(app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, app::util::Heap* pAppHeap, void** pReadBuffAddr);
//  bool fileReadSequece(void);

#if PM_DEBUG
  void debugkey(void);
	void debugkey_nearpoke(void);

#endif  // PM_DEBUG
  void BillboardVisible(bool bVisible);

 
  void BillboardMove(void);

  bool DeletePokemonASync(void);

  bool BottleTouchHitCheck( u16 x, u16 y);
  Savedata::ResortSave::PokeBeansRank ModelTouchHitCheck( u16 x, u16 y);
  bool PokeModelTouchHitCheck( u16 x, u16 y);
  bool BeansIvyCheck(float x,float y);
  //つたからまめをふらす
	public:
  bool BeansTreePut(u8* buff);
  void BeansTreePutEnd(void);
	public:
  bool IsPokemonCameraMode(void);
	private:
  bool IsPokemonBillboardMode(void);

  void PokeNearCameraInit(int no);
  void PokeNearCamera(int no);
//  bool PokeIndividualCamera(void);
  void PokeStanderdCamera(gfl2::math::Vector3* addpos, float adddist);
  bool PokeIndividualSimpleCamera(void);
  bool PokeIndividualIslandCamera(void);

//  bool PokeIndividualRotate(void);
  bool PokeIndividualSimpleRotate(void);
  bool PokeIndividualIslandRotate(void);


  
  void SetBillboardRotation(void);
  void KinomiKindStateUpdate(int x,int kinomino);

  
  void ConvertWorldPointToScreenPoint(
    f32*                          screen_point_x,
    f32*                          screen_point_y,
    const gfl2::math::Vector3&        world_point  ) const;


  void ConvertWorldPointToLowerScreenPoint(
    s32*                          lower_screen_point_x,
    s32*                          lower_screen_point_y,
    const gfl2::math::Vector3&        world_point ) const;


	void _pokemonSetRotate(float rotate);

  void PokemonRotateInit(void);
  bool PokemonRotate(void);
  void PokeMove(void);
  
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* GetJointNode( char *name, poke_3d::model::BaseModel* pBase );
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* GetMeshNode( char *name, poke_3d::model::BaseModel* pBase );
  gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* GetMaterialNode( char *name, poke_3d::model::BaseModel* pBase );
  int GetMaterialNodeIdx( char *name, poke_3d::model::BaseModel* pBase );

  void SetPokemonMaterialInstance(int i,bool flip);


  //void OpenPokemonCapture(void);
  //void ClosePokemonCapture(void);
  void PokemonCaptureRead(int arcFileIndex, void** pReadBuff, gfl2::heap::HeapBase* heap);
public:
  void PokeShadowCreate(void* pSData,void* pMData,void* pLData,void* pHData,void* pWData);
  void StartModelAnim(int type);
  void SimpleParamCreate(void);
  void SetFukidashiAllVisible(bool onoff);
  bool IsSignBoardTouchAnim(int z);
  void AdventureSignBoardTouchEnd(int boardno);
  void GimTouchEnd(int boardno);
  void GimSignBoardTouchEnd(int boardno);
  void HotSpaSignBoardTouchEnd(int boardno);
  void FarmSignBoardTouchEnd(int boardno);
  void AdventureTreasureBoxTouchEnd(void);
  void ResortButtonInputEnabled(bool bEnabled);
  void ButtonDisplayTypeEnabled( bool bflg );
  void ResortInputEnabled(bool bEnabled);
  bool IsAnimEndGetEffect(void);


private:
  void SimpleParamUpdate(void);
  int GetSimpPokemonMonsNo(int x);


private:
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* getPokemonJoint(int index);
  void backupPokemonIconScale(int index);
  void restorePokemonIconScale(int index);

  void PokeShadowDelete(void);
  void BeansAnim(void);
  f32 GetCamAdjustHeight(void);

  
  farm_touch_island_binstr* getTouchCheckTable(void);     //タッチのデータテーブルの取得
  bool getSkipCheck(farm_touch_island_binstr* pTouchCheck, int lv, int index);   //このデータを解析するかどうか
  void GetPokeBoxPos(gfl2::math::Vector3* pPos);
  bool GetSignBoardPos(gfl2::math::Vector3* pPos, int index);

  void BeansTreeCreate(void);
  void BeansTreeAnim(void);
  void BeansTreeUpdate(void);
  void BeansTreeReAdd(void);
  void BeansTreePause(void);
  void BeansTreeRemove(void);
  bool BeansTreeDelete(void);

  void BeansBoxTouchAnim(void);
  void BeansBoxCreate(void);
  int GetBeansBoxTouchAnimNum(void);
  void BeansBoxUpdate(void);
  void BeansBoxReAdd(void);
  void BeansBoxPause(void);
  void BeansBoxRemove(void);
  bool BeansBoxDelete(void);

  void TreasureBoxTouchAnim(void);
  void TreasureBoxCreate(void);
  void TreasureBoxUpdate(void);
  void TreasureBoxRemove(void);
  bool TreasureBoxDelete(void);
  void TreasureBoxReAdd(void);
  void TreasureBoxPause(void);
  
  //かんばんの作成
  void SignBoardCreate(void);
  void SignBoardTouchAnim(int z);
  void SignBoardUpdate(void);
  void SignBoardRemove(void);
  bool SignBoardDelete(void);

  void SignBoardReAdd(void);
  void SignBoardPause(void);

  
  bool GetBottleCheckPos(gfl2::math::Vector3* pPos);
  bool GetPokemonPos(int index,gfl2::math::Vector3* ppos);
	bool GetTreasureCheckPos(gfl2::math::Vector3* pPos);
  void SetPokemonVisible(int index, bool visible);
  void BeansUpdate(void);

  void PokemonCaptureRead(int no, int poke);

  int kanbanCheck(float x,float y);
  bool beansBoxCheck(float x,float y);
  bool touchCheck(float x,float y, gfl2::math::Vector3* pos);
  bool farmPosCheck(float x,float y);
  bool nutsPosCheck(float x,float y,int* retcode);
  bool gimPosCheck(float x,float y);

  void GetPokemon(pml::pokepara::CoreParam* pPoke, int x);
  void SetPokemon(pml::pokepara::CoreParam* pPoke, int x);
  bool PokemonRotateChange(void);

  void KinomiStateUpdate(void);
  void KinomiGrowStateUpdate(int x,int no);

  void createPokeLight(void);
  void destroyPokeLight(void);
  void updatePokeLight(void);

  void updateSeaOneDayAnim(s32 hour, s32 minute, s32 second);

  void _fukidashiUpdateCheck(void);

private:
  //--------------------------------------------------------------------------
  /**
   * @brief UIViewマネージャのボタン監視モード開始
   */
  //--------------------------------------------------------------------------
  void StartInputExculusiveKeyMode(void);
  //--------------------------------------------------------------------------
  /**
   * @brief UIViewマネージャのボタン監視モード終了
   * @param flg   true = ON, false = OFF
   */
  //--------------------------------------------------------------------------
  void EndInputExculusiveKeyMode( void );


  
  int m_Seq;       ///InitとEndのシーケンス
  int mFileSeq;
  int m_ModeSeq;
  int mPokemonWaistIndex;
  int mFukidasiUpdate;
  int mBeansBoxAnimNo;
#if PM_DEBUG
  bool mIsDebugCameraMode;
  float mDebugZoom;
  gfl2::math::Vector3 mDebugAddPos;
  float mDebugPokeRotate;
  int debugPokemonSelecetMonsNo;
  int debugPokemonSelecetFormNo;
  float mdebugmulnum;

#endif  // PM_DEBUG

  bool mbSkyboxNoDel;
  bool mbSpeedCheckNg;  //移動量が０で帰ってきた場合
 
  //読み込んだりソース管理
  static const int READ_DATA_NUM = GARC_islandground_DATA_NUM;
  app::util::FileAccessor::RequestData* m_pFrs[READ_DATA_NUM];


  static const int _SHADOW_READBUFF_MAX = 5;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pPokeShadowModelResourceNode[_SHADOW_READBUFF_MAX];

  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pSignBoardModelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pSignBoardAnimResourceNode;


  static const int _BEANSBOX_MAX = 2;  //まめ箱は空とＦＵＬＬの２つ
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansBoxModelResourceNode[_BEANSBOX_MAX];
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansBoxAnimResourceNode;

  static const int _TREASUREBOX_MAX = 2;  //宝箱は空とＦＵＬＬの２つ
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pTreasureBoxModelResourceNode[_TREASUREBOX_MAX];
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pTreasureBoxAnimResourceNode;

  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansTreeModelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansTreeAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_modelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_collisionResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansModelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansTypeAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansInAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansRollAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansBottleModelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansBottleAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pBeansBottlePickupAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pGroundAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pDrawEnvResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pDrawEnvMotionResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pKinomiModelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pKinomiAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pKinomiKiAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pKinomiMiAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_billResourceNode;    //ビルボードのリソース、
  gfl2::renderingengine::scenegraph::resource::ResourceNode* mpFukidashiAnimResourceNode; //ふきだしのアニメリソース
  gfl2::renderingengine::scenegraph::resource::ResourceNode* mpFukidashiResourceNode; //ふきだしのリソース

  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pGroundSeaAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pGroundPartsModelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pGroundPartsAnimResourceNode;

  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pTreasurePokemonInAnimResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pTreasurePokemonOutAnimResourceNode;
  
  void** mpResourceBuffer;   //読み込んだ素材管理バッファ

  TextureResourceNodeGetter mTexGetter;
  
  //使っていないのでコメントアウトgfl2::heap::HeapBase* m_pLocalDeviceHeap;                   // 専用のヒープ、寿命も管理する　
  //使っていないのでコメントアウトSystem::nijiAllocator* m_pLocalDeviceHeapAllocator;      // 専用のアロケーター、寿命も管理する　

  camera_island_binstr* mpstrx;

  
  app::util::Heap* mpAppHeap;   //メモリ
  gfl2::heap::HeapBase* mpPokeModelHeap;
  System::nijiAllocator* mpPokeModelNijiAllocator;

  App::Resort::APP_PARAM* mpAppParam;     //初期化パラメータ
  ResortInitLogic* mpLogic;
  
  poke_3d::model::BaseModel* m_pBaseModel;   //地形ベースモデル
  poke_3d::model::BaseModel* m_pPartsModel;   //地形パーツモデル
  gfl2::collision::CollisionModel* m_pCollisionModel;    //地面コリジョン

  ResortTouchToFlowListener* mpResortTouchToFlowListener;

  static const int BEANS_DATA_NUM = 20;  //まめのかず

  ModelData m_pBeansModel[BEANS_DATA_NUM];  //まめモデル


  ModelData m_pBeansBottleModel;  //まめびんモデル

  PokeTool::SimpleParam mPara[ Savedata::ResortSave::STOCKPOKEMON_MAX + Savedata::ResortSave::RESORT_FRIENDSHIP_PLAYPOKE_MAX ];

  KinomikiModelData m_pKinomiModel[Savedata::ResortSave::RESORT_FARM_MAX];

  ModelData mBeansTreeModel;  //ふれあいにあるふとい木、１つ

  ModelData m_pBeansBoxModel[_BEANSBOX_MAX];  //まめばこ

  ModelData m_pTreasureBoxModel[_TREASUREBOX_MAX];  //たからばこ
  static const int _SHINBOARD_MAX = 3;  //看板表示最大は３
  ModelData m_pSignBoardModel[_SHINBOARD_MAX];  //かんばん

  
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_drawEnvNode;  // カメラを含む環境
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;  // カメラを含む環境
  
  System::nijiAllocator* m_pNijiAllocator;  //レンダリングに必要なアロケータ
  app::util::AppRenderingManager* m_appRenderingManager;    ///APPレンダリングマネージャー
  app::util::AppRenderingManager::Description m_Description;  //APPレンダリングマネージャーの設定
  
  poke_3d::model::BaseCamera*                               m_camera;       // カメラ

  pml::pokepara::CoreParam* mpTempPokeCorePara;
  
//  touch_adv_island_binstr* mpTouchAdvBin;  //位置
 // touch_gim_island_binstr* mpTouchGimBin;  //位置
  walk_hotspa_island_binstr* mpHotSpaBin;  //温泉の移動値
  walk_hotspa_island_binstr* mpHotSpaMoveBin;  //温泉の移動値
  walk_adv_island_binstr* mpAdvMoveBin;  //温泉の移動値
  walk_farm_island_binstr* mpFarmMoveBin;  //温泉の移動値
  walk_friend_island_binstr* mpFriendMoveBin;  //温泉の移動値
  walk_gim_island_binstr* mpGimMoveBin;  //温泉の移動値
  kinomipos_island_binstr* mpKinomiTreeBin;  //木の実の位置
 // touch_kanbox_island_binstr* mpTouchKanbanBoxPosBin;  //看板とボックスの位置
  moveflg_island_binstr* mpPokeMoveFlg;  //ポケモンの移動可否フラグデータ
  standpos_island_binstr* mpStandPosBin;  //動かない立ってる位置

  farm_touch_island_binstr* mpFarmTouch; //きのみリゾートのタッチ関連
  hotspa_touch_island_binstr* mpHotSpaTouch; //きのみリゾートのタッチ関連
  adventure_touch_island_binstr* mpAdventureTouch; //きのみリゾートのタッチ関連
  friendship_touch_island_binstr* mpFriendShipTouch; //きのみリゾートのタッチ関連
  gim_touch_island_binstr* mpGimTouch; //きのみリゾートのタッチ関連
  beans_pos_island_binstr* mpBeansPos; //まめの置く場所
  poke_camera_s_island_binstr* mpPokeCameraSimple; //ポケモンカメラ簡易版個別調整
  poke_camera_i_island_binstr* mpPokeCameraIsland; //ポケモンカメラ島ごと個別調整
//  poke_camera_island_binstr* mpPokeCamera; //ポケモンカメラ個別調整
  
  
  System::EnvAccessor         m_EnvAccessor;    ///< DrawEnv共通処理アクセサー
  
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pPokeLight; // 下画面ポケモンモデル用のライト
  s32 m_resourcePokeLightNo;  // グラフィッカーさんのDrawEnvリソースに含まれるポケモン用のライトの番号
  
  // レイヤー番号
  enum
  {
    MODEL_LAYER_SKY    = 0,  // 天球用レイヤー
    MODEL_LAYER_BG     = 1,  // 背景用レイヤー
    MODEL_LAYER_POKE   = 2,  // ポケモン用レイヤー
  };

  enum
  {
    _FUKIDASI_START=0,
    _FUKIDASI_END,
  };

  //カメラ管理
  f32 BILLBOARD_COMP;
  f32 ISLAND_DIST_INIT;
  f32 ISLAND_HEIGHT_INIT;

  f32 m_IslandDist;  //注視点までの距離
  f32 island_camera_rotate;  //左右というか 回転
  gfl2::math::Vector3 m_TargetPos;    ///注視点保存用
  gfl2::math::Vector3 m_TargetPosBk;    ///注視点保存用
  gfl2::math::Vector3 m_PokeTargetPos;    ///ポケモン注視点保存用
  

  int m_asyncCreateDestroyPokeModelStep;    //ポケモンモデル生成用
  int m_PokeDeleteStep;   ///ポケモンモデル削除用

  gfl2::math::Vector3 mPokemonIconDefScale;  //ポケモンのアイコンデフォルトスケール保存用
  static const int BILL_DATA_NUM = 21;   //ビルボード管理用
  static const int BILL_DATA_NUM_NOT = 18;   //ビルボード管理用
  poke_3d::model::BaseModel* m_pBillboardModel;  //ビルボードのベースモデル
  int m_billPokeState[BILL_DATA_NUM];   //ビルボードの仮アニメのステートアルファ用
  int m_billPokeNo[BILL_DATA_NUM];     //表示しているポケモン番号
  int m_billFukidashiOn[BILL_DATA_NUM];     //吹き出しの時間差ON
  int m_TouchPokeIndex;      //タッチしたポケモンのインデックス番号
  PokeTool::SimpleParam m_TouchSimple;  //タッチしたポケモンのsimpleparam

  poke_3d::model::BaseModel* mpFukidashiModels[BILL_DATA_NUM];  //ふきだしモデル

  
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_CapResourceNode[BILL_DATA_NUM];
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_CapResourceNodeRecv[BILL_DATA_NUM];
  void* mpCapturePoke[BILL_DATA_NUM];
  void* mpCapturePokeRecv[BILL_DATA_NUM];

  // ポケモンアイコン3Dのヒープ
  gfl2::heap::HeapBase* mpPokeIcon3dTexParentHeap;
  gfl2::heap::HeapBase* mpPokeIcon3dTexHeap[BILL_DATA_NUM][2];  // 0=arci, 1=arcirecv
  

  //gfl2::heap::HeapBase *mpManagerHeap;  // 使っていないのでコメントアウト
  

  bool asyncCreatePokeModel(void);

  f32 GetAdjustHeight(void);
  
  // ポケモンモデル

  PokeTool::PokeModel* m_pokeModel;         //ポケモン3Dモデル
  ModelData m_ModelPokeShadow;         //ポケモンかげ
  PokeTool::PokeModelSystem* m_pokeModelSys;   //ポケモン表示システム
  bool m_pokeModelExist;  // ポケモン3Dモデルがアクセスできる状態で存在するときtrue

  bool mbFukidashiDisable;

  gfl2::util::GLHeapAllocator*  m_modelAllocator;
  gfl2::gfx::IGLAllocator* mpOldAllocator;   //セットされているアロケータを記憶するもの

  
  bool mbKeyEnable;
  bool m_WalkEnd;

  bool mbKinomiDeleteState;
  bool mbPokemonRotateChange;
  bool mBackupExclusiveKey;

  int mPokeCoseCount;
  int mPokeCoseIndex;
  int mCountTimer;
  
  gfl2::math::Vector3 pokeIconBackupScale;

private:
  static const f32 START_CAMERA_POSITION;
  static const f32 START_CAMERA_MOVE_VALUE;
  static const f32 START_CAMERA_MOVE_ADD;
  static const u32 START_CAMERA_MOVE_COUNT_MAX;
  f32 mStartCameraFovy;
  f32 mStartCameraMoveValue;
  u8 mStartCameraMoveCount;
  b8 mIsStartCameraMoveEnd;
  b8 mIsEndCameraMove;

  static const s16 UPDATE_CAMERA_ADD_MAX = 120;
  static const s16 UPDATE_CAMERA_VALUE = -1;
  int mUpdateCameraTotal;
  s16 mUpdateCameraAdd;
  s16 mUpdateCameraValue;

public:
  void SetStartCamera(void);
  void StartCameraMove( bool is_fade_start );
  bool IsStartCameraMoveEnd(void);
  void EndCameraMove( bool is_fade_out );
  void UpdateCameraMove(void);

};  //class BattleProc







//=================================================================================================
// コリジョンライブラリ、高さ判定コールバッククラス
//=================================================================================================
class SimpleCollisionCallbackable_Height : public gfl2::collision::ICollisionModelCallbackable
{
public:

  static const u32 HIT_COUNT_MAX = 55;  ///< 衝突情報を格納する最大数  豆とビルボードの数でキャッシュ

  ///< 衝突情報
  struct HIT_DATA
  {
    f32   height;
    const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;
  };

  // コンストラクタ、デストラクタ
  SimpleCollisionCallbackable_Height();
  virtual ~SimpleCollisionCallbackable_Height();

  /**
   * @brief 当たり判定コールバック関数
   */
  virtual bool CollisionModelHeightCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height);

  /**
   * @brief 指定の高さに一番近い高さを取得する
   * @note 指定した高さよりも低いところでしか当たっていない場合は指定した高さそのままを返す
   * @param 比べる高さ
   * @return 衝突ポリゴンの高さ
   */
  f32 GetNearestHeight( f32 height) const ;

  /**
   * @brief 1度でも当たったかを返す
   * @retval true 当たった
   * @retval false 当たっていない
   */
  bool IsHit() const;

  /**
   * @brief 再取得可能な状態にする
   */
  void Clear( void);

private:

  u32       m_hitCount;                         // 当たった数
  HIT_DATA  m_hitDataTable[ HIT_COUNT_MAX ];    // 衝突情報バッファ
};





GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)



#endif  //__RESORT_ISLAND_DRAW_H__
