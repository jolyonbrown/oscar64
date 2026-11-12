#include <assert.h>

// Overload resolution with union arguments ranks like it does with struct
// arguments: a pointer or reference that only gains a qualifier is a better
// match than a conversion to const void*, to an ellipsis or to an integer.
// A union with a user declared copy constructor is copied by calling it.

union U
{
	int		i;
	char	c;
};

struct S
{
	int		i;
	char	c;
};

static int f(const U* p)
{
	return 1;
}

static int f(const void* p)
{
	return 2;
}

static int f(const S* p)
{
	return 3;
}

static int g(const U& r)
{
	return 1;
}

static int g(int k)
{
	return 2;
}

static int g(const S& r)
{
	return 3;
}

static int h(const U* p)
{
	return 1;
}

static int h(...)
{
	return 2;
}

union V
{
	int		i;
	char	c;

	V(int k)
	{
		i = k;
	}

	V(const V& o)
	{
		i = o.i + 1;
	}
};

int main(void)
{
	U u;
	S s;
	u.i = 7;
	s.i = 8;

	assert(f(&u) == 1);
	assert(f(&s) == 3);
	assert(g(u) == 1);
	assert(g(s) == 3);
	assert(g(5) == 2);
	assert(h(&u) == 1);

	const U* cp = &u;
	U* mp = &u;
	const U& cr = u;
	assert(cp == mp);
	assert(!(cp != mp));
	assert(cr.i == 7);
	assert(g(cr) == 1);

	V a(5);
	V b(a);
	assert(b.i == 6);

	return 0;
}
