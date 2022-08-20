import gdelta 

base = b'awdhawdhawdhawdahwdahwdhawdhawdhawdhawdhawdhawdahwdh'
target =  b'awdhawdhawdhawdhawdhadhawdhawdhawdhadwhawdhawdhawdhdh'
delta = gdelta.encode(base, target)
target_hat = gdelta.decode(base, delta)

assert target == target_hat, "Target & decoded target should be the same"
print(gdelta.describe(delta))
