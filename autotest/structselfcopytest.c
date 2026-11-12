// Copies of a struct onto itself through a pointer: "*p = *p", or two
// copies that the optimizer joins into one.  Such a copy changes nothing,
// but the forwarding of loads and copies through it used to replace an
// address by itself and report a change on every pass, so the compiler
// never finished.

#include <assert.h>
#include <string.h>

struct S
{
	char	a[20];
	int		b;
};

struct I
{
	int		x[5];
};

struct T
{
	char	c[3];
	struct I	i, j;
};

struct S	k[4];
struct T	t[2];
char		buf[2][16];

volatile char	vi;

// a load after a self copy
int self_load(struct S * p)
{
	*p = *p;
	return p->b;
}

// two self copies, nothing loaded
int self_twice(struct S * p)
{
	*p = *p;
	*p = *p;
	return 7;
}

// a self copy followed by a real copy
int self_then_copy(struct S * p, struct S * q)
{
	*p = *p;
	*q = *p;
	return q->b;
}

// no self copy in the source, copy propagation turns "*p = t" into one
int save_restore(struct S * p, struct S * q)
{
	struct S	t = *p;
	*p = t;
	*q = *p;
	return p->b;
}

// two names for one pointer
int same_pointer(struct S * p)
{
	struct S	*	q = p;
	struct S	*	r = q;

	*q = *r;
	*r = *p;
	q->b++;
	*p = *q;
	return r->b;
}

// members copied onto themselves
int self_member(struct T * p)
{
	p->i = p->i;
	p->j = p->i;
	p->j = p->j;
	return p->j.x[4] + p->i.x[0];
}

// in a loop, under a condition
int self_loop(struct S * p, char n)
{
	int	s = 0;
	for(char i=0; i<n; i++)
	{
		if (i & 1)
			p[i] = p[i];
		s += p[i].b;
	}
	return s;
}

int self_memcpy(void)
{
	char	*	p = buf[vi];

	p[3] = 7;
	memcpy(p, p, 16);
	return p[3];
}

int main(void)
{
	for(char i=0; i<4; i++)
	{
		k[i].a[0] = 10 + i;
		k[i].a[19] = 20 + i;
		k[i].b = 1000 + i;
	}

	assert(self_load(k + vi) == 1000);
	assert(self_load(k + 1 + vi) == 1001);
	assert(self_twice(k + vi) == 7);

	assert(self_then_copy(k + vi, k + 3 - vi) == 1000);
	assert(k[3].a[0] == 10 && k[3].a[19] == 20 && k[3].b == 1000);

	assert(save_restore(k + 1 + vi, k + 2 + vi) == 1001);
	assert(k[2].a[0] == 11 && k[2].a[19] == 21 && k[2].b == 1001);
	assert(k[1].a[0] == 11 && k[1].a[19] == 21 && k[1].b == 1001);

	assert(same_pointer(k + vi) == 1001);
	assert(k[0].a[0] == 10 && k[0].a[19] == 20 && k[0].b == 1001);

	t[0].i.x[0] = 12;
	t[0].i.x[4] = 30;
	assert(self_member(t + vi) == 42);
	assert(t[0].j.x[0] == 12 && t[0].j.x[4] == 30);

	assert(self_loop(k + vi, 4) == 1001 + 1001 + 1001 + 1000);

	assert(self_memcpy() == 7);

	return 0;
}
