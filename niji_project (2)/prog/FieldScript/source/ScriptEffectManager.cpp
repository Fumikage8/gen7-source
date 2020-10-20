//==============================================================================
/**
 * @file        ScriptEffectManager.cpp
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/06/11(木) 18:22:17
 */
//==============================================================================

#include "FieldScript/include/ScriptEffectManager.h"
#include "FieldScript/include/ScriptWork.h"
#include "Field/FieldRo/include/Fieldmap.h"

#include "Field/FieldRo/include/Effect/content/FieldEffectBallonIcon.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectExclamation2.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectTrainerEncount.h"
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "niji_conv_header/field/FieldEffectTypes.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( FieldScript );

//------------------------------------------------------------------------------
/**
 * @func        ScriptEffectManager
 * @brief       コンストラクタ.
 * @author      N.Takeda
 * @date        2015/06/11(木) 18:46:05
 *
 * @param[in,out] gfl2::heap::HeapBase *  p_heap  .
 */
//------------------------------------------------------------------------------
ScriptEffectManager::ScriptEffectManager( gfl2::heap::HeapBase * p_heap, GameSys::GameManager *pGameMan )
 : m_iEntry( 0 )
 , m_pGameMan( pGameMan )
 , m_DynamicLoadNum( 0 )
 , m_iReleaseRequestNum( 0 )
{
  
  for( s32 ii=0 ; ii<EFFECTMAX ; ii++ )
  {
    initEffect( ii );

    m_DynamicLoad[ii] = Effect::EFFECT_TYPE_ERROR;
    m_uReleaseRequest[ii] = Effect::EFFECT_TYPE_ERROR;
  }
  
}

//------------------------------------------------------------------------------
/**
 * @func        ~ScriptEffectManager
 * @brief       デストラクタ.
 * @author      N.Takeda
 * @date        2015/06/12(金) 16:55:06
 */
//------------------------------------------------------------------------------
ScriptEffectManager::~ScriptEffectManager()
{
  
}


//------------------------------------------------------------------------------
/**
 * @func        TranslateId
 * @brief       スクリプトIDからプログラムIDに変換.
 * @author      N.Takeda
 * @date        2015/06/12(金) 21:53:23
 *
 * @param[in] const s32 iId ID.
 *
 * @return      Field::Effect::Type 
 */
