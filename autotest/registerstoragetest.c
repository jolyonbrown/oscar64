#include <assert.h>

// Exercise register storage-class declarations (C89 3.5.1, C99 6.7.1)
// for locals and function parameters; the allocation hint may be ignored.
// The keyword must not break the zero page register pragma, which has
// the same name, nor the preprocessor's handling of a macro named after it.

#pragma register(__tmpy, 0x02)

typedef unsigned short ushort;

struct point
{
	int	x, y;
};

static int sum_bytes(register const unsigned char * p, register unsigned char n)
{
	register int	s = 0;

	while (n--)
		s += *p++;

	return s;
}

static int manhattan(register struct point * pt)
{
	register int	dx = pt->x;
	register int	dy = pt->y;

	return dx + dy;
}

static ushort scale(register ushort v, const register ushort f)
{
	return v * f;
}

static long square(register long v)
{
	return v * v;
}

static float half(register float f)
{
	return f * 0.5;
}

static int count_nonzero(register const char * s)
{
	register int	n = 0;
	register char	c;

	while ((c = *s++) != 0)
	{
		if (c != '0')
			n++;
	}

	return n;
}

// Prototype and definition may differ in the storage class
static int add_one(register int v);

static int add_one(int v)
{
	return v + 1;
}

static int apply(int (*fn)(register int), register int v)
{
	return fn(v);
}

static int sum_to(int n)
{
	register int	s = 0;

	while (n > 0)
	{
		register int	t = n * 2;
		s += t;
		n--;
	}

	return s;
}

// Defined after main, so the macros below cannot hide the uses above
// from a preprocessor that expands them
static int defined_away(register int a, register int b);

int main(void)
{
	register int			i, s = 0;
	register unsigned char	c = 200;
	register const char	*	text = "register";
	register volatile int	v = 0;
	static const unsigned char	bytes[] = {1, 2, 3, 4, 5};
	struct point			pt = {3, 4};
	int						tv = 5;
	register int	*	const	p = &tv;

	for (i = 0; i < 10; i++)
		s += i;
	assert(s == 45);

	c += 100;
	assert(c == 44);

	assert(text[0] == 'r' && text[7] == 'r');

	v = v + 2;
	assert(v == 2);
	assert(*p == 5);

	assert(sum_bytes(bytes, 5) == 15);
	assert(manhattan(&pt) == 7);
	assert(scale(300, 100) == 30000);
	assert(square(300L) == 90000L);
	assert(half(3.0) == 1.5);
	assert(count_nonzero("10203") == 3);
	assert(add_one(41) == 42);
	assert(apply(add_one, 1) == 2);
	assert(sum_to(3) == 12);
	assert(defined_away(10, 3) == 7);

	for (register int k = 0; k < 4; k++)
		s -= k;
	assert(s == 39);

	{
		register ushort	u = 65535;
		u++;
		assert(u == 0);
	}

	switch (s)
	{
	case 39:
	{
		register int	t = s * 2;
		assert(t == 78);
		break;
	}
	default:
		assert(0);
	}

	return 0;
}

// Code written for debuggers defines the word away; the keyword must
// still be accepted with the macro in place
#define register

static int defined_away(register int a, register int b)
{
	register int	d = a - b;

	return d;
}

// A replacement list for the keyword must not leak into the program
// text; this one is never used and would not parse if it did
#undef register
#define register )
