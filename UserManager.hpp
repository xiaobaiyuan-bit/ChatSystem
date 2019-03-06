#pragma once//用户管理信息

#include <iostream>
#include <string>
#include <map>
#include <pthread.h>

class User
{
    private:
        std::string nick_name;//昵称
        std::string school;//学校
        std::string passwd;//密码
    public:
        User(const std::string& _n, const std::string& _s, const std::string& _pwd)
            :nick_name(_n)
            ,school(_s)
            ,passwd(_pwd)
        {}

		bool IsPasswdOk(const std::string& _p)
		{
			return passwd == _p ? true : false;
		}

        ~User()
        {}
};

class UserManager
{
    private:
        unsigned int assign_id;//指定QQ号
        std::map<unsigned int, User> users;//所有注册用户信息
        std::map<unsigned int, struct sockaddr_in> online_users;//在线用户信息
		pthread_mutex_t lock;//可能多人同时注册，所以要加锁保护临界资源

    public:
        UserManager()
            :assign_id(5000)
        {
			pthread_mutex_init(&lock, NULL);//初始化锁
		}

		unsigned int Insert(const std::string& _n,\
				const std::string& _s, const std::string& _p)
		{
			pthread_mutex_lock(&lock);//加锁

			unsigned int id = assign_id++;//用户id
			User u(_n, _s, _p);//用户个人信息
			if(users.find(id) == users.end())
			{
				users.insert(std::pair<unsigned int, User>(id, u));//插入map中
				pthread_mutex_unlock(&lock);
				return id;
			}
			pthread_mutex_unlock(&lock);//解锁
			return 1;//注册错误码
		}

		unsigned int Check(const unsigned int& id, const std::string& passwd)
		{
			pthread_mutex_lock(&lock);
			if((users.find(id)) != (users.end()))
			{
				User& u = (users.find(id))->second;//找到map中key所对应的value值
				if(u.IsPasswdOk(passwd))
				{
					pthread_mutex_unlock(&lock);
					return id;
				}
			}
			pthread_mutex_unlock(&lock);
			return 2;//返回登录错误码
		}
		
		void AddOnlineuser(const unsigned int& _id, const struct sockaddr_in& _peer)
		{
			pthread_mutex_lock(&lock);
			//检查该用户在不在在线列表里面， 不在就添加
			if(online_users.find(_id) == online_users.end())
			{
				online_users.insert(std::pair<unsigned int, \
						struct sockaddr_in>(_id, _peer));
			}
			pthread_mutex_unlock(&lock);
		}

		std::map<unsigned int, struct sockaddr_in> OnlineUser()
		{
			//pthread_mutex_lock(&lock);
		    //std::map<unsigned int, struct sockaddr_in> Online = online_users;
            return online_users;//在线用户信息
			//pthread_mutex_unlock(&lock);
			//return Online;
		}

        ~UserManager()
        {
			pthread_mutex_destroy(&lock);//销毁锁
		}
};

