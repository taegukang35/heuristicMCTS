import main
from loq2 import State

if __name__=="__main__":
    game = State().next() # when i am second player
    #game = State() # when i am first player
    #print(game)
    i=1
    history = main.Init(i%2==0)
    while(True):
        print(game)
        if game.win:
            break
        if(i%2==0): 
            (next_move, history) = main.next(game.mask, history)
            print(next_move)
            game = game.act(*next_move)
        else:
            t,x,y,w = map(int, input().split())
            next_move = (t,x,y,w)
            print(next_move)
            game = game.act(*next_move)
        i+=1
        #print(game)
