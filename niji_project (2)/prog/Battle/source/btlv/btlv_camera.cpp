//======================================================================
/**
 * @file	btlv_camera.cpp
 * @brief	バトル描画 カメラ
 * @author	ariizumi
 * @data	15/05/20
 */
//======================================================================
#undef DEBUG_ONLY_FOR_kijima_satoshi    // 個人デバッグ表示OFF

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryEnvData.h>
#include <model/include/gfl2_BaseCamera.h>

#include "sound/include/Sound.h"
#include "System/include/GflUse.h"

#include "btlv_core.h"
#include "btlv_ui.h"
#include "btlv_camera.h"
#include "btlv_seq_com_define.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/wazaeff_resource.gaix>

#include "niji_conv_header/battle/wait_camera.cdat"  // Import wait camera data


GFL_NAMESPACE_BEGIN(btl)

BtlvCameraSystem::BtlvCameraSystem( BattleViewSystem *btlvCore )
:mBtlvCore(btlvCore)
{
}

BtlvCameraSystem::~BtlvCameraSystem()
{
}

void BtlvCameraSystem::Initialize(void)
{
  mAnimeOffset.Set(0,0,0);
  mAnimeRotY = 0;
  mAnimeScale.Set(1,1,1);
  mIsFlipAnime = false;
  mIsPlayAnime = false;
  mCheckGround = false;

  mTargetPoke = BTL_VPOS_ERROR;
  mTargetTrainerIdx = -1;
  mTargetPoke_end = BTL_VPOS_ERROR;
  mTargetTrainerIdx_end  = -1;

  mWaitCamDataIdx = -1;

  IsWaitCamMoveTarget = false;
  mOffsetTransporter = NULL;

  mIsFirstWaitCamera = true;
}

void BtlvCameraSystem::Terminate(void)
{
  //通信ネラー時に残る場合がある。
  GflHeapSafeFreeMemory( mWaitCameraAnimeBuf_Play );
  GflHeapSafeFreeMemory( mWaitCameraEnvBuf_Play );
  GflHeapSafeFreeMemory( mWaitCameraAnimeBuf_Load );
  GflHeapSafeFreeMemory( mWaitCameraEnvBuf_Load );
  GFL_SAFE_DELETE( mOffsetTransporter );
}

void BtlvCameraSystem::Update(void)
{
  UpdateWaitCamera();

  gfl2::math::Matrix34 sndViewMtx;

  if( mIsPlayAnime )
  {
    mCameraAnime->UpdateAnimation();
    f32 tempFov = mCameraNode->GetFovy();
    f32 aspect = mCameraNode->GetAspectRatio();
    f32 anmFov  = gfl2::math::ConvRadToDeg(tempFov);
    f32 anmNear = mCameraNode->GetNear();
    f32 anmFar = mCameraNode->GetFar();
    //mAnimeFar = 12000.0f;
#if 1
    gfl2::math::Vector3 eye = mCameraNode->GetLocalTranslate();
    gfl2::math::Quaternion rotQ = mCameraNode->GetLocalRotation();
    gfl2::math::EulerRotation rot(rotQ);

    // こっちが元のビュー行列
    rotQ = rot.ToQuaternion();
    gfl2::math::Matrix34 viewMatrix;
    {
      // LookAtに分解してみる
      gfl2::math::Vector3 front = rotQ.Transform(gfl2::math::Vector3(0, 0, 1));
      gfl2::math::Vector3 up = rotQ.Transform(gfl2::math::Vector3(0, 1, 0));

      gfl2::math::Vector3 aim =  eye - (front * eye.Length());

      if( mIsFlipAnime )
      {
        eye.x *= -1;
        aim.x *= -1;
        up.x *= -1;
      }
      /*
      // アップベクトルをtwistに変換１
      //参考のためにソースを残しておく
      gfl2::math::Vector3 axis(0, 0, 1);
      if( axis != front )
      {
        axis = front.Cross(gfl2::math::Vector3(0, 0, 1));
        axis = axis.Normalize();
        f32 dot = front.Dot(gfl2::math::Vector3(0, 0, 1));
        f32 radAngle = gfl2::math::AcosRad(dot);
      
        gfl2::math::Quaternion quat(axis, radAngle);
        up = quat.Transform(up);
      }

      //反転処理諸々

      // アップベクトルをtwistに変換２
      f32 twist = ::atan2f(up.x, up.y);      //反転
      if( mIsFlipAnime )
      {
        //座標
        //ひねり
        twist *= -1;
      }

      // twistからアップベクトルを復元
      gfl2::math::Vector3 up3(gfl2::math::SinRad(twist), gfl2::math::CosRad(twist), 0);
      {
        gfl2::math::Vector3 front = eye - aim;
        front = front.Normalize();
        gfl2::math::Vector3 axis(0, 0, 1);
        if( axis != front )
        {
          axis = front.Cross(gfl2::math::Vector3(0, 0, 1));
          axis = axis.Normalize();
          f32 dot = front.Dot(gfl2::math::Vector3(0, 0, 1));
          f32 radAngle = gfl2::math::AcosRad(dot);
        
          gfl2::math::Quaternion quat(axis, radAngle);
          gfl2::math::Quaternion conj = quat.GetConj();

          up3 = conj.Transform(up3);
        }
      }
      */
      //回転
      BtlvRot_Rad(eye.x,eye.z,mAnimeRotY);
      BtlvRot_Rad(aim.x,aim.z,mAnimeRotY);

      //これも回転させないと向きがおかしくなる
      BtlvRot_Rad(up.x,up.z,mAnimeRotY);

      //スケーリング
      eye = eye.MulPerElem(mAnimeScale);
      aim = aim.MulPerElem(mAnimeScale);

      //オフセット追加
      eye += mAnimeOffset+mOffset;
      aim += mAnimeOffset+mOffset;

      // カメラの地面チェック
      static const f32 HEIGHT_LIMIT = 16.0f;
      if( mCheckGround )
	{
	  // Camera position
	  if( eye.y <= HEIGHT_LIMIT )
	    {
	      eye.y = HEIGHT_LIMIT;
	    }
	  // Aim point
	  if( aim.y <= HEIGHT_LIMIT )
	    {
	      aim.y = HEIGHT_LIMIT;
	    }
	}
      
      //ARIIZUMI_PRINT("Camera[%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n",mCameraAnime->GetAnimationFrame(),eye.x,eye.y,eye.z,aim.x,aim.y,aim.z,up.x,up.y,up.z);

      if(mIsEnableWaitCmaera == true)
      {
        CheckCameraCollision(&eye, &aim,anmNear);
      }

      const bool isHitWaitCamera = CheckHitWaitCamera(eye,anmNear);
      if( isHitWaitCamera == true )
      {
        //ぶつかったので座標更新させずに抜ける
        GFL_PRINT("WaitCamera Hit Target!![%d]\n",mWaitCamDataIdx);
        EndCameraAnime(true);
        mWaitCameraCnt = 0;
        return;
      }

      viewMatrix = gfl2::math::Matrix34::GetLookAt(eye, aim, up);
      //一保持用に使用する、最終カメラアニメの位置
      mAnimePos = eye;
      mAnimeTrg = aim;
      // twistに変換

      {
        gfl2::math::Vector3 tempUp;
        gfl2::math::Vector3 axis(0, 0, 1);
        gfl2::math::Vector3 front2 = eye - aim;
        front2 = front2.Normalize();
        if( axis != front2 )
        {
          axis = front2.Cross(gfl2::math::Vector3(0, 0, 1));
          axis = axis.Normalize();
          f32 dot = front2.Dot(gfl2::math::Vector3(0, 0, 1));
          f32 radAngle = gfl2::math::AcosRad(dot);
      
          gfl2::math::Quaternion quat(axis, radAngle);
          tempUp = quat.Transform(up);
        }
        mAnimeTwist = ::atan2f(tempUp.x, tempUp.y);
        //GFL_PRINT("Anime1[%f][%f:%f:%f:]\n",mAnimeTwist,tempUp.x,tempUp.y,tempUp.z);
      }
#if 0     
      //一旦twistに変換するテスト
      {
        gfl2::math::Vector3 axis(0, 0, 1);
        gfl2::math::Vector3 front2 = eye - aim;
        front2 = front2.Normalize();
        if( axis != front2 )
        {
          axis = front2.Cross(gfl2::math::Vector3(0, 0, 1));
          axis = axis.Normalize();
          f32 dot = front2.Dot(gfl2::math::Vector3(0, 0, 1));
          f32 radAngle = gfl2::math::AcosRad(dot);
      
          gfl2::math::Quaternion quat(axis, radAngle);
          up = quat.Transform(up);
        }
        // twistからアップベクトルを復元
        f32 twist = ::atan2f(up.x, up.y);
        gfl2::math::Vector3 up3(gfl2::math::SinRad(twist), gfl2::math::CosRad(twist), 0);
        if( axis != front2 )
        {
          axis = front2.Cross(gfl2::math::Vector3(0, 0, 1));
          axis = axis.Normalize();
          f32 dot = front2.Dot(gfl2::math::Vector3(0, 0, 1));
          f32 radAngle = gfl2::math::AcosRad(dot);
        
          gfl2::math::Quaternion quat(axis, radAngle);
          gfl2::math::Quaternion conj = quat.GetConj();

          up3 = conj.Transform(up3);
        }
        gfl2::math::Matrix34 viewMatrix2 = gfl2::math::Matrix34::GetLookAt(eye, aim, up3);
        GFL_PRINT("Anime2[%f][%f:%f:%f:]\n",twist,up3.x,up3.y,up3.z);
        // 値の比較
        for (u32 i = 0; i < 3; ++i)
        {
          for (u32 j = 0; j < 4; ++j)
          {
            GFL_PRINT("[%f][%f]_",viewMatrix[i][j],viewMatrix2[i][j]);
          }
          GFL_PRINT("\n");
        }
        for (u32 i = 0; i < 3; ++i)
        {
          for (u32 j = 0; j < 4; ++j)
          {
            GFL_ASSERT(gfl2::math::FAbs(viewMatrix[i][j] - viewMatrix2[i][j]) < 0.001f);
          }
        }
      }
      //一旦twistに変換するテスト ココマデ
#endif      
      mAnimeNear = anmNear;
      mAnimeFar = anmFar;
      mAnimeFov = anmFov;
      mBtlvCore->GetRenderSys()->SetCamProjMtx(tempFov,aspect,mAnimeNear,mAnimeFar);
      mBtlvCore->GetRenderSys()->SetCamViewMtx(viewMatrix);
      mSndViewMtx = viewMatrix;
    }
#endif

  }
  else
  {
    if( mIsUpdateCamera )
    {
	    // ビュー変換行列
	    gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

      //一旦twistに変換するテスト
      gfl2::math::Vector3 eye = mPosition+mOffset;
      gfl2::math::Vector3 aim = mTarget+mOffset;
      if( eye.x == aim.x && eye.z == aim.z )
      {
        aim.z += 1.0f;
      }
      if( !(-FLT_MAX <= (mTwist) && (mTwist) <= FLT_MAX) )
      {
        //nanチェック(NTR_SDKのNN_FLOAT_TASSERT_式から拝借)
        //カメラアニメーションの終了値がNANの場合にありえる。
        //厳密にはfront2が(0,0,-1)の場合に発生しうるが、その場合Twistが0以外である必要がある。
        //Twistが0意外なのはアニメーション終了の位置からの補完中のみで、そのケースはチェック済み
        mTwist = 0;
      }
      if( mTwist != 0 )
      {
        gfl2::math::Vector3 axis(0, 0, 1);
        gfl2::math::Vector3 front2 = eye - aim;
        front2 = front2.Normalize();
        upDirection.Set(gfl2::math::SinRad(mTwist), gfl2::math::CosRad(mTwist), 0);
        if( axis != front2 )
        {
          axis = front2.Cross(gfl2::math::Vector3(0, 0, 1));
          axis = axis.Normalize();
          f32 dot = front2.Dot(gfl2::math::Vector3(0, 0, 1));
          f32 radAngle = gfl2::math::AcosRad(dot);
        
          gfl2::math::Quaternion quat(axis, radAngle);
          gfl2::math::Quaternion conj = quat.GetConj();

          upDirection = conj.Transform(upDirection);
        }
        //GFL_PRINT("Camera[%f][%f:%f:%f:]\n",mTwist,upDirection.x,upDirection.y,upDirection.z);
      }

      gfl2::math::Matrix34 viewMtx = gfl2::math::Matrix34::GetLookAt(eye,aim, upDirection);
      mSndViewMtx = viewMtx;

      mBtlvCore->GetRenderSys()->SetCamViewMtx(viewMtx);
      mBtlvCore->GetRenderSys()->GetRenderingPipeLine()->SetBillboardViewMatrix(viewMtx);

      {
        u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
        u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
	      // 射影変換行列(パースペクティブ(透視法)射影)
	      mBtlvCore->GetRenderSys()->SetCamProjMtx(
			      gfl2::math::ConvDegToRad(mFov),		// 視野角30°
            (f32)displayWidth / (f32)displayHeight,
			      mNear,							// 前方投影面までの距離
			      mFar);							// 後方投影面までの距離
      }
    }
  }

  //3Dサウンド対応
  {
    gfl2::math::MTX34 mtx34;
    mtx34._00 = mSndViewMtx[0].x;
    mtx34._01 = mSndViewMtx[0].y;
    mtx34._02 = mSndViewMtx[0].z;
    mtx34._03 = mSndViewMtx[0].w;
    mtx34._10 = mSndViewMtx[1].x;
    mtx34._11 = mSndViewMtx[1].y;
    mtx34._12 = mSndViewMtx[1].z;
    mtx34._13 = mSndViewMtx[1].w;
    mtx34._20 = mSndViewMtx[2].x;
    mtx34._21 = mSndViewMtx[2].y;
    mtx34._22 = mSndViewMtx[2].z;
    mtx34._23 = mSndViewMtx[2].w;
    Sound::SetListenerMatrix( mtx34 );
  }

}

