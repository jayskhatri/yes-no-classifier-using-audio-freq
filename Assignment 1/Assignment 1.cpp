// Assignment 1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<conio.h>
#include<cmath>

const double threshold = 10.0, offset = 30.0, limit = 5000.0;
const int noFrames = 100;

double* countAvgsOfAmbience(){
	FILE *ip;
	//avg[0]: energy, avg[1] = zcr
	double* avg = (double*) malloc(2*sizeof(double));
	avg[0] = avg[1] = 0;

	ip = fopen("ambience.txt", "r");
	if(ip == NULL) exit(1);
	double energy = 0, total_sample = 0, prev_x = 0, n=0, zcr = 0, pmax = 0, nmax = 0;
	char line[100];

	while(!feof(ip)){
		//reading line from the input file
		fgets(line, 100, ip);
		
		//if the read line's first character is digit then it is the observation
		if(!isalpha(line[0])){	
			//coverting whole line to integer
			int x = atoi(line);
			if(x>=0 && x > pmax){
				pmax = x;
			}else if(x < 0 && x < nmax){
				nmax = x;
			}
			//printf("%d\n", x);
			//for every 150 sample we will calculate the energy and zcr
			if(n==noFrames){
				total_sample++;
				//writing the values of average energy and zcr into the file
				avg[0] += energy/noFrames;
				avg[1] += zcr;
				//printf("energy: %f, zcr: %f\n", energy/100.0, zcr);
				n = 0;
				energy = 0;
				zcr = 0;

				// printf("avg energy: %f, avg_zcr: %f\n", avg[0]/total_sample, avg[1]/total_sample);
			}
			n++;
			energy += x*x;
			if(x >= 0 && prev_x<0) zcr++;
			if(x<=0 && prev_x>0) zcr++;
			prev_x = x;
		}
	}
    avg[0] /= total_sample;
    avg[1] /= total_sample;
    
	printf("total sample: %f, avg energy: %f, avg_zcr: %f, pmax: %f, nmax: %f\n", total_sample, avg[0], avg[1], pmax, nmax);

	fclose(ip);
	return avg;
}

double countDC(){
	FILE *ip;
	ip = fopen("dc_op.txt", "r");
	if(ip == NULL) exit(1);
	
	double total_sample = 0, DCShift = 0;
	char line[100];

	while(!feof(ip)){
		//reading line from the input file
		fgets(line, 100, ip);
		
		//if the read line's first character is digit then it is the observation
		if(!isalpha(line[0])){	
			//coverting whole line to integer
			int x = atoi(line);
			DCShift += x;
			total_sample++;
		}
	}
    DCShift /= total_sample;

	fclose(ip);
	//printf("dcshift: %f\n", DCShift);
	//getch();
	return DCShift;
}

void  processWord(int word_s, int word_e, double energy[], double zcr[]){
    int size = word_e - word_s + 1;
	double fricative_cnt = 0;
	printf("ZCR\n");
	for(int i = word_s; i<word_e; i++){
		printf("%d ", zcr[i]);
	}
	printf("\n");
	for(int i = word_s; i<word_e; i++){
		//printf("%d ", zcr[i]);
		if(zcr[i] > 10 && zcr[i+1] > 10)
			fricative_cnt++;
	}

	if(fricative_cnt >= size * 0.5)
		printf("\n---->Yes\n");
	else
		printf("\n---->No\n");
}

