#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include <queue>
#include "../MCTS/base.h"
#include "./loq2/src/loq2/native/type.h"
#include "./loq2/src/loq2/native/state/state.h"
#include <tuple>
#include <time.h>

using namespace std;

class LoqAction : public MCTS::Action{
    public:
        int action;
        int x;
        int y;
        int w;

        LoqAction(int action, int x, int y, int w){
            this->action = action;
            this->x = x;
            this->y = y;
            this->w = w;
        }

        virtual ~LoqAction() = default;

        virtual Action* copy(){
            return new LoqAction(*this);
        }

        virtual void* get_info(int index){ // State can only get info using this method
            if(index==1)
                return &this->action;
            else if (index==2)
                return &this->x;
            else if (index==3)
                return &this->y;
            else if (index==4)
                return &this->w;

            return NULL;
        }

        virtual string to_string(){
            string s = "";
            switch(this->action){
                case 1:
                    s += "Move Agent";
                    break;
                case 2:
                    s += "Place I block";
                    break;
                case 3:
                    s += "Place L block";
                    break;
            }
            s += "\t x : " + std::to_string(this->x);
            s += "\t y : " + std::to_string(this->y);
            s += "\t w : " + std::to_string(this->w);

            return s;
        }
};


class OpeningState{
    bool turn;
    Point kaist;
    Point postech;
    LoqAction action;
    OpeningState(bool turn, Point kaist, Point postech, LoqAction action) : turn(turn), kaist(kaist), postech(postech), action(action){
        
    }
};
class LoqState : public MCTS::State{
    protected:
        ::State orgstate;
    
    public:
        LoqState(){ // this gives initial state
            orgstate = INIT_STATE;
            this->turn = orgstate.getTurn();
            srand(time(NULL));
        }

        LoqState(::State state, bool turn){
            orgstate = state;
            this->turn = turn;
            srand(time(NULL));
        }
        
        /*
        LoqState(const LoqState& copy){ // overriding copy constructor to deepcopy
            //std::copy(&copy.board[0][0], &copy.board[0][0]+BOARD_SIZE*BOARD_SIZE, &this->board[0][0]);
            //this->turn = copy.turn;
            //this->done = copy.done;
        }
        */

        virtual ~LoqState() = default;
	
        /*MCTS::Action* get_opening() {
            int pi = orgstate.getPIBlock();
            int pl = orgstate.getPLBlock();
            int ki = orgstate.getKIBlock();
            int kl = orgstate.getKLBlock();
            bool turn = orgstate.getTurn();
            Point k = orgstate.position(!turn);
            Point p = orgstate.position(turn);
            if(!(pi == I_COUNT && pl == L_COUNT && ki == I_COUNT && kl == L_COUNT))
                return NULL;
            OpeningState states[] = {
                OpeningState(false, Point(5, 1), Point(5,9), LoqAction(1,5,2,1)),
                
                OpeningState(false, Point(5, 2), Point(5,8), LoqAction(2,5,2,2)),
                
                OpeningState(false, Point(5, 2), Point(4,9), LoqAction(2,5,2,2)),
                OpeningState(false, Point(5, 2), Point(6,9), LoqAction(2,5,2,2))
            };
            for(OpeningState s : states) {
                if(turn == s.turn && k == s.kaist && p == s.postech) 
                {
                    if(orgstate.act(*(int*)s.action.get_info(0), *(int*)s.action.get_info(1), *(int*)s.action.get_info(2), *(int*)s.action.get_info(3)).position().x() != 0)
                        return s.action.copy();
                }
            }
	    return NULL;
        }*/
        virtual State* copy(){
            return new LoqState(*this);
        }

        ::State get_internal_state(){
            return orgstate;
        }
        
