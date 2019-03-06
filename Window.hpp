#pragma once

#include <iostream>
#include <string>
#include <cstring>//string.h
#include <ncurses.h>//界面库
#include <pthread.h>//刷新页面的时候需要加锁，因为多线程环境中，不加锁可能会花屏

class Window
{
	private:
		WINDOW *header;
		WINDOW *output;
		WINDOW *online;
		WINDOW *input;
		pthread_mutex_t _lock;

	public:
		Window()
		{
			initscr();//初始化界面
			curs_set(0);
			pthread_mutex_init(&_lock, NULL);

		}

		void Safewrefresh(WINDOW *w)
		{
			pthread_mutex_lock(&_lock);
			wrefresh(w);
			pthread_mutex_unlock(&_lock);
		}

		void DrawHeader()
		{
			int h = LINES/5;//高
			int w = COLS;//宽
			int y = 0;//行
			int x = 0;//列

			header = newwin(h, w, y, x);//开一个窗口，高，宽，坐标
			box(header, 0, 0);//绘制边界
			Safewrefresh(header);//刷新界面
		}

		void DrawOutput()
		{
			int h = LINES*3/5;//高
			int w = COLS*3/4;//宽
			int y = LINES/5;//行
			int x = 0;//列

			output = newwin(h, w, y, x);//开一个窗口，高，宽，坐标
			box(output, 0, 0);//绘制边界
			Safewrefresh(output);//刷新界面
		}

		void DrawOnline()
		{
			int h = LINES*3/5;//高
			int w = COLS/4;//宽
			int y = LINES/5;//行
			int x = COLS*3/4;//列

			online = newwin(h, w, y, x);//开一个窗口，高，宽，坐标
			box(online, 0, 0);//绘制边界
			Safewrefresh(online);//刷新界面
		}

		void DrawInput()
		{
			int h = LINES/5;//高
			int w = COLS;//宽
			int y = LINES*4/5;//行
			int x = 0;//列

			input = newwin(h, w, y, x);//开一个窗口，高，宽，坐标
			box(input, 0, 0);//绘制边界
			const std::string tips = "Please Enter## ";
		    PutMessageToWin(input, 2, 1, tips);
			Safewrefresh(input);//刷新界面
		}

		void GetMessageFromInput(std::string& message)
		{
			char buff[1024];
			memset(buff, 0, sizeof(buff));
			wgetnstr(input, buff, sizeof(buff));
			message = buff;
			delwin(input);
			DrawInput();
		}

		void PutMessageToWin(WINDOW *w, int y, int x, const std::string& message)
		{
			mvwaddstr(w, y, x, message.c_str());//把字符串写入到界面中
			Safewrefresh(w);//刷新窗口
		}

		void PutMessageToOutput(const std::string& message)
		{
			static int line = 1;
			int y, x;
			getmaxyx(output, y, x);
			if(line > (y - 2)){
				delwin(output);
				DrawOutput();
				line = 1;
			}
			PutMessageToWin(output, line++, 1, message);
		}

		//把在线用户列表放到online窗口中
		void PutListToOnline(std::vector<std::string>& online_user)
		{
			int i = 0;
			int size = online_user.size();
			for(i; i < size; i++){
				PutMessageToWin(online, i + 1, 2, online_user[i]);
			}
		}

		void Welcome()
		{
			const std::string welcome = "welcome to my chatsystem~";
			int num = 1;
			int dir = 0;//0 左到右    1 右到左
			int y, x;
			while(1)
			{
				DrawHeader();//画头窗口
				getmaxyx(header, y, x);//得到头窗口的高和宽
				PutMessageToWin(header, y/2, num, welcome);
				if(num >= (x - welcome.size() - 2)){
					dir = 1;
				}
				if(num <= 1){
					dir = 0;
				}
				if(dir == 0){
					num++;
				}else{
					num--;
				}

				usleep(400000);
				delwin(header);
			}
		}

		~Window()
		{
			endwin();//结束界面
			pthread_mutex_destroy(&_lock);
		}
};
