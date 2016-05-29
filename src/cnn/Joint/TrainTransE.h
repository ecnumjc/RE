#ifndef __BiaoShi_TrainTransE_h__
#define __BiaoShi_TrainTransE_h__

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <cstring>
#include <thread>
#include "header.h"

class TrainTransE {
private:

	bool flagL1;
	unsigned long long nextRandom;

	
	int n, method;
	char version[10];
	int thread;
	real res;
	real rate, margin;
	real belta;
	int* fb_h, *fb_l, *fb_r;
	int fb_num, relation_num, entity_num;
	real *relation_vec, *entity_vec;
	real *relation_tmp, *entity_tmp;
	real *left_num, *right_num;
	int nepoch;
    ShareEntity *ShareHead;

	void Rand();

	real RandInitialize(real min, real max);

	real NormalDouble(real x, real miu, real sigma);

	real RandN(real miu, real sigma, real min, double max);

	real Sqr(const real x) const;
    
    real VecLen(real *a, const int aBegin) const;
    
    void Norm(real *a, const int aBegin);

	int RandMax(int x);

	real CalcSum(int e1, int e2, int rel);
	void Gradient(int e1_a, int e2_a, int rel_a, int e1_b, int e2_b, int rel_b);
	void TrainKb(int e1_a, int e2_a, int rel_a, int e1_b, int e2_b, int rel_b, real &res);
	int FindOk(int e1, int e2, int rel);
	void Bfgs();
	void PrintEmbedding();
    void Share();
	void Clear();

public:

	TrainTransE();

	void SetN(int inN);

	void SetRate(real inRate);

	void SetMargin(real inMargin);

	void SetMethod(int inMethod);

	void SetNepoch(int inNepoch);

	void SetThread(int inTread);

	void SetRelationNum(int inRelationNum);

	void SetEntityNum(int inEntityNum);

	void SetRandom(real *inEntityVec, real *inRelationVec);

    void SetShare(real *control, int *dui, int tot);

	~TrainTransE();

	void SetTrainFile(int *fb_h, int *fb_l, int *fb_r, int fb_num);
	bool Run(real *inEntityVec, real *inRelationVec);
};


void TrainTransE::Rand() {
    nextRandom = nextRandom * (unsigned long long) 25214903917 + 11;
    //nextRandom = rand() ;
}

real TrainTransE::RandInitialize(real min, real max) {
    Rand();
    return min + (max - min) * (1.0 * nextRandom / INT64_MAX / 2);
}

real TrainTransE::NormalDouble(real x, real miu, real sigma) {
    return 1.0 / sqrt(2 * pi) / sigma * exp(-1 * (x - miu) * (x - miu) / (2 * sigma * sigma));
}

real TrainTransE::RandN(real miu, real sigma, real min, double max) {
    real x, y, dScope;
    do {
        x = RandInitialize(min, max);
        y = NormalDouble(x, miu, sigma);
        dScope = RandInitialize(0.0, NormalDouble(miu, miu, sigma));
    } while (dScope > y);
    return x;
}

real TrainTransE::Sqr(const real x) const {
    return x * x;
}

real TrainTransE::VecLen(real *a, const int aBegin) const {
    real res = 0;
    for (int i = 0; i < n; i++) {
        res += Sqr(a[aBegin + i]);
    }
    res = sqrt(res);
    return res;
}

void TrainTransE::Norm(real *a, const int aBegin) {
    real x = VecLen(a, aBegin);
    if (x > 1)
        for (int ii=0; ii < n; ii++)
            a[aBegin + ii] /= x;
}

int TrainTransE::RandMax(int x) {
    Rand();
    int res = nextRandom % x;
    while (res<0)
        res += x;
    return res;
}

real TrainTransE::CalcSum(int e1, int e2, int rel) {
    real sum=0;
    if (flagL1)
        for (int ii = 0; ii < n; ii++)
            sum += fabs(entity_tmp[e2 * n + ii]-entity_tmp[e1 * n + ii]-relation_tmp[rel * n + ii]);
    else
        for (int ii = 0; ii < n; ii++)
            sum += Sqr(entity_tmp[e2 * n + ii]-entity_tmp[e1 * n + ii]-relation_tmp[rel * n + ii]);
    return sum;
}

void TrainTransE::Gradient(int e1_a, int e2_a, int rel_a, int e1_b, int e2_b, int rel_b) {
    for (int ii = 0; ii < n; ii++) {
        real x = 2 * (entity_tmp[e2_a * n + ii]-entity_tmp[e1_a * n + ii]-relation_tmp[rel_a * n + ii]);
        if (flagL1) {
            if (x>0)
                x = 1;
            else
                x = -1;
        }
        
        relation_vec[rel_a * n + ii]-=-1*rate*x;
        entity_vec[e1_a * n + ii]-=-1*rate*x;
        entity_vec[e2_a * n + ii]+=-1*rate*x;
        
        x = 2 * (entity_tmp[e2_b * n + ii]-entity_tmp[e1_b * n + ii]-relation_tmp[rel_b * n + ii]);
        if (flagL1) {
            if (x>0)
                x = 1;
            else
                x = -1;
        }

        relation_vec[rel_b * n + ii]-=rate*x;
        entity_vec[e1_b * n + ii]-=rate*x;
        entity_vec[e2_b * n + ii]+=rate*x;
    }
}

