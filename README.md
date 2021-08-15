# Assignment 1 : yes/no classifier using recorded audio
# Roll Number: 214101023

## AIM: 
We are given i/p of audio file which contains yes and no as spoken word. We have to write the code which identify the words yes and no.

# Steps to run the project
1. Download zip and extract it
2. Open Microsoft visual studio and press ```Ctrl + Shift + O``` and browse to the folder where you extracted the zip.
3. select the **.sln** file, it will launch the project.
4. Go to Source files and select assn1.cpp file and press F5. 

## Process:
1. I have recorded the audio for finding the dc shift occuring in my laptop after decreasing the microphone sensitivity, which is in [dc_shift.txt](https://github.com/jayskhatri/yes-no-classifier-using-audio-freq/blob/main/assn1/dc_shift.txt) file.
2. Also recorded the ambience file to find out average noise energy and average zcr value which is [ambience.txt](https://github.com/jayskhatri/yes-no-classifier-using-audio-freq/blob/main/assn1/n_ambience.txt). Recorded YES/NO file in the same environment which is [yes_no.txt](https://github.com/jayskhatri/yes-no-classifier-using-audio-freq/blob/main/assn1/yes_no.txt). The file which was provided to us in facebook is [provided_yes_no.txt](https://github.com/jayskhatri/yes-no-classifier-using-audio-freq/blob/main/assn1/provided_yes_no.txt).
3. Now, first we find out the max value occuring in out yes_no.txt audio. that max value we will use for the normalization our data into +/- 5000.
4. ```setupGlobal()``` function is called to setting up the values of the dcShift, max (max value from the raw audio file), normalization factor.
5. Now we normalize our ambience data file as well as yes_no recorded data file to +/- 5000 using the global values we have set up. Function to normalize the data is ```normalize_data()```
6. Now, using the normalized ambience data file, I have find the average values of the noise energy and zcr.
7. using that average value, I have taken the multiplier value as 5 (found from tuning).
8. lastly we call the ```word_seggregation()``` function to mark the start and end of the spoken word from the file, which subsequently call the ```processWord()``` function which analyse the spoken word from start marker to end marker and echo the decision.   

# Logic
- To identify the word start we check for the continuous 4 frames energy, if it is high then some threshold energy then we consider it as word start
- To identify the word end we check for the continuous 4 frames enrgy, it it is low then some threshold energy then we consider it as word end
- Threshold energy is 5*noise energy
- Then we count the number of fricatives in that word, if it is more then the 25% of the word size, then it is considered as YES, else NO
