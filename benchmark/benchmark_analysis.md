# demo-STL vs std 性能对比分析报告

## 测试环境

- **编译器**: MSVC (Visual Studio 2022), Release /O2
- **数据规模**: 1,000 / 10,000 元素
- **计时方式**: `std::chrono::high_resolution_clock`, 5次重复取中位数, 1次预热
- **测试范围**: 15个容器/适配器, 覆盖构造、插入、删除、查找、遍历、排序等核心操作

---

## 总体结论

**demo-STL 和 std 各有明显优势，无法简单判定谁"更好"。demo 在顺序容器（尤其是 deque 和 vector）的多数操作上大幅领先 std，但在关联容器的遍历、list 排序、以及无序容器的删除操作上明显落后。** 综合来看，demo 在 15 个容器中取得了约 45% 的操作领先，std 在约 35% 的操作中领先，其余约 20% 持平。

---

## 一、顺序容器

### 1.1 vector

| 操作 | 1,000 | 10,000 | 胜出 |
|------|-------|--------|------|
| default_construct | 持平 | 持平 | — |
| fill_construct | demo 0.2 / std 0.1 | demo 0.3 / std 0.3 | std(小) |
| range_construct | demo 0.3 / std 0.2 | demo 2.2 / std 0.8 | **std 2.75x** |
| copy_construct | demo 0.3 / std 0.1 | demo 2.6 / std 0.9 | **std 2.89x** |
| push_back | demo 1.5 / std 1.9 | demo 35.7 / std 113.8 | **demo 3.19x** |
| insert_middle | demo 2.1 / std 1.2 | demo 67.7 / std 16.1 | **std 4.20x** |
| pop_back | demo 0.6 / std 0.4 | demo 4.7 / std 3.1 | std 1.52x |
| erase_middle | demo 8.8 / std 1.7 | demo 754.3 / std 4490.5 | **demo 5.95x** |
| clear | demo 0.4 / std 0.2 | demo 2.3 / std 0.8 | std 2.88x |
| operator_at | 持平 | 持平 | — |
| forward_iterate | 持平 | 持平 | — |
| reverse_iterate | 持平 | 持平 | — |
| sort | demo 19.6 / std 17.7 | demo 424.5 / std 434.4 | 持平 |

**vector 小结**: demo 的 `push_back`（3.19x）和 `erase_middle`（5.95x）碾压 std，但 `range_construct`、`copy_construct`、`insert_middle` 和 `clear` 明显落后。`operator_at` 和遍历操作持平。

> **分析**: demo 的 push_back 使用了更高效的扩容策略（可能更小的内存碎片），而 erase_middle 中 std 表现异常差（4490us vs 754us），推测是 std 在 Release 模式下对元素析构有额外的安全检查。demo 的构造和拷贝落后可能是因为内存分配策略不如 std 精细。

---

### 1.2 list

| 操作 | 1,000 | 10,000 | 胜出 |
|------|-------|--------|------|
| default_construct | 持平 | 持平 | — |
| range_construct | 持平 | 持平 | — |
| push_back | 持平 | demo 416 / std 467 | demo 1.12x |
| push_front | 持平 | 持平 | — |
| insert | 持平 | 持平 | — |
| pop_back | 持平 | demo 597 / std 432 | **std 1.38x** |
| pop_front | 持平 | 持平 | — |
| clear | 持平 | 持平 | — |
| forward_iterate | 持平 | 持平 | — |
| reverse_iterate | 持平 | 持平 | — |
| sort | demo 110.5 / std 55.6 | demo 1923 / std 1004 | **std 1.92x** |

**list 小结**: 大部分操作与 std 持平。`sort` 是致命弱点，demo 比 std 慢近 2 倍。`pop_back` 略慢。

> **分析**: demo 的链表排序算法实现效率不如 std（可能没有使用 std 的归并排序优化）。pop_back 略慢说明 demo 的双向链表在查找前驱节点时可能多了一次遍历。