void BtlvCameraSystem::SetPosition( const gfl2::math::Vector3 &pos , const gfl2::math::Vector3 &trg )
{
  mPosition = pos;
  mTarget = trg;
  mIsUpdateCamera = true;
}

void BtlvCameraSystem::SetOffset( const gfl2::math::Vector3 &ofs )
{
  mOffset = ofs;
  mIsUpdateCamera = true;
}

void BtlvCameraSystem::SetTwist(f32 twist)
{
  mTwist = twist;
  mIsUpdateCamera = true;
}


//アニメ中の座標も取れる
const gfl2::math::Vector3& BtlvCameraSystem::GetPositionAnime(void)const
{
  if( mIsPlayAnime )
  {
    return mAnimePos;
  }
  return GetPosition();
}

const gfl2::math::Vector3& BtlvCameraSystem::GetTargetAnime(void)const
{
  if( mIsPlayAnime )
  {
    return mAnimeTrg;
  }
  return GetTarget();
}

GFL_WARNING_PUSH  //この文で#pragma設定を退避
GFL_WARNING_CTR_IGNORE(815)  //#815-Dを抑止
//カメラアニメ再生中はNearの値が変わってしまう。
//そのためアニメ中はアニメの方の値をとる必要があるがただ置き換えると、
//技にも影響が出てしまうため影響範囲を待機カメラ中に収めるために専用関数で取る
const f32 BtlvCameraSystem::GetNearAnime_NMCat4000fix(void)const
{
  if( mIsPlayAnime && mIsEnableWaitCmaera )
  {
    return mAnimeNear;
  }
  return GetNear();
}
GFL_WARNING_POP  //この文で#pragma設定を回復

void BtlvCameraSystem::StartCameraAnime( void *envBuf , void *anmBuf , bool isWaitCamera )
{
  gfl2::heap::HeapBase *heap;
  gfl2::gfx::IGLAllocator* allocator;
  if( isWaitCamera )
  {
    heap = mBtlvCore->GetWaitCameraHeap();
    allocator = mBtlvCore->GetWaitCameraGLAllocator();
  }
  else
  {
    heap = mBtlvCore->GetTempHeap();
    allocator = mBtlvCore->GetTempGLAllocator();
  }
  
  EndCameraAnime(false);
  mBtlvCore->ResetHitBack();

  //カメラ処理用に専用のEnvNodeをつくって
  gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData envData;
  envData.SetBinaryEnvData( static_cast<c8*>(envBuf) );
  mEnvResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator,&envData );
  mDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(allocator,mEnvResNode);
  mCameraNode = mDrawEnvNode->GetCamera(0);

  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmResData;
  anmResData.SetMotionData(static_cast<c8*>(anmBuf));
  mAnmResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator, &anmResData );		

  // カメラ
  mCameraAnime = GFL_NEW(heap)poke_3d::model::BaseCamera();
  mCameraAnime->Create(allocator,heap,mDrawEnvNode,0);
  mCameraAnime->ChangeAnimationGlobalByResourceNode(mAnmResNode);

  mAnimeOffset.Set(0,0,0);
  mAnimeRotY = 0;
  mAnimeScale.Set(1,1,1);
  mIsFlipAnime = false;
  mCheckGround = false;

  mIsPlayAnime = true;
}

void BtlvCameraSystem::EndCameraAnime(bool keepPos)
{
  if( keepPos )
  {
    mPosition = mAnimePos;
    mTarget = mAnimeTrg;
    mOffset.Set(0,0,0);
    mFar = mAnimeFar;
    mNear = mAnimeNear;
    mFov = mAnimeFov;
    mTwist = mAnimeTwist;
    mIsUpdateCamera = true;
  }

  mIsPlayAnime = false;
  if( mCameraAnime )
  {
    mCameraAnime->Destroy();
  }
  GFL_SAFE_DELETE(mCameraAnime);
  GFL_SAFE_DELETE(mAnmResNode);
  GFL_SAFE_DELETE(mDrawEnvNode);
  GFL_SAFE_DELETE(mEnvResNode);
  mCameraNode = NULL; //これはDrawEnvにくっついているからおｋ
}

void BtlvCameraSystem::SetCameraAnimeIsLoop(bool flg)
{
  mCameraAnime->SetAnimationLoop(flg);
}

void BtlvCameraSystem::SetCameraAnimeSpeed(f32 speed)
{
  mCameraAnime->SetAnimationStepFrame(speed);
}

