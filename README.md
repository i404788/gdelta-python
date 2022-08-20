# GDelta Python bindings

## Example usage:
```
import gdelta 

base = b'awdhawdhawdhawdahwdahwdhawdhawdhawdhawdhawdhawdahwdh'
target =  b'awdhawdhawdhawdhawdhadhawdhawdhawdhadwhawdhawdhawdhdh'
delta = gdelta.encode(base, target)
target_hat = gdelta.decode(base, delta)

assert target == target_hat, "Target & decoded target should be the same"
print(gdelta.describe(delta))

#Instruction Length: 5 bytes
#Data Length: 34 bytes
#Units:
#- TakeBaseAtOffset(offset=28,length=19)
#- TakeDelta(length=34)
```


## Installation
This library requires [libgdelta](https://github.com/i404788/gdelta) to be installed when compiling.

