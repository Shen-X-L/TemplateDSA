#include "BloomFilter.h"
#include <cmath>
#include <cstring>
#include <stdexcept>
#include <iostream> 

//编译器常量ln(2)
constexpr double LN_2 = 0.69314718056;
void BloomFilter::initialize(uint64_t num_size, uint32_t num_hashes, uint32_t bits_item) {
	_num_items = 0;
	_num_size = align_up_to8(num_size);
	_num_hashes = num_hashes;
	_bits_item = bits_item < 8 ? next_pow2_ge(bits_item) : align_up_to8(bits_item);
	_byte_size = _num_size * _bits_item / 8;
	_data = new unsigned char[_byte_size];
	memset(_data, 0, _byte_size);
}
BloomFilter::BloomFilter(uint64_t num_items, double error, double overflow) {
	// 在构造函数体内计算参数
	uint64_t optimal_m = calculate_optimal_m(num_items, error);
	uint32_t optimal_k = calculate_optimal_k(optimal_m, num_items);
	uint32_t optimal_b = calculate_optimal_b(optimal_k * num_items / optimal_m, overflow);
	// 调用初始化函数
	initialize(optimal_m, optimal_k, optimal_b);
}
BloomFilter::BloomFilter(uint64_t num_items) : BloomFilter(num_items, 0.05, 0.05) {};
BloomFilter::~BloomFilter() {
	clear();
}
uint32_t BloomFilter::_hash_32(const void* key, size_t len, uint32_t seed) {
	// Redis 中使用的常量
	const uint32_t m = 0x5bd1e995;
	const int r = 24;
	// 初始化哈希值
	uint32_t h = seed ^ static_cast<uint32_t>(len);
	// 处理 4字节 块
	const unsigned char* data = static_cast<const unsigned char*>(key);
	while (len >= 4) {
		uint32_t k = *reinterpret_cast<const uint32_t*>(data);

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}
	// 处理剩余的字节
	switch (len) {
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
		h *= m;
	}
	// 最后的混合步骤
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}
uint64_t BloomFilter::_hash_64(const void* key, size_t len, uint64_t seed) {
	const uint64_t m = 0xc6a4a7935bd1e995ULL;
	const int r = 47;

	uint64_t h = seed ^ (len * m);

	const uint64_t* data = static_cast<const uint64_t*>(key);
	const uint64_t* end = data + (len / 8);

	while (data != end) {
		uint64_t k = *data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	const unsigned char* data2 = reinterpret_cast<const unsigned char*>(data);
	switch (len & 7) {
	case 7: h ^= static_cast<uint64_t>(data2[6]) << 48;
	case 6: h ^= static_cast<uint64_t>(data2[5]) << 40;
	case 5: h ^= static_cast<uint64_t>(data2[4]) << 32;
	case 4: h ^= static_cast<uint64_t>(data2[3]) << 24;
	case 3: h ^= static_cast<uint64_t>(data2[2]) << 16;
	case 2: h ^= static_cast<uint64_t>(data2[1]) << 8;
	case 1: h ^= static_cast<uint64_t>(data2[0]);
		h *= m;
	}

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}
double BloomFilter::error() {
	return pow(1 - exp(-static_cast<double>(_num_hashes) * _num_items / _num_size),
			   _num_hashes);
}
bool BloomFilter::find(const void* buffer, size_t len) {
	uint64_t hash_key_a = _hash_64(buffer, len, BloomFilter::SALT_64_A);
	uint64_t hash_key_b = _hash_64(buffer, len, BloomFilter::SALT_64_B);
	for (int i = 0; i < _num_hashes; ++i) {
		Index index(_bits_item, _num_size, hash_key_a + hash_key_b * i);
		//元素分配的位数 < 8 时检查对应索引 
		//目标字节 & 位掩码()
		if (_bits_item < 8) {
			if ((_data[index._byte_index] & index._mask) != 0) {} else { return false; }
		} else {
			for (int j = 0; j < _bits_item / 8; ++j) {
				if (_data[index._byte_index + j] != 0) {} else { return false; }
			}
		}
	}
	return true;
}
void BloomFilter::insert(const void* buffer, size_t len) {
	uint64_t hash_key_a = _hash_64(buffer, len, BloomFilter::SALT_64_A);
	uint64_t hash_key_b = _hash_64(buffer, len, BloomFilter::SALT_64_B);
	for (int i = 0; i < _num_hashes; ++i) {
		Index index(_bits_item, _num_size, hash_key_a + hash_key_b * i);
		//元素分配的位数 < 8 时检查对应索引 
		//目标字节 & 位掩码()
		if (_bits_item < 8) {
			if ((_data[index._byte_index] & index._mask) != index._mask) {
				_data[index._byte_index] += index._one;
			} else { throw std::overflow_error("insert计数溢出"); }
		} else {
			// 查找第一个非0xFF的字节
			int j = 0;
			for (; j < _bits_item / 8 && _data[index._byte_index + j] == 0xFF;++j) {}
			// 处理溢出情况
			if (j >= _bits_item / 8) { throw std::overflow_error("insert计数溢出"); }
			// 对之前0xFF字节执行进位操作
			else {
				for (int k = 0; k <= j; ++k)
					++_data[index._byte_index + k];
			}
		}
	}
}
void BloomFilter::erase(const void* buffer, size_t len) {
	uint64_t hash_key_a = _hash_64(buffer, len, BloomFilter::SALT_64_A);
	uint64_t hash_key_b = _hash_64(buffer, len, BloomFilter::SALT_64_B);
	for (int i = 0; i < _num_hashes; ++i) {
		Index index(_bits_item, _num_size, hash_key_a + hash_key_b * i);
		//元素分配的位数 < 8 时检查对应索引 
		//目标字节 & 位掩码()
		if (_bits_item < 8) {
			if ((_data[index._byte_index] & index._mask) != 0) {
				_data[index._byte_index] -= index._one;
			} else { throw std::underflow_error("erase计数溢出"); }
		} else {
			// 查找第一个非0x00的字节
			int j = 0;
			for (; j < _bits_item / 8 && _data[index._byte_index + j] == 0x00;
				 ++j) {
			}
			// 处理溢出情况
			if (j >= _bits_item / 8) { throw std::underflow_error("erase计数溢出"); }
			// 执行进位操作
			else {
				for (int k = 0; k <= j; ++k)
					--_data[index._byte_index + k];
			}
		}
	}
}
bool BloomFilter::empty() { return _num_items == 0 ? true : false; }
uint64_t BloomFilter::size() { return _num_items; }
void BloomFilter::clear() {
	delete[] _data;
	_data = nullptr;  // 添加这行
	_num_items = 0;   // 重置计数
}
uint32_t align_up_to8(uint32_t num) {
	return (num + 7) & ~7;
}
uint32_t next_pow2_ge(uint32_t x) {
	if (x == 0) return 1;  // 0 的下一个 2 的幂是 1
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;          // 返回 2^n
}
uint64_t next_pow2_ge(uint64_t x) {
	if (x == 0) return 1;  // 0 的下一个 2 的幂是 1
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x |= x >> 32;
	return x + 1;          // 返回 2^n
}
uint64_t calculate_optimal_m(uint64_t n, double p) {
	return static_cast<uint64_t>(-(n * log(p)) / (LN_2 * LN_2));
}
uint32_t calculate_optimal_k(uint64_t m, uint64_t n) {
	return static_cast<uint32_t>(round((m / n) * LN_2));
}
uint32_t calculate_optimal_b(double lmd) {
	return static_cast<uint32_t>(ceil(log(lmd + 3 * sqrt(lmd)) / LN_2));
}
uint32_t calculate_optimal_b(double avg_hits, double p) {
	uint32_t b = 1; //返回的b
	uint64_t factorial = 1; //阶乘
	double avg_hits_power = avg_hits; //lmd的次方
	double target = (1 - p) / exp(-avg_hits); //简化计算后需要大于的常数项
	int i = 1; //循环遍历
	double sum = avg_hits_power / factorial; //累计求和
	while (sum <= target) {
		int j = (1 << b) - 1; //2^b - 1
		for (; i < j; ++i) {
			avg_hits_power *= avg_hits;
			factorial *= i;
			sum += avg_hits_power / factorial;
		}
		++b;
	}
	return next_pow2_ge(b);
}
BloomFilter::Index::Index(uint32_t bits_item, uint64_t num_size, uint64_t hash_index) {
	uint64_t num_index = hash_index % num_size;
	if (bits_item < 8) {
		_byte_index = num_index / 8 * bits_item;
		_bits_offset = num_index % (8 / bits_item) * bits_item;
		_mask = ((1 << bits_item) - 1) << _bits_offset;
		_one = 1 << _bits_offset;
	} else {
		_byte_index = bits_item / 8 * num_index;
		_bits_offset = 0;
		_mask = 0;
		_one = 1;
	}
}