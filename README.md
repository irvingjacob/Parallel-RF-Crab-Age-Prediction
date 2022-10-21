# CS4170_FA2022_Project_MITCHELL

## Tentative Plan

Who knows if I'll actually be developing an RF or not. I just spent some time finding a data set and working on reading a csv. 

- Read CSV into 9 vectors. 
- Split into training and testing vectors
- Create RF 
- Test RF finding AdjR2 or R2 etc
- Parallelize RF compare times
    - MPI is what I'm thinking currently but who knows

## Setup of Repo
- data 
    - holds dataset in CrabAgePrediction.csv 
    - may create training dataset and testing dataset 80:20 split. Or maybe I won't and i'll do it all in the program
- src 
    - main.cpp
        - printVectorString()
            - prints vectors of type string to the console window
        - printVectorDouble() 
            - prints vectors of type double to the console window
        - readData()
            - passes in vectors by reference. Appends values from csv to vector for further manipulation
            - only done by head node
        - Create RF 
- PaperNotes
    - research papers and notes in MD
