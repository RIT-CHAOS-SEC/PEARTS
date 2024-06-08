# import hash library
import hashlib
import hmac

# define a key for HMAC-SHA224 in hex format and store in a byte format
class HashOp_SHA224:
    def __init__(self,key = None) -> None:
        if key is None:
            
            self.key = bytes('12345678901234567890123ASDA4'.encode())
        else:
            assert len(key) == 28, "Key must be 28 bytes long"
            assert type(key) == bytes, "Key must be in bytes format"
            self.key = key
        pass

    # hash a string using SHA224 algorithm
    def hash_sha224(self,string):
        if type(string) == str:
            string = string.encode()
        return hashlib.sha224(string).hexdigest()

    # mac a string using HMAC-SHA224 algorithm
    def hmac_sha224(self, string):
        if type(string) == str:
            string = string.encode()
        return hmac.new(self.key, string, hashlib.sha224).hexdigest()
    

def read_byte_stream(filepath):
    with open(filepath, 'r') as fp:
        lines = fp.readlines()

    # strip '\n' and swap endianess
    byte_stream = []
    for i in range(0, len(lines), 2):
        l0 = lines[i].strip()
        l1 = lines[i+1].strip()
        byte_stream.append(bytes.fromhex(l1 + l0))

    return byte_stream


if __name__ == "__main__":
    
    #### CALCULATE THE HASH OF NS WORLD ##########

    hashHamdler = HashOp_SHA224()
    path = 'objects/NonSecure.mem'
    # read byte stream from file
    byte_stream = read_byte_stream(path)
    # concatenate all the byte stream

    byte_stream = b''.join(byte_stream)
    byte_stream = b''.join([byte_stream[i:i+4][::-1] for i in range(0, len(byte_stream), 4)])
    
    print("Byte stream len : ",len(byte_stream))
    print("HASH SHA224:\n",hashHamdler.hash_sha224(byte_stream))
    print("HMAC SHA224:\n",hashHamdler.hmac_sha224(byte_stream))