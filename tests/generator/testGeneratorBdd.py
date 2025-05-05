import itertools
import random

def generate_random_dnf(num_vars=22, num_terms=110, max_term_size=22):
    variables = [chr(ord('A') + i) for i in range(num_vars)]
    terms = []

    for i in range(num_terms):
        term_size = random.randint(1, max_term_size)
        chosen_vars = random.sample(variables, term_size)
        term = []
        for var in chosen_vars:
            if random.choice([True, False]):
                term.append(f'!{var}')
            else:
                term.append(var)
        terms.append(''.join(term))
    return '+'.join(terms), variables

"""Parse expression into list of terms with literals"""
def parse_expression(expr):
    terms = expr.split('+')
    return [ [lit for lit in split_literals(term)] for term in terms ]

"""Split term like !A!B or AB into ['!A', '!B'] or ['A', 'B']"""
def split_literals(term):
    literals = []
    i = 0
    while i < len(term):
        if term[i] == '!':
            literals.append(term[i:i+2])
            i += 2
        else:
            literals.append(term[i])
            i += 1
    return literals

def evaluate_term(term, assignment):
    for literal in term:
        if literal.startswith('!'):
            if assignment[literal[1]] != 0:
                return False
        else:
            if assignment[literal] != 1:
                return False
    return True

def evaluate_expression(terms, assignment):
    return any(evaluate_term(term, assignment) for term in terms)

def generate_input_output(expr, variables):
    terms = parse_expression(expr)
    input_output = []

    for values in itertools.product([0, 1], repeat=len(variables)):
        assignment = dict(zip(variables, values))
        result = evaluate_expression(terms, assignment)
        input_output.append((assignment, int(result)))

    return input_output

def write_to_file(expr, variables, input_output, filename="bdd_test_output.txt"):
    filename = "../resources/" + filename
    with open(filename, 'w') as f:
        f.write(expr + '\n')
        f.write(''.join(variables) + '\n')
        for assignment, result in input_output:
            line = ''.join(str(assignment[v]) for v in variables) + ' - ' + str(result)
            f.write(line + '\n')

if __name__ == "__main__":
    random.seed()  # You can pass an int here for reproducibility
    expr, vars_order = generate_random_dnf()
    io = generate_input_output(expr, vars_order)
    write_to_file(expr, vars_order, io)