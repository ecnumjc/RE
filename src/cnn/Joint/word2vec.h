#ifndef WORD2VEC_H
#define WORD2VEC_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <thread>
#include <string>
#include <functional>
#include "header.h"

#define EXP_TABLE_SIZE 1000
#define MAX_EXP 6
#define MAX_SENTENCE_LENGTH 1000
#define MaxCodeLength 40

const long long MAXINITIAL = 1000000000000000LL;

class word2vec {
public:
	struct Vocab {
	long long f;
	int *point;
	char *word, *code, codelen;
};

unsigned long long RandomMutiplyNumber, RandomAddNumber, nextRandom;
unsigned long long hashSize; real hashLimit;
unsigned long long VocabMaxSize, vocabSize, VocabSizeStep;

unsigned long long NegativeTableSize;

unsigned long long wordFrequency;
int MaxStringLength;
int MinimalReduce, MinimalCount;
int dimension;
int hs, negative, binary, cbow, window, ThreadNums;
int iter;
real alpha, sample;
long long word_count_actual;
double starting_alpha;


struct Vocab *vocabList;
int *vocabHash;
int *NegativeTable;
real *syn0, *syn1, *syn1neg, *expTable;
char trainFile[100];
char outputFile[100];
long long fileSize;
ShareEntity *ShareHead;

unsigned long long GetNextRandom() {
	nextRandom = nextRandom * RandomMutiplyNumber + RandomAddNumber;
	return nextRandom;
}

void Initial() {
	vocabHash = (int *)calloc(hashSize,sizeof(int));
	vocabList = (struct Vocab *)calloc(VocabMaxSize,sizeof(Vocab));
	expTable = (real *)malloc((EXP_TABLE_SIZE + 1) * sizeof(real));
	for (int i = 0; i < EXP_TABLE_SIZE; i++) {
		expTable[i] = exp((i / (real)EXP_TABLE_SIZE * 2 - 1) * MAX_EXP);
		expTable[i] = expTable[i] / (expTable[i] + 1);
	}
}

int GetHash(const char *word) {
	unsigned long long res = 0;
	for (int i = 0; i < strlen(word); i++) 
		res = res * 257 + word[i];
	return res % hashSize;
}

void AddHash(const char *word, int number) {
	int hash = GetHash(word);
	while (vocabHash[hash] != -1) hash = (hash + 1) % hashSize;
	vocabHash[hash] = number;
}

void HashIntital() {
	for (int i = 0; i < hashSize; i++) vocabHash[i] = -1;
	for (int i = 0; i < vocabSize; i++) AddHash(vocabList[i].word,i);
}

int SearchWord(const char *word) {
	int hash = GetHash(word);
	while (1) {
		int tmp = vocabHash[hash];
		if (tmp == -1) return -1;
	if (!strcmp(word, vocabList[tmp].word)) return tmp;
	hash = (hash + 1) % hashSize;
	}
	return -1;
}

int AddVocab(const char *word) {
	int length = strlen(word) + 1;
	if (length > MaxStringLength) length = MaxStringLength;
	vocabList[vocabSize].word = (char *)calloc(length, sizeof(char));
	strcpy(vocabList[vocabSize].word, word);
	vocabList[vocabSize].f = 0;
	vocabSize++;
	if (vocabSize + 2 >= VocabMaxSize) {
		VocabMaxSize += VocabSizeStep;
		vocabList = (struct Vocab *)realloc(vocabList, VocabMaxSize * sizeof(struct Vocab));
	}
	AddHash(word,vocabSize-1);
	return vocabSize - 1;
}

void DeleteVocab() {
	int vocabSize1 = vocabSize;
	vocabSize = 0;
	for (int i = 0; i < vocabSize1; i++)
		if (vocabList[i].f > MinimalReduce) {
			vocabList[vocabSize].f = vocabList[i].f;
			vocabList[vocabSize].word = vocabList[i].word;
			vocabSize++;
		} else 
			free(vocabList[i].word);
	HashIntital();
	MinimalReduce++;
	printf("%llu\n",vocabSize);
}

static int VocabCompare(const void *a, const void *b) {
	return ((struct Vocab *)b)->f - ((struct Vocab *)a)->f;
}

void SortVocab() {
	qsort(&vocabList[1], vocabSize - 1, sizeof(struct Vocab), VocabCompare);
	long long size = vocabSize;
	for (int i = 0; i < size; i++) 
		if (vocabList[i].f < MinimalCount && i) {
			free(vocabList[i].word);
			vocabSize--;
		} else {
			//AddHash(vocabList[i].word,i);
			wordFrequency+=vocabList[i].f;
		}
	vocabList = (struct Vocab *)realloc(vocabList, (vocabSize + 1) * sizeof(struct Vocab));
	HashIntital();
	for (int i = 0; i < vocabSize; i++) {
		vocabList[i].code=(char *)calloc(MaxCodeLength,sizeof(char));
		vocabList[i].point=(int *)calloc(MaxCodeLength,sizeof(int));
	}
}

void ReadWord(char *word, FILE *fin) {
	long long i = 0;
	for ( ; !feof(fin); ) {
		char ch = fgetc(fin);
		if (ch == 13) continue;
		if (ch == ' ' || ch=='\t' || ch=='\n') {
			if (i >0 ) {
				if (ch == '\n') ungetc(ch, fin);
				break;
			}
			if (ch == '\n') {
				strcpy(word, (char *)"</s>");
				return;
			} else continue;
		}
		word[i] = ch;
		i += (i < MaxStringLength);
	}
	word[i] = 0;
}

void LoadFromTrainFile(char *route) {
	FILE *fin;
	char word[MaxStringLength];
	fin = fopen(route,"rb");
	HashIntital();
	AddVocab((char *)"</s>");
	while (!feof(fin)) {
		ReadWord(word,fin);
		if (feof(fin)) break;
		int tmp = SearchWord(word);
		if (tmp == -1) tmp = AddVocab(word);
		vocabList[tmp].f++;
		if (vocabSize > hashSize * hashLimit) DeleteVocab();
	}
	fseek(fin, 0, SEEK_END);
	fileSize = ftell(fin);
	fclose(fin);
	SortVocab();
}

void InitNegativeTable() {
	real power = 0.75, tot = 0;
	NegativeTable = (int *)malloc(NegativeTableSize * sizeof(int));
	for (long long i = 0; i < vocabSize; i++) tot += pow(vocabList[i].f, power);
	real j = pow(vocabList[0].f,power) / tot;
	for (long long i = 0, step = 0; i < NegativeTableSize; i++) {
		if (j * NegativeTableSize < i) j += pow(vocabList[++step].f, power) / tot;
		if (step >= vocabSize) step--;
		NegativeTable[i] = step;
	}
}

void InitTree() {
	char code[MaxCodeLength];
	long long point[MaxCodeLength];

	long long *count = (long long *)calloc(vocabSize * 2 + 1, sizeof(long long));
	long long *binary = (long long *)calloc(vocabSize * 2 + 1, sizeof(long long));
	long long *father = (long long *)calloc(vocabSize * 2 + 1, sizeof(long long));

	for (long long i = 0; i < vocabSize; i++) count[i] = vocabList[i].f;
	for (long long i = vocabSize; i < (vocabSize << 1); i++) count[i] = MAXINITIAL;

	for (long long i = 0, lef = vocabSize - 1, rig = vocabSize; i < vocabSize - 1; i++) {
		long long minimal[2];
		for (long long j = 0; j < 2; j++)
			if (lef >= 0 && count[lef] < count[rig]) {
				minimal[j] = lef; lef--;
			} else {
				minimal[j] = rig; rig++;
			}
		count[vocabSize + i] = count[minimal[0]] + count[minimal[1]];
		father[minimal[1]] = father[minimal[0]] = vocabSize + i;
		binary[minimal[1]] = 1;
	}
	for (long long i = 0; i < vocabSize; i++) {
		long long k = 0;
		for (long long j = i; j != (vocabSize << 1)-2; j = father[j], k++) {
			code[k] = binary[j];
			point[k] = j;
		}
		vocabList[i].codelen = k;
		vocabList[i].point[0] = vocabSize - 2;
		for (long long j = 0; j < k; j++) {
			vocabList[i].code[k - j - 1] = code[j];
			vocabList[i].point[k - j] = point[j] - vocabSize;
		}	
	}
	free(count);
	free(binary);
	free(father);
}

void InitNet() {
	long long a = posix_memalign((void **)&syn0, 128, (long long)vocabSize * dimension * sizeof(real));
	if (hs) {
		long long a = posix_memalign((void **)&syn1, 128, (long long)vocabSize * dimension * sizeof(real));
		for (long long i = 0; i < vocabSize; i++)
			for (long long j = 0; j < dimension; j++)
				syn1[i * dimension + j] = 0;
	}
	if (negative>0) {
		long long a = posix_memalign((void **)&syn1neg, 128, (long long)vocabSize * dimension * sizeof(real));
		for (long long i = 0; i < vocabSize; i++)
			for (long long j = 0; j < dimension; j++)
				syn1neg[i * dimension + j] = 0;
	}
	nextRandom = 1;
	for (long long i = 0; i < vocabSize; i++)
	for (long long j = 0; j < dimension; j++) {
			unsigned long long next_random = GetNextRandom();
			syn0[i * dimension + j] = (((next_random & 0xFFFF) / (real)65536) - 0.5) / dimension;
		}
	InitTree();
	InitNegativeTable();
}

void OutEmbeddings() {
	FILE *fout = fopen(outputFile, "wb");
	fprintf(fout, "%llu %d\n", vocabSize, dimension);
	for (long long i = 0; i < vocabSize; i++) {
		fprintf(fout, "%s ", vocabList[i].word);
		if (binary) 
			for (long long j = 0; j < dimension; j++) fwrite(&syn0[i * dimension + j], sizeof(real), 1, fout);
		else
			for (long long j = 0; j < dimension; j++) fprintf(fout, "%lf ", syn0[i * dimension + j]);
		fprintf(fout, "\n");
	}
	fclose(fout);
}

int ReadWordIndex(FILE *fin) {
	char word[MaxStringLength];
	ReadWord(word, fin);
	if (feof(fin)) return -1;
	return SearchWord(word);
}

void TrainModelThread(long long id) {
  long long a, b, d, cw, word, last_word, sentence_length = 0, sentence_position = 0;
  long long word_count = 0, last_word_count = 0, sen[MAX_SENTENCE_LENGTH + 1];
  long long l1, l2, c, target, label, local_iter = iter;
  real f, g;
  real *neu1 = (real *)calloc(dimension, sizeof(real));
  real *neu1e = (real *)calloc(dimension, sizeof(real));

  FILE *fin = fopen(trainFile, "rb");
  fseek(fin, fileSize / (long long)ThreadNums * (long long)id, SEEK_SET);
  clock_t start = clock();
  clock_t now;
  unsigned long long next_random = (long long)id;

    //  FILE*   logg=fopen("t2.txt","w");
    // fclose(logg);
//   printf("%d\n", id);
// int hx = 0;
  while (1) {
  	// hx=hx+1;
  	// printf("%d\n", hx );
    if (word_count - last_word_count > 10000) {
      word_count_actual += word_count - last_word_count;
      last_word_count = word_count;
        now=clock();
       // printf("%llu\n",wordFrequency);
        printf("%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, alpha,
         word_count_actual / (real)(iter * wordFrequency + 1) * 100,
         word_count_actual / ((real)(now - start + 1) / (real)CLOCKS_PER_SEC * 1000));
        fprintf(logg,"%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, alpha,
         word_count_actual / (real)(iter * wordFrequency + 1) * 100,
         word_count_actual / ((real)(now - start + 1) / (real)CLOCKS_PER_SEC * 1000));
        fflush(stdout);
      alpha = starting_alpha * (1 - word_count_actual / (real)(iter * wordFrequency + 1));
      if (alpha < starting_alpha * 0.0001) alpha = starting_alpha * 0.0001;
    }
    if (sentence_length == 0) {
      while (1) {
        word = ReadWordIndex(fin);
        if (feof(fin)) break;
        if (word == -1) continue;
        word_count++;
        if (word == 0) break;
        // The subsampling randomly discards frequent words while keeping the ranking same
        if (sample > 0) {
          real ran = (sqrt(vocabList[word].f / (sample * wordFrequency)) + 1);
	ran=ran * (sample * wordFrequency);
	ran=ran / vocabList[word].f;
          next_random = next_random * (unsigned long long)25214903917 + 11;
          if (ran < (next_random & 0xFFFF) / (real)65536) continue;
        }
        sen[sentence_length] = word;
        sentence_length++;
        if (sentence_length >= MAX_SENTENCE_LENGTH) break;
      }
      sentence_position = 0;
    }
    if (feof(fin) || (word_count > wordFrequency / ThreadNums)) {
      word_count_actual += word_count - last_word_count;
      local_iter--;
      if (local_iter == 0) break;
      word_count = 0;
      last_word_count = 0;
      sentence_length = 0;
      fseek(fin, fileSize / (long long)ThreadNums * (long long)id, SEEK_SET);
      continue;
    }
    word = sen[sentence_position];
    if (word == -1) continue;
    for (c = 0; c < dimension; c++) neu1[c] = 0;
    for (c = 0; c < dimension; c++) neu1e[c] = 0;
          next_random = next_random * (unsigned long long)25214903917 + 11;
    b = next_random % window;
    if (cbow) {  //train the cbow architecture
      // in -> hidden
      cw = 0;
      for (a = b; a < window * 2 + 1 - b; a++) if (a != window) {
        c = sentence_position - window + a;
        if (c < 0) continue;
        if (c >= sentence_length) continue;
        last_word = sen[c];
        if (last_word == -1) continue;
        for (c = 0; c < dimension; c++) neu1[c] += syn0[c + last_word * dimension];
        cw++;
      }
      if (cw) {
        for (c = 0; c < dimension; c++) neu1[c] /= cw;
        if (hs) for (d = 0; d < vocabList[word].codelen; d++) {
          f = 0;
          l2 = vocabList[word].point[d] * dimension;
          // Propagate hidden -> output
          for (c = 0; c < dimension; c++) f += neu1[c] * syn1[c + l2];
          if (f <= -MAX_EXP) continue;
          else if (f >= MAX_EXP) continue;
          else f = expTable[(int)((f + MAX_EXP) * ((double)EXP_TABLE_SIZE / (double)MAX_EXP / 2))];
          // 'g' is the gradient multiplied by the learning rate
          g = (1 - vocabList[word].code[d] - f) * alpha;
          // Propagate errors output -> hidden
          for (c = 0; c < dimension; c++) neu1e[c] += g * syn1[c + l2];
          // Learn weights hidden -> output
          for (c = 0; c < dimension; c++) syn1[c + l2] += g * neu1[c];
        }
        // NEGATIVE SAMPLING
        if (negative > 0) for (d = 0; d < negative + 1; d++) {
          if (d == 0) {
            target = word;
            label = 1;
          } else {
          next_random = next_random * (unsigned long long)25214903917 + 11;
            target = NegativeTable[(next_random >> 16) % NegativeTableSize];
            if (target == 0) target = next_random % (vocabSize - 1) + 1;
            if (target == word) continue;
            label = 0;
          }
          l2 = target * dimension;
          f = 0;
          for (c = 0; c < dimension; c++) f += neu1[c] * syn1neg[c + l2];
          if (f > MAX_EXP) g = (label - 1) * alpha;
          else if (f < -MAX_EXP) g = (label - 0) * alpha;
          else g = (label - expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]) * alpha;
          for (c = 0; c < dimension; c++) neu1e[c] += g * syn1neg[c + l2];
          for (c = 0; c < dimension; c++) syn1neg[c + l2] += g * neu1[c];

        }
//        hidden -> in
        for (a = b; a < window * 2 + 1 - b; a++) if (a != window) {
          c = sentence_position - window + a;
          if (c < 0) continue;
          if (c >= sentence_length) continue;
          last_word = sen[c];
          if (last_word == -1) continue;
          for (c = 0; c < dimension; c++) syn0[c + last_word * dimension] += neu1e[c];
          Share(last_word);
        }

      }
    } else {  //train skip-gram
      for (a = b; a < window * 2 + 1 - b; a++) if (a != window) {
        c = sentence_position - window + a;
        if (c < 0) continue;
        if (c >= sentence_length) continue;
        last_word = sen[c];
        if (last_word == -1) continue;
        l1 = last_word * dimension;

        for (c = 0; c < dimension; c++) neu1e[c] = 0;
        // HIERARCHICAL SOFTMAX
        if (hs) for (d = 0; d < vocabList[word].codelen; d++) {
          f = 0;
          l2 = vocabList[word].point[d] * dimension;
          // Propagate hidden -> output
          for (c = 0; c < dimension; c++) f += syn0[c + l1] * syn1[c + l2];
          if (f <= -MAX_EXP) continue;
          else if (f >= MAX_EXP) continue;
          else f = expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))];
          // 'g' is the gradient multiplied by the learning rate
          g = (1 - vocabList[word].code[d] - f) * alpha;
          // Propagate errors output -> hidden
          for (c = 0; c < dimension; c++) neu1e[c] += g * syn1[c + l2];
          // Learn weights hidden -> output
          for (c = 0; c < dimension; c++) syn1[c + l2] += g * syn0[c + l1];
        }
        // NEGATIVE SAMPLING
        if (negative > 0) for (d = 0; d < negative + 1; d++) {
          if (d == 0) {
            target = word;
            label = 1;
          } else {
          next_random = next_random * (unsigned long long)25214903917 + 11;
            target = NegativeTable[(next_random >> 16) % NegativeTableSize];
            if (target == 0) target = next_random % (vocabSize - 1) + 1;
            if (target == word) continue;
            label = 0;
          }
          l2 = target * dimension;
          f = 0;
          for (c = 0; c < dimension; c++) f += syn0[c + l1] * syn1neg[c + l2];
          if (f > MAX_EXP) g = (label - 1) * alpha;
          else if (f < -MAX_EXP) g = (label - 0) * alpha;
          else g = (label - expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]) * alpha;
          for (c = 0; c < dimension; c++) neu1e[c] += g * syn1neg[c + l2];
          for (c = 0; c < dimension; c++) syn1neg[c + l2] += g * syn0[c + l1];
        }
        for (c = 0; c < dimension; c++) syn0[c + l1] += neu1e[c];
        Share(last_word);
      }
    }

    sentence_position++;
    if (sentence_position >= sentence_length) {
      sentence_length = 0;
      continue;
    }
  }
  fclose(fin);
  free(neu1);
  free(neu1e);
}