        virtual vector<MCTS::Action*> get_all_actions(){
            vector<MCTS::Action*> actions;
            ::State curstate = this->orgstate;
            int dx[] = {1,-1,0,0,-1,-1,1,1,2,-2,0,0};
            int dy[] = {0,0,1,-1,1,-1,1,-1,0,0,-2,2};
            int mx = curstate.position().x();
            int my = curstate.position().y();
            int ivhdx[6] = {-1, -1, -1, 0, 0, 0}; 
                    int ivhdy[6] = {2, 1, 0, 2, 1, 0};
                    int ivvdx[2] = {0, 0}; 
                    int ivvdy[2] = {-1,2};
                    int ihhdx[2] = {-1,2}; 
                    int ihhdy[2] = {0, 0};
                    int ihvdx[6] = {0, 1, 2, 0, 1, 2}; 
                    int ihvdy[6] = {-1, -1, -1, 0, 0, 0};


                    // Left most point -> counter clockwise
                    int l1hdx[4] = {-1, -1, 0, 1};
                    int l1hdy[4] = {0, 1, 1, 0};
                    int l1vdx[4] = {0, 0, 1, 1};
                    int l1vdy[4] = {-1, 1, 0, -1};

                    int l2hdx[4] = {-1, 0, -1, 1};
                    int l2hdy[4] = {0, 0, 1, 1};
                    int l2vdx[4] = {0, 0, 1, 1};
                    int l2vdy[4] = {-1, 1, 1, 0};

                    int l3hdx[4] = {-1, 1, 0, 1};
                    int l3hdy[4] = {1, 1, 0, 0};
                    int l3vdx[4] = {0, 0, 1, 1};
                    int l3vdy[4] = {0, 1, 1, -1};

                    int l4hdx[4] = {-1, -1, 0, 1};
                    int l4hdy[4] = {0, 1, 1, 0};
                    int l4vdx[4] = {0, 0, 1, 1};
                    int l4vdy[4] = {-1, 1, 0, -1};
            int vdx[] = {0, 0, 1, 1};
            int vdy[] = {0, -1, 0, -1};
            int hdx[] = {0,-1, 0, -1};
            int hdy[] = {0,0, 1, 1};
            int my_dis = bfs(curstate.position(), curstate.getTurn());
            int oppo_dis = bfs(curstate.position(false), !curstate.getTurn());
            Point op = orgstate.position(0);
            int ox = (int)op.x();
            int oy = (int)op.y();
            int standard = my_dis + oppo_dis;

            if(this->done){ // no more action if state is done
                return actions;
            }

            
            // MOVE - 맵 밖을 벗어나는 것도 허용임
            int pos[] = {curstate.position().x(), curstate.position().y()};
            for (int d = 0; d < 12; d++){
                if (curstate.act(1,pos[0]+dx[d],pos[1]+dy[d],1).position().x() != 0){
                    LoqAction* action = new LoqAction(1,pos[0]+dx[d],pos[1]+dy[d],1);
                    if(avail_bfs(Point(pos[0], pos[1]), Point(pos[0] + dx[d], pos[1] + dy[d]), curstate.getTurn()))
                        actions.push_back(action);
                    else
                        delete action;
                }
            }

            int compen = 0;
            for(int x = 1; x <= BOARD_SIZE; x++){
                for(int y = 1; y <= BOARD_SIZE; y++){
                    for(int w = 1; w <= 2; w++){
                        ::State next_state = curstate.act(2,x,y,w);
                        LoqState* next_mcts_state = new LoqState(next_state, next_state.getTurn());
                            
                        if (next_state.position().x() != 0){
                            LoqAction* action = new LoqAction(2,x,y,w);
                            if ((int)(curstate.act(2,x,y,w).position().x()) != 0){
                                compen=0;
                                for(int i = 0; i < 4; ++i) {
                                    int nx = ox + vdx[i];
                                    int ny = oy + vdy[i];
                                    if(nx == x && ny == y && w == 1)
                                        compen++;
                                }
                                for(int i = 0; i < 4; ++i) {
                                    int nx = ox + hdx[i];
                                    int ny = oy + hdy[i];
                                    if(nx == x && ny == y && w == 2)
                                        compen++;
                                }
                                for(int i = 0; i < 4; ++i) {
                                    int nx = mx + vdx[i];
                                    int ny = my + vdy[i];
                                    if(nx == x && ny == y && w==1)
                                        compen++;
                                }
                                for(int i = 0; i < 4; ++i) {
                                    int nx = mx + hdx[i];
                                    int ny = my + hdy[i];
                                    if(nx == x && ny == y && w==2)
                                        compen++;
                                }
                                if(w == 1){
                                    for(int k = 0;k < 6; ++k) {
                                        if(x + ivhdx[k] <= BOARD_SIZE && y + ivhdy[k] <= BOARD_SIZE &&x + ivhdx[k] >= 1 && y + ivhdy[k] >= 1  && orgstate.getMap().v_block(Point(x + ivhdx[k],y + ivhdy[k])))
                                            compen++;
                                    }
                                    for(int k = 0;k < 2; ++k) {
                                        if(x + ivvdx[k] <= BOARD_SIZE && y + ivvdy[k] <= BOARD_SIZE &&x + ivvdx[k] >= 1 && y + ivvdy[k] >= 1 &&orgstate.getMap().h_block(Point(x + ivvdx[k],y + ivvdy[k])))
                                            compen++;
                                    }   
                                }
                                else{
                                    for(int k = 0;k < 2; ++k) {
                                        if(x + ihhdx[k] >= 1 && y + ihhdy[k] >= 1 &&orgstate.getMap().v_block(Point(x + ihhdx[k],y + ihhdy[k])))
                                            compen++;
                                    }
                                    for(int k = 0;k < 6; ++k) {
                                        if(x + ihvdx[k] >= 1 && y + ihvdy[k] >= 1 &&orgstate.getMap().h_block(Point(x + ihvdx[k],y + ihvdy[k])))
                                            compen++;
                                    }
                                }
                            }
                            // huristic policy apply
                            int sop_dis = next_mcts_state->bfs(next_state.position(), next_state.getTurn());
                            int smy_dis =next_mcts_state->bfs(next_state.position(false), !next_state.getTurn());


                            if(compen == 0){
                                delete action;
                                action = nullptr;
                            }
                            // 상대 경로가 줄어들지 않으면 action 없앰
                            if (action != nullptr && (sop_dis - oppo_dis <= 0)){
                                delete action;
                                action = nullptr;
                            }
                            // 내 경로가 증가하는데 상대 경로 3보다 증가하지 않으면 action 없앰
                            if (action != nullptr && (smy_dis - my_dis > 2)){
                                if (sop_dis - oppo_dis <= 3){
                                    delete action;
                                    action = nullptr;
                                }
                            }
                            if (action != nullptr)
                                actions.push_back(action);
                        }
                        delete next_mcts_state;
                    }
                    for(int w = 1; w <= 4; w++){
                        ::State next_state = curstate.act(3,x,y,w);
                        LoqState* next_mcts_state = new LoqState(next_state, next_state.getTurn());
                        if ((int)(curstate.act(3,x,y,w).position().x()) != 0){
                            compen=0;
                            if(x == mx && y == my){
                                compen++;
                            }
                            if (w==1){
                                for(int k = 0;k < 4; ++k) {
                                    // hblock
                                    if(x + l1hdx[k] <= BOARD_SIZE && y + l1hdy[k] <= BOARD_SIZE &&x + l1hdx[k] >= 1 && y + l1hdy[k] >= 1  && orgstate.getMap().v_block(Point(x + l1hdx[k],y + l1hdy[k])))
                                        compen++;
                                }
                                for(int k = 0;k < 4; ++k) {
                                    if(x + l1vdx[k] <= BOARD_SIZE && y + l1vdy[k] <= BOARD_SIZE &&x + l1vdx[k] >= 1 && y + l1vdy[k] >= 1 &&orgstate.getMap().h_block(Point(x + l1vdx[k],y + l1vdy[k])))
                                        compen++;
                                }
                            }
                            else if (w==2){
                                for(int k = 0;k < 4; ++k) {
                                    // hblock
                                    if(x + l2hdx[k] <= BOARD_SIZE && y + l2hdy[k] <= BOARD_SIZE &&x + l2hdx[k] >= 1 && y + l2hdy[k] >= 1  && orgstate.getMap().v_block(Point(x + l2hdx[k],y + l2hdy[k])))
                                        compen++;
                                }
                                for(int k = 0;k < 4; ++k) {
                                    if(x + l2vdx[k] <= BOARD_SIZE && y + l2vdy[k] <= BOARD_SIZE &&x + l2vdx[k] >= 1 && y + l2vdy[k] >= 1 &&orgstate.getMap().h_block(Point(x + l2vdx[k],y + l2vdy[k])))
                                        compen++;
                                }                                 
                            }
                            else if (w==3){
                                for(int k = 0;k < 4; ++k) {
                                    // hblock
                                    if(x + l3hdx[k] <= BOARD_SIZE && y + l3hdy[k] <= BOARD_SIZE &&x + l3hdx[k] >= 1 && y + l3hdy[k] >= 1  && orgstate.getMap().v_block(Point(x + l3hdx[k],y + l3hdy[k])))
                                        compen++;
                                }
                                for(int k = 0;k < 4; ++k) {
                                    if(x + l3vdx[k] <= BOARD_SIZE && y + l3vdy[k] <= BOARD_SIZE &&x + l3vdx[k] >= 1 && y + l3vdy[k] >= 1 &&orgstate.getMap().h_block(Point(x + l3vdx[k],y + l3vdy[k])))
                                        compen++;
                                }   
                            }
                            else {
                                for(int k = 0;k < 4; ++k) {
                                    // hblock
                                    if(x + l4hdx[k] <= BOARD_SIZE && y + l4hdy[k] <= BOARD_SIZE &&x + l4hdx[k] >= 1 && y + l4hdy[k] >= 1  && orgstate.getMap().v_block(Point(x + l4hdx[k],y + l4hdy[k])))
                                        compen++;
                                }
                                for(int k = 0;k < 4; ++k) {
                                    if(x + l4vdx[k] <= BOARD_SIZE && y + l4vdy[k] <= BOARD_SIZE &&x + l4vdx[k] >= 1 && y + l4vdy[k] >= 1 &&orgstate.getMap().h_block(Point(x + l4vdx[k],y + l4vdy[k])))
                                        compen++;
                                }        
                            }            
                        }
                        if (next_state.position().x() != 0){
                            LoqAction* action = new LoqAction(3,x,y,w);
                            
                            // huristic policy apply 
                            int sop_dis = next_mcts_state->bfs(next_state.position(), next_state.getTurn());
                            int smy_dis =next_mcts_state->bfs(next_state.position(false), !next_state.getTurn());

                            if(compen == 0){
                                delete action;
                                action = nullptr;
                            }
                            // 상대 경로가 줄어들지 않으면 action 없앰
                            if (action != nullptr && (sop_dis - oppo_dis <= 0)){
                                delete action;
                                action = nullptr;
                            }
                            // 내 경로가 증가하는데 상대 경로 3보다 증가하지 않으면 action 없앰
                            if (action != nullptr && (smy_dis - my_dis > 2)){
                                if (sop_dis - oppo_dis <= 3){
                                    delete action;
                                    action = nullptr;
                                }
                            }
                            if (action != nullptr)
                                actions.push_back(action);
                        }
                        delete next_mcts_state;
                    }
                }
            }
            // 만약 action 비어있으면 최단경로로 이동해라
            if (actions.empty()) actions.push_back((LoqAction*)get_best_move_action());
            return actions;
        }

