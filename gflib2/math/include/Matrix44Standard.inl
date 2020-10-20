inline Matrix44::Matrix44()
{
	row[0].Set(0, 0, 0, 0);
	row[1].Set(0, 0, 0, 0);
	row[2].Set(0, 0, 0, 0);
	row[3].Set(0, 0, 0, 0);
}

inline Matrix44::Matrix44( const Matrix44&  rhs )
{
	*this = rhs;
}

inline Matrix44::Matrix44( const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3 )
{
  this->row[0] = row0;
  this->row[1] = row1;
  this->row[2] = row2;
  this->row[3] = row3;
}

inline Matrix44::Matrix44( const Matrix34& m34 )
{
  row[0] = m34[0];
  row[1] = m34[1];
  row[2] = m34[2];
  row[3].Set(0, 0, 0, 1);
}

inline Matrix44& Matrix44::operator =( const Matrix44&  rhs )
{
	row[0] = rhs.row[0];
  row[1] = rhs.row[1];
  row[2] = rhs.row[2];
  row[3] = rhs.row[3];
  return *this;	
}

//! 配列アクセス
inline Vector4&  Matrix44::operator []( int row )
{
	return (this->row[row]);
}

//! 配列アクセス
inline const Vector4& Matrix44::operator []( int row ) const
{
	return (this->row[row]);
}

//! 要素の設定
inline Matrix44& Matrix44::SetElem( int row, int col, float val )
{
	this->row[row][col] = val;
	return *this;
}

//! 要素の取得
inline float Matrix44::GetElem( int row, int col ) const
{
	return this->row[row][col];
}

//! 位置要素の取得
inline Vector3 Matrix44::GetElemPosition() const
{
  return Vector3(this->row[0][3], this->row[1][3], this->row[2][3]);
}

//! 回転要素の取得ZYX（ラジアン）
inline Vector3 Matrix44::GetElemRotationZYX() const
{

  gfl2::math::Matrix44 tmp = *this;
  tmp.Normalize();

  f32 ysin = -tmp[2][0];

  // ジンバルロック対応
  static const f32 sc_errorValue = 0.00001f;
  if ((ysin <= -1.0f + sc_errorValue) || (ysin >= 1.0f - sc_errorValue))
  {
    if (ysin < 0.0f)
    {
      return gfl2::math::Vector3(
        0.0f,
        -PI * 0.5f,
        acos(tmp[1][1])
      );
    }
    else
    {
      return gfl2::math::Vector3(
        0.0f,
        PI * 0.5f,
        asin(tmp[1][2])
      );
    }
  }

  gfl2::math::Vector3 result(
    atan2f(tmp[2][1], tmp[2][2]), // xsin*ycos, xcos*ycos
    asinf(ysin),                  // -ysin
    atan2f(tmp[1][0], tmp[0][0])  // ycos*zsin, ycos*zcos
  );

  return result;
}

//! 拡大縮小要素の取得
inline Vector3 Matrix44::GetElemScaling() const
{
  math::Vector3 col0(this->row[0][0], this->row[1][0], this->row[2][0]);
  math::Vector3 col1(this->row[0][1], this->row[1][1], this->row[2][1]);
  math::Vector3 col2(this->row[0][2], this->row[1][2], this->row[2][2]);

  return Vector3(col0.Length(), col1.Length(), col2.Length());
}

//! 加算
inline Matrix44 Matrix44::operator +( const Matrix44&  rhs ) const
{
	Matrix44  m;
  m.row[0] = row[0] + rhs.row[0];
  m.row[1] = row[1] + rhs.row[1];
  m.row[2] = row[2] + rhs.row[2];
  m.row[3] = row[3] + rhs.row[3];
  return m;
}

//! 減算
inline Matrix44 Matrix44::operator -( const Matrix44&  rhs ) const
{
	Matrix44  m;
  m.row[0] = row[0] - rhs.row[0];
  m.row[1] = row[1] - rhs.row[1];
  m.row[2] = row[2] - rhs.row[2];
  m.row[3] = row[3] - rhs.row[3];
  return m;
}

//! negate
inline Matrix44 Matrix44::operator -( ) const
{
	Matrix44  m;
  m.row[0] = -row[0];
  m.row[1] = -row[1];
  m.row[2] = -row[2];
  m.row[3] = -row[3];
  return m;
}

