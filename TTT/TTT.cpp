#include <iostream>
#include "../MCTS/MCTS.h"
#include "TTT.h"
#include <stdlib.h>

using namespace std;

int main(void){
    /*
    int board[3][3] = {{-1,1,0},{0,1,-1},{0,0,0}};
    TTTState s(board, true);
    
    Action* a = new TTTAction(2,1);
    State* next = s.step(a, true);
    cout << next->to_string() << endl;
    cout << next->is_done() << endl;
    */

    const int THREAD_NUM = 3;

    TTTState initial;

    MCTS::SearchTree tree(&initial, THREAD_NUM);

    // training session
    int epochs = 20000;
    float rollout_num = 100;

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
    
    system("PAUSE");
    return 0;
}