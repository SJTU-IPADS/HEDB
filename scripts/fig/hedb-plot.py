# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import sys

import argparse
# from util_py3.data_util import *
from util import parse_args


# data_filename = './TPCH Query Results (client & server) - v4.xlsx' # default data_filename

def plot_replay(data_filename, sheet_name='s=1_vm_replay', eps_filename='replay.eps'):
    print('show replay overhead')
    data = pd.read_excel(
        data_filename,
        sheet_name,
        engine='openpyxl'
    )

    query = data['Query']
    insecure = data['Vanilla (w/o encryption)'].astype('float64')
    encrypted = data['UDF-based replay'].astype('float64')
    replay = data['Log-based replay'].astype('float64')

    data_norm = []
    for i in range(len(query)):
        data_norm.append((insecure[i], replay[i], encrypted[i]))

    dim = len(data_norm[0])
    w = 0.9
    dimw = w / dim
    
    fig, ax = plt.subplots()

    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.tick_params(bottom=False, left=False)

    ax.set_yscale('log', base=10)

    ax.grid(axis='y', linestyle = '--', linewidth = 0.5, color='#cccccc')
    ax.set_axisbelow(True)
    # ax.set_xlabel('Query')

    x = np.arange(len(data_norm))
    for i in range(len(data_norm[0])):
        y = [d[i] for d in data_norm]
        if i == 1 :
            b = ax.bar(x + i * dimw, y, dimw, edgecolor = '#FFFFFF', color = '#C82423', label="Log-based replay") # encrypted
        elif i == 2 :
            b = ax.bar(x + i * dimw, y, dimw, edgecolor = '#FFFFFF', color = '#FFBE7A', label='UDF-based replay') # replay
        else:
            b = ax.bar(x + i * dimw, y, dimw, edgecolor = '#FFFFFF', color = '#A1A9D0', label='Vanilla (w/o encryption)') 

    plt.xticks(x + dimw, query, fontsize=17)
    plt.yticks(fontsize=17)

    # for a, b in zip(x, data_norm):
    #     for i in range(len(b)):
    #         plt.text(a + i * dimw, b[i] + 1.4, '%.1f' % b[i] + "x" , ha='center', rotation=90, fontsize=9)

    ax.legend(fontsize=19, ncol=2, edgecolor = '#000000', loc='upper right')
    ax.set_ylabel('Query Time (ms)', fontsize=19)

    fig.set_size_inches(10, 4.5, forward=True)
    fig.tight_layout()
    
    plt.savefig(eps_filename, dpi=1000)


def plot_exp(data_filename, sheet_name='exp_s=0.1', eps_filename='exp.eps'):
    print('expression')
    data = pd.read_excel(
        data_filename,
        engine='openpyxl',
    )

    query = data['num']
    time = data['avg'].astype('float64')

    new_exps_time = []
    normal_exps_time = []
    for q, t in zip(query, time):
        if q[0] == 'e':
            new_exps_time.append(t)
        else:
            normal_exps_time.append(t)

    x = [2, 3, 4, 5, 6]

    fig, ax = plt.subplots()
    ax.plot(x, new_exps_time, label='new_exp', linestyle='dashdot', marker='o')
    ax.plot(x, normal_exps_time, label='normal_exp', linestyle='dashed', marker='o')

    plt.xticks(x)
    plt.yticks(rotation=-45)

    ax.grid(linewidth = 0.5, color='#cccccc')
    ax.set_axisbelow(True)

    ax.set_xlabel('Number of operands')
    ax.set_ylabel('Execution time(ms)')

    ax.legend(title='expression type')

    fig.tight_layout()
    plt.savefig(eps_filename, dpi=1000)


