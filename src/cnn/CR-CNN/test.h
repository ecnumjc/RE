#ifndef TEST_H
#define TEST_H
#include "init.h"
#include <algorithm>

int tipp = 0;
float ress = 0;

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
			s +=  matrixRelation[j * dimensionC + i] * r[i];
		if (s >= s2) {
			s2 = s;
			r2 = j;
		}
	}

	tipp = r2;
	if (s2 < 0) tipp = 0; 
	ress = s2;
}

float mxx = 0;
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
	float mxxy = res / 2263;
	mxxx = 2/((1/mxxx)+(1/mxxy));
	printf("%.6f\n: ",mxxx);
	if (mxxx>mxx) {
		mxx=mxxx;
		FILE *out=fopen("out/output.txt","w");
		for (int i=re1-1;i>=0;i--)
			fprintf(out, "%d %s\n", work[i].num, nam[work[i].ans].c_str());
		fclose(out);
	}

	free(matrixW1Dao);
	free(matrixB1Dao);
	free(r);
}

#endif