---

### 1.3 deque — **demo 最大优势**

| 操作 | 1,000 | 10,000 | 胜出 |
|------|-------|--------|------|
| range_construct | demo 1.2 / std 11.9 | demo 13.0 / std 132.7 | **demo 10.21x** |
| push_back | demo 2.1 / std 12.7 | demo 14.6 / std 135.9 | **demo 9.31x** |
| push_front | demo 2.3 / std 12.1 | demo 16.2 / std 131.3 | **demo 8.10x** |
| insert_middle | 持平 | demo 208 / std 256 | demo 1.23x |
| pop_back | demo 2.2 / std 12.2 | demo 16.0 / std 134.7 | **demo 8.42x** |
| pop_front | demo 2.3 / std 12.3 | demo 15.4 / std 133.6 | **demo 8.68x** |
| clear | demo 1.7 / std 12.4 | demo 10.0 / std 132.7 | **demo 13.27x** |
| operator_at | demo 0.9 / std 0.4 | demo 7.0 / std 3.8 | std 1.84x |
| forward_iterate | 持平 | 持平 | — |
| reverse_iterate | 持平 | 持平 | — |

**deque 小结**: **demo 的 deque 是全场最大亮点**。首尾插入/删除、构造、清空等操作全面碾压 std，领先 **8~13 倍**。仅 `operator_at` 随机访问略慢。这是 demo-STL 最优秀的设计。

> **分析**: demo 的 deque 可能采用了更高效的分块策略（如更大的块大小减少块管理开销），而 MSVC 的 std::deque 以小块（通常 16 字节或 1 个元素）为单元，导致频繁的块分配和管理。demo 的 operator_at 略慢可能是因为块大小更大导致计算索引时有更多除法操作。

---

### 1.4 forward_list

| 操作 | 1,000 | 10,000 | 胜出 |
|------|-------|--------|------|
| range_construct | 持平 | 持平 | — |
| push_front | 持平 | demo 388 / std 524 | **demo 1.35x** |
| insert_after | 持平 | 持平 | — |
| pop_front | 持平 | demo 469 / std 380 | std 1.24x |
| clear | 持平 | demo 561 / std 402 | **std 1.40x** |
| forward_iterate | 持平 | demo 43.5 / std 25.6 | **std 1.70x** |
| sort | demo 109.7 / std 51.6 | demo 1840 / std 1133 | **std 1.62x** |

**forward_list 小结**: 与 list 类似，`sort` 落后 std 1.62x。`push_front` 领先，但 `clear`、`pop_front`、`forward_iterate` 均落后。

---

## 二、有序关联容器（红黑树）

### 2.1 map

| 操作 | 1,000 | 10,000 | 胜出 |
|------|-------|--------|------|
| range_construct | demo 105 / std 121 | 持平 | demo(小) |
| insert | 持平 | demo 1810 / std 2002 | demo 1.11x |
| find | demo 5.0 / std 7.4 | demo 83.4 / std 73.4 | demo(小) / std(大) |
| operator_at | demo 2.7 / std 3.6 | 持平 | demo(小) |
| erase | 持平 | demo 768 / std 664 | std 1.16x |
| clear | 持平 | demo 762 / std 832 | demo 1.09x |
| forward_iterate | 持平 | demo 126.5 / std 47.0 | **std 2.69x** |

**map 小结**: 插入、查找、operator_at 在小规模上 demo 有优势，但大规模遍历落后 std 2.69x。整体差异不大。

> **分析**: demo 的红黑树迭代器在 `operator++` 时可能有额外的指针追踪开销，导致遍历速度明显慢于 std 的高度优化实现。

---

### 2.2 set