//カメラコリジョンチェック
void BtlvCameraSystem::CheckCameraCollision(gfl2::math::Vector3 *pos,gfl2::math::Vector3 *trg,f32 nearVal)
{
  //背景へのめり込み回避のための限界値チェック
  f32 chekcHeight = 800.0f;
  f32 checkRadius = 940.0f;

  //高さ
  if( pos->y > chekcHeight )
  {
    const f32 rate = chekcHeight/pos->y;
    pos->y *= rate;
    trg->y *= rate;
    //ARIIZUMI_PRINT("Cam Hit Height!!\n");
  }

  if( pos->y < nearVal )
  {
    trg->y += -pos->y+nearVal;
    pos->y = nearVal;
    //ARIIZUMI_PRINT("Cam Hit Ground!!\n");
  }

  //横位置
  {
    f32 lenCam = (pos->x*pos->x)+(pos->y*pos->y);
    f32 lenRad = checkRadius*checkRadius;
    if( lenCam > lenRad )
    {
      lenCam = gfl2::math::FSqrt(lenCam);
      const f32 rate = checkRadius/lenCam;
      pos->x *= rate;
      pos->z *= rate;
      trg->x *= rate;
      trg->z *= rate;
      ARIIZUMI_PRINT("Cam Hit Radius!![%f]\n",rate);
    }
  }  
}

// ------------------------------------------------------------
// 待機カメラ関連
// ------------------------------------------------------------

void BtlvCameraSystem::StartWaitCamera(void)
{
  mIsEnableWaitCmaera = true;
  mWaitCameraEndReq = false;
  mWaitCameraSeq = 0;

  mWaitCameraCnt = WAITCAM_START_WAIT_FRAME;     // 待機カメラ開始までのフレーム数

#if PM_DEBUG
  // フレーム数指定デバッグ機能
  if( mBtlvCore->mDebugWaitCamStartWaitFrame != -1 )
    {
      KIJIMA_PRINT ("[WC]* Wait camera debug Set wait frame : %d\n", mBtlvCore->mDebugWaitCamStartWaitFrame);
      mWaitCameraCnt = mBtlvCore->mDebugWaitCamStartWaitFrame;
    }
#endif

}

void BtlvCameraSystem::EndWaitCamera(void)
{
  mWaitCameraEndReq = true;
}

bool BtlvCameraSystem::IsFinishWaitCamera(void)
{
  return !mIsEnableWaitCmaera;
}

void BtlvCameraSystem::UpdateWaitCamera(void)
{
  enum
  {
    WAITCAM_SEQ_WAIT, //@attention 待機カメラ中にヒットチェックでこの値を外で設定するので、ずらすな！
    WAITCAM_SEQ_LOAD,
    //WAITCAM_SEQ_PLAY,

    WAITCAM_SEQ_END,
  };
#if PM_DEBUG
  if( mBtlvCore->mIsEnableWaitCamera == false )
    {
      mWaitCameraEndReq = true;
    }
#endif
  if( mIsEnableWaitCmaera )
    {
      switch( mWaitCameraSeq )
        {
        case WAITCAM_SEQ_WAIT:
          if( mWaitCameraEndReq )
            {
              mWaitCameraSeq = WAITCAM_SEQ_END;
            }
          else
            {
              // 座標補完
              if( ( IsWaitCamMoveTarget ) && (mOffsetTransporter != NULL) )
                {
                  gfl2::math::Vector3 pos =  mOffsetTransporter->Update();
                  SetCameraAnimeOffset( pos );
                  // KIJIMA_PRINT ("[WC]>> Move (%f,%f,%f)\n", pos.x,pos.y, pos.z );
                }

              mWaitCameraCnt--;
              if( mWaitCameraCnt < 0 )
                {
                  LodaWaitCameraData();
                  mWaitCameraSeq++;
                }
            }
          break;
        case WAITCAM_SEQ_LOAD:
          if( mBtlvCore->GetAsyncFileManager()->IsArcFileLoadDataFinished(&mWaitCameraAnimeBuf_Load) &&
              mBtlvCore->GetAsyncFileManager()->IsArcFileLoadDataFinished(&mWaitCameraEnvBuf_Load) )
            {
              //終了リクエストは読み込み終わりまで待つ
              if( mWaitCameraEndReq )
                {
                  mWaitCameraSeq = WAITCAM_SEQ_END;
                }
              else
                {
                  StartCameraAnime( mWaitCameraEnvBuf_Load , mWaitCameraAnimeBuf_Load , true );
              
                  GFL_SAFE_DELETE( mOffsetTransporter );    // 座標補完用インスタンス破棄

                  //前回のアニメのクリア
                  GflHeapSafeFreeMemory( mWaitCameraAnimeBuf_Play );
                  GflHeapSafeFreeMemory( mWaitCameraEnvBuf_Play );

                  mWaitCameraAnimeBuf_Play = mWaitCameraAnimeBuf_Load;
                  mWaitCameraEnvBuf_Play = mWaitCameraEnvBuf_Load;
                  mWaitCameraAnimeBuf_Load = NULL;
                  mWaitCameraEnvBuf_Load = NULL;


                  // 再生フレーム数のセット
                  mWaitCameraCnt = mCameraAnime->GetFrameCount();
                  KIJIMA_PRINT ("[WC]> Wait camera total frame : %d\n", mWaitCameraCnt );

                  SetWaitCameraParameter ();
                  SetCameraAnimeIsLoop(false);	// 待機カメラは基本的にループ無し
              
#if PM_DEBUG
                  // 待機カメラアニメ反転デバッグ機能
                  if( mBtlvCore->mDebugWaitCamReverse == 1 )
                    {
                      SetCameraAnimeFlip(true);
                      KIJIMA_PRINT ("[WC]* Wait camera debug anime X-FLIP.\n");
                    }
              
                  if( mBtlvCore->mDebugWaitCamReverse == 0 )
                    {
                      SetCameraAnimeFlip(false);
                      KIJIMA_PRINT ("[WC]* Wait camera debug anime NORMAL.\n");
                    }
#endif
                  //ポケモン待機Bの許可
                  mBtlvCore->SetAllEnableWaitB();

                  mWaitCameraSeq = WAITCAM_SEQ_WAIT;
                }
            }
          break;
        case WAITCAM_SEQ_END:
          if( IsPlayCameraAnime() )
          {
            EndCameraAnime(false);
            //ポケモン待機Bの終了
            mBtlvCore->ResetAllEnableWaitB();
          }
          mBtlvCore->ResetDefaultCamera(0);
          GflHeapSafeFreeMemory( mWaitCameraAnimeBuf_Play );
          GflHeapSafeFreeMemory( mWaitCameraEnvBuf_Play );
          GflHeapSafeFreeMemory( mWaitCameraAnimeBuf_Load );
          GflHeapSafeFreeMemory( mWaitCameraEnvBuf_Load );

          GFL_SAFE_DELETE( mOffsetTransporter );

          mIsEnableWaitCmaera = false;
          break;
        }
    }
}

//Waitカメラがキャラにヒットしたかのチェック
bool BtlvCameraSystem::CheckHitWaitCamera(gfl2::math::Vector3 &pos , f32 nearVal)
{
  bool isHit = false;
  if( mIsEnableWaitCmaera &&
      IsPlayCameraAnime() )
  {
    if( mTargetPoke != BTL_VPOS_ERROR )
    {
      BtlvObject_PokeModel *poke = mBtlvCore->GetPokeModel(mTargetPoke);
      isHit |= poke->CheckHitCamera(pos,nearVal);
    }
    if( mTargetPoke_end != BTL_VPOS_ERROR )
    {
      BtlvObject_PokeModel *poke = mBtlvCore->GetPokeModel(mTargetPoke_end);
      isHit |= poke->CheckHitCamera(pos,nearVal);
    }
    if( mTargetTrainerIdx != -1 )
    {
      BtlvObject_Chara *mdl = mBtlvCore->GetTrainerModelObj(mTargetTrainerIdx);
      isHit |= mdl->CheckHitCamera(pos,nearVal);
    }
    if( mTargetTrainerIdx_end != -1 )
    {
      BtlvObject_Chara *mdl = mBtlvCore->GetTrainerModelObj(mTargetTrainerIdx_end);
      isHit |= mdl->CheckHitCamera(pos,nearVal);
    }
  }
  return isHit;
}

// ------------------------------------------------------------
/**
 * @brief 待機カメラのデータロード
 * 
 */
// ------------------------------------------------------------
void BtlvCameraSystem::LodaWaitCameraData(void)
{

  u32 anmId,envId,waitCamDataIdx;

  // 待機カメラの抽選
  //  waitCamDataIdx = SelectNewWaitCamera();
  waitCamDataIdx = SelectNewWaitCamera_V2();

  anmId = BTLV_WAITCAM_DATA[waitCamDataIdx].fileName;
  envId = GARC_wazaeff_resource_waitcamera_DrawEnv_lz_GFBENV;

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
  loadReq.arcId = ARCID_WAZAEFF_RESOURCE;
  loadReq.align = 128;
  loadReq.heapForBuf = mBtlvCore->GetWaitCameraHeap();
  loadReq.heapForReq = mBtlvCore->GetWaitCameraHeap()->GetLowerHandle();
  loadReq.heapForCompressed = mBtlvCore->GetWaitCameraHeap()->GetLowerHandle();
  loadReq.ppBuf = &mWaitCameraAnimeBuf_Load;
  loadReq.datId = anmId;
  mBtlvCore->GetAsyncFileManager()->AddArcFileLoadDataReq(loadReq);

  loadReq.ppBuf = &mWaitCameraEnvBuf_Load;
  loadReq.datId = envId;
  mBtlvCore->GetAsyncFileManager()->AddArcFileLoadDataReq(loadReq);
}

