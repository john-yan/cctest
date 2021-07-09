#!/usr/bin/env python3
import random
import argparse
import sys

def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('--seed', "-s", default=0, help="random see.")
    parser.add_argument('--input', "-i", default=None, help="input file.")
    parser.add_argument('--expected', "-e", default=None, help="expected file.")

    args = parser.parse_args()
    seed = int(args.seed)
    random.seed(seed)

    inp = sys.stdout
    if args.input != None:
        inp = open(args.input, 'w')

    expected_file_name = 'expected.txt' if args.expected == None else args.expected
    out = open(expected_file_name, 'w')

    db = {}
    ops = ['get', 'put', 'update', 'remove_key', 'query']
    keys = [ 'key' + str(random.randint(0, 100)) for i in range(30) ]
    values = [ 'val' + str(random.randint(0, 100)) for i in range(30) ]


    inp.write("create_db db1\n")
    out.write('create_db Success.\n')

    for i in range(200):
        op = random.choice(ops)
        if op == 'get':
            key = random.choice(keys)
            inp.write("get %s\n" % key)
            if key not in db.keys():
                out.write('get RET = NULL\n')
            else:
                out.write('get RET = %s\n' % db[key])
        elif op == 'put':
            key = random.choice(keys)
            val = random.choice(values)
            inp.write("put %s %s\n" % (key, val))
            if key not in db.keys():
                db[key] = val
                out.write('put RET = true\n')
            else:
                out.write('put RET = false\n')
        elif op == 'update':
            key = random.choice(keys)
            val = random.choice(values)
            inp.write("update %s %s\n" % (key, val))
            if key not in db.keys():
                out.write('update RET = false\n')
            else:
                db[key] = val
                out.write('update RET = true\n')
        elif op == 'remove_key':
            key = random.choice(keys)
            inp.write("remove_key %s\n" % key)
            if key not in db.keys():
                out.write('remove_key RET = false\n')
            else:
                del db[key]
                out.write('remove_key RET = true\n')
        elif op == 'query':
            val = random.choice(values)
            inp.write("query %s\n" % val)
            if val not in db.values():
                out.write("query RET = NULL\n")
            else:
                res = []
                for k in db.keys():
                    if db[k] == val:
                        res.append(k)
                res.sort()
                res = ','.join(res)
                out.write("query RET = %s\n" % res)

    inp.write("close_db db1\n")
    out.write('close_db Success.\n')

    inp.close()
    out.close()

if __name__ == '__main__':
    main()


