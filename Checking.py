import itertools as it

l = 0
l_min = 100
bests = []

iterations = list(it.permutations([1, 2, 3, 4, 5]))

matrix = (
    (0, 1, 0, 1, 1),
    (1, 0, 1, 0, 0),
    (0, 1, 0, 1, 0),
    (1, 0, 1, 0, 1),
    (1, 0, 0, 1, 0))
for li in iterations:
    l = 0
    for i in range(5):
        for j in range(5):
            l += abs(i - j) * matrix[li[i]-1][li[j]-1]     
    l *= 0.5
    if l < l_min:
        bests.clear()
        bests.append(li)
        l_min = l
    elif l == l_min:
        bests.append(li)
        

print(bests)
print(len(bests))