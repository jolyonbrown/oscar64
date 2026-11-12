#include <assert.h>

// A scalar initializer may be a single expression enclosed in braces
// (C89 3.5.7, C99 6.7.8p11), with an optional trailing comma.  The same
// parser paths handle scalar compound literals (C99 6.5.2.5).

int		gi = { 3 };
char	gc = { 'A' };
long	gl = { 100000L };
float	gf = { 1.5 };
int		garr[4] = { 1, 2, 3, 4 };
int	*	gp = { garr + 1 };
const char* gs = { 0 };
int		gtc = { 4, };
static int	sgi = { -7 };
static const char* sgs = { 0 };
static long	sgl = { 123456L };
int	*	gcl = &(int){ 5 };

struct S { int a; char b; long c; };
struct T { int a[2]; int b; };

struct S	gst = { { 3 }, { 'x' }, { 70000L } };
struct S	gse = { 1, { 2 }, 3 };
struct T	gtt = { 1, 2, { 3 } };
int			gba[3] = { { 1 }, { 2 }, { 3 } };

volatile int	source = 7;

int counter(void)
{
	static int	sl = { 11 };
	return sl++;
}

long lcounter(void)
{
	static long	sl = { -5L };
	return sl--;
}

int twice(int a)
{
	return 2 * a;
}

int main(void)
{
	int		li = { 5 };
	char	lc = { 'z' };
	long	ll = { -300000L };
	float	lf = { 2.5 };
	int	*	lp = { garr + 2 };
	const char* ls = { 0 };
	int		ltc = { 6, };
	int		k = twice(li);
	int		lr = { twice(k) };
	int		lcm = { (1, 2) }, lcm2 = { 3, };
	int		lonce = { source++ };
	int		lcl = (int){ k + 1 };
	int	*	lclp = &(int){ 3 };
	struct S	lst = { { 9 }, { 'q' }, { 123456L } };
	struct S	lse = { 1, { 2 }, 3 };
	struct T	ltt = { { 1, 2 }, { 3 } };
	int			lba[3] = { { 7 }, { 8 }, { 9 } };
	const char* t;

	assert(gi == 3);
	assert(gc == 'A');
	assert(gl == 100000L);
	assert(gf == 1.5);
	assert(gp == garr + 1 && *gp == 2);
	// Compared through a local copy: a file-scope pointer initialized to 0
	// and compared with 0 directly is a separate, pre-existing problem at
	// -O2/-Os/-O3 that this test does not cover.
	t = gs;
	assert(t == 0);
	t = sgs;
	assert(t == 0);
	assert(gtc == 4);
	assert(sgi == -7);
	assert(sgl == 123456L);
	assert(*gcl == 5);
	assert(gst.a == 3 && gst.b == 'x' && gst.c == 70000L);
	assert(gse.a == 1 && gse.b == 2 && gse.c == 3);
	assert(gtt.a[0] == 1 && gtt.a[1] == 2 && gtt.b == 3);
	assert(gba[0] == 1 && gba[1] == 2 && gba[2] == 3);

	assert(counter() == 11);
	assert(counter() == 12);
	assert(lcounter() == -5L);
	assert(lcounter() == -6L);

	assert(li == 5);
	assert(lc == 'z');
	assert(ll == -300000L);
	assert(lf == 2.5);
	assert(lp == garr + 2 && *lp == 3);
	t = ls;
	assert(t == 0);
	assert(ltc == 6);
	assert(k == 10);
	assert(lr == 20);
	assert(lcm == 2 && lcm2 == 3);
	assert(lonce == 7 && source == 8);
	assert(lcl == 11);
	assert(*lclp == 3);
	*lclp = 4;
	assert(*lclp == 4);
	assert(lst.a == 9 && lst.b == 'q' && lst.c == 123456L);
	assert(lse.a == 1 && lse.b == 2 && lse.c == 3);
	assert(ltt.a[0] == 1 && ltt.a[1] == 2 && ltt.b == 3);
	assert(lba[0] == 7 && lba[1] == 8 && lba[2] == 9);

	return 0;
}
