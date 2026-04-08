import numpy as np
import matplotlib.pyplot as plt

def read_data(FILE_NAME: str = "out.dat"):
    with open(FILE_NAME, 'r') as file:
        lines = file.readlines()
        data = []
        for line in lines:
            if line.startswith("Loaded file:"):
                name = line.split("/")[-1].split("|")[0].strip()
                vertex_n = line.split("/")[-1].split(":")[1].strip()
                print(f"{name}, {vertex_n}")
        return data
    
read_data()