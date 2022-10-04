# Restaurant Management System
# Course
CS2006	Operating Systems <br />

# Instructions:
1. For the exact number of Customers, Cooks, and Waiters you can take input from the user. <br />
2. A manager has a Menu of items. (Menu contains dish name, price, and preparation time) <br />
3. The customer has customer_ID and customer_Name. <br />
4. Use pipe for communication between Customer, Manager and wherever this is applicable. <br />
5. In project report you can briefly justify your project implementation. <br />
6. 
# Description
A fast-food restaurant has a process of a Manager, Cook, Waiter, and Customer. At the start, the restaurant has one manager, cook, waiter and customer but you can create multiple Cooks and Waiters to handle more than one Customer using multithreading. When the customer will place its order you will calculate the total preparation time of all the dishes that the customer ordered and the customer will wait for that time. <br />

# Steps
Step-1: Customer will tell his name and ID to Manager and can ask him about Menu. <br />
Step-2: The manager will give the menu to the customer. <br />
Step-3: Customer will check menu and place order. <br />
Step-4: The manager will receive an order and keep these orders in the sequence. <br />
Step-5: The manager will calculate the bill of the customer and add into (Daily_Sales). <br />
Step-6: Now any available cook will get the order. <br />
Step-7: One order can have multiple dishes therefore multiple cooks can work for the preparation of one order to minimize preparation time. <br />
Step-8: Any available waiters will get prepared food and deliver to customers one waiter can deliver one order at a time. After delivery waiter will add order amount in (Daily_Waiter_Sales). <br />
Step-9: Customer will get his order give confirmation message and terminate. <br />
Step-10: At the end you have to compare Daily_Waiter_Sales with Daily_Sales and print their values. <br />
 
# Languages 
1. C++ Language

# Note
The Description.pdf contains the detailed description of this repository.
