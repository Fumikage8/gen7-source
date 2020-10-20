#if !defined(__FINDER_STUDIO_CAPTURE_VIEWER_MANAGER_H_INCLUDED__)
#define __FINDER_STUDIO_CAPTURE_VIEWER_MANAGER_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureViewerManager.h
 * @date    2016/12/22 19:51:28
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：Viewer管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>

#include <PokeTool/include/PokeTool.h>
#include <model/include/gfl2_DressUpParam.h>

#include <niji_conv_header/app/data/finder_studio/finder_studio_bg_def.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Viewer )
class FinderStudioViewer;
GFL_NAMESPACE_END( Viewer )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
struct Work;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    Viewer管理ベース
///
//////////////////////////////////////////////////////////////////////////
class ViewerManagerBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ViewerManagerBase );

protected:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ViewerManagerBase( void );
  virtual ~ViewerManagerBase( void ) { ; }

protected:
  //-------------------------------------------------------
  // @brief   生成・破棄
  //-------------------------------------------------------
  void _CreateViewer( gfl2::heap::HeapBase* heap, const PokeTool::SimpleParam& simpleParam, const poke_3d::model::DressUpParam& dressupParam );
  void _CreateViewer( gfl2::heap::HeapBase* heap, const Viewer::PhotoParam& photoParam, Viewer::ViewerMode viewerMode );
  void _DeleteViewer( void );

  //-------------------------------------------------------
  // @brief   初期化・終了
  //-------------------------------------------------------
  bool _IsFinishInitialize( void );
  bool _IsFinishTerminate( void );

  //-------------------------------------------------------
  // @brief   更新・描画
  //-------------------------------------------------------
  void _Update( void );
  void _Draw( gfl2::gfx::CtrDisplayNo dispNo );

protected:
  //-------------------------------------------------------
  // @brief   Viewerリクエスト関数
  //-------------------------------------------------------
  /// Viewerリセット
  void _ViewerReset( void );

  /// 準備内容
  void _ChangeCaptureMode( Viewer::CaptureMode mode );
  void _ChangePokemon( PokeTool::SimpleParam simpleParam );
  bool _IsFinishChangePokemon( void );
  void _ChangeBackGround( u32 bgID );
  bool _IsFinishChangeBackGround( void );
  void _ChangeTimeZone( Viewer::TimeZone timeZone );
  void _ChangeComposition( Viewer::Composition composition );
  bool _IsAvailableComposition( Viewer::Composition composition );
  Viewer::Composition _GetComposition( void );
  void _ChangeDressup( poke_3d::model::DressUpParam dressupParam );
  bool _IsFinishChangeDressup( void );

  /// トレーナー：回転
  bool _TrainerYAxisRotationLeft( void );
  bool _TrainerYAxisRotationRight( void );
  f32  _GetTrainerYAxisRotationOriginalDegree( void );
  bool _SetTrainerYAxisRotationDegree( f32 degree );
  f32  _GetTrainerYAxisRotationDegree( void );
  /// トレーナー：AABB
  gfl2::math::AABB _GetTrainerAABB( void );
  /// トレーナー：モーション
  void _TrainerMotionPlay( Viewer::ZMotionType type );
  void _TrainerMotionStop( void );
  void _TrainerMotionReset( void );
  bool _IsPlayingTrainerMotion( void );

  /// ポケモン：位置
  gfl2::math::Vector3 _GetPokemonOriginalPosition( void );
  bool _SetPokemonPosition( const gfl2::math::Vector3 position );
  gfl2::math::Vector3 _GetPokemonPosition( void );
  /// ポケモン：回転
  bool _PokemonYAxisRotationLeft( void );
  bool _PokemonYAxisRotationRight( void );
  f32  _GetPokemonYAxisRotationOriginalDegree( void );
  bool _SetPokemonYAxisRotationDegree( f32 degree );
  f32  _GetPokemonYAxisRotationDegree( void );
  /// ポケモン：AABB
  gfl2::math::AABB _GetPokemonAABB( void );
  /// ポケモン：モーション
  void _PokemonMotionPlay( void );
  void _SetPokemonMotionAutoPlayEnable( bool bAuto );
  void _ChangePokemonMotionType( Viewer::PokeMotionType type, bool bReset );
  bool _IsFinishChangePokemonMotionType( void );
  bool _IsNeedResetPokemonMotionType( Viewer::PokeMotionType type );
  f32  _GetPokemonMotionPlayableRate( void );

  /// トレーナーとポケモンの衝突判定
  bool _IsCollisionTrainerAndPokemon( void );

  /// カメラ：回転
  void _CameraXAxisRotationUp( void );
  void _CameraXAxisRotationDown( void );
  void _CameraYAxisRotationLeft( void );
  void _CameraYAxisRotationRight( void );
  f32  _GetCameraYAxisRotationDegree( void );
  /// カメラ：平行移動
  void _CameraYAxisTranslationUp( void );
  void _CameraYAxisTranslationDown( void );
  void _SetCameraYAxisTranslationRate( f32 rate );
  f32  _GetCameraYAxisTranslationRate( void );
  /// カメラ：ズーム
  void _CameraZoomIn( void );
  void _CameraZoomOut( void );
  void _SetCameraZoomRate( f32 rate );
  f32  _GetCameraZoomRate( void );
  /// カメラ：フォーカス
  void _CameraFocusTrainer( void );
  void _CameraFocusPokemon( void );
  void _SetCameraFocusRate( f32 rate );
  f32  _GetCameraFocusRate( void );

  /// 写真再現パラメータ
  void _GetPhotoParam( Viewer::PhotoParam* dstParam );

  /// 撮影
  void _CaptureStart( void** dstBufferArray, Viewer::PhotoParam* dstPhotoParamArray );
  bool _IsFinishCapture( void );

