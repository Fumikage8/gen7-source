#include <clr/include/math/gfl2_Quaternion_CLR.h>

namespace gfl2 { namespace clr { namespace math {
Quaternion::Quaternion()
{
	m_pNativeQuaternion = new gfl2::math::Quaternion();
}

Quaternion::Quaternion( f32 x, f32 y, f32 z, f32 w )
{
	m_pNativeQuaternion = new gfl2::math::Quaternion(x, y, z, w);
}

Quaternion::Quaternion( Quaternion^ other )
{
	m_pNativeQuaternion = new gfl2::math::Quaternion( *(other->GetNative()) );
}

Quaternion::Quaternion( gfl2::math::Quaternion& other )
{
	m_pNativeQuaternion = new gfl2::math::Quaternion( other );
}

Quaternion::~Quaternion()
{
	this->!Quaternion();
}

Quaternion::!Quaternion()
{
	if ( m_pNativeQuaternion )
	{
		delete m_pNativeQuaternion;
		m_pNativeQuaternion = 0;
	}
}

f32 Quaternion::GetElemX()
{
	return m_pNativeQuaternion->x;
}

f32 Quaternion::GetElemY()
{
	return m_pNativeQuaternion->y;
}

f32 Quaternion::GetElemZ()
{
	return m_pNativeQuaternion->z;
}

f32 Quaternion::GetElemW()
{
	return m_pNativeQuaternion->w;
}

void Quaternion::SetElem(f32 x, f32 y, f32 z, f32 w)
{
	m_pNativeQuaternion->x = x;
	m_pNativeQuaternion->y = y;
	m_pNativeQuaternion->z = z;
	m_pNativeQuaternion->w = w;
}

void Quaternion::Conj( Quaternion^ q )
{
	this->GetNative()->Conj( *q->GetNative() );
}

void Quaternion::Conj()
{
	this->GetNative()->Conj();
}

Quaternion^ Quaternion::GetConj()
{
	return gcnew Quaternion( m_pNativeQuaternion->GetConj() );
}

void Quaternion::Neg()
{
	m_pNativeQuaternion->Neg();
}

void Quaternion::Neg( Quaternion^ t )
{
	m_pNativeQuaternion->Neg( *t->GetNative() );
}

Quaternion^ Quaternion::GetNeg()
{
	return gcnew Quaternion( m_pNativeQuaternion->GetNeg() );
}

void Quaternion::Mul( Quaternion^ t, Quaternion^ s )
{
	m_pNativeQuaternion->Mul( *t->GetNative(), *s->GetNative() );
}

void Quaternion::Mul( Quaternion^ t )
{
	m_pNativeQuaternion->Mul( *t->GetNative() );
}

f32 Quaternion::Dot( Quaternion^ t )
{
	return m_pNativeQuaternion->Dot( *t->GetNative() );
}

void Quaternion::Inverse( Quaternion^ t )
{
	m_pNativeQuaternion->Inverse( *(t->GetNative()) );
}

void Quaternion::Inverse()
{
	m_pNativeQuaternion->Inverse();
}

Quaternion^ Quaternion::operator-()
{
	return gcnew Quaternion( -(*m_pNativeQuaternion ) );
}

void Quaternion::Slerp( Quaternion^ t, Quaternion^ s, f32 rate )
{
	m_pNativeQuaternion->Slerp( *t->GetNative(), *s->GetNative(), rate );
}

void Quaternion::Slerp( Quaternion^ t, f32 rate )
{
	m_pNativeQuaternion->Slerp( *t->GetNative(), rate );
}

void Quaternion::RadianToQuaternion( f32 x, f32 y, f32 z )
{
	m_pNativeQuaternion->RadianToQuaternion(x, y, z);
}

void Quaternion::RadianYXZToQuaternion( f32 x, f32 y, f32 z )
{
	m_pNativeQuaternion->RadianYXZToQuaternion(x, y, z);
}

void Quaternion::RadianToQuaternion( Vector^ v )
{
	m_pNativeQuaternion->RadianToQuaternion( v->x, v->y, v->z );
}

void Quaternion::RadianYXZToQuaternion( Vector^ v )
{
	m_pNativeQuaternion->RadianYXZToQuaternion( v->x, v->y, v->z );
}

void Quaternion::RadianYToQuaternion( f32 yang )
{
	m_pNativeQuaternion->RadianYToQuaternion(yang);
}

void Quaternion::QuaternionToRadianYXZ( Vector^ v )
{
	m_pNativeQuaternion->QuaternionToRadianYXZ( v->GetNative() );
}

f32 Quaternion::QuaternionToRadianY()
{
	return m_pNativeQuaternion->QuaternionToRadianY();
}

void Quaternion::QuaternionToRadian( Vector^ v )
{
	m_pNativeQuaternion->QuaternionToRadian( v->GetNative() );
}

Vector^ Quaternion::QuaternionToRadian()
{
	return gcnew Vector( m_pNativeQuaternion->QuaternionToRadian() );
}

void Quaternion::MatrixToQuaternion( Matrix^ mat )
{
  gfl2::math::Matrix34 m(
    (*mat->GetNative())[0],
    (*mat->GetNative())[1],
    (*mat->GetNative())[2]
  );

	m_pNativeQuaternion->MatrixToQuaternion(m);
}

void Quaternion::QuaternionToMatrix( Matrix^ mat )
{
  gfl2::math::Matrix34 m;
	m_pNativeQuaternion->QuaternionToMatrix(m);
  *mat->GetNative() = m;
}

Quaternion^ Quaternion::operator*=( Quaternion^ t )
{
	*m_pNativeQuaternion *= *(t->GetNative());
	return this;
}

Quaternion^ Quaternion::operator*( Quaternion^ t )
{
	return gcnew Quaternion( m_pNativeQuaternion->operator*( *(t->GetNative() ) ) );
}

void Quaternion::Sub( Quaternion^ t )
{
	m_pNativeQuaternion->Sub( *(t->GetNative()) );
}

void Quaternion::Sub( Quaternion^ t, Quaternion^ s )
{
	m_pNativeQuaternion->Sub( *(t->GetNative()), *(s->GetNative()) );
}

f32 Quaternion::Magnitute2()
{
	return m_pNativeQuaternion->Magnitude2();
}

f32 Quaternion::Magnitude2Recip()
{
	return m_pNativeQuaternion->Magnitude2Recip();
}

f32 Quaternion::Magnitude()
{
	return m_pNativeQuaternion->Magnitude();
}

f32 Quaternion::MagnitudeRecip()
{
	return m_pNativeQuaternion->MagnitudeRecip();
}

void Quaternion::Normalize()
{
	m_pNativeQuaternion->Normalize();
}

void Quaternion::Normalize( Quaternion^ t )
{
	m_pNativeQuaternion->Normalize( *(t->GetNative() ) );
}

f32 Quaternion::NormalizeRet()
{
	return m_pNativeQuaternion->NormalizeRet();
}

Quaternion^ Quaternion::operator-=( Quaternion^ t )
{
	return gcnew Quaternion( m_pNativeQuaternion->operator-=( *(t->GetNative() ) ) );
}

Quaternion^ Quaternion::operator-( Quaternion^ t )
{
	return gcnew Quaternion( m_pNativeQuaternion->operator-( *(t->GetNative() ) ) );
}

}}}