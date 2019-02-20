#include <fstream>
#include <string.h>
#include <stack>

using namespace std;

class Expression
{
public:
	virtual void print(ofstream&) = 0;
	virtual Expression* diff(char) = 0;
	virtual Expression* copyPaste() = 0;
	virtual Expression* simplification() = 0;
	virtual bool checkSimply(int) = 0;
	virtual ~Expression() {}
};

class Binary : public Expression
{
protected:
	Expression* e1;
	Expression* e2;
public:
	Binary(Expression* a, Expression* b) : e1(a), e2(b) {}

	virtual bool checkSimply()
	{
		return false;
	}

	virtual ~Binary() 
	{
		delete this->e1;
		delete this->e2;
	}
};

class Number : public Expression
{
	int c;
public:
	void print(ofstream& f)
	{
		f << c;
	}

	Expression* diff(char d)
	{
		return new Number(0);
	}

	bool checkSimply(int a)
	{
		if (this->c == a)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	Expression* copyPaste()
	{
		return new Number(this->c);
	}

	Expression* simplification()
	{
		return this->copyPaste();
	}

	Number(int C): c(C) {}
};

class Variable : public Expression
{
	char c;
public:
	void print(ofstream& f)
	{
		f << c;
	}

	Expression* diff(char d)
	{
		if (d == this->c)
		{
			return new Number(1);
		}
		else
		{
			return new Number(0);
		}
	}

	bool checkSimply(int a)
	{
		return false;
	}


	Expression* copyPaste()
	{
		return new Variable(this->c);
	}

	Expression* simplification()
	{
		return this->copyPaste();
	}

	Variable(char C): c(C) {}
};

class Add : public Binary 
{
public:
	void print(ofstream& f)
	{
		f << "(";
		this->e1->print(f);
		f << "+";
		this->e2->print(f);
		f << ")";
	}

	Expression* diff(char d)
	{
		return new Add(this->e1->diff(d), this->e2->diff(d));
	}

	Expression* copyPaste()
	{
		return new Add(this->e1->copyPaste(),this->e2->copyPaste());
	}

	bool checkSimply(int a)
	{
		return false;
	}

	Expression* simplification()
	{
		if (this->e1->checkSimply(0))
		{
			return this->e2->copyPaste()->simplification();
		}
		else if (this->e2->checkSimply(0))
		{
			return this->e1->copyPaste()->simplification();
		}
		else
		{
			return new Add(this->e1->copyPaste()->simplification(), this->e2->copyPaste()->simplification());
		}
	}

	Add(Expression* a, Expression* b): Binary(a,b) {}
};

class Sub : public Binary
{
public:
	void print(ofstream& f)
	{
		f << "(";
		this->e1->print(f);
		f << "-";
		this->e2->print(f);
		f << ")";
	}

	Expression* diff(char d)
	{
		return new Sub(this->e1->diff(d), this->e2->diff(d));
	}

	bool checkSimply(int a)
	{
		return false;
	}

	Expression* simplification()
	{
		if (this->e2->checkSimply(0))
		{
			return this->e1->copyPaste()->simplification();
		}
		else
		{
			return new Sub(this->e1->copyPaste()->simplification(), this->e2->copyPaste()->simplification());
		}
	}

	Expression* copyPaste()
	{
		return new Sub(this->e1->copyPaste(), this->e2->copyPaste());
	}

	Sub(Expression* a, Expression* b): Binary(a,b) {}
};

class Mul : public Binary
{
public:
	void print(ofstream& f)
	{
		f << "(";
		this->e1->print(f);
		f << "*";
		this->e2->print(f);
		f << ")";
	}
	
	Expression* diff(char d)
	{
		return new Add(new Mul(this->e1->diff(d), this->e2->copyPaste()), new Mul(this->e1->copyPaste(), this->e2->diff(d)));
	}

	bool checkSimply(int a)
	{
		return false;
	}

	Expression* simplification()
	{
		if (this->e1->checkSimply(1))
		{
			return this->e2->copyPaste()->simplification();
		}
		else if (this->e2->checkSimply(1))
		{
			return this->e1->copyPaste()->simplification();
		}
		if (this->e1->checkSimply(0) || this->e2->checkSimply(0))
		{
			return new Number(0);
		}
		else
		{
			return new Mul(this->e1->copyPaste()->simplification(), this->e2->copyPaste()->simplification());
		}
	}

	Expression* copyPaste()
	{
		return new Mul(this->e1->copyPaste(), this->e2->copyPaste());
	}

	Mul(Expression* a, Expression* b): Binary(a,b) {}
};

class Div : public Binary
{
public:
	void print(ofstream& f)
	{
		f << "(";
		this->e1->print(f);
		f << "/";
		this->e2->print(f);
		f << ")";

	}

