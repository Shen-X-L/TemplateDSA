#pragma once
#include <cstdint>
class BloomFilter {
private:
	uint64_t _num_items;      // �Ѳ���Ԫ������
	uint64_t _num_size;       // �����鳤��
	uint32_t _num_hashes;     // ��ϣ��������
	uint32_t _bits_item;      // ÿ��Ԫ�ط����λ��(bitλ)
	uint64_t _byte_size;      // λ���鳤��(�ֽڸ���)
	unsigned char* _data;     // λ����
	// ʹ���� = _num_items * _bits_per_item / _byte_size * 8
	//����ֵ
	static constexpr uint32_t SALT_32_A = 0x9747b28c;
	static constexpr uint32_t SALT_32_B = 0x1c3c3c3c;
	static constexpr uint64_t SALT_64_A = 0xc6a4a7935bd1e995ULL;
	static constexpr uint64_t SALT_64_B = 0x85EBCA6B7F4A7C15ULL;
private:
	struct Index {
	public:
		uint64_t _byte_index;  //�ֽ���������
		uint32_t _bits_offset; //bitƫ����
		unsigned char _mask;   //λ����
		unsigned char _one;    //��Ӧλ��1
		/**
		*	@param bits_item  ÿ��Ԫ�ط����λ��
		*	@param num_size   �����鳤��
		*	@param hash_index ��ϣ����
		*/
		Index(uint32_t bits_item, uint64_t num_size, uint64_t hash_index);
	};
private:
	/** ��ÿ����Ա���г�ʼ������
	*	@param num_size �ܴ洢Ԫ�ظ���
	*	@param num_hashes Ӧ�õĹ�ϣ��������
	*	@param bits_item ÿ��Ԫ�ط����λ��
	*/
	void initialize(uint64_t num_size, uint32_t num_hashes, uint32_t bits_item);
public:
	/**
	*	@param num_items Ԥ�ƴ洢Ԫ�ظ���
	*	@param error ������
	*	@param bits_per_item λ���������
	*/
	BloomFilter(uint64_t num_items, double error, double overflow);
	/**
	*	@param num_items Ԥ�ƴ洢Ԫ�ظ���
	*/
	BloomFilter(uint64_t num_items);
	~BloomFilter();
	BloomFilter(const BloomFilter&) = delete;
	BloomFilter& operator=(const BloomFilter&) = delete;
private:
	uint32_t _hash_32(const void* key, size_t len, uint32_t seed);
	uint64_t _hash_64(const void* key, size_t len, uint64_t seed);
public:
	//������
	double error();
	//Ѱ��Ԫ��
	bool find(const void* buffer, size_t len);
	//����Ԫ��
	void insert(const void* buffer, size_t len);
	//����Ԫ��
	void erase(const void* buffer, size_t len);
	//�Ƿ�Ϊ��
	bool empty();
	//Ԫ�ظ���
	uint64_t size();
public:
	//���λ����
	void clear();
};
//����ȡ��������� 8 �ı���
uint32_t align_up_to8(uint32_t num);
//���ش��ڵ��ڴ��ڻ��������ֵ����С 2 ����
uint32_t next_pow2_ge(uint32_t x);
uint64_t next_pow2_ge(uint64_t x);
/** ������ѵ�Ԫ������ m
*	@param n Ԥ�Ʋ���Ԫ������
*	@param p ������
*	@return λͼԪ������m
*/
uint64_t calculate_optimal_m(uint64_t n, double p);
/** ����Ӧ�õĹ�ϣ��������
*	@param n Ԥ�Ʋ���Ԫ������
*	@param m λͼԪ������m
*	@return Ӧ�õĹ�ϣ��������
*/
uint32_t calculate_optimal_k(uint64_t m, uint64_t n);
/** ���ظ��� 99.7% �����������µ���С��b
*	@param lmd ƽ��ÿ��Ԫ�����д��� lmd = kn/m
*/
uint32_t calculate_optimal_b(double lmd);
/** �����Զ����������p����µ���С��b
*	@param avg_hits ƽ��ÿ��Ԫ�����д��� average_hits = k*n/m
*	@param p        �Զ�������������
*/
uint32_t calculate_optimal_b(double avg_hits, double p);


