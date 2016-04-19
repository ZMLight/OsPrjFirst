#include <BurgerBuddiesPro.h>
#define max_num 250

sem_t burger, rack, order, cashier_ready;   //define the semaphore we need
int Cooker_Num[max_num], Cashier_Num[max_num], Customer_Num[max_num];

void *Cooker(void *argv)
{
  while(1)
  {
    sem_wait(&rack);
    //cook a burger
    sleep(1 + rand() % 4);
    sem_post(&burger);
    printf("Cook[%d] make a burger.\n", *(int *)argv);
  }
  return NULL;
}

void *Cashier(void *argv)
{
  while(1)
  {
    sem_wait(&order);
    printf("Casher[%d] accepts an order.\n", *(int *)argv);
    sem_wait(&burger);
    sem_post(&rack);
    printf("Casher[%d] take a burger to customer.\n", *(int *)argv);
    sem_post(&cashier_ready);
  }
  return NULL;
}

void *Customer(void *argv)
{
  sleep(1 + rand() % 5);
  printf("Customer[%d] come.\n", *(int *)argv);
  sem_post(&order);
  wait(&cashier_ready);
  return NULL;
}


int main(int argc,char *argv[])
{
  int cookerNum, cashierNum, customerNum, rackSize,i;
  pthread_t CookerThread[max_num], CashierThread[max_num], CustomerThread[max_num];
  srand(time(NULL));

  cookerNum = atoi(argv[1]);
  cashierNum = atoi(argv[2]);
  customerNum = atoi(argv[3]);
  rackSize = atoi(argv[4]);

  printf("Cooks [%d], Cashiers [%d], Customers [%d]\n",cookerNum,cashierNum,customerNum);
  printf("Begin run.\n");

  sem_init(&burger,0,0);
  sem_init(&order,0,0);
  sem_init(&rack,0,rackSize);
  sem_init(&cashier_ready,0,cashierNum);

  for(i = 0;i < cookerNum;i++)
  {
    Cooker_Num[i] = i + 1;
    pthread_create(&CookerThread[i], NULL, Cooker, &Cooker_Num[i]);
  }
  for(i = 0;i < cashierNum;i++)
  {
    Cashier_Num[i] = i + 1;
    pthread_create(&CashierThread[i], NULL, Cashier, &Cashier_Num[i]);
  }
  for(i = 0;i < customerNum;i++)
  {
    Customer_Num[i] = i + 1;
    pthread_create(&CustomerThread[i], NULL, Customer, &Customer_Num[i]);
  }

  for(i = 0;i < cookerNum;i++) pthread_join(CookerThread[i], NULL);
  for(i = 0;i < cashierNum;i++) pthread_join(CashierThread[i], NULL);
  for(i = 0;i < customerNum;i++) pthread_join(CustomerThread[i], NULL);


  return 0;
}
