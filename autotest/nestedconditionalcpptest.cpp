#include <assert.h>

// C++ shares the parser: the middle operand of ?: is a full expression here too

struct V
{
	int v;
	V(int x) : v(x) {}
};

static volatile int vz;

static const char * pick(int a, int b, int c)
{
	return a ? b ? "x" : "y" : c ? "z" : "w";
}

static V pickv(int a, int b, const V & x, const V & y, const V & z)
{
	return a ? b ? x : y : z;
}

static int one(int x)
{
	return x;
}

static int folded_comma_only_argument(void)
{
	return one(1 ? vz, 2 : 3);
}

template<int N> int tf(void)
{
	return N ? N > 1 ? 2 : 1 : 0;
}

constexpr int ce(int a, int b)
{
	return a ? b ? 1 : 2 : 3;
}

template<int N> struct T
{
	static const int value = N;
};

int main(void)
{
	volatile int va = 1, vb = 0;
	int x = 1, y = 2;

	// nested conditional in the middle operand
	assert(pick(1, 1, 1)[0] == 'x');
	assert(pick(1, 0, 1)[0] == 'y');
	assert(pick(0, 1, 1)[0] == 'z');
	assert(pick(0, 1, 0)[0] == 'w');

	V a(1), b(2), c(3);
	assert(pickv(1, 1, a, b, c).v == 1);
	assert(pickv(1, 0, a, b, c).v == 2);
	assert(pickv(0, 1, a, b, c).v == 3);

	// assignment and comma in the middle operand
	int w = va ? x = 4, y : 0;
	assert(w == 2 && x == 4);
	w = vb ? x = 5, y : 0;
	assert(w == 0 && x == 4);

	assert(folded_comma_only_argument() == 2);

	// constant expressions
	assert(tf<0>() == 0 && tf<1>() == 1 && tf<5>() == 2);
	assert(T<ce(0, 1)>::value == 3);
	assert(T<ce(1, 0)>::value == 2);
	assert(T<ce(1, 1)>::value == 1);

	bool bb = va ? vb ? false : true : false;
	assert(bb);

	return 0;
}