def plot_optimization(data_filename, sheet_name='s=1_vm', eps_filename='optimization.eps'):
    print('show optimization')
    data = pd.read_excel(
        data_filename,
        sheet_name,
        engine='openpyxl'
    )

    query = data['Query']
    insecure = data['Native'].astype('float64')
    encrypted = data['ARM-version StealthDB'].astype('float64')
    parallel = data['w/ O1(parallel)'].astype('float64')
    order = data['w/ O2(order)'].astype('float64')
    expression = data['w/ O3(expression)'].astype('float64')
    optimization = data['w/ HEDB\'s optimization'].astype('float64')

    data_norm = []
    for i in range(len(query)):
        data_norm.append((encrypted[i]/insecure[i], 
            parallel[i]/insecure[i], 
            order[i]/insecure[i], 
            expression[i]/insecure[i], 
            optimization[i]/insecure[i])
        )

    dim = len(data_norm[0])
    w = 0.9
    dimw = w / dim
    
    fig, ax = plt.subplots()

    ax.set_yscale('log', base=2)

    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.tick_params(bottom=False, left=False)

    ax.grid(axis='y', linestyle = '--', linewidth = 0.5, color='#cccccc')
    ax.set_axisbelow(True)
    # ax.set_xlabel('Query')

    x = np.arange(len(data_norm))
    for i in range(len(data_norm[0])):
        y = [d[i] for d in data_norm]
        if i == 0 :
            b = ax.bar(x + i * dimw, y, dimw, edgecolor = '#FFFFFF', color = '#D73027', label='ARM-version StealthDB') # BASELINE
        elif i == 1:
            b = ax.bar(x + i * dimw, y, dimw, edgecolor = '#FFFFFF', color = '#F46D43', label='w/ O1 (Parallel decryption)') # O1(parallel)
        elif i == 2:
            b = ax.bar(x + i * dimw, y, dimw, edgecolor = '#FFFFFF', color = '#4575B4', label='w/ O2 (Order-revealing encryption)') # O2(order)
        elif i == 3:
            b = ax.bar(x + i * dimw, y, dimw, edgecolor = '#FFFFFF', color = '#74ADD1', label='w/ O3 (Expression evaluation)') # O3(expression)
        else:
            b = ax.bar(x + i * dimw, y, dimw, edgecolor = '#FFFFFF', color = '#E0F3F8', label='w/ O1+O2+O3') # OPTIMIZED

    plt.xticks(x + dimw * 2, query, fontsize=15)
    plt.yticks(fontsize=15)

    upper_bound = 80
    ax.set_ybound(upper=upper_bound)
    ax.legend(fontsize=12, ncol=3, edgecolor = '#000000', loc='upper center', bbox_to_anchor=(0.42,1.2,0,0))
    # ax.legend(fontsize=11, edgecolor = '#000000', loc='upper center', bbox_to_anchor=(0.4,1.25,0,0))
    ax.set_ylabel('Normalized Overhead', fontsize=18)
    for a, b in zip(x, data_norm):
        for i in range(len(b)):
            if b[i] > upper_bound:
                plt.text(a + i * dimw, upper_bound + 1.4, '%.1f' % b[i] + "x" , ha='center', rotation=90, fontsize=9)
            else:
                plt.text(a + i * dimw, b[i] + 1.4, '%.1f' % b[i] + "x" , ha='center', rotation=90, fontsize=9)

    fig.set_size_inches(16, 3.8, forward=True)
    fig.tight_layout()
    plt.savefig(eps_filename, dpi=1000, format='eps')


def plot_record(data_filename, sheet_name = 's=1_vm', eps_filename = 'record.eps'):
    print('record overhead')
    data = pd.read_excel(
        data_filename,
        # sheet_name='s=1_vm',
        sheet_name,
        engine='openpyxl',
    )

    columns = data.columns.values

    query = data['Query']
    encrypted = data['ARM-version StealthDB'].astype('float64')
    record = data['w/ Record'].astype('float64')

    data_norm = []
    for i in range(len(query)):
        data_norm.append((record[i]/encrypted[i]-1)*100)
    
    w = 0.55

    fig, ax = plt.subplots()

    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.tick_params(bottom=False, left=False)

    ax.grid(axis='y', linestyle = '--', linewidth = 0.5, color='#cccccc')
    ax.set_axisbelow(True)
    # ax.set_xlabel('Query')

    x = np.arange(len(data_norm))
    ax.bar(x, data_norm, w, hatch='', edgecolor='#FFFFFF', color='#82B0E2')
    ax.bar(x, data_norm, w, edgecolor='k', color='none')

    plt.xticks(x, query, fontsize=17)
    plt.yticks(fontsize=17)

    ax.set_ylabel('Normalized Record Overhead', fontsize=19)

    import matplotlib.ticker as mtick
    fmt='%.1f%%'
    yticks = mtick.FormatStrFormatter(fmt)
    ax.yaxis.set_major_formatter(yticks)

    fig.set_size_inches(10, 4.5, forward=True)
    fig.tight_layout()
    plt.savefig(eps_filename, dpi=1000, format='eps')


