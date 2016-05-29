#ifndef SDK_H
#define SDK_H
#include "control.h"
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <cstdio>
#include <float.h>
#include <string.h>
#include <cmath>
#include <thread>

class sdk:public Control {
private:
	std::vector<Control *> command;
	Control *w2v, *te;

	int num_threads;
	int trainTimes;
	float alpha;
	float reduce;
	int dimensionC;
	int dimensionWPE;
	float marginPositive;
	float marginNegative;
	float margin;
	float Belt;
	int window;
	int limit;
	int tt,tt1;
	float *matrixB1, *matrixRelation, *matrixW1, *matrixRelationDao;
	float *positionVecE1, *positionVecE2, *matrixW1PositionE1, *matrixW1PositionE2;
	int batch;
	int turn;
	float rate;
	float *matrixRelation_1;
	float *wordVec;
	long long wordTotal, dimension, relationTotal;
	long long PositionMinE1, PositionMaxE1, PositionTotalE1,PositionMinE2, PositionMaxE2, PositionTotalE2;
	std::vector<int *> trainLists, trainPositionE1, trainPositionE2;
	std::vector<int> trainLength;
	std::vector<int> headList, tailList, relationList;
	std::map<std::string,int> wordMapping;
	std::map<std::string,int> relationMapping;

