#include<iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstring>
#include <vector>

#define MAX 1024

using namespace std;

struct Customer_Information
{
  char* name;
  int id;
};

struct Customer_Order_Information 
{
  char *name_of_dish;
  int price;
  int preparation_time;
};

struct Handler_Customer
{
  int id;
  sem_t * cust_semaphores;
  vector<Customer_Order_Information *> info;
};

struct Handler_Cook 
{
  int id;
  int availability;
};

struct Handler_Waiter 
{
  int id;
  int availability;
};

struct Handler_Array 
{
  char arr[MAX];
};

//Semaphores

sem_t sem;
sem_t semaphore_of_cook;
sem_t semaphore_of_waiter;
sem_t mutex;

//Pthread
pthread_t tid[4];

//Pipes
int first_fd[2];
int second_fd[2];
int third_fd[2];
int fourth_fd[2];
int fifth_fd[2];

//Iteration
int itr = 1;
int itr1 = 1;

//Declaring Variabes

int No_of_Customers = 0;
int No_of_Cooks = 0;
int No_of_Waiters = 0;



//Menu
char dishes[][16] = { "Burger", "Sandwitch", "Pasta", "Pizza", "Steak", "Nuggets", "HotDog", "Mexican Tacos" };
int prices[8] = {400, 450, 350, 1200, 700, 200, 500, 650};
int preparation_time[8] = {1, 2, 3, 4, 5, 6, 7, 8};
char menu[] = "\t Menu \n Option_To_Select Dish_Name Price Preparation Time \n Press 1 for Burger 400 1 \n Press 2 for Sandwitch 450 2 \n Press 3 for Pasta 350 3 \n Press 4 for Pizza 1200 4 \n Press 5 for Steak 700 5 \n Press 6 for Nuggets 200 6 \n Press 7 for HotDog 500 7 \n Press 8 for Mexican Tacos 650 8 \n Press -1 to terminate \n Enter your choice: ";

//Vector 
vector<Handler_Customer *> customer_of_semaphores;
vector<Handler_Cook *> cook_of_statuses;
vector<Handler_Waiter  *> waiter_of_statuses;
vector<int> queue;
vector<Handler_Array  *> names;

//Daily Sale Initization

int Daily_of_Sales = 0;
int Daily_of_Waiter_Sales = 0;

int Bill = 0;
bool End = false;

int orders_Served = 0;

void  Add_In_Daily_Sales(int val) 
{

    Daily_of_Sales = Daily_of_Sales + val;

    cout << "Manager Added Amount " << val << " in Daily Sales" << endl;

}


void * Customer(void * arg) 
{

  sem_wait(&mutex);

  Handler_Customer * handler_customer = (Handler_Customer *)arg;

  Customer_Information * customer_information = new Customer_Information();
  customer_information->id =  handler_customer ->id;
  customer_information->name = new char[20];
  cout << "Customer name: ";
  cin >> customer_information->name;

  char buffer[MAX];
  int size = 0;
  memset(buffer, 0, MAX);
  
  sprintf(buffer, "%d %s", customer_information->id, customer_information->name);
  write(second_fd[1], buffer, strlen(buffer));

  memset(buffer, 0, strlen(buffer));
  size = read(first_fd[0], buffer, MAX - 1);

  int order;
  
  cout << buffer;
  memset(buffer, 0, MAX);

  while(true)
  {
  
    cin >> order;

    if (order >= 1 && order <= 8) 
    {
      
      Customer_Order_Information * customer_information = new Customer_Order_Information();
      customer_information->name_of_dish = dishes[order- 1];
       customer_information->price = prices[order - 1];
      customer_information->preparation_time = preparation_time[order - 1];
      customer_information->price = prices[order - 1];

       handler_customer ->info.push_back(customer_information);
       
       cout << "Dish "<< customer_information->name_of_dish << " added in order" << endl;

    }
    
    


    else if (order == -1)
    {
      sprintf(buffer, "%d", customer_information->id);
      write(second_fd[1], buffer, strlen(buffer));
      cout << "Customer with Name: " << customer_information->name << " and ID: " << customer_information->id << " has ordered the following dishes" << endl;

      for (int iterate = 0; iterate <  handler_customer ->info.size(); iterate++) 
      {
         cout << iterate + 1 << ": " <<  handler_customer ->info[iterate]->name_of_dish << " " <<  handler_customer ->info[iterate]->price << " " <<  handler_customer ->info[iterate]->preparation_time << endl;
      }

      break;
    }

    else 
    {
      cout << "Dish name is invalid!!!" << endl;
    }

  }

  sem_post(&mutex);

  sem_wait(customer_of_semaphores[customer_information->id - 1]->cust_semaphores);

  cout << "Customer with Name: " << customer_information->name << " and ID: " << customer_information->id << " gives the received order confirmation" << endl;

  return NULL;


}