def plot_desenitize(data_filename,sheet_name = 'desenitize', eps_filename = 'desenitize.eps'):
    print('show hedb desenitize')
    data = pd.read_excel(
        data_filename,
        sheet_name,
        engine='openpyxl',
    )
    print(data)
    query = data['query']
    native = data['native-query-time'].astype('float64')
    before = data['before opt'].astype('float64')
    after = data['after opt'].astype('float64')

    data_norm = []
    for i in range(len(query)):
        if native[i] != 0 :
            data_norm.append((before[i]/ native[i], after[i]/native[i]))
        else:
            data_norm.append((0, 0))

    dim = len(data_norm[0])
    w = 0.9
    dimw = w / dim
    
    fig, ax = plt.subplots()

    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.tick_params(bottom=False, left=False)

    ax.grid(axis='y', linestyle = '--', linewidth = 0.5, color='#cccccc')
    ax.set_axisbelow(True)

    x = np.arange(len(data_norm))
    for i in range(len(data_norm[0])):
        y = [d[i] for d in data_norm]
        if i == 1 :
            b = ax.bar(x + i * dimw, y, dimw, edgecolor = '#FFFFFF', color = '#74ADD1', label='w/ optimizations') # optimized
        else :
            b = ax.bar(x + i * dimw, y, dimw, hatch = '///', edgecolor = '#FFFFFF', color = '#F46D43', label='KLEE execution and Z3 generation')

    plt.xticks(x + dimw/2, query, fontsize=17)
    plt.yticks(fontsize=17)
    ax.set_yscale('log')

    upper_bound = 10000000
    ax.set_ybound(upper=upper_bound)
    # ax.legend(fontsize=11, edgecolor = '#000000', loc='upper center', bbox_to_anchor=(0.4,1.25,0,0))
    for a, b in zip(x, data_norm):
        if a == 17:
        # for i in range(len(b)):
            plt.text(a + dimw/2, 500, 'Not Supported (Float-point Only)', ha='center', rotation=90, fontsize=15)

    ax.legend(fontsize=19, edgecolor = '#000000', loc='upper center', bbox_to_anchor=(0.34,1.03,0,0))
    # ax.set_ylabel('Normalized to optimized HEDB', fontsize=15)
    ax.set_ylabel('Data anonymization cost (s)', fontsize=19)

    fig.set_size_inches(10, 4.5, forward=True)
    fig.tight_layout()
    plt.savefig(eps_filename, dpi=1000, format='eps')

def main():
    if len(sys.argv) <= 2:
        print('usage python3 plot.py [graph] [datafile] e.g., python3 plot.py replay data.xlsx')
        return
    
    parser = argparse.ArgumentParser(description='basic graph plot')
    args = parse_args(parser)
    in_name, out_name = args.input, args.output
    print(in_name, out_name)
    plt.rcParams['hatch.linewidth'] = 1.5
    # if len(sys.argv) > 2:
    data_filename = in_name
    eps_filename = out_name
    
    if args.figure == 'replay':
        plot_replay(data_filename, eps_filename=eps_filename)
    if args.figure == 'record':
        plot_record(data_filename, eps_filename=eps_filename)
    if args.figure == 'exp':
        plot_exp(data_filename, eps_filename=eps_filename)
    if args.figure == 'optimization':
        plot_optimization(data_filename, eps_filename=eps_filename)
    if args.figure == 'desenitize':
        plot_desenitize(data_filename, eps_filename=eps_filename)
    
    plt.show()

if __name__ == '__main__':
    main()
