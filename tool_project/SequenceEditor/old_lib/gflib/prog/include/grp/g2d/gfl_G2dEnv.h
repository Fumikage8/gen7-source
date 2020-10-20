#ifndef __GFL_G2DENV_H__
#define __GFL_G2DENV_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G2dEnv.h
 *	@brief  2Dレンダリング環境情報
 *	@author	tomoya takahashi
 *	@date		2010.10.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>
#include <nn/gx.h>
#include <nn/ulcd.h>
#include <nw/gfx.h>
#include <GLES2/gl2.h>

#include <grp/gfl_Camera.h>
#include <grp/gfl_FragmentOperation.h>

#include <gfl_Macros.h>



namespace gfl {
namespace grp {
namespace g2d {

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
  
//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *    ViewPort設定
 */
//-----------------------------------------------------------------------------
typedef struct{
  u16 x;
  u16 y;
  u16 width;
  u16 height;
} EnvViewPort;

struct Parallax{
  f32 parallax_half;        ///< 視差値の半分の値
  f32 dLevel;               ///< 基準面までの距離
  f32 addDist;              ///< カメラとの距離に追加する値 
} ;
  
//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	2D描画環境管理クラス
//=====================================
class Env
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(Env);

public:
  enum CameraType
  {
    CAMERA_NORMAL,    // 通常or左目用
    CAMERA_RIGHT,     // 右目用

    CAMERA_NUM,
    ENUM_FORCE_DWORD(CameraType)
  };

  // 2D環境デフォルト　パラメータ
  static const ProjectionParam  DefaultProjection[DISPLAY_NORMAL_MAX];
  static const ViewParam        DefaultView[DISPLAY_NORMAL_MAX];
  
public:
  //-----------------------------------------------------------------------------
  /*
   * コンストラクタ・デストラクタ
   *
   * ＊デフォルトの設定を内部で行います。
   *
   */
  //-----------------------------------------------------------------------------
  Env(void);
  ~Env();

  //----------------------------------------------------------------------------
  /**
   *	@brief  カメラパラメータを設定
   *
   *  @param  type    カメラタイプ
   *	@param	proj    射影設定
   *	@param	view    View設定
   */
  //-----------------------------------------------------------------------------
  void SetCameraParam( Env::CameraType type, const ProjectionParam& proj, const ViewParam& view );

  //----------------------------------------------------------------------------
  /**
   *	@brief  フラグメントオペレーションパラメータを設定
   *
   *	@param	param   パラメータ
   */
  //-----------------------------------------------------------------------------
  void SetFragmentOperation( const FragmentOperationParam& param );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ViewPortの設定
   *
   *  @param  use       true　ビューポート設定を使用する。 false  フレームバッファのサイズ＝ViewPortと扱う。
   *	@param	ViewPort  設定情報
   */
  //-----------------------------------------------------------------------------
  void SetViewPort( bool use, const EnvViewPort* viewport = NULL );


  //----------------------------------------------------------------------------
  /**
   *	@brief  正射影視差の設定
   *
   *	@param  parallax    視差の調整の値（StereoCameraと同じ値）
   *	@param  dLevel      基準面への距離（StereoCameraと同じ値）
   *	@param  addDist     カメラとの距離に追加する値 
   *
   *	@note 正射影のまま、視差をつけたい場合には、こちらを使用してください。
   *	　    
   */
  //-----------------------------------------------------------------------------
  void SetParallax( f32 parallax, f32 dLevel, f32 addDist );

  //----------------------------------------------------------------------------
  /**
   *	@brief  視差情報の取得
   */
  //-----------------------------------------------------------------------------
  const Parallax& GetParallax( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  ViewPortの取得
   *
   *	@retval	ViewPort情報
   */
  //-----------------------------------------------------------------------------
  const EnvViewPort& GetViewPort( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  3dCommandをカレントのコマンドリストに設定
   *
   *	@param  レンダーターゲット
   */
  //-----------------------------------------------------------------------------
  void Add3DCommand( const RenderTarget& renderTarget ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  カメラ情報を取得
   */
  //-----------------------------------------------------------------------------
  const gfl::grp::Camera& GetCamera( Env::CameraType type ) const ;
  gfl::grp::Camera* GetCameraPtr( Env::CameraType type );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  フラグメントオペレーション情報を取得
   */
  //-----------------------------------------------------------------------------
  const gfl::grp::FragmentOperation& GetFragmentOperation( void ) const ;
  gfl::grp::FragmentOperation* GetFragmentOperationPtr( void );
  
  
private:
  gfl::grp::Camera                  m_camera[CAMERA_NUM];
  gfl::grp::FragmentOperation       m_fragmentOperation;
  EnvViewPort                       m_viewPort;
  Parallax                          m_parallax;
  bool                              m_viewPortUse;

  static bool                        m_sStereoCameraReady;
  static nn::ulcd::CTR::StereoCamera m_sStereoCamera;

};

  
} // g2d
} // grp
} // gfl
#endif // __GFL_G2DENV_H__
