#include <string.h>
#include "main.h"

array::array(const array &a)
{
	data = new void *[a.count];
	count = a.count;
	allocated = a.count;
	for (int i = 0; i < count; i++)
		data[i] = a[i];
}

array &array::operator=(const array &a)
{
	if (this == &a)
		return *this;

	delete[] data;

	data = new void *[a.count];
	count = a.count;
	allocated = a.count;
	for (int i = 0; i < count; i++)
		data[i] = a[i];

	return *this;
}

void array::add(void *x)
{
	expand();
	data[count++] = x;
}

void array::insert(int i, void *x)
{
	expand();
	memmove(data + i + 1, data + i, (count - i) * sizeof(void *));
	data[i] = x;
	count++;
}

int array::contains(void *x)
{
	for (int i = 0; i < count; i++)
		if (data[i] == x)
			return 1;
	return 0;
}

void array::remove(int i)
{
	memmove(data + i, data + i + 1, (count - i - 1) * sizeof(void *));
	count--;
}

void array::remove(void *x)
{
	for (int i = 0; i < count; i++)
		if (data[i] == x)
		{
			remove(i);
			break;
		}
}

void array::expand()
{
	if (count >= allocated)
	{
		int n = allocated * 2 + 1;
		void **d = new void *[n];

		memcpy(d, data, allocated * sizeof(void *));
		delete[] data;

		data = d;
		allocated = n;
	}
}
