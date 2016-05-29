#ifndef INIT_H
#define INIT_H
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <cstdio>
#include <float.h>
#include <cmath>
#include <ctime>

using namespace std;

int num_threads = 8;
int trainTimes = 100;
float alpha = 0.1;
float reduce = 0.98;
int dimensionC = 1000;
int dimensionWPE = 25;
float marginPositive = 2.5;
float marginNegative = 0.5;
float margin = 2;
float Belt = 0.001;
int window = 3;
int limit = 20;
int batch = 64;
int turn;
float rate = 1;
int tt,tt1;
float *matrixB1, *matrixRelation, *matrixW1, *matrixRelationDao;
float *positionVecE1, *positionVecE2, *matrixW1PositionE1, *matrixW1PositionE2;
double mx = 0;
FILE *logg;

float *wordVec;
long long wordTotal, dimension, relationTotal;
long long PositionMinE1, PositionMaxE1, PositionTotalE1,PositionMinE2, PositionMaxE2, PositionTotalE2;
map<string,int> wordMapping;
map<string,int> relationMapping;
vector<int *> trainLists, trainPositionE1, trainPositionE2;
vector<int> trainLength;
vector<int> headList, tailList, relationList;
vector<int *> testtrainLists, testPositionE1, testPositionE2;
vector<int> testtrainLength;
vector<int> testheadList, testtailList, testrelationList;
vector<std::string> nam;