void TrainTransE::TrainKb(int e1_a, int e2_a, int rel_a, int e1_b, int e2_b, int rel_b, real &res) {
    double sum1 = CalcSum(e1_a, e2_a, rel_a);
    double sum2 = CalcSum(e1_b, e2_b, rel_b);
    if (sum1 + margin > sum2) {
        res += margin + sum1 - sum2;
        Gradient(e1_a, e2_a, rel_a, e1_b, e2_b, rel_b);
    }
}

int TrainTransE::FindOk(int e1, int e2, int rel) {
    int i = 0, j = fb_num-1;
    int mid = 0;
    int k = 0;
    while (i != j) {
        mid = (i + j) / 2;
        if (fb_h[mid] < e1 || ((fb_h[mid] == e1) && (fb_l[mid] < e2)))
            i = mid + 1;
        else
            j = mid;
    }
    for (;j < fb_num; j++) {
        if (fb_h[j] != e1 || fb_l[j] > e2) return 0;
        if (fb_l[j] == e2 && fb_r[j] == rel) return j + 1;
    }
    return 0;
}

void TrainTransE::Bfgs() {
    int nbatches = 100;
    int batchsize = fb_num / nbatches;
    for (int epoch = 0; epoch < nepoch; epoch++) {
        real res=0;
        for (int batch = 0; batch < nbatches; batch++) {
            memcpy(relation_tmp, relation_vec, sizeof(real) * relation_num * n);
            memcpy(entity_tmp, entity_vec, sizeof(real) * entity_num * n);
            for (int k = 0; k < batchsize; k++)
            {
                int i=RandMax(fb_num);
                int j=RandMax(entity_num);
                real pr = 1000 * right_num[fb_r[i]] / (right_num[fb_r[i]] + left_num[fb_r[i]]);
                if (method == 0)
                    pr = 500;
                Rand();
                if (nextRandom % 1000 < pr)
                {
                    while (FindOk(fb_h[i], j, fb_r[i]))
                        j=RandMax(entity_num);
                    
                    TrainKb(fb_h[i],fb_l[i],fb_r[i],fb_h[i],j,fb_r[i], res);
                }
                else
                {
                    while (FindOk(j, fb_l[i], fb_r[i]))
                        j=RandMax(entity_num);  
                    TrainKb(fb_h[i],fb_l[i],fb_r[i],j,fb_l[i],fb_r[i], res);
                }
                Norm(relation_vec, n * fb_r[i]);
                Norm(entity_vec, n * fb_h[i]);
                Norm(entity_vec, n * fb_l[i]);
                Norm(entity_vec, n * j);
            }
            Share();
        }
        printf("epoch:%d %.6lf\n", epoch, res);
        fprintf(logg, "epoch:%d %.6lf\n", epoch, res);
    }
}

void TrainTransE::PrintEmbedding() {
    char relOutput[30]="";
    char enOutput[30]="";
    sprintf(relOutput, "relation2vec.%s", version);
    sprintf(enOutput, "entity2vec.%s", version);
    FILE* f2 = fopen(relOutput,"w");
    FILE* f3 = fopen(enOutput,"w");
    for (int i = 0, j = 0; i < relation_num * n; i++, j++)
    {
        fprintf(f2,"%.6lf\t",relation_vec[i]);
        if (j == n) {
            fprintf(f2,"\n");
            j = 0;
        }
    }
    for (int i = 0, j = 0; i < entity_num * n; i++, j++)
    {
        fprintf(f3,"%.6lf\t",entity_vec[i]);
        if (j == n) {
            fprintf(f3,"\n");
            j = 0;
        }
    }
    fclose(f2);
    fclose(f3);
}

void TrainTransE::Clear() {
    free(relation_tmp);
    free(entity_tmp);
    free(left_num);
    free(right_num);
}

TrainTransE::TrainTransE() {
    n = 100;
    rate = 0.001;
    margin = 1.0;
    method = 1;
    nepoch = 1000;
    nextRandom = time(0);
    thread = 1;
    flagL1 = 1;
    entity_num = 0;
    relation_num = 0;
    ShareHead = NULL;
    memset(version, 0, sizeof(version));
    memcpy(version, "bern", sizeof(char) * 4);
}

void TrainTransE::SetN(int inN) {
    n = inN;
}

void TrainTransE::SetRate(real inRate) {
    rate = inRate;
}

void TrainTransE::SetMargin(real inMargin) {
    margin = inMargin;
}

