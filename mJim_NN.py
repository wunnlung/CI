import numpy as np
import random
#
#
#
#
#update logs: changed most of the variables (mainly in for loops) so that they are more universal
#for things like hidden outputs
#it should still be able to work regardless of how many inputs there are, we would just need to adjust the weight list size
#
#here is the format for the changed random weights and thresholds (with 0's instead of randoms so u can read it)
#weights:
#[[0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0]]
#
#thresholds:
#[0, 0, 0, 0, 0, 0]
#
#
#there are going to be a lot of inputs
inputs = ['00000','00001','00010','00011','00100','00101','00110','00111',\
          '01000','01001','01010','01011','01100','01101','01110','01111',\
            '10000','10001','10010','10011','10100','10101','10110','10111',\
            '11000','11001','11010','11011','11100','11101','11110','11111',]

#the desired output is going to be 1 pretty much, and it's going to just be how close the hit is
desiredOutputs = [1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,1,1,1]
lr = .25
epochCount = 1000
#how many neurons
hiddenOutputs = [0,0,0,0,0]


#getting the random weights
weights = []
for j in range(len(hiddenOutputs)):
    temp = []
    for l in range(5):
        temp.append(random.random())
    weights.append(temp)
temp2 = []
for i in range(5):
    temp2.append(random.random())
weights.append(temp2)

#random thresholds
thresholds = []
for p in range(6):
    thresholds.append(random.random())

#print("weights:")
#print(weights)
#print("\nthresholds:")
#print(thresholds)


def sigmoid(x):
    return 1/(1+np.exp(-x))



for _ in range(epochCount):
    
    #getting the hidden outputs
    for temp in inputs:
        #this line is just for the formatting
        inp = [int(i) for i in temp]
        #resetting the hidden outputs
        for num in range(len(hiddenOutputs)):
            hiddenOutputs[num] = 0
        #looping through how many neurons in hidden layer
        for i in range(len(hiddenOutputs)):
            sInputs = 0
            #looping through how many inputs there are
            for p in range(len(inp)):
                #getting the weighted sum of the inputs (for one neuron)
                sInputs += weights[i][p] * inp[p]
            #getting the updated hidden output for each neuron
            hiddenOutputs[i] = sigmoid(sInputs + thresholds[i] * -1)
    
        #getting the final output
        fInput = 0
        fOutput = 0
        for pee in range(len(hiddenOutputs)):
            fInput += hiddenOutputs[pee] * weights[3][pee]
        fOutput = sigmoid(fInput + thresholds[3] * -1)


        #WEIGHT TRAINING!!! (time to get buff)
        #output layer
        error = desiredOutputs[inputs.index(temp)] - fOutput
        dk = fOutput * (1-fOutput) * error
        for hd in range(3):
            weights[3][hd] += lr*hiddenOutputs[hd]*dk

        
        thresholds[3] += lr*-1*dk

        #hidden layer
        for nr in range(3):
            dj = hiddenOutputs[nr] * (1-hiddenOutputs[nr]) * dk * weights[3][nr]
            for weight in range(5):
                weights[nr][weight] += lr * inp[weight] * dj
            
            thresholds[nr] += lr*-1*dj

        
        print(inp)
        print("output: " + str(fOutput) + "  expected output: " + str(desiredOutputs[inputs.index(temp)]))
        #if fOutput == 1:
        #    print(inp)

        