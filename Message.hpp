#pragma once

#include <iostream>
#include <string>
#include "json/json.h"

class Message
{
	private:
		std::string nick_name;
		std::string school;
		std::string text;
		unsigned int id;

	public:
		Message()//函数重载
		{}

		Message(std::string& _n, std::string& _s, std::string& _t, unsigned int& _id)
			 :nick_name(_n)
			 ,school(_s)
			 ,text(_t)
			 ,id(_id)
		{}

		void ToSendString(std::string& sendstring)
		{
			Json::Value root;
			root["name"] = nick_name;
			root["school"] = school;
			root["text"] = text;
			root["id"] = id;
			Util::Serialize(root, sendstring);
		}

		void ToRecvString(std::string& recvstring)
		{
			Json::Value root;
			Util::DeSerialize(recvstring, root);
			nick_name = root["name"].asString();
			school = root["school"].asString();
			text = root["text"].asString();
			id = root["id"].asInt();
		}

		const std::string& NickName()
		{
			return nick_name;
		}

		const std::string& School()
		{
			return school;
		}

		const std::string& Text()
		{
			return text;
		}
		
		const unsigned int& Id()
		{
			return id;
		}

		~Message()
		{}
};
