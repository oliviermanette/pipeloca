#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <tunet.h>
#include <SD.h>
#include <arduinoFFT.h>

#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13
// GUItool: begin automatically generated code
AudioControlSGTL5000     sgtl5000_1;
AudioInputI2S            i2s1;           //xy=223,143
AudioRecordQueue         fluxL;         //xy=632,116
AudioRecordQueue         fluxR;         //xy=650,182
AudioConnection          patchCord1(i2s1, 0, fluxL, 0);
AudioConnection          patchCord2(i2s1, 1, fluxR, 0);

AudioPlayQueue           queue3;         //xy=198,130
AudioPlayQueue           queue4;         //xy=200,266

AudioAnalyzeFFT1024       fft1024_L;       //xy=456,131
AudioAnalyzeFFT1024       fft1024_R;       //xy=476,265

AudioConnection          patchCord3(queue3, fft1024_L);
AudioConnection          patchCord4(queue4, fft1024_R);
// GUItool: end automatically generated code

const int myInput = AUDIO_INPUT_LINEIN;

#define VOLUME 0.01
#define BUFFER_SIZE 4096 // ça fait 16 slides
#define LOCAL_BUFFER 256
#define GLOBAL_THRESHOLD 512
#define DENDRITE_LENGTH 64
#define FFT_RESOLUTION 1024
#define DownSamplingRatio 4
#define FFTBUFFERLENGTH 1024
#define SAMPLING_FREQUENCY 11025

char gchrNbElements;
TUNet TUPos;
//TUBrain Brain;
arduinoFFT FFT = arduinoFFT();

bool isSDAvailable;

int16_t gblBufferL[BUFFER_SIZE];
int16_t gblBufferR[BUFFER_SIZE];
float gdblMeanR, gdblMeanL, gdblRatioPos, gdlbRatioMax;
int16_t gIntMaxValR, gIntMaxValL, gintMaxPosR, gintMaxPosL;
char gChrNbSlide;
bool gblInPeak;
unsigned long gUlngLastPeakTime;
unsigned int guintNbPeak;

float lfltValues[DENDRITE_LENGTH];

double gfltFFTR[FFT_RESOLUTION], gfltFFTL[FFT_RESOLUTION];
double gfltBuffer4FFTR[FFTBUFFERLENGTH], gfltBuffer4FFTL[FFTBUFFERLENGTH];
float gfltDownR[FFTBUFFERLENGTH], gfltDownL[FFTBUFFERLENGTH];
double peakR, peakL;
char gchrDownSamplingPosition;
File gFile;
String strLastEvent;

