import random

RANGE = 10_000_000
TEST_COUNT = 10_000_000

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
            if value in test_set:
                test_set.remove(value)

            operations.append(f"DELETE {value}")


    return operations


test_set = set()  # Tracks keys currently in the data structure
testInsert = generate_test_case(TEST_COUNT, ["INSERT", "DELETE", "SEARCH"], test_set)
print(len(test_set))

test = testInsert

with open("../resources/treeAll10milTest.txt", "w") as f:
    for op in test:
        f.write(op + "\n")
    f.write(str(sorted(test_set)) + '\n')