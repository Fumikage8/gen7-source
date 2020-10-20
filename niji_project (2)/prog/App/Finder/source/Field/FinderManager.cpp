//======================================================================
/**
 * @file FinderManager.cpp
 * @date 2016/02/01 11:38:14
 * @author miyachi_soichi
 * @brief ファインダー管理者
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <gfx/include/gfl2_GFGL.h>
// savedata
#include <Savedata/include/PokeFinderSave.h>
// self
#include "App/Finder/include/Field/FinderManager.h"
// gamesys
#include "GameSys/include/TimeZone.h"
// PokeTool
#include "PokeTool/include/KawaigariParamOptionCoreManager.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/FieldRenderingPipeLineManager.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"
// sound
#include "Field/FieldStatic/include/Sound/FieldSound.h"
// camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypePokeFinderForPlacement.h"
// placement
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldPokeFinderAccessor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldPokeFinderActor.h"
#include "Field/FieldRo/include/Traffic/FieldTraffic_define.h"
// model
#include "Field/FieldRo/include/TrialModel/FieldTrialModelFactory.h"
#include "Field/FieldRo/include/TrialModel/FieldFinderModel.h"
// menu
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProcManager.h"
#include "App/Finder/include/FinderProc.h"
// util
#include "Field/FieldRo/include/TrialModel/FieldFinderUtil.h"
// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/poke_finder_param.gaix>
#include <arc_index/NijiRenderPath.gaix>
#if defined(GF_PLATFORM_CTR)
#include <niji_conv_header/system/renderpath/ctr/DepthOfFieldResource.h>
#elif  defined(GF_PLATFORM_WIN32)
#include <niji_conv_header/system/renderpath/wingl/DepthOfFieldResource.h>
#endif
#include <niji_conv_header/field/poke_finder/situation_single.h>
#include <niji_conv_header/field/poke_finder/situation_double.h>
#include <niji_conv_header/field/poke_finder/situation_other.h>
// debug
#if PM_DEBUG
#include <gfx/include/gfl2_PCDebugUtil.h>
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldRo/include/Debug/FieldDebugPokeFinder.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "App/Finder/include/Field/DebugFinderInfo.h"
#endif

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Finder)

static const u32 SNAPSHOT_WAIT_FRAME = 4;
static const s32 CONV_MINUTE_TO_SEC = 60;
static const s32 CONV_HOUR_TO_SEC = 60*60;
static const f32 FINDER_WIDTH = 200.0f;
static const f32 FINDER_HEIGHT = 120.0f;
static const f32 FINDER_WIDTH_2 = 400.0f;
static const f32 FINDER_HEIGHT_2 = 240.0f;
static const u32 FINDER_CALL_RELOAD = 5 * 30;
static const f32 MAX_BGM_VOLUME = 0.3f; // 一之瀬さん要望対応：ボリューム調整
static const f32 MIN_BGM_VOLUME = 0.1f; // 一之瀬さん要望対応：ボリューム調整

#define ENABLE_RENDER_DOF 0

/**
 *  @class  FinderSubjectDataAccessor
 *  @brief  Subjectアクセッサ
 */
class FinderSubjectDataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderSubjectDataAccessor);
public:
  struct FinderModelData
  {
    u16 monsNo;       //!< ポケモンID
    u16 formNo;       //!< フォームID
    u32 basePoint;    //!< 基礎得点
    u8  sex;          //!< 性別
    u8  appearance;   //!< 出現確率
    u8  ext1;         //!< 拡張データ１
    u8  ext2;         //!< 拡張データ２
  };
//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止
  struct FinderSubjectData
  {
    u8                poke_num_zero; //!< ポケモン出現0匹の確率
    u8                poke_num_one;  //!< ポケモン出現1匹の確率
    u8                poke_num_two;  //!< ポケモン出現2匹の確率
    u8                rare_ratio;    //!< レア出現率
    u8                pass_a_num;    //!< Aパス使用数
    u8                pass_b_num;    //!< Bパス使用数
    u8                statue_num;    //!< 滞在パス使用数
    u8                rare_num;      //!< レアパス使用数
    FinderModelData   modelData[];   //!< レコードデータ
  };
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

public:
  /**
   *  @brief  コンストラクタ
   *  @param  buf  データ
   */
  FinderSubjectDataAccessor( void *buf )
  {
    GFL_ASSERT_MSG( buf, "FinderSubjectData is NULL!\n" );
    m_pData = reinterpret_cast<FinderSubjectData*>( buf );
  }
  /**
   *  @brief  出現数の抽選
   */
  s32 CalcSubjectNum( void )
  {
    u32 maxi = m_pData->poke_num_zero + m_pData->poke_num_one + m_pData->poke_num_two;
    GFL_ASSERT_MSG( maxi == 100, "PokeRate is not 100\n" );
    u32 rand = System::GflUse::GetPublicRand( maxi );
    u32 border = 0;
    // 0
    border += m_pData->poke_num_zero;
    if( rand < border )
    {
      return 0;
    }
    // 1
    border += m_pData->poke_num_one;
    if( rand < border )
    {
      return 1;
    }
    // 2
    border += m_pData->poke_num_two;
    if( rand < border )
    {
      return 2;
    }

    return 0;
  }
  /**
   *  @brief  通常抽選
   */
  s32 ExecLotteryNormal( s32 old_use = -1 )
  {
    s32 result = Field::TrialModel::FINDER_PATH_USE_NONE;
    u32 num = 0;
    s32 box[3] = { -1, -1, -1 };
    // Aを抽選箱に入れる
    if( m_pData->pass_a_num > 0 && Field::TrialModel::FINDER_PATH_USE_MOVE_A != old_use )
    {
      box[num] = Field::TrialModel::FINDER_PATH_USE_MOVE_A; ++ num;
    }
    // Bを抽選箱に入れる
    if( m_pData->pass_b_num > 0 && Field::TrialModel::FINDER_PATH_USE_MOVE_B != old_use )
    {
      box[num] = Field::TrialModel::FINDER_PATH_USE_MOVE_B; ++ num;
    }
    // Sを抽選箱に入れる
    if( m_pData->statue_num > 0 && Field::TrialModel::FINDER_PATH_USE_WAIT != old_use )
    {
      box[num] = Field::TrialModel::FINDER_PATH_USE_WAIT; ++ num;
    }

    // 安全装置
    if( num <= 0 )
    {
      return Field::TrialModel::FINDER_PATH_USE_NONE;
    }

    u32 rand = System::GflUse::GetPublicRand( num );
    // 箱を開ける
    result = box[rand];
    return result;
  }
  /**
   *  @brief  レア枠抽選
   */
  bool ExecLotteryRare( void )
  {
    // 抽選率が設定されていない
    if( m_pData->rare_ratio <= 0 ){ return false; }
    u32 rand = System::GflUse::GetPublicRand( 100 );
    u32 border = m_pData->rare_ratio;
    if( rand < border )
    {
      // 安全のためレア設定が0より大きいことを保証する
      GFL_ASSERT( m_pData->rare_num > 0 );
      // レア設定が0では以後の抽選ができないので論理式での返却とする
      return m_pData->rare_num > 0;
    }

    return false;
  }
  /**
   *  @brief  通常idxの取得
   */
  s32 GetNormalIndex( s32 eUse )
  {
    switch( eUse )
    {
    case Field::TrialModel::FINDER_PATH_USE_MOVE_A: return GetPassAIndex();
    case Field::TrialModel::FINDER_PATH_USE_MOVE_B: return GetPassBIndex();
    case Field::TrialModel::FINDER_PATH_USE_WAIT:   return GetStatueIndex();
    }

    return  -1;
  }
  /**
   *  @brief  パスAデータidxの取得
   */
  s32 GetPassAIndex( void )
  {
    s32 offset = 0;
    s32 num = m_pData->pass_a_num;
    s32 rand = static_cast<s32>( System::GflUse::GetPublicRand( 100 ) );
    s32 border = 0;
    for( s32 i = 0; i < num; ++ i )
    {
      s32 idx = offset + i;
      border += m_pData->modelData[idx].appearance;
      if( rand < border )
      {
        return idx;
      }
    }

    // 発見できず
    return -1;
  }
  /**
   *  @brief  パスBデータidxの取得
   */
  s32 GetPassBIndex( void )
  {
    s32 offset = m_pData->pass_a_num;
    s32 num = m_pData->pass_b_num;
    s32 rand = static_cast<s32>( System::GflUse::GetPublicRand( 100 ) );
    s32 border = 0;
    for( s32 i = 0; i < num; ++ i )
    {
      s32 idx = offset + i;
      border += m_pData->modelData[idx].appearance;
      if( rand < border )
      {
        return idx;
      }
    }

    // 発見できず
    return -1;
  }
  /**
   *  @brief  待機データidxの取得
   */
  s32 GetStatueIndex( void )
  {
    s32 offset = m_pData->pass_a_num + m_pData->pass_b_num;
    s32 num = m_pData->statue_num;
    s32 rand = static_cast<s32>( System::GflUse::GetPublicRand( 100 ) );
    s32 border = 0;
    for( s32 i = 0; i < num; ++ i )
    {
      s32 idx = offset + i;
      border += m_pData->modelData[idx].appearance;
      if( rand < border )
      {
        return idx;
      }
    }

    // 発見できず
    return -1;
  }
  /**
   *  @brief  レアデータidxの取得
   */
  s32 GetRareIndex( void )
  {
    s32 offset = m_pData->pass_a_num + m_pData->pass_b_num + m_pData->statue_num;
    s32 num = m_pData->rare_num;
    s32 rand = static_cast<s32>( System::GflUse::GetPublicRand( 100 ) );
    s32 border = 0;
    for( s32 i = 0; i < num; ++ i )
    {
      s32 idx = offset + i;
      border += m_pData->modelData[idx].appearance;
      if( rand < border )
      {
        return idx;
      }
    }

    // 発見できず
    return -1;
  }
  /**
   *  @brief  配列番号指定でのモデルデータの取得
   */
  const FinderModelData *GetModelData( s32 idx )
  {
    s32 maxi = m_pData->pass_a_num + m_pData->pass_b_num + m_pData->statue_num + m_pData->rare_num;
    if( 0 <= idx && idx < maxi )
    {
      return &( m_pData->modelData[ idx ] );
    }
    return NULL;
  }
  /**
   *  @brief  最大数を取得する
   */
  u32 GetMaxData( void )
  {
    return m_pData->pass_a_num + m_pData->pass_b_num + m_pData->statue_num + m_pData->rare_num;
  }
  /**
   *  @brief  配列番号から使用PASS_USEを見つける
   */
  s32 GetPassUse( s32 idx )
  {
    s32 border = 0;
    border += m_pData->pass_a_num;
    if( idx < border )
    {
      return Field::TrialModel::FINDER_PATH_USE_MOVE_A;
    }
    border += m_pData->pass_b_num;
    if( idx < border )
    {
      return Field::TrialModel::FINDER_PATH_USE_MOVE_B;
    }
    border += m_pData->statue_num;
    if( idx < border )
    {
      return Field::TrialModel::FINDER_PATH_USE_WAIT;
    }
    border += m_pData->rare_num;
    if( idx < border )
    {
      return Field::TrialModel::FINDER_PATH_USE_RARE;
    }
    return Field::TrialModel::FINDER_PATH_USE_NONE;
  }
private:
  FinderSubjectData   *m_pData;

}; // class FinderSubjectDataAccessor

/**
 *  @class  FinderPokeDataAccessor
 *  @brief  リソースごとのデータアクセッサ
 */
class FinderPokeDataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderPokeDataAccessor);
public:
  struct AppealData
  {
    u32 motion;
    u32 voice;
    u32 sort;
  };
  struct PokeData
  {
    u32 mons_no;
    u32 enable;
    f32 degree;
    f32 frame;
    f32 wait;
    u32 move_type;
    u32 sort;
    u32 voice;
    f32 min_x;
    f32 min_y;
    f32 min_z;
    f32 max_x;
    f32 max_y;
    f32 max_z;
    AppealData appeal[10];
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  FinderPokeDataAccessor( void *buf )
  {
    GFL_ASSERT( buf );
    m_pData = reinterpret_cast<PokeDataHeader*>(buf);
  }
  /**
   *  @brief  総数を取得
   */
  inline u32 GetDataMax( void ){ return m_pData->num; }

  /**
   *  @brief  IDを指定してデータを取得
   */
  const PokeData *GetData( u32 idx )
  {
    u32 max = GetDataMax();
    for( u32 i = 0; i < max; ++ i )
    {
      const PokeData *pData = &(m_pData->data[i]);
      if( pData->mons_no == idx )
      {
        return pData;
      }
    }
    return NULL;
  }

private:
//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止
  struct PokeDataHeader
  {
    u32 num;
    PokeData data[];
  };
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP
  PokeDataHeader *m_pData;

}; // class FinderPokeDataAccessor

/**
 *  @class  SituationDataAccessor
 *  @brief  コメントシチュエーションデータアクセッサ
 */
class SituationDataAccessor
{
public:
  /**
   *  @brief  コンストラクタ
   *  @param  buf   データ
   */
  SituationDataAccessor( void *buf )
  {
    GFL_ASSERT( buf );
    if( buf )
    {
      m_pData = reinterpret_cast<SituationDataHeader*>( buf );
    }
    else
    {
      m_pData = NULL;
    }
  }
  /**
   *  @brief  データ取得
   *  @param  idx       データID
   *  @param  out_data  返却用
   *  @return 取得成否の返却
   */
  bool GetData( u32 idx, Field::Finder::Manager::CommentData &out_data )
  {
    if( m_pData && idx < m_pData->num )
    {
      out_data.score      = m_pData->data[idx].score;
      out_data.msgID      = static_cast<s32>( m_pData->data[idx].comment );
      out_data.priority   = static_cast<s32>( m_pData->data[idx].priority );
      return true;
    }
    return false;
  }

private:
  struct SituationData
  {
    s32 score;
    s32 priority;
    u16 comment;
    u16 padding;
  };
//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止
  struct SituationDataHeader
  {
    u32 num;
    SituationData data[];
  };
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

  SituationDataHeader *m_pData;
}; // class SituationDataAccessor

/**
 *  @class  Manager
 *  @brief  ファインダー管理者
 */
/**
 *  @brief  コンストラクタ
 */
