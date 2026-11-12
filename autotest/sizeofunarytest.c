/* sizeof applied to a unary expression (C89 3.3.3.4, C99 6.5.3) */
#include <assert.h>
#include <string.h>

struct S { char a; int b; long c; };

int arr[10];
char buf[32];
struct S s, sa[5], *sp = sa;
int *p = arr;
long l;
char c;
int n;

/* only ever mentioned inside sizeof: must not be called and needs no definition */
extern long never_defined(void);

__noinline int paramsize(int a[10])
{
	/* an array parameter is a pointer */
	return sizeof a;
}

int main(void)
{
	int i;

	/* The constant comes first in each comparison: a parser that let the
	   operand swallow the rest of the expression would make assert(sizeof x == 20)
	   pass as sizeof (x == 20), which is never zero. */

	/* plain object */
	assert(20 == sizeof arr);
	assert(32 == sizeof buf);
	assert(4 == sizeof l);
	assert(1 == sizeof c);

	/* postfix operators belong to the operand */
	assert(2 == sizeof arr[0]);
	assert(sizeof (struct S) == sizeof sa[1]);
	assert(2 == sizeof s.b);
	assert(4 == sizeof sa[2].c);
	assert(4 == sizeof sp->c);

	/* prefix operators belong to the operand */
	assert(2 == sizeof *p);
	assert(sizeof s == sizeof *sp);
	assert(2 == sizeof &arr);
	assert(4 == sizeof -l);
	assert(2 == sizeof -c);
	assert(2 == sizeof +c);
	assert(2 == sizeof ~c);
	assert(sizeof (!l) == sizeof !l);
	assert(sizeof (sizeof arr) == sizeof sizeof arr);

	/* sizeof binds tighter than any binary operator; the value is stored
	   before it is compared so that a too greedy operand cannot hide */
	i = sizeof arr / sizeof arr[0];
	assert(i == 10);
	i = sizeof buf - 1;
	assert(i == 31);
	i = sizeof l * 2;
	assert(i == 8);
	i = sizeof c + sizeof l;
	assert(i == 5);
	i = sizeof arr == 20 ? 3 : 4;
	assert(i == 3);

	/* a parenthesised operand followed by postfix operators */
	i = sizeof (arr)[0];
	assert(i == 2);
	i = sizeof (s).c;
	assert(i == 4);
	i = sizeof (sp)->c;
	assert(i == 4);
	i = sizeof (arr) / sizeof (arr)[0];
	assert(i == 10);

	/* the parenthesised forms are unchanged */
	i = sizeof (arr);
	assert(i == 20);
	i = sizeof (arr) + 1;
	assert(i == 21);
	i = sizeof (int) * 3;
	assert(i == 6);
	assert(sizeof s == sizeof (struct S));
	i = sizeof (arr) / sizeof (arr[0]);
	assert(i == 10);

	/* the operand is not evaluated */
	n = 0;
	i = sizeof n++;
	assert(i == 2);
	i = sizeof ++n;
	assert(i == 2);
	i = sizeof arr[n++];
	assert(i == 2);
	i = sizeof (n)++;
	assert(i == 2);
	i = sizeof never_defined();
	assert(i == 4);
	i = sizeof (never_defined)();
	assert(i == 4);
	assert(n == 0);

	assert(2 == paramsize(arr));

	/* the memset idiom */
	memset(buf, 0x55, sizeof buf);
	for (i = 0; i < 32; i++)
		assert(buf[i] == 0x55);
	memset(arr, 0, sizeof arr);
	for (i = 0; i < 10; i++)
		assert(arr[i] == 0);

	return 0;
}
