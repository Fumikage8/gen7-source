//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		EnvSoundManager.cpp
 *	@brief  環境音
 *	@author	tomoya takahashi
 *	@date		2015.06.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldRo/include/EnvSound/EnvSoundManager.h"

// gfl2
#include <util/include/gfl2_List.h>
#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_heap_manager.h>
#include <math/include/gfl2_math.h>

#include <debug/include/gfl2_DebugPrint.h>



// niji
#include "Sound/include/Sound.h"
#include "System/include/HeapDefine.h"

#if PM_DEBUG
#include "Field/FieldRo/include/Debug/FieldDebugCollisionRenderManager.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include <util/include/gfl2_DrawUtilText.h>
#include <gfx/include/gfl2_DrawUtil.h>
#endif  //#if PM_DEBUG



GFL_NAMESPACE_BEGIN(Field)

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

#if PM_DEBUG
f32 EnvSoundManager::PAN_CONTROL_START_COS = 10.0f;
f32 EnvSoundManager::PAN_CONTROL_END_COS = 170.0f;
f32 EnvSoundManager::PAN_CONTROL_POWER = 0.90f;
#else
const f32 EnvSoundManager::PAN_CONTROL_START_COS = 10.0f;
const f32 EnvSoundManager::PAN_CONTROL_END_COS = 170.0f;
const f32 EnvSoundManager::PAN_CONTROL_POWER = 0.90f;
#endif
//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
// 環境音の情報を表示するためのワーク
struct EnvSoundElemDebugInfo
{
  // 取っておきたい情報
  gfl2::math::Vector3 m_PlayerPos;
  gfl2::math::Vector3 m_CollisionCenterPos;
  gfl2::math::Vector3 m_PlayerOrCameraDir;
  f32                 m_PanRotDeg;
  f32                 m_Distance;
  f32                 m_DistanceMax;

};

EnvSoundElemDebugInfo * EnvSoundManager::m_pDebugInfo = NULL;
static EnvSoundElemDebugInfo m_DebugInfoWork;
bool EnvSoundManager::m_DebugInfoPlayerForm = false;
FieldEnvSoundAccessor::EnvSoundTimeZone EnvSoundManager::m_DebugInfoTimezone = FieldEnvSoundAccessor::IDX_MORNING;
#endif // PM_DEBUG

/**
 *  @class EnvSoundElem
 *  @brief  環境音音源
 */
class EnvSoundElem
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( EnvSoundElem );


public:

  /**
   *  @brief  コンストラクタ
   */
  EnvSoundElem()
  {
  }

  /**
   *  @brief  デストラクタ 
   */
  ~EnvSoundElem()
  {
  }

  /**
   *  @brief  初期化
   */
  void Initialize( const FieldEnvSoundAccessor * cpData, u32 index )
  {
    m_cpData    = cpData;
    m_DataIndex = index;
    m_cpSoundParam = NULL; 
    m_Distance  = 0;
    m_Pan       = 0;
    m_DistanceMax = 0;///< 距離最大
    m_CurveIndex = 0; 
    m_Play      = false;
    m_Update    = false;   ///< 更新があったかチェック用
    m_Enable    = true;    ///< 有効/無効フラグ
  }

  /**
   *  @brief  破棄
   */
  void Terminate()
  {
    m_cpData    = NULL;
    m_DataIndex = 0;
    m_Distance  = 0;
    m_Pan       = 0;
    m_DistanceMax = 0;///< 距離最大
    m_CurveIndex = 0; 
    m_Play      = false;
    m_Update    = false;   ///< 更新があったかチェック用
    m_Enable    = false;    ///< 有効/無効フラグ
  }

  /**
   *  @brief  配置パラメータ取得
   */
  const FieldEnvSoundAccessor::EnvSoundPlacement* GetPlacementData() const
  {
    const FieldEnvSoundAccessor::EnvSoundPlacement* cpData = m_cpData->GetPlacement( m_DataIndex );
    return cpData;
  }

  /** 
   *  @brief  今回のサウンド定義情報を取得する。
   *  @param  timezone_type 今回のタイムゾーン
   *  @param  p_out_change  サウンド定義に更新があったかを返す。
   *
   *  @retval 今回のサウンド定義情報
   */
  const FieldEnvSoundAccessor::EnvSoundParam* GetSoundParam( FieldEnvSoundAccessor::EnvSoundTimeZone timezone_type, bool playerForm,  bool* p_out_change )
  {
    const FieldEnvSoundAccessor::EnvSoundParam* ret = NULL;

    const FieldEnvSoundAccessor::EnvSoundPlacement* cpData = m_cpData->GetPlacement( m_DataIndex );
    if( cpData && m_Enable )
    {
      if( (timezone_type < FieldEnvSoundAccessor::IDX_MAX) && (cpData->data_index[timezone_type] > 0) )  // timezoneの値が不正 or サウンド定義データがない。
      {
        ret = m_cpData->GetParam( cpData->data_index[timezone_type]-1 );

        // playerFormチェック
        if( ret != NULL )
        {
          switch( ret->player_from_type )
          {
          case ENV_SOUND_PLAYER_FORM_POKERIDE_OFF:       ///< ポケモンライド中にとめる。
            if( playerForm )
            {
              ret = NULL; //再生しない。 
            }
            break;

          case ENV_SOUND_PLAYER_FORM_ALL_PLAY:           ///< 全フォルムで再生される。
          case ENV_SOUND_PLAYER_FORM_NULL:               ///< ダミーデータ(ALL_PLAYと同様の動きをする。)
          default:
            // プレイヤー状態に依存せず、全て再生する。
            break;
          }
        }

      }
    }

    if( ret != m_cpSoundParam )
    {
      *p_out_change = true;
    }
    else
    {
      *p_out_change = false;
    }

    return ret;
  }

  /**
   *  @brief  距離と向きを更新
   */
  bool UpdataParam( f32 distance, f32 distance_max, f32 dir_cos, f32 dir_cross_y, const FieldEnvSoundAccessor::EnvSoundParam* cp_SoundParam )
  {
    bool is_clear = false;

    if( cp_SoundParam )
    {
      m_cpSoundParam = cp_SoundParam;
      m_Distance      = distance;
      m_DistanceMax   = distance_max;

      // パーセンテージ更新
      f32 distance_par = 0.0f;
      if( m_DistanceMax > 0.0f )
      {
        distance_par = m_Distance / m_DistanceMax;
      }

      // Curveインデックスを計算
      m_CurveIndex = static_cast<u32>(distance_par * FieldEnvSoundAccessor::CURVE_TBL_NUM);
      m_CurveIndex = gfl2::math::Clamp( m_CurveIndex, (u32)(0), (u32)(FieldEnvSoundAccessor::CURVE_TBL_NUM-1) );

      // パンの値を求める。
      if( dir_cross_y > 0.0f )
      {
        m_Pan        = CalcCosToPan(dir_cos) * EnvSoundManager::PAN_CONTROL_POWER;
      }
      else
      {
        m_Pan        = -CalcCosToPan(dir_cos) * EnvSoundManager::PAN_CONTROL_POWER;
      }
      //TOMOYA_PRINT( "cos %f cross_y %f cos_deg %f ans %f\n", dir_cos, dir_cross_y, gfl2::math::AcosDeg(dir_cos), m_Pan );

      m_Pan = gfl2::math::Clamp( m_Pan, -1.0f, 1.0f );
    }
    else
    {
      is_clear = true;
    }


    if( is_clear )
    {
      m_cpSoundParam = NULL;
      m_Distance  = 0;
      m_DistanceMax = 0;///< 距離最大
      m_Pan       = 0;
      m_CurveIndex = 0; 
    }

    return is_clear;
  }


  /** 
   *  再生するSE取得
   */
  u32 GetPlaySE() const
  {
    if( m_cpSoundParam )
    {
      return m_cpSoundParam->SE;
    }
    GFL_ASSERT(0);
    return 0;
  }

  /**
   *  音量取得
   */
  f32 GetPlayVolume() const
  {
    if( m_cpSoundParam )
    {
      f32 num = static_cast<f32>(m_cpSoundParam->volume[m_CurveIndex]) / static_cast<f32>(FieldEnvSoundAccessor::CURVE_PARAM_MAX);
      num = gfl2::math::Clamp( num, 0.0f, 1.0f );
      return num;
    }
    return 0;
  }

  /**
   *  ピッチ取得
   */
  f32 GetPlayPitch() const
  {
    if( m_cpSoundParam )
    {
      f32 num = static_cast<f32>(m_cpSoundParam->pitch[m_CurveIndex]);
      num = pow( 2.0f, (num / static_cast<f32>(FieldEnvSoundAccessor::CURVE_PARAM_PITCH_MAX)) - 2.0f );
      num = gfl2::math::Clamp( num, 0.0f, 4.0f );
      return num;
    }
    return 0;
  }

  /**
   *  ローパス取得
   */
  f32 GetPlayLowPass() const
  {
    if( m_cpSoundParam )
    {
      f32 num = static_cast<f32>(m_cpSoundParam->lawpass[m_CurveIndex]) / static_cast<f32>(FieldEnvSoundAccessor::CURVE_PARAM_MAX);
      num = gfl2::math::Clamp( num, -1.0f, 1.0f );
      return num;
    }
    return 0;
  }

  /**
   *  ハイパス取得
   */
  f32 GetPlayHighPass() const
  {
    if( m_cpSoundParam )
    {
      f32 num = static_cast<f32>(m_cpSoundParam->highpass[m_CurveIndex]) / static_cast<f32>(FieldEnvSoundAccessor::CURVE_PARAM_MAX);
      num = gfl2::math::Clamp( num, -1.0f, 1.0f );
      return num;
    }
    return 0;
  }


  /**
   *  パン設定取得
   */
  f32 GetPlayPan() const
  {
    if( m_cpSoundParam )
    {
      return m_Pan;
    }
    return 0.0f;
  }

  /**
   *  @brief  再生フラグ
   */
  void SetPlayFlag( bool play )
  {
    m_Play = play;
  }

  /**
   *  @brief  再生フラグ取得
   */
  bool IsPlayFlag() const
  {
    return m_Play;
  }

  /**
   *  @brief  更新フラグ
   */
  void SetUpdateFlag( bool enable )
  {
    m_Update = enable;
  }

  /**
   *  @brief  更新フラグ取得
   */
  bool IsUpdateFlag() const
  {
    return m_Update;
  }

  /**
   *  @brief  データの有効無効切り替え
   */
  void SetEnableFlag( bool enable )
  {
    m_Enable = enable;
  }

  /**
   *  @brief  データの有効無効フラグ取得
   */
  bool IsEnableFlag()const
  {
    return m_Enable;
  }

  /**
   *  @brief データインデックスの取得
   */
  u32 GetIndex() const
  {
    return m_DataIndex;
  }

  /**
   *  @brief  cosの値から、パンの数値を決定する。
   */
  f32 CalcCosToPan( f32 cos )
  {
    f32 start_cos = gfl2::math::CosDeg( EnvSoundManager::PAN_CONTROL_START_COS );
    f32 end_cos = gfl2::math::CosDeg( EnvSoundManager::PAN_CONTROL_END_COS );
    f32 length_max = gfl2::math::Abs(end_cos - start_cos);

    if( (start_cos >= cos) && (end_cos <= cos) )
    {
      f32 cos_len = gfl2::math::Abs(cos - start_cos);
      return gfl2::math::SinDeg( (cos_len / length_max) * 180.0f );
    }
    else
    {
      return 0.0f;
    }
  }

  const FieldEnvSoundAccessor::EnvSoundParam* GetUseSoundParam()
  {
    return m_cpSoundParam;
  }
 