//------------------------------------------------------------------------------
Field::Effect::Type ScriptEffectManager::TranslateId( const s32 iId )
{
  switch( iId )
  {
    case EFF_POS_EV0030_LENSFLARE:
      return Field::Effect::EFFECT_TYPE_W_F01R0102_EFF;
    case EFF_POS_TRIAL_START:
      return Field::Effect::EFFECT_TYPE_TRIAL_START;
    case EFF_CHR_ICON_E1:
    case EFF_CHR_ICON_E1_SILENT:
      return Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON1;
    case EFF_CHR_ICON_E2:
      return Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON2;
    case EFF_CHR_ICON_Q:
      return Field::Effect::EFFECT_TYPE_QUESTION_ICON;
    case EFF_POS_ROTOM:
      return Field::Effect::EFFECT_TYPE_ROTOM;
    case EFF_POS_BALL_DEMO:
      return Field::Effect::EFFECT_TYPE_B_DEMO;
    case EFF_POS_FESTIVAL_FIRE:
      return Field::Effect::EFFECT_TYPE_FESTIVAL_FIRE;
    case EFF_POS_GET_ITEM_1:
      return Field::Effect::EFFECT_TYPE_GET_ITEM_1;
    case EFF_POS_GET_ITEM_2:
      return Field::Effect::EFFECT_TYPE_GET_ITEM_2;
    case EFF_POS_GET_ITEM_3:
      return Field::Effect::EFFECT_TYPE_GET_ITEM_3;
    case EFF_POS_GET_ITEM_4:
      return Field::Effect::EFFECT_TYPE_GET_ITEM_4;
    case EFF_POS_DEMO_RIDE:
      return Field::Effect::EFFECT_TYPE_DEMO_RIDE;
    case EFF_POS_DEMO_TRIAL2:
      return Field::Effect::EFFECT_TYPE_DEMO_TRIAL2;
    case EFF_POS_DEMO_TRIAL3:
      return Field::Effect::EFFECT_TYPE_DEMO_TRIAL3;
    case EFF_POS_DEMO_TRIAL5:
      return Field::Effect::EFFECT_TYPE_DEMO_TRIAL5;
    case EFF_POS_DEMO_TRIAL7_1:
      return Field::Effect::EFFECT_TYPE_DEMO_TRIAL7_1;
    case EFF_POS_DEMO_TRIAL7_2:
      return Field::Effect::EFFECT_TYPE_DEMO_TRIAL7_2;
    case EFF_POS_BAG_EFFECT:
      return Field::Effect::EFFECT_TYPE_BAG_EFFECT;
    case EFF_POS_ICON_SP_E:
    case EFF_POS_ICON_SP_E2:
      return Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON1;
    case EFF_POS_ICON_SP_Q:
      return Field::Effect::EFFECT_TYPE_QUESTION_ICON;
    case EFF_CHR_ICON_E1_OVERLAY:
      return Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON1_OVERLAY;
    case EFF_CHR_ICON_E2_OVERLAY:
      return Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON2_OVERLAY;
    case EFF_CHR_ICON_Q_OVERLAY:
      return Field::Effect::EFFECT_TYPE_QUESTION_ICON_OVERLAY;
    case EFF_POS_DEMO_CONCENTRATE:
      return Field::Effect::EFFECT_TYPE_DEMO_CONCENTRATE;
    case EFF_POS_DEMO_FOG:
      return Field::Effect::EFFECT_TYPE_DEMO_FOG;
    case EFF_POS_DEMO_FLOWER_YELLOW:
      return Field::Effect::EFFECT_TYPE_DEMO_FLOWER_YELLOW;
    case EFF_POS_DEMO_FLOWER_PINK:
      return Field::Effect::EFFECT_TYPE_DEMO_FLOWER_PINK;
    case EFF_POS_DEMO_FIREWORK_YELLOW:
      return Field::Effect::EFFECT_TYPE_DEMO_FIREWORK_YELLOW;
    case EFF_POS_DEMO_FIREWORK_PINK:
      return Field::Effect::EFFECT_TYPE_DEMO_FIREWORK_PINK;
    case EFF_POS_DEMO_FIREWORK_RED:
      return Field::Effect::EFFECT_TYPE_DEMO_FIREWORK_RED;
    case EFF_POS_DEMO_FIREWORK_PURPLE:
      return Field::Effect::EFFECT_TYPE_DEMO_FIREWORK_PURPLE;
    case EFF_POS_DEMO_FLARE_SUN:
      return Field::Effect::EFFECT_TYPE_DEMO_FLARE_SUN;
    case EFF_POS_DEMO_FLARE_MOON:
      return Field::Effect::EFFECT_TYPE_DEMO_FLARE_MOON;
    case EFF_POS_KAMINARI_1:
      return Field::Effect::EFFECT_TYPE_EF_PH0301_DEN;
    case EFF_POS_KAMINARI_2:
      return Field::Effect::EFFECT_TYPE_EF_PH0301_DEN2;
    case EFF_POS_S5_1:
      return Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_01;
    case EFF_POS_S5_1_MDL:
      return Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_02;
    case EFF_POS_STEP_SMOKE:
      return Field::Effect::EFFECT_TYPE_STEP_SMOKE;
    case EFF_POS_UB_KAMI_SLASH:
      return Field::Effect::EFFECT_TYPE_UB_KAMI_SLASH;
    case EFF_POS_UB_KAMI_BLACKOUT:
      return Field::Effect::EFFECT_TYPE_UB_KAMI_BLACKOUT;
    case EFF_POS_ROTOM_POWER:
      return Field::Effect::EFFECT_TYPE_ROTOM_POWER;
    case EFF_POS_ROTOM_CUT_IN:
      return Field::Effect::EFFECT_TYPE_ROTOM_CUT_IN;
    case EFF_POS_DEMO_TRIAL2_2:
      return Field::Effect::EFFECT_TYPE_DEMO_TRIAL2_2;
    case EFF_CHR_FREAI_ICON_E1:
      return Field::Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1;
    case EFF_CHR_FREAI_ICON_E2:
      return Field::Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2;
    case EFF_CHR_FREAI_ICON_HEART:
      return Field::Effect::EFFECT_TYPE_FUREAI_ICON_HEART1;
    case EFF_CHR_FREAI_ICON_NOTE:
      return Field::Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1;
    case EFF_CHR_FREAI_ICON_Q:
      return Field::Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1;
    case EFF_CHR_FREAI_ICON_SMILE:
      return Field::Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1;
    case EFF_CHR_FREAI_ICON_TEN:
      return Field::Effect::EFFECT_TYPE_FUREAI_ICON_TEN1;
    case EFF_CHR_FREAI_ICON_ZZZ:
      return Field::Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1;
    case EFF_POS_RR_1:
      return Field::Effect::EFFECT_TYPE_R_ROCKET1;
    case EFF_POS_BTFES_WARP:
      return Field::Effect::EFFECT_TYPE_EF_BTFES_WARP;
  }
  return Field::Effect::EFFECT_TYPE_ERROR;
}

