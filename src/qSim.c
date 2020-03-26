#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// type- 1 Newly arrived customer, time will denote arrival time
// type- 2 Customer with completed task, time will denotes its total time spent in the bank
// type- 3 Teller with completed time, time will denote its service time
// type- 4 teller with idle time

struct Event
{
    int type;
    float time;
    struct Customer* cust;
    struct Teller* teller;
    struct Event* next;

};

typedef void (*Push) (struct Event* e, struct Event* current);
typedef struct Event* (*Pop) (struct Event* h);

struct Eventq
{
    struct Event* head;
    Push push;
    Pop pop;
};

void push_event(struct Event* e, struct Event* current){
	if (current-> next == NULL){
		current -> next = e;
		return;
	}
	if (current->next->time > e->time){
		e->next = current->next;
		current -> next = e;
	}else{
		push_event(e,current->next);
	}
}

struct Event* pop_event(struct Event* h){
	struct Event* e1 = h->next;
	h->next = e1->next;
	e1->next = NULL;
	return e1;
}

void printEventList(struct Event* e){
	if(e->next != NULL){
		printf("%f\n", e->next->time);
		printEventList(e->next);
	}
}

struct Customer
{
    float arrival_time;
    float wait_time;
    float service_time;
    float total_time;
    struct Customer* next;
};

typedef void (*Push_cust) (struct Customer*  c, struct Customer* tail, int* length);
typedef struct Customer* (*Pop_cust) (struct Customer* h, int* length);

struct Teller
{
    float total_idle_time;
    float total_service_time;
    int line_no;
    struct Teller* next;
};


struct Tellerq
{
	struct Customer* head;
	struct Customer* tail;
	int* length;
	Push_cust push;
	Pop_cust pop;
};

void push_customer(struct Customer* c, struct Customer* tail, int* length){
	tail->next = c;
	*length = *length + 1;
}

struct Customer* pop_customer(struct Customer* h, int* length){
	struct Customer* c = h->next;
	h->next = c->next;
	c->next = NULL;
	*length = *length - 1;
	return c;
}

