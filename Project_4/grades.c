// C file for grades

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "linked-list.h"
#include "grades.h"
#define FAIL 1
#define FAIL_AVG -1
#define MAX_GRADE 100
#define MIN_GRADE 0

/**
 * @brief Initializes the "grades" data-structure.
 * @returns A pointer to the data-structure, of NULL in case of an error
 */
struct grades* grades_init();

/**
 * @brief Destroys "grades", de-allocate all memory!
 */
void grades_destroy(struct grades *grades);

/**
 * @brief Adds a student with "name" and "id" to "grades"
 * @returns 0 on success
 * @note Failes if "grades" is invalid, or a student with
 * the same "id" already exists in "grades"
 */
int grades_add_student(struct grades *grades, const char *name, int id);

/**
 * @brief Adds a course with "name" and "grade" to the student with "id"
 * @return 0 on success
 * @note Failes if "grades" is invalid, if a student with "id" does not exist
 * in "grades", if the student already has a course with "name", or if "grade"
 * is not between 0 to 100.
 */
int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade);

/**
 * @brief Calcs the average of the student with "id" in "grades".
 * @param[out] out This method sets the variable pointed by "out" to the
 * student's name. Needs to allocate memory. The user is responsible for
 * freeing the memory.
 * @returns The average, or -1 on error
 * @note Fails if "grades" is invalid, or if a student with "id" does not exist
 * in "grades".
 * @note If the student has no courses, the average is 0.
 * @note On error, sets "out" to NULL.
 */
float grades_calc_avg(struct grades *grades, int id, char **out);

/**
 * @brief Prints the courses of the student with "id" in the following format:
 * STUDENT-NAME STUDENT-ID: COURSE-1-NAME COURSE-1-GRADE, [...]
 * @returns 0 on success
 * @note Fails if "grades" is invalid, or if a student with "id" does not exist
 * in "grades".
 * @note The courses should be printed according to the order
 * in which they were inserted into "grades"
 */
int grades_print_student(struct grades *grades, int id);

/**
 * @brief Prints all students in "grade", in the following format:
 * STUDENT-1-NAME STUDENT-1-ID: COURSE-1-NAME COURSE-1-GRADE, [...]
 * STUDENT-2-NAME STUDENT-2-ID: COURSE-1-NAME COURSE-1-GRADE, [...]
 * @returns 0 on success
 * @note Fails if "grades" is invalid
 * @note The students should be printed according to the order
 * in which they were inserted into "grades"
 * @note The courses should be printed according to the order
 * in which they were inserted into "grades"
 */
int grades_print_all(struct grades *grades);

/* User function, clones “student” to “output”. Returns 0 on success. */

static int student_clone_t (void *student, void **output);

/* User function, destroys “student”. Always succeed */

static void student_destroy_t (void *student);

/* User function, clones “course” to “output”. Returns 0 on success. */

static int course_clone_t (void *course, void **output);

/* User function, destroys “course”. Always succeed */
static void course_destroy_t (void *course);



/*
 * @brief Searches for a student with a given id
 * @param argv grades is a pointer to a grades structure
 * @param argc 2 input arguments
 * @returns NULL if such student was not found, iterator otherwise
 */
static struct iterator* find_id (struct list *grades, int id);

/*
 * @brief Searches for a course with a given name
 * @param argv student_courses is a pointer to a courses structure
 * @param argc 2 input arguments
 * @returns NULL if such course was not found, iterator otherwise
 */
static struct iterator* find_course (struct list *student_courses, 
				 const char *course_name);

// Our code starts here:

struct grades {

	struct list *grades;

};
typedef struct grades grades_t;

struct student {

	char *name;
	int id;
	struct list *student_courses;

};
typedef struct student student_t;

struct course {

	char *course_name;
	int grade;

};
typedef struct course course_t;



