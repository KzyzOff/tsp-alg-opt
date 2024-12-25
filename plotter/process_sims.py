import matplotlib.pyplot as plt
import pandas as pd
import os
import re

CURRENT_FILE = 'fl417'
RESULTS_PATH = '../simulation_results/'

OPTIMAL = {
    'berlin52': 7542,
    'kroA100': 21282,
    'kroA150': 26524,
    'fl417': 11861,
    'ali535': 202339,
    'gr666': 294358
}

def average_results(filename: str):
    data_frames = []
    for i in range(10):
        df = pd.read_csv(filename + f'_n{i}.csv', sep=';',
                         usecols=[0, 1, 2, 3],
                         names=['generation', 'best', 'mean', 'worst'],
                         header=None)
        data_frames.append(df)

    combined_data = pd.concat(data_frames)

    aggregated = combined_data.groupby('generation').agg(
        best=('best', 'min'),
        mean=('best', 'mean'),
        worst=('best', 'max')
    ).reset_index()

    return aggregated


def plot_results(data, dataset_name: str, output_path):
    plt.clf()

    plt.plot(data['generation'], data['best'], label='Best')
    plt.plot(data['generation'], data['mean'], label='Mean')
    plt.plot(data['generation'], data['worst'], label='Worst')
    
    plt.xlabel('Generation')
    plt.ylabel('Fitness')

    ymin = data['best'].min()
    if dataset_name in OPTIMAL.keys():
        plt.axhline(y = OPTIMAL[dataset_name], color='r', linestyle='--', label='Optimal')
        ymin = OPTIMAL[dataset_name]

    try:
        plt.ylim(ymin * 0.9, data['worst'].max() * 1.1)
        plt.xlim(0, data['generation'].max())
    except ValueError:
        print(f'Wrong data when trying to save to file: {output_path}')

    plt.legend(loc='upper right')
    plt.grid(True)
    plt.tight_layout()

    plt.savefig(output_path + '.png', format='png')


def process_sims():
    suffix_pattern = re.compile(r"(.*)_n[0-9]\.csv$")

    sim_results_dirs = os.listdir(RESULTS_PATH)
    for dir in sim_results_dirs:
        # if not CURRENT_FILE == dir:
        #     continue

        current_path = RESULTS_PATH + dir
        output_dir = current_path + '/aggregated/'
        if not os.path.exists(output_dir):
            os.mkdir(output_dir)

        unique_files = set()
        for filename in os.listdir(RESULTS_PATH + dir):
            if os.path.isdir(current_path + '/' + filename) or not filename.endswith('.csv'):
                continue

            matched = suffix_pattern.match(filename).group(1)
            unique_files.add(matched)

        for filename in unique_files:
            base_filename = output_dir + filename
            aggregated = average_results(current_path + '/' + filename)
            aggregated.to_csv(base_filename + '_agg.csv', sep=';', index=False, header=None)
                
            plot_results(aggregated, dir, base_filename)


process_sims()
