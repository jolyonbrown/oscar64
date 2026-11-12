#include <assert.h>

// Comparing a function pointer with a function name: the function
// designator converts to a pointer to the function (C89 3.3.9).

void fa(const char * s)
{
	(void)s;
}

void fb(const char * s)
{
	(void)s;
}

struct procs
{
	int			pad;
	void		(* raw_print)(const char * s);
};

typedef void (* fop)(const char * s);

fop	gp;

// Functions that are only compared, never called or assigned: the
// global optimizer must keep their return value and parameters, or the
// comparison fails with incompatible pointer types at -O2 and above.

int fc(void)
{
	return 1;
}

int fd(int x)
{
	return 2;
}

static int fe(void);

int		(* gq)(void);
int		(* gr)(int x);

static int fe(void)
{
	return 3;
}

// compared only, parameter unused: isolates parameter removal from
// return value removal
void ff(int x)
{
}

void	(* gs)(int x);

// compared, assigned and called through the pointer
int fh(int x)
{
	return x * 3;
}

// compared and called directly, never assigned or passed: once the direct
// call is inlined, only the comparison keeps the optimizer from changing
// the function's type
int fj(int x)
{
	return x * 5;
}

// declared through a typedef of the function type
typedef int fn_t(void);

fn_t	fi;

fn_t *	gt;

int fi(void)
{
	return 4;
}

int main(void)
{
	fop				p = fa;
	volatile fop	vp = fa;
	struct procs	w;

	w.raw_print = fb;
	gp = fa;

	// pointer variable vs function name, both orders
	assert(p == fa);
	assert(fa == p);
	assert(p != fb);
	assert(fb != p);
	assert(!(p == fb));
	assert(!(p != fa));

	// explicit address of a function
	assert(p == &fa);
	assert(&fa == p);
	assert(p != &fb);
	assert(&fb != p);

	// struct member holding a function pointer
	assert(w.raw_print == fb);
	assert(fb == w.raw_print);
	assert(w.raw_print != fa);
	assert(fa != w.raw_print);
	assert(w.raw_print == &fb);

	// global function pointer variable
	assert(gp == fa);
	assert(fa == gp);
	assert(gp != fb);

	// two function names
	assert(fa == fa);
	assert(fa != fb);
	assert(&fa == &fa);
	assert(&fa != &fb);

	// dereferenced function pointer is the function itself
	assert(*p == fa);
	assert(fa == *p);

	// used as a condition
	if (p == fa)
		p = fb;
	assert(p == fb);
	if (p != fb)
		p = fa;
	assert(p == fb);

	// volatile pointer, must be read
	assert(vp == fa);
	assert(fa == vp);
	assert(vp != fb);
	vp = fb;
	assert(vp == fb);
	assert(vp != fa);
	assert(fb == vp);

	// casts
	p = fa;
	assert((fop)fa == p);
	assert(p != (fop)fb);
	assert((fop)&fa == p);
	assert((void *)fa == (void *)p);
	assert((void *)fb != (void *)p);

	// function name vs null pointer constant, both orders
	assert(fa != 0);
	assert(p != 0);
	assert(0 != fa);
	assert(0 != p);
	assert(!(fa == 0));
	assert(!(0 == fa));
	assert(&fa != 0);
	assert(0 != &fa);

	// functions whose address is only compared, never called or assigned
	assert(gq != fc);
	assert(fc != gq);
	assert(!(gq == fc));
	assert(gr != fd);
	assert(fd != gr);
	assert(gq != fe);
	assert(fe != gq);
	assert(fc != fe);
	if (gq == fc || gq == fe || gr == fd)
		return 1;
	assert(gs != ff);
	assert(ff != gs);

	// compared function that is also assigned and called through the pointer
	assert(fh(4) == 12);
	assert(gr != fh);
	gr = fh;
	assert(gr == fh);
	assert(fh == gr);
	assert(gr != fd);
	assert(gr(5) == 15);

	// compared function that is called directly, never assigned or passed
	assert(fj(2) == 10);
	assert(gr != fj);
	assert(fj != gr);
	assert(!(gr == fj));

	// typedef'd function type
	assert(gt != fi);
	assert(fi != gt);
	gt = fi;
	assert(gt == fi);
	assert(fi == gt);
	assert(gt() == 4);

	return 0;
}
