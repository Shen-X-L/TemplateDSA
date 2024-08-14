#pragma once
#include "TheTreeHead.h"


template<typename T>
class Tree {
protected:
	T element;//����Ԫ��
	unsigned int degree = NULL;//�� ���ӽڵ�����
	Tree** child = nullptr;//���ӽڵ�
public:
	Tree() {}
	//�������ڵ�
	Tree(const T& in) :element(in) {}
	//���Ӷ���ӽڵ�
	Tree(const T& in, std::initializer_list<Tree*> inChildPtr) 
		:element(in), degree(inChildPtr.size()), child(degree ? new Tree * [degree], nullptr) {
		size_t i = 0;
		for (const T* it = inChildPtr.begin(); it != inChildPtr.end(); ++it, ++i) {
			child[i] = *it;
		}
	}
	//��������ӽڵ�
	Tree(const T& in, std::initializer_list<T> inChild)
		:element(in), degree(inChild.size()), child(degree ? new Tree * [degree], nullptr) {
		size_t i = 0;
		for (const T* it = inChild.begin(); it != inChild.end(); ++it, ++i) {
			child[i] = new Tree(*it);
		}
	}
	//������ȫN����
	Tree(std::initializer_list<T> inChild, int maxDegree){

	}
	//������
	Tree(const Tree& other) 
		:element(other.element), degree(other.degree), child(degree ? new Tree* [degree] : nullptr) {
		for (unsigned int i = 0; i < degree; ++i) {
			child[i] = new Tree(*(other.child[i]));
		}
	}
	//�ƶ���
	Tree(Tree&& other)
		:element(std::move(other.element)), child(other.child), degree(other.degree) {
		other.child = nullptr;
		other.degree = NULL;
	}
	//��������ӽڵ�
	~Tree() {
		for (unsigned int i = 0; i < degree; ++i) {
			delete child[i];
		}
		delete[] child;
	}
	//������ֵ����
	Tree& operator=(const Tree& other) {
		if (this != &other) {
			for (unsigned int i = 0; i < degree; ++i) {
				delete child[i];
			}
			delete[] child;
			element = other.element;
			degree = other.degree;
			child = degree ? new Tree* [degree] : nullptr;
			for (unsigned int i = 0; i < degree; ++i) {
				child[i] = new  Tree(*(other.child[i]));
			}
		}
		return *this;
	}
	//�ƶ���ֵ����
	Tree& operator=(Tree && other)noexcept {
	if (this != &other) {
		element = std::move(other.element);
		child = other.child;
		degree = other.degree;
		other.child = nullptr;
		other.degree = NULL;
		}
		return *this;
	}
	//�������
	size_t depthRecursive() const {
		size_t maxChildDepth = 0;
		size_t tempDepth = 0;
		for (unsigned int i = 0; i < degree; ++i) {
			tempDepth = child[i]->depthRecursive();
			maxChildDepth = maxChildDepth > tempDepth ? maxChildDepth : tempDepth;
		}
		return maxChildDepth + 1;
	}
	//���ظýڵ�Ķ�
	inline size_t getDegree() const {
		return degree;
	}
	//Ԫ������
	size_t sizeRecursive() const {
		size_t size = 1;
		for (unsigned int i = 0; i < degree; ++i) {
			size += child[i]->sizeRecursive();
		}
		return size;
	}
	template<template<typename>class Stack>
	size_t sizeIterative() const {
		size_t size = 0;
		Stack<Tree*> stack;
		Tree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr) {
				++size;
				for (unsigned int i = 0; i < ptr->degree; ++i) {
					stack.push(ptr->child[i]);
				}
				ptr = stack.top();
				stack.pop();
			}
			else {//�սڵ�,����ջ
				ptr = stack.top();
				stack.pop();
			}
		}
		return size;
	}
	//���ظ������Ķ�
	size_t maxDegree() {
		size_t max = degree;
		for (unsigned int i = 0; i < degree; ++i) {
			size_t temp= child[i]->maxDegree();
			max = max > temp ? max : temp;
		}
		return max;
	}
	//���ص�index���ӽڵ�
	Tree* getIndexChild(size_t index) {
		if (index < degree) {
			return child[index];
		}
		return nullptr;
	}
	//������ڵ������� ���ظ��ڵ�
	Tree* getParent(Tree* other) {
		if (other == this) {
#ifdef DEBUG
			throw std::out_of_range("Cannot find roots parent");
#endif // DEBUG
			return nullptr;
		}
		Stack<Tree*> stack;
		Tree* ptr = this, prev = nullptr;
		while (ptr != nullptr || !stack.empty()) {
			if (ptr != nullptr && ptr != prev) {
				stack.push(ptr);
				ptr = ptr->lChild;//������ڵ�
			}
			else {//�սڵ�,���˵�ջ���������ҽڵ�
				prev = stack.top();
				if (ptr == prev->rChild) {
					ptr = prev;
					stack.pop();
					if (ptr == other) {
						return stack.top();
					}
				}
				else
					ptr = ptr->rChild;
			}
		}
		return nullptr;
	}
	//������ڵ������� �����ֽڵ����� ��������
	Tree* getLeftSibling(Tree* other,size_t& index) {

	}
	//���������Ƿ���valueԪ��
	Tree* find(const T& value) {

	}
	//ǰ�����
	/*
	a3
	b4		c0		d2
	e1f0g0h2		i3j1
	k1	  l0m0		n0o0p0 q0
	r0
	ptr=a
	fun(a)	stack=b,c,d	ptr=d	stack=b,c
	fun(d)	stack=b,c,i,j	ptr=j	stack=b,c,i
	fun(j)	stack=b,c,i,q	ptr=q	stack=b,c,i
	fun(q)	stack=b,c,i	ptr=i	stack=b,c,n,o,p
	fun(i)	stack=b,c,n,o,p	ptr=p	stack=b,c,n,o
	fun(p)	stack=b,c,n,o	ptr=o	stack=b,c,n
	fun(o)	stack=b,c,n	ptr=n	stack=b,c
	fun(n)	stack=b,c	ptr=c	stack=b
	fun(c)	stack=b	ptr=b	stack=e,f,g,h
	fun(b)	stack=e,f,g,h	ptr=h	stack=e,f,g,l,m
	fun(h)	stack=e,f,g,l,m	ptr=m	stack=e,f,g,l
	fun(m)	stack=e,f,g,l	ptr=l	stack=e,f,g
	fun(l)	stack=e,f,g	ptr=g	stack=e,f
	fun(g)	stack=e,f	ptr=f	stack=e
	fun(f)	stack=e	ptr=e	stack=k
	fun(e)	stack=k	ptr=k	stack=r
	fun(k)	stack=r	ptr=r	stack=
	fun(r)	stack= end
	*/
	template <template<typename V>class Stack, typename Function>
	void _preOrderIterativeWithFunctionTraverse(Function func) {
		Stack<Tree*> stack;
		Tree* ptr = this;
		while (ptr != nullptr || !stack.empty()) {
			func(ptr->element);//���뱾�ڵ�
			for (unsigned int i = 0; i < ptr->degree; ++i) {
				stack.push(ptr->child[i]);
			}
			if (stack.empty()) { break; }
			ptr = stack.top();
			stack.pop();
		}
		return;
	}
};