        vector<MCTS::Action*> get_blocking_action() {
            Point op = orgstate.position(0);
            int x = (int)op.x();
            int y = (int)op.y();
            Point p = orgstate.position(1);
            int mx = (int)p.x();
            int my = (int)p.y();
            bool turn = orgstate.getTurn();
            ::State curstate = this->orgstate;
            vector<MCTS::Action*> result;
            for(int w = 1; w <= 4; ++w){
                if((int)(orgstate.act(3,x,y,w).position().x()) != 0)
                    result.push_back(new LoqAction(3, x, y, w));
            }
            int vdx[] = {0, 0, 1, 1};
            int vdy[] = {0, -1, 0, -1};
            int hdx[] = {0,-1, 0, -1};
            int hdy[] = {0,0, 1, 1};
            for(int i = 0; i < 4; ++i) {
                int nx = x + vdx[i];
                int ny = y + vdy[i];
                if((int)(orgstate.act(2,nx,ny,1).position().x()) !=0)
                    result.push_back(new LoqAction(2,nx,ny,1));
            }
            for(int i = 0; i < 4; ++i) {
                int nx = x + hdx[i];
                int ny = y + hdy[i];
                if((int)(orgstate.act(2,nx,ny,2).position().x()) !=0)
                    result.push_back(new LoqAction(2,nx,ny,2));
            }
            for(int i = 0; i < 4; ++i) {
                int nx = mx + vdx[i];
                int ny = my + vdy[i];
                if((int)(orgstate.act(2,nx,ny,1).position().x()) !=0)
                    result.push_back(new LoqAction(2,nx,ny,1));
            }
            for(int i = 0; i < 4; ++i) {
                int nx = mx + hdx[i];
                int ny = my + hdy[i];
                if((int)(orgstate.act(2,nx,ny,2).position().x()) !=0)
                    result.push_back(new LoqAction(2,nx,ny,2));
            }
	    
            return result;
        }
        vector<MCTS::Action*> get_adj_walls() {
            vector<MCTS::Action*> actions;
            ::State curstate = orgstate;
            int ivhdx[6] = {-1, -1, -1, 0, 0, 0}; 
                    int ivhdy[6] = {2, 1, 0, 2, 1, 0};
                    int ivvdx[2] = {0, 0}; 
                    int ivvdy[2] = {-1,2};
                    int ihhdx[2] = {-1,2}; 
                    int ihhdy[2] = {0, 0};
                    int ihvdx[6] = {0, 1, 2, 0, 1, 2}; 
                    int ihvdy[6] = {-1, -1, -1, 0, 0, 0};


                    // Left most point -> counter clockwise
                    int l1hdx[4] = {-1, -1, 0, 1};
                    int l1hdy[4] = {0, 1, 1, 0};
                    int l1vdx[4] = {0, 0, 1, 1};
                    int l1vdy[4] = {-1, 1, 0, -1};

                    int l2hdx[4] = {-1, 0, -1, 1};
                    int l2hdy[4] = {0, 0, 1, 1};
                    int l2vdx[4] = {0, 0, 1, 1};
                    int l2vdy[4] = {-1, 1, 1, 0};

                    int l3hdx[4] = {-1, 1, 0, 1};
                    int l3hdy[4] = {1, 1, 0, 0};
                    int l3vdx[4] = {0, 0, 1, 1};
                    int l3vdy[4] = {0, 1, 1, -1};

                    int l4hdx[4] = {-1, -1, 0, 1};
                    int l4hdy[4] = {0, 1, 1, 0};
                    int l4vdx[4] = {0, 0, 1, 1};
                    int l4vdy[4] = {-1, 1, 0, -1};
                    int compen = 0;
                    for(int x = 1; x <= BOARD_SIZE; x++){ // 9
                        for(int y = 1; y <= BOARD_SIZE; y++){ // 9
                            for(int w = 1; w <= 2; w++){
                                if ((int)(curstate.act(2,x,y,w).position().x()) != 0){
                                    LoqAction* action = new LoqAction(2,x,y,w);
                                    
                                        compen = 1;
                                        if(w == 1){
                                            for(int k = 0;k < 6; ++k) {
                                                if(x + ivhdx[k] <= BOARD_SIZE && y + ivhdy[k] <= BOARD_SIZE &&x + ivhdx[k] >= 1 && y + ivhdy[k] >= 1  && orgstate.getMap().v_block(Point(x + ivhdx[k],y + ivhdy[k])))
                                                    compen+=1;
                                            }
                                            for(int k = 0;k < 2; ++k) {
                                                if(x + ivvdx[k] <= BOARD_SIZE && y + ivvdy[k] <= BOARD_SIZE &&x + ivvdx[k] >= 1 && y + ivvdy[k] >= 1 &&orgstate.getMap().h_block(Point(x + ivvdx[k],y + ivvdy[k])))
                                                    compen++;
                                            }
                                            

                                        }
                                        else{
                                            for(int k = 0;k < 2; ++k) {
					      if(x + ihhdx[k] <= BOARD_SIZE && x + ihhdx[k] >= 1 && y + ihhdy[k] <= BOARD_SIZE && y + ihhdy[k] >= 1 &&orgstate.getMap().v_block(Point(x + ihhdx[k],y + ihhdy[k])))
                                                    compen++;
                                            }
                                            for(int k = 0;k < 6; ++k) {
                                                if(x + ihvdx[k] <= BOARD_SIZE && x + ihvdx[k] >= 1 && y + ihvdy[k] <= BOARD_SIZE && y + ihvdy[k] >= 1 &&orgstate.getMap().h_block(Point(x + ihvdx[k],y + ihvdy[k])))
                                                    compen++;
                                            }
                                        }
                                        for(int k = 0; k < sqrt(compen); ++k){
					                        actions.push_back(action->copy());}
                                        //pair<int, int> q = make_pair(2, w);
                                        //tuple<pair<int, int>, int, vector<MCTS::Action*>> p = make_tuple(q, compen, actions);
                                        //all_actions_t.push_back(p);
					
                                    delete action;
                                }
                            }
                            for(int w = 1; w <= 4; w++){
                                if ((int)(curstate.act(3,x,y,w).position().x()) != 0){
                                    LoqAction* action = new LoqAction(3,x,y,w);
                                        compen=1;
                                        if (w==1){
                                            for(int k = 0;k < 4; ++k) {
                                                // hblock
                                                if(x + l1hdx[k] <= BOARD_SIZE && y + l1hdy[k] <= BOARD_SIZE &&x + l1hdx[k] >= 1 && y + l1hdy[k] >= 1  && orgstate.getMap().v_block(Point(x + l1hdx[k],y + l1hdy[k])))
                                                    compen+=1;
                                            }
                                            for(int k = 0;k < 4; ++k) {
                                                if(x + l1vdx[k] <= BOARD_SIZE && y + l1vdy[k] <= BOARD_SIZE &&x + l1vdx[k] >= 1 && y + l1vdy[k] >= 1 &&orgstate.getMap().h_block(Point(x + l1vdx[k],y + l1vdy[k])))
                                                    compen++;
                                            }
                                        }
                                        else if (w==2){
                                            for(int k = 0;k < 4; ++k) {
                                                // hblock
                                                if(x + l2hdx[k] <= BOARD_SIZE && y + l2hdy[k] <= BOARD_SIZE &&x + l2hdx[k] >= 1 && y + l2hdy[k] >= 1  && orgstate.getMap().v_block(Point(x + l2hdx[k],y + l2hdy[k])))
                                                    compen+=1;
                                            }
                                            for(int k = 0;k < 4; ++k) {
                                                if(x + l2vdx[k] <= BOARD_SIZE && y + l2vdy[k] <= BOARD_SIZE &&x + l2vdx[k] >= 1 && y + l2vdy[k] >= 1 &&orgstate.getMap().h_block(Point(x + l2vdx[k],y + l2vdy[k])))
                                                    compen++;
                                            }                                 
                                        }
                                        else if (w==3){
                                            for(int k = 0;k < 4; ++k) {
                                                // hblock
                                                if(x + l3hdx[k] <= BOARD_SIZE && y + l3hdy[k] <= BOARD_SIZE &&x + l3hdx[k] >= 1 && y + l3hdy[k] >= 1  && orgstate.getMap().v_block(Point(x + l3hdx[k],y + l3hdy[k])))
                                                    compen+=1;
                                            }
                                            for(int k = 0;k < 4; ++k) {
                                                if(x + l3vdx[k] <= BOARD_SIZE && y + l3vdy[k] <= BOARD_SIZE &&x + l3vdx[k] >= 1 && y + l3vdy[k] >= 1 &&orgstate.getMap().h_block(Point(x + l3vdx[k],y + l3vdy[k])))
                                                    compen++;
                                            }   
                                        }
                                        else {
                                            for(int k = 0;k < 4; ++k) {
                                                // hblock
                                                if(x + l4hdx[k] <= BOARD_SIZE && y + l4hdy[k] <= BOARD_SIZE &&x + l4hdx[k] >= 1 && y + l4hdy[k] >= 1  && orgstate.getMap().v_block(Point(x + l4hdx[k],y + l4hdy[k])))
                                                    compen+=1;
                                            }
                                            for(int k = 0;k < 4; ++k) {
                                                if(x + l4vdx[k] <= BOARD_SIZE && y + l4vdy[k] <= BOARD_SIZE &&x + l4vdx[k] >= 1 && y + l4vdy[k] >= 1 &&orgstate.getMap().h_block(Point(x + l4vdx[k],y + l4vdy[k])))
                                                    compen++;
                                            }        
                                        }
                                    
                                    for(int k = 0; k < sqrt(compen); ++k)
                                        actions.push_back(action->copy());
                                    
                                    //pair<int, int> q = make_pair(3, w);
                                    //tuple<pair<int, int>, int, vector<MCTS::Action*>> p = make_tuple(q, compen, actions);
                                    //all_actions_t.push_back(p);
				    
                                    if (action == nullptr){
                                    cout << "oh god you saved me" << endl;
                                    }
                                    delete action;
				    
                                }
                            }
                            
                        }
                    }
		    
            return actions;
        }
        int get_postech_IWall() {
            for(int i = 0 ; i < I_COUNT; ++i) {
                if(!orgstate.block_ip[i])
                    return I_COUNT - i;
            }
            return 0;
        }
        int get_postech_LWall() {
            for(int i = 0 ; i < L_COUNT; ++i) {
                if(!orgstate.block_lp[i])
                    return L_COUNT - i;
            }
            return 0;
        }
        int get_kaist_IWall() {
            for(int i = 0 ; i < I_COUNT; ++i) {
                if(!orgstate.block_ik[i])
                    return I_COUNT - i;
            }
            
            return 0;
        }
        int get_kaist_LWall() {
            for(int i = 0 ; i < L_COUNT; ++i) {
                if(!orgstate.block_lk[i])
                    return L_COUNT - i;
            }
            return 0;
        }
        virtual vector<MCTS::Action*> get_all_actions_simulation(){
            static thread_local std::mt19937 generator;
            vector<MCTS::Action*> actions;
            ::State curstate = this->orgstate;
            int dx[] = {1,-1,0,0,-1,-1,1,1,2,-2,0,0};
            int dy[] = {0,0,1,-1,1,-1,1,-1,0,0,-2,2};
            int mx = curstate.position().x();
            int my = curstate.position().y();

            if(this->done){ // no more action if state is done
                return actions;
            }
            bool turn = orgstate.getTurn();
            int compen = 0;
            re:;
            float blockratio = 0.3; //전체 행동중 블럭을 놓는 비율
            float betweenwallratio = 0.6f; //블럭을 놓는 행동 중, 벽 근처에 놓는 비율. 다른 부분이 구현되면 수치 조정.
            //float moveratio = 0.7f; // 전체행동 중 움직이는 비율
            float bestmoveratio = 0.9f; // 움직이는 행동 중, 최단거리로 움직이는 비율
            int totalMovecnt = 100;
            std::uniform_int_distribution<int> distribution(0, 100);
            if(distribution(generator) < 100 * blockratio) {
                if(distribution(generator) < 100 * betweenwallratio){
                    return get_adj_walls();
                }
                else {
                    actions = get_blocking_action();
                    if(actions.size() == 0){
                        blockratio = 0;
                        goto re;
                    }
                    return get_blocking_action();

                }
            }
            else{
                //cout << "no!!!\n";
                // MOVE - 맵 밖을 벗어나는 것도 허용임
                // action 500 -> 300 개를 move // 2 : 50,  4 : 25
	      
                if(distribution(generator) > 100 * bestmoveratio) {
                    int pos[] = {curstate.position().x(), curstate.position().y()};
                    for (int d = 0; d < 12; d++){
                        if ((int)(curstate.act(1,pos[0]+dx[d],pos[1]+dy[d],1).position().x()) != 0){
                                LoqAction* action = new LoqAction(1,pos[0]+dx[d],pos[1]+dy[d],1);
                                actions.push_back(action);
                        }
                    }
                    
                    return actions;
                }
                else {
		  
                    actions.push_back((LoqAction*)get_best_move_action());
		   
                    return actions;
                }
            }
        }