//------------------------------------------------------------------------------
/**
 * @func        SetPos
 * @brief       位置に付随するエフェクトを作成.
 * @author      N.Takeda
 * @date        2015/06/12(金) 14:29:02
 *
 * @param[in] const s32 iId         ID.
 * @param[in] const s32 iEffectType タイプ.
 * @param[in] const f32 fX          .
 * @param[in] const f32 fY          .
 * @param[in] const f32 fZ          .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptEffectManager::SetPos( const s32 iId, const s32 iEffectType, const f32 fX, const f32 fY, const f32 fZ, bool se_play /*=true*/ )
{
  gfl2::math::Vector3   in_vPosition( fX, fY, fZ );
  
  GFL_ASSERT( m_iEntry < EFFECTMAX );
  if( m_iEntry >= EFFECTMAX ) return;
  
  initEffect( m_iEntry );
  m_Effect[ m_iEntry ].m_iId = iId;

  // 非常駐リソース読み込み
  LoadDynamicResource( iEffectType );

  m_Effect[ m_iEntry ].m_pPtr = m_pGameMan->GetFieldmap()->GetEffectManager()->CreateEffect( TranslateId(iEffectType), in_vPosition, se_play );
  if( m_Effect[ m_iEntry ].m_pPtr == NULL ){
    m_Effect[ m_iEntry ].m_iId = NO_ID;
    return;
  }


  if( iEffectType == EFF_POS_FESTIVAL_FIRE )  m_Effect[ m_iEntry ].m_bNoLoop = false;
  m_iEntry++;
}

