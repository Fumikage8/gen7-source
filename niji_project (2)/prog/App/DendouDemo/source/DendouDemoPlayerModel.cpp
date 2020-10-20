//======================================================================
/**
 * @file DendouDemoPlayerModel.cpp
 * @brief 殿堂入りデモ プレイヤーモデル関連
 * @author  nagihashi_toru(移植), Hiroyuki Nakamura
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <model/include/gfl2_CharaModelFactory.h>
#include <System/include/DressUp.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph\Resource/gfl2_ResourceManager.h>
#include <GameSys/include/GameManager.h>
#include <Savedata/include/MyStatus.h> 
#include <System/include/GflUse.h>

#include "DendouDemoPlayerModel.h"

#include <arc_def_index/arc_def.h>
#include <niji_conv_header/battle/chara/chara_model_id.h>
#include <niji_conv_header/battle/chara/p1_base.h>
#include <niji_conv_header/battle/chara/p2_base.h>

#include <niji_conv_header/battle/chara/chara_model_dynamic_motion.h>  //非常駐モーション種類
#include <niji_conv_header/battle/chara/btpc0001_00_action1.h>
#include <niji_conv_header/battle/chara/btpc0002_00_action1.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	app::dendou_demo
 * @brief			殿堂入りデモ
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace app {
namespace dendou_demo {

  namespace {
    enum
    {
      SEQ_LOAD_FILE_LOAD,
      SEQ_LOAD_FILE_WAIT,
      SEQ_LOAD_DRESS_UP_LOAD,
      SEQ_LOAD_DRESS_UP_LOAD_WAIT,
      SEQ_LOAD_CREATE_MODEL,
      SEQ_LOAD_CREATE_DRESS_UP,
      SEQ_LOAD_SETTING,
      SEQ_LOAD_SUCCESS,
    };
    enum
    {
      SEQ_IDLE,
      SEQ_START_POSE_MOTION,   //待機待ち
      SEQ_WAIT_POSE_MOTION,   //待機待ち
      SEQ_START_ACTION_MOTION,
      SEQ_WAIT_ACTION_MOTION,
    };

    static const u32 SMOOTH_FRAME = 9;

    //仕様
    //下記のように待機を数回やってからガッツポーズを行う
    //フェードアウト開始 → 待機*9 → ガッツポーズ → 待機*23 → ガッツポーズ → 待機*23 →…以下繰り返し
    static const u32 ACTION_MOTION_FIRST = 9;
    static const u32 ACTION_MOTION_NORMAL = 23;
  }

//--------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 *
 * @param		description 設定
 */
