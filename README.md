# Create-Database-using-C-language-and-Shared-Memory

I created a database using shared memory and C language for an assignment.

Technologies used:  C | shm | pthreads | mutex locks | deadlocks | Process Management | Memory Management

Task that is given to me is as follows,
Code is explaned in the ExplainedCode.pdf under Task 2

Develop a program which implements an application which contains two parties - HR and
accountants. The two entities should access a shared location to view, upload, download
and delete case studies.

A file called database is there and the information inside the database file looks like this.

		Name 	Attendance 	Salary 	per day 	Total salary
		Emp 	1000 				20 	    1500 		  	30000
		Emp 	1005 				25 	    1800 		   	45000	
		Emp 	1010 				23    	1300 			  29900
		
HR should be able to perform the following tasks

	➢ Add new employees to the database
	➢ Edit / Delete employees
	➢ Add / Edit / Delete attendance of employees
	➢ Add / Edit / Delete employees’ salary
	➢ View total salary of employees
	
The accountants should be able to perform the following tasks
	➢ View employees
	➢ View attendance of employees
	➢ View employee’s salary per day
	➢ Calculate the total salary of employees
	➢ Add / Edit / Delete employees’ total salary
	Total salary = attendance X salary per day
	
Total salary can be only calculated by the accountants.

There should not be any interruptions for any process that takes place. (When HR
people editing the file, accountants can’t perform any action to the file). There
should be only one file (database) and you can’t create 2 or more files. Use mutex
to lock and unlock the file.

Deliverables
Write a C program to cater to the above-mentioned requirements.	

