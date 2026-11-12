#include <assert.h>

// A run of independent conditionals between two uses of a value.  The
// number of paths through the run doubles with every conditional, and
// three walks over the flow graph visited a block once per path instead
// of once:
//
// IsSameRegisterSource (does the A, X or Y register already hold the
// value a load would give it, on every path into the block; here the
// index 1 in Y for the second read through the pointer), vsum below
// CanJoinEntryLoadStoreZP (moving a parameter copy up into the blocks
// in front of a call), argjoin below
// CheckSingleUseGlobalLoadStruct (forwarding an indexed load to the
// uses of the local it was stored in), the element functions below
//
// The last one repeated its second check of a block, first reached
// behind an overwrite, for every path, and never came back when the
// blocks behind the overwrite formed a loop: element_loop crashed the
// compiler with a stack overflow in the native configurations above -O0.
//
// Without the fix each of vsum, argjoin and element_chain takes about a
// second or more to compile in the native configurations (vsum at every
// optimisation level, the other two above -O0), and two or three times
// as long for every conditional added.  With it the file compiles in a
// fraction of a second.  The functions are called with more than one
// argument value, so the arguments are not folded into them.

#define CHAIN4(i) \
	if (fl[i]) acc += 1; \
	if (fl[i + 1]) acc += 2; \
	if (fl[i + 2]) acc += 3; \
	if (fl[i + 3]) acc += 4;

#define CHAIN12 CHAIN4(0) CHAIN4(4) CHAIN4(8)
#define CHAIN14 CHAIN12 if (fl[12]) acc += 1; if (fl[13]) acc += 2;
#define CHAIN16 CHAIN12 CHAIN4(12)
#define CHAIN24 CHAIN16 CHAIN4(16) CHAIN4(20)

char		fl[64];
char		arr[8] = {10, 20, 30, 40, 50, 60, 70, 80};
char		cb = 5;
char		gk;
int		acc;

volatile char	buf[4], buf2[4];

__noinline void bump(void)
{
	acc++;
}

__noinline int use(char * p)
{
	return p[0] + p[1];
}

__noinline int isink(int x, int y)
{
	return 100 * x + y;
}

// Two reads of the same volatile byte through a pointer: the index stays
// in the Y register across the chain, and is only loaded again on the
// path that reads p[2]

__noinline int vsum(volatile char * p)
{
	int s = 0;
	s += p[1];
	CHAIN16
	if (gk) s += p[2];
	s += p[1];
	return s;
}

// The value is passed in a different argument position on the two paths
// behind the chain

__noinline int argjoin(int a, int b)
{
	int x = a * 3 + b;
	CHAIN24
	if (fl[63]) return isink(x, 1);
	return isink(2, x);
}

// The element is overwritten in one arm in front of the chain

__noinline int element_chain(char k)
{
	char t[4];
	t[1] = 1;
	t[0] = arr[k];
	int r = 0;
	if (fl[63]) { r = use(t); t[0] = cb; r += use(t); }
	CHAIN14
	return r;
}

// As before, the element is read behind the chain

__noinline int element_chain_read(char k)
{
	char t[4];
	t[1] = 1;
	t[0] = arr[k];
	int r = 0;
	if (fl[63]) { r = use(t); t[0] = cb; r += use(t); }
	CHAIN4(0)
	return r + t[0];
}

// The element is read before it is overwritten in one arm

__noinline int element_read_chain(char k)
{
	char t[4];
	t[1] = 1;
	t[0] = arr[k];
	int r = t[0];
	if (fl[63]) t[0] = cb;
	CHAIN4(0)
	return r + use(t);
}

// A loop behind the overwrite that does not touch the element

__noinline int element_loop(char k)
{
	char t[4];
	t[1] = 1;
	t[0] = arr[k];
	int r = 0;
	if (fl[63]) { r = use(t); t[0] = cb; r += use(t); }
	for (char i = 0; i < k; i++) acc += fl[i];
	return r;
}

// A loop behind the overwrite that overwrites the element again

__noinline int element_loop_store(char k)
{
	char t[4];
	t[1] = 1;
	t[0] = arr[k];
	int r = 0;
	if (fl[63]) { r = use(t); t[0] = cb; r += use(t); }
	CHAIN4(0)
	for (char i = 0; i < k; i++) { if (fl[i + 8]) t[0] = i; bump(); }
	return r + t[0];
}

// A second overwrite behind the chain

__noinline int element_two_stores(char k)
{
	char t[4];
	t[1] = 1;
	t[0] = arr[k];
	int r = 0;
	if (fl[63]) { r = use(t); t[0] = cb; r += use(t); }
	CHAIN4(0)
	if (fl[62]) t[0] = 9;
	CHAIN4(4)
	bump();
	return r + use(t);
}

int main(void)
{
	buf[1] = 3;
	buf[2] = 4;
	buf2[1] = 7;
	buf2[2] = 8;

	assert(vsum(buf) == 6);
	assert(argjoin(5, 2) == 217);
	assert(argjoin(4, 5) == 217);
	assert(element_chain(1) == 0);
	assert(element_chain_read(1) == 20);
	assert(element_read_chain(1) == 41);
	assert(element_loop(3) == 0);
	assert(element_loop_store(2) == 30);
	assert(element_two_stores(2) == 31);
	assert(acc == 3);

	gk = 1;
	fl[1] = 1;
	fl[5] = 1;
	fl[9] = 1;
	fl[63] = 1;
	acc = 0;

	assert(vsum(buf2) == 22);
	assert(acc == 2 + 2 + 2);
	assert(argjoin(5, 2) == 1701);
	assert(argjoin(4, 5) == 1701);
	assert(element_chain(2) == 31 + 6);
	assert(element_chain_read(2) == 31 + 6 + 5);
	assert(element_read_chain(2) == 30 + 6);
	assert(element_loop(2) == 31 + 6);
	assert(element_loop_store(2) == 31 + 6 + 1);
	assert(acc == 6 + 12 + 6 + 2 + 2 + 1 + 4);

	fl[62] = 1;
	assert(element_two_stores(2) == 31 + 6 + 10);
	fl[63] = 0;
	assert(element_two_stores(2) == 10);
	fl[62] = 0;
	assert(element_two_stores(3) == 41);
	fl[8] = 1;
	assert(element_loop_store(2) == 1);
	assert(element_loop_store(1) == 0);
	assert(element_loop_store(0) == 10);

	return 0;
}