void TrainModel(char *route) {
	word_count_actual = 0;
	starting_alpha = alpha;
	
	std::thread *T = new std::thread[ThreadNums];
	for (int i = 0;i < ThreadNums; i++)
		T[i] = std::thread(std::bind(&word2vec::TrainModelThread, this, i));

	for (int i = 0; i < ThreadNums; i++) T[i].join();
		alpha=starting_alpha;
}

int ArgPos(char *str, int argc, char **argv) {
	int a;
	for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
		if (a == argc - 1) {
			printf("Argument missing for %s\n", str);
			exit(1);
		}
		return a;
	}
	return -1;
}

void init() {
	RandomMutiplyNumber = 25214903917, RandomAddNumber = 11, nextRandom = 0;
	hashSize = 3000000;
	hashLimit = 0.7;
	VocabMaxSize = 1000, vocabSize = 0, VocabSizeStep = 1000;

	NegativeTableSize = 1e8;
	wordFrequency = 0;
	MaxStringLength = 100;
	MinimalReduce = 1, MinimalCount = 5;
	dimension = 100;
	hs = 0;
	negative = 5;
	binary = 0, cbow = 1, window = 5, ThreadNums = 1;
	iter = 5;
	alpha=0.025;
	sample = 1e-3;
	starting_alpha = 0.025;
}

