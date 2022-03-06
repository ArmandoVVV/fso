#include <scheduler.h>
#include <stdio.h>

#define NQ 5

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

QUEUE ready[NQ];
QUEUE waitinginevent[MAXTHREAD];

int current_priority = 0;
int n_processes = 1;

void scheduler(int arguments){
	int old, next;
	int changethread = 0;
	int waitingthread = 0;

	int event = arguments & 0xFF00;
	int callingthread = arguments & 0xFF;

	if (event == NEWTHREAD){
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status = READY;
		_enqueue(&ready[0], callingthread);
		n_processes++;
	}

	if (event == TIMER){

		threads[callingthread].status = READY;
		if (current_priority < NQ - 1 && n_processes > 1)
			_enqueue(&ready[current_priority + 1], callingthread);
		else
			_enqueue(&ready[current_priority], callingthread);

		changethread = 1;
	}

	if (event == BLOCKTHREAD){
		threads[callingthread].status = BLOCKED;
		_enqueue(&waitinginevent[blockevent], callingthread);
		changethread = 1;
	}

	if (event == ENDTHREAD){
		threads[callingthread].status = END;
		changethread = 1;
	}

	if (event == UNBLOCKTHREAD){
		threads[callingthread].status = READY;
		if (current_priority < NQ - 1)
			_enqueue(&ready[current_priority + 1], callingthread);
		else
			_enqueue(&ready[current_priority], callingthread);
	}

	if (changethread){
		old = currthread;
		int i = 0;
		while(_emptyq(&ready[i]) && i < NQ){
			i++;
		}

		current_priority = i;
		next = _dequeue(&ready[i]);

		threads[next].status = RUNNING;
		_swapthreads(old, next);
	}

}