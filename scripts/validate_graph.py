# -*- coding: gbk -*-

import sys

def validate(filename):
    n_max = -1
    v_set = set()
    with open(filename) as fin:
        n_line = 0
        for line in fin:
            n_line += 1
            v_list = line.rstrip().split()
            for vid in v_list:
                vid = int(vid)
                if vid < 0:
                    print '(invalid) at line %d' % n_line
                    return
                v_set.add(vid)
                if vid > n_max:
                    n_max = vid
            if 'adj' in filename:
                if len(v_list) < 2:
                    print '(invalid) at line %d' % n_line
                    return
            if 'edge' in filename:
                if len(v_list) != 2 and len(v_list) != 3:
                    print '(invalid) at line %d' % n_line
                    return
        if len(v_set) == n_max and 0 not in v_set:
            print '(valid) start index: 1, total %d vertex.' % n_max
        elif len(v_set) == n_max + 1 and 0 in v_set:
            print '(valid) start index: 0, total %d vertex.' % (n_max + 1)
        else:
            print '(invalid) lost vid, total %d vertex, n_max=%d' % (len(v_set), n_max)
          
def main():
    if len(sys.argv) != 2:
        print 'validate_graph.py graph_file'
    else:
        validate(sys.argv[1])

if __name__ == '__main__':
    main()