#if PM_DEBUG
public:
  /// デバッグ用：Viewerの取得
  Viewer::FinderStudioViewer* GetFinderStudioViewer( void ) { return m_pViewer; }
#endif

private:
  Viewer::FinderStudioViewer*   m_pViewer;
};


//////////////////////////////////////////////////////////////////////////
///
/// @brief    Viewer管理
///
//////////////////////////////////////////////////////////////////////////
class ViewerManager
  : public ViewerManagerBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ViewerManager );

private:
  /// NULL値：撮影モード
  static const Viewer::CaptureMode CAPTURE_MODE_NULL = Viewer::CAPTURE_MODE_MAX;
  /// NULL値：ポケモンパラメータ
  static PokeTool::SimpleParam SIMPLE_PARAM_NULL;
  /// NULL値：背景ID
  static const u32 BACKGROUND_ID_NULL = BG_ID_NUM;
  /// NULL値：時間帯
  static const Viewer::TimeZone TIME_ZONE_NULL = Viewer::TIME_ZONE_MAX;
  /// NULL値：衣装データ
  static poke_3d::model::DressUpParam DRESSUP_PARAM_NULL;
  /// NULL値：ポケモンモーションタイプ
  static const Viewer::PokeMotionType POKE_MOTION_TYPE_NULL = Viewer::POKE_MOTION_TYPE_MAX;
  /// NULL値：構図ID
  static const Viewer::Composition COMPOSITION_NULL = Viewer::COMPOSITION_MAX;