        bool is_block_all_used(bool turn){
            IBlock* iblock;
            LBlock* lblock;
            if (!turn){
                iblock = orgstate.getKIBlock();
                lblock = orgstate.getKLBlock();
            }
            else{
                iblock = orgstate.getPIBlock();
                lblock = orgstate.getPLBlock();
            }
            for (int i=0; i<I_COUNT; ++i){
                if(!iblock[i]) return false;
            }
            for (int i=0; i<L_COUNT; ++i){
                if(!lblock[i]) return false;
            }
            return true;
        }
        
        bool avail_bfs(Point start,Point action, bool turn){
            queue<pair<Point, int>> q;
            vector<vector<bool>> visited(10, vector<bool>(10, false));
            q.push({action, 0});
            if(start.x() < 0 || start.y() < 0 || start.x() > 9 || start.y() > 9){
                cout << "\n+++++++++++++++++++++++++++++++++++\n"<<(int)start.x() << "|"<<(int)start.y() << endl;
                return 1;
            }
            visited[start.x()][start.y()] = true;
            visited[action.x()][action.y()] = true;
            int nx, ny, cx, cy;
            int dx[] = {-1, 0, 1, 0};
            int dy[] = {0, -1, 0, 1};
            int goal_y = turn? 1 : 9; // 포스텍 turn이 true임

            while (!q.empty()){
                Point current = q.front().first;
                int steps = q.front().second;
                if(!q.empty())
                    q.pop();
                
                cx = current.x();
                cy = current.y();
                //cout << cx << " " << cy << " " << steps << endl;
                if (cy == goal_y) return true;
                for (int i = 0; i < 4; ++i){
                    nx = cx + dx[i];
                    ny = cy + dy[i];
                    // 상대방이 없다고 가정
                    if (!(1 <= nx && nx <= 9 && 1 <= ny && ny <= 9))
                        continue;
                    if (orgstate.movable(Point(nx, ny), current, Point(-1, -1)) && !visited[nx][ny]){
                        q.push({Point(nx, ny), steps + 1});
                        visited[nx][ny] = true;
                    }
                }
            }
            return false; // No path found
        }

