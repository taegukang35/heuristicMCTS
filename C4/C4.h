#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include "../MCTS/base.h"

using namespace std;

class C4Action : public MCTS::Action{
    public:
        int x;

        C4Action(int x){
            this->x = x;
        }

        virtual void* get_info(int index){ // State can only get info using this method
            if(index==1)
                return &this->x;
            return NULL;
        }
};

class C4State : public MCTS::State{
    protected:
        const static int BOARD_V = 6;
        const static int BOARD_H = 7;
        int board[BOARD_V][BOARD_H] = {0};
    
    public:
        C4State(){ // this gives initial state
            for(int i=0 ; i<BOARD_V ; i++){
                for(int j=0 ; j<BOARD_H ; j++){
                    this->board[i][j] = 0;
                }
            }
            this->turn = true;
        }

        C4State(int board[BOARD_V][BOARD_H], bool turn){
            for(int i=0 ; i<BOARD_V ; i++){
                for(int j=0 ; j<BOARD_H ; j++){
                    this->board[i][j] = board[i][j];
                }
            }
            this->turn = turn;
            bool done = false;
            bool draw = false;
        }

        C4State(const C4State& copy){ // overriding copy constructor to deepcopy
            std::copy(&copy.board[0][0], &copy.board[0][0]+BOARD_V*BOARD_H, &this->board[0][0]);
            this->turn = copy.turn;
            this->done = copy.done;
            this->draw = copy.draw;
        }

        virtual vector<MCTS::Action*> get_all_actions(){
            vector<MCTS::Action*> actions;

            if(this->done){ // no more action if state is done
                return actions;
            }

            for(int j=0 ; j<BOARD_H ; j++){
                if(this->board[0][j]==0){
                    C4Action* action = new C4Action(j); // should be generated with 'new'
                    actions.push_back(action);
                }
            }
            return actions;
        }

        virtual MCTS::Action* policy(){
            return NULL;
        }

        static bool in_coord(int j, int i){
            return (0<=j)&&(j<BOARD_V)&&(0<=i)&&(i<BOARD_H);
        }

        virtual State* step(MCTS::Action* action, bool turn){ //manipulate done, draw, turn (deepcopy needed)  
            C4State* next_state = new C4State(*this); // deepcopy - should be generated with 'new'
            
            int x = *(int*)action->get_info(1);
            int y = BOARD_V-1;
            int now_num = (turn?1:-1);

            while(next_state->board[y][x]!=0)
                y--;
            next_state->board[y][x] = now_num;

            // check winner exists
            int count = -1;
            int i = x, j = y;

            count=-1;i=x;j=y;
            while(in_coord(j,i)&&(next_state->board[j][i]==now_num)){
                count++;
                i++;
            }
            i=x;j=y;
            while(in_coord(j,i)&&(next_state->board[j][i]==now_num)){
                count++;
                i--;
            }
            if(count>=4)
                next_state->done = true;
            
            count=-1;i=x;j=y;
            while(in_coord(j,i)&&(next_state->board[j][i]==now_num)){
                count++;
                j++;
            }
            i=x;j=y;
            while(in_coord(j,i)&&(next_state->board[j][i]==now_num)){
                count++;
                j--;
            }
            if(count>=4)
                next_state->done = true;
            
            count=-1;i=x;j=y;
            while(in_coord(j,i)&&(next_state->board[j][i]==now_num)){
                count++;
                i++;j++;
            }
            i=x;j=y;
            while(in_coord(j,i)&&(next_state->board[j][i]==now_num)){
                count++;
                i--;j--;
            }
            if(count>=4)
                next_state->done = true;
            
            count=-1;i=x;j=y;
            while(in_coord(j,i)&&(next_state->board[j][i]==now_num)){
                count++;
                i++;j--;
            }
            i=x;j=y;
            while(in_coord(j,i)&&(next_state->board[j][i]==now_num)){
                count++;
                i--;j++;
            }
            if(count>=4)
                next_state->done = true;

            // if no action but no winner, it means draw.
            bool no_action = true;
            for(int j=0 ; j<BOARD_H ; j++){
                if(next_state->board[0][j]==0)
                    no_action = false;
            }

            if(no_action&&(!next_state->done)){
                next_state->done = true;
                next_state->draw = true;
            }

            next_state->turn = !this->turn;

            return next_state;
        }

        virtual string to_string(){
            string str;
            for(int i=0 ; i<BOARD_V ; i++){
                for(int j=0 ; j<BOARD_H ; j++){
                    switch(this->board[i][j]){
                        case -1:
                            str.append("X");
                            break;
                        case 0:
                            str.append("_");
                            break;
                        case 1:
                            str.append("O");
                            break;
                    }
                }
                str.append("\n");
            }
            return str;
        }
};