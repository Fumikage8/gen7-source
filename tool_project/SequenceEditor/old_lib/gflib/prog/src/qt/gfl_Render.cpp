
#include "qt/gfl_Render.h"

// =============================================================================
// Static Parameters
// =============================================================================
const f32 gfl::qt::Render::_scColorTable[][4] = {
	{0.75f, 0.75f, 0.75f, 1.0f }, // PCOL_WHITE

	{0.75f, 0.15f, 0.15f, 1.0f }, // PCOL_RED
	{0.15f, 0.75f, 0.15f, 1.0f }, // PCOL_GREEN
	{0.15f, 0.15f, 0.75f, 1.0f }, // PCOL_BLUE
	{0.75f, 0.75f, 0.15f, 1.0f }, // PCOL_YELLOW (Gold?)
	{0.45f, 0.45f, 0.45f, 1.0f }, // PCOL_GRAY

	{0.5f, 0.1f, 0.1f, 1.0f }, // PCOL_DARKRED
	{0.1f, 0.5f, 0.1f, 1.0f }, // PCOL_DARKGREEN
	{0.1f, 0.1f, 0.5f, 1.0f }, // PCOL_DARKBLUE
	{0.5f, 0.5f, 0.1f, 1.0f }, // PCOL_DARKYELLOW
	{0.25f, 0.25f, 0.25f, 1.0f }, // PCOL_DARKGRAY

	{0.15f, 0.75f, 0.75f, 1.0f }, // PCOL_SKYBLUE
};

