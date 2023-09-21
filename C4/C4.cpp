#include <iostream>
#include "../MCTS/MCTS.h"
#include "C4.h"
#include <stdlib.h>

using namespace std;

int main(void){
    clock_t start, finish;
    start = clock();
    const int THREAD_NUM = 6;

    C4State initial;

    MCTS::SearchTree tree(&initial, THREAD_NUM);

    // training session
    int epochs = 50000;
    float rollout_num = 500;

    for(int i=0 ; i<epochs ; i++){
        tree.selection();
        tree.expansion();
        float win_num = tree.rollout(rollout_num);
        tree.backpropagation(win_num, rollout_num);

        if((i+1)%1000==0){
            cout << "Training : " << (i+1) << " epochs completed." << endl;
        }
    }

    // testing session
    tree.selection_test();

    finish = clock();
    double duration = (finish - start) / CLOCKS_PER_SEC;
    cout << duration << "s" << endl;

    return 0;
}