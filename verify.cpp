#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>    // std::binary_search
#include "mergeSort.h"

extern  control_block cb;

// Verify against the specified v_file
bool verify(std::vector<int> Keys, int lo, int hi, std::string v_file)
{
    std::ifstream f(v_file, std::ios::in | std::ios::binary);
    int x;
    for (int i = 0 ; i < lo; i++)
		f.read((char*)&x, sizeof(int)); // skip these values
    
	for (int i = lo ; i < hi; i++) {
        f.read((char*)&x, sizeof(int));
        if (x != Keys[i]) {
            return false;
        }
    }
    f.close();
    return true;
}


//
//
//  Sweep over the data verifying that it is correct
//  Note that verification of worst and best case input is trival
//  as the keys are taken from a simple, known, arithmetic progression.
//
//  Random input is different. This test is weak in the sense that, it
//  confirms only that the Keys list is sorted, not that the Keys are correct!
//
//  For that you need to also check that every input key is found
//  in the list of output keys, and you may enable this code
//  by uncommenting the line shown below
//  Beware, as this test is costly
//
//


bool exactVerify(std::vector<int> Keys, int lo, int hi) {

    // Best and worst case are trivial to verify
    if (cb.worst){
        for (auto i=lo;i<hi; i++) {
             if (Keys[i] != i)
                 return(0);
        }
        return 1;
    }
    else if (cb.best){
        for (auto i=lo;i<hi; i++) {
             if (Keys[i] != i)
                 return(0);
        }
        return 1;
    }
    // If the random case (and the list isn't too long,
    // we sort the keys using the C++ built in sort on 1 core
    // and compare
    // Initialize (seed) the random number generator
      srand(cb.sd);
     
      std::vector<int> *KeysE = new std::vector<int>();
      assert(KeysE);
      for (auto i=0; i< cb.N; i++)
          KeysE->push_back(rand());

      std::sort(KeysE->begin(),KeysE->begin()+cb.N);
      bool areEqual = std::equal(KeysE->begin(),KeysE->begin()+cb.N,Keys.begin());
      delete KeysE;
      return areEqual;
}

bool weakVerify(std::vector<int> Keys, int lo, int hi) {
    for (auto i=lo;i<hi; i++) {
        if (Keys[i] > Keys[i+1]){
            return(0);
	}
    }
   return 1;
}
