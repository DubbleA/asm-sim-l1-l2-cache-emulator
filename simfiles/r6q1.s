    movi $1, foo
    lw $2, foo($0)
    halt
foo:
    .fill 42

# Final state:
#         pc=    2
#         $0=    0
#         $1=    3
#         $2=   42
#         $3=    0
#         $4=    0
#         $5=    0
#         $6=    0
#         $7=    0
# 2083 8103 4002 002a 0000 0000 0000 0000
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