void parseSerial(bool lblStreamFile){
  // send data only when you receive data:
  if (Serial.available() > 0) {
          // read the incoming byte:
          char lchrIncomingByte;
          String strFullName = "save.tun";
          char chrFullName[9];
          strFullName.toCharArray(chrFullName,9);
          if (lblStreamFile){
            lchrIncomingByte = gFile.read();
          }
          else
            lchrIncomingByte = Serial.read();

          if (lchrIncomingByte==82){
            Serial.println("Here is the Right peak ! ");
            for (int i=0;i<BUFFER_SIZE;i++){
              Serial.println(gblBufferR[i]);
            }
          }
          else if (lchrIncomingByte==104){
            //Display Help message
            Serial.println("°_°");
            Serial.println("TempUnit OS Command List:");
            Serial.println("===============================");
            Serial.println("h: display this Help message");
            Serial.println("");
            Serial.println("L: display last peak signal from Left sensor");
            Serial.println("R: display last peak signal from Right sensor");
            Serial.println("K: display Left signal downsampled 4 times");
            Serial.println("Q: display Right signal downsampled 4 times");
            Serial.println("I: display calculated Information about last peak.");
            Serial.println("c: display Calculated Information about LEFT peak.");
            Serial.println("r: display calculated Information about RIGHT peak.");
            Serial.println("g: display evaluated information about the strenGth of the shock");
            Serial.println("f: display full FFT of the left Peak.");
            Serial.println("F: display full FFT of the right Peak.");
            Serial.println("b: display major frequency of the left Peak.");
            Serial.println("B: display major frequency of the right Peak.");
            Serial.println("");
            Serial.println("a:    Add new TempUnit neuron associated on last peak");
            Serial.println("l[0]: Learn last peak on TempUnit neuron");
            Serial.println("-------------------------------------------------------");
            Serial.println("s[0]: display Score of TempUnit neuron i on last peak");
            Serial.println("S:    Display output of all TU neurons");
            Serial.println("x:    Display maX output of each pool");
            Serial.println("n:    display Network size");
            Serial.println("D[0]: display DENDRITE_LENGTH of neuron i");
            Serial.println("m[0]: display vector of Mean values");
            Serial.println("w[0]: display Weight vector");
            Serial.println("e[0]: display standard dEviation vector");
            Serial.println("N:    display the parameters of all the Network");
            Serial.println("-------------------------------------------------------");
            Serial.println("P     : display the number of Pools (subnetworks)");
            Serial.println("p[0]  : Display the size of the pool i");
            Serial.println("q[0]  : Display the pool #ID of the selected neuron i");
            Serial.println("-------------------------------------------------------");
            Serial.println("");
            Serial.println("-------------------------------------------------------");
            Serial.println("T[64] : set the size of the neTwork");
            Serial.println("t[0]  : select TempUnit neuron i");
            Serial.println("Y[0]  : Set the pool #ID of the neuron i");
            Serial.println("J[64] : Set the length of the Dendrite");
            Serial.println("d[0]  : select Synapse lfltPosition j on Dendrite");
            Serial.println("W[1.0]: set the Weight of the TempUnit i on the synapse j");
            Serial.println("E[1.0]: set the Std on the Neuron i and the synapse j");
            Serial.println("M[0.0]: set the value of max response on neuron i, synapse j");
            Serial.println("");
            Serial.println("i     : Display the selected neuron I");
            Serial.println("j     : Display the selected synapse J");
            Serial.println("-------------------------------------------------------");
            Serial.println("C     : show yes if SD Card is available");
            Serial.println("v     : saVe current netword to SD card");
            Serial.println("o     : lOad network from SD card");
            Serial.println("|°_°|");
          }
          else if (lchrIncomingByte==78){ //Display the parameters of all the Network
              TUPos.showAllPoolParameters();
          }
          else if (lchrIncomingByte==98){ //Display the parameters of all the Network
              Serial.println(peakL);
          }
          else if (lchrIncomingByte==66){ //Display the parameters of all the Network
              Serial.println(peakR);
          }
          else if (lchrIncomingByte==67){
              if (isSDAvailable){
                Serial.println("yes");
              }
              else{
                Serial.println("no");
              }
          }
          else if (lchrIncomingByte==102){
            for (int j=0;j<FFT_RESOLUTION;j++){
              Serial.println(gfltFFTL[j]);
            }
          }
          else if (lchrIncomingByte==70){
            for (int j=0;j<FFT_RESOLUTION;j++){
              Serial.println(gfltFFTR[j]);
            }
          }
          else if (lchrIncomingByte==83){ // Display output of all TU neurons"
              TUPos.showAllPoolScore(lfltValues);
          }
          else if (lchrIncomingByte==120){ //Display max output of each pool
            TUPos.showMaxOfPoolScore(lfltValues);
          }
          else if (lchrIncomingByte==87){//Set the Weight of the TempUnit i on the synapse j
            unsigned int luintNb=0;
            float lfltPosition=100000;
            float luinSize = 0;
            if (lblStreamFile)
              lchrIncomingByte = gFile.read();
            else
              lchrIncomingByte = Serial.read();
            while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
              luintNb++;
              luinSize += (lchrIncomingByte-48)*lfltPosition;
              lfltPosition /=10;
              if (lblStreamFile)
                lchrIncomingByte = gFile.read();
              else
                lchrIncomingByte = Serial.read();
              if (luintNb==6){
                Serial.println("Error, max value 999'999");
                break;
              }
            }
            if (luintNb){
              luinSize /=pow(10,5-luintNb+1);
              if (lchrIncomingByte==46){
                if (lblStreamFile)
                  lchrIncomingByte = gFile.read();
                else
                  lchrIncomingByte = Serial.read();
                lfltPosition = 0.1;
                while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
                  luinSize += (lchrIncomingByte-48)*lfltPosition;
                  lfltPosition /=10;
                  if (lblStreamFile)
                    lchrIncomingByte = gFile.read();
                  else
                    lchrIncomingByte = Serial.read();
                }
              }
              Serial.print("Set the weight of the synapse to ");
              Serial.println(luinSize);
              TUPos.setWeight(luinSize);
            }
            else
              Serial.println("Cannot determine the desired weight.");
          }
          else if(lchrIncomingByte==68){ //Display Dendrite Length
            unsigned int luintNb=0;
            float lfltPosition=10;
            unsigned int luinSize = 0;
            lchrIncomingByte = Serial.read();
            while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
              luintNb++;
              luinSize += (lchrIncomingByte-48)*lfltPosition;
              lfltPosition /=10;
              if (lblStreamFile)
                lchrIncomingByte = gFile.read();
              else
                lchrIncomingByte = Serial.read();
            }
            if (luintNb){
              if (luintNb==1)
                luinSize /=10;
              TUPos.showDendriteLength(luinSize);
            }
            else
              Serial.println("Cannot determine the #ID of the requested neuron.");

          }
          else if (lchrIncomingByte==69){//Set the Std of the TempUnit i on the synapse j
            unsigned int luintNb=0;
            float lfltPosition=100000;
            float luinSize = 0;
            if (lblStreamFile)
              lchrIncomingByte = gFile.read();
            else
              lchrIncomingByte = Serial.read();
            while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
              luintNb++;
              luinSize += (lchrIncomingByte-48)*lfltPosition;
              lfltPosition /=10;
              if (lblStreamFile)
                lchrIncomingByte = gFile.read();
              else
                lchrIncomingByte = Serial.read();
              if (luintNb==6){
                Serial.println("Error, max value 999'999");
                break;
              }
            }
            if (luintNb){
              luinSize /=pow(10,5-luintNb+1);
              if (lchrIncomingByte==46){
                if (lblStreamFile)
                  lchrIncomingByte = gFile.read();
                else
                  lchrIncomingByte = Serial.read();
                lfltPosition = 0.1;
                while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
                  luinSize += (lchrIncomingByte-48)*lfltPosition;
                  lfltPosition /=10;
                  if (lblStreamFile)
                    lchrIncomingByte = gFile.read();
                  else
                    lchrIncomingByte = Serial.read();
                }
              }
              Serial.print("Set the std of the synapse to ");
              Serial.println(luinSize);
              TUPos.setStd(luinSize);
            }
            else
              Serial.println("Cannot determine the desired std.");
          }
          else if (lchrIncomingByte==77){//Set the value of max response on neuron i, synapse j
            unsigned int luintNb=0;
            float lfltPosition=100000;
            float luinSize = 0;
            if (lblStreamFile)
              lchrIncomingByte = gFile.read();
            else
              lchrIncomingByte = Serial.read();
            while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
              luintNb++;
              luinSize += (lchrIncomingByte-48)*lfltPosition;
              lfltPosition /=10;
              if (lblStreamFile)
                lchrIncomingByte = gFile.read();
              else
                lchrIncomingByte = Serial.read();
              if (luintNb==6){
                Serial.println("Error, max value 999'999");
                break;
              }
            }
            if (luintNb){
              luinSize /=pow(10,5-luintNb+1);
              if (lchrIncomingByte==46){
                if (lblStreamFile)
                  lchrIncomingByte = gFile.read();
                else
                  lchrIncomingByte = Serial.read();
                lfltPosition = 0.1;
                while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
                  luinSize += (lchrIncomingByte-48)*lfltPosition;
                  lfltPosition /=10;
                  if (lblStreamFile)
                    lchrIncomingByte = gFile.read();
                  else
                    lchrIncomingByte = Serial.read();
                }
              }

              Serial.print("Set the mean of the synapse to ");
              Serial.println(luinSize);
              TUPos.setDValue(luinSize);
            }
            else
              Serial.println("Cannot determine the desired mean.");
          }
          else if (lchrIncomingByte==84){//T[64] : Set the size of the network
              unsigned int luintNb=0;
              float lfltPosition=10;
              unsigned int luinSize = 0;
              if (lblStreamFile)
                lchrIncomingByte = gFile.read();
              else
                lchrIncomingByte = Serial.read();
              while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
                luintNb++;
                luinSize += (lchrIncomingByte-48)*lfltPosition;
                lfltPosition /=10;
                if (lblStreamFile)
                  lchrIncomingByte = gFile.read();
                else
                  lchrIncomingByte = Serial.read();
              }
              if (luintNb){
                if (luintNb==1)
                  luinSize /=10;
                Serial.print("Set the size of the network to ");
                Serial.println(luinSize);
                TUPos.setNetSize(luinSize);
              }
              else
                Serial.println("Cannot determine the size of the network.");
          }
          else if (lchrIncomingByte==103){
              Serial.println((gdblMeanR+gdblMeanL)/2.0);
          }
          else if (lchrIncomingByte==112){
            if (lblStreamFile)
              lchrIncomingByte = gFile.read();
            else
              lchrIncomingByte = Serial.read();
            unsigned int luintChar = lchrIncomingByte-48;
            if (luintChar>9)
              luintChar=0;
            TUPos.showPoolSize(luintChar);
          }
          else if (lchrIncomingByte==113){//q[0]  : Display the pool #ID of the selected neuron i
              unsigned int luintNb=0;
              float lfltPosition=10;
              unsigned int luinSize = 0;
              if (lblStreamFile)
                lchrIncomingByte = gFile.read();
              else
                lchrIncomingByte = Serial.read();
              while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
                luintNb++;
                luinSize += (lchrIncomingByte-48)*lfltPosition;
                lfltPosition /=10;
                if (lblStreamFile)
                  lchrIncomingByte = gFile.read();
                else
                  lchrIncomingByte = Serial.read();
              }
              if (luintNb){
                if (luintNb==1)
                  luinSize /=10;
                TUPos.showPoolID(luinSize);
              }
              else
                Serial.println("Cannot determine the desired Neuron ID.");
          }
          else if (lchrIncomingByte==80){
            TUPos.showPoolsNumber();
          }
          else if (lchrIncomingByte==109){
            if (lblStreamFile)
              lchrIncomingByte = gFile.read();
            else
              lchrIncomingByte = Serial.read();
            Serial.print("Argument :");
            unsigned int luintChar = lchrIncomingByte-48;
            Serial.println(luintChar);
            if (luintChar>9)
              luintChar=0;
            TUPos.showDValues(luintChar);
          }
          else if (lchrIncomingByte==119){
            if (lblStreamFile)
              lchrIncomingByte = gFile.read();
            else
              lchrIncomingByte = Serial.read();
            Serial.print("Argument :");
            unsigned int luintChar = lchrIncomingByte-48;
            Serial.println(luintChar);
            if (luintChar>9)
              luintChar=0;
            TUPos.showWeights(luintChar);
          }
          else if (lchrIncomingByte==116){//Select TempUnit i
            unsigned int luintNb=0;
            float lfltPosition=10;
            unsigned int luinSize = 0;
            if (lblStreamFile)
              lchrIncomingByte = gFile.read();
            else
              lchrIncomingByte = Serial.read();
            while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
              luintNb++;
              luinSize += (lchrIncomingByte-48)*lfltPosition;
              lfltPosition /=10;
              if (lblStreamFile)
                lchrIncomingByte = gFile.read();
              else
                lchrIncomingByte = Serial.read();
            }
            if (luintNb){
              if (luintNb==1)
                luinSize /=10;
              Serial.print("Select the TempUnit Neuron #");
              Serial.println(luinSize);
              TUPos.selectNeuron(luinSize);
            }
            else
              Serial.println("Cannot determine which TempUnit neuron you want");
          }
          else if (lchrIncomingByte==100){//Select Synapse j
            unsigned int luintNb=0;
            float lfltPosition=10;
            unsigned int luinSize = 0;
            if (lblStreamFile)
              lchrIncomingByte = gFile.read();
            else
              lchrIncomingByte = Serial.read();
            while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
              luintNb++;
              luinSize += (lchrIncomingByte-48)*lfltPosition;
              lfltPosition /=10;
              if (lblStreamFile)
                lchrIncomingByte = gFile.read();
              else
                lchrIncomingByte = Serial.read();
            }
            if (luintNb){
              if (luintNb==1)
                luinSize /=10;
              Serial.print("Select the Synapse #");
              Serial.println(luinSize);
              TUPos.selectSynapse(luinSize);
            }
            else
              Serial.println("Cannot determine which Synapse you want");
          }
          else if (lchrIncomingByte==101){
            if (lblStreamFile)
              lchrIncomingByte = gFile.read();
            else
              lchrIncomingByte = Serial.read();
            Serial.print("Argument :");
            unsigned int luintChar = lchrIncomingByte-48;
            Serial.println(luintChar);
            if (luintChar>9)
              luintChar=0;
            TUPos.showStd(luintChar);
          }
          else if (lchrIncomingByte==110){
            Serial.println(TUPos.getTUNetSize());
          }
          else if (lchrIncomingByte==97){// Add new TempUnit neuron associated on last peak
            if (guintNbPeak){
              TUPos.setNewTU(lfltValues);
            }
          }
          else if (lchrIncomingByte==108){//Learn, Adapt to last peak
              unsigned int luintNb=0;
              float lfltPosition=10;
              unsigned char lchrNeuronID = 0;
              if (lblStreamFile)
                lchrIncomingByte = gFile.read();
              else
                lchrIncomingByte = Serial.read();
                while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
                  luintNb++;
                  lchrNeuronID += (lchrIncomingByte-48)*lfltPosition;
                  lfltPosition /=10;
                  if (lblStreamFile)
                    lchrIncomingByte = gFile.read();
                  else
                    lchrIncomingByte = Serial.read();
                }
                if (luintNb){
                  if (luintNb==1)
                    lchrNeuronID /=10;
                  Serial.print("Learn the Neuron #");
                  Serial.println(lchrNeuronID);
                  String lstrDest = TUPos.getNetID();
                  lstrDest += "/";
                  lstrDest += lchrNeuronID;
                  lstrDest += strLastEvent.substring(strLastEvent.lastIndexOf("/"));

                  moveFile(strLastEvent,lstrDest);
                  TUPos.learnNewVector(lchrNeuronID,lfltValues);
                }
                else
                  Serial.println("Cannot determine which neuron you want");

          }
          else if (lchrIncomingByte==105){
            TUPos.showSelectedNeuron();
          }
          else if (lchrIncomingByte==106){
            TUPos.showSelectedSynapse();
          }
          else if (lchrIncomingByte==118){ //Save current netword to SD card
            saveNetwork2File();
          }
          else if (lchrIncomingByte==111){ //Load network from SD card
            gFile = SD.open(chrFullName, FILE_READ);
            do {
              parseSerial(true);
            } while (gFile.available());
            gFile.close();
          }
          else if (lchrIncomingByte==89){ //Y[0]  : Set the pool #ID of the neuron i
            unsigned int luintNb=0;
            float lfltPosition=10;
            unsigned int luinSize = 0;
            if (lblStreamFile)
              lchrIncomingByte = gFile.read();
            else
              lchrIncomingByte = Serial.read();
            while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
              luintNb++;

              luinSize += (lchrIncomingByte-48)*lfltPosition;
              lfltPosition /=10;
              if (lblStreamFile)
                lchrIncomingByte = gFile.read();
              else
                lchrIncomingByte = Serial.read();
            }
            if (luintNb){
              if (luintNb==1)
                luinSize /=10;
              Serial.print("Assign the current neuron to the pool #");
              Serial.println(luinSize);
              TUPos.setPoolID(luinSize);
            }
            else
              Serial.println("Cannot determine the length of the dendrite.");
          }
          else if (lchrIncomingByte==74){ //Set the length of the Dendrite
            unsigned int luintNb=0;
            float lfltPosition=10;
            unsigned int luinSize = 0;
            if (lblStreamFile)
              lchrIncomingByte = gFile.read();
            else
              lchrIncomingByte = Serial.read();
            while ((lchrIncomingByte>=48)&(lchrIncomingByte<=57)) {
              luintNb++;

              luinSize += (lchrIncomingByte-48)*lfltPosition;
              lfltPosition /=10;
              if (lblStreamFile)
                lchrIncomingByte = gFile.read();
              else
                lchrIncomingByte = Serial.read();
            }
            if (luintNb){
              if (luintNb==1)
                luinSize /=10;
              Serial.print("Set the size of the network to ");
              Serial.println(luinSize);
              TUPos.setDendriteSize(luinSize);
            }
            else
              Serial.println("Cannot determine the length of the dendrite.");
          }
          else if (lchrIncomingByte==115){//get TempUnit score on last peak
              if (lblStreamFile)
                lchrIncomingByte = gFile.read();
              else
                lchrIncomingByte = Serial.read();
              Serial.print("Argument :");
              unsigned int luintChar = lchrIncomingByte-48;
              Serial.println(luintChar);
              if (luintChar>9)
                luintChar=0;
              TUPos.showIndividualScore(luintChar,lfltValues);
          }
          else if (lchrIncomingByte==75){
            for (int i=0;i<FFTBUFFERLENGTH;i++){
              Serial.println(gfltDownL[i]);
            }
          }
          else if (lchrIncomingByte==81){
            for (int i=0;i<FFTBUFFERLENGTH;i++){
              Serial.println(gfltDownR[i]);
            }
          }
          else if (lchrIncomingByte==76){
            Serial.println("Here is the Left peak ! ");
            for (int i=0;i<BUFFER_SIZE;i++){
              Serial.println(gblBufferL[i]);
            }
          }
          else if (lchrIncomingByte==99){
            Serial.println(gdblMeanL);
            Serial.println(gIntMaxValL);
            Serial.println(gintMaxPosL);
            gblInPeak = false;
          }
          else if (lchrIncomingByte==67){
            Serial.println(gdblMeanR);
            Serial.println(gIntMaxValR);
            Serial.println(gintMaxPosR);
          }
          else if (lchrIncomingByte==73){
            Serial.print("Here is the Left peak Mean : ");
            Serial.println(gdblMeanL);
            Serial.print("Here is the Right peak Mean : ");
            Serial.println(gdblMeanR);
            Serial.print("Here is the Ratio Mean : ");
            Serial.println(gdblMeanR/gdblMeanL);

            Serial.print("Here is the Left peak Max Value : ");
            Serial.println(gIntMaxValL);
            Serial.print("Here is the Right peak Max Value : ");
            Serial.println(gIntMaxValR);
            Serial.print("Here is the Ratio Max Value : ");
            Serial.println(gdlbRatioMax);

            Serial.print("Here is the Left peak Max Position : ");
            Serial.println(gintMaxPosL);
            Serial.print("Here is the Right peak Max Positon : ");
            Serial.println(gintMaxPosR);
            Serial.print("Here is the Ratio Mean : ");
            Serial.println(gdblRatioPos);
          }
  }
}

