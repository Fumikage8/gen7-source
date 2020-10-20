inline Matrix::Matrix()
{
	col[0].Set(0, 0, 0, 0);
	col[1].Set(0, 0, 0, 0);
	col[2].Set(0, 0, 0, 0);
	col[3].Set(0, 0, 0, 0);
}

inline Matrix::Matrix( const Matrix&  rhs )
{
	*this = rhs;
}

#if defined( GFL2_MAYA )
inline Matrix::Matrix( const MMatrix&  rhs )
{
  for( int row = 0; row < 4; ++row )
  {
    for( int col = 0; col < 4; ++col )
    {
      this->col[col].f[row] = static_cast<float>( rhs(col, row) );
    }
  }
}
#endif

inline Matrix::Matrix( const Vector4& col0_, const Vector4& col1_, const Vector4& col2_, const Vector4& col3_ )
{
	this->col[0] = col0_;
    this->col[1] = col1_;
    this->col[2] = col2_;
    this->col[3] = col3_;
}

inline Matrix& Matrix::operator =( const Matrix&  rhs )
{
	col[0] = rhs.col[0];
  col[1] = rhs.col[1];
  col[2] = rhs.col[2];
  col[3] = rhs.col[3];
  return *this;	
}

#if defined( GFL2_MAYA )
inline Matrix& Matrix::operator =( const MMatrix&  rhs )
{
  for( int row = 0; row < 4; ++row )
  {
    for( int col = 0; col < 4; ++col )
    {
      this->col[col].f[row] = static_cast<float>( rhs(col, row) );
    }
  }

  return *this;	
}
#endif

//! 配列アクセス
inline Vector4&  Matrix::operator []( int col )
{
	return (this->col[col]);
}

//! 配列アクセス
inline const Vector4& Matrix::operator []( int col ) const
{
	return (this->col[col]);
}

//! 要素の設定
inline Matrix& Matrix::SetElem( int col, int row, float val )
{
	this->col[col][row] = val;
	return *this;
}

//! 要素の取得
inline float Matrix::GetElem( int col, int row ) const
{
	return this->col[col][row];
}

//! 位置要素の取得
inline Vector4 Matrix::GetElemPosition() const
{
  return this->col[3];
}

//! 回転要素の取得ZYX（ラジアン）
inline Vector4 Matrix::GetElemRotationZYX() const
{
  f32 ysin = -this->col[0][2];
  if (ysin < -1.0f) ysin = -1.0f;
  if (ysin > 1.0f) ysin = 1.0f;

  Vector4 result(
    atan2f(this->col[1][2], this->col[2][2]), // xsin*ycos, xcos*ycos
    asinf(ysin),                  // -ysin
    atan2f(this->col[0][1], this->col[0][0])  // ycos*zsin, ycos*zcos
  );

  // Y軸に90度以上回転した場合に見にくいので180度反転させておく
  if ((result.x < 0.0f) && (result.z  < 0.0f))
  {
    result.x = result.x + PI;
    result.y = -result.y + PI;
    result.z = result.z + PI;
  }

  return result;
}

//! 拡大縮小要素の取得
inline Vector4 Matrix::GetElemScaling() const
{
  return Vector4(this->col[0].Length(), this->col[1].Length(), this->col[2].Length());
}

//! 加算
inline Matrix Matrix::operator +( const Matrix&  rhs ) const
{
	Matrix  m;
    m.col[0] = col[0] + rhs.col[0];
    m.col[1] = col[1] + rhs.col[1];
    m.col[2] = col[2] + rhs.col[2];
    m.col[3] = col[3] + rhs.col[3];
    return m;
}

//! 減算
inline Matrix Matrix::operator -( const Matrix&  rhs ) const
{
	Matrix  m;
    m.col[0] = col[0] - rhs.col[0];
    m.col[1] = col[1] - rhs.col[1];
    m.col[2] = col[2] - rhs.col[2];
    m.col[3] = col[3] - rhs.col[3];
    return m;
}

//! negate
inline Matrix Matrix::operator -( ) const
{
	Matrix  m;
    m.col[0] = -col[0];
    m.col[1] = -col[1];
    m.col[2] = -col[2];
    m.col[3] = -col[3];
    return m;
}

inline Matrix Matrix::operator *( float rhs ) const
{
	return  Matrix (
        col[0] * rhs,
        col[1] * rhs,
        col[2] * rhs,
        col[3] * rhs
    );
}