void singleQueue(char* argv[]){
	printf("----------------------SINGLE QUEUE SIMULATION----------------------\n");
	float time = 0;

	struct Event* h;
	h = (struct Event*) malloc (sizeof(struct Event));
	h->time=-1;
	h->next =NULL;
	h->cust = NULL;
	h->teller = NULL;
	struct Eventq* event_queue;
	event_queue = (struct Eventq*) malloc (sizeof(struct Eventq));
	event_queue->head = h;
	event_queue->push = push_event;
	event_queue->pop = pop_event;


	int no_customers = atoi(argv[1]);
	int no_tellers = atoi(argv[2]);
	float simulationTime = atof(argv[3]);
	float avgServiceTime = atof(argv[4]);

	for(int i=0;i<no_tellers;i++){
		struct Event* e = (struct Event*) malloc (sizeof(struct Event));
		e->time =0 ;
		e->next = NULL;
		e->type = 3;

		struct Teller* t = (struct Teller*) malloc (sizeof(struct Teller));
		t->total_idle_time =0;
		t->total_service_time=0;
		t->next = NULL;
		t->line_no = 1;

		e->teller = t;
		e->cust = NULL;
		event_queue->push(e, event_queue->head);
	}

	for(int i=0;i<no_customers;i++){
		float arrTime = simulationTime * ((1.0*rand())/(1.0*RAND_MAX));
		struct Event* e;
		e = (struct Event*) malloc (sizeof(struct Event));
		e->time = arrTime;
		e->type = 1;
		e->next = NULL;

		struct Customer* new_c = (struct Customer*) malloc (sizeof(struct Customer));
		new_c-> arrival_time = arrTime;
		new_c->wait_time = 0 ;
		new_c->service_time = 0;
		new_c->total_time = arrTime;
		new_c->next = NULL;

		e->cust = new_c;
		e->teller = NULL;

		event_queue->push(e, event_queue->head);
	}
	
	// printEventList(event_queue->head);
	printf("%s\n", "-------------------------------------------------");
	struct Tellerq* teller_queue;
	teller_queue = (struct Tellerq*) malloc (sizeof(struct Tellerq));
	struct Customer* head_cust = (struct Customer*) malloc(sizeof(struct Customer));
	head_cust -> arrival_time = -1;
    head_cust -> wait_time = -1;
    head_cust -> service_time = -1;
    head_cust -> total_time = -1;
    head_cust -> next = NULL;
    teller_queue -> head = head_cust;
    teller_queue -> push = push_customer;
    teller_queue -> pop = pop_customer;
    teller_queue -> length = (int*)malloc(sizeof(int));
    *teller_queue -> length = 0;
    
    int total_no_cust_served = 0;
    float total_time_req_to_serve_all_cust = 0;
    // int type_of_system = 1;
    float avg_time_spent_in_bank=0;
    float max_wait_time = 0;
    float total_amt_service_time = 0;
    float total_amt_idle_time = 0;

	while(no_customers>0 && time<simulationTime){
		struct Event* e = event_queue -> pop(event_queue->head);
		printf("time : %f    ", e->time);
		if(e->type==1){
			printf("Event : %s\n\n", "Arrival of New Customer");
			struct Customer* new_c = e->cust;
    		new_c->next = NULL;
    		teller_queue->push(new_c,teller_queue->head, teller_queue->length);
    		time = e->time;
		}else if(e->type == 2){
			struct Customer* nc = e->cust;
			time = e->time;
			printf("%s", "Removal of Customer \n");
			printf("-----------%s-----------\n", "STATS");
			printf("Arrived Time : %f min\n", nc->arrival_time);
			printf("Wait Time: %f min\n", nc->wait_time);
			printf("Service Time : %f min\n", nc->service_time);
			printf("Total time in bank : %f min\n", nc->total_time);
			printf("-----------%s-----------\n\n", "-----");
			free(nc);
			free(e);
		}else{
			struct Teller* tell = e->teller;
			printf("Checking Teller queue for customers\n");
			if(teller_queue->head->next == NULL){
				printf("Teller queue is empty\n" );
				struct Teller* tell = e->teller;
				float idle_time = 2* (75.0/60.0) * ((1.0*rand())/(1.0*RAND_MAX));
				printf("Adding Teller event with random idle time   %f\n\n",idle_time);
				e->time = e->time + idle_time ;
				e->next = NULL;
				e->type = 4;
				tell -> total_idle_time = (tell -> total_idle_time) + idle_time;
				event_queue->push(e,event_queue->head);
				time = e->time;

				total_amt_idle_time = total_amt_idle_time + idle_time;
			}else{
				printf("%s\n", "Serving customer\n");
				struct Customer* c1 = teller_queue->pop(teller_queue->head, teller_queue->length);
    			c1->wait_time = e->time - c1->arrival_time;
    			c1->service_time = 2*avgServiceTime* ((1.0*rand())/(1.0*(RAND_MAX)));;
    			c1->total_time = c1->wait_time + c1->service_time;
    			

    			struct Event* cust_event = (struct Event*) malloc (sizeof(struct Event));
    			cust_event->time = e->time + c1->service_time;
				cust_event->next = NULL;
				cust_event->type = 2;
				cust_event->cust = c1;
				cust_event->teller = NULL;
				event_queue->push(cust_event,event_queue->head);

    			e->time = cust_event->time ;
				e->next = NULL;
				e->type = 3;
				tell->total_service_time = tell->total_service_time + c1->service_time;

				event_queue->push(e,event_queue->head);
				no_customers--;
				time = e->time;

				total_no_cust_served++;
				avg_time_spent_in_bank = avg_time_spent_in_bank + c1->total_time;
				if (max_wait_time < c1->wait_time){
					max_wait_time = c1 -> wait_time;
				}

				total_amt_service_time = total_amt_service_time + c1->service_time;
			}
		}
	}
	printf("FINAL STATS - SINGLE QUEUE\n");
	printf("total_no_cust_served = %d;\n", total_no_cust_served);
	// printf("total_time_req_to_serve_all_cust = %f\n", total_time_req_to_serve_all_cust);
	printf("avg_time_spent_in_bank = %f\n", ( avg_time_spent_in_bank/total_no_cust_served ));
	printf("max_wait_time = %f\n", max_wait_time);
	printf("total_amt_service_time = %f\n", total_amt_service_time);
	printf("total_amt_idle_time = %f\n\n", total_amt_idle_time);

	FILE* plot = fopen("./src/graph.data","a");
	fprintf(plot, "%d\t%f\n", no_tellers, ( avg_time_spent_in_bank/total_no_cust_served ) );
	fclose(plot);
}

