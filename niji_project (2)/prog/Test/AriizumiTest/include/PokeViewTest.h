//======================================================================
/**
 * @file  PokeViewTest.cpp
 * @brief 15/04/20
 * @author  ariizumi
 * @data  描画テスト
 */
//======================================================================

#ifndef __POKE_VIEW_TEST_H__
#define __POKE_VIEW_TEST_H__
#pragma once
#if PM_DEBUG

#include <Math/include/gfl2_Matrix.h>
#include <Math/include/gfl2_Quaternion.h>
#include <macro/include/gfl2_Macros.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <Effect/include/gfl2_EffectSystem.h>
#include <Effect/include/gfl2_EffectRenderPath.h>

#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"

#include "System/include/nijiAllocator.h"
#include "System/include/RenderPath/EdgeMapRenderPath.h"
#include "System/include/RenderPath/OutLinePostRenderPath.h"
#include "System/include/RenderPath/ModelRenderPath.h"
#include "System/include/RenderPath/FullPowerEffect.h"

#include "PokeTool/include/PokeModelSystem.h"

#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
#define USE_POKE 1
#else
#define USE_POKE 0
#endif

GFL_NAMESPACE_BEGIN(btl)
  class BgSystem;
GFL_NAMESPACE_END(btl)

GFL_NAMESPACE_BEGIN( gfl2 )
  GFL_NAMESPACE_BEGIN( renderingengine )
    GFL_NAMESPACE_BEGIN( renderer )
      GFL_NAMESPACE_BEGIN( util )
      class StretchBltFrameBufferPath;
      GFL_NAMESPACE_END( util )
    GFL_NAMESPACE_END( renderer )
  GFL_NAMESPACE_END( renderingengine )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( poke_3d )
  GFL_NAMESPACE_BEGIN( renderer )
  class DistortionPostRenderPath;
  GFL_NAMESPACE_END( renderer )
GFL_NAMESPACE_END( poke_3d )


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)

#if USE_AURA_SHADER

