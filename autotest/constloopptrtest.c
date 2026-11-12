#include <assert.h>
#include <string.h>

// Loops over constant tables that the compiler can run at compile time
// (ConstSingleLoopOptimization, ConstInnerLoopOptimization) and that load
// pointers from the table: strings, struct members, functions and the
// addresses of globals.

static const char * const names[] = { "Alpha", "Beta", "Gamma", "Delta", 0 };

struct shop
{
	const char *	name;
	char			symb;
	int				prob;
};

static const struct shop shops[] = {
	{ "general", '=', 42 },
	{ "book",    '+', 10 },
	{ "candle",  '(',  5 },
	{ 0, 0, 0 }
};

int count_names(void)
{
	int n;
	for (n = 0; names[n]; n++)
		;
	return n;
}

const char * last_name(void)
{
	int n;
	for (n = 0; names[n + 1]; n++)
		;
	return names[n];
}

int walk_names(void)
{
	const char * const * p;
	int n = 0;
	for (p = names; *p; p++)
		n++;
	return n;
}

int count_shops(void)
{
	int n;
	for (n = 0; shops[n].name; n++)
		;
	return n;
}

int total_prob(void)
{
	int p = 0;
	for (int i = 0; shops[i].name; i++)
		p += shops[i].prob;
	return p;
}

int find_shop(char symb)
{
	int i;
	for (i = 0; shops[i].name && shops[i].symb != symb; i++)
		;
	return i;
}

int one(void) { return 11; }
int two(void) { return 22; }
int three(void) { return 33; }

typedef int (*func)(void);

static const func funcs[] = { one, two, three, 0 };

int count_funcs(void)
{
	int n;
	for (n = 0; funcs[n]; n++)
		;
	return n;
}

func last_func(void)
{
	int n;
	for (n = 0; funcs[n + 1]; n++)
		;
	return funcs[n];
}

int values[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

static int * const refs[] = { values + 2, values, values + 7, 0 };

int * last_ref(void)
{
	int n;
	for (n = 0; refs[n + 1]; n++)
		;
	return refs[n];
}

// Loops of more than one block, with null entries inside the table

static const char * const sparse[] = { "Alpha", 0, "Gamma", "Delta", 0, "Zeta" };

int count_sparse(void)
{
	int n = 0;
	for (int i = 0; i < 6; i++)
	{
		if (sparse[i])
			n++;
	}
	return n;
}

int weigh_sparse(void)
{
	int n = 0;
	for (int i = 0; i < 6; i++)
	{
		if (sparse[i])
			n += 3;
		else
			n += 100;
	}
	return n;
}

int main(void)
{
	assert(count_names() == 4);
	assert(!strcmp(last_name(), "Delta"));
	assert(walk_names() == 4);
	assert(count_shops() == 3);
	assert(total_prob() == 57);
	assert(find_shop('+') == 1);
	assert(find_shop('?') == 3);
	assert(count_funcs() == 3);
	assert(last_func()() == 33);
	assert(last_ref() == values + 7);
	assert(*last_ref() == 8);
	assert(count_sparse() == 4);
	assert(weigh_sparse() == 212);
	return 0;
}
