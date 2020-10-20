inline Matrix34::Matrix34()
{
	row[0].Set(0, 0, 0, 0);
	row[1].Set(0, 0, 0, 0);
	row[2].Set(0, 0, 0, 0);
}

inline Matrix34::Matrix34( const Matrix34&  rhs )
{
	*this = rhs;
}

inline Matrix34::Matrix34( const Vector4& row0, const Vector4& row1, const Vector4& row2 )
{
  this->row[0] = row0;
  this->row[1] = row1;
  this->row[2] = row2;
}

inline Matrix34& Matrix34::operator =( const Matrix34&  rhs )
{
	row[0] = rhs.row[0];
  row[1] = rhs.row[1];
  row[2] = rhs.row[2];
  return *this;	
}

//! 配列アクセス
__forceinline Vector4&  Matrix34::operator []( int row )
{
	return (this->row[row]);
}

//! 配列アクセス
__forceinline const Vector4& Matrix34::operator []( int row ) const
{
	return (this->row[row]);
}

//! 要素の設定
inline Matrix34& Matrix34::SetElem( int row, int col, float val )
{
	this->row[row][col] = val;
	return *this;
}

//! 要素の取得
inline float Matrix34::GetElem( int row, int col ) const
{
	return this->row[row][col];
}

//! 位置要素の取得
inline Vector3 Matrix34::GetElemPosition() const
{
  return Vector3(this->row[0][3], this->row[1][3], this->row[2][3]);
}

//! X軸の取得
inline Vector3 Matrix34::GetElemAxisX() const
{
  return math::Vector3(this->row[0][0], this->row[1][0], this->row[2][0]);
}

//! Y軸の取得
inline Vector3 Matrix34::GetElemAxisY() const
{
  return math::Vector3(this->row[0][1], this->row[1][1], this->row[2][1]);
}

//! Z軸の取得
inline Vector3 Matrix34::GetElemAxisZ() const
{
  return math::Vector3(this->row[0][2], this->row[1][2], this->row[2][2]);
}