//-----------------------------------------------------------------------------
/**
 * @brief 新しい待機カメラ抽選
 *
 * @detail カメラ対象モデルの抽選がカメラデーターが確定した後の必要があるため
 */
//-----------------------------------------------------------------------------

u32 BtlvCameraSystem::SelectNewWaitCamera_V2 (void)
{
  KIJIMA_PRINT ("\n[WC]--------------------------------------------------------\n");

  int cameraLevel = GetCameraLevel ();
  
  // カメラ対象になれるモデルのステータス：下から4bitが0123に対応
  u32 targetPokeStatus = GetTargetablePokemonStatus();
  u32 targetTrainerStatus = GetTargetableTrainerStatus ();

  //momijiでトレーナーを出やすくするため、トレーナーが出るまで３回抽選する
  int tryNum = 1;
  if (mBtlvCore->IsBattleFesMode())
  {
    tryNum = 3;
  }
  int cameraDataIdx = 0;
  while (tryNum>0)
  {
    // 各種ステータスからカメラを抽選
    cameraDataIdx = SelectWaitCameraByStatus(cameraLevel, targetPokeStatus, targetTrainerStatus, mWaitCamDataIdx);
    if (BTLV_WAITCAM_DATA[cameraDataIdx].start_target == WAITCAM_TARGET_TRAINER ||
      BTLV_WAITCAM_DATA[cameraDataIdx].start_target == WAITCAM_TARGET_ALLY_TRAINER)
    {
      //終了(通常時は１回なのでこっちにきても来なくても１回で終わる
      tryNum = 0;
    }
    else
    {
      tryNum--;
    }
  }
  // カメラターゲットの抽選
  SelectCameraTargets( cameraDataIdx, targetPokeStatus, targetTrainerStatus );

#ifdef DEBUG_ONLY_FOR_kijima_satoshi
  KIJIMA_PRINT ("[WC]>>>>> Selected wait camera index : %d \n", cameraDataIdx);
#else
  GFL_PRINT ("[WC]>>>>> Selected wait camera index No. [%d]\n", cameraDataIdx);
#endif

  mWaitCamDataIdx = cameraDataIdx;

  return( cameraDataIdx );
}


// カメラターゲットを抽選
void BtlvCameraSystem::SelectCameraTargets( int camDataIdx, u32 targetPokeStat, u32 targetTrainerStat )
{

  mTargetPoke = BTL_VPOS_ERROR;
  mTargetTrainerIdx = -1;    
  mTargetPoke_end = BTL_VPOS_ERROR;
  mTargetTrainerIdx_end  = -1;

  IsWaitCamMoveTarget = false;

  BTLV_WAITCAM_TARGET target_st  = BTLV_WAITCAM_DATA[camDataIdx].start_target;
  BTLV_WAITCAM_TARGET target_end = BTLV_WAITCAM_DATA[camDataIdx].end_target;

  u32 mask_selected_poke = 0;
  u32 mask_selected_trainer = 0;

  // スタートターゲット
  switch( target_st )
    {
    case WAITCAM_TARGET_POKE:
    case WAITCAM_TARGET_ENEMY_POKE:
    case WAITCAM_TARGET_ALLY_POKE:
      {
        int st = SelectCameraTarget_Pokemon  ( target_st, targetPokeStat );
        if( st != -1 )
          {
            mTargetPoke = static_cast<BtlvPos>(st);

#if PM_DEBUG
            if( mBtlvCore->mDebugWaitCamTargetPokemon != -1 )
              {
                mTargetPoke = static_cast<BtlvPos>( mBtlvCore->mDebugWaitCamTargetPokemon );
                KIJIMA_PRINT( "[WC]* Taget Pokemon debug set : [%d]\n", mTargetPoke );
              }
#endif
            mask_selected_poke = (1<<st);
          }
      }
      break;


    case WAITCAM_TARGET_TRAINER:
    case WAITCAM_TARGET_ENEMY_TRAINER:
    case WAITCAM_TARGET_ALLY_TRAINER:
      {
        int st = SelectCameraTarget_Trainer  ( target_st, targetTrainerStat );
        mTargetTrainerIdx = st;

#if PM_DEBUG
        if( mBtlvCore->mDebugWaitCamTargetTrainer != -1 )
          {
            mTargetTrainerIdx = mBtlvCore->mDebugWaitCamTargetTrainer;
            KIJIMA_PRINT( "[WC]* Taget Trainer debug set : [%d]\n", mTargetTrainerIdx );
          }
#endif

        if( st != -1 ) // @fix cov_ctr[11728] SelectCameraTarget_Trainerが-1を返却する対策
        {
          mask_selected_trainer = (1<<st);
        }
      }
      break;

    default:
      /* NO-OP */
      break;
    }


  // 選択済みのものを排除
  targetPokeStat = (targetPokeStat & (~mask_selected_poke));
  targetTrainerStat = (targetTrainerStat & (~mask_selected_trainer));


  // エンドターゲット
  switch( target_end )
    {
    case WAITCAM_TARGET_POKE:
    case WAITCAM_TARGET_ENEMY_POKE:
    case WAITCAM_TARGET_ALLY_POKE:
      {
        int st = SelectCameraTarget_Pokemon  ( target_st, targetPokeStat );
        if( st != -1 )
          {
            mTargetPoke_end = static_cast<BtlvPos>(st);
            //            mask_selected_poke = (1<<st);

#if PM_DEBUG
            if( mBtlvCore->mDebugWaitCamTargetPokemon_end != -1 )
              {
                mTargetPoke_end = static_cast<BtlvPos>( mBtlvCore->mDebugWaitCamTargetPokemon_end );
                KIJIMA_PRINT( "[WC]* Taget Pokemon(end) debug set : [%d]\n", mTargetPoke_end );
              }
#endif
            IsWaitCamMoveTarget = true;
          }
      }
      break;


    case WAITCAM_TARGET_TRAINER:
    case WAITCAM_TARGET_ENEMY_TRAINER:
    case WAITCAM_TARGET_ALLY_TRAINER:
      {
        int st = SelectCameraTarget_Trainer  ( target_st, targetTrainerStat );
        mTargetTrainerIdx_end = st;
        //        mask_selected_trainer = (1<<st);

#if PM_DEBUG
        if( mBtlvCore->mDebugWaitCamTargetTrainer_end != -1 )
          {
            mTargetTrainerIdx_end = mBtlvCore->mDebugWaitCamTargetTrainer_end;
            KIJIMA_PRINT( "[WC]* Taget Trainer debug set : [%d]\n", mTargetTrainerIdx_end );
          }
#endif

        IsWaitCamMoveTarget = true;
      }
      break;

    default:
      /* NO-OP */
      break;
    }

  
  KIJIMA_PRINT( "[WC] --> Target Status : poke[%d][%d]- trainer[%d][%d]\n",
                mTargetPoke, mTargetPoke_end,
                mTargetTrainerIdx, mTargetTrainerIdx_end );

}

// カメラターゲットのポケモン抽選
// * @param[in]   target_status   : カメラ対象になれるモデルのステータス（ビットフィールド:下から4bitが0123に対応）
int BtlvCameraSystem::SelectCameraTarget_Pokemon  ( BTLV_WAITCAM_TARGET type, u32 target_status )
{
  BtlRule btl_rule = mBtlvCore->GetBattleRule();
  int select_idx = -1;

  switch( type )
    {
    case WAITCAM_TARGET_POKE:
      {
        u32 all_mask = (1<<0) + (1<<1) + (1<<2) + (1<<3);
        select_idx = SelectAvailableTargetIndex( target_status, all_mask, POKE_MODEL_NUM );
      }
      break;

    case WAITCAM_TARGET_ENEMY_POKE:
      {
        const u32 e_mask = MakeEnemyMaskByRule (btl_rule);
        select_idx = SelectAvailableTargetIndex ( target_status, e_mask, POKE_MODEL_NUM);
      }
      break;

    case WAITCAM_TARGET_ALLY_POKE:
      {
        const u32 a_mask = MakeAllyMaskByRule (btl_rule);
        select_idx = SelectAvailableTargetIndex ( target_status, a_mask, POKE_MODEL_NUM);
      }
      break;

    default:
      /* NO-OP */
      break;
    }

  return( select_idx );

}

