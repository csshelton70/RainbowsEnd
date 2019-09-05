struct array
{
	void **data;
	int count;
	int allocated;

	array()
	{
		data = 0;
		count = 0;
		allocated = 0;
	}

	~array()
	{
		delete[] data;
	}

	array(const array& a);
	array &operator=(const array &a);

	void *&operator[](int i) const
	{
		return data[i];
	}

	void add(void *x);
	void insert(int i, void *x);
	int contains(void *x);
	void remove(int i);
	void remove(void *x);

	void expand();
};
