// Assignment 1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *ip, *op;
	char line[100];
	double energy, zcr, prev_x, n = 0;
	energy = zcr = prev_x = 0;

	ip = fopen("assn1.txt", "r");
	op = fopen("op.txt", "a");
	
	if(ip==NULL || op==NULL) exit(1);
	
	while(!feof(ip)){
		//reading line from the input file
		fgets(line, 100, ip);
		
		//if the read line's first character is digit then it is the observation
		if(isdigit(line[0])){
			
			//coverting whole line to integer
			int x = atoi(line);
			
			//for every 150 sample we will calculate the energy and zcr
			if(n==150){
				//writing the values of energy and zcr into the file
				fprintf(op, "energy: %f, zcr: %f\n", energy/100.0, zcr);
				//printf("energy: %f, zcr: %f\n", energy/100.0, zcr);
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
	}
	fclose(ip);
	fclose(op);
	return 0;
}