//------------------------------------------------------------------------------
/**
* @func        SetPosScaleRot
* @brief       位置に付随するエフェクトを作成. + スケール/回転データ受け渡し機能
* @author      kawazoe_shinichi
* @date        2017/01/31(火) 15:33:01
*
* @param[in] const s32 iId         ID.
* @param[in] const s32 iEffectType タイプ.
* @param[in] const f32 fX          .
* @param[in] const f32 fY          .
* @param[in] const f32 fZ          .
* @param[in] const f32 fScale      .
* @param[in] bool      se_play     .
* @param[in] const f32 fRotX       .
* @param[in] const f32 fRotY       .
* @param[in] const f32 fRotZ       .
*
* @return      なし (none)
*/
//------------------------------------------------------------------------------
void  ScriptEffectManager::SetPosScaleRot(const s32 iId, const s32 iEffectType, const f32 fX, const f32 fY, const f32 fZ, const f32 fScale, bool se_play /*=true*/, const f32 fRotX /*= 0.0f*/, const f32 fRotY /*= 0.0f*/, const f32 fRotZ /*= 0.0f*/)
{
  gfl2::math::Vector3   in_vPosition(fX, fY, fZ);
  gfl2::math::Vector3   in_vRotation(fRotX, fRotY, fRotZ);

  GFL_ASSERT(m_iEntry < EFFECTMAX);
  if (m_iEntry >= EFFECTMAX) return;

  initEffect(m_iEntry);
  m_Effect[m_iEntry].m_iId = iId;

  // 非常駐リソース読み込み
  LoadDynamicResource(iEffectType);

  m_Effect[m_iEntry].m_pPtr = m_pGameMan->GetFieldmap()->GetEffectManager()->CreateEffect(TranslateId(iEffectType), in_vPosition, se_play, Field::Effect::EffectManager::WORK_TYPE_DEFAULT, NULL, fScale, in_vRotation);
  if (m_Effect[m_iEntry].m_pPtr == NULL){
    m_Effect[m_iEntry].m_iId = NO_ID;
    return;
  }

  if (iEffectType == EFF_POS_FESTIVAL_FIRE)  m_Effect[m_iEntry].m_bNoLoop = false;
  m_iEntry++;
}

//------------------------------------------------------------------------------
/**
 * @func        SetChr
 * @brief       キャラに付随するエフェクトを作成.
 * @author      N.Takeda
 * @date        2015/06/12(金) 14:29:01
 *
 * @param[in] const s32 iId         ID.
 * @param[in] const s32 iEffectType タイプ.
 * @param[in] const s32 chrID       .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptEffectManager::SetChr( const s32 iId, const s32 iEffectType, const s32 chrID, bool se_play /*=true*/, f32 step_frame, f32 scale )
{
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = m_pGameMan->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModel          *pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );
  
  GFL_ASSERT( m_iEntry < EFFECTMAX );
  if( m_iEntry >= EFFECTMAX ) return;
  
  initEffect( m_iEntry );
  m_Effect[ m_iEntry ].m_iId = iId;

  // 非常駐リソース読み込み
  LoadDynamicResource( iEffectType );

  m_Effect[ m_iEntry ].m_pPtr = m_pGameMan->GetFieldmap()->GetEffectManager()->CreateEffect( TranslateId(iEffectType), pCharaModel, se_play );
  if( m_Effect[ m_iEntry ].m_pPtr == NULL ){
    m_Effect[ m_iEntry ].m_iId = NO_ID;
    return;
  }
  

  // @fix NMcat[662]:「！」の吹き出しアイコンの優先順位が異なる対処、描画優先度が高い形式を追加
  if( iEffectType == EFF_CHR_ICON_E2 || iEffectType == EFF_CHR_ICON_E2_OVERLAY ) 
  {
    Field::Effect::EffectExclamation2* pBallonIcon = static_cast< Field::Effect::EffectExclamation2* >( m_Effect[ m_iEntry ].m_pPtr );
    pBallonIcon->SetStepFrame( step_frame );
  }

  // @note momiji追加、スケール指定機能
  switch( iEffectType )
  {
    case EFF_CHR_ICON_E1:
    case EFF_CHR_ICON_E2:
    case EFF_CHR_ICON_Q:
    case EFF_CHR_ICON_E1_SILENT:
    case EFF_CHR_ICON_E1_OVERLAY:
    case EFF_CHR_ICON_E2_OVERLAY:
    case EFF_CHR_ICON_Q_OVERLAY:
      {
        Field::Effect::EffectBallonIcon* pBallonIcon = static_cast< Field::Effect::EffectBallonIcon* >( m_Effect[ m_iEntry ].m_pPtr );
        pBallonIcon->SetScale( scale );
        break;
      }
  }

  m_iEntry++;
}

