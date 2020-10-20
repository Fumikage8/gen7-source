// sysdef2.gsh (Geometry Shader)
#version 150
#extension GL_EXT_geometry_shader4 : enable

#define varying     out

#define VertexMax 3

layout(triangles) in;
layout(triangle_strip, max_vertices= VertexMax) out;

const int TextureMax = 4;

in vec4 vs_outPos[VertexMax];
in vec4 vs_outUv[VertexMax][TextureMax];
in vec4 vs_outColor[VertexMax];
in vec3 vs_outObjNormal[VertexMax];
in vec3 vs_outObjLight[VertexMax];
in vec3 vs_outObjTangentVec[VertexMax][TextureMax];
in vec4 vs_outView[VertexMax];
in vec4 vs_outDebugVec[VertexMax];
in vec4 vs_outRaram[VertexMax];

varying vec4 outPos;
varying vec4 outUv[TextureMax];
varying vec4 outColor;
varying vec3 outObjNormal;
varying vec3 outObjLight;
varying vec3 outObjTangentVec[TextureMax];
varying vec4 outView;
varying vec4 outDebugVec;
varying vec4 outRaram;

uniform bool		cGsDrawUvSpace = false;

void main(void)
{
#if 1
	if ( cGsDrawUvSpace )
	{
		vec4			pos[VertexMax];

		for ( int i=0; i < VertexMax; i++ )
		{
			pos[i] = gl_in[i].gl_Position;
		}

		float			minX = pos[0].x;
		float			minY = pos[0].y;
		float			maxX = pos[0].x;
		float			maxY = pos[0].y;

		for ( int i = 1; i < VertexMax; i++ )
		{
			minX = min( pos[i].x, minX );
			minY = min( pos[i].y, minY );
			maxX = max( pos[i].x, maxX );
			maxY = max( pos[i].y, maxY );
		}

		float				shiftX = 0.0;
		float				shiftY = 0.0;

		if ( minX < 0.0f )
		{
			int			intX = int( -minX );
			intX ++;
			float		floatX = float(intX);

			shiftX += floatX;
		}

		if ( maxX > 1.0f )
		{
			int			intX = int( maxX );
			float		floatX = float(intX);

			shiftX -= floatX;
		}

		if ( minY < 0.0f )
		{
			int			intY = int( -minY );
			intY ++;
			float		floatY = float(intY);

			shiftY += floatY;
		}

		if ( maxY > 1.0f )
		{
			int			intY = int( maxY );
			float		floatY = float(intY);

			shiftY -= floatY;
		}

		for ( int i=0; i < VertexMax; i++ )
		{
			pos[i].x += shiftX;
			pos[i].y += shiftY;

			pos[i].xy -= 0.5;
			pos[i].xy *= 2.0;
			pos[i].z = 0.0;
			pos[i].w = 1.0;

			gl_Position = pos[i];
			outPos = vs_outPos[i];
			outColor = vs_outColor[i];
			outObjNormal = vs_outObjNormal[i];
			outObjLight = vs_outObjLight[i];
			outView = vs_outView[i];
			outDebugVec = vs_outDebugVec[i];
			outRaram = vs_outRaram[i];

			for( int texNo = 0; texNo < TextureMax; texNo++ )
			{
				outUv[texNo] = vs_outUv[i][texNo];
				outObjTangentVec[texNo] = vs_outObjTangentVec[i][texNo];
			}

			EmitVertex();    
		}
		EndPrimitive();
	}
	else
#endif
	{
		// ‰½‚à‚¹‚¸‚»‚Ì‚Ü‚Üo—Í
		for ( int i=0; i < VertexMax; i++ )
		{
			gl_Position = gl_in[i].gl_Position;

			outPos = vs_outPos[i];
			outColor = vs_outColor[i];
			outObjNormal = vs_outObjNormal[i];
			outObjLight = vs_outObjLight[i];
			outView = vs_outView[i];
			outDebugVec = vs_outDebugVec[i];
			outRaram = vs_outRaram[i];

			for( int texNo = 0; texNo < TextureMax; texNo++ )
			{
				outUv[texNo] = vs_outUv[i][texNo];
				outObjTangentVec[texNo] = vs_outObjTangentVec[i][texNo];
			}

			EmitVertex();    
		}
		EndPrimitive();
	}
}

