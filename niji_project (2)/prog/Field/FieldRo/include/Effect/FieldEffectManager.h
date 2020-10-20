/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectManager.h
 *  @brief  エフェクト管理
 *  @author miyachi_soichi
 *  @date   2015.05.20
 */

#if !defined(__FIELD_EFFECT_MANAGER_H__)
#define __FIELD_EFFECT_MANAGER_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>

//Field
#include <Field/FieldRo/include/Effect/FieldEffectBase.h>
#include "niji_conv_header/field/FieldEffectTypes.h"

#include "Field/FieldStatic/include/FieldWorldDefine.h"

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
class AsyncFileManager;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

// 先行宣言
class WordSetLoader;
GFL_NAMESPACE_BEGIN( Field )
class MyRenderingPipeLine;
class IField3DObjectNode;
class FieldRootNode;
GFL_NAMESPACE_BEGIN( Camera )
class CameraManager;
GFL_NAMESPACE_END( Camera )
GFL_NAMESPACE_BEGIN(FieldWindow)
class FieldWindowManager;
GFL_NAMESPACE_END(FieldWindow)
GFL_NAMESPACE_BEGIN(TrialModel)
class FieldTrialModel;
GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_BEGIN( Effect )
class EffectPlaceName;
class EffectTrainerFocus;

class EffectManager
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( EffectManager );

public:
  enum
  {
    SYS_EFFECT_NUM  =  30,                               //!< システムエフェクト発生数
    EVT_EFFECT_NUM  =   4,                               //!< イベントエフェクト発生数
    WTR_EFFECT_NUM  =   6,                               //!< 天候エフェクト発生数
    RID_EFFECT_NUM =    4,                               //!< らいどエフェクト発生数

    MAX_EFFECT_NUM  = SYS_EFFECT_NUM + EVT_EFFECT_NUM + WTR_EFFECT_NUM+RID_EFFECT_NUM,  //!< 最大エフェクト発生数
  };

  // ワークタイプ指定肢
  enum WorkType
  {
    WORK_TYPE_SYS = 0,     // システム枠
    WORK_TYPE_EVT,         // イベント枠
    WORK_TYPE_WTR,         // 天候エフェクト枠
    WORK_TYPE_RID,         // ライド枠
 
    WORK_TYPE_DEFAULT      // リソースデフォルト設定
  };


  struct SetupParam
  {
    gfl2::heap::HeapBase                    *pHeapBase;
    Field::MyRenderingPipeLine              *pPipeLine;
    gfl2::fs::BinLinkerAccessor             *pResource;
    Field::FieldRootNode                    *pRootNode;
    Field::Camera::CameraManager            *pCameraManager;
    Field::FieldWindow::FieldWindowManager  *pWindowManager;
    gfl2::Effect::GFHeap                    *pEffectHeap;
    gfl2::Effect::System                    *pEffectSystem;
    WordSetLoader                           *pWordSetLoader;
    gfl2::fs::AsyncFileManager              *pFileManager;
    const void *                            cpEncountGrassColorTable;
    WorldID                                 worldID;
  };
