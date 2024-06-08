

if __name__ == '__main__':

    print("Insert the binary number: ")

    binary = input()

    for i,b in enumerate(reversed(binary)):
        print(f"bit {i}: {b}")    

