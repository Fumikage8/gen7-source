//======================================================================
/**
 * @file        btlv_camera.h
 * @brief       バトル描画 カメラ
 * @author      ariizumi
 * @data        15/05/20
 */
//======================================================================
#if !defined( __BTLV_CAMERA_H__ )
#define __BTLV_CAMERA_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <math/include/gfl2_Transporter.h>

GFL_NAMESPACE_BEGIN( gfl2 )
  GFL_NAMESPACE_BEGIN( renderingengine )
    GFL_NAMESPACE_BEGIN( scenegraph )
      GFL_NAMESPACE_BEGIN( instance )
        class DrawEnvNode;
      GFL_NAMESPACE_END( instance )
    GFL_NAMESPACE_END( scenegraph )
  GFL_NAMESPACE_END( renderingengine )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( poke_3d )
  GFL_NAMESPACE_BEGIN( model )
  class BaseCamera;
  GFL_NAMESPACE_END( model )
GFL_NAMESPACE_END( poke_3d )


GFL_NAMESPACE_BEGIN(btl)

class BattleViewSystem;


// -----------------------------------
// 待機カメラデーター用定義
// -----------------------------------
enum BTLV_WAITCAM_TARGET
{
  WAITCAM_TARGET_FIELD,

  WAITCAM_TARGET_POKE,
  WAITCAM_TARGET_TRAINER,

  WAITCAM_TARGET_ENEMY_POKE,
  WAITCAM_TARGET_ENEMY_TRAINER,

  WAITCAM_TARGET_ALLY_POKE,
  WAITCAM_TARGET_ALLY_TRAINER,

  WAITCAM_TARGET_NONE,
  WAITCAM_TARGET_MAX,
};


enum BTLV_WAITCAM_NODE
{
  WAITCAM_NODE_ORIGIN,
  WAITCAM_NODE_CENTER,
  WAITCAM_NODE_FACE,

  WAITCAM_NODE_MAX,
};

enum BTLV_WAITCAM_EFFECT
{
  WAITCAM_EFFECT_NONE,
};

struct WaitCameraData
{
  u32 fileName;
  int level;
  f32 cameraRate[6];
  BTLV_WAITCAM_TARGET start_target;
  BTLV_WAITCAM_NODE start_node;
  BTLV_WAITCAM_TARGET end_target;
  BTLV_WAITCAM_NODE end_node;
  bool checkGround;
  BTLV_WAITCAM_EFFECT effect;
  int forbidNext[10];
  int lottery_weight;
};

enum
{
  CAMERA_RATE_RULE_SINGLE  = 0,    //< シングル
  CAMERA_RATE_RULE_DOUBLE  = 1,    //< ダブル
  CAMERA_RATE_RULE_ROYAL   = 2,    //< バトルロイヤル
  CAMERA_RATE_RULE_INTRUDE = 3,    //< 乱入
  
  CAMERA_RATE_RULE_SINGLE_WILD = 4,    //< シングル野生
  CAMERA_RATE_RULE_DOUBLE_WILD = 5,    //< ダブル野生

  WAITCAM_START_WAIT_FRAME = 90,        // 待機カメラ開始までのフレーム数
  WAITCAM_START_PLAY_DATA_IDX = 60,       // 戦闘で最初の待機カメラ再生で使われるカメラデータ
};
 
enum BTLV_WAITCAM_LEVEL
{
  WAIT_CAMERA_LEVEL_1 = 1,
  WAIT_CAMERA_LEVEL_2 = 2,
};

class BtlvCameraSystem
{
public:
  BtlvCameraSystem( BattleViewSystem *btlvCore );
  ~BtlvCameraSystem();

  void Initialize(void);
  void Terminate(void);
  void Update(void);

  //座標とか操作
  void SetPosition( const gfl2::math::Vector3 &pos , const gfl2::math::Vector3 &trg );
  const gfl2::math::Vector3& GetPosition(void)const{return mPosition;}
  const gfl2::math::Vector3& GetTarget(void)const{return mTarget;}
  void SetFov(f32 fov){mFov = fov;mIsUpdateCamera = true;}
  f32 GetFov(void)const{return mFov;}
  void SetNear(f32 nearVal){mNear = nearVal;mIsUpdateCamera = true;}
  void SetFar(f32 farVal){mFar = farVal;mIsUpdateCamera = true;}
  f32 GetNear(void)const{return mNear;}
  f32 GetFar(void)const{return mFar;}
  void SetOffset( const gfl2::math::Vector3 &ofs );
  const gfl2::math::Vector3& GetOffset(void)const{return mOffset;}
  void SetTwist(f32 twist);
  f32 GetTwist(void)const{return mTwist;}

  //アニメ中の座標も取れる
  const gfl2::math::Vector3& GetPositionAnime(void)const;
  const gfl2::math::Vector3& GetTargetAnime(void)const;
  GFL_WARNING_PUSH  //この文で#pragma設定を退避
  GFL_WARNING_CTR_IGNORE(815)  //#815-Dを抑止
  const f32 GetNearAnime_NMCat4000fix(void)const;
  GFL_WARNING_POP  //この文で#pragma設定を回復

  //デバッグ用
  void SetUpdateCamera(bool flg = true){mIsUpdateCamera = flg;}
  gfl2::math::Vector3* GetCameraPosPointer(void){return &mPosition;}
  gfl2::math::Vector3* GetCameraTrgPointer(void){return &mTarget;}
  f32* GetCameraTwistPointer(void){return &mTwist;}

  void StartCameraAnime( void *envBuf , void *anmBuf , bool isWaitCamera = false );
  void EndCameraAnime(bool keepPos);

