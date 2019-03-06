#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "json/json.h"//序列化与反序列化库
#include "Log.hpp"

#define BACKLOG 5
#define MESSAGE_SIZE 1024

class Request//自定应用层协议发起请求
{
    public:
        std::string method;//REGISTER  LOGIN  LOGOUT
        std::string content_length;//正文长度
        std::string blank;//空格
        std::string text;//正文

    public:
        Request()
            :blank("\n")
        {}
        ~Request()
        {}
};

class Util//注册步骤信息
{
    public:
        static bool RegisterEnter(std::string& _n, std::string& _s, std::string& _p)
        {
            std::cout << "Please Enter your Nick Name:> ";
            std::cin >> _n;
            std::cout << "Please Enter your School:> ";
            std::cin >> _s;
            std::cout << "Please Enter your Passwd:> ";
            std::cin >> _p;
            std::cout << "Please Enter your Passwd again:> ";
            std::string again;
            std::cin >> again;
            if(_p == again){
                return true;
            }
            return false;
        }

        static bool LoginEnter(unsigned int& id, std::string& passwd)
        {
            std::cout << "Please Enter Your Id: ";
            std::cin >> id;
            std::cout << "Please Enter Your Passwd: ";
            std::cin >> passwd;
	    return true;
        }

        static void Serialize(Json::Value& root, std::string& outstring)//序列化
        {
            Json::FastWriter w;
            outstring = w.write(root);
        }

        static void DeSerialize(std::string& instring, Json::Value& root)//反序列化
        {
            Json::Reader r;
            r.parse(instring, root, false);
        }

        static std::string IntToString(int x)//整型转字符串
        {
            std::stringstream ss;
            ss << x;
            return ss.str();
        }

        static int StringToInt(std::string& _str)
        {
            int x;
            std::stringstream ss(_str);
            ss >> x;
            return x;
        }

        static void RecvOneLine(int sock, std::string& outstring)
        {
            char c = 'x';
            while(1)
            {
                ssize_t s = recv(sock, &c, 1, 0);
                if((s > 0) && (c != '\n')){
                    outstring.push_back(c);
                }
                else{
                    break;
                }
            }
        }

        static void RecvRequest(int sock, Request& rq)//从sock文件中接收消息
        {
            RecvOneLine(sock, rq.method);
            RecvOneLine(sock, rq.content_length);
            RecvOneLine(sock, rq.blank);

            std::string& cl = rq.content_length;
            std::size_t pos = cl.find(": ");
            if(std::string::npos == pos){
                return;
            }
            std::string sub = cl.substr(pos+2);//街取到正文长度的字符串
            int size = StringToInt(sub);

            char c = 'x';
            size_t i = 0;
            for(i = 0; i < size; i++){
                recv(sock, &c, 1, 0);
                (rq.text).push_back(c);
            }
        }

        static void SendRequest(int sock, Request& _rq)//全部按照字节流发送
        {
            std::string& _m = _rq.method;
            std::string& _cl = _rq.content_length;
            std::string& _b = _rq.blank;
            std::string& _t = _rq.text;
            send(sock, _m.c_str(), _m.size(), 0);
            send(sock, _cl.c_str(), _cl.size(), 0);
            send(sock, _b.c_str(), _b.size(), 0);
            send(sock, _t.c_str(), _t.size(), 0);
        }

		static void RecvMessage(int sock, std::string& message, \
				struct sockaddr_in& peer)//收消息
		{
			char buff[MESSAGE_SIZE];
			memset(&buff, 0, sizeof(buff));
			socklen_t len = sizeof(peer);
			ssize_t s = recvfrom(sock, buff, sizeof(buff)-1, 0,\
					(struct sockaddr *)&peer, &len);
			if(s <= 0){
				LOG("recvfrom message error!", WARNING);
			}else{
				message = buff;
			}
		}

		static void SendMessage(int sock, const std::string& message, \
				struct sockaddr_in& peer)
		{
			sendto(sock, message.c_str(), message.size(), 0, \
					(struct sockaddr *)&peer, sizeof(peer));	
		}
				
		static void Add_User(std::vector<std::string>& online, std::string& f)
		{
			std::vector<std::string>::iterator it = online.begin();
			for(it; it != online.end(); it++){
				if((*it) == f){
					return;//找到了就直接返回
				}
			}
			online.push_back(f);//没有找到就放入vector中
		}

};


class SocketApi
{
    public:
        static int Socket(int type)
        {
            int sock = socket(AF_INET, type, 0);
            if(sock < 0){
                LOG("socket error!", ERROR);
                exit(2);
            }
        }

        static void Bind(int sock, int port)
        {
            struct sockaddr_in local;
            local.sin_family = AF_INET;//ipv4协议簇
            local.sin_addr.s_addr = htonl(INADDR_ANY);//4字节的ip地址
            local.sin_port = htons(port);//2字节的端口号

            if(bind(sock,(struct sockaddr*)&local, sizeof(local)) < 0){
                LOG("bind error!", ERROR);
                exit(3);
            }
        }

        static void Listen(int sock)
        {
            if(listen(sock, BACKLOG) < 0){
                LOG("listen error!", ERROR);
                exit(4);
            }
        }

        static int Accept(int listen_sock, std::string& out_ip, int& out_port)
        {
            struct sockaddr_in peer;//对端的套接字地址
            socklen_t len = sizeof(peer);//地址的字节大小
            int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
            if(sock < 0){
                LOG("accept error!", WARNING);//accept接收套接字出错，不影响，它可重收
                return -1;
            }
            out_ip = inet_ntoa(peer.sin_addr);
            out_port = htons(peer.sin_port);
            return sock;
        }

        //通过自己的sock套接字  连接对方的ip 和 对方的端口号
        static bool Connect(const int& sock, std::string peer_ip, const int& port)
        {
            struct sockaddr_in peer;//远方的套接字结构体
            peer.sin_family = AF_INET;
            peer.sin_addr.s_addr = inet_addr(peer_ip.c_str());
            peer.sin_port = htons(port);

            if(connect(sock, (struct sockaddr*)&peer, sizeof(peer)) < 0){
                LOG("connect error!", WARNING);
                return false;
            }
            return true;
        }
};

