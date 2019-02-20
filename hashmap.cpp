#include <fstream>
#include <functional>
#include <string>

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

		List(): size(0)
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
	List<K,T>** listHashMap;
	
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
					_minIndex = calcIndex(minIndex, &HashMap<K,T>::increment, &HashMap<K, T>::boolIncrement);
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
		news(3*n);

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

	HashMap(HashMap<K, T>& hash)
	{
		Iterator<K, T> iter;
		news(hash.n);
		
		for (iter = hash.begin(); iter != hash.end(); ++iter)
		{
			add(iter->key, iter->value);
		}
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
		Item<K,T>* p;
		HashMap<K, T>* h;
	public:

		Iterator() : index(0) {}

		Iterator(HashMap<K, T>* map)
		{
			h = map;
			index = map->minIndex;
		}

		Item<K,T> operator*()
		{
			return *p;
		}

		Item<K,T>* operator->()
		{
			return p;
		}

		Iterator<K,T>& operator++()
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


		bool operator!=(const Iterator<K,T>& iter)
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

	Iterator<K,T> begin()
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

	Item<K,T>* find(K key)
	{
		int i = getHash(key);

		for (Item<K,T>* item = listHashMap[i]->head; item != nullptr; item = item->next)
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
		int i = getHash(key);
		Item<K,T>* item = find(key);

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

		Item<K,T>* item = find(key);

		if (item == nullptr)
		{
			return;
		}

		removeItem(item,i);

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

template<typename K, typename T>
void start(K t1, T t2, fstream& in)
{
	ofstream out("output.txt");
	int n;
	char c;

	in >> n;

	HashMap<K, T> h(n);

	for (int i = 0; i < n; ++i)
	{
		in >> c;
		
		if (c == 'A')
		{
			in >> t1;
			in >> t2;
			h.add(t1, t2);
		}
		else if (c == 'R')
		{
			in >> t1;
			h.remove(t1);
		}
	}

	out << h.getCount() << " " << h.uniqueValue();

	out.close();
}

template<typename T>
void createType(T a, fstream& in)
{
	char c;
	in >> c;

	if (c == 'I')
	{
		start(a, 1, in);
	}
	else if (c == 'D')
	{
		start(a, 1.0, in);
	}
	else if (c == 'S')
	{
		string s;
		start(a, s, in);
	}
}

void checkType(fstream& in)
{
	char c;
	in >> c;

	if (c == 'I')
	{
		createType(1, in);
	}
	else if (c == 'D')
	{
		createType(1.0, in);
	}
	else if (c == 'S')
	{
		string s;
		createType(s, in);
	}
}


int main()
{
	fstream in("input.txt");

	checkType(in);

	in.close();

	return 0;
}