#include <assert.h>

// Braced initialization of a scalar in C++ goes through the same two parser
// paths as the C form: default member initializers, member initializer
// lists, braced call arguments, functional casts and static const members.

struct A
{
	int		x = { 3 };
	char	c;
	long	l;

	A(int v) : c{'q'}, l{v + 1L} {}
};

struct B
{
	static const int	s = { 4 };
};

int f(int a, long b)
{
	return a + (int)b;
}

template<class T> T make(T v)
{
	T	x = { v };
	return x;
}

int		g = { 7 };
int		gk = 6;

int main(void)
{
	A		a(41);
	int		k = { 5 };
	int		m = int{gk} + 1;
	long	n = long{7};

	assert(a.x == 3 && a.c == 'q' && a.l == 42L);
	assert(B::s == 4);
	assert(f({3}, {4L}) == 7);
	assert(make<int>(3) == 3 && make<long>(70000L) == 70000L && make<char>('a') == 'a');
	assert(g == 7);
	assert(k == 5 && m == 7 && n == 7);

	return 0;
}
