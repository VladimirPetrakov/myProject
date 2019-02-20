#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>

using namespace std;

class Matrix {
private:
	int n;
	int **a;
	void del();
	void news();

public:
	Matrix(int);
	Matrix(int, int);
	Matrix(const Matrix&);
	~Matrix();
	void t();
	void read(FILE *);
	void print(FILE *);

	int& operator() (int i, int j)
	{
		if ((i < this->n && i >= 0) || (j < this->n && j >= 0))
		{
			return this->a[i][j];
		}

		throw "¬ыход за границы индексов";
	}

	const int& operator() (int i, int j) const
	{
		if ((i < this->n && i >= 0) || (j < this->n && j >= 0))
		{
			return this->a[i][j];
		}

		throw "¬ыход за границы индексов";
	}


	Matrix operator* (const Matrix &b)
	{
		if (b.n == this->n)
		{
			Matrix res(b.n);

			for (int k = 0; k < b.n; k++)
			{
				for (int i = 0; i < b.n; i++)
				{
					res(k, i) = 0;
					for (int j = 0; j < b.n; j++)
					{
						res(k, i) = res(k, i) + (*this)(k, j) * b(j, i);
					}
				}
			}

			return res;
		}

		throw "Ќевозможно перемножить матрицы несоответствующих размеров";
	}

	Matrix operator+ (const Matrix &b)
	{
		if (b.n == this->n)
		{
			Matrix res(b.n);
			for (int i = 0; i < b.n; i++)
			{
				for (int j = 0; j < b.n; j++)
				{
					res(i, j) = (*this)(i, j) + b(i, j);
				}
			}

			return res;
		}
		
		throw "Ќевозможно сложить матрицы несоответствующих размеров";
	}

	Matrix& operator= (const Matrix &b)
	{
		this->del();
		this->news();

		for (int i = 0; i < b.n; i++)
		{
			for (int j = 0; j < b.n; j++)
			{
				(*this)(i, j) = b(i, j);
			}
		}
	
		return *this;
	}

};

Matrix::Matrix(int N,int k)
{
	this->n = N;
	this->news();
	
	for (int i = 0; i < this->n; i++)
	{
		memset(this->a[i], 0, this->n*sizeof(int));
	}

	for (int i = 0; i < this->n; i++)
	{
		this->a[i][i] = k;
	}
}

Matrix::Matrix(int N)
{
	this->n = N;
	this->news();

	for (int i = 0; i < this->n; i++)
	{
		for (int j = 0; j < this->n; j++)
		{
			this->a[i][j] = rand();
		}
	}
}


Matrix::Matrix(const Matrix& b)
{
	this->n = b.n;
	this->news();

	for (int i = 0; i < b.n; i++)
	{
		for (int j = 0; j < b.n; j++)
		{
			(*this)(i, j) = b(i, j);
		}
	}
}




Matrix::~Matrix()
{
	this->del();
}

void Matrix::t()
{
	int t;
	for (int i = 0; i < this->n; i++)
	{
		for (int j = 0; j < i; j++)
		{
			t = (*this)(j, i);
			(*this)(j, i) = (*this)(i, j);
			(*this)(i, j) = t;
		}
	}
}

void Matrix::del()
{
	for (int i = 0; i < this->n; i++)
	{
		delete[] this->a[i];
	}

	delete[] this->a;

}

void Matrix::news()
{
	this->a = new int*[this->n];

	for (int i = 0; i < this->n; i++)
	{
		this->a[i] = new int[this->n];
	}
}

void Matrix::read(FILE *in)
{
	int c;
	
	for (int i = 0, j = 0; i < this->n; j++)
	{
		fscanf_s(in, "%d ", &c);
		(*this)(i, j) = c;
		if (j == this->n - 1)
		{
			i++;
			j = -1;
			fscanf_s(in, "\n");
		}
	}
}

void Matrix::print(FILE *out)
{
	int t;
	
	for (int i = 0, j = 0; i < this->n; j++)
	{
		t = (*this)(i, j);
		fprintf_s(out, "%d ", t);
		if (j == this->n - 1)
		{
			i++;
			j = -1;
			fprintf_s(out, "\n");
		}
	}
}

int main()
{
	int n, k1;

	FILE* in = fopen("input.txt", "r");
	FILE* out = fopen("output.txt", "w");
	fscanf_s(in, "%d\n%d\n", &n, &k1);

	Matrix a(n), b(n), c(n), d(n), k(n,k1), res(n);
	a.read(in);
	b.read(in);
	c.read(in);
	d.read(in);

	c.t();
	d.t();

	res = (a + b * c + k)*d;
	res.print(out);

	fclose(in);
	fclose(out);

	return 0;
}