foo :
    movi $1 , foo # ram [0]
    lw $2 , foo ($0 ) # ram [1]
    halt # ram [2]

# Final state:
#         pc=    2
#         $0=    0
#         $1=    0
#         $2= 8320
#         $3=    0
#         $4=    0
#         $5=    0
#         $6=    0
#         $7=    0
# 2080 8100 4002 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000
# 0000 0000 0000 0000 0000 0000 0000 0000