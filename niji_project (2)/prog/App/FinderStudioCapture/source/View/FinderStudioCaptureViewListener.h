#if !defined __FINDER_STUDIO_CAPTURE_VIEW_LISTENER_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_VIEW_LISTENER_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureViewListener.h
 * @date    2016/11/28 16:33:42
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：View用リスナー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>

#include <model/include/gfl2_DressUpParam.h>
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <types/include/gfl2_Typedef.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara )
class PokemonParam;
GFL_NAMESPACE_END( pokepara )
GFL_NAMESPACE_END( pml )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影準備用リスナー
///
//////////////////////////////////////////////////////////////////////////
class PrepareListener
{
public:
  // 撮影モードの設定
  virtual void SetCaptureMode( Viewer::CaptureMode mode )
  {
    GFL_UNUSED( mode );
  }
  // 撮影モードの取得
  virtual Viewer::CaptureMode GetCaptureMode( void )
  {
    return Viewer::CAPTURE_MODE_BOTH;
  }
  // 撮影するポケモンの設定
  virtual void SetPokemon( pml::pokepara::PokemonParam* pp, u32 index )
  {
    GFL_UNUSED( pp );
    GFL_UNUSED( index );
  }
  // ビューワー側のポケモン切り替えの完了
  virtual bool IsFinishPokemon( void )
  {
    return true;
  }
  // 撮影する背景の設定
  virtual void SetBackGround( u32 index, Viewer::TimeZone timeZone )
  {
    GFL_UNUSED( index );
    GFL_UNUSED( timeZone );
  }
  // ビューワー側の背景切り替えの完了
  virtual bool IsFinishBackGround( void )
  {
    return true;
  }

  // 撮影する時間帯の設定
  virtual void SetTimeZone( Viewer::TimeZone timeZone )
  {
    GFL_UNUSED( timeZone );
  }
  // 撮影する時間帯の取得
  virtual Viewer::TimeZone GetTimeZone( void )
  {
    return Viewer::TIME_ZONE_MAX;
  }

  // 着せ替えデータを変更するセーブデータ領域のインデックス
  virtual void SetDressUpSaveIndex( poke_3d::model::DressUpParam dressUpParam, u8 index )
  {
    GFL_UNUSED( dressUpParam );
    GFL_UNUSED( index );
  }
  // ビューワー側の衣装更新
  virtual void SetDressUp( poke_3d::model::DressUpParam dressUpParam, u8 dressupIndex )
  {
    GFL_UNUSED( dressUpParam );
    GFL_UNUSED( dressupIndex );
  }
  // ビューワー側の衣装切り替えの完了
  virtual bool IsFinishDressup( void )
  {
    return true;
  }

  // 構図設定
  virtual void SetComposition( Viewer::Composition composition )
  {
    GFL_UNUSED( composition );
  }
  // 構図取得
  virtual Viewer::Composition GetComposition( void )
  {
    return Viewer::COMPOSITION_MAX;
  }
  // 使用可能構図
  virtual bool IsCanUseComposition( Viewer::Composition composition )
  {
    return true;
  }
};




//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影カメラ操作リスナー
///
//////////////////////////////////////////////////////////////////////////
class CameraControlListener
{
public:
  // カメラ X軸回転
  virtual void CameraRotateXUp( void ) { ; }
  // カメラ X軸回転
  virtual void CameraRotateXDown( void ) { ; }
  // カメラ Y軸回転
  virtual void CameraRotateYLeft( void ) { ; }
  // カメラ Y軸回転
  virtual void CameraRotateYRight( void ) { ; }
  // カメラ Y軸回転取得 : 0 - 360
  virtual f32  GetCameraRotateY( void )
  {
    return 0.0f;
  }

  // カメラ Y軸平行移動
  virtual void CameraMoveYUp( void ) { ; }
  // カメラ Y軸平行移動
  virtual void CameraMoveYDown( void ) { ; }
  // カメラ Y軸高さ設定
  virtual void SetCameraHeightRate( f32 rate )
  {
    GFL_UNUSED( rate );
  }
  // カメラ Y軸高さ取得
  virtual f32  GetCameraHeightRate( void )
  {
    return 0.0f;
  }

