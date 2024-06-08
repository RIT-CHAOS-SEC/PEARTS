#/bin/bash

projectDir=$(dirname "$(realpath "$0")")/
projectName=$(basename "${projectDir}")

SecureElf="${projectDir}Secure/Debug/${projectName}_Secure.elf"
NonSecureElf="${projectDir}NonSecure/Debug/${projectName}_NonSecure.elf"

# define objdump command
OBJDUMP_CMD=arm-none-eabi-objdump
# define python command
PYTHON_CMD=/c/Users/aj4775/AppData/Local/miniconda3/python.exe

# clean objects directory if exist
if [ -d objects ]; then
    rm -rf objects
fi

# create objects directory
mkdir objects

# test if project dir exists
if [ ! -d "${projectDir}" ]; then
    echo "Error: project directory not found"
    exit 1
fi

# test if PYTHON_CMD exists
if ! command -v "${PYTHON_CMD}" &> /dev/null
then
    echo "Error: python command not found !!!"
    exit 1
fi

# test if secure and non secure elf exist
if [ ! -f "${SecureElf}" ]; then
    echo "Error: secure elf not found"
    exit 1
fi
if [ ! -f "${NonSecureElf}" ]; then
    echo "Error: non-secure elf not found"
    exit 1
fi

echo ""
echo "Information :"
echo "Project directory: ${projectDir}"
echo "Project name: ${projectName}"
echo "Secure elf: ${SecureElf}"
echo "Non-Secure elf: ${NonSecureElf}"
echo ""

### objdump the secure world elf
"${OBJDUMP_CMD}" -dz "${SecureElf}" > objects/Secure.asm.tmp

sed '/>:$/d' objects/Secure.asm.tmp > objects/Secure.tmp
"${PYTHON_CMD}" scripts/format_words.py objects/Secure.tmp
sed -i 's/ //g' objects/Secure.tmp2
awk '{print $2}' objects/Secure.tmp2 > objects/Secure.mem
sed -i 's/\(.\{2\}\)/\1\n/g' objects/Secure.mem
sed -i '/^$/d' objects/Secure.mem

### Non-Secure world
"${OBJDUMP_CMD}" -dz "${NonSecureElf}" > objects/NonSecure.asm.tmp

sed '/>:$/d' objects/NonSecure.asm.tmp > objects/NonSecure.tmp
"${PYTHON_CMD}" scripts/format_words.py objects/NonSecure.tmp
sed -i 's/ //g' objects/NonSecure.tmp2
awk '{print $2}' objects/NonSecure.tmp2 > objects/NonSecure.mem
sed -i 's/\(.\{2\}\)/\1\n/g' objects/NonSecure.mem
sed -i '/^$/d' objects/NonSecure.mem


#check if files were created
if [ ! -f objects/Secure.mem ]; then
    echo "Error: objects/Secure.mem not found"
    exit 1
fi
if [ ! -f objects/NonSecure.mem ]; then
    echo "Error: objects/NonSecure.mem not found"
    exit 1
fi

echo "Done !"

# rm objects/*.tmp*



# alias conda=/c/Users/aj4775/AppData/Local/miniconda3/Library/bin/conda.bat 