inline Matrix44 Matrix44::operator *( float rhs ) const
{
	return  Matrix44 (
        row[0] * rhs,
        row[1] * rhs,
        row[2] * rhs,
        row[3] * rhs
    );
}

inline Matrix44& Matrix44::operator *=( float rhs )
{
  row[0] *= rhs;
  row[1] *= rhs;
  row[2] *= rhs;
  row[3] *= rhs;
  return (*this);
}

//! ベクトルとの乗算
inline Vector4 Matrix44::operator *( const Vector4& rhs ) const
{
  return Vector4(
    this->row[0][0] * rhs.x + this->row[0][1] * rhs.y + this->row[0][2] * rhs.z + this->row[0][3] * rhs.w,
    this->row[1][0] * rhs.x + this->row[1][1] * rhs.y + this->row[1][2] * rhs.z + this->row[1][3] * rhs.w,
    this->row[2][0] * rhs.x + this->row[2][1] * rhs.y + this->row[2][2] * rhs.z + this->row[2][3] * rhs.w,
    this->row[3][0] * rhs.x + this->row[3][1] * rhs.y + this->row[3][2] * rhs.z + this->row[3][3] * rhs.w
  );
}
//! ベクトルとの乗算
inline Vector3 Matrix44::operator *( const Vector3& rhs ) const
{
  return Vector3(
    this->row[0][0] * rhs.x + this->row[0][1] * rhs.y + this->row[0][2] * rhs.z + this->row[0][3],
    this->row[1][0] * rhs.x + this->row[1][1] * rhs.y + this->row[1][2] * rhs.z + this->row[1][3],
    this->row[2][0] * rhs.x + this->row[2][1] * rhs.y + this->row[2][2] * rhs.z + this->row[2][3]
  );
}

//! 乗算
inline Matrix44 Matrix44::operator *( const Matrix44&  rhs ) const
{
#if defined(GF_PLATFORM_CTR)
  // SDKの関数の方が速かった
  Matrix44 result;
  nn::math::MTX44Mult((nn::math::MTX44*)&result, (nn::math::MTX44*)this, (nn::math::MTX44*)&rhs);
  return result;
#else
	return Matrix44(
    Vector4(
      this->row[0][0] * rhs.row[0][0] + this->row[0][1] * rhs.row[1][0] + this->row[0][2] * rhs.row[2][0] + this->row[0][3] * rhs.row[3][0],
      this->row[0][0] * rhs.row[0][1] + this->row[0][1] * rhs.row[1][1] + this->row[0][2] * rhs.row[2][1] + this->row[0][3] * rhs.row[3][1],
      this->row[0][0] * rhs.row[0][2] + this->row[0][1] * rhs.row[1][2] + this->row[0][2] * rhs.row[2][2] + this->row[0][3] * rhs.row[3][2],
      this->row[0][0] * rhs.row[0][3] + this->row[0][1] * rhs.row[1][3] + this->row[0][2] * rhs.row[2][3] + this->row[0][3] * rhs.row[3][3]),

    Vector4(
      this->row[1][0] * rhs.row[0][0] + this->row[1][1] * rhs.row[1][0] + this->row[1][2] * rhs.row[2][0] + this->row[1][3] * rhs.row[3][0],
      this->row[1][0] * rhs.row[0][1] + this->row[1][1] * rhs.row[1][1] + this->row[1][2] * rhs.row[2][1] + this->row[1][3] * rhs.row[3][1],
      this->row[1][0] * rhs.row[0][2] + this->row[1][1] * rhs.row[1][2] + this->row[1][2] * rhs.row[2][2] + this->row[1][3] * rhs.row[3][2],
      this->row[1][0] * rhs.row[0][3] + this->row[1][1] * rhs.row[1][3] + this->row[1][2] * rhs.row[2][3] + this->row[1][3] * rhs.row[3][3]),

    Vector4(
      this->row[2][0] * rhs.row[0][0] + this->row[2][1] * rhs.row[1][0] + this->row[2][2] * rhs.row[2][0] + this->row[2][3] * rhs.row[3][0],
      this->row[2][0] * rhs.row[0][1] + this->row[2][1] * rhs.row[1][1] + this->row[2][2] * rhs.row[2][1] + this->row[2][3] * rhs.row[3][1],
      this->row[2][0] * rhs.row[0][2] + this->row[2][1] * rhs.row[1][2] + this->row[2][2] * rhs.row[2][2] + this->row[2][3] * rhs.row[3][2],
      this->row[2][0] * rhs.row[0][3] + this->row[2][1] * rhs.row[1][3] + this->row[2][2] * rhs.row[2][3] + this->row[2][3] * rhs.row[3][3]),

    Vector4(
      this->row[3][0] * rhs.row[0][0] + this->row[3][1] * rhs.row[1][0] + this->row[3][2] * rhs.row[2][0] + this->row[3][3] * rhs.row[3][0],
      this->row[3][0] * rhs.row[0][1] + this->row[3][1] * rhs.row[1][1] + this->row[3][2] * rhs.row[2][1] + this->row[3][3] * rhs.row[3][1],
      this->row[3][0] * rhs.row[0][2] + this->row[3][1] * rhs.row[1][2] + this->row[3][2] * rhs.row[2][2] + this->row[3][3] * rhs.row[3][2],
      this->row[3][0] * rhs.row[0][3] + this->row[3][1] * rhs.row[1][3] + this->row[3][2] * rhs.row[2][3] + this->row[3][3] * rhs.row[3][3])
  );
#endif
}

