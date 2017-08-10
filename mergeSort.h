#include <vector>
#include <assert.h>
typedef struct _control_block {
    int N;
    int minN;
    int nreps;
    int NT;
    int seedVal;
    int sd;
    bool par_merge, worst, best;
    std::string o_file, v_file;
} control_block;

inline
int ilog(int x){
    assert(x > 0);
    for (int z=0; z< 33; z++){
        if (x == 1)
            return(z);
        x >>= 1;
    }
    assert(1);          // This should never happen, failure if we get here
    return(-77);        // Return a value to humor the compiler
}

//
// External declarations
void Merge(std::vector<int> *keysIn, std::vector<int> *keysOut, int l0, int l1);
void DC_MergeSort(std::vector<int> *keysIn, int l0, int l1, std::vector<int> *keysOut, int level, bool& direction_flag);