static int student_clone_t (void *element, void **output) {

	// Used for accessing the void element parameter:
	student_t *param_stud = (student_t*)element; 
	// Used for copying into output argument:
	student_t *clone_stud = (student_t*)malloc(sizeof(student_t));
	if(clone_stud==NULL) {
		 return FAIL;
	}
	clone_stud->id = param_stud->id;
	
	//Create a list of courses for this student:
	clone_stud->student_courses=list_init ((element_clone_t)course_clone_t,
								(element_destroy_t)course_destroy_t ); 
	clone_stud->name=malloc(sizeof(char)*(strlen(param_stud->name)+1));
	if(clone_stud->name==NULL) {
		free(clone_stud);
		return FAIL;
	}
	strcpy(clone_stud->name,param_stud->name);
	*output=(void**)clone_stud;	
	return 0;
		
}


static void student_destroy_t (void *element) {
	// Used for accessing the void element parameter:
	student_t *param_stud = (student_t*)element;
	list_destroy(param_stud->student_courses);
	free(param_stud->name); // Free student's name from memory
	free(param_stud);



}

static int course_clone_t (void *element, void **output) {

	// Used for accessing the void element parameter:
	course_t *param_course = (course_t*)element;
	// Used for copying into output argument:
	course_t *clone_course = (course_t*)malloc(sizeof(course_t));
	if(clone_course==NULL) {
		 return FAIL;
	}
	clone_course->grade = param_course->grade;
	clone_course->course_name=malloc(sizeof(char)*
							  (strlen(param_course->course_name)+1));
	if(clone_course->course_name==NULL) {
		free(clone_course);
		return FAIL;
	}
	strcpy(clone_course->course_name,param_course->course_name);
	*output=(void**)clone_course;
	return 0;
}

static void course_destroy_t (void *element) {

	// Used for accessing the void element parameter:
	course_t *param_course = (course_t*)element; 
	free(param_course->course_name); // Free course's name
	free(param_course);

}


struct grades* grades_init() {

	grades_t *grades= (grades_t*)malloc(sizeof(grades_t));
	if(grades == NULL) {
		return FAIL;
	}
	// Init a linked list named "grades" with struct student elements as nodes:
	grades->grades = list_init ((element_clone_t)student_clone_t,
			  (element_destroy_t)student_destroy_t);
	return grades;
}

void grades_destroy(struct grades *grades) {

	list_destroy (grades->grades);
	free(grades);
}

int grades_add_student (struct grades *grades, const char *name, int id){
	if(grades==NULL) {
		return FAIL;
	}
	//Look for a student with similar ID
	struct iterator *itr = find_id(grades->grades, id);
	if(itr != NULL){ // Fail in case a similar ID was found
		 return FAIL;
	}
	// Create a struct student element: 
	student_t *stud=(student_t*)malloc(sizeof(student_t)); // Temporary use
	if(stud == NULL) {
		return FAIL;
	}
	int name_length = strlen(name)+1;
	stud->name=malloc(sizeof(char)*name_length); // Allocate memory for name of 
	                                             // student with
								 			   	 // unknown length
	if((stud->name)==NULL) {
		 return FAIL;
	}
	strcpy(stud->name,name);
	stud->id=id;
	stud->student_courses=list_init ((element_clone_t)course_clone_t,
							(element_destroy_t)course_destroy_t ); 
	list_push_back(grades->grades,(void*)stud); // Push this student as an
												// element to "grades" list	
	student_destroy_t((void*)stud);
	return 0;

}


struct iterator* find_id (struct list *grades, int id) {

	if(grades==NULL) {
		return FAIL;
	}
	struct iterator *itr = list_begin (grades);
	while (itr != NULL){
		student_t *stud = (student_t*)list_get(itr);
		if(stud->id == id){
			// In case a student with same id was found:
			return itr; 
		}
		itr=list_next(itr);
	}
	return NULL; // NULL in case no similar student ID was found
}

struct iterator* find_course (struct list *student_courses, 
							  const char *course_name){

	if(student_courses==NULL) {
		return FAIL;
	}
	struct iterator *itr = list_begin (student_courses);
	while (itr != NULL){
		course_t *course_tmp = (course_t*)list_get(itr);
		if(strcmp(course_tmp->course_name,course_name)==0){
			// In case a course with same name was found:
			return itr;
		}
		itr=list_next(itr);
	}
	return NULL; // NULL in case no similar course name was found
}



