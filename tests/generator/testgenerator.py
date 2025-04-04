import random

RANGE = 1000
TEST_COUNT = 1000

# operaton list: "INSERT", "DELETE", "SEARCH"
def generate_test_case(n_ops, operation_list, test_set):
    operations = []

    for _ in range(n_ops):
        op = random.choice(operation_list)

        value = random.randint(1, RANGE)

        if op == "INSERT":
            operations.append(f"INSERT {value}")
            test_set.add(value)
        
        elif op == "SEARCH":
            expect = 1 if value in test_set else 0  # 1 if found, 0 if not
            operations.append(f"SEARCH {value} {expect}")
        
        elif op == "DELETE":
            operations.append(f"DELETE {value}")
            if value in test_set:
                test_set.remove(value)

    return operations


test_set = set()  # Tracks keys currently in the data structure
testInsert = generate_test_case(TEST_COUNT * 1000, ["INSERT"], test_set)
print(len(test_set))
testSearch = generate_test_case(int(TEST_COUNT * 1000), ["SEARCH"], test_set)
print(len(test_set))

test = testInsert + testSearch

with open("../resources/perfSearchTest.txt", "w") as f:
    for op in test:
        f.write(op + "\n")
    f.write(str(sorted(test_set)) + '\n')