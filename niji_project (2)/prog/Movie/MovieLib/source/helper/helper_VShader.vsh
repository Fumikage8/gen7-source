// Input registers map
#define InputPosition    v0
#define InputTexCoord   v1

// Output registers map
#define OutputPosition    o0
#define OutputTexCoord    o1

#define ProjectionMatrix    c0
#define ModelViewMatrix        c4
 
#pragma bind_symbol(InputPosition,v0)
#pragma bind_symbol(InputTexCoord.xy,v1)
#pragma bind_symbol(ProjectionMatrix,c0,c3)
#pragma bind_symbol(ModelViewMatrix,c4,c7)

#pragma output_map (position,  o0 )
#pragma output_map (texture0, o1.xy)

main:

    mov     OutputTexCoord,        InputTexCoord.xy
    
    m4x4    r0,                    InputPosition,    ModelViewMatrix
    
    dp4     OutputPosition.x,    r0,                ProjectionMatrix[0]
    dp4     OutputPosition.y,    r0,                ProjectionMatrix[1]
    dp4     OutputPosition.z,    r0,                ProjectionMatrix[2]
    dp4     OutputPosition.w,    r0,                ProjectionMatrix[3]
    
    end
endmain:

