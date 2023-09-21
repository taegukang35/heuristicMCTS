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
#include "bush.h"


using namespace std;
struct ReturnType{
    int action;
    ::State* prev_state;
};

extern "C"{
    struct ReturnType get(::State* pstate, ::State* ppred_state, int action) {
        const int THREAD_NUM = 24;

        const int epochs = 1600;
        const int rollout_num = 48;
        ::State state = *pstate;
        ::State pred_state = *ppred_state;
	    int result = 0;
        LoqState now_state = LoqState(state, state.getTurn());
        LoqState p_state = LoqState(pred_state, pred_state.getTurn());
        
        MCTS::Action* prev_action = nullptr;
        
        LoqState predicted_state = LoqState(pred_state, pred_state.getTurn());

        // 선공일 때 가장 첫수는 nullptr
        if (action != -1){
            int w = action % 10;
            int y = (action % 100) / 10;
            int x = (action % 1000) / 100;
            int type = action / 1000;
            prev_action = new LoqAction(type, x, y, w);
        };
        // 만약 선공이라서 처음 state가 어떻게 받는가?
        now_state = makePredictedState(now_state, predicted_state, prev_action);
	
        MCTS::Action* policy = now_state.policy();
	if (action == -1){
	  policy = now_state.get_best_move_action();
	  LoqState* test_state = (LoqState*) now_state.step(policy, now_state.get_turn());
	  if (test_state->get_internal_state().p.x() != 0){
	  }
	  else
	    policy = NULL;
	}

	if(policy == NULL)
        {
	    
            MCTS::MoveInfo info = get_MoveInfo(now_state, epochs, rollout_num, THREAD_NUM);
            if(info.win_rate < 0.1f) {
                policy = now_state.get_best_move_action();
            }
            else
                policy = info.action;
        }
        
        bool turn = now_state.get_turn();
        Point k = now_state.get_internal_state().position(!turn);
        Point p = now_state.get_internal_state().position(turn);
        
        for(int i = 1; i <=4; ++i){
            result *= 10;
            result += (*(int*)(policy->get_info(i)));
        }
        
        // now state도 같이 반환해서 history에 기록해주고 싶은데
        LoqState* temp = (LoqState*)now_state.step(policy, now_state.get_turn());
        ::State* temp2 = (temp->get_internal_state().copy());
        struct ReturnType data;
        data.action = result;
        data.prev_state = temp2;
        return data;
    }
}
