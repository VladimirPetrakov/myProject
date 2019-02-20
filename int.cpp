#include <fstream>
#include <string>
#include <functional>
#include <climits>

using namespace std;

template<typename K, typename T>
class HashMap
{
private:
	template<typename K, typename T>
	struct Item
	{
		K key;
		T value;
		Item<K, T>* next;
		Item<K, T>* back;

		Item(K key, T value) : key(key), value(value) {}
	};

	template<typename K, typename T>
	struct List
	{
		Item<K, T>* head;
		Item<K, T>* tail;
		int size;

		List() : size(0)
		{
			head = tail = nullptr;
		}

		void push(K key, T value)
		{
			Item<K, T>* item = new Item<K, T>(key, value);

			if (head == nullptr)
			{
				head = tail = item;
				item->back = item->next = nullptr;
			}
			else
			{
				item->back = tail;
				tail->next = item;
				tail = item;
				item->next = nullptr;
			}

			++size;
		}

	};

	int n;
	int k;
	int maxIndex;
	int minIndex;
	int _minIndex;
	int _maxIndex;
	float loadFactor;
	List<K, T>** listHashMap;

	int decrement(int d) { return --d; }

	int increment(int i) { return ++i; }

	bool boolIncrement(int i) { return i < n; }

	bool boolDecrement(int d) { return d > 0; }

	int calcIndex(int i, int (HashMap<K, T>::*f)(int), bool (HashMap<K, T>::*b)(int))
	{
		for (int j = (this->*f)(i); (this->*b)(j); j = (this->*f)(j))
		{
			if (listHashMap[j]->head != nullptr)
			{
				return j;
			}
		}

		return 0;
	}

	void removeItem(Item<K, T>* item, int i)
	{
		if (i < n)
		{
			if (item == listHashMap[i]->head && item->next == nullptr)
			{
				delete item;
				listHashMap[i]->head = nullptr;

				if (i == minIndex)
				{
					minIndex = _minIndex;
					_minIndex = calcIndex(minIndex, &HashMap<K, T>::increment, &HashMap<K, T>::boolIncrement);
				}

				if (i == maxIndex)
				{
					maxIndex = _maxIndex;
					_maxIndex = calcIndex(maxIndex, &HashMap<K, T>::decrement, &HashMap<K, T>::boolDecrement);
				}
			}
			else if (item == listHashMap[i]->head)
			{
				listHashMap[i]->head = listHashMap[i]->head->next;
				delete item;
			}
			else if (item->next == nullptr)
			{
				listHashMap[i]->tail = item->back;
				listHashMap[i]->tail->next = nullptr;
				delete item;
			}
			else
			{
				item->back->next = item->next;
				item->next->back = item->back;
				delete item;
			}

			--(listHashMap[i]->size);

		}
	}

	void del()
	{
		for (int i = 0; i <= n; ++i)
		{
			while (listHashMap[i]->size > 0)
			{
				removeItem(listHashMap[i]->tail, i);
			}

			delete listHashMap[i];
		}

		delete[] listHashMap;
	}

	void news(int N)
	{
		listHashMap = new List<K, T>*[N + 1];

		n = N;

		for (int i = 0; i <= n; ++i)
		{
			listHashMap[i] = new List<K, T>();
		}

		k = 0;
		loadFactor = 0.5;

		_minIndex = minIndex = _maxIndex = maxIndex = 0;

	}

	int getHash(K key)
	{
		tr1::hash<K> simple_hash;
		return simple_hash(key) % n;
	}

	void reHash()
	{
		HashMap<K, T> hash(3 * n);
		Iterator<K, T> iter;

		for (iter = begin(); iter != end(); ++iter)
		{
			hash.add(iter->key, iter->value);
		}

		del();
		news(3 * n);

		for (iter = hash.begin(); iter != hash.end(); ++iter)
		{
			add(iter->key, iter->value);
		}
	}

public:
	HashMap(int n)
	{
		news(n);
	}

	HashMap()
	{
		news(0);
	}

	int getSize()
	{
		return n;
	}

	HashMap<K, T>& operator=(HashMap<K, T>& hash)
	{
		if (n > 0)
		{
			this->del();
		}

		this->news(hash.getSize());

		for (HashMap<K, T>::Iterator<K, T> iter = hash.begin(); iter != hash.end(); ++iter)
		{
			this->add(iter->key, iter->value);
		}

		return *this;
	}