//--------------------------------------------------------------------------------------------
PlayerModel::PlayerModel( const Description & description ):
	pHeap( description.heap ),
  asyncFileManager( description.asyncFileManager ),
  renderingManager( description.renderingManager ),
  pModelFactoryHeap(NULL),
  pModelResourceManagerHeap(NULL),
  pModel(NULL),
  pCharaModelFactory(NULL),
  pDressUpModelResourceManager(NULL),
  modelID(0),
  waitMotID(0),
  deleteSeq(0),
  setupSeq(0),
  updateSeq(0),
  counter(0),
  max_counter(0),
  actionMotID(0),
  actionMotPack(0)
{
  pModel  = GFL_NEW( pHeap->GetDeviceHeap() ) poke_3d::model::DressUpModel();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//--------------------------------------------------------------------------------------------
PlayerModel::~PlayerModel()
{
  GFL_SAFE_DELETE(pModel);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PlayerModel::Update(void)
{
  if( this->IsCreated() )
  {
    pModel->UpdateEye();
    pModel->UpdateMouth();

    switch( updateSeq )
    {
    case SEQ_IDLE:
      //wait
      break;
    case SEQ_START_POSE_MOTION:   //待機開始
      pModel->ChangeAnimation( waitMotID, 0 );
      pModel->SetAnimationLoop( false, 0 );
      updateSeq = SEQ_WAIT_POSE_MOTION;
      //fall through
    case SEQ_WAIT_POSE_MOTION:   //待機待ち
      pModel->UpdateAnimation();
      if( !pModel->IsAnimationLastFrame() )
      {
        break;//アニメ中なのでbreak
      }
      else
      {
        if( ++counter < max_counter )
        {
          pModel->ChangeAnimation( waitMotID, 0 );
          pModel->SetAnimationLoop( false, 0 );
          break;  //カウンター内なので、まだ待機
        }
        else
        {
          counter = 0;
          max_counter = ACTION_MOTION_NORMAL;
          updateSeq = SEQ_START_ACTION_MOTION;
          //fall through
        }
      }
    case SEQ_START_ACTION_MOTION:
      pModel->ChangeAnimation( actionMotID, 0 );
      pModel->SetAnimationLoop( false, 0 );
      updateSeq = SEQ_WAIT_ACTION_MOTION;
      //fall through
    case SEQ_WAIT_ACTION_MOTION:
      pModel->UpdateAnimation();
      if( pModel->IsAnimationLastFrame() )
      {
        pModel->ChangeAnimation( waitMotID, 0 );
        pModel->SetAnimationLoop( false, 0 );
        updateSeq = SEQ_WAIT_POSE_MOTION;
      }
      break;
    default:
      GFL_ASSERT(0);
      break;
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		モデル作成
 *
 * @param		myst								プレイヤーデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PlayerModel::Create( const Savedata::MyStatus * myst )
{

    //  トレーナモデルレンダーテクスチャのロードテーブル
    static const struct 
    {
      u32 modelId;
      u32 waitMotionId;
      u32 actoinMotionId;
      u32 actoinMotionPack;
    } PLAYER_RESOURCE_ID[]  =
    {
      { CHARA_MODEL_ID_P1_BASE, P1_BASE_BA005_WAIT01, P1_BASE_SD9060_ACTION01, BTPC0001_00_ACTION1 },
      { CHARA_MODEL_ID_P2_BASE, P2_BASE_BA005_WAIT01, P2_BASE_SD9060_ACTION01, BTPC0002_00_ACTION1 },
    };
    /*
    上記順番は下記の性別順
  enum Sex {
    SEX_MALE,    // オス
    SEX_FEMALE,  // メス
    SEX_UNKNOWN, // 不明
    SEX_NUM,
  };
    */

  pCharaModelFactory  = GFL_NEW( pHeap->GetDeviceHeap() ) poke_3d::model::CharaModelFactory();
  pModelFactoryHeap = GFL_CREATE_LOCAL_HEAP( pHeap->GetDeviceHeap(), System::DressUp::BATTLE_HEAP_SIZE_ANIM, gfl2::heap::HEAP_TYPE_EXP, false );
  pCharaModelFactory->Initialize( asyncFileManager, pModelFactoryHeap, ARCID_CHARA_MODEL_BATTLE_NORMAL );
  pDressUpModelResourceManager = GFL_NEW( pHeap->GetDeviceHeap() ) poke_3d::model::DressUpModelResourceManager();
  pModelResourceManagerHeap = GFL_CREATE_LOCAL_HEAP( pHeap->GetDeviceHeap(), System::DressUp::BATTLE_HEAP_SIZE_MODEL, gfl2::heap::HEAP_TYPE_EXP, false );
  pDressUpModelResourceManager->Initialize( asyncFileManager, pModelResourceManagerHeap, System::DressUp::GetDressUpArcIdListBattle() );

  modelID = PLAYER_RESOURCE_ID[ myst->GetSex() ].modelId;
  waitMotID = PLAYER_RESOURCE_ID[ myst->GetSex() ].waitMotionId;
  actionMotID = PLAYER_RESOURCE_ID[ myst->GetSex() ].actoinMotionId;
  actionMotPack = PLAYER_RESOURCE_ID[ myst->GetSex() ].actoinMotionPack;
  dressUpParam = myst->GetDressup();
}

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
bool PlayerModel::Delete(void)
{
  enum
  {
    SEQ_END_REMOVE_RENDER = 0,
    SEQ_END_MODEL_DELETE,
    SEQ_END_UNLOAD_DRESS,
    SEQ_END_UNLOAD_MODEL,
    SEQ_END
  };
  switch( deleteSeq )
  {
  case SEQ_END_REMOVE_RENDER:
    renderingManager->RemoveDressUpModel( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pModel, 2 );
    deleteSeq++;
    //fall
  case SEQ_END_MODEL_DELETE:
    if( pModel->CanDestroy() == true )
    {
      pModel->Destroy();
      deleteSeq++;
      //fall
    }
    else 
    {
      return false;
    }
  case SEQ_END_UNLOAD_DRESS:
    if( pDressUpModelResourceManager->CanUnloadDressUpParts( dressUpParam ) == true )
    {

      pCharaModelFactory->UnloadDynamicAnimation(actionMotPack);
      pDressUpModelResourceManager->UnloadDressUpDynamicAnimation(dressUpParam, actionMotPack);
      pDressUpModelResourceManager->UnloadDressUpParts( dressUpParam );
      deleteSeq++;
      //fall
    }
    else 
    {
      return false;
    }
  case SEQ_END_UNLOAD_MODEL:
    // 読み込んだリソースの解放ができるか確認
    if( pCharaModelFactory->CanUnloadModel( modelID ) == true )
    {
      // 読み込んだリソースの開放
      pCharaModelFactory->UnloadModel( modelID );


      GFL_SAFE_DELETE( pCharaModelFactory );
      GFL_SAFE_DELETE( pDressUpModelResourceManager );
      GFL_DELETE_HEAP( pModelFactoryHeap );
      GFL_DELETE_HEAP( pModelResourceManagerHeap );
      deleteSeq++;
      //fall
    }
    else 
    {
      return false;
    }
  case SEQ_END:
    return true;
  default:
    GFL_ASSERT(0);
    return true;
  }
}

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
bool PlayerModel::IsCreated(void)
{
	return setupSeq == SEQ_LOAD_SUCCESS;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		表示切り替え
 *
 * @param		flg		true = 表示, false = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PlayerModel::SetVisible( bool flg )
{
	pModel->SetVisible( flg );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		終了モーション設定
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PlayerModel::SetEndMotion(void)
{
  max_counter = ACTION_MOTION_FIRST;
  updateSeq = SEQ_START_POSE_MOTION;
}

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
void PlayerModel::SetEndMotionNext(void)
{
}

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
bool PlayerModel::Setup(void)
{
  switch ( setupSeq )
  {
  case SEQ_LOAD_FILE_LOAD:
    pCharaModelFactory->LoadModelAsync( modelID );
    pDressUpModelResourceManager->LoadDressUpPartsAsync( dressUpParam );
    pCharaModelFactory->LoadDynamicAnimationAsync(actionMotPack);
    pDressUpModelResourceManager->LoadDressUpDynamicAnimationAsync(dressUpParam,actionMotPack);

    ++setupSeq;
    //fallthrough
  case SEQ_LOAD_FILE_WAIT:
    {
      bool isEnd = true;
      isEnd &= pCharaModelFactory->IsModelLoaded( modelID );
      isEnd &= pDressUpModelResourceManager->IsDressUpPartsLoaded( dressUpParam );
      isEnd &= pCharaModelFactory->IsDynamicAnimationLoaded(actionMotPack);
      isEnd &= pDressUpModelResourceManager->IsDressUpDynamicAnimationLoaded(dressUpParam,actionMotPack);

      if( isEnd ) 
      {
        ++setupSeq;
        //fallthrough
      }else {
        return false;
      }
    }
  case SEQ_LOAD_DRESS_UP_LOAD:
    pDressUpModelResourceManager->SetupDressUpPartsAsync( pHeap->GetDevGLHeapAllocator(), dressUpParam );
    ++setupSeq;
    //fallthrough
  case SEQ_LOAD_DRESS_UP_LOAD_WAIT:
    if( pDressUpModelResourceManager->WaitSetupDressUpPartsAsync( pHeap->GetDevGLHeapAllocator(), dressUpParam ) )
    {
      ++setupSeq;
      //fallthrough
    }
    else 
    {
      return false;
    }
  case SEQ_LOAD_CREATE_MODEL:
    if( pCharaModelFactory->SetupModel( pHeap->GetDevGLHeapAllocator(), modelID ) == false )
    {
      //  ありえない
      GFL_ASSERT_STOP( 0 );
    }

    pCharaModelFactory->SetupDynamicAnimation(pHeap->GetDevGLHeapAllocator(), pHeap->GetDeviceHeap(), actionMotPack);
    pDressUpModelResourceManager->SetupDressUpDynamicAnimation(pHeap->GetDevGLHeapAllocator(), pHeap->GetDeviceHeap(), dressUpParam, actionMotPack);

    ++setupSeq;
    //fallthrough
  case SEQ_LOAD_CREATE_DRESS_UP:
    if( pCharaModelFactory->CreateDressUpModel( pHeap->GetDevGLHeapAllocator(), pHeap->GetDeviceHeap(), pModel, pDressUpModelResourceManager, modelID ) == false )
    {
      //  ありえない
      GFL_ASSERT_STOP( 0 );
    }
    ++setupSeq;
    //fallthrough
  case SEQ_LOAD_SETTING:
    renderingManager->AddDressUpModel( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, pModel, 2 );

    {
      static const gfl2::math::Vector3 trans( 0.0f, 0.0f, 0.0f );
      static const gfl2::math::Vector3 rotate( 0.0f, 0.0f, 0.0f );	// ラジアン

      pModel->SetPosition( trans );
      pModel->SetRotation( rotate );
      pModel->SetVisible( false );


      poke_3d::model::CharaModel::AutoBlinkMode autoBlinkMode = poke_3d::model::CharaModel::AUTO_BLINK_MODE_BATTLE;
      pModel->SetAutoBlinkMode( autoBlinkMode );
      pModel->SetupRandomFuc( System::GflUse::GetPublicRand );
    }
    ++setupSeq;
    //fallthrough
  case SEQ_LOAD_SUCCESS:
    return true;
  }

  return false;
}

}	// dendou_demo
}	// app

