// AD537x.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "dac/DAC.h"
#include <cmath>
#include <chrono>
int main()
{
   // auto start = std::chrono::high_resolution_clock::now();  
   // auto elapsed = std::chrono::high_resolution_clock::now() - start;
    AD537x::DAC *dac = AD537x::DAC::getInstance();
    
    dac->findAndInitializeAllBoards();

  //  start = std::chrono::high_resolution_clock::now();
            dac->writeSPIWord(0, "022000");
  //  elapsed = std::chrono::high_resolution_clock::now() - start;
  //  std::cout << "WriteSPIWord microSeconds: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << std::endl;
    
    
   
   // start = std::chrono::high_resolution_clock::now();
           
   // elapsed = std::chrono::high_resolution_clock::now() - start;
   // std::cout << "setLDAC microSeconds: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << std::endl;


    dac->writeSPIWord(0, "032000");
    dac->setLDAC(0);


    //start = std::chrono::high_resolution_clock::now();
        dac->writeVoltage(0, 0, -4.0);
   // elapsed = std::chrono::high_resolution_clock::now() - start;
    //std::cout << "writeVoltage microSeconds: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << std::endl;



        dac->setLDAC(0, VR_CLRLDAC);
   // long long microseconds = 0;
    int loopMax = 100000;
   // start = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < loopMax; j++)
    {
        
        //dac->setLDAC(j%2);
        dac->writeVoltage(0, 0, -1.0);
        dac->writeVoltage(0, 0, 1.0);
        dac->writeVoltage(0, 0, -1.0);
        dac->writeVoltage(0, 0, 1.0);
        dac->writeVoltage(0, 0, -1.0);
        dac->writeVoltage(0, 0, 1.0);
        dac->writeVoltage(0, 0, -1.0);
        dac->writeVoltage(0, 0, 1.0);
        
   }
  // elapsed = std::chrono::high_resolution_clock::now() - start;
  // microseconds += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
  // std::cout << "setLDAC AVEG for "<<loopMax << " iterations in microSeconds: " << microseconds/loopMax << std::endl;
  //




    dac->setLDAC(0, VR_CLRLDAC);
    
    double freq = 100;
    const int NB_OF_SAMPLES = 2000;
    double amplitude = 3;
    float  buffer[NB_OF_SAMPLES];
    long deltaArray[NB_OF_SAMPLES];
    int i = 0;

    const double pi = std::acos(-1);
    double angle = 0.0;
    for (int n = 0; n < NB_OF_SAMPLES; n++)
    {
        buffer[n] =  amplitude * std::sin(angle * (2 * pi));
        angle += freq / NB_OF_SAMPLES;
       // std::cout << buffer[n] << std::endl;
    }
 ///
 ///  // auto start = std::chrono::system_clock::now();
 ///  // auto displayStart = std::chrono::system_clock::now();
 ///   int counter = 0;
 ///   while (true) {    //keep looping quickly
 ///      // auto now = std::chrono::system_clock::now();
 ///      // auto interval = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
 ///      // if (interval.count() > 500) {   //if 100 microsecond do
 ///           dac->writeVoltage(0, 1, buffer[counter++]);
 ///          // dac->setLDAC(0);
 ///           //counter = counter + 1;
 ///           if (counter > NB_OF_SAMPLES)
 ///               counter = 0;
 ///       //    start = std::chrono::system_clock::now();
        //}
            
  ////
  //// std::cout << std::chrono::high_resolution_clock::period::den << std::endl;
  //// auto start_time = std::chrono::high_resolution_clock::now();
  //// int temp=1;
  //// for (int i = 0; i < 242000000; i++)
  ////     temp += temp;
  //// auto end_time = std::chrono::high_resolution_clock::now();
  //// std::cout << std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count() << ":";
  //// std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << ":";

       // }

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
