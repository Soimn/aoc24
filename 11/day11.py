import sys


todo = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
rules_map = {}

i = 0;
while len(todo) != 0:
    i += 1
    n = todo.pop(0)
    if n == 0:
        rule = [1]
    elif len(str(n)) % 2 != 0:
        rule = [2024*n]
    else:
        rule = [int(str(n)[:len(str(n))//2]), int(str(n)[len(str(n))//2:])]

    if n not in rules_map:
        rules_map[n] = rule

    for x in rule:
        if x not in rules_map and x not in todo:
            todo.append(x)
                
ns = sorted(list(rules_map.keys()))

for key in rules_map.keys():
    rules_map[key] = [ns.index(rules_map[key][0]), ns.index(rules_map[key][1])] if len(rules_map[key]) == 2 else [ns.index(rules_map[key][0]), len(ns)]

    
for n in ns:
    print("{" + f"{n}, {rules_map[n][0]}, {rules_map[n][1]}" + "},")