	void __init() {
		num_threads = 7;
		trainTimes = 81;
		alpha = 0.02;
		reduce = 1;
		dimensionC = 150;
		dimensionWPE = 25;
		dimension = 150;
		marginPositive = 2.5;
		marginNegative = 0.5;
		margin = 2;
		Belt = 0.001;
		window = 3;
		limit = 30;
		batch = 16;
		rate = 1;
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

	void train(int *sentence, int *trainPositionE1, int *trainPositionE2, int len, int e1, int e2, int r1, float &res, float &res1, float *matrixW1Dao, float *matrixB1Dao, float *r, float *matrixRelationDao,
		float *positionVecDaoE1, float *positionVecDaoE2, float*matrixW1PositionE1Dao, float*matrixW1PositionE2Dao,  float alpha) {
		int tip[dimensionC];
			
		for (int i = 0; i < dimensionC; i++) {
			int last = i * dimension * window;
			int lastt = i * dimensionWPE * window;
			float mx = -FLT_MAX;
			for (int i1 = 0; i1 <= len - window; i1++) {
				float res = 0;
				int tot = 0;
				int tot1 = 0;
				for (int j = i1; j < i1 + window; j++)  {
					int last1 = sentence[j] * dimension;
				 	for (int k = 0; k < dimension; k++) {
				 		res += matrixW1[last + tot] * wordVec[last1+k];
				 		tot++;
				 	}
				 	int last2 = trainPositionE1[j] * dimensionWPE;
				 	int last3 = trainPositionE2[j] * dimensionWPE;
				 	for (int k = 0; k < dimensionWPE; k++) {
				 		res += matrixW1PositionE1[lastt + tot1] * positionVecE1[last2+k];
				 		res += matrixW1PositionE2[lastt + tot1] * positionVecE2[last3+k];
				 		tot1++;
				 	}
				}
				if (res > mx) {
					mx = res;
					tip[i] = i1;
				}
			}
			r[i] = mx + matrixB1[i];
		}

		for (int i = 0; i < dimensionC; i++) {
			r[i] = CalcTanh(r[i]);
		}

		float s1, g, s2;
		int r2;
		s1 = -FLT_MAX;
		s2 = -FLT_MAX;
		for (int j = 0; j < relationTotal; j++) {
			float s = 0;
			for (int i = 0; i < dimensionC; i++)
				s +=  matrixRelation[j * dimensionC + i] * r[i];
			if (j == r1) s1 = s; else
			if (s >= s2) {
				s2 = s;
				r2 = j;
			}
		}

		g = -1* sigmod(margin*(marginPositive - s1)) * margin * alpha;
		for (int i = 0; i < dimensionC; i++) {
			int last = i * dimension * window;
			int tot = 0;
			int lastt = i * dimensionWPE * window;
			int tot1 = 0;
			float g1 = g * matrixRelation[r1 * dimensionC + i] * (1 -  r[i] * r[i]);
			for (int j = 0; j < window; j++)  {
				int last1 = sentence[tip[i] + j] * dimension;
				for (int k = 0; k < dimension; k++) {
					matrixW1Dao[last + tot] -= g1 * wordVec[last1+k];
					tot++;
				}
				int last2 = trainPositionE1[tip[i] + j] * dimensionWPE;
				int last3 = trainPositionE2[tip[i] + j] * dimensionWPE;
				for (int k = 0; k < dimensionWPE; k++) {
					matrixW1PositionE1Dao[lastt + tot1] -= g1 * positionVecE1[last2 + k];
					matrixW1PositionE2Dao[lastt + tot1] -= g1 * positionVecE2[last3 + k];
					positionVecDaoE1[last2 + k] -= g1 * matrixW1PositionE1[lastt + tot1];
					positionVecDaoE2[last3 + k] -= g1 * matrixW1PositionE2[lastt + tot1];				
					tot1++;
				}
			}
			matrixB1Dao[i] -= g1;
			matrixRelationDao[r1 * dimensionC + i] -= g * r[i];
		}


		g = sigmod(margin*(marginNegative + s2)) * margin * alpha;
		for (int i = 0; i < dimensionC; i++) {
			int last = i * dimension * window;
			int tot = 0;
			int lastt = i * dimensionWPE * window;
			int tot1 = 0;
			float g1 = g * matrixRelation[r2 * dimensionC + i] * (1 -  r[i] * r[i]);
			for (int j = 0; j < window; j++)  {
				int last1 = sentence[tip[i] + j] * dimension;
				for (int k = 0; k < dimension; k++) {
					matrixW1Dao[last + tot] -= g1 * wordVec[last1+k];
					tot++;
				}
				int last2 = trainPositionE1[tip[i] + j] * dimensionWPE;
				int last3 = trainPositionE2[tip[i] + j] * dimensionWPE;
				for (int k = 0; k < dimensionWPE; k++) {
					matrixW1PositionE1Dao[lastt + tot1] -= g1 * positionVecE1[last2 + k];
					matrixW1PositionE2Dao[lastt + tot1] -= g1 * positionVecE2[last3 + k];
					positionVecDaoE1[last2 + k] -= g1 * matrixW1PositionE1[lastt + tot1];
					positionVecDaoE2[last3 + k] -= g1 * matrixW1PositionE2[lastt + tot1];
					tot1++;
				}
			}
			matrixB1Dao[i] -= g1;
			matrixRelationDao[r2 * dimensionC + i] -= g * r[i];
		}

		for (int i = 0; i < dimensionC; i++) {
			int last = dimension * window * i;
			res1+=Belt * matrixB1[i] * matrixB1[i];

			for (int j = dimension * window -1; j >= 0; j--) {
				res1+= Belt * matrixW1[last + j] * matrixW1[last + j];
				matrixW1Dao[last + j] += - Belt * matrixW1[last + j] * alpha * 2; 
			}
			matrixB1Dao[i] += -Belt * matrixB1[i] *alpha * 2;
		}
	}

	void trainMode() {
		float *matrixRelationDao = (float *)calloc(dimensionC*relationTotal, sizeof(float));
		float *matrixW1Dao =  (float*)calloc(dimensionC * dimension * window, sizeof(float));
		float *matrixB1Dao =  (float*)calloc(dimensionC, sizeof(float));
		
		float *positionVecDaoE1 = (float *)calloc(PositionTotalE1 * dimensionWPE, sizeof(float));
		float *positionVecDaoE2 = (float *)calloc(PositionTotalE2 * dimensionWPE, sizeof(float));
		float *matrixW1PositionE1Dao = (float *)calloc(dimensionC * dimensionWPE * window, sizeof(float));
		float *matrixW1PositionE2Dao = (float *)calloc(dimensionC * dimensionWPE * window, sizeof(float));
		
		float *r = (float *)calloc(dimensionC, sizeof(float));
		{
			int len = trainLists.size();
			int npoch  =  len / (batch * num_threads);
			float alpha1 = alpha*rate/batch;
			for (int k = 1; k <= npoch; k++) {
				float res = 0;
				float res1 = 0;
				memset(positionVecDaoE1, 0, PositionTotalE1 * dimensionWPE* sizeof(float));
				memset(positionVecDaoE2, 0, PositionTotalE2 * dimensionWPE* sizeof(float));
				memset(matrixW1PositionE1Dao, 0, dimensionC * dimensionWPE * window* sizeof(float));
				memset(matrixW1PositionE2Dao, 0, dimensionC * dimensionWPE * window* sizeof(float));

				memset(matrixW1Dao, 0, sizeof(float) * dimensionC * dimension * window);
				memset(matrixB1Dao, 0, sizeof(float) * dimensionC);
				memset(matrixRelationDao,0, dimensionC*relationTotal * sizeof(float));
				for (int k1 = batch; k1 > 0; k1--) {
					int i = getRand(0, len);
					train(trainLists[i], trainPositionE1[i], trainPositionE2[i], trainLength[i], headList[i], tailList[i], relationList[i], res, res1, matrixW1Dao, matrixB1Dao, r, matrixRelationDao, 
					positionVecDaoE1, positionVecDaoE2, matrixW1PositionE1Dao, matrixW1PositionE2Dao, alpha1);
				}
				
				for (int i = 0; i < PositionTotalE1; i++) {
					int last = i * dimensionWPE;
					for (int j = 0; j < dimensionWPE; j++) 
						positionVecE1[last + j] += positionVecDaoE1[last + j];
				}
				for (int i = 0; i < PositionTotalE2; i++) {
					int last = i * dimensionWPE;
					for (int j = 0; j < dimensionWPE; j++) 
						positionVecE2[last + j] += positionVecDaoE2[last + j];
				}
				for (int i = 0; i < dimensionC; i++) {
					for (int j = 0;j <relationTotal;j++) {
						matrixRelation[j * dimensionC +i] += matrixRelationDao[j * dimensionC + i];
					}
					int last = dimension * window * i;
					for (int j = dimension * window -1; j >= 0; j--)
						matrixW1[last + j] += matrixW1Dao[last + j]; 
					last = i * window * dimensionWPE;
					for (int j = dimensionWPE * window - 1; j >=0; j--) {
						matrixW1PositionE1[last + j] += matrixW1PositionE1Dao[last + j];
						matrixW1PositionE2[last + j] += matrixW1PositionE2Dao[last + j];
					}
					matrixB1[i] += matrixB1Dao[i];
				}
			}
		}
		free(matrixW1Dao);
		free(matrixB1Dao);
		free(r);
	}

	void train() {	
		float con = sqrt(6.0/(dimensionC+relationTotal));
		float con1 = sqrt(6.0/((dimensionWPE+dimension)*window));
		matrixRelation_1 = (float *)calloc(dimensionC * relationTotal, sizeof(float));

		positionVecE1 = (float *)calloc(PositionTotalE1 * dimensionWPE, sizeof(float));
		positionVecE2 = (float *)calloc(PositionTotalE2 * dimensionWPE, sizeof(float));
		
		matrixW1 = (float*)calloc(dimensionC * dimension * window, sizeof(float));
		matrixW1PositionE1 = (float *)calloc(dimensionC * dimensionWPE * window, sizeof(float));
		matrixW1PositionE2 = (float *)calloc(dimensionC * dimensionWPE * window, sizeof(float));
		matrixB1 = (float*)calloc(dimensionC, sizeof(float));

		for (int i = 0; i < dimensionC; i++) {
			int last = i * window * dimension;
			for (int j = dimension * window - 1; j >=0; j--)
				matrixW1[last + j] = getRandU(-con1, con1);
			last = i * window * dimensionWPE;
			float tmp1 = 0;
			float tmp2 = 0;
			for (int j = dimensionWPE * window - 1; j >=0; j--) {
				matrixW1PositionE1[last + j] = getRandU(-con1, con1);
				tmp1 += matrixW1PositionE1[last + j]  * matrixW1PositionE1[last + j] ;
				matrixW1PositionE2[last + j] = getRandU(-con1, con1);
				tmp2 += matrixW1PositionE2[last + j]  * matrixW1PositionE2[last + j] ;
			}
			matrixB1[i] = getRandU(-con1, con1);
		}

		for (int i = 0; i < relationTotal; i++) 
			for (int j = 0; j < dimensionC; j++)
				matrixRelation[i * dimensionC + j] = getRandU(-con, con);

		for (int i = 0; i < PositionTotalE1; i++) {
			float tmp = 0;
			for (int j = 0; j < dimensionWPE; j++) {
				positionVecE1[i * dimensionWPE + j] = getRandU(-con1, con1);
				tmp += positionVecE1[i * dimensionWPE + j] * positionVecE1[i * dimensionWPE + j];
			}
		}

		for (int i = 0; i < PositionTotalE2; i++) {
			float tmp = 0;
			for (int j = 0; j < dimensionWPE; j++) {
				positionVecE2[i * dimensionWPE + j] = getRandU(-con1, con1);
				tmp += positionVecE2[i * dimensionWPE + j] * positionVecE2[i * dimensionWPE + j];
			}
		}

		for (turn = 0; turn < trainTimes; turn ++) {
			std::thread *T = new std::thread[num_threads];
			for (int i = 0;i < num_threads; i++)
				T[i] = std::thread(std::bind(&sdk::trainMode, this));
			for (int i = 0; i < num_threads; i++) T[i].join();
			delete[] T;
			printf("End SDK %d!\n", turn);
			// if ((turn+1)%1==0) {
			// 	memcpy(matrixRelation_1, matrixRelation, sizeof(float) *  dimensionC * relationTotal);
			// 	test();
			// }
			if ((turn+1)%1==0) rate=rate*reduce;
		}
		memcpy(matrixRelation_1, matrixRelation, sizeof(float) *  dimensionC * relationTotal);
		test();
		output();
	}

public:

	void Run() {
		printf("Run SDK!\n");
		train();

	}

	void Init() {
		//xu'ye
		__init();
		for (int i=0;i<command.size();i++)
			if (command[i]->GetName()=="te")
				te=command[i];
		relationTotal = te->GetWordTotal(1);
		matrixRelation = te->GetWordVec(1);
		
		char buffer[1000];
		FILE *f = fopen("cnn/data/vectors.bin", "rb");
			fscanf(f, "%lld", &wordTotal);
			fscanf(f, "%lld", &dimension);
			printf("%d\n",wordTotal);
			PositionMinE1 = 0;
			PositionMaxE1 = 0;
			PositionMinE2 = 0;
			PositionMaxE2 = 0;
			wordVec = (float *)malloc(wordTotal * dimension * sizeof(float));
			for (int b = 0; b < wordTotal; b++) {
				std::string name = "";
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

		f = fopen("cnn/data/test/cnn.txt", "r");			
		while (fscanf(f,"%s",buffer)==1)  {
			int head = wordMapping[(std::string)(buffer)];
			fscanf(f,"%s",buffer);
			int tail = wordMapping[(std::string)(buffer)];
			fscanf(f,"%s",buffer);
			int num =  te->GetWordNumber(buffer);
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
				if (len==2)
					printf("%d %d\n",head,tail );
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

		f = fopen("cnn/data/test/test.txt", "r");	
		while (fscanf(f,"%s",buffer)==1)  {
			int head = wordMapping[(std::string)(buffer)];
			fscanf(f,"%s",buffer);
			int tail = wordMapping[(std::string)(buffer)];
			fscanf(f,"%s",buffer);
			int num =  te->GetWordNumber(buffer);
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

		for (int i = 0; i < testPositionE1.size(); i++) {
			int len = testtrainLength[i];
			int *work1 = testPositionE1[i];
			for (int j = 0; j < len; j++)
				work1[j] = work1[j] - PositionMinE1;
			int *work2 = testPositionE2[i];
			for (int j = 0; j < len; j++)
				work2[j] = work2[j] - PositionMinE2;
		}

		for (int i = 0; i < trainPositionE1.size(); i++) {
			int len = trainLength[i];
			int *work1 = trainPositionE1[i];
			for (int j = 0; j < len; j++)
				work1[j] = work1[j] - PositionMinE1;
			int *work2 = trainPositionE2[i];
			for (int j = 0; j < len; j++)
				work2[j] = work2[j] - PositionMinE2;
		}

		PositionTotalE1 = PositionMaxE1 - PositionMinE1 + 1;
		PositionTotalE2 = PositionMaxE2 - PositionMinE2 + 1;
	}

	void output() {
		FILE *fout = fopen("out/matrixW1+B1.txt", "w");
		for (int i = 0; i < dimensionC; i++) {
			for (int j = 0; j < dimension * window; j++)
				fprintf(fout, "%f\t",matrixW1[i* dimension*window+j]);
			for (int j = 0; j < dimensionWPE * window; j++)
				fprintf(fout, "%f\t",matrixW1PositionE1[i* dimensionWPE*window+j]);
			for (int j = 0; j < dimensionWPE * window; j++)
				fprintf(fout, "%f\t",matrixW1PositionE2[i* dimensionWPE*window+j]);
			fprintf(fout, "%f\n", matrixB1[i]);
		}
		fclose(fout);

		fout = fopen("out/matrixRl.txt", "w");
		for (int i = 0; i < relationTotal; i++) {
			for (int j = 0; j < dimensionC; j++)
				fprintf(fout, "%f\t", matrixRelation_1[i * dimensionC + j]);
			fprintf(fout, "\n");
		}
		fclose(fout);

		fout = fopen("out/matrixPosition.txt", "w");
		for (int i = 0; i < PositionTotalE1; i++) {
			for (int j = 0; j < dimensionWPE; j++)
				fprintf(fout, "%f\t", positionVecE1[i * dimensionWPE + j]);
			fprintf(fout, "\n");
		}
		for (int i = 0; i < PositionTotalE2; i++) {
			for (int j = 0; j < dimensionWPE; j++)
				fprintf(fout, "%f\t", positionVecE2[i * dimensionWPE + j]);
			fprintf(fout, "\n");
		}
		fclose(fout);
	}

	void Output() {
	}

	sdk() {
		name = "sdk";
		command.clear();
	}

	void SetControl(Control * con) {
		command.push_back(con);
	} 

	~sdk() {

	}

std::vector<int *> testtrainLists, testPositionE1, testPositionE2;
std::vector<int> testtrainLength;
std::vector<int> testheadList, testtailList, testrelationList;
int tipp;
float ress;

void test(int *sentence, int *testPositionE1, int *testPositionE2, int len, int e1, int e2, int r1, float &res, float *r) {
	int tip[dimensionC];
		
	for (int i = 0; i < dimensionC; i++) {
		int last = i * dimension * window;
		int lastt = i * dimensionWPE * window;
		float mx = -FLT_MAX;
		for (int i1 = 0; i1 <= len - window; i1++) {
			float res = 0;
			int tot = 0;
			int tot1 = 0;
			for (int j = i1; j < i1 + window; j++)  {
				int last1 = sentence[j] * dimension;
			 	for (int k = 0; k < dimension; k++) {
			 		res += matrixW1[last + tot] * wordVec[last1+k];
			 		tot++;
			 	}
			 	int last2 = testPositionE1[j] * dimensionWPE;
			 	int last3 = testPositionE2[j] * dimensionWPE;
			 	for (int k = 0; k < dimensionWPE; k++) {
			 		res += matrixW1PositionE1[lastt + tot1] * positionVecE1[last2+k];
			 		res += matrixW1PositionE2[lastt + tot1] * positionVecE2[last3+k];
			 		tot1++;
			 	}
			}
			if (res > mx) mx = res;
		}
		r[i] = mx + matrixB1[i];
	}

	for (int i = 0; i < dimensionC; i++)
		r[i] = CalcTanh(r[i]);

	float s2 = -FLT_MAX;
	int r2;

	for (int i = 0; i < dimensionC; i++)
		r[i] = CalcTanh(r[i]);

	for (int j = 1; j < relationTotal; j++) {
		float s = 0;
		for (int i = 0; i < dimensionC; i++)
			s +=  matrixRelation_1[j * dimensionC + i] * r[i];
		if (s >= s2) {
			s2 = s;
			r2 = j;
		}
	}

	tipp = r2;
	if (s2 < 0) tipp = 0; 
	ress = s2;
}

struct arr {
	int num;
	int ans;
	float res;
};
arr work[100000];

struct cmp {
	bool operator()(const arr &a,const arr&b) {
		return a.res>b.res;
	}
};

void test() {
	float *matrixW1Dao =  (float*)calloc(dimensionC * dimension * window, sizeof(float));
	float *matrixB1Dao =  (float*)calloc(dimensionC, sizeof(float));
	float *r = (float *)calloc(dimensionC, sizeof(float));
	std::vector<int> ans;
	float res = 0;
	int re1 =  testtrainLists.size();
	ans.clear();
	int tot=0;
	for (int k1 = 0; k1 < re1; k1++) {
		int i=k1;
		test(testtrainLists[i],  testPositionE1[i], testPositionE2[i], testtrainLength[i], testheadList[i], testtailList[i], testrelationList[i], res, r);
		work[tot].num = 8001+k1;
		work[tot].ans = tipp;
		work[tot].res = ress;
		tot++;
	}
	res  = 0;
	float mxxx=0;
	for (int i = 0; i <tot;i++) {
		if (work[i].ans==0) continue;
		if (work[i].ans == testrelationList[work[i].num-8001]) res+=1;
		mxxx+=1;
	}
	mxxx = res / mxxx;
	printf("%.6f\n: ",mxxx);
	fprintf(logg,"%.6f\n", mxxx);
	free(matrixW1Dao);
	free(matrixB1Dao);
	free(r);
}

};

#endif

