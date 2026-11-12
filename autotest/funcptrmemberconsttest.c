#include <assert.h>

// Function pointer members of const structs: a call through the member
// compiles, an assignment to it is rejected with "Cannot assign to const
// type". Each FUNCPTR_CONST_* selector adds one invalid assignment, for the
// funcptrmemberconstconstraints rule

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

struct outer
{
	char				c;
	const struct procs	in;
};

const struct procs	cw = { 0, add1, { add1, add2 } };
struct procs		w;
struct outer		ow = { 'o', { 1, add2, { add2, add1 } } };
const struct procs	tab[2] = { { 0, add1, { add1, add1 } }, { 1, add2, { add2, add2 } } };

__noinline int call_ptr(const struct procs * p, int x)
{
	return p->fn(x) + p->arr[1](x);
}

int main(void)
{
	static const struct procs	ls = { 0, add2, { add1, add2 } };
	const struct procs			lc = { 0, add1, { add2, add1 } };

	assert(cw.fn(1) == 2 && cw.arr[1](1) == 3);
	assert(ow.in.fn(1) == 3 && ow.in.arr[0](1) == 3);
	assert(tab[1].fn(1) == 3 && tab[0].arr[1](1) == 2);
	assert(ls.fn(1) == 3 && lc.fn(1) == 2);
	assert(call_ptr(&cw, 1) == 2 + 3);
	assert(call_ptr(&lc, 1) == 2 + 2);

	w.fn = add2;
	w.arr[0] = add1;
	w.arr[1] = add1;
	assert(w.fn(1) == 3 && call_ptr(&w, 1) == 3 + 2);

#if defined(FUNCPTR_CONST_GLOBAL)
	cw.fn = add2;
#elif defined(FUNCPTR_CONST_LOCAL)
	lc.fn = add2;
#elif defined(FUNCPTR_CONST_STATIC_LOCAL)
	ls.fn = add2;
#elif defined(FUNCPTR_CONST_POINTER)
	const struct procs *	p = &w;
	p->fn = add2;
#elif defined(FUNCPTR_CONST_ARRAY)
	cw.arr[1] = add2;
#elif defined(FUNCPTR_CONST_NESTED)
	ow.in.fn = add2;
#elif defined(FUNCPTR_CONST_ARRAY_OF_STRUCTS)
	tab[1].fn = add2;
#endif

	return 0;
}