	~HashMap()
	{
		del();
	}

	template<typename K, typename T>
	class Iterator
	{
	private:
		int index;
		Item<K, T>* p;
		HashMap<K, T>* h;
	public:

		Iterator() : index(0) {}

		Iterator(HashMap<K, T>* map)
		{
			h = map;
			index = map->minIndex;
		}

		Item<K, T> operator*()
		{
			return *p;
		}

		Item<K, T>* operator->()
		{
			return p;
		}

		Iterator<K, T>& operator++()
		{
			if (p->next != nullptr)
			{
				p = p->next;
			}
			else
			{
				++index;

				if (index > h->maxIndex)
				{
					p = h->listHashMap[h->n]->head;
				}

				for (int i = index; i <= h->maxIndex; ++i)
				{
					if (h->listHashMap[i]->head != nullptr)
					{
						p = h->listHashMap[i]->head;
						index = i;
						break;
					}
				}
			}

			return *this;
		}


		bool operator!=(const Iterator<K, T>& iter)
		{
			return p != iter.p;
		}

		void setAdress(Item<K, T>* item) { p = item; }

	};

	Iterator<K, T> end()
	{
		Iterator<K, T> iter(this);
		iter.setAdress(listHashMap[n]->head);
		return iter;
	}

	Iterator<K, T> begin()
	{
		Iterator<K, T> iter(this);

		if (k == 0)
		{
			iter.setAdress(listHashMap[n]->head);
		}
		else
		{
			iter.setAdress(listHashMap[minIndex]->head);
		}

		return iter;
	}

	Item<K, T>* find(K key)
	{
		int i = getHash(key);

		for (Item<K, T>* item = listHashMap[i]->head; item != nullptr; item = item->next)
		{
			if (item->key == key)
			{
				return item;
			}
		}

		return nullptr;
	}

	void add(K key, T value)
	{
		if (n == 0)
		{
			news(5);
		}

		int i = getHash(key);
		Item<K, T>* item = find(key);

		if (item != nullptr)
		{
			item->value = value;
			return;
		}

		listHashMap[i]->push(key, value);
		++k;

		if (k == 1)
		{
			minIndex = maxIndex = i;
		}
		else if (k > 1)
		{
			if (i < minIndex)
			{
				_minIndex = minIndex;
				minIndex = i;
			}
			else if (i > maxIndex)
			{
				_maxIndex = maxIndex;
				maxIndex = i;
			}
		}

		if (k / n >= loadFactor)
		{
			reHash();
		}
	}

	void remove(K key)
	{
		int i = getHash(key);

		Item<K, T>* item = find(key);

		if (item == nullptr)
		{
			return;
		}

		removeItem(item, i);

		--k;
	}

	int getCount() { return k; }

	int uniqueValue()
	{
		typename HashMap<K, T>::Iterator<K, T> it1, it2, it3;
		HashMap<K, T> h = *this;

		for (it1 = h.begin(); it1 != h.end(); ++it1)
		{
			it3 = it1;
			for (it2 = it1; it2 != h.end(); ++it2)
			{
				if (it1->value == it2->value && it1 != it2)
				{

					h.remove(it2->key);
					it2 = it3;
				}

				it3 = it2;
			}
		}

		return h.k;
	}

};



class Expression
{
protected:
	string s;

public:
	virtual int getValue(HashMap<string, Expression*>& env)
	{
		throw "ERROR";
	}

	virtual void setScope(HashMap<string, Expression*>& env) {}

	virtual HashMap<string, Expression*>* getEnv() { return nullptr; }

	virtual Expression* copy(HashMap<string, Expression*>&) = 0;

	virtual string getId() { return ""; }

	virtual Expression* getBody(HashMap<string, Expression*>&)
	{
		return nullptr;
	}

	virtual void setBody(Expression*, HashMap<string, Expression*>& env) {}

	virtual Expression* fromEnv(string& str, HashMap<string, Expression*>& env)
	{
		if (env.find(str) != nullptr)
		{
			return env.find(str)->value;
		}

		throw "ERROR";
	}

	virtual Expression* eval(HashMap<string, Expression*>&) = 0;