        int bfs(Point start, bool turn){
            queue<pair<Point, int>> q;
            vector<vector<bool>> visited(10, vector<bool>(10, false));
            q.push({start, 0});
            if(start.x() < 0 || start.y() < 0 || start.x() > 9 || start.y() > 9){
                cout << "\n+++++++++++++++++++++++++++++++++++\n"<<(int)start.x() << "|"<<(int)start.y() << endl;
                return 1;
            }
            visited[start.x()][start.y()] = true;
            int nx, ny, cx, cy;
            int dx[] = {-1, 0, 1, 0};
            int dy[] = {0, -1, 0, 1};
            int goal_y = turn? 1 : 9; // 포스텍 turn이 true임

            while (!q.empty()){
                Point current = q.front().first;
                int steps = q.front().second;
                if(!q.empty())
                    q.pop();
                
                cx = current.x();
                cy = current.y();
                //cout << cx << " " << cy << " " << steps << endl;
                if (cy == goal_y) return steps;
                for (int i = 0; i < 4; ++i){
                    nx = cx + dx[i];
                    ny = cy + dy[i];
                    // 상대방이 없다고 가정
                    if (!(1 <= nx && nx <= 9 && 1 <= ny && ny <= 9))
                        continue;
                    if (orgstate.movable(Point(nx, ny), current, Point(-1, -1)) && !visited[nx][ny]){
                        q.push({Point(nx, ny), steps + 1});
                        visited[nx][ny] = true;
                    }
                }
            }
            return -1; // No path found
        }

