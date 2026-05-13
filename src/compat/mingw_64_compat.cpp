#include <setjmp.h>

/**
 * This provides the physical symbol that libpng.a is looking for.
 * We do NOT use 'inline' here because the pre-compiled library
 * needs an external symbol to latch onto.
 */
#ifdef __MINGW64__
extern "C" int _setjmpex(jmp_buf, void*);

extern "C" void* __intrinsic_setjmpex(jmp_buf buf, void* frame) {
	_setjmpex(buf, frame);
	return frame;
}
#endif