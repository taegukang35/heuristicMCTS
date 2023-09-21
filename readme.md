# **Mymcts.h**
2023/08/26
Loq self-play added. Memory leak fixed. <br/>
policy, MoveInfo added <br/>
TTT and C4 is no more available due to lots of update.

2023/08/23
Loq(League of Quordior) added. <br/>
Namespace MCTS added due to name collision.

2023/08/08
This is MCTS code written by C++. <br/>
Multi-thread rollout is applied.

## Description of files
### MCTS/
- base.h : defines Action, State class.
- MCTS.h : defines MCTS procedure with Action, State class.
### TTT/
- TTT.h : Inherits Action, State class and implement it to TTTAction, TTTState. Implemented the rule of the game.
- TTT.c : Includes all headers and trains it with for-loop.
### C4/
- This is Connect-4 game, and structure is same with Tic-Tac-Toe game.