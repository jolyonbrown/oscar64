#include <assert.h>

// Function pointer members of global struct variables: assignment, call
// through s.m(...), (*s.m)(...) and p->m(...), with and without -Ofp

int add1(int x)
{
	return x + 1;
}

int add2(int x)
{
	return x + 2;
}

int add3(int x)
{
	return x + 3;
}

int mul(int x, int y)
{
	return x * y;
}

int sub(int x, int y)
{
	return x - y;
}

static int called;

void setcalled(void)
{
	called = 1;
}

void setcalled_str(const char * s)
{
	called = s[0];
}

struct procs
{
	int		(*fn)(int x);
	int		(*fn2)(int x, int y);
	void	(*vfn)(void);
	void	(*sfn)(const char * s);
};

struct procs		gp;
static struct procs	sp;
struct procs		ip = { add1, mul, setcalled, setcalled_str };

// struct declared together with its variable
struct
{
	int		(*fn)(int x);
}	dp;

// nested struct and array of function pointers
struct nested
{
	char			c;
	struct procs	p;
}	np;

struct arrmem
{
	int		(*fn[2])(int x);
}	ap;

// initialized array member at a non zero offset
struct arrinit
{
	int		pad;
	int		(*fn[2])(int x);
}	ai = { 123, { add1, add2 } };

// array of structs and struct holding an array of structs, initialized
struct item
{
	char	name;
	int		(*fn)(int x);
};

struct item		objects[3] = { { 'a', add1 }, { 'b', add2 }, { 'c', add3 } };

struct table
{
	int			pad;
	struct item	it[2];
}	tb = { 9, { { 'x', add2 }, { 'y', add3 } } };

// bit fields around the function pointer: fn starts in the second byte of
// the storage unit of a, so it must be found by the bits used, not by the
// size of the bit field's type
struct bits
{
	unsigned	a : 3;
	unsigned	b : 5;
	int			(*fn)(int x);
	unsigned	c : 4;
}	bf;

// initialized, then reassigned in one function and called from another:
// the write must not be removed as a store to a constant
struct procs1
{
	int		pad;
	int		(*fn)(int x);
};

struct procs1			rw = { 0, add1 };
static struct procs1	rs = { 0, add1 };

struct arrinit			ra = { 0, { add1, add1 } };

struct nested1
{
	char			c;
	struct procs1	p;
}	rn = { 'n', { 0, add1 } };

__noinline void reassign(void)
{
	rw.fn = add2;
	rs.fn = add2;
	ra.fn[1] = add2;
	rn.p.fn = add2;
}

__noinline int call_reassigned(int x)
{
	return rw.fn(x) + rs.fn(x) + ra.fn[1](x) + rn.p.fn(x);
}

// a call through a member must be registered as a call: the caller keeps
// locals live across it, and the callee uses a helper with its own locals.
// The operands stay small so that no intermediate result overflows an int
struct procs2
{
	int		(*fn)(int a, int b);
}	cw;

struct procsarr
{
	int		pad;
	int		(*fn[2])(int a, int b);
}	cwa;

int		(*carr[2])(int a, int b);

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

__noinline int caller(int x, int y)
{
	int l1 = x * 13, l2 = y * 17, l3 = x * y + 19, l4 = x - y;
	int r = cw.fn(x, y);
	return l1 + l2 + l3 + l4 + r;
}

__noinline int caller_arr(int x, int y)
{
	int l1 = x * 13, l2 = y * 17, l3 = x * y + 19, l4 = x - y;
	int r = carr[1](x, y);
	return l1 + l2 + l3 + l4 + r;
}

__noinline int caller_varidx(int i, int x, int y)
{
	int l1 = x * 13, l2 = y * 17, l3 = x * y + 19, l4 = x - y;
	int r = cwa.fn[i](x, y);
	return l1 + l2 + l3 + l4 + r;
}

int ref_caller(int x, int y)
{
	int l1 = x * 13, l2 = y * 17, l3 = x * y + 19, l4 = x - y;
	int u = x + 10, v = y + 20, s = u * v;
	int r = (u * 3 + v * 5 + s * 7 + (x ^ y) * 11) + s;
	return l1 + l2 + l3 + l4 + r;
}

// as used by NetHack's window interface
#define askname (*ip.vfn)

__noinline void set_procs(struct procs * p, bool c)
{
	if (c)
	{
		p->fn = add1;
		p->fn2 = mul;
	}
	else
	{
		p->fn = add2;
		p->fn2 = sub;
	}
	p->vfn = setcalled;
	p->sfn = setcalled_str;
}

__noinline int call_procs(struct procs * p, int x, int y)
{
	return p->fn(x) + (*p->fn2)(x, y);
}

__noinline int call_global(int x, int y)
{
	return gp.fn(x) + (*gp.fn2)(x, y);
}

__noinline int call_static(int x, int y)
{
	return sp.fn(x) + (*sp.fn2)(x, y);
}

