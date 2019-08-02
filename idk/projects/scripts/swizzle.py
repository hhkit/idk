import sys

def main():
    swizzle_begin = "swizzle<vector, T,"
    swizzle_end   = ";\\"
    token = ['x', 'y', 'z', 'w']
    
    dim = int(sys.argv[1])
    
    for i in range(dim):
        for j in range(dim):
            print swizzle_begin, i, ',', j, '>', token[i] + token[j],  swizzle_end
                
    for i in range(dim):
        for j in range(dim):
            for k in range(dim):    
                print swizzle_begin, i, ',', j, ',', k, '>', token[i] + token[j] + token[k], swizzle_end
                    
    for i in range(dim):
        for j in range(dim):
            for k in range(dim):        
                for l in range(dim):
                    print swizzle_begin, i, ',', j, ',', k, ',', l, '>', token[i] + token[j] + token[k] + token[l], swizzle_end

main();