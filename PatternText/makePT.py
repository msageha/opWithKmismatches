import random

P = 10
T = 100000

file = open('pattern.txt', 'w')
for i in range(0, P):
  file.write(str(random.randint(1, 10000)) + '\n')
file.close()

file = open('text.txt', 'w')
for i in range(0, T):
  file.write(str(random.randint(1, 10000)) + '\n')
file.close()