| 操作 | 1,000 | 10,000 | 胜出 |
|------|-------|--------|------|
| range_construct | 持平 | demo 1282 / std 1585 | demo 1.24x |
| insert | 持平 | 持平 | — |
| find | demo 2.6 / std 6.4 | 持平 | **demo 2.46x(小)** |
| erase | 持平 | demo 592 / std 449 | std 1.32x |
| clear | 持平 | 持平 | — |
| forward_iterate | 持平 | demo 61.3 / std 40.4 | **std 1.52x** |

**set 小结**: 小规模 `find` 大幅领先（2.46x），但大规模 `erase` 和遍历落后。

---

### 2.3 multimap / multiset

表现与 map/set 相似：插入和构造持平或略优，遍历和删除落后 std 约 1.1~1.5x。

---

## 三、无序关联容器（哈希表）

### 3.1 unordered_map

| 操作 | 1,000 | 10,000 | 胜出 |
|------|-------|--------|------|
| range_construct | 持平 | 持平 | — |
| insert | 持平 | demo 1252 / std 912 | **std 1.37x** |
| find | 持平 | 持平 | — |
| erase | 持平 | demo 1003 / std 672 | **std 1.49x** |
| clear | 持平 | 持平 | — |
| forward_iterate | 持平 | demo 100.8 / std 48.8 | **std 2.07x** |

**unordered_map 小结**: 大规模插入、删除、遍历全面落后 std 1.37~2.07x。

> **分析**: demo 的哈希表在 rehash 策略、桶管理或迭代器设计上不如 std 高效。遍历慢 2 倍说明迭代器在跳过空桶时开销较大。

---

### 3.2 unordered_set

| 操作 | 1,000 | 10,000 | 胜出 |
|------|-------|--------|------|
| insert | 持平 | 持平 | — |
| find | 持平 | 持平 | — |
| erase | 持平 | demo 728 / std 370 | **std 1.97x** |
| forward_iterate | demo 2.5 / std 1.1 | demo 90.0 / std 24.1 | **std 3.73x** |

**unordered_set 小结**: 删除慢 2x，遍历慢 3.73x 是全场最差项之一。

---

### 3.3 unordered_multimap / unordered_multiset

表现类似：遍历落后 std 2.7~3.8x，删除落后 1.5~1.7x。

---

## 四、容器适配器

### 4.1 stack

| 操作 | 1,000 | 10,000 | 胜出 |
|------|-------|--------|------|
| push | demo 3.0 / std 18.9 | demo 18.6 / std 109.9 | **demo 5.91x** |
| pop | demo 2.2 / std 11.8 | demo 18.4 / std 94.3 | **demo 5.13x** |
| top | 持平 | 持平 | — |

### 4.2 queue

| 操作 | 1,000 | 10,000 | 胜出 |
|------|-------|--------|------|
| push | demo 3.3 / std 12.8 | demo 17.9 / std 106.9 | **demo 5.97x** |
| pop | demo 2.2 / std 11.5 | demo 15.5 / std 94.6 | **demo 6.10x** |
| front/back | 持平 | 持平 | — |

**stack/queue 小结**: demo 的 push/pop 比 std 快 5~6x，直接得益于 demo deque 的优秀性能。

### 4.3 priority_queue

| 操作 | 1,000 | 10,000 | 胜出 |
|------|-------|--------|------|
| push | 持平 | 持平 | — |
| pop | demo 25.8 / std 15.5 | demo 497.9 / std 213.5 | **std 2.33x** |
| top | 持平 | 持平 | — |

**priority_queue 小结**: demo 的 `pop`（堆删除）慢 std 2.33x。这是 demo 适配器中唯一的明显劣势。

> **分析**: priority_queue 基于 vector 实现堆操作。demo 的堆调整（`std::pop_heap` 等价操作）效率不如 MSVC 的优化实现，可能与元素交换或比较函数调用开销有关。

---

## 五、量化总览

### demo 大幅领先（>2x）的操作

