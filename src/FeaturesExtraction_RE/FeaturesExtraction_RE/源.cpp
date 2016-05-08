#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include<map>
#include<time.h>
#include "FeaturesExtraction.h"
using namespace std;



void FEClass ::process(char s[])
{
	char *w = strtok(s, " ");
	char *p, *q;
	wnum = 0;
	arg1 = -1;
	arg2 = -1;
	while (w != NULL)
	{
		if (w[strlen(w) - 1] == '\n') w[strlen(w) - 1] = 0;

		memset(word[wnum].word, 0, 1000);
		memset(word[wnum].type, 0, 10);

		if ((p = strstr(w, "<Na>")) != NULL && (q = strstr(w, "</Na>")) != NULL)
		{
			if (arg1 < 0) arg1 = wnum;
			else arg2 = wnum;
			strncpy(word[wnum].word, p + 4, q - p - 4);
			strcpy(word[wnum++].type, "Na");
		}
		else if ((p = strstr(w, "<Nab>")) != NULL && (q = strstr(w, "</Nab>")) != NULL)
		{
			if (arg1 < 0) arg1 = wnum;
			else arg2 = wnum;
			strncpy(word[wnum].word, p + 5, q - p - 5);
			strcpy(word[wnum++].type, "Nab");
		}
		else if ((p = strstr(w, "<Nf>")) != NULL && (q = strstr(w, "</Nf>")) != NULL)
		{
			if (arg1 < 0) arg1 = wnum;
			else arg2 = wnum;
			strncpy(word[wnum].word, p + 4, q - p - 4);
			strcpy(word[wnum++].type, "Nf");
		}
		else if ((p = strstr(w, "<Nc>")) != NULL && (q = strstr(w, "</Nc>")) != NULL)
		{
			if (arg1 < 0) arg1 = wnum;
			else arg2 = wnum;
			strncpy(word[wnum].word, p + 4, q - p - 4);
			strcpy(word[wnum++].type, "Nc");
		}
		else if ((p = strstr(w, "<Nd>")) != NULL && (q = strstr(w, "</Nd>")) != NULL)
		{
			if (arg1 < 0) arg1 = wnum;
			else arg2 = wnum;
			strncpy(word[wnum].word, p + 4, q - p - 4);
			strcpy(word[wnum++].type, "Nd");
		}
		else if ((p = strstr(w, "<Nl>")) != NULL && (q = strstr(w, "</Nl>")) != NULL)
		{
			if (arg1 < 0) arg1 = wnum;
			else arg2 = wnum;
			strncpy(word[wnum].word, p + 4, q - p - 4);
			strcpy(word[wnum++].type, "Nl");
		}
		else if ((p = strstr(w, "<Nt>")) != NULL && (q = strstr(w, "</Nt>")) != NULL)
		{
			if (arg1 < 0) arg1 = wnum;
			else arg2 = wnum;
			strncpy(word[wnum].word, p + 4, q - p - 4);
			strcpy(word[wnum++].type, "Nt");
		}
		else if ((q = strrchr(w, '/')) != NULL)
		{
			p = w;
			strncpy(word[wnum].word, p, q - p);
			strcpy(word[wnum++].type, q + 1);
		}


		w = strtok(NULL, " ");
	}
}
//判断实体关系对是否是所要求的6对之一
/*
艺术家名——艺术家别名：Na-Nab
艺术家名——影视名：Na-Nf
影视名——出品公司：Nf-Nc
影视名——时间日期：Nf-Nd
影视名——地点：Nf-Nl
影视名——影视类型：Nf-Nt
*/
int FEClass::judge()
{
	if (strcmp(word[arg1].type, "Na") == 0)
	{
		if (strcmp(word[arg2].type, "Nab") == 0 || strcmp(word[arg2].type, "Nf") == 0) return 1;
		else return 0;
	}
	else if (strcmp(word[arg1].type, "Nab") == 0)
	{
		if (strcmp(word[arg2].type, "Na") == 0) return 1;
		else return 0;
	}
	else if (strcmp(word[arg1].type, "Nf") == 0)
	{
		if (strcmp(word[arg2].type, "Na") == 0 || strcmp(word[arg2].type, "Nc") == 0 || strcmp(word[arg2].type, "Nd") == 0 || strcmp(word[arg2].type, "Nl") == 0 || strcmp(word[arg2].type, "Nt") == 0) return 1;
		else return 0;
	}
	else if (strcmp(word[arg1].type, "Nc") == 0)
	{
		if (strcmp(word[arg2].type, "Nf") == 0) return 1;
		else return 0;
	}
	else if (strcmp(word[arg1].type, "Nd") == 0)
	{
		if (strcmp(word[arg2].type, "Nf") == 0) return 1;
		else return 0;
	}
	else if (strcmp(word[arg1].type, "Nl") == 0)
	{
		if (strcmp(word[arg2].type, "Nf") == 0) return 1;
		else return 0;
	}
	else if (strcmp(word[arg1].type, "Nt") == 0)
	{
		if (strcmp(word[arg2].type, "Nf") == 0) return 1;
		else return 0;
	}
	else return 0;
}

