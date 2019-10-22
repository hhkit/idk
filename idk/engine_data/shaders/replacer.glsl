// This segment is hard coded into OpenGL::Program.cpp
// This file is left here as an example to HC and other GLSL programmers
//   on the defines used by the program.

#ifdef OGL
#define U_LAYOUT(SET, BIND) 
#define BLOCK(X) struct X
#endif
#ifdef VULKAN
#define U_LAYOUT(SET, BIND) layout(set = SET, binding = BIND) 
#define BLOCK(X) X
#endif
