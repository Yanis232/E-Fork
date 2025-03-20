from writeFiles import getTuples



donnees = getTuples("Classeur1.csv")

def generateTimestamp(frequency, data):
    duration = 0
    timestamp = []
    for _ in range(len(data)):
        duration += frequency
        timestamp.append(duration)
    return timestamp  # Conversion en tuple


def trapeze(point1,point2,Te):
    tmp=(point1+point2)/2
    prim=tmp*Te
    return prim



def calculatePrime(donnees) :
    tab = []
    for i in range(len(donnees)-1):
        tab.append(trapeze(donnees[i], donnees[i+1],0.100))
    return tab
vitesse = calculatePrime(donnees)
position = calculatePrime(vitesse)

print(vitesse)
print(position)