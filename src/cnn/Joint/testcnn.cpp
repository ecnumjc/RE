#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <cstdio>
#include <float.h>
#include <string.h>
#include <cmath>

using namespace std;

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
map<string,int> entity2id,relation2id;
vector<double *> entity2vec,relation2vec;


void __init() {
	num_threads = 7;
	trainTimes = 90;
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

std::vector<int *> testtrainLists, testPositionE1, testPositionE2;
std::vector<int> testtrainLength;
std::vector<int> testheadList, testtailList, testrelationList;
std::vector<int> testentityE1, testentityE2;
int tipp;
float ress;

void test(int *sentence, int *testPositionE1, int *testPositionE2, int len, int e1, int e2, int r1, float &res, float *r,int ee1,int ee2) {
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
		for (int i = 0; i < dimensionC; i++) {
			s +=  matrixRelation_1[j * dimensionC + i] * r[i];
			// s +=  matrixRelation_1[j * dimensionC + i] * (entity2vec[ee2][i] - entity2vec[ee1][i]);
			// float tmp =  -relation2vec[j][i]+(entity2vec[ee2][i] - entity2vec[ee1][i]);
			// if (tmp>0)
			// s += tmp; else s -= tmp;
		}
		// if (j==r1) printf("%f ",s);
		if (s >= s2) {
			s2 = s;
			r2 = j;
		}
	}
	// printf("%f\n", s2);
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
		test(testtrainLists[i],  testPositionE1[i], testPositionE2[i], testtrainLength[i], testheadList[i], testtailList[i], testrelationList[i], res, r, testentityE1[i], testentityE2[i]);
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
	free(matrixW1Dao);
	free(matrixB1Dao);
	free(r);
}

void Init() {
	__init();
	char buffer[1000];
	testentityE1.clear();
	testentityE2.clear();

	FILE *fin=fopen("out/entity2id.txt","r");
	while (fscanf(fin,"%s",buffer)==1) {
		int id;
		fscanf(fin,"%d",&id);
		string entity=buffer;
		entity2id[entity]=id;
	}
	fclose(fin);

	relationTotal = 0;
	fin=fopen("out/relation2id.txt","r");
	while (fscanf(fin,"%s",buffer)==1) {
		int id;
		fscanf(fin,"%d",&id);
		string entity=buffer;
		relation2id[entity]=id;
		relationTotal++;
	}
	fclose(fin);
	
	double num;

	fin = fopen("out/entity2vec.bern","r");
	while (fscanf(fin,"%lf",&num)==1) {
		double *a=(double *)calloc(dimension,sizeof(double));
		a[0]=num;
		for (int i = 1;i<dimension;i++) {
			fscanf(fin,"%lf",&num);
			a[i]=num;
		}
		entity2vec.push_back(a);
	}
	fclose(fin);

	fin = fopen("out/relation2vec.bern","r");
	while (fscanf(fin,"%lf",&num)==1) {
		double *a=(double *)calloc(dimension,sizeof(double));
		a[0]=num;
		for (int i = 1;i<dimension;i++) {
			fscanf(fin,"%lf",&num);
			a[i]=num;
		}
		relation2vec.push_back(a);
	}
	fclose(fin);

	FILE *f = fopen("cnn/data/vectors.bin", "rb");
		fscanf(f, "%lld", &wordTotal);
		fscanf(f, "%lld", &dimension);
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
	
	f = fopen("cnn/data/test/test.txt", "r");	
	while (fscanf(f,"%s",buffer)==1)  {
		int head = wordMapping[(std::string)(buffer)];
		int head1 = entity2id[(std::string)(buffer)];
		fscanf(f,"%s",buffer);
		int tail = wordMapping[(std::string)(buffer)];
		int tail1 = entity2id[(std::string)(buffer)];
		fscanf(f,"%s",buffer);
		int num = relation2id[(string)(buffer)];
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
			testentityE1.push_back(head1);
			testentityE2.push_back(tail1);
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

		matrixRelation_1 = (float *)calloc(dimensionC * relationTotal, sizeof(float));

		positionVecE1 = (float *)calloc(PositionTotalE1 * dimensionWPE, sizeof(float));
		positionVecE2 = (float *)calloc(PositionTotalE2 * dimensionWPE, sizeof(float));
		
		matrixW1 = (float*)calloc(dimensionC * dimension * window, sizeof(float));
		matrixW1PositionE1 = (float *)calloc(dimensionC * dimensionWPE * window, sizeof(float));
		matrixW1PositionE2 = (float *)calloc(dimensionC * dimensionWPE * window, sizeof(float));
		matrixB1 = (float*)calloc(dimensionC, sizeof(float));

		FILE *fout = fopen("out/matrixW1+B1.txt", "r");
		for (int i = 0; i < dimensionC; i++) {
			for (int j = 0; j < dimension * window; j++)
				fscanf(fout, "%f\t",&matrixW1[i* dimension*window+j]);
			for (int j = 0; j < dimensionWPE * window; j++)
				fscanf(fout, "%f\t",&matrixW1PositionE1[i* dimensionWPE*window+j]);
			for (int j = 0; j < dimensionWPE * window; j++)
				fscanf(fout, "%f\t",&matrixW1PositionE2[i* dimensionWPE*window+j]);
			fscanf(fout, "%f\n", &matrixB1[i]);
		}
		fclose(fout);

		fout = fopen("out/matrixRl.txt", "r");
		for (int i = 0; i < relationTotal; i++) {
			for (int j = 0; j < dimensionC; j++)
				fscanf(fout, "%f\t", &matrixRelation_1[i * dimensionC + j]);
			fscanf(fout, "\n");
		}
		fclose(fout);

		fout = fopen("out/matrixPosition.txt", "r");
		for (int i = 0; i < PositionTotalE1; i++) {
			for (int j = 0; j < dimensionWPE; j++)
				fscanf(fout, "%f\t", &positionVecE1[i * dimensionWPE + j]);
			fscanf(fout, "\n");
		}
		for (int i = 0; i < PositionTotalE2; i++) {
			for (int j = 0; j < dimensionWPE; j++)
				fscanf(fout, "%f\t", &positionVecE2[i * dimensionWPE + j]);
			fscanf(fout, "\n");
		}
		fclose(fout);
}

int main() {
	Init();
	test();
	return 0;
}