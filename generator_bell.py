import random

def generate_transaction(num_elements, max_read_write_size, mu=80, sigma=25):
    """
    Generate a single transaction with a bell-shaped (normal) distribution for gas fee.
    """
    read_set_size = random.randint(1, max_read_write_size)
    write_set_size = random.randint(1, max_read_write_size)

    read_set = random.sample(range(num_elements), read_set_size)
    write_set = random.sample(range(num_elements), write_set_size)

    # Create overlap between read and write sets with some probability
    if random.random() < 0.5:
        overlap_size = random.randint(1, min(len(read_set), len(write_set)))
        overlap_elements = random.sample(read_set, overlap_size)
        write_set.extend(overlap_elements)

    # Bell-shaped gas fee using normal distribution, clamped between 10 and 100
    gas_fee = int(min(max(random.gauss(mu, sigma), 10), 1000))

    return {
        'read_set': read_set,
        'write_set': write_set,
        'gas_fee': gas_fee
    }

def generate_input_file(filename, num_transactions, num_elements, max_read_write_size):
    """
    Generate an input file with bell-distributed gas fees and write to a file.
    """
    gas_fees = []

    with open(filename, 'w') as file:
        threads = 16
        conflict_percentage = 30
        total_time = 50
        file.write(f"{threads} {conflict_percentage} {total_time} {num_transactions}\n")

        for i in range(num_transactions):
            transaction = generate_transaction(num_elements, max_read_write_size)
            read_set_size = len(transaction['read_set'])
            write_set_size = len(transaction['write_set'])
            gas_fee = transaction['gas_fee']
            gas_fees.append(gas_fee)

            file.write(f"{i} {read_set_size} {write_set_size} {gas_fee}\n")
            file.write(" ".join(map(str, transaction['read_set'])) + "\n")
            file.write(" ".join(map(str, transaction['write_set'])) + "\n")
    
    return gas_fees  # Return for plotting

# Parameters
num_transactions = 1500
num_elements = 20
max_read_write_size = 5

# Generate input and get gas fees for plotting
gas_fees = generate_input_file('input.txt', num_transactions, num_elements, max_read_write_size)