private:
  const FieldEnvSoundAccessor *  m_cpData;     ///< 音源データ 
  u32           m_DataIndex;   ///< 音源データIndex
  const FieldEnvSoundAccessor::EnvSoundParam* m_cpSoundParam; ///< 今再生すべき音情報
  f32           m_Distance;   ///< 音源までの距離
  f32           m_DistanceMax;///< 距離最大
  u32           m_CurveIndex; ///< Curveテーブルインデックス
  f32           m_Pan;        ///< パンの値
  bool          m_Play;       ///< 現在再生中か
  bool          m_Update;   ///< 更新があったかチェック用

  bool          m_Enable;    ///< 有効無効フラグ  初期値有効

};







/**
 *  @brief  コンストラクタ
 */
EnvSoundManager::EnvSoundManager() : 
   m_pAccessor(NULL)
   ,m_pSoundElemList(NULL)
   ,m_pPlaySoundElemList(NULL)
   
{
}

/**
 *  @brief  デストラクタ
 */
EnvSoundManager::~EnvSoundManager()
{
  this->Terminate();
}

/**
 *  @brief  初期化
 */
void EnvSoundManager::Initialize( gfl2::heap::HeapBase* pHeap, const void * cpBuff )
{
  m_pSoundElemList      = GFL_NEW(pHeap) gfl2::util::List<EnvSoundElem*>(pHeap, ENV_SOUND_MANAGER_NUM);
  m_pPlaySoundElemList  = GFL_NEW(pHeap) gfl2::util::List<EnvSoundElem*>(pHeap, ENV_SOUND_MANAGER_NUM);


  // cpBuffから音源要素リストを作成
  m_pAccessor = GFL_NEW(pHeap) FieldEnvSoundAccessor();
  m_pAccessor->Initialize(cpBuff);

  // 配置データの生成
  //TOMOYA_PRINT( "EnvSoundPlacementDataMax %d\n", m_pAccessor->GetPlacementMax() );
  for( u32 i=0; i<m_pAccessor->GetPlacementMax(); ++i )
  {
    EnvSoundElem* pElem = GFL_NEW(pHeap) EnvSoundElem();
    pElem->Initialize( m_pAccessor, i );
    m_pSoundElemList->PushBack(pElem);
  }

#if PM_DEBUG
  // デバッグ用の情報保持ワークを用意
  {
    gfl2::heap::HeapBase * pDebug = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    m_pDebugInfo = (EnvSoundElemDebugInfo*)GflHeapAllocMemory(pDebug, sizeof(EnvSoundElemDebugInfo) *ENV_SOUND_MANAGER_NUM);
  }
#endif
}

/**
 *  @brief  破棄
 */
void EnvSoundManager::Terminate()
{
  if(m_pSoundElemList)
  {
#if PM_DEBUG
    GflHeapFreeMemory( m_pDebugInfo );
    m_pDebugInfo = NULL;
#endif

    // 音源要素を全て破棄。
    gfl2::util::List<EnvSoundElem*>::Iterator elem = m_pSoundElemList->Begin();


    // 音源破棄
    for( ; elem != m_pSoundElemList->End(); ++elem )
    {
      // 再生中であれば停止
      stopElem(*elem);

      GFL_DELETE(*elem);
    }

    // アクセサー破棄
    GFL_SAFE_DELETE( m_pAccessor );

    // リスト破棄 
    GFL_SAFE_DELETE( m_pPlaySoundElemList );
    GFL_SAFE_DELETE( m_pSoundElemList );
      
      
  }
}