__noinline int call_object(int i, int x)
{
	return objects[i].fn(x);
}

__noinline int call_static_local(int x)
{
	static struct procs1	ls = { 0, add1 };

	int	r = ls.fn(x);
	ls.fn = add2;
	return r;
}

int main(void)
{
	// assign and call, global
	gp.fn = add1;
	gp.fn2 = mul;
	gp.vfn = setcalled;
	gp.sfn = setcalled_str;
	assert(gp.fn(1) == 2);
	assert((*gp.fn)(2) == 3);
	assert(gp.fn2(3, 4) == 12);
	assert((*gp.fn2)(5, 6) == 30);
	called = 0; gp.vfn(); assert(called == 1);
	called = 0; gp.sfn("x"); assert(called == 'x');
	assert(call_global(2, 3) == 3 + 6);

	// static global
	sp.fn = add2;
	sp.fn2 = sub;
	assert(sp.fn(1) == 3);
	assert((*sp.fn2)(5, 2) == 3);
	assert(call_static(2, 3) == 4 - 1);

	// global with initializer
	assert(ip.fn(1) == 2);
	assert(ip.fn2(3, 4) == 12);
	called = 0; askname(); assert(called == 1);
	called = 0; ip.sfn("y"); assert(called == 'y');

	// struct declared together with its variable
	dp.fn = add2;
	assert(dp.fn(1) == 3);

	// through a pointer to the global, functions chosen at runtime
	for (int i = 0; i < 2; i++)
	{
		bool	c = i == 0;

		set_procs(&gp, c);
		assert(gp.fn(10) == (c ? 11 : 12));
		assert(gp.fn2(10, 2) == (c ? 20 : 8));
		assert(call_procs(&gp, 10, 2) == (c ? 11 + 20 : 12 + 8));
		assert(call_global(10, 2) == (c ? 11 + 20 : 12 + 8));

		set_procs(&sp, !c);
		assert(call_static(10, 2) == (c ? 12 + 8 : 11 + 20));

		called = 0; gp.vfn(); assert(called == 1);
		called = 0; sp.sfn("z"); assert(called == 'z');
	}

	// nested struct
	np.p.fn = add1;
	np.p.fn2 = sub;
	assert(np.p.fn(1) == 2);
	assert((*np.p.fn2)(4, 1) == 3);

	// array of function pointers
	ap.fn[0] = add1;
	ap.fn[1] = add2;
	assert(ap.fn[0](1) == 2);
	assert(ap.fn[1](1) == 3);
	for (int i = 0; i < 2; i++)
		assert(ap.fn[i](5) == 6 + i);

	// initialized array member
	assert(ai.pad == 123);
	assert(ai.fn[0](1) == 2);
	assert(ai.fn[1](1) == 3);
	for (int i = 0; i < 2; i++)
		assert(ai.fn[i](5) == 6 + i);

	// array of structs
	assert(objects[0].fn(1) == 2 && objects[1].fn(1) == 3 && objects[2].fn(1) == 4);
	assert(call_object(1, 10) == 12);
	objects[1].fn = add3;
	assert(objects[1].fn(1) == 4 && call_object(1, 1) == 4);
	assert(tb.it[0].fn(1) == 3 && tb.it[1].fn(1) == 4);
	assert(tb.pad == 9 && tb.it[1].name == 'y');
	tb.it[0].fn = add1;
	assert(tb.it[0].fn(1) == 2);

	// bit fields around the member
	bf.a = 5; bf.b = 21; bf.c = 9;
	bf.fn = add1;
	assert(bf.fn(1) == 2);
	assert(bf.a == 5 && bf.b == 21 && bf.c == 9);

	// initialized, then reassigned
	assert(call_reassigned(1) == 4 * 2);
	reassign();
	assert(call_reassigned(1) == 4 * 3);
	assert(rw.fn(1) == 3 && rs.fn(1) == 3 && ra.fn[1](1) == 3 && rn.p.fn(1) == 3);
	assert(ra.fn[0](1) == 2 && rn.c == 'n');

	// static local
	assert(call_static_local(1) == 2);
	assert(call_static_local(1) == 3);

	// call edge through a member, through a constant array index and through
	// a variable index into an array member
	cw.fn = callee;
	carr[1] = callee;
	cwa.fn[0] = callee;
	cwa.fn[1] = callee;
	for (int i = 0; i < 5; i++)
	{
		assert(caller(i, 2 * i + 1) == ref_caller(i, 2 * i + 1));
		assert(caller_arr(i, 2 * i + 1) == ref_caller(i, 2 * i + 1));
		assert(caller_varidx(i & 1, i, 2 * i + 1) == ref_caller(i, 2 * i + 1));
	}

	// member read as a value
	int	(*f)(int) = gp.fn;
	assert(f(7) == gp.fn(7));

	return 0;
}