//! 回転要素の取得ZYX（ラジアン）
inline Vector3 Matrix34::GetElemRotationZYX() const
{
  gfl2::math::Matrix34 tmp = *this;
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

// @fix GFNMCat[5302] : ジンバルロック時に正しい値が返ってこない
//! 回転要素の取得ZYX（ラジアン）：ジンバルロック修正版
inline Vector3 Matrix34::GetElemRotationZYX_fix() const
{
  // スケール成分が含まれる可能性があるので各軸を正規化しておく
  gfl2::math::Matrix34 tmp = *this;
  tmp.Normalize();

  // 回転行列はこの様に格納されている。ここから回転角を復元する
  // ycos*zcos,   xsin*ysin*zcos - xcos*zsin,   xcos*ysin*zcos + xsin*zsin
  // ycos*zsin,   xsin*ysin*zsin + xcos*zcos,   xcos*ysin*zsin - xsin*zcos
  //     -ysin,                    xsin*ycos,                    xcos*ycos

  f32 ysin = -tmp[2][0]; // -ysin

  // ysinが±1.0ならジンバルロックしているので通常の方法だと復元できない。先に判定しておく
  static const f32 sc_errorValue = 0.00001f;
  if ((ysin > -1.0f + sc_errorValue) && (ysin < 1.0f - sc_errorValue))
  {
    // 通常の分解
    return gfl2::math::Vector3(
      atan2f(tmp[2][1], tmp[2][2]), // xsin*ycos, xcos*ycos
      asinf(ysin),                  // ysin
      atan2f(tmp[1][0], tmp[0][0])  // ycos*zsin, ycos*zcos
    );
  }

  // ジンバルロック -π/2
  if (ysin < 0.0f)
  {
    // 「ysin = -1.0, ycos = 0.0」を代入して式変形
    //  0.0, -sin(x+z), -cos(x+z)
    //  0.0,  cos(x+z), -sin(x+z)
    //  1.0,       0.0,       0.0

    // x+zは分解できないので、x=0で固定して全てz成分ということにする
    return gfl2::math::Vector3(
      0.0f,                         // 0 で固定
      -PI * 0.5f,                   // -π/2 で固定
      atan2(-tmp[0][1], tmp[1][1])  // -sin(0+z), cos(0+z)
    );
  }

  // ジンバルロック π/2

  // 「ysin = 1.0, ycos = 0.0」を代入して式変形
  //  0.0,  sin(x-z),  cos(x-z)
  //  0.0,  cos(x-z), -sin(x-z)
  // -1.0,       0.0,       0.0

  // x-zは分解できないので、x=0で固定して全てz成分ということにする
  return gfl2::math::Vector3(
    0.0f,                         // 0 で固定
    PI * 0.5f,                    // π/2 で固定
    -atan2(tmp[0][1], tmp[1][1])  // sin(0-z), cos(0-z)
  );
}

//! 拡大縮小要素の取得
inline Vector3 Matrix34::GetElemScaling() const
{
  return Vector3(this->GetElemAxisX().Length(), this->GetElemAxisY().Length(), this->GetElemAxisZ().Length());
}

//! 加算
inline Matrix34 Matrix34::operator +( const Matrix34&  rhs ) const
{
	Matrix34  m;
    m.row[0] = row[0] + rhs.row[0];
    m.row[1] = row[1] + rhs.row[1];
    m.row[2] = row[2] + rhs.row[2];
    return m;
}

//! 減算
inline Matrix34 Matrix34::operator -( const Matrix34&  rhs ) const
{
	Matrix34  m;
    m.row[0] = row[0] - rhs.row[0];
    m.row[1] = row[1] - rhs.row[1];
    m.row[2] = row[2] - rhs.row[2];
    return m;
}

//! negate
inline Matrix34 Matrix34::operator -( ) const
{
	Matrix34  m;
    m.row[0] = -row[0];
    m.row[1] = -row[1];
    m.row[2] = -row[2];
    return m;
}

inline Matrix34 Matrix34::operator *( float rhs ) const
{
	return  Matrix34 (
        row[0] * rhs,
        row[1] * rhs,
        row[2] * rhs
    );
}

inline Matrix34& Matrix34::operator *=( float rhs )
{
  row[0] *= rhs;
  row[1] *= rhs;
  row[2] *= rhs;
  return (*this);
}

//! ベクトルとの乗算
inline Vector3 Matrix34::operator *( const Vector3& rhs ) const
{
  return Vector3(
    this->row[0][0] * rhs.x + this->row[0][1] * rhs.y + this->row[0][2] * rhs.z + this->row[0][3],
    this->row[1][0] * rhs.x + this->row[1][1] * rhs.y + this->row[1][2] * rhs.z + this->row[1][3],
    this->row[2][0] * rhs.x + this->row[2][1] * rhs.y + this->row[2][2] * rhs.z + this->row[2][3]
  );
}

//! 乗算
inline Matrix34 Matrix34::operator *( const Matrix34&  rhs ) const
{
#if defined(GF_PLATFORM_CTR)
  // SDKの関数の方が速かった
  Matrix34 result;
  nn::math::MTX34Mult((nn::math::MTX34*)&result, (nn::math::MTX34*)this, (nn::math::MTX34*)&rhs);
  return result;
#else
	return Matrix34(
    Vector4(
      this->row[0][0] * rhs.row[0][0] + this->row[0][1] * rhs.row[1][0] + this->row[0][2] * rhs.row[2][0],
      this->row[0][0] * rhs.row[0][1] + this->row[0][1] * rhs.row[1][1] + this->row[0][2] * rhs.row[2][1],
      this->row[0][0] * rhs.row[0][2] + this->row[0][1] * rhs.row[1][2] + this->row[0][2] * rhs.row[2][2],
      this->row[0][0] * rhs.row[0][3] + this->row[0][1] * rhs.row[1][3] + this->row[0][2] * rhs.row[2][3] + this->row[0][3]),

    Vector4(
      this->row[1][0] * rhs.row[0][0] + this->row[1][1] * rhs.row[1][0] + this->row[1][2] * rhs.row[2][0],
      this->row[1][0] * rhs.row[0][1] + this->row[1][1] * rhs.row[1][1] + this->row[1][2] * rhs.row[2][1],
      this->row[1][0] * rhs.row[0][2] + this->row[1][1] * rhs.row[1][2] + this->row[1][2] * rhs.row[2][2],
      this->row[1][0] * rhs.row[0][3] + this->row[1][1] * rhs.row[1][3] + this->row[1][2] * rhs.row[2][3] + this->row[1][3]),

    Vector4(
      this->row[2][0] * rhs.row[0][0] + this->row[2][1] * rhs.row[1][0] + this->row[2][2] * rhs.row[2][0],
      this->row[2][0] * rhs.row[0][1] + this->row[2][1] * rhs.row[1][1] + this->row[2][2] * rhs.row[2][1],
      this->row[2][0] * rhs.row[0][2] + this->row[2][1] * rhs.row[1][2] + this->row[2][2] * rhs.row[2][2],
      this->row[2][0] * rhs.row[0][3] + this->row[2][1] * rhs.row[1][3] + this->row[2][2] * rhs.row[2][3] + this->row[2][3])
  );
#endif
}

//! 加算代入
inline Matrix34&  Matrix34::operator +=( const Matrix34&  rhs )
{
  row[0] += rhs.row[0];
  row[1] += rhs.row[1];
  row[2] += rhs.row[2];
  return (*this);
}

//! 減算代入
inline Matrix34&  Matrix34::operator -=( const Matrix34&  rhs )
{
  row[0] -= rhs.row[0];
  row[1] -= rhs.row[1];
  row[2] -= rhs.row[2];
  return (*this);
}

//! 乗算代入
inline Matrix34&  Matrix34::operator *=( const Matrix34&  rhs )
{
	*this = *this * rhs;
  return *this;
}

//! マトリクスの転置
inline Matrix34 Matrix34::Transpose( void ) const
{
  // ※平行移動成分は消えます
  return Matrix34(
       Vector4( row[0].x, row[1].x, row[2].x, 0.0f ),
       Vector4( row[0].y, row[1].y, row[2].y, 0.0f ),
       Vector4( row[0].z, row[1].z, row[2].z, 0.0f )
    );
}

//! 逆マトリクス
inline Matrix34 Matrix34::Inverse( void ) const
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

  return Matrix34(
    Vector4(col0.x, col1.x, col2.x, col3.x),
    Vector4(col0.y, col1.y, col2.y, col3.y),
    Vector4(col0.z, col1.z, col2.z, col3.z)
    );
}