void * Manager(void * arg) 
{
  int orders_taken = 0;

  char buffer[MAX];
  int size = 0;
  
  //Customer Information
  int customer_id = 0;
  char * customer_name = NULL;

  while(orders_taken < No_of_Customers) 
  {

    memset(buffer, 0, MAX);

    read(second_fd[0], buffer, MAX - 1);

    customer_id = atoi(strtok(buffer, " "));
    customer_name = strtok(NULL, " ");

    write(first_fd[1], menu, strlen(menu));
    memset(buffer, 0, MAX);

    read(second_fd[0], buffer, MAX- 1);
    orders_taken++;

    queue.push_back(atoi(buffer));
    
  }

  
  for(int i = 0; i < queue.size(); i++) 
  {
    for(int j = 0; j < customer_of_semaphores.size(); j++) 
    {
       if(customer_of_semaphores[j]->id == queue[i]) 
       {

          for(int k = 0; k < customer_of_semaphores[j]->info.size(); k++) 
          {
              Add_In_Daily_Sales(customer_of_semaphores[j]->info[k]->price);

              int No_Cook = No_of_Cooks;

              while (No_Cook == No_of_Cooks)
              {
                No_Cook = 0;
                
                for(int order = 0; order < No_of_Cooks; order++) 
                {
                  if(cook_of_statuses[order]->availability)
                  {
                      break;
                  }
                  
                  else 
                  {
                  	No_Cook++;
                  }
                }
              }

              sem_post(&semaphore_of_cook);

              memset(buffer, 0, MAX);

              size = read(fourth_fd[0], buffer, MAX - 1);
              buffer[size] = '\0';

              memset(buffer, 0, MAX);

              sprintf(buffer, "%d %d", customer_of_semaphores[j]->id, k);

              write(third_fd[1], buffer, strlen(buffer));

          }

          break;
       }
    }

    int No_Cook = 0;
    
    //Waiting until all the dishes for the particular order is prepared
    while (No_Cook < No_of_Cooks)
    {
      No_Cook = 0;
      for(int kk = 0; kk < No_of_Cooks; kk++) 
      {
        if(cook_of_statuses[kk]->availability)
        {
            No_Cook++;
        }
      }
    }

    No_Cook = 0;

    while (No_Cook < No_of_Waiters)
    {
      No_Cook = 0;
      for(int kk = 0; kk < No_of_Waiters; kk++) 
      {
        if(waiter_of_statuses[kk])
        {
            No_Cook++;
        }
      }
    }

    sem_post(&semaphore_of_waiter);

    memset(buffer, 0, MAX);



    sprintf(buffer, "%d", customer_of_semaphores[i]->id);
    write(fifth_fd[1], buffer, strlen(buffer));

  }


  while(orders_Served < queue.size());

  int No_Cook = 0;
  while (No_Cook < No_of_Cooks)
  {
      No_Cook = 0;
      for(int kk = 0; kk < No_of_Cooks; kk++) 
      {
        if(cook_of_statuses[kk]->availability)
        {
            No_Cook++;
        }
      }
    }

    No_Cook = 0;

    while (No_Cook < No_of_Waiters)
    {
      No_Cook = 0;
      for(int order = 0; order < No_of_Waiters; order++) 
      {
        if(waiter_of_statuses[order]->availability)
        {
            No_Cook++;
        }
      }
    }

  End = true;
  for(int i = 0; i < No_of_Cooks; i++) 
  {
     sem_post(&semaphore_of_cook);
  }

  for(int i = 0; i < No_of_Waiters; i++) 
  {
     sem_post(&semaphore_of_waiter);
  }

  cout << "Daily Sales By Manager: " << Daily_of_Sales << endl;

  cout << "Daily Waiter Sales By Waiter: " << Daily_of_Waiter_Sales << endl;

   return NULL;
}