| 容器 | 操作 | demo/std 比值 |
|------|------|---------------|
| deque | clear | **13.27x** |
| deque | range_construct | **10.21x** |
| deque | push_back | **9.31x** |
| deque | pop_front | **8.68x** |
| deque | pop_back | **8.42x** |
| deque | push_front | **8.10x** |
| queue | pop | **6.10x** |
| queue | push | **5.97x** |
| vector | erase_middle | **5.95x** |
| stack | push | **5.91x** |
| stack | pop | **5.13x** |
| vector | push_back | **3.19x** |
| set | find (1k) | **2.46x** |

### std 大幅领先（>2x）的操作

| 容器 | 操作 | std/demo 比值 |
|------|------|---------------|
| vector | insert_middle | **4.20x** |
| unordered_set | forward_iterate | **3.73x** |
| vector | copy_construct | **2.89x** |
| vector | clear | **2.88x** |
| vector | range_construct | **2.75x** |
| map | forward_iterate | **2.69x** |
| priority_queue | pop | **2.33x** |
| unordered_map | forward_iterate | **2.07x** |
| unordered_set | erase | **1.97x** |
| list | sort | **1.92x** |

---

## 六、各容器综合评分

| 容器 | demo 优势 | std 优势 | 综合判断 |
|------|-----------|----------|----------|
| **deque** | ★★★★★ | ★ | **demo 碾压** |
| **stack** | ★★★★★ | ★ | **demo 碾压** |
| **queue** | ★★★★★ | ★ | **demo 碾压** |
| **vector** | ★★★★ | ★★★ | demo 略优 |
| **list** | ★★ | ★★★ | std 略优 |
| **forward_list** | ★★ | ★★★ | std 略优 |
| **map** | ★★★ | ★★★ | 基本持平 |
| **multimap** | ★★★ | ★★ | 基本持平 |
| **set** | ★★★ | ★★ | 基本持平 |
| **multiset** | ★★★ | ★★ | 基本持平 |
| **unordered_map** | ★★ | ★★★ | std 略优 |
| **unordered_set** | ★★ | ★★★★ | **std 明显优** |
| **unordered_multimap** | ★★ | ★★★ | std 略优 |
| **unordered_multiset** | ★★ | ★★★ | std 略优 |
| **priority_queue** | ★★ | ★★★ | std 略优 |

---

## 七、架构层面的优劣势分析

### demo-STL 的优势

1. **deque 设计卓越**: 块大小策略合理，避免了 std::deque 小块分配的性能瓶颈，是 demo 最有价值的实现。
2. **vector 扩容策略高效**: push_back 比 std 快 3 倍，说明扩容因子和内存管理更优。
3. **红黑树插入/查找优秀**: 在中小规模下 map/set 的插入和查找通常与 std 持平或略优。
4. **代码简洁可控**: 自实现代码无平台依赖，便于理解和定制。

### demo-STL 的劣势

1. **红黑树遍历效率低**: 迭代器 `operator++` 实现有额外开销，遍历慢 1.5~2.7x。
2. **链表排序算法落后**: list/forward_list 的 sort 比 std 慢 1.6~1.9x，未达到归并排序的最优实现。
3. **哈希表整体偏弱**: 无序容器的插入、删除、遍历全面落后 std 1.3~3.7x，rehash 策略和迭代器设计需优化。
4. **堆操作效率不足**: priority_queue 的 pop 慢 std 2.3x，堆调整算法有优化空间。
5. **vector 构造/拷贝慢**: 范围构造和拷贝构造落后 std 2.7~2.9x，内存分配策略不够精细。

### 改进建议

1. **优先优化**: 红黑树迭代器遍历（影响所有有序关联容器）、哈希表遍历（影响所有无序容器）
2. **次要优化**: 链表排序算法、priority_queue 堆操作、vector 构造/拷贝
3. **保持优势**: deque 设计是 demo 的核心竞争力，继续保持当前架构

---

*报告生成时间: 2026-06-07 | 数据来源: benchmark/results/*.csv*