//! 単位マトリクス
inline Matrix34 Matrix34::GetIdentity( )
{
  return Matrix34(
    Vector4(1.0f, 0.0f, 0.0f, 0.0f),
    Vector4(0.0f, 1.0f, 0.0f, 0.0f),
    Vector4(0.0f, 0.0f, 1.0f, 0.0f)
    );
}

//! ゼロマトリクス
inline Matrix34 Matrix34::GetZero()
{
  return Matrix34(
    Vector4(0.0f, 0.0f, 0.0f, 0.0f),
    Vector4(0.0f, 0.0f, 0.0f, 0.0f),
    Vector4(0.0f, 0.0f, 0.0f, 0.0f)
    );
}

//! X軸回転マトリクス
inline Matrix34 Matrix34::GetRotationX( float radians )
{
  f32 cos_ = cosf(radians);
  f32 sin_ = sinf(radians);

  return Matrix34(
    Vector4(1.0f, 0.0f, 0.0f, 0.0f),
    Vector4(0.0f, cos_, -sin_, 0.0f),
    Vector4(0.0f, sin_, cos_, 0.0f) );
}

//! Y軸回転マトリクス
inline Matrix34 Matrix34::GetRotationY( float radians )
{
  f32 cos_ = cosf(radians);
  f32 sin_ = sinf(radians);

  return Matrix34(
    Vector4(cos_, 0.0f, sin_, 0.0f),
    Vector4(0.0f, 1.0f, 0.0f, 0.0f),
    Vector4(-sin_, 0.0f, cos_, 0.0f) );
}

