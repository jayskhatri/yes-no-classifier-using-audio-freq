// assn1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<conio.h>
#include<cmath>

//constants
const int noFrames = 100, thrsld_multiplier = 5;
const double limit = 5000.0;

//global variables
long int max;
double dcShift, nFactor;

double getDCShift(){

    long int sample_count = 0;
    FILE *fp;
    char line[80];

    fp = fopen("dc_op.txt", "r");
    
    if(fp == NULL){
        printf("File not found\n");
        exit(1);
    }
    
    while(!feof(fp)){
        fgets(line, 80, fp);
        dcShift += atof(line);
        sample_count++;
    }
    dcShift /= sample_count;
    
    fclose(fp);
}

void setupGlobal(char* filename){
    FILE *fp;
    long int totalSample = 0;
    char line[100];

    fp = fopen(filename, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }

    //get max value
    max = 0;
    while(!feof(fp)){
        fgets(line, 100, fp);
        if(!isalpha(line[0])){
            totalSample++;
            if(max < abs(atoi(line)))
                max = abs(atoi(line));
        }
    }
    
    nFactor = (double)limit/max;
    getDCShift();
    fclose(fp);
}

void normalize_data(char* inputFileName, char* outputFileName){

    FILE *fp, *op;
    long int totalSample = 0;
    char line[100];

    fp = fopen(inputFileName, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }

    //remove("normalized_data.txt");
    remove(outputFileName);
    op = fopen(outputFileName, "w");
    if(op == NULL){
        printf("Error opening file\n");
        exit(1);
    }

    while(!feof(fp)){
        fgets(line, 100, fp);

        if(!isalpha(line[0])){
            int x = atoi(line);
            double normalizedX = floor((x-dcShift)*nFactor);
            if(abs(normalizedX) > 1)
                fprintf(op, "%d\n", (int)normalizedX);
        }
    }
    printf("normalization complete with following properties\n");
    printf("max: %ld, nFactor: %lf, dcsShift: %lf\n", max, nFactor, dcShift);
    fclose(fp);
    fclose(op); 
}

double* countAvgAmbience(char* inputFileName){
    FILE *fp, *op;
    long int total_frame = 0;
    int n = 0;
    char line[100];
    double energy = 0.0, zcr = 0.0, x, prev_x = 0.0;

    fp = fopen(inputFileName, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }
    double *avg = (double*)malloc(sizeof(double)*2);
    avg[0] = 0.0;
    avg[1] = 0.0;
    while(!feof(fp)){
        fgets(line, 100, fp);
        if(!isalpha(line[0])){
            x = atof(line);
            if(n == noFrames){
                total_frame++;
                avg[0] += energy/noFrames;
                avg[1] += zcr;

                energy = 0.0;
                zcr = 0.0;
                n = 0;
            }
            n++;
            energy += x*x;
            if((x >= 0 && prev_x<0) || (x<=0 && prev_x>0)) zcr++;
            prev_x = x;
        }
    }
    avg[0] /= total_frame;
    avg[1] /= total_frame;

    fclose(fp);
    return avg;
}

void processWord(long int word_s, long int word_e, double energy[], double zcr[]){
    int size = word_e - word_s + 1;
	//if(size < 6) return;
    int fctv_cnt = 0;
    for(int i=word_s; i<word_e; i++){
        if(zcr[i] > 10 && zcr[i+1] > 10){
            fctv_cnt++;
        }
    }

    printf("\nfricative cnt: %d, size: %d\ns", fctv_cnt, size);
    if(fctv_cnt >= size * 0.2)
		printf("---->Yes\n");
	else
		printf("---->No\n");
}

void word_seggregation(double* ambAvg, char* inputFileName){
    FILE *ip;
	char line[100];
    //reading of the energy and zcr value sample wise
    double energy[10000], zcr[10000];
    /* flag: word_start for index of the first observation of the word
     * flag: word_end for index of the last observation of the word
     * lastIndex tracks the last index where the marker should be placed for word
    */
    long int word_start = -1, word_end = -1, lastIndex = 0, n=0, total_frame = 0;
    //helps for calculating zcr
    double prev_x = 0, en = 0, z = 0, x;

    ip = fopen(inputFileName, "r");
	if(ip == NULL){
		printf("file could not be opened\n");
		exit(1);
	}

    while(!feof(ip)){
        fgets(line, 100, ip);
        if(!isalpha(line[0])){
            x = atof(line);
            if(abs(x) > 1){
                if(n == noFrames){
                    en /= noFrames;
                    total_frame++;
                    energy[lastIndex] = en;
                    zcr[lastIndex++] = z;
					//printf("energy: %lf, zcr: %lf\n", energy[lastIndex], energy[lastIndex-1]);
                    en = 0.0; 
                    z = 0.0; 
                    n = 0;
                }
                n++;
                en += x*x;
                if((x >= 0 && prev_x<0) || (x<=0 && prev_x>0)) z++;
                prev_x = x;
            }
        }
    }

    fclose(ip);
	long int i = 0;
	printf("total frames: %ld", lastIndex);
    for( ; i<lastIndex-3; ++i){
		//printf("ambience[0]: %lf, x270: %lf, en[i]: %lf, en[i+1]: %lf, en[i+3]: %lf\n", ambience_avg[0], ambience_avg[0] * 270, en[i+1], en[i+2], en[i+3]);
		if(word_start == -1 && word_end == -1 && energy[i+1] > ambAvg[0] * thrsld_multiplier && energy[i+2] > ambAvg[0] * thrsld_multiplier && energy[i+3] > ambAvg[0] * thrsld_multiplier){
			//printf("start: i: %ld, ambience[0]: %lf, x300: %lf, en[i+1]: %lf, en[i+2]: %lf, en[i+3]: %lf\n", i, ambAvg[0], ambAvg[0] * thrsld_multiplier, energy[i+1], energy[i+2], energy[i+3]);
			word_start = i;
		}
		else if(word_start != -1 && word_end == -1 && energy[i+1] <= ambAvg[0] * thrsld_multiplier && energy[i+2] <= ambAvg[0] * thrsld_multiplier && energy[i+3] <= ambAvg[0] * thrsld_multiplier){
			printf("\nend: i: %ld, ambience[0]: %lf, x multplr %lf, en[i+1]: %lf, en[i+2]: %lf, en[i+3]: %lf\n", i, ambAvg[0], ambAvg[0] * thrsld_multiplier, energy[i+1], energy[i+2], energy[i+3]);
			word_end = i;
			printf("start : %ld, end: %ld, energy at start: %lf & end: %lf\nword_start: %ld, word_end: %ld", word_start*noFrames, word_end*noFrames, energy[word_start], energy[word_end], word_start, word_end);
			processWord(word_start, word_end, energy, zcr);
			word_start = -1;
			word_end = -1;
		}

		//else if(word_start != -1 && word_end != -1){
			
		//}
	}
	printf("\nEnergy\n");
	for(int i=0; i<lastIndex; i++){
	 	printf("%d: %lf\n", i, energy[i]);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	setupGlobal("yes_no_n.txt");
    normalize_data("yes_no_n.txt", "normalized_yes_no.txt");
    normalize_data("n_ambience.txt", "normalized_ambience.txt");
    double* ambience_avg = countAvgAmbience("normalized_ambience.txt");
	printf("ambience avg energy: %lf, avg zcr: %lf\n", ambience_avg[0], ambience_avg[1]);
    word_seggregation(ambience_avg, "normalized_yes_no.txt");
	getch();
	return 0;
}

