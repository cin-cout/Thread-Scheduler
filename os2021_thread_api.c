#include "os2021_thread_api.h"

struct itimerval Signaltimer;
ucontext_t dispatch_context;
ucontext_t contextforback;
ucontext_t function_context;
ucontext_t TQ_context;
ucontext_t wait_context;

typedef struct node node;
node* nodenow;
struct node
{
    int tid;
    char *job;
    char *basepro;
    char *nowpro;
    int cancel;
    int becancel;
    int eveid;
    int waitornot;
    int q_time;
    int w_time;
    int needtowait;
    node* pre;
    node* next;
    ucontext_t nowcontext;
};

struct queue
{
    node* head;
    node* tail;
};
typedef struct queue queue;

queue* new(void)
{
    queue* q = malloc(sizeof(queue));
    q-> head = NULL;
    q -> tail = NULL;
    return q;
}

int isempty(queue* q)
{
    if(q->head == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

node* new_node(int t,char *j,char *bp,char *np,int c,int b,int e,int w,int qt,int wt,int ntw,ucontext_t nt)
{
    node* n = malloc(sizeof(node));
    n->tid = t;
    n -> job =j;
    n -> basepro = bp;
    n-> nowpro = np;
    n -> cancel = c;
    n -> becancel = b;
    n -> eveid = e;
    n ->waitornot = w;
    n ->q_time = qt;
    n ->w_time = wt;
    n ->needtowait = ntw;
    n-> pre = NULL;
    n -> next = NULL;
    n -> nowcontext = nt;
    return n;
}

void push(queue* q,node* n)
{

    if(q->head == NULL && q->tail == NULL)
    {
        q->head = n;
        q->tail = n;
        n ->pre = NULL;
        n->next = NULL;
    }

    else
    {
        q -> tail -> next = n;
        n -> pre = q->tail;
        q->tail = n;
        n -> next =NULL;
    }

}

node* front(queue* q)
{

    if(q->head == NULL)
    {
        return NULL;
    }

    return q->head;
}

int pop(queue* q)
{

    if(q->head == NULL)
    {
        return 0;
    }

    else if(q->head == q -> tail)
    {
        //free(q -> head);
        q->head = NULL;
        q->tail = NULL;
        return 1;
    }

    else
    {
        q->head = q-> head -> next;
        //free(q->head->pre);
        q->head->pre = NULL;
        return 1;
    }
}

queue* readyh;
queue* readyl;
queue* readym;
queue* waitingh;
queue* waitingm;
queue* waitingl;
queue* waitingt;
queue* terminate;
int id=0;
int TQ;
node* nodenow;


int OS2021_ThreadCreate(char *job_name, char *p_function, char *priority, int cancel_mode)
{

    if (strcmp(p_function,"Function1")==0)
    {
        CreateContext(&function_context,&dispatch_context, &Function1);
    }
    else if (strcmp(p_function,"Function2")==0)
    {
        CreateContext(&function_context,&dispatch_context, &Function2);
    }
    else if (strcmp(p_function,"Function3")==0)
    {
        CreateContext(&function_context,&dispatch_context, &Function3);
    }
    else if (strcmp(p_function,"Function4")==0)
    {
        CreateContext(&function_context,&dispatch_context, &Function4);
    }
    else if (strcmp(p_function,"Function5")==0)
    {
        CreateContext(&function_context,&dispatch_context, &Function5);
    }
    else if (strcmp(p_function,"ResourceReclaim")==0)
    {
        CreateContext(&function_context,&dispatch_context, &ResourceReclaim);
    }
    else
    {
        return -1;
    }

    node *n;
    char *np;
    np = malloc(2);
    if(strcmp(priority,"L")==0)
    {
        id++;
        strcpy(np,"L");
        n = new_node(id,job_name,priority,np, cancel_mode,0,-1,0,0,0,0,function_context);
        push(readyl,n);
        return id;
    }
    else if(strcmp(priority,"M")==0)
    {
        id++;
        strcpy(np,"M");
        n = new_node(id,job_name,priority,np, cancel_mode,0,-1,0,0,0,0,function_context);
        push(readym,n);
        return id;
    }
    else if(strcmp(priority,"H")==0)
    {
        id++;
        strcpy(np,"H");
        n = new_node(id,job_name,priority,np, cancel_mode,0,-1,0,0,0,0,function_context);
        push(readyh,n);
        return id;
    }
    free(np);
}

void OS2021_ThreadCancel(char *job_name)
{


    if(strcmp(nodenow->job,job_name)==0)
    {
        if(nodenow -> cancel == 0)
        {
            push(terminate,nodenow);
            printf("The thread %s has been canceled by %s.\n",nodenow->job,nodenow->job);
            setcontext(&dispatch_context);
        }
        else
        {
            printf("The thread %s wants to cancel %s.\n",nodenow->job,nodenow->job);
            nodenow -> becancel = 1;
        }
    }

    else
    {
        node *n;
        n = readyh -> head;
        while(n != NULL)
        {
            if(strcmp(n->job,job_name)==0)
            {
                if(n->cancel==0)
                {

                    if(n == readyh->head && n==readyh->tail)
                    {
                        readyh -> head = NULL;
                        readyh -> tail = NULL;
                        push(terminate,n);
                    }

                    else if(n == readyh->head)
                    {
                        readyh -> head = n-> next;
                        readyh -> head ->pre = NULL;
                        push(terminate,n);
                    }
                    else if(n == readyh-> tail)
                    {
                        readyh -> tail = n-> pre;
                        readyh -> tail -> next = NULL;
                        push(terminate,n);
                    }
                    else
                    {
                        n -> pre -> next = n-> next;
                        n -> next -> pre = n-> pre;
                        push(terminate,n);
                    }
                    printf("The thread %s has been canceled by %s.\n",n->job,nodenow->job);
                }
                else
                {
                    printf("The thread %s wants to cancel %s.\n",nodenow->job,n->job);
                    n -> becancel = 1;
                }
                return;
            }
            n = n->next;
        }


        n = readym -> head;
        while(n != NULL)
        {
            if(strcmp(n->job,job_name)==0)
            {
                if(n->cancel==0)
                {
                    if(n == readym->head && n==readym->tail)
                    {
                        readym -> head = NULL;
                        readym -> tail = NULL;
                        push(terminate,n);
                    }
                    else if(n == readym->head)
                    {
                        readym -> head = n-> next;
                        readym -> head ->pre = NULL;
                        push(terminate,n);
                    }
                    else if(n == readym-> tail)
                    {
                        readym -> tail = n-> pre;
                        readym -> tail -> next = NULL;
                        push(terminate,n);
                    }
                    else
                    {
                        n -> pre -> next = n-> next;
                        n -> next -> pre = n-> pre;
                        push(terminate,n);
                    }
                    printf("The thread %s has been canceled by %s.\n",n->job,nodenow->job);
                }
                else
                {
                    printf("The thread %s wants to cancel %s.\n",nodenow->job,n->job);
                    n -> becancel = 1;
                }
                return;
            }

            n = n->next;

        }

        n = readyl -> head;
        while(n != NULL)
        {
            if(strcmp(n->job,job_name)==0)
            {
                if(n->cancel==0)
                {
                    if(n == readyl->head && n==readyl->tail)
                    {
                        readyl -> head = NULL;
                        readyl -> tail = NULL;
                        push(terminate,n);
                    }
                    else if(n == readyl->head)
                    {
                        readyl -> head = n-> next;
                        readyl -> head ->pre = NULL;
                        push(terminate,n);
                    }
                    else if(n == readyl-> tail)
                    {
                        readyl -> tail = n-> pre;
                        readyl -> tail -> next = NULL;
                        push(terminate,n);
                    }
                    else
                    {
                        n -> pre -> next = n-> next;
                        n -> next -> pre = n-> pre;
                        push(terminate,n);
                    }
                    printf("The thread %s has been canceled by %s.\n",n->job,nodenow->job);
                }
                else
                {
                    printf("The thread %s wants to cancel %s.\n",nodenow->job,n->job);
                    n -> becancel = 1;
                }
                return;
            }

            n = n->next;

        }

        n = waitingh -> head;
        while(n != NULL)
        {
            if(strcmp(n->job,job_name)==0)
            {
                if(n->cancel==0)
                {
                    if(n == waitingh->head && n==waitingh->tail)
                    {
                        waitingh -> head = NULL;
                        waitingh -> tail = NULL;
                        push(terminate,n);
                    }
                    else if(n == waitingh->head)
                    {
                        waitingh -> head = n-> next;
                        waitingh -> head ->pre = NULL;
                        push(terminate,n);
                    }
                    else if(n == waitingh-> tail)
                    {
                        waitingh -> tail = n-> pre;
                        waitingh -> tail -> next = NULL;
                        push(terminate,n);
                    }
                    else
                    {
                        n -> pre -> next = n-> next;
                        n -> next -> pre = n-> pre;
                        push(terminate,n);
                    }
                    printf("The thread %s has been canceled by %s.\n",n->job,nodenow->job);
                }
                else
                {
                    printf("The thread %s wants to cancel %s.\n",nodenow->job,n->job);
                    n -> becancel = 1;
                }
                return;
            }

            n = n->next;
        }

        n = waitingm -> head;
        while(n != NULL)
        {
            if(strcmp(n->job,job_name)==0)
            {
                if(n->cancel==0)
                {
                    if(n == waitingm->head && n==waitingm->tail)
                    {
                        waitingm -> head = NULL;
                        waitingm -> tail = NULL;
                        push(terminate,n);
                    }
                    else if(n == waitingm->head)
                    {
                        waitingm -> head = n-> next;
                        waitingm -> head ->pre = NULL;
                        push(terminate,n);
                    }
                    else if(n == waitingm-> tail)
                    {
                        waitingm -> tail = n-> pre;
                        waitingm -> tail -> next = NULL;
                        push(terminate,n);
                    }
                    else
                    {
                        n -> pre -> next = n-> next;
                        n -> next -> pre = n-> pre;
                        push(terminate,n);
                    }
                    printf("The thread %s has been canceled by %s.\n",n->job,nodenow->job);
                }
                else
                {
                    printf("The thread %s wants to cancel %s.\n",nodenow->job,n->job);
                    n -> becancel = 1;
                }
                return;
            }

            n = n->next;
        }

        n = waitingl -> head;
        while(n != NULL)
        {
            if(strcmp(n->job,job_name)==0)
            {
                if(n->cancel==0)
                {
                    if(n == waitingl->head && n==waitingl->tail)
                    {
                        waitingl -> head = NULL;
                        waitingl -> tail = NULL;
                        push(terminate,n);
                    }
                    else if(n == waitingl->head)
                    {
                        waitingl -> head = n-> next;
                        waitingl -> head ->pre = NULL;
                        push(terminate,n);
                    }
                    else if(n == waitingl-> tail)
                    {
                        waitingl -> tail = n-> pre;
                        waitingl -> tail -> next = NULL;
                        push(terminate,n);
                    }
                    else
                    {
                        n -> pre -> next = n-> next;
                        n -> next -> pre = n-> pre;
                        push(terminate,n);
                    }
                    printf("The thread %s has been canceled by %s.\n",n->job,nodenow->job);
                }
                else
                {
                    printf("The thread %s wants to cancel %s.\n",nodenow->job,n->job);
                    n -> becancel = 1;
                }
                return;
            }

            n = n->next;
        }

    }
}

void OS2021_ThreadWaitEvent(int event_id)
{
    getcontext(&wait_context);
    if(nodenow->waitornot == 0)
    {
        nodenow ->eveid = event_id;
        nodenow ->nowcontext = wait_context;
        printf("%s wants to wait for event %d.\n",nodenow ->job,event_id);
        if(strcmp(nodenow -> nowpro,"H")==0)
        {
            push(waitingh,nodenow);
        }
        else if(strcmp(nodenow -> nowpro,"M")==0)
        {
            strcpy(nodenow ->nowpro,"H");
            printf("The priority of threads %s is changed from M to H.\n",nodenow->job);
            push(waitingm,nodenow);
        }
        else if(strcmp(nodenow -> nowpro,"L")==0)
        {
            strcpy(nodenow ->nowpro,"M");
            printf("The priority of threads %s is changed from L to M.\n",nodenow->job);
            push(waitingl,nodenow);
        }
        nodenow ->waitornot=1;
        setcontext(&dispatch_context);
    }
    else
    {
        nodenow -> waitornot = 0;
    }

}

void OS2021_ThreadSetEvent(int event_id)
{

    node *n;
    n = waitingh -> head;
    while(n != NULL)
    {
        if(n->eveid==event_id)
        {
            n->eveid = -1;
            if(n == waitingh->head && n==waitingh->tail)
            {
                waitingh -> head = NULL;
                waitingh -> tail = NULL;
                push(readyh,n);
            }
            else if(n == waitingh->head)
            {
                waitingh -> head = n-> next;
                waitingh -> head ->pre = NULL;
                push(readyh,n);
            }
            else if(n == waitingh-> tail)
            {
                waitingh -> tail = n-> pre;
                waitingh -> tail -> next = NULL;
                push(readyh,n);
            }
            else
            {
                n -> pre -> next = n-> next;
                n -> next -> pre = n-> pre;
                push(readyh,n);
            }

            printf("%s changes the staus of %s to READY.\n",nodenow->job,n->job);
            return;
        }
        n = n->next;
    }

    n = waitingm -> head;
    while(n != NULL)
    {
        if(n->eveid==event_id)
        {
            n->eveid = -1;
            if(n == waitingm->head && n==waitingm->tail)
            {
                waitingm -> head = NULL;
                waitingm -> tail = NULL;
                push(readyh,n);
            }
            else if(n == waitingm->head)
            {
                waitingm -> head = n-> next;
                waitingm -> head ->pre = NULL;
                push(readyh,n);
            }
            else if(n == waitingm-> tail)
            {
                waitingm -> tail = n-> pre;
                waitingm -> tail -> next = NULL;
                push(readyh,n);
            }
            else
            {
                n -> pre -> next = n-> next;
                n -> next -> pre = n-> pre;
                push(readyh,n);
            }
            printf("%s changes the staus of %s to READY.\n",nodenow->job,n->job);
            return;
        }
        n = n->next;
    }

    n = waitingl -> head;
    while(n != NULL)
    {
        if(n->eveid==event_id)
        {
            n->eveid = -1;
            if(n == waitingl->head && n==waitingl->tail)
            {
                waitingl -> head = NULL;
                waitingl -> tail = NULL;
                push(readym,n);
            }
            else if(n == waitingm->head)
            {
                waitingl -> head = n-> next;
                waitingl -> head ->pre = NULL;
                push(readym,n);
            }
            else if(n == waitingm-> tail)
            {
                waitingl -> tail = n-> pre;
                waitingl -> tail -> next = NULL;
                push(readym,n);
            }
            else
            {
                n -> pre -> next = n-> next;
                n -> next -> pre = n-> pre;
                push(readym,n);
            }
            printf("The priority of threads %s is changed from L to M.\n",n->job);
            printf("%s changes the staus of %s to READY.\n",nodenow->job,n->job);
            return;
        }
        n = n->next;
    }

}

void OS2021_ThreadWaitTime(int msec)
{
    getcontext(&wait_context);
    if(nodenow->waitornot == 0)
    {
        nodenow ->needtowait = msec;
        nodenow ->nowcontext = wait_context;
        if(strcmp(nodenow->nowpro,"M")==0)
        {
            strcpy(nodenow ->nowpro,"H");
            printf("The priority of threads %s is changed from M to H.\n",nodenow->job);
        }
        else if(strcmp(nodenow->nowpro,"L")==0)
        {
            strcpy(nodenow ->nowpro,"M");
            printf("The priority of threads %s is changed from L to M.\n",nodenow->job);
        }
        push(waitingt,nodenow);
        printf("%s wants to wait for %d msec.\n",nodenow ->job,msec);
        nodenow -> waitornot=1;
        setcontext(&dispatch_context);
    }
    else
    {
        nodenow -> waitornot = 0;
    }
}

void OS2021_DeallocateThreadResource()
{
    node* n;
    while(terminate -> head !=NULL)
    {
        n = terminate -> head -> next;
        printf("The memory space by %s has been released.\n",terminate -> head->job);
        free(terminate -> head);
        terminate -> head = n;
        if (terminate -> head == NULL)
        {
            terminate -> tail = NULL;
        }
    }

}

void OS2021_TestCancel()
{
    if(nodenow->cancel == 1)
    {
        if(nodenow ->becancel == 1)
        {
            printf("The thread %s get the safe point and has been canceled.\n",nodenow->job);
            push(terminate,nodenow);
            setcontext(&dispatch_context);
        }
    }
}

void CreateContext(ucontext_t *context, ucontext_t *next_context, void *func)
{
    getcontext(context);
    context->uc_stack.ss_sp = malloc(STACK_SIZE);
    context->uc_stack.ss_size = STACK_SIZE;
    context->uc_stack.ss_flags = 0;
    context->uc_link = next_context;
    makecontext(context,(void (*)(void))func,0);
}

void ResetTimer()
{
    Signaltimer.it_value.tv_sec = 0;
    Signaltimer.it_value.tv_usec = 10000;
    if(setitimer(ITIMER_REAL, &Signaltimer, NULL) < 0)
    {
        printf("ERROR SETTING TIME SIGALRM!\n");
    }
}

void Dispatcher()
{

    if(!isempty(readyh))
    {
        nodenow = front(readyh);
        pop(readyh);
        TQ = 100;
        //printf("%s\n",nodenow->job);
        setcontext(&(nodenow->nowcontext));
    }
    else if(!isempty(readym))
    {
        nodenow = front(readym);
        pop(readym);
        TQ = 200;
        //printf("%s\n",nodenow->job);
        setcontext(&(nodenow->nowcontext));
    }
    else if(!isempty(readyl))
    {
        nodenow = front(readyl);
        pop(readyl);
        TQ = 300;
        //printf("%s\n",nodenow->job);
        setcontext(&(nodenow->nowcontext));
    }

}

void signal_controlz()
{
    printf("\n*******************************************************************************\n");
    printf("TID\tName\t\tState\tB_Priority\tC_Priority\tQ_time\tW_time\n");
    if(strlen(nodenow->job)>5)
    {
        printf("%d\t%s\tRUNNING\t%s\t\t%s\t\t%d\t%d\n",nodenow->tid,nodenow->job,nodenow->basepro,nodenow->nowpro,nodenow->q_time,nodenow->w_time);
    }
    else
    {
        printf("%d\t%s\t\tRUNNING\t%s\t\t%s\t\t%d\t%d\n",nodenow->tid,nodenow->job,nodenow->basepro,nodenow->nowpro,nodenow->q_time,nodenow->w_time);
    }


    node *n;
    n = readyh -> head;
    while(n != NULL)
    {
        if(strlen(n->job)>5)
        {
            printf("%d\t%s\tREADY\t%s\t\t%s\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        else
        {
            printf("%d\t%s\t\tREADY\t%s\t\t%s\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        n = n->next;
    }
    n = readym->head;
    while(n != NULL)
    {

        if(strlen(n->job)>5)
        {
            printf("%d\t%s\tREADY\t%s\t%s\t\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        else
        {
            printf("%d\t%s\t\tREADY\t%s\t%s\t\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        n = n->next;
    }
    n = readyl->head;
    while(n != NULL)
    {

        if(strlen(n->job)>5)
        {
            printf("%d\t%s\tREADY\t%s\t\t%s\t\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        else
        {
            printf("%d\t%s\t\tREADY\t%s\t\t%s\t\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        n = n->next;
    }
    n = waitingh->head;
    while(n != NULL)
    {
        if(strlen(n->job)>5)
        {
            printf("%d\t%s\tWAITING\t%s\t\t%s\t\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        else
        {
            printf("%d\t%s\t\tWAITING\t%s\t\t%s\t\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        n = n->next;
    }
    n = waitingm->head;
    while(n != NULL)
    {

        if(strlen(n->job)>5)
        {
            printf("%d\t%s\tWAITING\t%s\t\t%s\t\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        else
        {
            printf("%d\t%s\t\tWAITING\t%s\t\t%s\t\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        n = n->next;
    }
    n = waitingl->head;
    while(n != NULL)
    {

        if(strlen(n->job)>5)
        {
            printf("%d\t%s\tWAITING\t%s\t\t%s\t\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        else
        {
            printf("%d\t%s\t\tWAITING\t%s\t\t%s\t\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        n = n->next;
    }
    n = waitingt->head;
    while(n != NULL)
    {

        if(strlen(n->job)>5)
        {
            printf("%d\t%s\tWAITING\t%s\t\t%s\t\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        else
        {
            printf("%d\t%s\t\tWAITING\t%s\t\t%s\t\t%d\t%d\n",n->tid,n->job,n->basepro,n->nowpro,n->q_time,n->w_time);
        }
        n = n->next;
    }
    printf("*******************************************************************************\n");
}

void signal_time()
{

    TQ -= 10;


    node *n;
    n = readyh -> head;
    while(n != NULL)
    {

        n ->q_time += 1;
        n = n->next;
    }
    n = readym->head;
    while(n != NULL)
    {

        n ->q_time += 1;
        n = n->next;
    }
    n = readyl->head;
    while(n != NULL)
    {

        n ->q_time += 1;
        n = n->next;
    }
    n = waitingh->head;
    while(n != NULL)
    {
        n ->w_time += 1;
        n = n->next;
    }
    n = waitingm->head;
    while(n != NULL)
    {

        n ->w_time += 1;
        n = n->next;
    }
    n = waitingl->head;
    while(n != NULL)
    {

        n ->w_time += 1;
        n = n->next;
    }
    n = waitingt->head;
    while(n != NULL)
    {

        n ->w_time += 1;
        n->needtowait -= 1;
        if(n -> needtowait <= 0)
        {

            if(n == waitingt->head && n==waitingt->tail)
            {
                waitingt -> head = NULL;
                waitingt -> tail = NULL;
            }
            else if(n == waitingt->head)
            {
                waitingt -> head = n-> next;
                waitingt -> head ->pre = NULL;
            }
            else if(n == waitingt-> tail)
            {
                waitingt -> tail = n-> pre;
                waitingt -> tail -> next = NULL;
            }
            else
            {
                n -> pre -> next = n-> next;
                n -> next -> pre = n-> pre;
            }
            if(strcmp(n->nowpro,"H")==0)
            {
                printf("%s changes the its staus to READY.\n",n->job);
                push(readyh,n);
            }
            else if(strcmp(n->nowpro,"M")==0)
            {
                printf("%s changes the its staus to READY.\n",n->job);
                push(readym,n);
            }
        }
        n = n->next;
    }



    getcontext(&TQ_context);
    if(TQ == 0 && strcmp(nodenow->nowpro,"H")==0)
    {
        strcpy(nodenow->nowpro,"M");
        nodenow ->nowcontext = TQ_context;
        push(readym,nodenow);
        printf("The priority of threads %s is changed from H to M.\n",nodenow->job);
        setcontext(&dispatch_context);
    }
    else if(TQ == 0 && strcmp(nodenow->nowpro,"M")==0)
    {
        strcpy(nodenow->nowpro,"L");
        nodenow ->nowcontext = TQ_context;
        push(readyl,nodenow);
        printf("The priority of threads %s is changed from M to L.\n",nodenow->job);
        setcontext(&dispatch_context);
    }
    else if(TQ == 0 && strcmp(nodenow->nowpro,"L")==0)
    {
        nodenow ->nowcontext = TQ_context;
        push(readyl,nodenow);
        setcontext(&dispatch_context);
    }

}

void setqueue()
{

    readyh = new();
    readyl = new();
    readym = new();
    waitingh = new();
    waitingm = new();
    waitingl = new();
    terminate = new();
    waitingt = new();

}


void StartSchedulingSimulation()
{
    /*Set Timer*/
    Signaltimer.it_interval.tv_usec = 10000;
    Signaltimer.it_interval.tv_sec = 0;
    ResetTimer();
    signal(SIGTSTP,signal_controlz);
    signal(SIGALRM,signal_time);
    /*Create Context*/
    CreateContext(&dispatch_context,NULL, &Dispatcher);
    setcontext(&dispatch_context);
}
