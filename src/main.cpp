#include "CStopWatch.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <mpi.h>
#include <iomanip>
#include <algorithm>

void printVectorString(std::vector<std::string> t){
    for(int i=0; i<t.size(); i++){
        std::cout << std::setw(2) << t[i] << " ";
    } 
}
void printVectorDouble(std::vector<double> t){
    for(int i=0; i<t.size(); i++){
        std::cout << std::setw(2) << t[i] << " ";
    } 
}
void printVectorInt(std::vector<int> t){
    for(int i=0; i<t.size(); i++){
        std::cout << std::setw(2) << t[i] << " ";
    } 
}

// Function to read in a particular data set. This is created specifically for the data set I chose and passes in vectors by reference and fills them with their necessary values
void readData(std::vector<std::string>& columns, std::vector<std::string>& sex, std::vector<std::string>& length,std::vector<std::string>& diameter,std::vector<std::string>& height,std::vector<std::string>& weight,std::vector<std::string>& shucked_weight,std::vector<std::string>& viscera_weight,std::vector<std::string>& shell_weight,std::vector<std::string>& age){
    int numProcs, myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    std::fstream fin;
    std::string line = "", col = "", temp, ssCharacter, data = "", checkForLastColumn = "";
    // Open an existing file
    fin.open("../data/CrabAgePrediction.csv", std::ios::in);
    int index = 0, colIndex = 0;
    std::cout << myRank;
    while(fin >> temp){
        std::stringstream ss(temp);
       // std::cout << std::endl << temp;
        if(index==0){ // Then we're reading the columns. 
            for(int stringIndex = 0; stringIndex < temp.length(); stringIndex++){
                ssCharacter = temp[stringIndex];
                if(ssCharacter != ","){
                    col += ssCharacter; 
                }else{
                    columns[colIndex] = col;
                    colIndex++;
                    col = "";
                }
            }
        }else{ //Then we're reading in the data
            colIndex=0;
            for(int stringIndex = 0; stringIndex < temp.length(); stringIndex++){
                ssCharacter = temp[stringIndex];
                if(ssCharacter != ","){ //Check for comma. Gobble up all data until then
                    if(colIndex !=8){ //8th column is the last column and it doesn't end with a comma. Seperate processing. There's probably a better way to do this. It's because i'm using a string and not the ss value so this will never be newline. 
                        data += ssCharacter;
                    }else{  //For the last column
                        checkForLastColumn = temp[temp.length()-2];  
                        if(checkForLastColumn != ","){ // These values are always 1 or two significant figures. Meaning this reading won't work for data that doesn't meet this criteria
                            data += temp[temp.length()-2]; //Extract last two digits if its two digits long
                            data += temp[temp.length()-1];
                        }else{
                            data += temp[temp.length()-1]; // Otherwise it's only one digit long
                        }
                        age[index-1] = data; //Won't make it to next processing step so just return the last column here and reset the data variable. 
                        data = "";
                    }
                }else{ //Depending on the column we're processing, add values to that vector. Increase the column index when finished. 
                    if(colIndex == 0){ //sex
                        sex[index-1] = data;
                        colIndex++;
                        data = "";
                    }else if (colIndex == 1){ //length
                        length[index-1] = data;
                        colIndex++;
                        data = "";
                    }else if (colIndex == 2){ //diameter
                        diameter[index-1] = data;
                        colIndex++;
                        data = "";
                    }else if (colIndex == 3){ //height
                        height[index-1] = data;
                        colIndex++;
                        data = "";
                    }else if (colIndex == 4){ //weight
                        weight[index-1] = data;
                        colIndex++;
                        data = "";
                    }else if (colIndex == 5){ //shucked_weight
                        shucked_weight[index-1] = data;
                        colIndex++;
                        data = "";
                    }else if (colIndex == 6){ //viscera_weight
                        viscera_weight[index-1] = data;
                        colIndex++;
                        data = "";
                    }else if (colIndex == 7){ //shell_weight
                        shell_weight[index-1] = data;
                        colIndex++;
                        data = "";
                    }
                }
            }
        }
        //printVectorString(columns);

        index++;
    }
    // printVectorString(columns);
    // printVectorString(sex);
    // printVectorString(age);
}