//特征提取1-41
void FEClass::extract()
{
	int i, t;
	char p[100];
	
	//F1
	strcpy(results[0].r, word[arg1].type);


	//F2
	strcpy(results[1].r, word[arg2].type);


	//F3
	strcpy(p, word[arg1].type);
	strcpy(results[2].r, strcat(strcat(p, "-"), word[arg2].type));


	//F4
	strcpy(results[3].r, word[arg1].word);


	//F5
	strcpy(results[4].r, word[arg2].word);


	//F6
	if (strcmp(word[arg1].type, "Na") == 0 && strcmp(word[arg2].type, "Nab") == 0) strcpy(results[5].r, "true");
	else if (strcmp(word[arg1].type, "Na") == 0 && strcmp(word[arg2].type, "Nf") == 0) strcpy(results[5].r, "true");
	else if (strcmp(word[arg1].type, "Nf") == 0 && strcmp(word[arg2].type, "Nc") == 0) strcpy(results[5].r, "true");
	else if (strcmp(word[arg1].type, "Nf") == 0 && strcmp(word[arg2].type, "Nd") == 0) strcpy(results[5].r, "true");
	else if (strcmp(word[arg1].type, "Nf") == 0 && strcmp(word[arg2].type, "Nl") == 0) strcpy(results[5].r, "true");
	else if (strcmp(word[arg1].type, "Nf") == 0 && strcmp(word[arg2].type, "Nt") == 0) strcpy(results[5].r, "true");
	else strcpy(results[5].r, "false");

	//F7
	if (arg1 > 0) strcpy(results[6].r, word[arg1 - 1].word);
	else strcpy(results[6].r, "null");


	//F8
	if (arg1 > 0) strcpy(results[7].r, word[arg1 - 1].type);
	else strcpy(results[7].r, "null");


	//F9
	if (arg1 > 1) strcpy(results[8].r, word[arg1 - 2].word);
	else strcpy(results[8].r, "null");


	//F10
	if (arg1 > 1) strcpy(results[9].r, word[arg1 - 2].type);
	else strcpy(results[9].r, "null");


	//F11
	if (arg2 < wnum - 1) strcpy(results[10].r, word[arg2 + 1].word);
	else strcpy(results[10].r, "null");


	//F12
	if (arg2 < wnum - 1) strcpy(results[11].r, word[arg2 + 1].type);
	else strcpy(results[11].r, "null");


	//F13
	if (arg2 < wnum - 2) strcpy(results[12].r, word[arg2 + 2].word);
	else strcpy(results[12].r, "null");


	//F14
	if (arg2<wnum - 2) strcpy(results[13].r, word[arg2 + 2].type);
	else strcpy(results[13].r, "null");


	//F15
	strcpy(p, word[arg1].type);
	if (arg1>0) strcpy(results[14].r, strcat(strcat(p, "-"), word[arg1 - 1].type));
	else strcpy(results[14].r, "null");


	//F16
	strcpy(p, word[arg2].type);
	if (arg2 < wnum - 1) strcpy(results[15].r, strcat(strcat(p, "-"), word[arg2 + 1].type));
	else strcpy(results[15].r, "null");


	//F17
	if (abs(arg2 - arg1) == 1) strcpy(results[16].r, "true");
	else strcpy(results[16].r, "false");


	//F18
	if (abs(arg2 - arg1) == 2) strcpy(results[17].r, word[arg1 + 1].word);
	else strcpy(results[17].r, "null");


	//F19
	if (abs(arg2 - arg1) == 2) strcpy(results[18].r, word[arg1 + 1].type);
	else strcpy(results[18].r, "null");


	//F20
	strcpy(p, word[arg1].type);
	if (abs(arg2 - arg1) == 2) strcpy(results[19].r, strcat(strcat(p, "-"), word[arg1 + 1].type));
	else strcpy(results[19].r, "null");


	//F21
	strcpy(p, word[arg2].type);
	if (abs(arg2 - arg1) == 2) strcpy(results[20].r, strcat(strcat(p, "-"), word[arg1 + 1].type));
	else strcpy(results[20].r, "null");


	//F22
	if (abs(arg2 - arg1) >= 3) strcpy(results[21].r, word[arg1 + 1].word);
	else strcpy(results[21].r, "null");


	//F23
	if (abs(arg2 - arg1) >= 3) strcpy(results[22].r, word[arg1 + 1].type);
	else strcpy(results[22].r, "null");


	//F24
	strcpy(p, word[arg1 + 1].type);
	if (abs(arg2 - arg1) >= 3) strcpy(results[23].r, strcat(strcat(p, "-"), word[arg1].type));
	else strcpy(results[23].r, "null");


	//F25
	strcpy(p, word[arg1 + 1].type);
	if (abs(arg2 - arg1) >= 3) strcpy(results[24].r, strcat(strcat(p, "-"), word[arg2].type));
	else strcpy(results[24].r, "null");


	//F26
	if (abs(arg2 - arg1) >= 3) strcpy(results[25].r, word[arg2 - 1].word);
	else strcpy(results[25].r, "null");


	//F27
	if (abs(arg2 - arg1) >= 3) strcpy(results[26].r, word[arg2 - 1].type);
	else strcpy(results[26].r, "null");


	//F28
	strcpy(p, word[arg2 - 1].type);
	if (abs(arg2 - arg1) >= 3) strcpy(results[27].r, strcat(strcat(p, "-"), word[arg1].type));
	else strcpy(results[27].r, "null");


	//F29
	strcpy(p, word[arg2 - 1].type);
	if (abs(arg2 - arg1) >= 3) strcpy(results[28].r, strcat(strcat(p, "-"), word[arg2].type));
	else strcpy(results[28].r, "null");


	//F30
	if (abs(arg2 - arg1) == 4) strcpy(results[29].r, word[arg1 + 2].word);
	else strcpy(results[29].r, "null");


	//F31
	if (abs(arg2 - arg1) == 4) strcpy(results[30].r, word[arg1 + 2].type);
	else strcpy(results[30].r, "null");


	//F32
	if (abs(arg2 - arg1) >= 5) strcpy(results[31].r, word[arg1 + 2].word);
	else strcpy(results[31].r, "null");


	//F33
	if (abs(arg2 - arg1) >= 5) strcpy(results[32].r, word[arg1 + 2].type);
	else strcpy(results[32].r, "null");


	//F34
	if (abs(arg2 - arg1) >= 5) strcpy(results[33].r, word[arg2 - 2].word);
	else strcpy(results[33].r, "null");


	//F35
	if (abs(arg2 - arg1) >= 5) strcpy(results[34].r, word[arg2 - 2].type);
	else strcpy(results[34].r, "null");


	//F36
	for (i = arg1 + 1; i < arg2; i++)
	{
		if (strcmp(word[i].type, "v") == 0) break;
	}
	if (i<arg2) strcpy(results[35].r, word[i].word);
	else strcpy(results[35].r, "null");


	//F37
	for (i = arg2 - 1; i>arg1; i--)
	{
		if (strcmp(word[i].type, "v") == 0) break;
	}
	if (i > arg1) strcpy(results[36].r, word[i].word);
	else strcpy(results[36].r, "null");


	//F38
	_itoa(arg2 - arg1 - 1, results[37].r, 10);


	//F39
	for (i = arg1 + 1, t = 0; i < arg2; i++)
	{
		if (strcmp(word[i].type, "Na") == 0) t++;
		if (strcmp(word[i].type, "Nab") == 0) t++;
		if (strcmp(word[i].type, "Nf") == 0) t++;
		if (strcmp(word[i].type, "Nc") == 0) t++;
		if (strcmp(word[i].type, "Nd") == 0) t++;
		if (strcmp(word[i].type, "Nl") == 0) t++;
		if (strcmp(word[i].type, "Nt") == 0) t++;
	}
	_itoa(t, results[38].r, 10);

	//F40
	for (i = arg1 + 1, t = 0; i < arg2; i++)
	{
		if (strcmp(word[i].type, "v") == 0) t++;
	}
	_itoa(t, results[39].r, 10);


	//F41
	for (i = arg1 + 1, t = 0; i < arg2; i++)
	{
		if (strcmp(word[i].type, "r") == 0) t++;
		else if (strcmp(word[i].type, "rr") == 0) t++;
		else if (strcmp(word[i].type, "rz") == 0) t++;
		else if (strcmp(word[i].type, "rzt") == 0) t++;
		else if (strcmp(word[i].type, "rzs") == 0) t++;
		else if (strcmp(word[i].type, "rzv") == 0) t++;
		else if (strcmp(word[i].type, "ry") == 0) t++;
		else if (strcmp(word[i].type, "ryt") == 0) t++;
		else if (strcmp(word[i].type, "rys") == 0) t++;
		else if (strcmp(word[i].type, "ryv") == 0) t++;
		else if (strcmp(word[i].type, "rg") == 0) t++;
	}
	_itoa(t, results[40].r, 10);



	//F42



	//F43


}

