#!/usr/bin/env python3

import subprocess
import sys
from tqdm import tqdm
from itertools import product
import os

# Bit lengths to try

lens = [1_000_000,
        4_000_000,
        16_000_000,
        64_000_000,
        256_000_000,
        1_024_000_000,
        ]

# Densities

densities = [0.1, 0.5, 0.9]

# Number of experiments 

repeats = 7

# Number of tested positions

num_pos = 70_000_000


def run_bench(binary_name, bench_name, uniform=True):
    with open("./bench-results/{}.csv".format(bench_name), "w") as f:
        for l, d in tqdm(list(product(lens, densities))):
            d1, d2 = (d, d) if uniform else (d * 0.01, d * 0.99)
            result = subprocess.run("./bin/{} {} {} {} {} {}".format(
                binary_name, l, repeats, num_pos, d1, d2), shell=True, stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
            f.write("{},{},{}\n".format(l, d, result))
            f.flush()


if __name__ == '__main__':
    choices = ["rank", "select", "select_non_uniform"]

    if len(sys.argv) < 2:
        print("Usage: bench.py [rank|select|select_non_uniform]")
        print()
        print("Run the specified benchmarks and saves CSV files in bench-results")
        print("Please compile the benchmarks with \"make ranksel\" first")
        sys.exit(1)

    choice = sys.argv[1]

    if choice not in choices:
        print("Invalid choice")
        print("Choices: rank, select, select_non_uniform")
        sys.exit(1)

    if not os.path.exists("./bench-results"):
        os.makedirs("./bench-results")

    if choice == "rank":
        print("rank9...")
        run_bench("testrank9", "rank9", uniform=True)
    elif choice == "select":
        print("rank9sel...")
        run_bench("testrank9sel", "select9", uniform=True)

        print("simple_select...")
        run_bench("testsimplesel3", "simple_select", uniform=True)
    elif choice == "select_non_uniform":
        print("rank9sel_non_uniform...")
        run_bench("testrank9sel", "select9_non_uniform", uniform=False)

        print("simple_select_non_uniform...")
        run_bench("testsimplesel3", "simple_select_non_uniform", uniform=False)
