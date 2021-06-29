#include <lib/string.h>
#include <lib/stdint.h>

void *memcpy(void *dst, const void *src, size_t len)
{
	size_t i;

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

void *memset(void *dst, int value, size_t num)
{
	if ((uintptr_t)dst % sizeof(long) == 0 && num % sizeof(long) == 0)
	{

		long *d = (long *)dst;

		for (size_t i = 0; i < num / sizeof(long); i++)
		{
			d[i] = value;
		}
	}
	else
	{
		char *d = (char *)dst;

		for (size_t i = 0; i < num; i++)
		{
			d[i] = value;
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

char *itoa(int32_t num, char *str, int base)
{
	int i = 0;
	bool isNegative = false;

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

	if (num < 0 && base == 10)
	{
		isNegative = true;
		num = -num;
	}

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

	if (isNegative)
		str[i++] = '-';

	str[i] = '\0';

	reverse(str, i);

	return str;
}

char *ltoa(int64_t num, char *str, int base)
{
	int i = 0;
	bool isNegative = false;

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

	if (num < 0 && base == 10)
	{
		isNegative = true;
		num = -num;
	}

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

	if (isNegative)
		str[i++] = '-';

	str[i] = '\0';

	reverse(str, i);

	return str;
}

char *utoa(unsigned int num, char *str, int base)
{
	int i = 0;
	
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

	str[i] = '\0';

	reverse(str, i);

	return str;
}

char *ltoa(uint64_t num, char *str, int base)
{
	int i = 0;
	
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

	str[i] = '\0';

	reverse(str, i);

	return str;
}

size_t strlen(char* str)
{
	size_t i = 0;
	while(str[i] != '\0')
		i++;
	return i;
}
