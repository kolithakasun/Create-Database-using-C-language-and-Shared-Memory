#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <pthread.h>
#define SHM_SIZE 4096

struct Emp{
	int EmpID; //to identify, its equal to the shared meomry identifier
	char Name[20];
	int Attendance;
	double salperday;
	double Total;
	int x; //if this is 0: shm empty, 1:shm under HR, 2: shm is filled
};

//Declaration of funtion
void contHR();
void contAcc();
void hrMenu();
void accMenu();
void Menu();
/* Global Variable */
FILE *fp;
FILE *fp1;
int t;
pthread_t thread;
pthread_mutex_t mutex;
//Shared Memory Variables
int shm_id; /* this variable is used to hold the returned segment identifier. */
key_t key;
char* shm_addr;
int* Emp_num; //Shared Memory Employee Unique Identifier, Assume Their is only 10 employees
struct Emp* employees; //Struct Object
int NoEmp;

//Create and Allocate Shared Memory
void create_sm(){
	
	key = ftok("shmfile",2331); //generate unique key
	
	//allocate a shared memory segment
	shm_id = shmget(key,SHM_SIZE, IPC_CREAT | 0766); 
	if (shm_id == -1){
		perror("shmget: ");
		exit(1);
	}
	
	
/* attach the given shared memory segment, at some free position */
/* that will be allocated by the system.   */
	shm_addr = shmat(shm_id,NULL, 0);
	if (!shm_addr){
		perror("shmat: ");
		exit(1);
	}
	else
			printf("Shared memory created. Shm ID: %d. Size: %d\n", shm_id, SHM_SIZE);

/* create a string index on the shared memory segment. */	
	Emp_num = (int*) shm_addr;
	*Emp_num = 0;
	employees = (struct Emp*) ((void*)shm_addr+sizeof(int)); //map employees object to shared memccpy
	
	
}

//Functions for threads
//Wadipura ekak scan wenwa balnna
void *readDB(void *vargp){
	pthread_mutex_lock(&mutex); //mutex locked
	fp = fopen("data2.dat","r");
	if(fp == NULL)
	{
		printf("\nCannot open Database");
		exit(0);
	}
	char name[20];
	int i,max;
	printf("Reading Database\n");
	(*Emp_num) = -1;

	for(i = 0; i <= 9; i++){

		if(i < 10){
			(*Emp_num)++;
			fscanf(fp,"%d %s%d%lf%lf", &employees[i].EmpID, employees[i].Name, &employees[i].Attendance, &employees[i].salperday, &employees[i].Total);
			
		}
	}
	printf("Read %d Employee Details\n\n", (*Emp_num) + 1);

	fclose(fp);
	
	pthread_mutex_unlock(&mutex); //mutex lock unlock
}

void *DisplayDB(void *vargp){
	pthread_mutex_lock(&mutex); //mutex lock on
	int i;
	printf("\nEmpID\tName\t    Attendance\tSal Per Day\tTotal\n");
	for(i=0; i <= (*Emp_num); i++){
		printf("%d\t%s\t%d\t%.2f\t\t%.2f\n",employees[i].EmpID, employees[i].Name, employees[i].Attendance, employees[i].salperday, employees[i].Total);
	}
	
	puts("");
	puts("");
	pthread_mutex_unlock(&mutex); //mutex lock unlocked
}

void *AddNewEmp(void *vargp){
	pthread_mutex_lock(&mutex); //mutex locked
	
	(*Emp_num)++; //create new pointer in SM for new employee
	
	printf("Employee ID Geneated: %d\n", (*Emp_num)+1);
	employees[(*Emp_num)].EmpID = (*Emp_num) + 1;  //Employee ID is going 1 number ahead than pointer number
	
	printf("Enter Employee Name: ");
	scanf(" %15s", &employees[(*Emp_num)].Name);
	printf("Enter Attendance: ");
	scanf("%d", &employees[(*Emp_num)].Attendance);
	printf("Enter Employee Salary Per Day: ");
	scanf("%lf", &employees[(*Emp_num)].salperday);
	
	printf("\n\nNew Employee Added to the Database\n");
	printf("%d\t%s\t%d\t%.2f\n",employees[(*Emp_num)].EmpID, employees[(*Emp_num)].Name, employees[(*Emp_num)].Attendance, employees[(*Emp_num)].salperday);
	
	pthread_mutex_unlock(&mutex); //mutex lock unlocked
	
}

