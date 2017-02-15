import random

print("please input P num")
P = int(input())
print("please input T num")
T = int(input())

file = open('pattern.txt', 'w')
for i in range(0, P):
  file.write(str(random.randint(1, 100)) + '\n')
file.close()

file = open('text.txt', 'w')
for i in range(0, T):
  file.write(str(random.randint(1, 100)) + '\n')
file.close()