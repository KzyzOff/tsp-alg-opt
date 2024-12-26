import os
import re
import json
import pandas as pd

RESULTS_DIR = '../simulation_results/'


def list_to_recalculate_files() -> list[str]:
    result = []
    for city_result_dir in os.listdir(RESULTS_DIR):
        print(f'--- {city_result_dir} ---')
        agg_path = RESULTS_DIR + city_result_dir + '/aggregated/'
        index = 1
        for file in os.listdir(agg_path):
            if os.path.isdir(file) or not file.endswith('_agg.csv'):
                continue

            with open(agg_path + "/" + file) as csv_file:
                line_count = len(csv_file.readlines())
                if line_count >= 10000:
                    print(f'{index: <3}: {file}')
                    index += 1


def filename_to_settings_dict(filename: str) -> dict:
    settings = dict()

    parts = filename.split(sep='_')
    if len(parts) != 5:
        raise ValueError(f'Wrong number of parts in filename: {filename}.')
    
    cross = parts[3].split('-')
    mut = parts[4].split('-')

    settings['pop_sz'] =        int(parts[0][1:])
    settings['elite_sz'] =      int(parts[1][3:])
    settings['tour_sz'] =       int(parts[2][3:])
    settings['cross_t'] =       parts[3][2]
    settings['mut_t'] =         parts[4][2]
    settings['cross_prob'] =    float(cross[1])
    settings['mut_prob'] =      float(mut[1])

    return settings


def get_simulation_list():
    results_dir = '../simulation_results/'
    suffix_pattern = re.compile(r"(.*)_n[0-9]\.csv$")

    result = dict()

    sim_results_dirs = os.listdir(results_dir)
    for city_dir in sim_results_dirs:
        current_path = results_dir + city_dir

        unique_sims = set()
        for filename in os.listdir(current_path):
            if os.path.isdir(current_path + '/' + filename) or not filename.endswith('.csv'):
                continue

            matched = suffix_pattern.match(filename).group(1)
            unique_sims.add(matched)

        city_sims = []
        for filename in unique_sims:
            city_sims.append(filename_to_settings_dict(filename))
            
        result[city_dir] = city_sims

    return result


output = json.dumps(get_simulation_list(), indent=4)
print(output)
with open('recalculation.json', 'w') as file:
    json.dump(get_simulation_list(), file, indent=4)