        MCTS::Action* get_best_move_action(){
        int nx, ny;
        int min_dis = 1e9;
        Point pos = orgstate.position();
        MCTS::Action* action = nullptr;
        vector<pair<int,int>> dis_arr;
        vector<MCTS::Action*> actions;
        int cx = pos.x();
        int cy = pos.y();
        int dx[] = {1,-1,0,0,-1,-1,1,1,2,-2,0,0};
        int dy[] = {0,0,1,-1,1,-1,1,-1,0,0,-2,2};

        for (int i = 0; i < 12; ++i){
            nx = cx + dx[i];
            ny = cy + dy[i];

            Point npos(nx, ny);
            if (orgstate.movable(npos, pos, orgstate.position(false))){
                int dis = bfs(npos, orgstate.getTurn());
                if (dis != -1 && dis <= min_dis){
                    min_dis = dis;
                    dis_arr.emplace_back(std::make_pair(i, min_dis));
                    //cout << dis << endl;
                }
            }
        }
        //cout << dis_arr.size() << "----";
        for (auto it: dis_arr){
            if (it.second == min_dis) {
                nx = cx + dx[it.first];
                ny = cy + dy[it.first];
                action = new LoqAction(1, nx, ny, 1);
                if (!orgstate.getTurn()){
                    if (dy[it.first] > 0) {
		                for(int i=0;i<8;i++){	
                            actions.emplace_back(action->copy());
		                }
                    }
                }
                else{
                    if (dy[it.first] < 0) {
		                for(int i=0;i<8;i++){
			            actions.emplace_back(action->copy());
		                }
                    }
                }
                // cout << it.first << endl;
                //if (action != nullptr) delete action;
                actions.emplace_back(action->copy());
            }
        }

        if(actions.size() == 0)
        {
            //printf("fuck!!");
            action = new LoqAction(1,5,8,2);
            //cout << to_string() << endl;
        }
        else{
	        action = actions[(rand() / 10) % actions.size()]->copy();
	    }
        for(auto it : actions){
	        if (it != nullptr){
	            delete it;
            }
        }
        //cout << "size: " << actions.size() << endl;
        //actions.clear();
        return action;
    }

