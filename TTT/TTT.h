#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include "../MCTS/base.h"

using namespace std;

class TTTAction : public MCTS::Action{
    public:
        int x;
        int y;

        TTTAction(int x, int y){
            this->x = x;
            this->y = y;
        }

        virtual void* get_info(int index){ // State can only get info using this method
            if(index==1)
                return &this->x;
            else if (index==2)
                return &this->y;
            return NULL;
        }

        virtual string to_string(){
            return std::to_string(x) + " " + std::to_string(y);
        }
};

class TTTState : public MCTS::State{
    protected:
        const static int BOARD_SIZE = 3;
        int board[BOARD_SIZE][BOARD_SIZE] = {0};
    
    public:
        TTTState(){ // this gives initial state
            for(int i=0 ; i<BOARD_SIZE ; i++){
                for(int j=0 ; j<BOARD_SIZE ; j++){
                    this->board[i][j] = 0;
                }
            }
            this->turn = true;
        }

        TTTState(int board[BOARD_SIZE][BOARD_SIZE], bool turn){
            for(int i=0 ; i<BOARD_SIZE ; i++){
                for(int j=0 ; j<BOARD_SIZE ; j++){
                    this->board[i][j] = board[i][j];
                }
            }
            this->turn = turn;
            bool done = false;
            bool draw = false;
        }

        TTTState(const TTTState& copy){ // overriding copy constructor to deepcopy
            std::copy(&copy.board[0][0], &copy.board[0][0]+BOARD_SIZE*BOARD_SIZE, &this->board[0][0]);
            this->turn = copy.turn;
            this->done = copy.done;
            this->draw = copy.draw;
        }

        virtual vector<MCTS::Action*> get_all_actions(){
            vector<MCTS::Action*> actions;

            if(this->done){ // no more action if state is done
                return actions;
            }

            for(int i=0 ; i<BOARD_SIZE ; i++){
                for(int j=0 ; j<BOARD_SIZE ; j++){
                    if(this->board[i][j]==0){
                        TTTAction* action = new TTTAction(i, j); // should be generated with 'new'
                        actions.push_back(action);
                    }
                }
            }
            return actions;
        }

        virtual MCTS::Action* policy(){ // policy when rollout
            return NULL; // NULL means no policy for this state
        }

        virtual State* step(MCTS::Action* action, bool turn){ //manipulate done, draw, turn (deepcopy needed)  
            TTTState* next_state = new TTTState(*this); // deepcopy - should be generated with 'new'
            next_state->board[*(int*)action->get_info(1)][*(int*)action->get_info(2)] = (turn?1:-1);

            // check winner exists
            int check = 0;
            for(int i=0 ; i<BOARD_SIZE ; i++){
                check = 0;
                for(int j=0 ; j<BOARD_SIZE ; j++){
                    check += next_state->board[i][j];
                }
                if(abs(check)==BOARD_SIZE){
                    next_state->done = true;
                }
            }

            for(int i=0 ; i<BOARD_SIZE ; i++){
                check = 0;
                for(int j=0 ; j<BOARD_SIZE ; j++){
                    check += next_state->board[j][i];
                }
                if(abs(check)==BOARD_SIZE){
                    next_state->done = true;
                }
            }

            check = 0;
            for(int i=0 ; i<BOARD_SIZE ; i++){
                check += next_state->board[i][i];
            }
            if(abs(check)==BOARD_SIZE){
                next_state->done = true;
            }

            check = 0;
            for(int i=0 ; i<BOARD_SIZE ; i++){
                check += next_state->board[i][BOARD_SIZE-1-i];
            }
            if(abs(check)==BOARD_SIZE){
                next_state->done = true;
            }
            
            // if no action but no winner, it means draw.
            bool no_action = true;
            for(int i=0 ; i<BOARD_SIZE ; i++){
                for(int j=0 ; j<BOARD_SIZE ; j++){
                    if(next_state->board[i][j]==0)
                        no_action = false;
                }
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
            for(int i=0 ; i<BOARD_SIZE ; i++){
                for(int j=0 ; j<BOARD_SIZE ; j++){
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