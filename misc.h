#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

inline int sign(int n)
{
	if (n < 0)
		return -1;
	if (n > 0)
		return 1;
	return 0;
}

inline int odd(int n)
{
	return n % 2;
}

inline int even(int n)
{
	return !odd(n);
}

char *copy(char *s);
void error(char *s);
void showmessage(char *caption, char *s);
void shuffle(void *a, int n);
