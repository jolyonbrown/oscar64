/* Comments are replaced by one space before directives are processed
 * (C89 2.1.1.2 / C99 5.1.1.2, translation phases 3 and 4), so a block
 * comment that starts on a #define line and ends on a later line belongs
 * to the directive; its continuation is not code. A comment delimiter
 * inside a string or character constant is not a delimiter.
 */
#include <assert.h>

#define A 1 /* a comment that
               carries on here */
#define B 2 /* closed on the line */ + 10
#define C 3 // line comment
#define D 4 /* contains // inside */ + 10
#define E 5 /* first */ /* the second one
               runs on */
#define F(x) ((x) /* inner */ + 1) /* trailer
   over two
   more lines */
#define G "a/*b" /* the string holds a comment opener
   */
#define H /* nothing but a comment
   */
#define I 6	/* tab before the comment, like NetHack's
		 * global.h: PL_PSIZ */
#define J 7 /* the directive ends at the end of the line
   the comment closes on, so this tail belongs to it */ + 10
#define K 8 /* spans
   three
   lines */ // and a line comment after it
#define L 9

/* A star at the end of one line and a slash at the start of the next do
 * not close the comment; a backslash-newline between them does.
 */
#define M 10 /* ends with a star *
/ still inside the comment */ + 1
#define N 11 /* closed by a spliced delimiter *\
/ + 1
#define O 12 /\
* opened by a spliced delimiter
   */ + 1
#define P 13 /* it's "quoted" and 'c' inside the comment
   ' " */ + 1 // and a line comment

/* Quotes: a '"' character constant, an escaped quote and a string that
 * holds a comment opener neither open nor close a comment.
 */
#define Q '"' /* a double quote in a character constant does not
   open a string */
#define R "a\"b" /* an escaped quote does not close the string
   either */
#define S "\"/*" /* so the opener in the string is still inside it */
#define T(c) ((c) == '"' ? "/*" : "") /* nor is this one */
#define BS "\\" /* two backslashes end the escape, the string is closed
   and the comment is seen */
#define UNUSED "\"/*"
#define FLAG 1
#define CLOSE "*/"

/* A directive inside the continued comment is not processed */
#define U 1 /* a directive inside the comment is not processed
#define V 2 */

#define W 3 /* used in #if
   below */ + 4

#ifdef V
#error "V must not be defined"
#endif
#ifndef FLAG
#error "FLAG must be defined"
#endif
#if W != 7
#error "W must be 7"
#endif

int main(void)
{
	int	h = H 8;

	assert(A == 1);
	assert(B == 12);
	assert(C == 3);
	assert(D == 14);
	assert(E == 5);
	assert(F(1) == 2);
	assert(G[0] == 'a');
	assert(G[1] == '/');
	assert(G[2] == '*');
	assert(G[3] == 'b');
	assert(h == 8);
	assert(I == 6);
	assert(J == 17);
	assert(K == 8);
	assert(L == 9);
	assert(M == 11);
	assert(N == 12);
	assert(O == 13);
	assert(P == 14);
	assert(Q == '"');
	assert(R[1] == '"' && R[3] == 0);
	assert(S[1] == '/' && S[2] == '*' && S[3] == 0);
	assert(T('"')[0] == '/');
	assert(T('a')[0] == 0);
	assert(BS[0] == '\\' && BS[1] == 0);
	assert(CLOSE[0] == '*' && CLOSE[1] == '/' && CLOSE[2] == 0);
	assert(U == 1);
	assert(W == 7);

	return 0;
}
