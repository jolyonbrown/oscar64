#include <assert.h>

// Pointers compared with the integer constant 0, which C treats as a null
// pointer constant.  With the global optimizer a never written pointer is
// replaced by its initializer and the comparison is folded, so the 0 must
// be a null pointer constant as well, or the fold compares an integer with
// a pointer and gets it wrong.  Oscar64 rejects the comparison in C++ mode,
// so this is a C only test.

struct S { int x; };

enum { ZERO, ONE };

char * p = 0;
char * q = (char *)0;
void * v = (void *)0;
struct S * s = (struct S *)0;
void (* hook)(void) = 0;

char c;
char * n = &c;
char * hi = (char *)0x8000;
char * w = 0;
char * volatile live = 0;

void set(char * x)
{
	w = x;
}

char * z(void)
{
	return 0;
}

int main(void)
{
	static char * l = 0;
	int r, k;

	assert(p == 0);
	assert(!(p != 0));
	assert(0 == p);
	assert(!(0 != p));

	assert(q == 0);
	assert(v == 0);
	assert(s == 0);
	assert(hook == 0);
	assert(l == 0);

	// Other spellings of the null pointer constant
	assert(p == 0L);
	assert(p == 0u);
	assert(0UL == p);
	assert(p == 0x0);
	assert(p == ZERO);
	assert(p == (2 - 2));
	assert(p == (int)0);
	assert(p == sizeof(char) - 1);

	assert((char *)0 == 0);
	assert(!((char *)0 != 0));
	assert(0 == (char *)0);

	assert(z() == 0);
	assert(0 == z());

	if (p != 0)
		assert(0);
	if (!(p == 0))
		assert(0);

	r = (p == 0);
	assert(r == 1);
	r = p != 0 ? 1 : 0;
	assert(r == 0);
	assert(p == 0 && q == 0);
	assert(!(p != 0 || q != 0));

	k = 0;
	while (p != 0 && k < 3)
		k++;
	assert(k == 0);

	// Logical not of a pointer used as a value
	r = !p;
	assert(r == 1);
	r = !(char *)0;
	assert(r == 1);
	r = !n;
	assert(r == 0);

	// A pointer initialised with the address of an object is not null,
	// whichever side the 0 is on
	assert(n != 0);
	assert(!(n == 0));
	assert(0 != n);
	assert(!(0 == n));

	// Ordered comparison of a pointer with 0 is an extension; pointers
	// compare unsigned, so an address at or above 0x8000 is above 0
	assert(p <= 0);
	assert(p >= 0);
	assert(hi > 0);
	assert(0 < hi);
	assert(0 <= hi);
	assert(!(0 > hi));
	assert(!(hi < 0));

	// Pointers written at run time
	set(&c);
	assert(w != 0);
	assert(0 != w);
	set(0);
	assert(w == 0);
	assert(0 == w);

	assert(live == 0);
	assert(0 == live);
	assert(!(live != 0));
	assert(!(0 != live));
	live = &c;
	assert(live != 0);
	assert(0 != live);
	live = (char *)0xd000;
	assert(0 < live);
	assert(live > 0);
	assert(!(0 >= live));

	return 0;
}
