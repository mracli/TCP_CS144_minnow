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

优势：使用 string 缓存的信息可以通过 string_view 获取对应的视图，较少内存拷贝开销;

最终 `speed_test` 结果：在云服务器中可达到 2.5Gbps;

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