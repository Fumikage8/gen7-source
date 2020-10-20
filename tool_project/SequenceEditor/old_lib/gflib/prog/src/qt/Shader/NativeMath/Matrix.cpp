#include <math.h>

//! 円周率 π
static const float PI  = ((float) 3.14159265358979323846);
static const float PI2 = PI * ((float) 2.0);

#include "qt/Shader/NativeMath/Matrix.h"

// @todo 完成したらcppは削除してヘッダーだけの単なるインライン関数にすべて変更する予定。ので将来的にはNativeMathはライブラリではなくなる。

namespace gfl { namespace maya { namespace Math {

Matrix::Matrix()
{

}

Matrix::Matrix( const Matrix&  rhs )
{
	*this = rhs;
}

Matrix::Matrix( const Vector& col0_, const Vector& col1_, const Vector& col2_, const Vector& col3_ )
{
	this->col0 = col0_;
    this->col1 = col1_;
    this->col2 = col2_;
    this->col3 = col3_;
}

Matrix::~Matrix()
{

}

Matrix& Matrix::operator =( const Matrix&  rhs )
{
	col[0] = rhs.col[0];
  col[1] = rhs.col[1];
  col[2] = rhs.col[2];
  col[3] = rhs.col[3];
  return *this;	
}

//! 配列アクセス
Vector&  Matrix::operator []( int col )
{
	return (this->col[col]);
}

//! 配列アクセス
const Vector& Matrix::operator []( int col ) const
{
	return (this->col[col]);
}

//! 要素の設定
Matrix& Matrix::SetElem( int col, int row, float val )
{
	this->col[col].f[row] = val;
	return *this;
}

//! 要素の取得
float Matrix::GetElem( int col, int row ) const
{
	return this->col[col].f[row];
}

//! 加算
Matrix Matrix::operator +( const Matrix&  rhs ) const
{
	Matrix  m;
    m.col[0] = col[0] + rhs.col[0];
    m.col[1] = col[1] + rhs.col[1];
    m.col[2] = col[2] + rhs.col[2];
    m.col[3] = col[3] + rhs.col[3];
    return m;
}

//! 減算
Matrix Matrix::operator -( const Matrix&  rhs ) const
{
	Matrix  m;
    m.col[0] = col[0] - rhs.col[0];
    m.col[1] = col[1] - rhs.col[1];
    m.col[2] = col[2] - rhs.col[2];
    m.col[3] = col[3] - rhs.col[3];
    return m;
}

//! negate
Matrix Matrix::operator -( ) const
{
	Matrix  m;
    m.col[0] = -col[0];
    m.col[1] = -col[1];
    m.col[2] = -col[2];
    m.col[3] = -col[3];
    return m;
}

Matrix Matrix::operator *( float rhs ) const
{
	return  Matrix (
        col0 * rhs,
        col1 * rhs,
        col2 * rhs,
        col3 * rhs
    );
}

Matrix& Matrix::operator *=( float rhs )
{
	col0 *= rhs;
    col1 *= rhs;
    col2 *= rhs;
    col3 *= rhs;
    return (*this);
}

//! ベクトルとの乗算
Vector Matrix::operator *( const Vector& rhs ) const
{
	return col[0] * rhs.x + col[1] * rhs.y + col[2] * rhs.z + col[3] * rhs.w;
}

//! 乗算
Matrix Matrix::operator *( const Matrix&  rhs ) const
{
	return Matrix( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2, *this * rhs.col3 );
}

//! 加算代入
Matrix&  Matrix::operator +=( const Matrix&  rhs )
{
	col0 += rhs.col0;
    col1 += rhs.col1;
    col2 += rhs.col2;
    col3 += rhs.col3;
    return (*this);
}

//! 減算代入
Matrix&  Matrix::operator -=( const Matrix&  rhs )
{
	col0 -= rhs.col0;
    col1 -= rhs.col1;
    col2 -= rhs.col2;
    col3 -= rhs.col3;
    return (*this);
}

//! 乗算代入
Matrix&  Matrix::operator *=( const Matrix&  rhs )
{
	*this = *this * rhs;
    return *this;
}

//! マトリクスの転置
Matrix Matrix::Transpose( void ) const
{
	return Matrix(
        Vector( col0.x, col1.x, col2.x, col3.x ),
        Vector( col0.y, col1.y, col2.y, col3.y ),
        Vector( col0.z, col1.z, col2.z, col3.z ),
        Vector( col0.w, col1.w, col2.w, col3.w )
    );
}

//! 逆マトリクス
Matrix Matrix::Inverse( void ) const
{
	Matrix ans;
    float   m00 = col0.x;
    float   m01 = col0.y;
    float   m02 = col0.z;
    float   m03 = col0.w;
    float   m10 = col1.x;
    float   m11 = col1.y;
    float   m12 = col1.z;
    float   m13 = col1.w;
    float   m20 = col2.x;
    float   m21 = col2.y;
    float   m22 = col2.z;
    float   m23 = col2.w;
    float   m30 = col3.x;
    float   m31 = col3.y;
    float   m32 = col3.z;
    float   m33 = col3.w;
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
    //assert(determ > Math::epsilon);
    
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

Matrix Matrix::AffineInverse( void ) const
{
	float   m00 = col0.x;
    float   m01 = col0.y;
    float   m02 = col0.z;
    float   m10 = col1.x;
    float   m11 = col1.y;
    float   m12 = col1.z;
    float   m20 = col2.x;
    float   m21 = col2.y;
    float   m22 = col2.z;
    float   m30 = col3.x;
    float   m31 = col3.y;
    float   m32 = col3.z;
    float det = 1.0f /
                ( m00 * (m11 * m22 - m12 * m21)
                - m01 * (m10 * m22 - m12 * m20)
                + m02 * (m10 * m21 - m11 * m20) );
    Vector  col0( det * (m11 * m22 - m12 * m21), -det * (m01 * m22 - m02 * m21), det * (m01 * m12 - m02 * m11), 0.0f);
    Vector  col1( -det * (m10 * m22 - m12 * m20), det * (m00 * m22 - m02 * m20), -det * (m00 * m12 - m02 * m10), 0.0f);
    Vector  col2( det * (m10 * m21 - m11 * m20), -det * (m00 * m21 - m01 * m20), det * (m00 * m11 - m01 * m10), 0.0f);
    Vector  col3( -(m30 * col0[0] + m31 * col1[0] + m32 * col2[0]), -(m30 * col0[1] + m31 * col1[1] + m32 * col2[1]), -(m30 * col0[2] + m31 * col1[2] + m32 * col2[2]), 1.0f );

    return Matrix( col0, col1, col2, col3 );
}

//! 単位マトリクス
Matrix Matrix::GetIdentity( )
{
	Matrix  m(	Vector( 1.0f, 0.0f, 0.0f, 0.0f ),
				Vector( 0.0f, 1.0f, 0.0f, 0.0f ),
				Vector( 0.0f, 0.0f, 1.0f, 0.0f ),
				Vector( 0.0f, 0.0f, 0.0f, 1.0f ) );
    return m;
}

//! ゼロマトリクス
Matrix Matrix::GetZero()
{
	Matrix  m(	Vector( 0.0f, 0.0f, 0.0f, 0.0f ),
				Vector( 0.0f, 0.0f, 0.0f, 0.0f ),
				Vector( 0.0f, 0.0f, 0.0f, 0.0f ),
				Vector( 0.0f, 0.0f, 0.0f, 0.0f ) );
    return m;
}

//! X軸回転マトリクス
Matrix Matrix::GetRotationX( float radians )
{
	Vector  v0( 0.0f, cosf( radians ), sinf( radians ), 0.0f );
    Vector  v1( 0.0f, -sinf( radians ), cosf( radians ), 0.0f );
    Matrix  m( Vector( 1.0f, 0.0f, 0.0f, 0.0f ), v0, v1, Vector( 0.0f, 0.0f, 0.0f, 1.0f ) );
    return m;
}

//! Y軸回転マトリクス
Matrix Matrix::GetRotationY( float radians )
{
	Vector  v0( cosf( radians ), 0.0f, -sinf( radians ), 0.0f );
    Vector  v1( sinf( radians ), 0.0f, cosf( radians ), 0.0f );
    Matrix  m( v0, Vector( 0.0f, 1.0f, 0.0f, 0.0f ), v1, Vector( 0.0f, 0.0f, 0.0f, 1.0f ) );
    return m;
}

//! Z軸回転マトリクス
Matrix Matrix::GetRotationZ( float radians )
{
	Vector  v0( cosf( radians ), sinf( radians ), 0.0f, 0.0f );
    Vector  v1( -sinf( radians ), cosf( radians ), 0.0f, 0.0f );
    Matrix  m( v0, v1, Vector( 0.0f, 0.0f, 1.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f, 1.0f ) );
    return m;
}

//! Z * Y * X 回転マトリクス
Matrix Matrix::GetRotationZYX( float radiansZ, float radiansY, float radiansX )
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
		Vector( (ycos*zcos), (ycos*zsin), (-ysin), 0.0f ),
		Vector( (xsin*ysin*zcos+xcos*-zsin), (xsin*ysin*zsin+xcos*zcos), (xsin*ycos), 0.0f ),
		Vector( (xcos*ysin*zcos+-xsin*-zsin), (xcos*ysin*zsin+-xsin*zcos), (xcos*ycos), 0.0f ),
		Vector( 0.0f, 0.0f, 0.0f ,1.0f )
		);
}

//! スケーリングマトリクス
Matrix Matrix::GetScaling( const Vector& scale )
{
	Matrix  m(  Vector( scale.x, 0.0f, 0.0f, 0.0f),
                Vector( 0.0f, scale.y, 0.0f, 0.0f),
                Vector( 0.0f, 0.0f, scale.z, 0.0f),
                Vector( 0.0f, 0.0f, 0.0f, 1.0f ) );
    return m;
}

//! 平行移動マトリクス
Matrix Matrix::GetTranslation( const Vector& translation )
{
	Vector			trans = translation;
	trans.w = 1.0f;

	Matrix  m(  Vector( 1.0f, 0.0f, 0.0f, 0.0f ),
                Vector( 0.0f, 1.0f, 0.0f, 0.0f ),
                Vector( 0.0f, 0.0f, 1.0f, 0.0f ),
                trans );
    return m;
}

Matrix Matrix::GetLookAt( const Vector& eye, const Vector& aim, const Vector& up )
{
	/*const Vector d = (aim - eye).Normalize3();
    const Vector s = d.Cross( up ).Normalize3();
    const Vector r = s.Cross( d );
    return Matrix( Vector( s.x, r.x, -d.x, 0.0f ),
                   Vector( s.y, r.y, -d.y, 0.0f ),
                   Vector( s.z, r.z, -d.z, 0.0f ),
                   Vector( -eye.Dot3( s ), -eye.Dot3( r ), eye.Dot3( d ), 1.0f ) );*/
	const Vector cz = -(aim - eye).Normalize3();
	const Vector cx = cz.Cross( up ).Normalize3();
	const Vector cy = cx.Cross( cz );

	return Matrix( Vector( -cx.x, cy.x, -cz.x, 0.0f ),
                   Vector( cx.y, cy.y, -cz.y, 0.0f ),
                   Vector( -cx.z, cy.z, -cz.z, 0.0f ),
				   Vector( eye.Dot3( cx ), eye.Dot3( cy ), eye.Dot3( cz ), 1.0f ) );
}

//! Perspective
Matrix Matrix::GetPerspective( float fovy, float aspect, float zNear, float zFar )
{
	return GetPerspectiveDX(fovy,aspect,zNear,zFar);
}

//! Frustum
Matrix Matrix::GetFrustum( float left, float right, float bottom, float top, float zNear, float zFar )
{
	return GetFrustumDX(left,right,bottom,top,zNear,zFar);
}

//! ortho
Matrix Matrix::GetOrthographic( float left, float right, float bottom, float top, float zNear, float zFar )
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
Matrix Matrix::GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar )
{
	float f, rangeInv;
    Vector  zero, col0, col1, col2, col3;

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
Matrix Matrix::GetPerspectiveDX( float fovy, float aspect, float zNear, float zFar )
{
	Math::Matrix	conv = Math::Matrix(
		Math::Vector::GetXUnit(),
		Math::Vector::GetYUnit(),
		Math::Vector( 0.0f, 0.0f, 0.5f, 0.0f ),
		Math::Vector( 0.0f, 0.0f, 0.5f, 1.0f )
	);

	Math::Matrix	z_minus = Math::Matrix(
		Math::Vector( 1.0f, 0.0f, 0.0f, 0.0f ),
		Math::Vector( 0.0f, 1.0f, 0.0f, 0.0f ),
		Math::Vector( 0.0f, 0.0f, -1.0f, 0.0f ),
		Math::Vector( 0.0f, 0.0f, 0.0f, 1.0f )
	);

	//Ｚレンジを[-1...+1]→[0...1]に変換。
	return conv * GetPerspectiveGL(fovy,aspect,zNear,zFar) * z_minus;
}

//! Frustum OpenGL互換(Depth=-1.0～+1.0に射影)
Matrix Matrix::GetFrustumGL( float left, float right, float bottom, float top, float zNear, float zFar )
{
	Vector  col0, col1, col2, col3;

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
Matrix Matrix::GetFrustumDX( float left, float right, float bottom, float top, float zNear, float zFar )
{
	Math::Matrix	conv = Math::Matrix(
		Math::Vector::GetXUnit(),
		Math::Vector::GetYUnit(),
		Math::Vector( 0.0f, 0.0f, 0.5f, 0.0f ),
		Math::Vector( 0.0f, 0.0f, 0.5f, 1.0f )
	);

	Math::Matrix	z_minus = Math::Matrix(
		Math::Vector( 0.0f, 0.0f, 0.0f, 0.0f ),
		Math::Vector( 0.0f, 0.0f, 0.0f, 0.0f ),
		Math::Vector( 0.0f, 0.0f, -1.0f, 0.0f ),
		Math::Vector( 0.0f, 0.0f, 0.0f, 0.0f )
	);

	//Ｚレンジを[-1...+1]→[0...1]に変換。
	return conv * GetFrustumGL(left,right,bottom,top,zNear,zFar) * z_minus;
}

//! ortho OpenGL互換(Depth=-1.0～+1.0に射影)
Matrix Matrix::GetOrthographicGL( float left, float right, float bottom, float top, float zNear, float zFar )
{
	Vector  col0, col1, col2, col3;

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
Matrix Matrix::GetOrthographicDX( float left, float right, float bottom, float top, float zNear, float zFar )
{
	Math::Matrix	conv = Math::Matrix(
		Math::Vector::GetXUnit(),
		Math::Vector::GetYUnit(),
		Math::Vector( 0.0f, 0.0f, 0.5f, 0.0f ),
		Math::Vector( 0.0f, 0.0f, 0.5f, 1.0f )
	);

	Math::Matrix	z_minus = Math::Matrix(
		Math::Vector( 0.0f, 0.0f, 0.0f, 0.0f ),
		Math::Vector( 0.0f, 0.0f, 0.0f, 0.0f ),
		Math::Vector( 0.0f, 0.0f, -1.0f, 0.0f ),
		Math::Vector( 0.0f, 0.0f, 0.0f, 0.0f )
	);

	//Ｚレンジを[-1...+1]→[0...1]に変換。
	return conv * GetOrthographicGL(left,right,bottom,top,zNear,zFar) * z_minus;
}

}}}