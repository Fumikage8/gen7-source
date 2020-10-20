//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Camera.cpp
 *	@brief  シンプルなカメラクラス
 *	@author	tomoya takahashi
 *	@date		2010.10.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>

#include <grp/gfl_Camera.h>

namespace gfl {
namespace grp {




//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
Camera::Camera(void)
{
  m_viewParam.pos       = gfl::math::VEC3( 0,0,1 );
  m_viewParam.up        = gfl::math::VEC3( 0,1,0 );
  m_viewParam.target    = gfl::math::VEC3( 0,0,0 );

  m_projParam.proj_type         = PROJECTION_PERSPECTIVE;
  m_projParam.perspective.fovy  = CAMERA_DEFAULT_FOVY;
  m_projParam.perspective.aspect= CAMERA_DEFAULT_ASPECT;
  m_projParam.near              = CAMERA_DEFAULT_NEAR;
  m_projParam.far               = CAMERA_DEFAULT_FAR;
  m_projParam.pivot             = CAMERA_DEFAULT_PIVOT; 


  m_const_proj = false;
  m_const_view = false;

  UpdateLookAt();
  UpdateProjection();
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
Camera::~Camera()
{
}


//----------------------------------------------------------------------------
/**
 *	@brief  射影行列の初期化　Frustum方式
 *
 *	@param	l         // ニアクリッピング面の視錐台左辺のX座標
 *	@param  r         // ニアクリッピング面の視錐台右辺のX座標
 *	@param  b         // ニアクリッピング面の視錐台下辺のY座標
 *	@param  t         // ニアクリッピング面の視錐台上辺のY座標
 *	@param  near      // ニア面の距離
 *	@param  far       // ファー面の距離
 *	@param  pivot     // 回転処理設定
 */
//-----------------------------------------------------------------------------
void Camera::SetUpFrustum( f32 l, f32 r, f32 b, f32 t, f32 near, f32 far, gfl::math::PivotDirection pivot )
{
  m_projParam.proj_type = PROJECTION_FRUSTUM;
  m_projParam.frustum.l = l;
  m_projParam.frustum.r = r;
  m_projParam.frustum.b = b;
  m_projParam.frustum.t = t;
  m_projParam.near      = near;
  m_projParam.far       = far;
  m_projParam.pivot     = pivot;

  m_const_proj = false;

  UpdateProjection();
}

//----------------------------------------------------------------------------
/**
 *	@brief  射影行列の初期化　Perspective方式
 *
 *	@param	fovy      // 縦方向の視野角(Radian)
 *	@param  aspect    // 視野のアスペクト比（幅/高さ）
 *	@param  near      // ニア面の距離
 *	@param  far       // ファー面の距離
 *	@param  pivot     // 回転処理設定
 */
//-----------------------------------------------------------------------------
void Camera::SetUpPerspective( f32 fovy, f32 aspect, f32 near, f32 far, gfl::math::PivotDirection pivot )
{
  m_projParam.proj_type = PROJECTION_PERSPECTIVE;
  m_projParam.perspective.fovy    = fovy;
  m_projParam.perspective.aspect  = aspect;
  m_projParam.near      = near;
  m_projParam.far       = far;
  m_projParam.pivot     = pivot;

  m_const_proj = false;

  UpdateProjection();
}

//----------------------------------------------------------------------------
/**
 *	@brief  射影行列の初期化　Ortho方式
 *
 *	@param	l         // ニアクリッピング面の視錐台左辺のX座標
 *	@param  r         // ニアクリッピング面の視錐台右辺のX座標
 *	@param  b         // ニアクリッピング面の視錐台下辺のY座標
 *	@param  t         // ニアクリッピング面の視錐台上辺のY座標
 *	@param  near      // ニア面の距離
 *	@param  far       // ファー面の距離
 *	@param  pivot     // 回転処理設定
 */
//-----------------------------------------------------------------------------
void Camera::SetUpOrtho( f32 l, f32 r, f32 b, f32 t, f32 near, f32 far, gfl::math::PivotDirection pivot )
{
  m_projParam.proj_type = PROJECTION_FRUSTUM;
  m_projParam.ortho.l   = l;
  m_projParam.ortho.r   = r;
  m_projParam.ortho.b   = b;
  m_projParam.ortho.t   = t;
  m_projParam.near      = near;
  m_projParam.far       = far;
  m_projParam.pivot     = pivot;

  m_const_proj = false;

  UpdateProjection();
}

//----------------------------------------------------------------------------
/**
 *	@brief  行列直接渡しによる　射影行列の初期化
 *
 *	@param	projection  射影行列
 *	行列直接渡しの場合、const_dataモードとなり、値の変更が反映されません。
 */
//-----------------------------------------------------------------------------
void Camera::SetUpProjection( const gfl::math::MTX44& projection )
{
  m_proj = projection;
  m_const_proj = true;
}


//----------------------------------------------------------------------------
/**
 *	@brief  カメラ行列の初期化　
 *
 *	@param	pos         // カメラ座標
 *	@param	up          // カメラ上方向
 *	@param	target      // ターゲット座標
 */
//-----------------------------------------------------------------------------
void Camera::SetUpLookAt( const gfl::math::VEC3& pos, const gfl::math::VEC3& up, const gfl::math::VEC3& target )
{
  m_viewParam.pos     = pos;
  m_viewParam.up      = up;
  m_viewParam.target  = target;

  m_const_view = false;

  UpdateLookAt();
}

//----------------------------------------------------------------------------
/**
 *	@brief  行列直接渡しによる　カメラ行列の初期化
 *
 *	@param	view  ビュー行列
 *	行列直接渡しの場合、const_dataモードとなり、値の変更が反映されません。
 */
//-----------------------------------------------------------------------------
void Camera::SetUpLookAt( const gfl::math::MTX34& view )
{
  m_view = view;
  m_const_view = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  Projectionパラメータを使用したProjection行列初期化
 *
 *	@param	param Projectionパラメータ
 */
//-----------------------------------------------------------------------------
void Camera::SetUpProjectionParam( const ProjectionParam& param )
{
  m_projParam = param;
  m_const_proj = false;
  UpdateProjection();
}

//----------------------------------------------------------------------------
/**
 *	@brief  Viewパラメータを使用したView行列初期化
 *
 *	@param	param Viewパラメータ
 */
//-----------------------------------------------------------------------------
void Camera::SetUpViewParam( const ViewParam& param )
{
  m_viewParam = param;
  m_const_view = false;
  UpdateLookAt();
}


//----------------------------------------------------------------------------
/**
 *	@brief  ViewParamを取得
 *
 *	@return ViewParam
 */
//-----------------------------------------------------------------------------
const ViewParam& Camera::GetViewParam( void ) const
{
  return m_viewParam;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ProjectionParamを取得
 *
 *	@return ProjectionParam
 */
//-----------------------------------------------------------------------------
const ProjectionParam& Camera::GetProjectionParam( void ) const
{
  return m_projParam;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ViewMtxを取得
 *
 *	@return ViewMtx
 */
//-----------------------------------------------------------------------------
const gfl::math::MTX34& Camera::GetViewMtx( void ) const
{
  return m_view;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ProjectionMtxを取得
 *
 *	@return ProjectionMtx
 */
//-----------------------------------------------------------------------------
const gfl::math::MTX44& Camera::GetProjectionMtx( void ) const
{
  return m_proj;
}

//----------------------------------------------------------------------------
/**
 *	@brief  パラメータの変更をView行列に反映する。
 */
//-----------------------------------------------------------------------------
void Camera::UpdateLookAt(void)
{
  //constデータモードの場合は再計算を行わない
  if( m_const_view ){
    return ;
  }
  gfl::math::MTX34LookAt( &m_view, 
      &m_viewParam.pos, &m_viewParam.up, &m_viewParam.target );
}

//----------------------------------------------------------------------------
/**
 *	@brief  パラメータの変更をProjection行列に反映する。
 */
//-----------------------------------------------------------------------------
void Camera::UpdateProjection(void)
{
  //constデータモードの場合は再計算を行わない
  if( m_const_proj ){
    return ;
  }

  if( m_projParam.proj_type == PROJECTION_FRUSTUM ){
    gfl::math::MTX44FrustumPivot( &m_proj, 
        m_projParam.frustum.l,
        m_projParam.frustum.r,
        m_projParam.frustum.b,
        m_projParam.frustum.t,
        m_projParam.near,
        m_projParam.far,
        m_projParam.pivot );
  }
  else if( m_projParam.proj_type == PROJECTION_PERSPECTIVE ){
    gfl::math::MTX44PerspectivePivotRad( &m_proj, 
        m_projParam.perspective.fovy,
        m_projParam.perspective.aspect,
        m_projParam.near,
        m_projParam.far,
        m_projParam.pivot );
  }
  else{
    gfl::math::MTX44OrthoPivot( &m_proj, 
        m_projParam.ortho.l,
        m_projParam.ortho.r,
        m_projParam.ortho.b,
        m_projParam.ortho.t,
        m_projParam.near,
        m_projParam.far,
        m_projParam.pivot );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ファークリップ面の距離を設定
 *
 *	@param	far
 */
//-----------------------------------------------------------------------------
void Camera::SetFar( f32 far )
{
  m_projParam.far = far;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ファークリップ面の距離を取得
 *
 *	@return　ファークリップ面の距離
 */
//-----------------------------------------------------------------------------
f32 Camera::GetFar( void ) const
{
  return m_projParam.far;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ニアクリップ面の距離を設定
 *
 *	@param	near
 */
//-----------------------------------------------------------------------------
void Camera::SetNear( f32 near )
{
  m_projParam.near = near;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ニアクリップ面の距離を取得
 *
 *	@return　ニアクリップ面の距離
 */
//-----------------------------------------------------------------------------
f32 Camera::GetNear( void ) const
{
  return m_projParam.near;
}

//----------------------------------------------------------------------------
/**
 *	@brief  Frustum形式の情報を設定
 *
 *	@param	l         // ニアクリッピング面の視錐台左辺のX座標
 *	@param  r         // ニアクリッピング面の視錐台右辺のX座標
 *	@param  b         // ニアクリッピング面の視錐台下辺のY座標
 *	@param  t         // ニアクリッピング面の視錐台上辺のY座標
 */
//-----------------------------------------------------------------------------
void Camera::SetFrustum( f32 l, f32 r, f32 b, f32 t )
{
  m_projParam.proj_type = PROJECTION_FRUSTUM;
  m_projParam.frustum.l = l;
  m_projParam.frustum.r = r;
  m_projParam.frustum.b = b;
  m_projParam.frustum.t = t;
}

//----------------------------------------------------------------------------
/**
 *	@brief  射影行列の初期化　Perspective方式
 *
 *	@param	fovy      // 縦方向の視野角(Radian)
 *	@param  aspect    // 視野のアスペクト比（幅/高さ）
 */
//-----------------------------------------------------------------------------
void Camera::SetPerspective( f32 fovy, f32 aspect )
{
  m_projParam.proj_type         = PROJECTION_PERSPECTIVE;
  m_projParam.perspective.fovy  = fovy;
  m_projParam.perspective.aspect= aspect;
}

//----------------------------------------------------------------------------
/**
 *	@brief  射影行列の初期化　Ortho方式
 *
 *	@param	l         // ニアクリッピング面の視錐台左辺のX座標
 *	@param  r         // ニアクリッピング面の視錐台右辺のX座標
 *	@param  b         // ニアクリッピング面の視錐台下辺のY座標
 *	@param  t         // ニアクリッピング面の視錐台上辺のY座標
 */
//-----------------------------------------------------------------------------
void Camera::SetOrtho( f32 l, f32 r, f32 b, f32 t )
{
  m_projParam.proj_type = PROJECTION_ORTHO;
  m_projParam.ortho.l = l;
  m_projParam.ortho.r = r;
  m_projParam.ortho.b = b;
  m_projParam.ortho.t = t;
}


//----------------------------------------------------------------------------
/**
 *	@brief  カメラ座標を設定
 *
 *	@param	pos   カメラ座標
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraPos( const gfl::math::VEC3& pos )
{
  m_viewParam.pos = pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief  カメラ座標を取得
 *
 *	@retval	カメラ座標
 */
//-----------------------------------------------------------------------------
const gfl::math::VEC3& Camera::GetCameraPos( void ) const
{
  return m_viewParam.pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief  カメラ上方向を設定
 *
 *  @param  up  カメラ上方向
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraUp( const gfl::math::VEC3& up )
{
  m_viewParam.up = up;
}

//----------------------------------------------------------------------------
/**
 *	@brief  カメラ上方向を取得
 *
 *  @retval  カメラ上方向
 */
//-----------------------------------------------------------------------------
const gfl::math::VEC3& Camera::GetCameraUp( void ) const
{
  return m_viewParam.up;
}

//----------------------------------------------------------------------------
/**
 *	@brief  カメラターゲットを設定
 *
 *  @param  target  カメラターゲット
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraTarget( const gfl::math::VEC3& target )
{
  m_viewParam.target = target;
}

//----------------------------------------------------------------------------
/**
 *	@brief  カメラターゲットを取得
 *
 *  @retval  カメラターゲット
 */
//-----------------------------------------------------------------------------
const gfl::math::VEC3& Camera::GetCameraTarget( void ) const
{
  return m_viewParam.target;
}

//----------------------------------------------------------------------------
/**
 *	@brief  デバッグ用パラメータ表示
 *
 *  @retval  カメラターゲット
 */
//-----------------------------------------------------------------------------
void Camera::Dump( void ) const
{
  GFL_PRINT("Camera::Dump\n");
  GFL_PRINT("ProjType[%d]\n",m_projParam.proj_type);
  GFL_PRINT("param[%.2f][%.2f][%.2f][%.2f]\n",m_projParam.frustum.l,m_projParam.frustum.r,m_projParam.frustum.b,m_projParam.frustum.t);
  GFL_PRINT("near[%.2f] far[%.2f]\n",m_projParam.near,m_projParam.far);
  GFL_PRINT("-------------------\n");
  GFL_PRINT("pos[%.2f][%.2f][%.2f]\n",m_viewParam.pos.x,m_viewParam.pos.y,m_viewParam.pos.z);
  GFL_PRINT("up [%.2f][%.2f][%.2f]\n",m_viewParam.up.x,m_viewParam.up.y,m_viewParam.up.z);
  GFL_PRINT("trg[%.2f][%.2f][%.2f]\n",m_viewParam.target.x,m_viewParam.target.y,m_viewParam.target.z);
  GFL_PRINT("-------------------\n");
  GFL_PRINT("m_view\n");
  m_view.Report();
  GFL_PRINT("-------------------\n");
  GFL_PRINT("m_proj\n");
  m_proj.Report();
}




} /* namespace end grp */
} /* namespace end gfl */