	virtual ~Expression() {}
};


class Val : public Expression
{
	int a;
public:
	Val(int a) : a(a) {}

	virtual Expression* copy(HashMap<string, Expression*>& env)
	{
		return new Val(this->a);
	}

	virtual int getValue(HashMap<string, Expression*>& env)
	{
		return a;
	}

	virtual Expression* eval(HashMap<string, Expression*>& env)
	{
		return this->copy(env);
	}

	virtual ~Val() {}
};

class Var : public Expression
{
	string s;
public:
	Var(string str) : s(str) {}

	virtual Expression* copy(HashMap<string, Expression*>& env)
	{
		return new Var(this->s);
	}

	virtual string getId()
	{
		return s;
	}

	virtual Expression* eval(HashMap<string, Expression*>& env)
	{
		Expression* e = this->fromEnv(s, env);

		return e->copy(env);
	}
};

class Add : public Expression
{
	Expression* e1;
	Expression* e2;
public:
	Add(Expression* e1, Expression* e2) : e1(e1), e2(e2) {}

	virtual Expression* copy(HashMap<string, Expression*>& env)
	{
		return new Add(this->e1->copy(env), this->e2->copy(env));
	}

	virtual Expression* eval(HashMap<string, Expression*>& env)
	{
		Expression* e3 = e1->eval(env);
		Expression* e4 = e2->eval(env);

		return new Val(e3->copy(env)->getValue(env) + e4->copy(env)->getValue(env));
	}

	virtual ~Add()
	{
		delete e1;
		delete e2;
	}
};

class If : public Expression
{
	Expression* e1;
	Expression* e2;
	Expression* e_then;
	Expression* e_else;
public:
	If(Expression* e1, Expression* e2, Expression* e_then, Expression* e_else) : e1(e1), e2(e2), e_then(e_then), e_else(e_else) {}

	virtual Expression* copy(HashMap<string, Expression*>& env)
	{
		return new If(this->e1->copy(env), this->e2->copy(env), this->e_then->copy(env), this->e_else->copy(env));
	}

	virtual Expression* eval(HashMap<string, Expression*>& env)
	{

		if (e1->eval(env)->copy(env)->getValue(env) > e2->eval(env)->copy(env)->getValue(env))
		{
			return e_then->eval(env)->copy(env);
		}

		return e_else->eval(env)->copy(env);
	}

	virtual ~If()
	{
		delete e1;
		delete e2;
		delete e_then;
		delete e_else;
	}

};

class Let : public Expression
{
	string id;
	Expression* e1;
	Expression* e2;
	HashMap<string, Expression*> myEnv;
public:
	Let(string& id, Expression* e1, Expression* e2) : id(id), e1(e1), e2(e2) {}

	virtual Expression* copy(HashMap<string, Expression*>& env)
	{
		return new Let(this->id, this->e1->copy(env), this->e2->copy(env));
	}

	virtual Expression* eval(HashMap<string, Expression*>& env)
	{
		myEnv = env;
		myEnv.add(id, e1);
		e1->setScope(myEnv);
		return e2->eval(myEnv)->copy(myEnv);
	}

	virtual ~Let()
	{
		delete e1;
		delete e2;
	}
};

class Function : public Expression
{
	string id;
	Expression* e;
	HashMap<string,Expression*> scope;
public:
	Function(string& id, Expression* e) : id(id), e(e) {}

	virtual Expression* copy(HashMap<string, Expression*>& env)
	{
		Expression* e1 = new Function(this->id, this->e->copy(env));
		e1->setScope(scope);

		return e1;
	}

	virtual void setScope(HashMap<string, Expression*>& env)
	{
		scope = env;
	}

	virtual string getId()
	{
		return id;
	}

	virtual HashMap<string, Expression*>* getEnv()
	{
		return &scope;
	}

	virtual Expression* getBody(HashMap<string, Expression*>& env)
	{
		return e->copy(env);
	}

	virtual Expression* eval(HashMap<string, Expression*>& env)
	{
		return this->copy(scope);
	}

	virtual ~Function()
	{
		delete e;
	}
};

class Call : public Expression
{
	Expression* e1;
	Expression* e2;
public:
	Call(Expression* e1, Expression* e2) : e1(e1), e2(e2) {}

