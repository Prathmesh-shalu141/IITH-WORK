import random

def generate_transaction(num_elements, max_read_write_size):
    """
    Generate a single transaction with a uniform distribution for gas fee.
    
    Parameters:
        num_elements (int): The number of elements to choose from for the read/write sets.
        max_read_write_size (int): Maximum size of the read and write sets.
        
    Returns:
        dict: A dictionary containing the transaction's read set, write set, and gas fee.
    """
    read_set_size = random.randint(1, max_read_write_size)
    write_set_size = random.randint(1, max_read_write_size)
    
    read_set = random.sample(range(num_elements), read_set_size)
    write_set = random.sample(range(num_elements), write_set_size)
    
    # Randomly ensure that the read and write sets overlap with some probability
    if random.random() < 0.5:
        overlap_size = random.randint(1, min(len(read_set), len(write_set)))
        overlap_elements = random.sample(read_set, overlap_size)
        write_set.extend(overlap_elements)
    
    gas_fee = int(random.uniform(10, 500))  # Uniform distribution for gas fee
    
    return {
        'read_set': read_set,
        'write_set': write_set,
        'gas_fee': gas_fee
    }

def generate_input_file(filename, num_transactions, num_elements, max_read_write_size):
    """
    Generate an input file with random transactions.
    
    Parameters:
        filename (str): The name of the file to save the transactions.
        num_transactions (int): The number of transactions to generate.
        num_elements (int): The number of elements to choose from for the read/write sets.
        max_read_write_size (int): The maximum size of the read and write sets.
    """
    with open(filename, 'w') as file:
        threads = 16 # Example: 2 threads
        conflict_percentage = 50# Example: 60% conflicts
        total_time = 10  # Example: total time is 10 units
        file.write(f"{threads} {conflict_percentage} {total_time} {num_transactions}\n")
        
        for i in range(num_transactions):
            transaction = generate_transaction(num_elements, max_read_write_size)
            read_set_size = len(transaction['read_set'])
            write_set_size = len(transaction['write_set'])
            gas_fee = transaction['gas_fee']
            
            # Write the transaction data in the desired format
            file.write(f"{i} {read_set_size} {write_set_size} {gas_fee}\n")
            file.write(" ".join(map(str, transaction['read_set'])) + "\n")
            file.write(" ".join(map(str, transaction['write_set'])) + "\n")

# Parameters for generating the input file
num_transactions = 1500  # Example: 100 transactions
num_elements = 20       # Example: 20 elements in the read/write sets
max_read_write_size = 5  # Example: maximum size of read/write sets is 5

# Generate the input file
generate_input_file('input.txt', num_transactions, num_elements, max_read_write_size)