void *EditEmp(void *vargp){
	
	pthread_mutex_lock(&mutex); //mutex locked
	int x;
	
	printf("Editing Existing Emp.\nEnter Employee ID: ");
	scanf("%d", &x);
	x--;
	printf("Enter Employee Name: ");
	scanf(" %s", &employees[x].Name);
	printf("Enter Attendance: ");
	scanf("%d", &employees[x].Attendance);
	printf("Enter Employee Salary Per Day: ");
	scanf("%lf", &employees[x].salperday);
	
	pthread_mutex_unlock(&mutex); //mutex unlocked
}

void *removeEmp(void *vargp){
	pthread_mutex_lock(&mutex); //mutex locked
	int x,i;
	printf("Enter Employee ID: ");
	scanf("%d", &x);
	i = x - 1;
	for(; x<=(*Emp_num); x++){
		
		employees[i].EmpID = employees[x].EmpID;
		strcpy(employees[i].Name,employees[x].Name);
		employees[i].Attendance = employees[x].Attendance;
		employees[i].salperday = employees[x].salperday;
		employees[i].Total = employees[x].Total;
		i++;
	}
	(*Emp_num)--;
	pthread_mutex_unlock(&mutex); //mutex unlocked
	
}

void *editEMpSal(void *vargp){
	
	pthread_mutex_lock(&mutex); //mutex locked
	int x;
	printf("Enter Employee ID: ");
	scanf("%d", &x);
	x--;
	printf("Employee: %s 's Salary: %.2f\n",employees[x].Name,employees[x].salperday);
	printf("Enter New Salary: ");
	scanf("%lf", &employees[x].salperday);
	pthread_mutex_unlock(&mutex); //mutex unlocked
}

void *editEmpAttend(void *vargp){
	pthread_mutex_lock(&mutex); //mutex locked
	int x;
	printf("Enter Employee ID: ");
	scanf("%d", &x);
	x--;
	printf("Employee: %s 's Attendance: %d\n",employees[x].Name,employees[x].Attendance);
	printf("Enter New Attendance: ");
	scanf("%d", &employees[x].Attendance);
	pthread_mutex_unlock(&mutex); //mutex unlocked
}

void *viewTotalSal(void *vargp){
	
	pthread_mutex_lock(&mutex); //mutex locked
	int i;
	for(i = 0; i <= ((*Emp_num)); i++){
		printf("%s\t%.2f\n",employees[i].Name,employees[i].Total);
	}
	pthread_mutex_unlock(&mutex); //mutex unlocked
}

void *viewAttendance(void *vargp){
	pthread_mutex_lock(&mutex); //mutex locked
	int i;
	for(i = 0; i <= ((*Emp_num)); i++){
		printf("%s\t%d\n",employees[i].Name,employees[i].Attendance);
	}
	pthread_mutex_unlock(&mutex); //mutex unlocked
}

void *viewEmployee(void *vargp){
	pthread_mutex_lock(&mutex); //mutex locked
	int i;
	for(i = 0; i <= ((*Emp_num)); i++){
		printf("%d\t%s\n",employees[i].EmpID,employees[i].Name);
	}
	pthread_mutex_unlock(&mutex); //mutex unlocked
}

void *viewSalDay(void *vargp){
	pthread_mutex_lock(&mutex); //mutex locked
	int i;
	for(i = 0; i <= ((*Emp_num)); i++){
		printf("%s\t%.2f\n",employees[i].Name,employees[i].salperday);
	}
	pthread_mutex_unlock(&mutex); //mutex unlocked
}

void *calculateTotal(void *vargp){
	
	pthread_mutex_lock(&mutex); //mutex locked
	int x;
	
	for(x=0; x <= ((*Emp_num)); x++){
		employees[x].Total = employees[x].Attendance * employees[x].salperday;
	}
	pthread_mutex_unlock(&mutex); //mutex unlocked
}