  // カメラ 注視点の移動
  virtual void CameraLookAtPlayer( void ) { ; }
  // カメラ 注視点の移動
  virtual void CameraLookAtPokemon( void ) { ; }
  // カメラ 注視度合いの設定
  virtual void SetCameraLookAtRate( f32 rate )
  {
    GFL_UNUSED( rate );
  }
  // カメラ 注視度合いの取得
  virtual f32  GetCameraLookAtRate( void )
  {
    return 0.0f;
  }

  // カメラ ズーム
  virtual void CameraZoomIn( void ) { ; }
  // カメラ ズーム
  virtual void CameraZoomOut( void ) { ; }
  // カメラ ズーム度合い設定
  virtual void SetCameraZoomRate( f32 rate )
  {
    GFL_UNUSED( rate );
  }
  // カメラ ズーム度合い取得
  virtual f32  GetCameraZoomRate( void )
  {
    return 0.0f;
  }
};




//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影対象操作リスナー
///
//////////////////////////////////////////////////////////////////////////
class TargetControlListener
{
public:
  // プレイヤー 回転 左右 取得
  virtual bool PlayerRotateLeft( void )
  {
    return false;
  }
  virtual bool PlayerRotateRight( void )
  {
    return false;
  }
  virtual bool SetPlayerRotate( f32 degree )
  {
    GFL_UNUSED( degree );
    return false;
  }
  virtual f32 GetPlayerRotate( void )
  {
    return 0.0f;  // 角度
  }
  virtual f32 GetViewerPlayerRotate( void )
  {
    return 0.0f;  // 角度
  }

  // プレイヤーAABBの取得
  virtual gfl2::math::AABB GetPlayerAABB( void )
  {
    return gfl2::math::AABB();
  }

  // ポケモン 回転 左右 取得
  virtual bool PokemonRotateLeft( void )
  {
    return false;
  }
  virtual bool PokemonRotateRight( void )
  {
    return false;
  }
  virtual bool SetPokemonRotate( f32 degree )
  {
    GFL_UNUSED( degree );
    return false;
  }
  virtual f32 GetPokemonRotate( void )
  {
    return 0.0f;  // 角度
  }
  virtual f32 GetViewerPokemonRotate( void )
  {
    return 0.0f;  // 角度
  }

  // ポケモン 位置設定
  virtual bool SetPokemonPosition( gfl2::math::Vector3 pos, bool isLift )
  {
    GFL_UNUSED( pos );
    GFL_UNUSED( isLift );
    return false;
  }
  // ポケモン 位置取得：初期位置取得で使用
  virtual gfl2::math::Vector3 GetPokemonPosition( void )
  {
    return gfl2::math::Vector3( 0, 0, 0 );
  }
  // ポケモン 位置取得：Viewer座標値
  virtual gfl2::math::Vector3 GetPokemonViewerPosition( void )
  {
    return gfl2::math::Vector3( 0, 0, 0 );
  }

  // ポケモンAABBの取得
  virtual gfl2::math::AABB GetPokemonAABB( void )
  {
    return gfl2::math::AABB();
  }
};




//////////////////////////////////////////////////////////////////////////
///
/// @brief    モーション用リスナー
///
//////////////////////////////////////////////////////////////////////////
class MotionControlListener
{
public:
  // Zモーションタイプの設定
  virtual void SetTrainerMotionType( Viewer::ZMotionType type )
  {
    GFL_UNUSED( type );
  }
  // Zモーションタイプの取得
  virtual Viewer::ZMotionType GetTrainerMotionType( void )
  {
    return Viewer::Z_MOTION_TYPE_MAX;
  }
  // モーション再生：プレイヤー
  virtual void TrainerMotionStart( Viewer::ZMotionType type = Viewer::Z_MOTION_TYPE_MAX )
  {
    GFL_UNUSED( type );
  }
  // モーション停止
  virtual void TrainerMotionStop( void ) { ; }
  // モーションリセット
  virtual void TrainerMotionReset( void ) { ; }
  // モーション再生中か？
  virtual bool IsPlayingTrainerMotion( void )
  {
    return false;
  }