// カメラターゲットのトレーナー抽選
// * @param[in]   target_status   : カメラ対象になれるモデルのステータス（ビットフィールド:下から4bitが0123に対応）
int BtlvCameraSystem::SelectCameraTarget_Trainer  ( BTLV_WAITCAM_TARGET type, u32 target_status )
{

  BtlRule btl_rule = mBtlvCore->GetBattleRule();
  int select_idx = -1;

  // スタートターゲット
  switch( type )
    {
    case WAITCAM_TARGET_TRAINER:
      {
        u32 all_mask = (1<<0) + (1<<1) + (1<<2) + (1<<3);
        if (mBtlvCore->IsBattleFesMode())
        {
          //フェスの控えは対象外にする
          all_mask &= (1 << 0) + (1 << 1);
        }
        select_idx = SelectAvailableTargetIndex ( target_status, all_mask, TRAINER_MODEL_NUM);
      }
      break;

    case WAITCAM_TARGET_ENEMY_TRAINER:
      {
        u32 e_mask = MakeEnemyMaskByRule (btl_rule);
        if (mBtlvCore->IsBattleFesMode())
        {
          //フェスの控えは対象外にする
          e_mask &= (1 << 0) + (1 << 1);
        }
        select_idx = SelectAvailableTargetIndex(target_status, e_mask, TRAINER_MODEL_NUM);
      }
      break;

    case WAITCAM_TARGET_ALLY_TRAINER:
      {
        u32 a_mask = MakeAllyMaskByRule (btl_rule);
        if (mBtlvCore->IsBattleFesMode())
        {
          //フェスの控えは対象外にする
          a_mask &= (1 << 0) + (1 << 1);
        }
        select_idx = SelectAvailableTargetIndex(target_status, a_mask, TRAINER_MODEL_NUM);
      }
      break;

    default:
      /* NO-OP */
      break;
    }

  return( select_idx );

}


//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 有効なターゲットの中からランダムで抽選したもののインデックスを返す
 *
 * @param[in]   target_status   : カメラ対象になれるモデルのステータス（ビットフィールド:下から4bitが0123に対応）
 * @param[in]   mask            : 有効になる位置条件。ビットの立っているところが有効。
 * @param[in]   max_num         : 対象の最大数（ループする回数）
 */
//---------------------------------------------------------------------------------------------------------------

int BtlvCameraSystem::SelectAvailableTargetIndex ( u32 target_status, u32 mask, int max_num )
{
  u32 available_target = target_status & mask;
  int available_num = 0;
  int available_idx[POKE_MODEL_NUM];

  GFL_ASSERT (max_num <= POKE_MODEL_NUM);
  
  for (int i=0; i<max_num; i++)
    {
      if (((1<<i) & available_target) != 0)
	{
	  available_idx[available_num] = i;
	  available_num++;
	}
    }

  if (1 == available_num)
    {
      // ひとつしかない
      return (available_idx[0]);
    }

  if (0 == available_num)
    {
      // 有効なターゲットがいない
      return -1;
    }

  int ri = System::GflUse::GetPublicRand(available_num);

  return (available_idx[ri]);

}

// ------------------------------------------------------------
/**
 * @brief 各種条件から待機カメラを抽選
 *
 * @return 選ばれた待機カメラデーターのインデックス
 */
// ------------------------------------------------------------
u32 BtlvCameraSystem::SelectNewWaitCamera (void)
{

  int loopCnt = 0;
  
  //この時点でポケモン抽選をしておく
  mTargetPoke = BTL_VPOS_ERROR;
  
  const int modelNum = POKE_MODEL_NUM;  // MAXポケモンモデル数 ("btlv_types.h"で定義)
  const int startIdx = System::GflUse::GetPublicRand( modelNum );
  int tempIdx = startIdx;

  for( int i=0;i<modelNum;i++ )
    {
      BtlvPos tempPos = static_cast<BtlvPos>(tempIdx);

      // モデルが生成されていない or（技などで）見えていない場合は対象外
      BtlvObject_PokeModel*poke = mBtlvCore->GetPokeModel(tempPos);
      if( poke->IsEnable() && poke->GetVisible() )
        {
          mTargetPoke = tempPos;
          break;
        }
      else
        {
          KIJIMA_PRINT ("[WC]>Disabled pokemon:BtlvPos [%d]", tempIdx );
          tempIdx = (tempIdx+1)%modelNum;
        }
    }

#if PM_DEBUG
  if( mBtlvCore->mDebugWaitCamTargetPokemon != -1 )
    {
      mTargetPoke = static_cast<BtlvPos>( mBtlvCore->mDebugWaitCamTargetPokemon );
      KIJIMA_PRINT( "[WC]* Taget Pokemon debug set : [%d]\n", mTargetPoke );
    }
#endif

  // トレーナーの抽選
  {
    const int trainerMax = TRAINER_MODEL_NUM;  // MAXトレーナーモデル数 ("btlv_types.h"で定義)
    int trainerIdx = System::GflUse::GetPublicRand( trainerMax );
    mTargetTrainerIdx = -1;    

    
    BtlvObject_Chara *t_mdl;
    
    for( int i=0;i<trainerMax;i++ )
      {
        t_mdl = mBtlvCore->GetTrainerModelObj( trainerIdx );
        if( t_mdl->IsEnable() && t_mdl->GetVisible() )
          {
            mTargetTrainerIdx = trainerIdx; 
            break;
          }
        else
          {
            KIJIMA_PRINT ("[WC]>Disabled Trainer[%d]", trainerIdx );
            trainerIdx = (trainerIdx+1)%trainerMax;
          }
      }
    
  }
#if PM_DEBUG
  if( mBtlvCore->mDebugWaitCamTargetTrainer != -1 )
    {
      mTargetTrainerIdx = mBtlvCore->mDebugWaitCamTargetTrainer;
      KIJIMA_PRINT( "[WC]* Taget Trainer debug set : [%d]\n", mTargetTrainerIdx );
    }
#endif
 
  int heatLevel = CalcHeatLevel ();
  bool isLoop = true;
  int dataIdx = mWaitCamDataIdx;
  while( isLoop )
    {
      dataIdx = SelectWaitCameraByHeatLevel(heatLevel);
      
      //ポケモンいない時に、ポケモンターゲットは再試行にカウントしない。
      //フィールド対象が無い場合は無いとする。
      if( mTargetPoke == BTL_VPOS_ERROR &&
          BTLV_WAITCAM_DATA[dataIdx].start_target == WAITCAM_TARGET_POKE )
        {
          continue;
        }
   
      loopCnt++;
      if( loopCnt > 30 )
        {
          KIJIMA_PRINT(">>>! 30回カメラの抽選ではまった。\n");
          break;
        }

      // バトルモードにおけるルール一致のチェック
      if( GetCameraRate ( &(BTLV_WAITCAM_DATA[dataIdx]) ) == 0.0f )
        {
          KIJIMA_PRINT("このバトルモードでは使えない : [%d]\n",dataIdx);
          continue;
        }
      KIJIMA_PRINT(">Battle rule:[%d]", mBtlvCore->GetBattleRule());


      // Sky rule : は今回無し

      // 次に来てはいけないカメラのチェック
      if( mWaitCamDataIdx != -1 )
        {
          bool isForbid = false;
          for( int i=0;i<10;i++ )
            {
              if( BTLV_WAITCAM_DATA[mWaitCamDataIdx].forbidNext[i] == dataIdx )
                {
                  KIJIMA_PRINT(">!Hit Forbid Next [%d]",dataIdx);
                  isForbid = true;
                }
            }
          
          if( isForbid )
            {
              continue;
            }
        }
      
      // ここまでチェックにひっかからなければループを抜ける
      isLoop = false;
    }
  
  
  mWaitCamDataIdx = dataIdx;

  KIJIMA_PRINT ("\n--------------------------------------------------------\n");
  KIJIMA_PRINT ("\n>>>>>= Selected wait camera index:%d (loop count : %d)\n", dataIdx, loopCnt);
  return dataIdx;

}

// ------------------------------------------------------------
/**
 * @brief 各種情報からカメラを抽選
 *
 * @param[in] level : ヒートレベル
 *
 * @return 選ばれた待機カメラデーターのインデックス
 */