Manager::Manager( void )
: m_pHeap( NULL )
, m_pHeap_FileRead( NULL )
, m_pDofHeap( NULL )
, m_pAllocator( NULL )
, m_pUIProc( NULL )
, m_pCameraManager( NULL )
, m_pCamera( NULL )
, m_pModelFactory( NULL )
, m_pWeather( NULL )
, m_pKawaigari( NULL )
, m_pFinderMenuParam( NULL )
, m_pPokeData( NULL )
, m_pSubjectData( NULL )
, m_pSituationSingle( NULL )
, m_pSituationDouble( NULL )
, m_pSituationOther( NULL )
, m_pRenderPath( NULL )
, m_pColorBuffer( NULL )
, m_TmpSkipDrawMode( GameSys::GameManager::SKIP_DRAWMODE_NONE )
, m_eState( State_Wait )
, m_eResult( EndResult_None )
, m_nStep( 0 )
, m_nSpotID( 0 )
, m_nSubjectID( -1 )
, m_nSnapCount( 0 )
, m_nCallReload( 0 )
, m_nDelayFrame( 0 )
, m_TrialMonsNo( MONSNO_NULL )
, m_Focus_Old( 0 )
, m_bInitialized( false )
, m_bFinalized( false )
, m_HasPathA( false )
, m_HasPathB( false )
, m_HasStatue( false )
, m_HasOneway( false )
, m_IsNoon( false )
, m_IsSnapshot( false )
, m_IsVerLimit( false )
{
  for( s32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    m_pModel[ i ] = NULL;
    m_pPokeHeap[ i ] = NULL;
  }
  m_rSituation.score = 0;
  for( s32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    m_rSituation.inScreen[ i ] = false;
    m_rSituation.scr_score[ i ] = 0.0f;
    m_rSituation.scr_out[ i ] = 0.0f;
    m_rSituation.rot[ i ] = 0.0f;
    m_rSituation.eRot[ i ] = ROT_DIR_FRONT;
    m_rSituation.min[ i ] = gfl2::math::Vector2::GetZero();
    m_rSituation.max[ i ] = gfl2::math::Vector2::GetZero();
  }
  for( s32 i = 0; i < Field::TrialModel::FINDER_VALUE_COMMENT_MAX; ++ i )
  {
    m_rComment[ i ].score = 0;
    m_rComment[ i ].msgID = 0;
    m_rComment[ i ].priority = 0;
  }
#if PM_DEBUG
  m_pDebugInfoWin = NULL;
#endif
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void Manager::Initialize( const Desc &desc )
{
  m_pHeap = desc.pHeap;
  m_pHeap_FileRead = desc.pHeap_FileRead;
  m_pDofHeap = desc.pHeap_Dof;
  m_pFinderMenuParam = desc.pFinderMenuParam;
  m_eState = State_Initialize;
  m_nStep = 0;
  m_nSpotID = desc.nSpotID;
  m_nSubjectID = desc.nSubjectID;
  m_TrialMonsNo = desc.nTrialMonsNo;
  m_bInitialized = false;

  // 時間帯取得
  GameSys::TimeZone::Zone eZone = GameSys::TimeZone::GetNowTimeZone();
  switch( eZone )
  {
  case GameSys::TimeZone::MORNING:   m_IsNoon = true;  break;
  case GameSys::TimeZone::NOON:      m_IsNoon = true;  break;
  case GameSys::TimeZone::EVENING:   m_IsNoon = true;  break;
  case GameSys::TimeZone::NIGHT:     m_IsNoon = false; break;
  case GameSys::TimeZone::MIDNIGHT:  m_IsNoon = false; break;
  }

#if PM_DEBUG
  m_pDebug = desc.pDebug;
  m_pFinderDebugInfo = GFL_NEW(m_pHeap) Field::Finder::DebugInfo( m_pHeap );
  m_pDebugInfoWin = desc.pDebugInfoWin;
  m_pDebugInfoWin->SetFinderInfo( m_pFinderDebugInfo );
#endif
}

/**
 *  @brief  初期化終了待ち
 */
bool Manager::IsInitialized( void )
{
  // 全初期化が完了した
  return m_bInitialized;
}

/**
 *  @brief  終了
 */
void Manager::Finalize( void )
{
  m_eState = State_Finalize;
  m_nStep = 0;
  m_bFinalized = false;
#if PM_DEBUG
  m_pDebugInfoWin->SetFinderInfo( NULL );
  GFL_SAFE_DELETE( m_pFinderDebugInfo );
  m_pDebug->m_pStatue = NULL;
#endif
}

/**
 *  @brief  終了完了待ち
 */
bool Manager::IsFinalized( void )
{
  // 全終了処理が完了した
  return m_bFinalized;
}

/**
 *  @brief  実行
 */
void Manager::Execute( void )
{
  m_eState = State_Execute;
  m_nStep = 0;

  if( m_pUIProc )
  {
    m_pUIProc->GetFinderFrame()->StartRotomFade();
  }
}

/**
 *  @brief  更新処理
 */
void Manager::Update( GameSys::GameManager *pGameManager )
{
  // ポケモン更新
  updatePokeModel();
  // メニュー更新
  updateMenu();
  // ぼけ更新
  updateFocus( pGameManager );
  // BGMボリューム更新
  updateBgmVolume( pGameManager );

#if PM_DEBUG
  if( m_pFinderDebugInfo )
  {
    if( !m_pFinderDebugInfo->IsEnablePokeRect() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawRect != 0 )
    {
      m_pFinderDebugInfo->SetEnablePokeRect( true );
    }
    if( m_pFinderDebugInfo->IsEnablePokeRect() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawRect == 0 )
    {
      m_pFinderDebugInfo->SetEnablePokeRect( false );
    }
    if( !m_pFinderDebugInfo->IsEnableWalkerA() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawWalkerA != 0 )
    {
      m_pFinderDebugInfo->SetEnableWalkerA( true );
    }
    if( m_pFinderDebugInfo->IsEnableWalkerA() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawWalkerA == 0 )
    {
      m_pFinderDebugInfo->SetEnableWalkerA( false );
    }
    if( !m_pFinderDebugInfo->IsEnableWalkerB() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawWalkerB != 0 )
    {
      m_pFinderDebugInfo->SetEnableWalkerB( true );
    }
    if( m_pFinderDebugInfo->IsEnableWalkerB() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawWalkerB == 0 )
    {
      m_pFinderDebugInfo->SetEnableWalkerB( false );
    }
    if( !m_pFinderDebugInfo->IsEnableStatue() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawStatue != 0 )
    {
      m_pFinderDebugInfo->SetEnableStatue( true );
    }
    if( m_pFinderDebugInfo->IsEnableStatue() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawStatue == 0 )
    {
      m_pFinderDebugInfo->SetEnableStatue( false );
    }
    if( !m_pFinderDebugInfo->IsEnableOneway() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawOneway != 0 )
    {
      m_pFinderDebugInfo->SetEnableOneway( true );
    }
    if( m_pFinderDebugInfo->IsEnableOneway() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawOneway == 0 )
    {
      m_pFinderDebugInfo->SetEnableOneway( false );
    }
    if( !m_pFinderDebugInfo->IsEnableScore() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawScore != 0 )
    {
        m_pFinderDebugInfo->SetEnableScore( true );
    }
    if( m_pFinderDebugInfo->IsEnableScore() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawScore == 0 )
    {
      m_pFinderDebugInfo->SetEnableScore( false );
    }
    if( Field::Debug::DebugTypes::s_DebugFinderParam.bSnapLock != 0 )
    {
      m_rUIParam.debug_shutter = true;
    }
    else
    {
      m_rUIParam.debug_shutter = false;
    }
    if( !m_pFinderDebugInfo->IsEnableComment() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawCom != 0 )
    {
        m_pFinderDebugInfo->SetEnableComment( true );
    }
    if( m_pFinderDebugInfo->IsEnableComment() && Field::Debug::DebugTypes::s_DebugFinderParam.bDrawCom == 0 )
    {
      m_pFinderDebugInfo->SetEnableComment( false );
    }
  }
#endif

  switch( m_eState )
  {
  case State_Wait:
    // 何もしない
    break;
  case State_Initialize:
    update_Initialize( pGameManager );
    break;
  case State_Execute:
    update_Execute( pGameManager );
    break;
  case State_Finalize:
    update_Finalize( pGameManager );
    break;
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化更新
 */
void Manager::update_Initialize( GameSys::GameManager *pGameManager )
{
  enum STEP
  {
    StepInit_OpenFile,
    StepInit_LoadFile,
    StepInit_CloseFile,
    StepInit_Kawaigari,
    StepInit_Kawaigari_Wait,
    StepInit_Data,
    StepInit_Savedata,
    StepInit_Device,
    StepInit_Camera,
    StepInit_UI,
    StepInit_UI_Wait,
    StepInit_PokeSys,
    StepInit_PokeModel,
    StepInit_SetupPokeModel,
    StepInit_Graphic,

    StepInit_End,
  };

  switch( m_nStep )
  {
  case StepInit_OpenFile:
    {
      openFile( pGameManager );
      ++ m_nStep;
    }
    break;
  case StepInit_LoadFile:
    if( isFinishOpenFile( pGameManager ) )
    {
      loadFile( pGameManager );
      ++ m_nStep;
    }
    break;
  case StepInit_CloseFile:
    if( isFinishLoadFile( pGameManager ) )
    {
      closeFile( pGameManager );
      ++ m_nStep;
    }
    break;
  case StepInit_Kawaigari:
    if( isFinishCloseFile( pGameManager ) )
    {
      createKawaigari( pGameManager );
      ++ m_nStep;
    }
    break;
  case StepInit_Kawaigari_Wait:
    if( isLoadedKawaigari( pGameManager ) )
    {
      ++ m_nStep;
    }
    break;
  case StepInit_Data:
    {
      createData( pGameManager );
      deleteKawaigari( pGameManager );
      ++ m_nStep;
    }
    break;
  case StepInit_Savedata:
    {
      setDataFromSavedata( pGameManager );
      ++ m_nStep;
    }
    // no break;
  case StepInit_Device:
    {
      m_pAllocator = GFL_NEW( m_pHeap ) System::nijiAllocator( m_pHeap );

#if defined(GF_PLATFORM_CTR)
      gfl2::ui::DeviceManager *pDeviceManager = pGameManager->GetUiDeviceManager();
      // 加速度センサーを有効化する
      pDeviceManager->BootAccelerometer( m_pHeap, gfl2::ui::DeviceManager::ACCELEROMETER_STANDARD );
      // ジャイロを有効化する
      pDeviceManager->BootGyroscope( m_pHeap, gfl2::ui::DeviceManager::GYROSCOPE_STANDARD );
      gfl2::ui::Gyroscope *pGyro = pDeviceManager->GetGyroscope( gfl2::ui::DeviceManager::GYROSCOPE_STANDARD );
      pGyro->EnableZeroDrift();
      pGyro->EnableZeroPlayParam();
      pGyro->EnableRevisionByAcceleration();
#endif

      ++ m_nStep;
    }
    break;
  case StepInit_Camera:
    {
      // カメラの切り替え
      Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
      m_pCameraManager = pFieldmap->GetCameraManager();
      // イベントモードに切り替え
      m_pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::EVENT );
      //ポケファインダーカメラ作成
      Camera::Controller::ControllerTypePokeFinderForPlacement::SetupParam prm;
      prm.position.Set( m_rSpotData.position[0], m_rSpotData.position[1], m_rSpotData.position[2] );
      prm.rotate.Set( m_rSpotData.rotation[0], m_rSpotData.rotation[1], m_rSpotData.rotation[2] );
      prm.limit_min.Set( m_rSpotData.limit[0], m_rSpotData.limit[2], 0.0f );
      prm.limit_max.Set( m_rSpotData.limit[1], m_rSpotData.limit[3], 0.0f );
      prm.zoom_maxi = m_rUIParam.zoom_max;
      m_pCamera = GFL_NEW( m_pHeap ) Camera::Controller::ControllerTypePokeFinderForPlacement( prm );
      // カメラを切り替え
      Camera::CameraUnit *pUnit = m_pCameraManager->GetMainGamePlayCamera();
      pUnit->ChangeController( m_pCamera );

#if PM_DEBUG
      m_pDebug->m_pCamera = m_pCamera;
#endif
      ++ m_nStep;
    }
    // no break;
  case StepInit_UI:
    {
      createColorBuffer();
      createMenu( pGameManager );

      ++ m_nStep;
    }
    // no break;
  case StepInit_UI_Wait:
    if( isCreateMenu( pGameManager ) )
    {
      ++ m_nStep;
    }
    break;
  case StepInit_PokeSys:
    {
      // システムの生成
      createPokeSystem( pGameManager );
      ++ m_nStep;
    }
    break;
  case StepInit_PokeModel:
    {
      if( !m_pModelFactory->IsInitialize() )
      {
        // システム生成待ち
        break;
      }

      createPokeModel( pGameManager );

      ++ m_nStep;
    }
    break;
  case StepInit_SetupPokeModel:
    {
      if( !isCreatePokeModel() )
      {
        // 作成待ち
        break;
      }

      setupPokeModel();

      ++ m_nStep;
    }
    break;
  case StepInit_Graphic:
    {
      startGraphic( pGameManager );
      pGameManager->GetGameData()->GetFieldSound()->ChangeBGMVolume( MAX_BGM_VOLUME, Sound::BGM_FADE_FAST ); // @fix GFNMCat[1458]BGMボリュームを初期化
      ++ m_nStep;
    }
    break;
  case StepInit_End:
    {
      m_bInitialized = true;
    }
    break;
  }
}

/**
 *  @brief  更新
 */
void Manager::update_Execute( GameSys::GameManager *pGameManager )
{
  enum StepExec
  {
    StepExec_Wait,
    StepExec_SafeSnapshot,
    StepExec_Snapshot_Wait,
    StepExec_WaitGPU_1,
    StepExec_WaitGPU_2,
    StepExec_CreateJpeg,
    StepExec_CheckEnd,
  };

  switch( m_nStep )
  {
  case StepExec_Wait:
#if PM_DEBUG
    if( Field::Debug::DebugTypes::s_DebugFinderParam.bDrawScore != 0 )
    {
      calcValue( pGameManager );
    }
    if( Field::Debug::DebugTypes::s_DebugFinderParam.nForceStop != 0 )
    {
      m_eResult = EndResult_Cancel;
    }
    else
#endif
    if( m_rUIParam.shutter_close )
    {
      setupSafeSnapshot();
      m_IsSnapshot = true;
      m_nStep = StepExec_SafeSnapshot;
    }
    else if( m_rUIParam.endMode == App::Finder::END_MODE_CANCEL )
    {
      m_eResult = EndResult_Cancel;
    }
    else if( m_rUIParam.endMode == App::Finder::END_MODE_FIN_SHOOT )
    {
      m_eResult = EndResult_Over;
    }
    break;
  case StepExec_SafeSnapshot:
    if( isSafeSnapshot() )
    {
      requestSnapshot( pGameManager );
      calcValue( pGameManager );
      ++ m_nStep;
    }
    break;
  case StepExec_Snapshot_Wait:
    if( isFinishSnapshot() )
    {
      ++ m_nStep;
    }
    break;
  case StepExec_WaitGPU_1:
    {
      ++ m_nStep;
    }
    break;
  case StepExec_WaitGPU_2:
    {
      ++ m_nStep;
    }
    break;
  case StepExec_CreateJpeg:
    {
      createJpeg( pGameManager );
      ++ m_nStep;
    }
    break;
  case StepExec_CheckEnd:
#if PM_DEBUG
    bool bExec = true;
    if( m_rUIParam.debug_shutter )
    {
      bExec = false;
      gfl2::ui::DeviceManager *pDeviceManager = pGameManager->GetUiDeviceManager();
      gfl2::ui::Button *pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
      if( pButton->IsTrigger( gfl2::ui::BUTTON_R ) )
      {
        bExec = true;
      }
    }
    if( bExec )
#endif
    {
      // 終了条件をチェックする
      checkEnd();
      if( m_TrialMonsNo != MONSNO_NULL && EndResult_Trial == m_eResult )
      {
        // @fix NMCat[2259]試練モードで撮影に成功したならシャッターを開けない
        break;
      }
      // シャッターを開ける
      m_rUIParam.shutter_open = true;
      if( m_nSnapCount < Field::TrialModel::FINDER_PHOTO_MAX )
      {
        // 操作再開
        m_IsSnapshot = false;
      }
      // 再開する
      requestResume();
      m_nStep = StepExec_Wait;
    }
    break;
  }
}

/**
 *  @brief  終了更新
 */
void Manager::update_Finalize( GameSys::GameManager *pGameManager )
{
  enum StepFin
  {
    StepFin_Graphic1,
    StepFin_Graphic2,
    StepFin_PokeModel_Request,
    StepFin_PokeModel,
    StepFin_PokeSys,
    StepFin_PokeSys_Wait,
    StepFin_UI,
    StepFin_UI_Wait,
    StepFin_Camera,
    StepFin_Device,
    StepFin_Savedata,
    StepFin_DeleteFile,

    StepFin_End,
  };

  switch( m_nStep )
  {
  case StepFin_Graphic1:
    {
      resetBgmVolume( pGameManager );
      endGraphic1( pGameManager );
      ++ m_nStep;
    }
    break;
  case StepFin_Graphic2:
    {
      endGraphic2( pGameManager );
      ++ m_nStep;
    }
    break;
  case StepFin_PokeModel_Request:
    {
      requestDeletePokeModel();
      ++ m_nStep;
    }
    break;
  case StepFin_PokeModel:
    if( isSafeDeletePokeModel() )
    {
      deletePokeModel();
      ++ m_nStep;
    }
    break;
  case StepFin_PokeSys:
    {
      deletePokeSystem();
      ++ m_nStep;
    }
    break;
  case StepFin_PokeSys_Wait:
    if( m_pModelFactory->IsDelete() )
    {
      m_pModelFactory->FinalizeSystem();
      ++ m_nStep;
    }
    break;
  case StepFin_UI:
    {
      deleteMenu( pGameManager );
      ++ m_nStep;
    }
    // no break;
  case StepFin_UI_Wait:
    if( isDeleteMenu( pGameManager ) )
    {
      deleteColorBuffer();
      ++ m_nStep;
    }
    break;
  case StepFin_Camera:
    {
#if PM_DEBUG
      if( m_pDebug )
      {
        m_pDebug->m_pCamera = NULL;
      }
#endif
      // 操作を元に戻す
      Camera::CameraUnit *pUnit = m_pCameraManager->GetMainGamePlayCamera();
      pUnit->ChangeController( NULL );
      // 通常モードに戻す
      m_pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::FIELD );
      m_pCamera = NULL;

      ++ m_nStep;
    }
    break;
  case StepFin_Device:
    {
#if defined(GF_PLATFORM_CTR)
      gfl2::ui::DeviceManager *pDeviceManager = pGameManager->GetUiDeviceManager();
      // 加速度センサーを無効化する
      pDeviceManager->StopAccelerometer( gfl2::ui::DeviceManager::ACCELEROMETER_STANDARD );
      // ジャイロを無効化する
      pDeviceManager->StopGyroscope( gfl2::ui::DeviceManager::GYROSCOPE_STANDARD );
#endif

      GFL_SAFE_DELETE( m_pAllocator );

      ++ m_nStep;
    }
    break;
  case StepFin_Savedata:
    {
      Savedata::PokeFinderSave *pSave = pGameManager->GetGameData()->GetPokeFinderSave();
      pSave->AddSnapCount( m_nSnapCount );
      pSave->SetGyroFlag( m_rUIParam.gyro_flg );
      pSave->SetSpotFlags( m_nSpotID, Savedata::PokeFinderSave::FINDERSPOT_FLAG_KNOWN, true );

      ++ m_nStep;
    }
    break;
  case StepFin_DeleteFile:
    {
      deleteFile();
      ++ m_nStep;
    }
    break;
  case StepFin_End:
    {
       m_bFinalized = true;
    }
    break;
  }
}

/**
 *  @brief  garcOpen
 */
void Manager::openFile( GameSys::GameManager *pGameManager )
{
  gfl2::fs::AsyncFileManager *pFileManager = pGameManager->GetAsyncFileManager();
  {
    // 設定
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openr;
    openr.arcId       = ARCID_POKE_FINDER_PARAM;
    openr.heapForFile = m_pHeap->GetLowerHandle();
    openr.heapForReq  = m_pHeap->GetLowerHandle();
    // リクエスト
    pFileManager->AddArcFileOpenReq( openr );
  }
  {
    // 設定
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openr;
    openr.arcId       = ARCID_POKEFINDER_SUBJECT;
    openr.heapForFile = m_pHeap->GetLowerHandle();
    openr.heapForReq  = m_pHeap->GetLowerHandle();
    // リクエスト
    pFileManager->AddArcFileOpenReq( openr );
  }
#if ENABLE_RENDER_DOF
  {
    // 設定
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openr;
    openr.arcId       = ARCID_RENDERPATH;
    openr.heapForFile = m_pDofHeap->GetLowerHandle();
    openr.heapForReq  = m_pDofHeap->GetLowerHandle();
    // リクエスト
    pFileManager->AddArcFileOpenReq( openr );
  }
#endif
}

/**
 *  @brief  garcOpenWait
 */
bool Manager::isFinishOpenFile( GameSys::GameManager *pGameManager )
{
  gfl2::fs::AsyncFileManager *pFileManager = pGameManager->GetAsyncFileManager();
  // Openをまつ
  if( !pFileManager->IsArcFileOpenFinished( ARCID_POKE_FINDER_PARAM ) )
  {
    return false;
  }
  if( !pFileManager->IsArcFileOpenFinished( ARCID_POKEFINDER_SUBJECT ) )
  {
    return false;
  }
#if ENABLE_RENDER_DOF
  if( !pFileManager->IsArcFileOpenFinished( ARCID_RENDERPATH ) )
  {
    return false;
  }
#endif
  return true;
}

/**
 *  @brief  garcLoad
 */
void Manager::loadFile( GameSys::GameManager *pGameManager )
{
  gfl2::fs::AsyncFileManager *pFileManager = pGameManager->GetAsyncFileManager();
  // キョロキョロ
  {
    // 設定
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadr;
    loadr.arcId             = ARCID_POKE_FINDER_PARAM;
    loadr.datId             = GARC_poke_finder_param_interest_BIN;
    loadr.ppBuf             = &m_pPokeData;
    loadr.heapForBuf        = m_pHeap;
    loadr.heapForReq        = m_pHeap->GetLowerHandle();
    loadr.heapForCompressed = NULL;
    // リクエスト
    pFileManager->AddArcFileLoadDataReq( loadr );
  }
  // Subject
  // BTS6323 対応 ポケモンが登場しない場所対応
  if( m_nSubjectID >= 0 )
  {
    u32 offset = m_IsNoon? 0: 1;
    // 設定
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadr;
    loadr.arcId             = ARCID_POKEFINDER_SUBJECT;
    loadr.datId             = m_nSubjectID * 2 + offset;
    loadr.ppBuf             = &m_pSubjectData;
    loadr.heapForBuf        = m_pHeap;
    loadr.heapForReq        = m_pHeap->GetLowerHandle();
    loadr.heapForCompressed = NULL;
    // リクエスト
    pFileManager->AddArcFileLoadDataReq( loadr );
  }
  // コメント条件
  {
    // 設定
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadr;
    loadr.arcId             = ARCID_POKE_FINDER_PARAM;
    loadr.datId             = GARC_poke_finder_param_situation_single_BIN;
    loadr.ppBuf             = &m_pSituationSingle;
    loadr.heapForBuf        = m_pHeap;
    loadr.heapForReq        = m_pHeap->GetLowerHandle();
    loadr.heapForCompressed = NULL;
    // リクエスト
    pFileManager->AddArcFileLoadDataReq( loadr );
  }
  {
    // 設定
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadr;
    loadr.arcId             = ARCID_POKE_FINDER_PARAM;
    loadr.datId             = GARC_poke_finder_param_situation_double_BIN;
    loadr.ppBuf             = &m_pSituationDouble;
    loadr.heapForBuf        = m_pHeap;
    loadr.heapForReq        = m_pHeap->GetLowerHandle();
    loadr.heapForCompressed = NULL;
    // リクエスト
    pFileManager->AddArcFileLoadDataReq( loadr );
  }
  {
    // 設定
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadr;
    loadr.arcId             = ARCID_POKE_FINDER_PARAM;
    loadr.datId             = GARC_poke_finder_param_situation_other_BIN;
    loadr.ppBuf             = &m_pSituationOther;
    loadr.heapForBuf        = m_pHeap;
    loadr.heapForReq        = m_pHeap->GetLowerHandle();
    loadr.heapForCompressed = NULL;
    // リクエスト
    pFileManager->AddArcFileLoadDataReq( loadr );
  }
#if ENABLE_RENDER_DOF
  // renderpath
  {
    // 設定
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadr;
    loadr.arcId             = ARCID_RENDERPATH;
    loadr.datId             = GARC_NijiRenderPath_DepthOfFieldResource_BIN;
    loadr.ppBuf             = &m_pRenderPath;
    loadr.align             = 128;
    loadr.heapForBuf        = m_pDofHeap;
    loadr.heapForReq        = m_pDofHeap->GetLowerHandle();
    loadr.heapForCompressed = NULL;
    // リクエスト
    pFileManager->AddArcFileLoadDataReq( loadr );
  }
#endif
}

/**
 *  @brief  garcLoadWait
 */
bool Manager::isFinishLoadFile( GameSys::GameManager *pGameManager )
{
  gfl2::fs::AsyncFileManager *pFileManager = pGameManager->GetAsyncFileManager();
  // loadをまつ
  if( !pFileManager->IsArcFileLoadDataFinished( &m_pPokeData ) )
  {
    return false;
  }
  if( !pFileManager->IsArcFileLoadDataFinished( &m_pSubjectData ) )
  {
    return false;
  }
  if( !pFileManager->IsArcFileLoadDataFinished( &m_pSituationSingle ) )
  {
    return false;
  }
  if( !pFileManager->IsArcFileLoadDataFinished( &m_pSituationDouble ) )
  {
    return false;
  }
  if( !pFileManager->IsArcFileLoadDataFinished( &m_pSituationOther ) )
  {
    return false;
  }
#if ENABLE_RENDER_DOF
  if( !pFileManager->IsArcFileLoadDataFinished( &m_pRenderPath ) )
  {
    return false;
  }
#endif
  return true;
}

/**
 *  @brief  garcClose
 */
void Manager::closeFile( GameSys::GameManager *pGameManager )
{
  gfl2::fs::AsyncFileManager *pFileManager = pGameManager->GetAsyncFileManager();
  {
    // 設定
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closr;
    closr.arcId       = ARCID_POKE_FINDER_PARAM;
    closr.heapForReq  = m_pHeap->GetLowerHandle();
    // リクエスト
    pFileManager->AddArcFileCloseReq( closr );
  }
  {
    // 設定
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closr;
    closr.arcId       = ARCID_POKEFINDER_SUBJECT;
    closr.heapForReq  = m_pHeap->GetLowerHandle();
    // リクエスト
    pFileManager->AddArcFileCloseReq( closr );
  }
#if ENABLE_RENDER_DOF
  {
    // 設定
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closr;
    closr.arcId       = ARCID_RENDERPATH;
    closr.heapForReq  = m_pDofHeap->GetLowerHandle();
    // リクエスト
    pFileManager->AddArcFileCloseReq( closr );
  }
#endif
}

/**
 *  @brief  garcCloseWait
 */
bool Manager::isFinishCloseFile( GameSys::GameManager *pGameManager )
{
  gfl2::fs::AsyncFileManager *pFileManager = pGameManager->GetAsyncFileManager();
  // closeをまつ
  if( !pFileManager->IsArcFileCloseFinished( ARCID_POKE_FINDER_PARAM ) )
  {
    return false;
  }
  if( !pFileManager->IsArcFileCloseFinished( ARCID_POKEFINDER_SUBJECT ) )
  {
    return false;
  }
#if ENABLE_RENDER_DOF
  if( !pFileManager->IsArcFileCloseFinished( ARCID_RENDERPATH ) )
  {
    return false;
  }
#endif
  return true;
}

/**
 *  @brief  かわいがリデータ作成
 */
void Manager::createKawaigari( GameSys::GameManager *pGameManager )
{
  if( m_pKawaigari == NULL )
  {
    gfl2::fs::AsyncFileManager *pFileManager = pGameManager->GetAsyncFileManager();
    m_pKawaigari = GFL_NEW(m_pHeap)PokeTool::KawaigariParamOptionCoreManager( m_pHeap, m_pHeap->GetLowerHandle(), pFileManager );
  }
}

/**
 *  @brief  カワイガリデータ作成待ち
 */
bool Manager::isLoadedKawaigari( GameSys::GameManager *pGameManager )
{
  if( m_pKawaigari != NULL )
  {
    if( !m_pKawaigari->IsLoaded() )
    {
      return false;
    }
  }
  return true;
}

/**
 *  @brief  データ作成
 */
void Manager::createData( GameSys::GameManager *pGameManager )
{
  Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
  m_pModelFactory = pFieldmap->GetTrialModelFactory();
  m_pWeather = pFieldmap->GetWeatherControl();
#if PM_DEBUG
  if( m_nSpotID == 0xFFFFFFFF )
  {
    // デバッグモードにて起動
    m_rSpotData.position[0] = m_pDebug->m_pos.x;
    m_rSpotData.position[1] = m_pDebug->m_pos.y;
    m_rSpotData.position[2] = m_pDebug->m_pos.z;
    m_rSpotData.rotation[0] = 0.0f;
    m_rSpotData.rotation[1] = gfl2::math::ConvDegToRad( m_pDebug->m_rot );
    m_rSpotData.limit[0]    = gfl2::math::ConvDegToRad( -180.0f );
    m_rSpotData.limit[1]    = gfl2::math::ConvDegToRad(  180.0f );
    m_rSpotData.limit[2]    = gfl2::math::ConvDegToRad( -180.0f );
    m_rSpotData.limit[3]    = gfl2::math::ConvDegToRad(  180.0f );
    m_rSpotData.spot_score  = 50;

    m_rStatueData.num = 1;
    m_rStatueData.data[0].position = m_pDebug->m_poke_pos;
    m_rStatueData.data[0].cool_min = 30;
    m_rStatueData.data[0].cool_max = 30;

    m_IsDebugMode = true;
  }
  else
#endif
  {
    // 通常起動
    Field::FieldResident *pResident = pGameManager->GetFieldResident();
    Field::PlacementDataManager *pPlacementDataManager = pResident->GetPlacementDataManager();
    Field::FieldPokeFinderActor *pActor = pPlacementDataManager->SearchZonePokeFinder( m_nSpotID );
    GFL_ASSERT_MSG( pActor, "FinderActor is NULL !!\n" );
    if( pActor )
    {
      // 配置設定
      const FieldPokeFinderAccessor::PokeFinderData *pPlaceData = pActor->GetData();
      if( pPlaceData )
      {
        // カメラ座標
        m_rSpotData.position[0] = pPlaceData->cam_pos[0];
        m_rSpotData.position[1] = pPlaceData->cam_pos[1];
        m_rSpotData.position[2] = pPlaceData->cam_pos[2];
        // カメラ方向
        gfl2::math::Vector3 dst_vec = gfl2::math::Vector3::GetZUnit();
        gfl2::math::Matrix34 mtx;
        gfl2::math::Quaternion quat( pPlaceData->cam_qua[0], pPlaceData->cam_qua[1], pPlaceData->cam_qua[2], pPlaceData->cam_qua[3] );
        quat.QuaternionToMatrix( mtx );
        dst_vec = mtx * dst_vec;
        gfl2::math::Vector3 xz_dir( dst_vec.x, 0.0f, dst_vec.z );
        xz_dir = xz_dir.Normalize();

        m_rSpotData.rotation[1] = atan2( xz_dir.x, xz_dir.z );

        mtx = gfl2::math::Matrix34::GetRotationY( - m_rSpotData.rotation[1] ) * mtx;
        dst_vec = mtx * gfl2::math::Vector3::GetZUnit();

        gfl2::math::Vector3 yz_dir( 0.0f, dst_vec.y, dst_vec.z );
        yz_dir = yz_dir.Normalize();
        m_rSpotData.rotation[0] = atan2( yz_dir.y * -1.0f, yz_dir.z );
        m_rSpotData.rotation[2] = 0.0f;
        // 限界角
        m_rSpotData.limit[0] = gfl2::math::ConvDegToRad( pPlaceData->cam_lim_x_min );
        m_rSpotData.limit[1] = gfl2::math::ConvDegToRad( pPlaceData->cam_lim_x_max );
        m_rSpotData.limit[2] = gfl2::math::ConvDegToRad( pPlaceData->cam_lim_y_min );
        m_rSpotData.limit[3] = gfl2::math::ConvDegToRad( pPlaceData->cam_lim_y_max );
        // 基礎得点
        m_rSpotData.spot_score = pPlaceData->base_score;
        // 属性
        m_rSpotData.spot_type = pPlaceData->place_type;
#if ENABLE_RENDER_DOF
        // 被写界深度初期値
        m_rSpotData.base_dof = pPlaceData->start_focus;
        m_rUIParam.pint_type = pPlaceData->start_focus;
        m_Focus_Old          = pPlaceData->start_focus;
#endif
      }
      // 巡回A
      const void *pMoveAData = pActor->GetMoveAData();
      if( pMoveAData )
      {
        m_HasPathA = true;
        uptr addr = reinterpret_cast<uptr>( pMoveAData );
        u16* point_num  = reinterpret_cast<u16*>(addr); addr += sizeof(u16);
        u16* appeal_num = reinterpret_cast<u16*>(addr); addr += sizeof(u16);
        m_rPathData[Field::TrialModel::FINDER_PATH_A].pos_num = *point_num;
        for( u32 idx = 0; idx < *point_num; ++ idx )
        {
          f32* x = reinterpret_cast<f32*>(addr);  addr += sizeof(f32);
          f32* y = reinterpret_cast<f32*>(addr);  addr += sizeof(f32);
          f32* z = reinterpret_cast<f32*>(addr);  addr += sizeof(f32);
          addr += sizeof(u32); // @padding

          m_rPathData[Field::TrialModel::FINDER_PATH_A].route[idx].pos.x = *x;
          m_rPathData[Field::TrialModel::FINDER_PATH_A].route[idx].pos.y = *y;
          m_rPathData[Field::TrialModel::FINDER_PATH_A].route[idx].pos.z = *z;
          for( u32 next_idx = 0; next_idx < 8; ++ next_idx )
          {
            s8*  next = reinterpret_cast<s8*>(addr); addr += sizeof(s8);
            if( next_idx < Field::TrialModel::FINDER_CONECTION_MAX && *next != 0xFF )
            {
              m_rPathData[Field::TrialModel::FINDER_PATH_A].route[idx].next_idx[next_idx] = *next;
            }
          }
        }
        for( u32 idx = 0; idx < *appeal_num; ++ idx )
        {
          u8*  start = reinterpret_cast<u8*>(addr);  addr += sizeof(u8);
          u8*  end   = reinterpret_cast<u8*>(addr);  addr += sizeof(u8);
          u16* rate  = reinterpret_cast<u16*>(addr); addr += sizeof(u16);
          f32* prog  = reinterpret_cast<f32*>(addr); addr += sizeof(f32);

          m_rPathData[Field::TrialModel::FINDER_PATH_A].appeal[idx].start    = *start;
          m_rPathData[Field::TrialModel::FINDER_PATH_A].appeal[idx].end      = *end;
          m_rPathData[Field::TrialModel::FINDER_PATH_A].appeal[idx].ratio    = *rate;
          m_rPathData[Field::TrialModel::FINDER_PATH_A].appeal[idx].progress = *prog;
        }

#if PM_DEBUG
        m_pFinderDebugInfo->SetWalkerDataA( &m_rPathData[Field::TrialModel::FINDER_PATH_A] );
        m_pDebug->m_pWalkerA = &m_rPathData[Field::TrialModel::FINDER_PATH_A];
#endif
      }
      // 巡回B
      const void *pMoveBData = pActor->GetMoveBData();
      if( pMoveBData )
      {
        m_HasPathB = true;
        uptr addr = reinterpret_cast<uptr>( pMoveBData );
        u16* point_num  = reinterpret_cast<u16*>(addr); addr += sizeof(u16);
        u16* appeal_num = reinterpret_cast<u16*>(addr); addr += sizeof(u16);
        m_rPathData[Field::TrialModel::FINDER_PATH_B].pos_num = *point_num;
        for( u32 idx = 0; idx < *point_num; ++ idx )
        {
          f32* x = reinterpret_cast<f32*>(addr);  addr += sizeof(f32);
          f32* y = reinterpret_cast<f32*>(addr);  addr += sizeof(f32);
          f32* z = reinterpret_cast<f32*>(addr);  addr += sizeof(f32);
          addr += sizeof(u32); // @padding

          m_rPathData[Field::TrialModel::FINDER_PATH_B].route[idx].pos.x = *x;
          m_rPathData[Field::TrialModel::FINDER_PATH_B].route[idx].pos.y = *y;
          m_rPathData[Field::TrialModel::FINDER_PATH_B].route[idx].pos.z = *z;
          for( u32 next_idx = 0; next_idx < 8; ++ next_idx )
          {
            s8*  next = reinterpret_cast<s8*>(addr); addr += sizeof(s8);
            if( next_idx < Field::TrialModel::FINDER_CONECTION_MAX && *next != 0xFF )
            {
              m_rPathData[Field::TrialModel::FINDER_PATH_B].route[idx].next_idx[next_idx] = *next;
            }
          }
        }
        for( u32 idx = 0; idx < *appeal_num; ++ idx )
        {
          u8*  start = reinterpret_cast<u8*>(addr);  addr += sizeof(u8);
          u8*  end   = reinterpret_cast<u8*>(addr);  addr += sizeof(u8);
          u16* rate  = reinterpret_cast<u16*>(addr); addr += sizeof(u16);
          f32* prog  = reinterpret_cast<f32*>(addr); addr += sizeof(f32);

          m_rPathData[Field::TrialModel::FINDER_PATH_B].appeal[idx].start    = *start;
          m_rPathData[Field::TrialModel::FINDER_PATH_B].appeal[idx].end      = *end;
          m_rPathData[Field::TrialModel::FINDER_PATH_B].appeal[idx].ratio    = *rate;
          m_rPathData[Field::TrialModel::FINDER_PATH_B].appeal[idx].progress = *prog;
        }
#if PM_DEBUG
        m_pFinderDebugInfo->SetWalkerDataB( &m_rPathData[Field::TrialModel::FINDER_PATH_B] );
        m_pDebug->m_pWalkerB = &m_rPathData[Field::TrialModel::FINDER_PATH_B];
#endif
      }
      // 待機
      const void *pWaitData = pActor->GetWaitData();
      if( pWaitData )
      {
        m_HasStatue = true;
        uptr addr = reinterpret_cast<uptr>( pWaitData );
        m_rStatueData.num = *(reinterpret_cast<u32*>( addr ));    addr += sizeof(u32);
        for( u32 idx = 0; idx < m_rStatueData.num; ++ idx )
        {
          f32* x        = reinterpret_cast<f32*>(addr);   addr += sizeof(f32);
          f32* y        = reinterpret_cast<f32*>(addr);   addr += sizeof(f32);
          f32* z        = reinterpret_cast<f32*>(addr);   addr += sizeof(f32);
          f32* rot      = reinterpret_cast<f32*>(addr);   addr += sizeof(f32);
          u16* cool_min = reinterpret_cast<u16*>(addr);   addr += sizeof(u16);
          u16* cool_max = reinterpret_cast<u16*>(addr);   addr += sizeof(u16);

          m_rStatueData.data[idx].position.x = *x;
          m_rStatueData.data[idx].position.y = *y;
          m_rStatueData.data[idx].position.z = *z;
          m_rStatueData.data[idx].rotation   = gfl2::math::ConvDegToRad(*rot);
          m_rStatueData.data[idx].cool_min   = *cool_min;
          m_rStatueData.data[idx].cool_max   = *cool_max;
        }
#if PM_DEBUG
        m_pFinderDebugInfo->SetStatueData( &m_rStatueData );
        m_pDebug->m_pStatue = &m_rStatueData;
#endif
      }
      // 一方通行
      const void *pOnewayData = pActor->GetOnewayData();
      if( pOnewayData )
      {
        m_HasOneway = true;
        const Field::Traffic::PathCoreData *pCoreData = reinterpret_cast<const Field::Traffic::PathCoreData*>( pOnewayData );
        m_rOnewayData.num = pCoreData->num;
#if PM_DEBUG
        if( m_pDebug && m_pDebug->m_overwrite >= 1 )
        {
          m_rOnewayData.num = m_pDebug->m_overwrite;
        }
#endif
        for( u32 idx = 0; idx < m_rOnewayData.num; ++ idx )
        {
#if PM_DEBUG
          if( m_pDebug && m_pDebug->m_overwrite >= 1 )
          {
            m_rOnewayData.position[idx] = m_pDebug->m_rare[idx];
          }
          else
#endif
          {
            u32 pos_idx = idx * 3;
            m_rOnewayData.position[idx].x = pCoreData->position[ pos_idx + 0 ];
            m_rOnewayData.position[idx].y = pCoreData->position[ pos_idx + 1 ];
            m_rOnewayData.position[idx].z = pCoreData->position[ pos_idx + 2 ];
          }
        }
#if PM_DEBUG
        m_pFinderDebugInfo->SetOnewayData( &m_rOnewayData );
        m_pDebug->m_pOneway = &m_rOnewayData;
        if( pPlaceData )
        {
          gfl2::math::Vector3 spot_pos( pPlaceData->position[0], pPlaceData->position[1], pPlaceData->position[2] );
          gfl2::math::Quaternion spot_qua( pPlaceData->quaternion[0], pPlaceData->quaternion[1], pPlaceData->quaternion[2], pPlaceData->quaternion[3] );
          gfl2::math::Matrix34 mt;
          gfl2::math::Matrix34 mr;
          spot_qua.QuaternionToMatrix( mr );
          mt = gfl2::math::Matrix34::GetTranslation( spot_pos );
          mr *= mt;
          m_pDebug->m_spotMtx = mr;
        }
#endif
      }
    }
  }

#if PM_DEBUG
  if( m_IsDebugMode )
  {
    setupSpotModelData( 0, 0 );
  }
  else
#endif
  if( m_nSubjectID >= 0 )
  {
    Field::TrialModel::FinderSpotReserveData *pReserveData = pGameManager->GetGameData()->GetFinderSpotReserveData( m_nSpotID );

    // 予約の確認
    bool bReserved = checkSpotReserve( pGameManager );
#if PM_DEBUG
    if( Field::Debug::DebugTypes::s_DebugFinderParam.bLotteryLock != 0 )
    {
      // 強制中なので予約されている風を装う
      bReserved = true;
    }
    {
      // 最大数を設置
      FinderSubjectDataAccessor sub_accessor( m_pSubjectData );
      Field::Debug::DebugTypes::s_DebugFinderParam.nMaxPokeIdx = sub_accessor.GetMaxData();
    }
#endif
    if( !bReserved )
    {
      // 予約がないので抽選する
      execLottery( pGameManager );
    }

    // 予約ポケモンでの生成
    for( u32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
    {
      s32 monsID = pReserveData->monsIdx[i];
#if PM_DEBUG
    if( Field::Debug::DebugTypes::s_DebugFinderParam.bLotteryLock != 0 )
    {
      if( i == 0 )
      {
        monsID = Field::Debug::DebugTypes::s_DebugFinderParam.nPokeIdx;
      }
      else
      {
        monsID = Field::Debug::DebugTypes::s_DebugFinderParam.nPokeIdx2;
      }
    }
#endif
      setupSpotModelData( i, monsID );
    }
  }
  // 試練モードの受け渡し
  m_rUIParam.trial_mode = m_TrialMonsNo != MONSNO_NULL;
}


/**
 *  @brief  ロック判定
 */
bool Manager::checkSpotReserve( GameSys::GameManager *pGameManager )
{
  Savedata::PokeFinderSave *pSave = pGameManager->GetGameData()->GetPokeFinderSave();
  Field::TrialModel::FinderSpotReserveData *pReserveData = pGameManager->GetGameData()->GetFinderSpotReserveData( m_nSpotID );

  if( ( pSave->GetSpotFlags( m_nSpotID ) & Savedata::PokeFinderSave::FINDERSPOT_FLAG_KNOWN ) == 0 )
  {
    // 未知のスポットに予約はできない
    return false;
  }
  if( !pReserveData->bReserve )
  {
    return false;
  }

  GameSys::DeviceDate date_now;
  bool bResult = true;
  if( date_now.GetYear() > pReserveData->year )
  {
    // 年単位で経過している
    bResult = false;
  }
  else if( date_now.GetMonth() > pReserveData->month )
  {
    // 月単位で経過している
    bResult = false;
  }
  else if( date_now.GetDay() > pReserveData->day )
  {
    // 日単位で経過している
    bResult = false;
  }
  else
  {
    // 現在時間を秒へ変換
    s32 sec_now  = date_now.GetSecond();
    sec_now += date_now.GetMinute() * CONV_MINUTE_TO_SEC;
    sec_now += date_now.GetHour() * CONV_HOUR_TO_SEC;
    s32 sec_spot = pReserveData->second;
    sec_spot += pReserveData->minute * CONV_MINUTE_TO_SEC;
    sec_spot += pReserveData->hour * CONV_HOUR_TO_SEC;
    s32 diff = sec_now - sec_spot;
    if( CONV_MINUTE_TO_SEC <= diff )
    {
      // 1分以上経過している
      bResult = false;
    }
    else if( diff < 0 )
    {
      // 過去にさかのぼっている
      bResult = false;
    }
  }

  // 以前の時間帯を取得する
  GameSys::TimeZone::Zone eZone_spot = static_cast<GameSys::TimeZone::Zone>(pReserveData->timezone);
  bool isNoon_spot = true;
  switch( eZone_spot )
  {
  case GameSys::TimeZone::MORNING:   isNoon_spot = true;  break;
  case GameSys::TimeZone::NOON:      isNoon_spot = true;  break;
  case GameSys::TimeZone::EVENING:   isNoon_spot = true;  break;
  case GameSys::TimeZone::NIGHT:     isNoon_spot = false; break;
  case GameSys::TimeZone::MIDNIGHT:  isNoon_spot = false; break;
  }
  if( isNoon_spot && !m_IsNoon )
  {
    // 予約は昼で今は夜
      bResult = false;
  }
  if( !isNoon_spot && m_IsNoon )
  {
    // 予約は夜で今は昼
      bResult = false;
  }

  return bResult;
}

/**
 *  @brief  配列番号からデータを習得する
 */
bool Manager::setupSpotModelData( u32 instance_index, s32 data_index )
{
  bool bResult = false;

#if PM_DEBUG
  if( m_IsDebugMode )
  {
    m_rModelData[ 0 ].monsNo          = static_cast<MonsNo>(m_pDebug->m_mons);
    m_rModelData[ 0 ].formNo          = m_pDebug->m_form;
    m_rModelData[ 0 ].sex             = static_cast<pml::Sex>(m_pDebug->m_sex);
    m_rModelData[ 0 ].animNo[ 0 ]     = m_pDebug->m_appeal01;
    m_rModelData[ 0 ].animNo[ 1 ]     = m_pDebug->m_appeal02;
    m_rModelData[ 0 ].animNo[ 2 ]     = m_pDebug->m_appeal03;
    m_rModelData[ 0 ].animNo[ 3 ]     = m_pDebug->m_appeal04;
    m_rModelData[ 0 ].animNo[ 4 ]     = m_pDebug->m_appeal05;
    m_rModelData[ 0 ].ext_prm2        = 1;
    m_rModelData[ 0 ].pathUse         = Field::TrialModel::FINDER_PATH_USE_WAIT;
    return bResult;
  }
#endif

  // データから作成する
  FinderSubjectDataAccessor sub_accessor( m_pSubjectData );
  const FinderSubjectDataAccessor::FinderModelData *pModelData = sub_accessor.GetModelData( data_index );
  if( pModelData )
  {
    // 性別専用処理
    pml::Sex sex = pml::SEX_MALE;
    switch( pModelData->sex )
    {
    case 0: sex = pml::SEX_MALE;    break;
    case 1: sex = pml::SEX_FEMALE;  break;
    case 3: sex = pml::SEX_UNKNOWN; break;

    case 2:
#if PM_DEBUG
      if( Field::Debug::DebugTypes::s_DebugFinderParam.nSexIdx == 0 )
      {
        sex = pml::SEX_MALE;
      }
      else if( Field::Debug::DebugTypes::s_DebugFinderParam.nSexIdx == 1 )
      {
        sex = pml::SEX_FEMALE;
      }
      else
#endif
      {
        // ランダム
        u32 rand = System::GflUse::GetPublicRand( 2 );
        sex = static_cast<pml::Sex>(rand);
      }
      break;
    }

    m_rModelData[ instance_index ].monsNo      = static_cast<MonsNo>(pModelData->monsNo);
    m_rModelData[ instance_index ].formNo      = pModelData->formNo;
    m_rModelData[ instance_index ].sex         = sex;
    m_rModelData[ instance_index ].basePoint   = pModelData->basePoint;
    m_rModelData[ instance_index ].ext_prm1    = pModelData->ext1;
    m_rModelData[ instance_index ].ext_prm2    = pModelData->ext2;
    m_rModelData[ instance_index ].pathUse     = sub_accessor.GetPassUse( data_index );

    FinderPokeDataAccessor poke_accessor(m_pPokeData);
    const FinderPokeDataAccessor::PokeData *pPokeData = poke_accessor.GetData(pModelData->monsNo);
    if( pPokeData )
    {
      // 属性
      m_rModelData[ instance_index ].figure = pPokeData->sort;
      // アピール設定＠昼5夜5のならびなのでoffsetでずらす
      u32 offset = m_IsNoon? 0: Field::TrialModel::FINDER_ANIM_MAX;
      for( u32 i = 0; i < Field::TrialModel::FINDER_ANIM_MAX; ++ i )
      {
        u32 idx = i + offset;
        if( pPokeData->appeal[idx].motion != 0 )
        {
          m_rModelData[ instance_index ].animNo[i]      = pPokeData->appeal[idx].motion;
          m_rModelData[ instance_index ].voice[i]       = pPokeData->appeal[idx].voice;
          m_rModelData[ instance_index ].figureAnime[i] = pPokeData->appeal[idx].sort;
        }
      }
    }

    // カワイガリデータ
    if( m_rModelData[ instance_index ].monsNo != MONSNO_NULL )
    {
      int ang = m_pKawaigari->GetAdjustLookAngle( m_rModelData[ instance_index ].monsNo, m_rModelData[ instance_index ].formNo,m_rModelData[ instance_index ].sex );
      m_rModelData[ instance_index ].adjustAngle = static_cast<f32>( ang );
    }
  }

  return bResult;
}

/**
 *  @brief  抽選処理
 */
void Manager::execLottery( GameSys::GameManager *pGameManager )
{
  s32 idx[Field::TrialModel::FINDER_POKE_MODEL_MAX] = { -1, -1 };
  s32 cnt = 0;

  FinderSubjectDataAccessor accessor( m_pSubjectData );
  // 登場数抽選
  s32 num = accessor.CalcSubjectNum();
  if( num > cnt )
  {
    s32 old_use = -1;
    // レア抽選
    if( accessor.ExecLotteryRare() )
    {
      // レアデータの設定
      idx[cnt] = accessor.GetRareIndex();
      ++ cnt;
    }
    if( num > cnt )
    {
      // 通常抽選1回目
      old_use = accessor.ExecLotteryNormal( old_use );
      idx[cnt] = accessor.GetNormalIndex( old_use );
      ++ cnt;
    }
    if( num > cnt )
    {
      // 通常抽選2回目
      old_use = accessor.ExecLotteryNormal( old_use );
      idx[cnt] = accessor.GetNormalIndex( old_use );
      ++ cnt;
    }
  }

  Field::TrialModel::FinderSpotReserveData *pReserveData = pGameManager->GetGameData()->GetFinderSpotReserveData( m_nSpotID );
  // 時間の更新
  GameSys::DeviceDate date_now;
  pReserveData->year   = date_now.GetYear();
  pReserveData->month  = date_now.GetMonth();
  pReserveData->day    = date_now.GetDay();
  pReserveData->hour   = date_now.GetHour();
  pReserveData->minute = date_now.GetMinute();
  pReserveData->second = date_now.GetSecond();
  pReserveData->timezone = GameSys::TimeZone::GetNowTimeZone();
  // 予約の更新
  pReserveData->monsIdx[0] = idx[0];
  pReserveData->monsIdx[1] = idx[1];
  pReserveData->bReserve = true;
}

/**
 *  @brief  カワイガリデータ破棄
 */
void Manager::deleteKawaigari( GameSys::GameManager *pGameManager )
{
  if( m_pKawaigari != NULL )
  {
    m_pKawaigari->Finalize();
    GFL_SAFE_DELETE( m_pKawaigari );
  }
}

/**
 *  @brief  セーブデータからデータを受け取る
 */
void Manager::setDataFromSavedata( GameSys::GameManager *pGameManager )
{
  Savedata::PokeFinderSave *pSave = pGameManager->GetGameData()->GetPokeFinderSave();
#if PM_DEBUG
  // デバッグ用：ファインダー全開
  {
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
    {
      // 全開にする
      if( pSave->GetCameraVersion() < Savedata::PokeFinderSave::MAX_POKE_FINDER_LEVEL )
      {
        pSave->SetCameraVersion( Savedata::PokeFinderSave::MAX_POKE_FINDER_LEVEL );
      }
    }
  }
#endif
  // チュートリアルを確認する
#if PM_DEBUG
  if( Field::Debug::DebugTypes::s_DebugFinderParam.nTutorial != 0 )
  {
    // 強制チュートリアルの場合
    switch ( Field::Debug::DebugTypes::s_DebugFinderParam.nTutorial )
    {
    case 1: m_rUIParam.tutorial_flg = Savedata::PokeFinderSave::FINDER_TUTORIAL_STARTUP;  break;
    case 2: m_rUIParam.tutorial_flg = Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM1;    break;
    case 3: m_rUIParam.tutorial_flg = Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM2;    break;
    case 4: m_rUIParam.tutorial_flg = Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM3;    break;
    case 5: m_rUIParam.tutorial_flg = Savedata::PokeFinderSave::FINDER_TUTORIAL_CALL;     break;
    }
  }
  else
#endif
  {
    u32 ver = pSave->GetCameraVersion();
    switch( ver )
    {
    case 0:
      if( ( pSave->GetTutorialFlags() & Savedata::PokeFinderSave::FINDER_TUTORIAL_STARTUP ) == 0 )
      {
        m_rUIParam.tutorial_flg = Savedata::PokeFinderSave::FINDER_TUTORIAL_STARTUP;
        pSave->SetTutorialFlags( Savedata::PokeFinderSave::FINDER_TUTORIAL_STARTUP, true );
      }
      break;
    case 1:
      if( ( pSave->GetTutorialFlags() & Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM1 ) == 0 )
      {
        m_rUIParam.tutorial_flg = Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM1;
        pSave->SetTutorialFlags( Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM1, true );
      }
      break;
    case 2:
      if( ( pSave->GetTutorialFlags() & Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM2 ) == 0 )
      {
        m_rUIParam.tutorial_flg = Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM2;
        pSave->SetTutorialFlags( Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM2, true );
      }
      break;
    case 3:
      if( ( pSave->GetTutorialFlags() & Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM3 ) == 0 )
      {
        m_rUIParam.tutorial_flg = Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM3;
        pSave->SetTutorialFlags( Savedata::PokeFinderSave::FINDER_TUTORIAL_ZOOM3, true );
      }
      break;
    case 4:
      if( ( pSave->GetTutorialFlags() & Savedata::PokeFinderSave::FINDER_TUTORIAL_CALL ) == 0 )
      {
        m_rUIParam.tutorial_flg = Savedata::PokeFinderSave::FINDER_TUTORIAL_CALL;
        pSave->SetTutorialFlags( Savedata::PokeFinderSave::FINDER_TUTORIAL_CALL, true );
      }
      break;
    }
  }
  // ズームを取得
#if   defined(GF_PLATFORM_CTR)
  Field::TrialModel::FinderUtil::GetFinderSkillZoom( pGameManager, &(m_rUIParam.zoom_max) );
#elif defined(GF_PLATFORM_WIN32)
  m_rUIParam.zoom_max = 4.0f;
#endif
  // ジャイロ
  m_rUIParam.gyro_flg = pSave->GetGyroFlag();
  // コメント制限
  if( pSave->GetCameraVersion() < 2 )
  {
    m_IsVerLimit = true;
  }
}

/**
 *  @brief  カラーバッファ作成
 */
void Manager::createColorBuffer( void )
{
  u32 size = sizeof(u32)*512*256;
  m_pColorBuffer = GflHeapAllocMemoryAlign( m_pHeap->GetLowerHandle(), size, 16 );
}

/**
 *  @brief  メニュー作成
 */
void Manager::createMenu( GameSys::GameManager *pGameManager )
{
  Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
  Field::SubScreen::FieldSubScreenProcManager *pFieldSubScreenManager = pFieldmap->GetFieldSubScreenManager();
  pFieldSubScreenManager->CallWindow( Field::SubScreen::WIN_FINDER );
}

/**
 *  @brief  メニュー作成待ち
 */
bool Manager::isCreateMenu( GameSys::GameManager *pGameManager )
{
  Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
  Field::SubScreen::FieldSubScreenProcManager *pSubScreenManager = pFieldmap->GetFieldSubScreenManager();

  if( !m_pUIProc )
  {
    // 既存の下画面が終了し、ポケファインダー下画面が生成されるの待ち
    m_pUIProc = pSubScreenManager->GetFienderProc();
    if( m_pUIProc )
    {
      // 生成されたらパラメータセットし初期化待ちへ
      m_pUIProc->SetAppParam( &m_rUIParam );
    }
    return false;
  }

  // 切替が完了していない
  if( !pFieldmap->GetFieldSubScreenManager()->IsChangeComplate() ){ return false; }
  // セットアップが完了していない
  if( !m_pUIProc->GetFinderFrame() ){ return false; }
  if( !m_pUIProc->GetFinderFrame()->IsStartSetupEnd() ){ return false; }

  return true;
}
/**
 *  @brief  ポケモンモデルシステムの作成
 */
void Manager::createPokeSystem( GameSys::GameManager *pGameManager )
{
  Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
  // ポケモン作成用ヒープ：1匹目
  m_pPokeHeap[0] = GFL_CREATE_LOCAL_HEAP( m_pHeap, 0x340000, gfl2::heap::HEAP_TYPE_EXP, false );
  // ポケモン作成用ヒープ：2匹目
  m_pPokeHeap[1] = pFieldmap->GetFieldExtHeap();

  m_pModelFactory->InitializeSystem( m_pHeap, m_pHeap_FileRead, Field::TrialModel::FINDER_POKE_MODEL_MAX );
}

/**
 *  @brief  ポケモンモデルを生成する
 */
void Manager::createPokeModel( GameSys::GameManager *pGameManager )
{
  m_pModelFactory->CreateSystem( m_pHeap, m_pAllocator, NULL, false );
  m_pModelFactory->SetFinderMode( true );

  FinderPokeDataAccessor accessor( m_pPokeData );

  for( u32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    // 抽選されなかった
    if( m_rModelData[i].monsNo == MONSNO_NULL ){ continue; }
    // ヒープが存在しない
    if( m_pPokeHeap[i] == NULL ){ continue; }

    PokeTool::SimpleParam param;
    param.monsNo = m_rModelData[i].monsNo;
    param.formNo = m_rModelData[i].formNo;
    param.sex = m_rModelData[i].sex;
    PokeTool::PokeModel::SetupOption option;
    option.dataHeap = m_pPokeHeap[i];
    option.workHeap = m_pHeap_FileRead;
    option.animeType = PokeTool::MODEL_ANIMETYPE_POKE_FINDER;
    TrialModel::TrialModelFactory::FinderModelSetupParam setup;
    setup.pHeap = m_pHeap;
    setup.param = &param;
    setup.option = &option;
    setup.model = &(m_rModelData[i]);

    m_pModel[ i ] = m_pModelFactory->CreateFinderModel( setup );

    // データ設定
    const FinderPokeDataAccessor::PokeData *pPokeData = accessor.GetData( m_rModelData[i].monsNo );
    if( pPokeData != NULL )
    {
      Field::TrialModel::FinderPokeData rData;
      rData.enable = pPokeData->enable != 0;
      rData.deg    = pPokeData->degree;
      rData.frame  = pPokeData->frame;
      rData.wait   = pPokeData->wait;
      rData.move   = pPokeData->move_type;
      rData.voice  = pPokeData->voice;
      rData.min.Set( pPokeData->min_x, pPokeData->min_y, pPokeData->min_z, 0.0f );
      rData.max.Set( pPokeData->max_x, pPokeData->max_y, pPokeData->max_z, 0.0f );
      // コピーを行っているのでローカル変数で渡して問題ない
      m_pModel[ i ]->SetPokeData( &rData );
    }
    MIYACHI_PRINT( "Finder Idx=%d Mons=%d Path=%d", i, m_rModelData[i].monsNo, m_rModelData[i].pathUse );
  }

#if PM_DEBUG
    if( m_pDebug )
    {
      m_pDebug->m_pMons1 = m_pModel[0];
      m_pDebug->m_pMons2 = m_pModel[1];
    }
#endif
}

/**
 *  @brief  ポケモンモデル作成待ち
 */
bool Manager::isCreatePokeModel( void )
{
  bool bResult = true;

  for( s32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    if( !m_pModel[ i ] ){ continue; }
    if( !m_pModel[ i ]->IsReady() )
    {
      bResult = false;
    }
  }

  return bResult;
}

/**
 *  @brief  ポケモンモデルセットアップ
 */
void Manager::setupPokeModel( void )
{
  for( s32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    if( !m_pModel[ i ] ){ continue; }

    m_pModel[ i ]->CreateModel();
    m_pModel[ i ]->SetFieldAdjustHeight();
    m_pModel[ i ]->SetEnableAmbientColor( true );
    m_pModel[ i ]->SetOptionalRenderKey();

    switch( m_rModelData[i].pathUse )
    {
    case Field::TrialModel::FINDER_PATH_USE_MOVE_A:
      m_pModel[ i ]->CreateControlWalker( m_pHeap, &m_rPathData[Field::TrialModel::FINDER_PATH_A] );
      break;
    case Field::TrialModel::FINDER_PATH_USE_MOVE_B:
      m_pModel[ i ]->CreateControlWalker( m_pHeap, &m_rPathData[Field::TrialModel::FINDER_PATH_B] );
      break;
    case Field::TrialModel::FINDER_PATH_USE_WAIT:
      m_pModel[ i ]->CreateControlStatue( m_pHeap, &m_rStatueData );
      break;
    case Field::TrialModel::FINDER_PATH_USE_RARE:
      m_pModel[ i ]->CreateControlOneway( m_pHeap, &m_rOnewayData );
      break;
    }
  }
}

/**
 *  @brief  描画変更
 */
void Manager::startGraphic( GameSys::GameManager *pGameManager )
{
#if ENABLE_RENDER_DOF
  Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
  gfl2::fs::BinLinkerAccessor linker;
  linker.Initialize( m_pRenderPath );

  void *pDofModelData = NULL;
  void *pDofVertexShaderData = NULL;
  void *pDofFrontPixelShaderData = NULL;
  void *pDofBackPixelShaderData = NULL;
  void *pDofTextureData = NULL;

#if defined(GF_PLATFORM_CTR)
  pDofModelData             = linker.GetData( BL_IDX_DEPTHOFFIELDRESOURCE_DOFSHADER_CTR_CTR_GFBMDL );
  pDofVertexShaderData      = linker.GetData( BL_IDX_DEPTHOFFIELDRESOURCE_DEFAULT_CTR_GFBVSH );
  pDofFrontPixelShaderData  = linker.GetData( BL_IDX_DEPTHOFFIELDRESOURCE_COMBFRONT_MANUAL_CTR_GFBFSH );
  pDofBackPixelShaderData   = linker.GetData( BL_IDX_DEPTHOFFIELDRESOURCE_COMBBACK_MANUAL_CTR_GFBFSH );
  pDofTextureData           = linker.GetData( BL_IDX_DEPTHOFFIELDRESOURCE_EDGEMAPIMAGE_CTR_BTEX );
#elif  defined(GF_PLATFORM_WIN32)
  pDofModelData             = linker.GetData( BL_IDX_DEPTHOFFIELDRESOURCE_DOFSHADER_WINGL_GFBMDL );
  pDofVertexShaderData      = linker.GetData( BL_IDX_DEPTHOFFIELDRESOURCE_DEFAULT_WINGL_GFBVSH );
  pDofFrontPixelShaderData  = linker.GetData( BL_IDX_DEPTHOFFIELDRESOURCE_COMBFRONT_MANUAL_WINGL_GFBFSH );
  pDofBackPixelShaderData   = linker.GetData( BL_IDX_DEPTHOFFIELDRESOURCE_COMBBACK_MANUAL_WINGL_GFBFSH );
  pDofTextureData           = linker.GetData( BL_IDX_DEPTHOFFIELDRESOURCE_EDGEMAPIMAGE_WINGL_BTEX );
#endif

  Field::MyRenderingPipeLine *PipeLine = pFieldmap->GetFieldRenderingPipeLineManager()->GetMyRenderingPipeLine();
  Field::MyRenderingPipeLine::DofRenderPathInitDescription desc;
  desc.pAllocator = m_pAllocator;
  desc.screen_width = 512;
  desc.screen_height = 256;

  desc.pDofModelData            = pDofModelData;
  desc.pDofVertexShaderData     = pDofVertexShaderData;
  desc.pDofFrontPixelShaderData = pDofFrontPixelShaderData;
  desc.pDofBackPixelShaderData  = pDofBackPixelShaderData;
  desc.pDofTextureData          = pDofTextureData;

  PipeLine->InitializeDofRenderPath( m_pDofHeap, desc );
  PipeLine->SetEnableDofRender( true );
#endif

  // 立体視ON
#if defined(GF_PLATFORM_CTR)
  gfl2::gfx::GFGL::SetStereoModeEnable( true );
  m_TmpSkipDrawMode = pGameManager->EnableSkipDrawMode();
  // 15FpsMode
  pGameManager->SetEnableSkipDrawMode( GameSys::GameManager::SKIP_DRAWMODE_FPS_HALF );
  nngxSetMemAccessPrioMode( NN_GX_MEM_ACCESS_PRIO_MODE_4 );
#endif // #if defined(GF_PLATFORM_CTR)
  // パラメータの初回設定
  updateFocus( pGameManager );
}

/**
 *  @brief  ポケモンの更新
 */
void Manager::updatePokeModel( void )
{
  gfl2::math::Vector3 pos_camera = gfl2::math::Vector3::GetZero();
  if( m_pCamera )
  {
    pos_camera = m_pCamera->GetPosition();
  }
  for( u32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    if( !m_pModel[ i ] ){ continue; }
    if( !m_pModel[ i ]->IsEnable() ){ continue; }
    m_pModel[ i ]->SetInterestPosition( pos_camera );
  }
}

/**
 *  @brief  メニューパラメータの更新
 */
void Manager::updateMenu( void )
{
  // Zoom
  if( m_pCamera )
  {
    // @fix GFNMCat[2350]SEラベルが逆
    // ZoomOut
    if( m_rUIParam.zoom_val > m_pCamera->GetZoom() )
    {
      if( !Sound::IsSEPlaying( SEQ_SE_PF_ZOOMOUT ) )
      {
        Sound::PlaySE( SEQ_SE_PF_ZOOMOUT );
      }
    }
    else
    {
      if( Sound::IsSEPlaying( SEQ_SE_PF_ZOOMOUT ) )
      {
        Sound::StopSE( SEQ_SE_PF_ZOOMOUT );
      }
    }
    // ZoomIn
    if( m_rUIParam.zoom_val < m_pCamera->GetZoom() )
    {
      if( !Sound::IsSEPlaying( SEQ_SE_PF_ZOOMIN ) )
      {
        Sound::PlaySE( SEQ_SE_PF_ZOOMIN );
      }
    }
    else
    {
      if( Sound::IsSEPlaying( SEQ_SE_PF_ZOOMIN ) )
      {
        Sound::StopSE( SEQ_SE_PF_ZOOMIN );
      }
    }

    m_rUIParam.zoom_min = m_pCamera->GetZoomMini();
    m_rUIParam.zoom_max = m_pCamera->GetZoomMaxi();
    m_rUIParam.zoom_val = m_pCamera->GetZoom();
  }
  // Gyro
  if( m_pCamera )
  {
    m_pCamera->SetGyroEnable( m_rUIParam.gyro_flg );
  }
  // Input
  if( m_pCamera )
  {
    bool enable = ( !m_rUIParam.ext_input_disable ) && ( !m_IsSnapshot );
    m_pCamera->SetInputEnable( enable );
  }
  // 残り枚数
  m_rUIParam.shutter_count = Field::TrialModel::FINDER_PHOTO_MAX - m_nSnapCount;
  // ポケモンを呼ぶ
  if( m_rUIParam.poke_call )
  {
    if( m_nCallReload > 0 )
    {
      // リロード中
      -- m_nCallReload;
      if( m_nCallReload == 0 )
      {
        // ボタンを有効化する
        m_rUIParam.poke_call = false;
      }
    }
    else
    {
      // ポケモンを呼ぶ
      requestPokeCall();
    }
  }
}

/**
 *  @brief  ぽけもんを呼ぶ
 */
void Manager::requestPokeCall( void )
{
  if( !m_pCamera ){ return; }

  gfl2::math::Matrix44 mtx_proj;
  gfl2::math::Matrix34 mtx_view;
  m_pCameraManager->GetMainViewCamera()->GetProjectionMatrix( &mtx_proj );
  m_pCameraManager->GetMainViewCamera()->GetViewMatrix( &mtx_view );

  gfl2::math::Vector3 pos_camera = m_pCamera->GetPosition();
  gfl2::math::Vector3 diff_chara[Field::TrialModel::FINDER_POKE_MODEL_MAX];
  for( u32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    diff_chara[ i ] = gfl2::math::Vector3::GetZero();
    if( !m_pModel[ i ] ){ continue; }

    gfl2::math::Vector3 pos = m_pModel[ i ]->GetPosition();
    // スクリーン変換
    gfl2::math::Vector4 view_pos = mtx_view * pos;
    gfl2::math::Vector4 proj_pos = mtx_proj * view_pos;
    diff_chara[i].x = proj_pos.x / proj_pos.w * FINDER_WIDTH;
    diff_chara[i].y = proj_pos.y / proj_pos.w * FINDER_HEIGHT;
  }

  u32 short_idx = 1;
  if( m_pModel[0] && m_pModel[1] && diff_chara[0].Length() < diff_chara[1].Length() )
  {
    short_idx = 0;
  }
  else if( m_pModel[0] )
  {
    short_idx = 0;
  }

  // 近いほうにリクエスト
  if( m_pModel[ short_idx ] )
  {
    m_pModel[ short_idx ]->RequestPokeCall();
  }

  m_nCallReload = FINDER_CALL_RELOAD;
}

/**
 *  @brief  ピントの更新
 */
void Manager::updateFocus( GameSys::GameManager *pGameManager )
{
#if ENABLE_RENDER_DOF
  Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
  Field::MyRenderingPipeLine *pPipeLine = pFieldmap->GetFieldRenderingPipeLineManager()->GetMyRenderingPipeLine();

  if( !pPipeLine->IsEnableDofRender() ){ return; }

  // 一之瀬さん要望対応：被写界深度変更SE追加
  if( m_Focus_Old > m_rUIParam.pint_type )
  {
    Sound::PlaySE( SEQ_SE_SYS_NJ_060 );
  }
  if( m_Focus_Old < m_rUIParam.pint_type )
  {
    Sound::PlaySE( SEQ_SE_SYS_NJ_059 );
  }

  m_Focus_Old = m_rUIParam.pint_type;
  u32 focus = m_rUIParam.pint_type;

  f32 offset = 50.0f;
  f32 width  = 280.0f;
  f32 COEF_F = 0.25f;
  f32 COEF_B = 0.25f;
  f32 step = 0.30f;

#if PM_DEBUG
  if( m_pDebug )
  {
    if( m_pDebug->m_debug_focus != 0 )
    {
      offset = m_pDebug->m_focus_offset;
      width  = m_pDebug->m_focus_width;
      COEF_F = m_pDebug->m_focus_fpow;
      COEF_B = m_pDebug->m_focus_bpow;
      step   = m_pDebug->m_focus_powstep;
    }
  }
#endif

  f32 front_len = width *  focus    + offset;
  f32 back_len  = width * (focus+1) + offset;

  // Front
  for( u32 idx = 0; idx < 3; ++ idx )
  {
    f32 f_len = front_len - idx*width;
    f32 b_len = back_len  + idx*width;
    f32 f_pow = COEF_F + idx*step;
    f32 b_pow = COEF_B + idx*step;
    pPipeLine->GetMakeBlurImagePath_Front()->SetFocusRange( idx, f_len, f_pow );
    pPipeLine->GetMakeBlurImagePath_Back()->SetFocusRange( idx, b_len, b_pow );
  }
  // DefRender
  pPipeLine->GetDefRenderPath_Front()->SetFocusLength( front_len );
  pPipeLine->GetDefRenderPath_Back()->SetFocusLength( back_len );
#endif
}

/**
 *  @brief  BGMボリュームの更新
 */
void Manager::updateBgmVolume( GameSys::GameManager *pGameManager )
{
  if( !m_pCamera ){ return; }
  if( !m_pCamera->GetZoomInput() ){ return; }

  f32 ratio = MAX_BGM_VOLUME;
  f32 range = m_pCamera->GetZoomMaxi() - m_pCamera->GetZoomMini();
  if( range > 0.0f )
  {
    f32 current = m_pCamera->GetZoom() - m_pCamera->GetZoomMini();
    f32 diff = MAX_BGM_VOLUME - MIN_BGM_VOLUME;
    ratio = current / range * diff;
    ratio = MAX_BGM_VOLUME - ratio;
  }

  pGameManager->GetGameData()->GetFieldSound()->ChangeBGMVolume( ratio, Sound::BGM_FADE_FAST );
}

/**
 *  @brief  安全に撮影するための設定
 */
void Manager::setupSafeSnapshot( void )
{
  // ポケモンの停止
  for( u32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    if( !m_pModel[ i ] ){ continue; }
    m_pModel[ i ]->RequestSuspend();
  }
}

/**
 *  @brief  安全に撮影できるかのチェック
 */
bool Manager::isSafeSnapshot( void )
{
  // モデルのブレンドをチェック
  for( u32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    if( !m_pModel[ i ] ){ continue; }
    if( m_pModel[ i ]->IsPause() ){ continue; }

    // 停止していない人がいる
    return false;
  }

  // 安全である
  return true;
}

/**
 *  @brief  スナップショットの作成
 */
void Manager::requestSnapshot( GameSys::GameManager *pGameManager )
{
  Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
  pFieldmap->GetFieldRenderingPipeLineManager()->RequestTransferColorBuffer( m_pColorBuffer );
  m_nDelayFrame = 0;
}

/**
 *  @brief  スナップショット完了待ち
 */
bool Manager::isFinishSnapshot( void )
{
  // 指定フレーム待機して完了を待つ
  if( m_nDelayFrame < SNAPSHOT_WAIT_FRAME )
  {
    ++ m_nDelayFrame;
    return false;
  }

  // 完了した
  return true;
}

/**
 *  @brief  Jpegの作成
 */
void Manager::createJpeg( GameSys::GameManager *pGameManager )
{
#if defined(GF_PLATFORM_CTR)
  Field::TrialModel::FinderPhotoData *pPhotoData = m_pFinderMenuParam->photo_data[ m_nSnapCount ];
  if( !pPhotoData ){ return; }

  Field::TrialModel::FinderUtil::EncodeJpeg( m_pHeap_FileRead, m_pColorBuffer, &pPhotoData->jpeg );
  MIYACHI_PRINT( "CreateJpeg\n" );
#endif
}

/**
 *  @brief  評価
 */
void Manager::calcValue( GameSys::GameManager *pGameManager )
{
  Field::TrialModel::FinderPhotoData *pPhotoData = m_pFinderMenuParam->photo_data[ m_nSnapCount ];
  if( !pPhotoData ){ return; }

  // コメントのクリア
  for( s32 i = 0; i < Field::TrialModel::FINDER_VALUE_COMMENT_MAX; ++ i )
  {
    m_rComment[i].msgID = 0;
    m_rComment[i].priority = 0;
  }

  Field::TrialModel::FinderPhotoValue *pValue = &( pPhotoData->data );

  // 評価式作成
  m_rSituation.score = 0;
  for( u32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    m_rSituation.score += calcBaseScore( i );
    m_rSituation.score += calcSituationSingleScore( i, pValue );
  }
  m_rSituation.score += calcSituationDoubleScore( pValue );
  m_rSituation.score += calcSituationOtherScore( pValue );
  m_rSituation.score += m_rSpotData.spot_score;
  // 乱数
  u32 rand = System::GflUse::GetPublicRand( 100 );
  m_rSituation.score += rand;
#if PM_DEBUG
  if( Field::Debug::DebugTypes::s_DebugFinderParam.nScoreLock > -1)
  {
    m_rSituation.score = Field::Debug::DebugTypes::s_DebugFinderParam.nScoreLock;
  }
#endif
  pValue->value = m_rSituation.score;
  // スコア引き渡し
  m_rUIParam.photo_score = m_rSituation.score;

  // コメント
  for( s32 i = 0; i < Field::TrialModel::FINDER_VALUE_COMMENT_MAX; ++ i )
  {
    pValue->comment[i] = m_rComment[i].msgID;
  }

  m_nDelayFrame = 0;

#if PM_DEBUG
  m_pDebug->m_score_total = m_rSituation.score;
  m_pDebug->m_score_spot  = m_rSpotData.spot_score;
  m_pFinderDebugInfo->SetScorePlace( m_rSituation.score, rand , m_rSpotData.spot_score );
  m_pFinderDebugInfo->SetPokeRect( m_rSituation.min[0], m_rSituation.max[0] );
  GameSys::TimeZone::Zone eZone = GameSys::TimeZone::GetNowTimeZone();
  Field::weather::WeatherKind eKind = m_pWeather->GetNowWeatherKind();
  m_pFinderDebugInfo->SetCommentParam( m_rSpotData.spot_type, eZone, eKind );

  pValue->place_score = m_rSpotData.spot_score;
  pValue->rand_score  = rand;

  if( Field::Debug::DebugTypes::s_DebugFinderParam.nCommentEnable != 0 )
  {
    s32 cnt = 0;
    for( s32 idx = 0; idx < Field::TrialModel::FINDER_VALUE_COMMENT_MAX; ++ idx )
    {
      pValue->comment[idx] = 0;
    }
    for( s32 idx = 0; idx < Field::TrialModel::FINDER_VALUE_COMMENT_MAX; ++ idx )
    {
      void *buf = NULL;
      s32 type = Field::Debug::DebugTypes::s_DebugFinderParam.aComDataType[idx];
      s32 id   = Field::Debug::DebugTypes::s_DebugFinderParam.aComDataID[idx];
      s32 msg  = Field::Debug::DebugTypes::s_DebugFinderParam.aComDataMsg[idx];

      if( msg == -1 ){ continue; }

      switch( type )
      {
      case SITUATION_SINGLE:  buf = m_pSituationSingle; break;
      case SITUATION_DOUBLE:  buf = m_pSituationDouble; break;
      case SITUATION_OTHER:   buf = m_pSituationOther;  break;
      }
      SituationDataAccessor accessor( buf );
      CommentData data;
      if( accessor.GetData( id, data ) )
      {
        if( data.msgID != 0 )
        {
          pValue->comment[cnt] = data.msgID + msg;
          ++ cnt;
        }
      }
    }
  }

#endif
}

/**
 *  @brief  基礎計算式：ポケモン
 */
u32 Manager::calcBaseScore( u32 idx )
{
  // 状況を設定する
  if( !calcSituation( idx ) ){ return 0; }

  f32 result = 0;

  // 個体が持つ得点
  f32 base_point = m_pModel[idx]->GetBasePoint();
  // アピール中
  f32 appeal = m_pModel[idx]->IsAppeal()? 1.5f: 1.0f;
  // 向き
  f32 dir = 1.0f;
  switch( m_rSituation.eRot[idx] )
  {
  case ROT_DIR_FRONT: dir = 2.0f; break;
  case ROT_DIR_SIDE:  dir = 1.0f; break;
  case ROT_DIR_BACK:  dir = 0.5f; break;
  }
  // 画面占有率
  f32 screen = calcPokeScreen( idx );
  m_rSituation.scr_score[idx] = screen;
  // はみ出具合
  f32 out = calcPokeOut( idx );
  m_rSituation.scr_out[idx] = out;

  result += base_point * appeal * dir * screen * out;

#if PM_DEBUG
  if( idx == 0 )
  {
    m_pDebug->m_score_basepoint_1 = base_point;
    m_pDebug->m_score_motion_1 = appeal;
    m_pDebug->m_score_dir_1 = dir;
    m_pDebug->m_score_screen_1 = screen;
    m_pDebug->m_score_out_1 = out;
  }
  if( idx == 1 )
  {
    m_pDebug->m_score_basepoint_2 = base_point;
    m_pDebug->m_score_motion_2 = appeal;
    m_pDebug->m_score_dir_2 = dir;
    m_pDebug->m_score_screen_2 = screen;
    m_pDebug->m_score_out_2 = out;
  }

  m_pFinderDebugInfo->SetScorePoke( idx, base_point, appeal, dir, screen, out );

  Field::TrialModel::FinderPhotoData *pPhotoData = m_pFinderMenuParam->photo_data[ m_nSnapCount ];
  if( pPhotoData )
  {
    Field::TrialModel::FinderPhotoValue *pValue = &( pPhotoData->data );
    pValue->base_point[idx] = base_point;
    pValue->appeal[idx]     = appeal;
    pValue->dir[idx]        = dir;
    pValue->screen[idx]     = screen;
    pValue->out[idx]        = out;
  }

#endif
  return static_cast<u32>(result);
}

/**
 *  @brief  状況を設定する
 */
bool Manager::calcSituation( u32 idx )
{
  if( idx >= Field::TrialModel::FINDER_POKE_MODEL_MAX ){ return false; }
  if( !m_pModel[ idx ] ){ return false; }

  // 一度画面外に設定
  m_rSituation.inScreen[idx] = false;
  if( !m_pModel[ idx ]->IsDisp() ){ return false; }

  Field::TrialModel::FinderModel *pFinderModel = m_pModel[idx];
  PokeTool::PokeModel *pModel = pFinderModel->GetPokeModel();

  gfl2::math::Matrix44 mtx_proj;
  gfl2::math::Matrix34 mtx_view;
  m_pCameraManager->GetMainViewCamera()->GetProjectionMatrix( &mtx_proj );
  m_pCameraManager->GetMainViewCamera()->GetViewMatrix( &mtx_view );
  // 正射影AABBの生成
  gfl2::math::AABB aabb;
  pFinderModel->GetAABB( &aabb );
  gfl2::math::Vector4 min = aabb.GetMin();
  gfl2::math::Vector4 max = aabb.GetMax();
  gfl2::math::Vector3 dst_aabb[8];
  dst_aabb[0] = gfl2::math::Vector3( min.x, min.y, min.z );
  dst_aabb[1] = gfl2::math::Vector3( min.x, min.y, max.z );
  dst_aabb[2] = gfl2::math::Vector3( min.x, max.y, min.z );
  dst_aabb[3] = gfl2::math::Vector3( min.x, max.y, max.z );
  dst_aabb[4] = gfl2::math::Vector3( max.x, min.y, min.z );
  dst_aabb[5] = gfl2::math::Vector3( max.x, min.y, max.z );
  dst_aabb[6] = gfl2::math::Vector3( max.x, max.y, min.z );
  dst_aabb[7] = gfl2::math::Vector3( max.x, max.y, max.z );

  { // @fix GFNMCat[1824]スクリーン外判定追加
    gfl2::math::Matrix44 viewProj = mtx_proj * mtx_view;
    gfl2::math::Matrix44 cullingMatrixTable[4];
    gfl2::math::AABB::Util::CalcFrustumPlanes( cullingMatrixTable, viewProj );
    gfl2::math::Vector4 modelPos( pFinderModel->GetPosition() );
    if( !gfl2::math::AABB::Util::IsIntersectFrustum( cullingMatrixTable, aabb.Offset( modelPos ) ) ){ return false; }
  }

  bool bFirst = true;
  for( u32 i = 0; i < 8; ++ i )
  {
    // 座標をWorldへ
    gfl2::math::Vector3 pos = pFinderModel->GetAABBMountPosition();
    gfl2::math::Vector3 rot = pModel->GetRotation();
    gfl2::math::Matrix34 mtx_rot = gfl2::math::Matrix34::GetRotationZYX( rot.z, rot.y, rot.x );
    pos += mtx_rot * dst_aabb[i];
    // スクリーン変換
    gfl2::math::Vector4 view_pos = mtx_view * pos;
    gfl2::math::Vector4 proj_pos = mtx_proj * view_pos;
    gfl2::math::Vector2 scrn_pos;
    scrn_pos.x = proj_pos.x / proj_pos.w * FINDER_WIDTH;
    scrn_pos.y = proj_pos.y / proj_pos.w * FINDER_HEIGHT;

    if( bFirst )
    {
      m_rSituation.min[idx] = scrn_pos;
      m_rSituation.max[idx] = scrn_pos;
      bFirst = false;
    }
    else
    {
      if( m_rSituation.min[idx].x > scrn_pos.x ) { m_rSituation.min[idx].x = scrn_pos.x; }
      if( m_rSituation.min[idx].y > scrn_pos.y ) { m_rSituation.min[idx].y = scrn_pos.y; }
      if( m_rSituation.max[idx].x < scrn_pos.x ) { m_rSituation.max[idx].x = scrn_pos.x; }
      if( m_rSituation.max[idx].y < scrn_pos.y ) { m_rSituation.max[idx].y = scrn_pos.y; }
    }
  }

  // 面積が一定以上を保証する
  {
    f32 min_x = gfl2::math::max<f32>( -FINDER_WIDTH, m_rSituation.min[idx].x );
    f32 min_y = gfl2::math::max<f32>( -FINDER_WIDTH, m_rSituation.min[idx].y );
    f32 max_x = gfl2::math::min<f32>(  FINDER_WIDTH, m_rSituation.max[idx].x );
    f32 max_y = gfl2::math::min<f32>(  FINDER_WIDTH, m_rSituation.max[idx].y );
    f32 rect = ( max_x - min_x ) * ( max_y - min_y );
    f32 ratio = rect * 100.0f / (FINDER_WIDTH_2*FINDER_HEIGHT_2);
    f32 threshold = 0.20f;
#if PM_DEBUG
    threshold = Field::Debug::DebugTypes::s_DebugFinderParam.fThreshold;
#endif
    if( ratio < threshold )
    {
#if PM_DEBUG
      m_pFinderDebugInfo->SetPokeRectColor( 1.0f, 0.2f, 0.2f );
#endif
      return false;
    }
#if PM_DEBUG
  m_pFinderDebugInfo->SetPokeRectColor( 1.0f, 1.0f, 0.2f );
#endif
  }

  // 画面外判定
  if( m_rSituation.min[idx].x >  FINDER_WIDTH  ){ return false; }
  if( m_rSituation.min[idx].y >  FINDER_HEIGHT ){ return false; }
  if( m_rSituation.max[idx].x < -FINDER_WIDTH  ){ return false; }
  if( m_rSituation.max[idx].y < -FINDER_HEIGHT ){ return false; }

  // AABBが画面中心を含んでいること
  if( m_rSituation.min[idx].x > 0.0f ){ return false; }
  if( m_rSituation.min[idx].y > 0.0f ){ return false; }
  if( m_rSituation.max[idx].x < 0.0f ){ return false; }
  if( m_rSituation.max[idx].y < 0.0f ){ return false; }

  m_rSituation.inScreen[idx] = true;
#if PM_DEBUG
  m_pFinderDebugInfo->SetPokeRectColor( 0.2f, 1.0f, 0.2f );
#endif

  // 向き
  f32 poke_rot = pModel->GetRotation().y;
  gfl2::math::Vector3 poke_pos = pFinderModel->GetPosition();
  gfl2::math::Vector3 came_pos = m_pCamera->GetPosition();
  gfl2::math::Vector3 dir = came_pos - poke_pos;
  f32 came_rot = atan2( dir.x, dir.z );
  // @fix NMCat[2036]範囲を限定することで角度を保証する
  f32 diff_rot = gfl2::math::Abs( gfl2::math::NormalizeRadianAngle( came_rot - poke_rot ) );
  m_rSituation.rot[idx] = diff_rot;

  f32 cond_rot_045 = gfl2::math::ConvDegToRad(  45.0f );
  f32 cond_rot_135 = gfl2::math::ConvDegToRad( 135.0f );
  if( diff_rot < cond_rot_045 )
  {
    m_rSituation.eRot[idx] = ROT_DIR_FRONT;
  }
  else if( diff_rot < cond_rot_135 )
  {
    m_rSituation.eRot[idx] = ROT_DIR_SIDE;
  }
  else
  {
    m_rSituation.eRot[idx] = ROT_DIR_BACK;
  }

  // @fix GFNMCat[1355] 図鑑登録対応
  if( m_TrialMonsNo == MONSNO_NULL )  // 貫田さん要望対応 試練６例外処理
  {
    Savedata::ZukanData *pZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
    // 無効値の場合は登録しない
    if( m_rModelData[idx].monsNo != MONSNO_NULL )
    {
      PokeTool::SimpleParam param;
      param.monsNo = m_rModelData[idx].monsNo;
      param.formNo = m_rModelData[idx].formNo;
      param.sex = m_rModelData[idx].sex;
      pZukanData->SetPokeSee( param );
    }
  }

  return true;
}

/**
 *  @brief  画面占有率の計算
 */
f32 Manager::calcPokeScreen( u32 idx )
{
  if( idx >= Field::TrialModel::FINDER_POKE_MODEL_MAX ){ return 0.0f; }
  if( !m_pModel[ idx ] ){ return 0.0f; }

  f32 result = 0.0f;

  // 大きさを決定
  f32 width     = m_rSituation.max[idx].x - m_rSituation.min[idx].x;
  f32 length    = m_rSituation.max[idx].y - m_rSituation.min[idx].y;
  f32 rev_w = gfl2::math::Abs( FINDER_WIDTH_2 -  width );
  f32 rev_h = gfl2::math::Abs( FINDER_HEIGHT_2 - length );

  // 短辺にて採点する
  if( rev_w < rev_h )
  {
    result = 1.0f - rev_w / FINDER_WIDTH_2;
  }
  else
  {
    result = 1.0f - rev_h / FINDER_HEIGHT_2;
  }

  // @fix NMCat[2973] 最小値保障
  result = gfl2::math::Max<f32>( 0.0f, result );
  // スケーリング
  result *= ( 2.0f - 0.25f );
  // 最低保証値
  result += 0.25f;

  return result;
}

f32 Manager::calcPokeOut( u32 idx )
{
  f32 result = 0.0f;

  // AABBの面積
  f32 width     = m_rSituation.max[idx].x - m_rSituation.min[idx].x;
  f32 length    = m_rSituation.max[idx].y - m_rSituation.min[idx].y;
  f32 size_aabb = width * length;

  // 画面内の面積
  f32 size_scrn = 0.0f;
  {
    f32 min_x = m_rSituation.min[idx].x;
    if( -1.0f * FINDER_WIDTH  > min_x ){ min_x = -1.0f * FINDER_WIDTH; }
    f32 min_y = m_rSituation.min[idx].y;
    if( -1.0f * FINDER_HEIGHT > min_y ){ min_y = -1.0f * FINDER_HEIGHT; }
    f32 max_x = m_rSituation.max[idx].x;
    if(         FINDER_WIDTH  < max_x ){ max_x =         FINDER_WIDTH; }
    f32 max_y = m_rSituation.max[idx].y;
    if(         FINDER_HEIGHT < max_y ){ max_y =         FINDER_HEIGHT; }
    width  = max_x - min_x;
    length = max_y - min_y;
    size_scrn = width * length;
  }

  result = size_scrn / size_aabb;

  return result;
}

/**
 *  @brief  シチュエーション：単体
 */
u32 Manager::calcSituationSingleScore( u32 idx, Field::TrialModel::FinderPhotoValue *pValue )
{
  if( idx >= Field::TrialModel::FINDER_POKE_MODEL_MAX ){ return 0; }
  if( !m_pModel[ idx ] ){ return 0; }
  if( !m_rSituation.inScreen[idx] ){ return 0; }

  u32 result = 0;

  // 正面をむいている
  if( m_rSituation.eRot[idx] == ROT_DIR_FRONT ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Poke_Front, m_IsVerLimit ); }
  // 側面をむいている
  if( m_rSituation.eRot[idx] == ROT_DIR_SIDE ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Poke_Side, m_IsVerLimit ); }
  // 背面をむいている
  if( m_rSituation.eRot[idx] == ROT_DIR_BACK ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Poke_Back, m_IsVerLimit ); }
  // Finder_SituationSingle_Screen_05
  if( m_rSituation.scr_score[idx] <= 0.5f ){ result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Screen_05, m_IsVerLimit ); }
  // Finder_SituationSingle_Screen_10
  if( 0.5f < m_rSituation.scr_score[idx] && m_rSituation.scr_score[idx] <= 1.0f ){ result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Screen_10, m_IsVerLimit ); }
  // Finder_SituationSingle_Screen_15
  if( 1.0f < m_rSituation.scr_score[idx] && m_rSituation.scr_score[idx] <= 1.5f ){ result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Screen_15, m_IsVerLimit ); }
  // Finder_SituationSingle_Screen_20
  if( 1.5f < m_rSituation.scr_score[idx] && m_rSituation.scr_score[idx] <= 2.0f ){ result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Screen_20, m_IsVerLimit ); }
  // ポケモンがかっこいい
  if( m_pModel[idx]->GetFigure() == Field::TrialModel::FIGURE_COOL ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Poke_Cool, false ); }
  // ポケモンがかわいい
  if( m_pModel[idx]->GetFigure() == Field::TrialModel::FIGURE_CUTE ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Poke_Cute, false ); }
  // ポケモンがかしこい
  if( m_pModel[idx]->GetFigure() == Field::TrialModel::FIGURE_WISE ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Poke_Wise, false ); }
  // ポケモンがたくましい
  if( m_pModel[idx]->GetFigure() == Field::TrialModel::FIGURE_TOUGH ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Poke_Tough, false ); }
  // ポケモンがうつくしい
  if( m_pModel[idx]->GetFigure() == Field::TrialModel::FIBURE_BEAUTY ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Poke_Beauty, false ); }
  // アピールがかっこいい
  if( m_pModel[idx]->GetFigureAppeal() == Field::TrialModel::FIGURE_COOL ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Appeal_Cool, false ); }
  // アピールがかわいい
  if( m_pModel[idx]->GetFigureAppeal() == Field::TrialModel::FIGURE_CUTE ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Appeal_Cute, false ); }
  // アピールがかしこい
  if( m_pModel[idx]->GetFigureAppeal() == Field::TrialModel::FIGURE_WISE ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Appeal_Wise, false ); }
  // アピールがたくましい
  if( m_pModel[idx]->GetFigureAppeal() == Field::TrialModel::FIGURE_TOUGH ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Appeal_Tough, false ); }
  // アピールがうつくしい
  if( m_pModel[idx]->GetFigureAppeal() == Field::TrialModel::FIBURE_BEAUTY ) { result += addComment( SITUATION_SINGLE, Finder_SituationSingle_Appeal_Beauty, false ); }
  // はみ出し25
  if( 0.375f < m_rSituation.scr_out[idx] && m_rSituation.scr_out[idx] <= 0.50f ){ result += addComment( SITUATION_SINGLE, Finder_SituationSingle_ScreenOut_25, m_IsVerLimit ); }
  // はみ出し50
  if( 0.25f < m_rSituation.scr_out[idx] && m_rSituation.scr_out[idx] <= 0.375f ){ result += addComment( SITUATION_SINGLE, Finder_SituationSingle_ScreenOut_50, m_IsVerLimit ); }
  // はみ出し75
  if( 0.0f < m_rSituation.scr_out[idx] && m_rSituation.scr_out[idx] <= 0.25f ){ result += addComment( SITUATION_SINGLE, Finder_SituationSingle_ScreenOut_75, m_IsVerLimit ); }

  return result;
}

