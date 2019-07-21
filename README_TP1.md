
There is:
1 semaphore for QueProcessed because Task1 and Task2 may write to it at the same time
2- No semaphor needed for QuTrans because there is not reading nor writing at the same time situation.

