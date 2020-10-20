//======================================================================
/**
 * @file Object_Cluster.h
 * @date 2015/08/15 13:53:57
 * @author ariizumi_nobuhiko
 * @brief オブジェクト 群体制御
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __OBJECT_CLUSTER_H_INCLUDED__
#define __OBJECT_CLUSTER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <DemoLib/Object/include/Object.h>
#include <DemoLib/Object/include/ObjectUtil.h>

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

class ObjectRenderPipeLine;
class ObjectManager;
class Object_Effect;

//群体オブジェクト
class Object_Cluster :public Object
{
public:
  //設定用定義
  //生成タイプ
  enum
  {
    SPAWN_PARENT,       //親(メインのObj
    SPAWN_TIME,         //親から一定間隔で生成
    SPAWN_REFRECT,      //親の反射時に生成
    SPAWN_DEATH,        //親の死亡時に生成
  };
  //エミッター形状
  enum
  {
    EMITTER_POINT,      //点
    EMITTER_SPHERE,     //球
    EMITTER_CYLINDER1,  //円柱(基準は底面
    EMITTER_CYLINDER2,  //円柱(基準は中央
    EMITTER_BOX,        //箱
  };

  //初速種類
  enum
  {
    SPEED_NONE,       //なし
    SPEED_DIFFUSE,    //拡散
    SPEED_DIR,        //指定方向
  };
  //加速種類
  enum
  {
    ACCEL_NONE,       //なし
    ACCEL_GRAVITY,    //重力
  };
  //基準位置定義
  enum
  {
    POS_ORIGIN,     //点
    POS_AXIS_X,     //軸
    POS_AXIS_Y,
    POS_AXIS_Z,
    POS_PLANE_X,    //面
    POS_PLANE_Y,
    POS_PLANE_Z,
  };
  //反射設定
  enum
  {
    REFRECT_NONE,   //なし
    REFRECT_NORMAL, //反射
    REFRECT_STOP,   //停止
    REFRECT_DEATH,  //死亡
  };

  //生成設定(共通部分
  static const int ANIME_PARAM_NUM = 5; //回転・拡縮アニメの個数
  struct SetupParam
  {
    SetupParam()
    {
      gfl2::std::MemClear( this, sizeof(SetupParam) );
      initSclMin.Set(1,1,1);
      initSclMax.Set(1,1,1);
      syncInitScl = false;
      for( int i=0;i<ANIME_PARAM_NUM;i++ )
      {
        animScaleValue[i].Set(1,1,1);
        animRotateValue[i].Set(1,1,1);
      }
    }

    gfl2::heap::HeapBase *heap; //ヒープ
    ObjectManager *objManager;  //Objectマネージャー
    int objectMax;      //最大生成個数

    //生成関係数値
    int spawnType;      //生成タイプ
    int spawnTime;      //生成時間
    int spawnInterval;  //生成感覚
    int spawnNum;       //１回辺りの生成個数
    int spawnRate;      //生成確立(%)

    //Objectのパラメーター
    int childLifeTime;  //寿命

    //エミッター
    int emitterType;    //形状
    int emitterAxis;    //軸
    gfl2::math::Vector3 emitterSize;  //サイズ
    int emitterStartAngle;  //開始角(Deg
    int emitterEndAngle;    //終了角(Deg
    int emitterCavityRate;  //空洞率(0～99%)

    //初速用
    int spdType;  //初速種類
    int spdAxis;  //初速軸(拡散用
    gfl2::math::Vector3 spdVec; //方向(拡散用・方向用
    int spdDecRand; //ランダム角度(方向用
    f32 spdSpd;   //速度
    f32 spdAcc;   //加速

    //加速設定用
    int accelType;  //加速種類
    int accelAxis;  //軸
    gfl2::math::Vector3 accelPos;     //加速対称点
    gfl2::math::Vector3 accelRotOfs;  //
    bool accelIsAbsolute;
    f32 accelRate;    

    //回転・拡大初期パラメーター
    gfl2::math::Vector3 initRotMin;
    gfl2::math::Vector3 initRotMax;
    gfl2::math::Vector3 initSclMin;
    gfl2::math::Vector3 initSclMax;
    bool syncInitScl;

    //Objectアニメパラメーター
    int animScaleFrame[ANIME_PARAM_NUM];
    gfl2::math::Vector3 animScaleValue[ANIME_PARAM_NUM];
    int animRotateFrame[ANIME_PARAM_NUM];
    gfl2::math::Vector3 animRotateValue[ANIME_PARAM_NUM];

    //子の設定
    bool isFollowParent;  //親に追従する
  };
  //反射設定
  struct RefrectParam
  {
    RefrectParam()
    {
      type = REFRECT_NONE;
    }
    f32   pos;    //反射基準位置
    bool  isAbs;  //絶対・相対
    int   axis;   //軸
    int   type;   //反射挙動
    f32   brake;  //減衰率
  };  

  //Object生成のパラメーター(Model)
  struct ModelParam
  {
    //ヒープとアロケーター
    gfl2::heap::HeapBase *heap;
    gfl2::gfx::IGLAllocator *allocator;

    //描画パイプライン
    ObjectRenderPipeLine *pipe;
    int drawOpt; //AddRednderPath用Opt
    int rotOrder;

    void* modelBuf; //モデルのリソース  
    void* anmBuf;   //アニメのリソース

    ModelParam()
      :rotOrder(gfl2::math::EulerRotation::RotationOrderZYX)
    {

    }
  };

  //Object生成のパラメーター(Effect)
  typedef struct
  {
    //ヒープとアロケーター
    gfl2::heap::HeapBase *heap;
    gfl2::Effect::GFHeap *effHeap;
    gfl2::Effect::System *effSystem; 

    int effResIdx;

    int drawOpt; //AddRednderPath用Opt
    int rotOrder;
  }EffectParam;

  //生成されたObject管理
  typedef struct
  {
    bool isEnable;
    Object *obj;  //オブジェ(モデルかパーティクル)

    Object_GfMdl  *objMdl;  //どっちかだけ入る
    Object_Effect *objEff;  

    Object *parent;  //親のオブジェ
    bool isCreateChild; //子を作ったか？

    int cnt;  //カウンタ

    gfl2::math::Vector3 dir;  //方向
    f32 spd;                  //速度
    f32 accelSpd;             //加速度

    //生成時にランダムで設定されるスケールと回転。
    //後で設定されるスケールにかけなきゃいけないので持って置く
    //SangoでAdjustに入れてたけど、子に継承されるので無し。
    gfl2::math::Vector3 randScale;  //ランダムスケール
    gfl2::math::Vector3 randRotate; //ランダム回転
  }SpawnObjectData;

  //内部用定義
  typedef enum
  {
    OBJECT_TYPE_MODEL,
    OBJECT_TYPE_EFFECT,
  }OBJECT_TYPE;

  //コンストラクタ
  Object_Cluster();
  //デストラクタ
  ~Object_Cluster();
  
  //モデルを使って生成
  void CreateModel(const SetupParam &param , const ModelParam &mdlParam);
  //エフェクトを使って生成
  void CreateEffect(const SetupParam &param , const EffectParam &mdlParam);

  //破棄
  bool Delete(void);

  //設定パラメータ取得
  SetupParam* GetSetupParam(void){return &mSetupParam;}
  //反射パラメータ取得
  RefrectParam* GetRefrectParam(void){return &mRefrectParam;}

  //親子の設定
  void SetParent(Object_Cluster *obj);
  void SetChild(Object_Cluster *obj);
  //生成タイプの取得(参照が多いので関数化)
  int GetSpawnType(void)const{return mSetupParam.spawnType;}

protected:
  //IsEnableはチェック済み。
  //毎フレーム呼ばれる
  virtual void UpdateAlways(int step=1);
  //IsUpdateが立っていたら来る関数。
  //IsEnableはチェック済みなので座標更新はこちらで。
  virtual void UpdateCore(int step=1);

private:
  //共通部分の生成
  void CreateCommon(const SetupParam &param);

  //Objectの作成
  void SpawnObject(Object *parent);

  //時間生成のチェック(親と子の判定があるので、関数化)
  void CheckSpawn_Time(int cnt,Object *parent);

  //生成処理
  void SpawnCore(SpawnObjectData *spawnObj,Object *parent);
  //モデルObjの生成
  void SpawnCore_Model(SpawnObjectData *spawnObj);
  //エフェクトObjの生成
  void SpawnCore_Effect(SpawnObjectData *spawnObj);
  
  //初期位置
  void SpawnCore_SetStartPosition(SpawnObjectData *spawnObj);
  //初期速度
  void SpawnCore_SetStartSpeed(SpawnObjectData *spawnObj);
  //初期設定その他(回転・拡縮)
  void SpawnCore_SetStartParam(SpawnObjectData *spawnObj);
  
  //オブジェクト更新処理
  void UpdateObject(SpawnObjectData *spawnObj);
  //オブジェクト更新処理（反射処理
  void UpdateObject_Refrect(SpawnObjectData *spawnObj , gfl2::math::Vector3 &befPos );
  //オブジェクト更新処理(回転・拡縮アニメ計算)
  bool ValueAnimeFunc( int *frameArr , gfl2::math::Vector3 *vecArr , int cnt, gfl2::math::Vector3 *ret );

  //オブジェクト破棄処理
  void DeleteObject(SpawnObjectData *spawnObj);
  //オブジェクト破棄処理(モデル
  void DeleteObject_Model(SpawnObjectData *spawnObj);
  //オブジェクト破棄処理(エフェクト
  void DeleteObject_Effect(SpawnObjectData *spawnObj);

  //追従の親が死ぬ時呼ばれて、親をはずす
  void CheckFollowChild(Object *obj);

  //各種設定パラメータ
  SetupParam mSetupParam;
  ModelParam mModelParam;
  EffectParam mEffectParam;
  RefrectParam mRefrectParam;

  OBJECT_TYPE mObjType; //オブジェクト種類
  int mSpawnCnt;  //生成用カウンタ
  bool mIsCallDelete; //破棄が開始されたか？

  SpawnObjectData *mSpawnObjArr;  //小オブジェクト

  //親と子
  Object_Cluster *mParentCluster;
  static const int CHILD_CLUSTER_NUM = 5;
  Object_Cluster *mChildClusterArr[CHILD_CLUSTER_NUM];
};

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)

#endif // __OBJECT_CLUSTER_H_INCLUDED__