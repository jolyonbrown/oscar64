#include <assert.h>
#include <string.h>

/*
 * Regression test for folding loads of signed values from known bytes.
 *
 * When the optimizer replaces a load from constant initializer data (or
 * from memory filled by memset) with the constant it finds there, it has
 * to sign extend the stored bytes when the loaded value is signed: -3
 * stored as 0xfffd must become -3, not 65533, or the constant folding and
 * value range analysis that follow see the wrong value (an abs() style
 * ?: then yields -65533 and a comparison with 3 is folded to false).
 *
 * Covered: a local struct read through its initializer (through a pointer
 * and directly), full width signed bitfields and single member struct
 * copies (loads without a range), a loop over a const table that is
 * evaluated at compile time, and loads forwarded from a memset.  The
 * unsigned members and the 8 and 32 bit values are controls.
 */

struct point { int x, y; };
struct spoint { signed char x, y; };
struct lpoint { long x, y; };
struct upoint { unsigned x, y; };
struct ulpoint { unsigned long x, y; };
struct cpoint { unsigned char x, y; };
struct bpoint { int x : 16; int y : 16; };
struct one { short v; };
struct uone { unsigned short v; };
struct minmax { signed char c; int i; long l; unsigned char uc; unsigned u; unsigned long ul; };

static const int tab[4] = {-3, 5, -7, 2};
static const unsigned utab[4] = {65533, 5, 7, 2};

static int manhattan(struct point * pt)
{
	int dx = pt->x < 0 ? -pt->x : pt->x;
	int dy = pt->y < 0 ? -pt->y : pt->y;
	return dx + dy;
}

static int smanhattan(struct spoint * pt)
{
	int dx = pt->x < 0 ? -pt->x : pt->x;
	int dy = pt->y < 0 ? -pt->y : pt->y;
	return dx + dy;
}

static long lmanhattan(struct lpoint * pt)
{
	long dx = pt->x < 0 ? -pt->x : pt->x;
	long dy = pt->y < 0 ? -pt->y : pt->y;
	return dx + dy;
}

static int ax(struct point * pt)
{
	return pt->x < 0 ? -pt->x : pt->x;
}

static int bx(struct bpoint * p)
{
	return p->x < 0 ? -p->x : p->x;
}

static int ov(struct one * p)
{
	return p->v < 0 ? -p->v : p->v;
}

static unsigned uov(struct uone * p)
{
	return p->v;
}

static int ucheck(struct ulpoint * p)
{
	return p->x > 0x7fffffffUL && (p->x >> 31) == 1UL && p->x / 65536UL == 65535UL;
}

int main(void)
{
	struct point pt = {-3, 4};
	struct spoint spt = {-3, 4};
	struct lpoint lpt = {-3, 4};
	struct lpoint lpt2 = {-70000, 4};
	struct upoint upt = {65533, 4};
	struct ulpoint ulpt = {0xfffffffdul, 4};
	struct cpoint cpt = {253, 4};
	struct bpoint bpt = {-3, 4};
	struct one o1 = {-3}, o2;
	struct uone uo1 = {65533}, uo2;
	struct minmax mn = {-128, -32768, -2147483647l - 1, 255, 65535, 0xfffffffful};
	struct minmax mx = {127, 32767, 2147483647l, 128, 32768, 0x80000000ul};
	int a[4];
	int s, i;
	unsigned us;

	/* struct member read through a pointer */
	assert(ax(&pt) == 3);
	assert(manhattan(&pt) == 7);
	assert(smanhattan(&spt) == 7);
	assert(lmanhattan(&lpt) == 7);
	assert(lmanhattan(&lpt2) == 70004);

	/* direct member access, no pointer */
	i = pt.x < 0 ? -pt.x : pt.x;
	assert(i == 3);

	assert(pt.x < 0);
	assert(pt.x == -3);
	assert(spt.x < 0);
	assert(spt.x == -3);
	assert(lpt.x < 0);
	assert(lpt.x == -3);

	/* full width signed bitfield: the load carries no range */
	assert(bx(&bpt) == 3);
	assert(bpt.x == -3);

	/* single member struct copy: the copy load carries no range */
	o2 = o1;
	assert(ov(&o2) == 3);
	uo2 = uo1;
	assert(uov(&uo2) == 65533);
	assert(uo2.v > 1000);

	/* loop over a const table, evaluated at compile time */
	s = 0;
	for (i = 0; i < 4; i++)
		s += tab[i];
	assert(s == -3);
	assert(s / 3 == -1);
	us = 0;
	for (i = 0; i < 4; i++)
		us += utab[i];
	assert(us == 65547u - 65536u);
	assert(utab[0] > 1000);

	/* load forwarded from a memset */
	memset(a, 0xff, sizeof(a));
	i = a[1] < 0 ? -a[1] : a[1];
	assert(i == 1);
	assert(a[2] == -1);

	/* signed minimum and maximum at each width, widened rather than negated */
	assert((long)mn.c == -128l);
	assert((long)mn.i == -32768l);
	assert(mn.l == -2147483647l - 1);
	assert(mn.l < 0);
	assert((long)mx.c == 127l);
	assert((long)mx.i == 32767l);
	assert(mx.l == 2147483647l);
	assert(mx.l > 0);

	/* unsigned controls at the sign bit and at the maximum */
	assert(upt.x > 1000);
	assert(upt.x == 65533);
	assert(ulpt.x > 1000);
	assert(ulpt.x == 0xfffffffdul);
	assert(ucheck(&ulpt));
	assert(cpt.x > 100);
	assert(cpt.x == 253);
	assert(mn.uc == 255);
	assert(mn.u == 65535);
	assert(mn.ul == 0xfffffffful);
	assert(mn.ul > 0x7ffffffful);
	assert(mx.uc == 128);
	assert(mx.u == 32768);
	assert(mx.u > 32767);
	assert(mx.ul == 0x80000000ul);
	assert(mx.ul > 0x7ffffffful);
	assert((mx.ul >> 31) == 1);

	return 0;
}
