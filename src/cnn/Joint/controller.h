#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include "control.h"
#include "sdk.h"

class Share {
private:
	int dimension, tot, VocabTot;
	std::vector<real *> vectorList;
	std::vector<Control *> controlList;
	std::vector<int *> numberList;
	std::vector<int *> freelist;
	std::string WordList;
	std::string name;

	void AddVocab(char *word) {
		VocabTot++;
		int *con = (int *)calloc(tot,sizeof(int));
		for (int i = 0; i < tot; i++) {
			con[i] = controlList[i]->GetWordNumber(word);
		}
		numberList.push_back(con);
	}

public:

	Share() {
		tot = 0;
		VocabTot = 0;
		vectorList.clear();
		controlList.clear();
	}

	Share(std::string name) {
		this->name = name;
		tot = 0;
		VocabTot = 0;
		vectorList.clear();
		controlList.clear();
	}

	~Share() {
		for (int i = numberList.size(); i >= 0; i--)
			free(numberList[i]);
		for (int i = 0; i < freelist.size(); i++) 
			free(freelist[i]);
		freelist.clear();	
	}

	void SetWordList(std::string route) {
		WordList = route;
	}

	void SetShare(Control *con) {
		tot++;
		dimension = con->GetDimension();
		controlList.push_back(con);
		vectorList.push_back(con->GetWordVec());
	}

	void Init() {
		FILE *fin = fopen(WordList.c_str(), "r");
		char buffer[BUFFER_SIZE];
		while (fscanf(fin,"%s",buffer)==1)
			AddVocab(buffer);
		fclose(fin);
		for (int i = 0; i < tot; i++)
			if (controlList[i]->GetAlgo() == "transE") {
				for (int j = 0; j < tot; j++)
					if (i != j) {
						int *con = (int *)calloc(VocabTot+VocabTot,sizeof(int));
						for (int k = 0; k < VocabTot; k++) {
							con[k + k] = numberList[k][i];
							con[k + k + 1] = numberList[k][j];
						}
						controlList[i] -> SetShare(controlList[j]->GetWordVec(), con, VocabTot);
						freelist.push_back(con);
					}
			} else {
				for (int j = 0; j < tot; j++)
					if (i != j) {
						int total = controlList[i] -> GetWordTotal();
						int *con = (int *)calloc(total,sizeof(int));
						for (int k = 0; k < total; k++)
							con[k] = -1;
						for (int k = 0; k < VocabTot; k++) {
							con[numberList[k][i]] = numberList[k][j];
						}
						controlList[i] -> SetShare(controlList[j]->GetWordVec(), con, total);
						freelist.push_back(con);
					}
			}
	}

	void Run(std::string name) {
		printf("share from %s\n",name.c_str());
		for (int i = 0; i < tot; i++)
			if (controlList[i]->GetName() == name) {
				for (int j = 0; j < tot; j++)
					if (i != j) {
						for (int k = 0; k < VocabTot; k++)
							if (numberList[k][j]!=-1 && numberList[k][i]!=-1)
								for (int d = 0; d < dimension; d++)
									vectorList[j][numberList[k][j] * dimension + d] = vectorList[i][numberList[k][i] * dimension + d];
					}				
				break;
			}
	}

	std::string GetName() {
		return name;
	}

};

class Controller {
private:
	std::string ConfigFile;
	std::vector <Control *> ControlList;
	std::vector <std::string> command_name;
	std::vector <std::string> command_times;
	std::vector <Share *> ShareList;

	int ArgPos(char *str, int argc, char **argv) {
		for (int a = 1; a < argc; a++)
			if (!strcmp(str, argv[a])) {
				if (a == argc - 1) return -1;
		 		return a;
		}
		return -1;
	}

	void ExplainTransE(FILE *fin) {
		Control *work = new TransEControl("transE");
		work -> Explain(fin);
		work -> Init();
		ControlList.push_back(work);
	}

	void ExplainWord2Vec(FILE *fin) {
		Control *work = new Word2VecControl("word2vec");
		work -> Explain(fin);
		work -> Init();
		ControlList.push_back(work);
	 }

	void ExplainShare(FILE *fin) {
		std::string name = "";
		while (!feof(fin)) {
			char ch = fgetc(fin);
			if (ch == '\n' || ch =='\r' || ch=='\t' || ch==' ') continue;
			if (ch=='{') break;
			name = name + ch;
		}
		Share *work = new Share(name);
		std::string cont = "";
		std::string title = "";
		std::string content = "";
		while (!feof(fin)) {
			char ch = fgetc(fin);
			if (ch == '\n' || ch =='\r' || ch=='\t' || ch==' ') continue;
			if (ch == '}') break;
			if (ch == ':') {
				title = cont;
				cont = "";
				continue;
			}
			if (ch == ';') {
				content = cont;
				if (title == "Share") work->SetShare(this->GetControlByName(content));
				if (title == "WordList") work->SetWordList(content);
				cont = "";
				continue;
			}
			cont = cont + ch;
		}
		work->Init();
		ShareList.push_back(work);
	}

	void ExplainRun(FILE *fin) {
		std::string algo = "";
		while (!feof(fin)) {
			char ch = fgetc(fin);
			if (ch == '\n' || ch =='\r' || ch=='\t' || ch==' ') continue;
			algo = algo + ch;
			if (algo == "(") break;
		}
		algo = "";
		while (!feof(fin)) {
			char ch = fgetc(fin);
			if (ch == '\n' || ch =='\r' || ch=='\t' || ch==' ') continue;
			if (ch == ',') break;
			algo = algo + ch;
		}
		command_name.push_back(algo);

		algo = "";
		while (!feof(fin)) {
			char ch = fgetc(fin);
			if (ch == '\n' || ch =='\r' || ch=='\t' || ch==' ') continue;
			if (ch == ')') break;
			algo = algo + ch;
		}
		command_times.push_back(algo);
	}