int main(int argc, char *argv[])
{
	int i;
	char s[5000], p[5000];
	string label_str;
	ifstream fin_label;
	map<int, string> labelmap;
	if (argc == 4)//一行命令包含四个部分，第一部分即为Features_extraction.exe应用程序
	{
		if ((in = fopen(argv[1], "r")) == NULL)//第二部分
		{
			cout << "Cannot open " << argv[1] << endl;
			return 1;
		}
		fin_label.open(argv[2]);
		if (!fin_label.is_open())//第三部分，读取label字典，注意存放label文件中label的顺序一定要和第二部分数据原文件的顺序对应起来，否则会造成数据和标签对应紊乱的严重问题
		{
			cout << "Cannot open " << argv[2] << endl;
			return 1;
		}

		if ((out = fopen(argv[3], "wb")) == NULL)//第四部分,文件输出（格式：特征+label）
		{
			cout << "Cannot open " << argv[3];
			return 1;
		}


		//将label内容读取并导入map中
		int labelid = 0;
		while (getline(fin_label,label_str))
		{
			labelid++;
			labelmap.insert(pair<int, string>(labelid, label_str));
		}
		int dataid = 0;
		while (!feof(in))
		{
			dataid++;
			strcpy(s, "");
			fgets(s, 5000, in);
			if (strlen(s) < 2)
				continue;
			s[strlen(s) - 1] = 0;
			strcpy(p, s);
			//拆分句子
			fe.process(p);
			//判断实体关系
			if (fe.judge() == 0)
			{
				continue;
			}
			//特征提取
			fe.extract();
			for (i = 0; i < 41; i++)
			{
				fprintf(out, "%s\t", results[i].r);
			}
			//在每行特征最后输出标签
			fprintf(out, "%s", labelmap[dataid].c_str());
			fprintf(out, "\r\n");
		}
		fclose(in);
		fclose(out);
	}
	else if (argc == 3)//一行命令包含三个部分，第一部分即为Features_extraction.exe应用程序
	{
		if ((in = fopen(argv[1], "r")) == NULL)//第二部分
		{
			cout<<"Cannot open "<<argv[1]<<endl;
			return 1;
		}


		if ((out = fopen(argv[2], "wb")) == NULL)//第三部分
		{
			cout<<"Cannot open "<<argv[2];
			return 1;
		}

		int j = 0;
		while (!feof(in))
		{
			strcpy(s, "");
			fgets(s, 5000, in);
			j++;
			if (strlen(s) < 2)
				continue;
			s[strlen(s) - 1] = 0;
			strcpy(p, s);
			//拆分句子
			fe.process(p);
			//判断实体关系
			if (fe.judge() == 0)
			{
				cout << j << endl;
				continue;
			}

			//特征提取
			fe.extract();
			//输出
			//char sss[100];
			//itoa(j,sss , 10);		
			//fputs(sss,out);
			for (i = 0; i < 41; i++)
			{
				fprintf(out, "%s\t", results[i].r);
			}
			fprintf(out, "\r\n");
		}
		fclose(in);
		fclose(out);
	}

	else if (argc == 2)//一行命令包含三个部分，第一部分即为Features_extraction.exe应用程序
	{
		strcpy(s, "");
		strcpy(s, argv[1]);//第二部分为完成词性标注和命名实体识别的句子
		if (strlen(s) < 2)
		{
			printf("输入待抽取特征的句子长度太短！\n");
			exit(0);
		}
		s[strlen(s) - 1] = 0;
		strcpy(p, s);
		//拆分句子
		fe.process(p);
		//判断实体关系
		if (fe.judge() == 0)
		{
			printf("实体对类型不在考虑范围内！\n");
			exit(0);
		}
		//特征提取
		fe.extract();
		//输出
		for (i = 0; i < 41; i++)
		{
			printf("%s\t", results[i].r);
		}
		printf("\n");
	}
	return 0;
}







