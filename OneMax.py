import random
import math

class GeneticAlgorithm:
    def __init__(self, chromosomeSize, populationSize, mutation_rate, crossover_rate):
        self.chromosomeSize = chromosomeSize
        self.populationSize = populationSize
        self.mutation_rate = mutation_rate
        self.crossover_rate = crossover_rate
        self.population = []

    def initialize_population(self):
        for ch in range(self.populationSize):
            tempChromosome = []
            for bit in range(self.chromosomeSize):
                tempChromosome.append(random.randint(0, 1))
            self.population.append(tempChromosome)

    def fromFile(self):
        text = open("population.txt", "r")
        self.population = eval(text.read())
        text.close()
        print(self.population[0])
    
    def toFile(self):
        text = open("population.txt", "w")
        text.write(str(self.population))
        text.close()
        print("saved to file.")

    def fitness(self, chromosome):
        return sum(chromosome) * sum(chromosome)
    
    def aFitness(self):
        sum = 0
        for ch in self.population:
            sum+=self.fitness(ch)
        return sum/self.populationSize


    def parentHelp(self):
        totalFitness = sum(self.fitness(chromosome) for chromosome in self.population)
        #get the fitness threshold
        fitThresh = random.uniform(0, totalFitness)
        cFit = 0
        #loop through the population
        for chromosome in self.population:
            #add the current chromosome fitness to the total fitness
            cFit += self.fitness(chromosome)
            #if the total fitness is greater than the fitness threshold
            if cFit >= fitThresh:
                #break the loop
                return chromosome
        
    
    #parent roulette select
    def selectParents(self):
        parents = []
        #gets the total fitness
        #both parents
        p1 = self.parentHelp()
        p2 = self.parentHelp()
        while p1==p2:
            p2=self.parentHelp()
        
        parents.append(p1)
        parents.append(p2)
        return parents

    def crossover(self, parent1, parent2):
        #only crossover to get one child
        #run the function twice
        crossPoint = random.randint(1, self.chromosomeSize - 1)
        child = parent1[:crossPoint] + parent2[crossPoint:]
        return child

    def mutate(self, chromosome):
        #do it for all the bits in the chromosome
        for i in range(self.chromosomeSize):
            #random.random is from range 0-1
            if random.random() < self.mutation_rate:
                #replace the chromosome
                chromosome[i] = 1 - chromosome[i]
        
        return chromosome


    #pretty much main function
    #runs the rest of the functions
    def evolve(self, generations):
        nPop = []
        #floor division (keep the same population size)
        for _ in range(self.populationSize):
            #get parents
            parents = self.selectParents()
            #Manan said to have crossover rate so its here
            if random.random() <= self.crossover_rate:
                #gets the two children after crossover
                child = self.crossover(parents[0], parents[1])
            #checks for mutation
            child = self.mutate(child)
            #put children in updated population
            nPop.append(child)
        #updatae class population
        self.population = nPop

    def bestChromosome(self):
        cMax = 0
        fMax = 0
        for i in range(self.populationSize):
            if self.fitness(self.population[i]) > fMax:
                cMax = self.population[i]
                fMax = self.fitness(self.population[i])

        return [cMax,fMax]


CHROMOSOMESIZE = 64
POPULATIONSIZE = 100
MUTATIONRATE = 0.001
CROSSOVERRATE = 1

ga = GeneticAlgorithm(CHROMOSOMESIZE, POPULATIONSIZE, MUTATIONRATE, CROSSOVERRATE)

#selecting the input method
fromFile = 1
if fromFile==0:
    ga.initialize_population()
else:
    ga.fromFile()

#resetting the info.txt
temp = open("info.txt", "w")
temp.write("")
temp.close

xGen = 0
#for generation in range(1000):
generation=0
while ga.bestChromosome()[1]!=4096:
    ga.evolve(1)
    temp = ga.bestChromosome()
    print("Generation " + str(generation+1) + ": Best Fitness = " + str(math.sqrt(temp[1])))
    if xGen==50:
        t = open("info.txt", "a")
        t.write("Generation " + str(generation+1) + ": Best Fitness=" + str(temp[1]) + ": " + str(temp[0]))
        t.write("Average Fitness: " + str(ga.aFitness()))
        t.write("\n")
        xGen = -1
    xGen+=1
    generation+=1


#ga.toFile()
print("Best Solution:", temp[0])