void TrainTransE::SetMethod(int inMethod) {
    method = inMethod;
    memset(version, 0, sizeof(version));
    if (method)
        memcpy(version, "bern", sizeof(char) * 4);
    else
        memcpy(version, "unif", sizeof(char) * 4);
}

void TrainTransE::SetNepoch(int inNepoch) {
    nepoch = inNepoch;
}

void TrainTransE::SetThread(int inTread) {
    thread = inTread;
}

void TrainTransE::SetRelationNum(int inRelationNum) {
    relation_num = inRelationNum;
}

void TrainTransE::SetEntityNum(int inEntityNum) {
    entity_num = inEntityNum;
}

TrainTransE::~TrainTransE() {
    free(relation_tmp);
    free(entity_tmp);
    free(left_num);
    free(right_num);
}

void TrainTransE::SetTrainFile(int *fb_h, int *fb_l, int *fb_r, int fb_num) {
    this->fb_num = fb_num;
    this->fb_h = fb_h;
    this->fb_l = fb_l;
    this->fb_r = fb_r;
    relation_tmp = (real *)calloc(relation_num * n, sizeof(real));
    entity_tmp = (real *)calloc(entity_num * n, sizeof(real));
    // relation_vec = (real *)calloc(relation_num * n, sizeof(real));
    // entity_vec = (real *)calloc(entity_num * n, sizeof(real));
    
    int *left_entity, *right_entity, *left_sum, *right_sum;
    left_entity = (int *)calloc(relation_num * entity_num, sizeof(int));
    right_entity = (int *)calloc(relation_num * entity_num, sizeof(int));
    left_sum = (int *)calloc(relation_num, sizeof(int));
    right_sum = (int *)calloc(relation_num, sizeof(int));
    left_num = (real *)calloc(relation_num, sizeof(real));
    right_num = (real *)calloc(relation_num, sizeof(real));
    for (int i = 0; i < fb_num; i++) {
        if (left_entity[fb_r[i] * entity_num + fb_h[i]] == 0) {
            left_sum[fb_r[i]]++;
        }
        if (right_entity[fb_r[i] * entity_num + fb_l[i]] == 0) {
            right_sum[fb_r[i]]++;
        }
        left_entity[fb_r[i] * entity_num + fb_h[i]]++;
        right_entity[fb_r[i] * entity_num + fb_l[i]]++;
        left_num[fb_r[i]] += 1;
        right_num[fb_r[i]] += 1;
    }
    
    for (int i = 0; i < relation_num; i++) {
        left_num[i] /= left_sum[i];
        right_num[i] /= right_sum[i];
    }
    
    free(left_entity);
    free(right_entity);
    free(left_sum);
    free(right_sum);
    
}

void TrainTransE::SetRandom(real *inEntityVec, real *inRelationVec) {

    entity_vec = inEntityVec;
    relation_vec = inRelationVec;

    for (int i = 0; i < entity_num * n; i++) {
        entity_vec[i] = RandN(0, 1.0 / n, -6 / sqrt(n), 6 / sqrt(n));
    }
    for (int i = 0; i < relation_num * n; i++) {
        relation_vec[i] = RandN(0, 1.0 / n, -6 / sqrt(n), 6 / sqrt(n));
    }
    
    for (int i = 0; i < entity_num; i++) {
        Norm(entity_vec, i * n);
    }
    for (int i = 0; i < relation_num; i++) {
        Norm(relation_vec, i * n);
    }

}

bool TrainTransE::Run(real *inEntityVec, real *inRelationVec) {
    
    printf("size = %d\n", n);
    printf("learing rate = %.6lf\n", rate);
    printf("margin = %.6lf\n", margin);
    printf("method = %s\n", version);
    printf("nepoch = %d\n", nepoch);
        
    if(entity_num == 0 || relation_num == 0) {
        printf("None Entity or Relation\n");
        return 1;
    }
    
    printf("Start Run\n");

    entity_vec = inEntityVec;
    relation_vec = inRelationVec;
    
    std::thread *T = new std::thread[thread];
    
    for (int i = 0;i < thread; i++)
        T[i] = std::thread(std::bind(&TrainTransE::Bfgs, this));
    
    for (int i = 0; i < thread; i++) {
        T[i].join();
    }
    
    delete[] T;
        
    return 0;
}

void TrainTransE::SetShare(real *matrix, int *dui, int tot) {
    ShareEntity *con = new ShareEntity(matrix, dui, tot);
    if (ShareHead == NULL) ShareHead = con; else {
        con->next = ShareHead;
        ShareHead = con;
    }
}

void TrainTransE::Share() {
    ShareEntity *head = ShareHead;
    while (head != NULL) {
        for (int i = 0; i < head->tot; i++) {
            int l1 = head->dui[i + i] * n;
            int l2 = head->dui[i + i + 1] * n;
            memcpy(head->mat + l2, entity_vec + l1, sizeof(real) * n);
            // for (int j = 0; j < n; j++)
            //     head -> mat[l2 + j] = entity_vec[l1 + j];
        }
        head = head -> next;
    }
}


#endif