//===================================================================================================================================================
// 
//  サンプルcameraクラス
//
//  y軸ビルボード行列をview行列から取り出すのがうまくいかなかったので
//  とりあえず動作確認用に用意した。いずれ本番環境に置き換える
// 
//===================================================================================================================================================
class SampleCamera
{
//---------------------------------------------------------------------------------------------------------------------------------------------------
public:
//---------------------------------------------------------------------------------------------------------------------------------------------------
// コンストラクタ
	SampleCamera()
  {
    Initialize( gfl2::math::Vector3( 0,0,0 ), gfl2::math::Vector3( 0,0,0 ), 256.0f );
  }

//-----------------------------------------------
// デストラクタ
  ~SampleCamera()
  {
  }

//-----------------------------------------------
//  初期設定
  void Initialize( const gfl2::math::Vector3& yow_pitch_roll, const gfl2::math::Vector3& target, const f32 distance )
  {
    m_YPR.x = yow_pitch_roll.x;
    m_YPR.y = yow_pitch_roll.y;
    m_YPR.z = yow_pitch_roll.z;

    m_target.x = target.x;
    m_target.y = target.y;
    m_target.z = target.z;

    m_distance = distance;

    updateVector(); // 初期更新
  }

//-----------------------------------------------
//  ビューマトリクス計算
  void GetViewMtx( gfl2::math::Matrix34* pMtx )
  {
	  nn::math::VEC3 posEye = -m_distance * m_vecEye + m_target;

    nn::math::MTX34 mtx;
    nn::math::MTX34LookAt( &mtx, &posEye, &m_vecUp, &m_target );

    nn::math::VEC4 r0 = mtx.GetRow(0);
    nn::math::VEC4 r1 = mtx.GetRow(1);
    nn::math::VEC4 r2 = mtx.GetRow(2);

    gfl2::math::Vector4 r0_1 = gfl2::math::Vector4( r0.x, r0.y, r0.z, r0.w );
    gfl2::math::Vector4 r1_1 = gfl2::math::Vector4( r1.x, r1.y, r1.z, r1.w );
    gfl2::math::Vector4 r2_1 = gfl2::math::Vector4( r2.x, r2.y, r2.z, r2.w );

    gfl2::math::Matrix34 outMtx;
    outMtx = gfl2::math::Matrix34( r0_1, r1_1, r2_1 );

    *pMtx = outMtx;
  }

//-----------------------------------------------
//  Y軸ビルボード用マトリクス計算
  void GetYBillboardMtx( gfl2::math::Matrix34* pMtx )
  {
	  nn::math::VEC3 axisY = nn::math::VEC3( 0,1,0 );

    nn::math::MTX34 mtx;
    nn::math::MTX34RotAxisRad( &mtx, &axisY, -m_YPR.y );
 
    nn::math::VEC4 r0 = mtx.GetRow(0);
    nn::math::VEC4 r1 = mtx.GetRow(1);
    nn::math::VEC4 r2 = mtx.GetRow(2);

    gfl2::math::Vector4 r0_1 = gfl2::math::Vector4( r0.x, r0.y, r0.z, r0.w );
    gfl2::math::Vector4 r1_1 = gfl2::math::Vector4( r1.x, r1.y, r1.z, r1.w );
    gfl2::math::Vector4 r2_1 = gfl2::math::Vector4( r2.x, r2.y, r2.z, r2.w );

    gfl2::math::Matrix34 outMtx;
    outMtx = gfl2::math::Matrix34( r0_1, r1_1, r2_1 );

    *pMtx = outMtx;
  }

//-----------------------------------------------
//  カメラコントロール(回転)
  void ControlRotate( f32 rotateH, f32 rotateV )
  {
    m_YPR.x += rotateV;
    m_YPR.y += rotateH;

    // 裏回り防止リミッター
    if( m_YPR.x < -1.5f ) { m_YPR.x = -1.5f; }
	  if( m_YPR.x > 1.5f )  { m_YPR.x = 1.5f; }

    updateVector(); // ベクトル更新
  }

//-----------------------------------------------
//  カメラコントロール(ズーム)
  void ControlZoom( f32 zoom )
  {
    m_distance -= zoom;

    // 距離制限リミッター
    if( m_distance > 8192.0f ){ m_distance = 8192.0f; }
    if( m_distance < 32.0f )  { m_distance = 32.0f; }
  }

//-----------------------------------------------
//  カメラコントロール(移動)
  void ControlMove( f32 moveH, f32 moveV )
  {
	  nn::math::VEC3 vecH, vecV;

    nn::math::VEC3Cross( &vecH, &m_vecEye, &m_vecUp );  // 水平移動ベクトル計算
    nn::math::VEC3Cross( &vecV, &m_vecEye, &vecH );     // 垂直移動ベクトル計算

    nn::math::VEC3Normalize( &vecH, &vecH );
    nn::math::VEC3Normalize( &vecV, &vecV );

    nn::math::VEC3 offsetH = vecH * moveH; 
    nn::math::VEC3 offsetV = vecV * moveV; 

	  m_target = m_target + offsetH + offsetV;
  }

//---------------------------------------------------------------------------------------------------------------------------------------------------
private:
//---------------------------------------------------------------------------------------------------------------------------------------------------
  nn::math::VEC3 m_YPR;
  nn::math::VEC3 m_target;
  f32            m_distance;
  nn::math::VEC3 m_vecEye;
  nn::math::VEC3 m_vecUp;

//-----------------------------------------------
// 各種ベクトル演算
  void updateVector( void )
  {
    f32 sinYaw, cosYaw, sinPitch, cosPitch, sinRoll,cosRoll;

    nn::math::SinCosRad( &sinYaw, &cosYaw, m_YPR.y );
    nn::math::SinCosRad( &sinPitch, &cosPitch, m_YPR.x );
    nn::math::SinCosRad( &sinRoll, &cosRoll, m_YPR.z );

	  nn::math::VEC3 vec;

    // 視線ベクトル計算
    vec = nn::math::VEC3( sinYaw * cosPitch, sinPitch, -cosYaw * cosPitch );
    nn::math::VEC3Normalize( &m_vecEye, vec );

	  // カメラの上方ベクトル計算
    vec = nn::math::VEC3( cosYaw * sinRoll, cosRoll, sinYaw * sinRoll );
    nn::math::VEC3Normalize( &m_vecUp, vec );
  }
};
#endif //USE_AURA_SHADER

class PokeViewTest : public GameSys::GameProc ,public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeViewTest); //コピーコンストラクタ＋代入禁止

public:
  PokeViewTest(); //コンストラクタ
  virtual ~PokeViewTest(void);  //デストラクタ

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

  //============================================================================
  //RenderingPipeLine
  //============================================================================
	//----------------------------------------------------------------------------
	/**
	*  @brief  最初のパスから描画できる状態にする
	*/
	//-----------------------------------------------------------------------------
	virtual b32 StartRenderPath(void);

	//----------------------------------------------------------------------------
	/**
	*  @brief  現状のパスにあったシーンを返す
	*  @return シーン
	*/
	//-----------------------------------------------------------------------------
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath(void);

	//----------------------------------------------------------------------------
	/**
	*  @brief  次のパスへ遷移
	*  @return trueなら次のパス、falseなら描画終了
	*/
	//-----------------------------------------------------------------------------
	virtual b32 NextRenderPath(void);
