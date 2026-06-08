#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
按容器独立绘图：每个容器生成一页多子图（所有操作 × 所有数据规模）。
"""

import os
import glob
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib
import numpy as np

matplotlib.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'DejaVu Sans']
matplotlib.rcParams['axes.unicode_minus'] = False

RESULTS_DIR = os.path.join(os.path.dirname(__file__), '..', 'results')
OUTPUT_DIR = os.path.join(os.path.dirname(__file__), 'output')
os.makedirs(OUTPUT_DIR, exist_ok=True)


def load_all_csvs():
    csv_files = glob.glob(os.path.join(RESULTS_DIR, '*.csv'))
    dfs = []
    for f in csv_files:
        df = pd.read_csv(f)
        dfs.append(df)
    if not dfs:
        return pd.DataFrame()
    return pd.concat(dfs, ignore_index=True)


def plot_per_container(df):
    """为每个容器生成一页多子图"""
    containers = df['Container'].unique()
    for container in containers:
        container_df = df[df['Container'] == container]
        operations = container_df['Operation'].unique()
        data_sizes = sorted(container_df['DataSize'].unique())

        n_ops = len(operations)
        cols = min(3, n_ops)
        rows = (n_ops + cols - 1) // cols

        fig, axes = plt.subplots(rows, cols, figsize=(cols * 6, rows * 4.5))
        fig.suptitle(f'{container} - Complete Performance Profile', fontsize=14, fontweight='bold')

        if rows * cols == 1:
            axes = np.array([[axes]])
        elif rows == 1:
            axes = axes.reshape(1, -1)
        elif cols == 1:
            axes = axes.reshape(-1, 1)

        for idx, op in enumerate(operations):
            r, c = idx // cols, idx % cols
            ax = axes[r, c]
            op_df = container_df[container_df['Operation'] == op]

            x = np.arange(len(data_sizes))
            width = 0.35

            demo_data = []
            std_data = []
            for ds in data_sizes:
                demo_val = op_df[(op_df['DataSize'] == ds) & (op_df['Implementation'] == 'demo')]['TimeUs']
                std_val = op_df[(op_df['DataSize'] == ds) & (op_df['Implementation'] == 'std')]['TimeUs']
                demo_data.append(demo_val.values[0] if len(demo_val) > 0 else 0)
                std_data.append(std_val.values[0] if len(std_val) > 0 else 0)

            ax.bar(x - width / 2, demo_data, width, label='demo', color='#2196F3')
            ax.bar(x + width / 2, std_data, width, label='std', color='#FF9800')
            ax.set_title(f'{op}', fontsize=10)
            ax.set_xticks(x)
            ax.set_xticklabels([str(ds) for ds in data_sizes], fontsize=7)
            ax.set_yscale('log')
            ax.legend(fontsize=7)

        # 隐藏多余的子图
        for idx in range(n_ops, rows * cols):
            r, c = idx // cols, idx % cols
            axes[r, c].set_visible(False)

        plt.tight_layout()
        plt.savefig(os.path.join(OUTPUT_DIR, f'{container}_profile.png'), dpi=150)
        plt.close()
        print(f'Saved: {container}_profile.png')


def main():
    df = load_all_csvs()
    if df.empty:
        print("No data to visualize. Run the benchmark first.")
        return

    print(f"Loaded {len(df)} rows from CSV files.")
    print("Generating per-container profile charts...")
    plot_per_container(df)
    print(f"All charts saved to {OUTPUT_DIR}")


if __name__ == '__main__':
    main()