/**
 *  @brief  シチュエーション：複合
 */
u32 Manager::calcSituationDoubleScore( Field::TrialModel::FinderPhotoValue *pValue )
{
  // 2体とも生成されていないと計算できない
  if( !m_pModel[ 0 ] || !m_pModel[ 1 ] ){ return 0; }
  u32 result = 0;

  // Finder_SituationDouble_InScreen
  if( m_rSituation.inScreen[0] && m_rSituation.inScreen[1] ){ result += addComment( SITUATION_DOUBLE, Finder_SituationDouble_InScreen, false ); }

  return result;
}

/**
 *  @brief  シチュエーション：その他
 */
u32 Manager::calcSituationOtherScore( Field::TrialModel::FinderPhotoValue *pValue )
{
  u32 result = 0;

  GameSys::TimeZone::Zone eZone = GameSys::TimeZone::GetNowTimeZone();
  Field::weather::WeatherKind eKind = m_pWeather->GetNowWeatherKind();

  // Finder_SituationOther_Place_01
  if( m_rSpotData.spot_type == 0 ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Place_01, false ); }
  // Finder_SituationOther_Place_02
  if( m_rSpotData.spot_type == 1 ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Place_02, false ); }
  // Finder_SituationOther_Place_03
  if( m_rSpotData.spot_type == 2 ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Place_03, false ); }
  // Finder_SituationOther_Place_04
  if( m_rSpotData.spot_type == 3 ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Place_04, false ); }
  // Finder_SituationOther_Place_05
  if( m_rSpotData.spot_type == 4 ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Place_05, false ); }
  // Finder_SituationOther_Place_06
  if( m_rSpotData.spot_type == 5 ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Place_06, false ); }
  // Finder_SituationOther_Place_07
  if( m_rSpotData.spot_type == 6 ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Place_07, false ); }
  // Finder_SituationOther_Place_08
  if( m_rSpotData.spot_type == 7 ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Place_08, false ); }
  // Finder_SituationOther_Place_09
  if( m_rSpotData.spot_type == 8 ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Place_09, false ); }
  // Finder_SituationOther_Place_10
  if( m_rSpotData.spot_type == 9 ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Place_10, false ); }
  // 時間帯が朝
  if( eZone == GameSys::TimeZone::MORNING ) { result += addComment( SITUATION_OTHER, Finder_SituationOther_Time_Morning, false ); }
  // 時間帯が昼
  if( eZone == GameSys::TimeZone::NOON ) { result += addComment( SITUATION_OTHER, Finder_SituationOther_Time_Noon, false ); }
  // 時間帯が夕
  if( eZone == GameSys::TimeZone::EVENING ) { result += addComment( SITUATION_OTHER, Finder_SituationOther_Time_Evening, false ); }
  // 時間帯が夜
  if( eZone == GameSys::TimeZone::NIGHT ) { result += addComment( SITUATION_OTHER, Finder_SituationOther_Time_Night, false ); }
  // 時間帯が深夜
  if( eZone == GameSys::TimeZone::MIDNIGHT ) { result += addComment( SITUATION_OTHER, Finder_SituationOther_Time_Midnight, false ); }
  // Finder_SituationOther_Weather_Sunny
  if( eKind == Field::weather::SUNNY ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Weather_Sunny, false ); }
  // Finder_SituationOther_Weather_Cloudiness
  if( eKind == Field::weather::CLOUDINESS ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Weather_Cloudiness, false ); }
  // Finder_SituationOther_Weather_Rain
  if( eKind == Field::weather::RAIN ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Weather_Rain, false ); }
  // Finder_SituationOther_Weather_Thunderstorm
  if( eKind == Field::weather::THUNDERSTORM ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Weather_Thunderstorm, false ); }
  // Finder_SituationOther_Weather_Snow
  if( eKind == Field::weather::SNOW ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Weather_Snow, false ); }
  // Finder_SituationOther_Weather_Snowstorm
  if( eKind == Field::weather::SNOWSTORM ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Weather_Snowstorm, false ); }
  // Finder_SituationOther_Weather_Dry
  if( eKind == Field::weather::DRY ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Weather_Dry, false ); }
  // Finder_SituationOther_Weather_Sandstorm
  if( eKind == Field::weather::SANDSTORM ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Weather_Sandstorm, false ); }
  // Finder_SituationOther_Weather_Mist
  if( eKind == Field::weather::MIST ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Weather_Mist, false ); }
  // Finder_SituationOther_Weather_Sunshower
  if( eKind == Field::weather::SUNSHOWER ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Weather_Sunshower, false ); }
  // Finder_SituationOther_Weather_Diamonddust
  if( eKind == Field::weather::DIAMONDDUST ){ result += addComment( SITUATION_OTHER, Finder_SituationOther_Weather_Diamonddust, false ); }
  // 得点が1000-4999
  if( m_rSituation.score + result >= 1000 && 4999 >= m_rSituation.score + result ) { result += addComment( SITUATION_OTHER, Finder_SituationOther_Score_1000, false ); }
  // 得点が5000-9999
  if( m_rSituation.score + result >= 5000 && 9999 >= m_rSituation.score + result ) { result += addComment( SITUATION_OTHER, Finder_SituationOther_Score_5000, false ); }
  // 得点が10000-MAX
  if( m_rSituation.score + result >= 10000  ) { result += addComment( SITUATION_OTHER, Finder_SituationOther_Score_10000, false ); }
  // 無条件
  { result += addComment( SITUATION_OTHER, Finder_SituationOther_Nothing, false ); }

  return result;
}

