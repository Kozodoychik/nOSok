#pragma once

template<typename T>
class Atomic {
public:
	Atomic() = default;
	Atomic(T value): val(value) {}

	inline T load(int order) const volatile {
		return __atomic_load_n(&val, order);
	}

	inline void store(T value, int order) volatile {
		__atomic_store_n(&val, value, order);
	}

private:
	T val;
};