public:
  /**
   *  @brief  コンストラクタ
   */
  EffectManager( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectManager( void );

  /**
   *  @brief  初期化：常駐エフェクト
   */
  void InitializeCommon( void *pCommonPack, void *pLayoutPack );

  /**
   *  @brief  初期化
   *  @param  param   初期化用パラメータ
   */
  void Initialize( const SetupParam &param );

  /**
   *  @brief  初期化の完了待ち
   */
  bool InitializeWait();

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  更新処理
   */
  void Update( void );

  /**
   *  @brief 非常駐リソースの読み込み
   *  @param  in_eType  エフェクト種別
   *  @param  in_pHeap  読み込みヒープ
   */
  void LoadDynamicEffectResource( Field::Effect::Type in_eType, gfl2::heap::HeapBase* in_pHeap, b32 sync = false );

  /**
   *  @brief 非常駐リソースの読み込み
   *  @param  in_eType  エフェクト種別
   */
  void SetupDynamicEffectResource( Field::Effect::Type in_eType, gfl2::heap::HeapBase* in_pHeap );

  /**
   *  @brief 非常駐リソースの読み込みが完了しているか確認
   */
  b32 IsLoadedEffectResource( Field::Effect::Type in_eType ) const;
  
  /**
   *  @brief 非常駐リソースの破棄
   *  @param  in_eType  エフェクト種別
   *  @param  in_pHeap  生成で指定したヒープ
   *  @param  isTerminate フィールド破棄時の解放（＝チェックが緩い）
   *
   *  @note   基本はisTerminate=falseにて破棄してください
   *          ただし、FieldmapがTerminateに移行しており、EffMgrのUpdateが回らないなどの理由で
   *          各EffectBaseの状態が移行できずAssertになる場合、isTerminate=trueにて回避をしてください
   */
  void UnLoadDynamicEffectResource( Field::Effect::Type in_eType, gfl2::heap::HeapBase* in_pHeap, bool isTerminate = false );

  /**
   *  @brief  エフェクト生成処理：オブジェクト指定
   *  @param  in_eType  エフェクト種別指定
   *  @param  in_pObject マウントするオブジェクトのポインタ
   *  @param  in_bPlaySe    SEを再生する
   *  @return 生成したエフェクトのポインタを返却する
   *
   *  エフェクトの発生に対してオブジェクトを要求するタイプのエフェクトを作成します
   */
  Field::Effect::IEffectBase *CreateEffect( Field::Effect::Type in_eType, Field::IField3DObjectNode *in_pObject, bool in_bPlaySe = false, WorkType workType = WORK_TYPE_DEFAULT, s32* pIndex = NULL );

  /**
   *  @brief  エフェクト生成処理：座標指定
   *  @param  in_eType      エフェクト種別指定
   *  @param  in_vPosition  エフェクト生成起点位置情報
   *  @param  in_bPlaySe    SEを再生する
   *  @param  workType
   *  @param  pIndex
   *  @param  in_fScale     拡大率指定
   *  @param  in_vRotation  回転角度指定
   *  @return 生成したエフェクトのポインタを返却する
   *
   *  エフェクトの発生に対して座標を要求するタイプのエフェクトを作成します
   */
  Field::Effect::IEffectBase *CreateEffect(Field::Effect::Type in_eType, const gfl2::math::Vector3 &in_vPosition, bool in_bPlaySe = false, WorkType workType = WORK_TYPE_DEFAULT, s32* pIndex = NULL, const f32 in_fScale = 1.0f, const gfl2::math::Vector3 &in_vRotation = gfl2::math::Vector3(0.0f, 0.0f, 0.0f));

  /**
   *  @brief  エフェクト取得処理：データIndex指定
   *  @param  data_index 取得するエフェクトのIndex
   *
   *  @return エフェクトのポインタを返却する
   */
  Field::Effect::IEffectBase* GetEffect( s32 data_index );

  /**
   *  @brief  エフェクト破棄処理：エフェクト指定
   *  @param  in_pEffect  破棄するエフェクトのポインタ
   *
   *  引数にて引き渡されたエフェクトを破棄します
   */
  void DeleteEffect( Field::Effect::IEffectBase *in_pEffect );

  /**
   *  @brief  エフェクト破棄処理：親オブジェクト指定
   *  @param  in_pObject   マウントしている親オブジェクト
   *
   *  親オブジェクト破棄に伴いエフェクトを破棄する
   */
  void DeleteEffect( Field::IField3DObjectNode *in_pObject );

  /**
   *  @brief  リソースの取得
   *  @param  in_eType  エフェクト種別指定
   *  @return 指定されたエフェクトのリソースを返却する
   */
  void *GetResource( Field::Effect::Type in_eType );

  /**
   *  @brief  地名表示リクエスト
   *  @param  bForce    強制で出す
   */
  void RequestStartPlaceName( bool bForce );

  /**
   *  @brief  地名表示強制けし
   */
  void RequestEndPlaceName( void );

  /**
   *  @brief  トレーナー集中エフェクトを取得
   */
  EffectTrainerFocus* GetTrainerFocus( void ){ return m_pTrainerFocus; }
  
  /**
   *  @brief  指定タイプのエフェクト数を取得する
   *  @param  in_eType    タイプの指定
   *  @return 生存中のエフェクト数を返却する
   */
  s32 GetEffectNum( Field::Effect::Type in_eType );

  /**
   *  @brief  指定タイプのエフェクト数を取得する
   *  @param  in_eType    タイプの指定
   *  @return 生存中のエフェクト数を返却する
   *
   *  @note   削除直前用にExecのみを探すモード
   */
  s32 GetEffectNumPreTerminate( Field::Effect::Type in_eType );

private:

  /**
   *  @brief  エフェクト配列の空きを取得する
   *  @param  in_eType      エフェクト種別指定
   *  @param  in_eWorkType  エフェクトワーク種別指定
   *  @retval ERROR_INDEX  空きが存在しない
   *  @retval 空きインデックスを返却する
   */
  s32 GetEmptyIndex( Field::Effect::Type in_eType, WorkType in_eWorkType = WORK_TYPE_DEFAULT ) const;

  /**
   *  @brief  エフェクト配列の空きを取得する
   *  @param  in_eType  エフェクト種別指定
   *  @retval ERROR_INDEX  空きが存在しない
   *  @retval 空きインデックスを返却する
   */
  s32 GetEmptyIndexByEffectType( Field::Effect::Type in_eType ) const;

  /**
   *  @brief  エフェクト配列の空きを取得する
   *  @param  in_eType  ワーク種別指定
   *  @retval ERROR_INDEX  空きが存在しない
   *  @retval 空きインデックスを返却する
   */
  s32 GetEmptyIndexByWorkType( WorkType in_eType ) const;

  /**
   *  @brief  エフェクト配列の空きを取得する：システム用
   *  @retval ERROR_INDEX  空きが存在しない
   *  @retval 空きインデックスを返却する
   */
  s32 GetEmptyIndexForSys( void ) const;
  /**
   *  @brief  エフェクト配列の空きを取得する：イベント用
   *  @retval ERROR_INDEX  空きが存在しない
   *  @retval 空きインデックスを返却する
   */
  s32 GetEmptyIndexForWtr( void ) const;
  /**
   *  @brief  エフェクト配列の空きを取得する：イベント用
   *  @retval ERROR_INDEX  空きが存在しない
   *  @retval 空きインデックスを返却する
   */
  s32 GetEmptyIndexForEvt( void ) const;
  /**
   *  @brief  エフェクト配列の空きを取得する：ライド用
   *  @retval ERROR_INDEX  空きが存在しない
   *  @retval 空きインデックスを返却する
   */
  s32 GetEmptyIndexForRID( void ) const;

  /**
   *  @brief 非常駐のエフェクトリソースインデックスを取得 : 非lyt
   */
  s32 GetDynamicResourceIndex3d( Field::Effect::Type in_eType ) const;

  /**
   *  @brief 非常駐のエフェクトリソースインデックスを取得 : lyt
   */
  s32 GetDynamicResourceIndexLyt( Field::Effect::Type in_eType ) const;


  /** 
   *  @brief 非常駐リソースの取得
   */
  void * GetDyanamicResourceData( Field::Effect::Type in_eType );

private:
  Field::Effect::IEffectBase              *m_pEffectArray[ MAX_EFFECT_NUM ];      //!< エフェクト配列
  gfl2::heap::HeapBase                    *m_pEffectHeapArray[ MAX_EFFECT_NUM ];  //!< エフェクト用ヒープ配列
  Field::FieldRootNode                    *m_pFieldRootNode;
  Field::MyRenderingPipeLine              *m_pPipeLine;                           //!< エフェクト登録先のPipeLine
  Camera::CameraManager                   *m_pCameraManager;                      //!< カメラ
  Field::FieldWindow::FieldWindowManager  *m_pWindowManager;                      //!< レイアウト用
  gfl2::Effect::GFHeap                    *m_pEffectHeap;
  gfl2::Effect::System                    *m_pEffectSystem;
  WordSetLoader                           *m_pWordSetLoader;
  gfl2::fs::AsyncFileManager              *m_pFileManager;
  gfl2::fs::BinLinkerAccessor             m_CommonResource;                       //!< 常駐リソースデータ
  gfl2::fs::BinLinkerAccessor             m_LayoutResource;                       //!< 常駐リソースデータ
  gfl2::fs::BinLinkerAccessor             *m_pResource;                           //!< リソースデータ
  bool                                    m_bInitiallized;                        //!< 多重初期化禁止

  void                                    *m_pDynamicResource[ Effect::EFFECT_TYPE_MAX ];  //!< 非常駐リソースデータ
  size_t                                  m_DynamicResourceSize[ Effect::EFFECT_TYPE_MAX ];//!< 非常駐リソースのサイズ

  EffectPlaceName                         *m_pPlaceName;                          //!< 地名表示
  EffectTrainerFocus                      *m_pTrainerFocus;                       //!< トレーナー集中エフェクト

  const void                              *m_cpEncountGrassColorTable;            //!< エンカウント草カラーテーブル

  WorldID                                 m_WorldID;                            //!< ワールドID

}; // class EffectManager

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // !defined(__FIELD_EFFECT_MANAGER_H__)