	virtual Expression* copy(HashMap<string, Expression*>& env)
	{
		return new Call(this->e1->copy(env), this->e2->copy(env));
	}

	virtual Expression* eval(HashMap<string, Expression*>& env)
	{
		Expression* e = e1->eval(env);

		HashMap<string, Expression*>* myEnv;
		myEnv = e->getEnv();
		myEnv->add(e->getId(), e2->eval(env)->copy(env));

		return e->getBody(*myEnv)->eval(*myEnv)->copy(*myEnv);
	}

	virtual ~Call()
	{
		delete e1;
		delete e2;
	}
};

class Set : public Expression
{
	string id;
	Expression* e;
public:
	Set(string& id, Expression* e) : id(id), e(e) {}
	
	virtual Expression* copy(HashMap<string, Expression*>& env)
	{
		return new Set(id, e->copy(env));
	}

	virtual Expression* eval(HashMap<string, Expression*>& env)
	{
		if (env.find(id) != nullptr)
		{
			env.add(id, e);
			return this->copy(env);
		}

		throw "ERROR";
	}

	virtual ~Set()
	{
		delete e;
	}
};

Expression* parser(ifstream& in, Expression* e, bool inStr)
{
	string s;
	char c;
	static int count = 0;

	in >> c;

	if (c == ' ')
	{
		return parser(in, e, inStr);

	}
	
	if (in.eof())
	{
		if (count == 0)
		{
			return e;
		}
		throw "ERROR";
	}

	if (c == '(')
	{
		++count;
		return parser(in, e, inStr);
	}

	in >> s;
	s = c + s;

	if (s != "in" && inStr)
	{
		throw "ERROR";
	}
	else if (inStr)
	{
		return parser(in, e, false);
	}

	if (s == "let")
	{
		string id;

		in >> id;
		in >> s;

		if (s == "=")
		{
			Expression* e1 = parser(in, e, inStr);
			Expression* e2 = parser(in, e, true);
			return new Let(id, e1, e2);
		}

		throw "ERROR";
	}

	if (s == "val")
	{
		in >> s;
		int f = s.find(")");
		count -= (s.length() - f);
		int sign = 1;
		int i = 0;

		if (s[0] == '-')
		{
			sign = -1;
			++i;
		}

		int a = s[i] - '0';

		if (f == 0)
		{
			throw "ERROR";
		}

		for (++i; i < f; ++i)
		{
			a = a * 10 + s[i] - '0';
		}

		return new Val(a*sign);

	}

	if (s == "set")
	{
		in >> s;

		Expression* e1 = new Set(s, parser(in, e, inStr));
		return parser(in, e,inStr);

	}

	if (s == "var")
	{
		in >> s;
		int f = s.find(")");
		count -= (s.length() - f);
		string s2 = "";

		if (f == 0)
		{
			throw "ERROR";
		}

		for (int i = 0; i < f; ++i)
		{
			s2 += s[i];
		}

		return new Var(s2);
	}

	if (s == "if")
	{
		Expression* e1 = parser(in, e, inStr);
		Expression* e2 = parser(in, e, inStr);
		Expression* e_then = parser(in, e, inStr);
		Expression* e_else = parser(in, e, inStr);

		return new If(e1, e2, e_then, e_else);
	}

	if ((s == "then" || s == "else"))
	{
		return parser(in, e, inStr);
	}

	if (s == "add")
	{
		Expression* e1 = parser(in, e, inStr);
		Expression* e2 = parser(in, e, inStr);

		return new Add(e1, e2);
	}

	if (s == "call")
	{
		Expression* e1 = parser(in, e, inStr);
		Expression* e2 = parser(in, e, inStr);

		return new Call(e1, e2);
	}

	if (s == "function")
	{
		in >> s;
		return new Function(s, parser(in, e, inStr));
	}

	throw "ERROR";
}

int main()
{
	ifstream in("input.txt");
	ofstream out("output.txt");
	HashMap<string, Expression*> env(5);
	Expression* e  = nullptr;
	try
	{
		e = parser(in, nullptr, false);
		e = e->eval(env);
	}
	catch (const char* s)
	{
		out << s;
		in.close();
		out.close();

		return 0;
	}

	out << "(val " << e->getValue(env) << ")";

	delete e;

	in.close();
	out.close();

	return 0;
}