//! Z軸回転マトリクス
inline Matrix34 Matrix34::GetRotationZ( float radians )
{
  f32 cos_ = cosf(radians);
  f32 sin_ = sinf(radians);

  return Matrix34(
    Vector4(cos_, -sin_, 0.0f, 0.0f),
    Vector4(sin_, cos_, 0.0f, 0.0f),
    Vector4(0.0f, 0.0f, 1.0f, 0.0f) );
}

//! Z * Y * X 回転マトリクス
inline Matrix34 Matrix34::GetRotationZYX( float radiansZ, float radiansY, float radiansX )
{
	f32 xsin = sinf( radiansX );
	f32 ysin = sinf( radiansY );
	f32 zsin = sinf( radiansZ );
	f32 xcos = cosf( radiansX );
	f32 ycos = cosf( radiansY );
	f32 zcos = cosf( radiansZ );

  f32 ysin_zcos = ysin * zcos;
  f32 ysin_zsin = ysin * zsin;

  return Matrix34(
    Vector4((ycos*zcos), (xsin*ysin_zcos + xcos*-zsin), (xcos*ysin_zcos + -xsin*-zsin), 0.0f),
    Vector4((ycos*zsin), (xsin*ysin_zsin + xcos*zcos), (xcos*ysin_zsin + -xsin*zcos), 0.0f),
    Vector4((-ysin), (xsin*ycos), (xcos*ycos), 0.0f) );
}
inline void Matrix34::GetRotationZYX(Matrix34& result, float radiansZ, float radiansY, float radiansX )
{
	f32 xsin = sinf( radiansX );
	f32 ysin = sinf( radiansY );
	f32 zsin = sinf( radiansZ );
	f32 xcos = cosf( radiansX );
	f32 ycos = cosf( radiansY );
	f32 zcos = cosf( radiansZ );

  f32 ysin_zcos = ysin * zcos;
  f32 ysin_zsin = ysin * zsin;

  result[0][0] = (ycos*zcos);
  result[0][1] = (xsin*ysin_zcos + xcos*-zsin);
  result[0][2] = (xcos*ysin_zcos + -xsin*-zsin);
  result[0][3] = 0.0f;

  result[1][0] = (ycos*zsin);
  result[1][1] = (xsin*ysin_zsin + xcos*zcos);
  result[1][2] = (xcos*ysin_zsin + -xsin*zcos);
  result[1][3] = 0.0f;

  result[2][0] = (-ysin);
  result[2][1] = (xsin*ycos);
  result[2][2] = (xcos*ycos);
  result[2][3] = 0.0f;
}

//! スケーリングマトリクス
inline Matrix34 Matrix34::GetScaling( const Vector3& scale )
{
  return Matrix34(
    Vector4( scale.x, 0.0f, 0.0f, 0.0f),
    Vector4( 0.0f, scale.y, 0.0f, 0.0f),
    Vector4( 0.0f, 0.0f, scale.z, 0.0f) );
}

//! 平行移動マトリクス
inline Matrix34 Matrix34::GetTranslation( const Vector3& translation )
{
  return Matrix34(
    Vector4( 1.0f, 0.0f, 0.0f, translation.x),
    Vector4( 0.0f, 1.0f, 0.0f, translation.y),
    Vector4( 0.0f, 0.0f, 1.0f, translation.z) );
}

