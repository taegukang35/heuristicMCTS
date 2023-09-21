#include <vector>
#include <limits>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>
#include <future>
#include <random>
#include <mutex>
#include "base.h"
#include<algorithm>

#if defined (_MSC_VER)  // Visual studio
    #define thread_local __declspec( thread )
#elif defined (__GCC__) // GCC
    #define thread_local __thread
#endif

using namespace std;

static int REAL_THREAD_NUM = std::thread::hardware_concurrency();
static float EXPLORE_VALUE = 1.2;
mutex win_lock, node_lock;

namespace MCTS{

class Node{
    private:

    public:
        State* state;
        Action* prev_action;
        bool turn;
        Node* parent;
        vector<Node*> childs;
        float W;
        float N;
        float UCT;
        Node(State* s, bool t){
            this->state = s;
            this->prev_action = NULL;
            this->turn = t;
            this->parent = NULL;
            this->W = 0.0f;
            this->N = 0.0f;
            this->UCT = 0.0f;
        }

        Node(State* s, bool t, Node* p, Action* a){
            this->state = s;
            this->prev_action = a;
            this->turn = t;
            this->parent = p;
            this->W = 0.0f;
            this->N = 0.0f;
            this->UCT = 0.0f;
        }

        ~Node(){
            for(auto child : this->childs){
                delete child;
            }
            delete prev_action;
            delete state;
        }

        float cal_UCT(float t){
            if(N==0)
                UCT = numeric_limits<float>::max();
            else
                UCT = (W/N)+EXPLORE_VALUE*sqrt(log(t)/N);
            return UCT;
        }

        float cal_winning_rate(){
            if(N==0)
                return 0.0f;
            else
                return (this->W)/(this->N);
        }

        Node* get_max_UCT(){ // cal UCT and find max UCT Node*
            float max_UCT = -100;
            Node* max_node = NULL;

            for(Node* node : this->childs){
                float tmp_UCT = node->cal_UCT(this->N);
                
                if(tmp_UCT > max_UCT){
                    max_UCT = tmp_UCT;
                    max_node = node;
                }
            }

            return max_node;
        }
        vector<pair<float,Node*>> get_head_winning_rate(int size) {
            vector<pair<float, Node*>> ve;
            for(int i =0; i < this->childs.size(); ++i)
                ve.push_back(pair<float,Node*>(-childs[i]->W/childs[i]->N, childs[i]));
            sort(ve.begin(), ve.end());
            for(int i = 0; i < ve.size(); ++i) {
                ve[i].first = -ve[i].first;
            }

            vector<pair<float, Node*>> result;
            for(int i = 0; i < size && i < ve.size(); ++i) {
                result.push_back(make_pair(-ve[i].first, ve[i].second));
            }
            return result;
        }
        Node* get_max_winning_rate(){ // now uses win count
            float max_wr = -1;
            Node* max_node = NULL;
            for(Node* node : this->childs){
                float tmp_wr = node->W;
                
                if (*(int *)node->prev_action->get_info(1) == 1){
                    vector<MCTS::Action*> best_actions = node->get_state()->get_all_best_move_actions();
                    for (auto it: best_actions){
                        if (node->prev_action == it){
                            //tmp_wr += 0.05;
                            tmp_wr *= 1.414;
                        }
                        //cout << tmp_wr << endl;
                    }
                }
                //float tmp_wr = node->W;
                if(tmp_wr > max_wr){
                    max_wr = tmp_wr;
                    max_node = node;
                }
            }

            return max_node;
        }

        int get_child_num(){
            return this->childs.size();
        }

        Node* get_child(int index){ // get child with index
            return this->childs[index];
        }

        State* get_state(){
            return this->state;
        }

        Action* get_action(){
            return this->prev_action;
        }

        float get_UCT_p(){
            float sum = 0;
            for(Node* node : this->parent->childs){
                node->cal_UCT(node->parent->N);
                sum += UCT;
            }
            return this->UCT / sum;
        }

        Node* get_parent(){
            return this->parent;
        }

        bool get_turn(){
            return this->turn;
        }

        float get_winning_rate(){
            if(this->N == 0)
                return '\0';
            return this->W / this->N;
        }

        int get_node_num(){
            int sum = 1;
            for(auto child : childs)
                sum += child->get_node_num();
            return sum;
        }

        Node* add_child(State* s_child, Action* a){ // add child of the node, doubly linked
            Node* child = new Node(s_child, !this->turn, this, a);
            this->childs.push_back(child);
            return child;
        }

        vector<Action*> get_all_actions(){
            return this->state->get_all_actions();
        }

        State* step(Action* action){
            return this->state->step(action, this->turn);
        }

        void add_wins(float W, float N){
            this->W += W;
            this->N += N;
        }

