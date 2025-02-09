from fastapi import FastAPI
from fastapi.responses import FileResponse
from pydantic import BaseModel
import numpy as np
from typing import List
import matplotlib
import matplotlib.pyplot as plt
import torch
import isolation_forest
import json

matplotlib.use('Agg')  # Usar backend no interactivo
app = FastAPI()

# Definir el modelo para el vector
class VectorF(BaseModel):
    vector: List[float]
    
@app.post("/isolation-forest")
def calculo(dataSize: int, numTrees: int, maxDepth:int):
    output_file = 'isolation_forest.png'
    np.random.seed(42)
    normal_data = np.random.randn(dataSize, 2)
    anomalies = np.random.uniform(low=-4, high=4, size=(5, 2))
    data = np.vstack([normal_data, anomalies])

    # Convertir a lista para C++
    data_list = data.tolist()

    # Entrenar el modelo
    num_trees=numTrees
    max_depth=maxDepth
    model = isolation_forest.IsolationForest(num_trees, max_depth)
    model.fit(data_list)
    scores = model.predict(data_list)

    # Graficar
    plt.figure(figsize=(8, 6))
    plt.scatter(data[:, 0], data[:, 1], c=scores, cmap="coolwarm", edgecolors="k")
    plt.colorbar(label="Anomaly Score")
    plt.title("Detección de Anomalías con Isolation Forest")
    plt.xlabel("Feature 1")
    plt.ylabel("Feature 2")
    #plt.show()
    plt.savefig(output_file)
    plt.close()
    
    j1 = {
        "Grafica": output_file
    }
    jj = json.dumps(str(j1))

    return jj

@app.get("/isolation-forest-graph")
def getGraph(output_file: str):
    return FileResponse(output_file, media_type="image/png", filename=output_file)