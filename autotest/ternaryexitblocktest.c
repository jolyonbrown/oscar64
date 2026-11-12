#include <assert.h>

// Functions whose exit block holds a conditional that is still a
// CMP/LDA #0/ADC #$ff/AND/EOR value select when ExpandADCToBranch turns it
// into a branch and splits the block.  The remaining instructions,
// including the temporary RTS, move to a new block behind one arm of the
// branch, but the exit block role was handed to that arm.  The RTS was
// then removed from the wrong block and the function returned early
// without its stores, until 83e805b, which made the final MergeBasicBlocks
// dereference a null successor instead, so the compiler crashed.
//
// For f the select survives to that point with -n -O0 only; for g, with
// the int select, it survives at every optimization level, so g fails
// with -n at every level.

unsigned char ga, gb;
int gi;

__noinline unsigned char f(unsigned char p0, unsigned char p1)
{
	unsigned char r = p1;
	if (r) ga = 8; else ga = 5;
	gb = p0 ? 16 : 1;
	return r;
}

__noinline void g(unsigned char p0, unsigned char p1)
{
	if (p0) ga = 2; else ga = 4;
	gb = (p0 >> 2) & 1;
	gi = p1 ? 242 : -127;
}

unsigned char z = 0, o = 1, v = 4;

int main(void)
{
	assert(f(z, o) == 1 && ga == 8 && gb == 1);
	assert(f(o, z) == 0 && ga == 5 && gb == 16);
	assert(f(o, o) == 1 && ga == 8 && gb == 16);
	assert(f(z, z) == 0 && ga == 5 && gb == 1);

	g(z, z); assert(ga == 4 && gb == 0 && gi == -127);
	g(o, o); assert(ga == 2 && gb == 0 && gi == 242);
	g(v, z); assert(ga == 2 && gb == 1 && gi == -127);
	g(z, v); assert(ga == 4 && gb == 0 && gi == 242);

	return 0;
}