        string to_string(){
            // implement not ended / only for test
            return this->turn ? "True" : "False";
        }
};

class SearchTree{
    private:
        Node* now_node;
        int thread_num;
    
    public:
        Node* root;
        SearchTree(State* s, int thread_num){
	  
            root = new Node(s->copy(), s->get_turn());
            now_node = root;

            

            if(thread_num > REAL_THREAD_NUM){
                cout << "Invalid thread number. Using single-thread." << endl;
                this->thread_num = 1;
            }
            else{
                
                this->thread_num = thread_num;
            }
        }

        void clear(){
            delete root;
        }

        State* selection(){
            now_node = root;
            while(now_node->get_child_num()!=0){
                // calculate UCT of childs
                // change now_node
                now_node = now_node->get_max_UCT();
            }
            return now_node->get_state();
        }

        State* selection_test(){
            cout << "Testing start." << endl; 
            now_node = root;
            cout << now_node->get_state()->to_string() << endl;
            while(now_node->get_child_num()!=0){
                // calculate 'winning rate' of childs
                // winning rate is standard for game
                now_node = now_node->get_max_winning_rate();
                cout << now_node->get_state()->to_string() << endl;
                cout << now_node->get_action()->to_string() << endl;
            }
            return now_node->get_state();
        }

        void expansion(){
            vector<Action*> actions = now_node->get_all_actions();
            for(Action* action : actions){
                State* tmp_state = now_node->step(action);
                now_node->add_child(tmp_state, action);
            }
            if(now_node->get_child_num()!=0){
                srand((unsigned int)time(NULL));
                now_node = now_node->get_child(rand()%now_node->get_child_num());
            }
        }

        float rollout(float N){
            State* state = now_node->get_state();
            bool turn = now_node->get_turn();
            if(state->is_done()){ // if there is no action to simulate
                if(state->is_draw())
                    return N/2;
                else
                    return N;
            }

            float W = 0;
            
            // multi-thread execution of simulation()
            // add all count
            vector<std::thread> thrs;
            vector<float> sim_nums;
            sim_nums.resize(this->thread_num);
            for(int i=0 ; i<this->thread_num ; i++){
                sim_nums[i] = (float)(((int)N)/(this->thread_num));
            }
            for(int i=0 ; i<(((int)N)%(this->thread_num)) ; i++){
                sim_nums[i] += 1.0;
            }

            for(int i=0 ; i<this->thread_num ; i++){
                thrs.push_back(std::thread(&SearchTree::simulation, this, sim_nums[i], &W));
            }

            for(int i=0 ; i<this->thread_num ; i++){
                thrs[i].join();
            }

            return W;
        }

        void simulation(float N, float* win){ // multi-thread unit for rollout()
            static thread_local std::mt19937 generator; // random generator
            node_lock.lock();
            State* state = now_node->get_state()->copy();
            bool turn = now_node->get_turn();
            node_lock.unlock();
            
            float W = 0;
            for(int i=0 ; i<N ; i++){
                State* now_state = state->copy();
                bool now_turn = turn;

                while(!now_state->is_done()){
                    State* tmp1 = now_state->copy();
                    Action* policy = tmp1->policy();
                    Action* action;

                    if(policy == NULL){ // if no policy, random search
		                vector<Action*> actions = tmp1->get_all_actions_simulation();
			
                        if(actions.size()==0)
                            break;
                        std::uniform_int_distribution<int> distribution(0, actions.size()-1);
                        action = actions[distribution(generator)]->copy();
                        for(Action* act : actions)
                            delete act;
                    }
                    else{ // if there is policy, use it
                        action = policy;
                    }

                    State* tmp2 = now_state->copy();
                    now_state = tmp2->step(action, now_turn);
                    delete action;
                    delete tmp1;
                    delete tmp2;

                    now_turn = !now_turn;
                }

                if(now_state->is_draw())
                    W+=0.5;
                else{
                    if(now_turn == turn)
                        W+=1;
                }
                delete now_state;
            }
            delete state;

            win_lock.lock();
            *win = *win + W;
            win_lock.unlock();
        }

        void backpropagation(float W, float N){ // W : winning count of now_node.turn
            while(1){
                now_node->add_wins(W, N);
                W = N-W;
                if(now_node == root)
                    break;
                else
                    now_node = now_node->get_parent();
            }
        }

        int get_node_num(){
            return this->root->get_node_num();
        }

        MoveInfo get_next_move(){
            MoveInfo m;
            Node* n = root->get_max_winning_rate();
            m.state = n->get_state();
            m.action = n->get_action();
            m.UCT_p = n->get_UCT_p();
            m.win_rate = n->get_winning_rate();

            return copy(m);
        }
};

}
