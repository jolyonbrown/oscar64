#include <assert.h>

// Function pointer members inherited from a base class, in a global object:
// assignment, call, and the call being registered (the caller keeps locals
// live across it, the callee uses a helper with its own locals; the operands
// stay small so that no intermediate result overflows an int)

__noinline int helper(int a, int b, int c, int d)
{
	int t[4];
	t[0] = a * 3; t[1] = b * 5; t[2] = c * 7; t[3] = d * 11;
	return t[0] + t[1] + t[2] + t[3];
}

__noinline int callee(int a, int b)
{
	int u = a + 10, v = b + 20, s = u * v;
	return helper(u, v, s, a ^ b) + s;
}

__noinline int leaf(int a, int b)
{
	return a * 2 + b;
}

struct Base
{
	int		pad;
	int		(*fn)(int a, int b);
};

struct Middle : Base
{
	char	c;
};

struct Derived : Middle
{
	int		extra;
	int		(*fn2)(int a, int b);
};

Derived	gd;

__noinline int caller(int x, int y)
{
	int l1 = x * 13, l2 = y * 17, l3 = x * y + 19, l4 = x - y;
	int r = gd.fn(x, y);
	return l1 + l2 + l3 + l4 + r;
}

__noinline int caller2(int x, int y)
{
	int l1 = x * 13, l2 = y * 17, l3 = x * y + 19, l4 = x - y;
	int r = gd.fn2(x, y);
	return l1 + l2 + l3 + l4 + r;
}

int ref_caller(int x, int y)
{
	int l1 = x * 13, l2 = y * 17, l3 = x * y + 19, l4 = x - y;
	int u = x + 10, v = y + 20, s = u * v;
	int r = (u * 3 + v * 5 + s * 7 + (x ^ y) * 11) + s;
	return l1 + l2 + l3 + l4 + r;
}

int main(void)
{
	gd.fn = leaf;
	gd.fn2 = leaf;
	gd.pad = 1; gd.c = 'c'; gd.extra = 2;
	assert(gd.fn(3, 4) == 10 && (*gd.fn2)(3, 4) == 10);
	assert(gd.pad == 1 && gd.c == 'c' && gd.extra == 2);

	Base * b = &gd;
	assert(b->fn(3, 4) == 10);
	b->fn = callee;

	gd.fn2 = callee;
	for (int i = 0; i < 5; i++)
	{
		assert(caller(i, 2 * i + 1) == ref_caller(i, 2 * i + 1));
		assert(caller2(i, 2 * i + 1) == ref_caller(i, 2 * i + 1));
	}

	return 0;
}
