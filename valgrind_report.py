'''This script executes a C program with make various times with different args and saves the valgrind output for further analysis'''

import os
from dataclasses import dataclass
import subprocess
import matplotlib.pyplot as plt

MAX_N = 12 # 2^12 = 4096
MAX_B = 7 # 2^7 = 128, 2, 4, 8, 16, 32, 64, 128

@dataclass
class ValgrindReport:
    '''Dataclass to store the valgrind report'''
    n: int
    b: int
    i1_miss_rate: float
    lli_miss_rate: float
    d1_miss_rate: dict
    lld_miss_rate: dict


def run_valgrind(n, b) -> ValgrindReport:
    '''Run the valgrind command with the given n and b values'''
    cmd: str = ''
    if b == 0 or b == 1:
        cmd = f'make valgrind_single N={n}'
    else:
        cmd = f'make valgrind_single N={n} B={b}'
    out = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    i1_miss_rate = None
    lli_miss_rate = None
    d1_miss_rate = None
    lld_miss_rate = None
    for line in out.stderr.split('\n'):
        if 'miss' in line and 'rate:' in line:
            if 'I1' in line:
                i1_miss_rate = line.split()[4]
            if 'LLi' in line:
                lli_miss_rate = line.split()[4]
            if 'D1' in line:
                d1_miss_rate = { 'avg': line.split()[4], 'read': line.split()[6], 'write': line.split()[8] }
            if 'LLd' in line:
                lld_miss_rate = { 'avg': line.split()[4], 'read': line.split()[6], 'write': line.split()[8] }
    return ValgrindReport(n, b, i1_miss_rate, lli_miss_rate, d1_miss_rate, lld_miss_rate)

def main():
    '''Main function of the script'''
    reports: list[ValgrindReport] = []
    for n in range(2, MAX_N + 1):
        for b in [2**i for i in range(0, MAX_B + 1)]: # 2, 4, 8, 16, 32, 64, 128
            print(f'Running valgrind for N={n} and B={b}')
            reports.append(run_valgrind(n, b))
    # Now save the reports and write them all down
    with open('valgrind_report.txt', 'w') as f:
        for report in reports:
            f.write(f'N={report.n} B={report.b}\n')
            f.write(f'I1 miss rate: {report.i1_miss_rate}\n')
            f.write(f'LLi miss rate: {report.lli_miss_rate}\n')
            f.write(f'D1 miss rate: {report.d1_miss_rate}\n')
            f.write(f'LLd miss rate: {report.lld_miss_rate}\n')
            f.write('\n')
    # Also graph the entire thing with matplotlib
    # We will plot the D1 and LLd miss rate lines (y axis) against the B values (x axis) for a given N
    for idx in range(2, MAX_N + 1):
        fig, ax = plt.subplots()
        y = [float(report.d1_miss_rate['avg'].strip("%")) for report in reports if report.n == idx]
        x = ['S'] + [f'B{2**i}' for i in range(1, MAX_B + 1)]
        x = x[:len(y)]
        ax.plot(x, y, label='D1 miss rate')
        y = [float(report.lld_miss_rate['avg'].strip("%")) for report in reports if report.n == idx]
        ax.plot(x, y, label='LLd miss rate')
        y = [float(report.lld_miss_rate['read'].strip("%")) for report in reports if report.n == idx]
        ax.plot(x, y, label='LLd read miss rate')
        y = [float(report.lld_miss_rate['write'].strip("%")) for report in reports if report.n == idx]
        ax.plot(x, y, label='LLd write miss rate')
        ax.set_xlabel('Block Size')
        ax.set_ylabel('Miss Rate (%)')
        ax.set_title(f'Cache Miss rates for N={2**idx}')
        ax.legend()
        plt.savefig(f'images/N_{2**idx}.png')
        plt.show()
    # Now plot the same thing (Block Size over Miss Rate) but plot for each N value only the D1 miss rate
    fig, ax = plt.subplots()
    for idx in range(2, MAX_N + 1):
        y = [float(report.d1_miss_rate['avg'].strip("%")) for report in reports if report.n == idx]
        x = ['S'] + [f'B{2**i}' for i in range(1, MAX_B + 1)]
        x = x[:len(y)]
        ax.plot(x, y, label=f'N={2**idx}')
    ax.set_xlabel('Block Size')
    ax.set_ylabel('Miss Rate (%)')
    ax.set_title(f'D1 Miss rates for different N values')
    ax.legend()
    plt.savefig('images/d1_miss_rates.png')
    plt.show()
    # Now for LLd miss rate
    fig, ax = plt.subplots()
    for idx in range(2, MAX_N + 1):
        y = [float(report.lld_miss_rate['avg'].strip("%")) for report in reports if report.n == idx]
        x = ['S'] + [f'B{2**i}' for i in range(1, MAX_B + 1)]
        x = x[:len(y)]
        ax.plot(x, y, label=f'N={2**idx}')
    ax.set_xlabel('Block Size')
    ax.set_ylabel('Miss Rate (%)')
    ax.set_title(f'LLd Miss rates for different N values')
    ax.legend()
    plt.savefig('images/lld_miss_rates.png')
    plt.show()


if __name__ == '__main__':
    main()
