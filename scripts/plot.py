import pandas as pd
import matplotlib.pyplot as plt

# -------- BUILD TIMES --------
df = pd.read_csv("../results/build_times.csv")

for dataset in df["dataset"].unique():
    subset = df[df["dataset"] == dataset]

    for metodo in ["nearest", "str"]:
        m = subset[subset["metodo"] == metodo]
        plt.plot(m["N"], m["tiempo_ms"], label=metodo)

    plt.title(f"Tiempo de construcción ({dataset})")
    plt.xlabel("N")
    plt.ylabel("Tiempo (ms)")
    plt.legend()
    plt.xscale("log")
    plt.savefig(f"../results/build_{dataset}.png")
    plt.clf()


# -------- QUERY IO --------
df = pd.read_csv("../results/query_results.csv")

for dataset in df["dataset"].unique():
    for metodo in ["nearest", "str"]:
        subset = df[(df["dataset"] == dataset) & (df["metodo"] == metodo)]
        plt.plot(subset["s"], subset["avg_io"], label=metodo)

    plt.title(f"I/O promedio ({dataset})")
    plt.xlabel("s")
    plt.ylabel("Lecturas a disco")
    plt.legend()
    plt.savefig(f"../results/io_{dataset}.png")
    plt.clf()


# -------- POINTS --------
for dataset in df["dataset"].unique():
    for metodo in ["nearest", "str"]:
        subset = df[(df["dataset"] == dataset) & (df["metodo"] == metodo)]
        plt.errorbar(subset["s"], subset["avg_points"],
                     yerr=subset["std_points"], label=metodo)

    plt.title(f"Puntos encontrados ({dataset})")
    plt.xlabel("s")
    plt.ylabel("Cantidad de puntos")
    plt.legend()
    plt.savefig(f"../results/points_{dataset}.png")
    plt.clf()