/**
 *  @brief  更新
 */
void EnvSoundManager::Update( const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate_player, const gfl2::math::Quaternion& rotate_camera, bool playerForm, FieldEnvSoundAccessor::EnvSoundTimeZone timezone )
{
  if( m_pSoundElemList ){
    this->updateElem( pos, rotate_player, rotate_camera, playerForm, timezone );
  }

#if PM_DEBUG
  m_DebugInfoPlayerForm = playerForm;
  m_DebugInfoTimezone = timezone;
#endif
}


/**
 *  @brief  環境音の再生/停止
 *          初期値は再生
 */
void EnvSoundManager::SetEnableElem( bool enable_flag, u32 se_id )
{
  if( se_id == SE_ALL )
  {
    gfl2::util::List<EnvSoundElem*>::Iterator elem = m_pSoundElemList->Begin();

    // 音源の無効化
    for( ; elem != m_pSoundElemList->End(); ++elem )
    {
      (*elem)->SetEnableFlag(enable_flag);
    }
  }
  else
  {
    gfl2::util::List<EnvSoundElem*>::Iterator elem = m_pSoundElemList->Begin();

    // 音源の無効化
    for( ; elem != m_pSoundElemList->End(); ++elem )
    {
      if( (*elem)->GetIndex() == se_id )
      {
        (*elem)->SetEnableFlag(enable_flag);
        return ;
      }
    }

    GFL_ASSERT_MSG( 0, "SetEnableElem se_id[%d]が無効", se_id );
  }
}

//===============================================================================================================
//    private:
//===============================================================================================================
/**
 *  @brief  今のフレーム再生すべき音源を更新する。
 *  
 *  @param  pos         プレイヤー位置
 *  @param  rotate      プレイヤー方向
 */
void EnvSoundManager::updateElem( const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate_player, const gfl2::math::Quaternion& rotate_camera, bool playerForm, FieldEnvSoundAccessor::EnvSoundTimeZone timezone )
{
  gfl2::util::List<EnvSoundElem*>::Iterator elem = m_pSoundElemList->Begin();

  // 音源の更新
  for( ; elem != m_pSoundElemList->End(); ++elem )
  {
    f32 distance, distance_max, dir_cos, dir_cross_y;

    // 音源とのヒット判定
    if( this->checkElem( *elem, pos, rotate_player, rotate_camera, &distance, &distance_max, &dir_cos, &dir_cross_y ) )
    {
      addPlayList( *elem, playerForm, distance, distance_max, dir_cos, dir_cross_y, timezone );
    }
  }

  // 更新が発生しなかった音源の再生をとめる。
  gfl2::util::List<EnvSoundElem*>::Iterator play_elem = m_pPlaySoundElemList->Begin();
  u32 playCount = 0;
  for( ; play_elem != m_pPlaySoundElemList->End();  )
  {
    // 更新されなかったもの、再生数が３以上になった場合は破棄
    if( (!(*play_elem)->IsUpdateFlag()) || (playCount >= ENV_SOUND_MANAGER_PLAY_LIST_MAX) )
    {
      play_elem = removePlayList( play_elem );
    }
    else
    {
      updatePlayList( *play_elem );
      ++play_elem;
      ++playCount;
    }
  }
}


/**
 *  @brief  音源情報とのヒット判定を行なう。
 */
bool EnvSoundManager::checkElem( const EnvSoundElem* cpElem, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate_player, const gfl2::math::Quaternion& rotate_camera, f32 * p_distance, f32 * p_distance_max, f32 * p_dir_cos, f32* p_dir_cross_y )
{
  // コリジョン情報の取得
  const FieldEnvSoundAccessor::EnvSoundPlacement* cpData = cpElem->GetPlacementData();
  if(cpData==NULL){ return false; }
  const void * cpCol = NULL;
  cpCol = m_pAccessor->GetCollisionData(*cpData);
  if(cpCol==NULL){ return false; }

  FieldEnvSoundCollisionAccessor accessor;
  accessor.Initialize(cpCol);



  // カメラ依存かプレイヤー依存か？
  gfl2::math::Quaternion rotate;
  switch( cpData->pan_type )
  {
  case FieldEnvSoundAccessor::PAN_TYPE_CAMERA: ///< カメラ依存
    {
      // Y軸180度回転
      rotate = rotate_camera * gfl2::math::Quaternion(gfl2::math::Vector3(0,1.0f,0), gfl2::math::ConvDegToRad(180.0f));
    }
    break;
  case FieldEnvSoundAccessor::PAN_TYPE_PLAYER: ///< キャラクター依存
    {
      rotate = rotate_player;
    }
    break;
  case FieldEnvSoundAccessor::PAN_TYPE_NONE:   ///< パン操作なし
    {
      rotate = rotate_player; // 角度で算出するが最終的に値は参照しない。
    }
    break;
  }


  //TOMOYA_PRINT( "elem %p\n", cpElem )
#if PM_DEBUG
  {
    bool is_change = false;
    const FieldEnvSoundAccessor::EnvSoundParam* cp_sound = const_cast<EnvSoundElem*>(cpElem)->GetSoundParam( FieldEnvSoundAccessor::IDX_NOON, true, &is_change);
    if( cp_sound ){
      //TOMOYA_PRINT( "noon se %d\n", cp_sound->SE );
    }
  }
#endif

  // 順にチェックしていき、最も距離の近い１つのコリジョンを選択する。
  f32 min_distance = FLT_MAX;
  f32 min_distance_max = FLT_MAX;
  f32 min_dir_cos = FLT_MAX;
  f32 min_dir_cross_y = FLT_MAX;
  bool ret_val = false;

  for( u32 i=0; i<accessor.GetDataMax(); ++i )
  {
    FieldEnvSoundCollisionAccessor::CollisionData data;
    f32 distance = FLT_MAX;
    f32 distance_max = FLT_MAX;
    f32 dir_cos = FLT_MAX;
    f32 dir_cross_y = FLT_MAX;
    bool is_hit = false;

    if( accessor.GetData( i, &data ) )
    {
      switch( data.type )
      {
      case FieldEnvSoundCollisionAccessor::CYLINDER:
        is_hit= checkElemCylinder( data.cylinder, pos, rotate, &distance, &distance_max, &dir_cos, &dir_cross_y );
        break;

      case FieldEnvSoundCollisionAccessor::LINE:
        is_hit= checkElemLine( data.line, pos, rotate, &distance, &distance_max, &dir_cos, &dir_cross_y );
        break;

      case FieldEnvSoundCollisionAccessor::SPHERE:
        is_hit= checkElemSphere( data.sphere, pos, rotate, &distance, &distance_max, &dir_cos, &dir_cross_y );
        break;

      case FieldEnvSoundCollisionAccessor::PIPE_LINE:
        is_hit= checkElemPipeLine( data.pipe_line, pos, rotate, &distance, &distance_max, &dir_cos, &dir_cross_y );
        break;
      }
    }
    
    // チェックに引っかかった場合
    if( is_hit && (min_distance > distance) )
    {
      //TOMOYA_PRINT( "Hit Min ColIndex[%d] type [%d] distance[%f] dir_cos[%f] dir_cross[%f]\n", i, data.type, distance, dir_cos, dir_cross_y );

      min_distance      = distance;
      min_distance_max  = distance_max;
      min_dir_cos       = dir_cos;
      min_dir_cross_y   = dir_cross_y;
      ret_val           = true;         

#if PM_DEBUG
      if( cpElem->GetIndex() < ENV_SOUND_MANAGER_NUM )
      {
        u32 elem_index = cpElem->GetIndex();
        m_pDebugInfo[elem_index] = m_DebugInfoWork;
      }
#endif
    }
  }

  if( ret_val )
  {
    *p_distance       = min_distance;
    *p_distance_max   = min_distance_max;

    if( cpData->pan_type == FieldEnvSoundAccessor::PAN_TYPE_NONE )
    {
      // NONEの場合はPAN操作はなし
      *p_dir_cos        = 1.0f;
      *p_dir_cross_y    = 1.0f;

#if PM_DEBUG
      if( cpElem->GetIndex() < ENV_SOUND_MANAGER_NUM )
      {
        u32 elem_index = cpElem->GetIndex();
        m_pDebugInfo[elem_index].m_PanRotDeg = 0.0f;
      }
#endif
    }
    else
    {
      *p_dir_cos        = min_dir_cos;
      *p_dir_cross_y    = min_dir_cross_y;
    }
  }

  return ret_val;
}


