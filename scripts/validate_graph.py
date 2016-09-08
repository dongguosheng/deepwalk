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
            if 'adj' in filename:
                if len(v_list) < 2:
                    print '(invalid) at line %d' % n_line
                    return
            if 'edge' in filename:
                if len(v_list) != 2 and len(v_list) != 3:
                    print '(invalid) at line %d' % n_line
                    return
                if len(v_list) == 3:
                    v_list = v_list[: 2]
            for vid in v_list:
                vid = int(vid)
                if vid < 0:
                    print '(invalid) at line %d' % n_line
                    return
                v_set.add(vid)
                if vid > n_max:
                    n_max = vid

        if len(v_set) == n_max and 0 not in v_set:
            print '(valid) start index: 1, max vertex idx: %d, total %d vertex.' % (n_max, len(v_set))
        elif len(v_set) == n_max + 1 and 0 in v_set:
            print '(valid) start index: 0, max vertex idx: %d, total %d vertex.' % (n_max, len(v_set))
        else:
            print '(valid) start index: 0, max vertex idx: %d, total %d vertex.' % (n_max, len(v_set))
            print 'vertex ids are not continuous, lost %d id.' % (n_max - len(v_set) if 0 in v_set else n_max - len(v_set) + 1)
          
def main():
    if len(sys.argv) != 2:
        print 'validate_graph.py graph_file'
    else:
        validate(sys.argv[1])

if __name__ == '__main__':
    main()