	Expression* diff(char d)
	{
		return new Div(new Sub(new Mul(this->e1->diff(d), this->e2->copyPaste()), new Mul(this->e1->copyPaste(), this->e2->diff(d))), 
			new Mul(this->e2->copyPaste(), this->e2->copyPaste()));
	}

	bool checkSimply(int a)
	{
		return false;
	}

	Expression* simplification()
	{
		if (this->e1->checkSimply(0))
		{
			return new Number(0);
		}
		else if (this->e2->checkSimply(1))
		{
			return this->e1->copyPaste()->simplification();
		}
		else
		{
			return new Div(this->e1->copyPaste()->simplification(), this->e2->copyPaste()->simplification());
		}
	}

	
	Expression* copyPaste()
	{
		return new Div(this->e1->copyPaste(), this->e2->copyPaste());
	}

	Div(Expression* a, Expression* b): Binary(a,b){}
};


Expression* parser(ifstream& f, Expression* e)
{
	char c;
	int v = 0, count = 0;
	static int count2 = 0;
	f >> c;
	if (f.eof())
	{
		if (count2 == 0)
		{
			return e;
		}
		else
		{
			return nullptr;
		}
	}
	else if (c == '(')
	{
		++count2;
		return parser(f, e);
	}
	else
	{
		while (c >= '0' && c <= '9' && !f.eof())
		{
			v = v * 10 + c - '0';
			f >> c;
			++count;
		}

		if (count > 0)
		{
			e = new Number(v);
		}
		else if (c >= 'a' && c <= 'z')
		{
			e = new Variable(c);
			f >> c;
		}

		if (c == ')')
		{
			--count2;
			return e;
		}

		if (c == '*')
		{
			e = parser(f, new Mul(e, parser(f, e)));
		}
		else if (c == '/')
		{
			e = parser(f, new Div(e, parser(f, e)));
		}
		else if (c == '+')
		{
			e = parser(f, new Add(e, parser(f, e)));
		}
		else if (c == '-')
		{
			e = parser(f, new Sub(e, parser(f, e)));
		}
		return e;
	}
}

int priority(char op)
{
	if (op == '+' || op == '-')
	{
		return 1;
	}
	else if (op == '*' || op == '/')
	{
		return 2;
	}
	else
	{
		return 3;
	}
}

Expression* createExp(stack <char> &st)
{
	if (st.size() > 0)
	{
		char t = st.top();
		st.pop();

		if (t == '+')
		{
			Expression* r = createExp(st);
			Expression* l = createExp(st);
			return new Add(l, r);
		}
		else if (t == '-')
		{
			Expression* r = createExp(st);
			Expression* l = createExp(st);
			return new Sub(l, r);
		}
		else if (t == '*')
		{
			Expression* r = createExp(st);
			Expression* l = createExp(st);
			return new Mul(l, r);
		}
		else if (t == '/')
		{
			Expression* r = createExp(st);
			Expression* l = createExp(st);
			return new Div(l, r);
		}
		else if (t >= '0' && t <= '9')
		{
			int v = t - '0';
			int d = 10;
			while (st.size() > 0 && st.top() == '_')
			{
				st.pop();
				v += (st.top() - '0') * d;
				st.pop();
				d *= 10;
			}

			return new Number(v);
		}
		else if (t >= 'a' && t <= 'z')
		{
			return new Variable(t);
		}
	}
	else
	{
		return nullptr;
	}
}

stack <char> createStack(ifstream& f)
{
	stack <char> st1, st2;
	char c;
	int count = -1;

	for (f >> c; !f.eof(); f >> c)
	{
		if (c >= '0' && c <= '9')
		{ 
			++count;
			if (count > 0)
			{
				st1.push('_');
			}

			st1.push(c);
		}
		else if (c >= 'a' && c <= 'z')
		{
			count = -1;
			st1.push(c);
		}
		else
		{
			count = -1;

			if (st2.size() == 0)
			{
				st2.push(c);
			}
			else
			{
				if (priority(st2.top()) >= priority(c))
				{
					st1.push(st2.top());
					st2.pop();
					st2.push(c);
				}
				else
				{
					st2.push(c);
				}
			}
		}
	}

	while (st2.size() > 0)
	{
		st1.push(st2.top());
		st2.pop();
	}

	return st1;
}

Expression* parser2(ifstream& f)
{
	stack <char> st = createStack(f);
	return createExp(st);
}

int main()
{
	ofstream f("output.txt");
	ifstream f2("input.txt");
	Expression* e = parser2(f2);

	if (e != nullptr)
	{
//		Expression* de = e->diff('x');
//		Expression* s = e->simplification();

		e->print(f);
//		delete s;
//		delete de;
	}
	else
	{
		f << "parser erorr";
	}

	f.close();
	f2.close();
	delete e;

	return 0;
}