void * Waiter(void * arg)
{
  Handler_Waiter  * handler_waiter = (Handler_Waiter  *)arg;
  char buffer[MAX];
  int size = 0;
  while(1) 
  {
    waiter_of_statuses[handler_waiter->id]->availability = 1;
    sem_wait(&semaphore_of_waiter);

    if (End) 
    {
      break;
    }

    waiter_of_statuses[handler_waiter->id]->availability = 0;

    memset(buffer, 0, MAX);
    size = read(fifth_fd[0], buffer, MAX - 1);

    int customer_id = atoi(buffer);

    cout << "Waiter ID: " << handler_waiter->id << " is delivering the order to Customer ID: " << customer_id  << endl;

    int daily_waiter_sale = 0;
    for (int i = 0; i < customer_of_semaphores.size(); i++) 
    {
       if(customer_of_semaphores[i]->id == customer_id) 
       {
         for(int j = 0; j < customer_of_semaphores[i]->info.size(); j++) 
         {
            daily_waiter_sale = daily_waiter_sale + customer_of_semaphores[i]->info[j]->price;
         }

         Daily_of_Waiter_Sales = Daily_of_Waiter_Sales + daily_waiter_sale;

          cout << "Waiter Added Bill " << daily_waiter_sale << " in Daily Waiter Sales" << endl;

          break;
       }
    }


    sleep(1);

    orders_Served++;

    sem_post((customer_of_semaphores[customer_id - 1])->cust_semaphores);

  }

  return NULL;
}

void * Cook(void * arg)
{

  Handler_Cook * handler_cook = (Handler_Cook *)arg;
  char buffer[MAX];
  int size = 0;

  while(1) 
  {
    cook_of_statuses[handler_cook->id]->availability = 1;
    sem_wait(&semaphore_of_cook);

    if (End) 
    {
      break;
    }

    cook_of_statuses[handler_cook->id]->availability = 0;

    memset(buffer, 0, MAX);
    sprintf(buffer, "%d", handler_cook->id);
    write(fourth_fd[1], buffer, strlen(buffer));

    memset(buffer, 0, MAX);
    size = read(third_fd[0], buffer, MAX - 1);

    cout << "Cook ID: " << handler_cook->id << " Started Preparing Food" << endl;

    int test_id = atoi(strtok(buffer, " "));
    int order_no = atoi(strtok(NULL, " "));

    for(int i = 0; i < customer_of_semaphores.size(); i++)
    {
      if (customer_of_semaphores[i]->id == handler_cook->id)
      {
        sleep(customer_of_semaphores[i]->info[order_no]->preparation_time);
        break;
      }
    }

    cout << "Cook ID: " << handler_cook->id << " Done Preparing Food" << endl;

  }
  
  return NULL;
}


int main()
{
 
  
  // Taking Input
  cout << "Enter Number of Customers: ";
  cin >> No_of_Customers;
  
  cout << "Enter Number of Cooks: ";
  cin >> No_of_Cooks; 
  
  cout << "Enter Number of Waiters: ";
  cin  >> No_of_Waiters;

  cout << "Total Customers: " << No_of_Customers << endl;

 if (pipe(first_fd) < 0)
  {
      return -1;
  }
  
  else if (pipe(second_fd) < 0)
  {
     return -1;
  } 
  
  else if (pipe(third_fd) < 0)
  {
     return -1;
  } 
  
  else if (pipe(fourth_fd) < 0)
  {
     return -1;
  } 
  
  else if (pipe(fifth_fd) < 0)
  {
     return -1;
  } 
  
  pthread_t manager_tid;
  pthread_create(&manager_tid,NULL,Manager,NULL);


  sem_init(&semaphore_of_cook, 0, 0);

  pthread_t cook_tids;

  for(int i = 0; i < No_of_Cooks; i++) 
  {
    Handler_Cook * c_handler = new Handler_Cook ();
    c_handler->id = i;
    c_handler->availability = 1;
    cook_of_statuses.push_back(c_handler);
    pthread_create(&cook_tids, NULL, Cook, c_handler);
  }

  sem_init(&semaphore_of_waiter, 0, 0);

  pthread_t waiter_tids;
  
  for(int i = 0; i < No_of_Waiters; i++) 
  {
    Handler_Waiter * w_handler = new Handler_Waiter();
    w_handler->id = i;
    w_handler->availability = 1;
    waiter_of_statuses.push_back(w_handler);
    pthread_create(&waiter_tids, NULL, Waiter, w_handler);
  }
  
    pthread_t customer_tids;

  sem_init(&mutex, 0, 1);

  for(int i = 0; i < No_of_Customers; i++) 
  {
    Handler_Customer * c_handler = new Handler_Customer();
    c_handler->id = i + 1;
    c_handler->cust_semaphores = (sem_t *)malloc(sizeof(sem_t)); 
    sem_init(c_handler->cust_semaphores, 0, 0);
    customer_of_semaphores.push_back(c_handler);
    pthread_create(&customer_tids, NULL, Customer, c_handler);
  }

  pthread_exit(NULL);
}

