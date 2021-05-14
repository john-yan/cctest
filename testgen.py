import random

ops = ['get', 'put', 'update', 'remove_key']
keys = [ 'key' + str(random.randint(0, 100)) for i in range(30) ]
values = [ 'val' + str(random.randint(0, 100)) for i in range(30) ]

def main():
    db = {}
    inp = open("input.txt", 'w')
    out = open('expected.txt', 'w')

    inp.write("create_db db1\n")
    out.write('Success.\n')

    for i in range(100):
        op = random.choice(ops)
        if op == 'get':
            key = random.choice(keys)
            inp.write("get %s\n" % key)
            if key not in db.keys():
                out.write('RET = NULL\n')
            else:
                out.write('RET = %s\n' % db[key])
        elif op == 'put':
            key = random.choice(keys)
            val = random.choice(values)
            inp.write("put %s %s\n" % (key, val))
            if key not in db.keys():
                db[key] = val
                out.write('RET = true\n')
            else:
                out.write('RET = false\n')
        elif op == 'update':
            key = random.choice(keys)
            val = random.choice(values)
            inp.write("update %s %s\n" % (key, val))
            if key not in db.keys():
                out.write('RET = false\n')
            else:
                db[key] = val
                out.write('RET = true\n')
        elif op == 'remove_key':
            key = random.choice(keys)
            inp.write("update %s %s\n" % (key, val))
            if key not in db.keys():
                out.write('RET = false\n')
            else:
                del db[key]
                out.write('RET = true\n')
    inp.close()
    out.close()

if __name__ == '__main__':
    main()


