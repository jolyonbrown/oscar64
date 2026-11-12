#include <assert.h>

/*
 * A pointer to a union may gain qualifiers, like a pointer to a struct or
 * to a scalar: as a function argument, in an assignment, in an
 * initialisation and in a return statement (C89 3.3.16.1, 3.3.2.2, 3.6.6.4;
 * C99 6.5.16.1, 6.5.2.2, 6.8.6.4). Pointers to the qualified and to the
 * unqualified union may be compared and subtracted (C99 6.5.6, 6.5.8, 6.5.9).
 */

union Value
{
	int		i;
	char	c[2];
};

struct Pair
{
	int		a, b;
};

typedef union Value Any;

/* volatile CValue is a const volatile union Value */
typedef const union Value CValue;

static int read_const(const union Value* p)
{
	return p->i;
}

static int read_volatile(volatile union Value* p)
{
	return p->i;
}

static int read_const_volatile(volatile CValue* p)
{
	return p->i;
}

static int read_typedef(const Any* p)
{
	return p->i;
}

static int read_struct(const struct Pair* p)
{
	return p->a + p->b;
}

static const union Value* keep_const(union Value* p)
{
	return p;
}

static volatile union Value* keep_volatile(union Value* p)
{
	return p;
}

static volatile CValue* keep_const_volatile(union Value* p)
{
	return p;
}

static int sum_const(const union Value* const* pp, int n)
{
	int s = 0;
	for (int i = 0; i < n; i++)
		s += pp[i]->i;
	return s;
}

int main(void)
{
	union Value v, w, arr[4];
	struct Pair pr;
	const union Value* cp;
	volatile union Value* vp;
	volatile CValue* cvp;
	union Value* mp = &v;

	v.i = 1234;
	w.i = 4321;
	pr.a = 20;
	pr.b = 22;

	/* argument */
	assert(read_const(&v) == 1234);
	assert(read_volatile(&v) == 1234);
	assert(read_const_volatile(&v) == 1234);
	assert(read_typedef(&v) == 1234);
	assert(read_struct(&pr) == 42);

	/* initialisation */
	const union Value* ip = &v;
	volatile union Value* jp = &w;
	volatile CValue* kp = &v;
	Any* tp = &v;
	const Any* ctp = tp;
	assert(ip->i == 1234);
	assert(jp->i == 4321);
	assert(kp->i == 1234);
	assert(ctp->i == 1234);

	/* assignment */
	cp = &w;
	assert(cp->i == 4321);
	cp = mp;
	assert(cp->i == 1234);
	vp = &w;
	assert(vp->i == 4321);
	vp = mp;
	assert(vp->i == 1234);

	/* a pointer that already has one qualifier gains the other */
	cvp = cp;
	assert(cvp->i == 1234);
	cvp = vp;
	assert(cvp->i == 1234);
	assert(read_const_volatile(cp) == 1234);
	assert(read_const_volatile(vp) == 1234);

	/* return */
	assert(keep_const(&w)->i == 4321);
	assert(keep_volatile(&w)->i == 4321);
	assert(keep_const_volatile(&w)->i == 4321);

	/* comparison and subtraction */
	for (int i = 0; i < 4; i++)
		arr[i].i = i;
	cp = &arr[3];
	mp = &arr[1];
	vp = &arr[1];
	assert(cp != mp);
	assert(cp > mp);
	assert(mp < cp);
	assert(cp - mp == 2);
	assert(mp - cp == -2);
	assert(vp == mp);
	assert(&arr[3] == cp);
	assert(keep_const(mp) == mp);
	assert(cp - 2 == mp);

	/* array of pointers to const */
	const union Value* ps[2];
	ps[0] = &v;
	ps[1] = &w;
	assert(sum_const(ps, 2) == 5555);

	/* writing through the mutable pointer is seen through the const one */
	cp = &v;
	mp = &v;
	mp->i = 99;
	assert(cp->i == 99);
	assert(read_const(mp) == 99);

	return 0;
}
