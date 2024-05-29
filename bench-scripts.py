import subprocess
import sys
from tqdm import tqdm
from itertools import product
import os

lens = [1_000_000,
        4_000_000,
        16_000_000,
        64_000_000,
        256_000_000,
        1_024_000_000,
        ]

densities = [0.1, 0.5, 0.9]

num_pos = 70_000_000

if __name__ == '__main__':
    choices = ["rank", "select"]

    if len(sys.argv) < 2:
        print("Usage: python3 bench-scripts.py <choice>")
        print("Choices: rank, select")
        sys.exit(1)

    choice = sys.argv[1]

    if choice not in choices:
        print("Invalid choice")
        sys.exit(1)

    if not os.path.exists("./bench-results"):
        os.makedirs("./bench-results")

    if choice == "rank":
        print("rank9...")
        with open("./bench-results/rank9.csv", "w") as f:
            for l, d in tqdm(list(product(lens, densities))):
                result = subprocess.run("./bin/testrank9 {} {} {}".format(
                    l, num_pos, d), shell=True, stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
                f.write("{},{},{}\n".format(l, d, result))
                f.flush()
    else:
        print("rank9sel...")
        with open("./bench-results/rank9sel.csv", "w") as f:
            for l, d in tqdm(list(product(lens, densities))):
                result = subprocess.run("./bin/testrank9sel {} {} {} {}".format(
                    l, num_pos, d, d), shell=True, stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
                f.write("{},{},{}\n".format(l, d, result))
                f.flush()

        print("simple_select...")
        with open("./bench-results/simple_select.csv", "w") as f:
            for l, d in tqdm(list(product(lens, densities))):
                result = subprocess.run("./bin/testsimplesel3 {} {} {} {}".format(
                    l, num_pos, d, d), shell=True, stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
                f.write("{},{},{}\n".format(l, d, result))
                f.flush()

        print("rank9sel_non_uniform...")
        with open("./bench-results/rank9sel_non_uniform.csv", "w") as f:
            for l, d in tqdm(list(product(lens, densities))):
                result = subprocess.run("./bin/testrank9sel {} {} {} {}".format(
                    l, num_pos, d * 0.01, d * 0.99), shell=True, stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
                f.write("{},{},{}\n".format(l, d, result))
                f.flush()

        print("simple_select_non_uniform...")
        with open("./bench-results/simple_select_non_uniform.csv", "w") as f:
            for l, d in tqdm(list(product(lens, densities))):
                result = subprocess.run("./bin/testsimplesel3 {} {} {} {}".format(
                    l, num_pos, d * 0.01, d * 0.99), shell=True, stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
                f.write("{},{},{}\n".format(l, d, result))
                f.flush()
