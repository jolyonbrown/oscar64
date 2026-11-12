/* Spaces and tabs between the '#' and the directive name are allowed
 * (C89 3.8, C99 6.10p5), in active code as well as in skipped groups.
 */
# include <assert.h>
# include <string.h>

# define A 1
#	define B 2
  #  define C 3

int c_value = C;

#ifdef A
# define D 10
#else
# define D 20
#endif

#ifndef UNDEFINED_THING
# ifdef B
#  define E 100
# else
#  define E 200
# endif
#else
# define E 300
#endif

/* The indented directives inside a skipped group must still be
 * recognised, or the group does not end where it should */
#ifdef UNDEFINED_THING
# define F 1
# ifdef A
#  define F 2
# else
#  define F 3
# endif
#else
# define F 4
#endif

#if 0
#	if 1
#		define G 1
#	endif
#else
#	define G 2
#endif

/* The unspaced #else belongs to the inner "# if", so neither branch of
 * it is active */
#if 0
# if 1
#  define WRONG 1
#else
#  define WRONG 2
# endif
#else
# define DEPTH_OK 1
#endif

#ifdef WRONG
# define WRONG_DEFINED 1
#else
# define WRONG_DEFINED 0
#endif

/* "# elif" and "# else" selecting a branch, in active code and inside a
 * skipped group */
# if defined(A) && defined(B)
#  define H 5
# elif defined(C)
#  define H 6
# else
#  define H 7
# endif

# if defined(UNDEFINED_THING)
#  define I 1
# elif defined(C)
#  define I 2
# else
#  define I 3
# endif

# if defined(UNDEFINED_THING)
#  define J 1
# elif defined(UNDEFINED_THING)
#  define J 2
# else
#  define J 3
# endif

#if 0
# define K 1
# elif 1
# define K 2
# else
# define K 3
# endif

# undef C
# ifdef C
#  define C_DEFINED 1
# else
#  define C_DEFINED 0
# endif

/* The stringify operator takes the same path, so "# x" must give the
 * same string as "#x" */
# define STR1(x) #x
# define STR2(x) # x
# define STR3(x) #	x

/* "##" is not affected */
# define CAT(a, b) a##b

int vw = 42;

/* In assembler macro text, '#' followed by blanks and a name is still
 * an immediate operand, not a directive or a stringify */
#define N5 5
#define IMM5 # N5
#define IMM(x) # x
#define LDA(v) lda v
enum { E9 = 9 };

char r0, r1, r2, r3;

int main(void)
{
	assert(A == 1);
	assert(B == 2);
	assert(c_value == 3);
	assert(D == 10);
	assert(E == 100);
	assert(F == 4);
	assert(G == 2);
	assert(DEPTH_OK == 1);
	assert(WRONG_DEFINED == 0);
	assert(H == 5);
	assert(I == 2);
	assert(J == 3);
	assert(K == 2);
	assert(C_DEFINED == 0);

	assert(strcmp(STR1(hello), STR2(hello)) == 0);
	assert(strcmp(STR1(a + b), STR3(a + b)) == 0);
	assert(CAT(v, w) == 42);

	__asm {
		lda IMM5
		sta r0
		lda IMM(7)
		sta r1
		LDA(# E9)
		sta r2
		LDA(# N5)
		sta r3
	}

	assert(r0 == 5);
	assert(r1 == 7);
	assert(r2 == 9);
	assert(r3 == 5);

	return 0;
}