/**
 *  @brief  個別ヒット判定「Cylinder」
 */
bool EnvSoundManager::checkElemCylinder( const FieldEnvSoundCollisionAccessor::Cylinder& colData, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate, f32 * p_distance, f32 * p_distance_max, f32 * p_dir_cos, f32* p_dir_cross_y )
{

  // 半径範囲内か？
  gfl2::math::Vector3 cylinder( colData.center[0], 0.0f, colData.center[2] );
  gfl2::math::Vector3 distance = cylinder - pos;

  //TOMOYA_PRINT( "Cylinder center (%f,%f,%f) length(%f) player (%f,%f,%f)\n", colData.center[0], colData.center[1], colData.center[2], colData.radius, pos.GetX(), pos.GetY(), pos.GetZ() );

  distance.SetY(0.0f);
  *p_distance = distance.Length();
  if( *p_distance > colData.radius ){ return false; }

  // 高さ範囲ないか？
  if( ((colData.height + colData.center[1]) < pos.GetY()) ||
      ((colData.center[1]) > pos.GetY()) )
  {
    return false;
  }

  // ヒットした。
  *p_distance_max = colData.radius;

  // 中心方向のベクトルと、プレイヤーかカメラ向きの角度と外積を求める。
  // どちらも平面で行なう。
  gfl2::math::Vector3 player_rot(0.0f,0.0f,1.0f);
  gfl2::math::Matrix34 rotMtx;
  rotate.QuaternionToMatrix(rotMtx);
  player_rot = rotMtx * player_rot;
  player_rot.SetY(0.0f);
  if( player_rot.LengthSq() > 0.0f ) // Y軸に値が入っているか。
  {
    player_rot = player_rot.Normalize();
  }
  else
  {
    player_rot.Set(0,0,1.0f); 
  }

  f32 length = distance.Length();
  if(length>0.0f)
  {
    *p_dir_cos = distance.Dot( player_rot );
    *p_dir_cos /= length;
    gfl2::math::Vector3 cross = distance.Cross( player_rot );
    *p_dir_cross_y = cross.GetY();
  }
  else
  {
    // プレイヤーと音源位置が一緒
    *p_dir_cos = 1.0f;
    *p_dir_cross_y = 1.0f;
  }


#if PM_DEBUG
  m_DebugInfoWork.m_PlayerPos                  = pos;
  m_DebugInfoWork.m_CollisionCenterPos         = cylinder;
  m_DebugInfoWork.m_PlayerOrCameraDir          = player_rot;
  m_DebugInfoWork.m_PanRotDeg                  = gfl2::math::AcosDeg(*p_dir_cos);
  m_DebugInfoWork.m_Distance                   = *p_distance;
  m_DebugInfoWork.m_DistanceMax                = *p_distance_max;
#endif

  //TOMOYA_PRINT( "dist vec(%f,%f)  player(%f,%f) cross Y %f cos %f \n", distance.GetX(), distance.GetZ(), player_rot.GetX(), player_rot.GetZ(), *p_dir_cross_y, *p_dir_cos );

  return true;
}

/**
 *  @brief  個別ヒット判定「Sphere」
 */
bool EnvSoundManager::checkElemSphere( const FieldEnvSoundCollisionAccessor::Sphere& colData, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate, f32 * p_distance, f32 * p_distance_max, f32 * p_dir_cos, f32* p_dir_cross_y )
{

  // 半径範囲内か？
  gfl2::math::Vector3 sphere( colData.center[0], colData.center[1], colData.center[2] );
  gfl2::math::Vector3 distance = sphere - pos;

  //TOMOYA_PRINT( "Cylinder center (%f,%f) length(%f) player (%f,%f)\n", colData.center[0], colData.center[2], colData.radius, pos.GetX(), pos.GetZ() );

  *p_distance = distance.Length();
  if( *p_distance > colData.radius ){ return false; }

  // ヒットした。
  *p_distance_max = colData.radius;

  // 中心方向のベクトルと、プレイヤーかカメラ向きの角度と外積を求める。
  // どちらも平面で行なう。
  gfl2::math::Vector3 player_rot(0.0f,0.0f,1.0f);
  gfl2::math::Matrix34 rotMtx;
  rotate.QuaternionToMatrix(rotMtx);
  player_rot = rotMtx * player_rot;
  player_rot.SetY(0.0f);
  if( player_rot.LengthSq() > 0.0f ) // Y軸に値が入っているか。
  {
    player_rot = player_rot.Normalize();
  }
  else
  {
    player_rot.Set(0,0,1.0f); 
  }

  distance.SetY(0.0f);
  f32 length = distance.Length();
  if( length > 0.0f )
  {
    *p_dir_cos = distance.Dot( player_rot );
    *p_dir_cos /= length;
    gfl2::math::Vector3 cross = distance.Cross( player_rot );
    *p_dir_cross_y = cross.GetY();
  }
  else
  {
    // プレイヤーと音源位置が一緒
    *p_dir_cos = 1.0f;
    *p_dir_cross_y = 1.0f;
  }

  //TOMOYA_PRINT( "dist vec(%f,%f)  player(%f,%f) cross Y %f cos %f \n", distance.GetX(), distance.GetZ(), player_rot.GetX(), player_rot.GetZ(), *p_dir_cross_y, *p_dir_cos );

#if PM_DEBUG
  m_DebugInfoWork.m_PlayerPos                  = pos;
  m_DebugInfoWork.m_CollisionCenterPos         = sphere;
  m_DebugInfoWork.m_PlayerOrCameraDir          = player_rot;
  m_DebugInfoWork.m_PanRotDeg                  = gfl2::math::AcosDeg(*p_dir_cos);
  m_DebugInfoWork.m_Distance                   = *p_distance;
  m_DebugInfoWork.m_DistanceMax                = *p_distance_max;
#endif

  return true;
}

/**
 *  @brief  個別ヒット判定「Line」
 */
