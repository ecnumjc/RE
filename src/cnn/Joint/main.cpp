#include "controller.h"

int main(int argc, char **argv) {
	logg=fopen("log.txt","w");
	if (argc <=1 ) {
		Controller corl;
		corl.Explain();
	} else {
		Controller corl(argc,argv);
		corl.Explain();		
	}
	return 0;
}