//private:
public: //デバッグでメンドイので外に出す

  void UpdateCamera(void);
#if USE_AURA_SHADER
  void UpdateCameraSample(void);
#endif //USE_AURA_SHADER
  void SetCameraPosTarget(const gfl2::math::Vector3 &pos,const gfl2::math::Vector3 &trg);
  void UpdateCameraMatrix(void);

  void InitDebug(void);
  void TermDebug(void);

  void ResetDebugParam(void);

  void AddFullPowerPath(void);
  void RemoveFullPowerPath(void);
  void DeleteFullPowerPath(void);
  bool mReserveReloadFullPower;
  int mReserveReloadFullPowerCnt;

  int mInitSeq;
  int mEndSeq;
  int mMainSeq;

  int mMonsNo;
  int mFormNo;
  int mSex;
  int mRandUp;
  int mRandDown;
  bool mRare;
  int mAnimeType;
  int mPokeIdx;
  int mMotionIdx;
  int mEyeIdx;
  int mMouthIdx;
  bool mDispIdModel;
  bool mLoadShadow;
  bool mLoadIdModel;

  //自動テスト用
  bool mActiveAutoTest;
  int mWaitCnt;
  int mStartMonsNo;
  int mEndMonsNo;
  int mLoadStartHeap;
  u32 mLoadStartCmdBuf;
  bool mAutoEnableFullPower;
  bool mCheckFPEnableMaterial;
  int mPrintMode;

  gfl2::math::Vector4 mShaderCol;
  gfl2::math::Vector4 mEnvCol;

  gfl2::heap::HeapBase *mHeap;
  gfl2::heap::HeapBase *mFPHeap;
  System::nijiAllocator *mAllocator;

  int mRenderPathCnt;
  gfl2::math::Matrix44 mCamProjMtx;   // 透視変換行列
  gfl2::math::Matrix34 mCamViewMtx;   // ビュー変換行列
  gfl2::math::Matrix44 mProjMtx[2];   // 透視変換行列
  gfl2::math::Matrix34 mViewMtx[2];   // ビュー変換行列

  f32 mCamRotVertical;
  f32 mCamRotHorizonal;
  gfl2::math::Vector3 mCamPos;


  static const int POKE_NUM = 2;
  PokeTool::PokeModelSystem *mPokeModelSys;
  PokeTool::PokeModel *mPokeModel[POKE_NUM];
  bool mIsCreateModel[POKE_NUM];

  void *mEdgeMapRenderPathRes;
  void *mOutLineRenderPathRes;
  void *mFullPowerRenderPathRes;
  System::NijiEdgeMapSceneRenderPath  *mEdgeMapPath;
  System::NijiOutLinePostSceneRenderPath  *mOutLinePath;
  System::ModelRenderPath *mModelRenderPath;
  System::ModelRenderPath *mIdModelRenderPath;
  //ゆがみシェーダー用
  gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath *mStretchBltFrameBufferPath;
  poke_3d::renderer::DistortionPostRenderPath                *mDistortionPostRenderPath;

  int mDrawCheckTagNum;
  int mDrawCheckUseBuffer;

#if USE_AURA_SHADER
  SampleCamera mSampleCamera;
  FullPowerEffectRenderPath0	*mFullPowerEffectRenderPath0;
  FullPowerEffectRenderPath1	*mFullPowerEffectRenderPath1;
  int mFPEnableMatArrIdx;
  int mFPEnableMatNo[disableMaterialInfo::DISABLE_MATERIAL_MAX];
#endif //USE_AURA_SHADER

  bool mDispAura;
  bool mDispRim;
  f32  mRimColorR;
  f32  mRimColorG;
  f32  mRimColorB;
  f32  mRimPow;
  f32  mRimWidth;
  u32  mRimMoveSpeed;
  f32  mRimMoveWidth;
  
  f32 mAuraColorR;
  f32 mAuraColorG;
  f32 mAuraColorB;
  f32 mAuraScaleX;
  f32 mAuraScaleY;
  u32 mAuraDensity;
  f32 mAuraBaseScaleX;
  f32 mAuraBaseScaleY;

  bool mIsLook;
  gfl2::math::Vector3 mLookPos;

};  //class BattleProc
GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)



#endif  //PM_DEBUG
#endif  //__POKE_VIEW_TEST_H__