bool EnvSoundManager::checkElemLine( const FieldEnvSoundCollisionAccessor::Line& colData, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate, f32 * p_distance, f32 * p_distance_max, f32 * p_dir_cos, f32* p_dir_cross_y )
{

  // 高さが範囲内かチェック
  //TOMOYA_PRINT( "height min %f max %f height %f player %f\n", colData.startpos[1], (colData.startpos[1] + colData.height), colData.height,  pos.GetY() );
  if( ((colData.startpos[1] + colData.height) < pos.GetY()) ||
      (colData.startpos[1] > pos.GetY()) )
  {
    return false;
  }

  // 始点と終点を求める。
  gfl2::math::Vector3 start_pos( colData.startpos[0], pos.GetY(), colData.startpos[2] );  // 始点
  gfl2::math::Vector3 end_pos( colData.endpos[0], pos.GetY(), colData.endpos[2] );    // 終点
  gfl2::math::Vector3 box_line;   // ボックスのライン方向
  gfl2::math::Vector3 player_line;// 始点からプレイヤー座標へのベクトル

  // 始点、終点　始点から終点へのベクトル　始点からプレイヤーへのベクトルを算出
  box_line  = end_pos - start_pos;
  player_line = pos - start_pos;
  f32 box_line_length = box_line.Length();

  //TOMOYA_PRINT( "Box start(%f,%f,%f) end(%f,%f,%f) player (%f,%f,%f)\n", start_pos.GetX(), start_pos.GetY(), start_pos.GetZ(), end_pos.GetX(), end_pos.GetY(), end_pos.GetZ(), pos.GetX(), pos.GetY(), pos.GetZ() );

  // box_lineの長さが0.0ならヒットしない。
  if( gfl2::math::IsSimilar( box_line_length, 0.0f ) ){ return false; }
  box_line = box_line.Normalize(); //player_lineの長さオンリーにしたいので正規化


  // 線分の領域内かチェック
  gfl2::math::Vector3 distance_vec(0,0,0);
  f32 dot =box_line.Dot(player_line); // box_lineに投影
  if( (dot >= 0.0f) && (dot <= box_line_length) )
  {
    // 領域内の場合　−−−　線分と直行する方向のベクトルが交差する座標との距離を求める。
    gfl2::math::Vector3 cross_pos = box_line;
    cross_pos = (cross_pos * dot) + start_pos;  // 交点

#if PM_DEBUG
    m_DebugInfoWork.m_CollisionCenterPos         = cross_pos;
#endif

    distance_vec = cross_pos - pos;
    distance_vec.SetY(0.0f);
    *p_distance = distance_vec.Length();        // ラインとの距離算出完了

    //TOMOYA_PRINT( "cross pos (%f,%f,%f)\n", cross_pos.GetX(), cross_pos.GetY(), cross_pos.GetZ() );
  }
  else
  {
    // 領域外の場合　−−−　始点or終点との距離を求め、ボックスの幅内かチェック
    if( dot <= 0.0f )
    {
      // 始点との距離を見る。
      distance_vec = start_pos - pos;

#if PM_DEBUG
      m_DebugInfoWork.m_CollisionCenterPos         = start_pos;
#endif
      //TOMOYA_PRINT( "cross start pos (%f,%f,%f)\n", start_pos.GetX(), start_pos.GetY(), start_pos.GetZ() );
    }
    else
    {
      // 終点との距離を見る。
      distance_vec = end_pos - pos;
      //TOMOYA_PRINT( "cross end pos (%f,%f,%f)\n", end_pos.GetX(), end_pos.GetY(), end_pos.GetZ() );
#if PM_DEBUG
      m_DebugInfoWork.m_CollisionCenterPos         = end_pos;
#endif
    }

    distance_vec.SetY(0.0f);
    *p_distance = distance_vec.Length();        // ラインとの距離算出完了
  }

  // 幅の大きさと距離を比較
  *p_distance_max = colData.width;
  
  //TOMOYA_PRINT( "cross distance (%f,%f,%f) dist %f dist_max %f\n", distance_vec.GetX(), distance_vec.GetY(), distance_vec.GetZ(), *p_distance, *p_distance_max );
  if( *p_distance > *p_distance_max ) // 幅領域外
  {
    return false;
  }

  // ヒットした
  // 中心方向のベクトルと、プレイヤーかカメラ向きの角度と外積を求める。
  // どちらも平面で行なう。
  gfl2::math::Vector3 player_rot(0.0f,0.0f,1.0f);
  gfl2::math::Matrix34 playerRotMtx;
  rotate.QuaternionToMatrix(playerRotMtx);
  player_rot = playerRotMtx * player_rot;
  player_rot.SetY(0.0f);
  if( player_rot.LengthSq() > 0.0f ) // Y軸に値が入っているか。
  {
    player_rot = player_rot.Normalize();
  }
  else
  {
    player_rot.Set(0,0,1.0f); 
  }

  f32 length = distance_vec.Length();
  if( length ){
    *p_dir_cos = distance_vec.Dot( player_rot );
    *p_dir_cos /= length;
    gfl2::math::Vector3 cross = distance_vec.Cross( player_rot );
    *p_dir_cross_y = cross.GetY();
  }
  else
  {
    // プレイヤーと音源位置が一緒
    *p_dir_cos = 1.0f;
    *p_dir_cross_y = 1.0f;
  }

#if PM_DEBUG
  m_DebugInfoWork.m_PlayerPos                  = pos;
  m_DebugInfoWork.m_PlayerOrCameraDir          = player_rot;
  m_DebugInfoWork.m_PanRotDeg                  = gfl2::math::AcosDeg(*p_dir_cos);
  m_DebugInfoWork.m_Distance                   = *p_distance;
  m_DebugInfoWork.m_DistanceMax                = *p_distance_max;
#endif

  //TOMOYA_PRINT( "dist vec(%f,%f)  player(%f,%f) cross Y %f cos %f \n", distance_vec.GetX(), distance_vec.GetZ(), player_rot.GetX(), player_rot.GetZ(), *p_dir_cross_y, *p_dir_cos );
  
  return true;
}

/**
 *  @brief  個別ヒット判定「PipeLine」
 */
