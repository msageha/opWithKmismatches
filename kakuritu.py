import numpy as np
np.random.seed(71)

T = 10000
P = np.array([10, 4, 5, 2, 4, 8], dtype=int)
P_order = np.argsort(P)
P_len = len(P)
data = np.random.randint(0, 1000, T)

count = 0
for i in range(0, T-P_len):
    if (P_order == np.argsort(data[i:i+P_len])).all():
        count += 1

print (count)