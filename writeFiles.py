import os
import csv
import pandas as pd

lignes = ["Ligne 1\n", "Ligne 2\n", "Ligne 3\n"]

def writInFiles(word,id):
    a = word+"\n"
    with open("repas"+id+".txt", "a", encoding="utf-8") as f:
        f.writelines(a)
def readSaves(file):
    with open(file, 'r+') as f:
        id = f.read()
        if(id == ''):
            id = 0
            return id
        else:
            return id

def writesave(id, file):
    f = open(file, 'w')
    f.write(id)

def changeSave(file):
    if (os.path.exists(file)):
        id = readSaves(file)
        id = int(id)
        newId = int(id) + 1
        writesave(str(newId),file)
    else:
        print("erreur, le fichier n'existais pas nous allons en créer un ")
        open(file,'w')
        writesave(str(1),file)
changeSave("sauvegarde.txt")


import pandas as pd

def getTuples(fichier_csv):
    df = pd.read_csv(fichier_csv, header=None)  # header=None si le fichier n'a pas d'en-tête
    donnees = df.iloc[:, 0].astype(float).tolist()  # Convertir en liste Python
    return donnees

donnees = getTuples("Classeur1.csv")

print(donnees)