    virtual vector<MCTS::Action*> get_all_best_move_actions(){
        int nx, ny;
        int min_dis = 1e9;
        Point pos = orgstate.position();
        MCTS::Action* action = nullptr;
        vector<pair<int,int>> dis_arr;
        vector<MCTS::Action*> actions;
        int cx = pos.x();
        int cy = pos.y();
        int dx[] = {1,-1,0,0,-1,-1,1,1,2,-2,0,0};
        int dy[] = {0,0,1,-1,1,-1,1,-1,0,0,-2,2};

        for (int i = 0; i < 12; ++i){
            nx = cx + dx[i];
            ny = cy + dy[i];

            Point npos(nx, ny);
            if (orgstate.movable(npos, pos, orgstate.position(false))){
                int dis = bfs(npos, orgstate.getTurn());
                if (dis != -1 && dis <= min_dis){
                    min_dis = dis;
                    dis_arr.emplace_back(std::make_pair(i, min_dis));
                    //cout << dis << endl;
                }
            }
        }
        //cout << "min_dis: "<< min_dis << endl;

        for (auto it: dis_arr){
            if (it.second == min_dis) {
                nx = cx + dx[it.first];
                ny = cy + dy[it.first];
                //cout << it.first << endl;
                //if (action != nullptr) delete action;
                action = new LoqAction(1, nx, ny, 1);
                actions.emplace_back(action);
            }
        }
        return actions;
    }

