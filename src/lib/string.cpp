#include <lib/string.h>
#include <lib/stdint.h>

void *memcpy(void *dst, const void *src, size_t len)
{
	size_t i;

	/*
         * memcpy does not support overlapping buffers, so always do it
         * forwards. (Don't change this without adjusting memmove.)
         *
         * For speedy copying, optimize the common case where both pointers
         * and the length are word-aligned, and copy word-at-a-time instead
         * of byte-at-a-time. Otherwise, copy by bytes.
         *
         * The alignment logic below should be portable. We rely on
         * the compiler to be reasonably intelligent about optimizing
         * the divides and modulos out. Fortunately, it is.
         */

	if ((uintptr_t)dst % sizeof(long) == 0 && (uintptr_t)src % sizeof(long) == 0 && len % sizeof(long) == 0)
	{

		long *d = (long *)dst;
		const long *s = (long *)src;

		for (i = 0; i < len / sizeof(long); i++)
		{
			d[i] = s[i];
		}
	}
	else
	{
		char *d = (char *)dst;
		const char *s = (char *)src;

		for (i = 0; i < len; i++)
		{
			d[i] = s[i];
		}
	}

	return dst;
}

template <typename T>
void swap(T &t1, T &t2)
{
	T tmp(t1);
	t1 = t2;
	t2 = tmp;
}

void reverse(char str[], int length)
{
	int start = 0;
	int end = length - 1;
	while (start < end)
	{
		swap(*(str + start), *(str + end));
		start++;
		end--;
	}
}

char *itoa(int num, char *str, int base)
{
	int i = 0;
	bool isNegative = false;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		if (base == 16)
		{
			str[i++] = '0';
			str[i++] = 'x';
		}
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled only with
	// base 10. Otherwise numbers are considered unsigned.
	if (num < 0 && base == 10)
	{
		isNegative = true;
		num = -num;
	}

	// Process individual digits
	while (num != 0)
	{
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	if (base == 16)
	{
		str[i++] = 'x';
		str[i++] = '0';
	}

	// If number is negative, append '-'
	if (isNegative)
		str[i++] = '-';

	str[i] = '\0'; // Append string terminator

	// Reverse the string
	reverse(str, i);

	return str;
}
