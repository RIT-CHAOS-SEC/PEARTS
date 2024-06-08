import re
import sys
def count_code_lines(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
        
    code_lines = 0
    inside_block_comment = False
    
    for line in lines:
        trimmed_line = line.strip()
        
        if inside_block_comment:
            if '*/' in trimmed_line:
                inside_block_comment = False
                trimmed_line = trimmed_line.split('*/', 1)[1].strip()
            else:
                continue
        
        if '/*' in trimmed_line:
            inside_block_comment = True
            trimmed_line = trimmed_line.split('/*', 1)[0].strip()
        
        if trimmed_line.startswith('//') or trimmed_line == '':
            continue
        
        code_lines += 1
    
    return code_lines


# Secure\\Core\\Src\\rtpox_its.c
# get file_path as input in the temrinald
if __name__ == '__main__':

    # read file input from argument 
    file_path = sys.argv[1]
    
    print(f'Total code lines (excluding comments): {count_code_lines(file_path)}')