  // モーション再生：ポケモン
  virtual void PokemonMotionStart( void ) { ; }
  // モーションタイプの設定
  virtual Viewer::PokeMotionType ChangePokemonMotionType( bool bReset )
  {
    return Viewer::POKE_MOTION_TYPE_MAX;
  }
  // モーションタイプの取得
  virtual Viewer::PokeMotionType GetPokemonMotionType( void )
  {
    return Viewer::POKE_MOTION_TYPE_MAX;
  }
  // モーションタイプの変更済み
  virtual bool IsChangePokemonMotionType( void )
  {
    return true;
  }
  // モーションタイプの変更でリセットが必要かどうか
  virtual bool IsNeedResetPokemonMotionType( void )
  {
    return true;
  }
  // ポケモンの再生可能なモーションの割合
  virtual f32 GetPokemonMotionPlayRate( void )
  {
    return 0.0f;
  }

  // ポケモンの自動モーションの設定
  virtual void SetPokemonMotionAuto( bool enable )
  {
    GFL_UNUSED(enable);
  }
};




//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影用リスナー
///
//////////////////////////////////////////////////////////////////////////
class CaptureListener
{
public:
  // 撮影開始
  virtual void StartCapture( void ) { ; }
  // 撮影終了
  virtual bool IsFinishCapture( void )
  {
    return true;
  }
};




//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真用リスナー
///
//////////////////////////////////////////////////////////////////////////
class PhotoListener
{
public:
  // 上画面に表示する写真を設定
  virtual void SetUpperPhotoImage( u32 index )
  {
    GFL_UNUSED( index );
  }
  // 写真テクスチャの取得
  virtual void* GetPhotoTexture( u32 index )
  {
    GFL_UNUSED( index );
    return NULL;
  }
};




//////////////////////////////////////////////////////////////////////////
///
/// @brief    上画面用リスナー
///
//////////////////////////////////////////////////////////////////////////
class UpperViewListener
{
public:
  /// 照準の表示切替
  virtual void VisibleSight( void ) { ; }
  virtual void InvisibleSight( void ) { ; }

  /// シャッター 開く
  virtual void ShutterOpen( void ) { ; }
  virtual bool IsFinishShutterOpen( void )
  {
    return true;
  }

  /// シャッター 閉じる
  virtual void ShutterClose( void ) { ; }
  virtual bool IsFinishShutterClose( void )
  {
    return true;
  }
};




//////////////////////////////////////////////////////////////////////////
///
/// @brief    View用リスナー
///
//////////////////////////////////////////////////////////////////////////
class ViewListener
  : public PrepareListener
  , public CameraControlListener
  , public TargetControlListener
  , public MotionControlListener
  , public CaptureListener
  , public PhotoListener
  , public UpperViewListener
{
public:
  // ビューワーの更新が可能か？
  // @return "true  == 更新可"
  // @return "false == 更新不可"
  virtual bool IsUpdateViewer( void )
  {
    return true;
  }

  // @brief   Viewer更新が終了したか？
  // @retval  "true"なら終了
  // @retval  "false"ならそれ以外
  virtual bool IsFinishUpdateViewer( void )
  {
    return true;
  }

  // @brief   Viewer更新をリクエストしたか？
  // @retval  "true"ならリクエスト済み
  // @retval  "false"ならそれ以外
  virtual bool IsRequestUpdateViewer( void )
  {
    return false;
  }

  /// 画面遷移の開始を通知
  /// 画面遷移のためPrepareCommonViewの下に表示されているViewの入力を禁止とする。
  virtual void StartScreenTransition( void ) { ; }

  /// ビューワーリセット：カーテン演出あり
  virtual void ViewerReset( void ) { ; }
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_VIEW_LISTENER_H_INCLUDED__