public:
  /// @brief  Viewerリクエストパラメータ
  struct RequestViewerParam
  {
    Viewer::CaptureMode captureMode;
    PokeTool::SimpleParam simpleParam;
    u32 bgID;
    Viewer::TimeZone timeZone;
    poke_3d::model::DressUpParam dressupParam;
    Viewer::PokeMotionType pokeMotionType;
    Viewer::Composition composition;
    bool bViewerReset;
    bool bTrainerMotionReset;
    bool bPokeMotionTypeReset;    //!< ポケモンモーションタイプ変更時にリセットを行うか

    // @brief コンストラクタ
    RequestViewerParam( void )
      : captureMode( CAPTURE_MODE_NULL )
      , simpleParam( SIMPLE_PARAM_NULL )
      , bgID( BACKGROUND_ID_NULL )
      , timeZone( TIME_ZONE_NULL )
      , dressupParam( DRESSUP_PARAM_NULL )
      , pokeMotionType( POKE_MOTION_TYPE_NULL )
      , composition( COMPOSITION_NULL )
      , bViewerReset( false )
      , bTrainerMotionReset( false )
      , bPokeMotionTypeReset( false )
    {
      ;
    }
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ViewerManager( gfl2::heap::HeapBase* heap );
  virtual ~ViewerManager( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   生成・破棄
  //-------------------------------------------------------
  bool CreateViewer( const PokeTool::SimpleParam& simpleParam, const poke_3d::model::DressUpParam& dressupParam );
  bool CreateViewer( const Viewer::PhotoParam& photoParam, Viewer::ViewerMode viewerMode );
  bool DeleteViewer( void );

  //-------------------------------------------------------
  // @brief   更新・描画
  //-------------------------------------------------------
  void Update( void );
  void Draw( gfl2::gfx::CtrDisplayNo dispNo );

  void StartUpdateViewer( void ) { _StartUpdateViewer(); }
  bool IsFinishUpdadteViewer( void ) { return _IsFinishUpdateViewer(); }

public:
  //-------------------------------------------------------
  // @brief   リクエスト：カーテン演出あり
  //-------------------------------------------------------
  void RequestViewerUpdate_All( RequestViewerParam param );
  void RequestViewerUpdate_CaptureMode( Viewer::CaptureMode mode );
  void RequestViewerUpdate_PokeModel( PokeTool::SimpleParam simpleParam );
  void RequestViewerUpdate_BackGround( u32 bgID, Viewer::TimeZone timeZone );
  void RequestViewerUpdate_Dressup( poke_3d::model::DressUpParam dressupParam );
  void RequestViewerUpdate_PokeMotionType( Viewer::PokeMotionType pokeMotionType, bool bReset );
  void RequestViewerUpdate_Composition( Viewer::Composition composition );
  void RequestViewerUpdate_AllReset( void );
  void RequestViewerUpdate_TrainerMotionReset( void );

  //-------------------------------------------------------
  // @brief   リクエスト：カーテン演出なし
  //-------------------------------------------------------
  void RequestViewerUpdate_TimeZone( Viewer::TimeZone timeZone ) { this->_ChangeTimeZone( timeZone ); }

  bool RequestTrainerYAxisRotationLeft( void )              { return this->_TrainerYAxisRotationLeft(); }
  bool RequestTrainerYAxisRotationRight( void )             { return this->_TrainerYAxisRotationRight(); }
  void RequestTrainerMotionPlay( Viewer::ZMotionType type ) { this->_TrainerMotionPlay( type ); }
  void RequestTrainerMotionStop( void )                     { this->_TrainerMotionStop(); }

  bool RequestPokemonYAxisRotationLeft( void )              { return this->_PokemonYAxisRotationLeft(); }
  bool RequestPokemonYAxisRotationRight( void )             { return this->_PokemonYAxisRotationRight(); }
  void RequestPokemonMotionPlay( void )                     { this->_PokemonMotionPlay(); }
  void RequestSetPokemonMotionAutoPlayEnable( bool bAuto )  { this->_SetPokemonMotionAutoPlayEnable( bAuto ); }

  void RequestCameraXAxisRotationUp( void )      { this->_CameraXAxisRotationUp(); }
  void RequestCameraXAxisRotationDown( void )    { this->_CameraXAxisRotationDown(); }
  void RequestCameraYAxisRotationLeft( void )    { this->_CameraYAxisRotationLeft(); }
  void RequestCameraYAxisRotationRight( void )   { this->_CameraYAxisRotationRight(); }
  void RequestCameraYAxisTranslationUp( void )   { this->_CameraYAxisTranslationUp(); }
  void RequestCameraYAxisTranslationDown( void ) { this->_CameraYAxisTranslationDown(); }
  void RequestCameraZoomIn( void )               { this->_CameraZoomIn(); }
  void RequestCameraZoomOut( void )              { this->_CameraZoomOut(); }
  void RequestCameraFocusTrainer( void )         { this->_CameraFocusTrainer(); }
  void RequestCameraFocusPokemon( void )         { this->_CameraFocusPokemon(); }

  void RequestCaptureStart( void** dstBufferArray, Viewer::PhotoParam* dstPhotoParamArray ) { this->_CaptureStart( dstBufferArray, dstPhotoParamArray ); }

public:
  //-------------------------------------------------------
  // @brief   設定
  //-------------------------------------------------------
  bool SetTrainerYAxisRotationDegree( f32 degree ) { return this->_SetTrainerYAxisRotationDegree( degree ); }

  bool SetPokemonPosition( const gfl2::math::Vector3 position ) { return this->_SetPokemonPosition( position ); }
  bool SetPokemonYAxisRotationDegree( f32 degree )              { return this->_SetPokemonYAxisRotationDegree( degree ); }

  void SetCameraYAxisTranslationRate( f32 rate ) { this->_SetCameraYAxisTranslationRate( rate ); }
  void SetCameraZoomRate( f32 rate )             { this->_SetCameraZoomRate( rate ); }
  void SetCameraFocusRate( f32 rate )            { this->_SetCameraFocusRate( rate ); }

  //-------------------------------------------------------
  // @brief   取得
  //-------------------------------------------------------
  Viewer::Composition GetComposition( void ) { return _GetComposition(); }

  f32 GetTrainerYAxisRotationOriginalDegree( void ) { return this->_GetTrainerYAxisRotationOriginalDegree(); }
  f32 GetTrainerYAxisRotationDegree( void )         { return this->_GetTrainerYAxisRotationDegree(); }
  gfl2::math::AABB GetTrainerAABB( void )           { return this->_GetTrainerAABB(); }

  gfl2::math::Vector3 GetPokemonOriginalPosition( void ) { return this->_GetPokemonOriginalPosition(); }
  gfl2::math::Vector3 GetPokemonPosition( void )         { return this->_GetPokemonPosition(); }
  f32 GetPokemonYAxisRotationOriginalDegree( void )      { return this->_GetPokemonYAxisRotationOriginalDegree(); }
  f32 GetPokemonYAxisRotationDegree( void )              { return this->_GetPokemonYAxisRotationDegree(); }
  gfl2::math::AABB GetPokemonAABB( void )                { return this->_GetPokemonAABB(); }
  f32 GetPokemonMotionPlayableRate( void )               { return this->_GetPokemonMotionPlayableRate(); }

  f32 GetCameraYAxisRotationDegree( void )  { return this->_GetCameraYAxisRotationDegree(); }
  f32 GetCameraYAxisTranslationRate( void ) { return this->_GetCameraYAxisTranslationRate(); }
  f32 GetCameraZoomRate( void )             { return this->_GetCameraZoomRate(); }
  f32 GetCameraFocusRate( void )            { return this->_GetCameraFocusRate(); }

  void GetPhotoParam( Viewer::PhotoParam* dstParam ) { this->_GetPhotoParam( dstParam ); }

  //-------------------------------------------------------
  // @brief   チェック
  //-------------------------------------------------------
  bool IsAvailableComposition( Viewer::Composition composition ) { return this->_IsAvailableComposition( composition ); }

  bool IsPlayingTrainerMotion( void ) { return this->_IsPlayingTrainerMotion(); }

  bool IsFinishChangePokemonMotionType( void )                     { return this->_IsFinishChangePokemonMotionType(); }
  bool IsNeedResetPokemonMotionType( Viewer::PokeMotionType type ) { return this->_IsNeedResetPokemonMotionType( type ); }

  bool IsCollisionTrainerAndPokemon( void ) { return this->_IsCollisionTrainerAndPokemon(); }

  bool IsFinishCapture( void ) { return this->_IsFinishCapture(); }

  bool IsRequestUpdateViewer( void );

private:
  //-------------------------------------------------------
  // @brief   更新 開始・終了：非公開
  //-------------------------------------------------------
  void _StartUpdateViewer( void );
  bool _IsStartUpdateViewer( void );
  bool _IsFinishUpdateViewer( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateRequestViewer( void );

  //-------------------------------------------------------
  // @brief   チェック：非公開
  //-------------------------------------------------------
  bool _IsCompareRequestParam( const RequestViewerParam param1, const RequestViewerParam param2 );
  bool _IsCompareSimpleParam( const PokeTool::SimpleParam& param1, const PokeTool::SimpleParam& param2 );
  bool _IsCompareDressupParam( const poke_3d::model::DressUpParam param1, const poke_3d::model::DressUpParam param2 );

private:
  const RequestViewerParam DEFAULT_REQUEST_VIEWER_PARAM;

private:
  gfl2::heap::HeapBase*   m_pHeap;

  RequestViewerParam      m_ReqParam;
  RequestViewerParam      m_UpdateParam;

  u8    m_MainSeq;

  u8    m_ReqSeq;

  bool  m_bUpdateViewer;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_VIEWER_MANAGER_H_INCLUDED__