inline Matrix34 Matrix34::GetLookAt( const Vector3& eye, const Vector3& aim, const Vector3& up )
{
  const Vector3 d = (aim - eye).Normalize();
  const Vector3 s = d.Cross( up ).Normalize();
  const Vector3 r = s.Cross( d );

  return Matrix34(
    Vector4(s.x, s.y, s.z, -eye.Dot(s)),
    Vector4(r.x, r.y, r.z, -eye.Dot(r)),
    Vector4(-d.x, -d.y, -d.z, eye.Dot(d))
    );
}


//  ｗ＝１と仮定したベクトルとマトリクスとの乗算
inline Vector3 Matrix34::TransformCoord( const Vector3& rhs ) const
{
  return Vector3(
    this->row[0][0] * rhs.x + this->row[0][1] * rhs.y + this->row[0][2] * rhs.z + this->row[0][3],
    this->row[1][0] * rhs.x + this->row[1][1] * rhs.y + this->row[1][2] * rhs.z + this->row[1][3],
    this->row[2][0] * rhs.x + this->row[2][1] * rhs.y + this->row[2][2] * rhs.z + this->row[2][3]
  );
}

//  ｗ＝１と仮定したベクトルとマトリクスとの乗算
inline Vector4 Matrix34::TransformCoord( const Vector4& rhs ) const
{
  return Vector4(
    this->row[0][0] * rhs.x + this->row[0][1] * rhs.y + this->row[0][2] * rhs.z + this->row[0][3],
    this->row[1][0] * rhs.x + this->row[1][1] * rhs.y + this->row[1][2] * rhs.z + this->row[1][3],
    this->row[2][0] * rhs.x + this->row[2][1] * rhs.y + this->row[2][2] * rhs.z + this->row[2][3],
    1.0f
  );
}

//  ｗ＝０と仮定したベクトルとマトリクスとの乗算
inline Vector3 Matrix34::TransformNormal( const Vector3& rhs ) const
{
  return Vector3(
    this->row[0][0] * rhs.x + this->row[0][1] * rhs.y + this->row[0][2] * rhs.z,
    this->row[1][0] * rhs.x + this->row[1][1] * rhs.y + this->row[1][2] * rhs.z,
    this->row[2][0] * rhs.x + this->row[2][1] * rhs.y + this->row[2][2] * rhs.z
  );
}

//  ｗ＝０と仮定したベクトルとマトリクスとの乗算
inline Vector4 Matrix34::TransformNormal( const Vector4& rhs ) const
{
  return Vector4(
    this->row[0][0] * rhs.x + this->row[0][1] * rhs.y + this->row[0][2] * rhs.z,
    this->row[1][0] * rhs.x + this->row[1][1] * rhs.y + this->row[1][2] * rhs.z,
    this->row[2][0] * rhs.x + this->row[2][1] * rhs.y + this->row[2][2] * rhs.z,
    0.0f
  );
}

// ベクトルとマトリクスとの乗算
inline Vector4 Matrix34::TransformVec( const Vector4& rhs ) const
{
  return Vector4(
    this->row[0][0] * rhs.x + this->row[0][1] * rhs.y + this->row[0][2] * rhs.z + this->row[0][3] * rhs.w,
    this->row[1][0] * rhs.x + this->row[1][1] * rhs.y + this->row[1][2] * rhs.z + this->row[1][3] * rhs.w,
    this->row[2][0] * rhs.x + this->row[2][1] * rhs.y + this->row[2][2] * rhs.z + this->row[2][3] * rhs.w,
    1.0f
  );
}


// absolute values per element
inline Matrix34 Matrix34::AbsPerElem( void ) const
{
  return Matrix34(  row[0].AbsPerElem(), 
                  row[1].AbsPerElem(),
                  row[2].AbsPerElem() );
}

