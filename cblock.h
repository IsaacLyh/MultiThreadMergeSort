typedef struct _control_block {
	int N;
	int minN;
        int nreps;
	int NT;
        int seedVal;
        int sd;
        bool par_merge, worst, best;
} control_block;
