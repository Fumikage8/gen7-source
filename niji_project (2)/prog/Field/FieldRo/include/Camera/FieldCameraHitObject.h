/**
* @brief  フィールドカメラを管理するクラス
* @file   FieldCameraHitObject.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERAHITOBJECTER_H_INCLUDED__ )
#define __FIELDCAMERAHITOBJECTER_H_INCLUDED__
#pragma once


// gfl2
#include <heap/include/gfl2_Heap.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

// niji
#include "System/include/nijiAllocator.h"

// field
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

/**
* @brief フィールドカメラを管理するクラス
*/
class CameraHitObject
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( CameraHitObject );

public:

  struct Mode
  {
    enum Value
    {
      VISIBLE = 0,  // visibleの切り替えのみ
      ALPHA,        // アルファ遷移
    };
  };

  struct MeshType
  {
    enum Value
    {
      BASE = 0,     // 元メッシュ
      ALPHA,        // 透過時のメッシュ( Mode::ALPHA時のみ有効 )

      MAX
    };
  };


  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:

    InitializeDescription() :
      m_pHeap( NULL )
      ,m_pMeshLink( NULL )
      ,m_pMeshLinkAfter( NULL )
      ,m_pModelInstanceNode( NULL )
      ,m_Mode( Mode::VISIBLE )
    {
    }


    gfl2::heap::HeapBase*                                                           m_pHeap;
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData*   m_pMeshLink;
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData*   m_pMeshLinkAfter;     // 操作対象メッシュ(透過後)
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*                 m_pModelInstanceNode;

    Mode::Value                                                                     m_Mode;
  };

  struct State
  {
    enum Value
    {
      DEF = 0,      // 通常表示
      DOWN_ALPHA,   // アルファ下降中
      UP_ALPHA,     // アルファ上昇中
      HIDE,         // 完全非表示
    };
  };


  struct Setting
  {
    s32 downFrame;
    gfl2::math::Easing::EaseFunc downEaseFunc;
    s32 upFrame;
    gfl2::math::Easing::EaseFunc upEaseFunc;
  };


  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  CameraHitObject( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~CameraHitObject( void );
  

  /**
  * @brief 初期化処理
  *
  * @param  rInitDesc   初期化設定
  *
  * @return 無し
  */
  void Initialize( const InitializeDescription& rInitDesc );

  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Terminate( void );
  
  /**
  * @brief 更新事前処理
  *
  * @return 無し
  */
  void PreUpdate( void );

  /**
  * @brief Hitした際に実行する
  *
  * @return 無し
  */
  void Hit( void );

  /**
  * @brief 更新処理
  *
  * @return 無し
  */
  void Update( void );

  /**
  * @brief カメラ接触でメッシュの非表示にする機能を有効化する
  *
  * @param oneTime true:透過遷移を挟さまない false:透過遷移を挟む
  */
  void HideOn( b32 oneTime = true );

  /**
  * @brief カメラ接触でメッシュの非表示にする機能を無効化する
  *
  * @param oneTime true:透過遷移を挟さまない false:透過遷移を挟む
  */
  void HideOff( b32 oneTime = true );

#if PM_DEBUG
  /**
  * @brief 設定構造体のポインタを取得
  *
  * @return ポインタ
  */
  static Setting* GetSettingPtr( void );

  /**
  * @brief 情報出力 
  *
  * @return 無し
  */
  void Dump( void ) const;
#endif

private:

  /**
  * @brief 更新処理(visibleの切り替えのみ)
  *
  * @return 無し
  */
  void UpdateModeVisible( void );
  
  /**
  * @brief 更新処理(アルファ遷移)
  *
  * @return 無し
  */
  void UpdateModeAlpha( void );

  /**
  * @brief 半透明へ遷移する設定を行う
  *
  * @param  nowValue        開始時のアルファ
  * @param  updateMaterial  マテリアル設定を半透明に書き換えるか
  *
  * @return 無し
  */
  void SetupAlphaDownStart( f32 nowValue, b32 updateMaterial = true );

  /**
  * @brief 透過遷移完了処理(Down完了)
  *
  * @return 無し
  */
  void SetupAlphaDownEnd( void );

  /**
  * @brief 透過遷移完了処理(UP完了)
  *
  * @return 無し
  */
  void SetupAlphaUpEnd( void );
  
  /**
  * @brief 不透明へ遷移する設定を行う
  *
  * @param  nowValue        開始時のアルファ
  * @param  updateMaterial  マテリアル設定を半透明に書き換えるか
  *
  * @return 無し
  */
  void SetupAlphaUpStart( f32 nowValue, b32 updateMaterial = true );

  /**
  * @brief 現在のマテリアル設定を通常設定として保存する
  *
  * @return 無し
  */
  void MemoryMaterialDefault( void );

  /**
  * @brief マテリアル設定を通常設定にする
  *
  * @return 無し
  */
  void SetupMaterialDefault( void );

  /**
  * @brief 内部制御のアルファ値をマテリアルに反映する
  *
  * @return 無し
  */
  void ApplyAlphaToMaterial(void );

  /**
  * @brief 表示・非表示を設定にする
  *
  * @param meshType 操作対象メッシュ
  * @param flag true表示、false非表示
  *
  * @return 無し
  */
  void SetVisible( MeshType::Value meshType, b32 flag );

#if PM_DEBUG
  /**
  * @brief メッシュ情報出力
  *
  * @param 対象メッシュ
  *
  * @return 無し
  */
  void DumpMesh( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMesh ) const;
#endif


  // 内部制御
  Mode::Value                                                                     m_Mode;
  b32                                                                             m_IsHit;                    // hit中ならtrue
  b32                                                                             m_IsHideStop;               // 非表示機能、停止中ならtrue
  b32                                                                             m_OneTimeHideReq;           // trueなら1フレームで遷移完了させる(受理したらクリア)
  State::Value                                                                    m_State;                    // 現在の状態
  Field::Camera::Area::ValueController                                            m_ValueControllerForAlpha;  // 透明度を管理するコントローラー
  
  struct FaceMemoryData;
  FaceMemoryData*                                                                 m_pMemoryData;
  
  // 外部オブジェクト
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData*   m_pMeshLink[ MeshType::MAX ];  // 操作対象メッシュ
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*                 m_pModelInstanceNode; // メッシュが所属するモデル(マテリアル操作に必要)





};  // class FieldCameraHitObject


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERAHITOBJECTER_H_INCLUDED__