void init() {
	srand(time(NULL));
	FILE *f = fopen("data/vectors1.bin", "rb");
	fscanf(f, "%lld", &wordTotal);
	fscanf(f, "%lld", &dimension);
	PositionMinE1 = 0;
	PositionMaxE1 = 0;
	PositionMinE2 = 0;
	PositionMaxE2 = 0;
	wordVec = (float *)malloc(wordTotal * dimension * sizeof(float));
	for (int b = 0; b < wordTotal; b++) {
		string name = "";
		while (1) {
			char ch = fgetc(f);
			if (feof(f) || ch == ' ') break;
			if (ch != '\n') name = name + ch;
		}
		long long last = b * dimension;
		float smp = 0;
		for (int a = 0; a < dimension; a++) {
			fread(&wordVec[a + last], sizeof(float), 1, f);
			smp += wordVec[a + last]*wordVec[a + last];
		}
		smp = sqrt(smp);
		for (int a = 0; a< dimension; a++)
			wordVec[a+last] = wordVec[a+last] / smp;
		wordMapping[name] = b;
	}
	fclose(f);
	
	char buffer[1000];
	f = fopen("data/relation2id.txt", "r");
	while (fscanf(f,"%s",buffer)==1) {
		int id;
		fscanf(f,"%d",&id);
		relationMapping[(string)(buffer)] = id;
		relationTotal++;
		nam.push_back((std::string)(buffer));
	}
	fclose(f);
	
	f = fopen("data/cnn.txt", "r");
		
	while (fscanf(f,"%s",buffer)==1)  {
		int head = wordMapping[(string)(buffer)];
		fscanf(f,"%s",buffer);
		int tail = wordMapping[(string)(buffer)];
		fscanf(f,"%s",buffer);
		int num = relationMapping[(string)(buffer)];
		int len = 0, lefnum, rignum;
		std::vector<int> tmpp;
		while (fscanf(f,"%s", buffer)==1) {
			std::string con = buffer;
			if (con=="#") break;
			int gg = wordMapping[con];
			if (gg == -1) continue;
			if (gg == head) lefnum = len;
			if (gg == tail) rignum = len;
			len++;
			tmpp.push_back(gg);
		}
		if (head != -1 && tail != -1) {
			headList.push_back(head);
			tailList.push_back(tail);
			relationList.push_back(num);
			trainLength.push_back(len);
			int *con=(int *)calloc(len,sizeof(int));
			int *conl=(int *)calloc(len,sizeof(int));
			int *conr=(int *)calloc(len,sizeof(int));
			for (int i = 0; i < len; i++) {
				con[i] = tmpp[i];
				conl[i] = lefnum - i;
				conr[i] = rignum - i;
				if (conl[i] >= limit) conl[i] = limit;
				if (conr[i] >= limit) conr[i] = limit;
				if (conl[i] <= -limit) conl[i] = -limit;
				if (conr[i] <= -limit) conr[i] = -limit;
				if (conl[i] > PositionMaxE1) PositionMaxE1 = conl[i];
				if (conr[i] > PositionMaxE2) PositionMaxE2 = conr[i];
				if (conl[i] < PositionMinE1) PositionMinE1 = conl[i];
				if (conr[i] < PositionMinE2) PositionMinE2 = conr[i];
			}
			trainLists.push_back(con);
			trainPositionE1.push_back(conl);
			trainPositionE2.push_back(conr);
		}
	}
	fclose(f);

	f = fopen("data/test.txt", "r");	
	while (fscanf(f,"%s",buffer)==1)  {
		int head = wordMapping[(string)(buffer)];
		fscanf(f,"%s",buffer);
		int tail = wordMapping[(string)(buffer)];
		fscanf(f,"%s",buffer);
		int num = relationMapping[(string)(buffer)];
		int len = 0 , lefnum = 0, rignum = 0;
		std::vector<int> tmpp;
		while (fscanf(f,"%s", buffer)==1) {
			std::string con = buffer;
			if (con=="#") break;
			int gg = wordMapping[con];
			if (gg == -1) continue;
			if (gg == head) lefnum = len;
			if (gg == tail) rignum = len;
			len++;
			tmpp.push_back(gg);
		}
		if (head != -1 && tail != -1) {
			testheadList.push_back(head);
			testtailList.push_back(tail);
			testrelationList.push_back(num);
			testtrainLength.push_back(len);
			int *con=(int *)calloc(len,sizeof(int));
			int *conl=(int *)calloc(len,sizeof(int));
			int *conr=(int *)calloc(len,sizeof(int));
			for (int i = 0; i < len; i++) {
				con[i] = tmpp[i];
				conl[i] = lefnum - i;
				conr[i] = rignum - i;
				if (conl[i] >= limit) conl[i] = limit;
				if (conr[i] >= limit) conr[i] = limit;
				if (conl[i] <= -limit) conl[i] = -limit;
				if (conr[i] <= -limit) conr[i] = -limit;
				if (conl[i] > PositionMaxE1) PositionMaxE1 = conl[i];
				if (conr[i] > PositionMaxE2) PositionMaxE2 = conr[i];
				if (conl[i] < PositionMinE1) PositionMinE1 = conl[i];
				if (conr[i] < PositionMinE2) PositionMinE2 = conr[i];
			}
			testtrainLists.push_back(con);
			testPositionE1.push_back(conl);
			testPositionE2.push_back(conr);
		}
	}
	fclose(f);

	for (int i = 0; i < trainPositionE1.size(); i++) {
		int len = trainLength[i];
		int *work1 = trainPositionE1[i];
		for (int j = 0; j < len; j++)
			work1[j] = work1[j] - PositionMinE1;
		int *work2 = trainPositionE2[i];
		for (int j = 0; j < len; j++)
			work2[j] = work2[j] - PositionMinE2;
	}

	for (int i = 0; i < testPositionE1.size(); i++) {
		int len = testtrainLength[i];
		int *work1 = testPositionE1[i];
		for (int j = 0; j < len; j++)
			work1[j] = work1[j] - PositionMinE1;
		int *work2 = testPositionE2[i];
		for (int j = 0; j < len; j++)
			work2[j] = work2[j] - PositionMinE2;
	}
	PositionTotalE1 = PositionMaxE1 - PositionMinE1 + 1;
	PositionTotalE2 = PositionMaxE2 - PositionMinE2 + 1;
	printf("%lld %lld\n", PositionTotalE1, PositionTotalE2);
}

float CalcTanh(float con) {
	if (con > 20) return 1.0;
	if (con < -20) return -1.0;
	float sinhx = exp(con) - exp(-con);
	float coshx = exp(con) + exp(-con);
	return sinhx / coshx;
}

float tanhDao(float con) {
	float res = CalcTanh(con);
	return 1 - res * res;
}

float sigmod(float con) {
	if (con > 20) return 1.0;
	if (con < -20) return 0.0;
	con = exp(con);
	return con / (1 + con);
}

int getRand(int l,int r) {
	int len = r - l;
	int res = rand() % len;
	return res + l;
}

float getRandU(float l, float r) {
	float len = r - l;
	float res = (float)(rand()) / RAND_MAX;
	return res * len + l;
}



#endif
