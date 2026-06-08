#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
主脚本：读取 results/ 下所有 CSV，为每个容器的每个操作生成独立的柱状图，
并生成一张总览热力图。
"""

import os
import glob
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib
import numpy as np

# 设置中文字体
matplotlib.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'DejaVu Sans']
matplotlib.rcParams['axes.unicode_minus'] = False

RESULTS_DIR = os.path.join(os.path.dirname(__file__), '..', 'results')
OUTPUT_DIR = os.path.join(os.path.dirname(__file__), 'output')
os.makedirs(OUTPUT_DIR, exist_ok=True)


def load_all_csvs():
    """加载所有 CSV 文件"""
    csv_files = glob.glob(os.path.join(RESULTS_DIR, '*.csv'))
    dfs = []
    for f in csv_files:
        df = pd.read_csv(f)
        dfs.append(df)
    if not dfs:
        print("No CSV files found in", RESULTS_DIR)
        return pd.DataFrame()
    return pd.concat(dfs, ignore_index=True)


def plot_per_container_operation(df):
    """为每个容器的每个操作生成独立的柱状图"""
    containers = df['Container'].unique()
    for container in containers:
        container_df = df[df['Container'] == container]
        operations = container_df['Operation'].unique()
        for op in operations:
            op_df = container_df[container_df['Operation'] == op]
            data_sizes = sorted(op_df['DataSize'].unique())

            fig, ax = plt.subplots(figsize=(10, 6))
            x = np.arange(len(data_sizes))
            width = 0.35

            demo_data = []
            std_data = []
            for ds in data_sizes:
                demo_val = op_df[(op_df['DataSize'] == ds) & (op_df['Implementation'] == 'demo')]['TimeUs']
                std_val = op_df[(op_df['DataSize'] == ds) & (op_df['Implementation'] == 'std')]['TimeUs']
                demo_data.append(demo_val.values[0] if len(demo_val) > 0 else 0)
                std_data.append(std_val.values[0] if len(std_val) > 0 else 0)

            bars1 = ax.bar(x - width / 2, demo_data, width, label='demo', color='#2196F3')
            bars2 = ax.bar(x + width / 2, std_data, width, label='std', color='#FF9800')

            ax.set_xlabel('Data Size')
            ax.set_ylabel('Time (us)')
            ax.set_title(f'{container} - {op} Performance Comparison')
            ax.set_xticks(x)
            ax.set_xticklabels([str(ds) for ds in data_sizes])
            ax.legend()
            ax.set_yscale('log')

            # 添加数值标签
            for bar in bars1:
                height = bar.get_height()
                if height > 0:
                    ax.annotate(f'{height:.1f}', xy=(bar.get_x() + bar.get_width() / 2, height),
                                xytext=(0, 3), textcoords="offset points", ha='center', va='bottom',
                                fontsize=7, rotation=90)
            for bar in bars2:
                height = bar.get_height()
                if height > 0:
                    ax.annotate(f'{height:.1f}', xy=(bar.get_x() + bar.get_width() / 2, height),
                                xytext=(0, 3), textcoords="offset points", ha='center', va='bottom',
                                fontsize=7, rotation=90)

            plt.tight_layout()
            filename = f'{container}_{op}.png'
            plt.savefig(os.path.join(OUTPUT_DIR, filename), dpi=150)
            plt.close()
            print(f'Saved: {filename}')


def plot_overview_heatmap(df):
    """生成总览热力图：demo/std 相对性能比值"""
    if df.empty:
        return

    # 计算每个容器每个操作在每个数据规模下的 demo/std 比值
    pivot_data = []
    containers = df['Container'].unique()
    for container in containers:
        container_df = df[df['Container'] == container]
        operations = container_df['Operation'].unique()
        for op in operations:
            op_df = container_df[container_df['Operation'] == op]
            data_sizes = sorted(op_df['DataSize'].unique())
            for ds in data_sizes:
                demo_val = op_df[(op_df['DataSize'] == ds) & (op_df['Implementation'] == 'demo')]['TimeUs']
                std_val = op_df[(op_df['DataSize'] == ds) & (op_df['Implementation'] == 'std')]['TimeUs']
                if len(demo_val) > 0 and len(std_val) > 0 and std_val.values[0] > 0:
                    ratio = demo_val.values[0] / std_val.values[0]
                    pivot_data.append({
                        'Container': container,
                        'Operation': op,
                        'DataSize': ds,
                        'Ratio': ratio
                    })

    if not pivot_data:
        return

    pivot_df = pd.DataFrame(pivot_data)

    # 为每个数据规模创建热力图
    data_sizes = sorted(pivot_df['DataSize'].unique())
    for ds in data_sizes:
        ds_df = pivot_df[pivot_df['DataSize'] == ds]
        # 透视表
        table = ds_df.pivot_table(values='Ratio', index='Container', columns='Operation', aggfunc='mean')

        if table.empty:
            continue

        fig, ax = plt.subplots(figsize=(max(12, len(table.columns) * 1.5),
                                         max(6, len(table.index) * 0.5)))
        im = ax.imshow(table.values, cmap='RdYlGn_r', aspect='auto', vmin=0.5, vmax=2.0)

        ax.set_xticks(np.arange(len(table.columns)))
        ax.set_yticks(np.arange(len(table.index)))
        ax.set_xticklabels(table.columns, rotation=45, ha='right', fontsize=8)
        ax.set_yticklabels(table.index, fontsize=8)

        # 添加数值标注
        for i in range(len(table.index)):
            for j in range(len(table.columns)):
                val = table.values[i, j]
                if not np.isnan(val):
                    color = 'white' if val > 1.5 else 'black'
                    ax.text(j, i, f'{val:.2f}', ha='center', va='center', color=color, fontsize=7)

        ax.set_title(f'Demo/STD Performance Ratio (DataSize={ds})')
        plt.colorbar(im, ax=ax, label='Ratio (demo/std)')
        plt.tight_layout()
        plt.savefig(os.path.join(OUTPUT_DIR, f'overview_heatmap_{ds}.png'), dpi=150)
        plt.close()
        print(f'Saved: overview_heatmap_{ds}.png')


def main():
    df = load_all_csvs()
    if df.empty:
        print("No data to visualize. Run the benchmark first.")
        return

    print(f"Loaded {len(df)} rows from CSV files.")
    print("Generating per-container-operation charts...")
    plot_per_container_operation(df)

    print("Generating overview heatmaps...")
    plot_overview_heatmap(df)

    print(f"All charts saved to {OUTPUT_DIR}")


if __name__ == '__main__':
    main()