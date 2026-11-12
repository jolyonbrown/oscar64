#include <assert.h>

// The middle operand of ?: is a full expression (C99 6.5.15, C++ [expr.cond]),
// so a nested unparenthesised ?: , an assignment or a comma expression is
// allowed there without parentheses.

static const char * pick(int a, int b, int c)
{
	return a ? b ? "x" : "y" : c ? "z" : "w";
}

// char parameters: an int-typed ?: nested in the last operand is miscompiled
// with -n -O0 on its own, independently of how it is parenthesised
static int pick3(char a, char b, char c, char d)
{
	return a ? b ? 1 : 2 : c ? 3 : d ? 4 : 5;
}

static int middle_comma(int a, int * p)
{
	return a ? *p = 7, 11 : 13;
}

// the comma operands are evaluated left to right
static int middle_comma_sequence(int a, int * p)
{
	return a ? *p = 7, *p + 4 : 13;
}

static int middle_comma3(int a, int * p)
{
	return a ? *p = 1, *p += 2, *p * 10 : 13;
}

static int middle_assign(int a, int * p)
{
	return a ? *p = 21 : 23;
}

// only the selected branch is evaluated
static int nested_side_effects(int a, int b, int * p, int * q)
{
	return a ? b ? (*p)++ : (*q)++ : b ? (*p)-- : (*q)--;
}

static int two(int x, int y)
{
	return 10 * x + y;
}

static int one(int x)
{
	return x;
}

static int first(int a, ...)
{
	return a;
}

static volatile int vz;

static int comma_in_argument(int a)
{
	// one argument, not two: the comma belongs to the middle operand
	return two(a ? 1, 2 : 3, a ? 4, 5 : 6);
}

static int folded_comma_in_argument(void)
{
	return two(1 ? 1, 2 : 3, 0 ? 4, 5 : 6);
}

// a ?: with a constant condition folds to its middle operand, which then
// stands in place of the call argument
static int folded_comma_only_argument(void)
{
	return one(1 ? vz, 2 : 3);
}

static int folded_comma_last_argument(void)
{
	return two(9, 1 ? vz, 5 : 6);
}

static int folded_comma3_only_argument(void)
{
	return one(1 ? vz, 2, 3 : 4);
}

static int folded_comma3_variadic(void)
{
	return first(1 ? vz, 2, 3 : 0);
}

static int first_precedence(int a, int b)
{
	// ?: binds weaker than || and relational operators
	return a || b ? 1 : 2;
}

static int relational_first(int a)
{
	return a >= 5 ? a > 10 ? 2 : 1 : 0;
}

enum
{
	CE1 = 1 ? 0 ? 3 : 4 : 5,
	CE2 = 0 ? 0 ? 3 : 4 : 5,
	CE3 = 1 ? 1 ? 3 : 4 : 5
};

static int gi = 1 ? 2 ? 10 : 20 : 30;
static const char * gs = 0 ? 1 ? "a" : "b" : 1 ? "c" : "d";

int main(void)
{
	volatile int va = 1, vb = 0, vc = 1;

	// nested conditional in the middle operand
	assert(pick(1, 1, 1)[0] == 'x');
	assert(pick(1, 0, 1)[0] == 'y');
	assert(pick(0, 1, 1)[0] == 'z');
	assert(pick(0, 1, 0)[0] == 'w');

	// nested conditional in the last operand (right associative)
	assert(pick3(1, 1, 0, 0) == 1);
	assert(pick3(1, 0, 0, 0) == 2);
	assert(pick3(0, 0, 1, 0) == 3);
	assert(pick3(0, 0, 0, 1) == 4);
	assert(pick3(0, 0, 0, 0) == 5);

	// comma expression in the middle operand
	int t = 0;
	assert(middle_comma(1, &t) == 11);
	assert(t == 7);
	t = 0;
	assert(middle_comma(0, &t) == 13);
	assert(t == 0);

	t = 0;
	assert(middle_comma_sequence(1, &t) == 11);
	assert(t == 7);
	t = 0;
	assert(middle_comma_sequence(0, &t) == 13);
	assert(t == 0);

	t = 0;
	assert(middle_comma3(1, &t) == 30);
	assert(t == 3);
	t = 0;
	assert(middle_comma3(0, &t) == 13);
	assert(t == 0);

	// assignment in the middle operand
	t = 0;
	assert(middle_assign(1, &t) == 21);
	assert(t == 21);
	t = 0;
	assert(middle_assign(0, &t) == 23);
	assert(t == 0);

	// only the selected branch has its side effect
	int p = 10, q = 20;
	assert(nested_side_effects(1, 1, &p, &q) == 10);
	assert(p == 11 && q == 20);
	assert(nested_side_effects(1, 0, &p, &q) == 20);
	assert(p == 11 && q == 21);
	assert(nested_side_effects(0, 1, &p, &q) == 11);
	assert(p == 10 && q == 21);
	assert(nested_side_effects(0, 0, &p, &q) == 21);
	assert(p == 10 && q == 20);

	// comma in the middle operand inside an argument list
	assert(comma_in_argument(1) == 25);
	assert(comma_in_argument(0) == 36);
	assert(folded_comma_in_argument() == 26);
	assert(folded_comma_only_argument() == 2);
	assert(folded_comma_last_argument() == 95);
	assert(folded_comma3_only_argument() == 3);
	assert(folded_comma3_variadic() == 3);

	// precedence of the first operand is unchanged
	assert(first_precedence(0, 0) == 2);
	assert(first_precedence(0, 1) == 1);
	assert(first_precedence(1, 0) == 1);
	assert(relational_first(3) == 0);
	assert(relational_first(7) == 1);
	assert(relational_first(12) == 2);

	// a ? b : c , d parses as (a ? b : c), d
	int u = (va ? 1 : 2, 3);
	assert(u == 3);

	// non-constant operands, folded at runtime
	const char * s = va ? vb ? "x" : "y" : vc ? "z" : "w";
	assert(s[0] == 'y');
	s = vb ? va ? "x" : "y" : vc ? "z" : "w";
	assert(s[0] == 'z');
	int n = va ? vb ? 1 : 2 : vc ? 3 : 4;
	assert(n == 2);
	n = vb ? vc ? 1 : 2 : va ? 3 : 4;
	assert(n == 3);

	// constant expressions
	assert(CE1 == 4);
	assert(CE2 == 5);
	assert(CE3 == 3);
	assert(gi == 10);
	assert(gs[0] == 'c');

	return 0;
}
