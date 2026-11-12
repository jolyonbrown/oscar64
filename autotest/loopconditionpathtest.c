#include <assert.h>

// A loop body with a long chain of independent conditionals.  The
// number of paths through the chain doubles with every conditional,
// and three walks over the predecessor graph used to visit a block
// once per path instead of once: CollectBlocksToDominator (hoisting
// common conditional paths), IsDirectLoopPathBlock (moving conditions
// out of loops) and CheckLoopIndexXRegisters with its twin
// CheckLoopIndexYRegisters (reducing index register shuffles).
// NetHack's explode() compiled this way grew the compiler to several
// gigabytes within seconds.
//
// f reaches the first two: a short path beside a chain of calls.
// g reaches CheckLoopIndexXRegisters: the loop keeps the high byte of
// its int count in X across the chain.  h reaches
// CheckLoopIndexYRegisters: the loop counts in a byte and keeps a
// condition of the chain in Y (without optimization in X, which
// reaches CheckLoopIndexXRegisters again); the store after its loop
// is needed for that with v1.32.273.  The addresses of f, g and h are
// taken, so the arguments stay variables and the chains are not folded
// away by constant parameter propagation.
//
// The chains are sized so that a compiler without the fix still
// finishes: f then costs about half a gigabyte of memory, g and h
// compile time, some seconds to tens of seconds each per native
// configuration.  With the fix the file needs about 40 megabytes and
// about a second.

int		acc;
unsigned long	mask;
char		last;

__noinline void bump(int v)
{
	acc += v;
}

void f(int x, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (x == 0)
			bump(100);
		else
		{
			if (mask & 0x000001ul) bump(1);
			if (mask & 0x000002ul) bump(2);
			if (mask & 0x000004ul) bump(3);
			if (mask & 0x000008ul) bump(4);
			if (mask & 0x000010ul) bump(5);
			if (mask & 0x000020ul) bump(6);
			if (mask & 0x000040ul) bump(7);
			if (mask & 0x000080ul) bump(8);
			if (mask & 0x000100ul) bump(9);
			if (mask & 0x000200ul) bump(10);
			if (mask & 0x000400ul) bump(11);
			if (mask & 0x000800ul) bump(12);
			if (mask & 0x001000ul) bump(13);
			if (mask & 0x002000ul) bump(14);
			if (mask & 0x004000ul) bump(15);
			if (mask & 0x008000ul) bump(16);
			if (mask & 0x010000ul) bump(17);
			if (mask & 0x020000ul) bump(18);
			if (mask & 0x040000ul) bump(19);
			if (mask & 0x080000ul) bump(20);
			if (mask & 0x100000ul) bump(21);
			if (mask & 0x200000ul) bump(22);
			if (mask & 0x400000ul) bump(23);
			if (mask & 0x800000ul) bump(24);
		}
	}
}

void g(int x, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (x == 0)
			acc += 100;
		else
		{
			if (mask & 0x00001ul) acc += 1;
			if (mask & 0x00002ul) acc += 2;
			if (mask & 0x00004ul) acc += 3;
			if (mask & 0x00008ul) acc += 4;
			if (mask & 0x00010ul) acc += 5;
			if (mask & 0x00020ul) acc += 6;
			if (mask & 0x00040ul) acc += 7;
			if (mask & 0x00080ul) acc += 8;
			if (mask & 0x00100ul) acc += 9;
			if (mask & 0x00200ul) acc += 10;
			if (mask & 0x00400ul) acc += 11;
			if (mask & 0x00800ul) acc += 12;
			if (mask & 0x01000ul) acc += 13;
			if (mask & 0x02000ul) acc += 14;
			if (mask & 0x04000ul) acc += 15;
			if (mask & 0x08000ul) acc += 16;
			if (mask & 0x10000ul) acc += 17;
			if (mask & 0x20000ul) acc += 18;
		}
	}
}

void h(char n)
{
	for (char i = 0; i < n; i++)
	{
		if (mask & 0x00001ul) acc += 1;
		if (mask & 0x00002ul) acc += 2;
		if (mask & 0x00004ul) acc += 3;
		if (mask & 0x00008ul) acc += 4;
		if (mask & 0x00010ul) acc += 5;
		if (mask & 0x00020ul) acc += 6;
		if (mask & 0x00040ul) acc += 7;
		if (mask & 0x00080ul) acc += 8;
		if (mask & 0x00100ul) acc += 9;
		if (mask & 0x00200ul) acc += 10;
		if (mask & 0x00400ul) acc += 11;
		if (mask & 0x00800ul) acc += 12;
		if (mask & 0x01000ul) acc += 13;
		if (mask & 0x02000ul) acc += 14;
		if (mask & 0x04000ul) acc += 15;
		if (mask & 0x08000ul) acc += 16;
		if (mask & 0x10000ul) acc += 17;
		if (mask & 0x20000ul) acc += 18;
	}

	last = n;
}

void (*fp)(int, int) = f;
void (*gp)(int, int) = g;
void (*hp)(char) = h;

int main(void)
{
	mask = 0x0f0f0ful;

	f(0, 2);
	assert(acc == 200);

	fp(1, 1);
	assert(acc == 200 + (1 + 2 + 3 + 4) + (9 + 10 + 11 + 12) + (17 + 18 + 19 + 20));

	mask = 0xf0f0f0ul;
	fp(1, 2);
	assert(acc == 326 + 2 * ((5 + 6 + 7 + 8) + (13 + 14 + 15 + 16) + (21 + 22 + 23 + 24)));

	acc = 0;
	mask = 0x0f0f0ful;

	g(0, 2);
	assert(acc == 200);

	gp(1, 1);
	assert(acc == 200 + (1 + 2 + 3 + 4) + (9 + 10 + 11 + 12) + (17 + 18));

	mask = 0xf0f0f0ul;
	gp(1, 2);
	assert(acc == 287 + 2 * ((5 + 6 + 7 + 8) + (13 + 14 + 15 + 16)));

	acc = 0;
	mask = 0x0f0f0ful;

	h(1);
	assert(acc == (1 + 2 + 3 + 4) + (9 + 10 + 11 + 12) + (17 + 18));
	assert(last == 1);

	hp(0);
	assert(acc == 87);
	assert(last == 0);

	mask = 0xf0f0f0ul;
	hp(2);
	assert(acc == 87 + 2 * ((5 + 6 + 7 + 8) + (13 + 14 + 15 + 16)));
	assert(last == 2);

	return 0;
}
