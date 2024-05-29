import matplotlib.pyplot as plt
import os
import json
import math
import pandas as pd
import numpy as np

colors = ['b', 'g', 'r', 'c', 'm']
num_of_densities = 3


def load_benches(path):
    df = pd.read_csv(path)
    df.columns = ["size", "dense", "mean"]
    return df


def compare_benches(benches, compare_name):
    fig, ax = plt.subplots(1, len(
        benches), constrained_layout=True, sharex=True, sharey=True, squeeze=False)
    fig.set_size_inches(10, 6)
    fig.text(0.5, -0.02, 'size [num of bits]', ha='center', va='center')
    fig.text(-0.01, 0.5, 'time [ns]', ha='center',
             va='center', rotation='vertical')

    for i, (bench, bench_name) in enumerate(benches):
        for d, (name, group) in enumerate(bench.groupby("dense")):
            ax[0, i].plot(group["size"], group["mean"], label=f"density={float(name)*100}%",
                          color=colors[d], marker="o", markersize=3, linewidth=1.0)
        ax[0, i].set_title(bench_name)
        ax[0, i].grid(True)
        ax[0, i].set_xscale("log")
        ax[0, i].set_yscale("log")

    means = np.sort(np.concatenate(
        list(map(lambda x: x[0]["mean"].unique(), benches)), axis=0))
    ticks = np.logspace(np.log10(means[0]), np.log10(means[-1]), num=6)
    ticks = list(map(lambda x: math.ceil(x), ticks))
    ax[0, 0].set_yticks(ticks)
    ax[0, 0].set_yticklabels(ticks)

    h1, l1 = ax[0, 0].get_legend_handles_labels()
    fig.legend(handles=h1, loc='upper center', bbox_to_anchor=(
        0.5, -0.04), fancybox=True, shadow=True, ncol=5)

    plt.savefig("./plots/{}.svg".format(compare_name),
                format="svg", bbox_inches="tight")
    plt.close(fig)


def compare_select():
    # Compare simple_select and rank9sel
    simple_benches = load_benches("bench-results/simple_select.csv")
    rank9sel_benches = load_benches("bench-results/rank9sel.csv")

    simple_non_uniform_benches = load_benches(
        "bench-results/simple_select_non_uniform.csv")
    rank9sel_non_uniform_benches = load_benches(
        "bench-results/rank9sel_non_uniform.csv")

    compare_benches([(simple_benches, "simple_select"),
                    (rank9sel_benches, "rank9sel"),
                    (simple_non_uniform_benches, "simple_select_non_uniform"),
                    (rank9sel_non_uniform_benches, "rank9sel_non_uniform")], "compare_select")


if __name__ == "__main__":
    compare_select()
