//==============================================================================
/**
 * @file	gfl_Vector.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2012/10/19, 14:26
 */
// =============================================================================

namespace gfl {
namespace core {

#ifndef GFL_PLATFORM_3DS
const VectorStruct2 VectorNative2::s_Zero = { 0.0f, 0.0f };
const VectorStruct2 VectorNative2::s_One = { 1.0f, 1.0f };
const VectorStruct3 VectorNative3::s_Zero = { 0.0f, 0.0f, 0.0f };
const VectorStruct3 VectorNative3::s_One = { 1.0f, 1.0f, 1.0f };
const VectorStruct4 VectorNative4::s_Zero = { 0.0f, 0.0f, 0.0f, 0.0f };
const VectorStruct4 VectorNative4::s_One = { 1.0f, 1.0f, 1.0f, 1.0f };
#endif
const VectorStruct3 Vector3::s_X1 = { 1.0f, 0.0f, 0.0f };
const VectorStruct3 Vector3::s_Y1 = { 0.0f, 1.0f, 0.0f };
const VectorStruct3 Vector3::s_Z1 = { 0.0f, 0.0f, 1.0f };
const VectorStruct4 Vector4::s_X1 = { 1.0f, 0.0f, 0.0f, 0.0f };
const VectorStruct4 Vector4::s_Y1 = { 0.0f, 1.0f, 0.0f, 0.0f };
const VectorStruct4 Vector4::s_Z1 = { 0.0f, 0.0f, 1.0f, 0.0f };
const VectorStruct4 Vector4::s_W1 = { 0.0f, 0.0f, 0.0f, 1.0f };

#ifndef GFL_PLATFORM_3DS
bool VectorNative4::IsZeroWOne( )
{
	return *this == Vector4::GetW1( );
}
const VectorNative4& VectorNative4::ZeroWOne( )
{
	return Vector4::GetW1( );
}
#endif

void Vector4::SetUnit( void ) {
	Set( reinterpret_cast<const Vector4&> (Vector4::GetW1()) );
}
//template void VectorTemplate<VectorNative4>::SetUnit( void );
//template void VectorTemplate<QuaternionNative>::SetUnit( void );

// -------------------------------------------------------------------------
// Calc Normal
// -------------------------------------------------------------------------
void Vector3::CalcNormal( const Vector3& v0, const Vector3& v1, const Vector3& v2 )
{
	Vector3 vd0, vd1;

	vd0.Sub( v1, v0 );
	vd1.Sub( v2, v0 );
	Cross( vd0, vd1 );
	Normalize( );
}
#ifdef GFL_PLATFORM_PC
VectorNative3* VEC3Set( VectorNative3 * pOut, const Vector4& p1 ) {
	((Vector3*)pOut)->Set( p1 );
	return pOut;
}

VectorNative3 * VEC3Maximize( VectorNative3 *pOut, const VectorNative3 * p1, const VectorNative3 * p2 ) {
	pOut->Set( ::std::max( p1->x, p2->x ), ::std::max( p1->y, p2->y ), ::std::max( p1->z, p2->z ) );
	return pOut;
}

VectorNative3 * VEC3Minimize( VectorNative3 *pOut, const VectorNative3 * p1, const VectorNative3 * p2 ) {
	pOut->Set( ::std::min( p1->x, p2->x ), ::std::min( p1->y, p2->y ), ::std::min( p1->z, p2->z ) );
	return pOut;
}

VectorNative3 * VEC3Add( VectorNative3 *pOut, const VectorNative3 * p1, const VectorNative3 * p2 ) {
	pOut->Add( *p1, *p2 );
	return pOut;
}

VectorNative3 * VEC3Sub( VectorNative3 *pOut, const VectorNative3 * p1, const VectorNative3 * p2 ) {
	pOut->Sub( *p1, *p2 );
	return pOut;
}

VectorNative3 * VEC3Scale( VectorNative3 *pOut, const VectorNative3 * p1, f32 scl ) {
	pOut->Mul( *p1, scl );
	return pOut;
}
#endif
}
}