bool EnvSoundManager::checkElemPipeLine( const FieldEnvSoundCollisionAccessor::PipeLine& colData, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rotate, f32 * p_distance, f32 * p_distance_max, f32 * p_dir_cos, f32* p_dir_cross_y )
{

  // 始点と終点を求める。
  gfl2::math::Vector3 start_pos( colData.startpos[0], colData.startpos[1], colData.startpos[2] );  // 始点
  gfl2::math::Vector3 end_pos( colData.endpos[0], colData.endpos[1], colData.endpos[2] );    // 終点
  gfl2::math::Vector3 box_line;   // ボックスのライン方向
  gfl2::math::Vector3 player_line;// 始点からプレイヤー座標へのベクトル

  // 始点、終点　始点から終点へのベクトル　始点からプレイヤーへのベクトルを算出
  box_line  = end_pos - start_pos;
  player_line = pos - start_pos;
  f32 box_line_length = box_line.Length();

  //TOMOYA_PRINT( "Box start(%f,%f,%f) end(%f,%f,%f) player (%f,%f,%f)\n", start_pos.GetX(), start_pos.GetY(), start_pos.GetZ(), end_pos.GetX(), end_pos.GetY(), end_pos.GetZ(), pos.GetX(), pos.GetY(), pos.GetZ() );

  // box_lineの長さが0.0ならヒットしない。
  if( gfl2::math::IsSimilar( box_line_length, 0.0f ) ){ return false; }
  box_line = box_line.Normalize(); //player_lineの長さオンリーにしたいので正規化


  // 線分の領域内かチェック
  gfl2::math::Vector3 distance_vec(0,0,0);
  f32 dot =box_line.Dot(player_line); // box_lineに投影
  if( (dot >= 0.0f) && (dot <= box_line_length) )
  {
    // 領域内の場合　−−−　線分と直行する方向のベクトルが交差する座標との距離を求める。
    gfl2::math::Vector3 cross_pos = box_line;
    cross_pos = (cross_pos * dot) + start_pos;  // 交点

#if PM_DEBUG
    m_DebugInfoWork.m_CollisionCenterPos         = cross_pos;
#endif

    distance_vec = cross_pos - pos;
    *p_distance = distance_vec.Length();        // ラインとの距離算出完了

    //TOMOYA_PRINT( "cross pos (%f,%f,%f)\n", cross_pos.GetX(), cross_pos.GetY(), cross_pos.GetZ() );
  }
  else
  {
    // 領域外の場合　−−−　始点or終点との距離を求め、ボックスの幅内かチェック
    if( dot <= 0.0f )
    {
      // 始点との距離を見る。
      distance_vec = start_pos - pos;

#if PM_DEBUG
    m_DebugInfoWork.m_CollisionCenterPos         = start_pos;
#endif

      //TOMOYA_PRINT( "cross start pos (%f,%f,%f)\n", start_pos.GetX(), start_pos.GetY(), start_pos.GetZ() );
    }
    else
    {
      // 終点との距離を見る。
      distance_vec = end_pos - pos;

#if PM_DEBUG
    m_DebugInfoWork.m_CollisionCenterPos         = end_pos;
#endif

      //TOMOYA_PRINT( "cross end pos (%f,%f,%f)\n", end_pos.GetX(), end_pos.GetY(), end_pos.GetZ() );
    }

    *p_distance = distance_vec.Length();        // ラインとの距離算出完了
  }

  // 幅の大きさと距離を比較
  *p_distance_max = colData.width;
  
  //TOMOYA_PRINT( "cross distance (%f,%f,%f) dist %f dist_max %f\n", distance_vec.GetX(), distance_vec.GetY(), distance_vec.GetZ(), *p_distance, *p_distance_max );
  if( *p_distance > *p_distance_max ) // 幅領域外
  {
    return false;
  }

  // ヒットした
  // 中心方向のベクトルと、プレイヤーかカメラ向きの角度と外積を求める。
  // どちらも平面で行なう。
  gfl2::math::Vector3 player_rot(0.0f,0.0f,1.0f);
  gfl2::math::Matrix34 playerRotMtx;
  rotate.QuaternionToMatrix(playerRotMtx);
  player_rot = playerRotMtx * player_rot;
  player_rot.SetY(0.0f);
  if( player_rot.LengthSq() > 0.0f ) // Y軸に値が入っているか。
  {
    player_rot = player_rot.Normalize();
  }
  else
  {
    player_rot.Set(0,0,1.0f); 
  }

  distance_vec = distance_vec.SetY(0.0f);
  f32 length = distance_vec.Length();
  if( length > 0.0f )
  {
    *p_dir_cos = distance_vec.Dot( player_rot );
    *p_dir_cos /= length;
    gfl2::math::Vector3 cross = distance_vec.Cross( player_rot );
    *p_dir_cross_y = cross.GetY();
  }
  else
  {
    // プレイヤーと音源位置が一緒
    *p_dir_cos = 1.0f;
    *p_dir_cross_y = 1.0f;
  }

#if PM_DEBUG
  m_DebugInfoWork.m_PlayerPos                  = pos;
  m_DebugInfoWork.m_PlayerOrCameraDir          = player_rot;
  m_DebugInfoWork.m_PanRotDeg                  = gfl2::math::AcosDeg(*p_dir_cos);
  m_DebugInfoWork.m_Distance                   = *p_distance;
  m_DebugInfoWork.m_DistanceMax                = *p_distance_max;
#endif

  //TOMOYA_PRINT( "dist vec(%f,%f)  player(%f,%f) cross Y %f cos %f \n", distance_vec.GetX(), distance_vec.GetZ(), player_rot.GetX(), player_rot.GetZ(), *p_dir_cross_y, *p_dir_cos );
  
  return true;
}


/**
 *  @brief  音源情報を更新する。
 */
void EnvSoundManager::addPlayList( EnvSoundElem* pElem, bool playerForm, f32 distance, f32 distance_max, f32 dir_cos, f32 dir_cross_y, FieldEnvSoundAccessor::EnvSoundTimeZone timezone )
{
  //pElemこのフレームの
  const FieldEnvSoundAccessor::EnvSoundParam* cp_SoundParam = NULL;
  bool is_change;
  bool is_playing = pElem->IsPlayFlag();

  cp_SoundParam = pElem->GetSoundParam( timezone, playerForm, &is_change );

  // 再生停止処理
  if( is_change )
  {
    // 今の物を止める。
    stopElem( pElem );
  }

  //pElemにパラメータ登録
  pElem->UpdataParam( distance, distance_max, dir_cos, dir_cross_y, cp_SoundParam );
  
  //音定義があれば、更新フラグを立てる。
  //音定義が無ければそこで終わる。
  if( cp_SoundParam )
  {

    // 本当に新規に再生開始のとき
    // 音の再生リストに追加
    if( !is_playing )
    {
      // 優先度から登録する場所を決める。
      gfl2::util::List<EnvSoundElem*>::Iterator play_elem = m_pPlaySoundElemList->Begin();
      bool is_insert = false;
      for( ; play_elem != m_pPlaySoundElemList->End();  )
      {
        //TOMOYA_PRINT( "list pri %d  elem pri %d\n", (*play_elem)->GetPlacementData()->priority, pElem->GetPlacementData()->priority );
       
        if( (*play_elem)->GetPlacementData()->priority >= pElem->GetPlacementData()->priority )
        {
          //TOMOYA_PRINT( "Insert Elem" );
          m_pPlaySoundElemList->Insert(play_elem, pElem);
          is_insert = true;
          break;
        }

        ++play_elem;
      }
      if( !is_insert ) 
      {
        //TOMOYA_PRINT( "PushBack Elem" );
        m_pPlaySoundElemList->PushBack(pElem);
      }

      // 再生
      playElem( pElem );
    }
    else if( is_change ) // @fix NMCat[1888] フィールド上で時間帯が切り替わるとポケボイス環境音（ポケモンの鳴き声）が鳴らなくなる事について
    {
      // is_changeがtrue = stopElemしているので再度Playにする。
      // 再生
      playElem( pElem );
    }
    
    //更新フラグを立てる。
    pElem->SetUpdateFlag(true);
  }
  else
  {
    // 次のサウンド定義が無い。
  }
}



/**
 *  @brief  更新処理
 */
void EnvSoundManager::updatePlayList( EnvSoundElem* pElem )
{
  if( pElem->IsPlayFlag() )
  {
    //TOMOYA_PRINT( "Update SE[%d] volume[%f] pitch[%f] pan[%f]\n", pElem->GetPlaySE(), pElem->GetPlayVolume(), pElem->GetPlayPitch(), pElem->GetPlayPan() );

    Sound::ChangeSEVolume( pElem->GetPlaySE(), pElem->GetPlayVolume() );
    Sound::ChangeSEPitch( pElem->GetPlaySE(), pElem->GetPlayPitch() );
    Sound::ChangeSEPan( pElem->GetPlaySE(), pElem->GetPlayPan() );
  }

  // 次回更新が必要かどうかチェックのために落とす。
  pElem->SetUpdateFlag(false);
}

/**
 *  @brief  音源情報を削除する
 */
gfl2::util::List<EnvSoundElem*>::Iterator EnvSoundManager::removePlayList( gfl2::util::List<EnvSoundElem*>::Iterator pElem )
{
  gfl2::util::List<EnvSoundElem*>::Iterator ret = m_pPlaySoundElemList->Erase(pElem);
  //音を停止
  stopElem( *pElem );
  
  return ret;
}

/** 
 *  @brief  音源の音を一時停止する。
 */
void EnvSoundManager::stopElem( EnvSoundElem* pElem )
{
  if( pElem->IsPlayFlag() )
  {
    Sound::StopSE( pElem->GetPlaySE(), ENV_SOUND_SE_STOP_FADE_TIME );
    pElem->SetPlayFlag(false);
  }
}