//------------------------------------------------------------------------------
/**
 * @func        SetTrainerEye
 * @brief       視線トレーナー専用エフェクトを作成.
 * @author      Miyuki Iwasawa
 * @date        2015/09/14(月)
 *
 * @param[in] const s32 iId         ID.
 * @param[in] const gfl2::math::VEC2& pos 座標
 * @param[in] const f32 rot   回転
 * @param[in] const u32 se_id エフェクト共に再生する視線SEのID
 * 
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptEffectManager::SetTrainerEye( const s32 iId, const gfl2::math::VEC2& pos, const f32 rot, const u32 eye_se_id )
{
  gfl2::math::Vector3 vec(0,0,0);
  
  GFL_ASSERT( m_iEntry < EFFECTMAX );

  if( m_iEntry >= EFFECTMAX ) return;
  m_Effect[ m_iEntry ].m_iId = iId;

  // 非常駐リソース読み込み
  loadDynamicResource( Effect::EFFECT_TYPE_TRAINER_ENCOUNT );

  m_Effect[ m_iEntry ].m_pPtr = m_pGameMan->GetFieldmap()->GetEffectManager()->CreateEffect( Effect::EFFECT_TYPE_TRAINER_ENCOUNT, vec, true );
  if( m_Effect[ m_iEntry ].m_pPtr == NULL ){
    m_Effect[ m_iEntry ].m_iId = NO_ID;
    return;
  }

  Effect::EffectTrainerEncount *eff = static_cast<Effect::EffectTrainerEncount*>(m_Effect[ m_iEntry ].m_pPtr);
  eff->StartAnimation(pos, rot, eye_se_id );

  m_iEntry++;
}


//------------------------------------------------------------------------------
/**
 * @func        SetScale
 * @brief       Scaleのセット. (エモーション専用)
 * @author      N.Takeda
 * @date        2016/01/13(水) 21:05:58
 *
 * @param[in] const s32 iId     ID.
 * @param[in] const f32 _fScale スケール.
 */
//------------------------------------------------------------------------------
void  ScriptEffectManager::SetScale( const s32 iId, const f32 _fScale )
{
  for( s32 ii=0 ; ii<m_iEntry ; ii++ )
  {
    if( m_Effect[ii].m_iId == iId )
    {
      if( m_Effect[ ii ].m_pPtr ){
        static_cast<Effect::EffectBallonIcon*>(m_Effect[ ii ].m_pPtr)->SetScale( _fScale );
        return;
      }
    }
  }
  GFL_ASSERT( 0 );
}


//------------------------------------------------------------------------------
/**
 * @func        IsEnd
 * @brief       Endの可否を得る.
 * @author      N.Takeda
 * @date        2015/06/12(金) 14:28:59
 *
 * @param[in] const s32 iId ID.
 *
 * @return      bool  TRUE:成 / FALSE:否
 */
//------------------------------------------------------------------------------
bool  ScriptEffectManager::IsEnd( const s32 iId )
{
  for( s32 ii=0 ; ii<m_iEntry ; ii++ )
  {
    if( m_Effect[ii].m_iId == iId )
    {
      return m_Effect[ii].m_pPtr->IsAnimationLastFrame();
    }
  }
  
  return true;  ///< 終了済みの物を取る場合もあるので assert はしない.
}


