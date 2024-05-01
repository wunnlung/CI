
def step(wsum):
    if wsum > 0:
        print(1)
        return 1
    else:
        print(0)
        return 0

def perceptron():
    for a in inputs:
        wsum = 0
        b = a
        input1 = [int(i) for i in b]
        for i,w in zip(input1, weights1):
            wsum += i*w
        step(wsum - threshold1)


inputs = ['00000','00001','00010','00011','00100','00101','00110','00111',\
          '01000','01001','01010','01011','01100','01101','01110','01111',\
            '10000','10001','10010','10011','10100','10101','10110','10111',\
            '11000','11001','11010','11011','11100','11101','11110','11111',]

desiredOutputs = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,1]
weights1 = [1, 1, 1, 1, 1]
threshold1 = 4


#perceptron()

#with these weights and threshold, the perceptron only returns
#1 with inputs of 01111, 10111, 11011, 11101, 11110, or 11111.
#how I did it was I saw that for all of those input values,
#they all have at least 4 1's as inputs. 





#####################################################################
#weight training

def nStep(w_sum):
    if w_sum > 0:
        return 1
    else:
        return 0

def nPerceptron(input, weight, threshold):
    weighted_sum = 0
    for x,w in zip(input, weight):
        weighted_sum += x*w
    return nStep(weighted_sum - threshold)


#step 1: set the weights and threshold to random numbers
weights = [.1, .3, -.2, .2, -.4]
threshold = 1
lr = .1

#set the first iteration of P
P = 0
epochCount = 1000
for _ in range(32*epochCount):
    #step 2: actually run the perceptron at iteration P
    #iteration P
    temp = inputs[P]
    inp = [int(i) for i in temp]
    output = nPerceptron(inp, weights, threshold)

    
    #getting the error: 
    desOutput = desiredOutputs[P]
    error = desOutput-output


    #step 3: update the weights
    for num in range(len(weights)):
        #print("num: " + str(num))
        #print("error: " + str(error))
        #print("lr: " + str(lr))
        #print("input: " + str(inp[num]))
        deltaWeight = lr*inp[num]*error
        #print("final delta weight: " + str(deltaWeight))
        #print("preweight: " + str(weights[num]))
        weights[num] += deltaWeight
        #print("final weight:" + str(weights[num]))
        #print("")


    deltaThreshold = lr*inp[num]*error
    threshold += deltaThreshold


    #print(inp)
    #print("output: " + str(output) + "  expected output: " + str(desOutput))
    #if output == 1:
        #print(inp)
        

    #Step 4: going to the next iteration
    if P==31:
        P=0
    else:
        P+=1

print("Weights:")
print(weights)
print("Threshold:")
print(threshold)

#FINALLY as my final weights and threshold i got:
#Weights: [0.5, 0.7, 0.5, 0.5, 0.4]
#Threshold: 1.800000000000007