/** 
 *  @brief  音源の再生を開始する。
 */
void EnvSoundManager::playElem( EnvSoundElem* pElem )
{
  if( !pElem->IsPlayFlag() )
  {
    Sound::PlayInfo info;
    info.volume = pElem->GetPlayVolume();
    info.pitch  = pElem->GetPlayPitch();
    info.pan    = pElem->GetPlayPan();

    Sound::PlaySE( pElem->GetPlaySE(), info );
    pElem->SetPlayFlag(true);
  }
}

#if PM_DEBUG
/** 
  *  @brief  環境音コリジョンをデバッグ表示へ登録
  *  @param  color   表示色
  *  @param  onlyPlayFlag  再生中のコリジョンのみ表示
  *  @param  onlyActiveFlag true:アクティブなコリジョンのみ表示する false：全て表示する
  */
void EnvSoundManager::DebugRegistRenderCollision( gfl2::math::Vector4 color, b32 onlyPlayFlag, b32 onlyActiveFlag )
{
  if( m_pSoundElemList == NULL ) return;

  gfl2::util::List<EnvSoundElem*>::Iterator elem = m_pSoundElemList->Begin();

  // 音源の更新
  for( ; elem != m_pSoundElemList->End(); ++elem )
  {
    // 無効データの表示をカット
    if( onlyActiveFlag && !(*elem)->IsEnableFlag() )
    {
      continue;
    }

    // 再生していないデータの表示をカット
    if( onlyPlayFlag && !(*elem)->IsPlayFlag() )
    {
      continue;
    }

    // コリジョン情報の取得
    const FieldEnvSoundAccessor::EnvSoundPlacement* cpData = static_cast<const EnvSoundElem*>(*elem)->GetPlacementData();
    if(cpData == NULL)
    {
      continue;
    }
    const void * cpCol = NULL;
    cpCol = m_pAccessor->GetCollisionData(*cpData);
    if(cpCol == NULL)
    {
      continue;
    }
    // デバッグコリジョン表示マネージャーに登録
    Field::Debug::DebugCollisionRenderManager* pManager = GFL_SINGLETON_INSTANCE( Field::Debug::DebugCollisionRenderManager );
    if( pManager )
    {
      pManager->RegistRenderEnvSoundCollision( cpCol, color );
    }
  }
}



/**
 *  @brief  環境音の情報を画面に表示
 */
#define VEC4_COLOR_WHITE gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f )
void EnvSoundManager::DebugDrawEnvSoundManagerInfo( gfl2::util::DrawUtilText* pDrawUtilText, const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext  )
{
  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::VISIBLE_WHERE_IS_THIS ) != 0 )  // WhereIsThis表示中は表示しない。
  {
    return;
  }

  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::VISIBLE_ENVSOUND_INFO ) == 0 )
  {
    return;
  }

  if( m_pSoundElemList == NULL )
  {
    return ;
  }


  // 裏板
#if defined(GF_PLATFORM_CTR)// Winは半透明がきかなかったため書かない
  gfl2::gfx::DrawUtil::DrawRect( 
    0.0f, 0.0f,
    320.0f, 240.0f,
    gfl2::math::Vector4( 0.3f, 0.3f, 0.3f, 0.8f )
    );
#endif

  pDrawUtilText->SetTextScale( 0.6f, 0.6f );
  pDrawUtilText->SetTextColor( VEC4_COLOR_WHITE );

  u32 DrawY = 0;
  u32 DrawX = 5;
  u32 AddDrawY = 10;

#if defined(GF_PLATFORM_WIN32)
  pDrawUtilText->DrawTextW( DrawX, DrawY, L"PlacementDataNum[%d] SeDataNum[%d]", m_pAccessor->GetPlacementMax(), m_pAccessor->GetParamMax() );
#elif defined(GF_PLATFORM_CTR)
  pDrawUtilText->DrawText( DrawX, DrawY, L"PlacementDataNum[%d] SeDataNum[%d]", m_pAccessor->GetPlacementMax(), m_pAccessor->GetParamMax() );
#endif
  DrawY += AddDrawY;

#if defined(GF_PLATFORM_WIN32)
  pDrawUtilText->DrawTextW( DrawX, DrawY, L"TimeZone[%d] PlayerRideOn[%d]", m_DebugInfoTimezone, m_DebugInfoPlayerForm );
#elif defined(GF_PLATFORM_CTR)
  pDrawUtilText->DrawText( DrawX, DrawY, L"TimeZone[%d] PlayerRideOn[%d]", m_DebugInfoTimezone, m_DebugInfoPlayerForm );