//! 加算代入
inline Matrix44&  Matrix44::operator +=( const Matrix44&  rhs )
{
  row[0] += rhs.row[0];
  row[1] += rhs.row[1];
  row[2] += rhs.row[2];
  row[3] += rhs.row[3];
  return (*this);
}

//! 減算代入
inline Matrix44&  Matrix44::operator -=( const Matrix44&  rhs )
{
  row[0] -= rhs.row[0];
  row[1] -= rhs.row[1];
  row[2] -= rhs.row[2];
  row[3] -= rhs.row[3];
  return (*this);
}

//! 乗算代入
inline Matrix44&  Matrix44::operator *=( const Matrix44&  rhs )
{
	*this = *this * rhs;
  return *this;
}

//! マトリクスの転置
inline Matrix44 Matrix44::Transpose( void ) const
{
  return Matrix44(
       Vector4( row[0].x, row[1].x, row[2].x, row[3].x ),
       Vector4( row[0].y, row[1].y, row[2].y, row[3].y ),
       Vector4( row[0].z, row[1].z, row[2].z, row[3].z ),
       Vector4( row[0].w, row[1].w, row[2].w, row[3].w )
    );
}

//! 逆マトリクス
inline Matrix44 Matrix44::Inverse( void ) const
{
	Matrix44 ans;

  f32 m00 = row[0].x;
  f32 m01 = row[1].x;
  f32 m02 = row[2].x;
  f32 m03 = row[3].x;

  f32 m10 = row[0].y;
  f32 m11 = row[1].y;
  f32 m12 = row[2].y;
  f32 m13 = row[3].y;

  f32 m20 = row[0].z;
  f32 m21 = row[1].z;
  f32 m22 = row[2].z;
  f32 m23 = row[3].z;

  f32 m30 = row[0].w;
  f32 m31 = row[1].w;
  f32 m32 = row[2].w;
  f32 m33 = row[3].w;

  // 行列式の計算
  ans.SetElem(0,0,
          + m11 * (m22 * m33 - m32 * m23)
          - m12 * (m21 * m33 - m31 * m23)
          + m13 * (m21 * m32 - m31 * m22));
  ans.SetElem(0,1,
          - m10 * (m22 * m33 - m32 * m23)
          + m12 * (m20 * m33 - m30 * m23)
          - m13 * (m20 * m32 - m30 * m22));
  ans.SetElem(0,2,
          + m10 * (m21 * m33 - m31 * m23)
          - m11 * (m20 * m33 - m30 * m23)
          + m13 * (m20 * m31 - m30 * m21));
  ans.SetElem(0,3,
          - m10 * (m21 * m32 - m31 * m22)
          + m11 * (m20 * m32 - m30 * m22)
          - m12 * (m20 * m31 - m30 * m21));
  f32 det =
          m00 * ans[0][0] +
          m01 * ans[0][1] +
          m02 * ans[0][2] +
          m03 * ans[0][3];
  //assert(determ > math::epsilon);

  // 各要素の計算
  ans.SetElem( 1,0,
          - m01 * (m22 * m33 - m32 * m23)
          + m02 * (m21 * m33 - m31 * m23)
          - m03 * (m21 * m32 - m31 * m22));
  ans.SetElem( 2,0,
          + m01 * (m12 * m33 - m32 * m13)
          - m02 * (m11 * m33 - m31 * m13)
          + m03 * (m11 * m32 - m31 * m12));
  ans.SetElem( 3,0,
          - m01 * (m12 * m23 - m22 * m13)
          + m02 * (m11 * m23 - m21 * m13)
          - m03 * (m11 * m22 - m21 * m12));
  ans.SetElem( 1,1,
          + m00 * (m22 * m33 - m32 * m23)
          - m02 * (m20 * m33 - m30 * m23)
          + m03 * (m20 * m32 - m30 * m22));
  ans.SetElem( 2,1,
          - m00 * (m12 * m33 - m32 * m13)
          + m02 * (m10 * m33 - m30 * m13)
          - m03 * (m10 * m32 - m30 * m12));
  ans.SetElem( 3,1,
          + m00 * (m12 * m23 - m22 * m13)
          - m02 * (m10 * m23 - m20 * m13)
          + m03 * (m10 * m22 - m20 * m12));
  ans.SetElem( 1,2,
          - m00 * (m21 * m33 - m31 * m23)
          + m01 * (m20 * m33 - m30 * m23)
          - m03 * (m20 * m31 - m30 * m21));
  ans.SetElem( 2,2,
          + m00 * (m11 * m33 - m31 * m13)
          - m01 * (m10 * m33 - m30 * m13)
          + m03 * (m10 * m31 - m30 * m11));
  ans.SetElem( 3,2, 
          - m00 * (m11 * m23 - m21 * m13)
          + m01 * (m10 * m23 - m20 * m13)
          - m03 * (m10 * m21 - m20 * m11));
  ans.SetElem( 1,3,
          + m00 * (m21 * m32 - m31 * m22)
          - m01 * (m20 * m32 - m30 * m22)
          + m02 * (m20 * m31 - m30 * m21));
  ans.SetElem( 2,3,
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

//! 逆マトリクス( アフィン変換(Matrix34)マトリクスとして処理  高速版 )
inline Matrix44 Matrix44::AffineInverse( void ) const
{
  f32 m00 = row[0].x;
  f32 m01 = row[1].x;
  f32 m02 = row[2].x;

  f32 m10 = row[0].y;
  f32 m11 = row[1].y;
  f32 m12 = row[2].y;

  f32 m20 = row[0].z;
  f32 m21 = row[1].z;
  f32 m22 = row[2].z;

  f32 m30 = row[0].w;
  f32 m31 = row[1].w;
  f32 m32 = row[2].w;

  f32 det = 1.0f /
              ( m00 * (m11 * m22 - m12 * m21)
              - m01 * (m10 * m22 - m12 * m20)
              + m02 * (m10 * m21 - m11 * m20) );

  Vector3 col0( det * (m11 * m22 - m12 * m21), -det * (m01 * m22 - m02 * m21), det * (m01 * m12 - m02 * m11));
  Vector3 col1( -det * (m10 * m22 - m12 * m20), det * (m00 * m22 - m02 * m20), -det * (m00 * m12 - m02 * m10));
  Vector3 col2( det * (m10 * m21 - m11 * m20), -det * (m00 * m21 - m01 * m20), det * (m00 * m11 - m01 * m10));
  Vector3 col3( -(m30 * col0[0] + m31 * col1[0] + m32 * col2[0]), -(m30 * col0[1] + m31 * col1[1] + m32 * col2[1]), -(m30 * col0[2] + m31 * col1[2] + m32 * col2[2]));

  return Matrix44(
    Vector4(col0.x, col1.x, col2.x, col3.x),
    Vector4(col0.y, col1.y, col2.y, col3.y),
    Vector4(col0.z, col1.z, col2.z, col3.z),
    Vector4(0.0f, 0.0f, 0.0f, 1.0f)
    );

}

//! 単位マトリクス
inline Matrix44 Matrix44::GetIdentity( )
{
  return Matrix44(
    Vector4(1.0f, 0.0f, 0.0f, 0.0f),
    Vector4(0.0f, 1.0f, 0.0f, 0.0f),
    Vector4(0.0f, 0.0f, 1.0f, 0.0f),
    Vector4(0.0f, 0.0f, 0.0f, 1.0f)
    );
}

//! ゼロマトリクス
inline Matrix44 Matrix44::GetZero()
{
  return Matrix44(
    Vector4(0.0f, 0.0f, 0.0f, 0.0f),
    Vector4(0.0f, 0.0f, 0.0f, 0.0f),
    Vector4(0.0f, 0.0f, 0.0f, 0.0f),
    Vector4(0.0f, 0.0f, 0.0f, 0.0f)
    );
}

//! X軸回転マトリクス
inline Matrix44 Matrix44::GetRotationX( float radians )
{
  f32 cos_ = cosf(radians);
  f32 sin_ = sinf(radians);

  return Matrix44(
    Vector4(1.0f, 0.0f, 0.0f, 0.0f),
    Vector4(0.0f, cos_, -sin_, 0.0f),
    Vector4(0.0f, sin_, cos_, 0.0f),
    Vector4(0.0f, 0.0f, 0.0f, 1.0f) );
}

//! Y軸回転マトリクス
inline Matrix44 Matrix44::GetRotationY( float radians )
{
  f32 cos_ = cosf(radians);
  f32 sin_ = sinf(radians);

  return Matrix44(
    Vector4(cos_, 0.0f, sin_, 0.0f),
    Vector4(0.0f, 1.0f, 0.0f, 0.0f),
    Vector4(-sin_, 0.0f, cos_, 0.0f),
    Vector4(0.0f, 0.0f, 0.0f, 1.0f) );
}

//! Z軸回転マトリクス
inline Matrix44 Matrix44::GetRotationZ( float radians )
{
  f32 cos_ = cosf(radians);
  f32 sin_ = sinf(radians);

  return Matrix44(
    Vector4(cos_, -sin_, 0.0f, 0.0f),
    Vector4(sin_, cos_, 0.0f, 0.0f),
    Vector4(0.0f, 0.0f, 1.0f, 0.0f),
    Vector4(0.0f, 0.0f, 0.0f, 1.0f) );
}

//! Z * Y * X 回転マトリクス
inline Matrix44 Matrix44::GetRotationZYX( float radiansZ, float radiansY, float radiansX )
{
	f32 xsin = sinf( radiansX );
	f32 ysin = sinf( radiansY );
	f32 zsin = sinf( radiansZ );
	f32 xcos = cosf( radiansX );
	f32 ycos = cosf( radiansY );
	f32 zcos = cosf( radiansZ );

  f32 ysin_zcos = ysin * zcos;
  f32 ysin_zsin = ysin * zsin;

  return Matrix44(
    Vector4((ycos*zcos), (xsin*ysin_zcos + xcos*-zsin), (xcos*ysin_zcos + -xsin*-zsin), 0.0f),
    Vector4((ycos*zsin), (xsin*ysin_zsin + xcos*zcos), (xcos*ysin_zsin + -xsin*zcos), 0.0f),
    Vector4((-ysin), (xsin*ycos), (xcos*ycos), 0.0f),
    Vector4(0.0f, 0.0f, 0.0f, 1.0f) );
}

//! スケーリングマトリクス
inline Matrix44 Matrix44::GetScaling( const Vector3& scale )
{
  return Matrix44(
    Vector4( scale.x, 0.0f, 0.0f, 0.0f),
    Vector4( 0.0f, scale.y, 0.0f, 0.0f),
    Vector4( 0.0f, 0.0f, scale.z, 0.0f),
    Vector4(0.0f, 0.0f, 0.0f, 1.0f) );
}

//! 平行移動マトリクス
inline Matrix44 Matrix44::GetTranslation( const Vector3& translation )
{
  return Matrix44(
    Vector4( 1.0f, 0.0f, 0.0f, translation.x),
    Vector4( 0.0f, 1.0f, 0.0f, translation.y),
    Vector4( 0.0f, 0.0f, 1.0f, translation.z),
    Vector4(0.0f, 0.0f, 0.0f, 1.0f) );
}

inline Matrix44 Matrix44::GetLookAt( const Vector3& eye, const Vector3& aim, const Vector3& up )
{
  const Vector3 d = (aim - eye).Normalize();
  const Vector3 s = d.Cross( up ).Normalize();
  const Vector3 r = s.Cross( d );

  return Matrix44(
    Vector4(s.x, s.y, s.z, -eye.Dot(s)),
    Vector4(r.x, r.y, r.z, -eye.Dot(r)),
    Vector4(-d.x, -d.y, -d.z, eye.Dot(d)),
    Vector4(0.0f, 0.0f, 0.0f, 1.0f) 
    );
}

//! Perspective
inline Matrix44 Matrix44::GetPerspective( float fovy, float aspect, float zNear, float zFar, bool isPivot )
{
#if defined(NN_PLATFORM_CTR)
  // @todo 9月ROMまで： とりあえずCTR_SDKつかって実装しました
  Matrix44 mtx;
  nn::math::PivotDirection pivot = (isPivot)
    ? nn::math::PIVOT_UPSIDE_TO_TOP
    : nn::math::PIVOT_NONE;
  nn::math::MTX44PerspectivePivotRad((nn::math::MTX44*)&mtx, fovy, aspect, zNear, zFar, pivot);
  return mtx;
#else
  (void)isPivot;

	return GetPerspectiveGL(fovy,aspect,zNear,zFar);
#endif
}

//! Frustum
inline Matrix44 Matrix44::GetFrustum( float left, float right, float bottom, float top, float zNear, float zFar, bool isPivot )
{
#if defined(NN_PLATFORM_CTR)
  // @todo 9月ROMまで： とりあえずCTR_SDKつかって実装しました
  Matrix44 mtx;
  nn::math::PivotDirection pivot = (isPivot)
    ? nn::math::PIVOT_UPSIDE_TO_TOP
    : nn::math::PIVOT_NONE;
  nn::math::MTX44FrustumPivot((nn::math::MTX44*)&mtx, left, right, bottom, top, zNear, zFar, pivot);
  return mtx;
#else
  (void)isPivot;

	return GetFrustumGL(left,right,bottom,top,zNear,zFar);
#endif
}

//! ortho
inline Matrix44 Matrix44::GetOrthographic( float left, float right, float bottom, float top, float zNear, float zFar, bool isPivot )
{
#if defined(NN_PLATFORM_CTR)
  // @todo 9月ROMまで： とりあえずCTR_SDKつかって実装しました
  Matrix44 mtx;
  nn::math::PivotDirection pivot = (isPivot)
    ? nn::math::PIVOT_UPSIDE_TO_TOP
    : nn::math::PIVOT_NONE;
  nn::math::MTX44OrthoPivot((nn::math::MTX44*)&mtx, left, right, bottom, top, zNear, zFar, pivot);
  return mtx;
#else
  (void)isPivot;

	return GetOrthographicGL(left,right,bottom,top,zNear,zFar);
#endif
}
//=============
// Perspective projection matrix:
// cot(fovy/2)/aspect        0            0                     0
//      0             cot(fovy/2)         0                     0
//      0                    0  (zFar+zNear)/(zNear-zFar) 2*zFar*zNear/(zNear-zFar)
//      0                    0           -1                     0
//! Perspective OpenGL互換(Depth=-1.0～+1.0に射影)
inline Matrix44 Matrix44::GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar )
{
  f32 f = tanf( (PI * 0.5f) - fovy * 0.5f );
  f32 rangeInv = 1.0f / ( zNear - zFar );

  return Matrix44(
    math::Vector4( f / aspect,  0.0f,   0.0f,                           0.0f ),
    math::Vector4( 0.0f,        f,      0.0f,                           0.0f ),
    math::Vector4( 0.0f,        0.0f,   ( zNear + zFar ) * rangeInv,    zNear * zFar * rangeInv * 2.0f ),
    math::Vector4( 0.0f,        0.0f,   -1.0f,                          0.0f )
    );
}

//! Perspective DirectX互換(Depth=0.0～+1.0に射影)
inline Matrix44 Matrix44::GetPerspectiveDX( float fovy, float aspect, float zNear, float zFar )
{
	math::Matrix44	conv = math::Matrix44(
		math::Vector4(1.0f, 0.0f, 0.0f, 0.0f),
		math::Vector4(0.0f, 1.0f, 0.0f, 0.0f),
		math::Vector4( 0.0f, 0.0f, 0.5f, 0.5f ),
		math::Vector4( 0.0f, 0.0f, 0.0f, 1.0f )
	);

	math::Matrix44	z_minus = math::Matrix44(
		math::Vector4( 1.0f, 0.0f, 0.0f, 0.0f ),
		math::Vector4( 0.0f, 1.0f, 0.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, -1.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, 0.0f, 1.0f )
	);

	//Ｚレンジを[-1...+1]→[0...1]に変換。
	return conv * GetPerspectiveGL(fovy,aspect,zNear,zFar);// * z_minus;
}

//! Frustum OpenGL互換(Depth=-1.0～+1.0に射影)
inline Matrix44 Matrix44::GetFrustumGL( float left, float right, float bottom, float top, float zNear, float zFar )
{
  f32 n2 = 2.0f * zNear;
  f32 w = right - left;
  f32 h = top - bottom;
  f32 d = zNear-zFar;

  return Matrix44(
    math::Vector4( n2 / w,      0.0f,       (right+left)/w,   0.0f ),
    math::Vector4( 0.0f,        n2 / h,     (top+bottom)/h,   0.0f ),
    math::Vector4( 0.0f,        0.0f,       (zFar+zNear)/d,   2.0f*zFar*zNear/d ),
    math::Vector4( 0.0f,        0.0f,       -1.0f,            0.0f )
    );
}
//! Frustum DirectX互換(Depth=0.0～+1.0に射影)
inline Matrix44 Matrix44::GetFrustumDX( float left, float right, float bottom, float top, float zNear, float zFar )
{
	math::Matrix44	conv = math::Matrix44(
		math::Vector4(1.0f, 0.0f, 0.0f, 0.0f),
		math::Vector4(0.0f, 1.0f, 0.0f, 0.0f),
		math::Vector4( 0.0f, 0.0f, 0.5f, 0.5f ),
		math::Vector4( 0.0f, 0.0f, 0.0f, 1.0f )
	);

	math::Matrix44	z_minus = math::Matrix44(
		math::Vector4( 1.0f, 0.0f, 0.0f, 0.0f ),
		math::Vector4( 0.0f, 1.0f, 0.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, -1.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, 0.0f, 1.0f )
	);

	//Ｚレンジを[-1...+1]→[0...1]に変換。
	return conv * GetFrustumGL(left,right,bottom,top,zNear,zFar) * z_minus;
}

//! ortho OpenGL互換(Depth=-1.0～+1.0に射影)
inline Matrix44 Matrix44::GetOrthographicGL( float left, float right, float bottom, float top, float zNear, float zFar )
{
  f32 w = right - left;
  f32 h = top - bottom;
  f32 d = zFar-zNear;

  return Matrix44(
    math::Vector4( 2.0f / w,  0.0f,        0.0f,          -(right+left)/w  ),
    math::Vector4( 0.0f,      2.0f / h,    0.0f,          -(top+bottom)/h   ),
    math::Vector4( 0.0f,      0.0f,        -2.0f / d,     -(zFar+zNear)/d  ),
    math::Vector4( 0.0f,      0.0f,        0.0f ,         1.0f )
    );
}

//! ortho DirectX互換(Depth=0.0～+1.0に射影)
inline Matrix44 Matrix44::GetOrthographicDX( float left, float right, float bottom, float top, float zNear, float zFar )
{
	math::Matrix44	conv = math::Matrix44(
		math::Vector4(1.0f, 0.0f, 0.0f, 0.0f),
		math::Vector4(0.0f, 1.0f, 0.0f, 0.0f),
		math::Vector4( 0.0f, 0.0f, 0.5f, 0.5f ),
		math::Vector4( 0.0f, 0.0f, 0.0f, 1.0f )
	);

	math::Matrix44	z_minus = math::Matrix44(
		math::Vector4( 1.0f, 0.0f, 0.0f, 0.0f ),
		math::Vector4( 0.0f, 1.0f, 0.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, -1.0f, 0.0f ),
		math::Vector4( 0.0f, 0.0f, 0.0f, 1.0f )
	);

	//Ｚレンジを[-1...+1]→[0...1]に変換。
	return conv * GetOrthographicGL(left,right,bottom,top,zNear,zFar) * z_minus;
}

/*inline Matrix44 Matrix44::GetBillboardMatrixRotationEye( const Vector4& cameraPosition, const Vector4& targetPosition )
{
	math::Vector4 direction = (cameraPosition - targetPosition).Normalize();
	math::Matrix44 rotationMatrix;

	rotationMatrix[0] = ( math::Vector4::GetYUnit().Cross( direction ).Normalize() );
	rotationMatrix[1] = ( direction );
	rotationMatrix[2] = ( direction.Cross( rotationMatrix[0] ) );
	rotationMatrix[3] = ( math::Vector4::GetWUnit() );

	return rotationMatrix;
}

inline Matrix44 Matrix44::GetBillboardMatrixRotationEyeY( const Vector4& cameraPosition, const Vector4& targetPosition )
{
	math::Vector4 direction = math::Vector4(cameraPosition - targetPosition).SetY(0.0f).Normalize();
	math::Matrix44 rotationMatrix;
	rotationMatrix[0] = ( math::Vector4::GetYUnit().Cross( direction ).Normalize() );
	rotationMatrix[2] = ( direction );
	rotationMatrix[1] = ( math::Vector4::GetYUnit() );
	rotationMatrix[3] = ( math::Vector4::GetWUnit() );
	return rotationMatrix;
}*/

inline Matrix44 Matrix44::GetBillboardMatrixRotationXYZ(const Matrix44& viewMatrix )
{
  return math::Matrix44(viewMatrix).SetTranslationVector( math::Vector4::GetWUnit() );
}

inline Matrix44 Matrix44::GetBillboardMatrixRotationY(const Matrix44& viewMatrix44 )
{
	math::Matrix44	rotationMatrix;
	math::Vector4		viewMatrix[4];

	for( u32 x = 0; x < 4; ++x )
	{
		for( u32 y = 0; y < 4; ++y )
		{
			viewMatrix[x][y] = viewMatrix44[x][y];
		}
	}

	rotationMatrix[1] = (math::Vector4::GetYUnit());
	rotationMatrix[0] = (rotationMatrix[1].Cross( viewMatrix[2] ).Normalize());
	rotationMatrix[2] = (rotationMatrix[0].Cross( rotationMatrix[1] ).Normalize());
	rotationMatrix[3] = ( math::Vector4::GetWUnit() );

	return rotationMatrix.Transpose();
}

//  ｗ＝１と仮定したベクトルとマトリクスとの乗算
inline Vector3 Matrix44::TransformCoord( const Vector3& rhs ) const
{
  Vector4 result = *this * Vector4(rhs.x, rhs.y, rhs.z, 1.0f);
  return Vector3(result.x / result.w, result.y / result.w, result.z / result.w);
}

//  ｗ＝０と仮定したベクトルとマトリクスとの乗算
inline Vector3 Matrix44::TransformNormal( const Vector3& rhs ) const
{
  return Vector3(
    this->row[0][0] * rhs.x + this->row[0][1] * rhs.y + this->row[0][2] * rhs.z,
    this->row[1][0] * rhs.x + this->row[1][1] * rhs.y + this->row[1][2] * rhs.z,
    this->row[2][0] * rhs.x + this->row[2][1] * rhs.y + this->row[2][2] * rhs.z
  );
}

// ベクトルとマトリクスとの乗算
inline Vector4 Matrix44::TransformVec( const Vector4& rhs ) const
{
  Vector4 result = (*this * rhs);
  return result / result.GetW();
}

// absolute values per element
inline Matrix44 Matrix44::AbsPerElem( void ) const
{
  return Matrix44(  row[0].AbsPerElem(), 
                  row[1].AbsPerElem(),
                  row[2].AbsPerElem(),
                  row[3].AbsPerElem()
                  );
}