inline Matrix& Matrix::operator *=( float rhs )
{
	col[0] *= rhs;
    col[1] *= rhs;
    col[2] *= rhs;
    col[3] *= rhs;
    return (*this);
}

//! ベクトルとの乗算
inline Vector4 Matrix::operator *( const Vector4& rhs ) const
{
	return col[0] * rhs.x + col[1] * rhs.y + col[2] * rhs.z + col[3] * rhs.w;
}

//! 乗算
inline Matrix Matrix::operator *( const Matrix&  rhs ) const
{
	return Matrix( *this * rhs.col[0], *this * rhs.col[1], *this * rhs.col[2], *this * rhs.col[3] );
}

//! 加算代入
inline Matrix&  Matrix::operator +=( const Matrix&  rhs )
{
	col[0] += rhs.col[0];
    col[1] += rhs.col[1];
    col[2] += rhs.col[2];
    col[3] += rhs.col[3];
    return (*this);
}

//! 減算代入
inline Matrix&  Matrix::operator -=( const Matrix&  rhs )
{
	col[0] -= rhs.col[0];
    col[1] -= rhs.col[1];
    col[2] -= rhs.col[2];
    col[3] -= rhs.col[3];
    return (*this);
}

//! 乗算代入
inline Matrix&  Matrix::operator *=( const Matrix&  rhs )
{
	*this = *this * rhs;
    return *this;
}

//! マトリクスの転置
inline Matrix Matrix::Transpose( void ) const
{
	return Matrix(
        Vector4( col[0].x, col[1].x, col[2].x, col[3].x ),
        Vector4( col[0].y, col[1].y, col[2].y, col[3].y ),
        Vector4( col[0].z, col[1].z, col[2].z, col[3].z ),
        Vector4( col[0].w, col[1].w, col[2].w, col[3].w )
    );
}

//! 逆マトリクス
inline Matrix Matrix::Inverse( void ) const
{
	Matrix ans;
    float   m00 = col[0].x;
    float   m01 = col[0].y;
    float   m02 = col[0].z;
    float   m03 = col[0].w;
    float   m10 = col[1].x;
    float   m11 = col[1].y;
    float   m12 = col[1].z;
    float   m13 = col[1].w;
    float   m20 = col[2].x;
    float   m21 = col[2].y;
    float   m22 = col[2].z;
    float   m23 = col[2].w;
    float   m30 = col[3].x;
    float   m31 = col[3].y;
    float   m32 = col[3].z;
    float   m33 = col[3].w;
    // 行列式の計算
    ans.SetElem(0,0,
            + m11 * (m22 * m33 - m32 * m23)
            - m12 * (m21 * m33 - m31 * m23)
            + m13 * (m21 * m32 - m31 * m22));
    ans.SetElem(1,0,
            - m10 * (m22 * m33 - m32 * m23)
            + m12 * (m20 * m33 - m30 * m23)
            - m13 * (m20 * m32 - m30 * m22));
    ans.SetElem(2,0,
            + m10 * (m21 * m33 - m31 * m23)
            - m11 * (m20 * m33 - m30 * m23)
            + m13 * (m20 * m31 - m30 * m21));
    ans.SetElem(3,0,
            - m10 * (m21 * m32 - m31 * m22)
            + m11 * (m20 * m32 - m30 * m22)
            - m12 * (m20 * m31 - m30 * m21));
    float det =
            m00 * ans[0][0] +
            m01 * ans[1][0] +
            m02 * ans[2][0] +
            m03 * ans[3][0];
    //assert(determ > math::epsilon);
    
    // 各要素の計算
    ans.SetElem( 0,1,
            - m01 * (m22 * m33 - m32 * m23)
            + m02 * (m21 * m33 - m31 * m23)
            - m03 * (m21 * m32 - m31 * m22));
    ans.SetElem( 0,2,
            + m01 * (m12 * m33 - m32 * m13)
            - m02 * (m11 * m33 - m31 * m13)
            + m03 * (m11 * m32 - m31 * m12));
    ans.SetElem( 0,3,
            - m01 * (m12 * m23 - m22 * m13)
            + m02 * (m11 * m23 - m21 * m13)
            - m03 * (m11 * m22 - m21 * m12));
    ans.SetElem( 1,1,
            + m00 * (m22 * m33 - m32 * m23)
            - m02 * (m20 * m33 - m30 * m23)
            + m03 * (m20 * m32 - m30 * m22));
    ans.SetElem( 1,2,
            - m00 * (m12 * m33 - m32 * m13)
            + m02 * (m10 * m33 - m30 * m13)
            - m03 * (m10 * m32 - m30 * m12));
    ans.SetElem( 1,3,
            + m00 * (m12 * m23 - m22 * m13)
            - m02 * (m10 * m23 - m20 * m13)
            + m03 * (m10 * m22 - m20 * m12));
    ans.SetElem( 2,1,
            - m00 * (m21 * m33 - m31 * m23)
            + m01 * (m20 * m33 - m30 * m23)
            - m03 * (m20 * m31 - m30 * m21));
    ans.SetElem( 2,2,
            + m00 * (m11 * m33 - m31 * m13)
            - m01 * (m10 * m33 - m30 * m13)
            + m03 * (m10 * m31 - m30 * m11));
    ans.SetElem( 2,3, 
            - m00 * (m11 * m23 - m21 * m13)
            + m01 * (m10 * m23 - m20 * m13)
            - m03 * (m10 * m21 - m20 * m11));
    ans.SetElem( 3,1,
            + m00 * (m21 * m32 - m31 * m22)
            - m01 * (m20 * m32 - m30 * m22)
            + m02 * (m20 * m31 - m30 * m21));
    ans.SetElem( 3,2,
            - m00 * (m11 * m32 - m31 * m12)
            + m01 * (m10 * m32 - m30 * m12)
            - m02 * (m10 * m31 - m30 * m11));
    ans.SetElem( 3,3,
            + m00 * (m11 * m22 - m21 * m12)
            - m01 * (m10 * m22 - m20 * m12)
            + m02 * (m10 * m21 - m20 * m11));

    // 行列式の逆数をかける
    ans *= 1.0f/det;
    return ans;
}