void multipleQueue(char* argv[]){
	printf("----------------------MULTIPLE QUEUE SIMULATION----------------------\n");

	float time = 0;

	struct Event* h;
	h = (struct Event*) malloc (sizeof(struct Event));
	h->time=-1;
	h->next =NULL;
	h->cust = NULL;
	h->teller = NULL;
	struct Eventq* event_queue;
	event_queue = (struct Eventq*) malloc (sizeof(struct Eventq));
	event_queue->head = h;
	event_queue->push = push_event;
	event_queue->pop = pop_event;


	int no_customers = atoi(argv[1]);
	int no_tellers = atoi(argv[2]);
	float simulationTime = atof(argv[3]);
	float avgServiceTime = atof(argv[4]);

	for(int i=0;i<no_tellers;i++){
		struct Event* e = (struct Event*) malloc (sizeof(struct Event));
		e->time =0 ;
		e->next = NULL;
		e->type = 3;

		struct Teller* t = (struct Teller*) malloc (sizeof(struct Teller));
		t->total_idle_time =0;
		t->total_service_time=0;
		t->next = NULL;
		t->line_no = i;

		e->teller = t;
		e->cust = NULL;
		event_queue->push(e, event_queue->head);
	}

	for(int i=0;i<no_customers;i++){
		float arrTime = simulationTime * ((1.0*rand())/(1.0*RAND_MAX));
		struct Event* e;
		e = (struct Event*) malloc (sizeof(struct Event));
		e->time = arrTime;
		e->type = 1;
		e->next = NULL;

		struct Customer* new_c = (struct Customer*) malloc (sizeof(struct Customer));
		new_c-> arrival_time = arrTime;
		new_c->wait_time = 0 ;
		new_c->service_time = 0;
		new_c->total_time = arrTime;
		new_c->next = NULL;

		e->cust = new_c;
		e->teller = NULL;

		event_queue->push(e, event_queue->head);
	}

	struct Tellerq* lines[no_tellers];
	for(int i=0;i<no_tellers;i++){
		lines[i] = (struct Tellerq*)malloc(sizeof(struct Tellerq));
		struct Customer* head_cust = (struct Customer*) malloc(sizeof(struct Customer));
		head_cust -> arrival_time = -1;
	    head_cust -> wait_time = -1;
	    head_cust -> service_time = -1;
	    head_cust -> total_time = -1;
	    head_cust -> next = NULL;
	    lines[i] -> head = head_cust;
	    lines[i] -> push = push_customer;
	    lines[i] -> pop = pop_customer;
	    lines[i] -> length = (int*)malloc(sizeof(int));
    	*lines[i] -> length = 0;
	}

	int total_no_cust_served = 0;
    float total_time_req_to_serve_all_cust = 0;
    // int type_of_system = 1;
    float avg_time_spent_in_bank=0;
    float max_wait_time = 0;
    float total_amt_service_time = 0;
    float total_amt_idle_time = 0;

	while(no_customers>0 && time<simulationTime){
		struct Event* e = event_queue -> pop(event_queue->head);
		printf("time : %f    ", e->time);
		if(e->type==1){
			printf("Event : %s\n\n", "Arrival of New Customer");
			struct Customer* new_c = e->cust;
    		new_c->next = NULL;
    			
    		//finding smallest queue
    		int min=no_customers;
    		int shortest_line;
    		int* arr = (int*) malloc (no_tellers*(sizeof(int)));
    		memset(arr,0,no_tellers*(sizeof(int)));
    		for(int i=0;i<no_tellers;i++){
    			if(*lines[i]->length < min){
    				min = *lines[i]->length;
    				shortest_line = i;
    			}
    		}
    		int p=0;
    		for(int i=0;i<no_tellers;i++){
    			if(*lines[i]->length == min){
    				arr[p] = i; p++;
    			}
    		}

    		int index= (rand() % p);
    		index = arr[index];
    		free(arr);

    		lines[index]->push(new_c,lines[index]->head, lines[index]->length);

    		time = e->time;
    	}
		else if(e->type == 2){
			struct Customer* nc = e->cust;
			time = e->time;
			printf("%s", "Removal of Customer \n");
			printf("-----------%s-----------\n", "STATS");
			printf("Arrived Time : %f min\n", nc->arrival_time);
			printf("Wait Time: %f min\n", nc->wait_time);
			printf("Service Time : %f min\n", nc->service_time);
			printf("Total time in bank : %f min\n", nc->total_time);
			printf("-----------%s-----------\n\n", "-----");
			free(nc);
			free(e);
		}
		else{
			struct Teller* tell = e->teller;
			int index  = tell->line_no;
			printf("Checking Teller queue for customers\n");
			if(lines[index]->head->next == NULL){
				printf("Teller queue is empty\n" );
				struct Teller* tell = e->teller;

				printf("Checking other Teller queues\n");
				int* arr = (int*) malloc (no_tellers*(sizeof(int)));
				int p=0;
				for(int i=0;i<no_tellers;i++){
					if(lines[i]->head->next !=NULL){
						arr[p] = i;p++;
					}
				}

				if(p!=0){
					printf("Serving Customer from Another line\n\n");
					int k1 = (rand() % p);
					k1 = arr[k1];

					struct Customer* c1 = lines[k1]->pop(lines[k1]->head, lines[k1]->length);
    				c1->wait_time = e->time - c1->arrival_time;
    				c1->service_time = 2.0*avgServiceTime* ((1.0*rand())/(1.0*(RAND_MAX)));;
    				c1->total_time = c1->wait_time + c1->service_time;

    				struct Event* cust_event = (struct Event*) malloc (sizeof(struct Event));
	    			cust_event->time = e->time + c1->service_time;
					cust_event->next = NULL;
					cust_event->type = 2;
					cust_event->cust = c1;
					cust_event->teller = NULL;
					event_queue->push(cust_event,event_queue->head);

					e->time = cust_event->time ;
					e->next = NULL;
					e->type = 3;
					tell->total_service_time = tell->total_service_time + c1->service_time;

					event_queue->push(e,event_queue->head);
					no_customers--;
					time = e->time;

					total_no_cust_served++;
					avg_time_spent_in_bank = avg_time_spent_in_bank + c1->total_time;
					if (max_wait_time < c1->wait_time){
						max_wait_time = c1 -> wait_time;
					}

					total_amt_service_time = total_amt_service_time + c1->service_time;

				}else{    //all queue are empty
					float idle_time = 2* (75.0/60.0) * ((1.0*rand())/(1.0*RAND_MAX));
					printf("Adding Teller event with random idle time   %f\n\n",idle_time);
					e->time = e->time + idle_time ;
					e->next = NULL;
					e->type = 4;
					tell -> total_idle_time = (tell -> total_idle_time) + idle_time;
					event_queue->push(e,event_queue->head);
					time = e->time;

					total_amt_idle_time = total_amt_idle_time + idle_time;
				}
				free(arr);
			}
			else{
				printf("%s\n", "Serving customer from own line\n");
				struct Customer* c1 = lines[index]->pop(lines[index]->head, lines[index]->length);
    			c1->wait_time = e->time - c1->arrival_time;
    			c1->service_time = 2*avgServiceTime* ((1.0*rand())/(1.0*(RAND_MAX)));;
    			c1->total_time = c1->wait_time + c1->service_time;
    			

    			struct Event* cust_event = (struct Event*) malloc (sizeof(struct Event));
    			cust_event->time = e->time + c1->service_time;
				cust_event->next = NULL;
				cust_event->type = 2;
				cust_event->cust = c1;
				cust_event->teller = NULL;
				event_queue->push(cust_event,event_queue->head);

    			e->time = cust_event->time ;
				e->next = NULL;
				e->type = 3;
				tell->total_service_time = tell->total_service_time + c1->service_time;

				event_queue->push(e,event_queue->head);
				no_customers--;
				time = e->time;

				total_no_cust_served++;
				avg_time_spent_in_bank = avg_time_spent_in_bank + c1->total_time;
				if (max_wait_time < c1->wait_time){
					max_wait_time = c1 -> wait_time;
				}

				total_amt_service_time = total_amt_service_time + c1->service_time;
			}
		}
	}    

	printf("FINAL STATS - MULTIPLE QUEUE\n");
	printf("total_no_cust_served = %d;\n", total_no_cust_served);
	// printf("total_time_req_to_serve_all_cust = %f\n", total_time_req_to_serve_all_cust);
	printf("avg_time_spent_in_bank = %f\n", ( avg_time_spent_in_bank/total_no_cust_served ));
	printf("max_wait_time = %f\n", max_wait_time);
	printf("total_amt_service_time = %f\n", total_amt_service_time);
	printf("total_amt_idle_time = %f\n", total_amt_idle_time);

}

int main(int argc, char *argv[]){
	singleQueue(argv);

	multipleQueue(argv);

	return 0;
}


