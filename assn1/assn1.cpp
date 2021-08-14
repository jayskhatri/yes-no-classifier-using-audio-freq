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
//threshold multiplier value is taken after tuning
const int noFrames = 150, thrsld_multiplier = 5;
const double limit = 5000.0;

//global variables
long int max;
double dcShift, nFactor;

//function to get dcshift value and set in global variable
double getDCShift(){

    long int sample_count = 0;
    FILE *fp;
    char line[80];

    //reading dc_shift.txt file
    fp = fopen("dc_shift.txt", "r");
    
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

//function to setup the global variable like, max and nFactor
//max and nFactor depends on the main yes/no word file and are used to do the normalization
void setupGlobal(char* filename){
    FILE *fp;
    long int totalSample = 0;
    char line[80];

    fp = fopen(filename, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }

    //get max value
    max = 0;
    while(!feof(fp)){
        fgets(line, 80, fp);
        if(!isalpha(line[0])){
            totalSample++;
            if(max < abs(atoi(line)))
                max = abs(atoi(line));
        }
    }
    
    nFactor = (double)limit/max;
    
    //setup dcShift
    getDCShift();
    fclose(fp);
}

// function to normalize the input file
// in the end it writes the normalized file to the output file and save it, for further identifying the words we will use the normalized file
void normalize_data(char* inputFileName, char* outputFileName){

    FILE *fp, *op;
    long int totalSample = 0;
    char line[80];

    fp = fopen(inputFileName, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }

    //removing the output file, if already exists
    remove(outputFileName);

    op = fopen(outputFileName, "w");
    if(op == NULL){
        printf("Error opening file\n");
        exit(1);
    }

    //reading the values from input file, normalizing it and writing it to output file
    while(!feof(fp)){
        fgets(line, 80, fp);
        
        //input file may contain header, so we skip it
        if(!isalpha(line[0])){
            int x = atoi(line);
			//normalizing the value
            double normalizedX = floor((x-dcShift)*nFactor);
            if(abs(normalizedX) > 1)
                fprintf(op, "%d\n", (int)normalizedX);
        }
    }

    //printing the properties we used to normalize the file 
    printf("normalization of file %s complete with following properties\n", inputFileName);
    printf("max: %ld, nFactor: %lf, dcsShift: %lf\n\n", max, nFactor, dcShift);

    //closing the files
    fclose(fp);
    fclose(op); 
}

//this function finds out the average energy and zcr value of ambience
// we record ambience sound beforehand in the same environment, so we can use that for identifying whether word spoken or not
double* countAvgAmbience(char* inputFileName){
    FILE *fp, *op;
    long int total_frame = 0;
    int n = 0;
    char line[80];
    double energy = 0.0, zcr = 0.0, x, prev_x = 0.0;

    fp = fopen(inputFileName, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }

    double *avg = (double*)malloc(sizeof(double)*2);
    avg[0] = 0.0;
    avg[1] = 0.0;
    
    //reading ambience file
    while(!feof(fp)){
        fgets(line, 80, fp);
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

//this function process the word using energy and zcr and echo the decision, whether the word is YES or NO
void processWord(long int word_s, long int word_e, double energy[], double zcr[]){
    int size = word_e - word_s + 1;

    //counting fricatives
    int fctv_cnt = 0;

    for(int i=word_s; i<word_e; i++){
        if(zcr[i] > 10 && zcr[i+1] > 10){
            fctv_cnt++;
        }
    }

    //decision
    if(fctv_cnt >= size * 0.25)
		printf("----> Yes\n");
	else
		printf("----> No\n");
}

//this function is called for seggregating the words from the input file, it will mark word start and word end and subsequently process the word
void word_seggregation(double* ambAvg, char* inputFileName){
    FILE *ip;
	char line[80];
    //reading of the energy and zcr value frame wise
    double energy[10000], zcr[10000];

    /* word_start: marker for index of the first observation of the word
     * word_end: marker for index of the last observation of the word
     * lastIndex tracks the last index where the marker should be placed for word
    */
    long int word_start = -1, word_end = -1, lastIndex = 0, n=0, total_frame = 0, i=0;
    //helps for calculating zcr
    double prev_x = 0, en = 0, z = 0, x;
	int totalWord = 0;

    ip = fopen(inputFileName, "r");
	if(ip == NULL){
		printf("file could not be opened\n");
		exit(1);
	}

    //reading the normalized word file and filling the energy and zcr array
    while(!feof(ip)){
        fgets(line, 80, ip);
        if(!isalpha(line[0])){
            x = atof(line);
            
            if(abs(x) > 1){
                if(n == noFrames){
                    //avg energy
                    en /= noFrames;
                    total_frame++;
                    energy[lastIndex] = en;
                    zcr[lastIndex++] = z;
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
    //closing file
    fclose(ip);

    printf("total frames: %ld\n\n", lastIndex);
	printf("Recognized words\n");
	
    for( ; i<lastIndex-3; ++i){
		if(word_start == -1 && word_end == -1 && energy[i+1] > ambAvg[0] * thrsld_multiplier && energy[i+2] > ambAvg[0] * thrsld_multiplier && energy[i+3] > ambAvg[0] * thrsld_multiplier){
			//marking word start
            word_start = i;
		}
		else if(word_start != -1 && word_end == -1 && energy[i+1] <= ambAvg[0] * thrsld_multiplier && energy[i+2] <= ambAvg[0] * thrsld_multiplier && energy[i+3] <= ambAvg[0] * thrsld_multiplier){
            //marking word end
            word_end = i;
			//printf("start : %ld, end: %ld, energy at start: %lf & end: %lf\nword_start: %ld, word_end: %ld", word_start*noFrames, word_end*noFrames, energy[word_start], energy[word_end], word_start, word_end);
			printf("Word %d: ", ++totalWord);
            //processing the word
            processWord(word_start, word_end, energy, zcr);
			
            //processing of word is completed, so we mark the markers to -1, to reuse it
            word_start = -1;
			word_end = -1;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
    //setup global values according to input
	setupGlobal("provided_yes_no.txt");

    //normalizing the input word data
    normalize_data("provided_yes_no.txt", "normalized_yes_no.txt");

    //normalizing the ambience data
    normalize_data("ambience.txt", "normalized_ambience.txt");

    //counting avg energy and zcr of the normalized ambience data
    double* ambience_avg = countAvgAmbience("normalized_ambience.txt");
    printf("ambience avg energy: %lf, avg zcr: %lf\n\n", ambience_avg[0], ambience_avg[1]);
    
    //calling function to seggregate the words and echo decision
    word_seggregation(ambience_avg, "normalized_yes_no.txt");

	getch();
	return 0;
}