void convertStringVecToDouble(std::vector<std::string>& stringVec, std::vector<double>& doubleVec){
    std::transform(stringVec.begin(), stringVec.end(), doubleVec.begin(), [](const std::string& val)
    {
        return std::stod(val);
    });
}
void convertStringVecToInt(std::vector<std::string>& stringVec, std::vector<int>& intVec){
    std::transform(stringVec.begin(), stringVec.end(), intVec.begin(), [](const std::string& val)
    {
        return std::stoi(val);
    });
}


int main(){
    MPI_Init(NULL, NULL); 

    int fileLength = 3893, trainingDataLength = 3407, testingDataLength = 486, indexForTraining = 0; // I did this manually but it should be 80/20
    //Holds column names (shouldn't be needed potentially but I'll take them)
    std::vector<std::string> columns(9);
    //String Vectors used to simplify reading process
    std::vector<std::string> sex(fileLength), lengthStr(fileLength), diameterStr(fileLength), heightStr(fileLength), weightStr(fileLength),shucked_weightStr(fileLength), viscera_weightStr(fileLength), shell_weightStr(fileLength), ageStr(fileLength);

    //Converted Data 
    std::vector<double> length(fileLength), diameter(fileLength), height(fileLength), weight(fileLength),shucked_weight(fileLength), viscera_weight(fileLength), shell_weight(fileLength);
    std::vector<int> age(fileLength);

    //Training Data
    std::vector<double> lengthTrain(trainingDataLength), diameterTrain(trainingDataLength), heightTrain(trainingDataLength), weightTrain(trainingDataLength),shucked_weightTrain(trainingDataLength), viscera_weightTrain(trainingDataLength), shell_weightTrain(trainingDataLength);
    std::vector<int> ageTrain(trainingDataLength);
    std::vector<std::string> sexTrain(trainingDataLength);

    //Testing Data
    std::vector<double> lengthTest(testingDataLength), diameterTest(testingDataLength), heightTest(testingDataLength), weightTest(testingDataLength),shucked_weightTest(testingDataLength), viscera_weightTest(testingDataLength), shell_weightTest(testingDataLength);
    std::vector<int> ageTest(testingDataLength);
    std::vector<std::string> sexTest(testingDataLength);
    

    //MPI Declarations
    int numProcs, myRank, myTrainingSize, myTestingSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    //Local MPI Training
    std::vector<double> myLengthTrain(trainingDataLength/numProcs), myDiameterTrain(trainingDataLength/numProcs), myHeightTrain(trainingDataLength/numProcs), myWeightTrain(trainingDataLength/numProcs), myShucked_weightTrain(trainingDataLength/numProcs), myViscera_weightTrain(trainingDataLength/numProcs), myShell_weightTrain(trainingDataLength/numProcs);
    std::vector<int> myAgeTrain(trainingDataLength/numProcs);
    std::vector<std::string> mySexTrain(trainingDataLength/numProcs);
    //Local MPI Testing
    std::vector<double> myLengthTest(testingDataLength/numProcs), myDiameterTest(testingDataLength/numProcs), myHeightTest(testingDataLength/numProcs), myWeightTest(testingDataLength/numProcs),myShucked_weightTest(testingDataLength/numProcs), myViscera_weightTest(testingDataLength/numProcs), myShell_weightTest(testingDataLength/numProcs);
    std::vector<int> myAgeTest(testingDataLength/numProcs);
    std::vector<std::string> mySexTest(testingDataLength/numProcs);

    if(myRank==0){
        //Read in my dataset
        readData(columns, sex, lengthStr, diameterStr, heightStr, weightStr,shucked_weightStr, viscera_weightStr, shell_weightStr, ageStr); 

        //Convert String vectors to double vectors and int vectors
        convertStringVecToDouble(lengthStr, length); convertStringVecToDouble(diameterStr, diameter); convertStringVecToDouble(heightStr, height);
        convertStringVecToDouble(weightStr, weight); convertStringVecToDouble(shucked_weightStr, shucked_weight); convertStringVecToDouble(viscera_weightStr, viscera_weight); 
        convertStringVecToDouble(shell_weightStr, shell_weight); convertStringVecToInt(ageStr, age);

        //Split for testing
        for(int i = 0; i < testingDataLength; i++){
            lengthTest[i] = length[i];
            diameterTest[i] = diameter[i];
            heightTest[i] = height[i];
            weightTest[i] = weight[i];
            shucked_weightTest[i] = shucked_weight[i];
            viscera_weightTest[i] = viscera_weight[i];
            shell_weightTest[i] = shell_weight[i];
            ageTest[i] = age[i];
            sexTest[i] = sex[i];
        }
        //Split for training 
        for(int i=testingDataLength; i < fileLength; i++){
            lengthTrain[indexForTraining] = length[i];
            diameterTrain[indexForTraining] = diameter[i];
            heightTrain[indexForTraining] = height[i];
            weightTrain[indexForTraining] = weight[i];
            shucked_weightTrain[indexForTraining] = shucked_weight[i];
            viscera_weightTrain[indexForTraining] = viscera_weight[i];
            shell_weightTrain[indexForTraining] = shell_weight[i];
            ageTrain[indexForTraining] = age[i];
            sexTain[indexForTraining] = sex[i]
            indexForTraining++;
        }
    }
    
    //Scatter Training Data
    MPI_Scatter(lengthTrain.data(), myTrainingSize, MPI_DOUBLE, myLengthTrain.data(), myTrainingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(diameterTrain.data(), myTrainingSize, MPI_DOUBLE, myDiameterTrain.data(), myTrainingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(heightTrain.data(), myTrainingSize, MPI_DOUBLE, myHeightTrain.data(), myTrainingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(weightTrain.data(), myTrainingSize, MPI_DOUBLE, myWeightTrain.data(), myTrainingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(shucked_weightTrain.data(), myTrainingSize, MPI_DOUBLE, myShucked_weightTrain.data(), myTrainingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(viscera_weightTrain.data(), myTrainingSize, MPI_DOUBLE, myViscera_weightTrain.data(), myTrainingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(shell_weightTrain.data(), myTrainingSize, MPI_DOUBLE, myShell_weightTrain.data(), myTrainingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(ageTrain.data(), myTrainingSize, MPI_INT, myAgeTrain.data(), myTrainingSize, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(sexTrain.data(), myTrainingSize, MPI_CHAR, myAgeTrain.data(), myTrainingSize, MPI_CHAR, 0, MPI_COMM_WORLD);
    
    //Scatter TestingData
    MPI_Scatter(lengthTest.data(), myTestingSize, MPI_DOUBLE, myLengthTest.data(), myTestingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(diameterTest.data(), myTestingSize, MPI_DOUBLE, myDiameterTest.data(), myTestingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(heightTest.data(), myTestingSize, MPI_DOUBLE, myHeightTest.data(), myTestingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(weightTest.data(), myTestingSize, MPI_DOUBLE, myWeightTest.data(), myTestingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(shucked_weightTest.data(), myTestingSize, MPI_DOUBLE, myShucked_weightTest.data(), myTestingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(viscera_weightTest.data(), myTestingSize, MPI_DOUBLE, myViscera_weightTest.data(), myTestingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(shell_weightTest.data(), myTestingSize, MPI_DOUBLE, myShell_weightTest.data(), myTestingSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(ageTest.data(), myTestingSize, MPI_INT, myAgeTest.data(), myTestingSize, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(sexTest.data(), myTestingSize, MPI_CHAR, myAgeTest.data(), myTestingSize, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Finalize(); 

    return 0;
}
