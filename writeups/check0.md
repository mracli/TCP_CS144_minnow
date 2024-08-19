Checkpoint 0 Writeup
====================

My name: [your name here]

My SUNet ID: [your sunetid here]

I collaborated with: [list sunetids here]

I would like to credit/thank these classmates for their help: [list sunetids here]

This lab took me about [n] hours to do. I [did/did not] attend the lab session.

My secret code from section 2.1 was: [code here]

I was surprised by or edified to learn that: [describe]

Describe ByteStream implementation. 

字节流实现：底层采用 string 实现，同时记录已经压入的字节数量 (`pushed_size_`) 和被上层使用并弹出的数量 (`popped_size_`);

优势：

- 使用 string 缓存的信息可以通过 string_view 获取对应的视图，较少内存拷贝开销;
- buffer_pop 时需要 O(n) 的复杂度

最终 `speed_test` 结果：在云服务器中可达到 2.5Gbps;

思考使用何种数据结构：
1. 双端队列，优势两边插入和弹出的复杂度都为 O(1), 但是在 peek buffer 需要将底层的地址暴露给 string_view, 而双端队列并不是连续的存储，不能 O(1)的提供对应的地址，过程中需要额外使用其他的连续存储结构来存中间变量，保证访问 string_view 的过程中不会出现空悬引用，这样双端队列的时间复杂度就退化为 O(n) 空间复杂度也为 O(n)；
2. 循环队列，同样的问题，需要处理不连续的情况，如果当前队列存储的内容是连续的状态，那么可以直接提供给 string_view 对应的地址和大小，但出现不连续的情况同样会使得循环队列 peek 的时间复杂度提高到 O(n), 同时需要额外的存储，空间复杂度为 O(n);
3. 使用 string, 虽然 string 在 pop 的过程中使用到 substr 使时间复杂度变为 O(n), 但是在 peek 的过程中不会出现不连续存储的情况，保证提供 string_view 的时间复杂度和空间复杂度都为 O(1).

[Describe data structures and
approach taken. Describe alternative designs considered or tested.
Describe benefits and weaknesses of your design compared with
alternatives -- perhaps in terms of simplicity/complexity, risk of
bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I contributed a new test case that catches a plausible bug
  not otherwise caught: [provide Pull Request URL]