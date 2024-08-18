Checkpoint 1 Writeup
====================

My name: [your name here]

My SUNet ID: [your sunetid here]

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [list sunetids here]

This lab took me about [n] hours to do. I [did/did not] attend the lab session.

I was surprised by or edified to learn that: [describe]

Describe Reassembler structure and design. [Describe data structures and
approach taken. Describe alternative designs considered or tested.
Describe benefits and weaknesses of your design compared with
alternatives -- perhaps in terms of simplicity/complexity, risk of
bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

流重组器的结构：

- vector<pair<size_t, string> > 保存待重组的数据，这些数据保证不重且在窗口范围内
- bool has_last 标记是否有 last_str 等待插入
- has_last 保存已读到最后一个字符串的记录
- 在合适的时机关闭 bytestream 写端

Implementation Challenges:
[
    需要处理 data 在不同情况下插入到重组器是应该如何处理，主要分为两种情况，可以和不可以直接插入到 bytestream 的 buffer 中，不可以插入的话应该如何将data 不重复的补充到 storage 中
]

Remaining Bugs:
[]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]