	void ExplainDo(FILE *fin) {
		std::string algo = "";
		while (!feof(fin)) {
			char ch = fgetc(fin);
			if (ch == '\n' || ch =='\r' || ch=='\t' || ch==' ') continue;
			algo = algo + ch;
			if (algo == "(") break;
		}
		algo = "";
		while (!feof(fin)) {
			char ch = fgetc(fin);
			if (ch == '\n' || ch =='\r' || ch=='\t' || ch==' ') continue;
			algo = algo + ch;
			if (ch == ')') break;
		}
		command_name.push_back("do");
		command_times.push_back(algo);
	}

	void RunThread(int l, int r) {
		std::vector<std::thread *> threadQueue;
		int last = l - 1;
		for (int i = l; i <= r; i++) 
			if (command_name[i] == "end") {
				std::thread *T = new  std::thread(std::bind(&Controller::Run, this, last + 1, i));
				threadQueue.push_back(T);
				last = i;
			}
		for (int i = 0; i < threadQueue.size(); i++)
			threadQueue[i]->join();
		for (int i = 0; i < threadQueue.size(); i++)
			delete threadQueue[i];
	}

	int Run(int con, int finial) {
		if (con>=finial) return 0;
		if (command_name[con] == "loop") return con;
		if (command_name[con] == "do") {
			int tmp, times = atoi(command_times[con].c_str());
			for (int i = 1; i <= times; i++) 
				tmp = Run(con+1, finial);
			return Run(tmp+1, finial);
		}
		if (command_name[con] == "thread_define") {
			int tmp;
			for (tmp = con; tmp < finial; tmp++) 
				if (command_name[tmp]=="thread_end") {
					RunThread(con+1,tmp-1);
					break;
				}
			return Run(tmp+1,finial);
		}
		if ('0' <= command_times[con][0] && command_times[con][0] <= '9') {
			Control *contr = GetControlByName(command_name[con]);
			int times = atoi(command_times[con].c_str());
			for (int i = 1; i <= times; i++)
				contr->Run();
			return Run(con+1, finial);
		} else {
			Share *contr = GetShareByName(command_name[con]);
			contr->Run(command_times[con]);
			return Run(con+1, finial);
		}
	}

public:
	
	Controller() {
		ControlList.clear();
		ConfigFile = "config";
	}

	Controller(int argc, char **argv) {
		ControlList.clear();
		int i;
		if ((i = ArgPos((char *)"-config", argc, argv)) > 0) ConfigFile = (std::string)(argv[i + 1]);
	}

	void Explain() {
		FILE *fin = fopen(ConfigFile.c_str(),"r");

		std::string algo = "";
		while (!feof(fin)) {
			char ch = fgetc(fin);
			if (ch == '\n' || ch =='\r' || ch=='\t' || ch==' ') continue;
			algo = algo + ch;
			if (algo == "#define") break;
		}

		algo = "";
		while (!feof(fin)) {
			char ch = fgetc(fin);
			if (ch == '\n' || ch =='\r' || ch=='\t' || ch==' ') continue;
			if (ch == ':') {
				if (algo=="share") {
					ExplainShare(fin);
					algo = "";
					printf("hx\n");
				} else
				if (algo=="word2vec") {
					ExplainWord2Vec(fin); 
					algo = "";
				} else 
				if (algo=="transE") {
					ExplainTransE(fin); 
					algo = "";
				} else {
					printf("Wrong in config file!\n");
					return;
				}
			} else
			algo = algo + ch;
			if (algo == "#end") break;
		}

		sdk *con = new sdk();
		for (int i = 0; i < ControlList.size(); i++) con->SetControl(ControlList[i]);
		Control *con1 = con;
		ControlList.push_back(con1);
		con1->Init();

		algo = "";
		command_name.push_back("do");
		command_times.push_back("1");
		while (!feof(fin)) {
			char ch = fgetc(fin);
			if (ch == '\n' || ch =='\r' || ch=='\t' || ch==' ') continue;
			algo = algo + ch;
			if (algo == "run") {
				ExplainRun(fin);
				algo = "";
			}
			if (algo == "do") {
				ExplainDo(fin);
				algo = "";
			}
			if (algo == "loop") {
				command_name.push_back("loop");
				command_times.push_back("0");
				algo = "";
			}
			if (algo == "thread_define") {
				command_name.push_back("thread_define");
				command_times.push_back("0");
				algo = "";				
			}
			if (algo == "thread_end") {
				command_name.push_back("thread_end");
				command_times.push_back("0");
				algo = "";				
			}
			if (algo == "end") {
				command_name.push_back("end");
				command_times.push_back("0");
				algo = "";				
			}
		}
		command_name.push_back("loop");
		command_times.push_back("0");
		int res=Run(0,command_name.size());
		for (int i = 0; i < ControlList.size(); i++)
//			if (ControlList[i]->GetName()!="sdk")
				 ControlList[i]->Output();
		fclose(fin);
	}

	Control *GetControlByName(char *con) {
		return GetControlByName(std::string(con));
	}

	Control *GetControlByName(std::string name) {
		for (int i = 0; i < ControlList.size(); i++) 
			if (ControlList[i]->GetName() == name) return ControlList[i];
		return NULL;
	}

	Share *GetShareByName(std::string name) {
		for (int i = 0; i < ShareList.size(); i++) 
			if (ShareList[i]->GetName() == name) return ShareList[i];
		return NULL;		
	}

	~Controller() {
		for (int i = 0; i < ControlList.size(); i++)
			if (ControlList[i]!=NULL)
				delete ControlList[i];
	}
};

#endif