//------------------------------------------------------------------------------
/**
 * @func        Delete
 * @brief       削除.
 * @author      N.Takeda
 * @date        2015/06/12(金) 14:28:57
 *
 * @param[in] const s32 iId ID.
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptEffectManager::Delete( const s32 iId )
{
  
  for( s32 ii=0 ; ii<m_iEntry ; ii++ )
  {
    if( m_Effect[ii].m_iId == iId )
    {
      effDelete( ii );
      break;
    }
  }
  
}


//------------------------------------------------------------------------------
/**
 * @func        Update
 * @brief       更新.
 * @author      N.Takeda
 * @date        2015/06/12(金) 11:58:21
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptEffectManager::Update()
{
  
  for( s32 ii=0 ; ii<m_iEntry ; ii++ )
  {
    if( m_Effect[ii].m_iId != NO_ID )
    {
      if( m_Effect[ii].m_pPtr->IsAnimationLastFrame() && m_Effect[ii].m_bNoLoop )
      {
        effDelete( ii );
        ii--;   ///< m_iEntry の値が変わるので.
      }
    }
  }
  
  /// リソースの開放. ///< Mcat#1560:試練４：キアヴェの試練開始直後にハング 対応でリクエスト形式に
  for( s32 ii=0 ; ii<m_iReleaseRequestNum ; ii++ )
  {
    Field::Effect::Type   eType = TranslateId( m_uReleaseRequest[ ii ] );
    s32   iCnt = m_pGameMan->GetFieldmap()->GetEffectManager()->GetEffectNum( eType );
    if( iCnt == 0 )  ///< 一応チェック. 厳密にはここで EffectNum が 0 でも開放されているとは限らない.
    {
      unloadDynamicResource( eType );
      m_iReleaseRequestNum--;
      m_uReleaseRequest[ ii ] = m_uReleaseRequest[ m_iReleaseRequestNum ];
      m_uReleaseRequest[ m_iReleaseRequestNum ] = Effect::EFFECT_TYPE_ERROR;
      ii--;
    }
  }
  
}


//------------------------------------------------------------------------------
/**
 * @func        CheckAllEnd
 * @brief       エフェクトが全て削除されているかチェック.
 * @author      N.Takeda
 * @date        2015/12/14(月) 19:48:27
 *
 * @return      bool  TRUE:成 / FALSE:否
 */
//------------------------------------------------------------------------------
bool  ScriptEffectManager::CheckAllEnd()
{
  for( s32 ii=0 ; ii<EFFECTMAX ; ii++ )
  {
    if( m_Effect[ii].m_iId != NO_ID )  return false;
  }
  return true;
}


void ScriptEffectManager::loadDynamicResource( const Effect::Type iType )
{
  if( !m_pGameMan->GetFieldmap()->GetEffectManager()->IsLoadedEffectResource( static_cast<Effect::Type>(iType) ) )
  {
    if( m_DynamicLoadNum >= DYNAMIC_EFFECT_LOAD_MAX ){
      GFL_ASSERT_MSG(0,"非常駐エフェクトのロード種類数オーバー MAX=%d\n",DYNAMIC_EFFECT_LOAD_MAX);
      return;
    }
    gfl2::heap::HeapBase * pheap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    m_pGameMan->GetFieldmap()->GetEffectManager()->LoadDynamicEffectResource( static_cast<Effect::Type>(iType), pheap, true );
    m_pGameMan->GetFieldmap()->GetEffectManager()->SetupDynamicEffectResource( static_cast<Effect::Type>(iType), pheap );
    m_DynamicLoad[ m_DynamicLoadNum ] = static_cast<Effect::Type>(iType);
    ++m_DynamicLoadNum;
  }
}

