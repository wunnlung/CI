import numpy as np
import random
#
#
#
#
#originally, the weight training worked great, and I was able to get a "final" weight and threshold. But, if we change
#the desired outputs to the new ones, we can see that it unfortunately doesn't work as well. The added outputs of 1 are
#00000, 10000, 01000, 00100, 00010, and 00001. Even after 1000 epochs, it's still spitting out incorrect outputs.
#
#
#
#
inputs = ['00000','00001','00010','00011','00100','00101','00110','00111',\
          '01000','01001','01010','01011','01100','01101','01110','01111',\
            '10000','10001','10010','10011','10100','10101','10110','10111',\
            '11000','11001','11010','11011','11100','11101','11110','11111',]

desiredOutputs = [1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,1,1,1]
lr = .25
epochCount = 1000


#getting the random weights
weights = []
for j in range(3):
    temp = []
    for l in range(5):
        temp.append(random.random())
    weights.append(temp)
temp2 = []
for i in range(3):
    temp2.append(random.random())
weights.append(temp2)

#random thresholds
thresholds = []
for p in range(4):
    thresholds.append(random.random())

print(weights)
print(thresholds)


def sigmoid(x):
    return 1/(1+np.exp(-x))



for _ in range(epochCount):
    
    #getting the hidden outputs
    for temp in inputs:
        inp = [int(i) for i in temp]
        hiddenOutputs = [0,0,0]
        for i in range(3):
            sInputs = 0
            for p in range(5):
                sInputs += weights[i][p] * inp[p]
            hiddenOutputs[i] = sigmoid(sInputs + thresholds[i] * -1)
    
        #getting the final output
        fInput = 0
        fOutput = 0
        for pee in range(3):
            fInput += hiddenOutputs[pee] * weights[3][pee]
        fOutput = sigmoid(fInput + thresholds[3] * -1)


        #WEIGHT TRAINING!!! (time to get buff)
        #output layer
        error = desiredOutputs[inputs.index(temp)] - fOutput
        dk = fOutput * (1-fOutput) * error
        for hd in range(3):
            weights[3][hd] += lr*hiddenOutputs[hd]*dk

        
        thresholds[3] += lr*-1*dk

        for nr in range(3):
            dj = hiddenOutputs[nr] * (1-hiddenOutputs[nr]) * dk * weights[3][nr]
            for weight in range(5):
                weights[nr][weight] += lr * inp[weight] * dj
            
            thresholds[nr] += lr*-1*dj

        
        #print(inp)
        #print("output: " + str(fOutput) + "  expected output: " + str(desiredOutputs[inputs.index(temp)]))
        #if fOutput == 1:
         #   print(inp)

        