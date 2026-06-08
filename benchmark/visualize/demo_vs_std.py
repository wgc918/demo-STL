#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
一张图可视化所有容器、适配器 demo 和 std 在整体上的对比。
使用分组柱状图展示各容器在 10000 数据规模下的核心操作性能对比。
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


def main():
    df = load_all_csvs()
    if df.empty:
        print("No data to visualize. Run the benchmark first.")
        return

    # 选择 10000 数据规模作为代表
    target_size = 10000
    df_filtered = df[df['DataSize'] == target_size]

    if df_filtered.empty:
        # fallback to smallest available
        sizes = sorted(df['DataSize'].unique())
        target_size = sizes[min(1, len(sizes) - 1)]
        df_filtered = df[df['DataSize'] == target_size]

    # 选择核心操作：insert（或 push_back）
    core_ops = ['push_back', 'insert', 'push_front', 'find', 'forward_iterate']
    df_core = df_filtered[df_filtered['Operation'].isin(core_ops)]

    # 为每个容器选择最具代表性的操作
    containers = sorted(df_core['Container'].unique())
    container_labels = []
    demo_times = []
    std_times = []

    for container in containers:
        c_df = df_core[df_core['Container'] == container]
        if c_df.empty:
            # 尝试任何操作
            c_df = df_filtered[df_filtered['Container'] == container]
        if c_df.empty:
            continue

        # 优先使用 push_back/insert
        for op in ['push_back', 'insert']:
            op_df = c_df[c_df['Operation'] == op]
            if not op_df.empty:
                break
        if op_df.empty:
            op_df = c_df

        demo_val = op_df[op_df['Implementation'] == 'demo']['TimeUs']
        std_val = op_df[op_df['Implementation'] == 'std']['TimeUs']

        if len(demo_val) > 0:
            container_labels.append(container)
            demo_times.append(demo_val.values[0])
            std_times.append(std_val.values[0] if len(std_val) > 0 else 0)

    if not container_labels:
        print("No data available for comparison.")
        return

    fig, ax = plt.subplots(figsize=(16, 7))
    x = np.arange(len(container_labels))
    width = 0.35

    bars1 = ax.bar(x - width / 2, demo_times, width, label='demo', color='#2196F3', alpha=0.85)
    bars2 = ax.bar(x + width / 2, std_times, width, label='std', color='#FF9800', alpha=0.85)

    ax.set_xlabel('Container')
    ax.set_ylabel('Time (us)')
    ax.set_title(f'Demo vs STD - Overall Performance Comparison (DataSize={target_size})')
    ax.set_xticks(x)
    ax.set_xticklabels(container_labels, rotation=30, ha='right', fontsize=9)
    ax.legend()
    ax.set_yscale('log')

    # 添加比率标注
    for i, (d, s) in enumerate(zip(demo_times, std_times)):
        if s > 0:
            ratio = d / s
            color = 'red' if ratio > 1.2 else ('green' if ratio < 0.8 else 'gray')
            ax.annotate(f'{ratio:.2f}x', xy=(x[i], max(d, s)),
                        xytext=(0, 5), textcoords="offset points", ha='center',
                        fontsize=8, color=color, fontweight='bold')

    plt.tight_layout()
    plt.savefig(os.path.join(OUTPUT_DIR, 'demo_vs_std_overview.png'), dpi=150)
    plt.close()
    print('Saved: demo_vs_std_overview.png')


if __name__ == '__main__':
    main()