int grades_add_grade(struct grades *grades,  const char *name, int id,
                     int grade) {
	//	Check given arguments validation:
	if(grades==NULL) {
		return FAIL;
	}
	if(grade>MAX_GRADE || grade<MIN_GRADE){ //Check grade validation 
		return FAIL;
	}
	struct iterator *itr =find_id(grades->grades, id);
	if(itr == NULL){
		return FAIL; // No student with similar ID was found
	}
	struct iterator *itr_c = find_course(((student_t*)list_get(itr))
										 ->student_courses, name);
	if(itr_c != NULL){ 
		return FAIL; // Fail in case a similar name course was found
	}
	course_t *course_tmp=(course_t*)malloc(sizeof(course_t));
	if(course_tmp == NULL) {
		return FAIL;
	}
	int name_length = strlen(name)+1;
	// Allocate memory for course's name
	course_tmp->course_name=malloc(sizeof(char)*name_length);
	if((course_tmp->course_name)==NULL) {
		return FAIL; 
	}
	strcpy(course_tmp->course_name, name);
	course_tmp->grade=grade;
	// Push the grade and course name into the linked list "grade":
	list_push_back(((student_t*)list_get(itr))->student_courses,
									     (void*)course_tmp); 
	course_destroy_t((void*)course_tmp);
	return 0;

 }



float grades_calc_avg(struct grades *grades, int id, char **out){
	if(grades==NULL) {
		*out=NULL;
		return FAIL_AVG;
	}
	// Search for student with the given ID:
	struct iterator *itr_s = find_id(grades->grades, id); 
	if(itr_s == NULL){ 
		*out=NULL;
		return FAIL_AVG;
	}
	// In case such student was found:
	student_t *stud=(student_t*)list_get(itr_s);
	// Memory allocation for "output" string name:
	*out=(char*)malloc(sizeof(char)*(strlen(stud->name)+1)); 
	if(*out == NULL) {
		return FAIL_AVG;
	}
	strcpy(*out,stud->name);
	float sum=0, avg=0;
	float num_of_courses= (float)list_size(stud->student_courses);
	if(num_of_courses==0) {
		return 0; // In case student has no courses in list - avg is 0
	}
	// This iterator is used for going over student's courses:
	struct iterator *itr_c=list_begin(stud->student_courses); 
	while(itr_c!=NULL){
		sum+=((course_t*)(list_get(itr_c)))->grade;
		itr_c=list_next(itr_c);
	}
	avg=(sum/num_of_courses);
	return avg;

}

int grades_print_student(struct grades *grades, int id){
	if(grades==NULL) {
		return FAIL;
	}
	struct iterator *itr_s = find_id(grades->grades, id); // Used for search
	if(itr_s == NULL){ 
		 return FAIL;
	}
	// Print name and id:
	printf("%s %d:",((student_t*)list_get(itr_s))->name,
		   ((student_t*)list_get(itr_s))->id);
	// Printing list of courses ang grades:
	struct iterator *itr_c=list_begin(((student_t*)list_get(itr_s))
									  ->student_courses);
	if(itr_c==NULL) { // In case student has no courses in list - enter line
		printf("\n"); 
		return 0; 
	}
	course_t *tmp = (course_t*)(list_get(itr_c));
	// First print of student's courses:
	printf(" %s %d" , tmp->course_name, tmp->grade); 
	itr_c=list_next(itr_c);
	// Continue print of student's courses (used for print settings):
	while(itr_c!=NULL) {
		tmp = (course_t*)(list_get(itr_c));
		printf(", %s %d" , tmp->course_name, tmp->grade);
		itr_c=list_next(itr_c);		
	}
	printf("\n");
	return 0; // Succes
}

int grades_print_all(struct grades *grades){

	if(grades==NULL) {
		return FAIL;
	}
	struct iterator *itr=list_begin(grades->grades);
	// Go over all student in "grades" list:
	while(itr!=NULL) {
		student_t *std = (student_t*)(list_get(itr)); 
		grades_print_student(grades,std->id); // Print each student
		itr=list_next(itr);
	}
	return 0;
}


int main(){

	return 0;
}