void *editTotal(void *vargp){
	
	pthread_mutex_lock(&mutex); //mutex locked
	int x;
	printf("Enter Employee ID: ");
	scanf("%d", &x);
	x--;
	printf("Employee: %s 's Total Salary: %.2f\n",employees[x].Name,employees[x].Total);
	printf("Enter New Total Value: ");
	scanf("%lf", &employees[x].Total);
	pthread_mutex_unlock(&mutex); //mutex unlocked
}

void *Exit(void *vargp){
	pthread_mutex_lock(&mutex); //mutex locked
	
	fp1 = fopen("data2.dat","w");
	int i;
	if(fp1 == NULL)
	{
		printf("cannot open File to Write");
		exit(1);
	}
	printf("Writing to Database\n");
	for(i=0; i <=(*Emp_num); i++){
		fprintf(fp1,"%d\t%s\t%d\t%f\t%f\n", employees[i].EmpID, employees[i].Name, employees[i].Attendance, employees[i].salperday, employees[i].Total);
	}
	printf("Writing Completed\n");
	fclose(fp1);
	shmctl(shm_id, IPC_RMID, NULL); // deleting SM
	exit(0);
	
	pthread_mutex_unlock(&mutex);
}

void hrMenu(){
	if(employees[0].x == 0)
	{
		pthread_create(&thread, NULL, readDB, NULL);
		pthread_join(thread, NULL);
		employees[0].x = 2; //Now shm is not empty
	}
	employees[0].x = 1; //to identify that shm is used by HR
	
	
	//rename("data2.dat","TmpData.dat"); //Renaming the Databse file so no other process can execute because their is no data.dat file at moment
	
	int x;
	pthread_t thread[10];
	int t[10];
	printf("*******************\n");
	printf("***** HR MENU *****\n");
	printf("*******************\n\n");
	printf("1. Add New Employee\n");
	printf("2. Edit Employee\n");
	printf("3. Add / Edit / Delete attendance of employees\n");
	printf("4. Add / Edit / Delete employees’ salary\n");
	printf("5. View total salary of employees\n");
	printf("6. Delete employees\n");
	printf("7. View Database\n");
	printf("99. Back\n");
	printf("0. exit\n");
	printf("\nSelect service: ");
	scanf("%d", &x);

	if(x == 1){
		t[x] = pthread_create(&thread[x], NULL, AddNewEmp, NULL);
		pthread_join(thread[x], NULL);
		contHR();
	}
	else if (x == 2){
		t[x] = pthread_create(&thread[x], NULL, EditEmp, NULL);
		pthread_join(thread[x], NULL);
		contHR();
	}
	else if (x == 3){
		t[x] = pthread_create(&thread[x], NULL, editEmpAttend, NULL);
		pthread_join(thread[x], NULL);
		contHR();
	}
	else if (x == 4){
		t[x] = pthread_create(&thread[x], NULL, editEMpSal, NULL);
		pthread_join(thread[x], NULL);
		contHR();
	}
	else if (x == 5){
		t[x] = pthread_create(&thread[x], NULL, viewTotalSal, NULL);
		pthread_join(thread[x], NULL);
		contHR();
	}
	else if (x == 6){
		t[x] = pthread_create(&thread[x], NULL, removeEmp, NULL);
		pthread_join(thread[x], NULL);
		contHR();
	}
	else if (x == 7){
		t[x] = pthread_create(&thread[x], NULL, DisplayDB, NULL);
		pthread_join(thread[x], NULL);
		contHR();
	}
	else if (x == 99){
		//rename("TmpData.dat","data2.dat");
		pthread_mutex_lock(&mutex);
		employees[0].x = 2; //to identify that shm is finished used by HR, but shm is not empty
		pthread_mutex_unlock(&mutex);
		system("clear");
		Menu();
	}
	else if (x == 0){
		//rename("TmpData.dat","data2.dat");
		employees[0].x = 2;
		t[x] = pthread_create(&thread[x], NULL, Exit, NULL);
		pthread_join(thread[x], NULL);
	}
	else{
		printf("Invalid Choise. Select Again\n");
		contHR();
	}

}

