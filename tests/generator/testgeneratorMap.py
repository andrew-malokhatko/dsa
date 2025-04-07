import random

RANGE = 200
TEST_COUNT = 1000

# operaton list: "INSERT", "DELETE", "SEARCH"
def generate_test_case(n_ops, operation_list, test_dict: dict):
    operations = []

    for _ in range(n_ops):
        op = random.choice(operation_list)

        key = random.randint(1, RANGE)
        value = random.randint(1, RANGE)

        if op == "INSERT":
            operations.append(f"INSERT {key}:{value}")
            test_dict[key] = value
        
        elif op == "SEARCH":
            #expect = 1 if value in test_dict else 0  # 1 if found, 0 if not
            expected = test_dict[key] if key in test_dict else 0
            operations.append(f"SEARCH {key}:{expected}")
        
        elif op == "DELETE":
            operations.append(f"DELETE {key}")
            if key in test_dict:
                del test_dict[key]

    return operations


test_dict = dict()  # Tracks keys currently in the data structure
testInsert = generate_test_case(100, ["INSERT"], test_dict)
print(len(test_dict))
#testSearch = generate_test_case(100, ["DELETE"], test_dict)
#print(len(test_dict))
testSearch = generate_test_case(100, ["SEARCH"], test_dict)
print(len(test_dict))

test = testInsert + testSearch

with open("../resources/dictSearchTest.txt", "w") as f:
    for op in test:
        f.write(op + "\n")

    f.write("\n");

     # Sort the dictionary by keys and write each key-value pair
    for key, value in sorted(test_dict.items()):
        f.write(f"{key}: {value}\n")