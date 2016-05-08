
typedef struct words
{
	char word[100];
	char type[10]; //词性或实体类型
} Word;

typedef struct result
{
	char r[100];
} Result;
Result results[43];

FILE *in, *out;
class FEClass
{
public:
	FEClass();
	~FEClass();
	Word word[20000];
	int wnum;
	int arg1;
	int arg2;
	char source[5000];

	void init();
	void process(char *s);
	int judge();
	void extract();
private:

};

FEClass::FEClass()
{
}

FEClass::~FEClass()
{
}
FEClass fe;