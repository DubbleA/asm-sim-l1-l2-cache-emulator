    movi $1, 7
    add $2, $1, $1
    sw $1, foobar($0)
    sw $2, foobar($1)
    halt
foobar:
    .fill 600
    .fill 700

# Final state:
#         pc=    4
#         $0=    0
#         $1=    7
#         $2=   14
#         $3=    0
#         $4=    0
#         $5=    0
#         $6=    0
#         $7=    0
# 2087 04a0 a085 a505 4004 0007 02bc 0000
# 0000 0000 0000 0000 000e 0000 0000 0000
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