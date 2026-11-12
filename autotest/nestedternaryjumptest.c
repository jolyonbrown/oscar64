#include <assert.h>

// A nested conditional with int operands.  The inner "d ? 4 : 5" becomes
// two one-instruction blocks that BranchAccuAlternativeElimination turns
// into "LDA #1 / EOR #1 / ORA #4" followed by a jump to the common store.
// MergeBasicBlocks then appended the "ORA #4" block to its single
// predecessor, but copied the register data set of the block *after* it,
// so the trailing JMP was emitted as a never taken BEQ and the function
// fell off its end.
//
// f, fv and fs reproduce the bug (f and fv with -n and -n -O0, fs with
// -n -O0).  fl and fp are controls that never failed; fp(&a, &b) also
// takes the addresses of a and b, which keeps f and fv on full 16-bit
// compares at -O2, -Os and -O3, where the bug then shows as well.

__noinline int f(int c, int d)
{
	return c ? 3 : (d ? 4 : 5);
}

__noinline unsigned long fl(unsigned long c, unsigned long d)
{
	return c ? 3 : (d ? 4 : 5);
}

__noinline int fp(int * c, int * d)
{
	return *c ? 3 : (*d ? 4 : 5);
}

__noinline int fv(volatile int c, volatile int d)
{
	return c ? 3 : (d ? 4 : 5);
}

static int fs(int c, int d)
{
	return c ? 3 : (d ? 4 : 5);
}

int a = 0, b = 1;

int main(void)
{
	assert(f(a, b) == 4);
	assert(f(a, a) == 5);
	assert(f(b, a) == 3);
	assert(f(b, b) == 3);

	assert(fl(a, b) == 4);
	assert(fl(a, a) == 5);
	assert(fl(b, a) == 3);
	assert(fl(b, b) == 3);

	assert(fp(&a, &b) == 4);
	assert(fp(&a, &a) == 5);
	assert(fp(&b, &a) == 3);
	assert(fp(&b, &b) == 3);

	assert(fv(a, b) == 4);
	assert(fv(a, a) == 5);
	assert(fv(b, a) == 3);
	assert(fv(b, b) == 3);

	assert(fs(0, 1) == 4);
	assert(fs(0, 0) == 5);
	assert(fs(1, 0) == 3);
	assert(fs(1, 1) == 3);

	return 0;
}