const u8 gfl::qt::Render::_scBoneEdge[][4] = {
	{1, 2, 3, 4 },
	{0, 3, 5, 1 },
	{0, 4, 5, 2 },
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void gfl::qt::Render::RotY( const gfl::core::Vector& pos, const f32 trans_y, const f32 rot_y, const ShadingType type )
{
	if( type == SHD_WIRE ){
		glDisable( GL_LIGHTING );
	}

	glPushMatrix( );
	glTranslatef( pos.GetX( ), CENTI(2.0f) + trans_y, pos.GetZ( ) );
	glRotatef( gfl::core::Math::RadianToDegree( rot_y ), 0.0f, 1.0f, 0.0f );

	if( type == SHD_NORMAL ){
		glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, _scColorTable[PCOL_WHITE] );
		glBegin( GL_TRIANGLES );
	} else {
		glColor3fv( _scColorTable[PCOL_WHITE] );
		glBegin( GL_LINE_LOOP );
	}
	glVertex3f( 0.0f, 0.0f, CENTI( 50.0f ) );
	glVertex3f( CENTI( 10.0f ), 0.0f, 0.0f );
	glVertex3f( -CENTI( 10.0f ), 0.0f, 0.0f );
	glEnd( );

	glPopMatrix( );

	if( type == SHD_WIRE ){
		glEnable( GL_LIGHTING );
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void gfl::qt::Render::Pivot( const f32 x, const f32 z, const ShadingType type )
{
	if( type == SHD_WIRE ){
		glDisable( GL_LIGHTING );
	}

	glPushMatrix( );
	glTranslatef( x, CENTI( 2.0f ), z );

	if( type == SHD_NORMAL ){
		glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, _scColorTable[PCOL_WHITE] );
		glBegin( GL_QUADS );
	} else {
		glColor3fv( _scColorTable[PCOL_WHITE] );
		glBegin( GL_LINE_LOOP );
	}

	const f32 size = CENTI( 10.0f );
	glVertex3f( size, 0.0f, size );
	glVertex3f( size, 0.0f, -size );
	glVertex3f( -size, 0.0f, -size );
	glVertex3f( -size, 0.0f, size );
	glEnd( );

	glPopMatrix( );

	if( type == SHD_WIRE ){
		glEnable( GL_LIGHTING );
	}
}

// -----------------------------------------------------------------------------
// set triangle normal
// -----------------------------------------------------------------------------
void gfl::qt::Render::SetTriangleNormal(const gfl::core::Vector3& v0, const gfl::core::Vector3& v1, const gfl::core::Vector3& v2)
{
	gfl::core::Vector3 n;
	n.CalcNormal( v0, v1, v2 );
	glNormal3fv( ( f32 * ) & n );
}

// -----------------------------------------------------------------------------
// display axis
// -----------------------------------------------------------------------------
void gfl::qt::Render::Axis( const f32 size, const f32 line_size )
{
	gfl::core::Vector v[2];

	glDisable( GL_LIGHTING );
	glBegin( GL_LINES );
	// GRID AXIS
	glLineWidth( line_size );
	v[0].SetZero( );

	// X axis
	glColor3fv( _scColorTable[PCOL_RED] );
	v[1].Set( size, 0.0f, 0.0f );
	glVertex3fv( v[0].Get( ) );
	glVertex3fv( v[1].Get( ) );

	// Y axis
	glColor3fv( _scColorTable[PCOL_GREEN] );
	v[1].Set( 0.0f, size, 0.0f );
	glVertex3fv( v[0].Get( ) );
	glVertex3fv( v[1].Get( ) );

	// Z axis
	glColor3fv( _scColorTable[PCOL_BLUE] );
	v[1].Set( 0.0f, 0.0f, size );
	glVertex3fv( v[0].Get( ) );
	glVertex3fv( v[1].Get( ) );

	glEnd( );
	glEnable( GL_LIGHTING );
}

// -----------------------------------------------------------------------------
// display bone
// -----------------------------------------------------------------------------
void gfl::qt::Render::Bone( const f32 sx, const f32 sy, const f32 sz, const ShadingType type, const PolygonColor cotex_no )
{
	u8 i, j;
	gfl::core::Vector vert[6];

	vert[0].SetX( -0.2f * sx );
	vert[0].SetY( 0.0f );
	vert[0].SetZ( 0.0f );

	vert[1].SetX( 0.0f );
	vert[1].SetY( sy );
	vert[1].SetZ( 0.0f );

	vert[2].SetX( 0.0f );
	vert[2].SetY( 0.0f );
	vert[2].SetZ( -sz );

	vert[3].SetX( 0.0f );
	vert[3].SetY( -sy );
	vert[3].SetZ( 0.0f );

	vert[4].SetX( 0.0f );
	vert[4].SetY( 0.0f );
	vert[4].SetZ( sz );

	vert[5].SetX( sx );
	vert[5].SetY( 0.0f );
	vert[5].SetZ( 0.0f );

	switch( type ){
	case SHD_NORMAL:
		glCullFace( GL_BACK );
		glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, _scColorTable[cotex_no] );

		glBegin( GL_TRIANGLE_FAN );
		SetTriangleNormal( vert[0], vert[1], vert[2] );
		glVertex3fv( ( f32 * ) & vert[0] );
		glVertex3fv( ( f32 * ) & vert[1] );
		glVertex3fv( ( f32 * ) & vert[2] );
		SetTriangleNormal( vert[0], vert[2], vert[3] );
		glVertex3fv( ( f32 * ) & vert[3] );
		SetTriangleNormal( vert[0], vert[3], vert[4] );
		glVertex3fv( ( f32 * ) & vert[4] );
		SetTriangleNormal( vert[0], vert[4], vert[1] );
		glVertex3fv( ( f32 * ) & vert[1] );
		glEnd( );
#if 0
		DispNormal( ( f32 * ) & vert[0], ( f32 * ) & vert[1], ( f32 * ) & vert[2] );
		DispNormal( ( f32 * ) & vert[0], ( f32 * ) & vert[2], ( f32 * ) & vert[3] );
		DispNormal( ( f32 * ) & vert[0], ( f32 * ) & vert[3], ( f32 * ) & vert[4] );
		DispNormal( ( f32 * ) & vert[0], ( f32 * ) & vert[4], ( f32 * ) & vert[1] );
#endif
		glBegin( GL_TRIANGLE_FAN );
		SetTriangleNormal( vert[5], vert[4], vert[3] );
		glVertex3fv( ( f32 * ) & vert[5] );
		glVertex3fv( ( f32 * ) & vert[4] );
		glVertex3fv( ( f32 * ) & vert[3] );
		SetTriangleNormal( vert[5], vert[3], vert[2] );
		glVertex3fv( ( f32 * ) & vert[2] );
		SetTriangleNormal( vert[5], vert[2], vert[1] );
		glVertex3fv( ( f32 * ) & vert[1] );
		SetTriangleNormal( vert[5], vert[1], vert[4] );
		glVertex3fv( ( f32 * ) & vert[4] );
		glEnd( );
#if 0
		DispNormal( ( f32 * ) & vert[5], ( f32 * ) & vert[4], ( f32 * ) & vert[3] );
		DispNormal( ( f32 * ) & vert[5], ( f32 * ) & vert[3], ( f32 * ) & vert[2] );
		DispNormal( ( f32 * ) & vert[5], ( f32 * ) & vert[2], ( f32 * ) & vert[1] );
		DispNormal( ( f32 * ) & vert[5], ( f32 * ) & vert[1], ( f32 * ) & vert[4] );
#endif
		break;

	case SHD_WIRE:
		glDisable( GL_LIGHTING );
		glColor3fv( _scColorTable[cotex_no] );
		for( i = 0; i < 3; i++ ){
			glBegin( GL_LINE_LOOP );
			for( j = 0; j < 4; j++ ){
				glVertex3fv( ( f32 * ) & vert[_scBoneEdge[i][j]] );
			}
			glEnd( );
		}
		glEnable( GL_LIGHTING );
		break;
	}
}

