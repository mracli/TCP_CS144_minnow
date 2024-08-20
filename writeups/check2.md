Checkpoint 2 Writeup
====================

My name: [your name here]

My SUNet ID: [your sunetid here]

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [list sunetids here]

This lab took me about 7 hours to do. I [did/did not] attend the lab session.

Describe Wrap32 and TCPReceiver structure and design. [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

wrap32 实现将 32位无符号 相对序列号 <-> 64位无符号 绝对序列号 相互转换，其中绝对序列号转相对序列号时需要考虑 round 操作，同时需要对溢出情况进行处理

其中 unwrap 操作较为复杂，思考了很久不知道应该如何转换，最后参考一些朋友的方法补全对应的内容。

TCPReceiver 中，需要考虑当前消息对应实际的流index在哪里，需要从 seq_no -> abs_seq_no -> stream_index 其中 stream_index 从实际的内容开始计算下标，而 abs_seq_no 则从 `SYN` segment 开始就开始统计，同时 FIN 也会占用 `abs_seq_no`, 所以当我们请求下一个 `segment` 时，需要根据当前是否建立连接， `bytestream` 是否已经 `close` 来计算从 stream_index -> abs_seq_no 的映射

同时，如果 Receiver 初始化前丢弃任何非 SYN报文的请求，仅建立 SYN 报文才开始处理message; 如果对端发来的 message 中出现了 RST 标志置 true 的情况，说明当前连接已经非法，那么我们同步设置当前状态为 error，同时当我们的bytestream 处于 error 状态时，发送任何 message 都会置 RST 为 true

Implementation Challenges:
[
    如何计算 seq_no -> abs_seq_no, 需要考虑的边界以及溢出情况；
]

Remaining Bugs:
[]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: [describe where to find]