#endif
  DrawY += AddDrawY;

  static gfl2::math::Vector4 color[ENV_SOUND_MANAGER_PLAY_LIST_MAX] = 
  {
    gfl2::math::Vector4(1.0f,0.0f,0.0f,1.0f),
    gfl2::math::Vector4(0.20f,1.0f,0.20f,1.0f),
    gfl2::math::Vector4(0.0f,1.0f,1.0f,1.0f),
  };

  static gfl2::gfx::DrawUtil::VertexFormat s_vertexFormat[3]; // ライン表示用のデータ

  // ライン描画のためにカメラ設定
  {
    // プロジェクション行列設定
    gfl2::gfx::DrawUtil::SetProjectionMatrix(rDrawContext.m_Projection);
    // ビュー行列設定
    gfl2::gfx::DrawUtil::SetViewMatrix(rDrawContext.m_View);
    // モデル行列設定
    gfl2::gfx::DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());
  }


  // 再生中の情報の数を求める。
  s32 data_count = 0;
  {
    gfl2::util::List<EnvSoundElem*>::Iterator play_elem = m_pPlaySoundElemList->Begin();
    for( ; play_elem != m_pPlaySoundElemList->End();  )
    {
      if( data_count >= ENV_SOUND_MANAGER_PLAY_LIST_MAX || !((*play_elem)->IsPlayFlag()) )
      {
        ++play_elem;
      }
      else
      {
        data_count++;
        ++play_elem;
      }
    }
  }


  // 再生中のサウンド情報 
  {
    gfl2::util::List<EnvSoundElem*>::Iterator play_elem = m_pPlaySoundElemList->Begin();
    s32 playCount = 0;
    for( ; play_elem != m_pPlaySoundElemList->End();  )
    {
      if( (playCount >= ENV_SOUND_MANAGER_PLAY_LIST_MAX) || !((*play_elem)->IsPlayFlag()) )
      {
        ++play_elem;
      }
      else
      {
        const EnvSoundElemDebugInfo* cpInfo = NULL;
        if( (*play_elem)->GetIndex() < ENV_SOUND_MANAGER_NUM )
        {
          cpInfo = &m_pDebugInfo[ (*play_elem)->GetIndex() ];

          const FieldEnvSoundAccessor::EnvSoundPlacement* cpEnvSoundPlacement = (*play_elem)->GetPlacementData();
          const FieldEnvSoundAccessor::EnvSoundParam* cpEnvSoundParam = (*play_elem)->GetUseSoundParam();

          if( (cpEnvSoundPlacement == NULL) || (cpEnvSoundParam == NULL) )
          {
        #if defined(GF_PLATFORM_WIN32)
            pDrawUtilText->DrawTextW( DrawX, DrawY, L"Error DataIndex[%d]", (*play_elem)->GetIndex() );
        #elif defined(GF_PLATFORM_CTR)
            pDrawUtilText->DrawText( DrawX, DrawY, L"Error DataIndex[%d]", (*play_elem)->GetIndex() );
        #endif
            DrawY += AddDrawY;
            ++play_elem;
            ++playCount;
            continue; // ここに来ることはない
          }

          if(playCount == 0)
          {
        #if defined(GF_PLATFORM_WIN32)
            pDrawUtilText->DrawTextW( DrawX, DrawY, L"PlayerPos[%.0f, %.0f, %.0f]", cpInfo->m_PlayerPos.x, cpInfo->m_PlayerPos.y, cpInfo->m_PlayerPos.z );
        #elif defined(GF_PLATFORM_CTR)
            pDrawUtilText->DrawText( DrawX, DrawY, L"PlayerPos[%.0f, %.0f, %.0f]", cpInfo->m_PlayerPos.x, cpInfo->m_PlayerPos.y, cpInfo->m_PlayerPos.z );
        #endif
            DrawY += AddDrawY;
          }


          // 表示色決定
          s32 color_index = data_count - playCount - 1;
          if( (color_index >= 0) && (color_index < ENV_SOUND_MANAGER_PLAY_LIST_MAX))
          {
            pDrawUtilText->SetTextColor( color[color_index] );
            for( u32 i=0; i<3; i++ )
            {
              s_vertexFormat[i].color =   color[color_index];
            }
          }
          else
          {
            pDrawUtilText->SetTextColor( VEC4_COLOR_WHITE );
            for( u32 i=0; i<3; i++ )
            {
              s_vertexFormat[i].color =   VEC4_COLOR_WHITE;
            }
          }


      #if defined(GF_PLATFORM_WIN32)
          pDrawUtilText->DrawTextW( DrawX, DrawY, L"=== PlayEnvSe[%d] ===", playCount );
      #elif defined(GF_PLATFORM_CTR)
          pDrawUtilText->DrawText( DrawX, DrawY, L"=== PlayEnvSe[%d] ===", playCount );
      #endif
          DrawY += AddDrawY;

      #if defined(GF_PLATFORM_WIN32)
          pDrawUtilText->DrawTextW( DrawX, DrawY, L"DataIndex[%d]     PlaySE[%d]", (*play_elem)->GetIndex(), (*play_elem)->GetPlaySE() );
      #elif defined(GF_PLATFORM_CTR)
          pDrawUtilText->DrawText( DrawX, DrawY, L"DataIndex[%d]     PlaySE[%d]", (*play_elem)->GetIndex(), (*play_elem)->GetPlaySE() );
      #endif
          DrawY += AddDrawY;

      #if defined(GF_PLATFORM_WIN32)
          pDrawUtilText->DrawTextW( DrawX, DrawY, L"pri[%d]    playerFormtype[%d]", cpEnvSoundPlacement->priority, cpEnvSoundParam->player_from_type );
      #elif defined(GF_PLATFORM_CTR)
          pDrawUtilText->DrawText( DrawX, DrawY, L"pri[%d]     playerFormtype[%d]", cpEnvSoundPlacement->priority, cpEnvSoundParam->player_from_type );
      #endif
          DrawY += AddDrawY;


      #if defined(GF_PLATFORM_WIN32)
          pDrawUtilText->DrawTextW( DrawX, DrawY, L"ColPos[%.0f, %.0f, %.0f]", cpInfo->m_CollisionCenterPos.x, cpInfo->m_CollisionCenterPos.y, cpInfo->m_CollisionCenterPos.z );
      #elif defined(GF_PLATFORM_CTR)
          pDrawUtilText->DrawText( DrawX, DrawY, L"ColPos[%.0f, %.0f, %.0f]", cpInfo->m_CollisionCenterPos.x, cpInfo->m_CollisionCenterPos.y, cpInfo->m_CollisionCenterPos.z );
      #endif
          DrawY += AddDrawY;

      #if defined(GF_PLATFORM_WIN32)
          pDrawUtilText->DrawTextW( DrawX, DrawY, L"Vol[%.2f] Pitch[%.2f] Dist[%.2f]/[%.2f]", (*play_elem)->GetPlayVolume(), (*play_elem)->GetPlayPitch(), cpInfo->m_Distance, cpInfo->m_DistanceMax );
      #elif defined(GF_PLATFORM_CTR)
          pDrawUtilText->DrawText( DrawX, DrawY, L"Vol[%.2f] Pitch[%.2f] Dist[%.2f]/[%.2f]", (*play_elem)->GetPlayVolume(), (*play_elem)->GetPlayPitch(), cpInfo->m_Distance, cpInfo->m_DistanceMax );
      #endif
          DrawY += AddDrawY;
          
      #if defined(GF_PLATFORM_WIN32)
          pDrawUtilText->DrawTextW( DrawX, DrawY, L"PanType[%d] Pan[%.2f] RotDeg[%.1f]", cpEnvSoundPlacement->pan_type, (*play_elem)->GetPlayPan(), cpInfo->m_PanRotDeg );
      #elif defined(GF_PLATFORM_CTR)
          pDrawUtilText->DrawText( DrawX, DrawY, L"PanType[%d] Pan[%.2f] RotDeg[%.1f]", cpEnvSoundPlacement->pan_type, (*play_elem)->GetPlayPan(), cpInfo->m_PanRotDeg );
      #endif
          DrawY += AddDrawY;

          gfl2::math::Vector3 colCenterDir = cpInfo->m_CollisionCenterPos - cpInfo->m_PlayerPos;
          if( colCenterDir.LengthSq() > EPSILON ){ colCenterDir = colCenterDir.Normalize(); }

      #if defined(GF_PLATFORM_WIN32)
          pDrawUtilText->DrawTextW( DrawX, DrawY, L" BDir[%.1f, %.1f, %.1f] CDir[%.1f, %.1f, %.1f]", cpInfo->m_PlayerOrCameraDir.x, cpInfo->m_PlayerOrCameraDir.y, cpInfo->m_PlayerOrCameraDir.z, colCenterDir.x, colCenterDir.y, colCenterDir.z );
      #elif defined(GF_PLATFORM_CTR)
          pDrawUtilText->DrawText( DrawX, DrawY, L" BDir[%.1f, %.1f, %.1f] CDir[%.1f, %.1f, %.1f]", cpInfo->m_PlayerOrCameraDir.x, cpInfo->m_PlayerOrCameraDir.y, cpInfo->m_PlayerOrCameraDir.z, colCenterDir.x, colCenterDir.y, colCenterDir.z );
      #endif
          DrawY += AddDrawY;



          // プレイヤー座標からコリジョン中心に向かってベクトルを表示
          s_vertexFormat[0].pos = cpInfo->m_PlayerPos;  //視点
          s_vertexFormat[1].pos = cpInfo->m_CollisionCenterPos;  //終点
          s_vertexFormat[2].pos = (cpInfo->m_PlayerPos + ((cpInfo->m_CollisionCenterPos - cpInfo->m_PlayerPos) * 0.50f));
          s_vertexFormat[2].pos.y += 8.0f;  //若干膨らませる。
          gfl2::gfx::DrawUtil::DrawTriangles( s_vertexFormat, 3, false );
          

        }
        // elemのdata_indexが大きすぎることを表示(ありえない)
        else
        {
      #if defined(GF_PLATFORM_WIN32)
          pDrawUtilText->DrawTextW( DrawX, DrawY, L"Error DataIndex[%d]", (*play_elem)->GetIndex() );
      #elif defined(GF_PLATFORM_CTR)
          pDrawUtilText->DrawText( DrawX, DrawY, L"Error DataIndex[%d]", (*play_elem)->GetIndex() );
      #endif
          DrawY += AddDrawY;
        }

      }

      ++play_elem;
      ++playCount;
    }
  }
}


#endif  //#if PM_DEBUG



GFL_NAMESPACE_END(Field)