void init(double *ambience_avg){
	
    FILE *ip;
    ip = fopen("normalized_data.txt", "r");
	if(ip == NULL){
		printf("file could not be opened\n");
		exit(1);
	}
	char line[100];

    //reading of the energy and zcr value sample wise
    double en[10000], z[10000];

    /* flag: word_start for index of the first observation of the word
     * flag: word_end for index of the last observation of the word
     * lastIndex tracks the last index where the marker should be placed for word
    */
    long int word_start = -1, word_end = -1, lastIndex = 0, n=0;
    
    //helps for calculating zcr
    double prev_x = 0, energy = 0, zcr = 0, x;

    while(!feof(ip)){
        //reading line from the input file
        fgets(line, 100, ip);
        //if the read line's first character is digit then it is the observation
        if(!isalpha(line[0])){
            //coverting whole line to integer
            x = atoi(line);

			if(abs(x) > 2){
				
				if(n==noFrames){
					//printf("en: %f, zcr: %f\n", energy, zcr);
					energy /= noFrames;
					
					//printf("en: %f, zcr: %f\n", energy, zcr);
					en[lastIndex] = energy;
					z[lastIndex++] = zcr;
					n = 0;
					energy = 0;
					zcr = 0;
				}
				n++;
				energy += x*x;
				if(x >= 0 && prev_x<0) zcr++;
				if(x<=0 && prev_x>0) zcr++;
				prev_x = x;
			}
        }//end of if for digit
    } // while loop ends here

	fclose(ip);
	int flag = 0;
	long int i = 0;
	for( ; i<lastIndex-3; ++i){
		//printf("ambience[0]: %lf, x270: %lf, en[i]: %lf, en[i+1]: %lf, en[i+3]: %lf\n", ambience_avg[0], ambience_avg[0] * 270, en[i+1], en[i+2], en[i+3]);
		if(word_start == -1 && word_end == -1 && en[i+1] > ambience_avg[0] * 270 && en[i+2] > ambience_avg[0] * 270 && en[i+3] > ambience_avg[0] * 270){
			word_start = i;
		}
		else if(word_start != -1 && word_end == -1 && en[i+1] <= ambience_avg[0] * 270 && en[i+2] <= ambience_avg[0] * 270 && en[i+3] <= ambience_avg[0] * 270){
			word_end = i;
		}

		else if(word_start != -1 && word_end != -1){
			printf("total frames: %ld,frame Number: %ld, start : %ld, end: %ld, energy at start: %lf & end: %lf", lastIndex, i, word_start*noFrames, word_end*noFrames, en[word_start], en[word_end]);
			processWord(word_start, word_end, en, z);
			word_start = -1;
			word_end = -1;
		}
	}
}

void normalize_data(char* filename){
	double dcshift = countDC();
	//printf("dc: %f", dcshift);

    FILE *ip, *op;
	if(ip = fopen(filename, "r")){
		remove(filename);
		fclose(ip);
	}
    ip = fopen(filename, "r");
    op = fopen("normalized_data.txt", "a");
    if(ip == NULL || op == NULL){
        printf("file could not be opened\n");
        exit(1);
    }
    char line[100];
    int max = 0;
    while(!feof(ip)){
        //reading line from the input file
        fgets(line, 100, ip);
        //if the read line's first character is digit then it is the observation
        if(!isalpha(line[0])){
            //coverting whole line to integer
            int x = atoi(line);
            if(abs(x) > max){
                max = abs(x);
            }
        }//end of if for digit
    }

	//normalization factor
    double Nfactor = (max - limit)/(limit*1.0);//limit / max;

    rewind(ip);
    //normalizing the data
    while(!feof(ip)){
        //reading line from the input file
        fgets(line, 100, ip);
        //if the read line's first character is digit then it is the observation
        if(!isalpha(line[0])){
            //coverting whole line to integer
            int x = atoi(line);
            long int normalizedX = floor((x - dcshift) * Nfactor);
			if(abs(normalizedX) > 1)
				fprintf(op, "%ld\n", normalizedX);
        }//end of if for digit
    }
	printf("normalization done with normalization factor: %f, DC-Shift: %f \n", Nfactor, dcshift);
}

int _tmain(int argc, _TCHAR* argv[]){

    // we will find out the average energy and zcr of the ambience, ambience sound is recorded beforehand
    double* ambience_avg = countAvgsOfAmbience();
    printf("Ambience - avg energy: %f, avg_zcr: %f\n", ambience_avg[0], ambience_avg[1]);
    getch();
	//normalizing the data complete
	normalize_data("yes_no.txt");

    //core part of the program
    init(ambience_avg);

	printf("init ended\n");
	getch();
	return 0;
}

