#include <iostream>
#include<ctime>
#include "./loq2/src/loq2/native/library.h"
#include "./loq2/src/loq2/native/state/util.cpp"
#include "./loq2/src/loq2/native/map/map.cpp"
#include "./loq2/src/loq2/native/point/point.cpp"
#include "./loq2/src/loq2/native/state/place.cpp"
#include "./loq2/src/loq2/native/state/endable.cpp"
#include "./loq2/src/loq2/native/state/move.cpp"
#include "./loq2/src/loq2/native/state/cross.cpp"
#include "./loq2/src/loq2/native/block/lblock.cpp"
#include "./loq2/src/loq2/native/block/iblock.cpp"
#include "./loq2/src/loq2/native/state/mask.cpp"

#include "../MCTS/MCTS.h"
#include "Loq.h"
#include "save.h"
#include <stdlib.h>


using namespace std;

int main(void){
    const int THREAD_NUM = 32;

    const int epochs = 300;
    const int rollout_num = 32;
    vector<MCTS::MoveInfo> infos;
    LoqState now_state;
    LoqState* st_ptr = nullptr;
    int i = 1;
    double sum = 0;
    int cnt = 0;
    double maxTime = -1;
        LoqAction* policy = new LoqAction(3,5,1,3);
        st_ptr = (LoqState*)now_state.step(policy, now_state.get_turn());
        now_state = *st_ptr;
        cout << now_state.to_string();
        policy = new LoqAction(2, 5, 1, 1);
        st_ptr = (LoqState*)now_state.step(policy, now_state.get_turn());
        now_state = *st_ptr;
        
        cout << now_state.to_string();
        for(int i = 0; i <10; ++i)
        cout << now_state.get_internal_state().position(0).x();
        cout << "print";
        for(int i = 0; i <10; ++i)
        cout << "|1"<<(int)(now_state.get_internal_state().position().x() )<< "1|";
cout << now_state.to_string();
        
    saveMoveInfo(infos);

    for(auto info : infos)
        free_MoveInfo(info);

    return 0;
}

void train(){
    clock_t start, finish;
    start = clock();
    const int THREAD_NUM = 24;

    LoqState initial;

    MCTS::SearchTree tree(&initial, THREAD_NUM);

    // training session
    int epochs = 100;
    float rollout_num = 96;

    for(int i=0 ; i<epochs ; i++){
        tree.selection();
        tree.expansion();
        float win_num = tree.rollout(rollout_num);
        tree.backpropagation(win_num, rollout_num);

        if((i+1)%10==0){
            cout << "Training : " << (i+1) << " epochs completed." << endl;
        }
    }

    // testing session
    tree.selection_test();

    finish = clock();
    double duration = (finish - start) / CLOCKS_PER_SEC / THREAD_NUM;
    cout << duration << "s" << endl;

    cout << tree.get_node_num() << endl;
}

/*
LoqState initial;

    LoqState next = *(LoqState*)get_MoveInfo(initial, 100, 12, 12).state;

    cout << next.to_string() << endl;
*/