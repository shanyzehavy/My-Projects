#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ABOVE_GRADE 101
#define PASS_GRADE 55
#define MAT_SIZE 10

/**
 * @brief swap between two variables
 * @param argc 2 int * arguments
 * @param argv *xp and *yp are two pointers for two different variables
 * @return void function
 * @note
 */

void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

/**
 * @brief sort a given array (using bubble algo)
 * @param argc 1 array and one integers, meaning 2 arguments total
 * @param argv array arr[] and an integer n (arr's size)
 * @return void function
 * @note
 */

void bubbleSort(int arr[], int n)
{
    int i, j;
    for (i = 0; i < n-1; i++)

    // Last i elements are already in place
    for (j = 0; j < n-i-1; j++)
        if (arr[j] > arr[j+1])
            swap(&arr[j], &arr[j+1]);
}


/**
 * @brief calculate the course statistics and print it into a 
 	file called course_statistics.txt
 * @param argc 2 arguments given
 * @param argv each argument is a path to a file
 * @return return 0, in case of errors return 1
 * @notes
 */

int main(int argc, char *argv[])
{
	char c;
	int num_stud=0, grade=0, sum = 0, max = 0, pass_grade = 0, median = 0;
	int min = ABOVE_GRADE;
	double pass_rate = 0,avg = 0;

	int hist[ABOVE_GRADE] = {0};

	FILE *fp_g = fopen (argv[1] , "r");
	if(fp_g==NULL) {	//In case the program wasn't able to open grades file:
		fprintf(stderr,"Can not open grades.txt file\n");
		return 1;
	
	}
	
	FILE *fp_st = fopen (argv[2] , "w");
		if(fp_st==NULL) {	//In case the program wasn't able to open
							//course_statistics file:
		fprintf(stderr,"Can not open course_statistics.txt file\n");
		fclose(fp_g);
		return 1;
	
	}

	//check for answers from the forum???
	
	//Count number of students:
	c=fgetc(fp_g);
	while (c!=EOF) {
		if(c=='\n') {
			num_stud++;
		}
		c=fgetc(fp_g);

		
	}

	fseek(fp_g, 0, SEEK_SET); //Return pointer back to the start of the file

	//Allocate an array for grades and sort it:
	int *grades_arr = malloc(num_stud*sizeof(int));
	if(grades_arr == NULL) {
		fprintf(stderr,"Dynamic allocation error\n");
	    fclose(fp_g);
		fclose(fp_st);
		return 1;
	}




	//Calculate sum of grades, creates a histogram, find max and min:
	for(int i =1; i < num_stud+1; i++){
		fscanf(fp_g, "%d", &grade);
		sum += grade;
		hist[grade]++;
		grades_arr[i-1]=grade;
		if (grade > max) {
			max =  grade;
		}
		if (min > grade) {
			min = grade;
		}
		if (grade >= PASS_GRADE){
			pass_grade++;
		}
	}

	bubbleSort(grades_arr, num_stud);
	//Calculate the statistics:
	median = grades_arr[((num_stud+1)/2)-1];
	avg = (double)sum/num_stud;
	pass_rate = ((double)pass_grade/num_stud)*100;
	

	//These commands insert the statistics into the statistics file:
	fprintf(fp_st, "num of students = %d\n" , num_stud);
	fprintf(fp_st, "avg = %.3f\n" , avg);
	fprintf(fp_st, "the median is - %d\n" , median);
	fprintf(fp_st, "max grade = %d, min grade = %d\n" , max, min);
	fprintf(fp_st, "pass rate = %.2f" , pass_rate);
	fprintf(fp_st, "%%");
	fprintf(fp_st, "\n");
	
	//Insrt the histogram matrix into the statistics file:
	for(int i = 0; i < MAT_SIZE; i++) {
		for(int j = 0; j < MAT_SIZE; j++) {
			if(j==(MAT_SIZE-1)) { //Avoid printing last space in line:
				fprintf( fp_st,"%d" , hist[i*10+j+1]);
			}
			else {
				fprintf( fp_st,"%d " , hist[i*10+j+1]);
			}
		}

		fprintf(fp_st, "\n");
	}




	//Close all files and free all allocated memory:
	fclose(fp_g);
	fclose(fp_st);
	free(grades_arr);





	return 0;
}