// ---------------------------------------------------------------------------
// display ground
// ---------------------------------------------------------------------------
void gfl::qt::Render::Ground( const s32 count, const GroundType kind, const f32 ofs_x, const f32 ofs_z )
{
	gfl::core::Vector v[2];

	glDisable( GL_LIGHTING );
	glBegin( GL_LINES );
	// GRID AXIS

	f32 xs, xe, zs, ze;
	glColor3fv( _scColorTable[PCOL_BLUE] );
	const f32 size = METER( count );
	for( s32 i = -count; i <= count; ++i ){
		xs = xe = METER( ( f32 )i ) + ofs_x;
		zs = -size + ofs_z;
		ze = size + ofs_z;

		v[0].Set( xs, CENTI( 1.0f ), zs );
		v[1].Set( xe, CENTI( 1.0f ), ze );

		glVertex3fv( ( f32 * ) & v[0] );
		glVertex3fv( ( f32 * ) & v[1] );
	}
	glColor3fv( _scColorTable[PCOL_RED] );
	for( s32 i = -count; i <= count; ++i ){
		zs = ze = METER( ( f32 )i ) + ofs_z;
		xs = -size + ofs_x;
		xe = size + ofs_x;

		v[0].Set( xs, CENTI( 1.0f ), zs );
		v[1].Set( xe, CENTI( 1.0f ), ze );

		glVertex3fv( ( f32 * ) & v[0] );
		glVertex3fv( ( f32 * ) & v[1] );
	}

	glEnd( );

	// Disp Ground
	glCullFace( GL_BACK );
	glEnable( GL_CULL_FACE );
	xs = zs = -size;
	xe = ze = size;
	xs += ofs_x;
	xe += ofs_x;
	zs += ofs_z;
	ze += ofs_z;
	glColor3fv( ( kind ) ? _scColorTable[PCOL_DARKGREEN] : _scColorTable[PCOL_DARKGRAY] );
	glBegin( GL_QUADS );
	glVertex3f( xs, 0.0f, zs );
	glVertex3f( xs, 0.0f, ze );
	glVertex3f( xe, 0.0f, ze );
	glVertex3f( xe, 0.0f, zs );
	glEnd( );

	glEnable( GL_LIGHTING );
}

#if 0 //
// ---------------------------------------------------------------------------
// display axis
// ---------------------------------------------------------------------------
GLuint GLWidget::makeAxis( )
{
	GLuint list = glGenLists( 1 );
	glNewList( list, GL_COMPILE );

	DisplayAxis( 20.0f );

	glEndList( );
	return list;
}
#endif

void gfl::qt::Render::SetMaterialColor( const PolygonColor col )
{
	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, _scColorTable[col] );
}
void gfl::qt::Render::SetColor( const PolygonColor col )
{
	glColor3fv( _scColorTable[col] );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::qt::Render::Cube(const f32 size, const f32 line_size)
{
	static const GLfloat n[6][3] = {
		{-1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{1.0, 0.0, 0.0},
		{0.0, -1.0, 0.0},
		{0.0, 0.0, 1.0},
		{0.0, 0.0, -1.0}
	};
	static const GLint faces[6][4] = {
		{0, 1, 2, 3},
		{3, 2, 6, 7},
		{7, 6, 5, 4},
		{4, 5, 1, 0},
		{5, 6, 2, 1},
		{7, 4, 0, 3}
	};
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

	for( i = 5; i >= 0; i-- ){
		glBegin( GL_QUADS );
		glNormal3fv( &n[i][0] );
		glVertex3fv( &v[faces[i][0]][0] );
		glVertex3fv( &v[faces[i][1]][0] );
		glVertex3fv( &v[faces[i][2]][0] );
		glVertex3fv( &v[faces[i][3]][0] );
		glEnd( );
	}
}