inline Matrix Matrix::AffineInverse( void ) const
{
	float   m00 = col[0].x;
    float   m01 = col[0].y;
    float   m02 = col[0].z;
    float   m10 = col[1].x;
    float   m11 = col[1].y;
    float   m12 = col[1].z;
    float   m20 = col[2].x;
    float   m21 = col[2].y;
    float   m22 = col[2].z;
    float   m30 = col[3].x;
    float   m31 = col[3].y;
    float   m32 = col[3].z;
    float det = 1.0f /
                ( m00 * (m11 * m22 - m12 * m21)
                - m01 * (m10 * m22 - m12 * m20)
                + m02 * (m10 * m21 - m11 * m20) );
    Vector4  col0( det * (m11 * m22 - m12 * m21), -det * (m01 * m22 - m02 * m21), det * (m01 * m12 - m02 * m11), 0.0f);
    Vector4  col1( -det * (m10 * m22 - m12 * m20), det * (m00 * m22 - m02 * m20), -det * (m00 * m12 - m02 * m10), 0.0f);
    Vector4  col2( det * (m10 * m21 - m11 * m20), -det * (m00 * m21 - m01 * m20), det * (m00 * m11 - m01 * m10), 0.0f);
    Vector4  col3( -(m30 * col0[0] + m31 * col1[0] + m32 * col2[0]), -(m30 * col0[1] + m31 * col1[1] + m32 * col2[1]), -(m30 * col0[2] + m31 * col1[2] + m32 * col2[2]), 1.0f );

    return Matrix( col0, col1, col2, col3 );
}

