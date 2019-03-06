#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include "protocolUtil.hpp"
#include "Message.hpp"
#include "Window.hpp"

#define TCP_PORT 8080
#define UDP_PORT 8888

class ChatClient;

struct ParamPair
{
	Window *wp;
	ChatClient *cp;
};

class ChatClient
{
    private:
        std::string peer_ip;//服务器的ip地址
        int tcp_sock;//tcp套接字
        int udp_sock;//udp套接字

        std::string passwd;//密码
		struct sockaddr_in server;
	public:
        unsigned int id;//qq号
        std::string nick_name;//昵称
        std::string school;//学校

    public:
        ChatClient(std::string _ip)
            :peer_ip(_ip)
            ,tcp_sock(-1)
            ,udp_sock(-1)
            ,id(0)
        {
			server.sin_family = AF_INET;
			server.sin_port = htons(UDP_PORT);
			server.sin_addr.s_addr = inet_addr(peer_ip.c_str());
		}

        void InitClient()//初始化客户端
        {
            udp_sock = SocketApi::Socket(SOCK_DGRAM);
        }

        bool ConnectServer()
        {
            tcp_sock = SocketApi::Socket(SOCK_STREAM);//易错点！！！注册登录都要创建它
            return SocketApi::Connect(tcp_sock, peer_ip, TCP_PORT);
        }

        bool Register()//注册
        {
            if(Util::RegisterEnter(nick_name, school, passwd) && ConnectServer()){

                Request rq;//模仿http协议向server发起请求

                rq.method = "REGISTER\n";//方法：注册
                Json::Value root;
                root["name"] = nick_name;
                root["school"] = school;
                root["passwd"] = passwd;
                Util::Serialize(root, rq.text);//正文序列化【把几个字符串合并为一个字符串】

                rq.content_length = "Content_Length: ";//正文长度
                rq.content_length += Util::IntToString((rq.text).size());
                rq.content_length += '\n';

                Util::SendRequest(tcp_sock, rq);//发送注册数据先到tcp发送缓冲区
                recv(tcp_sock, &id, sizeof(id), 0);
                bool res = false;
                if(id >= 5000){
                    std::cout << "Register success! ";
                    std::cout << "Your ID is: " << id << std::endl;
                    res = true;
                }else{
                    std::cout << "Register Failed! Code is: " << id << std::endl;
                }

                close(tcp_sock);
                return res;
            }
        }

        bool Login()//登录
        {
            if(Util::LoginEnter(id, passwd) && ConnectServer()){

                Request rq;//模仿http协议向server发起请求

                rq.method = "LOGIN\n";//方法：登录
                Json::Value root;
                root["id"] = id;
                root["passwd"] = passwd;
                Util::Serialize(root, rq.text);//正文序列化【把几个字符串合并为一个字符串】

                rq.content_length = "Content_Length: ";//正文长度
                rq.content_length += Util::IntToString((rq.text).size());
                rq.content_length += '\n';

                Util::SendRequest(tcp_sock, rq);//发送登录数据先到tcp发送缓冲区
                unsigned int result = 0;
                recv(tcp_sock, &result, sizeof(result), 0);
                bool res = false;
                if(result >= 5000){
                    std::cout << "Login success! " << std::endl;
                    res = true;
                }else{
                    std::cout << "Login Failed! Code is: " << result << std::endl;
                }

                close(tcp_sock);
                return res;
            }
        }

		void UdpSend(std::string& message_)
		{
			Util::SendMessage(udp_sock, message_, server);
		}

		void UdpRecv(std::string& _str)
		{
			struct sockaddr_in peer;
			Util::RecvMessage(udp_sock, _str, peer);//peer可以保存发送方的ip和port
		}

		static void *RunWelcome(void *arg)
		{
			pthread_detach(pthread_self());
			Window *wp = (Window *)arg;
			wp->Welcome();
		}

		static void *RunInput(void *arg)
		{
			pthread_detach(pthread_self());
			struct ParamPair *p = (struct ParamPair *)arg;
			Window *wp = p->wp;
			ChatClient *cp = p->cp;
			wp->DrawInput();//刷出聊天输入框

			std::string textstring;
			std::string _message;

			std::string s("i am onlning...");//登录后首先发一句我在线，就可以加入在线列表
			std::string _s;
			Message m(cp->nick_name, cp->school, s, cp->id);//要发送的信息
		    m.ToSendString(_s);//把发送的信息序列化进message字符串里
			cp->UdpSend(_s);
			for(;;){
				wp->GetMessageFromInput(textstring);//取出输入的消息放到text中
				Message msg(cp->nick_name, cp->school, textstring, cp->id);//要发送的信息
		        msg.ToSendString(_message);//把发送的信息序列化进message字符串里
				cp->UdpSend(_message);
			}
		}

        void Chat()//聊天
        {

			Window w;
			pthread_t h, l;
			struct ParamPair pp = {&w, this};
			pthread_create(&h, NULL, RunWelcome, &w);//欢迎窗口
			pthread_create(&l, NULL, RunInput, &pp);//输入窗口

			w.DrawOutput();//窗口记得画
			w.DrawOnline();//窗口要先画
			std::string str;
			std::string showString;

			std::vector<std::string> online;//在线列表显示
			for(;;){
				Message msg;
				UdpRecv(str);//主线程接收消息
				msg.ToRecvString(str);//把接收的字符串反序列化
				showString = msg.NickName();
				showString += '-';
				showString += msg.School();

				std::string f = showString;//zhangsan-xigong
				Util::Add_User(online, f);//把在线用户字符串加入到online中


				showString += "## ";//zhangsan-xigong##
				showString += msg.Text();
				w.PutMessageToOutput(showString);

				w.PutListToOnline(online);//把在线用户列表放到online窗口中
			}
		}

		~ChatClient()
		{}
};
