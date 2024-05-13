#pragma once

#include <algorithm>
#include <iterator>
#include <iostream>
#include <exception>
#include <cmath>
#include <vector>

template<typename K, typename T>
struct Pair {
	K key;
	T value;
	Pair<K, T>* next;

	Pair<K, T>(const K& key, const T& value) : key(key), value(value), next(nullptr) {};
};

template<typename K>
struct HashFunction {
	size_t operator()(const K& key) const {
		return size_t(key);
	}
};

template<typename K, typename T, typename Hash = HashFunction<K>>
class HashTable {
private:
	size_t _size;
	Pair<K, T>** _buskets;
	Hash hash;

	size_t multiplicative_hash(const K& key) const {
		double n;

		double machine_word_and_key = (double(677) / sizeof(size_t) * key);
		double modf = std::modf(machine_word_and_key, &n);
		return _size * modf;
	}

	void copy(const HashTable<K, T, Hash>& other) {
		for (size_t i(0); i < _size; ++i) {
			Pair<K, T>* tmp = other._buskets[i];
			while (tmp) {
				insert(tmp->key, tmp->value);
				tmp = tmp->next;
			}
		}
	}

public:

	HashTable<K, T, Hash>() {
		_size = 10;
		_buskets = new Pair<K, T>* [_size];
		for (size_t i(0); i < _size; ++i) {
			_buskets[i] = nullptr;
		}
	}

	HashTable<K, T, Hash>(const size_t& size) {
		_size = size;
		_buskets = new Pair<K, T>* [_size];
		for (size_t i(0); i < size; ++i) {
			_buskets[i] = nullptr;
		}
	}

	HashTable<K, T, Hash>(const size_t& size, const unsigned& seed) {
		_size = size;
		_buskets = new Pair<K, T>* [_size];
		for (size_t i(0); i < size; ++i) {
			_buskets[i] = nullptr;
		}

		srand(seed);
		size_t count = 0;
		while (count != 256) {
			unsigned x = unsigned(rand() % 256);
			if (!this->contains(x)) {
				this->insert(count, x);
				++count;
			}
		}
	}

	HashTable<K, T, Hash>(const HashTable<K, T, Hash>& other) {
		_size = other._size;
		_buskets = new Pair<K, T>* [_size];
		for (size_t i(0); i < _size; ++i) {
			_buskets[i] = nullptr;
		}
		copy(other);
	}

	~HashTable<K, T, Hash>() {
		clear();
		delete[] _buskets;
	}

	HashTable<K, T, Hash>& operator=(const HashTable<K, T, Hash>& other) {
		if (&other == this) {
			return *this;
		}
		clear();
		delete[] _buskets;
		_size = other._size;
		_buskets = new Pair<K, T>* [_size];
		for (size_t i(0); i < _size; ++i) {
			_buskets[i] = nullptr;
		}
		copy(other);
		return *this;
	}

	void clear() {
		Pair<K, T>* tmp = nullptr;
		for (size_t i(0); i < _size; ++i) {
			tmp = _buskets[i];
			while (tmp) {
				auto tmp_next = tmp->next;
				delete tmp;
				tmp = tmp_next;
			}
		}
	}

	void insert(const K& key, const T& value) const {
		size_t index = multiplicative_hash(key);
		auto tmp = _buskets[index];
		if (!tmp) {
			_buskets[index] = new Pair<K, T>(key, value);
			return;
		}
		while (tmp) {
			if (tmp->next) {
				tmp = tmp->next;
			}
			else {
				tmp->next = new Pair<K, T>(key, value);
				return;
			}
		}
	}

	void insert_or_assign(const K& key, const T& value) const {
		size_t index = multiplicative_hash(key);
		auto tmp = _buskets[index];
		if (!tmp) {
			_buskets[index] = new Pair<K, T>(key, value);
			return;
		}

		while (tmp) {
			auto tmp_del = tmp;
			tmp = tmp->next;
			delete tmp_del;
		}

		_buskets[index] = new Pair<K, T>(key, value);
		return;
	}


	bool contains(const T& value) const {
		for (size_t index(0); index < _size; ++index) {
			auto tmp = _buskets[index];
			while (tmp) {
				if (tmp->value == value) {
					return true;
				}
				tmp = tmp->next;
			}
		}
		return false;
	}

	T* search(const K& key) {
		size_t index = multiplicative_hash(key);
		auto tmp = _buskets[index];
		if (tmp == nullptr) {
			return nullptr;
		}
		while (tmp) {
			if (tmp->key == key) {
				return &tmp->value;
			}
			tmp = tmp->next;
		}
		return nullptr;
	}

	bool erase(const K& key) const {
		size_t index = multiplicative_hash(key);
		auto tmp = _buskets[index];
		if (tmp == nullptr) {
			return false;
		}
		if (tmp->key == key) {
			_buskets[index] = tmp->next;
			delete tmp;
			return true;
		}
		Pair<K, T>* prev = nullptr;
		while (tmp) {
			if (tmp->key == key) {
				prev->next = tmp->next;
				delete tmp;
				return true;
			}
			prev = tmp;
			tmp = tmp->next;
		}
		return false;
	}

	int count(const K& key) {
		size_t index = multiplicative_hash(key);
		auto tmp = _buskets[index];
		while (tmp) {
			if (tmp->key == key) {
				return 1;
			}
			tmp = tmp->next;
		}
		return 0;
	}

	void print() const {
		Pair<K, T>* tmp = nullptr;
		for (size_t i(0); i < _size; ++i) {
			tmp = _buskets[i];
			while (tmp) {
				std::cout << tmp->key << " : " << tmp->value << std::endl;
				tmp = tmp->next;
			}
		}
	}
};


HashTable<int, int> count_numbers(const std::vector<int>& vec)
{
	HashTable<int, int> table(vec.size());

	for (int i = 0; i < vec.size(); ++i) {
		table.insert_or_assign(vec[i], std::count(std::cbegin(vec), std::cend(vec), vec[i]));
	}
	return table;
}

void print_numbers_count(HashTable<int, int>&table, int size){
	for (int i = 0; i < size; ++i) {
		int* count_ptr = table.search(i);
		if (count_ptr != nullptr) {
			std::cout << "Number " << i << " counts " << *count_ptr << " times" << std::endl;
		}
	}
}