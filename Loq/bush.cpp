#include <iostream>
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
#include <string>
#include <algorithm>

using namespace std;

// 상대가 부쉬 안에 있고 내가 부쉬 밖에 있을 때 상대방의 액션 타입을 얻음
// 만약 상대가 부쉬밖에 블럭을 놓거나 부쉬 밖으로 이동하면 -를 붙여서 반환
int getActionType(LoqState masked_state, LoqState previous){
    int prev_remained_i = 0;
    int prev_remained_l = 0;
    int current_remained_i = 0;
    int current_remained_l = 0;
    int type;

    if (!masked_state.get_turn()){
        IBlock* previblock = previous.get_internal_state().getPIBlock();
        LBlock* prevlblock = previous.get_internal_state().getPLBlock();

        for (int i=0; i<7; ++i){
            if (!previblock[i]) prev_remained_i += 1;
        }
        for (int i=0; i<3; ++i){
            if (!prevlblock[i]) prev_remained_l += 1;
        }

        IBlock* currentiblock = masked_state.get_internal_state().getPIBlock();
        LBlock* currentlblock = masked_state.get_internal_state().getPLBlock();
        for (int i=0; i<7; ++i){
            if (!currentiblock[i]) current_remained_i += 1;
        }
        for (int i=0; i<3; ++i){
            if (!currentlblock[i]) current_remained_l += 1;
        }

        if (prev_remained_i != current_remained_i) type = 2;
        else if (prev_remained_l != current_remained_l) type = 3;
        else type = 1;

        if (type == 2){
            int idx = -1; // 제일 마지막에 둔 블럭 idx
            for (int i=0; i<7; ++i){
                if ((int)currentiblock[6-i].dir != 0){
                    idx = 6-i;
                    break;
                }
            }
            if ((int)currentiblock[idx].dir != 5) return -2;
            return 2;
        }
        else if (type == 3){
            int idx = -1; // 제일 마지막에 둔 블럭 idx
            for (int i=0; i<3; ++i){
                if ((int)currentlblock[2-i].dir != 0){
                    idx = 2-i;
                    break;
                }
            }
            if ((int)currentlblock[idx].dir != 5) return -3;
            return 3;
        }
        else{
            if ((int)masked_state.get_internal_state().position(false).x() != 0) return -1;
            return 1;
        }
    }
    else {
        IBlock* previblock = previous.get_internal_state().getKIBlock();
        LBlock* prevlblock = previous.get_internal_state().getKLBlock();

        for (int i=0; i<7; ++i){
            if (!previblock[i]) prev_remained_i += 1;
        }
        for (int i=0; i<3; ++i){
            if (!prevlblock[i]) prev_remained_l += 1;
        }

        IBlock* currentiblock = masked_state.get_internal_state().getKIBlock();
        LBlock* currentlblock = masked_state.get_internal_state().getKLBlock();
        for (int i=0; i<7; ++i){
            if (!currentiblock[i]) current_remained_i += 1;
        }
        for (int i=0; i<3; ++i){
            if (!currentlblock[i]) current_remained_l += 1;
        }

        if (prev_remained_i != current_remained_i) type = 2;
        else if (prev_remained_l != current_remained_l) type = 3;
        else type = 1;

        if (type == 2){
            int idx = -1; // 제일 마지막에 둔 블럭 idx
            for (int i=0; i<7; ++i){
                if ((int)currentiblock[6-i].dir != 0){
                    idx = 6-i;
                    break;
                }
            }
            if ((int)currentiblock[idx].dir != 5) return -2;
            return 2;
        }
        else if (type == 3){
            int idx = -1; // 제일 마지막에 둔 블럭 idx
            for (int i=0; i<3; ++i){
                if ((int)currentlblock[6-i].dir != 0){
                    idx = 6-i;
                    break;
                }
            }
            if ((int)currentlblock[idx].dir != 5) return -3;
            return 3;
        }
        else{
            if ((int)masked_state.get_internal_state().position(false).x() != 0) return -1;
            return 1;
        }
    }
}

