#pragma once

#include <iostream>
#include <pthread.h>
#include "protocolUtil.hpp"
#include "UserManager.hpp"
#include "Log.hpp"
#include "DataPool.hpp"
#include "Message.hpp"

class ChatServer;//声明下面有这个类

class Param
{
    public:
        ChatServer* sp;
        int sock;
		std::string ip;
		int port;
    public:
        Param(ChatServer* _sp, int& _sock, std::string& _ip, int& _port)
		 	 :sp(_sp)
			 ,sock(_sock)
			 ,ip(_ip)
			 ,port(_port)
        {}

        ~Param()
        {}
};

class ChatServer{
    private:
        int tcp_listen_sock;//登录注册套接字
        int tcp_port;//登录注册端口号

        int udp_work_sock;//聊天套接字
        int udp_port;//聊天端口号

        UserManager um;
		DataPool pool;
    public:
        ChatServer(int _tcp_port = 8080, int _udp_port = 8888)
            :tcp_listen_sock(-1)
            ,tcp_port(_tcp_port)
            ,udp_work_sock(-1)
            ,udp_port(_udp_port)//初始化四个成员变量
        {}
        void InitServer()//初始化函数【创建套接字等等准备工作】
        {
            tcp_listen_sock = SocketApi::Socket(SOCK_STREAM);
            udp_work_sock = SocketApi::Socket(SOCK_DGRAM);
            SocketApi::Bind(tcp_listen_sock, tcp_port);
            SocketApi::Bind(udp_work_sock, udp_port);

            SocketApi::Listen(tcp_listen_sock);
        }	
        
		unsigned int RegisterUser(const std::string& _n,\
				const std::string& _s, const std::string& _p)
		{
			return um.Insert(_n, _s, _p);
		}
		
		unsigned int LoginUser(const unsigned int& _id, const std::string& _passwd, \
				const std::string& _ip, const int& _port)
		{
			return um.Check(_id, _passwd);
		}

		void Product()//生产数据放到数据池
		{
			std::string message;
			struct sockaddr_in peer;
		    Util::RecvMessage(udp_work_sock, message, peer);//收消息
			std::cout << "debug: recv message" << message <<std::endl; 
			if(!message.empty()){
				pool.PutMessage(message);//放到数据池
				Message m;
				m.ToRecvString(message);//反序列化，得到用户id
				um.AddOnlineuser(m.Id(), peer);//生产者放数据到数据池的时候加入在线列表
			}
		}

		void Consume()//消费数据从数据池取消息
		{
			std::string message;
			pool.GetMessage(message);
			std::cout << "debug: send message" << message <<std::endl; 
			std::map<unsigned int, struct sockaddr_in> Online(um.OnlineUser());
	//for((Online.begin()); (Online.begin()) != (Online.end()); (Online.begin())++)
			std::map<unsigned int, struct sockaddr_in>::iterator it =  Online.begin();
			for(; it != Online.end(); it++)
			{
				//Util::SendMessage(udp_work_sock, message, Online.begin()->second);
				Util::SendMessage(udp_work_sock, message, it->second);
			}
		}

        static void* HandlerRequest(void *arg)//没有this指针
        {
            pthread_detach(pthread_self());//线程分离，让系统自动回收线程所占用资源

            Param *p = (Param *)arg;
            int sock = p->sock;
            ChatServer *sp = p->sp;
			std::string ip = p->ip;
			int port = p->port;
            delete p;

            //recv:sock【接收消息】
            //利用协议分析消息，处理消息
            Request rq;
            Util::RecvRequest(sock, rq);

            Json::Value root;
            LOG(rq.text, NORMAL);

            Util::DeSerialize(rq.text, root);//正文反序列化
            if(rq.method == "REGISTER"){
                std::string name = root["name"].asString();//root里面可存放各种类型
                std::string school = root["school"].asString();//所以在这里以字符串的形式
                std::string passwd = root["passwd"].asString();//提取字符串

                //把提取的用户注册信息输入到服务器用户注册名单里，返回给用户id[qq号]
                unsigned int id = sp->RegisterUser(name, school, passwd);
                send(sock, &id, sizeof(id), 0);//发送给客户端

            }else if(rq.method == "LOGIN"){
                unsigned int id = root["id"].asInt();
                std::string passwd = root["passwd"].asString();

                unsigned int result = sp->LoginUser(id, passwd, ip, port);
                send(sock, &result, sizeof(result), 0);
            }
            else{
                ;//LOGOUT
            }

            close(sock);
        }

        void Start()
        {
            std::string ip;
            int port;
            for(;;){
                int sock = SocketApi::Accept(tcp_listen_sock, ip, port);
                if(sock > 0){
                    std::cout << "get a new client " << ip << " : " << port << std::endl;

                    Param *p = new Param(this, sock, ip, port);
                    pthread_t tid;
                    pthread_create(&tid, NULL, HandlerRequest, p);
                }
            }
        }

        ~ChatServer()
        {}
};