word2vec() {
	ShareHead = NULL;
	init();
}

word2vec(int argc, char **argv) {
	init();
	int i;
	outputFile[0] = 0;
	if ((i = ArgPos((char *)"-size", argc, argv)) > 0) dimension = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-train", argc, argv)) > 0) strcpy(trainFile, argv[i + 1]);
	if ((i = ArgPos((char *)"-binary", argc, argv)) > 0) binary = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-cbow", argc, argv)) > 0) cbow = atoi(argv[i + 1]);
	if (cbow) alpha = 0.05;
	if ((i = ArgPos((char *)"-alpha", argc, argv)) > 0) alpha = atof(argv[i + 1]);
	if ((i = ArgPos((char *)"-output", argc, argv)) > 0) strcpy(outputFile, argv[i + 1]);
	if ((i = ArgPos((char *)"-window", argc, argv)) > 0) window = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-sample", argc, argv)) > 0) sample = atof(argv[i + 1]);
	if ((i = ArgPos((char *)"-hs", argc, argv)) > 0) hs = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-negative", argc, argv)) > 0) negative = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-threads", argc, argv)) > 0) ThreadNums = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-iter", argc, argv)) > 0) iter = atoi(argv[i + 1]);
	if ((i = ArgPos((char *)"-min-count", argc, argv)) > 0) MinimalCount = atoi(argv[i + 1]);
}