// ------------------------------------------------------------
int BtlvCameraSystem::SelectWaitCameraByStatus( int cameralevel, u32 targerPokeStat, u32 targetTrainerStat, int beforeCameraIndex )
{

#if PM_DEBUG
  // カメラデータインデックス指定デバッグ機能
  if( mBtlvCore->mDebugWaitCamDataIdx != -1 )
    {
      KIJIMA_PRINT( "[WC]* Wait camera data debug set : [%d]\n", mBtlvCore->mDebugWaitCamDataIdx );
      return mBtlvCore->mDebugWaitCamDataIdx;
    }
#endif

  // その戦闘で最初の待機カメラ再生の場合の特殊処理
  if( mIsFirstWaitCamera )
    {
      mIsFirstWaitCamera = false;
      int cam_idx = WAITCAM_START_PLAY_DATA_IDX;

#if PM_DEBUG
      // カメラデータインデックス指定デバッグ機能
      if( mBtlvCore->mDebugWaitCamFirstPlayDataIdx != -1 )
        {
          KIJIMA_PRINT( "[WC]* First camera data debug set : [%d]\n", mBtlvCore->mDebugWaitCamFirstPlayDataIdx );
          cam_idx = mBtlvCore->mDebugWaitCamFirstPlayDataIdx;
        }
#endif
      
      KIJIMA_PRINT("[WC]>!!First wait camera play : %d\n", cam_idx );
      return( cam_idx );
    }

  const int *indexTbl;
  int indexNum = -1;

  int goodIdxList[WAIT_CAMERA_DATA_NUM];	// オーバーランしないように最大サイズ
  int goodIdxNum = 0;
  int totalWeight = 0;  // 抽選用重み


  // 対象テーブルを選ぶ
  switch( cameralevel )
    {
    case WAIT_CAMERA_LEVEL_1:
    default:
      indexTbl = Level1IndexTable;
      indexNum = WAIT_CAMERA_LV1_DATA_NUM;
      break;

    case WAIT_CAMERA_LEVEL_2:
      indexTbl = Level2IndexTable;
      indexNum = WAIT_CAMERA_LV2_DATA_NUM;
      break;
    }


  // 対象テーブルから条件をクリアするデータをリスティング

  BtlRule btl_rule = mBtlvCore->GetBattleRule(); // Battle rule

  for (int i=0; i<indexNum; i++)
    {
      int idx = indexTbl[i];

      // Check Battle mode
      if( GetCameraRate( &(BTLV_WAITCAM_DATA[idx]) ) == 0.0f )
        {
          continue;
        }
 
      // Check Target (Poekmon & Trainer)
      if( CheckWaitCameraData_Target ( &(BTLV_WAITCAM_DATA[idx]), targerPokeStat, targetTrainerStat, btl_rule) )
        {
          continue;
        }
      
      // Check Forbit Next
      // これはループがあるから最後？
      if( CheckWaitCameraData_ForbidNext ( &(BTLV_WAITCAM_DATA[idx]), beforeCameraIndex ) )
        {
          //KIJIMA_PRINT (">!ForbidNext index : %d\n", idx);
          continue;
        }
      
      // All OK
      // KIJIMA_PRINT (" (Check pass : index = [%d])", idx);
      goodIdxList[goodIdxNum] = idx;
      goodIdxNum++;

      totalWeight += BTLV_WAITCAM_DATA[idx].lottery_weight;
    }

  KIJIMA_PRINT ("[WC]>Check pass camera data total : %d\n", goodIdxNum);

  if( 0 == goodIdxNum )
    {
      // おそらく来ないが念のため
      KIJIMA_PRINT ("[WC]>!!!!!!!!!!! Available wait camera is NOTHING !!!!\n");
      return 0;	// ０番のカメラ
    }

  // 乱数で選ぶ
//    int ri = System::GflUse::GetPublicRand(goodIdxNum);
//    int selectedIdx = goodIdxList[ri];

  // 抽選用重みを考慮した乱数抽選
  int rw = System::GflUse::GetPublicRand( totalWeight );
  int selectedIdx = 0;

  for(int i=0; i<goodIdxNum; i++)
    {
      int gc_idx = goodIdxList[i];
      int w = BTLV_WAITCAM_DATA[gc_idx].lottery_weight;
      rw -= w;
 
      if( rw < 0 )
        {
          selectedIdx = gc_idx;
          break;
        }
    }

  return ( selectedIdx );

}

//-------------------------------------------------------------------------------
/**
 * @brief 待機カメラデーター内の現在のバトルモードに対応するカメラ用スケールの取得
 *
 * @param[in]  waitCamData        : 待機カメラデーター
 *
 * @return カメラ用スケール。選択不可の場合0.0fが返る
 */
//-------------------------------------------------------------------------------
float BtlvCameraSystem::GetCameraRate ( const WaitCameraData *waitCamData )
{

  BtlCompetitor opponent =  mBtlvCore->GetBattleCompetitor();
  BtlRule rule = mBtlvCore->GetBattleRule();
  float ret = 0.0f;

  // 野生じゃない場合
  if( opponent != BTL_COMPETITOR_WILD ) 
    {
      ret = waitCamData->cameraRate[rule];
#if PM_DEBUG
  // カメラデータインデックス指定デバッグ機能が指定されている場合
  if( mBtlvCore->mDebugWaitCamDataIdx != -1 )
    {
      if( ret == 0.0f )
        {
          ret = 1.0f;
          KIJIMA_PRINT( "[WC]* Wait camera debug set camera rate : [%f]\n", ret );
        }
    }
#endif
      return ret;
    }


  // 野生の場合

  switch( rule )
    {
    case BTL_RULE_SINGLE:    ///< シングル
      {
        ret = waitCamData->cameraRate[CAMERA_RATE_RULE_SINGLE_WILD];
      }
      break;
      
    case BTL_RULE_DOUBLE:    ///< ダブル
      {
        ret = waitCamData->cameraRate[CAMERA_RATE_RULE_DOUBLE_WILD];
      }
      break;
      
    default:
    case BTL_RULE_ROYAL:     ///< バトルロイヤル
    case BTL_RULE_INTRUDE:   ///< 乱入( 1vs2　野生シングル戦の途中から派生する )
      {
        ret = waitCamData->cameraRate[rule];
      }
      break;
      
    }

#if PM_DEBUG
  // カメラデータインデックス指定デバッグ機能が指定されている場合
  if( mBtlvCore->mDebugWaitCamDataIdx != -1 )
    {
      if( ret == 0.0f )
        {
          ret = 1.0f;
          KIJIMA_PRINT( "[WC]* Wait camera debug set camera rate : [%f]\n", ret );
        }
    }
#endif
  
  return ret;

}

//-------------------------------------------------------------------------------
/**
 * @brief 次に選んではいけないカメラのチェック
 *
 * @param[in]  waitCamData        : 待機カメラデーター
 * @param[in]  beforeWaitcamIdx   : 前に選ばれていたカメラデーターのインデックス
 *
 * @return 選択不可の場合trueを返す
 */
//-------------------------------------------------------------------------------
bool BtlvCameraSystem::CheckWaitCameraData_ForbidNext ( const WaitCameraData *waitCamData, int beforeWaitcamIdx )
{

  if( -1 == beforeWaitcamIdx )
    {
      return false;
    }

  for( int i=0;i<10;i++ )
    {
      if( waitCamData->forbidNext[i] == beforeWaitcamIdx)
	{
	  return true;
	}
    }
  
  return false;
      
}

//----------------------------------------------------------------------------------------------------------------
/**
 * @brief カメラ対象可能ターゲット（ポケモンとトレーナー）によるチェック
 *
 * @param[in]  waitCamData        : 待機カメラデーター
 * @param[in]  targerPokeStat     : カメラ対象可能ポケモンのステータス（ビットフィールド:下から4bitが0123に対応）
 * @param[in]  targetTrainerStat  : カメラ対象可能トレーナーのステータス（ビットフィールド:下から4bitが0123に対応）
 * @param[in]  btl_rule           : バトルルール 
 *
 * @return 選択不可の場合trueを返す
 */
//----------------------------------------------------------------------------------------------------------------

bool BtlvCameraSystem::CheckWaitCameraData_Target ( const WaitCameraData *waitCamData, u32 targetPokeStat, u32 targetTrainerStat, BtlRule btl_rule )
{
  int any_poke = 0;
  int any_trainer = 0;
  int ally_poke = 0;
  int ally_trainer = 0;
  int enemy_poke = 0;
  int enemy_trainer = 0;


  // 開始ターゲット
  switch (waitCamData->start_target)
    {
    case WAITCAM_TARGET_POKE:
      { any_poke++; }
      break;

    case WAITCAM_TARGET_ENEMY_POKE:
      { enemy_poke++; }
      break;

    case WAITCAM_TARGET_ALLY_POKE:
      { ally_poke++; }
      break;

    case WAITCAM_TARGET_TRAINER:
      { any_trainer++; }
      break;

    case WAITCAM_TARGET_ENEMY_TRAINER:
      { enemy_trainer++; }
      break;

    case WAITCAM_TARGET_ALLY_TRAINER:
      { ally_trainer++; }
      break;

    case WAITCAM_TARGET_NONE:
    case WAITCAM_TARGET_FIELD:
    default:
      // 特に条件は無い
      /* NO-OP */
      break;
    }

  // 終了ターゲット
  switch (waitCamData->end_target)
    {
    case WAITCAM_TARGET_POKE:
      { any_poke++; }
      break;

    case WAITCAM_TARGET_ENEMY_POKE:
      { enemy_poke++; }
      break;

    case WAITCAM_TARGET_ALLY_POKE:
      { ally_poke++; }
      break;

    case WAITCAM_TARGET_TRAINER:
      { any_trainer++; }
      break;

    case WAITCAM_TARGET_ENEMY_TRAINER:
      { enemy_trainer++; }
      break;

    case WAITCAM_TARGET_ALLY_TRAINER:
      { ally_trainer++; }
      break;

    case WAITCAM_TARGET_NONE:
    case WAITCAM_TARGET_FIELD:
    default:
      // 特に条件は無い
      /* NO-OP */
      break;
    }

  // 必要な数が存在するか？（条件が厳しいものからチェック）

  if( enemy_trainer > 0 )
    {
      int ac = GetEnemyModelCount( targetTrainerStat, btl_rule );
      if (ac < enemy_trainer)
        {
          return true;
        }
    }

  if( ally_trainer > 0 )
    {
      int ac = GetAllyModelCount( targetTrainerStat, btl_rule );
      if (ac < ally_trainer)
        {
          return true;
        }
    }

  if( enemy_poke > 0 )
    {

      int ac = GetEnemyModelCount( targetPokeStat, btl_rule );
      if (ac < enemy_poke)
        {
          return true;
        }
    }

  if( ally_poke > 0 )
    {
      int ac = GetAllyModelCount( targetPokeStat, btl_rule );
      if (ac < ally_poke)
        {
          return true;
        }
    }

  if( any_poke > 0 )
    {
      int ac = GetBitCount( targetPokeStat );

      ac -= ally_poke;
      ac -= enemy_poke;

      if (ac < any_poke)
        {
          return true;
        }
    }

  if( any_trainer > 0 )
    {
      int ac = GetBitCount( targetTrainerStat );

      ac -= ally_trainer;
      ac -= enemy_trainer;

      if (ac < any_trainer)
        {
          return true;
        }
    }

  // All OK
  return false;
}