bool compare(const pair<MCTS::Action*, int> a, const pair<MCTS::Action*, int> b){
    return a.second < b.second;
}

// 만약 우리가 부쉬 잘못 예측해서 상대가 블럭을 놓았을 때 터지면 그 원인이 되는 블럭 가림
LoqState* shadedState(LoqState masked_state, LoqState previous, MCTS::Action* action){
    LoqState* st_ptr = nullptr;
    LoqState loqstate;
    int type = *(int*) action->get_info(1);
    int x = *(int*) action->get_info(2);
    int y = *(int*) action->get_info(3);
    int dir = *(int*) action->get_info(4);

    LBlock *block_l;
    IBlock *block_i;
    bool is_i_block_crossed = false;
    bool turn = masked_state.get_turn();
    int idx = -1;

    if (!turn){
        block_i = previous.get_internal_state().getPIBlock();
        block_l = previous.get_internal_state().getPLBlock();
    } else{
        block_i = previous.get_internal_state().getKIBlock();
        block_l = previous.get_internal_state().getKLBlock();
    }

    // 상대 I자 블럭 놓을 때 터지는 경우
    if (type == 2){
        State ns = previous.get_internal_state();

        if (dir == 1){
            // 겹치는 I 블럭 고려
            for (int i=0; i < 7; ++i){
                int block_dir = block_i[i].dir;
                int block_x = block_i[i].p.x();
                int block_y = block_i[i].p.y();

                if (block_dir == 1 && block_x == x && (block_y == y || block_y == y-1 || block_y == y+1)) {
                    is_i_block_crossed = true;
                    idx = i;
                }
                if (block_dir == 2 && block_x == x - 1 && block_y == y + 1) {
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            // 겹치는 L자 블럭 고려
            for (int i=0; i < 3; ++i){
                int block_dir = block_l[i].dir;
                int block_x = block_l[i].p.x();
                int block_y = block_l[i].p.y();
                if (x == 5 && y == 6){
                    if (block_dir == 1 && block_x == 5 && block_y == 6){
                        is_i_block_crossed = false;
                        idx = i;
                    }
                    if (block_dir == 4 && block_x == 4 && block_y == 6){
                        is_i_block_crossed = false;
                        idx = i;
                    }
                }
                if (x == 6 && y == 6){
                    if (block_dir == 1 && block_x == 6 && block_y == 6){
                        is_i_block_crossed = false;
                        idx = i;
                    }
                    if (block_dir == 4 && block_x == 5 && block_y == 6){
                        is_i_block_crossed = false;
                        idx = i;
                    }
                }
                if (x == 5 && y == 4){
                    if (block_dir == 3 && block_x == 4 && block_y == 4){
                        is_i_block_crossed = false;
                        idx = i;
                    }
                    if (block_dir == 2 && block_x == 5 && block_y == 4){
                        is_i_block_crossed = false;
                        idx = i;
                    }
                }
                if (x == 6 && y == 4){
                    if (block_dir == 2 && block_x == 6 && block_y == 4){
                        is_i_block_crossed = false;
                        idx = i;
                    }
                    if (block_dir == 3 && block_x == 5 && block_y == 4){
                        is_i_block_crossed = false;
                        idx = i;
                    }
                }
            }
        }
        else {
            // 겹치는 I자 고려
            for (int i = 0; i < 7; ++i) {
                int block_dir = block_i[i].dir;
                int block_x = block_i[i].p.x();
                int block_y = block_i[i].p.y();

                if (block_dir == 2 && block_y == y && (block_x == x || block_x == x - 1 || block_y == x + 1)) {
                    is_i_block_crossed = true;
                    idx = i;
                }
                if (block_dir == 1 && block_x == x + 1 && block_y == y - 1) {
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            // 겹치는 L자 고려
            for (int i = 0; i < 3; ++i) {
                int block_dir = block_l[i].dir;
                int block_x = block_l[i].p.x();
                int block_y = block_l[i].p.y();
                if (x == 3 && y == 6) {
                    if (block_dir == 4 && block_x == 4 && block_y == 6) {
                        is_i_block_crossed = false;
                        idx = i;
                    }
                    if (block_dir == 3 && block_x == 4 && block_y == 5) {
                        is_i_block_crossed = false;
                        idx = i;
                    }
                }
                if (x == 3 && y == 5) {
                    if (block_dir == 4 && block_x == 4 && block_y == 5) {
                        is_i_block_crossed = false;
                        idx = i;
                    }
                    if (block_dir == 3 && block_x == 4 && block_y == 4) {
                        is_i_block_crossed = false;
                        idx = i;
                    }
                }
                if (x == 6 && y == 6) {
                    if (block_dir == 1 && block_x == 6 && block_y == 6) {
                        is_i_block_crossed = false;
                        idx = i;
                    }
                    if (block_dir == 2 && block_x == 6 && block_y == 5) {
                        is_i_block_crossed = false;
                        idx = i;
                    }
                }
                if (x == 6 && y == 5) {
                    if (block_dir == 2 && block_x == 6 && block_y == 4) {
                        is_i_block_crossed = false;
                        idx = i;
                    }
                    if (block_dir == 1 && block_x == 6 && block_y == 5) {
                        is_i_block_crossed = false;
                        idx = i;
                    }
                }
            }
        }
    }

        // 상대 L 자 블럭을 놓을 때 터지는 경우
        // Todo: L자 블럭 놓을 때 겹치는 경우 고려해야 함 => 케이스 너무 많음
    else {
        // 겹치는 I자 고려
        for (int i = 0; i < 7; ++i) {
            int block_dir = block_i[i].dir;
            int block_x = block_i[i].p.x();
            int block_y = block_i[i].p.y();

            if (dir == 3 && x == 4 && y == 6) {
                if (block_dir == 1 && block_x == 5 && block_y == 5){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 1 && x == 4 && y == 6) {
                if (block_dir == 2 && block_x == 4 && block_y == 6){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 2 && x == 5 && y == 6) {
                if (block_dir == 1 && block_x == 5 && block_y == 5){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 3 && x == 5 && y == 6) {
                if (block_dir == 1 && block_x == 6 && block_y == 5){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 2 && x == 6 && y == 6) {
                if (block_dir == 1 && block_x == 6 && block_y == 5){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 4 && x == 6 && y == 6) {
                if (block_dir == 2 && block_x == 5 && block_y == 6){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 2 && x == 4 && y == 5) {
                if (block_dir == 2 && block_x == 4 && block_y == 6){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 1 && x == 4 && y == 5) {
                if (block_dir == 2 && block_x == 4 && block_y == 5){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 3 && x == 6 && y == 5) {
                if (block_dir == 2 && block_x == 5 && block_y == 6){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 4 && x == 6 && y == 5) {
                if (block_dir == 2 && block_x == 5 && block_y == 5){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 2 && x == 4 && y == 4) {
                if (block_dir == 2 && block_x == 4 && block_y == 5){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 4 && x == 4 && y == 4) {
                if (block_dir == 1 && block_x == 5 && block_y == 4){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 1 && x == 5 && y == 4) {
                if (block_dir == 1 && block_x == 5 && block_y == 4){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 4 && x == 5 && y == 4) {
                if (block_dir == 1 && block_x == 6 && block_y == 4){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 3 && x == 6 && y == 4) {
                if (block_dir == 2 && block_x == 5 && block_y == 5){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
            if (dir == 1 && x == 6 && y == 4) {
                if (block_dir == 1 && block_x == 6 && block_y == 4){
                    is_i_block_crossed = true;
                    idx = i;
                }
            }
        }

        // 겹치는 L자 블럭 고려
        for (int i = 0; i < 3; ++i) {
            int block_dir = block_l[i].dir;
            int block_x = block_l[i].p.x();
            int block_y = block_l[i].p.y();

            if (dir == 3 && x == 4 && y == 6) {
                if (block_dir == 1 && block_x == 5 && block_y == 6) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 4 && block_x == 4 && block_y == 6) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 1 && x == 4 && y == 6) {
                if (block_dir == 4 && block_x == 6 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 3 && block_x == 4 && block_y == 5) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 2 && x == 5 && y == 6) {
                if (block_dir == 1 && block_x == 5 && block_y == 6) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 4 && block_x == 6 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 3 && x == 5 && y == 6) {
                if (block_dir == 4 && block_x == 5 && block_y == 6) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 1 && block_x == 6 && block_y == 6) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 2 && x == 6 && y == 6) {
                if (block_dir == 4 && block_x == 5 && block_y == 6) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 1 && block_x == 6 && block_y == 6) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 4 && x == 6 && y == 6) {
                if (block_dir == 1 && block_x == 6 && block_y == 6) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 2 && block_x == 6 && block_y == 5) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 2 && x == 4 && y == 5) {
                if (block_dir == 3 && block_x == 4 && block_y == 5) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 4 && block_x == 4 && block_y == 6) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 1 && x == 4 && y == 5) {
                if (block_dir == 4 && block_x == 4 && block_y == 5) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 3 && block_x == 4 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 3 && x == 6 && y == 5) {
                if (block_dir == 2 && block_x == 6 && block_y == 5) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 1 && block_x == 6 && block_y == 6) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 4 && x == 6 && y == 5) {
                if (block_dir == 3 && block_x == 6 && block_y == 5) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 2 && block_x == 6 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 2 && x == 4 && y == 4) {
                if (block_dir == 3 && block_x == 4 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 4 && block_x == 4 && block_y == 5) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 4 && x == 4 && y == 4) {
                if (block_dir == 3 && block_x == 4 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 2 && block_x == 5 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 1 && x == 5 && y == 4) {
                if (block_dir == 2 && block_x == 5 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 3 && block_x == 4 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 4 && x == 5 && y == 4) {
                if (block_dir == 3 && block_x == 5 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 2 && block_x == 6 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 3 && x == 6 && y == 4) {
                if (block_dir == 2 && block_x == 6 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 1 && block_x == 6 && block_y == 5) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
            if (dir == 1 && x == 6 && y == 4) {
                if (block_dir == 2 && block_x == 6 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
                if (block_dir == 3 && block_x == 5 && block_y == 4) {
                    is_i_block_crossed = false;
                    idx = i;
                }
            }
        }
    }

    // 예측한 블럭 중 겹치는 블럭은 블러 처리함 dir = 10으로 둠
    if (!turn){
        if (is_i_block_crossed){
            ::State state = block_i[idx].apply(previous.get_internal_state(), false);
            state.block_ip[idx] = {{}, 10}; // dir = 10으로 처리
            st_ptr = new LoqState(state, state.getTurn());
        }
        else{
            ::State state = block_l[idx].apply(previous.get_internal_state(), false);
            state.block_lp[idx] = {{}, 10}; // dir = 10으로 처리
            st_ptr = new LoqState(state, state.getTurn());
        }
    }
    else {
        if (is_i_block_crossed){
            ::State state = block_i[idx].apply(previous.get_internal_state(), false);
            state.block_ik[idx] = {{}, 10}; // dir = 10으로 처리
            st_ptr = new LoqState(state, state.getTurn());
        }
        else{
            ::State state = block_l[idx].apply(previous.get_internal_state(), false);
            state.block_lk[idx] = {{}, 10}; // dir = 10으로 처리
            st_ptr = new LoqState(state, state.getTurn());
        }
    }

    // 블러 처리 한 후 원래 액션 적용
    st_ptr = (LoqState*) st_ptr->step(action, previous.get_turn());
    return st_ptr;
}


// 현재 masked_state를 서버에서 입력받았다 가정.
// previous는 이전에 상대가 블럭을 두기 전 우리가 예측했던(혹은 정확한) 상태임
LoqState* predictState(LoqState masked_state, LoqState previous) {
    LoqState* st_ptr;
    MCTS::Action* action;
    bool turn = previous.get_turn(); // 예측할 플레이어
    int type = getActionType(masked_state, previous);
    static thread_local std::mt19937 generator;

    cout << "action type: " << type << endl;

    if (type == -1){ // 상대가 움직였는데 부쉬 밖에 있을 때
        Point now_pos = masked_state.get_internal_state().position(false);

        // 부쉬 밖에 있다가 부쉬 밖으로 이동시
        if (!previous.get_internal_state().internal(true)){
            action = new LoqAction(1, now_pos.x(), now_pos.y(), 1);
            st_ptr = (LoqState*) previous.step(action, previous.get_turn());
            return st_ptr;
        }

            // 부쉬 안에서 부쉬 밖으로 이동시, 상대방 위치 직접 수정?
        else{
            ::State now_state = previous.get_internal_state();
            if (turn) now_state.p = now_pos;
            else now_state.k = now_pos;

            int y = previous.get_internal_state().position().y();
            now_state = now_state.next();
            st_ptr = new LoqState(now_state, now_state.getTurn());
            return st_ptr;
        }
    }

        // I자 블럭을 상대가 보이게 놓았을 때
    else if (type == -2){
        int idx = -1; // 제일 마지막에 둔 블럭 idx
        IBlock* iblock = turn?
                         masked_state.get_internal_state().getPIBlock(): masked_state.get_internal_state().getKIBlock();
        for (int i = 0; i < 7; ++i) {
            if ((int)iblock[6 - i].dir != 0) {
                idx = 6 - i;
                break;
            }
        }
        int px, py, dir;
        px = iblock[idx].p.x();
        py = iblock[idx].p.y();
        dir = iblock[idx].dir;
        action = new LoqAction(2, px, py, dir);
        st_ptr = (LoqState*) previous.step(action, previous.get_turn());

        if (st_ptr->get_internal_state().position().x() == 0){
            st_ptr = shadedState(masked_state, previous, action); // action도 적용하고, 잘 블럭들을 재조합
        }
        return st_ptr;
    }

        // L자 블럭을 상대가 보이게 놓았을 때
    else if (type == -3){
        int idx = -1; // 제일 마지막에 둔 블럭 idx
        LBlock* lblock = turn?
                         masked_state.get_internal_state().getPLBlock(): masked_state.get_internal_state().getKLBlock();
        for (int i = 0; i < 3; ++i) {
            if (lblock[2 - i].dir != 0) {
                idx = 2 - i;
                break;
            }
        }
        int px, py, dir;
        px = lblock[idx].p.x();
        py = lblock[idx].p.y();
        dir = lblock[idx].dir;
        action = new LoqAction(3, px, py, dir);
        st_ptr = (LoqState*) previous.step(action, previous.get_turn());
        if (st_ptr->get_internal_state().position().x() == 0){
            st_ptr = shadedState(masked_state, previous, action); // action도 적용하고, 잘 블럭들을 재조합
        }
        return st_ptr;
    }

        // 상대 부쉬 밖에서 안으로 들어오거나 부쉬 내부에서 움직이는 경우
    else if (type == 1){
        // 부쉬 밖에서 안으로 들어올 때
        if (masked_state.get_internal_state().position(false).x() == 0 && !previous.get_internal_state().internal(true)){
            Point pos = previous.get_internal_state().position(true);
            int x = pos.x(); int y = pos.y();
            int dx[] = {-1, 0, 1, 0};
            int dy[] = {0, -1, 0, 1};
            int nx, ny;
            for (int i=0; i < 4; ++i){
                nx = x + dx[i];
                ny = y + dy[i];
                if (4 <= nx && nx <= 6 && 4 <= ny && ny<= 6) break;
            }
            action = new LoqAction(1, nx, ny, 1);
            st_ptr = (LoqState*) previous.step(action, previous.get_turn());
            return st_ptr;
        }

            // 부쉬 안에서 안으로 움직일 때 최단경로로 이동 생각
        else{
            vector<MCTS::Action*> actions = previous.get_all_best_move_actions();
            vector<MCTS::Action*> valid_actions;
            for (auto it: actions){
                int x = *(int*) it->get_info(2);
                int y = *(int*) it->get_info(3);
                if ((4 <= x && x <= 6 && 4 <= y && y <= 6)){
                    valid_actions.emplace_back(it);
                }
            }
            // 만약 최단 경로로 움직일 때 밖으로 나갈 수 밖에 없다면 상태 업데이트 안함 => 이게 적절한지는 모르겠다
            if (valid_actions.empty()){
                ::State now_state = previous.get_internal_state();
                now_state = now_state.next();
                st_ptr = new LoqState(now_state, now_state.getTurn());
                return st_ptr;
            }
            else{
                std::uniform_int_distribution<int> distribution(0, valid_actions.size() -1);
                action = valid_actions[distribution(generator)];
                st_ptr = (LoqState*) previous.step(action, masked_state.get_turn());
                return st_ptr;
            }
        }
    }
        // 블럭을 놓을 땐 상대를 방해하면서 내가 방해받지 않는 블럭을 우선적으로 고름
    else{
        int op_dis = previous.bfs(previous.get_internal_state().position(true),previous.get_turn());
        int my_dis = previous.bfs(previous.get_internal_state().position(false), masked_state.get_turn());
        vector<MCTS::Action*> candidates;
        if (type == 2) {
            candidates = {new LoqAction(2, 5, 5, 1),
                          new LoqAction(2, 6, 5, 1),
                          new LoqAction(2, 5, 4, 1),
                          new LoqAction(2, 6, 4, 1),
                          new LoqAction(2, 4, 5, 2),
                          new LoqAction(2, 5, 5, 2),
                          new LoqAction(2, 4, 6, 2),
                          new LoqAction(2, 5, 6, 2)
            };
        }
        else{
            candidates = {new LoqAction(3, 4, 4, 3),
                          new LoqAction(3, 5, 4, 2),
                          new LoqAction(3, 5, 4, 3),
                          new LoqAction(3, 6, 4, 2),
                          new LoqAction(3, 4, 5, 3),
                          new LoqAction(3, 4, 5, 4),
                          new LoqAction(3, 5, 5, 1),
                          new LoqAction(3, 5, 5, 2),
                          new LoqAction(3, 5, 5, 3),
                          new LoqAction(3, 5, 5, 4),
                          new LoqAction(3, 6, 5, 3),
                          new LoqAction(3, 6, 5, 4),
                          new LoqAction(3, 4, 6, 4),
                          new LoqAction(3, 5, 6, 1),
                          new LoqAction(3, 5, 6, 4),
                          new LoqAction(3, 6, 6, 1)
            };
        }

        vector<pair<MCTS::Action*, int>> actions;
        for (auto action : candidates){
            st_ptr = (LoqState*)previous.step(action, previous.get_turn());
            LoqState new_state = *st_ptr;
            int n_op_dis = new_state.bfs(new_state.get_internal_state().position(false), previous.get_turn());
            int n_my_dis = new_state.bfs(new_state.get_internal_state().position(true), masked_state.get_turn());
            int val = (n_op_dis - op_dis) - (n_my_dis - my_dis); // val 작을수록 좋음
            if (st_ptr->get_internal_state().position().x() != 0)
                actions.emplace_back(make_pair(action, val));
        }

        // Todo: 왠지 actions가 비어있을 때 에러가 발생할 수 있지 않을까?
        sort(actions.begin(), actions.end(), compare);
        action = actions[0].first;
        st_ptr = (LoqState*) previous.step(action, masked_state.get_turn());
        return st_ptr;
    }
}

// 내가 무효수를 놓았을 때 predicted_state에서 이전에 둔 수를 무름
LoqState* correctState(LoqState masked_state, LoqState previous, MCTS::Action* prev_action){
    int type = *(int*)prev_action->get_info(1);
    bool turn = masked_state.get_turn();
    LoqState* st_ptr = nullptr;

    if (type == 2){
        int idx;
        IBlock* nowblock = turn? masked_state.get_internal_state().getPIBlock() : masked_state.get_internal_state().getKIBlock();
        IBlock* prevblock = turn? previous.get_internal_state().getPIBlock(): previous.get_internal_state().getKIBlock();

        for (int i = 0; i < 7; ++i) {
            if (prevblock[6 - i].dir != 0) {
                idx = 6 - i;
                break;
            }
        }
        if (nowblock[idx].dir == 0){
            ::State state = prevblock[idx].apply(previous.get_internal_state(), false);
            if (turn) state.block_ip[idx] = {{}, 0};
            else state.block_ik[idx] = {{},  0};
            st_ptr = new LoqState(state, state.getTurn());
        }
        else{
            st_ptr = (LoqState*) previous.copy();
        }
    }
    else if (type == 3){
        int idx;
        LBlock* nowblock = turn? masked_state.get_internal_state().getPLBlock() : masked_state.get_internal_state().getKLBlock();
        LBlock* prevblock = turn? previous.get_internal_state().getPLBlock(): previous.get_internal_state().getKLBlock();
        for (int i = 0; i < 3; ++i) {
            if (prevblock[2 - i].dir != 0) {
                idx = 2 - i;
                break;
            }
        }
        if (nowblock[idx].dir == 0){
            ::State state = prevblock[idx].apply(previous.get_internal_state(), false);
            if (turn) state.block_lp[idx] = {{}, 0};
            else state.block_lk[idx] = {{},  0};
            st_ptr = new LoqState(state, state.getTurn());
        }
        else{
            st_ptr = (LoqState*) previous.copy();
        }
    }
    else{
        st_ptr = (LoqState*) previous.copy();
    }
    return st_ptr;
}

int main(void) {
    ::State nstate = State({5, 1}, {5, 9}, false); // 포스텍 선공, 부쉬 들어오기 직전
    LoqState real_state = LoqState(nstate, nstate.getTurn()); // 실제 state 임
    MCTS::Action* action = nullptr;
    LoqState* st_ptr = nullptr;

    LoqState predicted_state = real_state; // 카이스트가 예측한 state
    MCTS::Action* prev_action = nullptr;
    ::State mask;
    LoqState masked_state;
    int type, x, y, w;
    cout << real_state.to_string() << endl;
    // 입력 받은 대로 움직임. 카이스트에만 mask 적용
    while (!real_state.is_done()){
        if (real_state.get_turn()){
            cout << "<POSTECH TURN>" << endl;
            cout << "insert type, x, y, w: ";
            cin >> type >> x >> y >> w;
            action = new LoqAction(type, x, y, w);
            st_ptr = (LoqState*) real_state.step(action, real_state.get_turn());
            real_state = *st_ptr;
            cout << "Real state: \n" << real_state.to_string() << endl;
        }
        else{
            cout << "<KAIST TURN>" << endl;
            mask = real_state.get_internal_state().mask();
            masked_state = LoqState(mask, real_state.get_turn());

            cout << "Masked state: \n" << masked_state.to_string() << endl;
            // 부쉬 안에 있으면 masked_state 적용 => 어짜피 다 보임
            if (masked_state.get_internal_state().internal(true)){
                predicted_state = masked_state;
            }
                // 부쉬 안에 없으면 마스킹 후 예측해서 predicted_state 적용
            else{
                mask = real_state.get_internal_state().mask();
                masked_state = LoqState(mask, real_state.get_turn());

                // 만약 내가 블럭을 두었을 때 상대를 가둬서 무효수였다면 예상한 state에 마지막에 내가 둔 수를 없앰
                if (prev_action != nullptr){
                    st_ptr = correctState(masked_state, predicted_state, prev_action);
                    predicted_state = *st_ptr; // turn은 상대턴임
                    predicted_state = *predictState(masked_state, predicted_state);
                }
            }
            cout << "Predicted state: \n" << predicted_state.to_string() << endl;
            cout << "Insert (type x y w): ";
            cin >> type >> x >> y >> w;
            action = new LoqAction(type, x, y, w);
            prev_action = action->copy();

            st_ptr = (LoqState*) real_state.step(action, real_state.get_turn());
            real_state = *st_ptr;
            st_ptr = (LoqState*) predicted_state.step(action, masked_state.get_turn());
            predicted_state = *st_ptr;

            cout << real_state.to_string() << endl;
        }
    }
    return 0;
}