real* GetEmbedding() {
	return syn0;
}

void SetIntial() {
	Initial();
	LoadFromTrainFile(trainFile);
	InitNet();
}

void Run() {
	TrainModel(trainFile);
}

void SetDimension(int dimension) {
	this->dimension = dimension;
}

void SetBinary(int binary) {
	this->binary = binary;
}

void SetMethod(int con) {
	this->cbow = con;
	if (cbow) alpha = 0.05;
}

void SetRate(real con) {
	this->alpha = con;
}

  void SetTrain(char *trainFile) {
    strcpy(this->trainFile, trainFile);
  }

  void SetOutput(char *output) {
    strcpy(this->outputFile, output);
  }

  void SetTrain(std::string train) {
    strcpy(this->trainFile,train.c_str());
  }

  void SetOutput(std::string output) {
    strcpy(this->outputFile, output.c_str());
  }

  void SetWindow(int con) {
    this->window = con;
  }

  void SetHs(int con) {
    this->hs = con;
  }

  void SetSample(real con) {
    this->sample = con;
  }

  void SetNegative(int con) {
    this->negative = con;
  }

  void SetThread(int con) {
    this->ThreadNums = con;
  }

  void SetIter(int con) {
    this->iter = con;
  }

  void SetMinCount(int con) {
    this->MinimalCount = con;
  }

  void SetShare(real *matrix, int *dui, int total = 0) {
    ShareEntity *con = new ShareEntity(matrix, dui, total);
    if (ShareHead == NULL) {ShareHead = con;printf("1\n");} else {
      con->next = ShareHead;
      ShareHead = con;
    }
  }

  void Share(int con) {
      ShareEntity *head = ShareHead;
      while (head != NULL) {
          int l2 = head->dui[con];
          if (l2 >=0 ) {
            int l1 = con * dimension;
            l2 = l2 * dimension;
            // for (int j = 0; j < dimension; j++)
            //     head -> mat[l2 + j] = syn0[l1 + j];
            memcpy(head->mat + l2, syn0 + l1, sizeof(real) * dimension);
          }
          head = head -> next;
      }
  }

};

#endif
