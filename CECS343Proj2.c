#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

int capacity = 0, occupied = 0;
bool question_asking = false, answer_answering = false;
pthread_mutex_t question, answer, office, prof;
pthread_cond_t not_full_cv, not_empty_cv, empty_cv, full_cv;

question_start() {
	pthread_mutex_lock(&question);
	while (answer_answering == true) {
		pthread_cond_wait(&empty_cv, &question); 
	}
	pthread_mutex_unlock(&question); 
	question_asking = true;
	pthread_cond_signal(&full_cv); 
}

question_done() {
	question_asking = false;
}

answer_start() {
	printf("Professor start answering");
	pthread_mutex_lock(&answer);
	while (question_asking == true) {
		pthread_cond_wait(&full_cv, &answer);
	}
	pthread_mutex_unlock(&answer);
	answer_answering = true;
	pthread_cond_signal(&empty_cv);
}

answer_done() {
	answer_answering = false;
	printf("Professor finished answer");
}

enter_office() {
	pthread_mutex_lock(&office);
	while (occupied >= capacity) {
		pthread_cond_wait(&not_full_cv, &office);
	}
	occupied++;
	pthread_cond_signal(&not_empty_cv);
	pthread_mutex_unlock(&office);
}

leave_office() {
	pthread_mutex_lock(&office);
	occupied--;
	pthread_cond_signal(&not_full_cv);
	pthread_mutex_unlock(&office);
}



void* student(void *id) {
	int Stu_id = *(int *)id;
	int number_questions = (Stu_id % 4) + 1;
	enter_office();
	printf("Student %d entered the office", Stu_id);
	for(int i = 0; i < number_questions; i++) {
		question_start();
		printf("Student %d asked question %d", Stu_id, i);
		question_done();
		printf("Student %d is satisfied", Stu_id);
	}
	leave_office();
	printf("Student %d left the office", Stu_id);
}
void* professor() {

	pthread_mutex_lock(&prof);
	
	while (occupied <= 0){
		pthread_cond_wait(&not_empty_cv, &prof);
	}
	
	pthread_mutex_unlock(&prof);
	
	while (1 == 1){
		answer_start();
		answer_done();
	}
}

int main (int argc, char *argv[]) {
	int student_count = 1;
	int capacity_count = 1;
	printf("How many students plan to ask questions?\n");
	scanf("%d", &student_count);
	printf("What is the capacity of the office?\n");
	scanf("%d", &capacity_count);
	if(student_count > 0 & capacity_count > 0){ 
		printf("Got in if");
		pthread_t thread[student_count + 1];
		printf("created threads");
		int i;
		printf("initialized i");

		//Creates mutex and conditions needed for synchronization
		pthread_mutex_init(&question, NULL);
		pthread_mutex_init(&answer, NULL);
		pthread_mutex_init(&office, NULL);
		pthread_mutex_init(&prof, NULL);
		pthread_cond_init(&not_empty_cv, NULL);
		pthread_cond_init(&not_full_cv, NULL);
		pthread_cond_init(&empty_cv, NULL);
		pthread_cond_init(&full_cv, NULL);
		printf("initialized mutex and cond");

		//Creates threads, runs their functions, and joins them together after forking.
		pthread_create(&thread[student_count], NULL, &professor, NULL);
		printf("made prof");
		for(i = 0; i < student_count; i++) pthread_create(&thread[i], NULL, &student, (void *)(intptr_t) i);
		printf("made stu");
		for(i = 0; i < student_count + 1; i++) pthread_join(thread[i], NULL);

		//Destroys mutex and barrier initialized outside of main.
		pthread_mutex_destroy(&question);
		pthread_mutex_destroy(&answer);
		pthread_mutex_destroy(&office);
		pthread_mutex_destroy(&prof);
		pthread_cond_destroy(&not_empty_cv);
		pthread_cond_destroy(&not_full_cv);
		pthread_cond_destroy(&empty_cv);
		pthread_cond_destroy(&full_cv);
	}
	//Prints for invalid input
	else {
		printf("Invalid Input\n");
	}
	return 0;
} 