void Menu() {
	
	
	int x;
	pthread_t thread;
	//system("clear");
	printf("****************\n");
	printf("***** MENU *****\n");
	printf("****************\n\n");
	printf("1. HR\n");
	printf("2. Accountant\n");
	printf("0. exit\n");
	printf("\nSelect User Type: ");
	scanf("%d", &x);

	if(x == 1){
		system("clear");
		hrMenu();
	}
	else if (x == 2){
		system("clear");
		accMenu();
	}
	else if (x == 0){
		pthread_create(&thread, NULL, Exit, NULL);
	}
	else{
		printf("Invalid Choise. Select Again");
		Menu();
	}
}

void accMenu(){
	
	if(employees[0].x == 1){
		char x;
		printf("*********************************");
		printf("\nCannot open Database:\nDatabase is under Developing\n");
		printf("*********************************");
		
		printf("\n Do you want to Go Back And Wait(Y/n): ");
		scanf(" %c",&x);
		if(x == 'y' || x == 'Y'){
			system("clear");
			Menu();
		}
		else{
			exit(0);
		}
	}else if(employees[0].x == 0){
		pthread_create(&thread, NULL, readDB, NULL);
		pthread_join(thread, NULL);
	}
	
	int x;
	pthread_t thread[5];
	int t[5];
	printf("***************************\n");
	printf("***** Accountant MENU *****\n");
	printf("***************************\n\n");
	printf("1. View Employee\n");
	printf("2. View attendance of Employee\n");
	printf("3. View Employee Salary Per Day\n");
	printf("4. Calculate Total Salary\n");
	printf("5. Edit Total Salary\n");
	printf("99. Back\n");
	printf("0. exit\n");
	printf("\nSelect service: ");
	scanf("%d", &x);
	
	if (x == 1){
		t[x] = pthread_create(&thread[x], NULL, viewEmployee, NULL);
		pthread_join(thread[x], NULL);
		contAcc();
	}
	else if (x == 2){
		t[x] = pthread_create(&thread[x], NULL, viewAttendance, NULL);
		pthread_join(thread[x], NULL);
		contAcc();
	}
	else if (x == 3){
		t[x] = pthread_create(&thread[x], NULL, viewSalDay, NULL);
		pthread_join(thread[x], NULL);
		contAcc();
	}
	else if (x == 4){
		t[x] = pthread_create(&thread[x], NULL, calculateTotal, NULL);
		pthread_join(thread[x], NULL);
		contAcc();
	}
	else if (x == 5){
		t[x] = pthread_create(&thread[x], NULL, editTotal, NULL);
		pthread_join(thread[x], NULL);
		contAcc();
	}
	else if (x == 99){
		system("clear");
		Menu();
	}
	else if (x == 0){
		t[x] = pthread_create(&thread[x], NULL, Exit, NULL);
		pthread_join(thread[x], NULL);
	}
	else{
		printf("Invalid Choise. Select Again\n");
		contAcc();
	}
}

void contHR(){
	char x;
	printf("\n Do you want to contiue(Y/n): ");
	scanf(" %c",&x);
	if(x == 'y' || x == 'Y'){
		system("clear");
		hrMenu();
	}
	else{
		t = pthread_create(&thread, NULL, Exit, NULL);
		pthread_join(thread, NULL);
	}
}


void contAcc(){
	char x;
	printf("\n Do you want to contiue(Y/n): ");
	scanf(" %c",&x);
	if(x == 'y' || x == 'Y'){
		system("clear");
		accMenu();
	}
	else{
		t = pthread_create(&thread, NULL, Exit, NULL);
		pthread_join(thread, NULL);
	}
}


int main(){
	
	system("clear");
	create_sm();
	employees[0].x =0; //to identify that shm is empty
	
	Menu();
	
	shmctl(shm_id, IPC_RMID, NULL);
	
	
	
	return 0;
}