//! 高速な乗算
inline void Matrix34::MultFast(Matrix34* out_pResult, const Matrix34& lhs, const Matrix34& rhs)
{
#if defined(GF_PLATFORM_CTR)
  nn::math::MTX34Mult((nn::math::MTX34*)out_pResult, (nn::math::MTX34*)&lhs, (nn::math::MTX34*)&rhs);
#else
  struct Tmp
  {
    f32 m[3][4];
  };

  Tmp* f_result = reinterpret_cast<Tmp*>(out_pResult);
  const Tmp* f_lhs = reinterpret_cast<const Tmp*>(&lhs);
  const Tmp* f_rhs = reinterpret_cast<const Tmp*>(&rhs);

  f_result->m[0][0] = f_lhs->m[0][0] * f_rhs->m[0][0] + f_lhs->m[0][1] * f_rhs->m[1][0] + f_lhs->m[0][2] * f_rhs->m[2][0];
  f_result->m[0][1] = f_lhs->m[0][0] * f_rhs->m[0][1] + f_lhs->m[0][1] * f_rhs->m[1][1] + f_lhs->m[0][2] * f_rhs->m[2][1];
  f_result->m[0][2] = f_lhs->m[0][0] * f_rhs->m[0][2] + f_lhs->m[0][1] * f_rhs->m[1][2] + f_lhs->m[0][2] * f_rhs->m[2][2];
  f_result->m[0][3] = f_lhs->m[0][0] * f_rhs->m[0][3] + f_lhs->m[0][1] * f_rhs->m[1][3] + f_lhs->m[0][2] * f_rhs->m[2][3] + f_lhs->m[0][3];

  f_result->m[1][0] = f_lhs->m[1][0] * f_rhs->m[0][0] + f_lhs->m[1][1] * f_rhs->m[1][0] + f_lhs->m[1][2] * f_rhs->m[2][0];
  f_result->m[1][1] = f_lhs->m[1][0] * f_rhs->m[0][1] + f_lhs->m[1][1] * f_rhs->m[1][1] + f_lhs->m[1][2] * f_rhs->m[2][1];
  f_result->m[1][2] = f_lhs->m[1][0] * f_rhs->m[0][2] + f_lhs->m[1][1] * f_rhs->m[1][2] + f_lhs->m[1][2] * f_rhs->m[2][2];
  f_result->m[1][3] = f_lhs->m[1][0] * f_rhs->m[0][3] + f_lhs->m[1][1] * f_rhs->m[1][3] + f_lhs->m[1][2] * f_rhs->m[2][3] + f_lhs->m[1][3];

  f_result->m[2][0] = f_lhs->m[2][0] * f_rhs->m[0][0] + f_lhs->m[2][1] * f_rhs->m[1][0] + f_lhs->m[2][2] * f_rhs->m[2][0];
  f_result->m[2][1] = f_lhs->m[2][0] * f_rhs->m[0][1] + f_lhs->m[2][1] * f_rhs->m[1][1] + f_lhs->m[2][2] * f_rhs->m[2][1];
  f_result->m[2][2] = f_lhs->m[2][0] * f_rhs->m[0][2] + f_lhs->m[2][1] * f_rhs->m[1][2] + f_lhs->m[2][2] * f_rhs->m[2][2];
  f_result->m[2][3] = f_lhs->m[2][0] * f_rhs->m[0][3] + f_lhs->m[2][1] * f_rhs->m[1][3] + f_lhs->m[2][2] * f_rhs->m[2][3] + f_lhs->m[2][3];
#endif
}

inline Matrix34 Matrix34::GetBillboardMatrixRotationXYZ(const Matrix34& viewMatrix )
{
  math::Matrix34 billboardMatrix = math::Matrix34(viewMatrix);
  billboardMatrix.SetTranslationVector(gfl2::math::Vector3::GetZero());
  return billboardMatrix;
}