void saveNetwork2File( ){
  if (isSDAvailable){
    File netFile;
    String strFullName = "save.tun";
    char chrFullName[9];
    strFullName.toCharArray(chrFullName,9);
    if (SD.exists(chrFullName)) {
      SD.remove(chrFullName);
      Serial.print("This file already exists, it will be replaced by new data : ");
      Serial.println(chrFullName);
    }
    netFile = SD.open(chrFullName, FILE_WRITE);
    netFile.print("T");
    unsigned char luchrNetSize = TUPos.getTUNetSize();
    netFile.println(luchrNetSize);

    for (unsigned char i=0;i<luchrNetSize;i++){
      netFile.print("t");
      netFile.println(i);
      TUPos.selectNeuron(i);
      netFile.print("J");
      unsigned char lintDL = TUPos.getDendriteSize(i);
      netFile.println(lintDL);
      // pool ID
      netFile.print("Y");
      netFile.println(TUPos.getPoolID(i));

      for (int j=0;j<lintDL;j++){
        netFile.print("d");
        netFile.println(j);
        TUPos.selectSynapse(j);
        netFile.print("W");
        netFile.println(TUPos.getWeight());
        netFile.print("E");
        netFile.println(TUPos.getStd());
        netFile.print("M");
        netFile.println(TUPos.getDValue());
      }
    }
    netFile.close();
  }
  else{
    Serial.println("Cannot save file to SD, SD is not available.");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("°_°");
  Serial.println("Welcome to TempUnit hardware interface");
  Serial.println("Type h to display an help message");
  Serial.println("======================================");

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  isSDAvailable =false;
  if (!(SD.begin(SDCARD_CS_PIN))) {
    Serial.println("Unable to access the SD card");
  }
  else{
    isSDAvailable = true;
    Serial.println("SD Ready");
  }
    gchrNbElements = 2*BUFFER_SIZE/LOCAL_BUFFER;
    gdblMeanR = 0;
    gIntMaxValR= 0;
    gIntMaxValL= 0;
    gintMaxPosR= 0;
    gintMaxPosL= 0;
    gdblMeanL = 0;
    gdblRatioPos= 0;
    gdlbRatioMax= 0;
    guintNbPeak = 0;
    AudioMemory(256);
    //Enable the audio shield
    sgtl5000_1.enable();
    sgtl5000_1.inputSelect(myInput);
    sgtl5000_1.volume(VOLUME);
    gChrNbSlide = 0;
    gblInPeak = false;
    for (int i=0;i<BUFFER_SIZE;i++){
        gblBufferR[i]=0;
        gblBufferL[i]=0;
    }
    fluxL.begin();
    fluxR.begin();

    TUPos.setAllNetworkDendriteSize(DENDRITE_LENGTH);
}
void loop() {
        int16_t bufferL[LOCAL_BUFFER];
        int16_t bufferR[LOCAL_BUFFER];
        for (int i=0;i<LOCAL_BUFFER;i++){
            bufferL[i]=0;
            bufferR[i]=0;
        }
        parseSerial(0);

        if ((millis()-gUlngLastPeakTime)>5000)
          gblInPeak = false;

        int lIntBufferSize = fluxR.available();
        if (lIntBufferSize>= 1){
          memcpy(bufferL, fluxL.readBuffer(), LOCAL_BUFFER);
          memcpy(bufferR, fluxR.readBuffer(), LOCAL_BUFFER);
          fluxL.freeBuffer();
          fluxR.freeBuffer();
          if (gChrNbSlide==0){
            if (gblInPeak==false){
              for (int i=0;i<LOCAL_BUFFER;i++){
                if ((bufferR[i]>GLOBAL_THRESHOLD)||
                    (bufferR[i]<(-1*GLOBAL_THRESHOLD))||
                    (bufferL[i]>GLOBAL_THRESHOLD)||
                    (bufferL[i]<(-1*GLOBAL_THRESHOLD))){
                    //Sauvegarde
                    gChrNbSlide = 1;
                    gblInPeak = true;
                    gUlngLastPeakTime = millis();
                    Serial.println("Peak détecté");
                    guintNbPeak++;
                    // Reset values for next peak
                    gdblMeanR = 0;
                    gdblMeanL = 0;
                    gIntMaxValR= 0;
                    gIntMaxValL= 0;
                    gintMaxPosR= 0;
                    gintMaxPosL= 0;
                    gdblRatioPos= 0;
                    gdlbRatioMax= 0;
                    gchrDownSamplingPosition = 0;
                    //Should start the FFT immediately
                    break;
                }
              }
            }
          }
          else if (gChrNbSlide<(gchrNbElements+1)){
            //Ajoute
            gChrNbSlide++;
          }
          else if (gChrNbSlide==(gchrNbElements+1)){
            gChrNbSlide = 0;
            //C'est fini, on peut travailler sur les données
            for (int i=0;i<FFT_RESOLUTION;i++){
              gfltFFTL[i] = 0;
              gfltFFTR[i] = 0;
            }
            downSampler();
            ArFFT();

            int16_t lintTmpValue;
            for (int i=0;i<BUFFER_SIZE;i++){
              if (gblBufferR[i]>=0)
                lintTmpValue = gblBufferR[i];
              else
                lintTmpValue = -1 * gblBufferR[i];
              gdblMeanR += lintTmpValue;

              if (lintTmpValue>gIntMaxValR){
                gIntMaxValR = lintTmpValue;
                gintMaxPosR = i;
              }
              if (gblBufferL[i]>=0)
                lintTmpValue = gblBufferL[i];
              else
                lintTmpValue = -1 * gblBufferL[i];
              if (lintTmpValue>gIntMaxValL){
                gIntMaxValL = lintTmpValue;
                gintMaxPosL = i;
              }
              gdblMeanL += lintTmpValue;
            }
            gdblMeanL /=BUFFER_SIZE;
            gdblMeanR /=BUFFER_SIZE;
            gdblRatioPos= (double)gintMaxPosL/(double)gintMaxPosR;
            gdlbRatioMax= (double)gIntMaxValR/(double)gIntMaxValL;

            lfltValues[0] = gdblMeanL;
            lfltValues[1] = gdblMeanR;
            lfltValues[2] = gdblMeanR/gdblMeanL;
            lfltValues[3] = gIntMaxValL;
            lfltValues[4] = gIntMaxValR;
            lfltValues[5] = gdlbRatioMax;
            lfltValues[6] = gintMaxPosL;
            lfltValues[7] = gintMaxPosR;
            lfltValues[8] = gdblRatioPos;
            lfltValues[9] = (gdblMeanR+gdblMeanL)/2.0;
            lfltValues[10] = peakL;
            lfltValues[11] = peakR;
            lfltValues[12] =peakL/peakR;
            {
              int k=13;
              for (int j=0;j<25;j++){
                lfltValues[k] = gfltFFTL[2*j+900];
                k++;
                lfltValues[k] = gfltFFTR[2*j+900];
                k++;
              }
              unsigned int result = TUPos.getWinnerID(lfltValues);
              Serial.println(result);
              Serial.println("debug value");
              saveEvent(lfltValues,result,TUPos.getDendriteSize(0));
            }
           }
          if ((gChrNbSlide<(gchrNbElements+1))&&(gChrNbSlide>0)){
            memcpy(gblBufferL+(LOCAL_BUFFER/2)*(gChrNbSlide-1), bufferL, LOCAL_BUFFER);
            memcpy(gblBufferR+(LOCAL_BUFFER/2)*(gChrNbSlide-1), bufferR, LOCAL_BUFFER);
          }
        }
}

bool saveEvent(float fltVector[], unsigned int TUId, unsigned char luchrDendriteLength){
  //const int lcstintFullName = 16;
  if (isSDAvailable){
    char chrFolderName[7];
    char chrTMP[3];
    String strFullName = TUPos.getNetID();
    strFullName.toCharArray(chrTMP,3);

    if (!SD.exists(chrTMP)){
      SD.mkdir(chrTMP);
      // Serial.print("Create Folder : ");
      // Serial.println(chrTMP);
    }
    strFullName += "/";
    strFullName += TUId;
    strFullName.toCharArray(chrFolderName,7);
    if (!SD.exists(chrFolderName)){
      SD.mkdir(chrFolderName);
      // Serial.print("Create Folder : ");
      // Serial.println(chrFolderName);
    }
    strFullName += "/";
    char lchrToto[5];
    ltoa(TUPos.getNetTimeStamp(),lchrToto,5);
    strFullName += lchrToto;
    strFullName += ".tum";
    char chrFullName[17];
    strLastEvent = strFullName;
    strFullName.toCharArray(chrFullName,17);
    File lfFile = SD.open(chrFullName, FILE_WRITE);
    if (!lfFile){
      // Serial.print("Cannot create the file :");
      // Serial.println(chrFullName);
      return false;
    }

    for (unsigned char i =0; i<luchrDendriteLength; i++){
      dtostrf(fltVector[i], 15, 2, chrFullName);
      for (int j=0;j<15-1;j++)
        lfFile.print(chrFullName[j]);
      lfFile.println(chrFullName[15-1]);
    }
    lfFile.close();
    return true;
  }
  return false;
}

void ArFFT(){
  double vImag[FFTBUFFERLENGTH];
  for(int i=0; i<FFTBUFFERLENGTH; i++)
          vImag[i] = 0;
  FFT.Windowing(gfltBuffer4FFTR, FFT_RESOLUTION, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(gfltBuffer4FFTR, vImag, FFT_RESOLUTION, FFT_FORWARD);
  FFT.ComplexToMagnitude(gfltBuffer4FFTR, vImag, FFT_RESOLUTION);
  peakR = FFT.MajorPeak(gfltBuffer4FFTR, FFT_RESOLUTION, SAMPLING_FREQUENCY);
  FFT.Windowing(gfltBuffer4FFTL, FFT_RESOLUTION, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(gfltBuffer4FFTL, vImag, FFT_RESOLUTION, FFT_FORWARD);
  FFT.ComplexToMagnitude(gfltBuffer4FFTL, vImag, FFT_RESOLUTION);
  peakL = FFT.MajorPeak(gfltBuffer4FFTL, FFT_RESOLUTION, SAMPLING_FREQUENCY);
  //Serial.println(peak);
  for (int j=0;j<FFT_RESOLUTION;j++){
    gfltFFTR[j] = gfltBuffer4FFTR[j];
    gfltFFTL[j] = gfltBuffer4FFTL[j];
    //Serial.println(gfltFFTR[j]);
  }
}

void downSampler(){
  int j = 0;
  for (int i = 0; i < BUFFER_SIZE; i+=DownSamplingRatio) {
    gfltBuffer4FFTR[j]=0;
    gfltBuffer4FFTL[j]=0;
    for (int k = i; k < i+DownSamplingRatio; k++) {
      gfltBuffer4FFTR[j]+=(double)gblBufferR[k];
      gfltBuffer4FFTL[j]+=(double)gblBufferL[k];
    }
    gfltDownR[j] = gfltBuffer4FFTR[j]/DownSamplingRatio;
    gfltDownL[j] = gfltBuffer4FFTL[j]/DownSamplingRatio;
    j++;
  }
}

void moveFile(String source, String destination) {
  if (isSDAvailable){
    char chrFullName[17];
    source.toCharArray(chrFullName,17);
    if (SD.exists(chrFullName)) {
      File myFileIn = SD.open(chrFullName, FILE_READ);
      destination.toCharArray(chrFullName,17);
      File myFileOut = SD.open(chrFullName, FILE_WRITE);
      while (myFileIn.available())
        myFileOut.write(myFileIn.read());
      myFileIn.close();
      myFileOut.close();
      source.toCharArray(chrFullName,17);
      SD.remove(chrFullName);
    }
  }
}