        // virtual MCTS::Action* policy(){
        //     if(is_block_all_used(true)&&is_block_all_used(false)){
        //         return get_best_move_action();
        //     }
        //     else{
        //         return NULL;
        //     }
        // }
        // revised part //
        static bool compare(const pair<MCTS::Action*, int> a, const pair<MCTS::Action*, int> b){
            return a.second < b.second;
        }

        virtual MCTS::Action* policy(){
            bool myturn = get_turn();
            int my_dis = bfs(orgstate.position(), myturn);
            int op_dis = bfs(orgstate.position(false), !myturn);
                
            
            if (is_block_all_used(!myturn)){
                if(my_dis <= op_dis || is_block_all_used(myturn))              
                    return get_best_move_action();
                else{
                    vector<MCTS::Action*>candidates;
                    for(int i = 1; i <=9; ++i) {
                        for(int j = 1; j <=9; ++j){
                            for(int w = 1; w<=2;++w)
                                if((int)orgstate.act(2,i,j,w).position().x() != 0)
                                    candidates.push_back(new LoqAction(2,i,j,w));
                            for(int w = 1; w<=4;++w)
                                if((int)orgstate.act(3,i,j,w).position().x() != 0)
                                    candidates.push_back(new LoqAction(3,i,j,w));
                        }
                    }
                    
                    vector<pair<MCTS::Action*, int>> actions;
                    for (auto action : candidates){
                        LoqState* st_ptr = (LoqState*) step(action, get_turn());
                        LoqState new_state = *st_ptr;
                        int n_op_dis = new_state.bfs(new_state.get_internal_state().position(false), !myturn);
                        int n_my_dis = new_state.bfs(new_state.get_internal_state().position(true), myturn);
                        int val = (n_my_dis - my_dis) - (n_op_dis - op_dis); // val 작을수록 좋음
                        if ((int)st_ptr->get_internal_state().position().x() != 0)
                            actions.emplace_back(make_pair(action, val));
                    }

                    sort(actions.begin(), actions.end(), compare);
                    if (actions.empty()) return nullptr;
                    MCTS::Action* act = actions[0].first->copy();
                    
                    for(auto it :candidates){
		      
                        delete it;
                    }
                    return act;
                }
            }
            else{
                return NULL;
            }
        }
        // revised part //
        
        virtual MCTS::State* step(MCTS::Action* action, bool turn){ //manipulate done, draw, turn (deepcopy needed)  
            // Since act is safe for change, no deepcopy
            ::State changed = this->orgstate.act(*(int*)action->get_info(1),*(int*)action->get_info(2),*(int*)action->get_info(3),*(int*)action->get_info(4));
            LoqState* next_state = new LoqState(changed, turn); // 이거 turn 시스템 수정해야됨 

            if (changed.win() != 0){
                next_state->done = true;
            }
            
            bool no_action = true;

            next_state->turn = !this->turn;

            return next_state;
        }

        virtual string to_string(){
            // for test
            //Todo : display the line index.
            string s = "";
            bool t = orgstate.getTurn();
            int kx = orgstate.position(!t).x();
            int ky = orgstate.position(!t).y();
            int px = orgstate.position(t).x();
            int py = orgstate.position(t).y();

            for(int j=0 ; j < 2*BOARD_SIZE-1 ; j++){
                for(int i=0 ; i < 2*BOARD_SIZE-1 ; i++){
                    if(j%2==0){
                        if(i % 2 == 0){
                            if((kx-1)*2 == i && (9-ky)*2 == j) // blue
                                s+="\033[34m O \033[0m";
                            else if((px-1)*2 == i && (9-py)*2 == j) // red
                                s+="\033[31m O \033[0m";
                            else
                                s+="   ";
                        }
                        else{
                            if(orgstate.h_block(Point(i/2+2, 9-j/2)))
                                s+="\033[92m┃\033[0m";
                            else
                                s+="│";
                        }
                    }
                    else{
                        if(i % 2 == 0){
                            if(orgstate.v_block(Point(i/2+1, 9-j/2)))
                                s+="\033[92m━━━\033[0m";
                            else
                                s+="───";
                        }
                        else
                            s+="┼";
                    }
                }
                s+="\n";
            }
            return s;
        }
};


MCTS::MoveInfo get_MoveInfo(LoqState state, int epochs, int rollout_num, int thread_num){
    time_t start = time(NULL);
    MCTS::SearchTree tree(&state, thread_num);
    while(1){
        tree.selection();
	    tree.expansion();
        float win_num = tree.rollout(rollout_num);
        tree.backpropagation(win_num, rollout_num);

        time_t middle = time(NULL);
        if((middle - start) >= 28){
            break;
        }
    }
    MCTS::MoveInfo info = tree.get_next_move();
    /*vector<pair<float,MCTS::Node*>> ve = root->get_head_winning_rate(0); // 0으로 하면 출력 안함
    for(int i = 0; i < ve.size(); ++i) {
        MCTS::Node* cur = ve[i].second;
        cout << i <<"th WXN : " << cur->W * cur->N<< endl;
        cout << i <<"th W : " << cur->W << endl;
        cout << i <<"th N : " << cur->N << endl;
        cout << i <<"th winning rate : " << cur->get_winning_rate() << endl;
        cout << cur->get_state()->to_string() << endl;
    }*/
    
    tree.clear();
    return info;
}	