//! 単位マトリクス
inline Matrix Matrix::GetIdentity( )
{
	Matrix  m(	Vector4( 1.0f, 0.0f, 0.0f, 0.0f ),
				Vector4( 0.0f, 1.0f, 0.0f, 0.0f ),
				Vector4( 0.0f, 0.0f, 1.0f, 0.0f ),
				Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
    return m;
}

//! ゼロマトリクス
inline Matrix Matrix::GetZero()
{
	Matrix  m(	Vector4( 0.0f, 0.0f, 0.0f, 0.0f ),
				Vector4( 0.0f, 0.0f, 0.0f, 0.0f ),
				Vector4( 0.0f, 0.0f, 0.0f, 0.0f ),
				Vector4( 0.0f, 0.0f, 0.0f, 0.0f ) );
    return m;
}

//! X軸回転マトリクス
inline Matrix Matrix::GetRotationX( float radians )
{
	Vector4  v1( 0.0f, cosf( radians ), sinf( radians ), 0.0f );
    Vector4  v2( 0.0f, -sinf( radians ), cosf( radians ), 0.0f );
    Matrix  m( Vector4( 1.0f, 0.0f, 0.0f, 0.0f ), v1, v2, Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
    return m;
}

//! Y軸回転マトリクス
inline Matrix Matrix::GetRotationY( float radians )
{
	Vector4  v0( cosf( radians ), 0.0f, -sinf( radians ), 0.0f );
    Vector4  v2( sinf( radians ), 0.0f, cosf( radians ), 0.0f );
    Matrix  m( v0, Vector4( 0.0f, 1.0f, 0.0f, 0.0f ), v2, Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
    return m;
}

//! Z軸回転マトリクス
inline Matrix Matrix::GetRotationZ( float radians )
{
	Vector4  v0( cosf( radians ), sinf( radians ), 0.0f, 0.0f );
    Vector4  v1( -sinf( radians ), cosf( radians ), 0.0f, 0.0f );
    Matrix  m( v0, v1, Vector4( 0.0f, 0.0f, 1.0f, 0.0f ), Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
    return m;
}

//! Z * Y * X 回転マトリクス
inline Matrix Matrix::GetRotationZYX( float radiansZ, float radiansY, float radiansX )
{
	float xsin, xcos, ysin, ycos, zsin, zcos;
	xsin = sinf( radiansX );
	ysin = sinf( radiansY );
	zsin = sinf( radiansZ );
	xcos = cosf( radiansX );
	ycos = cosf( radiansY );
	zcos = cosf( radiansZ );

	return Matrix
		(
		Vector4( (ycos*zcos), (ycos*zsin), (-ysin), 0.0f ),
		Vector4( (xsin*ysin*zcos+xcos*-zsin), (xsin*ysin*zsin+xcos*zcos), (xsin*ycos), 0.0f ),
		Vector4( (xcos*ysin*zcos+-xsin*-zsin), (xcos*ysin*zsin+-xsin*zcos), (xcos*ycos), 0.0f ),
		Vector4( 0.0f, 0.0f, 0.0f ,1.0f )
		);
}

//! スケーリングマトリクス
inline Matrix Matrix::GetScaling( const Vector4& scale )
{
	Matrix  m(  Vector4( scale.x, 0.0f, 0.0f, 0.0f),
                Vector4( 0.0f, scale.y, 0.0f, 0.0f),
                Vector4( 0.0f, 0.0f, scale.z, 0.0f),
                Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
    return m;
}

//! 平行移動マトリクス
inline Matrix Matrix::GetTranslation( const Vector4& translation )
{
	Vector4			trans = translation;
	trans.w = 1.0f;

	Matrix  m(  Vector4( 1.0f, 0.0f, 0.0f, 0.0f ),
                Vector4( 0.0f, 1.0f, 0.0f, 0.0f ),
                Vector4( 0.0f, 0.0f, 1.0f, 0.0f ),
                trans );
    return m;
}

inline Matrix Matrix::GetLookAt( const Vector4& eye, const Vector4& aim, const Vector4& up )
{
	const Vector4 d = (aim - eye).Normalize3();
    const Vector4 s = d.Cross( up ).Normalize3();
    const Vector4 r = s.Cross( d );
    return Matrix( Vector4( s.x, r.x, -d.x, 0.0f ),
                   Vector4( s.y, r.y, -d.y, 0.0f ),
                   Vector4( s.z, r.z, -d.z, 0.0f ),
                   Vector4( -eye.Dot3( s ), -eye.Dot3( r ), eye.Dot3( d ), 1.0f ) );
	/*const Vector4 cz = -(aim - eye).Normalize3();
	const Vector4 cx = cz.Cross( up ).Normalize3();
	const Vector4 cy = cx.Cross( cz );

	return Matrix( Vector4( -cx.x, cy.x, -cz.x, 0.0f ),
                   Vector4( cx.y, cy.y, -cz.y, 0.0f ),
                   Vector4( -cx.z, cy.z, -cz.z, 0.0f ),
				   Vector4( eye.Dot3( cx ), eye.Dot3( cy ), eye.Dot3( cz ), 1.0f ) );*/
}

//! Perspective
inline Matrix Matrix::GetPerspective( float fovy, float aspect, float zNear, float zFar, bool isPivot )
{
#if defined(NN_PLATFORM_CTR)
  // CTRの射影行列は3列目が反転している
	Matrix proj = GetPerspectiveDX(fovy,aspect,zNear,zFar);
  proj[2][2] = -proj[2][2];
  proj[3][2] = -proj[3][2];

  // 画面向きに合わせてピボットさせる
  if (isPivot)
  {
    // 1列目と2列目を入れ替える
    proj[0][1] = -proj[0][0];  // ここは反転
    proj[0][0] = 0.0f;

    proj[1][0] = proj[1][1];
    proj[1][1] = 0.0f;
  }

  return proj;
#else
  (void)isPivot;

	return GetPerspectiveDX(fovy,aspect,zNear,zFar);
#endif
}

//! Frustum
inline Matrix Matrix::GetFrustum( float left, float right, float bottom, float top, float zNear, float zFar )
{
	return GetFrustumDX(left,right,bottom,top,zNear,zFar);
}

//! ortho
inline Matrix Matrix::GetOrthographic( float left, float right, float bottom, float top, float zNear, float zFar )
{
	return GetOrthographicDX(left,right,bottom,top,zNear,zFar);
}
//=============
// Perspective projection matrix:
// cot(fovy/2)/aspect        0            0                     0
//      0             cot(fovy/2)         0                     0
//      0                    0  (zFar+zNear)/(zNear-zFar) 2*zFar*zNear/(zNear-zFar)
//      0                    0           -1                     0
//! Perspective OpenGL互換(Depth=-1.0～+1.0に射影)
inline Matrix Matrix::GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar )
{
	float f, rangeInv;
    Vector4  zero, col0, col1, col2, col3;

    f = tanf( (PI * 0.5f) - fovy * 0.5f );
    rangeInv = 1.0f / ( zNear - zFar );

    zero.Set(0.0f, 0.0f, 0.0f, 0.0f);
    col0.Set( f / aspect, 0.0f, 0.0f, 0.0f );
    col1.Set( 0.0f, f, 0.0f, 0.0f );
    col2.Set( 0.0f, 0.0f, ( zNear + zFar ) * rangeInv, -1.0f );
    col3.Set( 0.0f, 0.0f, zNear * zFar * rangeInv * 2.0f, 0.0f );

    return Matrix( col0, col1, col2, col3 );
}

//! Perspective DirectX互換(Depth=0.0～+1.0に射影)
inline Matrix Matrix::GetPerspectiveDX( float fovy, float aspect, float zNear, float zFar )
{
	math::Matrix	conv = math::Matrix(
		math::Vector4::GetXUnit(),
		math::Vector4::GetYUnit(),
		math::Vector4( 0.0f, 0.0f, 0.5f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, 0.5f, 1.0f )
	);

	math::Matrix	z_minus = math::Matrix(
		math::Vector4( 1.0f, 0.0f, 0.0f, 0.0f ),
		math::Vector4( 0.0f, 1.0f, 0.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, -1.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, 0.0f, 1.0f )
	);

	//Ｚレンジを[-1...+1]→[0...1]に変換。
	return conv * GetPerspectiveGL(fovy,aspect,zNear,zFar);// * z_minus;
}

//! Frustum OpenGL互換(Depth=-1.0～+1.0に射影)
inline Matrix Matrix::GetFrustumGL( float left, float right, float bottom, float top, float zNear, float zFar )
{
	Vector4  col0, col1, col2, col3;

    float   n2 = 2.0f * zNear;
    float   w = right - left;
    float   h = top - bottom;
    float   d = zNear-zFar;

    col0.Set( n2 / w, 0.0f, 0.0f, 0.0f );
    col1.Set( 0.0f, n2 / h, 0.0f, 0.0f );
    col2.Set( (right+left)/w, (top+bottom)/h, (zFar+zNear)/d, -1.0f );
    col3.Set( 0.0f, 0.0f, 2.0f*zFar*zNear/d , 0.0f );

    return Matrix( col0, col1, col2, col3 );
}
//! Frustum DirectX互換(Depth=0.0～+1.0に射影)
inline Matrix Matrix::GetFrustumDX( float left, float right, float bottom, float top, float zNear, float zFar )
{
	math::Matrix	conv = math::Matrix(
		math::Vector4::GetXUnit(),
		math::Vector4::GetYUnit(),
		math::Vector4( 0.0f, 0.0f, 0.5f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, 0.5f, 1.0f )
	);

	math::Matrix	z_minus = math::Matrix(
		math::Vector4( 0.0f, 0.0f, 0.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, 0.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, -1.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, 0.0f, 0.0f )
	);

	//Ｚレンジを[-1...+1]→[0...1]に変換。
	return conv * GetFrustumGL(left,right,bottom,top,zNear,zFar) * z_minus;
}

//! ortho OpenGL互換(Depth=-1.0～+1.0に射影)
inline Matrix Matrix::GetOrthographicGL( float left, float right, float bottom, float top, float zNear, float zFar )
{
	Vector4  col0, col1, col2, col3;

    float   w = right - left;
    float   h = top - bottom;
    float   d = zFar-zNear;

    col0.Set( 2.0f / w, 0.0f, 0.0f, 0.0f );
    col1.Set( 0.0f, 2.0f / h, 0.0f, 0.0f );
    col2.Set( 0.0f, 0.0f, -2.0f / d, 0.0f );
    col3.Set( -(right+left)/w, -(top+bottom)/h, -(zFar+zNear)/d , 1.0f );
    return Matrix( col0, col1, col2, col3 );
}

//! ortho DirectX互換(Depth=0.0～+1.0に射影)
inline Matrix Matrix::GetOrthographicDX( float left, float right, float bottom, float top, float zNear, float zFar )
{
	math::Matrix	conv = math::Matrix(
		math::Vector4::GetXUnit(),
		math::Vector4::GetYUnit(),
		math::Vector4( 0.0f, 0.0f, 0.5f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, 0.5f, 1.0f )
	);

	math::Matrix	z_minus = math::Matrix(
		math::Vector4( 0.0f, 0.0f, 0.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, 0.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, -1.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, 0.0f, 0.0f )
	);

	//Ｚレンジを[-1...+1]→[0...1]に変換。
	return conv * GetOrthographicGL(left,right,bottom,top,zNear,zFar) * z_minus;
}


inline Matrix Matrix::GetBillboardMatrixRotationEye( const Vector4& cameraPosition, const Vector4& targetPosition )
{
	math::Vector4 direction = (cameraPosition - targetPosition).Normalize();
	math::Matrix rotationMatrix;

	rotationMatrix[0] = ( math::Vector4::GetYUnit().Cross( direction ).Normalize() );
	rotationMatrix[1] = ( direction );
	rotationMatrix[2] = ( direction.Cross( rotationMatrix[0] ) );
	rotationMatrix[3] = ( math::Vector4::GetWUnit() );

	return rotationMatrix;
}

inline Matrix Matrix::GetBillboardMatrixRotationEyeY( const Vector4& cameraPosition, const Vector4& targetPosition )
{
	math::Vector4 direction = math::Vector4(cameraPosition - targetPosition).SetY(0.0f).Normalize();
	math::Matrix rotationMatrix;
	rotationMatrix[0] = ( math::Vector4::GetYUnit().Cross( direction ).Normalize() );
	rotationMatrix[2] = ( direction );
	rotationMatrix[1] = ( math::Vector4::GetYUnit() );
	rotationMatrix[3] = ( math::Vector4::GetWUnit() );
	return rotationMatrix;
}

inline Matrix Matrix::GetBillboardMatrixRotationXYZ(const Matrix& viewMatrix )
{
	return math::Matrix(viewMatrix).SetTranslationVector( math::Vector4::GetWUnit() );
}

inline Matrix Matrix::GetBillboardMatrixRotationY(const Matrix& viewMatrix )
{
	math::Matrix rotationMatrix;
	rotationMatrix[1] = (math::Vector4::GetYUnit());
	rotationMatrix[0] = (rotationMatrix[1].Cross( viewMatrix[2] ).Normalize());
	rotationMatrix[2] = (rotationMatrix[0].Cross( rotationMatrix[1] ).Normalize());
	rotationMatrix[3] = ( math::Vector4::GetWUnit() );
	return rotationMatrix;
}
	
//  ｗ＝１と仮定したベクトルとマトリクスとの乗算
inline Vector4 Matrix::TransformCoord( const Vector4& rhs ) const
{
    return col[0] * rhs.GetX() + col[1] * rhs.GetY() + col[2] * rhs.GetZ() + col[3];
}

//  ｗ＝０と仮定したベクトルとマトリクスとの乗算
inline Vector4 Matrix::TransformNormal( const Vector4& rhs ) const
{
    return col[0] * rhs.GetX() + col[1] * rhs.GetY() + col[2] * rhs.GetZ();
}

// absolute values per element
inline Matrix Matrix::AbsPerElem( void ) const
{
    return Matrix(  col[0].AbsPerElem(), 
                    col[1].AbsPerElem(),
                    col[2].AbsPerElem(),
                    col[3].AbsPerElem() );
}