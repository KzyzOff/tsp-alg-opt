import matplotlib.pyplot as plt
import pandas as pd
import os
import re

RESULTS_PATH = '../simulation_results/'

BEST = {
    'berlin52': 7542,
    'kroA100': 21282,
    'kroA150': 26524,
    'fl417': 11861,
    'ali535': 202339,
    'gr666': 294358
}

# FILES = os.listdir(FULL_FILEPATH)


def average_results(filename: str):
    # sim_files = [f for f in FILES if f.startswith(filename)]
    
    # if not sim_files:
    #     raise ValueError(f'No files found with specified name: {filename}')
    
    data_frames = []
    for i in range(10):
        df = pd.read_csv(filename + f'_n{i}.csv', sep=';',
                         usecols=[0, 1, 2, 3],
                         names=['generation', 'best', 'mean', 'worst'],
                         header=None)
        data_frames.append(df)

    combined_data = pd.concat(data_frames)
    averaged_data = combined_data.groupby("generation", as_index=False).mean()

    return averaged_data


def plot_results(data, dataset_name: str, output_path):
    plt.plot(data['generation'], data['best'], label='Best')
    plt.plot(data['generation'], data['mean'], label='Mean')
    plt.plot(data['generation'], data['worst'], label='Worst')
    plt.plot(data['generation'], [BEST[dataset_name] for _ in data['generation']], '--', label='Optimal')
    plt.xlabel('Generation')
    plt.ylabel('Fitness')
    plt.legend(loc='upper right')
    plt.ylim(data['best'].min() * 0.9, data['worst'].max() * 1.1)
    plt.xlim(0, data['generation'].max())
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output_path + '.png', format='png')


def process_sims():
    suffix_pattern = re.compile(r"(.*)_n[0-9]\.csv$")

    sim_results_dirs = os.listdir(RESULTS_PATH)
    for dir in sim_results_dirs:
        current_path = RESULTS_PATH + dir
        output_dir = current_path + '/aggregated/'
        if not os.path.exists(output_dir):
            os.mkdir(output_dir)

        unique_files = set()
        for filename in os.listdir(RESULTS_PATH + dir):
            if os.path.isdir(current_path + '/' + filename):
                continue

            matched = suffix_pattern.match(filename).group(1)
            unique_files.add(matched)

        for filename in unique_files:
            averaged = average_results(current_path + '/' + filename)
            averaged.to_csv(output_dir + filename + '_av.csv', sep=';', index=False, header=None)
            plot_results(averaged, dir, output_dir + filename)


process_sims()
