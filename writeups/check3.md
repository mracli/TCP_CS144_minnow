Checkpoint 3 Writeup
====================

My name: [your name here]

My SUNet ID: [your sunetid here]

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about 7 hours to do. I [did/did not] attend the lab session.

tcp sender 中使用 receiver 发送的包，来更新当前已经确认收到的序列号 `acked_seq_no` 以及 下一个要发送的tcp segment `next_seq_no`, 并随时按照对端发送的窗口尺寸更新自己的发送窗口大小, 为实现超时重传机制，添加 timer 类 辅助记录最早未被 ack 的 tcp segment 的超时时间，并当出现超时的情况重传最早的未被ack tcp segment, 同时使用 指数退避的拥塞避免方法，避开网络拥堵的情况

Implementation Challenges:
[
    sender 需要处理各种各样不同的发送情况，需要对每一种边界做具体的处理，才能够实现足够的稳定性；
]

Remaining Bugs:
[]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