//-------------------------------------------------------------------------------------------
/**
 * @brief モデルのステータスから敵味方の数をチェック（ポケモン、トレーナー共通）
 *
 * @param[in]  modelSt           : 対象のステータス（ビットフィールド:下から4bitが0123に対応）
 * @param[in]  btl_rule          : バトルルール 
 *
 * @return 敵味方の数
 */
//-------------------------------------------------------------------------------------------
int BtlvCameraSystem::GetEnemyModelCount( u32 modelSt, BtlRule btl_rule )
{

  u32 chk_bit = MakeEnemyMaskByRule( btl_rule );
  
  chk_bit = modelSt & chk_bit;

  return ( GetBitCount( chk_bit ) );

}

int BtlvCameraSystem::GetAllyModelCount( u32 modelSt, BtlRule btl_rule )
{
  u32 chk_bit = MakeAllyMaskByRule( btl_rule );

  chk_bit = modelSt & chk_bit;

  return ( GetBitCount( chk_bit ) );
  
}

// 立っているビットを数える
int BtlvCameraSystem::GetBitCount (u32 val, int bit_max )
{

  int n =0;

  for (int i=0; i<bit_max; i++)
    {
      if(val & 1)
        {
          n++;
        }
      val = val>>1;
    }

  return n;

}

//-----------------------------------------------------------------------------------------
/**
 * @brief バトルルールを考慮して敵か味方かのビットマスクを作る（ポケモン、トレーナー共通）
 *
 * @param[in]  btl_rule          : バトルルール 
 *
 * @return 敵か味方のビットマスク
 */
//-----------------------------------------------------------------------------------------
u32 BtlvCameraSystem::MakeEnemyMaskByRule( BtlRule btl_rule )
{
  int mask = (1<<1) + (1<<3);  // 敵は１番と３番

  // ロイヤルの時は２番も敵
  if (btl_rule == BTL_RULE_ROYAL)
    {
      mask += (1<<2);
    }
  
  return mask;
}

u32 BtlvCameraSystem::MakeAllyMaskByRule( BtlRule btl_rule )
{
  u32 mask = (1<<0);	// プレイヤーは０番

  // ダブルの時は２番が味方
  if (btl_rule == BTL_RULE_DOUBLE)
    {
      mask += (1<<2);
    }

  return mask;
}


//-------------------------------------------------------------------------------
/**
 * @brief HeatLevel から確立で待機カメラレベルを抽選
 *
 * @return 待機カメラレベル
 */
//-------------------------------------------------------------------------------

int BtlvCameraSystem::GetCameraLevel()
{
  // Heat Level : 0-5
  int level = CalcHeatLevel();

  // 2番グループを選ぶ確率
  int rateArr[6]={0,20,40,60,80,100};
  
  int rate = System::GflUse::GetPublicRand(100);
  if( rate < rateArr[level] )
    {
      return WAIT_CAMERA_LEVEL_2;
    }

  return WAIT_CAMERA_LEVEL_1;

}

// ------------------------------------------------------------
/**
 * @brief ヒートレベルからカメラを抽選
 *
 * @param[in] level : ヒートレベル
 *
 * @return 選ばれた待機カメラデーターのインデックス
 */
// ------------------------------------------------------------
u32 BtlvCameraSystem::SelectWaitCameraByHeatLevel( int level )
{

#if PM_DEBUG
  // カメラデータインデックス指定デバッグ機能
  if( mBtlvCore->mDebugWaitCamDataIdx != -1 )
    {
      KIJIMA_PRINT( "[WC]* Wait camera data debug set : [%d]\n", mBtlvCore->mDebugWaitCamDataIdx );
      return mBtlvCore->mDebugWaitCamDataIdx;
    }
#endif

  //レベルは0〜5
  //2番グループを選ぶ確率
  int rateArr[6]={0,20,40,60,80,100};
  
  int rate = System::GflUse::GetPublicRand(100);
  if( rate < rateArr[level] )
    {
      //Lv2
      KIJIMA_PRINT("[WC]>! Camera Level:2");
      return Level2IndexTable[System::GflUse::GetPublicRand(GFL_NELEMS(Level2IndexTable))];
    }
  else
    {
      //Lv1
      KIJIMA_PRINT("[WC]> Camera Level:1");
      return Level1IndexTable[System::GflUse::GetPublicRand(GFL_NELEMS(Level1IndexTable))];
    }

}

// ------------------------------------------------------------
/**
 * @brief ヒートレベルの計算
 *
 * @return ヒートレベル (0-5)
 *
 * @detail 敵味方で累計HP割合が少ない方が20%単位で0-5の６段階。
 */
// ------------------------------------------------------------
int BtlvCameraSystem::CalcHeatLevel(void)
{
  int hp[2] = {0,0};
  int maxHp[2] = {0,0};

#if PM_DEBUG
  // ヒートレベル指定デバッグ機能
  if( mBtlvCore->mDebugWaitCamHeatLevel != -1 )
  {
    KIJIMA_PRINT( "[WC]* HeatLevel debug set : [%d]\n", mBtlvCore->mDebugWaitCamHeatLevel );
    return mBtlvCore->mDebugWaitCamHeatLevel;
  }
#endif
  
  CalcHeatLevelFunc( mBtlvCore->GetBattleParty_Self() , &maxHp[0] , &hp[0] );
  CalcHeatLevelFunc( mBtlvCore->GetBattleParty_Enemy(0) , &maxHp[1] , &hp[1] );

  if( mBtlvCore->GetFriendCleintID() != BTL_CLIENTID_NULL )
    {
      CalcHeatLevelFunc( mBtlvCore->GetBattleParty_Friend()  , &maxHp[0] , &hp[0] );
    }
  if( mBtlvCore->IsMultiMode() )
    {
      CalcHeatLevelFunc( mBtlvCore->GetBattleParty_Enemy(1) , &maxHp[1] , &hp[1] );
    }
  
  int rate[2];
  for( int i=0;i<2;i++ )
    {
      rate[i] = hp[i]*100/maxHp[i];
    }
  
  int level = 5-(gfl2::math::Min(rate[0],rate[1]) / 20);

  // 敵味方で累計HP割合が少ないほうのHPが20%単位の0-5の６段階。
  KIJIMA_PRINT("[WC]> HeatLevel[%d](%d/%d)\n",level,rate[0],rate[1]);
  return level;
}

// ------------------------------------------------------------
/**
 * @brief PT全体のHP、MAXHPの累計を取得(ヒートレベル の計算用)
 *
 * @param[in] party : 対象パーティ
 * @param[out] maxHp : Max HP 累計
 * @param[out] hp : HP 累計
 */
// ------------------------------------------------------------
void BtlvCameraSystem::CalcHeatLevelFunc(BTL_PARTY* party , int *maxHp, int *hp )
{
  u32 member_count = party->GetMemberCount();
  for(u32 i=0; i<member_count; i++)
    {
      const BTL_POKEPARAM* bpp = party->GetMemberDataConst( i );
      (*maxHp) += bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP );
      (*hp)    += bpp->GetValue( BTL_POKEPARAM::BPP_HP );
    }
}

// ------------------------------------------------------------
/**
 * @brief ポケモデルから座標を取ってカメラのオフセットを設定
 *
 * @param[in] btlPos : 対象のポケモンのバトル位置
 * @param[in] type :  対象になるノードタイプ指定
 */
// ------------------------------------------------------------
void BtlvCameraSystem::SetCameraParameter_Pokemon( BtlvPos btlPos, BTLV_WAITCAM_NODE type)
{
  gfl2::math::Vector3 pos;

  BtlvObject_PokeModel *mdl = mBtlvCore->GetPokeModel( btlPos );
  //  GFL_ASSERT (mdl != NULL);

#if PM_DEBUG
  if( !(mdl->IsEnable()) || !(mdl->GetVisible()) )
    {
      KIJIMA_PRINT( "[WC]* Target Pokemon debug Unavailable. : [%d]\n", btlPos );
      return;
    }
#endif

  pos = GetPokemonPosition( btlPos, type ); 

  // Set offset
  SetCameraAnimeOffset( pos );


  // モデルの Rotate Y をカメラに適用してカメラをモデルに向ける
  f32 rot = mdl->GetRotateY();
  f32 cam_rot = 360 - rot;
  f32  rad_rot = gfl2::math::ConvDegToRad( cam_rot );
  KIJIMA_PRINT ("[WC]> Target Poekmon Rot Y (Deg) = [%.1f] (camera Rot Y (Deg) = [%.1f])\n", rot, cam_rot);
  SetCameraAnimeRotateY_Rad (rad_rot);

  // イマジナリーラインを超えないために味方の時は反転
  if ( IsAllySide( btlPos ) )
    {
      SetCameraAnimeFlip(true);
      KIJIMA_PRINT ("[WC]>! Ally Pokemon : FLIPPED.\n");
    }
  
}