/**
 *  @brief  コメントデータの追加
 */
u32 Manager::addComment( u32 type, u32 idx, bool bLimit )
{
  u32 result = 0;
  void *buf = NULL;
  switch( type )
  {
  case SITUATION_SINGLE:  buf = m_pSituationSingle; break;
  case SITUATION_DOUBLE:  buf = m_pSituationDouble; break;
  case SITUATION_OTHER:   buf = m_pSituationOther;  break;
  }

  SituationDataAccessor accessor( buf );
  CommentData data;
  if( accessor.GetData( idx, data ) )
  {
    // スコアの加算
    result += data.score;
    // コメントの追加
    addComment( data, bLimit );
  }

  return result;
}
void Manager::addComment( CommentData &data, bool bLimit )
{
  // コメントが設定されていない
  if( data.msgID <= 0 ){ return; }

  u32 cmd_max = 10; // 通常コメントは10種から抽選
  if( bLimit )
  {
    cmd_max = 3;  // 制限モードの場合は3種からの抽選
  }

  // 設定データを生成コメントにする
  s32 msg_add = static_cast<s32>( System::GflUse::GetPublicRand( cmd_max ) );
  s32 pri_add = static_cast<s32>( System::GflUse::GetPublicRand(  6 ) ) - 3;
  data.msgID    += msg_add;
  data.priority += pri_add;
  if( data.priority < 1 )
  {
    data.priority = 1;
  }

  // 一番小さいプライオリティをサーチ
  s32 idx = 0;
  s32 idx_pri = m_rComment[0].priority;
  for( s32 i = 1; i < Field::TrialModel::FINDER_VALUE_COMMENT_MAX; ++ i )
  {
    if( idx_pri > m_rComment[i].priority )
    {
      idx = i;
      idx_pri = m_rComment[i].priority;
    }
  }

  // コメントを入れ替える
  if( data.priority > idx_pri )
  {
    m_rComment[ idx ] = data;
  }
}