void ScriptEffectManager::unloadDynamicResource( const Effect::Type iType )
{
  gfl2::heap::HeapBase * pheap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  
  for( u32 i=0; i<m_DynamicLoadNum; ++i )
  {
    if(m_DynamicLoad[i] == iType)
    {
      m_pGameMan->GetFieldmap()->GetEffectManager()->UnLoadDynamicEffectResource( static_cast<Effect::Type>(m_DynamicLoad[i]), pheap );
      m_DynamicLoadNum--;
      m_DynamicLoad[i] = m_DynamicLoad[m_DynamicLoadNum];
      m_DynamicLoad[m_DynamicLoadNum] = Effect::EFFECT_TYPE_ERROR;
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @func        LoadDynamicResource
 * @brief       動的生成したリソースを読み込み
 * @author      takahashi
 * @date        2015/11/11(火) 23::23::00
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void ScriptEffectManager::LoadDynamicResource( const s32 iEffectType )
{
  loadDynamicResource( TranslateId(iEffectType) );
}

//------------------------------------------------------------------------------
/**
 * @func        UnloadDynamicResource
 * @brief       動的生成したリソースを破棄
 * @author      takahashi
 * @date        2015/11/11(火) 23::23::00
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void ScriptEffectManager::UnloadDynamicResource( const s32 iEffectType )
{
  unloadDynamicResource( TranslateId(iEffectType) );
}

//------------------------------------------------------------------------------
/**
 * @func        UnloadDynamicResource
 * @brief       動的生成したリソースを破棄
 * @author      takahashi
 * @date        2015/11/11(火) 23::23::00
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptEffectManager::UnloadDynamicResource(void)
{
  gfl2::heap::HeapBase * pheap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  
  /// エフェクトの終了チェック.
  GFL_ASSERT( CheckAllEnd() );
  
  for( u32 i=0; i<m_DynamicLoadNum; ++i )
  {
    if(m_pGameMan->GetFieldmap())  m_pGameMan->GetFieldmap()->GetEffectManager()->UnLoadDynamicEffectResource( static_cast<Effect::Type>(m_DynamicLoad[i]), pheap );
    m_DynamicLoad[i] = Effect::EFFECT_TYPE_ERROR;
  }

  m_DynamicLoadNum = 0;
}


//------------------------------------------------------------------------------
/**
 * @func        Terminate
 * @brief       終了処理. ゾーン移動で呼ばれる. EffectManager のTerminateと同期させる.
 * @author      N.Takeda
 * @date        2015/06/15(月) 13:10:43
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptEffectManager::Terminate()
{
  for( s32 ii=0 ; ii<m_iEntry ; ii++ )
  {
    if(m_pGameMan->GetFieldmap())  m_pGameMan->GetFieldmap()->GetEffectManager()->DeleteEffect( m_Effect[ ii ].m_pPtr );

    m_Effect[ ii ].m_iId = NO_ID;
    m_Effect[ ii ].m_pPtr = NULL;
  }
  m_iEntry = 0;
}


//------------------------------------------------------------------------------
/**
 * @func        effDelete
 * @brief       effDelete関数.
 * @author      N.Takeda
 * @date        2015/06/15(月) 13:04:57
 *
 * @param[in] const s32 iId ID.
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptEffectManager::effDelete( const s32 iId )
{
  
  if(m_pGameMan->GetFieldmap())  m_pGameMan->GetFieldmap()->GetEffectManager()->DeleteEffect( m_Effect[ iId ].m_pPtr );
  
  /// 末端のエフェクトを消した部分に持ってくる.
  m_iEntry--;
  GFL_ASSERT( 0 <= m_iEntry );
  m_Effect[ iId ] = m_Effect[ m_iEntry ];
  initEffect( m_iEntry );
}

//------------------------------------------------------------------------------
/**
 * @func        GetEffect
 * @brief       エフェクト取得.
 * @author      saita_kazuki
 * @date        2015/06/15(月) 16:37
 *
 * @param[in] const s32 iId ID.
 *
 * @return      StEffectポインタ
 */
//------------------------------------------------------------------------------
Field::Effect::IEffectBase* ScriptEffectManager::GetEffect( const s32 iId )
{
  for( s32 ii=0 ; ii<m_iEntry ; ii++ )
  {
    if( m_Effect[ii].m_iId == iId )
    {
      return m_Effect[ ii ].m_pPtr;
    }
  }
  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        ResourceReleaseRequest
 * @brief       リソースの開放リクエスト. ///< Mcat#1560:試練４：キアヴェの試練開始直後にハング 対応でリクエスト形式に
 * @author      N.Takeda
 * @date        2016/03/11(金) 12:47:43
 *
 * @param[in] const u32 _uEffectType  タイプ.
 *
 * @return      bool  TRUE:成 / FALSE:否
 */
//------------------------------------------------------------------------------
bool  ScriptEffectManager::ResourceReleaseRequest( const u32 _uEffectType )
{
  GFL_ASSERT( m_iReleaseRequestNum < EFFECTMAX );
  if( m_iReleaseRequestNum >= EFFECTMAX ) return false;
  
  m_uReleaseRequest[ m_iReleaseRequestNum ] = _uEffectType;
  m_iReleaseRequestNum++;
  
  return true;
}


GFL_NAMESPACE_END( FieldScript );
GFL_NAMESPACE_END( Field );