gfl2::math::Vector3 BtlvCameraSystem::GetPokemonPosition( BtlvPos btlPos, BTLV_WAITCAM_NODE type )
{

  gfl2::math::Vector3 pos;

  BtlvObject_PokeModel *mdl = mBtlvCore->GetPokeModel( btlPos );
  //  GFL_ASSERT (mdl != NULL);
  pos.Set(0,0,0);

  // Memo : GetNodePosition() の2nd argのidx は右左もしくは中右左の順らしい
  switch (type)
    {
    case WAITCAM_NODE_ORIGIN:
      pos = mdl->GetPosition();
      break;
      
    case WAITCAM_NODE_CENTER:
      mdl->GetNodePosition_Sequence( SEQ_DEF_NODE_CENTER,&pos);
      break;

    case WAITCAM_NODE_FACE:
      mdl->GetNodePosition_Sequence( SEQ_DEF_NODE_FACE,&pos);
      break;

    default:
      break;
    }

  return( pos );

}

// ----------------------------------------------------------------
/**
 * @brief トレーナーモデルから座標を取ってカメラのオフセットを設定
 *
 * @param[in] trainer_idx : 対象のトレーナーインデックス
 * @param[in] type :  対象になるノードタイプ指定
 */
// ----------------------------------------------------------------
void BtlvCameraSystem::SetCameraParameter_Trainer( int trainer_idx, BTLV_WAITCAM_NODE type )
{
  BtlvObject_Chara *mdl = mBtlvCore->GetTrainerModelObj(trainer_idx);
  //GFL_ASSERT (mdl != NULL);

#if PM_DEBUG
  if( !(mdl->IsEnable()) || !(mdl->GetVisible()) )
    {
      KIJIMA_PRINT( "[WC]* Target Trainer debug Unavailable. : [%d]\n", trainer_idx );
      return;
    }
#endif

  gfl2::math::Vector3 pos = GetTrainerPosition( trainer_idx, type );

  // Set offset
  SetCameraAnimeOffset( pos );


  // モデルの Rotate Y をカメラに適用してカメラをモデルに向ける
  f32 rot = mdl->GetRotateY();
  f32 cam_rot = 360 - rot;
  f32  rad_rot = gfl2::math::ConvDegToRad( cam_rot );
  KIJIMA_PRINT ("[WC]> Target Trainer Rot Y (Deg) = [%.1f], y=[%.1f] (camera Rot Y (Deg) = [%.1f])\n", rot, pos.y, cam_rot);
  SetCameraAnimeRotateY_Rad (rad_rot);


  // イマジナリーラインを超えないために味方の時は反転
  if ( IsAllySide( trainer_idx ) )
    {
      SetCameraAnimeFlip(true);
      KIJIMA_PRINT ("[WC]>! Ally Trainer : FLIPPED.\n");
    }

}

gfl2::math::Vector3 BtlvCameraSystem::GetTrainerPosition( int trainer_idx, BTLV_WAITCAM_NODE type )
{
  BtlvObject_Chara *mdl = mBtlvCore->GetTrainerModelObj(trainer_idx);
  //GFL_ASSERT (mdl != NULL);

  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* jointNode;
  gfl2::math::Vector3 pos;

  pos.Set(0,0,0);

  switch (type)
    {

    case WAITCAM_NODE_ORIGIN:
    default:
      {
        pos = mdl->GetPosition ();
      }
      break;

    case WAITCAM_NODE_FACE:
      {
        jointNode = mdl->GetJointNode("Head");
        const gfl2::math::Matrix34 mtx = jointNode->GetWorldMatrix();
        pos = mtx.GetElemPosition();
      }
      break;

    case WAITCAM_NODE_CENTER:
      {
        jointNode = mdl->GetJointNode("Waist");
        const gfl2::math::Matrix34 mtx = jointNode->GetWorldMatrix();
        pos = mtx.GetElemPosition();
      }
      break;
    }

  return( pos );

}

// ---------------------------------------------------------------------------
/**
 * @brief 待機カメラが決定するパラメーターのセット
 * 
 * @detail 以下の要素を設定する
 * オフセット
 * スケール
 * 地上チェック
 * 
 * ※StartCameraAnime()で初期化する項目なので、それ以降に設定する必要がある。
 */
// ---------------------------------------------------------------------------
void BtlvCameraSystem::SetWaitCameraParameter (void)
{
  //  GFL_ASSERT( mWaitCamDataIdx != -1 );
 
  // Set default
  mAnimeOffset.Set(0,0,0);
  mAnimeScale.Set(1,1,1);
  mIsFlipAnime = false;
  mCheckGround = false;

  BTLV_WAITCAM_NODE    st_node = BTLV_WAITCAM_DATA[mWaitCamDataIdx].start_node;
  BTLV_WAITCAM_NODE    ed_node = BTLV_WAITCAM_DATA[mWaitCamDataIdx].end_node;

  gfl2::math::Vector3  st_ofs, ed_ofs;
  

  // カメラ対象がある場合はオフセット設定
  if( BTL_VPOS_ERROR != mTargetPoke )
    {
      // ポケモデルから座標を取ってカメラのオフセットを設定
      SetCameraParameter_Pokemon( mTargetPoke, st_node);

#ifdef DEBUG_ONLY_FOR_kijima_satoshi
      KIJIMA_PRINT ( "[WC]>-> Pokemon targeted: index = [%d]\n", mTargetPoke );
#else
      GFL_PRINT ( "[WC]-> Pokemon targeted : [%d]\n", mTargetPoke );
#endif

      if( IsWaitCamMoveTarget )
        {
          st_ofs = GetPokemonPosition( mTargetPoke, st_node );
        }
    }

  if( -1 !=  mTargetTrainerIdx )
    {
      // トレーナーモデルから座標を取ってカメラのオフセットを設定
      SetCameraParameter_Trainer( mTargetTrainerIdx, st_node );

#ifdef DEBUG_ONLY_FOR_kijima_satoshi
      KIJIMA_PRINT ( "[WC]>-> Trainer targeted : index = [%d]\n", mTargetTrainerIdx );
#else
      GFL_PRINT ( "[WC]-> Trainer targeted : [%d]\n", mTargetTrainerIdx );
#endif

      if( IsWaitCamMoveTarget )
        {
          st_ofs = GetTrainerPosition( mTargetTrainerIdx, st_node );
        }
    }

  // エンドターゲットがある場合
  if( BTL_VPOS_ERROR != mTargetPoke_end )
    {
      ed_ofs = GetPokemonPosition( mTargetPoke_end, ed_node );
      MakeTransporterInstance (st_ofs, ed_ofs );
    }

  if( -1 !=  mTargetTrainerIdx_end )
    {
      ed_ofs = GetTrainerPosition( mTargetTrainerIdx_end, ed_node );
      MakeTransporterInstance( st_ofs, ed_ofs );
    }

  // バトルルールによるスケールを設定
  gfl2::math::Vector3 scale;
  float rate = GetCameraRate( &(BTLV_WAITCAM_DATA[mWaitCamDataIdx]) );
  scale.Set( rate, rate, rate );
  SetCameraAnimeScale( scale );

  // Ground check フラグのセット
  mCheckGround = BTLV_WAITCAM_DATA[mWaitCamDataIdx].checkGround;

}

// 座標線形補完用インスタンスの作成
void BtlvCameraSystem::MakeTransporterInstance (gfl2::math::Vector3 st_pos, gfl2::math::Vector3 ed_pos )
{
  gfl2::heap::HeapBase *heap = mBtlvCore->GetWaitCameraHeap();
  mOffsetTransporter = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransLiner<f32> >( st_pos, ed_pos, mWaitCameraCnt, heap );
  
  KIJIMA_PRINT ("[WC]>> Set Transport (Count %d) ", mWaitCameraCnt);
  KIJIMA_PRINT ("Start=(%f,%f,%f) - ", st_pos.x,st_pos.y,st_pos.z );
  KIJIMA_PRINT ("end=(%f,%f,%f)\n", ed_pos.x,ed_pos.y,ed_pos.z );

}

// 味方モンかどうかの判定
// 統一のためにUIクラスの関数で判断
bool BtlvCameraSystem::IsAllySide( int btlPos )
{
  return( !(mBtlvCore->GetUiSys()->BtlvPosIsEnemySide( (BtlvPos)btlPos )) ); 
}

// カメラの対象になれるポケモンのステータス
u32 BtlvCameraSystem::GetTargetablePokemonStatus ()
{
  u32 ret = 0;

  // POKE_MODEL_NUM;  MAXポケモンモデル数 ("btlv_types.h"で定義)
  for(int i=0;i<POKE_MODEL_NUM;i++ )
    {
      BtlvObject_PokeModel*poke = mBtlvCore->GetPokeModel(static_cast<BtlvPos>(i));
      if( poke->IsEnable() && poke->GetVisible() )
        {
	  ret |= (1<<i);
	}
    }
  
  return ret;
}

// カメラの対象になれるトレーナーのステータス
u32 BtlvCameraSystem::GetTargetableTrainerStatus ()
{
  u32 ret = 0;

  // TRAINER_MODEL_NUM : MAXトレーナーモデル数 ("btlv_types.h"で定義)
  for(int i=0;i<TRAINER_MODEL_NUM;i++ )
    {
    BtlvObject_Chara *t_mdl = mBtlvCore->GetTrainerModelObj( i );
    if( t_mdl->IsEnable() && t_mdl->GetVisible() )
      {
	ret |= (1<<i);
      }
    }

  return ret;

}


GFL_NAMESPACE_END(btl)

