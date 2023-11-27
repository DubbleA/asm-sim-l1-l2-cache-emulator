    add $1, $0, $0
    add $4, $0, $0
    lw $3, data($0)
Loop:
    slti $1, $3, 20
    jeq $1, $0, skip
    add $4, $4, $3
    addi $3, $3, 1
    jeq $0, $0, Loop
skip:
    halt
data:
    .fill 16

# Final state:
#         pc=    8
#         $0=    0
#         $1=    0
#         $2=    0
#         $3=   20
#         $4=   70
#         $5=    0
#         $6=    0
#         $7=    0
# 0010 0040 8189 ec94 c403 11c0 2d81 c07b
# 4008 0010 0000 0000 0000 0000 0000 0000
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