  bool IsPlayCameraAnime(void){return mIsPlayAnime;}

  void SetCameraAnimeOffset( gfl2::math::Vector3 &ofs){mAnimeOffset = ofs;}
  void SetCameraAnimeRotateY_Rad( f32 rot){mAnimeRotY = rot;}
  void SetCameraAnimeScale( gfl2::math::Vector3 &scl){mAnimeScale = scl;}
  void SetCameraAnimeFlip(bool flg){mIsFlipAnime = flg;}

  void SetCameraAnimeIsLoop(bool flg);
  void SetCameraAnimeSpeed(f32 speed);

  void StartWaitCamera(void);
  void EndWaitCamera(void);
  bool IsFinishWaitCamera(void);

  //トレーナーライトを取るため
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* GetDrawEnv(void){return mDrawEnvNode;}

  void CheckCameraCollision(gfl2::math::Vector3 *pos,gfl2::math::Vector3 *trg,f32 nearVal);

private:
  void UpdateWaitCamera(void);
  void LodaWaitCameraData(void);
  bool CheckHitWaitCamera(gfl2::math::Vector3 &pos , f32 near);

  // 待機カメラ関連
  u32 SelectNewWaitCamera (void);
  u32 SelectWaitCameraByHeatLevel( int level );

  u32 SelectNewWaitCamera_V2 (void);
  u32 GetTargetablePokemonStatus (void);
  u32 GetTargetableTrainerStatus (void);
  int SelectWaitCameraByStatus( int cameraLevel, u32 targerPokeStat, u32 targetTrainerStat, int beforeCameraIndex );
  int GetCameraLevel(void);
  int CalcHeatLevel(void);
  void CalcHeatLevelFunc(BTL_PARTY* party , int *maxHp, int *hp );

  float GetCameraRate ( const WaitCameraData *waitCamData );
  bool CheckWaitCameraData_ForbidNext ( const WaitCameraData *waitCamData, int beforeWaitcamIdx );
  bool CheckWaitCameraData_Target ( const WaitCameraData *waitCamData, u32 targetPokeStat, u32 targetTrainerStat, BtlRule btl_rule ); 
  u32 MakeEnemyMaskByRule( BtlRule btl_rule );
  u32 MakeAllyMaskByRule( BtlRule btl_rule );

  int GetBitCount( u32 val, int max=4 );
  int GetEnemyModelCount( u32 modelSt, BtlRule btl_rule );
  int GetAllyModelCount( u32 modelSt, BtlRule btl_rule );

  void SelectCameraTargets( int camDataIdx, u32 targetPokeStat, u32 targetTrainerStat );
  int SelectCameraTarget_Pokemon  ( BTLV_WAITCAM_TARGET type, u32 target_status );
  int SelectCameraTarget_Trainer  ( BTLV_WAITCAM_TARGET type, u32 target_status );

  int SelectAvailableTargetIndex ( u32 target_status, u32 mask, int max_num = 4 );

  void SetWaitCameraParameter (void);
  void SetCameraParameter_Pokemon( BtlvPos btlPos, BTLV_WAITCAM_NODE type);
  void SetCameraParameter_Trainer( int trainer_idx, BTLV_WAITCAM_NODE type);
  gfl2::math::Vector3 GetPokemonPosition( BtlvPos btlPos, BTLV_WAITCAM_NODE type );
  gfl2::math::Vector3 GetTrainerPosition( int trainer_idx, BTLV_WAITCAM_NODE type );

  void MakeTransporterInstance (gfl2::math::Vector3 st_pos, gfl2::math::Vector3 ed_pos );

  bool IsAllySide( int btlPos );

  BtlvPos mTargetPoke;
  int mTargetTrainerIdx;
  BtlvPos mTargetPoke_end;
  int mTargetTrainerIdx_end;
  bool mIsFirstWaitCamera;

  int mWaitCamDataIdx;
  bool mCheckGround;
  bool IsWaitCamMoveTarget;
  gfl2::math::TransBase<gfl2::math::Vector3> *mOffsetTransporter;

  BattleViewSystem *mBtlvCore;

  gfl2::renderingengine::scenegraph::resource::ResourceNode *mEnvResNode;
  gfl2::renderingengine::scenegraph::instance::CameraNode   *mCameraNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mAnmResNode;

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode  *mDrawEnvNode;
  poke_3d::model::BaseCamera *mCameraAnime;


  bool  mIsUpdateCamera;
  gfl2::math::Vector3 mPosition;
  gfl2::math::Vector3 mTarget;
  gfl2::math::Vector3 mOffset;
  f32 mFov;
  f32 mNear;
  f32 mFar;
  f32 mTwist;

  bool mIsPlayAnime;
  gfl2::math::Vector3 mAnimeOffset;
  f32 mAnimeRotY;
  gfl2::math::Vector3 mAnimeScale;
  bool mIsFlipAnime;

  //一保持用に使用する、最終カメラアニメの位置
  gfl2::math::Vector3 mAnimePos;
  gfl2::math::Vector3 mAnimeTrg;
  f32 mAnimeFov;
  f32 mAnimeNear;
  f32 mAnimeFar;
  f32 mAnimeTwist;


  bool mIsEnableWaitCmaera;
  bool mWaitCameraEndReq;
  int  mWaitCameraSeq;
  int  mWaitCameraCnt;
  void *mWaitCameraAnimeBuf_Load;
  void *mWaitCameraEnvBuf_Load;
  void *mWaitCameraAnimeBuf_Play;
  void *mWaitCameraEnvBuf_Play;


  gfl2::math::Matrix34 mSndViewMtx;

};
GFL_NAMESPACE_END(btl)


#endif  //__BTLV_CAMERA_H__
