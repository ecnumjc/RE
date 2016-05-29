#include <cstring>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <map>
#include <cmath>
#include <pthread.h>
#include "init.h"
#include "test.h"

using namespace std;

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
	for (int j = 1; j < relationTotal; j++) {
		float s = 0;
		for (int i = 0; i < dimensionC; i++)
			s +=  matrixRelation[j * dimensionC + i] * r[i];
		if (j == r1) s1 = s; else
		if (s >= s2) {
			s2 = s;
			r2 = j;
		}
	}

	if (r1!=0) {
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
	}
	int lef = r2;
	int rig = r2;
	if (r1 == 0) {
		lef = 1;
		rig = relationTotal - 1;
	}
	for (r2 = lef; r2 <= rig; r2++) {
		s2 = 0;
		for (int i = 0; i < dimensionC; i++)
			s2 +=  matrixRelation[r2 * dimensionC + i] * r[i];
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

void* trainMode(void *con ) {
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
		// float alpha1 = trainTimes*(trainTimes-turn);
		// printf("%f\n",alpha1);
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
				for (int j = 0;j <relationTotal;j++)
					matrixRelation[j * dimensionC +i] += matrixRelationDao[j * dimensionC + i];
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

	matrixRelation = (float*)calloc(dimensionC * relationTotal, sizeof(float));
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
		// tmp = sqrt(tmp);
		// for (int j = 0; j < dimensionWPE; j++) 
		// 	positionVecE1[i * dimensionWPE + j] = positionVecE1[i * dimensionWPE + j] / tmp; 		
	}

	for (int i = 0; i < PositionTotalE2; i++) {
		float tmp = 0;
		for (int j = 0; j < dimensionWPE; j++) {
			positionVecE2[i * dimensionWPE + j] = getRandU(-con1, con1);
			tmp += positionVecE2[i * dimensionWPE + j] * positionVecE2[i * dimensionWPE + j];
		}
		// tmp = sqrt(tmp);
		// for (int j = 0; j < dimensionWPE; j++) 
		// 	positionVecE2[i * dimensionWPE + j] = positionVecE2[i * dimensionWPE + j] / tmp; 		
	}

	for (turn = 0; turn < trainTimes; turn ++) {
		pthread_t *pt = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
		for (int a = 0; a < num_threads; a++)
			pthread_create(&pt[a], NULL, trainMode,  (NULL));
		for (int a = 0; a < num_threads; a++)
			pthread_join(pt[a], NULL);
		free(pt);
		printf("%d : ", turn);
		test();
		if ((turn+1)%1==0)
			rate=rate*reduce;
	}

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
			fprintf(fout, "%f\t", matrixRelation[i * dimensionC + j]);
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

int main(int argc, char ** argv) {
	logg = fopen("log.txt","w");
	init();
	train();
	fclose(logg);
}
