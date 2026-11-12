#include <assert.h>

// Function pointer members of global struct variables, cases not run with -Ofp:
// the address of a member or of an element of an array member is a constant,
// usable in a static initializer, multi dimensional arrays of function
// pointers (-Ofp tracks only one array level), and members of anonymous
// unions (not tracked by -Ofp)

int add1(int x)
{
	return x + 1;
}

int add2(int x)
{
	return x + 2;
}

struct procs
{
	int		pad;
	int		(*fn)(int x);
	int		(*arr[2])(int x);
};

struct procs		gp;
static struct procs	sp;

struct nested
{
	char			c;
	struct procs	p;
}	np;

struct t2
{
	int		pad;
	int		(*fn[2][2])(int x);
}	w2;

int	(*tab[2][2])(int x);

// a call through a member of an anonymous union, or through a variable index
// into a two dimensional array member, must be registered as a call: the
// caller keeps locals live across it, and the callee uses a helper with its
// own locals. The operands stay small so that no intermediate result
// overflows an int
struct procsu
{
	int		pad;
	union
	{
		int		(*fn)(int a, int b);
		void *	vp;
	};
}	cwu;

struct procs2
{
	int		pad;
	int		(*fn[2][2])(int a, int b);
}	cw2;

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

__noinline int caller_union(int x, int y)
{
	int l1 = x * 13, l2 = y * 17, l3 = x * y + 19, l4 = x - y;
	int r = cwu.fn(x, y);
	return l1 + l2 + l3 + l4 + r;
}

__noinline int caller_2d(int i, int x, int y)
{
	int l1 = x * 13, l2 = y * 17, l3 = x * y + 19, l4 = x - y;
	int r = cw2.fn[i][1 - i](x, y);
	return l1 + l2 + l3 + l4 + r;
}

int ref_caller(int x, int y)
{
	int l1 = x * 13, l2 = y * 17, l3 = x * y + 19, l4 = x - y;
	int u = x + 10, v = y + 20, s = u * v;
	int r = (u * 3 + v * 5 + s * 7 + (x ^ y) * 11) + s;
	return l1 + l2 + l3 + l4 + r;
}

int	(**gpp)(int) = &gp.fn;
int	(**spp)(int) = &sp.fn;
int	(**npp)(int) = &np.p.fn;

int	(**ap0)(int) = gp.arr;
int	(**ap1)(int) = &gp.arr[1];
int	(**ap2)(int) = sp.arr + 1;
int	(*(*app)[2])(int) = &gp.arr;
int	(**ap10)(int) = &w2.fn[1][0];

int main(void)
{
	gp.fn = add1;
	sp.fn = add2;
	np.p.fn = add2;

	assert((*gpp)(1) == 2);
	assert((*spp)(1) == 3);
	assert((*npp)(1) == 3);

	assert(gpp == &gp.fn);
	assert(npp == &np.p.fn);
	assert((char *)gpp == (char *)&gp + sizeof(int));

	*gpp = add2;
	assert(gp.fn(1) == 3);

	int	(**lpp)(int) = &sp.fn;
	*lpp = add1;
	assert(sp.fn(1) == 2);

	// elements of an array member
	assert(ap0 == &gp.arr[0]);
	assert(ap1 == ap0 + 1);
	assert(ap2 == &sp.arr[1]);
	assert((char *)ap1 == (char *)&gp + sizeof(int) + 2 * sizeof(gp.fn));
	*ap0 = add1;
	*ap1 = add2;
	assert(gp.arr[0](1) == 2 && gp.arr[1](1) == 3);
	assert((*app)[1](1) == 3);
	*ap2 = add2;
	assert(sp.arr[1](1) == 3);

	// static local
	static struct procs		ls;
	static int (**lsp)(int) = &ls.fn;
	static int (**lsa)(int) = &ls.arr[1];
	ls.fn = add1;
	ls.arr[1] = add2;
	assert((*lsp)(1) == 2);
	assert((*lsa)(1) == 3);
	assert(ls.fn(2) == 3 && ls.arr[1](2) == 4);

	// two dimensional arrays
	w2.fn[0][0] = add1;
	w2.fn[1][1] = add2;
	assert(w2.fn[0][0](1) == 2);
	assert(w2.fn[1][1](1) == 3);
	*ap10 = add2;
	assert(w2.fn[1][0](1) == 3 && ap10 == &w2.fn[1][0]);
	for (int i = 0; i < 2; i++)
		assert(w2.fn[i][i](1) == 2 + i);

	tab[0][0] = add1;
	tab[1][1] = add2;
	assert(tab[0][0](1) == 2);
	assert(tab[1][1](1) == 3);

	// call edge through a member of an anonymous union and through a variable
	// index into a two dimensional array member
	cwu.fn = callee;
	cw2.fn[0][1] = callee;
	cw2.fn[1][0] = callee;
	for (int i = 0; i < 5; i++)
	{
		assert(caller_union(i, 2 * i + 1) == ref_caller(i, 2 * i + 1));
		assert(caller_2d(i & 1, i, 2 * i + 1) == ref_caller(i, 2 * i + 1));
	}
	assert(cwu.fn(1, 2) == callee(1, 2));

	return 0;
}
