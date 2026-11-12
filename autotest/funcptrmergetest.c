#include <assert.h>

// Functions with identical code whose address is taken must still
// have distinct addresses, even if the linker merges identical
// constant objects or appends a function to its tail call target.
// This includes addresses that are only used as integers.

typedef void (* fop)(const char * s);
typedef void (* vfp)(void);
typedef int (* ifn)(int);

int seen, count;

// seen is never read, so the stores are removed and fa, fb are identical

void fa(const char * s) { (void)s; seen = 1; }
void fb(const char * s) { (void)s; seen = 2; }

void ga(const char * s) { (void)s; count++; }
void gb(const char * s) { (void)s; count++; }

// ja, jb are only used as integers

void ja(const char * s) { (void)s; count++; }
void jb(const char * s) { (void)s; count++; }

// hb is a bare tail call of ha

__noinline void ha(void) { count++; }
__noinline void hb(void) { ha(); }

// Called directly with a constant and indirectly through its address
// as an integer

__noinline int k(int x) { return x + 1; }

volatile fop gp = fa;

fop tab[2] = { ga, gb };

struct procs
{
	fop	init, exit;
};

struct procs tty_procs = { ga, gb };
struct procs * volatile wp = &tty_procs;

volatile unsigned ia, ib;

volatile vfp ht[2];

int main(void)
{
	fop q = fb;
	fop g = gb;

	volatile int i = 0;

	assert(gp != q);
	assert(tab[i] != tab[i + 1]);
	assert(wp->init != wp->exit);
	assert(tab[i] == wp->init);
	assert(tab[i + 1] == g);

	tab[i]("a");
	tab[i + 1]("b");
	wp->init("c");
	wp->exit("d");

	assert(count == 4);

	ia = (unsigned)ja;
	ib = (unsigned)jb;
	assert(ia != ib);
	((fop)ib)("e");
	assert(count == 5);

	ht[0] = ha;
	ht[1] = hb;
	assert(ht[0] != ht[1]);
	ht[1]();
	assert(count == 6);

	assert(k(5) == 6);
	ia = (unsigned)k;
	assert(((ifn)ia)(7) == 8);

	return 0;
}
