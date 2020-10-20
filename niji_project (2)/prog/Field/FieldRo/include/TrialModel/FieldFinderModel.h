//======================================================================
/**
 * @file FieldFinderModel.h
 * @date 2015/10/07 14:54:31
 * @author miyachi_soichi
 * @brief フィールドで使用するバトルモデル：ファインダー用
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_FINDERMODEL_H_INCLUDED__
#define __FIELD_FINDERMODEL_H_INCLUDED__
#pragma once
// PokeTool
#include "PokeTool/include/PokeModelPokeFinderMotion.h"
// field
#include "Field/FieldStatic/include/Field3DObjectNode.h"
#include "Field/FieldRo/include/TrialModel/FieldTrialModel.h"
#include "Field/FieldRo/include/TrialModel/FieldFinderModel_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)
class FinderModelControl;

/**
 *  @class  FinderModel
 *  @brief  ファインダーで使用するポケモンモデル
 */
class FinderModel : public FieldTrialModel
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderModel);

public:
  enum Mode
  {
    MODE_NORMAL,
    MODE_REQUEST_SUSPEND,
    MODE_SUSPEND,
  };
  enum InterestType
  {
    INTEREST_TYPE_NONE,
    INTEREST_TYPE_CAMERA,
    INTEREST_TYPE_SEARCH,
    INTEREST_TYPE_ENDING,
  };
  enum ControlType
  {
    CTRL_TYPE_None,
    CTRL_TYPE_Walker,
    CTRL_TYPE_Statue,
    CTRL_TYPE_Oneway,
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  FinderModel( PokeTool::PokeModel *pModel, Field::MyRenderingPipeLine *pPipe, const FinderModelData *pData, u32 idx );

  /**
   *  @brief  デストラクタ
   */
  ~FinderModel( void );

  /**
   *  @brief  移動コンポーネントを生成する
   */
  void CreateControlWalker( gfl2::heap::HeapBase *pHeap, const FinderPathData *pathData );

  /**
   *  @brief  待機コンポーネントを生成する
   */
  void CreateControlStatue( gfl2::heap::HeapBase *pHeap, const FinderStatueData *statueData );

  /**
   *  @brief  一通コンポーネントを生成する
   */
  void CreateControlOneway( gfl2::heap::HeapBase *pHeap, const FinderOnewayData *onewayData );

  /**
   *  @brief  操作コンポーネントを破壊する
   */
  void DestroyControl( void );

  /**
   *  @brief  安全に破棄できるか調べる
   */
  bool IsSafeDelete( void );

  /**
   *  @brief  更新
   *  @note   アニメーション更新前に行う
   */
  virtual void UpdateBeforeAnimation( void );

  /**
   *  @brief  更新
   *  @note   アニメーション更新後に行う
   */
  virtual void UpdateAfterAnimation( void );

  /**
   *  @brief  更新
   */
  virtual void UpdateAfterTraverse( void );

  /**
   *  @brief  待機モーションを取得する
   *  @param  anim  待機後発行するモーション
   */
  PokeTool::MODEL_ANIME_POKE_FINDER GetWaitAnimNo( u32 anim );

  /**
   *  @brief  移動モーションを取得する
   */
  PokeTool::MODEL_ANIME_POKE_FINDER GetMoveAnimNo( void );

  /**
   *  @brief  アピール挙動を取得する
   *  @param  out_AnimNo  モーション番号
   */
  void GetAppealAction( u32 &out_AnimNo );

  /**
   *  @brief  得点を取得する
   */
  u32 GetBasePoint( void );

  /**
   *  @brief  容姿の取得
   */
  u32 GetFigure( void );

  /**
   *  @brief  アピール容姿
   */
  u32 GetFigureAppeal( void );

  /**
   *  @brief  停止リクエスト
   */
  void RequestSuspend( void );

  /**
   *  @brief  再生リクエスト
   */
  void RequestResume( void );

  /**
   *  @brief  停止確認
   */
  bool IsPause( void );

  /**
   *  @brief  アピール
   */
  void SetAppeal( bool flag );

  /**
   *  @brief  描画優先度を専用モードに切り替え
   *  @note   ゴース用対応
   */
  void SetOptionalRenderKey( void );

  /**
   *  @brief  注視開始
   */
  void EnableInterestCamera( void );
  void EnableInterestSearch( void );

  /**
   *  @brief  注目終了
   */
  void DisableInterest( u32 frm = 0 );

  /**
   *  @brief  注目中確認
   */
  bool IsInterest( void );

  /**
   *  @brief  注目座標へのアプローチ中か
   */
  bool IsFinishInterest( void );

  /**
   *  @brief  注目座標更新
   */
  void SetInterestPosition( const gfl2::math::Vector3 &pos );
  void SetInterestPositionSearch( const gfl2::math::Vector3 &pos );
  void SetInterestSpeedSearch( f32 spd );

  /**
   *  @brief  注目データ設定
   */
  void SetPokeData( const FinderPokeData *pData )
  {
    m_rPokeData = *pData;
    m_HasPokeData = true;
  }
  /**
   *  @brief  注目データ取得
   */
  const FinderPokeData *GetPokeData( void )
  {
    if( m_HasPokeData )
    {
      return &(m_rPokeData);
    }
    return NULL;
  }

  /**
   *  @brief  拡張パラメータを取得する
   */
  inline u8 GetExtPrm1( void ) const { return m_rModelData.ext_prm1; }
  inline u8 GetExtPrm2( void ) const { return m_rModelData.ext_prm2; }

  /**
   *  @brief  アピールを保持しているか
   */
  bool HasAppeal( void );

  /**
   *  @brief  アピール中か
   */
  bool IsAppeal( void ){ return m_IsAppeal; }

  /**
   *  @brief  ポケモンを呼ぶ
   */
  void RequestPokeCall( void );

  /**
   *  @brief  挙動タイプの取得
   */
  inline ControlType GetCtrlType( void ){ return m_eCtrlType; }

  /**
   *  @brief  ボイスを再生する
   */
  void PlayVoice( u32 voiceID );

  /**
   *  @brief  ボイスのボリュームを再計算する
   */
  void CalcVoiceVolume( void );

  /**
   *  @brief  カメラ方向の設定
   */
  void SetCameraRot( f32 rot ){ m_fCameraRotY = rot; }

  /**
   *  @brief  ボイスのパンを再計算する
   */
  void CalcVoicePan( void );

  /**
   *  @brief  アピールなし用鳴き声
   */
  void UpdateNoAppealVoice( void );

  /**
   *  @brief  アピールなし鳴き声クールタイム計算
   */
  u32 CalcNoAppealVoiceCoolTime( void );

  /**
   *  @brief  ボイス取得
   */
  u32 GetCallVoice( void );

  /**
   *  @brief  AABBを取得する
   */
  void GetAABB( gfl2::math::AABB *pAABB );

  /**
   *  @brief  AABBをマウントする座標を取得する
   */
  gfl2::math::Vector3 GetAABBMountPosition( void );

private:
  /**
   *  @brief  注目更新
   */
  void updateInterestPosition( void );

private:
  FinderModelControl  *m_pControl;
  FinderModelData     m_rModelData;
  FinderPokeData      m_rPokeData;
  gfl2::math::Vector3 m_InterestPos_Camera;
  gfl2::math::Vector3 m_InterestPos_Search;
  Mode                m_eMode;
  InterestType        m_eInterestType;
  ControlType         m_eCtrlType;
  f32                 m_fStepFrame;
  s32                 m_FigureAnim;
  u32                 m_Voice;
  u32                 m_InterestResetFrame;
  u32                 m_SlotIndex;                 //!< 生成配列のIndex
  f32                 m_fCameraRotY;
  u32                 m_CoolTime_NoAppeal;

  u32                 m_nAdjustAngle_Life;
  f32                 m_fAdjustAngle_Now;
  f32                 m_fAdjustAngle_End;
  f32                 m_fAdjustAngle_Add;

  bool                m_IsAppeal;
  bool                m_HasPokeData;

#if PM_DEBUG
  // 以下デバッグ機能
public:
  f32 debug_getPosX(void);
  f32 debug_getPosY(void);
  f32 debug_getPosZ(void);
  void debug_setPosX(f32 dst);
  void debug_setPosY(f32 dst);
  void debug_setPosZ(f32 dst);
  f32 debug_getRotY(void);
  void debug_setRotY(f32 dst);

  s32 deb_1;
  s32 deb_2;
  s32 deb_3;

  void debug_getPrevWayPoint1( void );
  void debug_getNextWayPoint1( void );
  void debug_getPrevWayPoint2( void );
  void debug_getNextWayPoint2( void );
  void debug_getPrevWayPoint3( void );
  void debug_getNextWayPoint3( void );
  void debug_createPath( void );

  void debug_onemore( void );
  u32 debug_getOnewayDelay( void );

  u32 debug_getStatuePosition( void );
  void debug_setStatuePosition( u32 idx );

  f32 m_voice_volume;
  f32 m_voice_pan;

  u32 m_statue_motion;
  u32 debug_getStatueMotion( void ){ return m_statue_motion; }
  void debug_setStatueMotion( u32 motion ){ m_statue_motion = motion; }
  void debug_requestStatueMotion( void );
#endif
};

GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_FINDERMODEL_H_INCLUDED__
