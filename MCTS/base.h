#pragma once

using namespace std;

namespace MCTS{

class Action{
    public:
        virtual ~Action() = default;
        virtual Action* copy() = 0;
        virtual void* get_info(int index) = 0;
        virtual string to_string() = 0;
};

class State{
    protected:
        bool done = false;
        bool draw = false;
        bool turn = true;

    public:
        bool is_done(){
            return this->done;
        }

        bool is_draw(){
            return this->draw;
        }

        bool get_turn(){
            return this->turn;
        }

        virtual ~State() = default;
        virtual State* copy() = 0;
        virtual vector<Action*> get_all_actions() = 0;
        virtual vector<Action*> get_all_actions_simulation() = 0; // for biased action selection
        virtual vector<MCTS::Action*> get_all_best_move_actions() = 0;
        virtual Action* policy() = 0;
        virtual State* step(Action* action, bool turn) = 0;
        virtual string to_string() = 0;
};

struct MoveInfo{
    MCTS::State* state;
    MCTS::Action* action;
    float UCT_p;
    float win_rate;
};

MoveInfo copy(MoveInfo m){
    MoveInfo n;
    n.state = m.state->copy();
    n.action = m.action->copy();
    n.UCT_p = m.UCT_p;
    n.win_rate = m.win_rate;

    return n;
}

void free_MoveInfo(MoveInfo m){
    delete m.state;
    delete m.action;
}

}