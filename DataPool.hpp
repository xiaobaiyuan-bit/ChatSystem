#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <semaphore.h>

class DataPool
{
	private:
		std::vector<std::string> pool;//环形队列
		int cap;//环形队列容量
		sem_t data_sem;//数据信号量
		sem_t blank_sem;//空格信号量

		int product_step;
		int consume_step;

	public:
		DataPool(int _cap = 512)
			 :cap(_cap)
			 ,pool(512)
		{
			sem_init(&data_sem, 0, 0);//信号量初始化，数据0个
			sem_init(&blank_sem, 0, cap);//信号量初始化，空格cap个
			product_step = 0;
			consume_step = 0;
		}

		void PutMessage(const std::string& _msg)//往数据池放消息
		{
			sem_wait(&blank_sem);//p操作，获取资源，--
			pool[product_step] = _msg;
			product_step++;
			product_step %= cap;//循环队列
			sem_post(&data_sem);//v操作，释放资源，++
		}

		void GetMessage(std::string& msg)//从数据池取消息
		{
			sem_wait(&data_sem);//p操作，获取资源，--
			msg = pool[consume_step];
			consume_step++;
			consume_step %= cap;
			sem_post(&blank_sem);//v操作，释放资源，++
		}

		~DataPool()
		{
			sem_destroy(&data_sem);//信号量销毁
			sem_destroy(&blank_sem);//信号量销毁
		}

};