/**
 *  @brief  終了条件をチェック
 */
void Manager::checkEnd( void )
{
  // 写真の増加
#if PM_DEBUG
  if( Field::Debug::DebugTypes::s_DebugFinderParam.nInfinitPhoto == 0 )
#endif
  {
    ++ m_nSnapCount;
    // 残り枚数
    m_rUIParam.shutter_count = Field::TrialModel::FINDER_PHOTO_MAX - m_nSnapCount;
    m_pFinderMenuParam->shutter_count = m_nSnapCount;
  }
  // 試練用チェック
  if( m_TrialMonsNo != MONSNO_NULL )
  {
    // 試練指定があり該当ポケモンを撮影している
    for( u32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
    {
      // 指定ポケモンと一致すること
      if( m_rModelData[i].monsNo != m_TrialMonsNo ){ continue; }
      // 該当ポケモンが撮影されていること
      if( !m_rSituation.inScreen[i] ){ continue; }

      // 撮影できたので終了する
      m_eResult = EndResult_Trial;
      return;
    }
  }
}

/**
 *  @brief  ポケモンの動きを再開する
 */
void Manager::requestResume( void )
{
  // 再開する
  for( u32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    if( !m_pModel[ i ] ){ continue; }
    m_pModel[i]->RequestResume();
  }
}

/**
 *  @brief  BGMボリュームのリセット
 */
void Manager::resetBgmVolume( GameSys::GameManager *pGameManager )
{
  pGameManager->GetGameData()->GetFieldSound()->ChangeBGMVolume( 1.0f, Sound::BGM_FADE_FAST );
}

/**
 *  @brief  描画変更
 */
void Manager::endGraphic1( GameSys::GameManager *pGameManager )
{
  // 立体視OFF
#if defined(GF_PLATFORM_CTR)
  gfl2::gfx::GFGL::SetStereoModeEnable( false );
  pGameManager->SetEnableSkipDrawMode( m_TmpSkipDrawMode );
  nngxSetMemAccessPrioMode( NN_GX_MEM_ACCESS_PRIO_MODE_1 );
#endif // #if defined(GF_PLATFORM_CTR)

#if ENABLE_RENDER_DOF
  Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
  Field::MyRenderingPipeLine *PipeLine = pFieldmap->GetFieldRenderingPipeLineManager()->GetMyRenderingPipeLine();
  PipeLine->SetEnableDofRender( false );
#endif
}
void Manager::endGraphic2( GameSys::GameManager *pGameManager )
{
#if ENABLE_RENDER_DOF
  Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
  Field::MyRenderingPipeLine *PipeLine = pFieldmap->GetFieldRenderingPipeLineManager()->GetMyRenderingPipeLine();
  PipeLine->FinalizeDofRenderPath();
#endif
}

/**
 *  @brief  ポケモンモデル破棄準備
 */
void Manager::requestDeletePokeModel( void )
{
  // 描画から切り離す
  for( s32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    if( !m_pModel[ i ] ){ continue; }

    m_pModel[ i ]->DestroyControl();
    m_pModel[ i ]->SetDisp( false );
  }
}

/**
 *  @brief  ポケモンモデル破棄判定
 *  @retval true  安全に削除できる
 *  @retval false まだ待つ必要がある
 */
bool Manager::isSafeDeletePokeModel( void )
{
  for( s32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    if( !m_pModel[ i ] ){ continue; }

    // 安全に削除できないキャラがいる
    if( !m_pModel[ i ]->IsSafeDelete() ){ return false; }
  }
  return true;
}

/**
 *  @brief  ポケモンモデル破棄
 */
void Manager::deletePokeModel( void )
{
  for( s32 i = 0; i < Field::TrialModel::FINDER_POKE_MODEL_MAX; ++ i )
  {
    if( !m_pModel[ i ] ){ continue; }
    m_pModelFactory->DeleteFinderModel( m_pModel[ i ] );
    m_pModel[ i ] = NULL;
  }

#if PM_DEBUG
    if( m_pDebug )
    {
      m_pDebug->m_pMons1 = m_pModel[0];
      m_pDebug->m_pMons2 = m_pModel[1];
    }
#endif
}

/**
 *  @brief  ポケモンモデルシステム破棄
 */
void Manager::deletePokeSystem( void )
{
  m_pModelFactory->SetFinderMode( false );
  m_pModelFactory->DeleteSystem();
  // ポケモン作成用ヒープ：1匹目
  if( m_pPokeHeap[0] != NULL )
  {
    GFL_DELETE_HEAP( m_pPokeHeap[0] );
    m_pPokeHeap[0] = NULL;
  }
  // ポケモン作成用ヒープ：2匹目
  if( m_pPokeHeap[1] != NULL )
  {
    m_pPokeHeap[1] = NULL;
  }
}

/**
 *  @brief  メニュー破棄
 */
void Manager::deleteMenu( GameSys::GameManager *pGameManager )
{
  if( m_pUIProc )
  {
    Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
    m_pUIProc->GetFinderFrame()->EndFinder();
    pFieldmap->GetFieldSubScreenManager()->CallWindow( Field::SubScreen::WIN_MENU_MAIN );
    m_pUIProc = NULL;
  }
}

/**
 *  @brief  メニュー破棄待ち
 */
bool Manager::isDeleteMenu( GameSys::GameManager *pGameManager )
{
  Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
  // 切替が完了していない
  if( !pFieldmap->GetFieldSubScreenManager()->IsChangeComplate() ){ return false; }
  // 準備が完了した
  if( !pFieldmap->GetFieldSubScreenManager()->IsFieldMenuCreate() ){ return false; }

  return true;
}

/**
 *  @brief  カラーバッファ破棄
 */
void Manager::deleteColorBuffer( void )
{
  if( m_pColorBuffer )
  {
    GflHeapFreeMemory( m_pColorBuffer );
    m_pColorBuffer = NULL;
  }
}

/**
 *  @brief  ファイル破棄
 */
void Manager::deleteFile( void )
{
#if ENABLE_RENDER_DOF
  if( m_pRenderPath != NULL )
  {
    GflHeapFreeMemory( m_pRenderPath );
    m_pRenderPath = NULL;
  }
#endif
  if( m_pPokeData != NULL )
  {
    GflHeapFreeMemory( m_pPokeData );
    m_pPokeData = NULL;
  }
  if( m_pSubjectData != NULL )
  {
    GflHeapFreeMemory( m_pSubjectData );
    m_pSubjectData = NULL;
  }
  if( m_pSituationSingle != NULL )
  {
    GflHeapFreeMemory( m_pSituationSingle );
    m_pSituationSingle = NULL;
  }
  if( m_pSituationDouble != NULL )
  {
    GflHeapFreeMemory( m_pSituationDouble );
    m_pSituationDouble = NULL;
  }
  if( m_pSituationOther != NULL )
  {
    GflHeapFreeMemory( m_pSituationOther );
    m_pSituationOther = NULL;
  }
}

GFL_NAMESPACE_END(Finder)
GFL_NAMESPACE_END(Field)
