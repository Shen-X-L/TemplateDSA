#pragma once
#include <cstdint>
class BloomFilter {
private:
	uint64_t _num_items;      // 已插入元素数量
	uint64_t _num_size;       // 总数组长度
	uint32_t _num_hashes;     // 哈希函数次数
	uint32_t _bits_item;      // 每个元素分配的位数(bit位)
	uint64_t _byte_size;      // 位数组长度(字节个数)
	unsigned char* _data;     // 位数组
	// 使用率 = _num_items * _bits_per_item / _byte_size * 8
	//加盐值
	static constexpr uint32_t SALT_32_A = 0x9747b28c;
	static constexpr uint32_t SALT_32_B = 0x1c3c3c3c;
	static constexpr uint64_t SALT_64_A = 0xc6a4a7935bd1e995ULL;
	static constexpr uint64_t SALT_64_B = 0x85EBCA6B7F4A7C15ULL;
private:
	struct Index {
	public:
		uint64_t _byte_index;  //字节数组索引
		uint32_t _bits_offset; //bit偏移量
		unsigned char _mask;   //位掩码
		unsigned char _one;    //对应位的1
		/**
		*	@param bits_item  每个元素分配的位数
		*	@param num_size   总数组长度
		*	@param hash_index 哈希索引
		*/
		Index(uint32_t bits_item, uint64_t num_size, uint64_t hash_index);
	};
private:
	/** 对每个成员进行初始化构造
	*	@param num_size 总存储元素个数
	*	@param num_hashes 应用的哈希函数次数
	*	@param bits_item 每个元素分配的位数
	*/
	void initialize(uint64_t num_size, uint32_t num_hashes, uint32_t bits_item);
public:
	/**
	*	@param num_items 预计存储元素个数
	*	@param error 误判率
	*	@param bits_per_item 位数溢出概率
	*/
	BloomFilter(uint64_t num_items, double error, double overflow);
	/**
	*	@param num_items 预计存储元素个数
	*/
	BloomFilter(uint64_t num_items);
	~BloomFilter();
	BloomFilter(const BloomFilter&) = delete;
	BloomFilter& operator=(const BloomFilter&) = delete;
private:
	uint32_t _hash_32(const void* key, size_t len, uint32_t seed);
	uint64_t _hash_64(const void* key, size_t len, uint64_t seed);
public:
	//误判率
	double error();
	//寻找元素
	bool find(const void* buffer, size_t len);
	//插入元素
	void insert(const void* buffer, size_t len);
	//擦除元素
	void erase(const void* buffer, size_t len);
	//是否为空
	bool empty();
	//元素个数
	uint64_t size();
public:
	//清空位数组
	void clear();
};
//向上取整到最近的 8 的倍数
uint32_t align_up_to8(uint32_t num);
//返回大于等于大于或等于输入值的最小 2 的幂
uint32_t next_pow2_ge(uint32_t x);
uint64_t next_pow2_ge(uint64_t x);
/** 计算最佳的元素数量 m
*	@param n 预计插入元素数量
*	@param p 误判率
*	@return 位图元素数量m
*/
uint64_t calculate_optimal_m(uint64_t n, double p);
/** 计算应用的哈希函数数量
*	@param n 预计插入元素数量
*	@param m 位图元素数量m
*	@return 应用的哈希函数数量
*/
uint32_t calculate_optimal_k(uint64_t m, uint64_t n);
/** 返回覆盖 99.7% 溢出概率情况下的最小的b
*	@param lmd 平均每个元素命中次数 lmd = kn/m
*/
uint32_t calculate_optimal_b(double lmd);
/** 返回自定义溢出概率p情况下的最小的b
*	@param avg_hits 平均每个元素命中次数 average_hits = k*n/m
*	@param p        自定义最大溢出概率
*/
uint32_t calculate_optimal_b(double avg_hits, double p);


